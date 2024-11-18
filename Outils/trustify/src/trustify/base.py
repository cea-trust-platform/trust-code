""" Main module containing the set of root classes allowing to parse a TRUST dataset.

This module is a bit long, but its logic can be well understood by reading first
  - classes Abstract_Parser and BaseCommon_Parser
  - and then its most important child Dataset_Parser, containing the top logic to load a TRUST dataset into the generated Pydantic
  model.

Authors: A Bruneton, C Van Wambeke, G Sutra
"""

import trustify.misc_utilities as mutil
from trustify.misc_utilities import ClassFactory, TrustifyException, get_single_parser_base
from trustify.trust_parser import TRUSTTokens

########################################################################

class Abstract_Parser:
    """ Root class for all parsers 
    The main instance attribute is self._pyd_value which stores the actual value being parsed.
    """
    _braces = 1           #: whether we expect braces when reading the class - see doc of TRAD2 format - by default, expect braces.
    _read_type = False    #: whether to read the actual type before instanciating the class (typically for *_base or *_deriv classes) - By default we do *not* read the type
    _infoMain = []        #: a tuple giving the C++ source file and the line where the type was defined

    def __init__(self, pyd_value=None):
        self._pyd_value = pyd_value #: The associated 'pydantic' or builtin value associated with this parser
                                    # This is an object that lives in the pydantic schema side
        self._tokens = {}  #: A dictionnary giving for the current instance the tokens corresponding to each bit - see override
                           # in ConstrainBase_Parser for a more comprehensive explanation.
        # Link pydatic value to this parser:
        if not pyd_value is None and not Builtin_Parser.IsBuiltin(pyd_value.__class__):
            self._pyd_value._parser = self

    def toDatasetTokens(self):
        """ Produce a list of tokens representing the object in the format expected in a TRUST dataset.
        Note: this tries as much as possible to reproduce what was initially loaded, if the
        initial parsing (via ReadFromTokens()) was successful.
        In any case, it is the responsibility of this method to check that output data
        is coherent with the real value stored in the object.
        See also checkToken() in the same class.
        """
        raise NotImplementedError

    @classmethod
    def GenErr(cls, stream, msg, attr=None):
        """ Useful method to generate nice error message when an exception is raised.
        It provides the file name and line number of:
          - the faulty dataset (.data file)
          - the point in the C++ source code (or in the TRAD2.org) where the concerned grammar element
          was defined.
        """
        err = "\n" + mutil.RED + msg + mutil.END + "\n"
        ctx = mutil.YELLOW
        ctx +=  "=> This error was triggered in the following context:\n"
        ctx += "   Dataset: line %d  in file  '%s'\n" % (stream.currentLine(), stream.fileName())

        s = ""
        if len(cls._infoMain) > 0:  # automatic classes (like base types int, float...) do not have debug info
            s =  "   Model:   line %d  in file  '%s'\n" % (cls._infoMain[1], cls._infoMain[0])
        if not attr is None:
            ze_cls = cls
            while not (attr in ze_cls._infoAttr or ze_cls is BaseCommon_Parser):
                ze_cls = get_single_parser_base(ze_cls)
            if attr in ze_cls._infoAttr:
                fnam, lineno = ze_cls._infoAttr[attr]
            s = f"   Model:   line {lineno}  in file  '{fnam}'\n"
        ctx += s + mutil.END
        return err + ctx

    @classmethod
    def GetAllClassesFromSyno(cls, kw, stream):
        """ For a given keyword (which might be a synonym) give all possible original (pydantic) classes.
        NB: TRUST is a mess!! Take 'negligeable' for example: it is a standard TRUST keyword, but is also 
        used as a synonym in multiple places .... so we need to keep all of this ...
        """
        # Is this a (non-synonym) valid class name? If so, it has precedence over any other synonyms (e.g. 'partition' ...)
        ret = []
        if ClassFactory.Exist(ClassFactory.ToPydName(kw)):
            ret.append(ClassFactory.GetPydClassFromName(kw))
        if ret == [] and not kw in ClassFactory._SYNO_ORIG_NAME:
            # (neither an original keyword, nor a synonym)
            # If we arrive here we are in fact most likely to hit a point where a previous (non-brace) keyword
            # was missing an attribute, and we treat the next token (for example '}') as a class to be read ...
            err = cls.GenErr(stream, f"Invalid TRUST keyword: '{kw}' (or maybe, did you forget an attribute on the previous line?)")
            raise TrustifyException(err)
        ret.extend(ClassFactory._SYNO_ORIG_NAME.get(kw,[]))
        return ret

    @classmethod
    def GetOneClassFromSyno(cls, kw, stream, interp=True):
        """ Same as GetAllClassesFromSyno but selecting the most likely keyword ... this is ugly and to be fixed once TRIOXDATA is dead. """
        root_cls = cls.GetAllClassesFromSyno(kw, stream)
        ret = root_cls[0]
        if len(root_cls) > 1:
            ###
            ### Ugly! Ugly! Ugly!
            ###
            # We can have more than one class matching ('ef' and 'convection_ef')
            # Give precedence either to an 'interprete' (if requested) or to the non-synonym version hoping for the best ...
            if interp:
                interp_cls = ClassFactory.GetPydClassFromName("Interprete")
                l = [c for c in root_cls if issubclass(c, interp_cls)]
                assert len(l) <= 1, f"Synonym '{kw}' matches more than one Interprete keyword!!"
                if len(l):
                    return l[0]
            if not ClassFactory.Exist(ClassFactory.ToPydName(kw)):
                err = cls.GenErr(stream, f"Keyword '{kw}' is an ambiguous synonym! Can not determine which class this corresponds to (but I tried hard).")
                raise TrustifyException(err)
            ret = ClassFactory.GetPydClassFromName(kw)
        return ret

    def checkToken(self, key, expec, typ=str):
        """ Used in toDatasetTokensBuiltin() method: consider the following:
          - a dataset has been loaded in Python: while doing so, the original tokens are stored in self._tokens
          - a value is modified in the data (for ex: "ds.get("sch").tmax = 23") -> at this point the underyling value in self.token
          still contains what was initially read!
         So this method checks whether the value stored in the token (as a string) is consistent with the actual value of the attribute.
        """
        lst_low = self._tokens.get(key, TRUSTTokens()).low()  # Sometimes the token has not been registered at all! (added attribute for example)
        s = ''.join(lst_low).strip()
        try:  # conversion function 'typ' might fail, if for example trying to convert 'qdfs' into int:
            ret = typ(s) == typ(expec.strip())  # tolerant on whitespaces
        except:
            ret = False
        return ret

    def getBraceTokens(self, brace):
        """ Get output tokens for opening/closing brace """
        if self.checkToken(brace, brace):
            return self._tokens[brace].orig()
        else:
            if brace == "{":    return ["{\n"]
            elif brace == "}":  return ["\n}\n"]
            raise Exception(f"Internal error -  unexpected parameter: '{brace}'")

    @classmethod
    def GetAllTrustNames(cls):
        """ Returns all possible names (incl. synonyms) for a keyword as a nicely formatted string.
        Used for nice error messages.
        """
        raise NotImplementedError

    @classmethod
    def ConsumeBrace(cls, stream, brace):
        """ Consume the tokens in stream until a opening or closing brace is found. 'brace' is either '{' or '}' 
        This is put this high in the class hierarchy because even Chaine_parser needs it ...
        """
        nams = cls.GetAllTrustNames()
        brac_nam = {'{':"opening", '}': "closing"}[brace]
        tok = stream.probeNextLow()
        if tok != brace:
            err = cls.GenErr(stream, f"Keyword '{nams}' expected a {brac_nam} brace '{brace}' (but we read: '{tok}')")
            raise TrustifyException(err)
        stream.validateNext()

    @classmethod
    def Dbg(cls, msg):
        """ Handy debug printing - only prints something if debug level is lower than logging.DEBUG """
        from trustify.misc_utilities import logger
        import logging
        # The 'inspect' module is very costly - so skip if not in highest logging level:
        if logger.getEffectiveLevel() > logging.DEBUG:
            return
        import inspect
        curframe = inspect.currentframe()
        calframe = inspect.getouterframes(curframe, 2)
        cal_nam = calframe[1][3]
        def pad(s):
            return s + " "*(max(55-len(s), 0))
        s = f"[{cls.__name__}.{cal_nam}] --"
        m = msg.replace("@FUNC@", s)
        logger.debug(m)

    @classmethod
    def WithBraces(cls):
        """
        Get brace style for the keyword when reading it from the data file (with braces or without). What a mess... :
        -1: like the parent class
        0: no braces, no attribute names
        1: with braces, attributes explicitly named
        -2: like 0, but should come after 'discretize' (not checked here)
        -3: like 1, but should come after 'discretize' (not checked here)
        """
        mp = {0: False, 1: True, -2: False, -3: True}
        if cls._braces in [0, 1, -2, -3]:
            return mp[cls._braces]
        # Need to look at the parent
        for c in cls.__bases__:
            if c.__name__.endswith(ClassFactory._PARSER_SUFFIX):
                return c.WithBraces()
        raise Exception("Internal error")

