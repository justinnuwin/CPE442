#!/bin/bash

git submodule init petalinux-docker
git submodule update petalinux-docker

# petalinux-v2018.3-final-installer.run
echo "Download petalinux from https://www.xilinx.com/support/download.html"

read -p "Is Petalinux downloaded? (y/n) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    exit 1
fi

docker build --build-arg PETA_VERSION=2018.3 --build-arg PETA_RUN_FILE=petalinux-v2018.3-final-installer.run -t petalinux:2018.3 ./petalinux-docker

mkdir -p project 
wget -o project/ https://github.com/Digilent/Petalinux-Zybo/releases/download/v2017.4-1/Petalinux-Zybo-2017.4-1.bsp
