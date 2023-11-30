![TRUST](bin/HTML/logo_trust.gif)

**TRUST** is a thermalhydraulic software package for **CFD** simulations. It was originally designed for **incompressible** single-phase and **Low Mach Number** flows, but now also allows simulating real **compressible multi-phase** flows. TRUST is also being progressively ported to support **GPU** acceleration (NVidia/AMD).

This software is **OpenSource ([BSD license](https://github.com/cea-trust-platform/trust-code/blob/master/License.txt))**. 

You can visit **[TRUST website](https://cea-trust-platform.github.io/)**

# **How to install TRUST-1.9.3 version ?**

### **First method (git configuration)**

```bash
# Check out the sources for TRUST
git clone https://github.com/cea-trust-platform/trust-code.git TRUST-1.9.3
# Move to the cloned TRUST folder
cd TRUST-1.9.3
# Download the required external packages
wget ftp://ftp.cea.fr/pub/TRUST/externalpackages/externalpackages-1.9.3.tar
# or:
# curl ftp://ftp.cea.fr/pub/TRUST/externalpackages/externalpackages-1.9.3.tar > externalpackages-1.9.3.tar
# Unzip the downloaded external packages folder
tar xf externalpackages-1.9.3.tar
# Have a look at the available configuration options
./configure -help
# Execute the configure script with your selected options
# At the end of configure, file env/machine.env will be created. 
# You can then edit it and some others (env/make.linux*) to change some default values.
./configure $OPTIONS
```

**Warning:** Check carefully the MPI version you want to use for parallel computing before installing the code, especially onto a HPC cluster.

Run the following command, mpicxx should be found into the bin directory of the MPI version you plan to use

```bash
type mpicxx
```

### **Second method (download a tar.gz)**

```bash
# Download the sources for TRUST
wget ftp://ftp.cea.fr/pub/TRUST/TRUST/Version1.9.3/TRUST-1.9.3.tar.gz
# Unzip the downloaded folder
tar xzf TRUST-1.9.3.tar.gz
# Move to the unziped TRUST folder
cd TRUST-1.9.3
# Execute the configure script with your selected options
./configure $OPTIONS
```

# **How to install TRUST development version ?**
**For developers and those interested in new features only.**

**Warning: "next" branch may not compile or some tests fail if important developments merged...**

```bash
# Check out the sources for TRUST
git clone https://github.com/cea-trust-platform/trust-code.git TRUST-next
# Move to the cloned TRUST folder
cd TRUST-next
# Checkout the developement TRUST branch
git checkout next
# Download the requested external packages
wget ftp://ftp.cea.fr/pub/TRUST/externalpackages/externalpackages-next.tar
# or:
# curl ftp://ftp.cea.fr/pub/TRUST/externalpackages/externalpackages-next.tar > externalpackages-next.tar
# Unzip the downloaded packages
tar xf externalpackages-next.tar
# Execute the configure script with your selected options
./configure $OPTIONS 
```

# **How to start ?**

- **Initializing TRUST environment**

```bash
# To initialize TRUST environment
source ./env_TRUST.sh

# To initialize the TRUST python's environment (MEDCoupling, swig and ICoCo python) (TRUST should be built)
source ./env_for_python.sh
```

- **Building TRUST sources / libraries / tools**

```bash
# To build TRUST as a user application (optimized mode only)
make optim

# To build TRUST as a developer (optimized + debug modes)
make
```

- **Useful TRUST env variables**

```bash
# To go to the TRUST root directory from any location (supposing that TRUST is initialized)
cd $TRUST_ROOT

# To go to the TRUST tests directory from any location (supposing that TRUST is initialized)
cd $TRUST_TESTS
```

- **Testing TRUST**

```bash
# To check TRUST in a parallel way (more than one test case at a time)
make ctest_optim

# To check TRUST in a sequential way (one test case at a time)
make check
```

- **Cleaning TRUST**

```bash
# To clean (will delete all .o and .a of TRUST sources, inside directories or MonoDir_... directories)
make clean

#To clean TRUST's third party libraries
./configure -clean
```

- **TRUST documentation**

```bash
# To see TRUST's documentation
trust -index
```

You are also invited to visit the TRUST **Doxygen** documentation which is available **[here](https://cea-trust-platform.github.io/TRUST_Doxygen.github.io/html/index.html)**.

# **Quick directories description**

- **TRUST scripts** => bin/

- **Documentation files** => doc/

- **Environment files** => env_src/ & env/

- **Binaries** => exec/

- **Link to all TRUST include files (automatically updated)** => include/

- **Libraries** => ThirdPart/ & lib/

- **TRUST sources** => src/

- **TRUST tests** => tests/

- **Tools to build TRUST** => Outils/

- **Location of Validation forms** => Validation/


# **Quick files description**

- **README.md**			=> This file

- **env\_TRUST.sh**		=> Script loading TRUST environment

- **env\_for\_python.sh**	=> TRUST python tools initialization script

- **RELEASE_NOTES**		=> Release notes (manually and regulary updated by support)

- **DEVELOPER_NOTES**		=> Developer notes (manually and regulary updated by support)

- **Makefile**			=>	List of targets to build the code

- **ToDo**				=> List of tasks to do

- **configure**			=> configure script

- **index.html**			=> Index description (gives access to documentation, tutorials, ...)

- **license**				=> TRUST and third party licences


# **Thermo-Physical Properties interface**

TRUST offers a generic interface that can be used to calculate thermo-physical properties (useful for multi-phase flows). For that, two external libraries can be used:
	
- **CoolProp** (open source [http://www.coolprop.org/](http://www.coolprop.org/))

- **EOS** (only for CEA agents and requires permission)


### CoolProp

CoolProp is an open source ([MIT License](https://github.com/CoolProp/CoolProp/blob/master/LICENSE)) library useful to compute thermo-physical properties for the masses.

A pre-compiled static library of CoolProp is mandatory if a TRUST user wants to use its interface via TRUST. Here are the steps that can be followed to obtain a libcoolprop.a

```bash
# Get the sources archive (version 6.5.0)
wget https://sourceforge.net/projects/coolprop/files/CoolProp/6.5.0/source/CoolProp_sources.zip
unzip CoolProp_sources.zip
mv source CoolProp
# or download CoolProp (attention recursive, beta version that may have issues with TRUST !!!)
git clone https://github.com/CoolProp/CoolProp --recursive
# Move into the folder you just created
cd CoolProp
# Make a build folder
mkdir -p build && cd build
# Build the makefile using CMake
cmake .. -DCOOLPROP_STATIC_LIBRARY=ON -DCOOLPROP_FPIC=ON
# Make the static library
cmake --build .
# Instead, you can do : make -j all ...
```

Once done, **you need to configure TRUST** with the following (maybe additional) option

```bash
./configure $OPTIONS -with-coolprop=YOUR_PATH_COOLPROP
make
```
This configure with make it also possible to access CoolProp via a python module. You can try it

```bash
source env_TRUST.sh
python
from CoolProp.CoolProp import PropsSI
```
**ATTENTION:** `YOUR_PATH_COOLPROP` is the path towards the cloned CoolProp repository and not the build. It is recommended to verify first that the folder `include` exists in `YOUR_PATH_COOLPROP` and that the library `libCoolProp.a` exists in `YOUR_PATH_COOLPROP/build`.

**Note:** It is possible to call REFPROP via the TRUST CoolProp interface. However, this requires a [license](https://www.nist.gov/srd/refprop) and a well built shared library of REFPROP. In this case, **you need to configure TRUST** with the following (maybe additional) option

```bash
./configure $OPTIONS -with-refprop=YOUR_PATH_REFPROP
make
```
Here, `YOUR_PATH_REFPROP` is the path towards the shared library of REFPROP.
