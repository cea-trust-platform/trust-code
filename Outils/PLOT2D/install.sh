git clone is221559:/data/tmplgls/GIT_REPOS/PLOT2D.git PLOT2D_SRC
#git checkout  origin/abn/dynamic
sed "s/len(sys.argv)>=/len(sys.argv)> /" -i PLOT2D_SRC/src/python/test/dynamic/DynamicPlot.py
mkdir build
cd build
cmake ../PLOT2D_SRC/ -DSALOME_PLOT2D_STANDALONE=ON -DCMAKE_INSTALL_PREFIX=$TRUST_ROOT/exec/PLOT_2D
make 
make install
cd ..
cp Plot2d.sh $TRUST_ROOT/exec/PLOT_2D