########################################################################

class Builtin_Parser(Abstract_Parser):
    """ Base class for all the parsers handling builtin Python types: str, int, float, list, etc.
    For builtin types the logic is a bit different than for the rest, mostly because we can not attach a parser to the type
    itself (it is forbidden in Python to add an attribute to 'str' or 'int'!!)
    So the work-around is to store in BaseCommon_Parser a list of sub-parsers for the attributes having builtin types.
    See member _leafParsers in BaseCommon_Parser.
    
    Main method here is InstanciateFromBuiltin() generating those parsers.
    """
    def readFromTokensBuiltin(self, stream):
        """ Similar to Abstract_Parser.ReadFromTokens()
        
        For builtin types, the reading process is not a class method (compare with Abstract_Parser.ReadFromTokens()), because the parser 
        is instanciated first, and then the parsing process is invoked. """
        val = self.readFromTokensBuiltin_impl(stream)
        self._pyd_value = val
        return val

    @classmethod
    def IsBuiltin(cls, a_cls):
        """ Test whether a_cls is a builtin type that will need to be handled by one of the child of Builtin_Parser """
        from typing import Literal
        _, typs, _ = mutil.break_type(a_cls)
        return typs[0] in [str, float, int, bool, list, Literal]

    @classmethod
    def InstanciateFromBuiltin(cls, builtin_cls):
        """ From a builtin Python class, return a proper instance of a builtin parser.
        It needs to return an instance (not a class) because of constrained values (int in [2,3] for example).
        @param builtin_cls a type as defined in 'typing' module, e.g. 'str', or 'List[float]'
        @return a instance of a child of Builtin_Parser class (careful, this is in the *Parser hierarchy, not the pydantic one!)
        """
        import typing as tp
        typ_map = {str :                Chaine_Parser,
                   int:                 Int_Parser,
                   float:               Float_Parser,
                   bool:                Flag_Parser,
                   (list, float, None):          ListFloat_Parser,
                   (list, int, None):            ListInt_Parser,
                   (list, str, None):            ListChaine_Parser,
                   (list, float, 'size_is_dim'): ListFloatDim_Parser,
                   (list, int, 'size_is_dim'):   ListIntDim_Parser,
                   (list, str, 'size_is_dim'):   ListChaineDim_Parser

                   }
        # If type is 'Optional[str]', get 'str':
        _, typs, ann = mutil.break_type(builtin_cls)
        if len(typs) == 2:   key = (typs[0], typs[1], ann)
        else:                key = typs[0]
        # Retrieve parser class:
        pars_cls = typ_map.get(key)
        if pars_cls is None:
            Listobj_impl_cls = ClassFactory.GetPydClassFromName("listobj_impl")  # will always exist
            # Is it a Literal?
            if typs[0] is tp.Literal:
                # Read first constrained value to see if str or int ...
                if isinstance(typs[1], int):
                    pars = IntConstrained_Parser()
                else:
                    assert isinstance(typs[1], str), "Duh? Literal not an int, not a str??"
                    pars = ChaineConstrained_Parser()
                # Set constrains on class
                pars._allowedValues = typs[1:]
            # List of complex (non builtin) objects
            elif typs[0] is list:
                # Read the corresponding parser class from the type annotation
                pars_cls_nam = ann
                assert not ann is None, "Complex list types should be annotated!"
                assert ClassFactory.Exist(ClassFactory.ToParserName(pars_cls_nam)), f"The list parser type '{pars_cls_nam}' was not found! Should never happen."
                pars = ClassFactory.GetParserClassFromName(pars_cls_nam)()
            elif issubclass(typs[0], Listobj_impl_cls):
                pars = ClassFactory.GetParserFromPyd(typs[0])()
            else:
                raise TrustifyException(f"Builtin type '{str(builtin_cls)}' not supported!")
            return pars
        return pars_cls()

    @classmethod
    def GetAllTrustNames(cls):
        """ Override. 
        Returns all possible names (incl. synonyms) for a keyword as a nicely formatted string. 
        For builtin types, no synonyms, should never be called?
        """
        return f"<builtin-{cls.__name__}>"

    def validateValue(self, value, stream):
        """ Used when outputing tokens (toDatasetTokens) 
        Should raise if the value is not valid (like having a string instead of an int)
        """
        raise NotImplementedError

    def getFormattedType(self):
        """ @return a string describing the current type as a nicely formatted string (useful for keyword documentation)
        e.g. for IntConstrained_Parser, returns 'int in [2,3]'
        """
        raise NotImplementedError

########################################################################

class BaseCommon_Parser(Abstract_Parser):
    """ Abstract base class containing all the methods relevant to TRUST data model parsing
    logic for high level types : keywords or lists of objects (not builtin types such as strings, int, etc.)
    By far the two most important methods are:
      ReadFromTokens() - which instanciate a class from a stream of tokens (i.e. from the .data file)
      toDatasetTokens() - inherited from parent class, which does the inverse operation and serializes the content of the instance as a stream of tokens
    """
    _braces = 1           #: whether we expect braces when reading the class - see doc_TRAD2 - by default, expect braces.
    _read_type = False    #: whether to read the actual type before instanciating the class (typically for *_base or *_deriv classes) - By default we do *not* read the type
                          # In the Python code generation this attribute is overloaded when hitting _base or _deriv.
    _infoAttr = {}        #: same thing for class attributes (dictionnary indexed by main attr name)

    def __init__(self, pyd_value=None):
        Abstract_Parser.__init__(self, pyd_value=pyd_value)
        self._leafParsers = {}       #: Dictionnary holding leaf attributes (which are always builtin Python types) - key: attr name, value: a parser object

    @classmethod
    def ReadFromTokens(cls, stream, pars_cls=None):
        """ Override - see doc in base class
        By default, to read a piece of dataset we:
        - instanciate the parser
        - invoke 'readFromTokens_impl' to handle the logic specific to the keyword (see method doc)
        - register the parsed value inside the parser
        - and finally return the parsed value.
        """
        pars_cls = pars_cls or cls
        pars = pars_cls()
        pars._pyd_value = pars.readFromTokens_impl(stream)
        if not Builtin_Parser.IsBuiltin(pars._pyd_value.__class__):
            pars._pyd_value._parser = pars
        return pars._pyd_value

    def readFromTokens_impl(self, stream):
        """ Virtual method overriden in derived classes actually producing the (pydantic) value from the stream of tokens passed.
        """
        raise NotImplementedError

    @classmethod
    def GetAllTrustNames(cls):
        """ Returns all possible names (incl. synonyms) for a keyword as a nicely formatted string """
        pyd_cls = ClassFactory.GetPydFromParser(cls)
        res = [pyd_cls.__name__.lower()]
        res.extend(pyd_cls._synonyms[None])
        return "|".join(res)

    @classmethod
    def IsOptional(cls, attr_nam):
        """ Returns True if an attribute is optional """
        pyd_cls = ClassFactory.GetPydFromParser(cls)
        attr_typ = pyd_cls.model_fields[attr_nam].annotation
        opt, _, _ = mutil.break_type(attr_typ)
        return opt

    @classmethod
    def GetInfoAttr(cls, attr_nam):
        """ Scan cls._infoAttr dictionary and look for 'attr_nam' key. If the attribute is not found in cls, 
        try base classes since the attribute might have been inherited. """
        if attr_nam in cls._infoAttr:
            return cls._infoAttr[attr_nam]
        # Look in base class:
        base_cls = get_single_parser_base(cls)
        if base_cls is Abstract_Parser:
            raise Exception("Should not happen, missing infoAttr?")
        return base_cls.GetInfoAttr(attr_nam)

