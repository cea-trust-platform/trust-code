""" From a TRAD2 file, generates:
 - the Pydantic schema (as a generated Python file containing one Pydantic class per TRUST keyword) 
 - the parsing classes (as a generated Python file, containing one Python class per TRUST keyword)
"""

import pathlib
import textwrap
import datetime
import trustify.trad2_utilities as tu
from trustify.misc_utilities import logger, ClassFactory, TrustifyException

################################################################

def valid_variable_name(s):
    """ Make a valid variable name from any str. """
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
    return cls_nam.endswith("_deriv") or cls_nam.endswith("_base") or cls_nam == "class_generic"

def write_pyd_block(block, pyd_file, all_blocks):
    """ Write a TRAD2Block as a pydantic class, in the pyd_file
    """
    assert isinstance(block, tu.TRAD2Block)

    if block.pyd_written: return

    # dependencies must be written before self
    dependencies = [block.name_base] + [a.typ for a in block.attrs]
    for dependency in dependencies:
        dependency = all_blocks.get(dependency, None)
        if dependency:
            write_pyd_block(dependency, pyd_file, all_blocks)

    # Get base class name. If void (like for Objet_U), inherit from TRUSTBaseModel:
    base_cls_n = ClassFactory.ToPydName(block.name_base) or "TRUSTBaseModel"
    lines = [
        f'#' * 64,
        f'',
        f'class {ClassFactory.ToPydName(block.nam)}({base_cls_n}):',
    ]
    lines += format_docstring(block.desc)

    syns = [s for s in block.synos if s != block.nam]
    synonyms = {None: syns}

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
        args = f'default_factory=lambda: eval("{ClassFactory.ToPydName(attr.typ)}()")'
        attr_typ = attr.typ
        attr_desc = attr.desc

        if attr.typ in all_blocks:
            cls = all_blocks[attr.typ]
            if isinstance(cls, tu.TRAD2BlockList):
                attr_typ = f'Annotated[List["{ClassFactory.ToPydName(cls.classtype)}"], "{ClassFactory.ToPydName(attr.typ)}"]'
                attr_desc = cls.desc
                args = f'default_factory=list'
            else:
                attr_typ = ClassFactory.ToPydName(attr.typ)
        elif attr.typ in typ_map:
            attr_typ, args = typ_map[attr.typ]
        elif attr.typ.startswith("chaine(into="):
            choices = attr.typ[13:].split("]")[0]
            attr_typ= f'Literal[{choices}]'
            args = f'default={choices.split(",")[0]}'
        elif attr.typ.startswith("entier(into="):
            choices = attr.typ[13:].split("]")[0]
            choices = [int(x.strip('"').strip("'")) for x in choices.split(",")]
            attr_typ= f'Literal{choices}'
            args = f'default={choices[0]}'
        elif attr.typ.startswith("entier(max="):
            max_val = int(attr.typ[11:-1])
            attr_typ= "int"
            args = f'default=0, le={max_val}'
        elif attr.typ == "suppress_param":
            # NOTE tricky
            # hide the inherited *instance* attribute by a *class* attribute with same name
            attr_typ = "ClassVar[str]"
            attr_desc = "suppress_param"
            args = 'default="suppress_param"'
        elif attr.typ.startswith("ref_"):
            attr_typ = "str"
            args = f'default=""'
        else:
            message = f"unresolved type in XD tags : '{attr.typ}' - this was found line {attr.info[1]} of file '{attr.info[0]}' - did you forget to define it?"
            logger.error(message)
            raise NotImplementedError(message)

        # Is the attribute optional?
        if attr.is_opt and not attr_desc =="suppress_param":
            attr_typ = f"Optional[{attr_typ}]"
            args = "default=None"  # attribute is not set by default
        # Fix attribute name:
        attr_nam = valid_variable_name(attr.nam)
        # Register attribute synonyms (only if attribute was not suppressed)
        if attr_desc != "suppress_param":
            synonyms[attr_nam] = attr.synos
            # If attribute name was fixed, register original name as a synonym:
            if attr_nam != attr.nam:
                synonyms[attr_nam].append(attr.nam)

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
    dependencies = [block.name_base] + [a.typ for a in block.attrs]
    for dependency in dependencies:
        dependency = all_blocks.get(dependency, None)
        if dependency and dependency.nam not in ["listobj", "listobj_impl"]:
            write_pars_block(dependency, pars_file, all_blocks)

    # Get base class name. If void (like for Objet_U), inherit from base.ConstrainBase_Parser:
    if block.name_base != "":
        if block.name_base == "listobj":
            base_cls_n = "base.ListOfBase_Parser"
        else:
            base_cls_n = ClassFactory.ToParserName(block.name_base)
    else:
        base_cls_n = "base.ConstrainBase_Parser"
    cls_nam = ClassFactory.ToParserName(block.nam)
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
        info_attr[valid_variable_name(attr.nam)] = tuple(attr.info)

    lines += [f'    _braces: int = {block.mode}']

    # The XXX_base class (and similar) will need to read their type directly in the dataset, so need this:
    if is_base_or_deriv(block.nam):
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

    all_blocks = tu.TRAD2Content.BuildContentFromTRAD2(trad2_filename, trad2_nfo=trad2_nfo_filename).data

    # add two base classes that are required and not declared in TRAD2 file
    objet_u = tu.TRAD2Block()
    objet_u.nam = "objet_u"
    objet_u.mode = 1
    listobj_impl = tu.TRAD2Block()
    listobj_impl.nam = "listobj_impl"
    listobj_impl.mode = 1
    all_blocks += [objet_u, listobj_impl]

    # make a dict to easily find block by name
    all_blocks = { block.nam: block for block in all_blocks }

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
        from typing_extensions import Annotated, Literal
        from typing import ClassVar, List, Optional, Any, Dict
        import pydantic
        from pydantic import ConfigDict, Field

        class TRUSTBaseModel(pydantic.BaseModel):
            model_config = ConfigDict(validate_assignment=True)
            
            def __init__(self, *args, **kwargs):
                pydantic.BaseModel.__init__(self, *args, **kwargs)
                self._parser = None
            
            def toDatasetTokens(self):
                from trustify.misc_utilities import ClassFactory
                if self._parser is None:
                    self._parser = ClassFactory.GetParserFromPyd(self.__class__)()
                self._parser._pyd_value = self
                return self._parser.toDatasetTokens()
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
        _synonyms: ClassVar[dict] = {{None: ['lire'], 'a_object': [], 'bloc': []}}

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
            if block.nam in ["listobj", "listobj_impl"]:  # Those were directly redirected to ListOfBase_Parser
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


if __name__ == '__main__':
    logger.setLevel(logging.DEBUG)
    root_dir = pathlib.Path(os.getenv("TRUST_ROOT")) / "Outils" / "trustify" / "test"
    trad2_filename = root_dir / "trad2" / "TRAD_2_adr_simple"
    out_pyd_filename = root_dir / "generated" / (trad2_filename.name + "_pyd.py")
    out_pars_filename = root_dir / "generated" / (trad2_filename.name + "_pars.py")
    all_blocks = generate_pyd_and_pars(trad2_filename, None, out_pyd_filename, out_pars_filename)
    # test_loading(out_pyd_filename, all_blocks)

