# Developper's documentation

This is the developper's documentation of `trustify`. If you are looking at how to use it you should turn to the root README file which is more user-oriented.

## Detailed explanations on the process

In `trustify` each TRUST keyword is mapped (via Python code generation) to :
- a generated `pydantic` Python class. Each of this class is generated in such a way that its (Python) members correspond to the attributes that the TRUST keyword expects.
- a generated *parsing* Python class (generated from scratch, no external dependency). This mirror class hierarchy mostly contains information relative to the parsing of a TRUST dataset (does the keyword use braces or commas, etc.)

For example, the keyword `Read_MED` becomes:
- the class `Read_med` (notice all lower case except first letter), and its members are `file` (of type `str`) , `mesh` (also of type `str`, etc.) correponding at the various attributes that the `Read_MED` keyword expects in an actual TRUST dataset.
- the class `Read_med_Parser` for the parsing part.

In more details:
- the C++ TRUST source code is instrumented with specific comments (XD tags) indicating for each TRUST keyword:
    - its type ;
    - a short description ;
    - and also all the attributes expected by the keyword (for example, reading a 'medium', one has to specify its thermic conductivity 'lambda').
- the module `trad2_utilities.py` parses all the C++ sources to gather all those tags. Those entries are appended to what is manually entered in `$TRUST_ROOT/Outils/TRIOXDATA/XTriou/TRAD_2.org` to produce a single file: `TRAD_2`
- the module `trad2_pydantic.py` is then used to generate (from this `TRAD_2`) two (big) Python modules usually called:
    - `trustify_generated_pyd.py`. This module contains one `pydantic` class per keyword, with the list of members of the class corresponding to the various attributes that the keyword expects. For example the class `Fluide_incompressible` representing the keyword `fluide_incompressible` has a `beta_th` attribute, of type `Field_base`, corresponding to the dilatability coefficient of the fluid. Note how each attribute is this file has a type annotation (from the standard `typing` Python module).
    - `trustify_generated_pars.py`. This second module has one parser class per keyword. It is used to parse and load in the data model part of a TRUST dataset. Each parser class holds information allowing the parsing process to work properly (e.g. : does the keyword expect curly braces '{' and '}', or for a list, should items be separated by commas, etc.)
- those two modules (along with `base.py`) can be used to programmatically (= with a script) load a TRUST dataset (a .data file) and modify it (see an example of this in the jupyter notebook under `doc/examples`). 
- the modified dataset can be written back with its modifications, the rest being preserved;


Note that the environment variable `TRUSTIFY_DEBUG=1` can be set to debug the tool itself (warning: this is very verbose!)


## TRAD_2 structure

The `TRAD_2` file is the central piece gathering the documentation for all keywords that can be used in a TRUST (or BALTIK) dataset. This file is built by concatenating the content of the manual entries found in `$TRUST_ROOT/Outils/TRIOXDATA/XTriou/TRAD_2.org` and what is extracted from the C++ source files (`// XD` tags in the C++ source files).

This section describes its format. 

The `TRAD_2` file is made up of several blocks. Each block has a header line, describing the keyword, and several lines (can be zero) describing the attributes that the keyword expects.

**NB:** note that this format existed before `trustify` was put into place. In the future, this file/format might be converted to a more standard format (JSON?).

See more details on how to write a proper TRAD_2 entry and/or an XD entry in the C++ code directly under `doc/README_user.md`.

### Header

The format of the header line is as follows:

```
class_name  parent_class_name  class_name_synonym  with_brace  description
```

The `with_brace` flag can take one of the following values:
- (-1) like the parent class
- (0) keyword does not expect braces when read in the dataset. Names of the attributes are not used for reading or writing.  Example : `Champ_Uniforme 3 0. 0. 0.`
- (1) keyword expects curly braces, the name of the attributes is explicitly provided. Example: `Lire sch { tinit 0. tmax 0. }`
- (-2) like 0 but wait after discretisation to write free part
- (-3) like 1 but wait after discretisation to write free part
    - example: all problems, discretisation must have been done before reading them.

NB: mode -2 and -3 are not used in `trustify`.