class ConstrainBase_Parser(BaseCommon_Parser):
    """ Class representing any complex type/keyword having attributes.
    """
    _attributeList = None
    """ List of all attributes of the class (included inherited ones) - see _GetAttributeList() """
    _attributeSynos = None
    """ All attribute synonyms (also incl. inherited ones) - see _InvertAttrSyno() - a dictionary giving for a synonym (str) a
    couple (original attribute name, type) """

    def __init__(self, pyd_value=None):
        BaseCommon_Parser.__init__(self, pyd_value)
        opbr  = TRUSTTokens(low=["{"], orig=[" {\n"])
        clobr = TRUSTTokens(low=["}"], orig=[" \n}\n"])
        clsnam = TRUSTTokens(low=[], orig=[" " + ClassFactory.GetPydFromParser(self.__class__).__name__])
        self._tokens = {"{": opbr,           #: For a ConstrainBase we might need opening and closing brace. By default those
                        "}": clobr,          # are simple '{' and '}' followed by a line return. If ReadFromTokens() was
                        "cls_nam": clsnam }  # invoked to build the object this will respect the initial input (with potentially more spaces)
        self._attrInOrder = []               #: Just to save the order in which the attributes were read ...

    @classmethod
    def _GetAttributeList(cls):
        """ Return all the attributes of a keyword, even the inherited ones, in the correct order:
        - child attributes
        - then inherited attributes 
        (yes, this is not very logical ...)
        @return a list of tuples (str, type)
        """
        if cls._attributeList is None:
            cls._attributeList = []
            pyd_cls = ClassFactory.GetPydFromParser(cls)
            # Use the _synonyms attribute which was properly polulated by trad2_pydantic to list **all** attributes (even inherited ones)
            # in the correct order:
            for attr_nam in pyd_cls._synonyms:
                if not attr_nam is None:
                    fld_nfo = pyd_cls.model_fields[attr_nam]
                    # Use rebuild_annotation to get full original type annotation.
                    # Otherwise Annotated[List[str], 0] becomes List[str], and 0 is stored in metatdata of the field ...
                    cls._attributeList.append((attr_nam, fld_nfo.rebuild_annotation()))
        return cls._attributeList

    @classmethod
    def _InvertAttrSyno(cls):
        """ Private. Invert all attributes synonyms - returns a dict giving for each synonym (incl. the original attribute name itself)
        the real underlying attribute name and the corresponding class.
        """
        def append_synos(all_syno, one_cls):
            """ Helper method - add a bunch of synonyms to a global dict.
            Precedence is given to child class so if attr/syno is already there, do not override """
            pyd_cls = ClassFactory.GetPydFromParser(one_cls)
            for attr_nam in pyd_cls._synonyms:  # _synonyms will contain only non-inherited members ...
                if attr_nam is None or attr_nam in all_syno:  # class synonym or attribute already covered previously
                    continue
                attr_cls = pyd_cls.model_fields[attr_nam].rebuild_annotation()
                all_syno[attr_nam] = (attr_nam, attr_cls)
                for syno in pyd_cls._synonyms.get(attr_nam, []):
                    all_syno[syno] = (attr_nam, attr_cls)

        if cls._attributeSynos is None:  # was not already computed
            cls._attributeSynos = {}
            one_cls = cls
            while one_cls is not ConstrainBase_Parser:
                append_synos(cls._attributeSynos, one_cls)
                one_cls = get_single_parser_base(one_cls)
        return cls._attributeSynos

    def _parseAndSetAttribute(self, stream, attr_nam, attr_cls, flavor=""):
        """ Private. Parse an attribute value from the stream and registers it in self._pyd_value """
        cls = self.__class__
        cls.Dbg(f"@FUNC@ attr_nam '{attr_nam}' attr_cls: '{str(attr_cls)}' {flavor}")
        if Builtin_Parser.IsBuiltin(attr_cls):
            # Retrieve correct builtin parser class, for example a Chaine_Parser:
            pars_attr = Builtin_Parser.InstanciateFromBuiltin(attr_cls)
            # Temporarily load debug instruction from current class on the child attribute type.
            # This is useful for builtin types which are actually not defined anywhere in the
            # TRAD2 or in the C++ (like ints, floats, strings, etc...). Thus we can for example display an error message
            # saying that '3.5' is not an 'int', and provide the actual TRAD2 entry for the attribute itself, not for 'int' ....
            pars_attr.__class__._infoMain = cls.GetInfoAttr(attr_nam)
            # Parse builtin attribute
            val_attr = pars_attr.readFromTokensBuiltin(stream)
            self._leafParsers[attr_nam] = pars_attr
            # Reset debug info:
            pars_attr.__class__._infoMain = []
        else:
            # Strip Optional (if any) and get Parser class:
            attr_cls = mutil.strip_optional(attr_cls)
            pars_attr_cls = ClassFactory.GetParserFromPyd(attr_cls)
            # Parse child attribute:
            val_attr = pars_attr_cls.ReadFromTokens(stream)

        # Set the attribute value
        cls.Dbg(f"@FUNC@ setting {flavor} attribute '{attr_nam}' with val '{str(val_attr)}'")
        setattr(self._pyd_value, attr_nam, val_attr)

    def handleUnexpectedAttribute(self, stream, tok, nams):
        """ What should happen when we encounter an unexpected attribute.
        This is overriden in some very weird cases, like Front Tracking where we can 
        have in the datasets blocks/attributes which are directly named with an equation label.  
        By default, raise an error.
        """
        err = self.GenErr(stream, f"Unexpected attribute '{tok}' in keyword '{nams}'")
        raise TrustifyException(err) from None

    def _readFromTokens_braces(self, stream):
        """ Read from a stream of tokens using the key/value syntax of TRUST keyword, i.e. keywords
        using opening and closing braces.
        """
        cls = self.__class__

        nams = cls.GetAllTrustNames()
        invert_attr_syno = cls._InvertAttrSyno()

        # Parse opening brace
        cls.ConsumeBrace(stream, "{")
        self._tokens["{"] = stream.lastReadTokens()

        # Identify mandatory attributes:
        self._attr_ok = {}
        for attr_nam, _ in cls._GetAttributeList():
            if not cls.IsOptional(attr_nam):
                self._attr_ok[attr_nam] = False  # False="was not read yet"
        tok = stream.probeNextLow()
        while tok != "}":
            stream.validateNext()
            if tok in invert_attr_syno:
                attr_nam, attr_cls = invert_attr_syno[tok]
                self._tokens[attr_nam] = stream.lastReadTokens()
                self._attrInOrder.append(attr_nam)
                self._parseAndSetAttribute(stream, attr_nam, attr_cls)
                if attr_nam in self._attr_ok:
                    self._attr_ok[attr_nam] = True
            else:
                self.handleUnexpectedAttribute(stream, tok, nams)
            tok = stream.probeNextLow()
        # Have we parsed all mandatory attributes?
        for k, v in self._attr_ok.items():
            if not v:
                err = cls.GenErr(stream, f"Attribute '{k}' is mandatory for keyword '{nams}' and was not read", attr=k)
                raise TrustifyException(err) from None

        # Parse closing brace
        cls.ConsumeBrace(stream, "}")
        self._tokens["}"] = stream.lastReadTokens()

    def _readFromTokens_no_braces(self, stream):
        """ Read from a stream of tokens, in the order dictated by cls.model_fields
        Used for TRUST keywords which are **not** expecting opening/closing braces. 
        This is **painful** ...
        """
        from trustify.trust_parser import TRUSTEndOfStreamException
        cls = self.__class__

        invert_attr_syno, ca, nams = cls._InvertAttrSyno(), cls._GetAttributeList(), cls.GetAllTrustNames()
        cls.Dbg(f"@FUNC@   attribute list is '{ca}'")
        attr_idx = 0
        while attr_idx < len(ca):
            # On a given token, try as much as possible to match it against the attribute list:
            #   - try all consecutive optional attributes first
            #   - otherwise, if we are on a mandatory attribute, should match directly
            # If this fails, the token stream is ill formed, so raise.
            opt_match = False
            while attr_idx < len(ca) and cls.IsOptional(ca[attr_idx][0]):
                attr_nam, attr_cls = ca[attr_idx]
                cls.Dbg(f"@FUNC@ cur_attr '{attr_nam}' from class '{attr_cls.__class__}'")
                stream.save("BEFORE_OPT")   # Save the stream state at this point in case we need to rewind for mis-parsed attribute
                try:
                    self._parseAndSetAttribute(stream, attr_nam, attr_cls, "(optional)")
                    opt_match = True # OK, optional attribute match!
                    attr_idx += 1
                    break
                except (TrustifyException, TRUSTEndOfStreamException) as e:
                    # Could not set optional attribute - no problem, it was ... optional :-) Just reset the stream.
                    cls.Dbg(f"@FUNC@ optional attr '{attr_nam}' failed")
                    stream.restore("BEFORE_OPT")
                attr_idx += 1
            if attr_idx < len(ca) and not opt_match:
                # No optional match - must match current mandatory attr
                attr_nam, attr_cls = ca[attr_idx]
                self._parseAndSetAttribute(stream, attr_nam, attr_cls, "(mandatory)")
                attr_idx += 1

        # Have we parsed everything?
        if attr_idx < len(ca):
            # Only optional attr should remain:
            for attr_nam, _ in ca[attr_idx:]:
                if not cls.IsOptional(attr_nam):
                    # But actually most of the time (always?) a TRUSTEndOfStreamException will be raised before:
                    err = cls.GenErr(stream, f"Keyword '{nams}' - mandatory attribute '{attr_nam}' missing or ill-formed", attr=attr_nam)
                    raise TrustifyException(err)

    @classmethod
    def _ReadClassName(cls, stream):
        """ Read the class name.
        This method is invoked by all the keywords where the actual class name should be read (_read_type = True). For example all keyword accepting as an
        attribute a field_base, might receive a champ_uniforme, or a champ_fonc_med, etc.. So type must actually be read.
        """
        # Parse one token to identify real class being read:
        kw = Chaine_Parser.ParseOneWord(stream)

        ###
        ###   HACK HACK
        ###
        # From here, we might have a lot of fun :-) Some synonyms are duplicated.
        # For example 'moyenne' might be a 'stat_post_moyenne' or a 'champ_post_statistiques_base' ... we need context ...
        # So give a preference to the class inheriting the current 'cls' (which is given by the TRAD2)
        for r in cls.GetAllClassesFromSyno(kw, stream):  # GetAllClassesFromSyno will also check if valid TRUST keyword
            me_as_pyd = ClassFactory.GetPydFromParser(cls)
            if issubclass(r, me_as_pyd):
                return r.__name__
        # See comment above when testing key in _SYNO_ORIG_NAME:
        err = cls.GenErr(stream, f"Invalid TRUST keyword: '{kw}' (or maybe, did you forget an attribute on the previous line?)")
        raise TrustifyException(err)

    @classmethod
    def ReadFromTokens(cls, stream, pars_cls=None):
        """ Override. See BaseCommon_Parser.  """
        pars_cls = pars_cls or cls
        if pars_cls._read_type:
            cls_nam = pars_cls._ReadClassName(stream)
            cls.Dbg(f"@FUNC@ parsed type: '{cls_nam}'")
            tok = stream.lastReadTokens()
            ze_pars_cls = ClassFactory.GetParserClassFromName(cls_nam)
            assert ze_pars_cls is not None   # already checked in _ReadClassName()
        else:
            ze_pars_cls = pars_cls
            cls.Dbg(f"@FUNC@ NOT parsing type (forced type: '{cls.__name__}')")
        # Instanciate the resulting pydantic value
        # WARNING: here we might instanciate a Uniform_field pydantic object,
        # whereas being still in the Field_base_Parser parsing logic ...
        val_cls = ClassFactory.GetPydFromParser(ze_pars_cls)  # from 'pars_cls', not from 'cls'
        val = val_cls()
        # Instanciate the parser, and link the other way around (the parser registered in the pydantic value ...)
        pars = ze_pars_cls(val)
        val._parser = pars
        if pars_cls._read_type:
            # NB: not 'if ze_pars_cls._read_type' - we want the exact same condition as at the begining of the method
            pars._tokens["cls_nam"] = tok
            cls.Dbg(f"@FUNC@ Class tokens are: {pars._tokens['cls_nam'].orig()} - pars is '{pars}'")
        # The two reading modes are really different:
        if ze_pars_cls.WithBraces():
            # Here key/value pairs, order doesn't matter
            cls.Dbg(f"@FUNC@ reading '{ze_pars_cls.__name__}' with braces")
            pars._readFromTokens_braces(stream)
        else:
            # Here order matters a lot
            cls.Dbg(f"@FUNC@ reading WITHOUT braces (class '{ze_pars_cls.__name__}')")
            pars._readFromTokens_no_braces(stream)
        return val

    def toDatasetTokens(self):
        """ Override. """
        if self.WithBraces():
            return self._toDatasetTokens_braces()
        else:
            return self._toDatasetTokens_no_braces()

    def _getClsTokens(self):
        """ Private. Return output tokens for the class name (i.e. the TRUST keyword itself)
        """
        cls = self.__class__
        if self._read_type:
            pyd_cls = ClassFactory.GetPydFromParser(cls)
            expec = [pyd_cls.__name__] + pyd_cls._synonyms[None]
            for e in expec:
                if self.checkToken("cls_nam", e.lower()):
                    return self._tokens["cls_nam"].orig()
            return [" " + pyd_cls.__name__.lower()]
        return []

    def _extendWithAttrTokens(self, tok_lst, attr_nam, attr_val):
        """ Complete the output tokens with the content coming from a specific attribute. This is where
        we potentially use back what is stored inside self._leafParsers if the attribute is a builtin (like an int or a str) """
        attr_cls = attr_val.__class__
        if Builtin_Parser.IsBuiltin(attr_cls):
            # Is this a new attr?
            if not attr_nam in self._leafParsers:
                # Retrieve initial type for this attribute (do not use attr_val.__class__ which might be a simple
                # Python builtin type like 'list' instead of List[float] for example!!
                att_dict = dict(self.__class__._GetAttributeList())
                attr_cls = att_dict[attr_nam]
                pars = Builtin_Parser.InstanciateFromBuiltin(attr_cls)
                pars._pyd_value = attr_val
                self._leafParsers[attr_nam] = pars
            attr_pars = self._leafParsers[attr_nam]
        else:
            # Is this a new attribute?
            attr_pars = attr_val._parser
            if attr_pars is None:
                attr_pars = ClassFactory.GetParserFromPyd(attr_val.__class__)()
        attr_pars._pyd_value = attr_val      # IMPORTANT: update internal value, it might have been changed
        tok_lst.extend(attr_pars.toDatasetTokens())

    def _toDatasetTokens_braces(self):
        """ Method used to output a keyword with braces and names key/value pairs """
        s = []
        pyd_val = self._pyd_value
        # class name
        s.extend(self._getClsTokens())

        # Opening brace
        s.extend(self.getBraceTokens('{'))

        # Check whether some other (new) attributes were added - if so put, them at the end:
        extra_attr = []
        for attr_nam, _ in self._GetAttributeList():
            attr_val = getattr(pyd_val, attr_nam)
            if attr_val is not None and attr_nam not in self._attrInOrder:
                extra_attr.append(attr_nam)

        self.Dbg(f"@FUNC@ Internal leafsAttr is {self._leafParsers}")
        for attr_nam in self._attrInOrder + extra_attr:
            # Was attribute deleted in this instance?
            attr_val = getattr(pyd_val, attr_nam)
            if attr_val is None:
                continue
            # When braces are there, we need the attribute name explicitly.
            all_n = [attr_nam] + pyd_val.__class__._synonyms.get(attr_nam,[])
            # WARNING: special case - flags need to be completely removed if False (we don't even want the attribute name .. which is the flag)
            if attr_val.__class__ is bool and not(attr_val):
                continue
            # Check whether there is a registered token for the actual attribute name
            #  (we don't need to check the validity of the attribute token since this can't be changed - only value can be changed)
            # Otherwise build a new string for this (newly added) attribute:
            if attr_nam in self._tokens:  tok = self._tokens[attr_nam].orig()
            else:                         tok = ["\n" + attr_nam + " "]
            s.extend(tok)
            # Append attribute value itself:
            self._extendWithAttrTokens(s, attr_nam, attr_val)
        # Closing brace
        br = self.getBraceTokens('}')
        s.extend(br)
        return s

    def _toDatasetTokens_no_braces(self):
        """ Method used to output a keyword without braces """
        s = []
        s.extend(self._getClsTokens())  # Class name
        for attr_nam, _ in self._GetAttributeList():
            attr_val = getattr(self._pyd_value, attr_nam)
            if attr_val is not None:
                self._extendWithAttrTokens(s, attr_nam, attr_val)
        return s

