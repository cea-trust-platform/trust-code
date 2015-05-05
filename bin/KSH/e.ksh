#!/bin/bash
line=$1
file=$2
vi -c :e +$line $file
