""" From a TRAD2 file, generates:
 - the Pydantic schema (as a generated Python file containing one Pydantic class per TRUST keyword) 
 - the parsing classes (as a generated Python file, containing one Python class per TRUST keyword)
 
Authors: G Sutra, A Bruneton
"""

import pathlib
import textwrap
import datetime
import trustify.trad2_utilities as tu
from trustify.misc_utilities import logger, ClassFactory, TrustifyException

################################################################

def valid_variable_name(s):
    """ Make a valid variable name from any str. Useful to avoid variable name colinding with Python 
    keywords ('lambda' for example ...) """
    import re
    import keyword

    # replace invalid characters with underscores
    s = re.sub(r'[^0-9a-zA-Z_]', '_', s)
    # prefix leading number with 'i'
    s = re.sub(r'^([0-9])', r'i\1', s)
    # suffix reserved keywords with '_'
    if keyword.iskeyword(s):
        s += '_'
    return s

def format_docstring(description):
    docstring = f'r"""\n{description}\n"""'
    docstring = docstring.replace("NL1", "\n")
    docstring = docstring.replace("NL2", "\n\n")
    docstring = docstring.splitlines()
    docstring = [textwrap.wrap(line, width=90) if line else [""] for line in docstring]
    docstring = sum(docstring, [])
    docstring = [f'    {line.lstrip()}' for line in docstring]
    return docstring

def is_base_or_deriv(cls_nam):
    """ The TRUST keywords ending with _base or _deriv usually needs to have their type explicitely read in the dataset """
    return cls_nam.endswith("_deriv") or cls_nam.endswith("_base") or cls_nam == "class_generic"

def generate_attribute_synos(block, all_blocks):
    """ Generate a dictionary containing:
    - under the 'None' key : the list of synonyms for the keyword itself ;
    - and for **all** the attributes of a class (even the inherited ones) their synonyms.
    Important: the keys (i.e. the attribute names) are ordered in a specific logic:
        - attribute of the child class first
        - and then inherited attributes from the mother class ...
    """
    import sys
    assert sys.hexversion >= 0x3060000, "Need Python > 3.6 to guarantee key ordering in dictionary!"

    if not block.attr_synos is None:
        return block.attr_synos
    # Get parent class
    base_syn = {}
    if block.name_base in all_blocks:
        base_syn = generate_attribute_synos(all_blocks[block.name_base], all_blocks)
    # Build final return value
    # 1. First the synos of the keyword itself:
    ret = {None: [s for s in block.synos if s != block.name]}
    # 2. Then the attributes of the current class
    for attr in block.attrs:
        if attr.type != "suppress_param":
            attr_nam = valid_variable_name(attr.name)
            ret[attr_nam] = attr.synos[:]
            # Append original attribute name if it was modified by valid_variable_name():
            if attr_nam != attr.name:
                ret[attr_nam].append(attr.name)
        else:
            # Remove 'suppressed' attributes from inherited part:
            base_syn.pop(attr.name)
    # 3. Then the attributes of the inherited class (after suppressed have been removed, and discarding None key
    # which corresponds to the syno of the keyword itself!)
    for k in base_syn:
        if not k is None:
            ret[k] =  base_syn[k]
    block.attr_synos = ret
    return ret