######################################################
## List-like types
######################################################
class ListOfBase_Parser(Builtin_Parser):
    """ Root class for all list-like objects. For example a list of float, or a list of 'Source'.
    This is a child of Builtin_Parser because on the pydantic side, such an attribute will be annotated as a standard 
    Python list 'List[XXX]' 
    """
    _comma = 1             #:  1: with comma to separate items, 0: without, -1: like parent
    _itemType = None       #: The (single) pydantic (or builtin) type of the items in the list - overriden in derived classes
    _itemParserType = None #: The associated Parser class - will be set by readFromTokensBuiltin_impl

    def __init__(self, pyd_value=None):
        Builtin_Parser.__init__(self, pyd_value)
        self._outputItemType = False
        """ Should we append type in front of a list item we are going to output?
        By default always false, since the object itself outputs its type when needed.
        But see tricky exception in hacks.py for list of equations ..."""

    @classmethod
    def _MustReadSize(cls):
        """ Private. Whether we should read the list size.
        At present, the rule is simple: when there are no braces, we must read a size.
        """
        return not cls.WithBraces()

    @classmethod
    def _WithComma(cls):
        """ Private.
        Tell whether list items are separated by a comma.
        -1: like the parent class
        0: no comma to separate items
        1: with comma
        """
        mp = {0: False, 1: True}
        if cls._comma in [0, 1]:
            return mp[cls._comma]
        # Need to look at the parent
        for c in cls.__bases__:
            if c.__name__.endswith(ClassFactory._PARSER_SUFFIX):
                return c._WithComma()
        raise Exception("Internal error")

    @classmethod
    def _ConsumeComma(cls, stream):
        """ Private. Consume a single comma ',' for list. """
        nams = cls.GetAllTrustNames()
        tok = stream.probeNextLow()
        if tok != ',':
            err = cls.GenErr(stream, f"Keyword '{nams}' expected a comma (',') to separate list items")
            raise TrustifyException(err)
        stream.validateNext()

    def getFormattedType(self):
        """ Override """
        it = self._itemType.__name__
        return f"list of {it}"

    def readListSize(self, stream):
        """ Return size of the list - overriden in ListOfFloatDim """
        t = stream.probeNextLow()
        try:
            n = int(t)
        except:
            err = self.GenErr(stream, f"Invalid number of elements in list: '{t}', expected an integer")
            raise TrustifyException(err)
        stream.validateNext()
        if n < 0:
            err = self.GenErr(stream, f"Invalid number of elements in list: '{t}', expected a positive integer")
            raise TrustifyException(err)
        return n

    def initItemParserType(self):
        """ Initialize the type of parser that will be used to parse items in the list. 
        This is overriden for list of builtins.
        """
        # Class level variable - so setting it once, at the first time a list containing this
        # type of objects is encountered is enough:
        if self.__class__._itemParserType is None:
            self.__class__._itemParserType = ClassFactory.GetParserFromPyd(self._itemType)

    def parseAndAppendItem(self, stream, lst):
        """ Append an item to the current list. Overriden in ListOfBuiltin_Parser """
        item_val = self._itemParserType.ReadFromTokens(stream)
        lst.append(item_val)

    def readFromTokensBuiltin_impl(self, stream):
        """ Override. Read N objects from the input. N (an integer) must be the first token found.
        This override only works for single-type list.
        """
        cls = self.__class__
        cls.Dbg(f"@FUNC@ about to read list ...")
        val = []

        self.initItemParserType()

        if cls._MustReadSize():
            cls.Dbg(f"@FUNC@ reading list size ...")
            n = self.readListSize(stream)
            self._tokens["len"] = stream.lastReadTokens()
        elif not cls.WithBraces():
            err = cls.GenErr(stream, f"Internal error: Can not read list with no brace and no size info.")
            raise Exception(err)

        if cls.WithBraces():
            withComma = cls._WithComma()
            cls.Dbg(f"@FUNC@ reading list with braces, and comma flag={withComma} ...")
            cls.ConsumeBrace(stream, "{")
            self._tokens["{"] = stream.lastReadTokens()
            tok = stream.probeNextLow()
            i = 0
            while tok != '}':
                self.parseAndAppendItem(stream, val)
                tok = stream.probeNextLow()
                if withComma and tok != '}':
                    cls._ConsumeComma(stream)
                    # Yes, the ',' token might be different after each element (comments etc...):
                    # Build a key containing the index of the comma to discriminate between the various commas:
                    self._tokens[",%d" % i] = stream.lastReadTokens()
                    tok = stream.probeNextLow()
                i += 1
            cls.ConsumeBrace(stream, "}")
            self._tokens["}"] = stream.lastReadTokens()
        else:  # no braces but we should know somehow how many objects to be read:
            assert n >= 0
            cls.Dbg(f"@FUNC@ reading list **without** braces (exp size is {n}) ...")
            for cnt in range(n):
                self.parseAndAppendItem(stream, val)

        return val

    def extendWithSize(self, ret):
        """ Append the size of the list to the list of output tokens.
        Overriden in ListXXXXDim_Parser classes where the size is fixed.
        """
        len_as_str = str(len(self._pyd_value))
        if self.checkToken("len", len_as_str):
            ln = self._tokens["len"].orig()
        else:
            ln = [" " + len_as_str]
        ret.extend(ln)

    def getItemTokens(self, index, elem_pyd):
        """ @return tokens for an item of the list, possibly dealing with the case where item has no associated parser. """
        pars = elem_pyd._parser
        if pars is None:
            pars_cls = ClassFactory.GetParserFromPyd(elem_pyd.__class__)
            pars = pars_cls(elem_pyd)
            elem_pyd._parser = pars
        else:
            pars_cls = pars.__class__
        # Important - updated pyd_value associated with the parser with the last value from the actual Pydantic object:
        pars._pyd_value = elem_pyd
        s = []
        if self._outputItemType:
            toks = pars._tokens["cls_nam"].orig()
            s.extend(toks)
            self.Dbg(f"@FUNC@  outputing list item {pars} -> will output type with toks {toks}")
        s.extend(pars.toDatasetTokens())
        return s

    def toDatasetTokens(self):
        """ Override. See doc in mother class. """
        ret = []
        dft_com = TRUSTTokens(orig=[" ,\n"])
        if self._MustReadSize():  # if size must be read, it must be written out too ...
            dim = Dimension_Parser._DIMENSION
            if isinstance(self, AbstractSizeIsDim) and len(self._pyd_value) != dim:
                raise TrustifyException(f"List '{self._pyd_value}' has a number of items which do not match problem dimension (={dim})!")
            self.extendWithSize(ret)
        if self.WithBraces():
            br = self.getBraceTokens("{")
            ret.extend(br)
        for i, v in enumerate(self._pyd_value):
            toks = self.getItemTokens(i, v)
            ret.extend(toks)
            if self._WithComma() and i != len(self._pyd_value)-1:
                # yes, the ',' token might be different after each element (comments etc...):
                com = self._tokens.get(",%d" % i, dft_com)
                ret.extend(com.orig())
        if self.WithBraces():
            br = self.getBraceTokens("}")
            ret.extend(br)
        return ret

