import os
import pytest
import pathlib
import pydantic
from trustify.trad2_pydantic import generate_pydantic


test_files = [
    "TRAD_2_adr_simple",
    "TRAD_2_GENEPI_V17"
]

@pytest.mark.parametrize("file", test_files)
def test_generate_pydantic(file):

    filename = pathlib.Path("test", file)
    assert filename.exists()

    # generate pydantic schema
    generate_pydantic(trad2_filename=filename, output_filename="pydantic_schema.py")

    # reload the generated pydantic schema
    import pydantic_schema
    import importlib
    importlib.reload(pydantic_schema)

    # list of all classes in pydantic_schema
    all_classes = []
    for keyword in pydantic_schema.__dict__.values():
        if isinstance(keyword, type) and issubclass(keyword, pydantic.BaseModel):
            all_classes.append(keyword)

    assert len(all_classes) > 2

    # try to instantiate each class
    for cls in all_classes[1:]:
        instance = cls()
        assert cls._braces in [-3, -2, -1, 0, 1]
        assert isinstance(cls._synonyms, dict) and None in cls._synonyms
        assert isinstance(cls._traces, dict) and None in cls._traces

    # delete the generated pydantic_schema.py
    os.unlink("pydantic_schema.py")