def write_pyd_block(block, pyd_file, all_blocks):
    """ Write a TRAD2Block as a pydantic class, in the pyd_file
    """
    assert isinstance(block, tu.TRAD2Block)

    if block.pyd_written: return

    # dependencies must be written before self
    dependencies = [block.name_base] + [a.type for a in block.attrs]
    for dependency in dependencies:
        dependency = all_blocks.get(dependency, None)
        if dependency:
            write_pyd_block(dependency, pyd_file, all_blocks)

    # Get base class name. If void (like for Objet_U), inherit from TRUSTBaseModel:
    base_cls_n = ClassFactory.ToPydName(block.name_base) or "TRUSTBaseModel"
    lines = [
        f'#' * 64,
        f'',
        f'class {ClassFactory.ToPydName(block.name)}({base_cls_n}):',
    ]
    lines += format_docstring(block.desc)

    # Prepare keyword and arg synonyms:
    synonyms = generate_attribute_synos(block, all_blocks)

    typ_map = {
        "entier":     ("int",             "default=0"),
        "floattant":  ("float",           "default=0.0"),
        "chaine":     ("str",             "default=''"),
        "rien":       ("bool",            "default=False"),
        "list":       ("List[float]",     "default_factory=list"),
        "listf":      ("Annotated[List[float], 'size_is_dim']",    "default_factory=list"),
        "listentier": ("List[int]",                                "default_factory=list"),
        "listentierf":("Annotated[List[int], 'size_is_dim']",      "default_factory=list"),
        "listchaine": ("List[str]",                                "default_factory=list"),
        "listchainef":("Annotated[List[str], 'size_is_dim']",      "default_factory=list"),
        }

    for attr in block.attrs:
        assert isinstance(attr, tu.TRAD2Attr)
        args = f'default_factory=lambda: eval("{ClassFactory.ToPydName(attr.type)}()")'
        attr_typ = attr.type
        attr_desc = attr.desc

        if attr.type in all_blocks:    # Complex attribute (=another TRUST class)
            cls = all_blocks[attr.type]
            if isinstance(cls, tu.TRAD2BlockList):
                attr_typ = f'Annotated[List["{ClassFactory.ToPydName(cls.classtype)}"], "{ClassFactory.ToPydName(attr.type)}"]'
                attr_desc = cls.desc
                args = f'default_factory=list'
            else:
                attr_typ = ClassFactory.ToPydName(attr.type)
        elif attr.type in typ_map:
            attr_typ, args = typ_map[attr.type]
        elif attr.type.startswith("chaine(into="):
            choices = attr.type[13:].split("]")[0]
            attr_typ= f'Literal[{choices}]'
            args = f'default={choices.split(",")[0]}'
        elif attr.type.startswith("entier(into="):
            choices = attr.type[13:].split("]")[0]
            choices = [int(x.strip('"').strip("'")) for x in choices.split(",")]
            attr_typ= f'Literal{choices}'
            args = f'default={choices[0]}'
        elif attr.type.startswith("entier(max="):
            max_val = int(attr.type[11:-1])
            attr_typ= "int"
            args = f'default=0, le={max_val}'
        elif attr.type == "suppress_param":
            # NOTE tricky
            # hide the inherited *instance* attribute by a *class* attribute with same name
            attr_typ = "ClassVar[str]"
            attr_desc = "suppress_param"
            args = 'default="suppress_param"'
        elif attr.type.startswith("ref_"):
            attr_typ = "str"
            args = f'default=""'
        else:
            message = f"unresolved type in XD tags : '{attr.type}' - this was found line {attr.info[1]} of file '{attr.info[0]}' - did you forget to define it?"
            logger.error(message)
            raise NotImplementedError(message)

        # Is the attribute optional?
        if attr.is_opt and not attr_desc =="suppress_param":
            attr_typ = f"Optional[{attr_typ}]"
            args = "default=None"  # attribute is not set by default
        # Fix attribute name:
        attr_nam = valid_variable_name(attr.name)

        lines.append(f'    {attr_nam}: {attr_typ} = Field(description=r"{attr_desc}", {args})')

    lines += [
        f'    _synonyms: ClassVar[dict] = {synonyms}',
    ]
    lines.append('\n')
    # actual file writing
    pyd_file.write('\n'.join(lines))
    block.pyd_written = True

def write_pars_block(block, pars_file, all_blocks):
    """ Write a TRAD2Block as a Parser class, in the pars_file
    """
    import trustify.base as tb
    assert isinstance(block, tu.TRAD2Block)
    if block.pars_written: return

    # dependencies must be written before self
    dependencies = [block.name_base] + [a.type for a in block.attrs]
    for dependency in dependencies:
        dependency = all_blocks.get(dependency, None)
        if dependency and dependency.name not in ["listobj", "listobj_impl"]:
            write_pars_block(dependency, pars_file, all_blocks)

    # Get base class name. If void (like for Objet_U), inherit from base.ConstrainBase_Parser:
    if block.name_base != "":
        if block.name_base == "listobj":
            base_cls_n = "base.ListOfBase_Parser"
        else:
            base_cls_n = ClassFactory.ToParserName(block.name_base)
    else:
        base_cls_n = "base.ConstrainBase_Parser"
    cls_nam = ClassFactory.ToParserName(block.name)
    # If the class is already defined in base.py, skip it:
    if cls_nam in tb.__dict__:
        logger.debug(f"Class {cls_nam} already found in 'base' module - not generated.")
        block.pars_written = True
        return
    lines = [
        f'#' * 64,
        f'',
        f'class {cls_nam}({base_cls_n}):',
    ]

    info_attr = {}
    for attr in block.attrs:
        info_attr[valid_variable_name(attr.name)] = tuple(attr.info)

    lines += [f'    _braces: int = {block.mode}']

    # The XXX_base class (and similar) will need to read their type directly in the dataset, so need this:
    if is_base_or_deriv(block.name):
        lines += [f'    _read_type: bool = True']

    # Lists need extra information:
    if block.name_base == "listobj":
        assert isinstance(block, tu.TRAD2BlockList)
        lines += [f'    _comma: int = {block.comma}',
                  f'    _itemType: Objet_u = {ClassFactory.ToPydName(block.classtype)}']

    lines += [
        f'    _infoMain: list = {block.info}',
        f'    _infoAttr: dict = {info_attr}',
        f'    _attributeList: Optional[list] = None',  # Needs to be defined in every class.
        f'    _attributeSynos: Optional[dict] = None'  # Idem - see methods _GetAtributeList() and _InvertSynos() in base.py
    ]
    lines.append('\n')
    # actual file writing
    pars_file.write('\n'.join(lines))
    block.pars_written = True

