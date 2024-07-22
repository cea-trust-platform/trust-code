import pathlib
import textwrap
import datetime
from trustpy.trad2_utilities import TRAD2Content, TRAD2Block, TRAD2Attr, TRAD2BlockList


################################################################
# LOGGER
################################################################

def init_logger():
    import logging
    logger = logging.getLogger("trad2_pydantic")
    handler = logging.StreamHandler()
    formatter = logging.Formatter(fmt="%(asctime)s %(levelname)-8s %(message)s", datefmt="%H:%M:%S")
    handler.setFormatter(formatter)
    logger.addHandler(handler)
    logger.setLevel(logging.DEBUG)
    return logger
logger = init_logger()


filename = "src/trustpy/test/TRAD_2_GENEPI_V17"
filename = pathlib.Path(filename)

trad2 = TRAD2Content.BuildContentFromTRAD2(filename)

objet_u = TRAD2Block()
objet_u.nam = "objet_u"
objet_u.mode = 1
listobj_impl = TRAD2Block()
listobj_impl.nam = "listobj_impl"
listobj_impl.mode = 1

# initialize two base classes that are not in TRAD2 file
trad2.data += [objet_u, listobj_impl]

# mapping used for ordering the dependencies
ALL_CLASSES = { block.nam: block for block in trad2.data }

written = []

def valid_variable_name(s):

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


def write(block, file):
    """ Write a TRAD2Block as pydantic class. """

    assert isinstance(block, TRAD2Block)

    if block.nam in written:
        return

    logger.debug(block.nam)

    # dependencies must be written before self
    dependencies = [block.name_base] + [a.typ for a in block.attrs]
    for dependency in dependencies:
        dependency = ALL_CLASSES.get(dependency, None)
        if dependency:
            write(dependency, file)


    lines = [
        f'#' * 64,
        f'',
        f'class {block.nam}({block.name_base or "BaseModel"}):',
        f'    __braces:int = {block.mode}',
        f'    __description: str = r"{block.desc}"',
        f'    __synonyms: str = {block.synos}',
    ]

    suppressed_params = []

    for attribute in block.attrs:

        assert isinstance(attribute, TRAD2Attr)
        # attr_name, attr_type, attr_syno, attr_mode, attr_desc = attribute
        attr_name = attribute.nam
        attr_type = attribute.typ
        attr_syno = attribute.synos
        attr_mode = attribute.is_opt
        attr_desc = attribute.desc            

        attr_name = valid_variable_name(attr_name)

        args = f'default_factory=lambda: eval("{attr_type}()")'

        if attr_type in ALL_CLASSES:
            cls = ALL_CLASSES[attr_type]
            if isinstance(cls, TRAD2BlockList):
                attr_type = f'typing.Annotated[typing.List["{cls.classtype}"], {cls.comma}]'
                attr_desc = cls.desc
                args = f'default=[]'

        elif attr_type == "entier":
            attr_type = "int"
            args = f'default=0'

        elif attr_type == "floattant":
            attr_type = "float"
            args = f'default=0.0'

        elif attr_type == "chaine":
            # TODO create custom class str_trust to handle "{ how do you { do } }" ?
            # SEE class chaine_Tru
            attr_type = "str"
            args = f'default=""'

        elif attr_type in ["list", "listf"]:
            attr_type = "typing.List[float]"
            args = "default=[]"

        elif attr_type in ["listentier", "listentierf"]:
            attr_type = "typing.List[int]"
            args = "default=[]"

        elif attr_type in ["listchaine", "listchainef"]:
            attr_type = "typing.List[str]"
            args = "default=[]"

        elif attr_type.startswith("chaine(into="):
            choices = attr_type[13:].split("]")[0]
            attr_type= f'typing.Literal[{choices}]'
            args = f'default={choices.split(",")[0]}'

        elif attr_type.startswith("entier(into="):
            choices = attr_type[13:].split("]")[0]
            choices = [int(x.strip('"').strip("'")) for x in choices.split(",")]
            attr_type= f'typing.Literal{choices}'
            args = f'default={choices[0]}'

        elif attr_type.startswith("entier(max="):
            max_val = int(attr_type[11:-1])
            attr_type= "int"
            args = f'default=0, le={max_val}'

        elif attr_type == "rien":
            attr_type = "bool"
            args = f'default=False'

        elif attr_type == "suppress_param":
            suppressed_params.append(attr_name)
            continue

        elif attr_type.startswith("ref_"):
            attr_type = "str"
            args = f'default=""'

        else:
            logger.error(f"unresolved type : {str(attribute)}")
            continue

        if str(attr_type) == str(attr_name):
            attr_name += '_'

        lines.append(f'    {attr_name}: {attr_type} = Field(description=r"{attr_desc}", {args})')

    if suppressed_params:
        lines.append('')
        lines.append('# suppress fields from parent class')
        for param in suppressed_params:
            lines.append(f'{block.nam}.model_fields.pop("{param}")')

    lines.append('\n')

    file.write('\n'.join(lines))

    written.append(block.nam)


header = f'''
    ################################################################
    # This file was generated automatically from :
    # {str(filename.resolve())}
    # {datetime.datetime.now().strftime(r"%y-%m-%d at %H:%M:%S")}
    ################################################################

    import sys
    if sys.version_info >= (3, 8):
        import typing
    else:
        import typing_extensions as typing
    from pydantic import BaseModel, Field
'''


output_filename = filename.name + ".py"
with open(output_filename, "w", encoding="utf-8") as file:
    file.write(textwrap.dedent(header).strip())
    file.write("\n" * 3)
    for data in trad2.data:
        write(data, file)
    


test_filename = filename.name + "_test.py"

with open(test_filename, "w", encoding="utf-8") as file:
    
    file.write(f'import {filename.name}')
    file.write("\n" * 3)

    for cls in written:
        file.write(f'{filename.name}.{cls}()\n')

