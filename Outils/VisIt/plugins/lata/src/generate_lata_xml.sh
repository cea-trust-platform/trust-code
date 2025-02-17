#!/bin/bash

set -e  # Exit on error

all_srcs=$1  # All plugin sources ...
if [ "$2" = "-with_med" ]; then
    echo "  -> with MED support"
    med_extension="*.med"
    medc_root="${TRUST_MEDCOUPLING_ROOT}/${TRUST_ARCH}_opt"
    med_cxx_flags="-DNDEBUG -DWITH_MEDLOADER -I${medc_root}/include/ -I${TRUST_MED_ROOT}/include"
    med_libs="${medc_root}/lib/libmedloader.a  ${medc_root}/lib/libmedcouplingremapper.a ${medc_root}/lib/libmedcoupling.a ${medc_root}/lib/libinterpkernel.a ${TRUST_MED_ROOT}/lib/libmed.a ${TRUST_MED_ROOT}/lib/libmedimport.a ${TRUST_MED_ROOT}/lib/libmedC.a ${TRUST_HDF5_ROOT}/lib/libhdf5.a -lz -lm  "  
fi

extra_incl=" -I${TRUST_ROOT}/src/Kernel/Utilitaires -I${TRUST_ROOT}/src/Kernel/Math -I${TRUST_ROOT}/src/Kernel/Geometrie "

#
# 64b management - lata_tools (and ViSit plugin) are always compiled in 64b
#
if [ "$TRUST_INT64" = "0" ] || [ "$TRUST_INT64_NEW" = "1" ]; then
  # TRUST was compiled in 32b, or in 64b new mode, force new 64b mode. Checking for conversion error is not possible as VisIt headers
  # are not clean.
  def_latatools=" -DLATATOOLS=1 -DINT_is_64_=2 "
else
  # TRUST is compiled in 64b, but with the old method:
  def_latatools=" -DLATATOOLS=1 -DINT_is_64_=1 "
fi

# -Wno-deprecated ne marche pas sous windows
cat > lata.xml <<EOF
<?xml version="1.0"?>
  <Plugin name="lata" type="database" label="lata import file" version="1.0" enabled="true" mdspecificcode="false" onlyengine="false" noengine="false" dbtype="MTMD" haswriter="false" hasoptions="false">
    <CXXFLAGS>
        ${def_latatools}
        ${med_cxx_flags} 
        ${extra_incl}
    </CXXFLAGS>
    <LIBS>
        ${med_libs}
    </LIBS>

EOF
      #-O2 -DNDEBUG -Wall 
if [ "$version_visit" = "1" ]; then
    cat >> lata.xml<<EOF
          <Extensions>
          lml
          lata
          21
          </Extensions>
EOF
else
    cat >> lata.xml<<EOF
       <FilePatterns>
          ${med_extension}
          *.lml
          *.lata
          *.21
          *FORT21*
        </FilePatterns>
EOF
fi

cat >> lata.xml<<EOF
    
    <Files components="M">

      avtlataFileFormat.C
EOF

echo $all_srcs >> lata.xml

cat >> lata.xml <<EOF
    </Files>
    <Files components="E">
      avtlataFileFormat.C
EOF

echo $all_srcs >> lata.xml

cat >> lata.xml <<EOF
    </Files>
    <Attribute name="" purpose="" persistent="true" exportAPI="" exportInclude="">
    </Attribute>
  </Plugin>
EOF
