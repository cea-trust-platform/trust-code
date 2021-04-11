
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

> - **FAQ**	=>Faq (obsolete?)

> - **Makefile** =>	List of targets to build the code

> - **README** => This file

> - **RELEASE_NOTES** => Release notes (manually and regulary updated by support)

> - **tags** => Tags (automatically updated)

> - **TAGS** => Tags (automatically updated)

> - **ToDo** => List of tasks to do

> - **configure** => configure script

> - **index.html** => Index description

> - **license** => TRUST and third party licences

