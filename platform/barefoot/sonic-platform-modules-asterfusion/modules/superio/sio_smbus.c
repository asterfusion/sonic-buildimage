/*
 * This sio_smbus driver, Used to receive bmc information.
 * - Add Error check for I2C_SMBUS_BYTE_DATA
 * - Fix read sfp eeprom make sio_smbus fail
 * Version 1.6
 *
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/i2c.h>

#if 0
struct mutex bmc_device_lock;
struct mutex pca9548_device_lock;
#endif

#define DRVNAME "sio_smbus"

enum kinds { nct6106, nct6775, nct6776, nct6779, nct6791, nct6792, nct6793,
    nct6795, nct6796, nct6797, nct6798 };

/* used to set data->name = nct6775_device_names[data->sio_kind] */
static const char * const nct6775_device_names[] = {
    "nct6106",
    "nct6775",
    "nct6776",
    "nct6779",
    "nct6791",
    "nct6792",
    "nct6793",
    "nct6795",
    "nct6796",
    "nct6797",
    "nct6798",
};

static const char * const nct6775_sio_names[] = {
    "NCT6106D",
    "NCT6775F",
    "NCT6776D/F",
    "NCT6779D",
    "NCT6791D",
    "NCT6792D",
    "NCT6793D",
    "NCT6795D",
    "NCT6796D",
    "NCT6797D",
    "NCT6798D",
};

/* SMBus Register Map 
 * SMBus Master base address in register Logic Device B CR62h(MSB), CR63h(LSB).
 * NCT6679D section 19.7
 * NCT5525  section 18.7 */
#define SMDATA              0
#define SMWRSIZE            1
#define SMCMD               2
#define SMIDX               3
#define SMCTL               4
#define SMADDR              5
#define SCLFREQ             6
#define BS_MASK             (~7)
#define PCHADDR             8
#define ERROR_STATUS        9
#define PCHCMD              0xB
#define TSI_AGENT           0xD
#define SMCTL3              0xE
#define SMCTL2              0xF

#define CHIP_ID 58

/*
 * Super-I/O constants and functions
 */

#define NCT6775_LD_ACPI     0x0a
#define NCT6775_LD_HWM      0x0b
#define NCT6775_LD_VID      0x0d
#define NCT6775_LD_12       0x12

/* Chip (Global) Control Register (CRxxh) */
#define SIO_REG_LDSEL       0x07    /* Logical device select */
#define SIO_REG_DEVID       0x20    /* Device ID (2 bytes) */
#define SIO_REG_ENABLE      0x30    /* Logical device enable */
#define SIO_REG_ADDR        0x60    /* Logical device address (2 bytes) */
#define SIO_ADDR            0x2e

#define SIO_NCT6106_ID      0xc450
#define SIO_NCT6775_ID      0xb470
#define SIO_NCT6776_ID      0xc330
#define SIO_NCT6779_ID      0xc560
#define SIO_NCT6791_ID      0xc800
#define SIO_NCT6792_ID      0xc910
#define SIO_NCT6793_ID      0xd120
#define SIO_NCT6795_ID      0xd350
#define SIO_NCT6796_ID      0xd420
#define SIO_NCT6797_ID      0xd450
#define SIO_NCT6798_ID      0xd428
#define SIO_ID_MASK         0xFFF8

/*
SCLFQ (SMBCLK Frequency). This field defines the SMBCLK period (low time and high time). The
clock low time and high time ate defined as follows:
    0000 : 365KHz
    0001 : 261KHz
    0010 : 200KHz
    0011 : 162KHz
    0100 : 136KHz
    0101 : 117KHz
    0110 : 103KHz
    0111 : 92KHz (Default)
    1000 : 83KHz
    1001 : 76KHz
    1010 : 71KHz
    1011 : 65KHz
    1100 : 61KHz
    1101 : 57KHz
    1110 : 53KHz
    1111 : 47KHz
*/
int smbus_clock = 2;  //0x2=0010=200KHz=100KHz of waveform

module_param(smbus_clock, int, 0);

struct sio_smbus_priv {
    struct i2c_adapter adapter;
    struct i2c_msg *msg;
    struct device *dev;
    struct mutex acpi_lock;
    int bs_addr;            /* SMBus Master base address in Logic Device B CR62h(MSB), CR63h(LSB) */
};

/* RD val from reg */
static inline int
superio_inb(int ioreg, int reg)
{
    outb(reg, ioreg);
    return inb(ioreg + 1);
}

