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
git clone --recursive https://github.com/Digilent/Petalinux-Zybo-Z7-10.git project/Petalinux-Zybo/
git -C project/Petalinux-Zybo/ checkout v2017.4-1

mkdir -p project/ubuntu-base-18.04-armhf/rootfs
# General Ubuntu Base release page: http://cdimage.ubuntu.com/ubuntu-base/releases/18.04/release/
wget http://cdimage.ubuntu.com/ubuntu-base/releases/18.04/release/ubuntu-base-18.04.3-base-armhf.tar.gz -P project/ubuntu-base-18.04-armhf/

# read to delay sudo password prompt timeout expiring
read -p "Press any key to continue " -n 1 -r
sudo tar -xzf project/ubuntu-base-18.04-armhf/*.tar.gz -C project/ubuntu-base-18.04-armhf/rootfs/
