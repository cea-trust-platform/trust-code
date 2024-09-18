import pathlib
import textwrap
import datetime
import trustify.trad2_utilities as tu
from trustify.misc_utilities import logger, change_class_name

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
    if cls_nam.endswith("_deriv") or cls_nam.endswith("_base") or cls_nam == "class_generic":
        return True
    return False

def write_pyd_block(block, pyd_file, all_blocks):
    """ Write a TRAD2Block as a pydantic class, in the pyd_file
    """
    assert isinstance(block, tu.TRAD2Block)

    if block.pyd_written: return

    logger.debug(block.nam)

    # dependencies must be written before self
    dependencies = [block.name_base] + [a.typ for a in block.attrs]
    for dependency in dependencies:
        dependency = all_blocks.get(dependency, None)
        if dependency:
            write_pyd_block(dependency, pyd_file, all_blocks)

    # Get base class name. If void (like for Objet_U), inherit from pydantic.BaseModel:
    base_cls_n = change_class_name(block.name_base) or "pydantic.BaseModel"
    lines = [
        f'#' * 64,
        f'',
        f'class {change_class_name(block.nam)}({base_cls_n}):',
    ]
    lines += format_docstring(block.desc)

    synonyms = {None: block.synos}

    for attribute in block.attrs:
        assert isinstance(attribute, tu.TRAD2Attr)
        attr_name = attribute.nam
        attr_type = attribute.typ
        attr_mode = attribute.is_opt
        attr_desc = attribute.desc
        synonyms[attr_name] = attribute.synos
        attr_name = valid_variable_name(attr_name)
        args = f'default_factory=lambda: eval("{change_class_name(attr_type)}()")'

        if attr_type in all_blocks:
            cls = all_blocks[attr_type]
            if isinstance(cls, tu.TRAD2BlockList):
                attr_type = f'Annotated[List["{change_class_name(cls.classtype)}"], {cls.comma}]'
                attr_desc = cls.desc
                args = f'default_factory=list'
            else:
                attr_type = change_class_name(attr_type)
        elif attr_type == "entier":
            attr_type = "int"
            args = f'default=0'
        elif attr_type == "floattant":
            attr_type = "float"
            args = f'default=0.0'
        elif attr_type == "chaine":
            attr_type = "str"
            args = f'default=""'
        elif attr_type in ["list", "listf"]:
            # TODO handle difference list vs listf ?
            attr_type = "List[float]"
            args = "default_factory=list"
        elif attr_type in ["listentier", "listentierf"]:
            # TODO handle difference listentier vs listentierf ?
            attr_type = "List[int]"
            args = "default_factory=list"
        elif attr_type in ["listchaine", "listchainef"]:
            # TODO handle difference listchaine vs listchainef ?
            attr_type = "List[str]"
            args = "default_factory=list"
        elif attr_type.startswith("chaine(into="):
            choices = attr_type[13:].split("]")[0]
            attr_type= f'Literal[{choices}]'
            args = f'default={choices.split(",")[0]}'
        elif attr_type.startswith("entier(into="):
            choices = attr_type[13:].split("]")[0]
            choices = [int(x.strip('"').strip("'")) for x in choices.split(",")]
            attr_type= f'Literal{choices}'
            args = f'default={choices[0]}'
        elif attr_type.startswith("entier(max="):
            max_val = int(attr_type[11:-1])
            attr_type= "int"
            args = f'default=0, le={max_val}'
        elif attr_type == "rien":
            attr_type = "bool"
            args = f'default=False'
        elif attr_type == "suppress_param":
            # NOTE tricky
            # hide the inherited *instance* attribute by a *class* attribute with same name
            attr_type = "ClassVar[str]"
            attr_desc = "suppress_param"
            args = 'default="suppress_param"'
        elif attr_type.startswith("ref_"):
            attr_type = "str"
            args = f'default=""'
        else:
            message = f"unresolved type : {attr_type}"
            logger.error(message)
            raise NotImplementedError(message)

        lines.append(f'    {attr_name}: {attr_type} = Field(description=r"{attr_desc}", {args})')

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

    logger.debug(block.nam)

    # dependencies must be written before self
    dependencies = [block.name_base] + [a.typ for a in block.attrs]
    for dependency in dependencies:
        dependency = all_blocks.get(dependency, None)
        if dependency:
            write_pars_block(dependency, pars_file, all_blocks)

    # Get base class name. If void (like for Objet_U), inherit from base.Base_common_Parser:
    base_cls_n = change_class_name(block.name_base) or "Base_common"
    base_cls_n += tb.PARS_SUFFIX
    cls_nam = change_class_name(block.nam) + tb.PARS_SUFFIX
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

    read_type = is_base_or_deriv(block.nam)
    info_attr = {}
    for attr in block.attrs:
        info_attr[attr.nam] = tuple(attr.info)

    lines += [
        f'    _braces: int = {block.mode}',
        f'    _read_type: bool = {read_type}',
        f'    _infoMain: dict = {block.info}',
        f'    _infoAttr: dict = {info_attr}',
    ]
    lines.append('\n')
    # actual file writing
    pars_file.write('\n'.join(lines))
    block.pars_written = True

def generate_pyd_and_pars(trad2_filename, out_pyd_filename, out_pars_filename, testing=False):

    trad2_filename = pathlib.Path(trad2_filename)

    all_blocks = tu.TRAD2Content.BuildContentFromTRAD2(trad2_filename).data

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
        from typing import ClassVar, List, Optional
        import pydantic
        from pydantic import ConfigDict, Field

        class BaseModel(pydantic.BaseModel):
            model_config = ConfigDict(validate_assignment=True)
    '''
    header_pars = header_com + f'''
        from trustify.base import *
    '''

    out_pyd_filename = out_pyd_filename or trad2_filename.name + "_pyd.py"
    out_pars_filename = out_pars_filename or trad2_filename.name + "_pars.py"

    # Writing Pydantic schema:
    with open(out_pyd_filename, "w", encoding="utf-8") as pyd_file:
        pyd_file.write(textwrap.dedent(header_pyd).strip())
        pyd_file.write("\n" * 3)
        for block in all_blocks.values():
            write_pyd_block(block, pyd_file, all_blocks)

    # Writing parsing classes:
    with open(out_pars_filename, "w", encoding="utf-8") as pars_file:
        pars_file.write(textwrap.dedent(header_pars).strip())
        pars_file.write("\n" * 3)
        for block in all_blocks.values():
            write_pars_block(block, pars_file, all_blocks)

    if testing:
        test_filename = str(out_pyd_filename).replace(".py", "_test.py")
        with open(test_filename, "w", encoding="utf-8") as file:
            file.write(f'import {trad2_filename.name}')
            file.write("\n" * 3)
            for cls in all_blocks.keys():
                file.write(f'{trad2_filename.name}.{cls}()\n')


if __name__ == '__main__':
    import os, logging
    logger.setLevel(logging.DEBUG)
    root_dir = os.getenv("TRUST_ROOT") + "/Outils/trustify/test"
    trad2_filename = pathlib.Path(root_dir + "/trad2/TRAD_2_adr_simple")
    out_pyd_filename = pathlib.Path(root_dir + "/generated/" + trad2_filename.name + "_pyd.py")
    out_pars_filename = pathlib.Path(root_dir + "/generated/" + trad2_filename.name + "_pars.py")
    generate_pyd_and_pars(trad2_filename, out_pyd_filename, out_pars_filename, testing=True)
