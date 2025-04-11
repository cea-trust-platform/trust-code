#!/bin/sh

trust -partition test_conduc_para.data 2  # don't know why this sometimes returns non 0 even if OK ...
mpirun -np 2 python test_trusticoco_para.py

