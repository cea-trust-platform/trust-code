
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

# **How to start ?**

To detect the environment, run:

	$> ./configure

A file env/machine.env will be created. You can then edit this file and some others (env/make.linux*) to change some default values.

To initialize:

	$> source ./env_TRUST.sh

To build a binary with a single directory build:

	$> make # Create an optimized debug binaries
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

> - **DEVELOPER_NOTES**	=> Developer notes (manually and regulary updated by support)

> - **Makefile** =>	List of targets to build the code

> - **README.md** => This file

> - **RELEASE_NOTES** => Release notes (manually and regulary updated by support)

> - **tags** => Tags (automatically updated)

> - **TAGS** => Tags (automatically updated)

> - **ToDo** => List of tasks to do

> - **configure** => configure script

> - **index.html** => Index description

> - **license** => TRUST and third party licences

# **How to run a TRUST preinstalled version**

If you are a CEA worker, it is possible to use a TMA preinstalled version of TRUST. Here is the list of the machines and the paths that can be used to source the TRUST environment

- for a local machine at CEA Saclay

      $> source /home/triou/env_TRUST_1.8.3.sh

      or

      $> source /home/trust_trio-public/env_TRUST-1.8.3.sh

- for CEA Saclay cluster (orcus):

      $> source /home/trust_trio/env_TRUST-1.8.3.sh

- for CEA Marcoule cluster (ceres2):

      $> source /softs/trio_u/env_TRUST-1.8.3.sh

- for CCRT-TGCC supercomputers (cobalt & irene-ccrt):

      $> source /ccc/cont002/home/den/triou/env_TRUST-1.8.3.sh

- for CCRT-TGCC supercomputers (irene-amd int64):

      $> source /ccc/work/cont002/den/triou/irene-ccrt/TRUST/TRUST-1.8.1-int64/env_TRUST.sh

- for CINES supercomputer (occigen):

      $> source /panfs/panasas/softs/applications/trio_u/TRUST/env_TRUST-1.8.3.sh
