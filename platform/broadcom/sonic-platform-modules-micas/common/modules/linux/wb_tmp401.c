/* tmp401.c
 *
 * Copyright (C) 2007,2008 Hans de Goede <hdegoede@redhat.com>
 * Preliminary tmp411 support by:
 * Gabriel Konat, Sander Leget, Wouter Willems
 * Copyright (C) 2009 Andre Prendel <andre.prendel@gmx.de>
 *
 * Cleanup and support for TMP431 and TMP432 by Guenter Roeck
 * Copyright (c) 2013 Guenter Roeck <linux@roeck-us.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * Driver for the Texas Instruments TMP401 SMBUS temperature sensor IC.
 *
 * Note this IC is in some aspect similar to the LM90, but it has quite a
 * few differences too, for example the local temp has a higher resolution
 * and thus has 16 bits registers for its value and limit instead of 8 bits.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/bitops.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/sysfs.h>
#include <linux/delay.h>

/* Addresses to scan */
/* static const unsigned short normal_i2c[] = { 0x48, 0x49, 0x4a, 0x4c, 0x4d,
	0x4e, 0x4f, I2C_CLIENT_END }; */

enum chips { tmp401, tmp411, tmp431, tmp432, tmp435, tmp461 };

/*
 * The TMP401 registers, note some registers have different addresses for
 * reading and writing
 */
#define TMP401_STATUS                   (0x02)
#define TMP401_CONFIG_READ              (0x03)
#define TMP401_CONFIG_WRITE             (0x09)
#define TMP401_CONVERSION_RATE_READ     (0x04)
#define TMP401_CONVERSION_RATE_WRITE    (0x0A)
#define TMP401_TEMP_CRIT_HYST           (0x21)
#define TMP401_MANUFACTURER_ID_REG      (0xFE)
#define TMP401_DEVICE_ID_REG            (0xFF)
#define TMP401_DEVICE_CAR_REG           (0x22) /* Consecutive Alert Register */

static const u8 TMP401_TEMP_MSB_READ[7][2] = {
	{ 0x00, 0x01 },	/* temp */
	{ 0x06, 0x08 },	/* low limit */
	{ 0x05, 0x07 },	/* high limit */
	{ 0x20, 0x19 },	/* therm (crit) limit */
	{ 0x30, 0x34 },	/* lowest */
	{ 0x32, 0x36 },	/* highest */
	{ 0, 0x11 },	/* offset */
};

static const u8 TMP401_TEMP_MSB_WRITE[7][2] = {
	{ 0, 0 },	/* temp (unused) */
	{ 0x0C, 0x0E },	/* low limit */
	{ 0x0B, 0x0D },	/* high limit */
	{ 0x20, 0x19 },	/* therm (crit) limit */
	{ 0x30, 0x34 },	/* lowest */
	{ 0x32, 0x36 },	/* highest */
	{ 0, 0x11 },	/* offset */
};

static const u8 TMP401_TEMP_LSB[7][2] = {
	{ 0x15, 0x10 },	/* temp */
	{ 0x17, 0x14 },	/* low limit */
	{ 0x16, 0x13 },	/* high limit */
	{ 0, 0 },	/* therm (crit) limit (unused) */
	{ 0x31, 0x35 },	/* lowest */
	{ 0x33, 0x37 },	/* highest */
	{ 0, 0x12 },	/* offset */
};

static const u8 TMP432_TEMP_MSB_READ[4][3] = {
	{ 0x00, 0x01, 0x23 },	/* temp */
	{ 0x06, 0x08, 0x16 },	/* low limit */
	{ 0x05, 0x07, 0x15 },	/* high limit */
	{ 0x20, 0x19, 0x1A },	/* therm (crit) limit */
};

static const u8 TMP432_TEMP_MSB_WRITE[4][3] = {
	{ 0, 0, 0 },		/* temp  - unused */
	{ 0x0C, 0x0E, 0x16 },	/* low limit */
	{ 0x0B, 0x0D, 0x15 },	/* high limit */
	{ 0x20, 0x19, 0x1A },	/* therm (crit) limit */
};

static const u8 TMP432_TEMP_LSB[3][3] = {
	{ 0x29, 0x10, 0x24 },	/* temp */
	{ 0x3E, 0x14, 0x18 },	/* low limit */
	{ 0x3D, 0x13, 0x17 },	/* high limit */
};

/* [0] = fault, [1] = low, [2] = high, [3] = therm/crit */
static const u8 TMP432_STATUS_REG[] = {
	0x1b, 0x36, 0x35, 0x37 };

