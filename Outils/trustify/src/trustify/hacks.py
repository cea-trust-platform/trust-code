"""
The content of this file will be appended at the end of the automatically generated code.
This is the place to put all the hacks needed when some keywords do not follow the TRUST grammar.

(Generally the hacks are put in try/except clauses so that simple tests that do not generate some
classes do not fail when passing over this piece of code)
"""

# First hack which is **not** in this file: handling of the attributes named 'lambda'.
# This is a reserved Python keyword. So it is changed everywhere in 'Lambda'.
# This is done in trad2_code_gen.py (look for 'Lambda') and in base._InvertSyno()
# where we register 'lambda' as a synonym for 'Lambda'


@classmethod
def ReadFromTokens_dim(cls, stream):
    """ Dimension is very special - we store it as a static variable at the top level of the class hierarchy
        once the 'dimension' keyword has been read.
        It is used to dimension some lists of coordinates when defining probes for example.
        See class baseTru.AbstractSizeIsDim_Tru and derived.
        TODO : should almost be put in baseTru.py??
    """
    val = super(dimension_Tru, cls).ReadFromTokens(stream)
    AbstractSizeIsDim_Tru._DIMENSION = val.dim
    new_defbord = f"defbord_{val.dim}_Tru"
    for c in packagespy_classes:
        if issubclass(c, ConstrainBase_Tru):
            for i, (att_nam, att_cls) in enumerate(c._attributesList):
                # Take all possibilities since this loop might already have been called:
                if att_cls in ["defbord_Tru", "defbord_2_Tru", "defbord_3_Tru"]:
                    c._attributesList[i] = (att_nam, new_defbord)
    return val

try:    dimension_Tru.ReadFromTokens = ReadFromTokens_dim
except: pass

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
for c in packagespy_classes:
    if not issubclass(c, ConstrainBase_Tru):
        continue
    try:   # (try/except since they might not be defined in simple test cases)
        if issubclass(c, milieu_base_Tru) or \
           issubclass(c, mor_eqn_Tru) or \
           issubclass(c, corps_postraitement_Tru) or \
           issubclass(c, pb_base_Tru):
             # Relevant dataset examples which will fail without the hack:
             #   milieu_base_Tru: any dataset with a milieu!!
             #   mor_eqn_Tru:             diffusion_implicite_jdd6
             #   corps_postraitement_Tru: distance_paroi_jdd1.data
             #   pb_base_Tru:             Kernel_ecrire_champ_med.data
            c._read_type = False
        # eqn_base is an exception to mor_eqn_Tru because of listeqn in Scalaires_passifs - example: WC_multi_2_3_espece.data
        eqn_base_Tru._read_type = True
    except:
        pass

# Build multi-map from synonyms to root names:
# Motivating example: 'convection { negligeable } / diffusion { negligleable }'
# This can only be done here, once all classes have been defined.
# See also ConstrainBase_Tru class definition in baseTru.py
ConstrainBase_Tru._rootNamesForSyno = {}
for c in packagespy_classes:
    if not issubclass(c, ConstrainBase_Tru):
        continue
    for s in c._synonyms:
        ConstrainBase_Tru._rootNamesForSyno.setdefault(s, []).append(c.__name__)

# Motivating example (scalaires_passifs): Quasi_Comp_Cond_GP_VDF_FM.data
def toDSToken_hack(self):
    """ When writing out a derived class of equation_base, its type should be written.
    Normally this is controlled by the '_read_type' class member which says that when the type
    should be read for a class, it should also be written out.
    But in this very specific case, when the class is read it is just an equation_base and at writing
    time it has been typed.
    """
    for v in self:
        v._read_type = True
    return ListOfBase_Tru.toDatasetTokens(self)

try:
    listeqn_Tru.toDatasetTokens = toDSToken_hack
except:
    pass
