"""
The content of this file will be appended at the end of the automatically generated code.
This is the place to put all the hacks needed when some keywords do not follow the usual TRUST grammar.

(Generally the hacks are put in try/except clauses so that simple unit tests that do not generate some
classes do not fail when passing over this piece of code)
"""

"""
When parsing a 'milieu_base' attribute, or an equation, the attribute name is actually the class name!
For example, we have

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
        # eqn_base is an exception to mor_eqn_Tru because of listeqn in Scalaires_passifs - example: WC_multi_2_3_espece.data
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
    TODO
    for v in self:
        v._read_type = True
    return ListOfBase_Parser.toDatasetTokens(self)

try:
    Listeqn_Parser.toDatasetTokens = toDSToken_hack
except:
    pass
