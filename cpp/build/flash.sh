#!/bin/bash

cmake ..
make hello_world
echo "Put stick to boot mode"
read
sudo mount /dev/sdb1 /mnt/usb
sudo cp hello_world.uf2 /mnt/usb
sudo umount /mnt/usb