/* WR val to reg */
static inline void
superio_outb(int ioreg, int reg, int val)
{
    outb(reg, ioreg);
    outb(val, ioreg + 1);
}

/* */
static inline int
superio_enter(int ioreg)
{
    if (!request_muxed_region(ioreg, 2, DRVNAME))
        return -EBUSY;

	/*
	 * Try to reserve <ioreg> and <ioreg + 1> for exclusive access.
	 */
    outb(0x87, ioreg);
    outb(0x87, ioreg);

    return 0;
}

static inline void
superio_select(int ioreg, int ld)
{
    outb(SIO_REG_LDSEL, ioreg);
    outb(ld, ioreg + 1);
}

static inline void
superio_exit(int ioreg)
{
    outb(0xaa, ioreg);
    outb(0x02, ioreg);
    outb(0x02, ioreg + 1);
    release_region(ioreg, 2);
}

/*
 * ISA constants
 */

#define IOREGION_ALIGNMENT  (~7)
#define IOREGION_OFFSET     5
#define IOREGION_LENGTH     2
#define ADDR_REG_OFFSET     0
#define DATA_REG_OFFSET     1

#define NCT6775_REG_BANK    0x4E
#define NCT6775_REG_CONFIG  0x40

static int superio_proc(struct sio_smbus_priv *priv)
{
    int ret = 0;
    int sioaddr = SIO_ADDR;
    int addr = 0;
    enum kinds kind;
    int enable_val = 0;
    u16 val = 0;
    //u16 val_1A = 0,val_1B = 0,val_2F = 0;

    ret = superio_enter(sioaddr);
    if (ret)
        return ret;

    superio_select(sioaddr, NCT6775_LD_HWM);

    //superio_outb(SIO_ADDR, 0x02, 0x1);
    /* SMBus Master base address in register Logic Device B CR62h(MSB), CR63h(LSB). */
    val = (superio_inb(sioaddr, 0x62) << 8) | superio_inb(sioaddr, 0x63);

    priv->bs_addr = val & BS_MASK;
    enable_val = superio_inb(sioaddr, SIO_REG_ENABLE);
    if (!enable_val){
        superio_outb(sioaddr, SIO_REG_ENABLE, enable_val | 0x01);
    }

    val = (superio_inb(sioaddr, SIO_REG_DEVID) << 8) | superio_inb(sioaddr, SIO_REG_DEVID + 1);
    switch (val & SIO_ID_MASK) {
        case SIO_NCT6106_ID:
            kind = nct6106;
            break;
        case SIO_NCT6775_ID:
            kind = nct6775;
            break;
        case SIO_NCT6776_ID:
            kind = nct6776;
            break;
        case SIO_NCT6779_ID:
            kind = nct6779;
            break;
        case SIO_NCT6791_ID:
            kind = nct6791;
            break;
        case SIO_NCT6792_ID:
            kind = nct6792;
            break;
        case SIO_NCT6793_ID:
            kind = nct6793;
            break;
        case SIO_NCT6795_ID:
            kind = nct6795;
            break;
        case SIO_NCT6796_ID:
            kind = nct6796;
            break;
        case SIO_NCT6797_ID:
            kind = nct6797;
            break;
        case SIO_NCT6798_ID:
            kind = nct6798;
            break;
        default:
            if (val != 0xffff)
                pr_debug("unsupported chip ID: 0x%04x\n", val);
            superio_exit(sioaddr);
            return -ENODEV;
    }

    superio_outb(sioaddr, 0x1A, 0x0);
    superio_outb(sioaddr, 0x1B, 0x0);
    superio_exit(sioaddr);

    usleep_range(100, 500);
    outb_p(0x00, priv->bs_addr + SMCTL);
    usleep_range(100, 500);
    outb_p(0x40, priv->bs_addr + SMCTL);
    usleep_range(100, 500);
    outb_p(0x00, priv->bs_addr + SMCTL);
    usleep_range(100, 500);
    outb_p(smbus_clock, priv->bs_addr + SCLFREQ);

    /* We have a known chip, find the HWM I/O address */
    superio_select(sioaddr, NCT6775_LD_HWM);
    val = (superio_inb(sioaddr, SIO_REG_ADDR) << 8)
        | superio_inb(sioaddr, SIO_REG_ADDR + 1);
    addr = val & IOREGION_ALIGNMENT;
    if (addr == 0) {
        pr_err("Refusing to enable a Super-I/O device with a base I/O port 0\n");
        superio_exit(sioaddr);
        return -ENODEV;
    }

    superio_exit(sioaddr);
    pr_info("Found %s or compatible chip at %#x:%#x SMBus CLK 0x%x\n",
            nct6775_sio_names[kind], sioaddr, addr, smbus_clock);

    return ret;
}

