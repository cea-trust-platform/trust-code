"""
The content of this file will be appended at the end of the automatically generated code.
This is the place to put all the hacks needed when some keywords do not follow the usual 
TRUST grammar.

(Generally the hacks are put in try/except clauses so that simple unit tests that do not generate 
some classes do not fail when passing over this piece of code)
"""

"""
When parsing a 'milieu_base' attribute, or an equation, the attribute name is actually the 
class name! For example, we have

    pb_thermohydraulique pb
    read pb {
      fluide_incompressible { ... }
    }

and not

    pb_thermohydraulique pb
    read pb {
      fluide_incompressible fluide_incompressible { ... }
    }

'fluide_incompressible' is a (named) attribute of 'pb_thermohydraulique', but type has been skipped!
For those specific keyword, we should instruct the parser **not** to read the type.
"""
from trustify.misc_utilities import ClassFactory
for c in ClassFactory.GetAllConstrainBaseParser():
    try:   # (try/except since they might not be defined in simple test cases)
        if issubclass(c, Milieu_base_Parser) or \
           issubclass(c, Mor_eqn_Parser) or \
           issubclass(c, Corps_postraitement_Parser) or \
           issubclass(c, Pb_base_Parser):
             # Relevant dataset examples which will fail without the hack:
             #   Milieu_base_Parser:         any dataset with a milieu!!
             #   Mor_eqn_Parser:             diffusion_implicite_jdd6
             #   Corps_postraitement_Parser: distance_paroi_jdd1.data
             #   Pb_base_Parser:             Kernel_ecrire_champ_med.data
            c._read_type = False
        # eqn_base is an exception to mor_eqn_Tru because of listeqn in Scalaires_passifs
        # -> example: WC_multi_2_3_espece.data
        Eqn_base_Parser._read_type = True
    except:
        pass

# Motivating example (scalaires_passifs): Quasi_Comp_Cond_GP_VDF_FM.data
def toDSToken_hack(self):
    """ When writing out a derived class of equation_base, its type should be written.
    Normally this is controlled by the '_read_type' class member which says that when the type
    should be read for a class, it should also be written out.
    But in this very specific case, when the class is read it is just an equation_base, but then at writing
    time, it has been typed correclty ... pfew!
    """
    self._outputItemType = True
    return ListOfBase_Parser.toDatasetTokens(self)

try:
    Listeqn_Parser.toDatasetTokens = toDSToken_hack
except:
    pass

#
# Specificities for FT problems, and all generic problems which may contain an arbitrary
# number of equations after the
# other medium/constituants attributes:
# (see for example dataset in Trio: tests/Reference/Multiphase/Front_tracking_discontinu/Front_tracking_discontinu/Rotation_IBC_3D_Reaction_M1)
#

# 1. Add an attribute 'liste_equations' in the pydantic classes to store the list of declared
# equations. We do this by substituing the current model by a new one augmented with an extra
# attribute
try:
    Eqn_base
    has_Eqn_base = True
except:
    has_Eqn_base = False
if has_Eqn_base:
    to_modif = [c
                for c in ClassFactory.GetAllConstrainBasePyd()
                if issubclass(c, Problem_read_generic)]
    for c in to_modif:
        c._synonyms['liste_equations'] = []
        # Update global scope, and class factory
        c_new = c.with_fields(liste_equations=(List[Eqn_base], []))
        globals()[c.__name__] = c_new
        ClassFactory._ALL_CLASSES[c.__name__] = c_new
    del to_modif
del has_Eqn_base

# 2. Override the behavior happening when we encounter an unknown attribute to see if this
# actually corresponds to one of the equation added. If so add this equation to the hidden list.
def handleUnexpectedAttribute_pb_generic(self, stream, tok, nams):
    # Check provided name is in the list of 'solved_equations'
    slv_eq = {}
    slv_eq_att = []
    # This attribute might not exist if user forgot block "solved_equations"!
    try:     slv_eq_att = self._pyd_value.solved_equations
    except:  pass
    for two_words in slv_eq_att:
        slv_eq[two_words.mot_2] = two_words.mot_1
    if tok not in slv_eq:
        # A genuine unexpected attribute:
        err = cls.GenErr(stream, f"Unexpected attribute or equation alias '{tok}' in keyword '{nams}'")
        raise ValueError(err) from None
    # Save the full description of the token that brought us here (=the equation alias):
    tok_full = stream.lastReadTokens()
    # At least one equation has been read:
    self._attr_ok['liste_equations'] = True
    # Now parse the coming block as an equation of the proper type:
    eq_cls = ClassFactory.GetParserClassFromName(slv_eq[tok])
    self.Dbg(f"@FUNC@ About to ReadFromTokens class '{eq_cls}' alias '{tok}'")
    obj = eq_cls.ReadFromTokens(stream)
    # Hack the saved tokens so that when the equation will be written out again,
    # the alias will be used instead of the class name ...
    obj._parser._tokens['cls_nam'] = tok_full
    # Create the list attribute itself if not there (first equation read typically):
    if self._pyd_value.liste_equations == []:
        # Make sure the equation list will appear at the right place:
        self._attrInOrder.append("liste_equations")
        # Register the parser for it
        self._leafParsers["liste_equations"] = Builtin_Parser.InstanciateFromBuiltin(Annotated[List[Eqn_base], "Listeqn"])
        # Hack the tokens for the attribute itself:
        #  - the attribute name ('liste_equations') should not be written
        #  - nor the braces '{' and '}'
        self._tokens["liste_equations"] = TRUSTTokens()
        def dummyGetBrace(br):
            return ""
        self._leafParsers["liste_equations"].getBraceTokens = dummyGetBrace
    # Append equation instance in this list:
    self._pyd_value.liste_equations.append(obj)

try:
    Problem_read_generic_Parser.handleUnexpectedAttribute = handleUnexpectedAttribute_pb_generic
except:
    pass
