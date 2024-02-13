# TRUSTPY

## Presentation

`trustpy` is a python package based on `packagespy`. Its main purpose is to generate, from the TRUST C++ source code (or one of the TRUST-based project - like GENEPI/PPGP), a `packagespy` Python module.
This generated Python module then provides:
- a Model-View-Controller graphical user interface (GUI)
- all the scripting tools (in Python) to load, manipulate/modify, and write back a TRUST dataset (.data file).

## Quick start / hello world

The best thing to start is to look at the example Jupyter-Notebook provided in the folder examples/hello_world:

```bash
source $TRUST_ROOT/env_for_python.sh
cd $TRUST_ROOT/

```


You can also go to [Launch Python unit-tests](#py-unittest) if you want to see it in action quickly.

If you want to know how to properly document your code, checkout the `README_user.md` file in the `doc` subfolder.

If you want more detailed explanations on the process, check out [Detailed explanations](#details)

More specific documentation (notably developper's doc) can be found in the `doc` subfolder. The file `TODO.md` lists what remains to be corrected/done.


## Launch python unit tests

The package comes with a set of Python unit-tests. You need to have the environment properly loaded for them to work.
Tests are located in `trustpy/tests` subfolder and are named with the pattern `test_???_*.py`.

### Individual test launch
Tests can be launched individually :

```bash
cd ${TRUSTPY_ROOT_DIR}/trustpy/test

# launch one file python unittest
./test_200_classmethod.py
./test_500_Triou_All.py
./test_504_read_write_data.py
```

### Launch some/all tests

Tests can also be launched all together using the tool provided in `packagespy`:

`${PACKAGESPY_ROOT_DIR}/packagespy/unittestpy/AllTestLauncher.py`

```
cd .../trustpy/trustpy
${PACKAGESPY_ROOT_DIR}/packagespy/unittestpy/AllTestLauncher.py --rootPath .
```

- use `AllTestLauncher.py` from PACKAGESPY
- as `${PACKAGESPY_ROOT_DIR}/packagespy/unittestpy/AllTestLauncher.py`

```
# help AllTestLauncher.py
cd .../trustpy/trustpy
${PACKAGESPY_ROOT_DIR}/packagespy/unittestpy/AllTestLauncher.py --help
  usage: AllTestLauncher.py [-h] [-d] [-v int] [-r dirPath] [-p filePattern] [-t outputType] [-n dirName]
  
  launch All python tests
  
  optional arguments:
    -h, --help            show this help message and exit
    -d, --debug           set debug mode, more verbose
    -v int, --verbosity int
                          set verbosity of unittests [0|1|2...]
    -r dirPath, --rootPath dirPath
                          dir name with absolute or relative path stand for root directory of recursive searching unittest python files
    -p filePattern, --pattern filePattern
                          file pattern for unittest files ['test_*.py'|'*Test.py'...]
    -t outputType, --type outputType
                          type of output: ['std'(standart ascii)|'xml'|'html']
    -n dirName, --name dirName
                          (only for type xml) name of directory output: ['test_reports'|...]. If name = 'stdout' then all-in-one xml output at 'sys.stdout'. For pipe
                          redirection: '>> AllTestLauncher.py -t xml -n stdout > tmp.xml'
```

- launch some unittest files

```
# an example
cd .../trustpy/trustpy
${PACKAGESPY_ROOT_DIR}/packagespy/unittestpy/AllTestLauncher.py --pattern "test_50?_*.py" --rootPath .
# and shorter argument names
${PACKAGESPY_ROOT_DIR}/packagespy/unittestpy/AllTestLauncher.py -p "test_50?_*.py" -r .
```

- launch all unittest files 

```
# an example
# example all unittest file included from one directory WhereYouWant
${PACKAGESPY_ROOT_DIR}/packagespy/unittestpy/AllTestLauncher.py -r WhereYouWant
```


## Miscellaneous
### More information about `packagespy`

In short `packagespy` is a Python toolbox allowing you to enforce type checking and automatic GUI generation for a given datamodel. Take a look at:
- the official documentation `${PACKAGESPY_ROOT_DIR}/doc/PACKAGESPY.pdf`
- try Jupyter notebooks `${PACKAGESPY_ROOT_DIR}/doc/PACKAGESPY_howto*.ipynb`

<a id="details"></a>

### Known limitations / bugs
Dataset with repeated keywords are most of the time perfectly fine for TRUST, but not for `trustpy`. The check process wich reads and writes back a dataset notably expects only a single instance of a given keyword, or of a given option in any part of the dataset.

So for example repeating `dt_max 32` twice in the time scheme block will be OK for the reading part (only the last instance will be kept); but when writing back the dataset, only this single instance of the keyword will be written. This is hard to work around, since I don't want the whole `trustpy`logic to be able to deal with multiple instances of the same option/keyword.

Work in progress to see if this behaviour should be forbidden in TRUST too.

### Detailed explanations on the process

In `trustpy` each TRUST keyword is mapped to a Python class. Each of this class is generated in such a way that its (Python) members correspond to the attributes that the TRUST keyword expects.

For example, the keyword `Read_MED` becomes the class `read_med_tru` (the `_Tru` suffix stands for TRUST), and its members are `file` (of type `chaine_Tru`) , `mesh` (also of type `chaine_Tru`, etc.) correponding at the various attributes that the `Read_MED` keyword expects in an actual TRUST dataset.

In more details:
- the C++ TRUST source code is instrumented with specific comments (XD tags) indicating for each TRUST keyword:
    - its type
    - a short description
    - and also all the attributes expected by the keyword (for example, reading a 'medium', one has to specify its thermic conductivity 'lambda')
- the module `trad2_utilities.py` parses all the C++ sources to gather all those tags. Those entries are appended to what is manually entered in `$TRUST_ROOT/Outils/TRIOXDATA/XTriou/TRAD_2.org` to produce a single file: `TRAD_2`
- the module `trad2_code_gen.py` is then used to generate (from this `TRAD_2`) a (big) Python module usually called `trust_packagespy_generated_xxx.py`. This module contains one Python class per keyword, with the list of members of the class corresponding to the various attributes that the keyword expects. For example the class `fluide_incompressible_Tru` representing the keyword `fluide_incompressible` has a `beta_th` attribute, of type `field_base_Tru`, corresponding to the dilatability coefficient of the fluid.
- this module (along with `baseTru.py`) can be used to programmatically (= with a script) to load a TRUST dataset (a .data file) and modify it (see an example in `test_700_rw_full_datasets.py` test case `test_200`). The following features are then provided:
    - model-data dynamic *tree structure*
    - automatic casting/type-checking on data modifications
- the modified dataset can be written back with its modifications, the rest being preserved;
- the dataset can also be displayed/edited/saved back in an automatically generated graphical user interface (see the same `test_200` for an example).
