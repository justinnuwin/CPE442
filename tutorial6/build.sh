#!/bin/bash

git submodule update --init petalinux-docker

cp petalinux-docker/Dockerfile petalinux-docker/accept-eula.sh .

patch -u Dockerfile -i Dockerfile.patch

echo "Download petalinux from https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-design-tools.html"
# Installer should have a name like: petalinux-v2017.4-final-installer.run
# Use Petalinux 2017.4 as it matches the latest Zybo build. Using a newer
# version will break on kernel compilation as 2017 builds Kernel 4.9. This
# process was tested on 2018.3 which builds Kernel 4.14 and fails version check.
peta_version=2017.4
peta_installer=petalinux-v$peta_version-final-installer.run

read -p "Is Petalinux downloaded? (y/n) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    exit 1
fi

docker build --build-arg PETA_VERSION=$peta_version --build-arg PETA_RUN_FILE=$peta_installer --tag petalinux:$peta_version .

rm -f Dockerfile accept-eula.sh

# Yocto can't build git dependencies with nested submodules, so we will clone
git clone --recursive https://github.com/Digilent/Petalinux-Zybo.git project/Petalinux-Zybo/
git -C project/Petalinux-Zybo/ checkout v2017.4-1


mkdir -p project/rootfs/

# Ubuntu Base 18.04
# wget http://cdimage.ubuntu.com/ubuntu-base/releases/18.04/release/ubuntu-base-18.04.3-base-armhf.tar.gz -P project/rootfs/

# Ubuntu Minimal 18.04: https://www.digikey.com/eewiki/display/linuxonarm/Zynq-7000#Zynq-7000-Ubuntu18.04LTS
wget https://rcn-ee.com/rootfs/eewiki/minfs/ubuntu-18.04.1-minimal-armhf-2018-07-30.tar.xz -P project/rootfs/
diff -s \
    <(sha256sum project/rootfs/ubuntu-18.04.1-minimal-armhf-2018-07-30.tar.xz) \
    <(echo "6b212ee7dd0d5c9c0af49c22cf78b63e6ad20cec641c303232fca9f21a18804c  project/rootfs/ubuntu-18.04.1-minimal-armhf-2018-07-30.tar.xz")

if [ $? -ne 0 ]; then
    echo "Rootfs image checksum mismatch!"
    exit 1
else
    echo "Ensure that rootfs has chown root:root && chmod 755 permissions"
fi