def generate_pyd_and_pars(trad2_filename, trad2_nfo_filename, out_pyd_filename,
                          out_pars_filename, testing=False):
    """ Generate both modules (pydantic and parsing ones) """

    all_blocks = tu.TRAD2Content.BuildContentFromTRAD2(trad2_filename, trad2_nfo=trad2_nfo_filename).data

    # add two base classes that are required and not declared in TRAD2 file
    objet_u = tu.TRAD2Block()
    objet_u.name = "objet_u"
    objet_u.mode = 1
    listobj_impl = tu.TRAD2Block()
    listobj_impl.name = "listobj_impl"
    listobj_impl.mode = 1
    all_blocks += [objet_u, listobj_impl]

    # make a dict to easily find block by name
    all_blocks = { block.name: block for block in all_blocks }

    # add two properties used during writing of blocks
    for block in all_blocks.values():
        block.pyd_written = False
        block.pars_written = False

    header_com = f'''
        ################################################################
        # This file was generated automatically from :
        # {str(trad2_filename)}
        # {datetime.datetime.now().strftime(r"%y-%m-%d at %H:%M:%S")}
        ################################################################

    '''
    header_pyd = header_com + f'''
        from typing import Annotated, ClassVar, List, Literal, Optional, Any, Dict
        import pydantic
        from pydantic import ConfigDict, Field

        class TRUSTBaseModel(pydantic.BaseModel):
            model_config = ConfigDict(validate_assignment=True)
            
            def __init__(self, *args, **kwargs):
                pydantic.BaseModel.__init__(self, *args, **kwargs)
                self._parser = None   #: An instance of AbstractCommon_Parser that is used to build the current pydantic object 
            
            def toDatasetTokens(self):
                """ Convert a pydantic object (self) back to a stream of tokens that can be output in a file to reproduce
                a TRUST dataset. """
                from trustify.misc_utilities import ClassFactory
                if self._parser is None:
                    self._parser = ClassFactory.GetParserFromPyd(self.__class__)()
                self._parser._pyd_value = self
                return self._parser.toDatasetTokens()
                
            def __getattribute__(self, nam):
                """ Override to allow the (scripting) user to use attribute synonyms, for 
                example 'pb.post_processing' instead of 'pb.postraitement' ... 
                """
                cls = super().__getattribute__("__class__")
                if not nam in super().__getattribute__("model_fields"):
                    # Not very efficient, but the lists should never be too big ...
                    while not cls is TRUSTBaseModel:
                        for attr_nam, lst_syno in cls._synonyms.items():
                            if not attr_nam is None and nam in lst_syno:
                                return super().__getattribute__(attr_nam)
                        cls = cls.__base__ # Go one level up
                return super().__getattribute__(nam)

            def __setattr__(self, nam, val):
                """ Override to allow the (scripting) user to use attribute synonyms, for 
                example 'pb.post_processing' instead of 'pb.postraitement' ... 
                """
                # Go over all base class to try to find a synonym
                cls = self.__class__
                if not nam in self.model_fields:
                    # Not very efficient, but the lists should never be too big ...
                    while not cls is TRUSTBaseModel:
                        for attr_nam, lst_syno in cls._synonyms.items():
                            if not attr_nam is None and nam in lst_syno:
                                return super().__setattr__(nam, val)
                        cls = cls.__base__ # Go one level up
                return super().__setattr__(nam, val)

    '''

    footer_pyd = f'''
    ##################################################
    ### Classes Declaration and Dataset
    ##################################################
    class Declaration(Objet_u):
        """ Added Pydantic class to handle forward declaration in the TRUST dataset """
        ze_type    : type = Field(description='Class type being read in the forward declaration', default=None)
        identifier : str  = Field(description='Name assigned to the object in the dataset', default='??')

    class Read(Interprete):
        """ The 'read' instruction in a TRUST dataset. Overriden from the automatic generation to make the second argument a Objet_u.
            See also Read_Parser class in base.py module.
        """
        identifier: str = Field(description='Identifier of the class being read. Must match a previous forward Declaration.', default="??")
        obj       : Objet_u = Field(description='The object being read.', default=None)
        _synonyms: ClassVar[dict] = {{None: ['lire'], 'identifier': [], 'obj': []}}

    class Dataset(Objet_u):
        """ A full TRUST dataset! It is an ordered list of objects. """
        _declarations: Dict[str,Any] = {{ }}  # Private - key: declaration name (like 'pb' in 'pb_conduction pb'), value: a couple (cls, index) 
                                            # where 'cls' is a Declaration object, and index is its position in the 'entries' member
        entries   : List[Objet_u]   = Field(description='The objects making up the dataset', default=[])

        def get(self, identifier):
            """ User method - Returns the object associated with a name in the data set """
            from trustify.misc_utilities import TrustifyException
            if not identifier in self._declarations:
                raise TrustifyException(f"Invalid identifer '{{identifier}}'")
            it_num = self._declarations[identifier][1]
            if it_num < 0:
                raise TrustifyException(f"Identifer '{{identifier}}' has been declared, but has not been read in the dataset (no 'read {{identifier}} ...' instruction)")
            # Return the object attached into the 'read' instance:
            return self.entries[it_num].obj
    '''

    header_pars = header_com + f'''
        import trustify.base as base
        from trustify.base import *
        # Import all the Pydantic generated classes:
        from {out_pyd_filename.stem} import *
    '''

    footer_pars = f'''
        ################################################################
        # Register all classes declared so far:
        ################################################################

        from trustify.misc_utilities import ClassFactory
        g = globals()
        for nam in list(g):
            val = g[nam]
            if isinstance(val, type) and (issubclass(val, Abstract_Parser) or issubclass(val, Objet_u)):
                ClassFactory.RegisterClass(nam, val)
        
        ################################################################
        # Register all synonyms - see method documentation for more explanations:
        ################################################################
        ClassFactory.BuildSynonymMap()
        
        ################################################################
        # Import all very weird hacks that TRUST mis-behaved keywords 
        # force us to do ... 
        ################################################################
    '''


    out_pyd_filename = out_pyd_filename or trad2_filename.name + "_pyd.py"
    out_pars_filename = out_pars_filename or trad2_filename.name + "_pars.py"

    # Generate output dirs if needed:
    out_pyd_filename.parents[0].mkdir(parents=True, exist_ok=True)
    out_pars_filename.parents[0].mkdir(parents=True, exist_ok=True)

    # Writing Pydantic schema:
    with open(out_pyd_filename, "w", encoding="utf-8") as pyd_file:
        pyd_file.write(textwrap.dedent(header_pyd).strip())
        pyd_file.write("\n" * 3)
        for block in all_blocks.values():
            write_pyd_block(block, pyd_file, all_blocks)
        # Put implementation of Declaration and Dataset:
        pyd_file.write(textwrap.dedent(footer_pyd).strip())
        pyd_file.write("\n")

    # Writing parsing classes:
    with open(out_pars_filename, "w", encoding="utf-8") as pars_file:
        pars_file.write(textwrap.dedent(header_pars).strip())
        pars_file.write("\n" * 3)
        for block in all_blocks.values():
            if block.name in ["listobj", "listobj_impl"]:  # Those were directly redirected to ListOfBase_Parser
                continue
            write_pars_block(block, pars_file, all_blocks)
        # Register all classes in factory:
        pars_file.write(textwrap.dedent(footer_pars).strip())
        pars_file.write("\n")
        # Write hacks:
        import trustify
        mod_pth = pathlib.Path(trustify.__file__)
        hack_file = mod_pth.parents[0].joinpath("hacks.py")
        with open(hack_file, "r") as hack_f:
            pars_file.write(hack_f.read())

    return all_blocks


