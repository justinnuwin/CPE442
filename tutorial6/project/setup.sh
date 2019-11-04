touch Petalinux-Zybo/Zybo/project-spec/meta-user/recipes-core/images/petalinux-image-full.bbappend
patch -u Petalinux-Zybo/Zybo/project-spec/meta-user/recipes-bsp/device-tree/files/system-user.dtsi -i system-user.dtsi.patch
cd Petalinux-Zybo/Zybo/


petalinux-build
petalinux-package --boot --force --fsbl images/linux/zynq_fsbl.elf --fpga images/linux/system_wrapper.bit --u-boot
