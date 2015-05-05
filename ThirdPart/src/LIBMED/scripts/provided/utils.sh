#! /bin/bash
#
# --
# HDF5 install builder
#
# Author : Eli LAUCOIN (CEA)
# --

sed_() {
    f=$1
    shift
    c="$*"
    mv "${f}" "${f}.bak"
    sed -e "${c}" "${f}.bak" > "${f}"
    rm -f "${f}.bak"
}
