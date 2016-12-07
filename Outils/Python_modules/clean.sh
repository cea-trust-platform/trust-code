#!/bin/bash

dirs=`cd $TRUST_ROOT/externalpackages/Python_modules/; ls *.tar.gz *.zip | sed "s/.tar.gz//g;s/.zip//"`
rm -rf $dirs

