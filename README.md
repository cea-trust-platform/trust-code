
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

# **How to install TRUST-1.9.0 version ?**

### **First method**

    $> git clone https://github.com/cea-trust-platform/trust-code.git TRUST-1.9.0
    $> cd TRUST-1.9.0
    $> wget ftp://ftp.cea.fr/pub/TRUST/externalpackages/externalpackages-1.9.0.tar
    $> tar xf externalpackages-1.9.0.tar
    $> ./configure -help # To see the possible configure options.
    $> ./configure $OPTIONS  # At the end of configure, file env/machine.env will be created. You can then edit it and some others (env/make.linux*) to change some default values.
    $> make

**Warning:** Check carefully the MPI version you want to use for parallel computing before installing the code, especially onto a HPC cluster.

Run the following command, mpicxx should be found into the bin directory of the MPI version you plan to use:

    $> type mpicxx

### **Second method**

    $> wget ftp://ftp.cea.fr/pub/TRUST/TRUST/Version1.9.0/TRUST-1.9.0.tar.gz
    $> tar xzf TRUST-1.9.0.tar.gz
    $> cd TRUST-1.9.0
    $> ./configure $OPTIONS
    $> make

# **How to install TRUST development version  ?**
**for developers and those interested in new features only.**

**Warning: "next" branch may not compile or some tests fail if important developments merged**

    $> git clone https://github.com/cea-trust-platform/trust-code.git TRUST-next
    $> cd TRUST-next
    $> git checkout next
    $> wget ftp://ftp.cea.fr/pub/TRUST/externalpackages/externalpackages-next.tar
    $> tar xf externalpackages-next.tar
    $> ./configure $OPTIONS 
    $> make

# **How to start ?**

To initialize TRUST:

	$> source ./env_TRUST.sh

To build a binary with a single directory build:

	$> make       # Create optimized and debug binaries and tools
	$> make optim # Create an optimized binary
	$> make debug # Create a debug binary
	$> make prof  # Create a profiled binary

To check:

	$> make check

To clean (will delete all .o and .a of TRUST sources, inside directories or MonoDir_... directories)

	$> make clean

To clean also the third party libraries:

	$> ./configure -clean


# **Quick directories description**

> - **TRUST scripts** => bin/

> - **Documentation files** => doc/

> - **Environment files** => env_src/ & env/

> - **Binaries** => exec/

> - **Link to all TRUST include files (automatically updated)** => include/

> - **Libraries** => ThirdPart/ & lib/

> - **TRUST sources** => src/

> - **TRUST tests** => tests/

> - **Tools to build TRUST** => Outils/

> - **Location of Validation forms** => Validation/


# **Quick files description**

> - **README.md**			=> This file

> - **env_TRUST.sh**		=> Script loading TRUST environment

> - **env_for_python.sh**	=> TRUST python tools initialization script

> - **RELEASE_NOTES**		=> Release notes (manually and regulary updated by support)

> - **DEVELOPER_NOTES**		=> Developer notes (manually and regulary updated by support)

> - **Makefile**			=>	List of targets to build the code

> - **ToDo**				=> List of tasks to do

> - **configure**			=> configure script

> - **index.html**			=> Index description (gives access to documentation, tutorials, ...)

> - **license**				=> TRUST and third party licences

