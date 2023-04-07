
```sh
   _________  _______             _______  _________   
   \__   __/ (  ____ ) |\     /| (  ____ \ \__   __/   
      ) (    | (    )| | )   ( | | (    \/    ) (      
      | |    | (____)| | |   | | | (_____     | |      
      | |    |     __) | |   | | (_____  )    | |      
      | |    | (\ (    | |   | |       ) |    | |      
      | |    | ) \ \__ | (___) | /\____) |    | |      
      )_(    |/   \__/ (_______) \_______)    )_(      


BSD3 license

```

# **How to install TRUST-1.9.1 version ?**

### **First method**

```bash
# Check out the sources for TRUST
git clone https://github.com/cea-trust-platform/trust-code.git TRUST-1.9.1
# Move to the cloned TRUST folder
cd TRUST-1.9.1
# Download the requested external packages
wget ftp://ftp.cea.fr/pub/TRUST/externalpackages/externalpackages-1.9.1.tar
# Unzip the downloaded packages
tar xf externalpackages-1.9.1.tar
# Have a look at the available configuration options
./configure -help
# Execute the configure script with your selected options
# At the end of configure, file env/machine.env will be created. You can then edit it and some others (env/make.linux*) to change some default values.
./configure $OPTIONS
# Finally, compile the TRUST libraries/sources/tools
make
```

**Warning:** Check carefully the MPI version you want to use for parallel computing before installing the code, especially onto a HPC cluster.

Run the following command, mpicxx should be found into the bin directory of the MPI version you plan to use:

```bash
type mpicxx
```

### **Second method**

```bash
# Download the sources for TRUST
wget ftp://ftp.cea.fr/pub/TRUST/TRUST/Version1.9.1/TRUST-1.9.1.tar.gz
# Unzip the downloaded folder
tar xzf TRUST-1.9.1.tar.gz
# Move to the unziped TRUST folder
cd TRUST-1.9.1
# Execute the configure script with your selected options
./configure $OPTIONS
# Finally, compile the TRUST libraries/sources/tools
make
```

# **How to install TRUST development version ?**
**for developers and those interested in new features only.**

**Warning: "next" branch may not compile or some tests fail if important developments merged**

```bash
# Check out the sources for TRUST
git clone https://github.com/cea-trust-platform/trust-code.git TRUST-next
# Move to the cloned TRUST folder
cd TRUST-next
# Checkout the developement TRUST version
git checkout next
# Download the requested external packages
wget ftp://ftp.cea.fr/pub/TRUST/externalpackages/externalpackages-next.tar
# Unzip the downloaded packages
tar xf externalpackages-next.tar
# Execute the configure script with your selected options
./configure $OPTIONS 
# Finally, compile the TRUST libraries/sources/tools
make
```

# **How to start ?**

To initialize TRUST:

```bash
source ./env_TRUST.sh
```

To build a binary with a single directory build:

```bash
make       # Create optimized and debug binaries and tools
make optim # Create an optimized binary
make debug # Create a debug binary
make prof  # Create a profiled binary
```

To check:

```bash
make check
```

To clean (will delete all .o and .a of TRUST sources, inside directories or MonoDir_... directories)

```bash
make clean
```

To clean also the third party libraries:

```bash
./configure -clean
```

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
# Check out the sources for CoolProp
git clone https://github.com/CoolProp/CoolProp --recursive
# Move into the folder you just created
cd CoolProp
# Make a build folder
mkdir -p build && cd build
# Build the makefile using CMake
cmake .. -DCOOLPROP_STATIC_LIBRARY=ON
# Make the static library
cmake --build .
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

**Note:** It is possible to call REFPROP via the TRUST CoolProp interface. However, this requires a [license](https://www.nist.gov/srd/refprop) and a well built shared library of REFPROP. In this case, **you need to configure TRUST** with the following (maybe additional) option

```bash
./configure $OPTIONS -with-refprop=YOUR_PATH_REFPROP
make
```

