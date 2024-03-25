#!/bin/bash
source /usr/local/oecore-x86_64/environment-setup-aarch64-oe-linux

echo -e "$CXX Cv2xTxApp.cpp -o tx -L. libtelux_cv2x.so"
$CXX Cv2xTxApp.cpp -o tx -L. libtelux_cv2x.so
