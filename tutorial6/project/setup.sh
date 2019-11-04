cd Petalinux-Zybo/Zybo/

touch project-spec/meta-user/recipes-core/images/petalinux-image-full.bbappend

petalinux-build
petalinux-package --boot --force --fsbl images/linux/zynq_fsbl.elf --fpga images/linux/system_wrapper.bit --u-boot
