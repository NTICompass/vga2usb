#!/bin/sh

# install udev rules
[ -d "/lib/udev/rules.d" ] && install -m 0644 extra/40-epiphan.rules /lib/udev/rules.d/

# for vga2pci only
[ -f extra/v2u_flash ] && install -m 0755 extra/v2u_flash /usr/bin/v2u_flash
