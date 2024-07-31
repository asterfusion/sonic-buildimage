#!/usr/bin/env bash

# Manually specify platform.conf file path
cfgfile="/usr/share/sonic/device/x86_64-asterfusion_cx308p_48y_t-r0/platform.conf"
# cfgfile="/usr/share/sonic/device/x86_64-asterfusion_cx312p_48y_t-r0/platform.conf"
# cfgfile="/usr/share/sonic/device/x86_64-asterfusion_cx532p_t-r0/platform.conf"
# cfgfile="/usr/share/sonic/device/x86_64-asterfusion_cx564p_t-r0/platform.conf"

function check_config_existence() {
    # Color unicode-escape code
    local yellow='\E[1;33m'
    local res="\E[0m"
    if [ -f $cfgfile ]; then
        echo -e "${yellow}Warning${res}: We found that the $cfgfile already exists and no new configuration file will be generated."
        echo -e "${yellow}Warning${res}: The purpose of this prompt is to take into account that your system may have worked fine in the past."
        echo -e "${yellow}Warning${res}: If you want to generate a new configuration file, delete the old one first."
        exit 0
    fi
}

function get_from_bmc_with_delay() {
    result=$(bmc_get $@)
    sleep 0.3
    echo $result
}

function generate_platform_config() {
    # Useful variables
    default_cme="CME3000"
    default_i2c="255"
    hw_platform="N/A"
    hw_version="0"
    enable_uart=0
    enable_iic=1
    xt_platform=""
    xt_hwver=""
    xt_hwsubver=""

    # First we try using bmc_get to retrieve eeprom data
    if ! bmc_version_10hex=$(get_from_bmc_with_delay 0xd 0xaa 0xaa); then
        echo "Error: Failed to get bmc version."
    else
        if [[ "$bmc_version_10hex"x = "read failed"x ]]; then
            enable_uart=0
        else
            bmc_version_10hex=${bmc_version_10hex:2}
            var1=$(echo "$bmc_version_10hex"|awk -F ' ' '{print $1}')
            var2=$(echo "$bmc_version_10hex"|awk -F ' ' '{print $2}')
            var3=$(echo "$bmc_version_10hex"|awk -F ' ' '{print $3}')
            var1=$((16#$var1))
            var2=$((16#$var2))
            var3=$((16#$var3))
            xt_platform=$(get_from_bmc_with_delay 0x1 0x21 0xaa)
            enable_uart=1
            xt_hwver=$(get_from_bmc_with_delay 0x1 0x31 0xaa)
            num=$(echo "$xt_hwver" | awk -F"-" '{print NF-1}')
            if [[ $num = 3 ]]; then
                xt_hwver=${xt_hwver%-*}
            fi
            hw_version=${xt_hwver: -3}
            echo "BMC firmware version is $var1.$var2.$var3."
        fi
    fi

    # Second we try using i2cset & i2cdump
    if [ $enable_uart -eq 0 ] && [ "$xt_platform"X = ""X ]; then
        i2c=$(i2cdetect -l | awk -F '[- ]' '/sio_smbus/{print $2}')
        default_i2c=${i2c:0:1}
        if [ ! "$default_i2c" ]; then
            default_i2c=$(i2cdetect -l | grep CP2112 | awk '{print $1}')
            if [ "$default_i2c" ]; then
                echo "Error: Failed to find default i2c."
            fi
        fi
        i2cset -y "$default_i2c" 0x3e 0x01 0x21 0xaa s
        if ! rc=$(i2cdump -y "$default_i2c" 0x3e s 0xff); then
            i2c=$(i2cdetect -l | awk -F '[- ]' '/CP2112/{print $2}')
            default_i2c=${i2c:0:1}
            if [ ! "$default_i2c" ]; then
                echo "Error: Failed to find default i2c."
            fi
            i2cset -y "$default_i2c" 0x3e 0x01 0x21 0xaa s
            if ! rc=$(i2cdump -y "$default_i2c" 0x3e s 0xff); then
                echo "Error: Failed to dump eeprom data with i2cdump."
            fi
        fi
        if [[ $rc =~ "312" ]]; then
            xt_platform="X312P-T"
            i2cset -y "$default_i2c" 0x3e 0x01 0x26 0xaa s
            xt_hwver=$(i2cdump -y "$default_i2c" 0x3e s 0xff)
            i2cset -y "$default_i2c" 0x3e 0x01 0x27 0xaa s
            xt_hwsubver=$(i2cdump -y "$default_i2c" 0x3e s 0xff)
            hw_version="${xt_hwver: -1}.${xt_hwsubver: -1}"
        fi
    fi

    # Lastly we try using cgosi2c
    if [ $enable_uart -eq 0 ] && [ "$xt_platform"X = ""X ]; then
        rc=$(cgosi2c 0x3e 0x01 0x21 0xaa)
        if [ "$rc"X = "ERROR: Failed to write to EEPROM."X ]; then
            echo "Error: Failed to write to EEPROM."
        else
            xt_platform=$(cgosi2c 0x3e)
        fi
    fi

    if [ "$xt_platform"X = ""X ]; then
        echo "Error: Failed to get platform."
    fi

    if [ "$xt_platform"X != ""X ] && [ $enable_uart = 1 ]; then
        xt_cme=$(get_from_bmc_with_delay 0x1 0x32 0xaa)
        if [[ $xt_cme =~ "CGT" ]]; then
            default_cme="CG1527"
            if [[ $xt_cme =~ "08" ]]; then
                default_cme="CG1508"
            fi
        fi
        if [[ $xt_cme =~ "ADV" ]]; then
            default_cme="ADV1527"
            if [[ $xt_cme =~ "08" ]]; then
                default_cme="ADV1508"
            fi
        fi
        if [[ $xt_cme =~ "S02" ]]; then
            default_cme="S021527"
            if [[ $xt_cme =~ "08" ]]; then
                default_cme="S021508"
            fi
        fi
        if [[ $xt_cme =~ "CME3000" ]]; then
            default_cme="CME3000"
        fi
    fi

    if [[ $xt_platform =~ "308" ]]; then
        enable_iic=0
        if [[ $default_cme =~ "CME3000" ]]; then
            i2c=$(i2cdetect -l | awk -F '[- ]' '/sio_smbus/{print $2}')
            default_i2c=${i2c:0:1}
            enable_iic=1
        fi
        hw_platform="X308P-T"
    fi

    if [ "$default_cme"X = "CME3000"X ] && [[ $xt_platform =~ "312" ]]; then
        enable_iic=1
        hw_platform="X312P-T"
    fi

    if [[ $xt_platform =~ "532" ]]; then
        enable_iic=0
        if [[ $default_cme =~ "CME3000" ]]; then
            i2c=$(i2cdetect -l | awk -F '[- ]' '/sio_smbus/{print $2}')
            default_i2c=${i2c:0:1}
            enable_iic=1
        fi
        hw_platform="X532P-T"
    fi

    if [[ $xt_platform =~ "564" ]]; then
        enable_iic=0
        if [[ $default_cme =~ "CME3000" ]]; then
            i2c=$(i2cdetect -l | awk -F '[- ]' '/sio_smbus/{print $2}')
            default_i2c=${i2c:0:1}
            enable_iic=1
        fi
        hw_platform="X564P-T"
    fi

    echo "Generating $cfgfile ..."
    {
        echo "# Generated platform.conf by xt-cfgen.sh"
        echo ""
        echo "# X-T Bare Metal Hardware Platform."
        echo "# Currently supported X-T Bare Metal like below:"
        echo "#   1. X532P-T (Default)"
        echo "#   2. X564P-T"
        echo "#   3. X308P-T"
        echo "#   4. X312P-T"
        echo "platform:$hw_platform"
        echo ""
        echo "# X-T Bare Metal Hardware Version."
        echo "hwver:$hw_version"
        echo ""
        echo "# COM-Express (X86)."
        echo "# Currently supported COM-Express listed below:"
        echo "#   1. CG1508 (Default)"
        echo "#   2. CG1527"
        echo "#   3. ADV1508"
        echo "#   4. ADV1527"
        echo "#   5. S021508"
        echo "#   6. S021527"
        echo "#   7. CME3000"
        echo "#   8. CME7000"
        echo "com-e:$default_cme"
        echo ""
        echo "# Master I2C which is used to access CPLD and/or BMC."
        echo "#"
        echo "#             [X312P-T V3.0 and later]      [X308P-T]           [X532P-T/X564P-T]"
        echo "#                      |                       |                        |"
        echo "#  BMC          <---- nct6779d          <---- UART               <---- UART"
        echo "#  CPLD         <---- nct6779d          <---- cp2112             <---- cp2112"
        echo "#  Transceiver  <---- cp2112            <---- cp2112             <---- cp2112"
        echo "#"
        echo "# Details"
        echo "# For X532P-T/X564P-T with ComExpress CG15xx serials, i2c-127 means CPLD forcely accessed through cgosdrv (transition scenarios)."
        echo "# For X532P-T V1.0 and V1.1, CPLD can be accessed by cgosdrv and as well by cp2112 (default by cp2112)."
        echo "# For X532P-T V2.0, CPLD can be accessed by cgosdrv and as well by cp2112 (default by cp2112)."
        echo "# For X564P-T V1.0 and V1.1, CPLD can be and only can be accessed by cgosdrv."
        echo "# For X564P-T V1.2, CPLD can be accessed by cgosdrv and as well by cp2112 (default by cp2112)."
        echo "# For X564P-T V2.0, CPLD can be and only can be accessed by cp2112."
        echo "# For X312P-T V1.0, nct6779d is used to access BMC/CPLD/Transceiver."
        echo "# For X312P-T V2.0, cp2112 is used to access BMC/CPLD/Transceiver."
        if [ $enable_iic = 1 ]; then
            echo "i2c:$default_i2c"
        else
            echo "#i2c:$default_i2c"
        fi
        echo ""
        echo "# An internal console which is used to access BMC."
        echo "# If given, BSP will access BMC through this console, otherwise, use i2c instead."
        echo "# by tsihang, 2021-07-05"
        if [ $enable_uart = 1 ]; then
            echo "uart:/dev/ttyS1"
        else
            echo "#uart:/dev/ttyS1"
        fi
        echo ""
    } > $cfgfile
    echo "Generating $cfgfile done."
}

function get_bmc_mac_address() {
    mac_cache="/var/cache/sonic/system-macaddr/macaddr_cache"
    if [ ! -e $mac_cache ]; then
        mac_addr=$(get_from_bmc_with_delay 0x01 0x24 0xaa)
        echo "$mac_addr" > $mac_cache
    fi
    if [[ "$1" = "-r" ]]; then
        cat "$mac_cache"
    fi
}

function display_help_information() {
    echo "usage: xt-cfgen.sh [-h] [-p] [-m] [-r]"
    echo ""
    echo "Generate platform specific configuration files."
    echo ""
    echo "optional arguments:"
    echo "-h    show this help message and exit"
    echo "-p    generate platform.conf under platform device directory"
    echo "-m    generate bmc mac address under /var/cache/sonic/"
    echo "-r    generate and print bmc mac address"
    exit 0
}

while getopts ":hpmr" opt; do
    case $opt in 
        h)
            display_help_information
            ;;
        p)
            check_config_existence
            generate_platform_config
            ;;
        m)
            get_bmc_mac_address
            ;;
        r)
            get_bmc_mac_address -r
            ;;
    esac
done