static s32 sio_smbus_access(struct i2c_adapter *adapter, u16 addr, unsigned short flags, char read_write, u8 command, int size, union i2c_smbus_data *data)
{
    struct sio_smbus_priv *priv = i2c_get_adapdata(adapter);
    int ret = 0;
    int i = 0;
    int len = 0;
    u8 reg = 0, reg1 = 0;
    int start_tx = 1;

    //illegal command filter
    //if(data->block[0] == 0x0 && addr == 0x3e && command != 0xff)
    //{
    //    dev_err(priv->dev, "Unsupported command 0x%x\n",command);
    //    ret = -EOPNOTSUPP;
    //    goto out;
    //}

    mutex_lock(&priv->acpi_lock);

    //reg = inb(priv->bs_addr + SMCTL3);
    //reg1 = inb(priv->bs_addr + ERROR_STATUS);
    //if ((reg & 0x04) || (reg1 & 0x04))
    //{
    //    ret = -EBUSY;
    //    goto out;
    //}

    outb_p(0x0, priv->bs_addr + SMCTL);
    msleep(1);
    switch (size) {
        case I2C_SMBUS_BYTE:
            //Chip not support the command I2C_SMBUS_BYTE.
            outb_p(((addr & 0x7f) << 1) | (read_write & 0x01), priv->bs_addr + SMADDR);
            if (read_write == I2C_SMBUS_WRITE){
                outb_p(command, priv->bs_addr + SMIDX);
            }
            msleep(1);
            /* Manual mode set enable */
            outb_p(0x80, priv->bs_addr + SMCTL);
            break;
        case I2C_SMBUS_BYTE_DATA:
            reg = inb(priv->bs_addr + SMDATA);
            outb_p(((addr & 0x7f) << 1) | (read_write & 0x01), priv->bs_addr + SMADDR);
            outb_p(command, priv->bs_addr + SMIDX);
            if (read_write == I2C_SMBUS_WRITE) {
                outb_p(0x8, priv->bs_addr + SMCMD);
                outb_p(data->byte, priv->bs_addr + SMDATA);
            } else {
                outb_p(0x0, priv->bs_addr + SMCMD);
            }
            msleep(1);
            outb_p(0x80, priv->bs_addr + SMCTL);
            break;
        case I2C_SMBUS_WORD_DATA:
            outb_p(((addr & 0x7f) << 1) | (read_write & 0x01), priv->bs_addr + SMADDR);
            outb_p(command, priv->bs_addr + SMIDX);
            if (read_write == I2C_SMBUS_WRITE) {
                outb_p(0x9, priv->bs_addr + SMCMD);
                outb_p(data->word & 0xff, priv->bs_addr + SMDATA);
                outb_p((data->word & 0xff00) >> 8, priv->bs_addr + SMDATA);
            }else{
                outb_p(0x3, priv->bs_addr + SMCMD);
            }
            msleep(1);
            outb_p(0x80, priv->bs_addr + SMCTL);
            break;
        case I2C_SMBUS_BLOCK_DATA:
            outb_p(((addr & 0x7f) << 1) | (read_write & 0x01), priv->bs_addr + SMADDR);
            outb_p(command, priv->bs_addr + SMIDX);
            if (read_write == I2C_SMBUS_WRITE)
            {
                if (data->block[0] > I2C_SMBUS_BLOCK_MAX-1)
                {
                    ret=-EMSGSIZE;
                    goto out;
                }
                else if (data->block[0] == 0)
                {
                    ret=-EBADMSG;
                    goto out;
                }

                outb_p(0x0A, priv->bs_addr + SMCMD);
                outb_p( data->block[0], priv->bs_addr + SMWRSIZE);
                len = 0;
                for (i=0;i<1000;i++)
                {
                    reg = inb(priv->bs_addr + SMCTL3);
                    if (reg & 0x02)
                    {
                        if (((reg & 0x04) == 0) && (start_tx == 1))
                        {
                            outb_p(0x80, priv->bs_addr + SMCTL);
                            usleep_range(10, 50);
                            start_tx = 0;
                        }
                        else
                            usleep_range(10, 50);
                    }
                    else
                    {
                        outb_p(data->block[len+1], priv->bs_addr + SMDATA);
                        len++;
                    }

                    if (len == data->block[0])
                    {
                        if (start_tx == 1)
                        {
                            outb_p(0x80, priv->bs_addr + SMCTL);
                            usleep_range(10, 50);
                        }
                        break;
                    }
                }
            }
            else
            {
                outb_p(0x02, priv->bs_addr + SMCMD);
                msleep(1);
                outb_p(0x80, priv->bs_addr + SMCTL);
            }
            break;
        default:
            printk(KERN_ALERT "Unsupported transaction %d\n",size);
            ret = -EOPNOTSUPP;
            goto out;
    }

    if (read_write == I2C_SMBUS_WRITE)
    {
        //msleep(1);
        for (i=0;i<1000;i++)
        {
            reg = inb(priv->bs_addr + SMCTL3);
            if ((reg & 0x04) == 0)
                break;

            usleep_range(100, 500);
        }

        if (reg & 0x04)
        {
            //pr_warn("Operation can not finish by SMBUS_WRITE\n");
            printk(KERN_ALERT "operation can NOT finish\n");
            ret=-ETIMEDOUT;
            goto out;
        }
        goto out;
    }

    //msleep(1);
    switch (size) {
        case I2C_SMBUS_BYTE:
        case I2C_SMBUS_BYTE_DATA:
            for (i=0;i<1000;i++)
            {
                reg1 = inb_p(priv->bs_addr + SMCTL3);
                if (!(reg1 & 0x01))
                {
                    data->byte = inb(priv->bs_addr + SMDATA);
                    break;
                }
                else
                {
                    usleep_range(10, 50);
                }
            }
#if 0
            if ((reg1 & 0x01) && (I2C_SMBUS_BYTE_DATA == size))
            {
                ret = -ETIMEDOUT;
                pr_warn("I2C_SMBUS_BYTE_DATA operation timed out\n");
                goto out;
            }
#endif
            break;
        case I2C_SMBUS_WORD_DATA:
            data->word = 0;
            len = 0;
            for (i=0;i<1000;i++)
            {
                reg1 = inb_p(priv->bs_addr + SMCTL3);
                if (!(reg1 & 0x01))
                {
                    data->word = data->word  | (inb_p(priv->bs_addr + SMDATA) << 8);
                    len++;
                    if (len == 2)
                        break;
                }
                else
                    usleep_range(10, 50);
            }
            //if (len != 2)
            //{
            //    ret=-ETIMEDOUT;
            //    pr_warn("I2C_SMBUS_WORD_DATA operation timed out\n");
            //    goto out;
            //}
            break;

        case I2C_SMBUS_BLOCK_DATA:
            len = 0;
            data->block[0] = 0;

            for (i=0;i<1000;i++)
            {
                reg1 = inb_p(priv->bs_addr + SMCTL3);
                if (!(reg1 & 0x01))
                {
                    data->block[0] = inb(priv->bs_addr + SMDATA);
                    //ret = 0;
                    break;
                }
                msleep(2);
            }

            if (data->block[0] == 0)
            {
                //if (i == 100)
                //{
                //    ret = -ETIMEDOUT;
                //    pr_warn("I2C_SMBUS_BLOCK_DATA operation timed out\n");
                //}
                //else
                //{
                //    pr_err("I2C_SMBUS_BLOCK_DATA operation get date len is zero!\n");
                //}
                printk(KERN_ALERT "%s %d Get data len=0\n", __func__,__LINE__);
                goto out;
            }

            for (i=0;i<1000;i++)
            {
                reg1 = inb_p(priv->bs_addr + SMCTL3);
                if (!(reg1 & 0x01))
                {
                    data->block[len+1] = inb(priv->bs_addr + SMDATA);
                    len++;
                    if ((len > I2C_SMBUS_BLOCK_MAX-1) || (len == data->block[0]))
                        break;
                }
                else
                {
                    usleep_range(10, 50);
                }
            }
            if (data->block[0] != len)
            {
                //pr_warn("Expect to get len %d but actual get %d\n", data->block[0], len);
                printk(KERN_ALERT "!!!%s %d get len=%d but actual get %d\n", __func__,__LINE__,
					data->block[0],len);
                data->block[0] = len;
            }
            break;
        default:
            break;
    }
out:
    //SMBUS software reset
    outb_p(NCT6775_REG_CONFIG, priv->bs_addr + SMCTL);
    //if (ret != 0)
    //{
    //    reg = inb(priv->bs_addr + SMCTL3);
    //    reg1 = inb(priv->bs_addr + ERROR_STATUS);
    //    pr_warn("Operation error, addr = 0x%x size = %d SMCTL3 = 0x%x ERROR_STATUS = 0x%x\n",
    //            addr, size, reg, reg1);
    //}

    mutex_unlock(&priv->acpi_lock);
    return ret;
}

