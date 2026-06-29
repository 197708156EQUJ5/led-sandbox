#!/bin/bash
sudo kill -9 `sudo ss -xlpn | grep '/tmp/led-display.sock' | awk {'print $9'} | cut -d',' -f 2 | cut -d'=' -f 2`