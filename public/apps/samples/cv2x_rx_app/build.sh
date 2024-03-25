#!/bin/bash
source /usr/local/oecore-x86_64/environment-setup-aarch64-oe-linux

echo -e "$CXX -fpic -shared -o libv2x.so v2x.cpp"
$CXX -fpic -shared -o libv2x.so -lpthread v2x.cpp
echo -e "$CXX rx.c -o rx -L. libtelux_cv2x.so libv2x.so"
$CXX rx.c -o rx-tx -lpthread -L. libtelux_cv2x.so libv2x.so
#echo -e "$CXX tx.c -o tx -L. libtelux_cv2x.so libv2x.so"
#$CXX tx.c -o tx -lpthread -L. libtelux_cv2x.so libv2x.so