class ListOfBuiltin_Parser(ListOfBase_Parser):
    """ Base class for all lists with items of builtin types (like list of floats, but not list of Source) 
    For those lists we need to save the parser objects too since they can not be attached to the values themselves.
    """
    _braces = 0        #: No braces for those simple lists
    _comma = 0         #: No comma either

    def __init__(self, pyd_value=None):
        ListOfBase_Parser.__init__(self, pyd_value)
        self._builtin_parsers = []   # The leaf parsers for each item in the list if those items are builtins.
                                     # Similar to the member _leafAttr in ConstrainBase_Parser

    def initItemParserType(self):
        """ Do nothing here - for list of builtin types, a specific parser
        will be instanciated. See parseAndAppendItem() below. """
        pass

    def parseAndAppendItem(self, stream, lst):
        """ Override. For builtin types, we need to store the parsers for each item too. """
        item_pars = Builtin_Parser.InstanciateFromBuiltin(self._itemType)

        # Attach debug information of the list itself to the (builtin) item type
        # For a list of int (or float, etc.), in case of error, this will return the point in the C++ source where the attribute is defined:
        item_pars.__class__._infoMain = self._infoMain

        item_val = item_pars.readFromTokensBuiltin(stream)

        # Reset debug info:
        item_pars.__class__._infoMain = []

        self._builtin_parsers.append(item_pars)  # for builtin type, save parser too
        lst.append(item_val)

    def getItemTokens(self, idx, pyd_val):
        """ Override - for a list of builtin we use the internal dictionnary of builtin parsers since the value
        itself (= a builting type like str or int) can not have a parser attached. """
        # If a new item is in the list:
        if idx >= len(self._builtin_parsers):
            item_pars = Builtin_Parser.InstanciateFromBuiltin(self._itemType)
            self._builtin_parsers.append(item_pars)
        # !!Important!! Update builtin value stored in the item parser itself, to synchronize
        # with the actual value of the list item as stored in self.__builtin_value.
        self._builtin_parsers[idx]._pyd_value = pyd_val
        return self._builtin_parsers[idx].toDatasetTokens()

