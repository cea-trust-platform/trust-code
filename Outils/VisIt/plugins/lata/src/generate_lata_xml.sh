#!/bin/bash

if [ "$2" = "-with_med" ]
then
echo "on active le support med"
med_extension="*.med"
med_cxx_flags="-DNDEBUG -DWITH_MEDLOADER -I"${TRUST_MEDCOUPLING_ROOT}"/include/ -I"${TRUST_MED_ROOT}"/include"
med_libs="${TRUST_MEDCOUPLING_ROOT}/lib/libmedloader.a    ${TRUST_MEDCOUPLING_ROOT}/lib/libmedcouplingremapper.a ${TRUST_MEDCOUPLING_ROOT}/lib/libmedcoupling.a  ${TRUST_MEDCOUPLING_ROOT}/lib/libinterpkernel.a ${TRUST_MED_ROOT}/lib/libmed.a ${TRUST_MED_ROOT}/lib/libmedimport.a ${TRUST_MED_ROOT}/lib/libmedC.a ${TRUST_MED_ROOT}/lib/libhdf5.a -lz -lm  "  
fi


paste_filenames ()
{
	for i in $1/commun_triou/*.cpp $1/triou_compat/*.cpp
	do
		  echo "        " `basename $i .cpp`.C
	done >>lata.xml
}

if [ ! -d "$1/commun_triou" ]
then
	echo Error: expected parameter lata2dx source directory
	exit
fi
LATASRC=$1

# -Wno-depreacted ne marche pas sous windows
cat > lata.xml <<EOF
<?xml version="1.0"?>
  <Plugin name="lata" type="database" label="lata import file" version="1.0" enabled="true" mdspecificcode="false" onlyengine="false" noengine="false" dbtype="MTMD" haswriter="false" hasoptions="false">
    <CXXFLAGS>
    ${med_cxx_flags}
    </CXXFLAGS>
  <LIBS>
    ${med_libs}
    </LIBS>

    
EOF
      #-O2 -DNDEBUG -Wall 
if [ "$version_visit" = "1" ]
then
cat >> lata.xml<<EOF
      <Extensions>
      lml
      lata
      </Extensions>
EOF
else
cat >> lata.xml<<EOF
   <FilePatterns>
      ${med_extension}
      *.lml
      *.lata
    </FilePatterns>
EOF
fi
cat >> lata.xml<<EOF
    
    <Files components="M">

      avtlataFileFormat.C
EOF
paste_filenames $LATASRC
cat >> lata.xml <<EOF
    </Files>
    <Files components="E">
      avtlataFileFormat.C
EOF
paste_filenames $LATASRC
cat >> lata.xml <<EOF
    </Files>
    <Attribute name="" purpose="" persistent="true" exportAPI="" exportInclude="">
    </Attribute>
  </Plugin>
EOF
