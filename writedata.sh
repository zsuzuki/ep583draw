#!/bin/sh

if [ "$1" == "-t" ]; then
    picotool load ../bdfconv/data/myrc22.fnt -t bin -o 0x10100000
    picotool load ../bdfconv/data/ibmm22.fnt -t bin -o 0x10130000
    picotool load ../bdfconv/data/kpop22.fnt -t bin -o 0x10160000
    picotool load ../bdfconv/data/logo22.fnt -t bin -o 0x10190000
    picotool load ../bdfconv/data/pop22.fnt  -t bin -o 0x101C0000
fi
sleep 1
if [ -e /Volumes/RPI-RP2/picoe583b.uf2 ]; then
    rmdir /Volumes/RPI-RP2/picoe583b.uf2
fi
cp ./build/picoe583b.uf2 /Volumes/RPI-RP2
