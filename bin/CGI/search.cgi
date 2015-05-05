#!/bin/bash
DOXYSEARCH=/usr/bin/doxysearch
DOXYPATH=../../doc/html
if [ -f $DOXYSEARCH ]
then
  $DOXYSEARCH $DOXYPATH
else
  echo "Content-Type: text/html"
  echo ""
  echo "<h2>Error: $DOXYSEARCH not found. Check cgi script!</h2>"
fi
