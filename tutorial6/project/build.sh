patch -u Petalinux-Zybo/Zybo/project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi -i system-user.dtsi.patch
cp --verbose petalinux_config Petalinux-Zybo/Zybo/project-spec/configs/config

cd Petalinux-Zybo/Zybo/

petalinux-util --webtalk off
petalinux-build
petalinux-package --boot --force --fsbl images/linux/zynq_fsbl.elf --fpga images/linux/system_wrapper.bit --u-boot
