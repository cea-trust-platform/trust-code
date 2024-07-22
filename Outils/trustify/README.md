# trustify

## Presentation

`trustify` is a python package based on `pydantic`. Its main purpose is to generate, from the TRUST C++ source code (or one of the TRUST-based project - like GENEPI/PPGP), a  Python module describing the data model of a TRUST dataset.
This generated Python module then provides:
- all the scripting tools (in Python) to load, manipulate/modify, and write back a TRUST dataset (.data file).
- the base information for automatic generation of Graphical User Interfaces compliant with this data model.


## Quick start / hello world

The best thing to start is to look at the example Jupyter-Notebook provided in the folder examples/hello_world:

```bash
source $TRUST_ROOT/env_for_python.sh
cd $TRUST_ROOT/

```


You can also go to [Launch Python unit-tests](#py-unittest) if you want to see it in action quickly.

If you want to know how to properly document your C++ code, checkout the `README_user.md` file in the `doc` subfolder.

If you want more detailed explanations on the process, check out [Detailed explanations](#details)

More specific documentation (notably developper's doc) can be found in the `doc` subfolder. 


## Launch python unit tests

The package comes with a set of Python unit-tests. You need to have the environment properly loaded for them to work.
Tests are located in the `test` subfolder and are named with the pattern `test_*.py`.

### Individual test launch
Tests can be launched individually :

```bash
cd ${trustify_ROOT_DIR}/trustify/test

# launch one file python unittest
./test_200_classmethod.py
./test_500_Triou_All.py
./test_504_read_write_data.py
```

### Launch some/all tests

You can use 

```bash
make test
```

or 

```bash
make test_debug
```

To launch a single test, you can use:

```bash
python -m unittest test.test_rw_elementary.TestCase.test_builtin
```

## Miscellaneous
### More information about `pydantic`

The data model of any TRUST dataset (.data file) is turned into a Python module using the [`pydantic`](https://docs.pydantic.dev/latest) Python package. 
More info on can be found here:

[https://docs.pydantic.dev/latest](https://docs.pydantic.dev/latest)


<a id="details"></a>

### Known limitations / bugs
Dataset with repeated keywords are most of the time perfectly fine for TRUST, but not for `trustify`. The check process wich reads and writes back a dataset notably expects only a single instance of a given keyword, or of a given option in any part of the dataset.

So for example repeating `dt_max 32` twice in the time scheme block will be OK for the reading part (only the last instance will be kept); but when writing back the dataset, only this single instance of the keyword will be written. This is hard to work around, since I don't want the whole `trustify`logic to be able to deal with multiple instances of the same option/keyword.

Work in progress to see if this behaviour should be forbidden in TRUST too.

### Detailed explanations on the process

In `trustify` each TRUST keyword is mapped to a generated `pydantic` Python class. Each of this class is generated in such a way that its (Python) members correspond to the attributes that the TRUST keyword expects.

For example, the keyword `Read_MED` becomes the class `Read_med` (notice all lower case except first letter), and its members are `file` (of type `str`) , `mesh` (also of type `str`, etc.) correponding at the various attributes that the `Read_MED` keyword expects in an actual TRUST dataset.

In more details:
- the C++ TRUST source code is instrumented with specific comments (XD tags) indicating for each TRUST keyword:
    - its type ;
    - a short description ;
    - and also all the attributes expected by the keyword (for example, reading a 'medium', one has to specify its thermic conductivity 'lambda').
- the module `trad2_utilities.py` parses all the C++ sources to gather all those tags. Those entries are appended to what is manually entered in `$TRUST_ROOT/Outils/TRIOXDATA/XTriou/TRAD_2.org` to produce a single file: `TRAD_2`
- the module `trad2_pydantic.py` is then used to generate (from this `TRAD_2`) two (big) Python modules usually called:
    - `trustify_generated_pyd.py`. This module contains one `pydantic` class per keyword, with the list of members of the class corresponding to the various attributes that the keyword expects. For example the class `Fluide_incompressible` representing the keyword `fluide_incompressible` has a `beta_th` attribute, of type `Field_base`, corresponding to the dilatability coefficient of the fluid. Note how each attribute is this file has a type annotation (from the standard `typing` Python module).
    - `trustify_generated_pars.py`. This second module has one parser class per keyword. Each parser class holds information allowing the parsing process to work properly (e.g. : does the keyword expect curly braces '{' and '}', or for a list, should items be separated by commas, etc.)
- those two modules (along with `base.py`) can be used to programmatically (= with a script) load a TRUST dataset (a .data file) and modify it (see an example in `test_rw_full_datasets.py` test case `test_ds_upwind`). The dataset can be modified with standard Python instructions like 
```python

```
- the modified dataset can be written back with its modifications, the rest being preserved;