static u32 sio_smbus_func(struct i2c_adapter *adapter)
{
    return (I2C_FUNC_SMBUS_BYTE | I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA | I2C_FUNC_SMBUS_BLOCK_DATA | I2C_FUNC_SMBUS_BLOCK_PROC_CALL);
}

static const struct i2c_algorithm smbus_algorithm = {
    .smbus_xfer = sio_smbus_access,
    .functionality  = sio_smbus_func,
};

static int sio_smbus_probe(struct platform_device *pdev)
{
    int ret = 0;
    struct sio_smbus_priv *priv = NULL;

    priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
    if (!priv)
        return -ENOMEM;

    strlcpy(priv->adapter.name, DRVNAME, sizeof(priv->adapter.name));

    priv->adapter.owner = THIS_MODULE;
    priv->adapter.algo = &smbus_algorithm;
    priv->adapter.algo_data = priv;
    priv->adapter.dev.parent = &pdev->dev;
    i2c_set_adapdata(&priv->adapter, priv);
    priv->dev = &pdev->dev;

    platform_set_drvdata(pdev, priv);
    mutex_init(&priv->acpi_lock);
    ret = i2c_add_adapter(&priv->adapter);
    if (ret) {
        //dev_err(&pdev->dev, "failed to add bus to i2c core \n");
        printk(KERN_ALERT "failed to add bus to i2c core \n");
        return ret;
    }

    ret = superio_proc(priv);

    return ret;
}

