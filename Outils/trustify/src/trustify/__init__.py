import contextlib

@contextlib.contextmanager
def temporary_sys_path(path):
    import sys
    original_path = sys.path.copy()
    sys.path.insert(0, path)
    try:
        yield
    finally:
        sys.path[:] = original_path
        
        
def load_dataset(filename, parser_module=None):
    """
    Load a TRUST dataset from file.data using the TRUST parser module.

    Arguments
    ---------
    filename: path-like
        The trust.data filename.
    parser_module: path-like
        The python filename containing TRUST Parsers.
        Usually a file named TRAD2_*_pars.py.
        Leave it None if TRAD2_*_pars.py is already imported.

    Returns
    -------
    pydantic.BaseModel
        The TRUST dataset as a pydantic.BaseModel instance.
    """

    # read the file.data
    with open(filename, "r") as file:        
        data = file.read().rstrip()
    
    # import the parser module if argument is provided
    if parser_module is not None:
        import pathlib
        parser_module = pathlib.Path(parser_module)
        if not parser_module.exists():
            raise ModuleNotFoundError(parser_module)
        import importlib
        with temporary_sys_path(str(parser_module.parent.resolve())):
            module_name = parser_module.stem
            spec = importlib.util.spec_from_file_location(module_name, str(parser_module))
            module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(module)
    
    # parse dataset
    from trustify import trust_parser, misc_utilities
    parser = trust_parser.TRUSTParser()
    parser.tokenize(data)
    stream = trust_parser.TRUSTStream(parser=parser)
    cls = misc_utilities.ClassFactory.GetParserClassFromName("Dataset")
    model = cls.ReadFromTokens(stream)

    return model

