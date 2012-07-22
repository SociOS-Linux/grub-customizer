#!/bin/sh
cmake . && make && sudo cp --parents -u grub-customizer grubcfg-proxy /opt/grub-customizer/ && sudo cp --parents translation/german.mo /opt/grub-customizer/
