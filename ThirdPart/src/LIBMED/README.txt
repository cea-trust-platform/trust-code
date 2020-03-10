Installation procedure of MED file and HDF5
===========================================

HDF5 is a prerequisite of MED file.

For both packages, user can choose to use an external installation if the
corresponding env variable is set:

* TRUST_USE_EXTERNAL_HDF: if this is non empty and points to a valid HDF5
installation, this directory will be used instead of recompiling the 
default HDF5 provided with TRUST
* TRUST_USE_EXTERNAL_MED: same as above for MED file.

The compilation is performed in 
  build/lib/hdf5_build
  build/lib/med_build
  
The final installation is done in 
  lib/src/LIBMED
which contains links to:
  * the external installation if one was specified
  * to lib/src/LIBMED/_install if the TRUST version was compiled.


The two scripts dealing with the installation are
  * install_hdf5.sh
  * install_med.sh
which are very similar.
