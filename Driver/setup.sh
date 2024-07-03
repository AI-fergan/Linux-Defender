#!/bin/bash
sudo mknod /dev/spybot c 510 0
user=$(whoami)
sudo chown $user /dev/spybot
make
addr=$(python kallsyms.py)
sudo insmod spybot.ko kallsyms_lookup_addr="$addr"
