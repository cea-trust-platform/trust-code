# trustify

## Presentation

`trustify` is a python package based on the Python library `pydantic`. Its main purpose is to generate, from the TRUST C++ source code (or one of the TRUST-based project - like GENEPI/PPGP), a Python module describing the data model of a TRUST dataset.


Each TRUST keyword is mapped to a Python class with the same name, and whose attributes are the attributes of the TRUST keyword itself. For example `read_med` becomes a Python class with attributes `fichier`, `mesh` etc. This class can then be used in a Python script or another application.


Thus, the generated Python modules provide:
- all the scripting tools to load, manipulate/modify, and write back a TRUST dataset (i.e. a `.data` file).
- the basis for the automatic generation of Graphical User Interfaces compliant with this data model.


If you seek more information on the syntax to be used inside the C++ source code to document a keyword take a look at doc/README_user.md


If you need to develop or fix a bug in `trustify` itself, take a look at `doc/README_dev.md`


## Quick start / hello world

From a user perspective, the tool can be used to check that a dataset complies with the TRUST grammar using the command:

```bash
trust -trustify /path/to/toto.data
```

or

```bash
trust -trustify titi
```

if `titi` is a registered test case.


Next, the best thing to start using the tool programmatically is to look at the example Jupyter-Notebook provided in the folder `doc/examples/hello_world`.
To have this notebook working properly you need to source the relevant environment first:

```bash
source $TRUST_ROOT/env_for_python.sh
```

or more specifically:

```bash
source $TRUST_ROOT/Outils/trustify/env.sh
```

(the former includes the latter). And then launch Jupyter:

```bash
jupyter-notebook
```


## Launch python unit tests

The package comes with a set of Python unit-tests. You need to have the environment properly loaded for them to work (see above)
Tests are located in the `test` subfolder and are named with the pattern `test_*.py`.

Once the package has been properly installed with

```bash
make install
```


they can be launched with


```bash
make test
```

Look at the makefile to see the exact Python `unittest` command used to launch the tests.

Note that the environment variable `TRUSTIFY_DEBUG=1` can be set to debug the tool itself (warning: this is very verbose!)


## Miscellaneous
### More information about `pydantic`

The data model of any TRUST dataset (.data file) is turned into a Python module using the [`pydantic`](https://docs.pydantic.dev/latest) Python package. 
More info on can be found here:

[https://docs.pydantic.dev/latest](https://docs.pydantic.dev/latest)

### TRUST daily checks

In the daily checks of TRUST itself, this process is used to check that the documentation for each keyword is coherent with what is found in all the test cases:
- the `trustify` Python modules are generated from the C++ sources
- all the TRUST test datasets are parsed, loaded inside `trustify` and written back.

If the output differs from the original input, it means that something is wrong either with the dataset itself (but this is very unlikely, since this dataset runs in the NR tests daily), or that something is wrong with the documentation of the keyword in the code itself. The latter is what we really intend to check here.


### Known limitations / bugs
Dataset with repeated keywords are most of the time perfectly fine for TRUST, but not for `trustify`. The check process wich reads and writes back a dataset  expects only a single instance of a given keyword, or of a given option in any part of the dataset.

So for example repeating `dt_max 32` twice in the time scheme block will be OK for the reading part (only the last instance will be kept); but when writing back the dataset, only this single instance of the keyword will be written. This is hard to work around, since I don't want the whole `trustify` logic to be able to deal with multiple instances of the same option/keyword.

Work in progress to see if this behaviour should be forbidden in TRUST too.