static int sio_smbus_remove(struct platform_device *pdev)
{
    struct sio_smbus_priv *priv = platform_get_drvdata(pdev);
    i2c_del_adapter(&priv->adapter);
    return 0;
}

static void sio_smbus_release(struct device *dev)
{
    //Nothing to do now, take care iomem resources
    return;
}

static struct platform_driver sio_smbus_driver = {
    .driver     = {
        .name   = DRVNAME,
    },
    .probe      = sio_smbus_probe,
    .remove     = sio_smbus_remove,
};

static struct platform_device sio_smbus_device = {
    .name = DRVNAME,
    .id = -1,
    .dev = {
        .release = sio_smbus_release,
    },
};

static int sio_smbus_init(void)
{
    int ret = 0;
#if 0
    mutex_init(&bmc_device_lock);
    mutex_init(&pca9548_device_lock);
#endif    
    ret = platform_device_register(&sio_smbus_device);
    if (ret)
        goto exit_error;

    ret = platform_driver_register(&sio_smbus_driver);
    if (ret)
        goto exit_device_unregister;

    //pr_info("SuperIO SMBus driver init\n");
    printk(KERN_ALERT "SuperIO SMBus driver init \n");
    return 0;

exit_device_unregister:
    platform_device_unregister(&sio_smbus_device);
exit_error:
    return ret;
}

void sio_smbus_exit(void){
    platform_device_unregister(&sio_smbus_device);
    platform_driver_unregister(&sio_smbus_driver);
    //pr_info("SuperIO SMBus driver exit\n");
    printk(KERN_ALERT "SuperIO SMBus driver exit \n");
}

#if 0
void sio_smbus_3e_lock(void)
{
    mutex_lock(&bmc_device_lock);
}

void sio_smbus_3e_unlock(void)
{
    mutex_unlock(&bmc_device_lock);
}

void sio_smbus_71_lock(void)
{
    mutex_lock(&pca9548_device_lock);
}

void sio_smbus_71_unlock(void)
{
    mutex_unlock(&pca9548_device_lock);
}
#endif

MODULE_AUTHOR("wangzhui <wangzhui@asterfusion.com>");
MODULE_DESCRIPTION("SuperIO SMBus driver");
MODULE_LICENSE("GPL") ;

module_init(sio_smbus_init);
module_exit(sio_smbus_exit);

#if 0
EXPORT_SYMBOL(sio_smbus_3e_lock);
EXPORT_SYMBOL(sio_smbus_3e_unlock);
EXPORT_SYMBOL(sio_smbus_71_lock);
EXPORT_SYMBOL(sio_smbus_71_unlock);
#endif