#!/bin/bash
sed "s/\\\\-\\\\//g;s/,//;s/{\\\bf//;s/\\\\_/_/g" doc/doc.ind  | awk '{ if ($1=="\\item") {print "|"$2}}' | sort -u  > Keywords.txt

