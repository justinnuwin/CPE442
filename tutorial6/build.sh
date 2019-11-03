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

mkdir -p project 
wget --show-progress -P project/ https://github.com/Digilent/Petalinux-Zybo/releases/download/v2017.4-1/Petalinux-Zybo-2017.4-1.bsp
