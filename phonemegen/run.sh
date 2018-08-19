#!/bin/sh
ipa2chip | minicom -b 115200 -D /dev/ttyUSB0
