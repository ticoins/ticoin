#!/bin/bash
# create multiresolution windows icon
ICON_SRC=../../src/qt/res/icons/ticoin.png
ICON_DST=../../src/qt/res/icons/ticoin.ico
convert ${ICON_SRC} -resize 16x16 ticoin-16.png
convert ${ICON_SRC} -resize 32x32 ticoin-32.png
convert ${ICON_SRC} -resize 48x48 ticoin-48.png
convert ticoin-16.png ticoin-32.png ticoin-48.png ${ICON_DST}

