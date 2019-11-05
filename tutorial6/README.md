# CPE442 Zybo-7000 Set up

## Build bootloader, kernel, rootfs

Requires: Docker

Download PetaLinux from [https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-design-tools.html](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-design-tools.html)

Be sure to download version 2017.4 and place it in this directory.

```
$ ./build.sh
$ ./launch.sh
docker-env$ ./build.sh
```

## Set up SD Card

Partition a SD card with the following:

1: primary FAT >500M bootable
2: primary ext4 >3G

## Install bootloader, kernel, and rootfs

Copy `BOOT.BIN` and `image.ub` from the Petalinux output (project/Petalinux-Zybo/Zybo/images/linux/) to partition 1 of the SD card. Make sure they are owned by root:root.

Extract the rootfs to partition 2 of the SD card. Make sure permissions are preserved or owned by root:root and 755.
