#!/bin/bash
DEST=$TRUST_MEDCOUPLING_ROOT
dest=$DEST/include/ICoCoMEDField.hxx
cp -af $dest .
make clean

if  [ "$TRUST_DISABLE_MED" = "1" ] || [ "$TRUST_DISABLE_MEDCOUPLING" = "1" ] 
then
    mkdir -p $DEST/include
    rm -rf $DEST/lib
    echo "MED or MEDCOUPLING DISABLE"
    echo "#define NO_MEDFIELD " > prov.h

    if [ "`diff ICoCoMEDField.hxx prov.h 2>&1`" != "" ]
	then
	cp prov.h $dest
    else
	cp -a ICoCoMEDField.hxx $dest
    fi
    rm -f prov.h ICoCoMEDField.hxx
    exit 0
fi
tar zxf MED_SRC.tgz


#echo patching MEDCouplingDataArrayTypemaps.i
#sed "s/NPY_ARRAY_OWNDATA/NPY_OWNDATA/g" -i MED_SRC/src/MEDCoupling_Swig/MEDCouplingDataArrayTypemaps.i

#echo patching MPIAcces.h
#sed -i  "s/return (MPI_Datatype ) NULL /return MPI_DATATYPE_NULL /"    MED_SRC/src/ParaMEDMEM/MPIAccess.hxx

#echo patching MPIProcessorGroup.cxx
#sed -i "s?_comm_interface.commCreate(_world_comm, _group, &_comm);?_comm_interface.commCreate(_world_comm, _group, \&_comm);MPI_Group_free(\&group_world);?"  MED_SRC/src/ParaMEDMEM/MPIProcessorGroup.cxx 
mkdir build
cd build

#export HDF5_ROOT=$TRUST_MED_ROOT
#export MED3HOME=$TRUST_MED_ROOT


USE_MPI=ON
[ "$TRUST_DISABLE_MPI" -eq 1 ] && USE_MPI=OFF
MED_COUPLING_PYTHON="ON"
OPTIONS="-DCMAKE_BUILD_TYPE=Release -DSALOME_USE_MPI=$USE_MPI -DCMAKE_INSTALL_PREFIX=$DEST -DCMAKE_CXX_COMPILER=$TRUST_CC -DSALOME_MED_STANDALONE=ON  -DHDF5_ROOT_DIR=$TRUST_MED_ROOT  -DMEDFILE_ROOT_DIR=$TRUST_MED_ROOT -DSALOME_BUILD_DOC=OFF  -DSALOME_MED_PARTITIONER_METIS=OFF -DSALOME_MED_PARTITIONER_SCOTCH=OFF -DSALOME_MED_ENABLE_RENUMBER=OFF -DSALOME_MED_ENABLE_PARTITIONER=OFF -DSALOME_BUILD_TESTS=OFF -DSALOME_MED_WITH_FILE_EXAMPLES=OFF"
 
cmake ../MED_SRC/ $OPTIONS

status=$?
if [ $status -ne 0 ]
    then
    echo "we tried to compile without python"
    cmake ../MED_SRC/ $OPTIONS -DSALOME_MED_ENABLE_PYTHON=OFF
    MED_COUPLING_PYTHON="OFF"
fi


make -j $TRUST_NB_PROCS
# si make install fonctionne coorectement ce fichier sera ecrase
# echo "#define NO_MEDFIELD " > $DEST/include/ICoCoMEDField.hxx
make install
status=$?
if [ $status -ne 0 ]
    then
    echo "we tried to compile without python"
    cmake ../MED_SRC/ $OPTIONS -DSALOME_MED_ENABLE_PYTHON=OFF
    MED_COUPLING_PYTHON="OFF"
    make install
fi
status=$?

ar cru $DEST/lib/libParaMEDMEM.a  `find src -name '*'.o`
if [ $status -eq 0 ]
then
    cd ..
    rm -rf build MED_SRC
fi

cd $DEST

version=`python  -c "import sys; print sys.version[:3]"`
echo "export MED_COUPLING_ROOT=$PWD"> env.sh
echo "export LD_LIBRARY_PATH=$PWD/lib/salome:$TRUST_MED_ROOT/lib:\${LD_LIBRARY_PATH}" >> env.sh
echo "export PYTHONPATH=$PWD/bin/salome:$PWD/lib/python$version/site-packages/salome:\$PYTHONPATH" >> env.sh
echo "export MED_COUPLING_PYTHON=$MED_COUPLING_PYTHON" >> env.sh

touch include/salome/*
for dir in include lib
do
  mkdir -p $dir
  cd $dir
  ln -sf salome/* .
  cd -
done
[ ! -f $DEST/include/ICoCoMEDField.hxx ] && echo "#define NO_MEDFIELD " > $DEST/include/ICoCoMEDField.hxx

exit $status
