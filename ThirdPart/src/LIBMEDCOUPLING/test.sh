#!/bin/bash
. ./install/env.sh
copie_cas_test Kernel_Post_MED

python test.py
status=$?
rm -rf Kernel_Post_MED
exit $status
