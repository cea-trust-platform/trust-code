#!/usr/bin/env python
# -*- coding: UTF-8 -*-

"""
Automatic generation with method 'trad2xy' of file
  /export/home/adrien/Projets/TRUST/trustpy/trustpy/trad2_code_gen.py
From initial file definition from TRUST compilation
  /export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple
  ->
  /export/home/adrien/Projets/TRUST/trustpy/trustpy/test/trust_packagespy_generated_simple.py
"""

from trustpy.baseTru import *          # ConstrainBase_Tru, BaseChaine_Tru, BaseEntier_Tru, BaseFloattant_Tru etc
from xyzpy.intFloatListXyz import *    # StrXyz, StrInList,IntXyz, FloatXyz, TodoXyz ...
from trustpy.trust_utilities import *

  
class read_med_Tru(interprete_Tru): #####
  r"""
  Keyword to read MED mesh files where 'domain' corresponds to the domain name, 'file' 
  corresponds to the file (written in the MED format) containing the mesh named mesh_name.
  
  Note about naming boundaries: When reading 'file', TRUST will detect boundaries between 
  domains (Raccord) when the name of the boundary begins by type_raccord_.
  For example, a boundary named type_raccord_wall in 'file' will be considered by TRUST 
  as a boundary named 'wall' between two domains.
  
  NB: To read several domains from a mesh issued from a MED file, use Read_Med to read 
  the mesh then use Create_domain_from_sub_domain keyword.
  
  NB: If the MED file contains one or several subdomaine defined as a group of volumes, 
  then Read_MED will read it and will create two files domain_name_ssz.geo and domain_name_ssz_par.geo 
  defining the subdomaines for sequential and/or parallel calculations.
  These subdomaines will be read in sequential in the datafile by including (after 
  Read_Med keyword) something like:
  
  Read_Med ....
  
  Read_file domain_name_ssz.geo ;
  
  During the parallel calculation, you will include something:
  
  Scatter { ...
  }
  
  Read_file domain_name_ssz_par.geo ;
  """
  # From: line 2, in file '/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple'
  _braces = 1
  _synonyms = ['lire_med_Tru']
  _attributesList = [
    ('convertalltopoly', 'rien_Tru'),   #
    ('no_family_names_from_group_names', 'rien_Tru'),   #
    ('file', 'chaine_Tru'),   #
    ('mesh', 'chaine_Tru'),   #
    ('exclude_groups', 'listchaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'convertalltopoly': ['syno1', 'syno2'],
    'exclude_groups': ['exclure_groupes'],
    'file': ['fichier'],
    'mesh': ['maillage']
  }
  _optionals = set(['convertalltopoly',
   'no_family_names_from_group_names',
   'mesh',
   'exclude_groups'])
  _infoMain = ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple', 2]
  _infoAttr = {'convertalltopoly': ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple',
                        3],
   'exclude_groups': ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple',
                      7],
   'file': ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple',
            5],
   'mesh': ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple',
            6],
   'no_family_names_from_group_names': ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple',
                                        4]}
  _attributesList = check_append_list('read_med_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('read_med_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('read_med_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'convertalltopoly':
          ('[rien] Option to convert mesh with mixed cells into polyhedral/polygonal cells', ''),
    'no_family_names_from_group_names':
          ('[rien] Awful option just to keep naked family names from MED file.\n'
           'Rarely used, to be removed very soon.',
           ''),
    'file':
          ("[chaine] File (written in the MED format, with extension '.med') containing the mesh", ''),
    'mesh':
          ('[chaine] Name of the mesh in med file.\nIf not specified, the first mesh will be read.', ''),
    'exclude_groups':
          ('[listchaine] List of face groups to skip in the MED file.', ''),
  }
  _helpDict = check_append_dict('read_med_Tru', _helpDict, interprete_Tru._helpDict)
##### end read_med_Tru class

class toto_base_Tru(ConstrainBase_Tru): #####
  r"""
  just so that it has _base at the end of the name ...
  this triggers type reading.
  """
  # From: line 9, in file '/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple'
  _braces = 0
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple', 9]
  _infoAttr = {}
  _helpDict = {}
##### end toto_base_Tru class

class read_med_bidon_Tru(toto_base_Tru): #####
  r"""
  Keyword to read MED mesh files where 'domain' corresponds to the domain name, 'file' 
  corresponds to the file (written in the MED format) containing the mesh named mesh_name.
  
  Note about naming boundaries: When reading 'file', TRUST will detect boundaries between 
  domains (Raccord) when the name of the boundary begins by type_raccord_.
  For example, a boundary named type_raccord_wall in 'file' will be considered by TRUST 
  as a boundary named 'wall' between two domains.
  
  NB: To read several domains from a mesh issued from a MED file, use Read_Med to read 
  the mesh then use Create_domain_from_sub_domain keyword.
  
  NB: If the MED file contains one or several subdomaine defined as a group of volumes, 
  then Read_MED will read it and will create two files domain_name_ssz.geo and domain_name_ssz_par.geo 
  defining the subdomaines for sequential and/or parallel calculations.
  These subdomaines will be read in sequential in the datafile by including (after 
  Read_Med keyword) something like:
  
  Read_Med ....
  
  Read_file domain_name_ssz.geo ;
  
  During the parallel calculation, you will include something:
  
  Scatter { ...
  }
  
  Read_file domain_name_ssz_par.geo ;
  """
  # From: line 10, in file '/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple'
  _braces = 1
  _synonyms = ['lire_med_bidon_Tru']
  _attributesList = [
    ('convertalltopoly', 'rien_Tru'),   #
    ('no_family_names_from_group_names', 'rien_Tru'),   #
    ('file', 'chaine_Tru'),   #
    ('mesh', 'chaine_Tru'),   #
    ('exclude_groups', 'listchaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'convertalltopoly': ['syno1', 'syno2'],
    'exclude_groups': ['exclure_groupes'],
    'file': ['fichier'],
    'mesh': ['maillage']
  }
  _optionals = set(['convertalltopoly',
   'no_family_names_from_group_names',
   'mesh',
   'exclude_groups'])
  _infoMain = ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple', 10]
  _infoAttr = {'convertalltopoly': ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple',
                        11],
   'exclude_groups': ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple',
                      15],
   'file': ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple',
            13],
   'mesh': ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple',
            14],
   'no_family_names_from_group_names': ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple',
                                        12]}
  _attributesList = check_append_list('read_med_bidon_Tru', _attributesList, toto_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('read_med_bidon_Tru', _attributesSynonyms, toto_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('read_med_bidon_Tru', _infoAttr, toto_base_Tru._infoAttr)
  _optionals.update(toto_base_Tru._optionals)
  _helpDict = {
    'convertalltopoly':
          ('[rien] Option to convert mesh with mixed cells into polyhedral/polygonal cells', ''),
    'no_family_names_from_group_names':
          ('[rien] Awful option just to keep naked family names from MED file.\n'
           'Rarely used, to be removed very soon.',
           ''),
    'file':
          ("[chaine] File (written in the MED format, with extension '.med') containing the mesh", ''),
    'mesh':
          ('[chaine] Name of the mesh in med file.\nIf not specified, the first mesh will be read.', ''),
    'exclude_groups':
          ('[listchaine] List of face groups to skip in the MED file.', ''),
  }
  _helpDict = check_append_dict('read_med_bidon_Tru', _helpDict, toto_base_Tru._helpDict)
##### end read_med_bidon_Tru class

class field_base_Tru(ConstrainBase_Tru): #####
  r"""
  Basic class of fields.
  """
  # From: line 17, in file '/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple'
  _braces = 0
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = ['champ_base_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple', 17]
  _infoAttr = {}
  _helpDict = {}
##### end field_base_Tru class

class champ_don_base_Tru(field_base_Tru): #####
  r"""
  Basic class for data fields (not calculated), p.e.
  physics properties.
  """
  # From: line 18, in file '/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple', 18]
  _infoAttr = {}
  _attributesList = check_append_list('champ_don_base_Tru', _attributesList, field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_don_base_Tru', _attributesSynonyms, field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_don_base_Tru', _infoAttr, field_base_Tru._infoAttr)
  _optionals.update(field_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('champ_don_base_Tru', _helpDict, field_base_Tru._helpDict)
##### end champ_don_base_Tru class

class uniform_field_Tru(champ_don_base_Tru): #####
  r"""
  Field that is constant in space and stationary.
  """
  # From: line 19, in file '/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple'
  _braces = -1
  _synonyms = ['champ_uniforme_Tru']
  _attributesList = [
    ('flag_bidon', 'chaine_16e861_Tru'),   #
    ('val', 'list_Tru'),   #
    ('flag_bidon2', 'chaine_018e79_Tru'),   #
    ('val2', 'list_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['flag_bidon', 'flag_bidon2', 'val2'])
  _infoMain = ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple', 19]
  _infoAttr = {'flag_bidon': ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple',
                  20],
   'flag_bidon2': ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple',
                   22],
   'val': ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple',
           21],
   'val2': ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple',
            23]}
  _attributesList = check_append_list('uniform_field_Tru', _attributesList, champ_don_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('uniform_field_Tru', _attributesSynonyms, champ_don_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('uniform_field_Tru', _infoAttr, champ_don_base_Tru._infoAttr)
  _optionals.update(champ_don_base_Tru._optionals)
  _helpDict = {
    'flag_bidon':
          ('[chaine(into=["flag_bidon"])] Un flag bidon optionnel juste pour test', ''),
    'val':
          ('[list] Values of field components.', ''),
    'flag_bidon2':
          ('[chaine(into=["flag_bidon2"])] Un autre flag bidon optionnel juste pour test', ''),
    'val2':
          ('[list] Values of field components.', ''),
  }
  _helpDict = check_append_dict('uniform_field_Tru', _helpDict, champ_don_base_Tru._helpDict)
##### end uniform_field_Tru class

class coucou_base_Tru(ConstrainBase_Tru): #####
  r"""
  not set
  """
  # From: line 25, in file '/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple'
  _braces = 1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple', 25]
  _infoAttr = {}
  _helpDict = {}
##### end coucou_base_Tru class

class coucou_Tru(coucou_base_Tru): #####
  r"""
  Basic class for medium (physics properties of medium).
  """
  # From: line 26, in file '/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('attr_bidon', '<inherited>uniform_field_Tru'),   #
    ('gravite', '<inherited>field_base_Tru'),   #
    ('porosites_champ', '<inherited>field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['attr_bidon', 'gravite', 'porosites_champ'])
  _infoMain = ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple', 26]
  _infoAttr = {'attr_bidon': ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple',
                  27],
   'gravite': ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple',
               28],
   'porosites_champ': ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple',
                       29]}
  _attributesList = check_append_list('coucou_Tru', _attributesList, coucou_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('coucou_Tru', _attributesSynonyms, coucou_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('coucou_Tru', _infoAttr, coucou_base_Tru._infoAttr)
  _optionals.update(coucou_base_Tru._optionals)
  _helpDict = {
    'attr_bidon':
          ('[uniform_field] just to have a leaf type (unif field).', ''),
    'gravite':
          ('[field_base] Gravity field (optional).', ''),
    'porosites_champ':
          ('[field_base] The porosity is given at each element and the porosity at each face, \n'
           'Psi(face), is calculated by the average of the porosities of the two neighbour elements \n'
           'Psi(elem1), Psi(elem2) : Psi(face)=2/(1/Psi(elem1)+1/Psi(elem2)).\n'
           'This keyword is optional.',
           ''),
  }
  _helpDict = check_append_dict('coucou_Tru', _helpDict, coucou_base_Tru._helpDict)
##### end coucou_Tru class

class nom_Tru(ConstrainBase_Tru): #####
  r"""
  Class to name the TRUST objects.
  """
  # From: line 31, in file '/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('mot', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['mot'])
  _infoMain = ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple', 31]
  _infoAttr = {'mot': ['/export/home/adrien/Projets/TRUST/trustpy/trustpy/test/TRAD_2_adr_simple',
           32]}
  _helpDict = {
    'mot':
          ('[chaine] Chain of characters.', ''),
  }
##### end nom_Tru class


##### automatic classes

class rien_Tru(Rien_Tru):
  """
  automatic class 0 key rien from read_med.convertalltopoly ->
  rien
  """  
  pass
##### end rien_Tru class

class listchaine_Tru(ListOfChaine_Tru):
  """
  automatic class 1 key listchaine from read_med.exclude_groups ->
  listchaine
  """  
  pass
##### end listchaine_Tru class

class chaine_16e861_Tru(BaseChaineInList_Tru):
  """
  automatic class 2 key chaine_16e861 from uniform_field.flag_bidon ->
  chaine(into=["flag_bidon"])
  """  
  _allowedList = ["flag_bidon"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_16e861_Tru class

class list_Tru(ListOfFloat_Tru):
  """
  automatic class 3 key list from uniform_field.val ->
  list
  """  
  pass
##### end list_Tru class

class chaine_018e79_Tru(BaseChaineInList_Tru):
  """
  automatic class 4 key chaine_018e79 from uniform_field.flag_bidon2 ->
  chaine(into=["flag_bidon2"])
  """  
  _allowedList = ["flag_bidon2"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_018e79_Tru class

    
def initXyzClasses():
  """case of CLFX.reset() done may be useful to redo appendAllXyzClasses"""
  if "champ_don_base_Tru" not in CLFX.getAllXyzClassesNames():
    CLFX.appendAllXyzClasses(packagespy_classes)

# useful for future import on this file
packagespy_classes = [
  champ_don_base_Tru,
  coucou_Tru,
  coucou_base_Tru,
  field_base_Tru,
  nom_Tru,
  read_med_Tru,
  read_med_bidon_Tru,
  toto_base_Tru,
  uniform_field_Tru,
  ########## self._automatic_classes,
  rien_Tru,
  listchaine_Tru,
  chaine_16e861_Tru,
  list_Tru,
  chaine_018e79_Tru
]

# needs explicit class factory appends for *_generated_*.py, as precaution
# avoid automatic as implicit
# initXyzClasses()  # avoid this automatic as implicit

  
# number of classes: 9
# number of automatic classes: 5
# number of automatic identical classes: 0

###################### HACKS ####################################
# Those hacks can be amended editing the file
# /export/home/adrien/Projets/TRUST/trustpy/trustpy/trust_hacks.py
# which is simply appended here
#################################################################

"""
The content of this file will be appended at the end of the automatically generated code.
This is the place to put all the hacks needed when some keywords do not follow the TRUST grammar.

(Generally the hacks are put in try/except clauses so that simple tests that do not generate some
classes do not fail when passing over this piece of code)
"""

# First hack which is **not** in this file: handling of the attributes named 'lambda'.
# This is a reserved Python keyword. So it is changed everywhere in 'Lambda'.
# This is done in trad2_code_gen.py (look for 'Lambda') and in baseTru._InvertSyno()
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
