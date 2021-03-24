#!/bin/bash
# give access rights for an install by specifying the path

if [ "$1" = "" ]
then
	echo "Usage :"
	echo "> change_droits.sh folder"
	echo "where folder is the name of TRUST folder"
	return 255
fi
cd $1 || exit 1

echo "changing access rights in: `pwd`"

echo "Add +r for all folders and files in specified path"
chmod -R ugo+r .

echo "Add +x for all sub-folders"
chmod ugo+rx `find . -type d`

echo "Add +x for all executables"
chmod ugo+x `find . -type f -executable`

echo "everything should be fine now!"