### Attributes

The format of an attribute line is as follows:

```
<space><space>attr  nam|nam_syno1  type  nam_syno2  opt  description
```
where:
- `opt` can take the value 0 (mandatory attribute) or 1 (optional attribute) indicating whether the attribute must be provided or is optional.
- `type` is the class name describing the type of the attribute.

When `attr` is followed by `ref type_name` the class must be associated with a class of type `type_name` (keyword `associer` in the dataset). This is not checked currently in `trustify` logic.


Some pecularities:
- `deriv` is not used anymore(?)

### Listobj

If the name of the parent class is `listobj`, syntax is

```
class_name  listobj  class_name_synonym  with_brace  classe_type  virgule  description
```

This expresses the creation of a list of objects of type `class_type`

The flag `virgule` (0|1) tells whether a comma is expected to separate list elements or not.


### Base types
The base types are:
- `entier` a single integer value
- `floattant`a single floatting value
- `chaine` either a single word (i.e. a string without any space) **or** (this is weird!!) a full block encompassed in curly braces. Example: `toto` is a chaine, and `{ how do you { do } }` is also a `chaine`, but `titi toto` is not. See specifically the class `trustify.base.Chaine_Parser` 
- `rien`: a void item - this type is usually used for flags, where just the presence/absence of the attribute suffices to indicate whether it is turned on or off
- `ref_toto` expects a name refering to an already created object in the dataset. In `trustify` this is just read as a chaine
- `list` (list of doubles: nval val1 val2 ...),  `listf` (liste of doubles with a number of elements equal to the dimension) ,
- `listentier`,  `listentierf`, same as before for `entier`
- `listchaine`, `listechainef`, same as before for `chaine`

The special attribute type `suppress_param` allows to delete the attribute that would otherwise be inherited from the parent class.

In the `description` field, the special tags `NL1`, `NL2`  can be used to explicitely add new lines.
- `NL1` single new line (ok for a tooltip, but not for Tex)
- `NL2` two newlines (ok for a tooltip and for Tex)

## Overview of the package's structure

The `trustify` package is made of several Python modules:
- `trad2_utilities.py` which holds the internal memory representation of the TRAD2 format. It also takes care of generating the `TRAD_2` file from the C++ source code and the (manually written) `TRAD_2.org`
- `trad2_pydantic.py` which generates the two Python modules (usually named something like `trustify_gen_pyd.py` and `trustify_gen.py`) :
    - one module for all the `pydandic` classes : this contains only the grammar of a TRUST dataset, with no information on the parsing. In this module we also add or override artificially a few classes (`Read`, `Declaration` and `Dataset`).
    - one module for all the parsing classes
- `base.py` which contains all the root parser classes
- `trust_parser.py` which contains the parsing logic (actually it is just the tokens extractions for what is done in `trustify` - this file was taken from a former process)
- `hacks.py` which contains various hacks that we want to keep clearly separated from the automatic generation process, and due to the workarounds that we have to do for some TRUST keywords which do not comply with the usual . This file is appended at the end of `trust_packagespy_generated_xxx.py` and contains highly specific stuff, that we have tried to properly document ...
- `misc_utilities.py` various utilities including logging, and most importantly the `ClassFactory` allowing to easily retrieve either a parser or pydantic class from a string.


### Dataset reading/parsing logic

This section describes how a TRUST dataset is parsed, eventually modified programmatically, and written back.

#### Overview

The class `trust_parser.TRUSTParser` provides a method `tokenize` which:
- opens the dataset
- slices its content (processing the comments, etc.)
- and produces a `TRUSTStream` object.

A `TRUSTStream` object is essentially made of two Python lists :
- one which stores the tokens (=the words) exactly as they were initially readen (preserving the case and the comments)
- one which is all lower-cased, with all comments erased. 

The second list will serve the actual interpretation of the dataset, while the first one will be used when writing back the dataset.

From there, the static methods `ReadFromTokens` of all the automatically generated classes takes as main input a `TRUSTStream` object. The static method acts as a factory to instanciate and fill the instance of the current class with the data found in the tokens. 