/* Flags */
#define TMP401_CONFIG_RANGE             BIT(2)
#define TMP401_CONFIG_SHUTDOWN          BIT(6)
#define TMP401_STATUS_LOCAL_CRIT        BIT(0)
#define TMP401_STATUS_REMOTE_CRIT       BIT(1)
#define TMP401_STATUS_REMOTE_OPEN       BIT(2)
#define TMP401_STATUS_REMOTE_LOW        BIT(3)
#define TMP401_STATUS_REMOTE_HIGH       BIT(4)
#define TMP401_STATUS_LOCAL_LOW         BIT(5)
#define TMP401_STATUS_LOCAL_HIGH        BIT(6)

/* On TMP432, each status has its own register */
#define TMP432_STATUS_LOCAL             BIT(0)
#define TMP432_STATUS_REMOTE1           BIT(1)
#define TMP432_STATUS_REMOTE2           BIT(2)

/* Manufacturer / Device ID's */
#define TMP401_MANUFACTURER_ID      (0x55)
#define TMP401_DEVICE_ID            (0x11)
#define TMP411A_DEVICE_ID           (0x12)
#define TMP411B_DEVICE_ID           (0x13)
#define TMP411C_DEVICE_ID           (0x10)
#define TMP431_DEVICE_ID            (0x31)
#define TMP432_DEVICE_ID            (0x32)
#define TMP435_DEVICE_ID            (0x35)

/* Timeout function bit */
#define TIMEOUT_STATE_BIT                  (7)   /* 1:enable 0:disable */
#define TIMEOUT_STATE_EN                   (1)   /* 1:enable  */
#define TIMEOUT_STATE_IEN                  (0)   /* 0:disable */
#define TIMEOUT_STATE_NA                   "NA"
#define TMP401_TEMP_INVALID_RETRY_TIMES    (3)

/* input temp threshold check */
typedef struct tmp401_temp_threshold_s {
    int chip_type;
    int temp_max;
    int temp_min;
} tmp401_temp_threshold_t;

static tmp401_temp_threshold_t g_tmp401_input_threshold_info[] = {
    {
        .chip_type = tmp411,
        .temp_max = 127000,
        .temp_min = -55000,
    },
};

/*
 * Driver data (common to all clients)
 */

