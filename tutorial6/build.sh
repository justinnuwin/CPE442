#!/bin/bash

git submodule init petalinux-docker
git submodule update petalinux-docker

cp petalinux-docker/Dockerfile petalinux-docker/accept-eula.sh .

patch -u Dockerfile -i Dockerfile.patch -R

echo "Download petalinux from https://www.xilinx.com/support/download.html"
# Installer should have a name like: petalinux-v2018.3-final-installer.run
peta_version=2018.3
peta_installer=petalinux-v$peta_version-final-installer.run

read -p "Is Petalinux downloaded? (y/n) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    exit 1
fi

docker build --build-arg PETA_VERSION=$peta_version --build-arg PETA_RUN_FILE=$peta_installer --tag petalinux:$peta_version .

rm -f Dockerfile accept-eula.sh

git submodule init Petalinux-Zybo
git submodule update Petalinux-Zybo
git -C Petalinux-Zybo checkout v2017.4-1
# wget https://github.com/Digilent/Petalinux-Zybo/releases/download/v2017.4-1/Petalinux-Zybo-2017.4-1.bsp -P project/

mkdir -p project/ubuntu-base-18.04-armhf/rootfs
# http://cdimage.ubuntu.com/ubuntu-base/releases/18.04/release/
wget http://cdimage.ubuntu.com/ubuntu-base/releases/18.04/release/ubuntu-base-18.04.3-base-armhf.tar.gz -P project/ubuntu-base-18.04-armhf/
sudo tar -xzf project/ubuntu-base-18.04-armhf/*.tar.gz -C project/ubuntu-base-18.04-armhf/rootfs/
