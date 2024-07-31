#!/bin/bash

function load_drivers() {
    depmod -a
    modprobe i2c_i801
    modprobe i2c_dev
    modprobe i2c_mux
    modprobe i2c_mux_pca954x force_deselect_on_exit=1
    modprobe cgosdrv
    modprobe ixgbe
    modprobe ip_gre
    modprobe ip6_gre
    modprobe mpls_router
    modprobe mpls_iptunnel
    sleep 1
}

function unload_drivers() {
    modprobe -r mpls_iptunnel
    modprobe -r mpls_router
    modprobe -r ip6_gre
    modprobe -r ip_gre
    modprobe -r ixgbe
    modprobe -r cgosdrv
    modprobe -r i2c_mux_pca954x
    modprobe -r i2c_mux
    modprobe -r i2c_dev
    modprobe -r i2c_i801
    sleep 1
}

function init_cache() {
    mkdir -p /var/cache/sonic/system-macaddr
    mkdir -p /var/cache/sonic/decode-syseeprom
    xt-cfgen.sh -m -p
}

function inst_pyapi() {
    device="/usr/share/sonic/device"
    platform=$(sonic-cfggen -H -v DEVICE_METADATA.localhost.platform)
    pip3 install $device/$platform/sonic_platform-1.0-py3-none-any.whl
}

function setup_hardware() {
    if [ "$( cat /sys/class/net/eth0/operstate )" = "up" ]; then
        echo "Sync time..."
        timedatectl set-timezone Asia/Shanghai
        hwclock --systohc
    fi
    sudo ifconfig eth1 up
    sudo ifconfig eth2 up
}

case "$1" in
    start)
        echo "Setting up platform..."
        load_drivers
        init_cache
        inst_pyapi
        setup_hardware
        echo "Setting up platform done."
        ;;

    stop)
        echo "De-initializing platform..."
        unload_drivers
        echo "De-initializing done."
        ;;
    restart)
        $0 stop
        $0 start
        ;;
    status)
        echo "Asterfusion CX532P-T is running..."
        ;;
    *)
        echo "Usage: $0 {start|stop|status|restart}"
    esac

exit 0