class AbstractSizeIsDim(object):
    """ Shared methods for all ListOfXXXDim_Parser classes - implements the necessary stuff to force
    the size of the list to be the dimension read at the top level of the dataset when reading the
    'dimension' keyword
    """
    def readListSize(self, stream):
        """ Size is fixed and does not need to be provided """
        if Dimension_Parser._DIMENSION not in [2, 3]:
            raise TrustifyException("Invalid dataset: It seems the dimension has not been defined")
        return Dimension_Parser._DIMENSION

    def extendWithSize(self, ret):
        """ Override. Do nothing. Here the size is the dimension and does not need to be output """
        pass

class ListChaine_Parser(ListOfBuiltin_Parser):
    """ List of strings (with no blanks), in the form 'N val1 val2 ...'  """
    _itemType = str

class ListChaineDim_Parser(ListChaine_Parser, AbstractSizeIsDim):
    """
    List of strings, in the form 'val1 val2 ...'
    The number of expected strings is given by the dimension of the problem.
    """
    readListSize = AbstractSizeIsDim.readListSize
    extendWithSize = AbstractSizeIsDim.extendWithSize

class ListFloat_Parser(ListOfBuiltin_Parser):
    """ List of floats, in the form 'N val1 val2 ...'  """
    _itemType = float

class ListFloatDim_Parser(ListFloat_Parser, AbstractSizeIsDim):
    """
    List of floats, in the form 'val1 val2 ...'
    The number of expected floats is given by the dimension of the problem.
    """
    readListSize = AbstractSizeIsDim.readListSize
    extendWithSize = AbstractSizeIsDim.extendWithSize

class ListInt_Parser(ListOfBuiltin_Parser):
    """ List of ints, in the form 'N val1 val2 ...'  """
    _itemType = int

class ListIntDim_Parser(ListInt_Parser, AbstractSizeIsDim):
    """
    List of ints, in the form 'N val1 val2 ...'
    The number of expected ints is given by the dimension of the problem.
    """
    readListSize = AbstractSizeIsDim.readListSize
    extendWithSize = AbstractSizeIsDim.extendWithSize

######################################################
# Class Interprete
######################################################
class Interprete_Parser(ConstrainBase_Parser):
    """ Class 'Interprete_Parser' has nothing special in itself, except that it needs
    to be known early, so that the Read_Parser class can be defined.
    So force its definition here, and hence avoid its automatic generation from the
    TRAD2 file.
    """
    _braces = 0   #: By default, 'interprete' does not need braces

class Read_Parser(Interprete_Parser):
    """ Class to parse the 'read' keyword.
    Its main specificity is to turn off the type reading of the object that will be read, since the type was provided in the forward declaration. 
    """
    _braces = 0       #: The 'Read' keyword behaves like an object expecting no braces: "read <identifier> <obj>" where 'identifier' and 'obj' are two mandatory attr
    _read_type = True
    _ze_type = None   #: The actual (parser) class that the 'read' instruction will parse. This is set in Dataset_Parser._Handle_Read_instruction()
    _infoMain = ["<trustify builtin>", -1]
    _infoAttr = {"identifier": ["<trustify builtin>", -1],
                 "obj": ["<trustify builtin>", -1]}

    @classmethod
    def ReadFromTokens(cls, stream):
        """ Revert to default behavior from BaseCommon_Parser. """
        return BaseCommon_Parser.ReadFromTokens(stream, pars_cls=cls)

    def readFromTokens_impl(self, stream):
        """ Override - see base doc.
        Parse the identifier and the full object passed to the 'read' keyword.
        Making sure that the identifier is valid is checked before arriving here, in Dataset_Parser.
        """
        cls = self.__class__
        val = ClassFactory.GetPydFromParser(cls)()

        # Parse 'read' itself (we know for sure it is 'read' already, just save tokens)
        Chaine_Parser.ParseOneWord(stream)
        self._tokens["read"] = stream.lastReadTokens()

        # Parse identifier
        ident = Chaine_Parser.ParseOneWord(stream)
        val.identifier = ident
        self._tokens["identifier"] = stream.lastReadTokens()

        # Parse object itself
        # WARNING! Here we must skip class reading, so temporarily switch this off:
        old_read_type = cls._ze_type._read_type
        cls._ze_type._read_type = False
        val.obj = cls._ze_type.ReadFromTokens(stream)
        cls._ze_type._read_type = old_read_type

        return val

    def toDatasetTokens(self):
        """ Override - see base doc """
        s = []

        # Output 'read' keyword
        if 'read' in self._tokens:
            s.extend(self._tokens["read"].orig())
        else:
            s.extend([" read"])

        # Output identifier
        identif = self._pyd_value.identifier
        if self.checkToken("identifier", identif):
            s.extend(self._tokens["identifier"].orig())
        else:
            s.extend([" " + identif])

        # Output object attribute of the 'read' keyword
        tok_obj_pars = self._pyd_value.obj._parser
        if tok_obj_pars is None:
            tok_obj_pars = ClassFactory.GetParserFromPyd(self._pyd_value.obj.__class__)()
        tok_obj_pars._pyd_value = self._pyd_value.obj
        # WARNING! Here we must skip class reading, so temporarily switch this off (similar to what is done in ReadFromTokens())
        old_read_type = tok_obj_pars.__class__._read_type
        tok_obj_pars.__class__._read_type = False
        tok_obj = tok_obj_pars.toDatasetTokens()
        tok_obj_pars.__class__._read_type = old_read_type
        s.extend(tok_obj)

        return s


######################################################
# Class Dimension
######################################################
class Dimension_Parser(Interprete_Parser):
    """ Dimension is very special - we store it as a static variable at the top level of the class hierarchy
        once the 'dimension' keyword has been read.
        It is used to correctly size some lists of coordinates when defining probes for example.
        See class base.AbstractSizeIsDim and derived.
    """
    _infoMain = ['<Dimension>', -1]
    _infoAttr = {'dim': ['<builtin>', -1]}
    _DIMENSION = -1    #: The unique dimension for a TRUST dataset as defined at the begining

    @classmethod
    def ReadFromTokens(cls, stream):
        """ Override - see class doc """
        val = Interprete_Parser.ReadFromTokens(stream, pars_cls=Dimension_Parser)
        Dimension_Parser._DIMENSION = val.dim
        cls.Dbg(f"@FUNC@ Dimension was set globally to {val.dim}")
        new_defbord_nam = f"Defbord_{val.dim}"
        # In some elementary tests (test_rw_elementary.test_dim_pars) we do not have Defbord classes yet:
        try:
            new_defbord_cls = ClassFactory.GetPydClassFromName(new_defbord_nam)
            defs_bords = [ClassFactory.GetPydClassFromName(n) for n in ["Defbord", "Defbord_2", "Defbord_3"]]
        except:
            new_defbord_cls = None
            defs_bords = []
        # Dynamically change all attribute types using this:
        for c in ClassFactory.GetAllConstrainBasePyd():
            for k, fld_nfo in c.model_fields.items():
                typ_attr = fld_nfo.annotation
                # Inspect all possibilities since this loop might already have been called (notably in tests where we
                # do several parsing with different datasets of different dimensions in one go ...)
                if typ_attr in defs_bords:
                    c.model_fields[k].annotation = new_defbord_cls
                    # Also clear the cached information on the parser side!
                    pars_cls = ClassFactory.GetParserFromPyd(c)
                    pars_cls._attributeList = None
                    pars_cls._attributeSyno = None
        return val

    def toDatasetTokens(self):
        """ Some tests have the bad habits of playing with the Dimension keyword.
        They switch several times in the same dataset, see for example Kernel_Extrusion_en20.data
        When outputing the dataset list lengths is checked, so if we are outputing a list of length 2 (read when 'dimension 2' was
        active), and then read 'dimension 3', the check will fail. 
        When outputing we need to tweak dimension too. 
        """
        Dimension_Parser._DIMENSION = self._pyd_value.dim
        self.Dbg(f"@FUNC@ Dimension was set globally to {Dimension_Parser._DIMENSION}")
        return Interprete_Parser.toDatasetTokens(self)