def test_loading(out_pyd_filename, all_blocks):
    if testing:
        test_filename = str(out_pyd_filename).replace(".py", "_test.py")
        with open(test_filename, "w", encoding="utf-8") as file:
            file.write(f'import {trad2_filename.name}')
            file.write("\n" * 3)
            for cls in all_blocks.keys():
                file.write(f'{trad2_filename.name}.{cls}()\n')

def do_main():
    import sys
    if len(sys.argv) > 1:
        trad2_file, pyd_file, pars_file = map(pathlib.Path, sys.argv[1:4])
        # Get info file (for debug infos)
        nfo_file = trad2_file.with_suffix(".nfo")
    else:
        root_dir = pathlib.Path(os.getenv("TRUST_ROOT")) / "Outils" / "trustify" / "test"
        trad2_file = root_dir / "trad2" / "TRAD_2_adr_simple"
        nfo_file = None
        pyd_file = root_dir / "generated" / (trad2_filename.name + "_pyd.py")
        pars_file = root_dir / "generated" / (trad2_filename.name + "_pars.py")
    generate_pyd_and_pars(trad2_file, nfo_file, pyd_file, pars_file)
    logger.info(f"Generated file: {str(pyd_file)}")
    logger.info(f"Generated file: {str(pars_file)}")

if __name__ == '__main__':
    #logger.setLevel(logging.DEBUG)
    do_main()
    # test_loading(out_pyd_filename, all_blocks)