The `ReadFromTokens` at the top level invokes the same method on other classes: when an attribute of a keyword is parsed, its type is determined (more on this later), and the `ReadFromTokens` from this new type is invoked to build the instance of the attribute.

Along the way, part of the tokens of the stream are consumed (=used) and stored internally, and the stream is moved forward.

#### Type reading

At the dataset level (class `Dataset_Parser`) the class type must always be read, whether this is a forward declaration, or a direct instanciation of an `interprete`.

When parsing an attribute in a keyword, the type need not always to be read. The class (not instance) member `_read_type` indicates whether the keyword expect to read its type or not. **TODO: example of this**

As can be seen in `trad2_pydantic.py` (method `is_base_or_deriv()`), generally all the keywords/classes ending in `_base` expect to read their type. For example the keyword `fluide_incompressible` takes an attribute `rho` (volumic mass). The user can pass any field deriving from `field_base` (for example a `champ_uniforme`) as valid `rho` value.

In the dataset this looks like

```
  fluide_incompressible {
      rho champ_uniforme 1 0.28
      ...
  }
```

so when parsing the `rho` attribute, we parse also the type of the object read next (a `champ_front_uniforme`).

In some other cases this is not necessary, for example when we read a `bloc_lecture` or simple types like floats or strings.

#### Synonyms

Synonyms (for keywords, not for attributes) are a pain. The same synonym might be used by different keywords.
This is typically the case with `negligeable` which can be either a synonym for  `convection_negligeable` or for `diffusion_negligeable`, allowing one to write in a dataset:

```
read pb {
  convection { negligeable }
  # or #
  diffusion { negligeable }  
  ...
}
```

To make this work, when we find such a synonym, we give the precedence to the one matching the type expected by the `TRAD_2`. This is done in `ConstrainBase._ReadClassName()`.

So if we read `negligeable` when we expected a `convection_deriv`, we return the actual class name to be `convection_negligeable`.


The list of synonyms is built once upon loading by `trustify.misc_utilities.BuildSynonymMap` method.

#### Parsing forward declarations

In a dataset like this:

```
    dimension 2
    pb_conduction pb
    ...
    read pb { }
```

line #2 is called a **forward declaration**. Such declarations can be done if and only if the keyword does not derive from `interprete`. The `interprete` in TRUST are usually keywords triggering an immediate action in the code, rather than being used to fill data. A typical `interprete` is `solve` which triggers the resolution of the problem.

In `trustify` forward declarations are mapped to the specific (pydantic) class `Declaration` which is explicitely added to the generated code by the `trad2_pydantic.py` module.

### Dataset writing logic

The main entry point for the writing logic is the method `toDatasetTokens()`. This method produces a list of tokens (similar to what is produced the other way around when we parse the dataset). This list of strings can simply be concatenated to find back a complete dataset.


To write back a dataset from a Python instance, the following main actions are performed:
- token check: to make sure that the actual stored token still corresponds to the value registered in the Python module. If the value has changed, the saved token is invalidated, and a new token (not the original one) is produced from the internal value. See method `base.Abstract_Parser.checkToken()`
- calling the `toDatasetTokens()` method of each of the class attributes (in the correct order) to generate the stream of tokens for the attributes of the class.

### Error checking

Great care is taken in `trustify` to provide clear and comprehensive error messages. Notably whenever an error is triggered we try hard to provide:
- the source line in the dataset where the error happened ;
- but also, the source line in the original C++ code (or in the TRAD2.org file) where the corresponding keyword and/or attribute was documented. 


This greatly helps fixing any potential issue.

### Test structure

In the `test`subfolder:
- `test_rw_elementary.py` : testing read and write of small pieces of datasets. For those tests a dummy, reduced version of a TRAD2 is used: TRAD_2_adr_simple.
- `test_rw_full_datasets.py` : testing read and write of complete datasets using a typical (and complete) TRUST TRAD2 file (TRAD2_trustify).
- `test_rw_all_datasets.py` : for **all** TRUST datasets, load each of them in `trustify`, and write them back. If the output differs from the input, an error is raised.