######################################################
# Main classes to parse the dataset and forward declarations
######################################################

class Declaration_Parser(ConstrainBase_Parser):
    """ Class parsing a simple forward declaration in the dataset like
    'Pb_conduction pb'.
    """
    _read_type = True     # Always since the type ... is the main information here!
    _infoMain = ["<trustify builtin>", -1]
    _infoAttr = {"identifier": ["<trustify builtin>", -1],
                 "ze_type": ["<trustify builtin>", -1]}

    def __init__(self, pyd_value=None):
        ConstrainBase_Parser.__init__(self, pyd_value)
        self._pars_cls = None  #: The actual type that will be read after the identifier. This is set by Dataset_Parser

    @classmethod
    def ReadFromTokens(cls, stream):
        """ Revert to default behavior from BaseCommon_Parser. """
        return BaseCommon_Parser.ReadFromTokens(stream, pars_cls=cls)

    def readFromTokens_impl(self, stream):
        """ Override - see base class documentation """
        decl = ClassFactory.GetPydFromParser(self.__class__)()  # a Declaration object
        # Check whether the class (=1st param of the declaration) is valid and get corresponding parser class
        kw = Chaine_Parser.ParseOneWord(stream)
        root_cls = self.GetOneClassFromSyno(kw, stream, interp=False)   # will check valid TRUST keyword
        decl.ze_type = root_cls
        self._pars_cls = ClassFactory.GetParserFromPyd(root_cls)
        self._tokens["cls_nam"] = stream.lastReadTokens()
        ident = Chaine_Parser.ParseOneWord(stream)
        # Basic checks on the identifier:
        if ident.startswith("{") or ident.startswith("}") or ident[0].isnumeric():
            err = self.GenErr(stream, f"Invalid identifier '{ident}' in forward declaration of type '{kw}'")
            raise TrustifyException(err)
        decl.identifier = ident
        self._tokens["identifier"] = stream.lastReadTokens()
        return decl

    def toDatasetTokens(self):
        """ Override - see BaseCommon_Parser """
        s = []
        ze_type = self._pyd_value.ze_type
        # List of all possible names for the type stored in the Declaration:
        ok = [ze_type.__name__.lower()] + ze_type._synonyms[None] # Key 'None' has all the synonyms for the keyword itself (not its attribute)
        # Is the current stored token one of them?
        tok_merged = ''.join(self._tokens.get("cls_nam", TRUSTTokens()).low())
        if tok_merged in ok:
            cn = self._tokens["cls_nam"].orig()
        else:
            cn = [" " + ze_type.__name__]
        s.extend(cn)
        identif = self._pyd_value.identifier
        if self.checkToken("identifier", identif):
            tok_id = self._tokens["identifier"].orig()
        else:
            tok_id = [" " + identif]
        s.extend(tok_id)
        return s

class Dataset_Parser(ListOfBase_Parser):
    """ Class used to parse a complete TRUST dataset.
    A TRUST dataset is a list of 'Objet_u', with the following allowed classes:
      - Declaration, representing forward declarations like 'Pb_conduction pb'
      - or any child of ConstrainBase, representing all TRUST interprets like 'lire_med { file ... }'
      
    WARNING: contrary to other children of ListOfBase_Parser, the corresponding pydantic object (Dataset) is not directly
    a Python list. This the more complex 'Dataset' class, in which the member 'entries' is the actual list of objects in 
    the dataset. This 'Dataset' class is generated by the 'trad2_pydantic' module.
    
    We still inherit from ListOfBase_Parser here to benefit from useful methods like getItemTokens(). 
    """

    def _prepare_read_instruction(self, stream):
        """ Private. When encountering a read instruction we set the internal '_pars_cls' class attribute of Declaration_Parser
        to prepare the proper parsing of the keyword. 
        """
        stream.save("READ_INSTR")
        stream.nextLow()           # 'read' keyword itelf
        identif = stream.nextLow() # identifier, like 'pb' in 'read pb { ...'
        # Is this a valid identifier in the dataset (i.e. is there a matching forward declaration)?
        if not identif in self._pyd_value._declarations:
            err = self.GenErr(stream, f"Referencing object '{identif}' (with a 'read|lire' instruction) which was not declared before")
            raise TrustifyException(err)
        # Retrieve real underlying parser class from initial forward declaration
        decl, _ = self._pyd_value._declarations[identif]
        ze_cls = decl._parser._pars_cls
        Read_Parser._ze_type = ze_cls  # Set the class type that will be parsed by Read_Parser
        self.Dbg(f"@FUNC@  'Read' instruction: Identifier: '{identif}' - Read_Parser set to read type '{ze_cls}'")
        stream.restore("READ_INSTR")
        return identif

    def _handle_interprete(self, tok, stream, ds, pars_cls):
        """ Private. Invoke the ReadFromTokens method for a TRUST keyword triggering the parsing of the corresponding block in 
        the dataset. """
        # In a dataset the type must **always** be read - so it is just a matter to know if the keyword
        # itself reads it, or if it is the Dataset that reads it:
        if pars_cls._read_type:
            self.Dbg(f"@FUNC@ '{tok}' standard instanciation with type reading")
            inst = pars_cls.ReadFromTokens(stream)
        else:
            self.Dbg(f"@FUNC@ '{tok}' standard instanciation WITHOUT type reading")
            stream.validateNext() # consume type keyword
            cls_tok = stream.lastReadTokens()
            inst = pars_cls.ReadFromTokens(stream)
            inst._parser._tokens["cls_nam"] = cls_tok
        # Link the parser of the item with the item value itself:
        inst._parser._pyd_value = inst
        ds.entries.append(inst)

    @classmethod
    def ReadFromTokens(cls, stream):
        """ Override. See BaseCommon_Parser. Reads a complete TRUST dataset
        """
        ds = ClassFactory.GetPydClassFromName("Dataset")()  # A Dataset object (pydantic)
        pars = cls(ds)   # A Dataset_Parser object

        interp_cls = ClassFactory.GetPydClassFromName("interprete")
        while not stream.eof():
            tok = stream.probeNextLow()
            cls.Dbg(f"@FUNC@ probing tok '{tok}'")

            # 'Read' instruction
            is_read = tok in ["read", "lire"]
            if is_read:
                cls.Dbg(f"@FUNC@ 'read' keyword encountered")
                read_key = pars._prepare_read_instruction(stream)

            # Extract original Pydantic class from keyword
            root_cls = cls.GetOneClassFromSyno(tok, stream, interp=True)
            pars_entry_cls = ClassFactory.GetParserFromPyd(root_cls)

            # 'interprete' are the highest level keywords in TRUST - they perform an action. They are only allowed at top level
            # in the dataset.
            if issubclass(pars_entry_cls, Interprete_Parser):
                # Standard keyword instanciation
                pars._handle_interprete(tok, stream, ds, pars_entry_cls)
                # This was a 'read' instruction, update internal declaration mapping:
                if is_read:
                    assert read_key in ds._declarations
                    ds._declarations[read_key][1] = len(ds.entries)-1
                # Should we stop here?
                if pars_entry_cls is Fin_Parser:
                    break
            else:
                # Forward declaration
               decl = Declaration_Parser.ReadFromTokens(stream)
               cls.Dbg(f"@FUNC@ fwd decl - '{decl.identifier}' with class '{decl.ze_type.__name__}'")
               ds._declarations[decl.identifier] = [decl, -1]  # -1 because for now positionof the real instanciation in the dataset is not known
               ds.entries.append(decl)

        return ds

    def toDatasetTokens(self):
        """ Override. Here we don't need all the fuss around the class type. And we need to iterate on self._pyd_value.entries,
        (which is a Dataset instance), not self._pyd_value itself ...."""
        s = []
        for i, v in enumerate(self._pyd_value.entries):
            item_pars_cls = ClassFactory.GetParserFromPyd(v.__class__)
            # This deserves an explanation: when outputing the full dataset, we must always write out the type of the objects!
            # (whether it is a derived class of a 'xxx_base' or not)
            # Classes which are children of 'xxx_base' will already do so by themselves when their 'toDatasetTokens' is invoked ;
            # but for the other ones, we must force it:
            self._outputItemType = not item_pars_cls._read_type
            s.extend(self.getItemTokens(i, v))
        return s

