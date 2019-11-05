patch -u Petalinux-Zybo/Zybo-7Z-10/project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi -i system-user.dtsi.patch
cp --verbose petalinux_config Petalinux-Zybo/Zybo-7Z-10/project-spec/configs/config

cd Petalinux-Zybo/Zybo-Z7-10/

petalinux-build
petalinux-package --boot --force --fsbl images/linux/zynq_fsbl.elf --fpga images/linux/system_wrapper.bit --u-boot