static const struct i2c_device_id tmp401_id[] = {
	{ "wb_tmp401", tmp401 },
	{ "wb_tmp411", tmp411 },
	{ "wb_tmp431", tmp431 },
	{ "wb_tmp432", tmp432 },
	{ "wb_tmp435", tmp435 },
	{ "wb_tmp461", tmp461 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, tmp401_id);

/*
 * Client data (each client gets its own)
 */

struct tmp401_data {
	struct i2c_client *client;
	const struct attribute_group *groups[3];
	struct mutex update_lock;
	char valid; /* zero until following fields are valid */
	unsigned long last_updated; /* in jiffies */
	enum chips kind;

	unsigned int update_interval;	/* in milliseconds */

	/* register values */
	u8 status[4];
	u8 config;
	u16 temp[7][3];
	u8 temp_crit_hyst;
};

/*
 * Sysfs attr show / store functions
 */

static int tmp401_register_to_temp(u16 reg, u8 config)
{
	int temp = reg;

	if (config & TMP401_CONFIG_RANGE)
		temp -= 64 * 256;

	return DIV_ROUND_CLOSEST(temp * 125, 32);
}

static u16 tmp401_temp_to_register(long temp, u8 config, int zbits)
{
	if (config & TMP401_CONFIG_RANGE) {
		temp = clamp_val(temp, -64000, 191000);
		temp += 64000;
	} else
		temp = clamp_val(temp, 0, 127000);

	return DIV_ROUND_CLOSEST(temp * (1 << (8 - zbits)), 1000) << zbits;
}

static int tmp401_update_device_reg16(struct i2c_client *client,
				      struct tmp401_data *data)
{
	int i, j, val;
	int num_regs = data->kind == tmp411 ? 6 : 4;
	int num_sensors = data->kind == tmp432 ? 3 : 2;

	for (i = 0; i < num_sensors; i++) {		/* local / r1 / r2 */
		for (j = 0; j < num_regs; j++) {	/* temp / low / ... */
			u8 regaddr;
			/*
			 * High byte must be read first immediately followed
			 * by the low byte
			 */
			regaddr = data->kind == tmp432 ?
						TMP432_TEMP_MSB_READ[j][i] :
						TMP401_TEMP_MSB_READ[j][i];
			val = i2c_smbus_read_byte_data(client, regaddr);
			if (val < 0)
				return val;
			data->temp[j][i] = val << 8;
			if (j == 3)		/* crit is msb only */
				continue;
			regaddr = data->kind == tmp432 ? TMP432_TEMP_LSB[j][i]
						       : TMP401_TEMP_LSB[j][i];
			val = i2c_smbus_read_byte_data(client, regaddr);
			if (val < 0)
				return val;
			data->temp[j][i] |= val;
		}
	}
	return 0;
}

static struct tmp401_data *tmp401_update_device(struct device *dev)
{
	struct tmp401_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
	struct tmp401_data *ret = data;
	int i, val;
	unsigned long next_update;

	mutex_lock(&data->update_lock);

	next_update = data->last_updated +
		      msecs_to_jiffies(data->update_interval);
	if (time_after(jiffies, next_update) || !data->valid) {
		if (data->kind != tmp432) {
			/*
			 * The driver uses the TMP432 status format internally.
			 * Convert status to TMP432 format for other chips.
			 */
			val = i2c_smbus_read_byte_data(client, TMP401_STATUS);
			if (val < 0) {
				ret = ERR_PTR(val);
				goto abort;
			}
			data->status[0] =
			  (val & TMP401_STATUS_REMOTE_OPEN) >> 1;
			data->status[1] =
			  ((val & TMP401_STATUS_REMOTE_LOW) >> 2) |
			  ((val & TMP401_STATUS_LOCAL_LOW) >> 5);
			data->status[2] =
			  ((val & TMP401_STATUS_REMOTE_HIGH) >> 3) |
			  ((val & TMP401_STATUS_LOCAL_HIGH) >> 6);
			data->status[3] = val & (TMP401_STATUS_LOCAL_CRIT
						| TMP401_STATUS_REMOTE_CRIT);
		} else {
			for (i = 0; i < ARRAY_SIZE(data->status); i++) {
				val = i2c_smbus_read_byte_data(client,
							TMP432_STATUS_REG[i]);
				if (val < 0) {
					ret = ERR_PTR(val);
					goto abort;
				}
				data->status[i] = val;
			}
		}

		val = i2c_smbus_read_byte_data(client, TMP401_CONFIG_READ);
		if (val < 0) {
			ret = ERR_PTR(val);
			goto abort;
		}
		data->config = val;
		val = tmp401_update_device_reg16(client, data);
		if (val < 0) {
			ret = ERR_PTR(val);
			goto abort;
		}
		val = i2c_smbus_read_byte_data(client, TMP401_TEMP_CRIT_HYST);
		if (val < 0) {
			ret = ERR_PTR(val);
			goto abort;
		}
		data->temp_crit_hyst = val;

		data->last_updated = jiffies;
		data->valid = 1;
	}

abort:
	mutex_unlock(&data->update_lock);
	return ret;
}

static int tmp401_input_temp_check(struct tmp401_data *data, int input_val)
{
    int i, size;

    size = ARRAY_SIZE(g_tmp401_input_threshold_info);

    for (i = 0; i < size; i++) {
        if (g_tmp401_input_threshold_info[i].chip_type == data->kind) {
            if ((input_val > g_tmp401_input_threshold_info[i].temp_max)
                    || (input_val < g_tmp401_input_threshold_info[i].temp_min)) {
                dev_dbg(&data->client->dev, "input temp: %d not in range[%d, %d]\n",
                    input_val, g_tmp401_input_threshold_info[i].temp_min,
                    g_tmp401_input_threshold_info[i].temp_max);
                return -EINVAL;
            }
            dev_dbg(&data->client->dev, "input temp: %d in range[%d, %d]", input_val,
                g_tmp401_input_threshold_info[i].temp_min, g_tmp401_input_threshold_info[i].temp_max);
            return 0;
        }
    }
    return 0;
}

static ssize_t show_temp(struct device *dev, struct device_attribute *devattr, char *buf)
{
    int nr, index, i, value, ret;
    struct tmp401_data *data;
    struct i2c_client *client;

    data = dev_get_drvdata(dev);
    client = data->client;

    nr = to_sensor_dev_attr_2(devattr)->nr;
    index = to_sensor_dev_attr_2(devattr)->index;

    for (i = 0; i < TMP401_TEMP_INVALID_RETRY_TIMES; i++) {
        data = tmp401_update_device(dev);
        if (IS_ERR(data)) {
            return PTR_ERR(data);
        }
        value = tmp401_register_to_temp(data->temp[nr][index], data->config);
        if (nr != 0) {  /* not input temp, return value */
            return sprintf(buf, "%d\n", value);
        }
        /* nr == 0 is temp input, do input_temp_check */
        ret = tmp401_input_temp_check(data, value);
        if (ret == 0) { /* input temp check ok */
           return sprintf(buf, "%d\n", value);
        }
        if ((i + 1) < TMP401_TEMP_INVALID_RETRY_TIMES) {
            msleep(data->update_interval);
        }
    }
    dev_info(&client->dev, "temp%d_input value: %d invalid\n", index + 1, value);
    return -EINVAL;
}

static ssize_t show_temp_crit_hyst(struct device *dev,
	struct device_attribute *devattr, char *buf)
{
	int temp, index = to_sensor_dev_attr(devattr)->index;
	struct tmp401_data *data = tmp401_update_device(dev);

	if (IS_ERR(data))
		return PTR_ERR(data);

	mutex_lock(&data->update_lock);
	temp = tmp401_register_to_temp(data->temp[3][index], data->config);
	temp -= data->temp_crit_hyst * 1000;
	mutex_unlock(&data->update_lock);

	return sprintf(buf, "%d\n", temp);
}

static ssize_t show_status(struct device *dev,
	struct device_attribute *devattr, char *buf)
{
	int nr = to_sensor_dev_attr_2(devattr)->nr;
	int mask = to_sensor_dev_attr_2(devattr)->index;
	struct tmp401_data *data = tmp401_update_device(dev);

	if (IS_ERR(data))
		return PTR_ERR(data);

	return sprintf(buf, "%d\n", !!(data->status[nr] & mask));
}

static ssize_t store_temp(struct device *dev, struct device_attribute *devattr,
			  const char *buf, size_t count)
{
	int nr = to_sensor_dev_attr_2(devattr)->nr;
	int index = to_sensor_dev_attr_2(devattr)->index;
	struct tmp401_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
	long val;
	u16 reg;
	u8 regaddr;

	if (kstrtol(buf, 10, &val))
		return -EINVAL;

	reg = tmp401_temp_to_register(val, data->config, nr == 3 ? 8 : 4);

	mutex_lock(&data->update_lock);

	regaddr = data->kind == tmp432 ? TMP432_TEMP_MSB_WRITE[nr][index]
				       : TMP401_TEMP_MSB_WRITE[nr][index];
	i2c_smbus_write_byte_data(client, regaddr, reg >> 8);
	if (nr != 3) {
		regaddr = data->kind == tmp432 ? TMP432_TEMP_LSB[nr][index]
					       : TMP401_TEMP_LSB[nr][index];
		i2c_smbus_write_byte_data(client, regaddr, reg & 0xFF);
	}
	data->temp[nr][index] = reg;

	mutex_unlock(&data->update_lock);

	return count;
}

static ssize_t store_temp_crit_hyst(struct device *dev, struct device_attribute
	*devattr, const char *buf, size_t count)
{
	int temp, index = to_sensor_dev_attr(devattr)->index;
	struct tmp401_data *data = tmp401_update_device(dev);
	long val;
	u8 reg;

	if (IS_ERR(data))
		return PTR_ERR(data);

	if (kstrtol(buf, 10, &val))
		return -EINVAL;

	if (data->config & TMP401_CONFIG_RANGE)
		val = clamp_val(val, -64000, 191000);
	else
		val = clamp_val(val, 0, 127000);

	mutex_lock(&data->update_lock);
	temp = tmp401_register_to_temp(data->temp[3][index], data->config);
	val = clamp_val(val, temp - 255000, temp);
	reg = ((temp - val) + 500) / 1000;

	i2c_smbus_write_byte_data(data->client, TMP401_TEMP_CRIT_HYST,
				  reg);

	data->temp_crit_hyst = reg;

	mutex_unlock(&data->update_lock);

	return count;
}

/*
 * Resets the historical measurements of minimum and maximum temperatures.
 * This is done by writing any value to any of the minimum/maximum registers
 * (0x30-0x37).
 */
static ssize_t reset_temp_history(struct device *dev,
	struct device_attribute	*devattr, const char *buf, size_t count)
{
	struct tmp401_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
	long val;

	if (kstrtol(buf, 10, &val))
		return -EINVAL;

	if (val != 1) {
		dev_err(dev,
			"temp_reset_history value %ld not supported. Use 1 to reset the history!\n",
			val);
		return -EINVAL;
	}
	mutex_lock(&data->update_lock);
	i2c_smbus_write_byte_data(client, TMP401_TEMP_MSB_WRITE[5][0], val);
	data->valid = 0;
	mutex_unlock(&data->update_lock);

	return count;
}

static ssize_t show_update_interval(struct device *dev,
				    struct device_attribute *attr, char *buf)
{
	struct tmp401_data *data = dev_get_drvdata(dev);

	return sprintf(buf, "%u\n", data->update_interval);
}

static ssize_t set_update_interval(struct device *dev,
				   struct device_attribute *attr,
				   const char *buf, size_t count)
{
	struct tmp401_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
	unsigned long val;
	int err, rate;

	err = kstrtoul(buf, 10, &val);
	if (err)
		return err;

	/*
	 * For valid rates, interval can be calculated as
	 *	interval = (1 << (7 - rate)) * 125;
	 * Rounded rate is therefore
	 *	rate = 7 - __fls(interval * 4 / (125 * 3));
	 * Use clamp_val() to avoid overflows, and to ensure valid input
	 * for __fls.
	 */
	val = clamp_val(val, 125, 16000);
	rate = 7 - __fls(val * 4 / (125 * 3));
	mutex_lock(&data->update_lock);
	i2c_smbus_write_byte_data(client, TMP401_CONVERSION_RATE_WRITE, rate);
	data->update_interval = (1 << (7 - rate)) * 125;
	mutex_unlock(&data->update_lock);

	return count;
}

/*
 * Enable/disable the state of the timeout function
 * @dev: device info
 * @state: 1:enable 0:disable
 */
static int timeout_cfg(struct device *dev, int state)
{
    int rv, chip_type;
    u8 reg_value;
    struct tmp401_data *data;
    struct i2c_client *client;

    data = dev_get_drvdata(dev);
    client = data->client;

    /* get chip type */
    chip_type = data->kind;
    dev_dbg(&client->dev, "set timeout. chip:%d, state:%d\n", chip_type, state);

    /* chip type check */
    if(chip_type != tmp401 && chip_type != tmp411)  {
        dev_info(&client->dev,
            "Chip type: %d, not support timeout config.!\n", chip_type);
        return -EPERM;
    }

    /* parameter check */
    if(state != TIMEOUT_STATE_EN && state != TIMEOUT_STATE_IEN) {
        dev_err(&client->dev,
            "Parameter check error. state: %d not supported.!\n", state);
        return -EINVAL;
    }

    mutex_lock(&data->update_lock);
    /* read the Consecutive alert register */
    reg_value = i2c_smbus_read_byte_data(client, TMP401_DEVICE_CAR_REG);
    if (reg_value < 0) {
        dev_err(&client->dev, "Failed to read. reg:0x%0x, value:%d\n", TMP401_DEVICE_CAR_REG, reg_value);
        mutex_unlock(&data->update_lock);
        return -EIO;
    }
    dev_dbg(&client->dev, "get register value. reg:0x%0x, value:0x%0x\n", TMP401_DEVICE_CAR_REG, reg_value);

    /* same value case, do not write */
    if((u8)state == (reg_value >> TIMEOUT_STATE_BIT)) {
        mutex_unlock(&data->update_lock);
        dev_info(&client->dev, "timeout config has been set and the current state is %d.\n", state);
        return 0;
    }

    /* calculate the register value */
    reg_value = (reg_value & ~(1 << TIMEOUT_STATE_BIT)) | (state << TIMEOUT_STATE_BIT);

    /* set the Consecutive alert register */
    dev_dbg(&client->dev, "set register value. reg:0x%0x, value:0x%0x\n", TMP401_DEVICE_CAR_REG, reg_value);
    rv = i2c_smbus_write_byte_data(client, TMP401_DEVICE_CAR_REG, reg_value);
    if (rv < 0) {
        dev_err(&client->dev,
            "set the register Error. reg:0x%0x, value:%d\n", TMP401_DEVICE_CAR_REG, reg_value);
        mutex_unlock(&data->update_lock);
        return -EIO;
    }
    mutex_unlock(&data->update_lock);

    dev_info(&client->dev, "set bus timeout success. reg:0x%0x, value:0x%0x\n", TMP401_DEVICE_CAR_REG, reg_value);

    return 0;
}

static ssize_t set_timeout_en(struct device *dev,
                   struct device_attribute *attr,
                   const char *buf, size_t count)
{
    int val, err;
    struct i2c_client *client;
    struct tmp401_data *data;

    data = dev_get_drvdata(dev);
    client = data->client;

    err = kstrtoint(buf, 0, &val);
    if (err) {
        dev_err(&client->dev,
            "kstrtoint error: %d.\n", err);
        return err;
    }

    err = timeout_cfg(dev, val);
    if(err < 0) {
        dev_err(&client->dev,
            "set bus timeout error: %d. value:%d!\n", err, val);
        return err;
    }

    return count;
}

static ssize_t show_timeout_en(struct device *dev,
    struct device_attribute *devattr, char *buf)
{
    int chip_type;
    u8 reg_value;
    struct tmp401_data *data;
    struct i2c_client *client;

    data = dev_get_drvdata(dev);
    client = data->client;

    /* get chip type */
    chip_type = data->kind;
    dev_dbg(&client->dev, "get timeout. chip:%d\n", chip_type);

    /* chip type check */
    if(chip_type != tmp401 && chip_type != tmp411)  {
        dev_info(&client->dev,
            "Chip type: %d, not support timeout config.!\n", chip_type);
        /* not support, return NA */
        return (ssize_t)snprintf(buf, PAGE_SIZE, "%s\n", TIMEOUT_STATE_NA);
    }

    /* read the Consecutive alert register */
    reg_value = i2c_smbus_read_byte_data(client, TMP401_DEVICE_CAR_REG);
    if (reg_value < 0) {
        dev_err(&client->dev, "Failed to read. reg:0x%0x, value:%d\n", TMP401_DEVICE_CAR_REG, reg_value);
        return -EIO;
    }
    dev_dbg(&client->dev, "get register value. reg:0x%0x, value:0x%0x\n", TMP401_DEVICE_CAR_REG, reg_value);

    /* decode the register value */
    reg_value = reg_value >> TIMEOUT_STATE_BIT;

    return (ssize_t)snprintf(buf, PAGE_SIZE, "%d\n", reg_value);
}

static SENSOR_DEVICE_ATTR_2(temp1_input, S_IRUGO, show_temp, NULL, 0, 0);
static SENSOR_DEVICE_ATTR_2(temp1_min, S_IWUSR | S_IRUGO, show_temp,
			    store_temp, 1, 0);
static SENSOR_DEVICE_ATTR_2(temp1_max, S_IWUSR | S_IRUGO, show_temp,
			    store_temp, 2, 0);
static SENSOR_DEVICE_ATTR_2(temp1_crit, S_IWUSR | S_IRUGO, show_temp,
			    store_temp, 3, 0);
static SENSOR_DEVICE_ATTR(temp1_crit_hyst, S_IWUSR | S_IRUGO,
			  show_temp_crit_hyst, store_temp_crit_hyst, 0);
static SENSOR_DEVICE_ATTR_2(temp1_min_alarm, S_IRUGO, show_status, NULL,
			    1, TMP432_STATUS_LOCAL);
static SENSOR_DEVICE_ATTR_2(temp1_max_alarm, S_IRUGO, show_status, NULL,
			    2, TMP432_STATUS_LOCAL);
static SENSOR_DEVICE_ATTR_2(temp1_crit_alarm, S_IRUGO, show_status, NULL,
			    3, TMP432_STATUS_LOCAL);
static SENSOR_DEVICE_ATTR_2(temp2_input, S_IRUGO, show_temp, NULL, 0, 1);
static SENSOR_DEVICE_ATTR_2(temp2_min, S_IWUSR | S_IRUGO, show_temp,
			    store_temp, 1, 1);
static SENSOR_DEVICE_ATTR_2(temp2_max, S_IWUSR | S_IRUGO, show_temp,
			    store_temp, 2, 1);
static SENSOR_DEVICE_ATTR_2(temp2_crit, S_IWUSR | S_IRUGO, show_temp,
			    store_temp, 3, 1);
static SENSOR_DEVICE_ATTR(temp2_crit_hyst, S_IRUGO, show_temp_crit_hyst,
			  NULL, 1);
static SENSOR_DEVICE_ATTR_2(temp2_fault, S_IRUGO, show_status, NULL,
			    0, TMP432_STATUS_REMOTE1);
static SENSOR_DEVICE_ATTR_2(temp2_min_alarm, S_IRUGO, show_status, NULL,
			    1, TMP432_STATUS_REMOTE1);
static SENSOR_DEVICE_ATTR_2(temp2_max_alarm, S_IRUGO, show_status, NULL,
			    2, TMP432_STATUS_REMOTE1);
static SENSOR_DEVICE_ATTR_2(temp2_crit_alarm, S_IRUGO, show_status, NULL,
			    3, TMP432_STATUS_REMOTE1);

static DEVICE_ATTR(update_interval, S_IRUGO | S_IWUSR, show_update_interval,
		   set_update_interval);
static DEVICE_ATTR(timeout_en, S_IRUGO | S_IWUSR, show_timeout_en, set_timeout_en);

static struct attribute *tmp401_attributes[] = {
	&sensor_dev_attr_temp1_input.dev_attr.attr,
	&sensor_dev_attr_temp1_min.dev_attr.attr,
	&sensor_dev_attr_temp1_max.dev_attr.attr,
	&sensor_dev_attr_temp1_crit.dev_attr.attr,
	&sensor_dev_attr_temp1_crit_hyst.dev_attr.attr,
	&sensor_dev_attr_temp1_max_alarm.dev_attr.attr,
	&sensor_dev_attr_temp1_min_alarm.dev_attr.attr,
	&sensor_dev_attr_temp1_crit_alarm.dev_attr.attr,

	&sensor_dev_attr_temp2_input.dev_attr.attr,
	&sensor_dev_attr_temp2_min.dev_attr.attr,
	&sensor_dev_attr_temp2_max.dev_attr.attr,
	&sensor_dev_attr_temp2_crit.dev_attr.attr,
	&sensor_dev_attr_temp2_crit_hyst.dev_attr.attr,
	&sensor_dev_attr_temp2_fault.dev_attr.attr,
	&sensor_dev_attr_temp2_max_alarm.dev_attr.attr,
	&sensor_dev_attr_temp2_min_alarm.dev_attr.attr,
	&sensor_dev_attr_temp2_crit_alarm.dev_attr.attr,

	&dev_attr_update_interval.attr,
	&dev_attr_timeout_en.attr,

	NULL
};

static const struct attribute_group tmp401_group = {
	.attrs = tmp401_attributes,
};

/*
 * Additional features of the TMP411 chip.
 * The TMP411 stores the minimum and maximum
 * temperature measured since power-on, chip-reset, or
 * minimum and maximum register reset for both the local
 * and remote channels.
 */
static SENSOR_DEVICE_ATTR_2(temp1_lowest, S_IRUGO, show_temp, NULL, 4, 0);
static SENSOR_DEVICE_ATTR_2(temp1_highest, S_IRUGO, show_temp, NULL, 5, 0);
static SENSOR_DEVICE_ATTR_2(temp2_lowest, S_IRUGO, show_temp, NULL, 4, 1);
static SENSOR_DEVICE_ATTR_2(temp2_highest, S_IRUGO, show_temp, NULL, 5, 1);
static SENSOR_DEVICE_ATTR(temp_reset_history, S_IWUSR, NULL, reset_temp_history,
			  0);

static struct attribute *tmp411_attributes[] = {
	&sensor_dev_attr_temp1_highest.dev_attr.attr,
	&sensor_dev_attr_temp1_lowest.dev_attr.attr,
	&sensor_dev_attr_temp2_highest.dev_attr.attr,
	&sensor_dev_attr_temp2_lowest.dev_attr.attr,
	&sensor_dev_attr_temp_reset_history.dev_attr.attr,
	NULL
};

static const struct attribute_group tmp411_group = {
	.attrs = tmp411_attributes,
};

static SENSOR_DEVICE_ATTR_2(temp3_input, S_IRUGO, show_temp, NULL, 0, 2);
static SENSOR_DEVICE_ATTR_2(temp3_min, S_IWUSR | S_IRUGO, show_temp,
			    store_temp, 1, 2);
static SENSOR_DEVICE_ATTR_2(temp3_max, S_IWUSR | S_IRUGO, show_temp,
			    store_temp, 2, 2);
static SENSOR_DEVICE_ATTR_2(temp3_crit, S_IWUSR | S_IRUGO, show_temp,
			    store_temp, 3, 2);
static SENSOR_DEVICE_ATTR(temp3_crit_hyst, S_IRUGO, show_temp_crit_hyst,
			  NULL, 2);
static SENSOR_DEVICE_ATTR_2(temp3_fault, S_IRUGO, show_status, NULL,
			    0, TMP432_STATUS_REMOTE2);
static SENSOR_DEVICE_ATTR_2(temp3_min_alarm, S_IRUGO, show_status, NULL,
			    1, TMP432_STATUS_REMOTE2);
static SENSOR_DEVICE_ATTR_2(temp3_max_alarm, S_IRUGO, show_status, NULL,
			    2, TMP432_STATUS_REMOTE2);
static SENSOR_DEVICE_ATTR_2(temp3_crit_alarm, S_IRUGO, show_status, NULL,
			    3, TMP432_STATUS_REMOTE2);

static struct attribute *tmp432_attributes[] = {
	&sensor_dev_attr_temp3_input.dev_attr.attr,
	&sensor_dev_attr_temp3_min.dev_attr.attr,
	&sensor_dev_attr_temp3_max.dev_attr.attr,
	&sensor_dev_attr_temp3_crit.dev_attr.attr,
	&sensor_dev_attr_temp3_crit_hyst.dev_attr.attr,
	&sensor_dev_attr_temp3_fault.dev_attr.attr,
	&sensor_dev_attr_temp3_max_alarm.dev_attr.attr,
	&sensor_dev_attr_temp3_min_alarm.dev_attr.attr,
	&sensor_dev_attr_temp3_crit_alarm.dev_attr.attr,

	NULL
};

static const struct attribute_group tmp432_group = {
	.attrs = tmp432_attributes,
};

/*
 * Additional features of the TMP461 chip.
 * The TMP461 temperature offset for the remote channel.
 */
static SENSOR_DEVICE_ATTR_2(temp2_offset, S_IWUSR | S_IRUGO, show_temp,
			    store_temp, 6, 1);

static struct attribute *tmp461_attributes[] = {
	&sensor_dev_attr_temp2_offset.dev_attr.attr,
	NULL
};

static const struct attribute_group tmp461_group = {
	.attrs = tmp461_attributes,
};

/*
 * Begin non sysfs callback code (aka Real code)
 */

static int tmp401_init_client(struct tmp401_data *data,
			      struct i2c_client *client)
{
	int config, config_orig, status = 0;

	/* Set the conversion rate to 2 Hz */
	i2c_smbus_write_byte_data(client, TMP401_CONVERSION_RATE_WRITE, 5);
	data->update_interval = 500;

	/* Start conversions (disable shutdown if necessary) */
	config = i2c_smbus_read_byte_data(client, TMP401_CONFIG_READ);
	if (config < 0)
		return config;

	config_orig = config;
	config &= ~TMP401_CONFIG_SHUTDOWN;

	if (config != config_orig)
		status = i2c_smbus_write_byte_data(client,
						   TMP401_CONFIG_WRITE,
						   config);

	return status;
}

#if 0
static int tmp401_detect(struct i2c_client *client,
			 struct i2c_board_info *info)
{
	enum chips kind;
	struct i2c_adapter *adapter = client->adapter;
	u8 reg;

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
		return -ENODEV;

	/* Detect and identify the chip */
	reg = i2c_smbus_read_byte_data(client, TMP401_MANUFACTURER_ID_REG);
	if (reg != TMP401_MANUFACTURER_ID)
		return -ENODEV;

	reg = i2c_smbus_read_byte_data(client, TMP401_DEVICE_ID_REG);

	switch (reg) {
	case TMP401_DEVICE_ID:
		if (client->addr != 0x4c)
			return -ENODEV;
		kind = tmp401;
		break;
	case TMP411A_DEVICE_ID:
		if (client->addr != 0x4c)
			return -ENODEV;
		kind = tmp411;
		break;
	case TMP411B_DEVICE_ID:
		if (client->addr != 0x4d)
			return -ENODEV;
		kind = tmp411;
		break;
	case TMP411C_DEVICE_ID:
		if (client->addr != 0x4e)
			return -ENODEV;
		kind = tmp411;
		break;
	case TMP431_DEVICE_ID:
		if (client->addr != 0x4c && client->addr != 0x4d)
			return -ENODEV;
		kind = tmp431;
		break;
	case TMP432_DEVICE_ID:
		if (client->addr != 0x4c && client->addr != 0x4d)
			return -ENODEV;
		kind = tmp432;
		break;
	case TMP435_DEVICE_ID:
		kind = tmp435;
		break;
	default:
		return -ENODEV;
	}

	reg = i2c_smbus_read_byte_data(client, TMP401_CONFIG_READ);
	if (reg & 0x1b)
		return -ENODEV;

	reg = i2c_smbus_read_byte_data(client, TMP401_CONVERSION_RATE_READ);
	/* Datasheet says: 0x1-0x6 */
	if (reg > 15)
		return -ENODEV;

	strlcpy(info->type, tmp401_id[kind].name, I2C_NAME_SIZE);

	return 0;
}
#endif

static int tmp401_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	static const char * const names[] = {
		"TMP401", "TMP411", "TMP431", "TMP432", "TMP435", "TMP461"
	};
	struct device *dev = &client->dev;
	struct device *hwmon_dev;
	struct tmp401_data *data;
	int groups = 0, status;

	data = devm_kzalloc(dev, sizeof(struct tmp401_data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	data->client = client;
	mutex_init(&data->update_lock);
	data->kind = id->driver_data;

	/* Initialize the TMP401 chip */
	status = tmp401_init_client(data, client);
	if (status < 0)
		return status;

	/* Register sysfs hooks */
	data->groups[groups++] = &tmp401_group;

	/* Register additional tmp411 sysfs hooks */
	if (data->kind == tmp411)
		data->groups[groups++] = &tmp411_group;

	/* Register additional tmp432 sysfs hooks */
	if (data->kind == tmp432)
		data->groups[groups++] = &tmp432_group;

	if (data->kind == tmp461)
		data->groups[groups++] = &tmp461_group;

	hwmon_dev = devm_hwmon_device_register_with_groups(dev, client->name,
							   data, data->groups);
	if (IS_ERR(hwmon_dev))
		return PTR_ERR(hwmon_dev);

    /* disable the timeout function */
    status = timeout_cfg(hwmon_dev, TIMEOUT_STATE_IEN);
    if((status < 0) && (status != -EPERM)) {
        dev_err(dev,
            "set bus timeout error when probing: %d.!\n", status);
        /* here, no need call devm_hwmon_device_unregister, device managed. */
        return status;
    }

	dev_info(dev, "Detected TI %s chip\n", names[data->kind]);

	return 0;
}

static struct i2c_driver tmp401_driver = {
	.class		= I2C_CLASS_HWMON,
	.driver = {
		.name	= "wb_tmp401",
	},
	.probe		= tmp401_probe,
	.id_table	= tmp401_id,
	/* .detect		= tmp401_detect, */
	/* .address_list	= normal_i2c, */
};

module_i2c_driver(tmp401_driver);

MODULE_AUTHOR("support");
MODULE_DESCRIPTION("Texas Instruments TMP401 temperature sensor driver");
MODULE_LICENSE("GPL");