######################################################
# Simple builtin types (string, int, floats, ...)
######################################################
class AbstractChaine_Parser(Builtin_Parser):
    """ Base class for all (constrained or not) strings """

    def readFromTokensBuiltin_impl(self, stream):
        """ Override. See Builtin_Parser.
        In this override simply return the next token which is a valid (non blank) string.
        """
        s = stream.nextLow()
        self.validateValue(s, stream)
        self._tokens["val"] = stream.lastReadTokens()
        return s

    def toDatasetTokens(self):
        """ Override. """
        if self.checkToken("val", self._pyd_value):
            return self._tokens["val"].orig()
        else:
            # Important! put a space to start with, otherwise the string will be stuck to the previous token!
            # (The parser include all spaces, comments, etc. before the actual token ...
            ret = [" " + str(self._pyd_value)]
            # Try as much as possible to keep comments etc ... around string value:
            if "val" in self._tokens:
                low = self._tokens["val"].low()
                ori = self._tokens["val"].orig()
                not_empty = list(filter(lambda s: s != "", low))
                if len(not_empty) == 1:
                    idx = low.index(not_empty[0])
                    ret = ori[:idx] + ret + ori[idx+1:]
            return ret

class Chaine_Parser(AbstractChaine_Parser):
    """A simple 'chaine' (string) ... but this is tricky. It might be made of several tokens if braces are found.
    This is the case for 'bloc_lecture' objects for example.
    See readFromTokens below ..
    """
    def __init__(self, pyd_value=None):
        AbstractChaine_Parser.__init__(self, pyd_value)
        self._isBloc = False     # Whether this chain is a full bloc with '{ }'

    def getFormattedType(self):
        """ Override """
        return "string"

    @classmethod
    def ParseOneWord(cls, stream):
        """ Parse a single word (=token) from the stream and returns it """
        pars = cls()
        return pars.readFromTokensBuiltin(stream)

    def readFromTokensBuiltin_impl(self, stream):
        """ Reads a 'chaine' : this can be:
        - either a single token 'toto' (in which case, just take one token)
        - or a full block, like '{ toto { tata } }' (in which case take all tokens until closed brace)
        
        WARNING: in this last case, the original version (not lower case) of the tokens are kept even
        to build the string value of the Chaine. This is because in bloc_lecture (like for PETSc), we
        need to preserve case-sensitivity.
        """
        tok = stream.probeNextLow()
        withBr = (tok == '{')
        if not withBr:
            # Easy just take one token - take its **lower-case** version
            stream.validateNext()
            lst_rd = stream.lastReadTokens()
            s = ''.join(lst_rd.low())
            if s.strip().startswith("{") or s.strip().startswith("}"):
                err = self.GenErr(stream, f"Misformatted string or block within braces -> '{s}'")
                raise TrustifyException(err)
            self._isBloc, self._tokens["val"] = False, lst_rd
        else:
            # With braces:
            l = []
            self.ConsumeBrace(stream, "{")
            l.append(stream.lastReadTokens())
            cnt_brace = 1
            while cnt_brace != 0:
                tok = stream.probeNextLow()
                if tok == '{':
                    cnt_brace += 1
                if tok == '}':
                    cnt_brace -= 1
                    if cnt_brace == 0: break  # without validating token so that it is consumed by ConsumeBrace()
                stream.validateNext()
                # !!!!!!
                # !!When reading a full block, keep the upper case version!!
                # !!!!!!
                l.append(stream.lastReadTokens())
            # Read closing brace:
            self.ConsumeBrace(stream, "}")
            l.append(stream.lastReadTokens())
            l2 = TRUSTTokens.Join(l)
            s = ''.join(l2.orig())  # orig(), not low() see comment above!
            self._isBloc, self._tokens["val"] = True, l2
        return s

    def toDatasetTokens(self):
        """ Override - see base class doc.
        See also WARNING in readFromTokensBuiltin_impl() above. """
        if self._isBloc:
            v = self._pyd_value
            if not v[0] in [" ", "\n", "\t"]:
                v = f" {v}"
            return [v]
        else:
            return AbstractChaine_Parser.toDatasetTokens(self)

class ChaineConstrained_Parser(AbstractChaine_Parser):
    """ Same as Chaine_Parser, but with constrained string values """

    def __init__(self, pyd_value=None):
        AbstractChaine_Parser.__init__(self, pyd_value)
        self._allowedValues = []    # Allowed value for this string

    def getFormattedType(self):
        """ Override """
        return f"string into {self._allowedValues}"

    def validateValue(self, val, stream):
        if val not in self._allowedValues:
            err = self.GenErr(stream, f"Invalid value: '{val}', not in allowed list: '%s'" % str(self._allowedValues))
            raise TrustifyException(err)

class Fin_Parser(Interprete_Parser):
    """ The 'end' keyword at the end of the dataset. It is an 'interprete'.
    It has its own class because the main parsing loop in Dataset needs to spot it specifically :-)
    """
    _braces = 0
    _read_type = False

class Int_Parser(Builtin_Parser):
    """ Base class for all (constrained or not) integers """

    def getFormattedType(self):
        """ Override """
        return "int"

    def validateValue(self, val, stream):
        """ To be overriden in derived class. Should raise if value is not allowed. """
        pass

    def readFromTokensBuiltin_impl(self, stream):
        """ Parse a single int """
        s = stream.nextLow()
        self.validateValue(s, stream)
        try:
            val = int(s)  # Build the actual int
        except:
            err = self.GenErr(stream, f"Invalid int value: '{s}'")
            raise TrustifyException(err) from None
        self._tokens["val"] = stream.lastReadTokens()
        return val

    def toDatasetTokens(self):
        """ Override. """
        self_as_str = str(self._pyd_value)
        if self.checkToken("val", self_as_str, int):
            ret = self._tokens["val"].orig()
        else:
            ret = [" " + self_as_str]
        return ret

class IntConstrained_Parser(Int_Parser):
    """ Integer in a constrained list """

    def __init__(self, pyd_value=None):
        Int_Parser.__init__(self, pyd_value)
        self._allowedValues = []    # Allowed value for this int

    def getFormattedType(self):
        """ Override """
        return f"int into {self._allowedValues}"

    def validateValue(self, val, stream):
        """ Override - check that this is really an int! """
        try:
            i = int(val)
        except:
            err = self.GenErr(stream, f"Invalid value: '{val}', could not be interpreted as an integer")
            raise TrustifyException(err)
        if i not in self._allowedValues:
            err = self.GenErr(stream, f"Invalid value: '{val}', not in allowed list: '%s'" % str(self._allowedValues))
            raise TrustifyException(err)

class Float_Parser(Builtin_Parser):
    def getFormattedType(self):
        """ Override """
        return "float"

    def readFromTokensBuiltin_impl(self, stream):
        """ Parse a single float """
        s = stream.nextLow()
        try:
            val = float(s)  # Build the actual float
        except:
            err = self.GenErr(stream, f"Invalid float value: '{s}'")
            raise TrustifyException(err) from None
        self._tokens["val"] = stream.lastReadTokens()
        return val

    def toDatasetTokens(self):
        """ Override. """
        self_as_str = str(self._pyd_value)  # [ABN] can't remember why I needed to do the check as str ...
        if self.checkToken("val", self_as_str, float):
            ret = self._tokens["val"].orig()
        else:
            ret = [" " + self_as_str]
        return ret

class Flag_Parser(Builtin_Parser):
    """ Boolean flags are of type 'rien' in the TRAD_2 file ... when they are present, they correspond to a True value
    when absent, False. """
    def getFormattedType(self):
        """ Override """
        return "flag"

    def readFromTokensBuiltin_impl(self, stream):
        """Override. If we call this method it means the flag has been read (as an attribute name!), 
        so it is always successful, and no further token is read."""
        return True

    def toDatasetTokens(self):
        """ Override. Nothing to output! A flag is either set in which case the attribute name will be output 
        or it is not. In either case the flag value itself is irrelevant. """
        return ['']

