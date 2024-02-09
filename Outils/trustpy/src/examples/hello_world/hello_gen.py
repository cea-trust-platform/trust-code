#!/usr/bin/env python
# -*- coding: UTF-8 -*-

"""
Automatic generation with method 'trad2xy' of file
  /export/home/adrien/Projets/TRUST/trustpy/trustpy/trad2_code_gen.py
From initial file definition from TRUST compilation
  /export/home/adrien/Projets/TRUST/trustpy/examples/hello_world/TRAD2_example
  ->
  /export/home/adrien/Projets/TRUST/trustpy/examples/hello_world/hello_gen.py
"""

from trustpy.baseTru import *          # ConstrainBase_Tru, BaseChaine_Tru, BaseEntier_Tru, BaseFloattant_Tru etc
from xyzpy.intFloatListXyz import *    # StrXyz, StrInList,IntXyz, FloatXyz, TodoXyz ...
from trustpy.trust_utilities import *

  
class comment_Tru(ConstrainBase_Tru): #####
  r"""
  Comments in a data file.
  """
  # From: line 1, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['#_Tru']
  _attributesList = [
    ('comm', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1]
  _infoAttr = {'comm': ['TRAD2_example', 2]}
  _helpDict = {
    'comm':
          ('[chaine] Text to be commented.', ''),
  }
##### end comment_Tru class

class bloc_comment_Tru(ConstrainBase_Tru): #####
  r"""
  bloc of Comment in a data file.
  """
  # From: line 3, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['/*_Tru']
  _attributesList = [
    ('comm', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 3]
  _infoAttr = {'comm': ['TRAD2_example', 4]}
  _helpDict = {
    'comm':
          ('[chaine] Text to be commented.', ''),
  }
##### end bloc_comment_Tru class

class read_Tru(interprete_Tru): #####
  r"""
  Interpretor to read the a_object objet defined between the braces.
  """
  # From: line 6, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['lire_Tru']
  _attributesList = [
    ('a_object', 'chaine_Tru'),   #
    ('bloc', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 6]
  _infoAttr = {'a_object': ['TRAD2_example', 7], 'bloc': ['TRAD2_example', 8]}
  _attributesList = check_append_list('read_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('read_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('read_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'a_object':
          ('[chaine] Object to be read.', ''),
    'bloc':
          ('[chaine] Definition of the object.', ''),
  }
  _helpDict = check_append_dict('read_Tru', _helpDict, interprete_Tru._helpDict)
##### end read_Tru class

class associate_Tru(interprete_Tru): #####
  r"""
  This interpretor allows one object to be associated with another.
  The order of the two objects in this instruction is not important.
  The object objet_2 is associated to objet_1 if this makes sense; if not either objet_1 
  is associated to objet_2 or the program exits with error because it cannot execute 
  the Associate (Associer) instruction.
  For example, to calculate water flow in a pipe, a Pb_Hydraulique type object needs 
  to be defined.
  But also a Domaine type object to represent the pipe, a Scheme_euler_explicit type 
  object for time discretization, a discretization type object (VDF or VEF) and a Fluide_Incompressible 
  type object which will contain the water properties.
  These objects must then all be associated with the problem.
  """
  # From: line 9, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['associer_Tru']
  _attributesList = [
    ('objet_1', 'chaine_Tru'),   #
    ('objet_2', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 9]
  _infoAttr = {'objet_1': ['TRAD2_example', 10], 'objet_2': ['TRAD2_example', 11]}
  _attributesList = check_append_list('associate_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('associate_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('associate_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'objet_1':
          ('[chaine] Objet_1', ''),
    'objet_2':
          ('[chaine] Objet_2', ''),
  }
  _helpDict = check_append_dict('associate_Tru', _helpDict, interprete_Tru._helpDict)
##### end associate_Tru class

class listobj_Tru(ConstrainBase_Tru): #####
  r"""
  List of objects.
  """
  # From: line 12, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 12]
  _infoAttr = {}
  _helpDict = {}
##### end listobj_Tru class

class objet_lecture_Tru(ConstrainBase_Tru): #####
  r"""
  Auxiliary class for reading.
  """
  # From: line 13, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 13]
  _infoAttr = {}
  _helpDict = {}
##### end objet_lecture_Tru class

class bloc_lecture_Tru(objet_lecture_Tru): #####
  r"""
  to read between two braces
  """
  # From: line 14, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('bloc_lecture', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 14]
  _infoAttr = {'bloc_lecture': ['TRAD2_example', 15]}
  _attributesList = check_append_list('bloc_lecture_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('bloc_lecture_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('bloc_lecture_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'bloc_lecture':
          ('[chaine] not_set', ''),
  }
  _helpDict = check_append_dict('bloc_lecture_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end bloc_lecture_Tru class

class deuxmots_Tru(objet_lecture_Tru): #####
  r"""
  Two words.
  """
  # From: line 16, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('mot_1', 'chaine_Tru'),   #
    ('mot_2', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 16]
  _infoAttr = {'mot_1': ['TRAD2_example', 17], 'mot_2': ['TRAD2_example', 18]}
  _attributesList = check_append_list('deuxmots_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('deuxmots_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('deuxmots_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'mot_1':
          ('[chaine] First word.', ''),
    'mot_2':
          ('[chaine] Second word.', ''),
  }
  _helpDict = check_append_dict('deuxmots_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end deuxmots_Tru class

class troismots_Tru(objet_lecture_Tru): #####
  r"""
  Three words.
  """
  # From: line 19, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('mot_1', 'chaine_Tru'),   #
    ('mot_2', 'chaine_Tru'),   #
    ('mot_3', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 19]
  _infoAttr = {'mot_1': ['TRAD2_example', 20],
   'mot_2': ['TRAD2_example', 21],
   'mot_3': ['TRAD2_example', 22]}
  _attributesList = check_append_list('troismots_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('troismots_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('troismots_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'mot_1':
          ('[chaine] First word.', ''),
    'mot_2':
          ('[chaine] Snd word.', ''),
    'mot_3':
          ('[chaine] Third word.', ''),
  }
  _helpDict = check_append_dict('troismots_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end troismots_Tru class

class format_file_Tru(objet_lecture_Tru): #####
  r"""
  File formatted.
  """
  # From: line 23, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('format', 'chaine_db4169_Tru'),   #
    ('name_file', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['format'])
  _infoMain = ['TRAD2_example', 23]
  _infoAttr = {'format': ['TRAD2_example', 24], 'name_file': ['TRAD2_example', 25]}
  _attributesList = check_append_list('format_file_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('format_file_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('format_file_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'format':
          ('[chaine(into=["binaire","formatte","xyz","single_hdf"])] Type of file (the file format).', ''),
    'name_file':
          ('[chaine] Name of file.', ''),
  }
  _helpDict = check_append_dict('format_file_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end format_file_Tru class

class deuxentiers_Tru(objet_lecture_Tru): #####
  r"""
  Two integers.
  """
  # From: line 26, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('int1', 'entier_3282e0_Tru'),   #
    ('int2', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 26]
  _infoAttr = {'int1': ['TRAD2_example', 27], 'int2': ['TRAD2_example', 28]}
  _attributesList = check_append_list('deuxentiers_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('deuxentiers_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('deuxentiers_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'int1':
          ('[entier] First integer.', ''),
    'int2':
          ('[entier] Second integer.', ''),
  }
  _helpDict = check_append_dict('deuxentiers_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end deuxentiers_Tru class

class floatfloat_Tru(objet_lecture_Tru): #####
  r"""
  Two reals.
  """
  # From: line 29, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('a', 'floattant_820d8b_Tru'),   #
    ('b', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 29]
  _infoAttr = {'a': ['TRAD2_example', 30], 'b': ['TRAD2_example', 31]}
  _attributesList = check_append_list('floatfloat_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('floatfloat_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('floatfloat_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'a':
          ('[floattant] First real.', ''),
    'b':
          ('[floattant] Second real.', ''),
  }
  _helpDict = check_append_dict('floatfloat_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end floatfloat_Tru class

class entierfloat_Tru(objet_lecture_Tru): #####
  r"""
  An integer and a real.
  """
  # From: line 32, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('the_int', 'entier_3282e0_Tru'),   #
    ('the_float', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 32]
  _infoAttr = {'the_float': ['TRAD2_example', 34], 'the_int': ['TRAD2_example', 33]}
  _attributesList = check_append_list('entierfloat_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('entierfloat_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('entierfloat_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'the_int':
          ('[entier] Integer.', ''),
    'the_float':
          ('[floattant] Real.', ''),
  }
  _helpDict = check_append_dict('entierfloat_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end entierfloat_Tru class

class champ_a_post_Tru(objet_lecture_Tru): #####
  r"""
  Field to be post-processed.
  """
  # From: line 35, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('champ', 'chaine_Tru'),   #
    ('localisation', 'chaine_a96986_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['localisation'])
  _infoMain = ['TRAD2_example', 35]
  _infoAttr = {'champ': ['TRAD2_example', 36], 'localisation': ['TRAD2_example', 37]}
  _attributesList = check_append_list('champ_a_post_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_a_post_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_a_post_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'champ':
          ('[chaine] Name of the post-processed field.', ''),
    'localisation':
          ('[chaine(into=["elem","som","faces"])] Localisation of post-processed field values: \n'
           'The two available values are elem, som, or faces (LATA format only) used respectively \n'
           'to select field values at mesh centres (CHAMPMAILLE type field in the lml file) or \n'
           'at mesh nodes (CHAMPPOINT type field in the lml file).\n'
           'If no selection is made, localisation is set to som by default.',
           ''),
  }
  _helpDict = check_append_dict('champ_a_post_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end champ_a_post_Tru class

class field_base_Tru(ConstrainBase_Tru): #####
  r"""
  Basic class of fields.
  """
  # From: line 38, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = ['champ_base_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 38]
  _infoAttr = {}
  _helpDict = {}
##### end field_base_Tru class

class champs_a_post_Tru(ListOfBase_Tru): #####
  """
  Fields to be post-processed.
  """
  # From: line 39, in file 'TRAD2_example'
  _braces = -1
  _comma = 0
  _allowedClasses = ["champ_a_post_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 39]
##### end champs_a_post_Tru class

class champs_posts_Tru(objet_lecture_Tru): #####
  r"""
  Field's write mode.
  """
  # From: line 40, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('format', 'chaine_7d729e_Tru'),   #
    ('mot', 'chaine_824559_Tru'),   #
    ('period', 'chaine_Tru'),   #
    ('champs', 'champs_a_post_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'champs': ['fields']
  }
  _optionals = set(['format'])
  _infoMain = ['TRAD2_example', 40]
  _infoAttr = {'champs': ['TRAD2_example', 44],
   'format': ['TRAD2_example', 41],
   'mot': ['TRAD2_example', 42],
   'period': ['TRAD2_example', 43]}
  _attributesList = check_append_list('champs_posts_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champs_posts_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champs_posts_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'format':
          ('[chaine(into=["binaire","formatte"])] Type of file.', ''),
    'mot':
          ('[chaine(into=["dt_post","nb_pas_dt_post"])] Keyword to set the kind of the field\'s \n'
           'write frequency.\n'
           'Either a time period or a time step period.',
           ''),
    'period':
          ('[chaine] Value of the period which can be like (2.*t).', ''),
    'champs':
          ('[champs_a_post] Post-processed fields.', ''),
  }
  _helpDict = check_append_dict('champs_posts_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end champs_posts_Tru class

class stat_post_deriv_Tru(objet_lecture_Tru): #####
  r"""
  not_set
  """
  # From: line 45, in file 'TRAD2_example'
  _braces = 0
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 45]
  _infoAttr = {}
  _attributesList = check_append_list('stat_post_deriv_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('stat_post_deriv_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('stat_post_deriv_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('stat_post_deriv_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end stat_post_deriv_Tru class

class stat_post_t_deb_Tru(stat_post_deriv_Tru): #####
  r"""
  not_set
  """
  # From: line 46, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['t_deb_Tru']
  _attributesList = [
    ('val', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 46]
  _infoAttr = {'val': ['TRAD2_example', 47]}
  _attributesList = check_append_list('stat_post_t_deb_Tru', _attributesList, stat_post_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('stat_post_t_deb_Tru', _attributesSynonyms, stat_post_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('stat_post_t_deb_Tru', _infoAttr, stat_post_deriv_Tru._infoAttr)
  _optionals.update(stat_post_deriv_Tru._optionals)
  _helpDict = {
    'val':
          ('[floattant] not_set', ''),
  }
  _helpDict = check_append_dict('stat_post_t_deb_Tru', _helpDict, stat_post_deriv_Tru._helpDict)
##### end stat_post_t_deb_Tru class

class stat_post_t_fin_Tru(stat_post_deriv_Tru): #####
  r"""
  not_set
  """
  # From: line 48, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['t_fin_Tru']
  _attributesList = [
    ('val', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 48]
  _infoAttr = {'val': ['TRAD2_example', 49]}
  _attributesList = check_append_list('stat_post_t_fin_Tru', _attributesList, stat_post_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('stat_post_t_fin_Tru', _attributesSynonyms, stat_post_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('stat_post_t_fin_Tru', _infoAttr, stat_post_deriv_Tru._infoAttr)
  _optionals.update(stat_post_deriv_Tru._optionals)
  _helpDict = {
    'val':
          ('[floattant] not_set', ''),
  }
  _helpDict = check_append_dict('stat_post_t_fin_Tru', _helpDict, stat_post_deriv_Tru._helpDict)
##### end stat_post_t_fin_Tru class

class stat_post_moyenne_Tru(stat_post_deriv_Tru): #####
  r"""
  not_set
  """
  # From: line 50, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['moyenne_Tru', 'champ_post_statistiques_moyenne_Tru']
  _attributesList = [
    ('field', 'chaine_Tru'),   #
    ('localisation', 'chaine_a96986_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['localisation'])
  _infoMain = ['TRAD2_example', 50]
  _infoAttr = {'field': ['TRAD2_example', 51], 'localisation': ['TRAD2_example', 52]}
  _attributesList = check_append_list('stat_post_moyenne_Tru', _attributesList, stat_post_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('stat_post_moyenne_Tru', _attributesSynonyms, stat_post_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('stat_post_moyenne_Tru', _infoAttr, stat_post_deriv_Tru._infoAttr)
  _optionals.update(stat_post_deriv_Tru._optionals)
  _helpDict = {
    'field':
          ('[chaine] not_set', ''),
    'localisation':
          ('[chaine(into=["elem","som","faces"])] Localisation of post-processed field value', ''),
  }
  _helpDict = check_append_dict('stat_post_moyenne_Tru', _helpDict, stat_post_deriv_Tru._helpDict)
##### end stat_post_moyenne_Tru class

class stat_post_ecart_type_Tru(stat_post_deriv_Tru): #####
  r"""
  not_set
  """
  # From: line 53, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['ecart_type_Tru', 'champ_post_statistiques_ecart_type_Tru']
  _attributesList = [
    ('field', 'chaine_Tru'),   #
    ('localisation', 'chaine_a96986_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['localisation'])
  _infoMain = ['TRAD2_example', 53]
  _infoAttr = {'field': ['TRAD2_example', 54], 'localisation': ['TRAD2_example', 55]}
  _attributesList = check_append_list('stat_post_ecart_type_Tru', _attributesList, stat_post_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('stat_post_ecart_type_Tru', _attributesSynonyms, stat_post_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('stat_post_ecart_type_Tru', _infoAttr, stat_post_deriv_Tru._infoAttr)
  _optionals.update(stat_post_deriv_Tru._optionals)
  _helpDict = {
    'field':
          ('[chaine] not_set', ''),
    'localisation':
          ('[chaine(into=["elem","som","faces"])] Localisation of post-processed field value', ''),
  }
  _helpDict = check_append_dict('stat_post_ecart_type_Tru', _helpDict, stat_post_deriv_Tru._helpDict)
##### end stat_post_ecart_type_Tru class

class stat_post_correlation_Tru(stat_post_deriv_Tru): #####
  r"""
  not_set
  """
  # From: line 56, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['champ_post_statistiques_correlation_Tru', 'correlation_Tru']
  _attributesList = [
    ('first_field', 'chaine_Tru'),   #
    ('second_field', 'chaine_Tru'),   #
    ('localisation', 'chaine_a96986_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['localisation'])
  _infoMain = ['TRAD2_example', 56]
  _infoAttr = {'first_field': ['TRAD2_example', 57],
   'localisation': ['TRAD2_example', 59],
   'second_field': ['TRAD2_example', 58]}
  _attributesList = check_append_list('stat_post_correlation_Tru', _attributesList, stat_post_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('stat_post_correlation_Tru', _attributesSynonyms, stat_post_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('stat_post_correlation_Tru', _infoAttr, stat_post_deriv_Tru._infoAttr)
  _optionals.update(stat_post_deriv_Tru._optionals)
  _helpDict = {
    'first_field':
          ('[chaine] not_set', ''),
    'second_field':
          ('[chaine] not_set', ''),
    'localisation':
          ('[chaine(into=["elem","som","faces"])] Localisation of post-processed field value', ''),
  }
  _helpDict = check_append_dict('stat_post_correlation_Tru', _helpDict, stat_post_deriv_Tru._helpDict)
##### end stat_post_correlation_Tru class

class list_stat_post_Tru(ListOfBase_Tru): #####
  """
  Post-processing for statistics
  """
  # From: line 60, in file 'TRAD2_example'
  _braces = -1
  _comma = 0
  _allowedClasses = ["stat_post_deriv_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 60]
##### end list_stat_post_Tru class

class stats_posts_Tru(objet_lecture_Tru): #####
  r"""
  Field's write mode.
  \input{{statistiques}}
  """
  # From: line 61, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('mot', 'chaine_824559_Tru'),   #
    ('period', 'chaine_Tru'),   #
    ('champs', 'list_stat_post_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'champs': ['fields']
  }
  _optionals = set([])
  _infoMain = ['TRAD2_example', 61]
  _infoAttr = {'champs': ['TRAD2_example', 64],
   'mot': ['TRAD2_example', 62],
   'period': ['TRAD2_example', 63]}
  _attributesList = check_append_list('stats_posts_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('stats_posts_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('stats_posts_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'mot':
          ('[chaine(into=["dt_post","nb_pas_dt_post"])] Keyword to set the kind of the field\'s \n'
           'write frequency.\n'
           'Either a time period or a time step period.',
           ''),
    'period':
          ('[chaine] Value of the period which can be like (2.*t).', ''),
    'champs':
          ('[list_stat_post] Post-processed fields.', ''),
  }
  _helpDict = check_append_dict('stats_posts_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end stats_posts_Tru class

class stats_serie_posts_Tru(objet_lecture_Tru): #####
  r"""
  Post-processing for statistics.
  \input{{statistiquesseries}}
  """
  # From: line 65, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('mot', 'chaine_38aea9_Tru'),   #
    ('dt_integr', 'floattant_820d8b_Tru'),   #
    ('stat', 'list_stat_post_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 65]
  _infoAttr = {'dt_integr': ['TRAD2_example', 67],
   'mot': ['TRAD2_example', 66],
   'stat': ['TRAD2_example', 68]}
  _attributesList = check_append_list('stats_serie_posts_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('stats_serie_posts_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('stats_serie_posts_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'mot':
          ('[chaine(into=["dt_integr"])] Keyword is used to set the statistics period of integration \n'
           'and write period.',
           ''),
    'dt_integr':
          ('[floattant] Average on dt_integr time interval is post-processed every dt_integr seconds.', ''),
    'stat':
          ('[list_stat_post] not_set', ''),
  }
  _helpDict = check_append_dict('stats_serie_posts_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end stats_serie_posts_Tru class

class sonde_base_Tru(objet_lecture_Tru): #####
  r"""
  Basic probe.
  Probes refer to sensors that allow a value or several points of the domain to be 
  monitored over time.
  The probes may be a set of points defined one by one (keyword Points) or a set of 
  points evenly distributed over a straight segment (keyword Segment) or arranged according 
  to a layout (keyword Plan) or according to a parallelepiped (keyword Volume).
  The fields allow all the values of a physical value on the domain to be known at 
  several moments in time.
  """
  # From: line 69, in file 'TRAD2_example'
  _braces = 0
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 69]
  _infoAttr = {}
  _attributesList = check_append_list('sonde_base_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('sonde_base_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('sonde_base_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('sonde_base_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end sonde_base_Tru class

class un_point_Tru(objet_lecture_Tru): #####
  r"""
  A point.
  """
  # From: line 70, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('pos', 'listf_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 70]
  _infoAttr = {'pos': ['TRAD2_example', 71]}
  _attributesList = check_append_list('un_point_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('un_point_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('un_point_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'pos':
          ('[listf] Point coordinates.', ''),
  }
  _helpDict = check_append_dict('un_point_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end un_point_Tru class

class listpoints_Tru(ListOfBase_Tru): #####
  """
  Points.
  """
  # From: line 72, in file 'TRAD2_example'
  _braces = 0
  _comma = 0
  _allowedClasses = ["un_point_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 72]
##### end listpoints_Tru class

class points_Tru(sonde_base_Tru): #####
  r"""
  Keyword to define the number of probe points.
  The file is arranged in columns.
  """
  # From: line 73, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('points', 'listpoints_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 73]
  _infoAttr = {'points': ['TRAD2_example', 74]}
  _attributesList = check_append_list('points_Tru', _attributesList, sonde_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('points_Tru', _attributesSynonyms, sonde_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('points_Tru', _infoAttr, sonde_base_Tru._infoAttr)
  _optionals.update(sonde_base_Tru._optionals)
  _helpDict = {
    'points':
          ('[listpoints] Probe points.', ''),
  }
  _helpDict = check_append_dict('points_Tru', _helpDict, sonde_base_Tru._helpDict)
##### end points_Tru class

class point_Tru(points_Tru): #####
  r"""
  Point as class-daughter of Points.
  """
  # From: line 75, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 75]
  _infoAttr = {}
  _attributesList = check_append_list('point_Tru', _attributesList, points_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('point_Tru', _attributesSynonyms, points_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('point_Tru', _infoAttr, points_Tru._infoAttr)
  _optionals.update(points_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('point_Tru', _helpDict, points_Tru._helpDict)
##### end point_Tru class

class numero_elem_sur_maitre_Tru(sonde_base_Tru): #####
  r"""
  Keyword to define a probe at the special element.
  Useful for min/max sonde.
  """
  # From: line 76, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('numero', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 76]
  _infoAttr = {'numero': ['TRAD2_example', 77]}
  _attributesList = check_append_list('numero_elem_sur_maitre_Tru', _attributesList, sonde_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('numero_elem_sur_maitre_Tru', _attributesSynonyms, sonde_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('numero_elem_sur_maitre_Tru', _infoAttr, sonde_base_Tru._infoAttr)
  _optionals.update(sonde_base_Tru._optionals)
  _helpDict = {
    'numero':
          ('[entier] element number', ''),
  }
  _helpDict = check_append_dict('numero_elem_sur_maitre_Tru', _helpDict, sonde_base_Tru._helpDict)
##### end numero_elem_sur_maitre_Tru class

class segmentpoints_Tru(points_Tru): #####
  r"""
  This keyword is used to define a probe segment from specifics points.
  The nom_champ field is sampled at ns specifics points.
  """
  # From: line 78, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 78]
  _infoAttr = {}
  _attributesList = check_append_list('segmentpoints_Tru', _attributesList, points_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('segmentpoints_Tru', _attributesSynonyms, points_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('segmentpoints_Tru', _infoAttr, points_Tru._infoAttr)
  _optionals.update(points_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('segmentpoints_Tru', _helpDict, points_Tru._helpDict)
##### end segmentpoints_Tru class

class position_like_Tru(sonde_base_Tru): #####
  r"""
  Keyword to define a probe at the same position of another probe named autre_sonde.
  """
  # From: line 79, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('autre_sonde', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 79]
  _infoAttr = {'autre_sonde': ['TRAD2_example', 80]}
  _attributesList = check_append_list('position_like_Tru', _attributesList, sonde_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('position_like_Tru', _attributesSynonyms, sonde_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('position_like_Tru', _infoAttr, sonde_base_Tru._infoAttr)
  _optionals.update(sonde_base_Tru._optionals)
  _helpDict = {
    'autre_sonde':
          ('[chaine] Name of the other probe.', ''),
  }
  _helpDict = check_append_dict('position_like_Tru', _helpDict, sonde_base_Tru._helpDict)
##### end position_like_Tru class

class segment_Tru(sonde_base_Tru): #####
  r"""
  Keyword to define the number of probe segment points.
  The file is arranged in columns.
  """
  # From: line 81, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('nbr', 'entier_3282e0_Tru'),   #
    ('point_deb', 'un_point_Tru'),   #
    ('point_fin', 'un_point_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 81]
  _infoAttr = {'nbr': ['TRAD2_example', 82],
   'point_deb': ['TRAD2_example', 83],
   'point_fin': ['TRAD2_example', 84]}
  _attributesList = check_append_list('segment_Tru', _attributesList, sonde_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('segment_Tru', _attributesSynonyms, sonde_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('segment_Tru', _infoAttr, sonde_base_Tru._infoAttr)
  _optionals.update(sonde_base_Tru._optionals)
  _helpDict = {
    'nbr':
          ('[entier] Number of probe points of the segment, evenly distributed.', ''),
    'point_deb':
          ('[un_point] First outer probe segment point.', ''),
    'point_fin':
          ('[un_point] Second outer probe segment point.', ''),
  }
  _helpDict = check_append_dict('segment_Tru', _helpDict, sonde_base_Tru._helpDict)
##### end segment_Tru class

class plan_Tru(sonde_base_Tru): #####
  r"""
  Keyword to set the number of probe layout points.
  The file format is type .lml
  """
  # From: line 85, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('nbr', 'entier_3282e0_Tru'),   #
    ('nbr2', 'entier_3282e0_Tru'),   #
    ('point_deb', 'un_point_Tru'),   #
    ('point_fin', 'un_point_Tru'),   #
    ('point_fin_2', 'un_point_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 85]
  _infoAttr = {'nbr': ['TRAD2_example', 86],
   'nbr2': ['TRAD2_example', 87],
   'point_deb': ['TRAD2_example', 88],
   'point_fin': ['TRAD2_example', 89],
   'point_fin_2': ['TRAD2_example', 90]}
  _attributesList = check_append_list('plan_Tru', _attributesList, sonde_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('plan_Tru', _attributesSynonyms, sonde_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('plan_Tru', _infoAttr, sonde_base_Tru._infoAttr)
  _optionals.update(sonde_base_Tru._optionals)
  _helpDict = {
    'nbr':
          ('[entier] Number of probes in the first direction.', ''),
    'nbr2':
          ('[entier] Number of probes in the second direction.', ''),
    'point_deb':
          ('[un_point] First point defining the angle.\nThis angle should be positive.', ''),
    'point_fin':
          ('[un_point] Second point defining the angle.\nThis angle should be positive.', ''),
    'point_fin_2':
          ('[un_point] Third point defining the angle.\nThis angle should be positive.', ''),
  }
  _helpDict = check_append_dict('plan_Tru', _helpDict, sonde_base_Tru._helpDict)
##### end plan_Tru class

class volume_Tru(sonde_base_Tru): #####
  r"""
  Keyword to define the probe volume in a parallelepiped passing through 4 points and 
  the number of probes in each direction.
  """
  # From: line 91, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('nbr', 'entier_3282e0_Tru'),   #
    ('nbr2', 'entier_3282e0_Tru'),   #
    ('nbr3', 'entier_3282e0_Tru'),   #
    ('point_deb', 'un_point_Tru'),   #
    ('point_fin', 'un_point_Tru'),   #
    ('point_fin_2', 'un_point_Tru'),   #
    ('point_fin_3', 'un_point_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 91]
  _infoAttr = {'nbr': ['TRAD2_example', 92],
   'nbr2': ['TRAD2_example', 93],
   'nbr3': ['TRAD2_example', 94],
   'point_deb': ['TRAD2_example', 95],
   'point_fin': ['TRAD2_example', 96],
   'point_fin_2': ['TRAD2_example', 97],
   'point_fin_3': ['TRAD2_example', 98]}
  _attributesList = check_append_list('volume_Tru', _attributesList, sonde_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('volume_Tru', _attributesSynonyms, sonde_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('volume_Tru', _infoAttr, sonde_base_Tru._infoAttr)
  _optionals.update(sonde_base_Tru._optionals)
  _helpDict = {
    'nbr':
          ('[entier] Number of probes in the first direction.', ''),
    'nbr2':
          ('[entier] Number of probes in the second direction.', ''),
    'nbr3':
          ('[entier] Number of probes in the third direction.', ''),
    'point_deb':
          ('[un_point] Point of origin.', ''),
    'point_fin':
          ('[un_point] Point defining the first direction (from point of origin).', ''),
    'point_fin_2':
          ('[un_point] Point defining the second direction (from point of origin).', ''),
    'point_fin_3':
          ('[un_point] Point defining the third direction (from point of origin).', ''),
  }
  _helpDict = check_append_dict('volume_Tru', _helpDict, sonde_base_Tru._helpDict)
##### end volume_Tru class

class circle_Tru(sonde_base_Tru): #####
  r"""
  Keyword to define several probes located on a circle.
  """
  # From: line 99, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('nbr', 'entier_3282e0_Tru'),   #
    ('point_deb', 'un_point_Tru'),   #
    ('direction', 'entier_6f2a33_Tru'),   #
    ('radius', 'floattant_820d8b_Tru'),   #
    ('theta1', 'floattant_820d8b_Tru'),   #
    ('theta2', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['direction'])
  _infoMain = ['TRAD2_example', 99]
  _infoAttr = {'direction': ['TRAD2_example', 102],
   'nbr': ['TRAD2_example', 100],
   'point_deb': ['TRAD2_example', 101],
   'radius': ['TRAD2_example', 103],
   'theta1': ['TRAD2_example', 104],
   'theta2': ['TRAD2_example', 105]}
  _attributesList = check_append_list('circle_Tru', _attributesList, sonde_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('circle_Tru', _attributesSynonyms, sonde_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('circle_Tru', _infoAttr, sonde_base_Tru._infoAttr)
  _optionals.update(sonde_base_Tru._optionals)
  _helpDict = {
    'nbr':
          ('[entier] Number of probes between teta1 and teta2 (angles given in degrees).', ''),
    'point_deb':
          ('[un_point] Center of the circle.', ''),
    'direction':
          ('[entier(into=[0,1,2])] Axis normal to the circle plane (0:x axis, 1:y axis, 2:z axis).', ''),
    'radius':
          ('[floattant] Radius of the circle.', ''),
    'theta1':
          ('[floattant] First angle.', ''),
    'theta2':
          ('[floattant] Second angle.', ''),
  }
  _helpDict = check_append_dict('circle_Tru', _helpDict, sonde_base_Tru._helpDict)
##### end circle_Tru class

class circle_3_Tru(sonde_base_Tru): #####
  r"""
  Keyword to define several probes located on a circle (in 3-D space).
  """
  # From: line 106, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('nbr', 'entier_3282e0_Tru'),   #
    ('point_deb', 'un_point_Tru'),   #
    ('direction', 'entier_6f2a33_Tru'),   #
    ('radius', 'floattant_820d8b_Tru'),   #
    ('theta1', 'floattant_820d8b_Tru'),   #
    ('theta2', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 106]
  _infoAttr = {'direction': ['TRAD2_example', 109],
   'nbr': ['TRAD2_example', 107],
   'point_deb': ['TRAD2_example', 108],
   'radius': ['TRAD2_example', 110],
   'theta1': ['TRAD2_example', 111],
   'theta2': ['TRAD2_example', 112]}
  _attributesList = check_append_list('circle_3_Tru', _attributesList, sonde_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('circle_3_Tru', _attributesSynonyms, sonde_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('circle_3_Tru', _infoAttr, sonde_base_Tru._infoAttr)
  _optionals.update(sonde_base_Tru._optionals)
  _helpDict = {
    'nbr':
          ('[entier] Number of probes between teta1 and teta2 (angles given in degrees).', ''),
    'point_deb':
          ('[un_point] Center of the circle.', ''),
    'direction':
          ('[entier(into=[0,1,2])] Axis normal to the circle plane (0:x axis, 1:y axis, 2:z axis).', ''),
    'radius':
          ('[floattant] Radius of the circle.', ''),
    'theta1':
          ('[floattant] First angle.', ''),
    'theta2':
          ('[floattant] Second angle.', ''),
  }
  _helpDict = check_append_dict('circle_3_Tru', _helpDict, sonde_base_Tru._helpDict)
##### end circle_3_Tru class

class sondes_Tru(ListOfBase_Tru): #####
  """
  List of probes.
  """
  # From: line 113, in file 'TRAD2_example'
  _braces = 1
  _comma = 0
  _allowedClasses = ["sonde_Tru"]
  _synonyms = ['probes_Tru']
  _infoMain = ['TRAD2_example', 113]
##### end sondes_Tru class

class champ_generique_base_Tru(ConstrainBase_Tru): #####
  r"""
  not_set
  """
  # From: line 114, in file 'TRAD2_example'
  _braces = 1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 114]
  _infoAttr = {}
  _helpDict = {}
##### end champ_generique_base_Tru class

class listchamp_generique_Tru(ListOfBase_Tru): #####
  """
  XXX
  """
  # From: line 115, in file 'TRAD2_example'
  _braces = 1
  _comma = 1
  _allowedClasses = ["champ_generique_base_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 115]
##### end listchamp_generique_Tru class

class champ_post_de_champs_post_Tru(champ_generique_base_Tru): #####
  r"""
  not_set
  """
  # From: line 116, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('source', '<inherited>champ_generique_base_Tru'),   #
    ('nom_source', 'chaine_Tru'),   #
    ('source_reference', 'chaine_Tru'),   #
    ('sources_reference', 'list_nom_virgule_Tru'),   #
    ('sources', 'listchamp_generique_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['source', 'nom_source', 'source_reference', 'sources_reference', 'sources'])
  _infoMain = ['TRAD2_example', 116]
  _infoAttr = {'nom_source': ['TRAD2_example', 118],
   'source': ['TRAD2_example', 117],
   'source_reference': ['TRAD2_example', 119],
   'sources': ['TRAD2_example', 121],
   'sources_reference': ['TRAD2_example', 120]}
  _attributesList = check_append_list('champ_post_de_champs_post_Tru', _attributesList, champ_generique_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_post_de_champs_post_Tru', _attributesSynonyms, champ_generique_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_post_de_champs_post_Tru', _infoAttr, champ_generique_base_Tru._infoAttr)
  _optionals.update(champ_generique_base_Tru._optionals)
  _helpDict = {
    'source':
          ('[champ_generique_base] the source field.', ''),
    'nom_source':
          ('[chaine] To name a source field with the nom_source keyword', ''),
    'source_reference':
          ('[chaine] not_set', ''),
    'sources_reference':
          ('[list_nom_virgule] not_set', ''),
    'sources':
          ('[listchamp_generique] sources { Champ_Post....\n{ ...\n} Champ_Post..\n{ ...\n}}', ''),
  }
  _helpDict = check_append_dict('champ_post_de_champs_post_Tru', _helpDict, champ_generique_base_Tru._helpDict)
##### end champ_post_de_champs_post_Tru class

class champ_post_operateur_eqn_Tru(champ_post_de_champs_post_Tru): #####
  r"""
  not_set
  """
  # From: line 122, in file 'TRAD2_example'
  _braces = 1
  _synonyms = ['operateur_eqn_Tru']
  _attributesList = [
    ('numero_op', 'entier_3282e0_Tru'),   #
    ('numero_source', 'entier_3282e0_Tru'),   #
    ('sans_solveur_masse', 'rien_Tru'),   #
    ('compo', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['numero_op', 'numero_source', 'sans_solveur_masse', 'compo'])
  _infoMain = ['TRAD2_example', 122]
  _infoAttr = {'compo': ['TRAD2_example', 126],
   'numero_op': ['TRAD2_example', 123],
   'numero_source': ['TRAD2_example', 124],
   'sans_solveur_masse': ['TRAD2_example', 125]}
  _attributesList = check_append_list('champ_post_operateur_eqn_Tru', _attributesList, champ_post_de_champs_post_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_post_operateur_eqn_Tru', _attributesSynonyms, champ_post_de_champs_post_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_post_operateur_eqn_Tru', _infoAttr, champ_post_de_champs_post_Tru._infoAttr)
  _optionals.update(champ_post_de_champs_post_Tru._optionals)
  _helpDict = {
    'numero_op':
          ('[entier] not_set', ''),
    'numero_source':
          ('[entier] not_set', ''),
    'sans_solveur_masse':
          ('[rien] not_set', ''),
    'compo':
          ('[entier] If you want to post-process only one component of a vector field, you can \n'
           'specify the number of the component after compo keyword.\n'
           'By default, it is set to -1 which means that all the components will be post-processed.\n'
           'This feature is not available in VDF disretization.',
           ''),
  }
  _helpDict = check_append_dict('champ_post_operateur_eqn_Tru', _helpDict, champ_post_de_champs_post_Tru._helpDict)
##### end champ_post_operateur_eqn_Tru class

class transformation_Tru(champ_post_de_champs_post_Tru): #####
  r"""
  To create a field with a transformation.
  """
  # From: line 127, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['champ_post_transformation_Tru']
  _attributesList = [
    ('methode', 'chaine_deccae_Tru'),   #
    ('expression', 'listchaine_Tru'),   #
    ('numero', 'entier_3282e0_Tru'),   #
    ('localisation', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['expression', 'numero', 'localisation'])
  _infoMain = ['TRAD2_example', 127]
  _infoAttr = {'expression': ['TRAD2_example', 129],
   'localisation': ['TRAD2_example', 131],
   'methode': ['TRAD2_example', 128],
   'numero': ['TRAD2_example', 130]}
  _attributesList = check_append_list('transformation_Tru', _attributesList, champ_post_de_champs_post_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('transformation_Tru', _attributesSynonyms, champ_post_de_champs_post_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('transformation_Tru', _infoAttr, champ_post_de_champs_post_Tru._infoAttr)
  _optionals.update(champ_post_de_champs_post_Tru._optionals)
  _helpDict = {
    'methode':
          ('[chaine(into=["produit_scalaire","norme","vecteur","formule","composante"])] methode \n'
           'norme : will calculate the norm of a vector given by a source field\n'
           '\n'
           'methode produit_scalaire : will calculate the dot product of two vectors given by \n'
           'two sources fields\n'
           '\n'
           'methode composante numero integer : will create a field by extracting the integer \n'
           'component of a field given by a source field\n'
           '\n'
           'methode formule expression 1 : will create a scalar field located to elements using \n'
           'expressions with x,y,z,t parameters and field names given by a source field or several \n'
           'sources fields.\n'
           '\n'
           'methode vecteur expression N f1(x,y,z,t) fN(x,y,z,t) : will create a vector field \n'
           'located to elements by defining its N components with N expressions with x,y,z,t parameters \n'
           'and field names given by a source field or several sources fields.',
           ''),
    'expression':
          ('[listchaine] see methodes formule and vecteur', ''),
    'numero':
          ('[entier] see methode composante', ''),
    'localisation':
          ('[chaine] type_loc indicate where is done the interpolation (elem for element or som \n'
           'for node).\n'
           'The optional keyword methode is limited to calculer_champ_post for the moment',
           ''),
  }
  _helpDict = check_append_dict('transformation_Tru', _helpDict, champ_post_de_champs_post_Tru._helpDict)
##### end transformation_Tru class

class refchamp_Tru(champ_generique_base_Tru): #####
  r"""
  Field of prolem
  """
  # From: line 132, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['champ_post_refchamp_Tru']
  _attributesList = [
    ('pb_champ', 'deuxmots_Tru'),   #
    ('nom_source', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['nom_source'])
  _infoMain = ['TRAD2_example', 132]
  _infoAttr = {'nom_source': ['TRAD2_example', 134], 'pb_champ': ['TRAD2_example', 133]}
  _attributesList = check_append_list('refchamp_Tru', _attributesList, champ_generique_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('refchamp_Tru', _attributesSynonyms, champ_generique_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('refchamp_Tru', _infoAttr, champ_generique_base_Tru._infoAttr)
  _optionals.update(champ_generique_base_Tru._optionals)
  _helpDict = {
    'pb_champ':
          ('[deuxmots] { Pb_champ nom_pb nom_champ } : nom_pb is the problem name and nom_champ \n'
           'is the selected field name.',
           ''),
    'nom_source':
          ('[chaine] The alias name for the field', ''),
  }
  _helpDict = check_append_dict('refchamp_Tru', _helpDict, champ_generique_base_Tru._helpDict)
##### end refchamp_Tru class

class champ_post_operateur_base_Tru(champ_post_de_champs_post_Tru): #####
  r"""
  not_set
  """
  # From: line 135, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 135]
  _infoAttr = {}
  _attributesList = check_append_list('champ_post_operateur_base_Tru', _attributesList, champ_post_de_champs_post_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_post_operateur_base_Tru', _attributesSynonyms, champ_post_de_champs_post_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_post_operateur_base_Tru', _infoAttr, champ_post_de_champs_post_Tru._infoAttr)
  _optionals.update(champ_post_de_champs_post_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('champ_post_operateur_base_Tru', _helpDict, champ_post_de_champs_post_Tru._helpDict)
##### end champ_post_operateur_base_Tru class

class gradient_Tru(champ_post_operateur_base_Tru): #####
  r"""
  To calculate gradient of a given field.
  """
  # From: line 136, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['champ_post_operateur_gradient_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 136]
  _infoAttr = {}
  _attributesList = check_append_list('gradient_Tru', _attributesList, champ_post_operateur_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('gradient_Tru', _attributesSynonyms, champ_post_operateur_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('gradient_Tru', _infoAttr, champ_post_operateur_base_Tru._infoAttr)
  _optionals.update(champ_post_operateur_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('gradient_Tru', _helpDict, champ_post_operateur_base_Tru._helpDict)
##### end gradient_Tru class

class divergence_Tru(champ_post_operateur_base_Tru): #####
  r"""
  To calculate divergency of a given field.
  """
  # From: line 137, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['champ_post_operateur_divergence_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 137]
  _infoAttr = {}
  _attributesList = check_append_list('divergence_Tru', _attributesList, champ_post_operateur_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('divergence_Tru', _attributesSynonyms, champ_post_operateur_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('divergence_Tru', _infoAttr, champ_post_operateur_base_Tru._infoAttr)
  _optionals.update(champ_post_operateur_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('divergence_Tru', _helpDict, champ_post_operateur_base_Tru._helpDict)
##### end divergence_Tru class

class champ_post_statistiques_base_Tru(champ_post_de_champs_post_Tru): #####
  r"""
  not_set
  """
  # From: line 138, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = [
    ('t_deb', 'floattant_820d8b_Tru'),   #
    ('t_fin', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 138]
  _infoAttr = {'t_deb': ['TRAD2_example', 139], 't_fin': ['TRAD2_example', 140]}
  _attributesList = check_append_list('champ_post_statistiques_base_Tru', _attributesList, champ_post_de_champs_post_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_post_statistiques_base_Tru', _attributesSynonyms, champ_post_de_champs_post_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_post_statistiques_base_Tru', _infoAttr, champ_post_de_champs_post_Tru._infoAttr)
  _optionals.update(champ_post_de_champs_post_Tru._optionals)
  _helpDict = {
    't_deb':
          ('[floattant] Start of integration time', ''),
    't_fin':
          ('[floattant] End of integration time', ''),
  }
  _helpDict = check_append_dict('champ_post_statistiques_base_Tru', _helpDict, champ_post_de_champs_post_Tru._helpDict)
##### end champ_post_statistiques_base_Tru class

class correlation_Tru(champ_post_statistiques_base_Tru): #####
  r"""
  to calculate the correlation between the two fields.
  """
  # From: line 141, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['champ_post_statistiques_correlation_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 141]
  _infoAttr = {}
  _attributesList = check_append_list('correlation_Tru', _attributesList, champ_post_statistiques_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('correlation_Tru', _attributesSynonyms, champ_post_statistiques_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('correlation_Tru', _infoAttr, champ_post_statistiques_base_Tru._infoAttr)
  _optionals.update(champ_post_statistiques_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('correlation_Tru', _helpDict, champ_post_statistiques_base_Tru._helpDict)
##### end correlation_Tru class

class moyenne_Tru(champ_post_statistiques_base_Tru): #####
  r"""
  to calculate the average of the field over time
  """
  # From: line 142, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['champ_post_statistiques_moyenne_Tru']
  _attributesList = [
    ('moyenne_convergee', '<inherited>field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['moyenne_convergee'])
  _infoMain = ['TRAD2_example', 142]
  _infoAttr = {'moyenne_convergee': ['TRAD2_example', 143]}
  _attributesList = check_append_list('moyenne_Tru', _attributesList, champ_post_statistiques_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('moyenne_Tru', _attributesSynonyms, champ_post_statistiques_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('moyenne_Tru', _infoAttr, champ_post_statistiques_base_Tru._infoAttr)
  _optionals.update(champ_post_statistiques_base_Tru._optionals)
  _helpDict = {
    'moyenne_convergee':
          ('[field_base] This option allows to read a converged time averaged field in a .xyz \n'
           'file in order to calculate, when resuming the calculation, the statistics fields (rms, \n'
           'correlation) which depend on this average.\n'
           'In that case, the time averaged field is not updated during the resume of calculation.\n'
           'In this case, the time averaged field must be fully converged to avoid errors when \n'
           'calculating high order statistics.',
           ''),
  }
  _helpDict = check_append_dict('moyenne_Tru', _helpDict, champ_post_statistiques_base_Tru._helpDict)
##### end moyenne_Tru class

class ecart_type_Tru(champ_post_statistiques_base_Tru): #####
  r"""
  to calculate the standard deviation (statistic rms) of the field nom_champ.
  """
  # From: line 144, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['champ_post_statistiques_ecart_type_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 144]
  _infoAttr = {}
  _attributesList = check_append_list('ecart_type_Tru', _attributesList, champ_post_statistiques_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('ecart_type_Tru', _attributesSynonyms, champ_post_statistiques_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('ecart_type_Tru', _infoAttr, champ_post_statistiques_base_Tru._infoAttr)
  _optionals.update(champ_post_statistiques_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('ecart_type_Tru', _helpDict, champ_post_statistiques_base_Tru._helpDict)
##### end ecart_type_Tru class

class extraction_Tru(champ_post_de_champs_post_Tru): #####
  r"""
  To create a surface field (values at the boundary) of a volume field
  """
  # From: line 145, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['champ_post_extraction_Tru']
  _attributesList = [
    ('domaine', 'ref_domaine_Tru'),   #
    ('nom_frontiere', 'chaine_Tru'),   #
    ('methode', 'chaine_10bcdf_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['methode'])
  _infoMain = ['TRAD2_example', 145]
  _infoAttr = {'domaine': ['TRAD2_example', 146],
   'methode': ['TRAD2_example', 148],
   'nom_frontiere': ['TRAD2_example', 147]}
  _attributesList = check_append_list('extraction_Tru', _attributesList, champ_post_de_champs_post_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('extraction_Tru', _attributesSynonyms, champ_post_de_champs_post_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('extraction_Tru', _infoAttr, champ_post_de_champs_post_Tru._infoAttr)
  _optionals.update(champ_post_de_champs_post_Tru._optionals)
  _helpDict = {
    'domaine':
          ('[ref_domaine] name of the volume field', ''),
    'nom_frontiere':
          ('[chaine] boundary name where the values of the volume field will be picked', ''),
    'methode':
          ('[chaine(into=["trace","champ_frontiere"])] name of the extraction method (trace by_default \n'
           'or champ_frontiere)',
           ''),
  }
  _helpDict = check_append_dict('extraction_Tru', _helpDict, champ_post_de_champs_post_Tru._helpDict)
##### end extraction_Tru class

class morceau_equation_Tru(champ_post_de_champs_post_Tru): #####
  r"""
  To calculate a field related to a piece of equation.
  For the moment, the field which can be calculated is the stability time step of an 
  operator equation.
  The problem name and the unknown of the equation should be given by Source refChamp 
  { Pb_Champ problem_name unknown_field_of_equation }
  """
  # From: line 149, in file 'TRAD2_example'
  _braces = 1
  _synonyms = ['champ_post_morceau_equation_Tru']
  _attributesList = [
    ('type', 'chaine_Tru'),   #
    ('numero', 'entier_3282e0_Tru'),   #
    ('option', 'chaine_0bf8d2_Tru'),   #
    ('compo', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['compo'])
  _infoMain = ['TRAD2_example', 149]
  _infoAttr = {'compo': ['TRAD2_example', 153],
   'numero': ['TRAD2_example', 151],
   'option': ['TRAD2_example', 152],
   'type': ['TRAD2_example', 150]}
  _attributesList = check_append_list('morceau_equation_Tru', _attributesList, champ_post_de_champs_post_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('morceau_equation_Tru', _attributesSynonyms, champ_post_de_champs_post_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('morceau_equation_Tru', _infoAttr, champ_post_de_champs_post_Tru._infoAttr)
  _optionals.update(champ_post_de_champs_post_Tru._optionals)
  _helpDict = {
    'type':
          ('[chaine] can only be operateur for equation operators.', ''),
    'numero':
          ('[entier] numero will be 0 (diffusive operator) or 1 (convective operator).', ''),
    'option':
          ('[chaine(into=["stabilite","flux_bords","flux_surfacique_bords"])] option is stability \n'
           'for time steps or flux_bords for boundary fluxes or flux_surfacique_bords for boundary \n'
           'surfacic fluxes',
           ''),
    'compo':
          ('[entier] compo will specify the number component of the boundary flux (for boundary \n'
           'fluxes, in this case compo permits to specify the number component of the boundary \n'
           'flux choosen).',
           ''),
  }
  _helpDict = check_append_dict('morceau_equation_Tru', _helpDict, champ_post_de_champs_post_Tru._helpDict)
##### end morceau_equation_Tru class

class definition_champ_Tru(objet_lecture_Tru): #####
  r"""
  Keyword to create new complex field for advanced postprocessing.
  """
  # From: line 154, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('name', 'chaine_Tru'),   #
    ('champ_generique', '<inherited>champ_generique_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 154]
  _infoAttr = {'champ_generique': ['TRAD2_example', 156], 'name': ['TRAD2_example', 155]}
  _attributesList = check_append_list('definition_champ_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('definition_champ_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('definition_champ_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'name':
          ('[chaine] The name of the new created field.', ''),
    'champ_generique':
          ('[champ_generique_base] not_set', ''),
  }
  _helpDict = check_append_dict('definition_champ_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end definition_champ_Tru class

class definition_champs_Tru(ListOfBase_Tru): #####
  """
  List of definition champ
  """
  # From: line 157, in file 'TRAD2_example'
  _braces = 1
  _comma = 0
  _allowedClasses = ["definition_champ_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 157]
##### end definition_champs_Tru class

class postraitement_base_Tru(objet_lecture_Tru): #####
  r"""
  not_set
  """
  # From: line 158, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 158]
  _infoAttr = {}
  _attributesList = check_append_list('postraitement_base_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('postraitement_base_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('postraitement_base_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('postraitement_base_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end postraitement_base_Tru class

class postraitement_Tru(postraitement_base_Tru): #####
  r"""
  An object of post-processing (without name).
  """
  # From: line 1520, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['post_processing_Tru']
  _attributesList = [
    ('fichier', 'chaine_Tru'),   #
    ('format', 'chaine_8faf53_Tru'),   #
    ('domaine', 'chaine_Tru'),   #
    ('sous_domaine', 'chaine_Tru'),   #
    ('parallele', 'chaine_e7fecf_Tru'),   #
    ('definition_champs', 'definition_champs_Tru'),   #
    ('definition_champs_fichier', 'definition_champs_fichier_Tru'),   #
    ('sondes', 'sondes_Tru'),   #
    ('sondes_mobiles', 'sondes_Tru'),   #
    ('sondes_fichier', 'sondes_fichier_Tru'),   #
    ('deprecatedkeepduplicatedprobes', 'entier_3282e0_Tru'),   #
    ('champs', 'champs_posts_Tru'),   #
    ('statistiques', 'stats_posts_Tru'),   #
    ('statistiques_en_serie', 'stats_serie_posts_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'champs': ['fields'],
    'definition_champs_fichier': ['definition_champs_file'],
    'sondes': ['probes'],
    'sondes_fichier': ['probes_file'],
    'sondes_mobiles': ['mobile_probes'],
    'sous_domaine': ['sous_zone']
  }
  _optionals = set(['fichier',
   'format',
   'domaine',
   'sous_domaine',
   'parallele',
   'definition_champs',
   'definition_champs_fichier',
   'sondes',
   'sondes_mobiles',
   'sondes_fichier',
   'deprecatedkeepduplicatedprobes',
   'champs',
   'statistiques',
   'statistiques_en_serie'])
  _infoMain = ['TRAD2_example', 1520]
  _infoAttr = {'champs': ['TRAD2_example', 1532],
   'definition_champs': ['TRAD2_example', 1526],
   'definition_champs_fichier': ['TRAD2_example', 1527],
   'deprecatedkeepduplicatedprobes': ['TRAD2_example', 1531],
   'domaine': ['TRAD2_example', 1523],
   'fichier': ['TRAD2_example', 1521],
   'format': ['TRAD2_example', 1522],
   'parallele': ['TRAD2_example', 1525],
   'sondes': ['TRAD2_example', 1528],
   'sondes_fichier': ['TRAD2_example', 1530],
   'sondes_mobiles': ['TRAD2_example', 1529],
   'sous_domaine': ['TRAD2_example', 1524],
   'statistiques': ['TRAD2_example', 1533],
   'statistiques_en_serie': ['TRAD2_example', 1534]}
  _attributesList = check_append_list('postraitement_Tru', _attributesList, postraitement_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('postraitement_Tru', _attributesSynonyms, postraitement_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('postraitement_Tru', _infoAttr, postraitement_base_Tru._infoAttr)
  _optionals.update(postraitement_base_Tru._optionals)
  _helpDict = {
    'fichier':
          ('[chaine] Name of file.', ''),
    'format':
          ('[chaine(into=["lml","lata","lata_v2","med","med_major"])] This optional parameter \n'
           'specifies the format of the output file.\n'
           'The basename used for the output file is the basename of the data file.\n'
           'For the fmt parameter, choices are lml or lata.\n'
           'A short description of each format can be found below.\n'
           'The default value is lml.',
           ''),
    'domaine':
          ('[chaine] This optional parameter specifies the domain on which the data should be \n'
           'interpolated before it is written in the output file.\n'
           'The default is to write the data on the domain of the current problem (no interpolation).',
           ''),
    'sous_domaine':
          ('[chaine] This optional parameter specifies the sub_domaine on which the data should \n'
           'be interpolated before it is written in the output file.\n'
           'It is only available for sequential computation.',
           ''),
    'parallele':
          ('[chaine(into=["simple","multiple","mpi-io"])] Select simple (single file, sequential \n'
           'write), multiple (several files, parallel write), or mpi-io (single file, parallel \n'
           'write) for LATA format',
           ''),
    'definition_champs':
          ('[definition_champs] Keyword to create new or more complex field for advanced postprocessing.', ''),
    'definition_champs_fichier':
          ('[definition_champs_fichier] Definition_champs read from file.', ''),
    'sondes':
          ('[sondes] Probe.', ''),
    'sondes_mobiles':
          ('[sondes] Mobile probes useful for ALE, their positions will be updated in the mesh.', ''),
    'sondes_fichier':
          ('[sondes_fichier] Probe read in a file.', ''),
    'deprecatedkeepduplicatedprobes':
          ('[entier] Flag to not remove duplicated probes in .son files (1: keep duplicate probes, \n'
           '0: remove duplicate probes)',
           ''),
    'champs':
          ("[champs_posts] Field's write mode.", ''),
    'statistiques':
          ('[stats_posts] Statistics between two points fixed : start of integration time and \n'
           'end of integration time.',
           ''),
    'statistiques_en_serie':
          ('[stats_serie_posts] Statistics between two points not fixed : on period of integration.', ''),
  }
  _helpDict = check_append_dict('postraitement_Tru', _helpDict, postraitement_base_Tru._helpDict)
##### end postraitement_Tru class

class corps_postraitement_Tru(postraitement_Tru): #####
  r"""
  not_set
  """
  # From: line 159, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 159]
  _infoAttr = {}
  _attributesList = check_append_list('corps_postraitement_Tru', _attributesList, postraitement_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('corps_postraitement_Tru', _attributesSynonyms, postraitement_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('corps_postraitement_Tru', _infoAttr, postraitement_Tru._infoAttr)
  _optionals.update(postraitement_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('corps_postraitement_Tru', _helpDict, postraitement_Tru._helpDict)
##### end corps_postraitement_Tru class

class un_postraitement_Tru(objet_lecture_Tru): #####
  r"""
  An object of post-processing (with name).
  """
  # From: line 160, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('nom', 'chaine_Tru'),   #
    ('post', '<inherited>corps_postraitement_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 160]
  _infoAttr = {'nom': ['TRAD2_example', 161], 'post': ['TRAD2_example', 162]}
  _attributesList = check_append_list('un_postraitement_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('un_postraitement_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('un_postraitement_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'nom':
          ('[chaine] Name of the post-processing.', ''),
    'post':
          ('[corps_postraitement] Definition of the post-processing.', ''),
  }
  _helpDict = check_append_dict('un_postraitement_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end un_postraitement_Tru class

class postraitements_Tru(ListOfBase_Tru): #####
  """
  Keyword to use several results files.
  List of objects of post-processing (with name).
  """
  # From: line 163, in file 'TRAD2_example'
  _braces = -1
  _comma = 0
  _allowedClasses = ["un_postraitement_Tru"]
  _synonyms = ['post_processings_Tru']
  _infoMain = ['TRAD2_example', 163]
##### end postraitements_Tru class

class type_un_post_Tru(objet_lecture_Tru): #####
  r"""
  not_set
  """
  # From: line 164, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('type', 'chaine_e3346e_Tru'),   #
    ('post', 'un_postraitement_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 164]
  _infoAttr = {'post': ['TRAD2_example', 166], 'type': ['TRAD2_example', 165]}
  _attributesList = check_append_list('type_un_post_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('type_un_post_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('type_un_post_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'type':
          ('[chaine(into=["postraitement","post_processing"])] not_set', ''),
    'post':
          ('[un_postraitement] not_set', ''),
  }
  _helpDict = check_append_dict('type_un_post_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end type_un_post_Tru class

class type_postraitement_ft_lata_Tru(objet_lecture_Tru): #####
  r"""
  not_set
  """
  # From: line 167, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('type', 'chaine_1feca2_Tru'),   #
    ('nom', 'chaine_Tru'),   #
    ('bloc', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 167]
  _infoAttr = {'bloc': ['TRAD2_example', 170],
   'nom': ['TRAD2_example', 169],
   'type': ['TRAD2_example', 168]}
  _attributesList = check_append_list('type_postraitement_ft_lata_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('type_postraitement_ft_lata_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('type_postraitement_ft_lata_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'type':
          ('[chaine(into=["postraitement_ft_lata","postraitement_lata"])] not_set', ''),
    'nom':
          ('[chaine] Name of the post-processing.', ''),
    'bloc':
          ('[chaine] not_set', ''),
  }
  _helpDict = check_append_dict('type_postraitement_ft_lata_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end type_postraitement_ft_lata_Tru class

class un_postraitement_spec_Tru(objet_lecture_Tru): #####
  r"""
  An object of post-processing (with type +name).
  """
  # From: line 171, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('type_un_post', 'type_un_post_Tru'),   #
    ('type_postraitement_ft_lata', 'type_postraitement_ft_lata_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['type_un_post', 'type_postraitement_ft_lata'])
  _infoMain = ['TRAD2_example', 171]
  _infoAttr = {'type_postraitement_ft_lata': ['TRAD2_example', 173],
   'type_un_post': ['TRAD2_example', 172]}
  _attributesList = check_append_list('un_postraitement_spec_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('un_postraitement_spec_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('un_postraitement_spec_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'type_un_post':
          ('[type_un_post] not_set', ''),
    'type_postraitement_ft_lata':
          ('[type_postraitement_ft_lata] not_set', ''),
  }
  _helpDict = check_append_dict('un_postraitement_spec_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end un_postraitement_spec_Tru class

class liste_post_Tru(ListOfBase_Tru): #####
  """
  Keyword to use several results files.
  List of objects of post-processing (with name)
  """
  # From: line 174, in file 'TRAD2_example'
  _braces = -1
  _comma = 0
  _allowedClasses = ["un_postraitement_spec_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 174]
##### end liste_post_Tru class

class nom_postraitement_Tru(objet_lecture_Tru): #####
  r"""
  not_set
  """
  # From: line 175, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('nom', 'chaine_Tru'),   #
    ('post', '<inherited>postraitement_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 175]
  _infoAttr = {'nom': ['TRAD2_example', 176], 'post': ['TRAD2_example', 177]}
  _attributesList = check_append_list('nom_postraitement_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('nom_postraitement_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('nom_postraitement_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'nom':
          ('[chaine] Name of the post-processing.', ''),
    'post':
          ('[postraitement_base] the post', ''),
  }
  _helpDict = check_append_dict('nom_postraitement_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end nom_postraitement_Tru class

class liste_post_ok_Tru(ListOfBase_Tru): #####
  """
  Keyword to use several results files.
  List of objects of post-processing (with name)
  """
  # From: line 178, in file 'TRAD2_example'
  _braces = -1
  _comma = 0
  _allowedClasses = ["nom_postraitement_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 178]
##### end liste_post_ok_Tru class

class pb_gen_base_Tru(ConstrainBase_Tru): #####
  r"""
  Basic class for problems.
  """
  # From: line 179, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 179]
  _infoAttr = {}
  _helpDict = {}
##### end pb_gen_base_Tru class

class discretize_Tru(interprete_Tru): #####
  r"""
  Keyword to discretise a problem problem_name according to the discretization dis.
  
  IMPORTANT: A number of objects must be already associated (a domain, time scheme, 
  central object) prior to invoking the Discretize (Discretiser) keyword.
  The physical properties of this central object must also have been read.
  """
  # From: line 180, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['discretiser_Tru']
  _attributesList = [
    ('problem_name', 'ref_pb_gen_base_Tru'),   #
    ('dis', 'ref_discretisation_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 180]
  _infoAttr = {'dis': ['TRAD2_example', 182], 'problem_name': ['TRAD2_example', 181]}
  _attributesList = check_append_list('discretize_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('discretize_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('discretize_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'problem_name':
          ('[ref_pb_gen_base] Name of problem.', ''),
    'dis':
          ('[ref_discretisation_base] Name of the discretization object.', ''),
  }
  _helpDict = check_append_dict('discretize_Tru', _helpDict, interprete_Tru._helpDict)
##### end discretize_Tru class

class condinit_Tru(objet_lecture_Tru): #####
  r"""
  Initial condition.
  """
  # From: line 183, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('nom', 'chaine_Tru'),   #
    ('ch', '<inherited>field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 183]
  _infoAttr = {'ch': ['TRAD2_example', 185], 'nom': ['TRAD2_example', 184]}
  _attributesList = check_append_list('condinit_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('condinit_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('condinit_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'nom':
          ('[chaine] Name of initial condition field.', ''),
    'ch':
          ('[field_base] Type field and the initial values.', ''),
  }
  _helpDict = check_append_dict('condinit_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end condinit_Tru class

class condinits_Tru(ListOfBase_Tru): #####
  """
  Initial conditions.
  """
  # From: line 186, in file 'TRAD2_example'
  _braces = -1
  _comma = 0
  _allowedClasses = ["condinit_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 186]
##### end condinits_Tru class

class condlim_base_Tru(ConstrainBase_Tru): #####
  r"""
  Basic class of boundary conditions.
  """
  # From: line 187, in file 'TRAD2_example'
  _braces = 0
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 187]
  _infoAttr = {}
  _helpDict = {}
##### end condlim_base_Tru class

class condlimlu_Tru(objet_lecture_Tru): #####
  r"""
  Boundary condition specified.
  """
  # From: line 188, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('bord', 'chaine_Tru'),   #
    ('cl', '<inherited>condlim_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 188]
  _infoAttr = {'bord': ['TRAD2_example', 189], 'cl': ['TRAD2_example', 190]}
  _attributesList = check_append_list('condlimlu_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('condlimlu_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('condlimlu_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'bord':
          ('[chaine] Name of the edge where the boundary condition applies.', ''),
    'cl':
          ('[condlim_base] Boundary condition at the boundary called bord (edge).', ''),
  }
  _helpDict = check_append_dict('condlimlu_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end condlimlu_Tru class

class condlims_Tru(ListOfBase_Tru): #####
  """
  Boundary conditions.
  """
  # From: line 191, in file 'TRAD2_example'
  _braces = -1
  _comma = 0
  _allowedClasses = ["condlimlu_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 191]
##### end condlims_Tru class

class source_base_Tru(ConstrainBase_Tru): #####
  r"""
  Basic class of source terms introduced in the equation.
  """
  # From: line 192, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 192]
  _infoAttr = {}
  _helpDict = {}
##### end source_base_Tru class

class source_generique_Tru(source_base_Tru): #####
  r"""
  to define a source term depending on some discrete fields of the problem and (or) 
  analytic expression.
  It is expressed by the way of a generic field usually used for post-processing.
  """
  # From: line 193, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('champ', '<inherited>champ_generique_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 193]
  _infoAttr = {'champ': ['TRAD2_example', 194]}
  _attributesList = check_append_list('source_generique_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('source_generique_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('source_generique_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'champ':
          ('[champ_generique_base] the source field', ''),
  }
  _helpDict = check_append_dict('source_generique_Tru', _helpDict, source_base_Tru._helpDict)
##### end source_generique_Tru class

class sources_Tru(ListOfBase_Tru): #####
  """
  The sources.
  """
  # From: line 195, in file 'TRAD2_example'
  _braces = -1
  _comma = -1
  _allowedClasses = ["source_base_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 195]
##### end sources_Tru class

class nom_Tru(ConstrainBase_Tru): #####
  r"""
  Class to name the TRUST objects.
  """
  # From: line 196, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('mot', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['mot'])
  _infoMain = ['TRAD2_example', 196]
  _infoAttr = {'mot': ['TRAD2_example', 197]}
  _helpDict = {
    'mot':
          ('[chaine] Chain of characters.', ''),
  }
##### end nom_Tru class

class nom_anonyme_Tru(nom_Tru): #####
  r"""
  not_set
  """
  # From: line 198, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 198]
  _infoAttr = {}
  _attributesList = check_append_list('nom_anonyme_Tru', _attributesList, nom_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('nom_anonyme_Tru', _attributesSynonyms, nom_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('nom_anonyme_Tru', _infoAttr, nom_Tru._infoAttr)
  _optionals.update(nom_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('nom_anonyme_Tru', _helpDict, nom_Tru._helpDict)
##### end nom_anonyme_Tru class

class vect_nom_Tru(ListOfBase_Tru): #####
  """
  Vect of name.
  """
  # From: line 199, in file 'TRAD2_example'
  _braces = 0
  _comma = 0
  _allowedClasses = ["nom_anonyme_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 199]
##### end vect_nom_Tru class

class list_nom_Tru(ListOfBase_Tru): #####
  """
  List of name.
  """
  # From: line 200, in file 'TRAD2_example'
  _braces = 1
  _comma = 0
  _allowedClasses = ["nom_anonyme_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 200]
##### end list_nom_Tru class

class list_nom_virgule_Tru(ListOfBase_Tru): #####
  """
  List of name.
  """
  # From: line 201, in file 'TRAD2_example'
  _braces = 1
  _comma = 1
  _allowedClasses = ["nom_anonyme_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 201]
##### end list_nom_virgule_Tru class

class mailler_base_Tru(objet_lecture_Tru): #####
  r"""
  Basic class to mesh.
  """
  # From: line 202, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 202]
  _infoAttr = {}
  _attributesList = check_append_list('mailler_base_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('mailler_base_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('mailler_base_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('mailler_base_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end mailler_base_Tru class

class defbord_Tru(objet_lecture_Tru): #####
  r"""
  Class to define an edge.
  """
  # From: line 203, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 203]
  _infoAttr = {}
  _attributesList = check_append_list('defbord_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('defbord_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('defbord_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('defbord_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end defbord_Tru class

class defbord_3_Tru(defbord_Tru): #####
  r"""
  2-D edge (plane) in the 3-D space.
  """
  # From: line 213, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('dir', 'chaine_ad85af_Tru'),   #
    ('eq', 'chaine_52b4a3_Tru'),   #
    ('pos', 'floattant_820d8b_Tru'),   #
    ('pos2_min', 'floattant_820d8b_Tru'),   #
    ('inf1', 'chaine_4aae18_Tru'),   #
    ('dir2', 'chaine_036e4c_Tru'),   #
    ('inf2', 'chaine_4aae18_Tru'),   #
    ('pos2_max', 'floattant_820d8b_Tru'),   #
    ('pos3_min', 'floattant_820d8b_Tru'),   #
    ('inf3', 'chaine_4aae18_Tru'),   #
    ('dir3', 'chaine_241470_Tru'),   #
    ('inf4', 'chaine_4aae18_Tru'),   #
    ('pos3_max', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 213]
  _infoAttr = {'dir': ['TRAD2_example', 214],
   'dir2': ['TRAD2_example', 219],
   'dir3': ['TRAD2_example', 224],
   'eq': ['TRAD2_example', 215],
   'inf1': ['TRAD2_example', 218],
   'inf2': ['TRAD2_example', 220],
   'inf3': ['TRAD2_example', 223],
   'inf4': ['TRAD2_example', 225],
   'pos': ['TRAD2_example', 216],
   'pos2_max': ['TRAD2_example', 221],
   'pos2_min': ['TRAD2_example', 217],
   'pos3_max': ['TRAD2_example', 226],
   'pos3_min': ['TRAD2_example', 222]}
  _attributesList = check_append_list('defbord_3_Tru', _attributesList, defbord_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('defbord_3_Tru', _attributesSynonyms, defbord_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('defbord_3_Tru', _infoAttr, defbord_Tru._infoAttr)
  _optionals.update(defbord_Tru._optionals)
  _helpDict = {
    'dir':
          ('[chaine(into=["x","y","z"])] Edge is perpendicular to this direction.', ''),
    'eq':
          ('[chaine(into=["="])] Equality sign.', ''),
    'pos':
          ('[floattant] Position value.', ''),
    'pos2_min':
          ('[floattant] Minimal value.', ''),
    'inf1':
          ('[chaine(into=["<="])] Less than or equal to sign.', ''),
    'dir2':
          ('[chaine(into=["x","y"])] Edge is parallel to this direction.', ''),
    'inf2':
          ('[chaine(into=["<="])] Less than or equal to sign.', ''),
    'pos2_max':
          ('[floattant] Maximal value.', ''),
    'pos3_min':
          ('[floattant] Minimal value.', ''),
    'inf3':
          ('[chaine(into=["<="])] Less than or equal to sign.', ''),
    'dir3':
          ('[chaine(into=["y","z"])] Edge is parallel to this direction.', ''),
    'inf4':
          ('[chaine(into=["<="])] Less than or equal to sign.', ''),
    'pos3_max':
          ('[floattant] Maximal value.', ''),
  }
  _helpDict = check_append_dict('defbord_3_Tru', _helpDict, defbord_Tru._helpDict)
##### end defbord_3_Tru class

class bord_base_Tru(objet_lecture_Tru): #####
  r"""
  Basic class for block sides.
  Block sides that are neither edges nor connectors are not specified.
  The duplicate nodes of two blocks in contact are automatically recognized and deleted.
  """
  # From: line 227, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 227]
  _infoAttr = {}
  _attributesList = check_append_list('bord_base_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('bord_base_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('bord_base_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('bord_base_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end bord_base_Tru class

class bord_Tru(bord_base_Tru): #####
  r"""
  The block side is not in contact with another block and boundary conditions are applied 
  to it.
  """
  # From: line 228, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('nom', 'chaine_Tru'),   #
    ('defbord', 'defbord_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 228]
  _infoAttr = {'defbord': ['TRAD2_example', 230], 'nom': ['TRAD2_example', 229]}
  _attributesList = check_append_list('bord_Tru', _attributesList, bord_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('bord_Tru', _attributesSynonyms, bord_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('bord_Tru', _infoAttr, bord_base_Tru._infoAttr)
  _optionals.update(bord_base_Tru._optionals)
  _helpDict = {
    'nom':
          ('[chaine] Name of block side.', ''),
    'defbord':
          ('[defbord] Definition of block side.', ''),
  }
  _helpDict = check_append_dict('bord_Tru', _helpDict, bord_base_Tru._helpDict)
##### end bord_Tru class

class raccord_Tru(bord_base_Tru): #####
  r"""
  The block side is in contact with the block of another domain (case of two coupled 
  problems).
  """
  # From: line 231, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('type1', 'chaine_43a355_Tru'),   #
    ('type2', 'chaine_c39562_Tru'),   #
    ('nom', 'chaine_Tru'),   #
    ('defbord', 'defbord_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 231]
  _infoAttr = {'defbord': ['TRAD2_example', 235],
   'nom': ['TRAD2_example', 234],
   'type1': ['TRAD2_example', 232],
   'type2': ['TRAD2_example', 233]}
  _attributesList = check_append_list('raccord_Tru', _attributesList, bord_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('raccord_Tru', _attributesSynonyms, bord_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('raccord_Tru', _infoAttr, bord_base_Tru._infoAttr)
  _optionals.update(bord_base_Tru._optionals)
  _helpDict = {
    'type1':
          ('[chaine(into=["local","distant"])] Contact type.', ''),
    'type2':
          ('[chaine(into=["homogene"])] Contact type.', ''),
    'nom':
          ('[chaine] Name of block side.', ''),
    'defbord':
          ('[defbord] Definition of block side.', ''),
  }
  _helpDict = check_append_dict('raccord_Tru', _helpDict, bord_base_Tru._helpDict)
##### end raccord_Tru class

class internes_Tru(bord_base_Tru): #####
  r"""
  To indicate that the block has a set of internal faces (these faces will be duplicated 
  automatically by the program and will be processed in a manner similar to edge faces).
  
  Two boundaries with the same boundary conditions may have the same name (whether 
  or not they belong to the same block).
  
  The keyword Internes (Internal) must be used to execute a calculation with plates, 
  followed by the equation of the surface area covered by the plates.
  """
  # From: line 236, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('nom', 'chaine_Tru'),   #
    ('defbord', 'defbord_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 236]
  _infoAttr = {'defbord': ['TRAD2_example', 238], 'nom': ['TRAD2_example', 237]}
  _attributesList = check_append_list('internes_Tru', _attributesList, bord_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('internes_Tru', _attributesSynonyms, bord_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('internes_Tru', _infoAttr, bord_base_Tru._infoAttr)
  _optionals.update(bord_base_Tru._optionals)
  _helpDict = {
    'nom':
          ('[chaine] Name of block side.', ''),
    'defbord':
          ('[defbord] Definition of block side.', ''),
  }
  _helpDict = check_append_dict('internes_Tru', _helpDict, bord_base_Tru._helpDict)
##### end internes_Tru class

class list_bord_Tru(ListOfBase_Tru): #####
  """
  The block sides.
  """
  # From: line 239, in file 'TRAD2_example'
  _braces = 1
  _comma = 0
  _allowedClasses = ["bord_base_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 239]
##### end list_bord_Tru class

class pave_Tru(mailler_base_Tru): #####
  r"""
  Class to create a pave (block) with boundaries.
  """
  # From: line 240, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('name', 'chaine_Tru'),   #
    ('bloc', 'bloc_pave_Tru'),   #
    ('list_bord', 'list_bord_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 240]
  _infoAttr = {'bloc': ['TRAD2_example', 242],
   'list_bord': ['TRAD2_example', 243],
   'name': ['TRAD2_example', 241]}
  _attributesList = check_append_list('pave_Tru', _attributesList, mailler_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pave_Tru', _attributesSynonyms, mailler_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pave_Tru', _infoAttr, mailler_base_Tru._infoAttr)
  _optionals.update(mailler_base_Tru._optionals)
  _helpDict = {
    'name':
          ('[chaine] Name of the pave (block).', ''),
    'bloc':
          ('[bloc_pave] Definition of the pave (block).', ''),
    'list_bord':
          ('[list_bord] Domain boundaries definition.', ''),
  }
  _helpDict = check_append_dict('pave_Tru', _helpDict, mailler_base_Tru._helpDict)
##### end pave_Tru class

class epsilon_Tru(mailler_base_Tru): #####
  r"""
  Two points will be confused if the distance between them is less than eps.
  By default, eps is set to 1e-12.
  The keyword Epsilon allows an alternative value to be assigned to eps.
  """
  # From: line 244, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('eps', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 244]
  _infoAttr = {'eps': ['TRAD2_example', 245]}
  _attributesList = check_append_list('epsilon_Tru', _attributesList, mailler_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('epsilon_Tru', _attributesSynonyms, mailler_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('epsilon_Tru', _infoAttr, mailler_base_Tru._infoAttr)
  _optionals.update(mailler_base_Tru._optionals)
  _helpDict = {
    'eps':
          ('[floattant] New value of precision.', ''),
  }
  _helpDict = check_append_dict('epsilon_Tru', _helpDict, mailler_base_Tru._helpDict)
##### end epsilon_Tru class

class domain_Tru(mailler_base_Tru): #####
  r"""
  Class to reuse a domain.
  """
  # From: line 246, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 246]
  _infoAttr = {'domain_name': ['TRAD2_example', 247]}
  _attributesList = check_append_list('domain_Tru', _attributesList, mailler_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('domain_Tru', _attributesSynonyms, mailler_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('domain_Tru', _infoAttr, mailler_base_Tru._infoAttr)
  _optionals.update(mailler_base_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of domain.', ''),
  }
  _helpDict = check_append_dict('domain_Tru', _helpDict, mailler_base_Tru._helpDict)
##### end domain_Tru class

class list_bloc_mailler_Tru(ListOfBase_Tru): #####
  """
  List of block mesh.
  """
  # From: line 248, in file 'TRAD2_example'
  _braces = 1
  _comma = 1
  _allowedClasses = ["mailler_base_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 248]
##### end list_bloc_mailler_Tru class

class mailler_Tru(interprete_Tru): #####
  r"""
  The Mailler (Mesh) interpretor allows a Domain type object domaine to be meshed with 
  objects objet_1, objet_2, etc...
  """
  # From: line 249, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('domaine', 'ref_domaine_Tru'),   #
    ('bloc', 'list_bloc_mailler_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 249]
  _infoAttr = {'bloc': ['TRAD2_example', 251], 'domaine': ['TRAD2_example', 250]}
  _attributesList = check_append_list('mailler_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('mailler_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('mailler_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domaine':
          ('[ref_domaine] Name of domain.', ''),
    'bloc':
          ('[list_bloc_mailler] Instructions to mesh.', ''),
  }
  _helpDict = check_append_dict('mailler_Tru', _helpDict, interprete_Tru._helpDict)
##### end mailler_Tru class

class maillerparallel_Tru(interprete_Tru): #####
  r"""
  creates a parallel distributed hexaedral mesh of a parallelipipedic box.
  It is equivalent to creating a mesh with a single Pave, splitting it with Decouper 
  and reloading it in parallel with Scatter.
  It only works in 3D at this time.
  It can also be used for a sequential computation (with all NPARTS=1)}
  """
  # From: line 252, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('domain', 'ref_domaine_Tru'),   #
    ('nb_nodes', 'listentier_Tru'),   #
    ('splitting', 'listentier_Tru'),   #
    ('ghost_thickness', 'entier_3282e0_Tru'),   #
    ('perio_x', 'rien_Tru'),   #
    ('perio_y', 'rien_Tru'),   #
    ('perio_z', 'rien_Tru'),   #
    ('function_coord_x', 'chaine_Tru'),   #
    ('function_coord_y', 'chaine_Tru'),   #
    ('function_coord_z', 'chaine_Tru'),   #
    ('file_coord_x', 'chaine_Tru'),   #
    ('file_coord_y', 'chaine_Tru'),   #
    ('file_coord_z', 'chaine_Tru'),   #
    ('boundary_xmin', 'chaine_Tru'),   #
    ('boundary_xmax', 'chaine_Tru'),   #
    ('boundary_ymin', 'chaine_Tru'),   #
    ('boundary_ymax', 'chaine_Tru'),   #
    ('boundary_zmin', 'chaine_Tru'),   #
    ('boundary_zmax', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['perio_x',
   'perio_y',
   'perio_z',
   'function_coord_x',
   'function_coord_y',
   'function_coord_z',
   'file_coord_x',
   'file_coord_y',
   'file_coord_z',
   'boundary_xmin',
   'boundary_xmax',
   'boundary_ymin',
   'boundary_ymax',
   'boundary_zmin',
   'boundary_zmax'])
  _infoMain = ['TRAD2_example', 252]
  _infoAttr = {'boundary_xmax': ['TRAD2_example', 267],
   'boundary_xmin': ['TRAD2_example', 266],
   'boundary_ymax': ['TRAD2_example', 269],
   'boundary_ymin': ['TRAD2_example', 268],
   'boundary_zmax': ['TRAD2_example', 271],
   'boundary_zmin': ['TRAD2_example', 270],
   'domain': ['TRAD2_example', 253],
   'file_coord_x': ['TRAD2_example', 263],
   'file_coord_y': ['TRAD2_example', 264],
   'file_coord_z': ['TRAD2_example', 265],
   'function_coord_x': ['TRAD2_example', 260],
   'function_coord_y': ['TRAD2_example', 261],
   'function_coord_z': ['TRAD2_example', 262],
   'ghost_thickness': ['TRAD2_example', 256],
   'nb_nodes': ['TRAD2_example', 254],
   'perio_x': ['TRAD2_example', 257],
   'perio_y': ['TRAD2_example', 258],
   'perio_z': ['TRAD2_example', 259],
   'splitting': ['TRAD2_example', 255]}
  _attributesList = check_append_list('maillerparallel_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('maillerparallel_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('maillerparallel_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain':
          ('[ref_domaine] the name of the domain to mesh (it must be an empty domain object).', ''),
    'nb_nodes':
          ('[listentier] dimension defines the spatial dimension (currently only dimension=3 is \n'
           'supported), and nX, nY and nZ defines the total number of nodes in the mesh in each \n'
           'direction.',
           ''),
    'splitting':
          ('[listentier] dimension is the spatial dimension and npartsX, npartsY and npartsZ are \n'
           'the number of parts created.\n'
           'The product of the number of parts must be equal to the number of processors used \n'
           'for the computation.',
           ''),
    'ghost_thickness':
          ('[entier] he number of ghost cells (equivalent to the epaisseur_joint parameter of \nDecouper.',
           ''),
    'perio_x':
          ('[rien] change the splitting method to provide a valid mesh for periodic boundary conditions.', ''),
    'perio_y':
          ('[rien] change the splitting method to provide a valid mesh for periodic boundary conditions.', ''),
    'perio_z':
          ('[rien] change the splitting method to provide a valid mesh for periodic boundary conditions.', ''),
    'function_coord_x':
          ('[chaine] By default, the meshing algorithm creates nX nY nZ coordinates ranging between \n'
           '0 and 1 (eg a unity size box).\n'
           'If function_coord_x} is specified, it is used to transform the [0,1] segment to the \n'
           'coordinates of the nodes.\n'
           'funcX must be a function of the x variable only.',
           ''),
    'function_coord_y':
          ('[chaine] like function_coord_x for y', ''),
    'function_coord_z':
          ('[chaine] like function_coord_x for z', ''),
    'file_coord_x':
          ('[chaine] Keyword to read the Nx floating point values used as nodes coordinates in \nthe file.',
           ''),
    'file_coord_y':
          ('[chaine] idem file_coord_x for y', ''),
    'file_coord_z':
          ('[chaine] idem file_coord_x for z', ''),
    'boundary_xmin':
          ('[chaine] the name of the boundary at the minimum X direction.\n'
           'If it not provided, the default boundary names are xmin, xmax, ymin, ymax, zmin and \n'
           'zmax.\n'
           'If the mesh is periodic in a given direction, only the MIN boundary name is used, \n'
           'for both sides of the box.',
           ''),
    'boundary_xmax':
          ('[chaine] not_set', ''),
    'boundary_ymin':
          ('[chaine] not_set', ''),
    'boundary_ymax':
          ('[chaine] not_set', ''),
    'boundary_zmin':
          ('[chaine] not_set', ''),
    'boundary_zmax':
          ('[chaine] not_set', ''),
  }
  _helpDict = check_append_dict('maillerparallel_Tru', _helpDict, interprete_Tru._helpDict)
##### end maillerparallel_Tru class

class write_file_Tru(interprete_Tru): #####
  r"""
  Keyword to write the object of name name_obj to a file filename.
  Since the v1.6.3, the default format is now binary format file.
  """
  # From: line 272, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['ecrire_fichier_Tru', 'ecrire_fichier_bin_Tru']
  _attributesList = [
    ('name_obj', 'chaine_Tru'),   #
    ('filename', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 272]
  _infoAttr = {'filename': ['TRAD2_example', 274], 'name_obj': ['TRAD2_example', 273]}
  _attributesList = check_append_list('write_file_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('write_file_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('write_file_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'name_obj':
          ('[chaine] Name of the object to be written.', ''),
    'filename':
          ('[chaine] Name of the file.', ''),
  }
  _helpDict = check_append_dict('write_file_Tru', _helpDict, interprete_Tru._helpDict)
##### end write_file_Tru class

class ecrire_fichier_formatte_Tru(write_file_Tru): #####
  r"""
  Keyword to write the object of name name_obj to a file filename in ASCII format.
  """
  # From: line 275, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 275]
  _infoAttr = {}
  _attributesList = check_append_list('ecrire_fichier_formatte_Tru', _attributesList, write_file_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('ecrire_fichier_formatte_Tru', _attributesSynonyms, write_file_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('ecrire_fichier_formatte_Tru', _infoAttr, write_file_Tru._infoAttr)
  _optionals.update(write_file_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('ecrire_fichier_formatte_Tru', _helpDict, write_file_Tru._helpDict)
##### end ecrire_fichier_formatte_Tru class

class write_Tru(interprete_Tru): #####
  r"""
  Keyword to write the object of name name_obj to a standard outlet.
  """
  # From: line 276, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['ecrire_Tru']
  _attributesList = [
    ('name_obj', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 276]
  _infoAttr = {'name_obj': ['TRAD2_example', 277]}
  _attributesList = check_append_list('write_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('write_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('write_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'name_obj':
          ('[chaine] Name of the object to be written.', ''),
  }
  _helpDict = check_append_dict('write_Tru', _helpDict, interprete_Tru._helpDict)
##### end write_Tru class

class read_file_Tru(interprete_Tru): #####
  r"""
  Keyword to read the object name_obj contained in the file filename.
  
  This is notably used when the calculation domain has already been meshed and the 
  mesh contains the file filename, simply write read_file dom filename (where dom is 
  the name of the meshed domain).
  
  If the filename is ;, is to execute a data set given in the file of name name_obj 
  (a space must be entered between the semi-colon and the file name).
  """
  # From: line 278, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['lire_fichier_Tru']
  _attributesList = [
    ('name_obj', 'chaine_Tru'),   #
    ('filename', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 278]
  _infoAttr = {'filename': ['TRAD2_example', 280], 'name_obj': ['TRAD2_example', 279]}
  _attributesList = check_append_list('read_file_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('read_file_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('read_file_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'name_obj':
          ('[chaine] Name of the object to be read.', ''),
    'filename':
          ('[chaine] Name of the file.', ''),
  }
  _helpDict = check_append_dict('read_file_Tru', _helpDict, interprete_Tru._helpDict)
##### end read_file_Tru class

class read_unsupported_ascii_file_from_icem_Tru(read_file_Tru): #####
  r"""
  not_set
  """
  # From: line 281, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 281]
  _infoAttr = {}
  _attributesList = check_append_list('read_unsupported_ascii_file_from_icem_Tru', _attributesList, read_file_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('read_unsupported_ascii_file_from_icem_Tru', _attributesSynonyms, read_file_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('read_unsupported_ascii_file_from_icem_Tru', _infoAttr, read_file_Tru._infoAttr)
  _optionals.update(read_file_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('read_unsupported_ascii_file_from_icem_Tru', _helpDict, read_file_Tru._helpDict)
##### end read_unsupported_ascii_file_from_icem_Tru class

class read_file_bin_Tru(read_file_Tru): #####
  r"""
  Keyword to read an object name_obj in the unformatted type file filename.
  """
  # From: line 282, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['lire_fichier_bin_Tru', 'read_file_binary_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 282]
  _infoAttr = {}
  _attributesList = check_append_list('read_file_bin_Tru', _attributesList, read_file_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('read_file_bin_Tru', _attributesSynonyms, read_file_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('read_file_bin_Tru', _infoAttr, read_file_Tru._infoAttr)
  _optionals.update(read_file_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('read_file_bin_Tru', _helpDict, read_file_Tru._helpDict)
##### end read_file_bin_Tru class

class read_tgrid_Tru(interprete_Tru): #####
  r"""
  Keyword to reaf Tgrid/Gambit mesh files.
  2D (triangles or quadrangles) and 3D (tetra or hexa elements) meshes, may be read 
  by TRUST.
  """
  # From: line 283, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['lire_tgrid_Tru']
  _attributesList = [
    ('dom', 'ref_domaine_Tru'),   #
    ('filename', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 283]
  _infoAttr = {'dom': ['TRAD2_example', 284], 'filename': ['TRAD2_example', 285]}
  _attributesList = check_append_list('read_tgrid_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('read_tgrid_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('read_tgrid_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'dom':
          ('[ref_domaine] Name of domaine.', ''),
    'filename':
          ('[chaine] Name of file containing the mesh.', ''),
  }
  _helpDict = check_append_dict('read_tgrid_Tru', _helpDict, interprete_Tru._helpDict)
##### end read_tgrid_Tru class

class discretisation_base_Tru(ConstrainBase_Tru): #####
  r"""
  Basic class for space discretization of thermohydraulic turbulent problems.
  """
  # From: line 286, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 286]
  _infoAttr = {}
  _helpDict = {}
##### end discretisation_base_Tru class

class vdf_Tru(discretisation_base_Tru): #####
  r"""
  Finite difference volume discretization.
  """
  # From: line 287, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 287]
  _infoAttr = {}
  _attributesList = check_append_list('vdf_Tru', _attributesList, discretisation_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('vdf_Tru', _attributesSynonyms, discretisation_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('vdf_Tru', _infoAttr, discretisation_base_Tru._infoAttr)
  _optionals.update(discretisation_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('vdf_Tru', _helpDict, discretisation_base_Tru._helpDict)
##### end vdf_Tru class

class polymac_Tru(discretisation_base_Tru): #####
  r"""
  polymac discretization (polymac discretization that is not compatible with pb_multi).
  """
  # From: line 288, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 288]
  _infoAttr = {}
  _attributesList = check_append_list('polymac_Tru', _attributesList, discretisation_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('polymac_Tru', _attributesSynonyms, discretisation_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('polymac_Tru', _infoAttr, discretisation_base_Tru._infoAttr)
  _optionals.update(discretisation_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('polymac_Tru', _helpDict, discretisation_base_Tru._helpDict)
##### end polymac_Tru class

class polymac_p0p1nc_Tru(discretisation_base_Tru): #####
  r"""
  polymac_P0P1NC discretization (previously polymac discretization compatible with pb_multi).
  """
  # From: line 289, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 289]
  _infoAttr = {}
  _attributesList = check_append_list('polymac_p0p1nc_Tru', _attributesList, discretisation_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('polymac_p0p1nc_Tru', _attributesSynonyms, discretisation_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('polymac_p0p1nc_Tru', _infoAttr, discretisation_base_Tru._infoAttr)
  _optionals.update(discretisation_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('polymac_p0p1nc_Tru', _helpDict, discretisation_base_Tru._helpDict)
##### end polymac_p0p1nc_Tru class

class polymac_p0_Tru(discretisation_base_Tru): #####
  r"""
  polymac_p0 discretization (previously covimac discretization compatible with pb_multi).
  """
  # From: line 290, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 290]
  _infoAttr = {}
  _attributesList = check_append_list('polymac_p0_Tru', _attributesList, discretisation_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('polymac_p0_Tru', _attributesSynonyms, discretisation_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('polymac_p0_Tru', _infoAttr, discretisation_base_Tru._infoAttr)
  _optionals.update(discretisation_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('polymac_p0_Tru', _helpDict, discretisation_base_Tru._helpDict)
##### end polymac_p0_Tru class

class vef_Tru(discretisation_base_Tru): #####
  r"""
  Finite element volume discretization (P1NC/P1-bubble element).
  Since the 1.5.5 version, several new discretizations are available thanks to the 
  optional keyword Read.
  By default, the VEFPreP1B keyword is equivalent to the former VEFPreP1B formulation 
  (v1.5.4 and sooner).
  P0P1 (if used with the strong formulation for imposed pressure boundary) is equivalent 
  to VEFPreP1B but the convergence is slower.
  VEFPreP1B dis is equivalent to VEFPreP1B dis Read dis { P0 P1 Changement_de_base_P1Bulle 
  1 Cl_pression_sommet_faible 0 }
  """
  # From: line 291, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['vefprep1b_Tru']
  _attributesList = [
    ('p0', 'rien_Tru'),   #
    ('p1', 'rien_Tru'),   #
    ('pa', 'rien_Tru'),   #
    ('changement_de_base_p1bulle', 'entier_8bf71a_Tru'),   #
    ('cl_pression_sommet_faible', 'entier_8bf71a_Tru'),   #
    ('modif_div_face_dirichlet', 'entier_8bf71a_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['p0',
   'p1',
   'pa',
   'changement_de_base_p1bulle',
   'cl_pression_sommet_faible',
   'modif_div_face_dirichlet'])
  _infoMain = ['TRAD2_example', 291]
  _infoAttr = {'changement_de_base_p1bulle': ['TRAD2_example', 295],
   'cl_pression_sommet_faible': ['TRAD2_example', 296],
   'modif_div_face_dirichlet': ['TRAD2_example', 297],
   'p0': ['TRAD2_example', 292],
   'p1': ['TRAD2_example', 293],
   'pa': ['TRAD2_example', 294]}
  _attributesList = check_append_list('vef_Tru', _attributesList, discretisation_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('vef_Tru', _attributesSynonyms, discretisation_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('vef_Tru', _infoAttr, discretisation_base_Tru._infoAttr)
  _optionals.update(discretisation_base_Tru._optionals)
  _helpDict = {
    'p0':
          ('[rien] Pressure nodes are added on element centres', ''),
    'p1':
          ('[rien] Pressure nodes are added on vertices', ''),
    'pa':
          ('[rien] Only available in 3D, pressure nodes are added on bones', ''),
    'changement_de_base_p1bulle':
          ('[entier(into=[0,1])] This option may be used to have the P1NC/P0P1 formulation (value \n'
           'set to 0) or the P1NC/P1Bulle formulation (value set to 1, the default).',
           ''),
    'cl_pression_sommet_faible':
          ('[entier(into=[0,1])] This option is used to specify a strong formulation (value set \n'
           'to 0, the default) or a weak formulation (value set to 1) for an imposed pressure \n'
           'boundary condition.\n'
           'The first formulation converges quicker and is stable in general cases.\n'
           'The second formulation should be used if there are several outlet boundaries with \n'
           'Neumann condition (see Ecoulement_Neumann test case for example).',
           ''),
    'modif_div_face_dirichlet':
          ('[entier(into=[0,1])] This option (by default 0) is used to extend control volumes \n'
           'for the momentum equation.',
           ''),
  }
  _helpDict = check_append_dict('vef_Tru', _helpDict, discretisation_base_Tru._helpDict)
##### end vef_Tru class

class solve_Tru(interprete_Tru): #####
  r"""
  Interpretor to start calculation with TRUST.
  """
  # From: line 298, in file 'TRAD2_example'
  _braces = -2
  _synonyms = ['resoudre_Tru']
  _attributesList = [
    ('pb', 'ref_pb_gen_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 298]
  _infoAttr = {'pb': ['TRAD2_example', 299]}
  _attributesList = check_append_list('solve_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('solve_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('solve_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'pb':
          ('[ref_pb_gen_base] Name of problem to be solved.', ''),
  }
  _helpDict = check_append_dict('solve_Tru', _helpDict, interprete_Tru._helpDict)
##### end solve_Tru class

class execute_parallel_Tru(interprete_Tru): #####
  r"""
  This keyword allows to run several computations in parallel on processors allocated 
  to TRUST.
  The set of processors is split in N subsets and each subset will read and execute 
  a different data file.
  Error messages usualy written to stderr and stdout are redirected to .log files (journaling 
  must be activated).
  """
  # From: line 300, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('liste_cas', 'listchaine_Tru'),   #
    ('nb_procs', 'listentier_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['nb_procs'])
  _infoMain = ['TRAD2_example', 300]
  _infoAttr = {'liste_cas': ['TRAD2_example', 301], 'nb_procs': ['TRAD2_example', 302]}
  _attributesList = check_append_list('execute_parallel_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('execute_parallel_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('execute_parallel_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'liste_cas':
          ('[listchaine] N datafile1 ...\n'
           'datafileN.\n'
           'datafileX the name of a TRUST data file without the .data extension.',
           ''),
    'nb_procs':
          ('[listentier] nb_procs is the number of processors needed to run each data file.\n'
           'If not given, TRUST assumes that computations are sequential.',
           ''),
  }
  _helpDict = check_append_dict('execute_parallel_Tru', _helpDict, interprete_Tru._helpDict)
##### end execute_parallel_Tru class

class dimension_Tru(interprete_Tru): #####
  r"""
  Keyword allowing calculation dimensions to be set (2D or 3D), where dim is an integer 
  set to 2 or 3.
  This instruction is mandatory.
  """
  # From: line 304, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('dim', 'entier_216c6a_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 304]
  _infoAttr = {'dim': ['TRAD2_example', 305]}
  _attributesList = check_append_list('dimension_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('dimension_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('dimension_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'dim':
          ('[entier(into=[2,3])] Number of dimensions.', ''),
  }
  _helpDict = check_append_dict('dimension_Tru', _helpDict, interprete_Tru._helpDict)
##### end dimension_Tru class

class bidim_axi_Tru(interprete_Tru): #####
  r"""
  Keyword allowing a 2D calculation to be executed using axisymetric coordinates (R, 
  Z).
  If this instruction is not included, calculations are carried out using Cartesian 
  coordinates.
  """
  # From: line 306, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 306]
  _infoAttr = {}
  _attributesList = check_append_list('bidim_axi_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('bidim_axi_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('bidim_axi_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('bidim_axi_Tru', _helpDict, interprete_Tru._helpDict)
##### end bidim_axi_Tru class

class axi_Tru(interprete_Tru): #####
  r"""
  This keyword allows a 3D calculation to be executed using cylindrical coordinates 
  (R,$\jolitheta$,Z).
  If this instruction is not included, calculations are carried out using Cartesian 
  coordinates.
  """
  # From: line 307, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 307]
  _infoAttr = {}
  _attributesList = check_append_list('axi_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('axi_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('axi_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('axi_Tru', _helpDict, interprete_Tru._helpDict)
##### end axi_Tru class

class domaine_Tru(ConstrainBase_Tru): #####
  r"""
  Keyword to create a domain.
  """
  # From: line 308, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 308]
  _infoAttr = {}
  _helpDict = {}
##### end domaine_Tru class

class transformer_Tru(interprete_Tru): #####
  r"""
  Keyword to transform the coordinates of the geometry.
  
  Exemple to rotate your mesh by a 90o rotation and to scale the z coordinates by a 
  factor 2: Transformer domain_name -y -x 2*z
  """
  # From: line 309, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
    ('formule', 'listchainef_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 309]
  _infoAttr = {'domain_name': ['TRAD2_example', 310], 'formule': ['TRAD2_example', 311]}
  _attributesList = check_append_list('transformer_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('transformer_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('transformer_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of domain.', ''),
    'formule':
          ('[listchainef] Function_for_x Function_for_y \\[ Function_for z \\]', ''),
  }
  _helpDict = check_append_dict('transformer_Tru', _helpDict, interprete_Tru._helpDict)
##### end transformer_Tru class

class rotation_Tru(interprete_Tru): #####
  r"""
  Keyword to rotate the geometry of an arbitrary angle around an axis aligned with Ox, 
  Oy or Oz axis.
  """
  # From: line 312, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
    ('dir', 'chaine_ad85af_Tru'),   #
    ('coord1', 'floattant_820d8b_Tru'),   #
    ('coord2', 'floattant_820d8b_Tru'),   #
    ('angle', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 312]
  _infoAttr = {'angle': ['TRAD2_example', 317],
   'coord1': ['TRAD2_example', 315],
   'coord2': ['TRAD2_example', 316],
   'dir': ['TRAD2_example', 314],
   'domain_name': ['TRAD2_example', 313]}
  _attributesList = check_append_list('rotation_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('rotation_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('rotation_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of domain to wich the transformation is applied.', ''),
    'dir':
          ('[chaine(into=["x","y","z"])] X, Y or Z to indicate the direction of the rotation axis', ''),
    'coord1':
          ('[floattant] coordinates of the center of rotation in the plane orthogonal to the rotation \n'
           'axis.\n'
           'These coordinates must be specified in the direct triad sense.',
           ''),
    'coord2':
          ('[floattant] not_set', ''),
    'angle':
          ('[floattant] angle of rotation (in degrees)', ''),
  }
  _helpDict = check_append_dict('rotation_Tru', _helpDict, interprete_Tru._helpDict)
##### end rotation_Tru class

class dilate_Tru(interprete_Tru): #####
  r"""
  Keyword to multiply the whole coordinates of the geometry.
  """
  # From: line 318, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
    ('alpha', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 318]
  _infoAttr = {'alpha': ['TRAD2_example', 320], 'domain_name': ['TRAD2_example', 319]}
  _attributesList = check_append_list('dilate_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('dilate_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('dilate_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of domain.', ''),
    'alpha':
          ('[floattant] Value of dilatation coefficient.', ''),
  }
  _helpDict = check_append_dict('dilate_Tru', _helpDict, interprete_Tru._helpDict)
##### end dilate_Tru class

class class_generic_Tru(ConstrainBase_Tru): #####
  r"""
  not_set
  """
  # From: line 321, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 321]
  _infoAttr = {}
  _helpDict = {}
##### end class_generic_Tru class

class dt_start_Tru(class_generic_Tru): #####
  r"""
  not_set
  """
  # From: line 322, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 322]
  _infoAttr = {}
  _attributesList = check_append_list('dt_start_Tru', _attributesList, class_generic_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('dt_start_Tru', _attributesSynonyms, class_generic_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('dt_start_Tru', _infoAttr, class_generic_Tru._infoAttr)
  _optionals.update(class_generic_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('dt_start_Tru', _helpDict, class_generic_Tru._helpDict)
##### end dt_start_Tru class

class dt_calc_dt_calc_Tru(dt_start_Tru): #####
  r"""
  The time step at first iteration is calculated in agreement with CFL condition.
  """
  # From: line 323, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['dt_calc_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 323]
  _infoAttr = {}
  _attributesList = check_append_list('dt_calc_dt_calc_Tru', _attributesList, dt_start_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('dt_calc_dt_calc_Tru', _attributesSynonyms, dt_start_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('dt_calc_dt_calc_Tru', _infoAttr, dt_start_Tru._infoAttr)
  _optionals.update(dt_start_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('dt_calc_dt_calc_Tru', _helpDict, dt_start_Tru._helpDict)
##### end dt_calc_dt_calc_Tru class

class dt_calc_dt_min_Tru(dt_start_Tru): #####
  r"""
  The first iteration is based on dt_min.
  """
  # From: line 324, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['dt_min_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 324]
  _infoAttr = {}
  _attributesList = check_append_list('dt_calc_dt_min_Tru', _attributesList, dt_start_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('dt_calc_dt_min_Tru', _attributesSynonyms, dt_start_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('dt_calc_dt_min_Tru', _infoAttr, dt_start_Tru._infoAttr)
  _optionals.update(dt_start_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('dt_calc_dt_min_Tru', _helpDict, dt_start_Tru._helpDict)
##### end dt_calc_dt_min_Tru class

class dt_calc_dt_fixe_Tru(dt_start_Tru): #####
  r"""
  The first time step is fixed by the user (recommended when resuming calculation with 
  Crank Nicholson temporal scheme to ensure continuity).
  """
  # From: line 325, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['dt_fixe_Tru']
  _attributesList = [
    ('value', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 325]
  _infoAttr = {'value': ['TRAD2_example', 326]}
  _attributesList = check_append_list('dt_calc_dt_fixe_Tru', _attributesList, dt_start_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('dt_calc_dt_fixe_Tru', _attributesSynonyms, dt_start_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('dt_calc_dt_fixe_Tru', _infoAttr, dt_start_Tru._infoAttr)
  _optionals.update(dt_start_Tru._optionals)
  _helpDict = {
    'value':
          ('[floattant] first time step.', ''),
  }
  _helpDict = check_append_dict('dt_calc_dt_fixe_Tru', _helpDict, dt_start_Tru._helpDict)
##### end dt_calc_dt_fixe_Tru class

class schema_temps_base_Tru(ConstrainBase_Tru): #####
  r"""
  Basic class for time schemes.
  This scheme will be associated with a problem and the equations of this problem.
  """
  # From: line 1539, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = [
    ('tinit', 'floattant_820d8b_Tru'),   #
    ('tmax', 'floattant_820d8b_Tru'),   #
    ('tcpumax', 'floattant_820d8b_Tru'),   #
    ('dt_min', 'floattant_820d8b_Tru'),   #
    ('dt_max', 'chaine_Tru'),   #
    ('dt_sauv', 'floattant_820d8b_Tru'),   #
    ('dt_impr', 'floattant_820d8b_Tru'),   #
    ('facsec', 'floattant_820d8b_Tru'),   #
    ('seuil_statio', 'floattant_820d8b_Tru'),   #
    ('residuals', 'residuals_Tru'),   #
    ('diffusion_implicite', 'entier_3282e0_Tru'),   #
    ('seuil_diffusion_implicite', 'floattant_820d8b_Tru'),   #
    ('impr_diffusion_implicite', 'entier_3282e0_Tru'),   #
    ('impr_extremums', 'entier_3282e0_Tru'),   #
    ('no_error_if_not_converged_diffusion_implicite', 'entier_3282e0_Tru'),   #
    ('no_conv_subiteration_diffusion_implicite', 'entier_3282e0_Tru'),   #
    ('dt_start', '<inherited>dt_start_Tru'),   #
    ('nb_pas_dt_max', 'entier_3282e0_Tru'),   #
    ('niter_max_diffusion_implicite', 'entier_3282e0_Tru'),   #
    ('precision_impr', 'entier_3282e0_Tru'),   #
    ('periode_sauvegarde_securite_en_heures', 'floattant_820d8b_Tru'),   #
    ('no_check_disk_space', 'rien_Tru'),   #
    ('disable_progress', 'rien_Tru'),   #
    ('disable_dt_ev', 'rien_Tru'),   #
    ('gnuplot_header', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['tinit',
   'tmax',
   'tcpumax',
   'dt_min',
   'dt_max',
   'dt_sauv',
   'dt_impr',
   'facsec',
   'seuil_statio',
   'residuals',
   'diffusion_implicite',
   'seuil_diffusion_implicite',
   'impr_diffusion_implicite',
   'impr_extremums',
   'no_error_if_not_converged_diffusion_implicite',
   'no_conv_subiteration_diffusion_implicite',
   'dt_start',
   'nb_pas_dt_max',
   'niter_max_diffusion_implicite',
   'precision_impr',
   'periode_sauvegarde_securite_en_heures',
   'no_check_disk_space',
   'disable_progress',
   'disable_dt_ev',
   'gnuplot_header'])
  _infoMain = ['TRAD2_example', 1539]
  _infoAttr = {'diffusion_implicite': ['TRAD2_example', 1550],
   'disable_dt_ev': ['TRAD2_example', 1563],
   'disable_progress': ['TRAD2_example', 1562],
   'dt_impr': ['TRAD2_example', 1546],
   'dt_max': ['TRAD2_example', 1544],
   'dt_min': ['TRAD2_example', 1543],
   'dt_sauv': ['TRAD2_example', 1545],
   'dt_start': ['TRAD2_example', 1556],
   'facsec': ['TRAD2_example', 1547],
   'gnuplot_header': ['TRAD2_example', 1564],
   'impr_diffusion_implicite': ['TRAD2_example', 1552],
   'impr_extremums': ['TRAD2_example', 1553],
   'nb_pas_dt_max': ['TRAD2_example', 1557],
   'niter_max_diffusion_implicite': ['TRAD2_example', 1558],
   'no_check_disk_space': ['TRAD2_example', 1561],
   'no_conv_subiteration_diffusion_implicite': ['TRAD2_example', 1555],
   'no_error_if_not_converged_diffusion_implicite': ['TRAD2_example', 1554],
   'periode_sauvegarde_securite_en_heures': ['TRAD2_example', 1560],
   'precision_impr': ['TRAD2_example', 1559],
   'residuals': ['TRAD2_example', 1549],
   'seuil_diffusion_implicite': ['TRAD2_example', 1551],
   'seuil_statio': ['TRAD2_example', 1548],
   'tcpumax': ['TRAD2_example', 1542],
   'tinit': ['TRAD2_example', 1540],
   'tmax': ['TRAD2_example', 1541]}
  _helpDict = {
    'tinit':
          ('[floattant] Value of initial calculation time (0 by default).', ''),
    'tmax':
          ('[floattant] Time during which the calculation will be stopped (1e30s by default).', ''),
    'tcpumax':
          ('[floattant] CPU time limit (must be specified in hours) for which the calculation \n'
           'is stopped (1e30s by default).',
           ''),
    'dt_min':
          ('[floattant] Minimum calculation time step (1e-16s by default).', ''),
    'dt_max':
          ('[chaine] Maximum calculation time step as function of time (1e30s by default).', ''),
    'dt_sauv':
          ('[floattant] Save time step value (1e30s by default).\n'
           'Every dt_sauv, fields are saved in the .sauv file.\n'
           'The file contains all the information saved over time.\n'
           'If this instruction is not entered, results are saved only upon calculation completion.\n'
           'To disable the writing of the .sauv files, you must specify 0.\n'
           'Note that dt_sauv is in terms of physical time (not cpu time).',
           ''),
    'dt_impr':
          ('[floattant] Scheme parameter printing time step in time (1e30s by default).\n'
           'The time steps and the flux balances are printed (incorporated onto every side of \n'
           'processed domains) into the .out file.',
           ''),
    'facsec':
          ('[floattant] Value assigned to the safety factor for the time step (1.\n'
           'by default).\n'
           'The time step calculated is multiplied by the safety factor.\n'
           'The first thing to try when a calculation does not converge with an explicit time \n'
           'scheme is to reduce the facsec to 0.5.\n'
           '\n'
           'Warning: Some schemes needs a facsec lower than 1 (0.5 is a good start), for example \n'
           'Schema_Adams_Bashforth_order_3.',
           ''),
    'seuil_statio':
          ('[floattant] Value of the convergence threshold (1e-12 by default).\n'
           'Problems using this type of time scheme converge when the derivatives dGi/dt NL1 \n'
           'of all the unknown transported values Gi have a combined absolute value less than \n'
           'this value.\n'
           'This is the keyword used to set the permanent rating threshold.',
           ''),
    'residuals':
          ('[residuals] To specify how the residuals will be computed (default max norm, possible \n'
           'to choose L2-norm instead).',
           ''),
    'diffusion_implicite':
          ('[entier] Keyword to make the diffusive term in the Navier-Stokes equations implicit \n'
           '(in this case, it should be set to 1).\n'
           'The stability time step is then only based on the convection time step '
           '(dt=facsec*dt_convection).\n'
           'Thus, in some circumstances, an important gain is achieved with respect to the time \n'
           'step (large diffusion with respect to convection on tightened meshes).\n'
           'Caution: It is however recommended that the user avoids exceeding the convection \n'
           'time step by selecting a too large facsec value.\n'
           'Start with a facsec value of 1 and then increase it gradually if you wish to accelerate \n'
           'calculation.\n'
           'In addition, for a natural convection calculation with a zero initial velocity, in \n'
           'the first time step, the convection time is infinite and therefore dt=facsec*dt_max.',
           ''),
    'seuil_diffusion_implicite':
          ('[floattant] This keyword changes the default value (1e-6) of convergency criteria \n'
           'for the resolution by conjugate gradient used for implicit diffusion.',
           ''),
    'impr_diffusion_implicite':
          ('[entier] Unactivate (default) or not the printing of the convergence during the resolution \n'
           'of the conjugate gradient.',
           ''),
    'impr_extremums':
          ('[entier] Print unknowns extremas', ''),
    'no_error_if_not_converged_diffusion_implicite':
          ('[entier] not_set', ''),
    'no_conv_subiteration_diffusion_implicite':
          ('[entier] not_set', ''),
    'dt_start':
          ('[dt_start] dt_start dt_min : the first iteration is based on dt_min.\n'
           '\n'
           'dt_start dt_calc : the time step at first iteration is calculated in agreement with \n'
           'CFL condition.\n'
           '\n'
           'dt_start dt_fixe value : the first time step is fixed by the user (recommended when \n'
           'resuming calculation with Crank Nicholson temporal scheme to ensure continuity).\n'
           '\n'
           'By default, the first iteration is based on dt_calc.',
           ''),
    'nb_pas_dt_max':
          ('[entier] Maximum number of calculation time steps (1e9 by default).', ''),
    'niter_max_diffusion_implicite':
          ('[entier] This keyword changes the default value (number of unknowns) of the maximal \n'
           'iterations number in the conjugate gradient method used for implicit diffusion.',
           ''),
    'precision_impr':
          ('[entier] Optional keyword to define the digit number for flux values printed into \n'
           '.out files (by default 3).',
           ''),
    'periode_sauvegarde_securite_en_heures':
          ('[floattant] To change the default period (23 hours) between the save of the fields \n'
           'in .sauv file.',
           ''),
    'no_check_disk_space':
          ('[rien] To disable the check of the available amount of disk space during the calculation.', ''),
    'disable_progress':
          ('[rien] To disable the writing of the .progress file.', ''),
    'disable_dt_ev':
          ('[rien] To disable the writing of the .dt_ev file.', ''),
    'gnuplot_header':
          ('[entier] Optional keyword to modify the header of the .out files.\n'
           'Allows to use the column title instead of columns number.',
           ''),
  }
##### end schema_temps_base_Tru class

class euler_scheme_Tru(schema_temps_base_Tru): #####
  r"""
  This is the Euler explicit scheme.
  """
  # From: line 327, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['schema_euler_explicite_Tru', 'scheme_euler_explicit_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 327]
  _infoAttr = {}
  _attributesList = check_append_list('euler_scheme_Tru', _attributesList, schema_temps_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('euler_scheme_Tru', _attributesSynonyms, schema_temps_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('euler_scheme_Tru', _infoAttr, schema_temps_base_Tru._infoAttr)
  _optionals.update(schema_temps_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('euler_scheme_Tru', _helpDict, schema_temps_base_Tru._helpDict)
##### end euler_scheme_Tru class

class schema_predictor_corrector_Tru(schema_temps_base_Tru): #####
  r"""
  This is the predictor-corrector scheme (second order).
  It is more accurate and economic than MacCormack scheme.
  It gives best results with a second ordre convective scheme like quick, centre (VDF).
  """
  # From: line 328, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 328]
  _infoAttr = {}
  _attributesList = check_append_list('schema_predictor_corrector_Tru', _attributesList, schema_temps_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('schema_predictor_corrector_Tru', _attributesSynonyms, schema_temps_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('schema_predictor_corrector_Tru', _infoAttr, schema_temps_base_Tru._infoAttr)
  _optionals.update(schema_temps_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('schema_predictor_corrector_Tru', _helpDict, schema_temps_base_Tru._helpDict)
##### end schema_predictor_corrector_Tru class

class solveur_implicite_base_Tru(ConstrainBase_Tru): #####
  r"""
  Class for solver in the situation where the time scheme is the implicit scheme.
  Solver allows equation diffusion and convection operators to be set as implicit terms.
  """
  # From: line 329, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 329]
  _infoAttr = {}
  _helpDict = {}
##### end solveur_implicite_base_Tru class

class solveur_sys_base_Tru(class_generic_Tru): #####
  r"""
  Basic class to solve the linear system.
  """
  # From: line 330, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 330]
  _infoAttr = {}
  _attributesList = check_append_list('solveur_sys_base_Tru', _attributesList, class_generic_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('solveur_sys_base_Tru', _attributesSynonyms, class_generic_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('solveur_sys_base_Tru', _infoAttr, class_generic_Tru._infoAttr)
  _optionals.update(class_generic_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('solveur_sys_base_Tru', _helpDict, class_generic_Tru._helpDict)
##### end solveur_sys_base_Tru class

class sch_cn_iteratif_Tru(schema_temps_base_Tru): #####
  r"""
  The Crank-Nicholson method of second order accuracy.
  A mid-point rule formulation is used (Euler-centered scheme).
  The basic scheme is: $$u(t+1) = u(t) + du/dt(t+1/2)*dt$$ The estimation of the time 
  derivative du/dt at the level (t+1/2) is obtained either by iterative process.
  The time derivative du/dt at the level (t+1/2) is calculated iteratively with a simple 
  under-relaxations method.
  Since the method is implicit, neither the cfl nor the fourier stability criteria 
  must be respected.
  The time step is calculated in a way that the iterative procedure converges with 
  the less iterations as possible.
  
  Remark : for stationary or RANS calculations, no limitation can be given for time 
  step through high value of facsec_max parameter (for instance : facsec_max 1000).
  In counterpart, for LES calculations, high values of facsec_max may engender numerical 
  instabilities.
  """
  # From: line 331, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('niter_min', 'entier_3282e0_Tru'),   #
    ('niter_max', 'entier_3282e0_Tru'),   #
    ('niter_avg', 'entier_3282e0_Tru'),   #
    ('facsec_max', 'floattant_820d8b_Tru'),   #
    ('seuil', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['niter_min', 'niter_max', 'niter_avg', 'facsec_max', 'seuil'])
  _infoMain = ['TRAD2_example', 331]
  _infoAttr = {'facsec_max': ['TRAD2_example', 335],
   'niter_avg': ['TRAD2_example', 334],
   'niter_max': ['TRAD2_example', 333],
   'niter_min': ['TRAD2_example', 332],
   'seuil': ['TRAD2_example', 336]}
  _attributesList = check_append_list('sch_cn_iteratif_Tru', _attributesList, schema_temps_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('sch_cn_iteratif_Tru', _attributesSynonyms, schema_temps_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('sch_cn_iteratif_Tru', _infoAttr, schema_temps_base_Tru._infoAttr)
  _optionals.update(schema_temps_base_Tru._optionals)
  _helpDict = {
    'niter_min':
          ('[entier] minimal number of p-iterations to satisfy convergence criteria (2 by default)', ''),
    'niter_max':
          ('[entier] number of maximum p-iterations allowed to satisfy convergence criteria (6 \nby default)',
           ''),
    'niter_avg':
          ('[entier] threshold of p-iterations (3 by default).\n'
           'If the number of p-iterations is greater than niter_avg, facsec is reduced, if lesser \n'
           'than niter_avg, facsec is increased (but limited by the facsec_max value).',
           ''),
    'facsec_max':
          ('[floattant] maximum ratio allowed between dynamical time step returned by iterative \n'
           'process and stability time returned by CFL condition (2 by default).',
           ''),
    'seuil':
          ('[floattant] criteria for ending iterative process (Max( || u(p) - u(p-1)||/Max || \n'
           'u(p) ||) < seuil) (0.001 by default)',
           ''),
  }
  _helpDict = check_append_dict('sch_cn_iteratif_Tru', _helpDict, schema_temps_base_Tru._helpDict)
##### end sch_cn_iteratif_Tru class

class sch_cn_ex_iteratif_Tru(sch_cn_iteratif_Tru): #####
  r"""
  This keyword also describes a Crank-Nicholson method of second order accuracy but 
  here, for scalars, because of instablities encountered when dt>dt_CFL, the Crank Nicholson 
  scheme is not applied to scalar quantities.
  Scalars are treated according to Euler-Explicite scheme at the end of the CN treatment 
  for velocity flow fields (by doing p Euler explicite under-iterations at dt<=dt_CFL).
  Parameters are the sames (but default values may change) compare to the Sch_CN_iterative 
  scheme plus a relaxation keyword: niter_min (2 by default), niter_max (6 by default), 
  niter_avg (3 by default), facsec_max (20 by default), seuil (0.05 by default)
  """
  # From: line 337, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('omega', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['omega'])
  _infoMain = ['TRAD2_example', 337]
  _infoAttr = {'omega': ['TRAD2_example', 338]}
  _attributesList = check_append_list('sch_cn_ex_iteratif_Tru', _attributesList, sch_cn_iteratif_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('sch_cn_ex_iteratif_Tru', _attributesSynonyms, sch_cn_iteratif_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('sch_cn_ex_iteratif_Tru', _infoAttr, sch_cn_iteratif_Tru._infoAttr)
  _optionals.update(sch_cn_iteratif_Tru._optionals)
  _helpDict = {
    'omega':
          ('[floattant] relaxation factor (0.1 by default)', ''),
  }
  _helpDict = check_append_dict('sch_cn_ex_iteratif_Tru', _helpDict, sch_cn_iteratif_Tru._helpDict)
##### end sch_cn_ex_iteratif_Tru class

class leap_frog_Tru(schema_temps_base_Tru): #####
  r"""
  This is the leap-frog scheme.
  """
  # From: line 339, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 339]
  _infoAttr = {}
  _attributesList = check_append_list('leap_frog_Tru', _attributesList, schema_temps_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('leap_frog_Tru', _attributesSynonyms, schema_temps_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('leap_frog_Tru', _infoAttr, schema_temps_base_Tru._infoAttr)
  _optionals.update(schema_temps_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('leap_frog_Tru', _helpDict, schema_temps_base_Tru._helpDict)
##### end leap_frog_Tru class

class solveur_lineaire_std_Tru(solveur_implicite_base_Tru): #####
  r"""
  not_set
  """
  # From: line 340, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('solveur', '<inherited>solveur_sys_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['solveur'])
  _infoMain = ['TRAD2_example', 340]
  _infoAttr = {'solveur': ['TRAD2_example', 341]}
  _attributesList = check_append_list('solveur_lineaire_std_Tru', _attributesList, solveur_implicite_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('solveur_lineaire_std_Tru', _attributesSynonyms, solveur_implicite_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('solveur_lineaire_std_Tru', _infoAttr, solveur_implicite_base_Tru._infoAttr)
  _optionals.update(solveur_implicite_base_Tru._optionals)
  _helpDict = {
    'solveur':
          ('[solveur_sys_base] not_set', ''),
  }
  _helpDict = check_append_dict('solveur_lineaire_std_Tru', _helpDict, solveur_implicite_base_Tru._helpDict)
##### end solveur_lineaire_std_Tru class

class simpler_Tru(solveur_implicite_base_Tru): #####
  r"""
  Simpler method for incompressible systems.
  """
  # From: line 342, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('seuil_convergence_implicite', 'floattant_820d8b_Tru'),   #
    ('seuil_convergence_solveur', 'floattant_820d8b_Tru'),   #
    ('seuil_generation_solveur', 'floattant_820d8b_Tru'),   #
    ('seuil_verification_solveur', 'floattant_820d8b_Tru'),   #
    ('seuil_test_preliminaire_solveur', 'floattant_820d8b_Tru'),   #
    ('solveur', '<inherited>solveur_sys_base_Tru'),   #
    ('no_qdm', 'rien_Tru'),   #
    ('nb_it_max', 'entier_3282e0_Tru'),   #
    ('controle_residu', 'rien_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['seuil_convergence_solveur',
   'seuil_generation_solveur',
   'seuil_verification_solveur',
   'seuil_test_preliminaire_solveur',
   'solveur',
   'no_qdm',
   'nb_it_max',
   'controle_residu'])
  _infoMain = ['TRAD2_example', 342]
  _infoAttr = {'controle_residu': ['TRAD2_example', 351],
   'nb_it_max': ['TRAD2_example', 350],
   'no_qdm': ['TRAD2_example', 349],
   'seuil_convergence_implicite': ['TRAD2_example', 343],
   'seuil_convergence_solveur': ['TRAD2_example', 344],
   'seuil_generation_solveur': ['TRAD2_example', 345],
   'seuil_test_preliminaire_solveur': ['TRAD2_example', 347],
   'seuil_verification_solveur': ['TRAD2_example', 346],
   'solveur': ['TRAD2_example', 348]}
  _attributesList = check_append_list('simpler_Tru', _attributesList, solveur_implicite_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('simpler_Tru', _attributesSynonyms, solveur_implicite_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('simpler_Tru', _infoAttr, solveur_implicite_base_Tru._infoAttr)
  _optionals.update(solveur_implicite_base_Tru._optionals)
  _helpDict = {
    'seuil_convergence_implicite':
          ('[floattant] Keyword to set the value of the convergence criteria for the resolution \n'
           'of the implicit system build to solve either the Navier_Stokes equation (only for \n'
           'Simple and Simpler algorithms) or a scalar equation.\n'
           'It is adviced to use the default value (1e6) to solve the implicit system only once \n'
           'by time step.\n'
           'This value must be decreased when a coupling between problems is considered.',
           ''),
    'seuil_convergence_solveur':
          ('[floattant] value of the convergence criteria for the resolution of the implicit system \n'
           'build by solving several times per time step the Navier_Stokes equation and the scalar \n'
           'equations if any.\n'
           'This value MUST be used when a coupling between problems is considered (should be \n'
           'set to a value typically of 0.1 or 0.01).',
           ''),
    'seuil_generation_solveur':
          ('[floattant] Option to create a GMRES solver and use vrel as the convergence threshold \n'
           '(implicit linear system Ax=B will be solved if residual error ||Ax-B|| is lesser than \n'
           'vrel).',
           ''),
    'seuil_verification_solveur':
          ('[floattant] Option to check if residual error ||Ax-B|| is lesser than vrel after the \n'
           'implicit linear system Ax=B has been solved.',
           ''),
    'seuil_test_preliminaire_solveur':
          ('[floattant] Option to decide if the implicit linear system Ax=B should be solved by \n'
           'checking if the residual error ||Ax-B|| is bigger than vrel.',
           ''),
    'solveur':
          ('[solveur_sys_base] Method (different from the default one, Gmres with diagonal '
           'preconditioning) \n'
           'to solve the linear system.',
           ''),
    'no_qdm':
          ('[rien] Keyword to not solve qdm equation (and turbulence models of these equation).', ''),
    'nb_it_max':
          ('[entier] Keyword to set the maximum iterations number for the Gmres.', ''),
    'controle_residu':
          ('[rien] Keyword of Boolean type (by default 0).\n'
           'If set to 1, the convergence occurs if the residu suddenly increases.',
           ''),
  }
  _helpDict = check_append_dict('simpler_Tru', _helpDict, solveur_implicite_base_Tru._helpDict)
##### end simpler_Tru class

class piso_Tru(simpler_Tru): #####
  r"""
  Piso (Pressure Implicit with Split Operator) - method to solve N_S.
  """
  # From: line 352, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('seuil_convergence_implicite', 'floattant_820d8b_Tru'),   #
    ('nb_corrections_max', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['seuil_convergence_implicite', 'nb_corrections_max'])
  _infoMain = ['TRAD2_example', 352]
  _infoAttr = {'nb_corrections_max': ['TRAD2_example', 354],
   'seuil_convergence_implicite': ['TRAD2_example', 353]}
  _attributesList = check_append_list('piso_Tru', _attributesList, simpler_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('piso_Tru', _attributesSynonyms, simpler_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('piso_Tru', _infoAttr, simpler_Tru._infoAttr)
  _optionals.update(simpler_Tru._optionals)
  _helpDict = {
    'seuil_convergence_implicite':
          ('[floattant] Convergence criteria.', ''),
    'nb_corrections_max':
          ('[entier] Maximum number of corrections performed by the PISO algorithm to achieve \n'
           'the projection of the velocity field.\n'
           'The algorithm may perform less corrections then nb_corrections_max if the accuracy \n'
           'of the projection is sufficient.\n'
           '(By default nb_corrections_max is set to 21).',
           ''),
  }
  _helpDict = check_append_dict('piso_Tru', _helpDict, simpler_Tru._helpDict)
##### end piso_Tru class

class implicite_Tru(piso_Tru): #####
  r"""
  similar to PISO, but as it looks like a simplified solver, it will use fewer timesteps.
  But it may run faster because the pressure matrix is not re-assembled and thus provides 
  CPU gains.
  """
  # From: line 355, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 355]
  _infoAttr = {}
  _attributesList = check_append_list('implicite_Tru', _attributesList, piso_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('implicite_Tru', _attributesSynonyms, piso_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('implicite_Tru', _infoAttr, piso_Tru._infoAttr)
  _optionals.update(piso_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('implicite_Tru', _helpDict, piso_Tru._helpDict)
##### end implicite_Tru class

class simple_Tru(piso_Tru): #####
  r"""
  SIMPLE type algorithm
  """
  # From: line 356, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('relax_pression', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['relax_pression'])
  _infoMain = ['TRAD2_example', 356]
  _infoAttr = {'relax_pression': ['TRAD2_example', 357]}
  _attributesList = check_append_list('simple_Tru', _attributesList, piso_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('simple_Tru', _attributesSynonyms, piso_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('simple_Tru', _infoAttr, piso_Tru._infoAttr)
  _optionals.update(piso_Tru._optionals)
  _helpDict = {
    'relax_pression':
          ('[floattant] Value between 0 and 1 (by default 1), this keyword is used only by the \n'
           'SIMPLE algorithm for relaxing the increment of pressure.',
           ''),
  }
  _helpDict = check_append_dict('simple_Tru', _helpDict, piso_Tru._helpDict)
##### end simple_Tru class

class solveur_u_p_Tru(simple_Tru): #####
  r"""
  similar to simple.
  """
  # From: line 358, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 358]
  _infoAttr = {}
  _attributesList = check_append_list('solveur_u_p_Tru', _attributesList, simple_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('solveur_u_p_Tru', _attributesSynonyms, simple_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('solveur_u_p_Tru', _infoAttr, simple_Tru._infoAttr)
  _optionals.update(simple_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('solveur_u_p_Tru', _helpDict, simple_Tru._helpDict)
##### end solveur_u_p_Tru class

class schema_implicite_base_Tru(schema_temps_base_Tru): #####
  r"""
  Basic class for implicite time scheme.
  """
  # From: line 359, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = [
    ('max_iter_implicite', 'entier_3282e0_Tru'),   #
    ('solveur', '<inherited>solveur_implicite_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['max_iter_implicite'])
  _infoMain = ['TRAD2_example', 359]
  _infoAttr = {'max_iter_implicite': ['TRAD2_example', 360],
   'solveur': ['TRAD2_example', 361]}
  _attributesList = check_append_list('schema_implicite_base_Tru', _attributesList, schema_temps_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('schema_implicite_base_Tru', _attributesSynonyms, schema_temps_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('schema_implicite_base_Tru', _infoAttr, schema_temps_base_Tru._infoAttr)
  _optionals.update(schema_temps_base_Tru._optionals)
  _helpDict = {
    'max_iter_implicite':
          ('[entier] Maximum number of iterations allowed for the solver (by default 200).', ''),
    'solveur':
          ('[solveur_implicite_base] This keyword is used to designate the solver selected in \n'
           'the situation where the time scheme is an implicit scheme.\n'
           'solver is the name of the solver that allows equation diffusion and convection operators \n'
           'to be set as implicit terms.\n'
           'Keywords corresponding to this functionality are Simple (SIMPLE type algorithm), \n'
           'Simpler (SIMPLER type algorithm) for incompressible systems, Piso (Pressure Implicit \n'
           'with Split Operator), and Implicite (similar to PISO, but as it looks like a simplified \n'
           'solver, it will use fewer timesteps, and ICE (for PB_multiphase).\n'
           'But it may run faster because the pressure matrix is not re-assembled and thus provides \n'
           'CPU gains.\n'
           '\n'
           'Advice: Since the 1.6.0 version, we recommend to use first the Implicite or Simple, \n'
           'then Piso, and at least Simpler.\n'
           'Because the two first give a fastest convergence (several times) than Piso and the \n'
           'Simpler has not been validated.\n'
           'It seems also than Implicite and Piso schemes give better results than the Simple \n'
           'scheme when the flow is not fully stationary.\n'
           'Thus, if the solution obtained with Simple is not stationary, it is recommended to \n'
           'switch to Piso or Implicite scheme.',
           ''),
  }
  _helpDict = check_append_dict('schema_implicite_base_Tru', _helpDict, schema_temps_base_Tru._helpDict)
##### end schema_implicite_base_Tru class

class un_pb_Tru(objet_lecture_Tru): #####
  r"""
  pour les groupes
  """
  # From: line 362, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('mot', 'ref_pb_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 362]
  _infoAttr = {'mot': ['TRAD2_example', 363]}
  _attributesList = check_append_list('un_pb_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('un_pb_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('un_pb_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'mot':
          ('[ref_pb_base] the string', ''),
  }
  _helpDict = check_append_dict('un_pb_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end un_pb_Tru class

class list_un_pb_Tru(ListOfBase_Tru): #####
  """
  pour les groupes
  """
  # From: line 364, in file 'TRAD2_example'
  _braces = 1
  _comma = 1
  _allowedClasses = ["un_pb_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 364]
##### end list_un_pb_Tru class

class list_list_nom_Tru(ListOfBase_Tru): #####
  """
  pour les groupes
  """
  # From: line 365, in file 'TRAD2_example'
  _braces = 1
  _comma = 1
  _allowedClasses = ["list_un_pb_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 365]
##### end list_list_nom_Tru class

class coupled_problem_Tru(pb_gen_base_Tru): #####
  r"""
  This instruction causes a probleme_couple type object to be created.
  This type of object has an associated problem list, that is, the coupling of n problems 
  among them may be processed.
  Coupling between these problems is carried out explicitly via conditions at particular 
  contact limits.
  Each problem may be associated either with the Associate keyword or with the Read/groupes 
  keywords.
  The difference is that in the first case, the four problems exchange values then 
  calculate their timestep, rather in the second case, the same strategy is used for 
  all the problems listed inside one group, but the second group of problem exchange 
  values with the first group of problems after the first group did its timestep.
  So, the first case may then also be written like this:
  
  Probleme_Couple pbc
  
  Read pbc { groupes { { pb1 , pb2 , pb3 , pb4 } } }
  
  There is a physical environment per problem (however, the same physical environment 
  could be common to several problems).
  
  Each problem is resolved in a domain.
  
  Warning : Presently, coupling requires coincident meshes.
  In case of non-coincident meshes, boundary condition 'paroi_contact' in VEF returns 
  error message (see paroi_contact for correcting procedure).
  """
  # From: line 366, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['probleme_couple_Tru']
  _attributesList = [
    ('groupes', 'list_list_nom_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['groupes'])
  _infoMain = ['TRAD2_example', 366]
  _infoAttr = {'groupes': ['TRAD2_example', 367]}
  _attributesList = check_append_list('coupled_problem_Tru', _attributesList, pb_gen_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('coupled_problem_Tru', _attributesSynonyms, pb_gen_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('coupled_problem_Tru', _infoAttr, pb_gen_base_Tru._infoAttr)
  _optionals.update(pb_gen_base_Tru._optionals)
  _helpDict = {
    'groupes':
          ('[list_list_nom] { groupes { { pb1 , pb2 } , { pb3 , pb4 } } }', ''),
  }
  _helpDict = check_append_dict('coupled_problem_Tru', _helpDict, pb_gen_base_Tru._helpDict)
##### end coupled_problem_Tru class

class mor_eqn_Tru(ConstrainBase_Tru): #####
  r"""
  Class of equation pieces (morceaux d'equation).
  """
  # From: line 368, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 368]
  _infoAttr = {}
  _helpDict = {}
##### end mor_eqn_Tru class

class bords_ecrire_Tru(objet_lecture_Tru): #####
  r"""
  not_set
  """
  # From: line 369, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('chaine', 'chaine_f0532c_Tru'),   #
    ('bords', 'listchaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 369]
  _infoAttr = {'bords': ['TRAD2_example', 371], 'chaine': ['TRAD2_example', 370]}
  _attributesList = check_append_list('bords_ecrire_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('bords_ecrire_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('bords_ecrire_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'chaine':
          ('[chaine(into=["bords"])] not_set', ''),
    'bords':
          ('[listchaine] Keyword to post-process only on some boundaries :\n'
           '\n'
           'bords nb_bords boundary1 ...\n'
           'boundaryn\n'
           '\n'
           'where\n'
           '\n'
           'nb_bords : number of boundaries\n'
           '\n'
           'boundary1 ...\n'
           'boundaryn : name of the boundaries.',
           ''),
  }
  _helpDict = check_append_dict('bords_ecrire_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end bords_ecrire_Tru class

class ecrire_fichier_xyz_valeur_param_Tru(objet_lecture_Tru): #####
  r"""
  To write the values of a field for some boundaries in a text file.
  
  The name of the files is pb_name_field_name_time.dat
  
  Several Ecrire_fichier_xyz_valeur keywords may be written into an equation to write 
  several fields.
  This kind of files may be read by Champ_don_lu or Champ_front_lu for example.
  """
  # From: line 372, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('name', 'chaine_Tru'),   #
    ('dt_ecrire_fic', 'floattant_820d8b_Tru'),   #
    ('bords', 'bords_ecrire_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['bords'])
  _infoMain = ['TRAD2_example', 372]
  _infoAttr = {'bords': ['TRAD2_example', 375],
   'dt_ecrire_fic': ['TRAD2_example', 374],
   'name': ['TRAD2_example', 373]}
  _attributesList = check_append_list('ecrire_fichier_xyz_valeur_param_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('ecrire_fichier_xyz_valeur_param_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('ecrire_fichier_xyz_valeur_param_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'name':
          ('[chaine] Name of the field to write (Champ_Inc, Champ_Fonc or a post_processed field).', ''),
    'dt_ecrire_fic':
          ('[floattant] Time period for printing in the file.', ''),
    'bords':
          ('[bords_ecrire] to post-process only on some boundaries', ''),
  }
  _helpDict = check_append_dict('ecrire_fichier_xyz_valeur_param_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end ecrire_fichier_xyz_valeur_param_Tru class

class convection_deriv_Tru(objet_lecture_Tru): #####
  r"""
  not_set
  """
  # From: line 376, in file 'TRAD2_example'
  _braces = 0
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 376]
  _infoAttr = {}
  _attributesList = check_append_list('convection_deriv_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_deriv_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_deriv_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('convection_deriv_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end convection_deriv_Tru class

class bloc_convection_Tru(objet_lecture_Tru): #####
  r"""
  not_set
  """
  # From: line 377, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('aco', 'chaine_e3a651_Tru'),   #
    ('operateur', '<inherited>convection_deriv_Tru'),   #
    ('acof', 'chaine_af6447_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 377]
  _infoAttr = {'aco': ['TRAD2_example', 378],
   'acof': ['TRAD2_example', 380],
   'operateur': ['TRAD2_example', 379]}
  _attributesList = check_append_list('bloc_convection_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('bloc_convection_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('bloc_convection_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'aco':
          ('[chaine(into=["{"])] Opening curly bracket.', ''),
    'operateur':
          ('[convection_deriv] not_set', ''),
    'acof':
          ('[chaine(into=["}"])] Closing curly bracket.', ''),
  }
  _helpDict = check_append_dict('bloc_convection_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end bloc_convection_Tru class

class diffusion_deriv_Tru(objet_lecture_Tru): #####
  r"""
  not_set
  """
  # From: line 381, in file 'TRAD2_example'
  _braces = 0
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 381]
  _infoAttr = {}
  _attributesList = check_append_list('diffusion_deriv_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('diffusion_deriv_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('diffusion_deriv_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('diffusion_deriv_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end diffusion_deriv_Tru class

class op_implicite_Tru(objet_lecture_Tru): #####
  r"""
  not_set
  """
  # From: line 382, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('implicite', 'chaine_a28892_Tru'),   #
    ('mot', 'chaine_2a7c96_Tru'),   #
    ('solveur', '<inherited>solveur_sys_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 382]
  _infoAttr = {'implicite': ['TRAD2_example', 383],
   'mot': ['TRAD2_example', 384],
   'solveur': ['TRAD2_example', 385]}
  _attributesList = check_append_list('op_implicite_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('op_implicite_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('op_implicite_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'implicite':
          ('[chaine(into=["implicite"])] not_set', ''),
    'mot':
          ('[chaine(into=["solveur"])] not_set', ''),
    'solveur':
          ('[solveur_sys_base] not_set', ''),
  }
  _helpDict = check_append_dict('op_implicite_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end op_implicite_Tru class

class bloc_diffusion_Tru(objet_lecture_Tru): #####
  r"""
  not_set
  """
  # From: line 386, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('aco', 'chaine_e3a651_Tru'),   #
    ('operateur', '<inherited>diffusion_deriv_Tru'),   #
    ('op_implicite', 'op_implicite_Tru'),   #
    ('acof', 'chaine_af6447_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['operateur', 'op_implicite'])
  _infoMain = ['TRAD2_example', 386]
  _infoAttr = {'aco': ['TRAD2_example', 387],
   'acof': ['TRAD2_example', 390],
   'op_implicite': ['TRAD2_example', 389],
   'operateur': ['TRAD2_example', 388]}
  _attributesList = check_append_list('bloc_diffusion_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('bloc_diffusion_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('bloc_diffusion_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'aco':
          ('[chaine(into=["{"])] Opening curly bracket.', ''),
    'operateur':
          ('[diffusion_deriv] if none is specified, the diffusive scheme used is a 2nd-order scheme.', ''),
    'op_implicite':
          ('[op_implicite] To have diffusive implicitation, it use Uzawa algorithm.\n'
           'Very useful when viscosity has large variations.',
           ''),
    'acof':
          ('[chaine(into=["}"])] Closing curly bracket.', ''),
  }
  _helpDict = check_append_dict('bloc_diffusion_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end bloc_diffusion_Tru class

class traitement_particulier_base_Tru(objet_lecture_Tru): #####
  r"""
  Basic class to post-process particular values.
  """
  # From: line 391, in file 'TRAD2_example'
  _braces = 1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 391]
  _infoAttr = {}
  _attributesList = check_append_list('traitement_particulier_base_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('traitement_particulier_base_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('traitement_particulier_base_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('traitement_particulier_base_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end traitement_particulier_base_Tru class

class traitement_particulier_Tru(objet_lecture_Tru): #####
  r"""
  Auxiliary class to post-process particular values.
  """
  # From: line 392, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('aco', 'chaine_e3a651_Tru'),   #
    ('trait_part', '<inherited>traitement_particulier_base_Tru'),   #
    ('acof', 'chaine_af6447_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 392]
  _infoAttr = {'aco': ['TRAD2_example', 393],
   'acof': ['TRAD2_example', 395],
   'trait_part': ['TRAD2_example', 394]}
  _attributesList = check_append_list('traitement_particulier_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('traitement_particulier_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('traitement_particulier_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'aco':
          ('[chaine(into=["{"])] Opening curly bracket.', ''),
    'trait_part':
          ('[traitement_particulier_base] Type of traitement_particulier.', ''),
    'acof':
          ('[chaine(into=["}"])] Closing curly bracket.', ''),
  }
  _helpDict = check_append_dict('traitement_particulier_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end traitement_particulier_Tru class

class eqn_base_Tru(mor_eqn_Tru): #####
  r"""
  Basic class for equations.
  """
  # From: line 1503, in file 'TRAD2_example'
  _braces = -3
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = [
    ('disable_equation_residual', 'chaine_Tru'),   #
    ('convection', 'bloc_convection_Tru'),   #
    ('diffusion', 'bloc_diffusion_Tru'),   #
    ('conditions_limites', 'condlims_Tru'),   #
    ('conditions_initiales', 'condinits_Tru'),   #
    ('sources', 'sources_Tru'),   #
    ('ecrire_fichier_xyz_valeur_bin', 'ecrire_fichier_xyz_valeur_param_Tru'),   #
    ('ecrire_fichier_xyz_valeur', 'ecrire_fichier_xyz_valeur_param_Tru'),   #
    ('parametre_equation', '<inherited>parametre_equation_base_Tru'),   #
    ('equation_non_resolue', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'conditions_initiales': ['initial_conditions'],
    'conditions_limites': ['boundary_conditions']
  }
  _optionals = set(['disable_equation_residual',
   'convection',
   'diffusion',
   'conditions_limites',
   'conditions_initiales',
   'sources',
   'ecrire_fichier_xyz_valeur_bin',
   'ecrire_fichier_xyz_valeur',
   'parametre_equation',
   'equation_non_resolue'])
  _infoMain = ['TRAD2_example', 1503]
  _infoAttr = {'conditions_initiales': ['TRAD2_example', 1508],
   'conditions_limites': ['TRAD2_example', 1507],
   'convection': ['TRAD2_example', 1505],
   'diffusion': ['TRAD2_example', 1506],
   'disable_equation_residual': ['TRAD2_example', 1504],
   'ecrire_fichier_xyz_valeur': ['TRAD2_example', 1511],
   'ecrire_fichier_xyz_valeur_bin': ['TRAD2_example', 1510],
   'equation_non_resolue': ['TRAD2_example', 1513],
   'parametre_equation': ['TRAD2_example', 1512],
   'sources': ['TRAD2_example', 1509]}
  _attributesList = check_append_list('eqn_base_Tru', _attributesList, mor_eqn_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('eqn_base_Tru', _attributesSynonyms, mor_eqn_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('eqn_base_Tru', _infoAttr, mor_eqn_Tru._infoAttr)
  _optionals.update(mor_eqn_Tru._optionals)
  _helpDict = {
    'disable_equation_residual':
          ('[chaine] The equation residual will not be used for the problem residual used when \n'
           'checking time convergence or computing dynamic time-step',
           ''),
    'convection':
          ('[bloc_convection] Keyword to alter the convection scheme.', ''),
    'diffusion':
          ('[bloc_diffusion] Keyword to specify the diffusion operator.', ''),
    'conditions_limites':
          ('[condlims] Boundary conditions.', ''),
    'conditions_initiales':
          ('[condinits] Initial conditions.', ''),
    'sources':
          ('[sources] To introduce a source term into an equation (in case of several source terms \n'
           'into the same equation, the blocks corresponding to the various terms need to be separated \n'
           'by a comma)',
           ''),
    'ecrire_fichier_xyz_valeur_bin':
          ('[ecrire_fichier_xyz_valeur_param] This keyword is used to write the values of a field \n'
           'only for some boundaries in a binary file with the following format: n_valeur\n'
           '\n'
           'x_1 y_1 [z_1] val_1\n'
           '\n'
           '...\n'
           '\n'
           'x_n y_n [z_n] val_n\n'
           '\n'
           'The created files are named : pbname_fieldname_[boundaryname]_time.dat',
           ''),
    'ecrire_fichier_xyz_valeur':
          ('[ecrire_fichier_xyz_valeur_param] This keyword is used to write the values of a field \n'
           'only for some boundaries in a text file with the following format: n_valeur\n'
           '\n'
           'x_1 y_1 [z_1] val_1\n'
           '\n'
           '...\n'
           '\n'
           'x_n y_n [z_n] val_n\n'
           '\n'
           'The created files are named : pbname_fieldname_[boundaryname]_time.dat',
           ''),
    'parametre_equation':
          ('[parametre_equation_base] Keyword used to specify additional parameters for the equation', ''),
    'equation_non_resolue':
          ('[chaine] The equation will not be solved while condition(t) is verified if '
           'equation_non_resolue \n'
           'keyword is used.\n'
           'Exemple: The Navier-Stokes equations are not solved between time t0 and t1.\n'
           '\n'
           'Navier_Sokes_Standard\n'
           '\n'
           '{ equation_non_resolue (t>t0)*(t<t1) }',
           ''),
  }
  _helpDict = check_append_dict('eqn_base_Tru', _helpDict, mor_eqn_Tru._helpDict)
##### end eqn_base_Tru class

class navier_stokes_standard_Tru(eqn_base_Tru): #####
  r"""
  Navier-Stokes equations.
  """
  # From: line 396, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('methode_calcul_pression_initiale', 'chaine_39eae7_Tru'),   #
    ('projection_initiale', 'entier_3282e0_Tru'),   #
    ('solveur_pression', '<inherited>solveur_sys_base_Tru'),   #
    ('solveur_bar', '<inherited>solveur_sys_base_Tru'),   #
    ('dt_projection', 'deuxmots_Tru'),   #
    ('seuil_divu', 'floatfloat_Tru'),   #
    ('traitement_particulier', 'traitement_particulier_Tru'),   #
    ('correction_matrice_projection_initiale', 'entier_3282e0_Tru'),   #
    ('correction_calcul_pression_initiale', 'entier_3282e0_Tru'),   #
    ('correction_vitesse_projection_initiale', 'entier_3282e0_Tru'),   #
    ('correction_matrice_pression', 'entier_3282e0_Tru'),   #
    ('correction_vitesse_modifie', 'entier_3282e0_Tru'),   #
    ('gradient_pression_qdm_modifie', 'entier_3282e0_Tru'),   #
    ('correction_pression_modifie', 'entier_3282e0_Tru'),   #
    ('postraiter_gradient_pression_sans_masse', 'rien_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['methode_calcul_pression_initiale',
   'projection_initiale',
   'solveur_pression',
   'solveur_bar',
   'dt_projection',
   'seuil_divu',
   'traitement_particulier',
   'correction_matrice_projection_initiale',
   'correction_calcul_pression_initiale',
   'correction_vitesse_projection_initiale',
   'correction_matrice_pression',
   'correction_vitesse_modifie',
   'gradient_pression_qdm_modifie',
   'correction_pression_modifie',
   'postraiter_gradient_pression_sans_masse'])
  _infoMain = ['TRAD2_example', 396]
  _infoAttr = {'correction_calcul_pression_initiale': ['TRAD2_example', 405],
   'correction_matrice_pression': ['TRAD2_example', 407],
   'correction_matrice_projection_initiale': ['TRAD2_example', 404],
   'correction_pression_modifie': ['TRAD2_example', 410],
   'correction_vitesse_modifie': ['TRAD2_example', 408],
   'correction_vitesse_projection_initiale': ['TRAD2_example', 406],
   'dt_projection': ['TRAD2_example', 401],
   'gradient_pression_qdm_modifie': ['TRAD2_example', 409],
   'methode_calcul_pression_initiale': ['TRAD2_example', 397],
   'postraiter_gradient_pression_sans_masse': ['TRAD2_example', 411],
   'projection_initiale': ['TRAD2_example', 398],
   'seuil_divu': ['TRAD2_example', 402],
   'solveur_bar': ['TRAD2_example', 400],
   'solveur_pression': ['TRAD2_example', 399],
   'traitement_particulier': ['TRAD2_example', 403]}
  _attributesList = check_append_list('navier_stokes_standard_Tru', _attributesList, eqn_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('navier_stokes_standard_Tru', _attributesSynonyms, eqn_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('navier_stokes_standard_Tru', _infoAttr, eqn_base_Tru._infoAttr)
  _optionals.update(eqn_base_Tru._optionals)
  _helpDict = {
    'methode_calcul_pression_initiale':
          ('[chaine(into=["avec_les_cl","avec_sources","avec_sources_et_operateurs","sans_rien"])] \n'
           'Keyword to select an option for the pressure calculation before the fist time step.\n'
           'Options are : avec_les_cl (default option lapP=0 is solved with Neuman boundary conditions \n'
           'on pressure if any), avec_sources (lapP=f is solved with Neuman boundaries conditions \n'
           'and f integrating the source terms of the Navier-Stokes equations) and '
           'avec_sources_et_operateurs \n'
           '(lapP=f is solved as with the previous option avec_sources but f integrating also \n'
           'some operators of the Navier-Stokes equations).\n'
           'The two last options are useful and sometime necessary when source terms are implicited \n'
           'when using an implicit time scheme to solve the Navier-Stokes equations.',
           ''),
    'projection_initiale':
          ('[entier] Keyword to suppress, if boolean equals 0, the initial projection which checks \n'
           'DivU=0.\n'
           'By default, boolean equals 1.',
           ''),
    'solveur_pression':
          ('[solveur_sys_base] Linear pressure system resolution method.', ''),
    'solveur_bar':
          ('[solveur_sys_base] This keyword is used to define when filtering operation is called \n'
           '(typically for EF convective scheme, standard diffusion operator and Source_Qdm_lambdaup \n'
           ').\n'
           'A file (solveur.bar) is then created and used for inversion procedure.\n'
           'Syntax is the same then for pressure solver (GCP is required for multi-processor \n'
           'calculations and, in a general way, for big meshes).',
           ''),
    'dt_projection':
          ('[deuxmots] nb value : This keyword checks every nb time-steps the equality of velocity \n'
           'divergence to zero.\n'
           'value is the criteria convergency for the solver used.',
           ''),
    'seuil_divu':
          ('[floatfloat] value factor : this keyword is intended to minimise the number of iterations \n'
           'during the pressure system resolution.\n'
           "The convergence criteria during this step ('seuil' in solveur_pression) is dynamically \n"
           'adapted according to the mass conservation.\n'
           'At tn , the linear system Ax=B is considered as solved if the residual ||Ax-B||<seuil(tn).\n'
           'For tn+1, the threshold value seuil(tn+1) will be evualated as:\n'
           '\n'
           'If ( |max(DivU)*dt|<value )\n'
           '\n'
           'Seuil(tn+1)= Seuil(tn)*factor\n'
           '\n'
           'Else\n'
           '\n'
           'Seuil(tn+1)= Seuil(tn)*factor\n'
           '\n'
           'Endif\n'
           '\n'
           'The first parameter (value) is the mass evolution the user is ready to accept per \n'
           "timestep, and the second one (factor) is the factor of evolution for 'seuil' (for \n"
           'example 1.1, so 10% per timestep).\n'
           'Investigations has to be lead to know more about the effects of these two last parameters \n'
           'on the behaviour of the simulations.',
           ''),
    'traitement_particulier':
          ('[traitement_particulier] Keyword to post-process particular values.', ''),
    'correction_matrice_projection_initiale':
          ('[entier] (IBM advanced) fix matrix of initial projection for PDF', ''),
    'correction_calcul_pression_initiale':
          ('[entier] (IBM advanced) fix initial pressure computation for PDF', ''),
    'correction_vitesse_projection_initiale':
          ('[entier] (IBM advanced) fix initial velocity computation for PDF', ''),
    'correction_matrice_pression':
          ('[entier] (IBM advanced) fix pressure matrix for PDF', ''),
    'correction_vitesse_modifie':
          ('[entier] (IBM advanced) fix velocity for PDF', ''),
    'gradient_pression_qdm_modifie':
          ('[entier] (IBM advanced) fix pressure gradient', ''),
    'correction_pression_modifie':
          ('[entier] (IBM advanced) fix pressure for PDF', ''),
    'postraiter_gradient_pression_sans_masse':
          ('[rien] (IBM advanced) avoid mass matrix multiplication for the gradient postprocessing', ''),
  }
  _helpDict = check_append_dict('navier_stokes_standard_Tru', _helpDict, eqn_base_Tru._helpDict)
##### end navier_stokes_standard_Tru class

class penalisation_l2_ftd_lec_Tru(objet_lecture_Tru): #####
  r"""
  not_set
  """
  # From: line 412, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('postraiter_gradient_pression_sans_masse', 'entier_3282e0_Tru'),   #
    ('correction_matrice_projection_initiale', 'entier_3282e0_Tru'),   #
    ('correction_calcul_pression_initiale', 'entier_3282e0_Tru'),   #
    ('correction_vitesse_projection_initiale', 'entier_3282e0_Tru'),   #
    ('correction_matrice_pression', 'entier_3282e0_Tru'),   #
    ('matrice_pression_penalisee_h1', 'entier_3282e0_Tru'),   #
    ('correction_vitesse_modifie', 'entier_3282e0_Tru'),   #
    ('correction_pression_modifie', 'entier_3282e0_Tru'),   #
    ('gradient_pression_qdm_modifie', 'entier_3282e0_Tru'),   #
    ('bord', 'chaine_Tru'),   #
    ('val', 'list_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['postraiter_gradient_pression_sans_masse',
   'correction_matrice_projection_initiale',
   'correction_calcul_pression_initiale',
   'correction_vitesse_projection_initiale',
   'correction_matrice_pression',
   'matrice_pression_penalisee_h1',
   'correction_vitesse_modifie',
   'correction_pression_modifie',
   'gradient_pression_qdm_modifie'])
  _infoMain = ['TRAD2_example', 412]
  _infoAttr = {'bord': ['TRAD2_example', 422],
   'correction_calcul_pression_initiale': ['TRAD2_example', 415],
   'correction_matrice_pression': ['TRAD2_example', 417],
   'correction_matrice_projection_initiale': ['TRAD2_example', 414],
   'correction_pression_modifie': ['TRAD2_example', 420],
   'correction_vitesse_modifie': ['TRAD2_example', 419],
   'correction_vitesse_projection_initiale': ['TRAD2_example', 416],
   'gradient_pression_qdm_modifie': ['TRAD2_example', 421],
   'matrice_pression_penalisee_h1': ['TRAD2_example', 418],
   'postraiter_gradient_pression_sans_masse': ['TRAD2_example', 413],
   'val': ['TRAD2_example', 423]}
  _attributesList = check_append_list('penalisation_l2_ftd_lec_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('penalisation_l2_ftd_lec_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('penalisation_l2_ftd_lec_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'postraiter_gradient_pression_sans_masse':
          ('[entier] (IBM advanced) avoid mass matrix multiplication for the gradient postprocessing', ''),
    'correction_matrice_projection_initiale':
          ('[entier] (IBM advanced) fix matrix of initial projection for PDF', ''),
    'correction_calcul_pression_initiale':
          ('[entier] (IBM advanced) fix initial pressure computation for PDF', ''),
    'correction_vitesse_projection_initiale':
          ('[entier] (IBM advanced) fix initial velocity computation for PDF', ''),
    'correction_matrice_pression':
          ('[entier] (IBM advanced) fix pressure matrix for PDF', ''),
    'matrice_pression_penalisee_h1':
          ('[entier] (IBM advanced) fix pressure matrix for PDF', ''),
    'correction_vitesse_modifie':
          ('[entier] (IBM advanced) fix velocity for PDF', ''),
    'correction_pression_modifie':
          ('[entier] (IBM advanced) fix pressure for PDF', ''),
    'gradient_pression_qdm_modifie':
          ('[entier] (IBM advanced) fix pressure gradient', ''),
    'bord':
          ('[chaine] not_set', ''),
    'val':
          ('[list] not_set', ''),
  }
  _helpDict = check_append_dict('penalisation_l2_ftd_lec_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end penalisation_l2_ftd_lec_Tru class

class penalisation_l2_ftd_Tru(ListOfBase_Tru): #####
  """
  not_set
  """
  # From: line 424, in file 'TRAD2_example'
  _braces = 1
  _comma = 0
  _allowedClasses = ["penalisation_l2_ftd_lec_Tru"]
  _synonyms = ['pp_Tru']
  _infoMain = ['TRAD2_example', 424]
##### end penalisation_l2_ftd_Tru class

class convection_diffusion_temperature_Tru(eqn_base_Tru): #####
  r"""
  Energy equation (temperature diffusion convection).
  """
  # From: line 425, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('penalisation_l2_ftd', 'penalisation_l2_ftd_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['penalisation_l2_ftd'])
  _infoMain = ['TRAD2_example', 425]
  _infoAttr = {'penalisation_l2_ftd': ['TRAD2_example', 426]}
  _attributesList = check_append_list('convection_diffusion_temperature_Tru', _attributesList, eqn_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_diffusion_temperature_Tru', _attributesSynonyms, eqn_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_diffusion_temperature_Tru', _infoAttr, eqn_base_Tru._infoAttr)
  _optionals.update(eqn_base_Tru._optionals)
  _helpDict = {
    'penalisation_l2_ftd':
          ('[penalisation_l2_ftd] to activate or not (the default is Direct Forcing method) the \n'
           'Penalized Direct Forcing method to impose the specified temperature on the solid-fluid \n'
           'interface.',
           ''),
  }
  _helpDict = check_append_dict('convection_diffusion_temperature_Tru', _helpDict, eqn_base_Tru._helpDict)
##### end convection_diffusion_temperature_Tru class

class turbulence_paroi_base_Tru(ConstrainBase_Tru): #####
  r"""
  Basic class for wall laws for Navier-Stokes equations.
  """
  # From: line 427, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 427]
  _infoAttr = {}
  _helpDict = {}
##### end turbulence_paroi_base_Tru class

class dt_impr_ustar_mean_only_Tru(objet_lecture_Tru): #####
  r"""
  not_set
  """
  # From: line 428, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('dt_impr', 'floattant_820d8b_Tru'),   #
    ('boundaries', 'listchaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['boundaries'])
  _infoMain = ['TRAD2_example', 428]
  _infoAttr = {'boundaries': ['TRAD2_example', 430], 'dt_impr': ['TRAD2_example', 429]}
  _attributesList = check_append_list('dt_impr_ustar_mean_only_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('dt_impr_ustar_mean_only_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('dt_impr_ustar_mean_only_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'dt_impr':
          ('[floattant] not_set', ''),
    'boundaries':
          ('[listchaine] not_set', ''),
  }
  _helpDict = check_append_dict('dt_impr_ustar_mean_only_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end dt_impr_ustar_mean_only_Tru class

class modele_turbulence_hyd_deriv_Tru(objet_lecture_Tru): #####
  r"""
  Basic class for turbulence model for Navier-Stokes equations.
  """
  # From: line 431, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = [
    ('correction_visco_turb_pour_controle_pas_de_temps', 'rien_Tru'),   #
    ('correction_visco_turb_pour_controle_pas_de_temps_parametre', 'floattant_820d8b_Tru'),   #
    ('turbulence_paroi', '<inherited>turbulence_paroi_base_Tru'),   #
    ('dt_impr_ustar', 'floattant_820d8b_Tru'),   #
    ('dt_impr_ustar_mean_only', 'dt_impr_ustar_mean_only_Tru'),   #
    ('nut_max', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['correction_visco_turb_pour_controle_pas_de_temps',
   'correction_visco_turb_pour_controle_pas_de_temps_parametre',
   'turbulence_paroi',
   'dt_impr_ustar',
   'dt_impr_ustar_mean_only',
   'nut_max'])
  _infoMain = ['TRAD2_example', 431]
  _infoAttr = {'correction_visco_turb_pour_controle_pas_de_temps': ['TRAD2_example', 432],
   'correction_visco_turb_pour_controle_pas_de_temps_parametre': ['TRAD2_example',
                                                                  433],
   'dt_impr_ustar': ['TRAD2_example', 435],
   'dt_impr_ustar_mean_only': ['TRAD2_example', 436],
   'nut_max': ['TRAD2_example', 437],
   'turbulence_paroi': ['TRAD2_example', 434]}
  _attributesList = check_append_list('modele_turbulence_hyd_deriv_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('modele_turbulence_hyd_deriv_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('modele_turbulence_hyd_deriv_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'correction_visco_turb_pour_controle_pas_de_temps':
          ('[rien] Keyword to set a limitation to low time steps due to high values of turbulent \n'
           'viscosity.\n'
           'The limit for turbulent viscosity is calculated so that diffusive time-step is equal \n'
           'or higher than convective time-step.\n'
           'For a stationary flow, the correction for turbulent viscosity should apply only during \n'
           'the first time steps and not when permanent state is reached.\n'
           'To check that, we could post process the corr_visco_turb field which is the correction \n'
           'of turbulent viscosity: it should be 1.\n'
           'on the whole domain.',
           ''),
    'correction_visco_turb_pour_controle_pas_de_temps_parametre':
          ('[floattant] Keyword to set a limitation to low time steps due to high values of turbulent \n'
           'viscosity.\n'
           'The limit for turbulent viscosity is the ratio between diffusive time-step and convective \n'
           'time-step is higher or equal to the given value [0-1]',
           ''),
    'turbulence_paroi':
          ('[turbulence_paroi_base] Keyword to set the wall law.', ''),
    'dt_impr_ustar':
          ('[floattant] This keyword is used to print the values (U +, d+, u$\\star$) obtained \n'
           'with the wall laws into a file named datafile_ProblemName_Ustar.face and periode refers \n'
           'to the printing period, this value is expressed in seconds.',
           ''),
    'dt_impr_ustar_mean_only':
          ('[dt_impr_ustar_mean_only] This keyword is used to print the mean values of u* ( obtained \n'
           'with the wall laws) on each boundary, into a file named '
           'datafile_ProblemName_Ustar_mean_only.out.\n'
           'periode refers to the printing period, this value is expressed in seconds.\n'
           "If you don't use the optional keyword boundaries, all the boundaries will be considered.\n"
           'If you use it, you must specify nb_boundaries which is the number of boundaries on \n'
           'which you want to calculate the mean values of u*, then you have to specify their \n'
           'names.',
           ''),
    'nut_max':
          ('[floattant] Upper limitation of turbulent viscosity (default value 1.e8).', ''),
  }
  _helpDict = check_append_dict('modele_turbulence_hyd_deriv_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end modele_turbulence_hyd_deriv_Tru class

class turbulence_paroi_scalaire_base_Tru(ConstrainBase_Tru): #####
  r"""
  Basic class for wall laws for energy equation.
  """
  # From: line 438, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 438]
  _infoAttr = {}
  _helpDict = {}
##### end turbulence_paroi_scalaire_base_Tru class

class modele_turbulence_scal_base_Tru(ConstrainBase_Tru): #####
  r"""
  Basic class for turbulence model for energy equation.
  """
  # From: line 439, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = [
    ('turbulence_paroi', '<inherited>turbulence_paroi_scalaire_base_Tru'),   #
    ('dt_impr_nusselt', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['dt_impr_nusselt'])
  _infoMain = ['TRAD2_example', 439]
  _infoAttr = {'dt_impr_nusselt': ['TRAD2_example', 441],
   'turbulence_paroi': ['TRAD2_example', 440]}
  _helpDict = {
    'turbulence_paroi':
          ('[turbulence_paroi_scalaire_base] Keyword to set the wall law.', ''),
    'dt_impr_nusselt':
          ('[floattant] Keyword to print local values of Nusselt number and temperature near a \n'
           'wall during a turbulent calculation.\n'
           'The values will be printed in the _Nusselt.face file each dt_impr_nusselt time period.\n'
           'The local Nusselt expression is as follows : Nu = ((lambda+lambda_t)/lambda)*d_wall/d_eq \n'
           'where d_wall is the distance from the first mesh to the wall and d_eq is given by \n'
           'the wall law.\n'
           'This option also gives the value of d_eq and h = (lambda+lambda_t)/d_eq and the fluid \n'
           'temperature of the first mesh near the wall.\n'
           '\n'
           'For the Neumann boundary conditions (flux_impose), the <<equivalent>> wall temperature \n'
           'given by the wall law is also printed (Tparoi equiv.) preceded for VEF calculation \n'
           'by the edge temperature <<T face de bord>>.',
           ''),
  }
##### end modele_turbulence_scal_base_Tru class

class paroi_fixe_Tru(condlim_base_Tru): #####
  r"""
  Keyword to designate a situation of adherence to the wall called bord (edge) (normal 
  and tangential velocity at the edge is zero).
  """
  # From: line 442, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 442]
  _infoAttr = {}
  _attributesList = check_append_list('paroi_fixe_Tru', _attributesList, condlim_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('paroi_fixe_Tru', _attributesSynonyms, condlim_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('paroi_fixe_Tru', _infoAttr, condlim_base_Tru._infoAttr)
  _optionals.update(condlim_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('paroi_fixe_Tru', _helpDict, condlim_base_Tru._helpDict)
##### end paroi_fixe_Tru class

class symetrie_Tru(condlim_base_Tru): #####
  r"""
  1).
  For Navier-Stokes equations, this keyword is used to designate a symmetry condition 
  concerning the velocity at the boundary called bord (edge) (normal velocity at the 
  edge equal to zero and tangential velocity gradient at the edge equal to zero); 2).
  For scalar transport equation, this keyword is used to set a symmetry condition on 
  scalar on the boundary named bord (edge).
  """
  # From: line 443, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 443]
  _infoAttr = {}
  _attributesList = check_append_list('symetrie_Tru', _attributesList, condlim_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('symetrie_Tru', _attributesSynonyms, condlim_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('symetrie_Tru', _infoAttr, condlim_base_Tru._infoAttr)
  _optionals.update(condlim_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('symetrie_Tru', _helpDict, condlim_base_Tru._helpDict)
##### end symetrie_Tru class

class periodic_Tru(condlim_base_Tru): #####
  r"""
  1).
  For Navier-Stokes equations, this keyword is used to indicate that the horizontal 
  inlet velocity values are the same as the outlet velocity values, at every moment.
  As regards meshing, the inlet and outlet edges bear the same name.; 2).
  For scalar transport equation, this keyword is used to set a periodic condition on 
  scalar.
  The two edges dealing with this periodic condition bear the same name.
  """
  # From: line 444, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['periodique_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 444]
  _infoAttr = {}
  _attributesList = check_append_list('periodic_Tru', _attributesList, condlim_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('periodic_Tru', _attributesSynonyms, condlim_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('periodic_Tru', _infoAttr, condlim_base_Tru._infoAttr)
  _optionals.update(condlim_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('periodic_Tru', _helpDict, condlim_base_Tru._helpDict)
##### end periodic_Tru class

class paroi_adiabatique_Tru(condlim_base_Tru): #####
  r"""
  Normal zero flux condition at the wall called bord (edge).
  """
  # From: line 445, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 445]
  _infoAttr = {}
  _attributesList = check_append_list('paroi_adiabatique_Tru', _attributesList, condlim_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('paroi_adiabatique_Tru', _attributesSynonyms, condlim_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('paroi_adiabatique_Tru', _infoAttr, condlim_base_Tru._infoAttr)
  _optionals.update(condlim_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('paroi_adiabatique_Tru', _helpDict, condlim_base_Tru._helpDict)
##### end paroi_adiabatique_Tru class

class front_field_base_Tru(ConstrainBase_Tru): #####
  r"""
  Basic class for fields at domain boundaries.
  """
  # From: line 446, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = ['champ_front_base_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 446]
  _infoAttr = {}
  _helpDict = {}
##### end front_field_base_Tru class

class dirichlet_Tru(condlim_base_Tru): #####
  r"""
  Dirichlet condition at the boundary called bord (edge) : 1).
  For Navier-Stokes equations, velocity imposed at the boundary; 2).
  For scalar transport equation, scalar imposed at the boundary.
  """
  # From: line 447, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 447]
  _infoAttr = {}
  _attributesList = check_append_list('dirichlet_Tru', _attributesList, condlim_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('dirichlet_Tru', _attributesSynonyms, condlim_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('dirichlet_Tru', _infoAttr, condlim_base_Tru._infoAttr)
  _optionals.update(condlim_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('dirichlet_Tru', _helpDict, condlim_base_Tru._helpDict)
##### end dirichlet_Tru class

class paroi_defilante_Tru(dirichlet_Tru): #####
  r"""
  Keyword to designate a condition where tangential velocity is imposed on the wall 
  called bord (edge).
  If the velocity components set by the user is not tangential, projection is used.
  """
  # From: line 448, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('ch', '<inherited>front_field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 448]
  _infoAttr = {'ch': ['TRAD2_example', 449]}
  _attributesList = check_append_list('paroi_defilante_Tru', _attributesList, dirichlet_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('paroi_defilante_Tru', _attributesSynonyms, dirichlet_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('paroi_defilante_Tru', _infoAttr, dirichlet_Tru._infoAttr)
  _optionals.update(dirichlet_Tru._optionals)
  _helpDict = {
    'ch':
          ('[front_field_base] Boundary field type.', ''),
  }
  _helpDict = check_append_dict('paroi_defilante_Tru', _helpDict, dirichlet_Tru._helpDict)
##### end paroi_defilante_Tru class

class paroi_knudsen_non_negligeable_Tru(dirichlet_Tru): #####
  r"""
  Boundary condition for number of Knudsen (Kn) above 0.001 where slip-flow condition 
  appears: the velocity near the wall depends on the shear stress : Kn=l/L with l is 
  the mean-free-path of the molecules and L a characteristic length scale.
  
  U(y=0)-Uwall=k(dU/dY)
  
  Where k is a coefficient given by several laws:
  
  Mawxell : k=(2-s)*l/s
  
  Bestok\&Karniadakis :k=(2-s)/s*L*Kn/(1+Kn)
  
  Xue\&Fan :k=(2-s)/s*L*tanh(Kn)
  
  s is a value between 0 and 2 named accomodation coefficient.
  s=1 seems a good value.
  
  Warning : The keyword is available for VDF calculation only for the moment.
  """
  # From: line 450, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('name_champ_1', 'chaine_d75b13_Tru'),   #
    ('champ_1', '<inherited>front_field_base_Tru'),   #
    ('name_champ_2', 'chaine_d75b13_Tru'),   #
    ('champ_2', '<inherited>front_field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 450]
  _infoAttr = {'champ_1': ['TRAD2_example', 452],
   'champ_2': ['TRAD2_example', 454],
   'name_champ_1': ['TRAD2_example', 451],
   'name_champ_2': ['TRAD2_example', 453]}
  _attributesList = check_append_list('paroi_knudsen_non_negligeable_Tru', _attributesList, dirichlet_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('paroi_knudsen_non_negligeable_Tru', _attributesSynonyms, dirichlet_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('paroi_knudsen_non_negligeable_Tru', _infoAttr, dirichlet_Tru._infoAttr)
  _optionals.update(dirichlet_Tru._optionals)
  _helpDict = {
    'name_champ_1':
          ('[chaine(into=["vitesse_paroi","k"])] Field name.', ''),
    'champ_1':
          ('[front_field_base] Boundary field type.', ''),
    'name_champ_2':
          ('[chaine(into=["vitesse_paroi","k"])] Field name.', ''),
    'champ_2':
          ('[front_field_base] Boundary field type.', ''),
  }
  _helpDict = check_append_dict('paroi_knudsen_non_negligeable_Tru', _helpDict, dirichlet_Tru._helpDict)
##### end paroi_knudsen_non_negligeable_Tru class

class neumann_Tru(condlim_base_Tru): #####
  r"""
  Neumann condition at the boundary called bord (edge) : 1).
  For Navier-Stokes equations, constraint imposed at the boundary; 2).
  For scalar transport equation, flux imposed at the boundary.
  """
  # From: line 455, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 455]
  _infoAttr = {}
  _attributesList = check_append_list('neumann_Tru', _attributesList, condlim_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('neumann_Tru', _attributesSynonyms, condlim_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('neumann_Tru', _infoAttr, condlim_base_Tru._infoAttr)
  _optionals.update(condlim_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('neumann_Tru', _helpDict, condlim_base_Tru._helpDict)
##### end neumann_Tru class

class frontiere_ouverte_vitesse_imposee_Tru(dirichlet_Tru): #####
  r"""
  Class for velocity-inlet boundary condition.
  The imposed velocity field at the inlet is vectorial and the imposed velocity values 
  are expressed in m.s-1.
  """
  # From: line 456, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('ch', '<inherited>front_field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 456]
  _infoAttr = {'ch': ['TRAD2_example', 457]}
  _attributesList = check_append_list('frontiere_ouverte_vitesse_imposee_Tru', _attributesList, dirichlet_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('frontiere_ouverte_vitesse_imposee_Tru', _attributesSynonyms, dirichlet_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('frontiere_ouverte_vitesse_imposee_Tru', _infoAttr, dirichlet_Tru._infoAttr)
  _optionals.update(dirichlet_Tru._optionals)
  _helpDict = {
    'ch':
          ('[front_field_base] Boundary field type.', ''),
  }
  _helpDict = check_append_dict('frontiere_ouverte_vitesse_imposee_Tru', _helpDict, dirichlet_Tru._helpDict)
##### end frontiere_ouverte_vitesse_imposee_Tru class

class frontiere_ouverte_vitesse_imposee_sortie_Tru(frontiere_ouverte_vitesse_imposee_Tru): #####
  r"""
  Sub-class for velocity boundary condition.
  The imposed velocity field at the open boundary is vectorial and the imposed velocity 
  values are expressed in m.s-1.
  """
  # From: line 458, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 458]
  _infoAttr = {}
  _attributesList = check_append_list('frontiere_ouverte_vitesse_imposee_sortie_Tru', _attributesList, frontiere_ouverte_vitesse_imposee_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('frontiere_ouverte_vitesse_imposee_sortie_Tru', _attributesSynonyms, frontiere_ouverte_vitesse_imposee_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('frontiere_ouverte_vitesse_imposee_sortie_Tru', _infoAttr, frontiere_ouverte_vitesse_imposee_Tru._infoAttr)
  _optionals.update(frontiere_ouverte_vitesse_imposee_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('frontiere_ouverte_vitesse_imposee_sortie_Tru', _helpDict, frontiere_ouverte_vitesse_imposee_Tru._helpDict)
##### end frontiere_ouverte_vitesse_imposee_sortie_Tru class

class frontiere_ouverte_rho_u_impose_Tru(frontiere_ouverte_vitesse_imposee_sortie_Tru): #####
  r"""
  This keyword is used to designate a condition of imposed mass rate at an open boundary 
  called bord (edge).
  The imposed mass rate field at the inlet is vectorial and the imposed velocity values 
  are expressed in kg.s-1.
  This boundary condition can be used only with the Quasi compressible model.
  """
  # From: line 459, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 459]
  _infoAttr = {}
  _attributesList = check_append_list('frontiere_ouverte_rho_u_impose_Tru', _attributesList, frontiere_ouverte_vitesse_imposee_sortie_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('frontiere_ouverte_rho_u_impose_Tru', _attributesSynonyms, frontiere_ouverte_vitesse_imposee_sortie_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('frontiere_ouverte_rho_u_impose_Tru', _infoAttr, frontiere_ouverte_vitesse_imposee_sortie_Tru._infoAttr)
  _optionals.update(frontiere_ouverte_vitesse_imposee_sortie_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('frontiere_ouverte_rho_u_impose_Tru', _helpDict, frontiere_ouverte_vitesse_imposee_sortie_Tru._helpDict)
##### end frontiere_ouverte_rho_u_impose_Tru class

class frontiere_ouverte_gradient_pression_libre_vef_Tru(neumann_Tru): #####
  r"""
  Class for outlet boundary condition in VEF like Orlansky.
  There is no reference for pressure for theses boundary conditions so it is better 
  to add pressure condition (with Frontiere_ouverte_pression_imposee) on one or two 
  cells (for symmetry in a channel) of the boundary where Orlansky conditions are imposed.
  """
  # From: line 460, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 460]
  _infoAttr = {}
  _attributesList = check_append_list('frontiere_ouverte_gradient_pression_libre_vef_Tru', _attributesList, neumann_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('frontiere_ouverte_gradient_pression_libre_vef_Tru', _attributesSynonyms, neumann_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('frontiere_ouverte_gradient_pression_libre_vef_Tru', _infoAttr, neumann_Tru._infoAttr)
  _optionals.update(neumann_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('frontiere_ouverte_gradient_pression_libre_vef_Tru', _helpDict, neumann_Tru._helpDict)
##### end frontiere_ouverte_gradient_pression_libre_vef_Tru class

class frontiere_ouverte_gradient_pression_libre_vefprep1b_Tru(neumann_Tru): #####
  r"""
  Class for outlet boundary condition in VEF P1B/P1NC like Orlansky.
  """
  # From: line 461, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 461]
  _infoAttr = {}
  _attributesList = check_append_list('frontiere_ouverte_gradient_pression_libre_vefprep1b_Tru', _attributesList, neumann_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('frontiere_ouverte_gradient_pression_libre_vefprep1b_Tru', _attributesSynonyms, neumann_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('frontiere_ouverte_gradient_pression_libre_vefprep1b_Tru', _infoAttr, neumann_Tru._infoAttr)
  _optionals.update(neumann_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('frontiere_ouverte_gradient_pression_libre_vefprep1b_Tru', _helpDict, neumann_Tru._helpDict)
##### end frontiere_ouverte_gradient_pression_libre_vefprep1b_Tru class

class frontiere_ouverte_gradient_pression_impose_Tru(neumann_Tru): #####
  r"""
  Normal imposed pressure gradient condition on the open boundary called bord (edge).
  This boundary condition may be only used in VDF discretization.
  The imposed $\partial P/\partial n$ value is expressed in Pa.m-1.
  """
  # From: line 462, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('ch', '<inherited>front_field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 462]
  _infoAttr = {'ch': ['TRAD2_example', 463]}
  _attributesList = check_append_list('frontiere_ouverte_gradient_pression_impose_Tru', _attributesList, neumann_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('frontiere_ouverte_gradient_pression_impose_Tru', _attributesSynonyms, neumann_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('frontiere_ouverte_gradient_pression_impose_Tru', _infoAttr, neumann_Tru._infoAttr)
  _optionals.update(neumann_Tru._optionals)
  _helpDict = {
    'ch':
          ('[front_field_base] Boundary field type.', ''),
  }
  _helpDict = check_append_dict('frontiere_ouverte_gradient_pression_impose_Tru', _helpDict, neumann_Tru._helpDict)
##### end frontiere_ouverte_gradient_pression_impose_Tru class

class frontiere_ouverte_pression_imposee_Tru(neumann_Tru): #####
  r"""
  Imposed pressure condition at the open boundary called bord (edge).
  The imposed pressure field is expressed in Pa.
  """
  # From: line 464, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('ch', '<inherited>front_field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 464]
  _infoAttr = {'ch': ['TRAD2_example', 465]}
  _attributesList = check_append_list('frontiere_ouverte_pression_imposee_Tru', _attributesList, neumann_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('frontiere_ouverte_pression_imposee_Tru', _attributesSynonyms, neumann_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('frontiere_ouverte_pression_imposee_Tru', _infoAttr, neumann_Tru._infoAttr)
  _optionals.update(neumann_Tru._optionals)
  _helpDict = {
    'ch':
          ('[front_field_base] Boundary field type.', ''),
  }
  _helpDict = check_append_dict('frontiere_ouverte_pression_imposee_Tru', _helpDict, neumann_Tru._helpDict)
##### end frontiere_ouverte_pression_imposee_Tru class

class frontiere_ouverte_pression_imposee_orlansky_Tru(neumann_Tru): #####
  r"""
  This boundary condition may only be used with VDF discretization.
  There is no reference for pressure for this boundary condition so it is better to 
  add pressure condition (with Frontiere_ouverte_pression_imposee) on one or two cells 
  (for symetry in a channel) of the boundary where Orlansky conditions are imposed.
  """
  # From: line 466, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 466]
  _infoAttr = {}
  _attributesList = check_append_list('frontiere_ouverte_pression_imposee_orlansky_Tru', _attributesList, neumann_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('frontiere_ouverte_pression_imposee_orlansky_Tru', _attributesSynonyms, neumann_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('frontiere_ouverte_pression_imposee_orlansky_Tru', _infoAttr, neumann_Tru._infoAttr)
  _optionals.update(neumann_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('frontiere_ouverte_pression_imposee_orlansky_Tru', _helpDict, neumann_Tru._helpDict)
##### end frontiere_ouverte_pression_imposee_orlansky_Tru class

class frontiere_ouverte_pression_moyenne_imposee_Tru(neumann_Tru): #####
  r"""
  Class for open boundary with pressure mean level imposed.
  """
  # From: line 467, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('pext', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 467]
  _infoAttr = {'pext': ['TRAD2_example', 468]}
  _attributesList = check_append_list('frontiere_ouverte_pression_moyenne_imposee_Tru', _attributesList, neumann_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('frontiere_ouverte_pression_moyenne_imposee_Tru', _attributesSynonyms, neumann_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('frontiere_ouverte_pression_moyenne_imposee_Tru', _infoAttr, neumann_Tru._infoAttr)
  _optionals.update(neumann_Tru._optionals)
  _helpDict = {
    'pext':
          ('[floattant] Mean pressure.', ''),
  }
  _helpDict = check_append_dict('frontiere_ouverte_pression_moyenne_imposee_Tru', _helpDict, neumann_Tru._helpDict)
##### end frontiere_ouverte_pression_moyenne_imposee_Tru class

class frontiere_ouverte_Tru(neumann_Tru): #####
  r"""
  Boundary outlet condition on the boundary called bord (edge) (diffusion flux zero).
  This condition must be associated with a boundary outlet hydraulic condition.
  """
  # From: line 469, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('var_name', 'chaine_917c90_Tru'),   #
    ('ch', '<inherited>front_field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 469]
  _infoAttr = {'ch': ['TRAD2_example', 471], 'var_name': ['TRAD2_example', 470]}
  _attributesList = check_append_list('frontiere_ouverte_Tru', _attributesList, neumann_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('frontiere_ouverte_Tru', _attributesSynonyms, neumann_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('frontiere_ouverte_Tru', _infoAttr, neumann_Tru._infoAttr)
  _optionals.update(neumann_Tru._optionals)
  _helpDict = {
    'var_name':
          ('[chaine(into=["t_ext","c_ext","y_ext","k_eps_ext","fluctu_temperature_ext","flux_chaleur_turb_ext","v2_ext","a_ext","tau_ext","k_ext","omega_ext"])] \n'
           'Field name.',
           ''),
    'ch':
          ('[front_field_base] Boundary field type.', ''),
  }
  _helpDict = check_append_dict('frontiere_ouverte_Tru', _helpDict, neumann_Tru._helpDict)
##### end frontiere_ouverte_Tru class

class frontiere_ouverte_temperature_imposee_Tru(dirichlet_Tru): #####
  r"""
  Imposed temperature condition at the open boundary called bord (edge) (in the case 
  of fluid inlet).
  This condition must be associated with an imposed inlet velocity condition.
  The imposed temperature value is expressed in oC or K.
  """
  # From: line 472, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('ch', '<inherited>front_field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 472]
  _infoAttr = {'ch': ['TRAD2_example', 473]}
  _attributesList = check_append_list('frontiere_ouverte_temperature_imposee_Tru', _attributesList, dirichlet_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('frontiere_ouverte_temperature_imposee_Tru', _attributesSynonyms, dirichlet_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('frontiere_ouverte_temperature_imposee_Tru', _infoAttr, dirichlet_Tru._infoAttr)
  _optionals.update(dirichlet_Tru._optionals)
  _helpDict = {
    'ch':
          ('[front_field_base] Boundary field type.', ''),
  }
  _helpDict = check_append_dict('frontiere_ouverte_temperature_imposee_Tru', _helpDict, dirichlet_Tru._helpDict)
##### end frontiere_ouverte_temperature_imposee_Tru class

class frontiere_ouverte_concentration_imposee_Tru(dirichlet_Tru): #####
  r"""
  Imposed concentration condition at an open boundary called bord (edge) (situation 
  corresponding to a fluid inlet).
  This condition must be associated with an imposed inlet velocity condition.
  """
  # From: line 474, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('ch', '<inherited>front_field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 474]
  _infoAttr = {'ch': ['TRAD2_example', 475]}
  _attributesList = check_append_list('frontiere_ouverte_concentration_imposee_Tru', _attributesList, dirichlet_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('frontiere_ouverte_concentration_imposee_Tru', _attributesSynonyms, dirichlet_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('frontiere_ouverte_concentration_imposee_Tru', _infoAttr, dirichlet_Tru._infoAttr)
  _optionals.update(dirichlet_Tru._optionals)
  _helpDict = {
    'ch':
          ('[front_field_base] Boundary field type.', ''),
  }
  _helpDict = check_append_dict('frontiere_ouverte_concentration_imposee_Tru', _helpDict, dirichlet_Tru._helpDict)
##### end frontiere_ouverte_concentration_imposee_Tru class

class paroi_contact_Tru(condlim_base_Tru): #####
  r"""
  Thermal condition between two domains.
  Important: the name of the boundaries in the two domains should be the same.
  (Warning: there is also an old limitation not yet fixed on the sequential algorithm 
  in VDF to detect the matching faces on the two boundaries: faces should be ordered 
  in the same way).
  The kind of condition depends on the discretization.
  In VDF, it is a heat exchange condition, and in VEF, a temperature condition.
  
  Such a coupling requires coincident meshes for the moment.
  In case of non-coincident meshes, run is stopped and two external files are automatically 
  generated in VEF (connectivity_failed_boundary_name and connectivity_failed_pb_name.med).
  In 2D, the keyword Decouper_bord_coincident associated to the connectivity_failed_boundary_name 
  file allows to generate a new coincident mesh.
  
  In 3D, for a first preliminary cut domain with HOMARD (fluid for instance), the second 
  problem associated to pb_name (solide in a fluid/solid coupling problem) has to be 
  submitted to HOMARD cutting procedure with connectivity_failed_pb_name.med.
  
  Such a procedure works as while the primary refined mesh (fluid in our example) impacts 
  the fluid/solid interface with a compact shape as described below (values 2 or 4 indicates 
  the number of division from primary faces obtained in fluid domain at the interface 
  after HOMARD cutting):
  
  2-2-2-2-2-2
  
  2-4-4-4-4-4-2 \\; 2-2-2
  
  2-4-4-4-4-2 \\; 2-4-2
  
  2-2-2-2-2 \\; 2-2
  
  OK
  
  NL2 2-2 \\; \\; 2-2-2
  
  2-4-2 \\; 2-2
  
  2-2 \\; 2-2
  
  NOT OK
  """
  # From: line 476, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('autrepb', 'ref_pb_base_Tru'),   #
    ('nameb', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 476]
  _infoAttr = {'autrepb': ['TRAD2_example', 477], 'nameb': ['TRAD2_example', 478]}
  _attributesList = check_append_list('paroi_contact_Tru', _attributesList, condlim_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('paroi_contact_Tru', _attributesSynonyms, condlim_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('paroi_contact_Tru', _infoAttr, condlim_base_Tru._infoAttr)
  _optionals.update(condlim_base_Tru._optionals)
  _helpDict = {
    'autrepb':
          ('[ref_pb_base] Name of other problem.', ''),
    'nameb':
          ('[chaine] boundary name of the remote problem which should be the same than the local \nname', ''),
  }
  _helpDict = check_append_dict('paroi_contact_Tru', _helpDict, condlim_base_Tru._helpDict)
##### end paroi_contact_Tru class

class paroi_contact_fictif_Tru(condlim_base_Tru): #####
  r"""
  This keyword is derivated from paroi_contact and is especially dedicated to compute 
  coupled fluid/solid/fluid problem in case of thin material.
  Thanks to this option, solid is considered as a fictitious media (no mesh, no domain 
  associated), and coupling is performed by considering instantaneous thermal equilibrium 
  in it (for the moment).
  """
  # From: line 479, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('autrepb', 'ref_pb_base_Tru'),   #
    ('nameb', 'chaine_Tru'),   #
    ('conduct_fictif', 'floattant_820d8b_Tru'),   #
    ('ep_fictive', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 479]
  _infoAttr = {'autrepb': ['TRAD2_example', 480],
   'conduct_fictif': ['TRAD2_example', 482],
   'ep_fictive': ['TRAD2_example', 483],
   'nameb': ['TRAD2_example', 481]}
  _attributesList = check_append_list('paroi_contact_fictif_Tru', _attributesList, condlim_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('paroi_contact_fictif_Tru', _attributesSynonyms, condlim_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('paroi_contact_fictif_Tru', _infoAttr, condlim_base_Tru._infoAttr)
  _optionals.update(condlim_base_Tru._optionals)
  _helpDict = {
    'autrepb':
          ('[ref_pb_base] Name of other problem.', ''),
    'nameb':
          ('[chaine] Name of bord.', ''),
    'conduct_fictif':
          ('[floattant] thermal conductivity', ''),
    'ep_fictive':
          ('[floattant] thickness of the fictitious media', ''),
  }
  _helpDict = check_append_dict('paroi_contact_fictif_Tru', _helpDict, condlim_base_Tru._helpDict)
##### end paroi_contact_fictif_Tru class

class paroi_echange_contact_vdf_Tru(condlim_base_Tru): #####
  r"""
  Boundary condition type to model the heat flux between two problems.
  Important: the name of the boundaries in the two problems should be the same.
  """
  # From: line 484, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('autrepb', 'ref_pb_base_Tru'),   #
    ('nameb', 'chaine_Tru'),   #
    ('temp', 'chaine_Tru'),   #
    ('h', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 484]
  _infoAttr = {'autrepb': ['TRAD2_example', 485],
   'h': ['TRAD2_example', 488],
   'nameb': ['TRAD2_example', 486],
   'temp': ['TRAD2_example', 487]}
  _attributesList = check_append_list('paroi_echange_contact_vdf_Tru', _attributesList, condlim_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('paroi_echange_contact_vdf_Tru', _attributesSynonyms, condlim_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('paroi_echange_contact_vdf_Tru', _infoAttr, condlim_base_Tru._infoAttr)
  _optionals.update(condlim_base_Tru._optionals)
  _helpDict = {
    'autrepb':
          ('[ref_pb_base] Name of other problem.', ''),
    'nameb':
          ('[chaine] Name of bord.', ''),
    'temp':
          ('[chaine] Name of field.', ''),
    'h':
          ('[floattant] Value assigned to a coefficient (expressed in W.K-1m-2) that characterises \n'
           'the contact between the two mediums.\n'
           'In order to model perfect contact, h must be taken to be infinite.\n'
           'This value must obviously be the same in both the two problems blocks.\n'
           '\n'
           'The surface thermal flux exchanged between the two mediums is represented by :\n'
           '\n'
           'fi = h (T1-T2) where 1/h = d1/lambda1 + 1/val_h_contact + d2/lambda2\n'
           '\n'
           'where di : distance between the node where Ti and the wall is found.',
           ''),
  }
  _helpDict = check_append_dict('paroi_echange_contact_vdf_Tru', _helpDict, condlim_base_Tru._helpDict)
##### end paroi_echange_contact_vdf_Tru class

class paroi_echange_externe_impose_Tru(condlim_base_Tru): #####
  r"""
  External type exchange condition with a heat exchange coefficient and an imposed external 
  temperature.
  """
  # From: line 489, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('h_imp', 'chaine_Tru'),   #
    ('himpc', '<inherited>front_field_base_Tru'),   #
    ('text', 'chaine_Tru'),   #
    ('ch', '<inherited>front_field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 489]
  _infoAttr = {'ch': ['TRAD2_example', 493],
   'h_imp': ['TRAD2_example', 490],
   'himpc': ['TRAD2_example', 491],
   'text': ['TRAD2_example', 492]}
  _attributesList = check_append_list('paroi_echange_externe_impose_Tru', _attributesList, condlim_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('paroi_echange_externe_impose_Tru', _attributesSynonyms, condlim_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('paroi_echange_externe_impose_Tru', _infoAttr, condlim_base_Tru._infoAttr)
  _optionals.update(condlim_base_Tru._optionals)
  _helpDict = {
    'h_imp':
          ('[chaine] Heat exchange coefficient value (expressed in W.m-2.K-1).', ''),
    'himpc':
          ('[front_field_base] Boundary field type.', ''),
    'text':
          ('[chaine] External temperature value (expressed in oC or K).', ''),
    'ch':
          ('[front_field_base] Boundary field type.', ''),
  }
  _helpDict = check_append_dict('paroi_echange_externe_impose_Tru', _helpDict, condlim_base_Tru._helpDict)
##### end paroi_echange_externe_impose_Tru class

class paroi_echange_global_impose_Tru(condlim_base_Tru): #####
  r"""
  Global type exchange condition (internal) that is to say that diffusion on the first 
  fluid mesh is not taken into consideration.
  """
  # From: line 494, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('h_imp', 'chaine_Tru'),   #
    ('himpc', '<inherited>front_field_base_Tru'),   #
    ('text', 'chaine_Tru'),   #
    ('ch', '<inherited>front_field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 494]
  _infoAttr = {'ch': ['TRAD2_example', 498],
   'h_imp': ['TRAD2_example', 495],
   'himpc': ['TRAD2_example', 496],
   'text': ['TRAD2_example', 497]}
  _attributesList = check_append_list('paroi_echange_global_impose_Tru', _attributesList, condlim_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('paroi_echange_global_impose_Tru', _attributesSynonyms, condlim_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('paroi_echange_global_impose_Tru', _infoAttr, condlim_base_Tru._infoAttr)
  _optionals.update(condlim_base_Tru._optionals)
  _helpDict = {
    'h_imp':
          ('[chaine] Global exchange coefficient value.\n'
           'The global exchange coefficient value is expressed in W.m-2.K-1.',
           ''),
    'himpc':
          ('[front_field_base] Boundary field type.', ''),
    'text':
          ('[chaine] External temperature value.\nThe external temperature value is expressed in oC or K.',
           ''),
    'ch':
          ('[front_field_base] Boundary field type.', ''),
  }
  _helpDict = check_append_dict('paroi_echange_global_impose_Tru', _helpDict, condlim_base_Tru._helpDict)
##### end paroi_echange_global_impose_Tru class

class boussinesq_temperature_Tru(source_base_Tru): #####
  r"""
  Class to describe a source term that couples the movement quantity equation and energy 
  equation with the Boussinesq hypothesis.
  """
  # From: line 499, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('t0', 'chaine_Tru'),   #
    ('verif_boussinesq', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['verif_boussinesq'])
  _infoMain = ['TRAD2_example', 499]
  _infoAttr = {'t0': ['TRAD2_example', 500], 'verif_boussinesq': ['TRAD2_example', 501]}
  _attributesList = check_append_list('boussinesq_temperature_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('boussinesq_temperature_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('boussinesq_temperature_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    't0':
          ('[chaine] Reference temperature value (oC or K).\n'
           'It can also be a time dependant function since the 1.6.6 version.',
           ''),
    'verif_boussinesq':
          ('[entier] Keyword to check (1) or not (0) the reference temperature in comparison with \n'
           'the mean temperature value in the domain.\n'
           'It is set to 1 by default.',
           ''),
  }
  _helpDict = check_append_dict('boussinesq_temperature_Tru', _helpDict, source_base_Tru._helpDict)
##### end boussinesq_temperature_Tru class

class boussinesq_concentration_Tru(source_base_Tru): #####
  r"""
  Class to describe a source term that couples the movement quantity equation and constituent 
  transport equation with the Boussinesq hypothesis.
  """
  # From: line 502, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('c0', 'list_Tru'),   #
    ('verif_boussinesq', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['verif_boussinesq'])
  _infoMain = ['TRAD2_example', 502]
  _infoAttr = {'c0': ['TRAD2_example', 503], 'verif_boussinesq': ['TRAD2_example', 504]}
  _attributesList = check_append_list('boussinesq_concentration_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('boussinesq_concentration_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('boussinesq_concentration_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'c0':
          ('[list] Reference concentration field type.\n'
           'The only field type currently available is Champ_Uniforme (Uniform field).',
           ''),
    'verif_boussinesq':
          ('[entier] Keyword to check (1) or not (0) the reference concentration in comparison \n'
           'with the mean concentration value in the domain.\n'
           'It is set to 1 by default.',
           ''),
  }
  _helpDict = check_append_dict('boussinesq_concentration_Tru', _helpDict, source_base_Tru._helpDict)
##### end boussinesq_concentration_Tru class

class dirac_Tru(source_base_Tru): #####
  r"""
  Class to define a source term corresponding to a volume power release in the energy 
  equation.
  """
  # From: line 505, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('position', 'list_Tru'),   #
    ('ch', '<inherited>field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 505]
  _infoAttr = {'ch': ['TRAD2_example', 507], 'position': ['TRAD2_example', 506]}
  _attributesList = check_append_list('dirac_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('dirac_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('dirac_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'position':
          ('[list] not_set', ''),
    'ch':
          ('[field_base] Thermal power field type.\n'
           'To impose a volume power on a domain sub-area, the Champ_Uniforme_Morceaux '
           '(partly_uniform_field) \n'
           'type must be used.\n'
           '\n'
           'Warning : The volume thermal power is expressed in W.m-3.',
           ''),
  }
  _helpDict = check_append_dict('dirac_Tru', _helpDict, source_base_Tru._helpDict)
##### end dirac_Tru class

class puissance_thermique_Tru(source_base_Tru): #####
  r"""
  Class to define a source term corresponding to a volume power release in the energy 
  equation.
  """
  # From: line 508, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('ch', '<inherited>field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 508]
  _infoAttr = {'ch': ['TRAD2_example', 509]}
  _attributesList = check_append_list('puissance_thermique_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('puissance_thermique_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('puissance_thermique_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'ch':
          ('[field_base] Thermal power field type.\n'
           'To impose a volume power on a domain sub-area, the Champ_Uniforme_Morceaux '
           '(partly_uniform_field) \n'
           'type must be used.\n'
           '\n'
           'Warning : The volume thermal power is expressed in W.m-3 in 3D (in W.m-2 in 2D).\n'
           'It is a power per volume unit (in a porous media, it is a power per fluid volume \n'
           'unit).',
           ''),
  }
  _helpDict = check_append_dict('puissance_thermique_Tru', _helpDict, source_base_Tru._helpDict)
##### end puissance_thermique_Tru class

class source_qdm_lambdaup_Tru(source_base_Tru): #####
  r"""
  This source term is a dissipative term which is intended to minimise the energy associated 
  to non-conformscales u' (responsible for spurious oscillations in some cases).
  The equation for these scales can be seen as: du'/dt= -lambda.
  u' + grad P' where -lambda.
  u' represents the dissipative term, with lambda = a/Delta t For Crank-Nicholson temporal 
  scheme, recommended value for a is 2.
  
  Remark : This method requires to define a filtering operator.
  """
  # From: line 510, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('Lambda', 'floattant_820d8b_Tru'),   #
    ('lambda_min', 'floattant_820d8b_Tru'),   #
    ('lambda_max', 'floattant_820d8b_Tru'),   #
    ('ubar_umprim_cible', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'Lambda': ['lambda_u']
  }
  _optionals = set(['lambda_min', 'lambda_max', 'ubar_umprim_cible'])
  _infoMain = ['TRAD2_example', 510]
  _infoAttr = {'Lambda': ['TRAD2_example', 511],
   'lambda_max': ['TRAD2_example', 513],
   'lambda_min': ['TRAD2_example', 512],
   'ubar_umprim_cible': ['TRAD2_example', 514]}
  _attributesList = check_append_list('source_qdm_lambdaup_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('source_qdm_lambdaup_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('source_qdm_lambdaup_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'Lambda':
          ('[floattant] value of lambda', ''),
    'lambda_min':
          ('[floattant] value of lambda_min', ''),
    'lambda_max':
          ('[floattant] value of lambda_max', ''),
    'ubar_umprim_cible':
          ('[floattant] value of ubar_umprim_cible', ''),
  }
  _helpDict = check_append_dict('source_qdm_lambdaup_Tru', _helpDict, source_base_Tru._helpDict)
##### end source_qdm_lambdaup_Tru class

class source_th_tdivu_Tru(source_base_Tru): #####
  r"""
  This term source is dedicated for any scalar (called T) transport.
  Coupled with upwind (amont) or muscl scheme, this term gives for final expression 
  of convection : div(U.T)-T.div (U)=U.grad(T) This ensures, in incompressible flow 
  when divergence free is badly resolved, to stay in a better way in the physical boundaries.
  
  Warning: Only available in VEF discretization.
  """
  # From: line 515, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 515]
  _infoAttr = {}
  _attributesList = check_append_list('source_th_tdivu_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('source_th_tdivu_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('source_th_tdivu_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('source_th_tdivu_Tru', _helpDict, source_base_Tru._helpDict)
##### end source_th_tdivu_Tru class

class source_robin_Tru(source_base_Tru): #####
  r"""
  This source term should be used when a Paroi_decalee_Robin boundary condition is set 
  in a hydraulic equation.
  The source term will be applied on the N specified boundaries.
  To post-process the values of tauw, u_tau and Reynolds_tau into the files tauw_robin.dat, 
  reynolds_tau_robin.dat and u_tau_robin.dat, you must add a block Traitement_particulier 
  { canal { } }
  """
  # From: line 516, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('bords', 'vect_nom_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 516]
  _infoAttr = {'bords': ['TRAD2_example', 517]}
  _attributesList = check_append_list('source_robin_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('source_robin_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('source_robin_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'bords':
          ('[vect_nom] not_set', ''),
  }
  _helpDict = check_append_dict('source_robin_Tru', _helpDict, source_base_Tru._helpDict)
##### end source_robin_Tru class

class source_robin_scalaire_Tru(source_base_Tru): #####
  r"""
  This source term should be used when a Paroi_decalee_Robin boundary condition is set 
  in a an energy equation.
  The source term will be applied on the N specified boundaries.
  The values temp_wall_valueI are the temperature specified on the Ith boundary.
  The last value dt_impr is a printing period which is mandatory to specify in the 
  data file but has no effect yet.
  """
  # From: line 518, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('bords', 'listdeuxmots_sacc_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 518]
  _infoAttr = {'bords': ['TRAD2_example', 519]}
  _attributesList = check_append_list('source_robin_scalaire_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('source_robin_scalaire_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('source_robin_scalaire_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'bords':
          ('[listdeuxmots_sacc] not_set', ''),
  }
  _helpDict = check_append_dict('source_robin_scalaire_Tru', _helpDict, source_base_Tru._helpDict)
##### end source_robin_scalaire_Tru class

class canal_perio_Tru(source_base_Tru): #####
  r"""
  Momentum source term to maintain flow rate.
  The expression of the source term is:
  
  S(t) = (2*(Q(0) - Q(t))-(Q(0)-Q(t-dt))/(coeff*dt*area)
  
  NL2 Where:
  
  coeff=damping coefficient
  
  area=area of the periodic boundary
  
  Q(t)=flow rate at time t
  
  dt=time step
  
  NL2 Three files will be created during calculation on a datafile named DataFile.data.
  The first file contains the flow rate evolution.
  The second file is useful for resuming a calculation with the flow rate of the previous 
  stopped calculation, and the last one contains the pressure gradient evolution:
  
  -DataFile_Channel_Flow_Rate_ProblemName_BoundaryName
  
  -DataFile_Channel_Flow_Rate_repr_ProblemName_BoundaryName
  
  -DataFile_Pressure_Gradient_ProblemName_BoundaryName
  """
  # From: line 520, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('bord', 'chaine_Tru'),   #
    ('h', 'floattant_820d8b_Tru'),   #
    ('coeff', 'floattant_820d8b_Tru'),   #
    ('debit_impose', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['h', 'coeff', 'debit_impose'])
  _infoMain = ['TRAD2_example', 520]
  _infoAttr = {'bord': ['TRAD2_example', 521],
   'coeff': ['TRAD2_example', 523],
   'debit_impose': ['TRAD2_example', 524],
   'h': ['TRAD2_example', 522]}
  _attributesList = check_append_list('canal_perio_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('canal_perio_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('canal_perio_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'bord':
          ('[chaine] The name of the (periodic) boundary normal to the flow direction.', ''),
    'h':
          ('[floattant] Half heigth of the channel.', ''),
    'coeff':
          ('[floattant] Damping coefficient (optional, default value is 10).', ''),
    'debit_impose':
          ('[floattant] Optional option to specify the aimed flow rate Q(0).\n'
           'If not used, Q(0) is computed by the code after the projection phase, where velocity \n'
           'initial conditions are slighlty changed to verify incompressibility.',
           ''),
  }
  _helpDict = check_append_dict('canal_perio_Tru', _helpDict, source_base_Tru._helpDict)
##### end canal_perio_Tru class

class champ_don_base_Tru(field_base_Tru): #####
  r"""
  Basic class for data fields (not calculated), p.e.
  physics properties.
  """
  # From: line 525, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 525]
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
  # From: line 526, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['champ_uniforme_Tru']
  _attributesList = [
    ('val', 'list_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 526]
  _infoAttr = {'val': ['TRAD2_example', 527]}
  _attributesList = check_append_list('uniform_field_Tru', _attributesList, champ_don_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('uniform_field_Tru', _attributesSynonyms, champ_don_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('uniform_field_Tru', _infoAttr, champ_don_base_Tru._infoAttr)
  _optionals.update(champ_don_base_Tru._optionals)
  _helpDict = {
    'val':
          ('[list] Values of field components.', ''),
  }
  _helpDict = check_append_dict('uniform_field_Tru', _helpDict, champ_don_base_Tru._helpDict)
##### end uniform_field_Tru class

class champ_uniforme_morceaux_Tru(champ_don_base_Tru): #####
  r"""
  Field which is partly constant in space and stationary.
  """
  # From: line 528, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('nom_dom', 'ref_domaine_Tru'),   #
    ('nb_comp', 'entier_3282e0_Tru'),   #
    ('data', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 528]
  _infoAttr = {'data': ['TRAD2_example', 531],
   'nb_comp': ['TRAD2_example', 530],
   'nom_dom': ['TRAD2_example', 529]}
  _attributesList = check_append_list('champ_uniforme_morceaux_Tru', _attributesList, champ_don_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_uniforme_morceaux_Tru', _attributesSynonyms, champ_don_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_uniforme_morceaux_Tru', _infoAttr, champ_don_base_Tru._infoAttr)
  _optionals.update(champ_don_base_Tru._optionals)
  _helpDict = {
    'nom_dom':
          ('[ref_domaine] Name of the domain to which the sub-areas belong.', ''),
    'nb_comp':
          ('[entier] Number of field components.', ''),
    'data':
          ('[bloc_lecture] { Defaut val_def sous_zone_1 val_1 ...\n'
           'sous_zone_i val_i } By default, the value val_def is assigned to the field.\n'
           'It takes the sous_zone_i identifier Sous_Zone (sub_area) type object value, val_i.\n'
           'Sous_Zone (sub_area) type objects must have been previously defined if the operator \n'
           'wishes to use a Champ_Uniforme_Morceaux(partly_uniform_field) type object.',
           ''),
  }
  _helpDict = check_append_dict('champ_uniforme_morceaux_Tru', _helpDict, champ_don_base_Tru._helpDict)
##### end champ_uniforme_morceaux_Tru class

class champ_uniforme_morceaux_tabule_temps_Tru(champ_uniforme_morceaux_Tru): #####
  r"""
  this type of field is constant in space on one or several sub_zones and tabulated 
  as a function of time.
  """
  # From: line 532, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 532]
  _infoAttr = {}
  _attributesList = check_append_list('champ_uniforme_morceaux_tabule_temps_Tru', _attributesList, champ_uniforme_morceaux_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_uniforme_morceaux_tabule_temps_Tru', _attributesSynonyms, champ_uniforme_morceaux_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_uniforme_morceaux_tabule_temps_Tru', _infoAttr, champ_uniforme_morceaux_Tru._infoAttr)
  _optionals.update(champ_uniforme_morceaux_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('champ_uniforme_morceaux_tabule_temps_Tru', _helpDict, champ_uniforme_morceaux_Tru._helpDict)
##### end champ_uniforme_morceaux_tabule_temps_Tru class

class testeur_Tru(interprete_Tru): #####
  r"""
  not_set
  """
  # From: line 533, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('data', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 533]
  _infoAttr = {'data': ['TRAD2_example', 534]}
  _attributesList = check_append_list('testeur_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('testeur_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('testeur_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'data':
          ('[bloc_lecture] not_set', ''),
  }
  _helpDict = check_append_dict('testeur_Tru', _helpDict, interprete_Tru._helpDict)
##### end testeur_Tru class

class valeur_totale_sur_volume_Tru(champ_uniforme_morceaux_Tru): #####
  r"""
  Similar as Champ_Uniforme_Morceaux with the same syntax.
  Used for source terms when we want to specify a source term with a value given for 
  the volume (eg: heat in Watts) and not a value per volume unit (eg: heat in Watts/m3).
  """
  # From: line 535, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 535]
  _infoAttr = {}
  _attributesList = check_append_list('valeur_totale_sur_volume_Tru', _attributesList, champ_uniforme_morceaux_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('valeur_totale_sur_volume_Tru', _attributesSynonyms, champ_uniforme_morceaux_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('valeur_totale_sur_volume_Tru', _infoAttr, champ_uniforme_morceaux_Tru._infoAttr)
  _optionals.update(champ_uniforme_morceaux_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('valeur_totale_sur_volume_Tru', _helpDict, champ_uniforme_morceaux_Tru._helpDict)
##### end valeur_totale_sur_volume_Tru class

class field_func_xyz_Tru(champ_don_base_Tru): #####
  r"""
  Field defined by analytical functions.
  It makes it possible the definition of a field that depends on (x,y,z).
  """
  # From: line 536, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['champ_fonc_xyz_Tru']
  _attributesList = [
    ('dom', 'ref_domaine_Tru'),   #
    ('val', 'listchaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 536]
  _infoAttr = {'dom': ['TRAD2_example', 537], 'val': ['TRAD2_example', 538]}
  _attributesList = check_append_list('field_func_xyz_Tru', _attributesList, champ_don_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('field_func_xyz_Tru', _attributesSynonyms, champ_don_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('field_func_xyz_Tru', _infoAttr, champ_don_base_Tru._infoAttr)
  _optionals.update(champ_don_base_Tru._optionals)
  _helpDict = {
    'dom':
          ('[ref_domaine] Name of domain of calculation.', ''),
    'val':
          ('[listchaine] List of functions on (x,y,z).', ''),
  }
  _helpDict = check_append_dict('field_func_xyz_Tru', _helpDict, champ_don_base_Tru._helpDict)
##### end field_func_xyz_Tru class

class field_func_txyz_Tru(champ_don_base_Tru): #####
  r"""
  Field defined by analytical functions.
  It makes it possible the definition of a field that depends on the time and the space.
  """
  # From: line 539, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['champ_fonc_txyz_Tru']
  _attributesList = [
    ('dom', 'ref_domaine_Tru'),   #
    ('val', 'listchaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 539]
  _infoAttr = {'dom': ['TRAD2_example', 540], 'val': ['TRAD2_example', 541]}
  _attributesList = check_append_list('field_func_txyz_Tru', _attributesList, champ_don_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('field_func_txyz_Tru', _attributesSynonyms, champ_don_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('field_func_txyz_Tru', _infoAttr, champ_don_base_Tru._infoAttr)
  _optionals.update(champ_don_base_Tru._optionals)
  _helpDict = {
    'dom':
          ('[ref_domaine] Name of domain of calculation.', ''),
    'val':
          ('[listchaine] List of functions on (t,x,y,z).', ''),
  }
  _helpDict = check_append_dict('field_func_txyz_Tru', _helpDict, champ_don_base_Tru._helpDict)
##### end field_func_txyz_Tru class

class champ_don_lu_Tru(champ_don_base_Tru): #####
  r"""
  Field to read a data field (values located at the center of the cells) in a file.
  """
  # From: line 542, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('dom', 'ref_domaine_Tru'),   #
    ('nb_comp', 'entier_3282e0_Tru'),   #
    ('file', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 542]
  _infoAttr = {'dom': ['TRAD2_example', 543],
   'file': ['TRAD2_example', 545],
   'nb_comp': ['TRAD2_example', 544]}
  _attributesList = check_append_list('champ_don_lu_Tru', _attributesList, champ_don_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_don_lu_Tru', _attributesSynonyms, champ_don_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_don_lu_Tru', _infoAttr, champ_don_base_Tru._infoAttr)
  _optionals.update(champ_don_base_Tru._optionals)
  _helpDict = {
    'dom':
          ('[ref_domaine] Name of the domain.', ''),
    'nb_comp':
          ('[entier] Number of field components.', ''),
    'file':
          ('[chaine] Name of the file.\n'
           '\n'
           'This file has the following format:\n'
           '\n'
           'nb_val_lues -> Number of values readen in th file\n'
           '\n'
           'Xi Yi Zi -> Coordinates readen in the file\n'
           '\n'
           'Ui Vi Wi -> Value of the field',
           ''),
  }
  _helpDict = check_append_dict('champ_don_lu_Tru', _helpDict, champ_don_base_Tru._helpDict)
##### end champ_don_lu_Tru class

class init_par_partie_Tru(champ_don_base_Tru): #####
  r"""
  ne marche que pour n_comp=1
  """
  # From: line 546, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('n_comp', 'entier_a44528_Tru'),   #
    ('val1', 'floattant_820d8b_Tru'),   #
    ('val2', 'floattant_820d8b_Tru'),   #
    ('val3', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 546]
  _infoAttr = {'n_comp': ['TRAD2_example', 547],
   'val1': ['TRAD2_example', 548],
   'val2': ['TRAD2_example', 549],
   'val3': ['TRAD2_example', 550]}
  _attributesList = check_append_list('init_par_partie_Tru', _attributesList, champ_don_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('init_par_partie_Tru', _attributesSynonyms, champ_don_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('init_par_partie_Tru', _infoAttr, champ_don_base_Tru._infoAttr)
  _optionals.update(champ_don_base_Tru._optionals)
  _helpDict = {
    'n_comp':
          ('[entier(into=[1])] not_set', ''),
    'val1':
          ('[floattant] not_set', ''),
    'val2':
          ('[floattant] not_set', ''),
    'val3':
          ('[floattant] not_set', ''),
  }
  _helpDict = check_append_dict('init_par_partie_Tru', _helpDict, champ_don_base_Tru._helpDict)
##### end init_par_partie_Tru class

class champ_front_uniforme_Tru(front_field_base_Tru): #####
  r"""
  Boundary field which is constant in space and stationary.
  """
  # From: line 551, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('val', 'list_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 551]
  _infoAttr = {'val': ['TRAD2_example', 552]}
  _attributesList = check_append_list('champ_front_uniforme_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_uniforme_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_uniforme_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'val':
          ('[list] Values of field components.', ''),
  }
  _helpDict = check_append_dict('champ_front_uniforme_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_uniforme_Tru class

class champ_front_fonc_pois_ipsn_Tru(front_field_base_Tru): #####
  r"""
  Boundary field champ_front_fonc_pois_ipsn.
  """
  # From: line 553, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('r_tube', 'floattant_820d8b_Tru'),   #
    ('umoy', 'list_Tru'),   #
    ('r_loc', 'listf_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 553]
  _infoAttr = {'r_loc': ['TRAD2_example', 556],
   'r_tube': ['TRAD2_example', 554],
   'umoy': ['TRAD2_example', 555]}
  _attributesList = check_append_list('champ_front_fonc_pois_ipsn_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_fonc_pois_ipsn_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_fonc_pois_ipsn_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'r_tube':
          ('[floattant] not_set', ''),
    'umoy':
          ('[list] not_set', ''),
    'r_loc':
          ('[listf] not_set', ''),
  }
  _helpDict = check_append_dict('champ_front_fonc_pois_ipsn_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_fonc_pois_ipsn_Tru class

class champ_front_fonc_pois_tube_Tru(front_field_base_Tru): #####
  r"""
  Boundary field champ_front_fonc_pois_tube.
  """
  # From: line 557, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('r_tube', 'floattant_820d8b_Tru'),   #
    ('umoy', 'list_Tru'),   #
    ('r_loc', 'listf_Tru'),   #
    ('r_loc_mult', 'listentierf_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 557]
  _infoAttr = {'r_loc': ['TRAD2_example', 560],
   'r_loc_mult': ['TRAD2_example', 561],
   'r_tube': ['TRAD2_example', 558],
   'umoy': ['TRAD2_example', 559]}
  _attributesList = check_append_list('champ_front_fonc_pois_tube_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_fonc_pois_tube_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_fonc_pois_tube_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'r_tube':
          ('[floattant] not_set', ''),
    'umoy':
          ('[list] not_set', ''),
    'r_loc':
          ('[listf] not_set', ''),
    'r_loc_mult':
          ('[listentierf] not_set', ''),
  }
  _helpDict = check_append_dict('champ_front_fonc_pois_tube_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_fonc_pois_tube_Tru class

class champ_front_tangentiel_vef_Tru(front_field_base_Tru): #####
  r"""
  Field to define the tangential velocity vector field standard at the boundary in VEF 
  discretization.
  """
  # From: line 562, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('mot', 'chaine_36f0dc_Tru'),   #
    ('vit_tan', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 562]
  _infoAttr = {'mot': ['TRAD2_example', 563], 'vit_tan': ['TRAD2_example', 564]}
  _attributesList = check_append_list('champ_front_tangentiel_vef_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_tangentiel_vef_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_tangentiel_vef_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'mot':
          ('[chaine(into=["vitesse_tangentielle"])] Name of vector field.', ''),
    'vit_tan':
          ('[floattant] Vector field standard [m/s].', ''),
  }
  _helpDict = check_append_dict('champ_front_tangentiel_vef_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_tangentiel_vef_Tru class

class champ_front_lu_Tru(front_field_base_Tru): #####
  r"""
  boundary field which is given from data issued from a read file.
  The format of this file has to be the same that the one generated by Ecrire_fichier_xyz_valeur
  
  Example for K and epsilon quantities to be defined for inlet condition in a boundary 
  named 'entree': NL2entree frontiere_ouverte_K_Eps_impose Champ_Front_lu dom 2pb_K_EPS_PERIO_1006.306198.dat
  """
  # From: line 565, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('domaine', 'ref_domaine_Tru'),   #
    ('dim', 'entier_3282e0_Tru'),   #
    ('file', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'domaine': ['domain']
  }
  _optionals = set([])
  _infoMain = ['TRAD2_example', 565]
  _infoAttr = {'dim': ['TRAD2_example', 567],
   'domaine': ['TRAD2_example', 566],
   'file': ['TRAD2_example', 568]}
  _attributesList = check_append_list('champ_front_lu_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_lu_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_lu_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'domaine':
          ('[ref_domaine] Name of domain', ''),
    'dim':
          ('[entier] number of components', ''),
    'file':
          ('[chaine] path for the read file', ''),
  }
  _helpDict = check_append_dict('champ_front_lu_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_lu_Tru class

class boundary_field_inward_Tru(front_field_base_Tru): #####
  r"""
  this field is used to define the normal vector field standard at the boundary in VDF 
  or VEF discretization.
  """
  # From: line 569, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('normal_value', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 569]
  _infoAttr = {'normal_value': ['TRAD2_example', 570]}
  _attributesList = check_append_list('boundary_field_inward_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('boundary_field_inward_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('boundary_field_inward_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'normal_value':
          ('[chaine] normal vector value (positive value for a vector oriented outside to inside) \n'
           'which can depend of the time.',
           ''),
  }
  _helpDict = check_append_dict('boundary_field_inward_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end boundary_field_inward_Tru class

class milieu_base_Tru(ConstrainBase_Tru): #####
  r"""
  Basic class for medium (physics properties of medium).
  """
  # From: line 1514, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = [
    ('gravite', '<inherited>field_base_Tru'),   #
    ('porosites_champ', '<inherited>field_base_Tru'),   #
    ('diametre_hyd_champ', '<inherited>field_base_Tru'),   #
    ('porosites', 'porosites_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['gravite', 'porosites_champ', 'diametre_hyd_champ', 'porosites'])
  _infoMain = ['TRAD2_example', 1514]
  _infoAttr = {'diametre_hyd_champ': ['TRAD2_example', 1517],
   'gravite': ['TRAD2_example', 1515],
   'porosites': ['TRAD2_example', 1518],
   'porosites_champ': ['TRAD2_example', 1516]}
  _helpDict = {
    'gravite':
          ('[field_base] Gravity field (optional).', ''),
    'porosites_champ':
          ('[field_base] The porosity is given at each element and the porosity at each face, \n'
           'Psi(face), is calculated by the average of the porosities of the two neighbour elements \n'
           'Psi(elem1), Psi(elem2) : Psi(face)=2/(1/Psi(elem1)+1/Psi(elem2)).\n'
           'This keyword is optional.',
           ''),
    'diametre_hyd_champ':
          ('[field_base] Hydraulic diameter field (optional).', ''),
    'porosites':
          ('[porosites] Porosities.', ''),
  }
##### end milieu_base_Tru class

class constituant_Tru(milieu_base_Tru): #####
  r"""
  Constituent.
  """
  # From: line 571, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('rho', '<inherited>field_base_Tru'),   #
    ('cp', '<inherited>field_base_Tru'),   #
    ('Lambda', '<inherited>field_base_Tru'),   #
    ('coefficient_diffusion', '<inherited>field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'Lambda': ['lambda_u']
  }
  _optionals = set(['rho', 'cp', 'Lambda', 'coefficient_diffusion'])
  _infoMain = ['TRAD2_example', 571]
  _infoAttr = {'Lambda': ['TRAD2_example', 574],
   'coefficient_diffusion': ['TRAD2_example', 575],
   'cp': ['TRAD2_example', 573],
   'rho': ['TRAD2_example', 572]}
  _attributesList = check_append_list('constituant_Tru', _attributesList, milieu_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('constituant_Tru', _attributesSynonyms, milieu_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('constituant_Tru', _infoAttr, milieu_base_Tru._infoAttr)
  _optionals.update(milieu_base_Tru._optionals)
  _helpDict = {
    'rho':
          ('[field_base] Density (kg.m-3).', ''),
    'cp':
          ('[field_base] Specific heat (J.kg-1.K-1).', ''),
    'Lambda':
          ('[field_base] Conductivity (W.m-1.K-1).', ''),
    'coefficient_diffusion':
          ('[field_base] Constituent diffusion coefficient value (m2.s-1).\n'
           'If a multi-constituent problem is being processed, the diffusivite will be a vectorial \n'
           'and each components will be the diffusion of the constituent.',
           ''),
  }
  _helpDict = check_append_dict('constituant_Tru', _helpDict, milieu_base_Tru._helpDict)
##### end constituant_Tru class

class champ_front_pression_from_u_Tru(front_field_base_Tru): #####
  r"""
  this field is used to define a pressure field depending of a velocity field.
  """
  # From: line 576, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('expression', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 576]
  _infoAttr = {'expression': ['TRAD2_example', 577]}
  _attributesList = check_append_list('champ_front_pression_from_u_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_pression_from_u_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_pression_from_u_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'expression':
          ('[chaine] value depending of a velocity (like $2*u_moy^2$).', ''),
  }
  _helpDict = check_append_dict('champ_front_pression_from_u_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_pression_from_u_Tru class

class source_constituant_Tru(source_base_Tru): #####
  r"""
  Keyword to specify source rates, in [[C]/s], for each one of the nb constituents.
  [C] is the concentration unit.
  """
  # From: line 578, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('ch', '<inherited>field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 578]
  _infoAttr = {'ch': ['TRAD2_example', 579]}
  _attributesList = check_append_list('source_constituant_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('source_constituant_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('source_constituant_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'ch':
          ('[field_base] Field type.', ''),
  }
  _helpDict = check_append_dict('source_constituant_Tru', _helpDict, source_base_Tru._helpDict)
##### end source_constituant_Tru class

class radioactive_decay_Tru(source_base_Tru): #####
  r"""
  Radioactive decay source term of the form $-\lambda_i c_i$, where $0 \leq i \leq N$, 
  N is the number of component of the constituent, $c_i$ and $\lambda_i$ are the concentration 
  and the decay constant of the i-th component of the constituant.
  """
  # From: line 580, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('val', 'list_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 580]
  _infoAttr = {'val': ['TRAD2_example', 581]}
  _attributesList = check_append_list('radioactive_decay_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('radioactive_decay_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('radioactive_decay_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'val':
          ('[list] n is the number of decay constants to read (int), and val1, val2...\n'
           'are the decay constants (double)',
           ''),
  }
  _helpDict = check_append_dict('radioactive_decay_Tru', _helpDict, source_base_Tru._helpDict)
##### end radioactive_decay_Tru class

class form_a_nb_points_Tru(objet_lecture_Tru): #####
  r"""
  The structure fonction is calculated on nb points and we should add the 2 directions 
  (0:OX, 1:OY, 2:OZ) constituting the homegeneity planes.
  Example for channel flows, planes parallel to the walls.
  """
  # From: line 582, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('nb', 'entier_6f194b_Tru'),   #
    ('dir1', 'entier_fd157e_Tru'),   #
    ('dir2', 'entier_fd157e_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 582]
  _infoAttr = {'dir1': ['TRAD2_example', 584],
   'dir2': ['TRAD2_example', 585],
   'nb': ['TRAD2_example', 583]}
  _attributesList = check_append_list('form_a_nb_points_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('form_a_nb_points_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('form_a_nb_points_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'nb':
          ('[entier(into=[4])] Number of points.', ''),
    'dir1':
          ('[entier(max=2)] First direction.', ''),
    'dir2':
          ('[entier(max=2)] Second direction.', ''),
  }
  _helpDict = check_append_dict('form_a_nb_points_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end form_a_nb_points_Tru class

class tparoi_vef_Tru(champ_post_de_champs_post_Tru): #####
  r"""
  This keyword is used to post process (only for VEF discretization) the temperature 
  field with a slight difference on boundaries with Neumann condition where law of the 
  wall is applied on the temperature field.
  nom_pb is the problem name and field_name is the selected field name.
  A keyword (temperature_physique) is available to post process this field without 
  using Definition_champs.
  """
  # From: line 586, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['champ_post_tparoi_vef_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 586]
  _infoAttr = {}
  _attributesList = check_append_list('tparoi_vef_Tru', _attributesList, champ_post_de_champs_post_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('tparoi_vef_Tru', _attributesSynonyms, champ_post_de_champs_post_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('tparoi_vef_Tru', _infoAttr, champ_post_de_champs_post_Tru._infoAttr)
  _optionals.update(champ_post_de_champs_post_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('tparoi_vef_Tru', _helpDict, champ_post_de_champs_post_Tru._helpDict)
##### end tparoi_vef_Tru class

class fourfloat_Tru(objet_lecture_Tru): #####
  r"""
  Four reals.
  """
  # From: line 587, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('a', 'floattant_820d8b_Tru'),   #
    ('b', 'floattant_820d8b_Tru'),   #
    ('c', 'floattant_820d8b_Tru'),   #
    ('d', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 587]
  _infoAttr = {'a': ['TRAD2_example', 588],
   'b': ['TRAD2_example', 589],
   'c': ['TRAD2_example', 590],
   'd': ['TRAD2_example', 591]}
  _attributesList = check_append_list('fourfloat_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('fourfloat_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('fourfloat_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'a':
          ('[floattant] First real.', ''),
    'b':
          ('[floattant] Second real.', ''),
    'c':
          ('[floattant] Third real.', ''),
    'd':
          ('[floattant] Fourth real.', ''),
  }
  _helpDict = check_append_dict('fourfloat_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end fourfloat_Tru class

class twofloat_Tru(objet_lecture_Tru): #####
  r"""
  two reals.
  """
  # From: line 592, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('a', 'floattant_820d8b_Tru'),   #
    ('b', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 592]
  _infoAttr = {'a': ['TRAD2_example', 593], 'b': ['TRAD2_example', 594]}
  _attributesList = check_append_list('twofloat_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('twofloat_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('twofloat_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'a':
          ('[floattant] First real.', ''),
    'b':
          ('[floattant] Second real.', ''),
  }
  _helpDict = check_append_dict('twofloat_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end twofloat_Tru class

class sonde_tble_Tru(objet_lecture_Tru): #####
  r"""
  not_set
  """
  # From: line 595, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('name', 'chaine_Tru'),   #
    ('point', 'un_point_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 595]
  _infoAttr = {'name': ['TRAD2_example', 596], 'point': ['TRAD2_example', 597]}
  _attributesList = check_append_list('sonde_tble_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('sonde_tble_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('sonde_tble_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'name':
          ('[chaine] not_set', ''),
    'point':
          ('[un_point] not_set', ''),
  }
  _helpDict = check_append_dict('sonde_tble_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end sonde_tble_Tru class

class liste_sonde_tble_Tru(ListOfBase_Tru): #####
  """
  not_set
  """
  # From: line 598, in file 'TRAD2_example'
  _braces = 0
  _comma = 0
  _allowedClasses = ["sonde_tble_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 598]
##### end liste_sonde_tble_Tru class

class paroi_Tru(condlim_base_Tru): #####
  r"""
  Impermeability condition at a wall called bord (edge) (standard flux zero).
  This condition must be associated with a wall type hydraulic condition.
  """
  # From: line 599, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 599]
  _infoAttr = {}
  _attributesList = check_append_list('paroi_Tru', _attributesList, condlim_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('paroi_Tru', _attributesSynonyms, condlim_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('paroi_Tru', _infoAttr, condlim_base_Tru._infoAttr)
  _optionals.update(condlim_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('paroi_Tru', _helpDict, condlim_base_Tru._helpDict)
##### end paroi_Tru class

class precond_base_Tru(ConstrainBase_Tru): #####
  r"""
  Basic class for preconditioning.
  """
  # From: line 600, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 600]
  _infoAttr = {}
  _helpDict = {}
##### end precond_base_Tru class

class ssor_Tru(precond_base_Tru): #####
  r"""
  Symmetric successive over-relaxation algorithm.
  """
  # From: line 601, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('omega', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['omega'])
  _infoMain = ['TRAD2_example', 601]
  _infoAttr = {'omega': ['TRAD2_example', 602]}
  _attributesList = check_append_list('ssor_Tru', _attributesList, precond_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('ssor_Tru', _attributesSynonyms, precond_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('ssor_Tru', _infoAttr, precond_base_Tru._infoAttr)
  _optionals.update(precond_base_Tru._optionals)
  _helpDict = {
    'omega':
          ('[floattant] Over-relaxation facteur (between 1 and 2, default value 1.6).', ''),
  }
  _helpDict = check_append_dict('ssor_Tru', _helpDict, precond_base_Tru._helpDict)
##### end ssor_Tru class

class ssor_bloc_Tru(precond_base_Tru): #####
  r"""
  not_set
  """
  # From: line 603, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('alpha_0', 'floattant_820d8b_Tru'),   #
    ('precond0', '<inherited>precond_base_Tru'),   #
    ('alpha_1', 'floattant_820d8b_Tru'),   #
    ('precond1', '<inherited>precond_base_Tru'),   #
    ('alpha_a', 'floattant_820d8b_Tru'),   #
    ('preconda', '<inherited>precond_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['alpha_0', 'precond0', 'alpha_1', 'precond1', 'alpha_a', 'preconda'])
  _infoMain = ['TRAD2_example', 603]
  _infoAttr = {'alpha_0': ['TRAD2_example', 604],
   'alpha_1': ['TRAD2_example', 606],
   'alpha_a': ['TRAD2_example', 608],
   'precond0': ['TRAD2_example', 605],
   'precond1': ['TRAD2_example', 607],
   'preconda': ['TRAD2_example', 609]}
  _attributesList = check_append_list('ssor_bloc_Tru', _attributesList, precond_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('ssor_bloc_Tru', _attributesSynonyms, precond_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('ssor_bloc_Tru', _infoAttr, precond_base_Tru._infoAttr)
  _optionals.update(precond_base_Tru._optionals)
  _helpDict = {
    'alpha_0':
          ('[floattant] not_set', ''),
    'precond0':
          ('[precond_base] not_set', ''),
    'alpha_1':
          ('[floattant] not_set', ''),
    'precond1':
          ('[precond_base] not_set', ''),
    'alpha_a':
          ('[floattant] not_set', ''),
    'preconda':
          ('[precond_base] not_set', ''),
  }
  _helpDict = check_append_dict('ssor_bloc_Tru', _helpDict, precond_base_Tru._helpDict)
##### end ssor_bloc_Tru class

class precondsolv_Tru(precond_base_Tru): #####
  r"""
  not_set
  """
  # From: line 610, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('solveur', '<inherited>solveur_sys_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 610]
  _infoAttr = {'solveur': ['TRAD2_example', 611]}
  _attributesList = check_append_list('precondsolv_Tru', _attributesList, precond_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('precondsolv_Tru', _attributesSynonyms, precond_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('precondsolv_Tru', _infoAttr, precond_base_Tru._infoAttr)
  _optionals.update(precond_base_Tru._optionals)
  _helpDict = {
    'solveur':
          ('[solveur_sys_base] Solver type.', ''),
  }
  _helpDict = check_append_dict('precondsolv_Tru', _helpDict, precond_base_Tru._helpDict)
##### end precondsolv_Tru class

class optimal_Tru(solveur_sys_base_Tru): #####
  r"""
  Optimal is a solver which tests several solvers of the previous list to choose the 
  fastest one for the considered linear system.
  """
  # From: line 612, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('seuil', 'floattant_820d8b_Tru'),   #
    ('impr', 'rien_Tru'),   #
    ('quiet', 'rien_Tru'),   #
    ('save_matrice', 'rien_Tru'),   #
    ('frequence_recalc', 'entier_3282e0_Tru'),   #
    ('nom_fichier_solveur', 'chaine_Tru'),   #
    ('fichier_solveur_non_recree', 'rien_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'save_matrice': ['save_matrix']
  }
  _optionals = set(['impr',
   'quiet',
   'save_matrice',
   'frequence_recalc',
   'nom_fichier_solveur',
   'fichier_solveur_non_recree'])
  _infoMain = ['TRAD2_example', 612]
  _infoAttr = {'fichier_solveur_non_recree': ['TRAD2_example', 619],
   'frequence_recalc': ['TRAD2_example', 617],
   'impr': ['TRAD2_example', 614],
   'nom_fichier_solveur': ['TRAD2_example', 618],
   'quiet': ['TRAD2_example', 615],
   'save_matrice': ['TRAD2_example', 616],
   'seuil': ['TRAD2_example', 613]}
  _attributesList = check_append_list('optimal_Tru', _attributesList, solveur_sys_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('optimal_Tru', _attributesSynonyms, solveur_sys_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('optimal_Tru', _infoAttr, solveur_sys_base_Tru._infoAttr)
  _optionals.update(solveur_sys_base_Tru._optionals)
  _helpDict = {
    'seuil':
          ('[floattant] Convergence threshold', ''),
    'impr':
          ('[rien] To print the convergency of the fastest solver', ''),
    'quiet':
          ('[rien] To disable printing of information', ''),
    'save_matrice':
          ('[rien] To save the linear system (A, x, B) into a file', ''),
    'frequence_recalc':
          ('[entier] To set a time step period (by default, 100) for re-checking the fatest solver', ''),
    'nom_fichier_solveur':
          ('[chaine] To specify the file containing the list of the tested solvers', ''),
    'fichier_solveur_non_recree':
          ('[rien] To avoid the creation of the file containing the list', ''),
  }
  _helpDict = check_append_dict('optimal_Tru', _helpDict, solveur_sys_base_Tru._helpDict)
##### end optimal_Tru class

class test_solveur_Tru(interprete_Tru): #####
  r"""
  To test several solvers
  """
  # From: line 620, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('fichier_secmem', 'chaine_Tru'),   #
    ('fichier_matrice', 'chaine_Tru'),   #
    ('fichier_solution', 'chaine_Tru'),   #
    ('nb_test', 'entier_3282e0_Tru'),   #
    ('impr', 'rien_Tru'),   #
    ('solveur', '<inherited>solveur_sys_base_Tru'),   #
    ('fichier_solveur', 'chaine_Tru'),   #
    ('genere_fichier_solveur', 'floattant_820d8b_Tru'),   #
    ('seuil_verification', 'floattant_820d8b_Tru'),   #
    ('pas_de_solution_initiale', 'rien_Tru'),   #
    ('ascii', 'rien_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['fichier_secmem',
   'fichier_matrice',
   'fichier_solution',
   'nb_test',
   'impr',
   'solveur',
   'fichier_solveur',
   'genere_fichier_solveur',
   'seuil_verification',
   'pas_de_solution_initiale',
   'ascii'])
  _infoMain = ['TRAD2_example', 620]
  _infoAttr = {'ascii': ['TRAD2_example', 631],
   'fichier_matrice': ['TRAD2_example', 622],
   'fichier_secmem': ['TRAD2_example', 621],
   'fichier_solution': ['TRAD2_example', 623],
   'fichier_solveur': ['TRAD2_example', 627],
   'genere_fichier_solveur': ['TRAD2_example', 628],
   'impr': ['TRAD2_example', 625],
   'nb_test': ['TRAD2_example', 624],
   'pas_de_solution_initiale': ['TRAD2_example', 630],
   'seuil_verification': ['TRAD2_example', 629],
   'solveur': ['TRAD2_example', 626]}
  _attributesList = check_append_list('test_solveur_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('test_solveur_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('test_solveur_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'fichier_secmem':
          ('[chaine] Filename containing the second member B', ''),
    'fichier_matrice':
          ('[chaine] Filename containing the matrix A', ''),
    'fichier_solution':
          ('[chaine] Filename containing the solution x', ''),
    'nb_test':
          ('[entier] Number of tests to measure the time resolution (one preconditionnement)', ''),
    'impr':
          ('[rien] To print the convergence solver', ''),
    'solveur':
          ('[solveur_sys_base] To specify a solver', ''),
    'fichier_solveur':
          ('[chaine] To specify a file containing a list of solvers', ''),
    'genere_fichier_solveur':
          ('[floattant] To create a file of the solver with a threshold convergence', ''),
    'seuil_verification':
          ('[floattant] Check if the solution satisfy ||Ax-B||<precision', ''),
    'pas_de_solution_initiale':
          ("[rien] Resolution isn't initialized with the solution x", ''),
    'ascii':
          ('[rien] Ascii files', ''),
  }
  _helpDict = check_append_dict('test_solveur_Tru', _helpDict, interprete_Tru._helpDict)
##### end test_solveur_Tru class

class ilu_Tru(precond_base_Tru): #####
  r"""
  This preconditionner can be only used with the generic GEN solver.
  """
  # From: line 632, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('type', 'entier_3282e0_Tru'),   #
    ('filling', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['type', 'filling'])
  _infoMain = ['TRAD2_example', 632]
  _infoAttr = {'filling': ['TRAD2_example', 634], 'type': ['TRAD2_example', 633]}
  _attributesList = check_append_list('ilu_Tru', _attributesList, precond_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('ilu_Tru', _attributesSynonyms, precond_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('ilu_Tru', _infoAttr, precond_base_Tru._infoAttr)
  _optionals.update(precond_base_Tru._optionals)
  _helpDict = {
    'type':
          ('[entier] values can be 0|1|2|3 for null|left|right|left-and-right preconditionning \n'
           '(default value = 2)',
           ''),
    'filling':
          ('[entier] default value = 1.', ''),
  }
  _helpDict = check_append_dict('ilu_Tru', _helpDict, precond_base_Tru._helpDict)
##### end ilu_Tru class

class gen_Tru(solveur_sys_base_Tru): #####
  r"""
  not_set
  """
  # From: line 635, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('solv_elem', 'chaine_Tru'),   #
    ('precond', '<inherited>precond_base_Tru'),   #
    ('seuil', 'floattant_820d8b_Tru'),   #
    ('impr', 'rien_Tru'),   #
    ('save_matrice', 'rien_Tru'),   #
    ('quiet', 'rien_Tru'),   #
    ('nb_it_max', 'entier_3282e0_Tru'),   #
    ('force', 'rien_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'save_matrice': ['save_matrix']
  }
  _optionals = set(['seuil', 'impr', 'save_matrice', 'quiet', 'nb_it_max', 'force'])
  _infoMain = ['TRAD2_example', 635]
  _infoAttr = {'force': ['TRAD2_example', 643],
   'impr': ['TRAD2_example', 639],
   'nb_it_max': ['TRAD2_example', 642],
   'precond': ['TRAD2_example', 637],
   'quiet': ['TRAD2_example', 641],
   'save_matrice': ['TRAD2_example', 640],
   'seuil': ['TRAD2_example', 638],
   'solv_elem': ['TRAD2_example', 636]}
  _attributesList = check_append_list('gen_Tru', _attributesList, solveur_sys_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('gen_Tru', _attributesSynonyms, solveur_sys_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('gen_Tru', _infoAttr, solveur_sys_base_Tru._infoAttr)
  _optionals.update(solveur_sys_base_Tru._optionals)
  _helpDict = {
    'solv_elem':
          ('[chaine] To specify a solver among gmres or bicgstab.', ''),
    'precond':
          ('[precond_base] The only preconditionner that we can specify is ilu.', ''),
    'seuil':
          ('[floattant] Value of the final residue.\n'
           'The solver ceases iterations when the Euclidean residue standard ||Ax-B|| is less \n'
           'than this value.\n'
           'default value 1e-12.',
           ''),
    'impr':
          ('[rien] Keyword which is used to request display of the Euclidean residue standard \n'
           'each time this iterates through the conjugated gradient (display to the standard outlet).',
           ''),
    'save_matrice':
          ('[rien] To save the matrix in a file.', ''),
    'quiet':
          ('[rien] To not displaying any outputs of the solver.', ''),
    'nb_it_max':
          ('[entier] Keyword to set the maximum iterations number for the GEN solver.', ''),
    'force':
          ('[rien] Keyword to set ipar[5]=-1 in the GEN solver.\n'
           'This is helpful if you notice that the solver does not perform more than 100 iterations.\n'
           'If this keyword is specified in the datafile, you should provide nb_it_max.',
           ''),
  }
  _helpDict = check_append_dict('gen_Tru', _helpDict, solveur_sys_base_Tru._helpDict)
##### end gen_Tru class

class petsc_Tru(solveur_sys_base_Tru): #####
  r"""
  Solver via Petsc API
  
  \input{{solvpetsc}}
  """
  # From: line 644, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('solveur', 'chaine_Tru'),   #
    ('option_solveur', 'bloc_lecture_Tru'),   #
    ('atol', 'floattant_820d8b_Tru'),   #
    ('rtol', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['atol', 'rtol'])
  _infoMain = ['TRAD2_example', 644]
  _infoAttr = {'atol': ['TRAD2_example', 647],
   'option_solveur': ['TRAD2_example', 646],
   'rtol': ['TRAD2_example', 648],
   'solveur': ['TRAD2_example', 645]}
  _attributesList = check_append_list('petsc_Tru', _attributesList, solveur_sys_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('petsc_Tru', _attributesSynonyms, solveur_sys_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('petsc_Tru', _infoAttr, solveur_sys_base_Tru._infoAttr)
  _optionals.update(solveur_sys_base_Tru._optionals)
  _helpDict = {
    'solveur':
          ('[chaine] not_set', ''),
    'option_solveur':
          ('[bloc_lecture] not_set', ''),
    'atol':
          ('[floattant] Absolute threshold for convergence (same as seuil option)', ''),
    'rtol':
          ('[floattant] Relative threshold for convergence', ''),
  }
  _helpDict = check_append_dict('petsc_Tru', _helpDict, solveur_sys_base_Tru._helpDict)
##### end petsc_Tru class

class amgx_Tru(petsc_Tru): #####
  r"""
  Solver via AmgX API
  """
  # From: line 649, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('solveur', 'chaine_Tru'),   #
    ('option_solveur', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 649]
  _infoAttr = {'option_solveur': ['TRAD2_example', 651], 'solveur': ['TRAD2_example', 650]}
  _attributesList = check_append_list('amgx_Tru', _attributesList, petsc_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('amgx_Tru', _attributesSynonyms, petsc_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('amgx_Tru', _infoAttr, petsc_Tru._infoAttr)
  _optionals.update(petsc_Tru._optionals)
  _helpDict = {
    'solveur':
          ('[chaine] not_set', ''),
    'option_solveur':
          ('[bloc_lecture] not_set', ''),
  }
  _helpDict = check_append_dict('amgx_Tru', _helpDict, petsc_Tru._helpDict)
##### end amgx_Tru class

class rocalution_Tru(petsc_Tru): #####
  r"""
  Solver via rocALUTION API
  """
  # From: line 652, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('solveur', 'chaine_Tru'),   #
    ('option_solveur', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 652]
  _infoAttr = {'option_solveur': ['TRAD2_example', 654], 'solveur': ['TRAD2_example', 653]}
  _attributesList = check_append_list('rocalution_Tru', _attributesList, petsc_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('rocalution_Tru', _attributesSynonyms, petsc_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('rocalution_Tru', _infoAttr, petsc_Tru._infoAttr)
  _optionals.update(petsc_Tru._optionals)
  _helpDict = {
    'solveur':
          ('[chaine] not_set', ''),
    'option_solveur':
          ('[bloc_lecture] not_set', ''),
  }
  _helpDict = check_append_dict('rocalution_Tru', _helpDict, petsc_Tru._helpDict)
##### end rocalution_Tru class

class solv_gcp_Tru(solveur_sys_base_Tru): #####
  r"""
  Preconditioned conjugated gradient.
  """
  # From: line 655, in file 'TRAD2_example'
  _braces = 1
  _synonyms = ['gcp_Tru']
  _attributesList = [
    ('precond', '<inherited>precond_base_Tru'),   #
    ('precond_nul', 'rien_Tru'),   #
    ('seuil', 'floattant_820d8b_Tru'),   #
    ('impr', 'rien_Tru'),   #
    ('quiet', 'rien_Tru'),   #
    ('save_matrice', 'rien_Tru'),   #
    ('optimized', 'rien_Tru'),   #
    ('nb_it_max', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'save_matrice': ['save_matrix']
  }
  _optionals = set(['precond',
   'precond_nul',
   'impr',
   'quiet',
   'save_matrice',
   'optimized',
   'nb_it_max'])
  _infoMain = ['TRAD2_example', 655]
  _infoAttr = {'impr': ['TRAD2_example', 659],
   'nb_it_max': ['TRAD2_example', 663],
   'optimized': ['TRAD2_example', 662],
   'precond': ['TRAD2_example', 656],
   'precond_nul': ['TRAD2_example', 657],
   'quiet': ['TRAD2_example', 660],
   'save_matrice': ['TRAD2_example', 661],
   'seuil': ['TRAD2_example', 658]}
  _attributesList = check_append_list('solv_gcp_Tru', _attributesList, solveur_sys_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('solv_gcp_Tru', _attributesSynonyms, solveur_sys_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('solv_gcp_Tru', _infoAttr, solveur_sys_base_Tru._infoAttr)
  _optionals.update(solveur_sys_base_Tru._optionals)
  _helpDict = {
    'precond':
          ('[precond_base] Keyword to define system preconditioning in order to accelerate resolution \n'
           'by the conjugated gradient.\n'
           'Many parallel preconditioning methods are not equivalent to their sequential counterpart, \n'
           'and you should therefore expect differences, especially when you select a high value \n'
           'of the final residue (seuil).\n'
           'The result depends on the number of processors and on the mesh splitting.\n'
           'It is sometimes useful to run the solver with no preconditioning at all.\n'
           'In particular:\n'
           '\n'
           '- when the solver does not converge during initial projection,\n'
           '\n'
           '- when comparing sequential and parallel computations.\n'
           '\n'
           'With no preconditioning, except in some particular cases (no open boundary), the \n'
           'sequential and the parallel computations should provide exactly the same results within \n'
           'fpu accuracy.\n'
           'If not, there might be a coding error or the system of equations is singular.',
           ''),
    'precond_nul':
          ('[rien] Keyword to not use a preconditioning method.', ''),
    'seuil':
          ('[floattant] Value of the final residue.\n'
           'The gradient ceases iteration when the Euclidean residue standard ||Ax-B|| is less \n'
           'than this value.',
           ''),
    'impr':
          ('[rien] Keyword which is used to request display of the Euclidean residue standard \n'
           'each time this iterates through the conjugated gradient (display to the standard outlet).',
           ''),
    'quiet':
          ('[rien] To not displaying any outputs of the solver.', ''),
    'save_matrice':
          ('[rien] to save the matrix in a file.', ''),
    'optimized':
          ('[rien] This keyword triggers a memory and network optimized algorithms useful for \n'
           'strong scaling (when computing less than 100 000 elements per processor).\n'
           'The matrix and the vectors are duplicated, common items removed and only virtual \n'
           'items really used in the matrix are exchanged.NL2 Warning: this is experimental and \n'
           'known to fail in some VEF computations (L2 projection step will not converge).\n'
           'Works well in VDF.',
           ''),
    'nb_it_max':
          ('[entier] Keyword to set the maximum iterations number for the Gcp.', ''),
  }
  _helpDict = check_append_dict('solv_gcp_Tru', _helpDict, solveur_sys_base_Tru._helpDict)
##### end solv_gcp_Tru class

class gcp_ns_Tru(solv_gcp_Tru): #####
  r"""
  not_set
  """
  # From: line 664, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('solveur0', '<inherited>solveur_sys_base_Tru'),   #
    ('solveur1', '<inherited>solveur_sys_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 664]
  _infoAttr = {'solveur0': ['TRAD2_example', 665], 'solveur1': ['TRAD2_example', 666]}
  _attributesList = check_append_list('gcp_ns_Tru', _attributesList, solv_gcp_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('gcp_ns_Tru', _attributesSynonyms, solv_gcp_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('gcp_ns_Tru', _infoAttr, solv_gcp_Tru._infoAttr)
  _optionals.update(solv_gcp_Tru._optionals)
  _helpDict = {
    'solveur0':
          ('[solveur_sys_base] Solver type.', ''),
    'solveur1':
          ('[solveur_sys_base] Solver type.', ''),
  }
  _helpDict = check_append_dict('gcp_ns_Tru', _helpDict, solv_gcp_Tru._helpDict)
##### end gcp_ns_Tru class

class cholesky_Tru(solveur_sys_base_Tru): #####
  r"""
  Cholesky direct method.
  """
  # From: line 667, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('impr', 'rien_Tru'),   #
    ('quiet', 'rien_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['impr', 'quiet'])
  _infoMain = ['TRAD2_example', 667]
  _infoAttr = {'impr': ['TRAD2_example', 668], 'quiet': ['TRAD2_example', 669]}
  _attributesList = check_append_list('cholesky_Tru', _attributesList, solveur_sys_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('cholesky_Tru', _attributesSynonyms, solveur_sys_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('cholesky_Tru', _infoAttr, solveur_sys_base_Tru._infoAttr)
  _optionals.update(solveur_sys_base_Tru._optionals)
  _helpDict = {
    'impr':
          ('[rien] Keyword which may be used to print the resolution time.', ''),
    'quiet':
          ('[rien] To disable printing of information', ''),
  }
  _helpDict = check_append_dict('cholesky_Tru', _helpDict, solveur_sys_base_Tru._helpDict)
##### end cholesky_Tru class

class gmres_Tru(solveur_sys_base_Tru): #####
  r"""
  Gmres method (for non symetric matrix).
  """
  # From: line 670, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('impr', 'rien_Tru'),   #
    ('quiet', 'rien_Tru'),   #
    ('seuil', 'floattant_820d8b_Tru'),   #
    ('diag', 'rien_Tru'),   #
    ('nb_it_max', 'entier_3282e0_Tru'),   #
    ('controle_residu', 'entier_b08690_Tru'),   #
    ('save_matrice', 'rien_Tru'),   #
    ('dim_espace_krilov', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'save_matrice': ['save_matrix']
  }
  _optionals = set(['impr',
   'quiet',
   'seuil',
   'diag',
   'nb_it_max',
   'controle_residu',
   'save_matrice',
   'dim_espace_krilov'])
  _infoMain = ['TRAD2_example', 670]
  _infoAttr = {'controle_residu': ['TRAD2_example', 676],
   'diag': ['TRAD2_example', 674],
   'dim_espace_krilov': ['TRAD2_example', 678],
   'impr': ['TRAD2_example', 671],
   'nb_it_max': ['TRAD2_example', 675],
   'quiet': ['TRAD2_example', 672],
   'save_matrice': ['TRAD2_example', 677],
   'seuil': ['TRAD2_example', 673]}
  _attributesList = check_append_list('gmres_Tru', _attributesList, solveur_sys_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('gmres_Tru', _attributesSynonyms, solveur_sys_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('gmres_Tru', _infoAttr, solveur_sys_base_Tru._infoAttr)
  _optionals.update(solveur_sys_base_Tru._optionals)
  _helpDict = {
    'impr':
          ('[rien] Keyword which may be used to print the convergence.', ''),
    'quiet':
          ('[rien] To disable printing of information', ''),
    'seuil':
          ('[floattant] Convergence value.', ''),
    'diag':
          ('[rien] Keyword to use diagonal preconditionner (in place of pilut that is not parallel).', ''),
    'nb_it_max':
          ('[entier] Keyword to set the maximum iterations number for the Gmres.', ''),
    'controle_residu':
          ('[entier(into=["0","1"])] Keyword of Boolean type (by default 0).\n'
           'If set to 1, the convergence occurs if the residu suddenly increases.',
           ''),
    'save_matrice':
          ('[rien] to save the matrix in a file.', ''),
    'dim_espace_krilov':
          ('[entier] not_set', ''),
  }
  _helpDict = check_append_dict('gmres_Tru', _helpDict, solveur_sys_base_Tru._helpDict)
##### end gmres_Tru class

class postraiter_domaine_Tru(interprete_Tru): #####
  r"""
  To write one or more domains in a file with a specified format (MED,LML,LATA).
  """
  # From: line 679, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('format', 'chaine_f07ecc_Tru'),   #
    ('fichier', 'chaine_Tru'),   #
    ('domaine', 'ref_domaine_Tru'),   #
    ('sous_zone', 'ref_sous_zone_Tru'),   #
    ('domaines', 'bloc_lecture_Tru'),   #
    ('joints_non_postraites', 'entier_8bf71a_Tru'),   #
    ('binaire', 'entier_8bf71a_Tru'),   #
    ('ecrire_frontiere', 'entier_8bf71a_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'domaine': ['domain'], 'fichier': ['file']
  }
  _optionals = set(['fichier',
   'domaine',
   'sous_zone',
   'domaines',
   'joints_non_postraites',
   'binaire',
   'ecrire_frontiere'])
  _infoMain = ['TRAD2_example', 679]
  _infoAttr = {'binaire': ['TRAD2_example', 686],
   'domaine': ['TRAD2_example', 682],
   'domaines': ['TRAD2_example', 684],
   'ecrire_frontiere': ['TRAD2_example', 687],
   'fichier': ['TRAD2_example', 681],
   'format': ['TRAD2_example', 680],
   'joints_non_postraites': ['TRAD2_example', 685],
   'sous_zone': ['TRAD2_example', 683]}
  _attributesList = check_append_list('postraiter_domaine_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('postraiter_domaine_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('postraiter_domaine_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'format':
          ('[chaine(into=["lml","lata","lata_v2","med"])] File format.', ''),
    'fichier':
          ('[chaine] The file name can be changed with the fichier option.', ''),
    'domaine':
          ('[ref_domaine] Name of domain', ''),
    'sous_zone':
          ('[ref_sous_zone] Name of the sub_zone', ''),
    'domaines':
          ('[bloc_lecture] Names of domains : { name1 name2 }', ''),
    'joints_non_postraites':
          ('[entier(into=[0,1])] The joints_non_postraites (1 by default) will not write the boundaries \n'
           'between the partitioned mesh.',
           ''),
    'binaire':
          ('[entier(into=[0,1])] Binary (binaire 1) or ASCII (binaire 0) may be used.\n'
           'By default, it is 0 for LATA and only ASCII is available for LML and only binary \n'
           'is available for MED.',
           ''),
    'ecrire_frontiere':
          ('[entier(into=[0,1])] This option will write (if set to 1, the default) or not (if \n'
           'set to 0) the boundaries as fields into the file (it is useful to not add the boundaries \n'
           'when writing a domain extracted from another domain)',
           ''),
  }
  _helpDict = check_append_dict('postraiter_domaine_Tru', _helpDict, interprete_Tru._helpDict)
##### end postraiter_domaine_Tru class

class modif_bord_to_raccord_Tru(interprete_Tru): #####
  r"""
  Keyword to convert a boundary of domain_name domain of kind Bord to a boundary of 
  kind Raccord (named boundary_name).
  It is useful when using meshes with boundaries of kind Bord defined and to run a 
  coupled calculation.
  """
  # From: line 688, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('domaine', 'ref_domaine_Tru'),   #
    ('nom_bord', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'domaine': ['domain']
  }
  _optionals = set([])
  _infoMain = ['TRAD2_example', 688]
  _infoAttr = {'domaine': ['TRAD2_example', 689], 'nom_bord': ['TRAD2_example', 690]}
  _attributesList = check_append_list('modif_bord_to_raccord_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('modif_bord_to_raccord_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('modif_bord_to_raccord_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domaine':
          ('[ref_domaine] Name of domain', ''),
    'nom_bord':
          ('[chaine] Name of the boundary to transform.', ''),
  }
  _helpDict = check_append_dict('modif_bord_to_raccord_Tru', _helpDict, interprete_Tru._helpDict)
##### end modif_bord_to_raccord_Tru class

class remove_elem_bloc_Tru(objet_lecture_Tru): #####
  r"""
  not_set
  """
  # From: line 691, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('liste', 'listentier_Tru'),   #
    ('fonction', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['liste', 'fonction'])
  _infoMain = ['TRAD2_example', 691]
  _infoAttr = {'fonction': ['TRAD2_example', 693], 'liste': ['TRAD2_example', 692]}
  _attributesList = check_append_list('remove_elem_bloc_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('remove_elem_bloc_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('remove_elem_bloc_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'liste':
          ('[listentier] not_set', ''),
    'fonction':
          ('[chaine] not_set', ''),
  }
  _helpDict = check_append_dict('remove_elem_bloc_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end remove_elem_bloc_Tru class

class remove_elem_Tru(interprete_Tru): #####
  r"""
  Keyword to remove element from a VDF mesh (named domaine_name), either from an explicit 
  list of elements or from a geometric condition defined by a condition f(x,y)>0 in 
  2D and f(x,y,z)>0 in 3D.
  All the new borders generated are gathered in one boundary called : newBord (to rename 
  it, use RegroupeBord keyword.
  To split it to different boundaries, use DecoupeBord_Pour_Rayonnement keyword).
  Example of a removed zone of radius 0.2 centered at (x,y)=(0.5,0.5):
  
  Remove_elem dom { fonction $0.2*0.2-(x-0.5)^2-(y-0.5)^2>0$ }
  
  Warning : the thickness of removed zone has to be large enough to avoid singular 
  nodes as decribed below : \includepng{{removeelem.png}}{{11.234}}
  """
  # From: line 694, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('domaine', 'ref_domaine_Tru'),   #
    ('bloc', 'remove_elem_bloc_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'domaine': ['domain']
  }
  _optionals = set([])
  _infoMain = ['TRAD2_example', 694]
  _infoAttr = {'bloc': ['TRAD2_example', 696], 'domaine': ['TRAD2_example', 695]}
  _attributesList = check_append_list('remove_elem_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('remove_elem_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('remove_elem_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domaine':
          ('[ref_domaine] Name of domain', ''),
    'bloc':
          ('[remove_elem_bloc] not_set', ''),
  }
  _helpDict = check_append_dict('remove_elem_Tru', _helpDict, interprete_Tru._helpDict)
##### end remove_elem_Tru class

class regroupebord_Tru(interprete_Tru): #####
  r"""
  Keyword to build one boundary new_bord with several boundaries of the domain named 
  domaine.
  """
  # From: line 697, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('domaine', 'ref_domaine_Tru'),   #
    ('new_bord', 'chaine_Tru'),   #
    ('bords', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'domaine': ['domain']
  }
  _optionals = set([])
  _infoMain = ['TRAD2_example', 697]
  _infoAttr = {'bords': ['TRAD2_example', 700],
   'domaine': ['TRAD2_example', 698],
   'new_bord': ['TRAD2_example', 699]}
  _attributesList = check_append_list('regroupebord_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('regroupebord_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('regroupebord_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domaine':
          ('[ref_domaine] Name of domain', ''),
    'new_bord':
          ('[chaine] Name of the new boundary', ''),
    'bords':
          ('[bloc_lecture] { Bound1 Bound2 }', ''),
  }
  _helpDict = check_append_dict('regroupebord_Tru', _helpDict, interprete_Tru._helpDict)
##### end regroupebord_Tru class

class supprime_bord_Tru(interprete_Tru): #####
  r"""
  Keyword to remove boundaries (named Boundary_name1 Boundary_name2 ) of the domain 
  named domain_name.
  """
  # From: line 701, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('domaine', 'ref_domaine_Tru'),   #
    ('bords', 'list_nom_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'domaine': ['domain']
  }
  _optionals = set([])
  _infoMain = ['TRAD2_example', 701]
  _infoAttr = {'bords': ['TRAD2_example', 703], 'domaine': ['TRAD2_example', 702]}
  _attributesList = check_append_list('supprime_bord_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('supprime_bord_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('supprime_bord_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domaine':
          ('[ref_domaine] Name of domain', ''),
    'bords':
          ('[list_nom] { Boundary_name1 Boundaray_name2 }', ''),
  }
  _helpDict = check_append_dict('supprime_bord_Tru', _helpDict, interprete_Tru._helpDict)
##### end supprime_bord_Tru class

class lecture_bloc_moment_base_Tru(objet_lecture_Tru): #####
  r"""
  Auxiliary class to compute and print the moments.
  """
  # From: line 704, in file 'TRAD2_example'
  _braces = 0
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 704]
  _infoAttr = {}
  _attributesList = check_append_list('lecture_bloc_moment_base_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('lecture_bloc_moment_base_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('lecture_bloc_moment_base_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('lecture_bloc_moment_base_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end lecture_bloc_moment_base_Tru class

class calcul_Tru(lecture_bloc_moment_base_Tru): #####
  r"""
  The centre of gravity will be calculated.
  """
  # From: line 705, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 705]
  _infoAttr = {}
  _attributesList = check_append_list('calcul_Tru', _attributesList, lecture_bloc_moment_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('calcul_Tru', _attributesSynonyms, lecture_bloc_moment_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('calcul_Tru', _infoAttr, lecture_bloc_moment_base_Tru._infoAttr)
  _optionals.update(lecture_bloc_moment_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('calcul_Tru', _helpDict, lecture_bloc_moment_base_Tru._helpDict)
##### end calcul_Tru class

class centre_de_gravite_Tru(lecture_bloc_moment_base_Tru): #####
  r"""
  To specify the centre of gravity.
  """
  # From: line 706, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('point', 'un_point_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 706]
  _infoAttr = {'point': ['TRAD2_example', 707]}
  _attributesList = check_append_list('centre_de_gravite_Tru', _attributesList, lecture_bloc_moment_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('centre_de_gravite_Tru', _attributesSynonyms, lecture_bloc_moment_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('centre_de_gravite_Tru', _infoAttr, lecture_bloc_moment_base_Tru._infoAttr)
  _optionals.update(lecture_bloc_moment_base_Tru._optionals)
  _helpDict = {
    'point':
          ('[un_point] A centre of gravity.', ''),
  }
  _helpDict = check_append_dict('centre_de_gravite_Tru', _helpDict, lecture_bloc_moment_base_Tru._helpDict)
##### end centre_de_gravite_Tru class

class calculer_moments_Tru(interprete_Tru): #####
  r"""
  Calculates and prints the torque (moment of force) exerted by the fluid on each boundary 
  in output files (.out) of the domain nom_dom.
  """
  # From: line 708, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('nom_dom', 'ref_domaine_Tru'),   #
    ('mot', '<inherited>lecture_bloc_moment_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 708]
  _infoAttr = {'mot': ['TRAD2_example', 710], 'nom_dom': ['TRAD2_example', 709]}
  _attributesList = check_append_list('calculer_moments_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('calculer_moments_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('calculer_moments_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'nom_dom':
          ('[ref_domaine] Name of domain.', ''),
    'mot':
          ('[lecture_bloc_moment_base] Keyword.', ''),
  }
  _helpDict = check_append_dict('calculer_moments_Tru', _helpDict, interprete_Tru._helpDict)
##### end calculer_moments_Tru class

class imprimer_flux_Tru(interprete_Tru): #####
  r"""
  This keyword prints the flux per face at the specified domain boundaries in the data 
  set.
  The fluxes are written to the .face files at a frequency defined by dt_impr, the 
  evaluation printing frequency (refer to time scheme keywords).
  By default, fluxes are incorporated onto the edges before being displayed.
  """
  # From: line 711, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
    ('noms_bord', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 711]
  _infoAttr = {'domain_name': ['TRAD2_example', 712], 'noms_bord': ['TRAD2_example', 713]}
  _attributesList = check_append_list('imprimer_flux_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('imprimer_flux_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('imprimer_flux_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of the domain.', ''),
    'noms_bord':
          ('[bloc_lecture] List of boundaries, for ex: { Bord1 Bord2 }', ''),
  }
  _helpDict = check_append_dict('imprimer_flux_Tru', _helpDict, interprete_Tru._helpDict)
##### end imprimer_flux_Tru class

class imprimer_flux_sum_Tru(imprimer_flux_Tru): #####
  r"""
  This keyword prints the sum of the flux per face at the domain boundaries defined 
  by the user in the data set.
  The fluxes are written into the .out files at a frequency defined by dt_impr, the 
  evaluation printing frequency (refer to time scheme keywords).
  """
  # From: line 714, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 714]
  _infoAttr = {}
  _attributesList = check_append_list('imprimer_flux_sum_Tru', _attributesList, imprimer_flux_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('imprimer_flux_sum_Tru', _attributesSynonyms, imprimer_flux_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('imprimer_flux_sum_Tru', _infoAttr, imprimer_flux_Tru._infoAttr)
  _optionals.update(imprimer_flux_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('imprimer_flux_sum_Tru', _helpDict, imprimer_flux_Tru._helpDict)
##### end imprimer_flux_sum_Tru class

class bloc_origine_cotes_Tru(objet_lecture_Tru): #####
  r"""
  Class to create a rectangle (or a box).
  """
  # From: line 715, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('name', 'chaine_a32a8b_Tru'),   #
    ('origin', 'listf_Tru'),   #
    ('name2', 'chaine_a7556e_Tru'),   #
    ('cotes', 'listf_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'origin': ['origine']
  }
  _optionals = set([])
  _infoMain = ['TRAD2_example', 715]
  _infoAttr = {'cotes': ['TRAD2_example', 719],
   'name': ['TRAD2_example', 716],
   'name2': ['TRAD2_example', 718],
   'origin': ['TRAD2_example', 717]}
  _attributesList = check_append_list('bloc_origine_cotes_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('bloc_origine_cotes_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('bloc_origine_cotes_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'name':
          ('[chaine(into=["origine"])] Keyword to define the origin of the rectangle (or the box).', ''),
    'origin':
          ('[listf] Coordinates of the origin of the rectangle (or the box).', ''),
    'name2':
          ('[chaine(into=["cotes"])] Keyword to define the length along the axes.', ''),
    'cotes':
          ('[listf] Length along the axes.', ''),
  }
  _helpDict = check_append_dict('bloc_origine_cotes_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end bloc_origine_cotes_Tru class

class bloc_couronne_Tru(objet_lecture_Tru): #####
  r"""
  Class to create a couronne (2D).
  """
  # From: line 720, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('name', 'chaine_a32a8b_Tru'),   #
    ('origin', 'listf_Tru'),   #
    ('name3', 'chaine_1d8e3b_Tru'),   #
    ('ri', 'floattant_820d8b_Tru'),   #
    ('name4', 'chaine_08814c_Tru'),   #
    ('re', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'origin': ['origine']
  }
  _optionals = set([])
  _infoMain = ['TRAD2_example', 720]
  _infoAttr = {'name': ['TRAD2_example', 721],
   'name3': ['TRAD2_example', 723],
   'name4': ['TRAD2_example', 725],
   'origin': ['TRAD2_example', 722],
   're': ['TRAD2_example', 726],
   'ri': ['TRAD2_example', 724]}
  _attributesList = check_append_list('bloc_couronne_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('bloc_couronne_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('bloc_couronne_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'name':
          ('[chaine(into=["origine"])] Keyword to define the center of the circle.', ''),
    'origin':
          ('[listf] Center of the circle.', ''),
    'name3':
          ('[chaine(into=["ri"])] Keyword to define the interior radius.', ''),
    'ri':
          ('[floattant] Interior radius.', ''),
    'name4':
          ('[chaine(into=["re"])] Keyword to define the exterior radius.', ''),
    're':
          ('[floattant] Exterior radius.', ''),
  }
  _helpDict = check_append_dict('bloc_couronne_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end bloc_couronne_Tru class

class bloc_tube_Tru(objet_lecture_Tru): #####
  r"""
  Class to create a tube (3D).
  """
  # From: line 727, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('name', 'chaine_a32a8b_Tru'),   #
    ('origin', 'listf_Tru'),   #
    ('name2', 'chaine_56e2aa_Tru'),   #
    ('direction', 'chaine_ad85af_Tru'),   #
    ('name3', 'chaine_1d8e3b_Tru'),   #
    ('ri', 'floattant_820d8b_Tru'),   #
    ('name4', 'chaine_08814c_Tru'),   #
    ('re', 'floattant_820d8b_Tru'),   #
    ('name5', 'chaine_8c4850_Tru'),   #
    ('h', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'origin': ['origine']
  }
  _optionals = set([])
  _infoMain = ['TRAD2_example', 727]
  _infoAttr = {'direction': ['TRAD2_example', 731],
   'h': ['TRAD2_example', 737],
   'name': ['TRAD2_example', 728],
   'name2': ['TRAD2_example', 730],
   'name3': ['TRAD2_example', 732],
   'name4': ['TRAD2_example', 734],
   'name5': ['TRAD2_example', 736],
   'origin': ['TRAD2_example', 729],
   're': ['TRAD2_example', 735],
   'ri': ['TRAD2_example', 733]}
  _attributesList = check_append_list('bloc_tube_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('bloc_tube_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('bloc_tube_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'name':
          ('[chaine(into=["origine"])] Keyword to define the center of the tube.', ''),
    'origin':
          ('[listf] Center of the tube.', ''),
    'name2':
          ('[chaine(into=["dir"])] Keyword to define the direction of the main axis.', ''),
    'direction':
          ('[chaine(into=["x","y","z"])] direction of the main axis X, Y or Z', ''),
    'name3':
          ('[chaine(into=["ri"])] Keyword to define the interior radius.', ''),
    'ri':
          ('[floattant] Interior radius.', ''),
    'name4':
          ('[chaine(into=["re"])] Keyword to define the exterior radius.', ''),
    're':
          ('[floattant] Exterior radius.', ''),
    'name5':
          ('[chaine(into=["hauteur"])] Keyword to define the heigth of the tube.', ''),
    'h':
          ('[floattant] Heigth of the tube.', ''),
  }
  _helpDict = check_append_dict('bloc_tube_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end bloc_tube_Tru class

class sous_zone_Tru(ConstrainBase_Tru): #####
  r"""
  It is an object type describing a domain sub-set.
  
  A Sous_Zone (Sub-area) type object must be associated with a Domaine type object.
  The Read (Lire) interpretor is used to define the items comprising the sub-area.
  
  Caution: The Domain type object nom_domaine must have been meshed (and triangulated 
  or tetrahedralised in VEF) prior to carrying out the Associate (Associer) nom_sous_zone 
  nom_domaine instruction; this instruction must always be preceded by the read instruction.
  """
  # From: line 738, in file 'TRAD2_example'
  _braces = 1
  _synonyms = ['sous_domaine_Tru']
  _attributesList = [
    ('restriction', 'ref_sous_zone_Tru'),   #
    ('rectangle', 'bloc_origine_cotes_Tru'),   #
    ('segment', 'bloc_origine_cotes_Tru'),   #
    ('boite', 'bloc_origine_cotes_Tru'),   #
    ('liste', 'listentier_Tru'),   #
    ('fichier', 'chaine_Tru'),   #
    ('intervalle', 'deuxentiers_Tru'),   #
    ('polynomes', 'bloc_lecture_Tru'),   #
    ('couronne', 'bloc_couronne_Tru'),   #
    ('tube', 'bloc_tube_Tru'),   #
    ('fonction_sous_zone', 'chaine_Tru'),   #
    ('union', 'ref_sous_zone_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'boite': ['box'],
    'fichier': ['filename'],
    'fonction_sous_zone': ['fonction_sous_domaine'],
    'union': ['union_with']
  }
  _optionals = set(['restriction',
   'rectangle',
   'segment',
   'boite',
   'liste',
   'fichier',
   'intervalle',
   'polynomes',
   'couronne',
   'tube',
   'fonction_sous_zone',
   'union'])
  _infoMain = ['TRAD2_example', 738]
  _infoAttr = {'boite': ['TRAD2_example', 742],
   'couronne': ['TRAD2_example', 747],
   'fichier': ['TRAD2_example', 744],
   'fonction_sous_zone': ['TRAD2_example', 749],
   'intervalle': ['TRAD2_example', 745],
   'liste': ['TRAD2_example', 743],
   'polynomes': ['TRAD2_example', 746],
   'rectangle': ['TRAD2_example', 740],
   'restriction': ['TRAD2_example', 739],
   'segment': ['TRAD2_example', 741],
   'tube': ['TRAD2_example', 748],
   'union': ['TRAD2_example', 750]}
  _helpDict = {
    'restriction':
          ('[ref_sous_zone] The elements of the sub-area nom_sous_zone must be included into the \n'
           'other sub-area named nom_sous_zone2.\n'
           'This keyword should be used first in the Read keyword.',
           ''),
    'rectangle':
          ('[bloc_origine_cotes] The sub-area will include all the domain elements whose centre \n'
           'of gravity is within the Rectangle (in dimension 2).',
           ''),
    'segment':
          ('[bloc_origine_cotes] not_set', ''),
    'boite':
          ('[bloc_origine_cotes] The sub-area will include all the domain elements whose centre \n'
           'of gravity is within the Box (in dimension 3).',
           ''),
    'liste':
          ('[listentier] The sub-area will include n domain items, numbers No.\n1 No.\ni No.\nn.', ''),
    'fichier':
          ('[chaine] The sub-area is read into the file filename.', ''),
    'intervalle':
          ('[deuxentiers] The sub-area will include domain items whose number is between n1 and \n'
           'n2 (where n1<=n2).',
           ''),
    'polynomes':
          ('[bloc_lecture] A REPRENDRE', ''),
    'couronne':
          ('[bloc_couronne] In 2D case, to create a couronne.', ''),
    'tube':
          ('[bloc_tube] In 3D case, to create a tube.', ''),
    'fonction_sous_zone':
          ('[chaine] Keyword to build a sub-area with the the elements included into the area \n'
           'defined by fonction>0.',
           ''),
    'union':
          ('[ref_sous_zone] The elements of the sub-area nom_sous_zone3 will be added to the sub-area \n'
           'nom_sous_zone.\n'
           'This keyword should be used last in the Read keyword.',
           ''),
  }
##### end sous_zone_Tru class

class decouper_bord_coincident_Tru(interprete_Tru): #####
  r"""
  In case of non-coincident meshes and a paroi_contact condition, run is stopped and 
  two external files are automatically generated in VEF (connectivity_failed_boundary_name 
  and connectivity_failed_pb_name.med).
  In 2D, the keyword Decouper_bord_coincident associated to the connectivity_failed_boundary_name 
  file allows to generate a new coincident mesh.
  """
  # From: line 751, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
    ('bord', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 751]
  _infoAttr = {'bord': ['TRAD2_example', 753], 'domain_name': ['TRAD2_example', 752]}
  _attributesList = check_append_list('decouper_bord_coincident_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('decouper_bord_coincident_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('decouper_bord_coincident_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of domain.', ''),
    'bord':
          ('[chaine] connectivity_failed_boundary_name', ''),
  }
  _helpDict = check_append_dict('decouper_bord_coincident_Tru', _helpDict, interprete_Tru._helpDict)
##### end decouper_bord_coincident_Tru class

class raffiner_anisotrope_Tru(interprete_Tru): #####
  r"""
  Only for VEF discretizations, allows to cut triangle elements in 3, or tetrahedra 
  in 4 parts, by defining a new summit located at the center of the element: \includepng{{raffineranisotri.pdf}}{{4}} 
  \includepng{{raffineranisotetra.jpeg}}{{6}}
  
  Note that such a cut creates flat elements (anisotropic).
  """
  # From: line 754, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 754]
  _infoAttr = {'domain_name': ['TRAD2_example', 755]}
  _attributesList = check_append_list('raffiner_anisotrope_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('raffiner_anisotrope_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('raffiner_anisotrope_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of domain.', ''),
  }
  _helpDict = check_append_dict('raffiner_anisotrope_Tru', _helpDict, interprete_Tru._helpDict)
##### end raffiner_anisotrope_Tru class

class raffiner_isotrope_Tru(interprete_Tru): #####
  r"""
  For VDF and VEF discretizations, allows to cut triangles/quadrangles or tetrahedral/hexaedras 
  elements respectively in 4 or 8 new ones by defining new summits located at the middle 
  of edges (and center of faces and elements for quadrangles and hexaedra).
  Such a cut preserves the shape of original elements (isotropic).
  For 2D elements: \includetabfig{{raffinerisotri.pdf}}{{4}}{{raffinerisorect.pdf}}{{4}} 
  For 3D elements: \includetabfig{{raffinerisotetra.jpeg}}{{6}}{{raffinerisohexa.jpeg}}{{5}}.
  """
  # From: line 756, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['raffiner_simplexes_Tru']
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 756]
  _infoAttr = {'domain_name': ['TRAD2_example', 757]}
  _attributesList = check_append_list('raffiner_isotrope_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('raffiner_isotrope_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('raffiner_isotrope_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of domain.', ''),
  }
  _helpDict = check_append_dict('raffiner_isotrope_Tru', _helpDict, interprete_Tru._helpDict)
##### end raffiner_isotrope_Tru class

class triangulate_Tru(interprete_Tru): #####
  r"""
  To achieve a triangular mesh from a mesh comprising rectangles (2 triangles per rectangle).
  Should be used in VEF discretization.
  Principle:
  
  \includepng{{trianguler.pdf}}{{10}}
  """
  # From: line 758, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['trianguler_Tru']
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 758]
  _infoAttr = {'domain_name': ['TRAD2_example', 759]}
  _attributesList = check_append_list('triangulate_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('triangulate_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('triangulate_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of domain.', ''),
  }
  _helpDict = check_append_dict('triangulate_Tru', _helpDict, interprete_Tru._helpDict)
##### end triangulate_Tru class

class trianguler_h_Tru(triangulate_Tru): #####
  r"""
  To achieve a triangular mesh from a mesh comprising rectangles (4 triangles per rectangle).
  Should be used in VEF discretization.
  Principle:
  
  \includepng{{triangulerh.pdf}}{{10}}
  """
  # From: line 760, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 760]
  _infoAttr = {}
  _attributesList = check_append_list('trianguler_h_Tru', _attributesList, triangulate_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('trianguler_h_Tru', _attributesSynonyms, triangulate_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('trianguler_h_Tru', _infoAttr, triangulate_Tru._infoAttr)
  _optionals.update(triangulate_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('trianguler_h_Tru', _helpDict, triangulate_Tru._helpDict)
##### end trianguler_h_Tru class

class trianguler_fin_Tru(triangulate_Tru): #####
  r"""
  Trianguler_fin is the recommended option to triangulate rectangles.
  
  As an extension (subdivision) of Triangulate_h option, this one cut each initial 
  rectangle in 8 triangles (against 4, previously).
  This cutting ensures :
  
  - a correct cutting in the corners (in respect to pressure discretization PreP1B).
  
  - a better isotropy of elements than with Trianguler_h option.
  
  - a better alignment of summits (this could have a benefit effect on calculation 
  near walls since first elements in contact with it are all contained in the same constant 
  thickness, and, by this way, a 2D cartesian grid based on summits can be engendered 
  and used to realize statistical analysis in plane channel configuration for instance).
  Principle:
  
  \includepng{{triangulerfin.pdf}}{{10}}
  """
  # From: line 761, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 761]
  _infoAttr = {}
  _attributesList = check_append_list('trianguler_fin_Tru', _attributesList, triangulate_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('trianguler_fin_Tru', _attributesSynonyms, triangulate_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('trianguler_fin_Tru', _infoAttr, triangulate_Tru._infoAttr)
  _optionals.update(triangulate_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('trianguler_fin_Tru', _helpDict, triangulate_Tru._helpDict)
##### end trianguler_fin_Tru class

class tetraedriser_Tru(interprete_Tru): #####
  r"""
  To achieve a tetrahedral mesh based on a mesh comprising blocks, the Tetraedriser 
  (Tetrahedralise) interpretor is used in VEF discretization.
  Initial block is divided in 6 tetrahedra: \includepng{{tetraedriser.jpeg}}{{5}}
  """
  # From: line 762, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 762]
  _infoAttr = {'domain_name': ['TRAD2_example', 763]}
  _attributesList = check_append_list('tetraedriser_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('tetraedriser_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('tetraedriser_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of domain.', ''),
  }
  _helpDict = check_append_dict('tetraedriser_Tru', _helpDict, interprete_Tru._helpDict)
##### end tetraedriser_Tru class

class orientefacesbord_Tru(interprete_Tru): #####
  r"""
  Keyword to modify the order of the boundary vertices included in a domain, such that 
  the surface normals are outer pointing.
  """
  # From: line 764, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 764]
  _infoAttr = {'domain_name': ['TRAD2_example', 765]}
  _attributesList = check_append_list('orientefacesbord_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('orientefacesbord_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('orientefacesbord_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of domain.', ''),
  }
  _helpDict = check_append_dict('orientefacesbord_Tru', _helpDict, interprete_Tru._helpDict)
##### end orientefacesbord_Tru class

class reorienter_tetraedres_Tru(interprete_Tru): #####
  r"""
  This keyword is mandatory for front-tracking computations with the VEF discretization.
  For each tetrahedral element of the domain, it checks if it has a positive volume.
  If the volume (determinant of the three vectors) is negative, it swaps two nodes 
  to reverse the orientation of this tetrahedron.
  """
  # From: line 766, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 766]
  _infoAttr = {'domain_name': ['TRAD2_example', 767]}
  _attributesList = check_append_list('reorienter_tetraedres_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('reorienter_tetraedres_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('reorienter_tetraedres_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of domain.', ''),
  }
  _helpDict = check_append_dict('reorienter_tetraedres_Tru', _helpDict, interprete_Tru._helpDict)
##### end reorienter_tetraedres_Tru class

class reorienter_triangles_Tru(interprete_Tru): #####
  r"""
  not_set
  """
  # From: line 768, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 768]
  _infoAttr = {'domain_name': ['TRAD2_example', 769]}
  _attributesList = check_append_list('reorienter_triangles_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('reorienter_triangles_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('reorienter_triangles_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of domain.', ''),
  }
  _helpDict = check_append_dict('reorienter_triangles_Tru', _helpDict, interprete_Tru._helpDict)
##### end reorienter_triangles_Tru class

class tetraedriser_homogene_Tru(tetraedriser_Tru): #####
  r"""
  Use the Tetraedriser_homogene (Homogeneous_Tetrahedralisation) interpretor in VEF 
  discretization to mesh a block in tetrahedrals.
  Each block hexahedral is no longer divided into 6 tetrahedrals (keyword Tetraedriser 
  (Tetrahedralise)), it is now broken down into 40 tetrahedrals.
  Thus a block defined with 11 nodes in each X, Y, Z direction will contain 10*10*10*40=40,000 
  tetrahedrals.
  This also allows problems in the mesh corners with the P1NC/P1iso/P1bulle or P1/P1 
  discretization items to be avoided.
  Initial block is divided in 40 tetrahedra: \includepng{{tetraedriserhomogene.jpeg}}{{5}}
  """
  # From: line 770, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 770]
  _infoAttr = {}
  _attributesList = check_append_list('tetraedriser_homogene_Tru', _attributesList, tetraedriser_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('tetraedriser_homogene_Tru', _attributesSynonyms, tetraedriser_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('tetraedriser_homogene_Tru', _infoAttr, tetraedriser_Tru._infoAttr)
  _optionals.update(tetraedriser_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('tetraedriser_homogene_Tru', _helpDict, tetraedriser_Tru._helpDict)
##### end tetraedriser_homogene_Tru class

class tetraedriser_homogene_fin_Tru(tetraedriser_Tru): #####
  r"""
  Tetraedriser_homogene_fin is the recommended option to tetrahedralise blocks.
  As an extension (subdivision) of Tetraedriser_homogene_compact, this last one cut 
  each initial block in 48 tetrahedra (against 24, previously).
  This cutting ensures :
  
  - a correct cutting in the corners (in respect to pressure discretization PreP1B),
  
  - a better isotropy of elements than with Tetraedriser_homogene_compact,
  
  - a better alignment of summits (this could have a benefit effect on calculation 
  near walls since first elements in contact with it are all contained in the same constant 
  thickness and ii/ by the way, a 3D cartesian grid based on summits can be engendered 
  and used to realise spectral analysis in HIT for instance).
  Initial block is divided in 48 tetrahedra: \includepng{{tetraedriserhomogenefin.jpeg}}{{5}}
  """
  # From: line 771, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 771]
  _infoAttr = {}
  _attributesList = check_append_list('tetraedriser_homogene_fin_Tru', _attributesList, tetraedriser_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('tetraedriser_homogene_fin_Tru', _attributesSynonyms, tetraedriser_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('tetraedriser_homogene_fin_Tru', _infoAttr, tetraedriser_Tru._infoAttr)
  _optionals.update(tetraedriser_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('tetraedriser_homogene_fin_Tru', _helpDict, tetraedriser_Tru._helpDict)
##### end tetraedriser_homogene_fin_Tru class

class tetraedriser_homogene_compact_Tru(tetraedriser_Tru): #####
  r"""
  This new discretization generates tetrahedral elements from cartesian or non-cartesian 
  hexahedral elements.
  The process cut each hexahedral in 6 pyramids, each of them being cut then in 4 tetrahedral.
  So, in comparison with tetra_homogene, less elements (*24 instead of*40) with more 
  homogeneous volumes are generated.
  Moreover, this process is done in a faster way.
  Initial block is divided in 24 tetrahedra: \includepng{{tetraedriserhomogenecompact.jpeg}}{{5}}
  """
  # From: line 772, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 772]
  _infoAttr = {}
  _attributesList = check_append_list('tetraedriser_homogene_compact_Tru', _attributesList, tetraedriser_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('tetraedriser_homogene_compact_Tru', _attributesSynonyms, tetraedriser_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('tetraedriser_homogene_compact_Tru', _infoAttr, tetraedriser_Tru._infoAttr)
  _optionals.update(tetraedriser_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('tetraedriser_homogene_compact_Tru', _helpDict, tetraedriser_Tru._helpDict)
##### end tetraedriser_homogene_compact_Tru class

class tetraedriser_par_prisme_Tru(tetraedriser_Tru): #####
  r"""
  Tetraedriser_par_prisme generates 6 iso-volume tetrahedral element from primary hexahedral 
  one (contrarily to the 5 elements ordinarily generated by tetraedriser).
  This element is suitable for calculation of gradients at the summit (coincident with 
  the gravity centre of the jointed elements related with) and spectra (due to a better 
  alignment of the points).
  \includetabfig{{tetraedriserparprisme.jpeg}}{{5}}{{tetraedriserparprisme2.jpeg}}{{5}} 
  Initial block is divided in 6 prismes.
  """
  # From: line 773, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 773]
  _infoAttr = {}
  _attributesList = check_append_list('tetraedriser_par_prisme_Tru', _attributesList, tetraedriser_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('tetraedriser_par_prisme_Tru', _attributesSynonyms, tetraedriser_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('tetraedriser_par_prisme_Tru', _infoAttr, tetraedriser_Tru._infoAttr)
  _optionals.update(tetraedriser_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('tetraedriser_par_prisme_Tru', _helpDict, tetraedriser_Tru._helpDict)
##### end tetraedriser_par_prisme_Tru class

class paroi_flux_impose_Tru(condlim_base_Tru): #####
  r"""
  Normal flux condition at the wall called bord (edge).
  The surface area of the flux (W.m-1 in 2D or W.m-2 in 3D) is imposed at the boundary 
  according to the following convention: a positive flux is a flux that enters into 
  the domain according to convention.
  """
  # From: line 774, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('ch', '<inherited>front_field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 774]
  _infoAttr = {'ch': ['TRAD2_example', 775]}
  _attributesList = check_append_list('paroi_flux_impose_Tru', _attributesList, condlim_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('paroi_flux_impose_Tru', _attributesSynonyms, condlim_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('paroi_flux_impose_Tru', _infoAttr, condlim_base_Tru._infoAttr)
  _optionals.update(condlim_base_Tru._optionals)
  _helpDict = {
    'ch':
          ('[front_field_base] Boundary field type.', ''),
  }
  _helpDict = check_append_dict('paroi_flux_impose_Tru', _helpDict, condlim_base_Tru._helpDict)
##### end paroi_flux_impose_Tru class

class paroi_temperature_imposee_Tru(dirichlet_Tru): #####
  r"""
  Imposed temperature condition at the wall called bord (edge).
  """
  # From: line 776, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('ch', '<inherited>front_field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 776]
  _infoAttr = {'ch': ['TRAD2_example', 777]}
  _attributesList = check_append_list('paroi_temperature_imposee_Tru', _attributesList, dirichlet_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('paroi_temperature_imposee_Tru', _attributesSynonyms, dirichlet_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('paroi_temperature_imposee_Tru', _infoAttr, dirichlet_Tru._infoAttr)
  _optionals.update(dirichlet_Tru._optionals)
  _helpDict = {
    'ch':
          ('[front_field_base] Boundary field type.', ''),
  }
  _helpDict = check_append_dict('paroi_temperature_imposee_Tru', _helpDict, dirichlet_Tru._helpDict)
##### end paroi_temperature_imposee_Tru class

class temperature_imposee_paroi_Tru(paroi_temperature_imposee_Tru): #####
  r"""
  Imposed temperature condition at the wall called bord (edge).
  """
  # From: line 778, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 778]
  _infoAttr = {}
  _attributesList = check_append_list('temperature_imposee_paroi_Tru', _attributesList, paroi_temperature_imposee_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('temperature_imposee_paroi_Tru', _attributesSynonyms, paroi_temperature_imposee_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('temperature_imposee_paroi_Tru', _infoAttr, paroi_temperature_imposee_Tru._infoAttr)
  _optionals.update(paroi_temperature_imposee_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('temperature_imposee_paroi_Tru', _helpDict, paroi_temperature_imposee_Tru._helpDict)
##### end temperature_imposee_paroi_Tru class

class champ_front_contact_vef_Tru(front_field_base_Tru): #####
  r"""
  This field is used on a boundary between a solid and fluid domain to exchange a calculated 
  temperature at the contact face of the two domains according to the flux of the two 
  problems.
  """
  # From: line 779, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('local_pb', 'ref_pb_base_Tru'),   #
    ('local_boundary', 'chaine_Tru'),   #
    ('remote_pb', 'ref_pb_base_Tru'),   #
    ('remote_boundary', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 779]
  _infoAttr = {'local_boundary': ['TRAD2_example', 781],
   'local_pb': ['TRAD2_example', 780],
   'remote_boundary': ['TRAD2_example', 783],
   'remote_pb': ['TRAD2_example', 782]}
  _attributesList = check_append_list('champ_front_contact_vef_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_contact_vef_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_contact_vef_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'local_pb':
          ('[ref_pb_base] Name of the problem.', ''),
    'local_boundary':
          ('[chaine] Name of the boundary.', ''),
    'remote_pb':
          ('[ref_pb_base] Name of the second problem.', ''),
    'remote_boundary':
          ('[chaine] Name of the boundary in the second problem.', ''),
  }
  _helpDict = check_append_dict('champ_front_contact_vef_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_contact_vef_Tru class

class bloc_lecture_poro_Tru(objet_lecture_Tru): #####
  r"""
  Surface and volume porosity values.
  """
  # From: line 784, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('volumique', 'floattant_820d8b_Tru'),   #
    ('surfacique', 'list_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 784]
  _infoAttr = {'surfacique': ['TRAD2_example', 786], 'volumique': ['TRAD2_example', 785]}
  _attributesList = check_append_list('bloc_lecture_poro_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('bloc_lecture_poro_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('bloc_lecture_poro_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'volumique':
          ('[floattant] Volume porosity value.', ''),
    'surfacique':
          ('[list] Surface porosity values (in X, Y, Z directions).', ''),
  }
  _helpDict = check_append_dict('bloc_lecture_poro_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end bloc_lecture_poro_Tru class

class porosites_Tru(ConstrainBase_Tru): #####
  r"""
  To define the volume porosity and surface porosity that are uniform in every direction 
  in space on a sub-area.
  
  Porosity was only usable in VDF discretization, and now available for VEF P1NC/P0.
  
  Observations :
  
  - Surface porosity values must be given in every direction in space (set this value 
  to 1 if there is no porosity),
  
  - Prior to defining porosity, the problem must have been discretized.NL2 Can 't be 
  used in VEF discretization, use Porosites_champ instead.
  """
  # From: line 787, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('aco', 'chaine_e3a651_Tru'),   #
    ('sous_zone', 'chaine_Tru'),   #
    ('bloc', 'bloc_lecture_poro_Tru'),   #
    ('sous_zone2', 'chaine_Tru'),   #
    ('bloc2', 'bloc_lecture_poro_Tru'),   #
    ('acof', 'chaine_af6447_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'sous_zone': ['sous_zone1']
  }
  _optionals = set(['sous_zone2', 'bloc2'])
  _infoMain = ['TRAD2_example', 787]
  _infoAttr = {'aco': ['TRAD2_example', 788],
   'acof': ['TRAD2_example', 793],
   'bloc': ['TRAD2_example', 790],
   'bloc2': ['TRAD2_example', 792],
   'sous_zone': ['TRAD2_example', 789],
   'sous_zone2': ['TRAD2_example', 791]}
  _helpDict = {
    'aco':
          ('[chaine(into=["{"])] Opening curly bracket.', ''),
    'sous_zone':
          ('[chaine] Name of the sub-area to which porosity are allocated.', ''),
    'bloc':
          ('[bloc_lecture_poro] Surface and volume porosity values.', ''),
    'sous_zone2':
          ('[chaine] Name of the 2nd sub-area to which porosity are allocated.', ''),
    'bloc2':
          ('[bloc_lecture_poro] Surface and volume porosity values.', ''),
    'acof':
          ('[chaine(into=["}"])] Closing curly bracket.', ''),
  }
##### end porosites_Tru class

class discretiser_domaine_Tru(interprete_Tru): #####
  r"""
  Useful to discretize the domain domain_name (faces will be created) without defining 
  a problem.
  """
  # From: line 794, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 794]
  _infoAttr = {'domain_name': ['TRAD2_example', 795]}
  _attributesList = check_append_list('discretiser_domaine_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('discretiser_domaine_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('discretiser_domaine_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of the domain.', ''),
  }
  _helpDict = check_append_dict('discretiser_domaine_Tru', _helpDict, interprete_Tru._helpDict)
##### end discretiser_domaine_Tru class

class champ_front_calc_Tru(front_field_base_Tru): #####
  r"""
  This keyword is used on a boundary to get a field from another boundary.
  The local and remote boundaries should have the same mesh.
  If not, the Champ_front_recyclage keyword could be used instead.
  It is used in the condition block at the limits of equation which itself refers to 
  a problem called pb1.
  We are working under the supposition that pb1 is coupled to another problem.
  """
  # From: line 796, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('problem_name', 'ref_pb_base_Tru'),   #
    ('bord', 'chaine_Tru'),   #
    ('field_name', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 796]
  _infoAttr = {'bord': ['TRAD2_example', 798],
   'field_name': ['TRAD2_example', 799],
   'problem_name': ['TRAD2_example', 797]}
  _attributesList = check_append_list('champ_front_calc_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_calc_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_calc_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'problem_name':
          ('[ref_pb_base] Name of the other problem to which pb1 is coupled.', ''),
    'bord':
          ('[chaine] Name of the side which is the boundary between the 2 domains in the domain \n'
           'object description associated with the problem_name object.',
           ''),
    'field_name':
          ('[chaine] Name of the field containing the value that the user wishes to use at the \n'
           'boundary.\n'
           'The field_name object must be recognized by the problem_name object.',
           ''),
  }
  _helpDict = check_append_dict('champ_front_calc_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_calc_Tru class

class champ_tabule_temps_Tru(champ_don_base_Tru): #####
  r"""
  Field that is constant in space and tabulated as a function of time.
  """
  # From: line 800, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('dim', 'entier_3282e0_Tru'),   #
    ('bloc', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 800]
  _infoAttr = {'bloc': ['TRAD2_example', 802], 'dim': ['TRAD2_example', 801]}
  _attributesList = check_append_list('champ_tabule_temps_Tru', _attributesList, champ_don_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_tabule_temps_Tru', _attributesSynonyms, champ_don_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_tabule_temps_Tru', _infoAttr, champ_don_base_Tru._infoAttr)
  _optionals.update(champ_don_base_Tru._optionals)
  _helpDict = {
    'dim':
          ('[entier] Number of field components.', ''),
    'bloc':
          ('[bloc_lecture] Values as a table.\n'
           'The value of the field at any time is calculated by linear interpolation from this \n'
           'table.',
           ''),
  }
  _helpDict = check_append_dict('champ_tabule_temps_Tru', _helpDict, champ_don_base_Tru._helpDict)
##### end champ_tabule_temps_Tru class

class champ_fonc_t_Tru(champ_don_base_Tru): #####
  r"""
  Field that is constant in space and is a function of time.
  """
  # From: line 803, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('val', 'listchaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 803]
  _infoAttr = {'val': ['TRAD2_example', 804]}
  _attributesList = check_append_list('champ_fonc_t_Tru', _attributesList, champ_don_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_fonc_t_Tru', _attributesSynonyms, champ_don_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_fonc_t_Tru', _infoAttr, champ_don_base_Tru._infoAttr)
  _optionals.update(champ_don_base_Tru._optionals)
  _helpDict = {
    'val':
          ('[listchaine] Values of field components (time dependant functions).', ''),
  }
  _helpDict = check_append_dict('champ_fonc_t_Tru', _helpDict, champ_don_base_Tru._helpDict)
##### end champ_fonc_t_Tru class

class champ_fonc_tabule_Tru(champ_don_base_Tru): #####
  r"""
  Field that is tabulated as a function of another field.
  """
  # From: line 1297, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('inco', 'chaine_Tru'),   #
    ('dim', 'entier_3282e0_Tru'),   #
    ('bloc', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1297]
  _infoAttr = {'bloc': ['TRAD2_example', 1300],
   'dim': ['TRAD2_example', 1299],
   'inco': ['TRAD2_example', 1298]}
  _attributesList = check_append_list('champ_fonc_tabule_Tru', _attributesList, champ_don_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_fonc_tabule_Tru', _attributesSynonyms, champ_don_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_fonc_tabule_Tru', _infoAttr, champ_don_base_Tru._infoAttr)
  _optionals.update(champ_don_base_Tru._optionals)
  _helpDict = {
    'inco':
          ('[chaine] Name of the field (for example: temperature).', ''),
    'dim':
          ('[entier] Number of field components.', ''),
    'bloc':
          ('[bloc_lecture] Values (the table (the value of the field at any time is calculated \n'
           'by linear interpolation from this table) or the analytical expression (with keyword \n'
           'expression to use an analytical expression)).',
           ''),
  }
  _helpDict = check_append_dict('champ_fonc_tabule_Tru', _helpDict, champ_don_base_Tru._helpDict)
##### end champ_fonc_tabule_Tru class

class champ_fonc_fonction_Tru(champ_fonc_tabule_Tru): #####
  r"""
  Field that is a function of another field.
  """
  # From: line 805, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('problem_name', 'ref_pb_base_Tru'),   #
    ('inco', 'chaine_Tru'),   #
    ('expression', 'listchaine_Tru'),   #
  ]
  _suppressed = ['dim', 'bloc', 'inco']
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 805]
  _infoAttr = {'expression': ['TRAD2_example', 811],
   'inco': ['TRAD2_example', 810],
   'problem_name': ['TRAD2_example', 809]}
  _attributesList = check_append_list('champ_fonc_fonction_Tru', _attributesList, champ_fonc_tabule_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_fonc_fonction_Tru', _attributesSynonyms, champ_fonc_tabule_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_fonc_fonction_Tru', _infoAttr, champ_fonc_tabule_Tru._infoAttr)
  _optionals.update(champ_fonc_tabule_Tru._optionals)
  _helpDict = {
    'problem_name':
          ('[ref_pb_base] Name of problem.', ''),
    'inco':
          ('[chaine] Name of the field (for example: temperature).', ''),
    'expression':
          ('[listchaine] Number of field components followed by the analytical expression for \n'
           'each field component.',
           ''),
  }
  _helpDict = check_append_dict('champ_fonc_fonction_Tru', _helpDict, champ_fonc_tabule_Tru._helpDict)
##### end champ_fonc_fonction_Tru class

class frontiere_ouverte_gradient_pression_impose_vefprep1b_Tru(frontiere_ouverte_gradient_pression_impose_Tru): #####
  r"""
  Keyword for an outlet boundary condition in VEF P1B/P1NC on the gradient of the pressure.
  """
  # From: line 812, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 812]
  _infoAttr = {}
  _attributesList = check_append_list('frontiere_ouverte_gradient_pression_impose_vefprep1b_Tru', _attributesList, frontiere_ouverte_gradient_pression_impose_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('frontiere_ouverte_gradient_pression_impose_vefprep1b_Tru', _attributesSynonyms, frontiere_ouverte_gradient_pression_impose_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('frontiere_ouverte_gradient_pression_impose_vefprep1b_Tru', _infoAttr, frontiere_ouverte_gradient_pression_impose_Tru._infoAttr)
  _optionals.update(frontiere_ouverte_gradient_pression_impose_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('frontiere_ouverte_gradient_pression_impose_vefprep1b_Tru', _helpDict, frontiere_ouverte_gradient_pression_impose_Tru._helpDict)
##### end frontiere_ouverte_gradient_pression_impose_vefprep1b_Tru class

class bloc_lec_champ_init_canal_sinal_Tru(objet_lecture_Tru): #####
  r"""
  Parameters for the class champ_init_canal_sinal.
  
  in 2D:
  
  U=ucent*y(2h-y)/h/h
  
  V=ampli_bruit*rand+ampli_sin*sin(omega*x)
  
  rand: unpredictable value between -1 and 1.
  
  in 3D:
  
  U=ucent*y(2h-y)/h/h
  
  V=ampli_bruit*rand1+ampli_sin*sin(omega*x)
  
  W=ampli_bruit*rand2
  
  rand1 and rand2: unpredictables values between -1 and 1.
  """
  # From: line 813, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('ucent', 'floattant_820d8b_Tru'),   #
    ('h', 'floattant_820d8b_Tru'),   #
    ('ampli_bruit', 'floattant_820d8b_Tru'),   #
    ('ampli_sin', 'floattant_820d8b_Tru'),   #
    ('omega', 'floattant_820d8b_Tru'),   #
    ('dir_flow', 'entier_6f2a33_Tru'),   #
    ('dir_wall', 'entier_6f2a33_Tru'),   #
    ('min_dir_flow', 'floattant_820d8b_Tru'),   #
    ('min_dir_wall', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['ampli_sin', 'dir_flow', 'dir_wall', 'min_dir_flow', 'min_dir_wall'])
  _infoMain = ['TRAD2_example', 813]
  _infoAttr = {'ampli_bruit': ['TRAD2_example', 816],
   'ampli_sin': ['TRAD2_example', 817],
   'dir_flow': ['TRAD2_example', 819],
   'dir_wall': ['TRAD2_example', 820],
   'h': ['TRAD2_example', 815],
   'min_dir_flow': ['TRAD2_example', 821],
   'min_dir_wall': ['TRAD2_example', 822],
   'omega': ['TRAD2_example', 818],
   'ucent': ['TRAD2_example', 814]}
  _attributesList = check_append_list('bloc_lec_champ_init_canal_sinal_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('bloc_lec_champ_init_canal_sinal_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('bloc_lec_champ_init_canal_sinal_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'ucent':
          ('[floattant] Velocity value at the center of the channel.', ''),
    'h':
          ('[floattant] Half hength of the channel.', ''),
    'ampli_bruit':
          ('[floattant] Amplitude for the disturbance.', ''),
    'ampli_sin':
          ('[floattant] Amplitude for the sinusoidal disturbance (by default equals to ucent/10).', ''),
    'omega':
          ('[floattant] Value of pulsation for the of the sinusoidal disturbance.', ''),
    'dir_flow':
          ('[entier(into=[0,1,2])] Flow direction for the initialization of the flow in a channel.\n'
           '\n'
           '- if dir_flow=0, the flow direction is X\n'
           '\n'
           '- if dir_flow=1, the flow direction is Y\n'
           '\n'
           '- if dir_flow=2, the flow direction is Z\n'
           '\n'
           'Default value for dir_flow is 0',
           ''),
    'dir_wall':
          ('[entier(into=[0,1,2])] Wall direction for the initialization of the flow in a channel.\n'
           '\n'
           '- if dir_wall=0, the normal to the wall is in X direction\n'
           '\n'
           '- if dir_wall=1, the normal to the wall is in Y direction\n'
           '\n'
           '- if dir_wall=2, the normal to the wall is in Z direction\n'
           '\n'
           'Default value for dir_flow is 1',
           ''),
    'min_dir_flow':
          ('[floattant] Value of the minimum coordinate in the flow direction for the initialization \n'
           'of the flow in a channel.\n'
           'Default value for dir_flow is 0.',
           ''),
    'min_dir_wall':
          ('[floattant] Value of the minimum coordinate in the wall direction for the initialization \n'
           'of the flow in a channel.\n'
           'Default value for dir_flow is 0.',
           ''),
  }
  _helpDict = check_append_dict('bloc_lec_champ_init_canal_sinal_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end bloc_lec_champ_init_canal_sinal_Tru class

class champ_init_canal_sinal_Tru(champ_don_base_Tru): #####
  r"""
  For a parabolic profile on U velocity with an unpredictable disturbance on V and W 
  and a sinusoidal disturbance on V velocity.
  """
  # From: line 823, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('dim', 'entier_3282e0_Tru'),   #
    ('bloc', 'bloc_lec_champ_init_canal_sinal_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 823]
  _infoAttr = {'bloc': ['TRAD2_example', 825], 'dim': ['TRAD2_example', 824]}
  _attributesList = check_append_list('champ_init_canal_sinal_Tru', _attributesList, champ_don_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_init_canal_sinal_Tru', _attributesSynonyms, champ_don_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_init_canal_sinal_Tru', _infoAttr, champ_don_base_Tru._infoAttr)
  _optionals.update(champ_don_base_Tru._optionals)
  _helpDict = {
    'dim':
          ('[entier] Number of field components.', ''),
    'bloc':
          ('[bloc_lec_champ_init_canal_sinal] Parameters for the class champ_init_canal_sinal.', ''),
  }
  _helpDict = check_append_dict('champ_init_canal_sinal_Tru', _helpDict, champ_don_base_Tru._helpDict)
##### end champ_init_canal_sinal_Tru class

class champ_som_lu_vdf_Tru(champ_don_base_Tru): #####
  r"""
  Keyword to read in a file values located at the nodes of a mesh in VDF discretization.
  """
  # From: line 826, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
    ('dim', 'entier_3282e0_Tru'),   #
    ('tolerance', 'floattant_820d8b_Tru'),   #
    ('file', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 826]
  _infoAttr = {'dim': ['TRAD2_example', 828],
   'domain_name': ['TRAD2_example', 827],
   'file': ['TRAD2_example', 830],
   'tolerance': ['TRAD2_example', 829]}
  _attributesList = check_append_list('champ_som_lu_vdf_Tru', _attributesList, champ_don_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_som_lu_vdf_Tru', _attributesSynonyms, champ_don_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_som_lu_vdf_Tru', _infoAttr, champ_don_base_Tru._infoAttr)
  _optionals.update(champ_don_base_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of the domain.', ''),
    'dim':
          ('[entier] Value of the dimension of the field.', ''),
    'tolerance':
          ('[floattant] Value of the tolerance to check the coordinates of the nodes.', ''),
    'file':
          ('[chaine] name of the file\n'
           '\n'
           'This file has the following format:\n'
           '\n'
           'Xi Yi Zi -> Coordinates of the node\n'
           '\n'
           'Ui Vi Wi -> Value of the field on this node\n'
           '\n'
           'Xi+1 Yi+1 Zi+1 -> Next point\n'
           '\n'
           'Ui+1 Vi+1 Zi+1 -> Next value ...',
           ''),
  }
  _helpDict = check_append_dict('champ_som_lu_vdf_Tru', _helpDict, champ_don_base_Tru._helpDict)
##### end champ_som_lu_vdf_Tru class

class champ_som_lu_vef_Tru(champ_don_base_Tru): #####
  r"""
  Keyword to read in a file values located at the nodes of a mesh in VEF discretization.
  """
  # From: line 831, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
    ('dim', 'entier_3282e0_Tru'),   #
    ('tolerance', 'floattant_820d8b_Tru'),   #
    ('file', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 831]
  _infoAttr = {'dim': ['TRAD2_example', 833],
   'domain_name': ['TRAD2_example', 832],
   'file': ['TRAD2_example', 835],
   'tolerance': ['TRAD2_example', 834]}
  _attributesList = check_append_list('champ_som_lu_vef_Tru', _attributesList, champ_don_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_som_lu_vef_Tru', _attributesSynonyms, champ_don_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_som_lu_vef_Tru', _infoAttr, champ_don_base_Tru._infoAttr)
  _optionals.update(champ_don_base_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of the domain.', ''),
    'dim':
          ('[entier] Value of the dimension of the field.', ''),
    'tolerance':
          ('[floattant] Value of the tolerance to check the coordinates of the nodes.', ''),
    'file':
          ('[chaine] Name of the file.\n'
           '\n'
           'This file has the following format:\n'
           '\n'
           'Xi Yi Zi -> Coordinates of the node\n'
           '\n'
           'Ui Vi Wi -> Value of the field on this node\n'
           '\n'
           'Xi+1 Yi+1 Zi+1 -> Next point\n'
           '\n'
           'Ui+1 Vi+1 Zi+1 -> Next value ...',
           ''),
  }
  _helpDict = check_append_dict('champ_som_lu_vef_Tru', _helpDict, champ_don_base_Tru._helpDict)
##### end champ_som_lu_vef_Tru class

class acceleration_Tru(source_base_Tru): #####
  r"""
  Momentum source term to take in account the forces due to rotation or translation 
  of a non Galilean referential R' (centre 0') into the Galilean referential R (centre 
  0).
  """
  # From: line 836, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('vitesse', '<inherited>field_base_Tru'),   #
    ('acceleration', '<inherited>field_base_Tru'),   #
    ('omega', '<inherited>field_base_Tru'),   #
    ('domegadt', '<inherited>field_base_Tru'),   #
    ('centre_rotation', '<inherited>field_base_Tru'),   #
    ('option', 'chaine_1ca9d5_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['vitesse', 'acceleration', 'omega', 'domegadt', 'centre_rotation', 'option'])
  _infoMain = ['TRAD2_example', 836]
  _infoAttr = {'acceleration': ['TRAD2_example', 838],
   'centre_rotation': ['TRAD2_example', 841],
   'domegadt': ['TRAD2_example', 840],
   'omega': ['TRAD2_example', 839],
   'option': ['TRAD2_example', 842],
   'vitesse': ['TRAD2_example', 837]}
  _attributesList = check_append_list('acceleration_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('acceleration_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('acceleration_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'vitesse':
          ("[field_base] Keyword for the velocity of the referential R' into the R referential \n"
           "(dOO'/dt term [m.s-1]).\n"
           'The velocity is mandatory when you want to print the total cinetic energy into the \n'
           'non-mobile Galilean referential R (see Ec_dans_repere_fixe keyword).',
           ''),
    'acceleration':
          ("[field_base] Keyword for the acceleration of the referential R' into the R referential \n"
           "(d2OO'/dt2 term [m.s-2]).\n"
           'field_base is a time dependant field (eg: Champ_Fonc_t).',
           ''),
    'omega':
          ("[field_base] Keyword for a rotation of the referential R' into the R referential [rad.s-1].\n"
           'field_base is a 3D time dependant field specified for example by a Champ_Fonc_t keyword.\n'
           'The time_field field should have 3 components even in 2D (In 2D: 0 0 omega).',
           ''),
    'domegadt':
          ('[field_base] Keyword to define the time derivative of the previous rotation [rad.s-2].\n'
           'Should be zero if the rotation is constant.\n'
           'The time_field field should have 3 components even in 2D (In 2D: 0 0 domegadt).',
           ''),
    'centre_rotation':
          ("[field_base] Keyword to specify the centre of rotation (expressed in R' coordinates) \n"
           "of R' into R (if the domain rotates with the R' referential, the centre of rotation \n"
           "is 0'=(0,0,0)).\n"
           'The time_field should have 2 or 3 components according the dimension 2 or 3.',
           ''),
    'option':
          ('[chaine(into=["terme_complet","coriolis_seul","entrainement_seul"])] Keyword to specify \n'
           'the kind of calculation: terme_complet (default option) will calculate both the Coriolis \n'
           'and centrifugal forces, coriolis_seul will calculate the first one only, entrainement_seul \n'
           'will calculate the second one only.',
           ''),
  }
  _helpDict = check_append_dict('acceleration_Tru', _helpDict, source_base_Tru._helpDict)
##### end acceleration_Tru class

class coriolis_Tru(source_base_Tru): #####
  r"""
  Keyword for a Coriolis term in hydraulic equation.
  Warning: Only available in VDF.
  """
  # From: line 843, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('omega', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 843]
  _infoAttr = {'omega': ['TRAD2_example', 844]}
  _attributesList = check_append_list('coriolis_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('coriolis_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('coriolis_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'omega':
          ('[chaine] Value of omega.', ''),
  }
  _helpDict = check_append_dict('coriolis_Tru', _helpDict, source_base_Tru._helpDict)
##### end coriolis_Tru class

class champ_front_recyclage_Tru(front_field_base_Tru): #####
  r"""
  \input{{champfrontrecyclage}}
  """
  # From: line 845, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('bloc', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 845]
  _infoAttr = {'bloc': ['TRAD2_example', 846]}
  _attributesList = check_append_list('champ_front_recyclage_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_recyclage_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_recyclage_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'bloc':
          ('[chaine] not_set', ''),
  }
  _helpDict = check_append_dict('champ_front_recyclage_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_recyclage_Tru class

class paroi_echange_externe_impose_h_Tru(paroi_echange_externe_impose_Tru): #####
  r"""
  Particular case of class paroi_echange_externe_impose for enthalpy equation.
  """
  # From: line 847, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 847]
  _infoAttr = {}
  _attributesList = check_append_list('paroi_echange_externe_impose_h_Tru', _attributesList, paroi_echange_externe_impose_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('paroi_echange_externe_impose_h_Tru', _attributesSynonyms, paroi_echange_externe_impose_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('paroi_echange_externe_impose_h_Tru', _infoAttr, paroi_echange_externe_impose_Tru._infoAttr)
  _optionals.update(paroi_echange_externe_impose_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('paroi_echange_externe_impose_h_Tru', _helpDict, paroi_echange_externe_impose_Tru._helpDict)
##### end paroi_echange_externe_impose_h_Tru class

class entree_temperature_imposee_h_Tru(frontiere_ouverte_temperature_imposee_Tru): #####
  r"""
  Particular case of class frontiere_ouverte_temperature_imposee for enthalpy equation.
  """
  # From: line 848, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 848]
  _infoAttr = {}
  _attributesList = check_append_list('entree_temperature_imposee_h_Tru', _attributesList, frontiere_ouverte_temperature_imposee_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('entree_temperature_imposee_h_Tru', _attributesSynonyms, frontiere_ouverte_temperature_imposee_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('entree_temperature_imposee_h_Tru', _infoAttr, frontiere_ouverte_temperature_imposee_Tru._infoAttr)
  _optionals.update(frontiere_ouverte_temperature_imposee_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('entree_temperature_imposee_h_Tru', _helpDict, frontiere_ouverte_temperature_imposee_Tru._helpDict)
##### end entree_temperature_imposee_h_Tru class

class sortie_libre_temperature_imposee_h_Tru(neumann_Tru): #####
  r"""
  Open boundary for heat equation with enthalpy as unknown.
  """
  # From: line 849, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('ch', '<inherited>front_field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 849]
  _infoAttr = {'ch': ['TRAD2_example', 850]}
  _attributesList = check_append_list('sortie_libre_temperature_imposee_h_Tru', _attributesList, neumann_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('sortie_libre_temperature_imposee_h_Tru', _attributesSynonyms, neumann_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('sortie_libre_temperature_imposee_h_Tru', _infoAttr, neumann_Tru._infoAttr)
  _optionals.update(neumann_Tru._optionals)
  _helpDict = {
    'ch':
          ('[front_field_base] Boundary field type.', ''),
  }
  _helpDict = check_append_dict('sortie_libre_temperature_imposee_h_Tru', _helpDict, neumann_Tru._helpDict)
##### end sortie_libre_temperature_imposee_h_Tru class

class champ_ostwald_Tru(field_base_Tru): #####
  r"""
  This keyword is used to define the viscosity variation law:
  
  Mu(T)= K(T)*(D:D/2)**((n-1)/2)
  """
  # From: line 851, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 851]
  _infoAttr = {}
  _attributesList = check_append_list('champ_ostwald_Tru', _attributesList, field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_ostwald_Tru', _attributesSynonyms, field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_ostwald_Tru', _infoAttr, field_base_Tru._infoAttr)
  _optionals.update(field_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('champ_ostwald_Tru', _helpDict, field_base_Tru._helpDict)
##### end champ_ostwald_Tru class

class tayl_green_Tru(champ_don_base_Tru): #####
  r"""
  Class Tayl_green.
  """
  # From: line 852, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('dim', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 852]
  _infoAttr = {'dim': ['TRAD2_example', 853]}
  _attributesList = check_append_list('tayl_green_Tru', _attributesList, champ_don_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('tayl_green_Tru', _attributesSynonyms, champ_don_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('tayl_green_Tru', _infoAttr, champ_don_base_Tru._infoAttr)
  _optionals.update(champ_don_base_Tru._optionals)
  _helpDict = {
    'dim':
          ('[entier] Dimension.', ''),
  }
  _helpDict = check_append_dict('tayl_green_Tru', _helpDict, champ_don_base_Tru._helpDict)
##### end tayl_green_Tru class

class fonction_champ_reprise_Tru(objet_lecture_Tru): #####
  r"""
  not_set
  """
  # From: line 854, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('mot', 'chaine_d74614_Tru'),   #
    ('fonction', 'listchaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 854]
  _infoAttr = {'fonction': ['TRAD2_example', 856], 'mot': ['TRAD2_example', 855]}
  _attributesList = check_append_list('fonction_champ_reprise_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('fonction_champ_reprise_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('fonction_champ_reprise_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'mot':
          ('[chaine(into=["fonction"])] not_set', ''),
    'fonction':
          ('[listchaine] n f1(val) f2(val) ...\nfn(val)] time', ''),
  }
  _helpDict = check_append_dict('fonction_champ_reprise_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end fonction_champ_reprise_Tru class

class source_qdm_Tru(source_base_Tru): #####
  r"""
  Momentum source term in the Navier-Stokes equations.
  """
  # From: line 857, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('ch', '<inherited>field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'ch': ['champ']
  }
  _optionals = set([])
  _infoMain = ['TRAD2_example', 857]
  _infoAttr = {'ch': ['TRAD2_example', 858]}
  _attributesList = check_append_list('source_qdm_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('source_qdm_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('source_qdm_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'ch':
          ('[field_base] Field type.', ''),
  }
  _helpDict = check_append_dict('source_qdm_Tru', _helpDict, source_base_Tru._helpDict)
##### end source_qdm_Tru class

class perte_charge_singuliere_Tru(source_base_Tru): #####
  r"""
  Source term that is used to model a pressure loss over a surface area (transition 
  through a grid, sudden enlargement) defined by the faces of elements located on the 
  intersection of a subzone named subzone_name and a X,Y, or Z plane located at X,Y 
  or Z = location.
  """
  # From: line 859, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('dir', 'chaine_13672b_Tru'),   #
    ('coeff', 'floattant_820d8b_Tru'),   #
    ('regul', 'bloc_lecture_Tru'),   #
    ('surface', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['coeff', 'regul'])
  _infoMain = ['TRAD2_example', 859]
  _infoAttr = {'coeff': ['TRAD2_example', 861],
   'dir': ['TRAD2_example', 860],
   'regul': ['TRAD2_example', 862],
   'surface': ['TRAD2_example', 863]}
  _attributesList = check_append_list('perte_charge_singuliere_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('perte_charge_singuliere_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('perte_charge_singuliere_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'dir':
          ('[chaine(into=["kx","ky","kz","k"])] KX, KY or KZ designate directional pressure loss \n'
           'coefficients for respectively X, Y or Z direction.\n'
           'Or in the case where you chose a target flow rate with regul.\n'
           'Use K for isotropic pressure loss coefficient',
           ''),
    'coeff':
          ('[floattant] Value (float) of friction coefficient (KX, KY, KZ).', ''),
    'regul':
          ('[bloc_lecture] option to have adjustable K with flowrate target\n'
           '\n'
           '{ K0 valeur_initiale_de_k deb debit_cible eps intervalle_variation_mutiplicatif}.',
           ''),
    'surface':
          ('[bloc_lecture] Three syntaxes are possible for the surface definition block:\n'
           '\n'
           'For VDF and VEF: { X|Y|Z = location subzone_name }\n'
           '\n'
           'Only for VEF: { Surface surface_name }.\n'
           '\n'
           'For polymac { Surface surface_name Orientation champ_uniforme }',
           ''),
  }
  _helpDict = check_append_dict('perte_charge_singuliere_Tru', _helpDict, source_base_Tru._helpDict)
##### end perte_charge_singuliere_Tru class

class dp_impose_Tru(source_base_Tru): #####
  r"""
  Source term to impose a pressure difference according to the formula : DP = A + B 
  * (Q - Q0)
  """
  # From: line 864, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('dp', '<inherited>field_base_Tru'),   #
    ('surface', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 864]
  _infoAttr = {'dp': ['TRAD2_example', 865], 'surface': ['TRAD2_example', 866]}
  _attributesList = check_append_list('dp_impose_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('dp_impose_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('dp_impose_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'dp':
          ('[field_base] the parameters of the previous formula champ_uniforme 3 A B Q0 where \n'
           'Q0 is a volume flow (m3/s).',
           ''),
    'surface':
          ('[bloc_lecture] Three syntaxes are possible for the surface definition block:\n'
           '\n'
           'For VDF and VEF: { X|Y|Z = location subzone_name }\n'
           '\n'
           'Only for VEF: { Surface surface_name }.\n'
           '\n'
           'For polymac { Surface surface_name Orientation champ_uniforme }.',
           ''),
  }
  _helpDict = check_append_dict('dp_impose_Tru', _helpDict, source_base_Tru._helpDict)
##### end dp_impose_Tru class

class perte_charge_directionnelle_Tru(source_base_Tru): #####
  r"""
  Directional pressure loss.
  """
  # From: line 867, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('Lambda', 'chaine_Tru'),   #
    ('diam_hydr', '<inherited>champ_don_base_Tru'),   #
    ('direction', '<inherited>champ_don_base_Tru'),   #
    ('sous_zone', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'Lambda': ['lambda_u']
  }
  _optionals = set(['sous_zone'])
  _infoMain = ['TRAD2_example', 867]
  _infoAttr = {'Lambda': ['TRAD2_example', 868],
   'diam_hydr': ['TRAD2_example', 869],
   'direction': ['TRAD2_example', 870],
   'sous_zone': ['TRAD2_example', 871]}
  _attributesList = check_append_list('perte_charge_directionnelle_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('perte_charge_directionnelle_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('perte_charge_directionnelle_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'Lambda':
          ('[chaine] Function for loss coefficient which may be Reynolds dependant (Ex: 64/Re).', ''),
    'diam_hydr':
          ('[champ_don_base] Hydraulic diameter value.', ''),
    'direction':
          ('[champ_don_base] Field which indicates the direction of the pressure loss.', ''),
    'sous_zone':
          ('[chaine] Optional sub-area where pressure loss applies.', ''),
  }
  _helpDict = check_append_dict('perte_charge_directionnelle_Tru', _helpDict, source_base_Tru._helpDict)
##### end perte_charge_directionnelle_Tru class

class perte_charge_isotrope_Tru(source_base_Tru): #####
  r"""
  Isotropic pressure loss.
  """
  # From: line 872, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('Lambda', 'chaine_Tru'),   #
    ('diam_hydr', '<inherited>champ_don_base_Tru'),   #
    ('sous_zone', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'Lambda': ['lambda_u']
  }
  _optionals = set(['sous_zone'])
  _infoMain = ['TRAD2_example', 872]
  _infoAttr = {'Lambda': ['TRAD2_example', 873],
   'diam_hydr': ['TRAD2_example', 874],
   'sous_zone': ['TRAD2_example', 875]}
  _attributesList = check_append_list('perte_charge_isotrope_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('perte_charge_isotrope_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('perte_charge_isotrope_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'Lambda':
          ('[chaine] Function for loss coefficient which may be Reynolds dependant (Ex: 64/Re).', ''),
    'diam_hydr':
          ('[champ_don_base] Hydraulic diameter value.', ''),
    'sous_zone':
          ('[chaine] Optional sub-area where pressure loss applies.', ''),
  }
  _helpDict = check_append_dict('perte_charge_isotrope_Tru', _helpDict, source_base_Tru._helpDict)
##### end perte_charge_isotrope_Tru class

class perte_charge_anisotrope_Tru(source_base_Tru): #####
  r"""
  Anisotropic pressure loss.
  """
  # From: line 876, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('Lambda', 'chaine_Tru'),   #
    ('lambda_ortho', 'chaine_Tru'),   #
    ('diam_hydr', '<inherited>champ_don_base_Tru'),   #
    ('direction', '<inherited>champ_don_base_Tru'),   #
    ('sous_zone', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'Lambda': ['lambda_u']
  }
  _optionals = set(['sous_zone'])
  _infoMain = ['TRAD2_example', 876]
  _infoAttr = {'Lambda': ['TRAD2_example', 877],
   'diam_hydr': ['TRAD2_example', 879],
   'direction': ['TRAD2_example', 880],
   'lambda_ortho': ['TRAD2_example', 878],
   'sous_zone': ['TRAD2_example', 881]}
  _attributesList = check_append_list('perte_charge_anisotrope_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('perte_charge_anisotrope_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('perte_charge_anisotrope_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'Lambda':
          ('[chaine] Function for loss coefficient which may be Reynolds dependant (Ex: 64/Re).', ''),
    'lambda_ortho':
          ('[chaine] Function for loss coefficient in transverse direction which may be Reynolds \n'
           'dependant (Ex: 64/Re).',
           ''),
    'diam_hydr':
          ('[champ_don_base] Hydraulic diameter value.', ''),
    'direction':
          ('[champ_don_base] Field which indicates the direction of the pressure loss.', ''),
    'sous_zone':
          ('[chaine] Optional sub-area where pressure loss applies.', ''),
  }
  _helpDict = check_append_dict('perte_charge_anisotrope_Tru', _helpDict, source_base_Tru._helpDict)
##### end perte_charge_anisotrope_Tru class

class perte_charge_circulaire_Tru(source_base_Tru): #####
  r"""
  New pressure loss.
  """
  # From: line 882, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('Lambda', 'chaine_Tru'),   #
    ('lambda_ortho', 'chaine_Tru'),   #
    ('diam_hydr', '<inherited>champ_don_base_Tru'),   #
    ('diam_hydr_ortho', '<inherited>champ_don_base_Tru'),   #
    ('direction', '<inherited>champ_don_base_Tru'),   #
    ('sous_zone', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'Lambda': ['lambda_u']
  }
  _optionals = set(['sous_zone'])
  _infoMain = ['TRAD2_example', 882]
  _infoAttr = {'Lambda': ['TRAD2_example', 883],
   'diam_hydr': ['TRAD2_example', 885],
   'diam_hydr_ortho': ['TRAD2_example', 886],
   'direction': ['TRAD2_example', 887],
   'lambda_ortho': ['TRAD2_example', 884],
   'sous_zone': ['TRAD2_example', 888]}
  _attributesList = check_append_list('perte_charge_circulaire_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('perte_charge_circulaire_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('perte_charge_circulaire_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'Lambda':
          ('[chaine] Function f(Re_tot, Re_long, t, x, y, z) for loss coefficient in the longitudinal \n'
           'direction',
           ''),
    'lambda_ortho':
          ('[chaine] function: Function f(Re_tot, Re_ortho, t, x, y, z) for loss coefficient in \n'
           'transverse direction',
           ''),
    'diam_hydr':
          ('[champ_don_base] Hydraulic diameter value.', ''),
    'diam_hydr_ortho':
          ('[champ_don_base] Transverse hydraulic diameter value.', ''),
    'direction':
          ('[champ_don_base] Field which indicates the direction of the pressure loss.', ''),
    'sous_zone':
          ('[chaine] Optional sub-area where pressure loss applies.', ''),
  }
  _helpDict = check_append_dict('perte_charge_circulaire_Tru', _helpDict, source_base_Tru._helpDict)
##### end perte_charge_circulaire_Tru class

class convection_diffusion_concentration_Tru(eqn_base_Tru): #####
  r"""
  Constituent transport vectorial equation (concentration diffusion convection).
  """
  # From: line 889, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('nom_inconnue', 'chaine_Tru'),   #
    ('masse_molaire', 'floattant_820d8b_Tru'),   #
    ('alias', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['nom_inconnue', 'masse_molaire', 'alias'])
  _infoMain = ['TRAD2_example', 889]
  _infoAttr = {'alias': ['TRAD2_example', 892],
   'masse_molaire': ['TRAD2_example', 891],
   'nom_inconnue': ['TRAD2_example', 890]}
  _attributesList = check_append_list('convection_diffusion_concentration_Tru', _attributesList, eqn_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_diffusion_concentration_Tru', _attributesSynonyms, eqn_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_diffusion_concentration_Tru', _infoAttr, eqn_base_Tru._infoAttr)
  _optionals.update(eqn_base_Tru._optionals)
  _helpDict = {
    'nom_inconnue':
          ('[chaine] Keyword Nom_inconnue will rename the unknown of this equation with the given \n'
           'name.\n'
           'In the postprocessing part, the concentration field will be accessible with this \n'
           'name.\n'
           'This is usefull if you want to track more than one concentration (otherwise, only \n'
           'the concentration field in the first concentration equation can be accessed).',
           ''),
    'masse_molaire':
          ('[floattant] not_set', ''),
    'alias':
          ('[chaine] not_set', ''),
  }
  _helpDict = check_append_dict('convection_diffusion_concentration_Tru', _helpDict, eqn_base_Tru._helpDict)
##### end convection_diffusion_concentration_Tru class

class darcy_Tru(source_base_Tru): #####
  r"""
  Class for calculation in a porous media with source term of Darcy -nu/K*V.
  This keyword must be used with a permeability model.
  For the moment there are two models : permeability constant or Ergun's law.
  Darcy source term is available for quasi compressible calculation.
  A new keyword is aded for porosity (porosite).
  """
  # From: line 893, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('bloc', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 893]
  _infoAttr = {'bloc': ['TRAD2_example', 894]}
  _attributesList = check_append_list('darcy_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('darcy_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('darcy_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'bloc':
          ('[bloc_lecture] Description.', ''),
  }
  _helpDict = check_append_dict('darcy_Tru', _helpDict, source_base_Tru._helpDict)
##### end darcy_Tru class

class forchheimer_Tru(source_base_Tru): #####
  r"""
  Class to add the source term of Forchheimer -Cf/sqrt(K)*V2 in the Navier-Stokes equations.
  We must precise a permeability model : constant or Ergun's law.
  Moreover we can give the constant Cf : by default its value is 1.
  Forchheimer source term is available also for quasi compressible calculation.
  A new keyword is aded for porosity (porosite).
  """
  # From: line 895, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('bloc', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 895]
  _infoAttr = {'bloc': ['TRAD2_example', 896]}
  _attributesList = check_append_list('forchheimer_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('forchheimer_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('forchheimer_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'bloc':
          ('[bloc_lecture] Description.', ''),
  }
  _helpDict = check_append_dict('forchheimer_Tru', _helpDict, source_base_Tru._helpDict)
##### end forchheimer_Tru class

class debut_bloc_Tru(interprete_Tru): #####
  r"""
  Block's beginning.
  """
  # From: line 897, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['{_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 897]
  _infoAttr = {}
  _attributesList = check_append_list('debut_bloc_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('debut_bloc_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('debut_bloc_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('debut_bloc_Tru', _helpDict, interprete_Tru._helpDict)
##### end debut_bloc_Tru class

class fin_bloc_Tru(interprete_Tru): #####
  r"""
  Block's end.
  """
  # From: line 898, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['}_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 898]
  _infoAttr = {}
  _attributesList = check_append_list('fin_bloc_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('fin_bloc_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('fin_bloc_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('fin_bloc_Tru', _helpDict, interprete_Tru._helpDict)
##### end fin_bloc_Tru class

class export_Tru(interprete_Tru): #####
  r"""
  Class to make the object have a global range, if not its range will apply to the block 
  only (the associated object will be destroyed on exiting the block).
  """
  # From: line 899, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 899]
  _infoAttr = {}
  _attributesList = check_append_list('export_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('export_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('export_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('export_Tru', _helpDict, interprete_Tru._helpDict)
##### end export_Tru class

class debog_Tru(interprete_Tru): #####
  r"""
  Class to debug some differences between two TRUST versions on a same data file.
  
  If you want to compare the results of the same code in sequential and parallel calculation, 
  first run (mode=0) in sequential mode (the files fichier1 and fichier2 will be written 
  first) then the second run in parallel calculation (mode=1).
  
  During the first run (mode=0), it prints into the file DEBOG, values at different 
  points of the code thanks to the C++ instruction call.
  see for example in Kernel/Framework/Resoudre.cpp file the instruction: Debog::verifier(msg,value); 
  Where msg is a string and value may be a double, an integer or an array.
  
  During the second run (mode=1), it prints into a file Err_Debog.dbg the same messages 
  than in the DEBOG file and checks if the differences between results from both codes 
  are less than a given value (error).
  If not, it prints Ok else show the differences and the lines where it occured.
  """
  # From: line 900, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('pb', 'ref_pb_gen_base_Tru'),   #
    ('fichier1', 'chaine_Tru'),   #
    ('fichier2', 'chaine_Tru'),   #
    ('seuil', 'floattant_820d8b_Tru'),   #
    ('mode', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'fichier1': ['file1'], 'fichier2': ['file2']
  }
  _optionals = set([])
  _infoMain = ['TRAD2_example', 900]
  _infoAttr = {'fichier1': ['TRAD2_example', 902],
   'fichier2': ['TRAD2_example', 903],
   'mode': ['TRAD2_example', 905],
   'pb': ['TRAD2_example', 901],
   'seuil': ['TRAD2_example', 904]}
  _attributesList = check_append_list('debog_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('debog_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('debog_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'pb':
          ('[ref_pb_gen_base] Name of the problem to debug.', ''),
    'fichier1':
          ('[chaine] Name of the file where domain will be written in sequential calculation.', ''),
    'fichier2':
          ('[chaine] Name of the file where faces will be written in sequential calculation.', ''),
    'seuil':
          ('[floattant] Minimal value (by default 1.e-20) for the differences between the two \ncodes.', ''),
    'mode':
          ('[entier] By default -1 (nothing is written in the different files), you will set 0 \n'
           'for the sequential run, and 1 for the parallel run.',
           ''),
  }
  _helpDict = check_append_dict('debog_Tru', _helpDict, interprete_Tru._helpDict)
##### end debog_Tru class

class pilote_icoco_Tru(interprete_Tru): #####
  r"""
  not_set
  """
  # From: line 906, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('pb_name', 'chaine_Tru'),   #
    ('main', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 906]
  _infoAttr = {'main': ['TRAD2_example', 908], 'pb_name': ['TRAD2_example', 907]}
  _attributesList = check_append_list('pilote_icoco_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pilote_icoco_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pilote_icoco_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'pb_name':
          ('[chaine] not_set', ''),
    'main':
          ('[chaine] not_set', ''),
  }
  _helpDict = check_append_dict('pilote_icoco_Tru', _helpDict, interprete_Tru._helpDict)
##### end pilote_icoco_Tru class

class ch_front_input_Tru(front_field_base_Tru): #####
  r"""
  not_set
  """
  # From: line 909, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('nb_comp', 'entier_3282e0_Tru'),   #
    ('nom', 'chaine_Tru'),   #
    ('initial_value', 'list_Tru'),   #
    ('probleme', 'chaine_Tru'),   #
    ('sous_zone', 'ref_sous_zone_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['initial_value', 'sous_zone'])
  _infoMain = ['TRAD2_example', 909]
  _infoAttr = {'initial_value': ['TRAD2_example', 912],
   'nb_comp': ['TRAD2_example', 910],
   'nom': ['TRAD2_example', 911],
   'probleme': ['TRAD2_example', 913],
   'sous_zone': ['TRAD2_example', 914]}
  _attributesList = check_append_list('ch_front_input_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('ch_front_input_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('ch_front_input_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'nb_comp':
          ('[entier] not_set', ''),
    'nom':
          ('[chaine] not_set', ''),
    'initial_value':
          ('[list] not_set', ''),
    'probleme':
          ('[chaine] not_set', ''),
    'sous_zone':
          ('[ref_sous_zone] not_set', ''),
  }
  _helpDict = check_append_dict('ch_front_input_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end ch_front_input_Tru class

class ch_front_input_uniforme_Tru(ch_front_input_Tru): #####
  r"""
  for coupling, you can use ch_front_input_uniforme which is a champ_front_uniforme, 
  which use an external value.
  It must be used with Problem.setInputField.
  """
  # From: line 915, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 915]
  _infoAttr = {}
  _attributesList = check_append_list('ch_front_input_uniforme_Tru', _attributesList, ch_front_input_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('ch_front_input_uniforme_Tru', _attributesSynonyms, ch_front_input_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('ch_front_input_uniforme_Tru', _infoAttr, ch_front_input_Tru._infoAttr)
  _optionals.update(ch_front_input_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('ch_front_input_uniforme_Tru', _helpDict, ch_front_input_Tru._helpDict)
##### end ch_front_input_uniforme_Tru class

class champ_input_base_Tru(field_base_Tru): #####
  r"""
  not_set
  """
  # From: line 916, in file 'TRAD2_example'
  _braces = 1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = [
    ('nb_comp', 'entier_3282e0_Tru'),   #
    ('nom', 'chaine_Tru'),   #
    ('initial_value', 'list_Tru'),   #
    ('probleme', 'chaine_Tru'),   #
    ('sous_zone', 'ref_sous_zone_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['initial_value', 'sous_zone'])
  _infoMain = ['TRAD2_example', 916]
  _infoAttr = {'initial_value': ['TRAD2_example', 919],
   'nb_comp': ['TRAD2_example', 917],
   'nom': ['TRAD2_example', 918],
   'probleme': ['TRAD2_example', 920],
   'sous_zone': ['TRAD2_example', 921]}
  _attributesList = check_append_list('champ_input_base_Tru', _attributesList, field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_input_base_Tru', _attributesSynonyms, field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_input_base_Tru', _infoAttr, field_base_Tru._infoAttr)
  _optionals.update(field_base_Tru._optionals)
  _helpDict = {
    'nb_comp':
          ('[entier] not_set', ''),
    'nom':
          ('[chaine] not_set', ''),
    'initial_value':
          ('[list] not_set', ''),
    'probleme':
          ('[chaine] not_set', ''),
    'sous_zone':
          ('[ref_sous_zone] not_set', ''),
  }
  _helpDict = check_append_dict('champ_input_base_Tru', _helpDict, field_base_Tru._helpDict)
##### end champ_input_base_Tru class

class champ_input_p0_Tru(champ_input_base_Tru): #####
  r"""
  not_set
  """
  # From: line 922, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 922]
  _infoAttr = {}
  _attributesList = check_append_list('champ_input_p0_Tru', _attributesList, champ_input_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_input_p0_Tru', _attributesSynonyms, champ_input_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_input_p0_Tru', _infoAttr, champ_input_base_Tru._infoAttr)
  _optionals.update(champ_input_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('champ_input_p0_Tru', _helpDict, champ_input_base_Tru._helpDict)
##### end champ_input_p0_Tru class

class moyenne_volumique_Tru(interprete_Tru): #####
  r"""
  This keyword should be used after Resoudre keyword.
  It computes the convolution product of one or more fields with a given filtering 
  function.
  """
  # From: line 923, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('nom_pb', 'ref_pb_base_Tru'),   #
    ('nom_domaine', 'ref_domaine_Tru'),   #
    ('noms_champs', 'listchaine_Tru'),   #
    ('nom_fichier_post', 'chaine_Tru'),   #
    ('format_post', 'chaine_Tru'),   #
    ('localisation', 'chaine_7347ec_Tru'),   #
    ('fonction_filtre', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['nom_fichier_post', 'format_post', 'localisation'])
  _infoMain = ['TRAD2_example', 923]
  _infoAttr = {'fonction_filtre': ['TRAD2_example', 930],
   'format_post': ['TRAD2_example', 928],
   'localisation': ['TRAD2_example', 929],
   'nom_domaine': ['TRAD2_example', 925],
   'nom_fichier_post': ['TRAD2_example', 927],
   'nom_pb': ['TRAD2_example', 924],
   'noms_champs': ['TRAD2_example', 926]}
  _attributesList = check_append_list('moyenne_volumique_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('moyenne_volumique_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('moyenne_volumique_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'nom_pb':
          ('[ref_pb_base] name of the problem where the source fields will be searched.', ''),
    'nom_domaine':
          ('[ref_domaine] name of the destination domain (for example, it can be a coarser mesh, \n'
           'but for optimal performance in parallel, the domain should be split with the same \n'
           'algorithm as the computation mesh, eg, same tranche parameters for example)',
           ''),
    'noms_champs':
          ('[listchaine] name of the source fields (these fields must be accessible from the '
           'postraitement) \n'
           'N source_field1 source_field2 ...\n'
           'source_fieldN',
           ''),
    'nom_fichier_post':
          ('[chaine] indicates the filename where the result is written', ''),
    'format_post':
          ('[chaine] gives the fileformat for the result (by default : lata)', ''),
    'localisation':
          ('[chaine(into=["elem","som"])] indicates where the convolution product should be computed: \n'
           'either on the elements or on the nodes of the destination domain.',
           ''),
    'fonction_filtre':
          ('[bloc_lecture] to specify the given filter\n'
           '\n'
           'Fonction_filtre {NL2 type filter_typeNL2 demie-largeur lNL2 [ omega w ]\n'
           '\n'
           '[ expression string ]NL2 }\n'
           '\n'
           'NL2 type filter_type : This parameter specifies the filtering function.\n'
           'Valid filter_type are:NL2 Boite is a box filter, $f(x,y,z)=(abs(x)<l)*(abs(y) <l)*(abs(z) \n'
           '<l) / (8 l^3)$NL2 Chapeau is a hat filter (product of hat filters in each direction) \n'
           'centered on the origin, the half-width of the filter being l and its integral being \n'
           '1.NL2 Quadra is a 2nd order filter.NL2 Gaussienne is a normalized gaussian filter \n'
           'of standard deviation sigma in each direction (all field elements outside a cubic \n'
           'box defined by clipping_half_width are ignored, hence, taking clipping_half_width=2.5*sigma \n'
           'yields an integral of 0.99 for a uniform unity field).NL2 Parser allows a user defined \n'
           'function of the x,y,z variables.\n'
           'All elements outside a cubic box defined by clipping_half_width are ignored.\n'
           'The parser is much slower than the equivalent c++ coded function...NL2\n'
           '\n'
           'demie-largeur l : This parameter specifies the half width of the filterNL2 [ omega \n'
           'w ] : This parameter must be given for the gaussienne filter.\n'
           'It defines the standard deviation of the gaussian filter.NL2 [ expression string] \n'
           ': This parameter must be given for the parser filter type.\n'
           'This expression will be interpreted by the math parser with the predefined variables \n'
           'x, y and z.',
           ''),
  }
  _helpDict = check_append_dict('moyenne_volumique_Tru', _helpDict, interprete_Tru._helpDict)
##### end moyenne_volumique_Tru class

class lire_ideas_Tru(interprete_Tru): #####
  r"""
  Read a geom in a unv file.
  3D tetra mesh elements only may be read by TRUST.
  """
  # From: line 931, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('nom_dom', 'ref_domaine_Tru'),   #
    ('file', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 931]
  _infoAttr = {'file': ['TRAD2_example', 933], 'nom_dom': ['TRAD2_example', 932]}
  _attributesList = check_append_list('lire_ideas_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('lire_ideas_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('lire_ideas_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'nom_dom':
          ('[ref_domaine] Name of domain.', ''),
    'file':
          ('[chaine] Name of file.', ''),
  }
  _helpDict = check_append_dict('lire_ideas_Tru', _helpDict, interprete_Tru._helpDict)
##### end lire_ideas_Tru class

class system_Tru(interprete_Tru): #####
  r"""
  To run Unix commands from the data file.
  Example: System 'echo The End | mail trust@cea.fr'
  """
  # From: line 934, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('cmd', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 934]
  _infoAttr = {'cmd': ['TRAD2_example', 935]}
  _attributesList = check_append_list('system_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('system_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('system_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'cmd':
          ('[chaine] command to execute.', ''),
  }
  _helpDict = check_append_dict('system_Tru', _helpDict, interprete_Tru._helpDict)
##### end system_Tru class

class redresser_hexaedres_vdf_Tru(interprete_Tru): #####
  r"""
  Keyword to convert a domain (named domain_name) with quadrilaterals/VEF hexaedras 
  which looks like rectangles/VDF hexaedras into a domain with real rectangles/VDF hexaedras.
  """
  # From: line 936, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 936]
  _infoAttr = {'domain_name': ['TRAD2_example', 937]}
  _attributesList = check_append_list('redresser_hexaedres_vdf_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('redresser_hexaedres_vdf_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('redresser_hexaedres_vdf_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of domain to resequence.', ''),
  }
  _helpDict = check_append_dict('redresser_hexaedres_vdf_Tru', _helpDict, interprete_Tru._helpDict)
##### end redresser_hexaedres_vdf_Tru class

class analyse_angle_Tru(interprete_Tru): #####
  r"""
  Keyword Analyse_angle prints the histogram of the largest angle of each mesh elements 
  of the domain named name_domain.
  nb_histo is the histogram number of bins.
  It is called by default during the domain discretization with nb_histo set to 18.
  Useful to check the number of elements with angles above 90 degrees.
  """
  # From: line 938, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
    ('nb_histo', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 938]
  _infoAttr = {'domain_name': ['TRAD2_example', 939], 'nb_histo': ['TRAD2_example', 940]}
  _attributesList = check_append_list('analyse_angle_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('analyse_angle_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('analyse_angle_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of domain to resequence.', ''),
    'nb_histo':
          ('[entier] not_set', ''),
  }
  _helpDict = check_append_dict('analyse_angle_Tru', _helpDict, interprete_Tru._helpDict)
##### end analyse_angle_Tru class

class remove_invalid_internal_boundaries_Tru(interprete_Tru): #####
  r"""
  Keyword to suppress an internal boundary of the domain_name domain.
  Indeed, some mesh tools may define internal boundaries (eg: for post processing task 
  after the calculation) but TRUST does not support it yet.
  """
  # From: line 941, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 941]
  _infoAttr = {'domain_name': ['TRAD2_example', 942]}
  _attributesList = check_append_list('remove_invalid_internal_boundaries_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('remove_invalid_internal_boundaries_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('remove_invalid_internal_boundaries_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of domain.', ''),
  }
  _helpDict = check_append_dict('remove_invalid_internal_boundaries_Tru', _helpDict, interprete_Tru._helpDict)
##### end remove_invalid_internal_boundaries_Tru class

class resequencing_Tru(interprete_Tru): #####
  r"""
  The Reordonner interpretor is required sometimes for a VDF mesh which is not produced 
  by the internal mesher.
  Example where this is used:
  
  Read_file dom fichier.geom
  
  Reordonner dom
  
  Observations: This keyword is redundant when the mesh that is read is correctly sequenced 
  in the TRUST sense.
  This significant mesh operation may take some time...
  The message returned by TRUST is not explicit when the Reordonner (Resequencing) 
  keyword is required but not included in the data set...
  """
  # From: line 943, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['reordonner_Tru']
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 943]
  _infoAttr = {'domain_name': ['TRAD2_example', 944]}
  _attributesList = check_append_list('resequencing_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('resequencing_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('resequencing_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of domain to resequence.', ''),
  }
  _helpDict = check_append_dict('resequencing_Tru', _helpDict, interprete_Tru._helpDict)
##### end resequencing_Tru class

class precisiongeom_Tru(interprete_Tru): #####
  r"""
  Class to change the way floating-point number comparison is done.
  By default, two numbers are equal if their absolute difference is smaller than 1e-10.
  The keyword is useful to modify this value.
  Moreover, nodes coordinates will be written in .geom files with this same precision.
  """
  # From: line 945, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('precision', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 945]
  _infoAttr = {'precision': ['TRAD2_example', 946]}
  _attributesList = check_append_list('precisiongeom_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('precisiongeom_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('precisiongeom_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'precision':
          ('[floattant] New value of precision.', ''),
  }
  _helpDict = check_append_dict('precisiongeom_Tru', _helpDict, interprete_Tru._helpDict)
##### end precisiongeom_Tru class

class nettoiepasnoeuds_Tru(interprete_Tru): #####
  r"""
  Keyword NettoiePasNoeuds does not delete useless nodes (nodes without elements) from 
  a domain.
  """
  # From: line 947, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 947]
  _infoAttr = {'domain_name': ['TRAD2_example', 948]}
  _attributesList = check_append_list('nettoiepasnoeuds_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('nettoiepasnoeuds_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('nettoiepasnoeuds_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of domain.', ''),
  }
  _helpDict = check_append_dict('nettoiepasnoeuds_Tru', _helpDict, interprete_Tru._helpDict)
##### end nettoiepasnoeuds_Tru class

class scatter_Tru(interprete_Tru): #####
  r"""
  Class to read a partionned mesh in the files during a parallel calculation.
  The files are in binary format.
  """
  # From: line 949, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('file', 'chaine_Tru'),   #
    ('domaine', 'ref_domaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 949]
  _infoAttr = {'domaine': ['TRAD2_example', 951], 'file': ['TRAD2_example', 950]}
  _attributesList = check_append_list('scatter_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('scatter_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('scatter_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'file':
          ('[chaine] Name of file.', ''),
    'domaine':
          ('[ref_domaine] Name of domain.', ''),
  }
  _helpDict = check_append_dict('scatter_Tru', _helpDict, interprete_Tru._helpDict)
##### end scatter_Tru class

class scattermed_Tru(scatter_Tru): #####
  r"""
  This keyword will read the partition of the domain_name domain into a the MED format 
  files file.med created by Medsplitter.
  """
  # From: line 952, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 952]
  _infoAttr = {}
  _attributesList = check_append_list('scattermed_Tru', _attributesList, scatter_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('scattermed_Tru', _attributesSynonyms, scatter_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('scattermed_Tru', _infoAttr, scatter_Tru._infoAttr)
  _optionals.update(scatter_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('scattermed_Tru', _helpDict, scatter_Tru._helpDict)
##### end scattermed_Tru class

class listdeuxmots_sacc_Tru(ListOfBase_Tru): #####
  """
  List of groups of two words (without curly brackets).
  """
  # From: line 953, in file 'TRAD2_example'
  _braces = 0
  _comma = 0
  _allowedClasses = ["deuxmots_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 953]
##### end listdeuxmots_sacc_Tru class

class partitionneur_deriv_Tru(ConstrainBase_Tru): #####
  r"""
  not_set
  """
  # From: line 954, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = [
    ('nb_parts', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['nb_parts'])
  _infoMain = ['TRAD2_example', 954]
  _infoAttr = {'nb_parts': ['TRAD2_example', 955]}
  _helpDict = {
    'nb_parts':
          ('[entier] The number of non empty parts that must be generated (generally equal to \n'
           'the number of processors in the parallel run).',
           ''),
  }
##### end partitionneur_deriv_Tru class

class partitionneur_metis_Tru(partitionneur_deriv_Tru): #####
  r"""
  Metis is an external partitionning library.
  It is a general algorithm that will generate a partition of the domain.
  """
  # From: line 956, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['metis_Tru']
  _attributesList = [
    ('kmetis', 'rien_Tru'),   #
    ('use_weights', 'rien_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['kmetis', 'use_weights'])
  _infoMain = ['TRAD2_example', 956]
  _infoAttr = {'kmetis': ['TRAD2_example', 957], 'use_weights': ['TRAD2_example', 958]}
  _attributesList = check_append_list('partitionneur_metis_Tru', _attributesList, partitionneur_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('partitionneur_metis_Tru', _attributesSynonyms, partitionneur_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('partitionneur_metis_Tru', _infoAttr, partitionneur_deriv_Tru._infoAttr)
  _optionals.update(partitionneur_deriv_Tru._optionals)
  _helpDict = {
    'kmetis':
          ('[rien] The default values are pmetis, default parameters are automatically chosen \n'
           'by Metis.\n'
           "'kmetis' is faster than pmetis option but the last option produces better partitioning \n"
           'quality.\n'
           'In both cases, the partitioning quality may be slightly improved by increasing the \n'
           'nb_essais option (by default N=1).\n'
           'It will compute N partitions and will keep the best one (smallest edge cut number).\n'
           'But this option is CPU expensive, taking N=10 will multiply the CPU cost of partitioning \n'
           'by 10.\n'
           '\n'
           'Experiments show that only marginal improvements can be obtained with non default \n'
           'parameters.',
           ''),
    'use_weights':
          ('[rien] If use_weights is specified, weighting of the element-element links in the \n'
           'graph is used to force metis to keep opposite periodic elements on the same processor.\n'
           'This option can slightly improve the partitionning quality but it consumes more memory \n'
           'and takes more time.\n'
           'It is not mandatory since a correction algorithm is always applied afterwards to \n'
           'ensure a correct partitionning for periodic boundaries.',
           ''),
  }
  _helpDict = check_append_dict('partitionneur_metis_Tru', _helpDict, partitionneur_deriv_Tru._helpDict)
##### end partitionneur_metis_Tru class

class partitionneur_sous_zones_Tru(partitionneur_deriv_Tru): #####
  r"""
  This algorithm will create one part for each specified subzone.
  All elements contained in the first subzone are put in the first part, all remaining 
  elements contained in the second subzone in the second part, etc...
  
  If all elements of the domain are contained in the specified subzones, then N parts 
  are created, otherwise, a supplemental part is created with the remaining elements.
  
  If no subzone is specified, all subzones defined in the domain are used to split 
  the mesh.
  """
  # From: line 959, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['sous_zones_Tru', 'partitionneur_sous_domaines_Tru']
  _attributesList = [
    ('sous_zones', 'listchaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 959]
  _infoAttr = {'sous_zones': ['TRAD2_example', 960]}
  _attributesList = check_append_list('partitionneur_sous_zones_Tru', _attributesList, partitionneur_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('partitionneur_sous_zones_Tru', _attributesSynonyms, partitionneur_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('partitionneur_sous_zones_Tru', _infoAttr, partitionneur_deriv_Tru._infoAttr)
  _optionals.update(partitionneur_deriv_Tru._optionals)
  _helpDict = {
    'sous_zones':
          ('[listchaine] N SUBZONE_NAME_1 SUBZONE_NAME_2 ...', ''),
  }
  _helpDict = check_append_dict('partitionneur_sous_zones_Tru', _helpDict, partitionneur_deriv_Tru._helpDict)
##### end partitionneur_sous_zones_Tru class

class partitionneur_tranche_Tru(partitionneur_deriv_Tru): #####
  r"""
  This algorithm will create a geometrical partitionning by slicing the mesh in the 
  two or three axis directions, based on the geometric center of each mesh element.
  nz must be given if dimension=3.
  Each slice contains the same number of elements (slices don't have the same geometrical 
  width, and for VDF meshes, slice boundaries are generally not flat except if the number 
  of mesh elements in each direction is an exact multiple of the number of slices).
  First, nx slices in the X direction are created, then each slice is split in ny slices 
  in the Y direction, and finally, each part is split in nz slices in the Z direction.
  The resulting number of parts is nx*ny*nz.
  If one particular direction has been declared periodic, the default slicing (0, 1, 
  2, ..., n-1)is replaced by (0, 1, 2, ...
  n-1, 0), each of the two '0' slices having twice less elements than the other slices.
  """
  # From: line 961, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['tranche_Tru']
  _attributesList = [
    ('tranches', 'listentierf_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['tranches'])
  _infoMain = ['TRAD2_example', 961]
  _infoAttr = {'tranches': ['TRAD2_example', 962]}
  _attributesList = check_append_list('partitionneur_tranche_Tru', _attributesList, partitionneur_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('partitionneur_tranche_Tru', _attributesSynonyms, partitionneur_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('partitionneur_tranche_Tru', _infoAttr, partitionneur_deriv_Tru._infoAttr)
  _optionals.update(partitionneur_deriv_Tru._optionals)
  _helpDict = {
    'tranches':
          ('[listentierf] Partitioned by nx in the X direction, ny in the Y direction, nz in the \n'
           'Z direction.\n'
           'Works only for structured meshes.\n'
           'No warranty for unstructured meshes.',
           ''),
  }
  _helpDict = check_append_dict('partitionneur_tranche_Tru', _helpDict, partitionneur_deriv_Tru._helpDict)
##### end partitionneur_tranche_Tru class

class partitionneur_partition_Tru(partitionneur_deriv_Tru): #####
  r"""
  This algorithm re-use the partition of the domain named DOMAINE_NAME.
  It is useful to partition for example a post processing domain.
  The partition should match with the calculation domain.
  """
  # From: line 963, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['partition_Tru', 'decouper_Tru']
  _attributesList = [
    ('domaine', 'ref_domaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 963]
  _infoAttr = {'domaine': ['TRAD2_example', 964]}
  _attributesList = check_append_list('partitionneur_partition_Tru', _attributesList, partitionneur_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('partitionneur_partition_Tru', _attributesSynonyms, partitionneur_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('partitionneur_partition_Tru', _infoAttr, partitionneur_deriv_Tru._infoAttr)
  _optionals.update(partitionneur_deriv_Tru._optionals)
  _helpDict = {
    'domaine':
          ('[ref_domaine] domain name', ''),
  }
  _helpDict = check_append_dict('partitionneur_partition_Tru', _helpDict, partitionneur_deriv_Tru._helpDict)
##### end partitionneur_partition_Tru class

class partitionneur_fichier_decoupage_Tru(partitionneur_deriv_Tru): #####
  r"""
  This algorithm reads an array of integer values on the disc, one value for each mesh 
  element.
  Each value is interpreted as the target part number n>=0 for this element.
  The number of parts created is the highest value in the array plus one.
  Empty parts can be created if some values are not present in the array.
  
  The file format is ASCII, and contains space, tab or carriage-return separated integer 
  values.
  The first value is the number nb_elem of elements in the domain, followed by nb_elem 
  integer values (positive or zero).
  
  This algorithm has been designed to work together with the 'ecrire_decoupage' option.
  You can generate a partition with any other algorithm, write it to disc, modify it, 
  and read it again to generate the .Zone files.
  
  Contrary to other partitioning algorithms, no correction is applied by default to 
  the partition (eg.
  element 0 on processor 0 and corrections for periodic boundaries).
  If 'corriger_partition' is specified, these corrections are applied.
  """
  # From: line 965, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['fichier_decoupage_Tru']
  _attributesList = [
    ('fichier', 'chaine_Tru'),   #
    ('corriger_partition', 'rien_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['corriger_partition'])
  _infoMain = ['TRAD2_example', 965]
  _infoAttr = {'corriger_partition': ['TRAD2_example', 967],
   'fichier': ['TRAD2_example', 966]}
  _attributesList = check_append_list('partitionneur_fichier_decoupage_Tru', _attributesList, partitionneur_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('partitionneur_fichier_decoupage_Tru', _attributesSynonyms, partitionneur_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('partitionneur_fichier_decoupage_Tru', _infoAttr, partitionneur_deriv_Tru._infoAttr)
  _optionals.update(partitionneur_deriv_Tru._optionals)
  _helpDict = {
    'fichier':
          ('[chaine] FILENAME', ''),
    'corriger_partition':
          ('[rien] not_set', ''),
  }
  _helpDict = check_append_dict('partitionneur_fichier_decoupage_Tru', _helpDict, partitionneur_deriv_Tru._helpDict)
##### end partitionneur_fichier_decoupage_Tru class

class distance_paroi_Tru(interprete_Tru): #####
  r"""
  Class to generate external file Wall_length.xyz devoted for instance, for mixing length 
  modelling.
  In this file, are saved the coordinates of each element (center of gravity) of dom 
  domain and minimum distance between this point and boundaries (specified bords) that 
  user specifies in data file (typically, those associated to walls).
  A field Distance_paroi is available to post process the distance to the wall.
  """
  # From: line 968, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('dom', 'ref_domaine_Tru'),   #
    ('bords', 'listchaine_Tru'),   #
    ('format', 'chaine_7d729e_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 968]
  _infoAttr = {'bords': ['TRAD2_example', 970],
   'dom': ['TRAD2_example', 969],
   'format': ['TRAD2_example', 971]}
  _attributesList = check_append_list('distance_paroi_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('distance_paroi_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('distance_paroi_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'dom':
          ('[ref_domaine] Name of domain.', ''),
    'bords':
          ('[listchaine] Boundaries.', ''),
    'format':
          ('[chaine(into=["binaire","formatte"])] Value for format may be binaire (a binary file \n'
           'Wall_length.xyz is written) or formatte (moreover, a formatted file Wall_length_formatted.xyz \n'
           'is written).',
           ''),
  }
  _helpDict = check_append_dict('distance_paroi_Tru', _helpDict, interprete_Tru._helpDict)
##### end distance_paroi_Tru class

class troisf_Tru(objet_lecture_Tru): #####
  r"""
  Auxiliary class to extrude.
  """
  # From: line 972, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('lx', 'floattant_820d8b_Tru'),   #
    ('ly', 'floattant_820d8b_Tru'),   #
    ('lz', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 972]
  _infoAttr = {'lx': ['TRAD2_example', 973],
   'ly': ['TRAD2_example', 974],
   'lz': ['TRAD2_example', 975]}
  _attributesList = check_append_list('troisf_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('troisf_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('troisf_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'lx':
          ('[floattant] X direction of the extrude operation.', ''),
    'ly':
          ('[floattant] Y direction of the extrude operation.', ''),
    'lz':
          ('[floattant] Z direction of the extrude operation.', ''),
  }
  _helpDict = check_append_dict('troisf_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end troisf_Tru class

class extruder_Tru(interprete_Tru): #####
  r"""
  Class to create a 3D tetrahedral/hexahedral mesh (a prism is cut in 14) from a 2D 
  triangular/quadrangular mesh.
  """
  # From: line 976, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('domaine', 'ref_domaine_Tru'),   #
    ('direction', 'troisf_Tru'),   #
    ('nb_tranches', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'domaine': ['domain_name']
  }
  _optionals = set([])
  _infoMain = ['TRAD2_example', 976]
  _infoAttr = {'direction': ['TRAD2_example', 978],
   'domaine': ['TRAD2_example', 977],
   'nb_tranches': ['TRAD2_example', 979]}
  _attributesList = check_append_list('extruder_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('extruder_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('extruder_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domaine':
          ('[ref_domaine] Name of the domain.', ''),
    'direction':
          ('[troisf] Direction of the extrude operation.', ''),
    'nb_tranches':
          ('[entier] Number of elements in the extrusion direction.', ''),
  }
  _helpDict = check_append_dict('extruder_Tru', _helpDict, interprete_Tru._helpDict)
##### end extruder_Tru class

class extract_2d_from_3d_Tru(interprete_Tru): #####
  r"""
  Keyword to extract a 2D mesh by selecting a boundary of the 3D mesh.
  To generate a 2D axisymmetric mesh prefer Extract_2Daxi_from_3D keyword.
  """
  # From: line 980, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('dom3d', 'ref_domaine_Tru'),   #
    ('bord', 'chaine_Tru'),   #
    ('dom2d', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 980]
  _infoAttr = {'bord': ['TRAD2_example', 982],
   'dom2d': ['TRAD2_example', 983],
   'dom3d': ['TRAD2_example', 981]}
  _attributesList = check_append_list('extract_2d_from_3d_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('extract_2d_from_3d_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('extract_2d_from_3d_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'dom3d':
          ('[ref_domaine] Domain name of the 3D mesh', ''),
    'bord':
          ('[chaine] Boundary name.\n'
           'This boundary becomes the new 2D mesh and all the boundaries, in 3D, attached to \n'
           'the selected boundary, give their name to the new boundaries, in 2D.',
           ''),
    'dom2d':
          ('[chaine] Domain name of the new 2D mesh', ''),
  }
  _helpDict = check_append_dict('extract_2d_from_3d_Tru', _helpDict, interprete_Tru._helpDict)
##### end extract_2d_from_3d_Tru class

class extract_2daxi_from_3d_Tru(extract_2d_from_3d_Tru): #####
  r"""
  Keyword to extract a 2D axisymetric mesh by selecting a boundary of the 3D mesh.
  """
  # From: line 984, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 984]
  _infoAttr = {}
  _attributesList = check_append_list('extract_2daxi_from_3d_Tru', _attributesList, extract_2d_from_3d_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('extract_2daxi_from_3d_Tru', _attributesSynonyms, extract_2d_from_3d_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('extract_2daxi_from_3d_Tru', _infoAttr, extract_2d_from_3d_Tru._infoAttr)
  _optionals.update(extract_2d_from_3d_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('extract_2daxi_from_3d_Tru', _helpDict, extract_2d_from_3d_Tru._helpDict)
##### end extract_2daxi_from_3d_Tru class

class extruder_en3_Tru(extruder_Tru): #####
  r"""
  Class to create a 3D tetrahedral/hexahedral mesh (a prism is cut in 3) from a 2D triangular/quadrangular 
  mesh.
  The names of the boundaries (by default, devant (front) and derriere (back)) may 
  be edited by the keyword nom_cl_devant and nom_cl_derriere.
  If NULL is written for nom_cl, then no boundary condition is generated at this place.
  
  Recommendation : to ensure conformity between meshes (in case of fluid/solid coupling) 
  it is recommended to extrude all the domains at the same time.
  """
  # From: line 985, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('domaine', 'listchaine_Tru'),   #
    ('nom_cl_devant', 'chaine_Tru'),   #
    ('nom_cl_derriere', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'domaine': ['domain_name']
  }
  _optionals = set(['nom_cl_devant', 'nom_cl_derriere'])
  _infoMain = ['TRAD2_example', 985]
  _infoAttr = {'domaine': ['TRAD2_example', 986],
   'nom_cl_derriere': ['TRAD2_example', 988],
   'nom_cl_devant': ['TRAD2_example', 987]}
  _attributesList = check_append_list('extruder_en3_Tru', _attributesList, extruder_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('extruder_en3_Tru', _attributesSynonyms, extruder_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('extruder_en3_Tru', _infoAttr, extruder_Tru._infoAttr)
  _optionals.update(extruder_Tru._optionals)
  _helpDict = {
    'domaine':
          ('[listchaine] List of the domains', ''),
    'nom_cl_devant':
          ('[chaine] New name of the first boundary.', ''),
    'nom_cl_derriere':
          ('[chaine] New name of the second boundary.', ''),
  }
  _helpDict = check_append_dict('extruder_en3_Tru', _helpDict, extruder_Tru._helpDict)
##### end extruder_en3_Tru class

class extruder_en20_Tru(interprete_Tru): #####
  r"""
  It does the same task as Extruder except that a prism is cut into 20 tetraedra instead 
  of 3.
  The name of the boundaries will be devant (front) and derriere (back).
  But you can change these names with the keyword RegroupeBord.
  """
  # From: line 989, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('domaine', 'ref_domaine_Tru'),   #
    ('direction', 'troisf_Tru'),   #
    ('nb_tranches', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'domaine': ['domain_name']
  }
  _optionals = set(['direction'])
  _infoMain = ['TRAD2_example', 989]
  _infoAttr = {'direction': ['TRAD2_example', 991],
   'domaine': ['TRAD2_example', 990],
   'nb_tranches': ['TRAD2_example', 992]}
  _attributesList = check_append_list('extruder_en20_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('extruder_en20_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('extruder_en20_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domaine':
          ('[ref_domaine] Name of the domain.', ''),
    'direction':
          ('[troisf] 0 Direction of the extrude operation.', ''),
    'nb_tranches':
          ('[entier] Number of elements in the extrusion direction.', ''),
  }
  _helpDict = check_append_dict('extruder_en20_Tru', _helpDict, interprete_Tru._helpDict)
##### end extruder_en20_Tru class

class extrudeparoi_Tru(interprete_Tru): #####
  r"""
  Keyword dedicated in 3D (VEF) to create prismatic layer at wall.
  Each prism is cut into 3 tetraedra.
  """
  # From: line 993, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('domaine', 'ref_domaine_Tru'),   #
    ('nom_bord', 'chaine_Tru'),   #
    ('epaisseur', 'list_Tru'),   #
    ('critere_absolu', 'entier_3282e0_Tru'),   #
    ('projection_normale_bord', 'rien_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'domaine': ['domain_name']
  }
  _optionals = set(['epaisseur', 'critere_absolu', 'projection_normale_bord'])
  _infoMain = ['TRAD2_example', 993]
  _infoAttr = {'critere_absolu': ['TRAD2_example', 997],
   'domaine': ['TRAD2_example', 994],
   'epaisseur': ['TRAD2_example', 996],
   'nom_bord': ['TRAD2_example', 995],
   'projection_normale_bord': ['TRAD2_example', 998]}
  _attributesList = check_append_list('extrudeparoi_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('extrudeparoi_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('extrudeparoi_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domaine':
          ('[ref_domaine] Name of the domain.', ''),
    'nom_bord':
          ('[chaine] Name of the (no-slip) boundary for creation of prismatic layers.', ''),
    'epaisseur':
          ('[list] n r1 r2 ....\nrn : (relative or absolute) width for each layer.', ''),
    'critere_absolu':
          ('[entier] relative (0, the default) or absolute (1) width for each layer.', ''),
    'projection_normale_bord':
          ('[rien] keyword to project layers on the same plane that contiguous boundaries.\n'
           'defaut values are : epaisseur_relative 1 0.5 projection_normale_bord 1',
           ''),
  }
  _helpDict = check_append_dict('extrudeparoi_Tru', _helpDict, interprete_Tru._helpDict)
##### end extrudeparoi_Tru class

class frontiere_ouverte_fraction_massique_imposee_Tru(condlim_base_Tru): #####
  r"""
  not_set
  """
  # From: line 999, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('ch', '<inherited>front_field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 999]
  _infoAttr = {'ch': ['TRAD2_example', 1000]}
  _attributesList = check_append_list('frontiere_ouverte_fraction_massique_imposee_Tru', _attributesList, condlim_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('frontiere_ouverte_fraction_massique_imposee_Tru', _attributesSynonyms, condlim_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('frontiere_ouverte_fraction_massique_imposee_Tru', _infoAttr, condlim_base_Tru._infoAttr)
  _optionals.update(condlim_base_Tru._optionals)
  _helpDict = {
    'ch':
          ('[front_field_base] Boundary field type.', ''),
  }
  _helpDict = check_append_dict('frontiere_ouverte_fraction_massique_imposee_Tru', _helpDict, condlim_base_Tru._helpDict)
##### end frontiere_ouverte_fraction_massique_imposee_Tru class

class paroi_echange_contact_correlation_vdf_Tru(condlim_base_Tru): #####
  r"""
  Class to define a thermohydraulic 1D model which will apply to a boundary of 2D or 
  3D domain.
  
  Warning : For parallel calculation, the only possible partition will be according 
  the axis of the model with the keyword Tranche.
  """
  # From: line 1001, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('dir', 'entier_3282e0_Tru'),   #
    ('tinf', 'floattant_820d8b_Tru'),   #
    ('tsup', 'floattant_820d8b_Tru'),   #
    ('Lambda', 'chaine_Tru'),   #
    ('rho', 'chaine_Tru'),   #
    ('cp', 'floattant_820d8b_Tru'),   #
    ('dt_impr', 'floattant_820d8b_Tru'),   #
    ('mu', 'chaine_Tru'),   #
    ('debit', 'floattant_820d8b_Tru'),   #
    ('dh', 'floattant_820d8b_Tru'),   #
    ('volume', 'chaine_Tru'),   #
    ('nu', 'chaine_Tru'),   #
    ('reprise_correlation', 'rien_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'Lambda': ['lambda_u']
  }
  _optionals = set(['reprise_correlation'])
  _infoMain = ['TRAD2_example', 1001]
  _infoAttr = {'Lambda': ['TRAD2_example', 1005],
   'cp': ['TRAD2_example', 1007],
   'debit': ['TRAD2_example', 1010],
   'dh': ['TRAD2_example', 1011],
   'dir': ['TRAD2_example', 1002],
   'dt_impr': ['TRAD2_example', 1008],
   'mu': ['TRAD2_example', 1009],
   'nu': ['TRAD2_example', 1013],
   'reprise_correlation': ['TRAD2_example', 1014],
   'rho': ['TRAD2_example', 1006],
   'tinf': ['TRAD2_example', 1003],
   'tsup': ['TRAD2_example', 1004],
   'volume': ['TRAD2_example', 1012]}
  _attributesList = check_append_list('paroi_echange_contact_correlation_vdf_Tru', _attributesList, condlim_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('paroi_echange_contact_correlation_vdf_Tru', _attributesSynonyms, condlim_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('paroi_echange_contact_correlation_vdf_Tru', _infoAttr, condlim_base_Tru._infoAttr)
  _optionals.update(condlim_base_Tru._optionals)
  _helpDict = {
    'dir':
          ('[entier] Direction (0 : axis X, 1 : axis Y, 2 : axis Z) of the 1D model.', ''),
    'tinf':
          ('[floattant] Inlet fluid temperature of the 1D model (oC or K).', ''),
    'tsup':
          ('[floattant] Outlet fluid temperature of the 1D model (oC or K).', ''),
    'Lambda':
          ('[chaine] Thermal conductivity of the fluid (W.m-1.K-1).', ''),
    'rho':
          ('[chaine] Mass density of the fluid (kg.m-3) which may be a function of the temperature \nT.', ''),
    'cp':
          ('[floattant] Calorific capacity value at a constant pressure of the fluid (J.kg-1.K-1).', ''),
    'dt_impr':
          ('[floattant] Printing period in name_of_data_file_time.dat files of the 1D model results.', ''),
    'mu':
          ('[chaine] Dynamic viscosity of the fluid (kg.m-1.s-1) which may be a function of thetemperature \n'
           'T.',
           ''),
    'debit':
          ('[floattant] Surface flow rate (kg.s-1.m-2) of the fluid into the channel.', ''),
    'dh':
          ('[floattant] Hydraulic diameter may be a function f(x) with x position along the 1D \n'
           'axis (xinf <= x <= xsup)',
           ''),
    'volume':
          ('[chaine] Exact volume of the 1D domain (m3) which may be a function of the hydraulic \n'
           'diameter (Dh) and the lateral surface (S) of the meshed boundary.',
           ''),
    'nu':
          ('[chaine] Nusselt number which may be a function of the Reynolds number (Re) and the \n'
           'Prandtl number (Pr).',
           ''),
    'reprise_correlation':
          ('[rien] Keyword in the case of a resuming calculation with this correlation.', ''),
  }
  _helpDict = check_append_dict('paroi_echange_contact_correlation_vdf_Tru', _helpDict, condlim_base_Tru._helpDict)
##### end paroi_echange_contact_correlation_vdf_Tru class

class paroi_echange_contact_correlation_vef_Tru(condlim_base_Tru): #####
  r"""
  Class to define a thermohydraulic 1D model which will apply to a boundary of 2D or 
  3D domain.
  
  Warning : For parallel calculation, the only possible partition will be according 
  the axis of the model with the keyword Tranche_geom.
  """
  # From: line 1015, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('dir', 'entier_3282e0_Tru'),   #
    ('tinf', 'floattant_820d8b_Tru'),   #
    ('tsup', 'floattant_820d8b_Tru'),   #
    ('Lambda', 'chaine_Tru'),   #
    ('rho', 'chaine_Tru'),   #
    ('cp', 'floattant_820d8b_Tru'),   #
    ('dt_impr', 'floattant_820d8b_Tru'),   #
    ('mu', 'chaine_Tru'),   #
    ('debit', 'floattant_820d8b_Tru'),   #
    ('dh', 'chaine_Tru'),   #
    ('n', 'entier_3282e0_Tru'),   #
    ('surface', 'chaine_Tru'),   #
    ('nu', 'chaine_Tru'),   #
    ('xinf', 'floattant_820d8b_Tru'),   #
    ('xsup', 'floattant_820d8b_Tru'),   #
    ('emissivite_pour_rayonnement_entre_deux_plaques_quasi_infinies', 'floattant_820d8b_Tru'),   #
    ('reprise_correlation', 'rien_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'Lambda': ['lambda_u']
  }
  _optionals = set(['emissivite_pour_rayonnement_entre_deux_plaques_quasi_infinies',
   'reprise_correlation'])
  _infoMain = ['TRAD2_example', 1015]
  _infoAttr = {'Lambda': ['TRAD2_example', 1019],
   'cp': ['TRAD2_example', 1021],
   'debit': ['TRAD2_example', 1024],
   'dh': ['TRAD2_example', 1025],
   'dir': ['TRAD2_example', 1016],
   'dt_impr': ['TRAD2_example', 1022],
   'emissivite_pour_rayonnement_entre_deux_plaques_quasi_infinies': ['TRAD2_example',
                                                                     1031],
   'mu': ['TRAD2_example', 1023],
   'n': ['TRAD2_example', 1026],
   'nu': ['TRAD2_example', 1028],
   'reprise_correlation': ['TRAD2_example', 1032],
   'rho': ['TRAD2_example', 1020],
   'surface': ['TRAD2_example', 1027],
   'tinf': ['TRAD2_example', 1017],
   'tsup': ['TRAD2_example', 1018],
   'xinf': ['TRAD2_example', 1029],
   'xsup': ['TRAD2_example', 1030]}
  _attributesList = check_append_list('paroi_echange_contact_correlation_vef_Tru', _attributesList, condlim_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('paroi_echange_contact_correlation_vef_Tru', _attributesSynonyms, condlim_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('paroi_echange_contact_correlation_vef_Tru', _infoAttr, condlim_base_Tru._infoAttr)
  _optionals.update(condlim_base_Tru._optionals)
  _helpDict = {
    'dir':
          ('[entier] Direction (0 : axis X, 1 : axis Y, 2 : axis Z) of the 1D model.', ''),
    'tinf':
          ('[floattant] Inlet fluid temperature of the 1D model (oC or K).', ''),
    'tsup':
          ('[floattant] Outlet fluid temperature of the 1D model (oC or K).', ''),
    'Lambda':
          ('[chaine] Thermal conductivity of the fluid (W.m-1.K-1).', ''),
    'rho':
          ('[chaine] Mass density of the fluid (kg.m-3) which may be a function of the temperature \nT.', ''),
    'cp':
          ('[floattant] Calorific capacity value at a constant pressure of the fluid (J.kg-1.K-1).', ''),
    'dt_impr':
          ('[floattant] Printing period in name_of_data_file_time.dat files of the 1D model results.', ''),
    'mu':
          ('[chaine] Dynamic viscosity of the fluid (kg.m-1.s-1) which may be a function of thetemperature \n'
           'T.',
           ''),
    'debit':
          ('[floattant] Surface flow rate (kg.s-1.m-2) of the fluid into the channel.', ''),
    'dh':
          ('[chaine] Hydraulic diameter may be a function f(x) with x position along the 1D axis \n'
           '(xinf <= x <= xsup)',
           ''),
    'n':
          ('[entier] Number of 1D cells of the 1D mesh.', ''),
    'surface':
          ('[chaine] Section surface of the channel which may be function f(Dh,x) of the hydraulic \n'
           'diameter (Dh) and x position along the 1D axis (xinf <= x <= xsup)',
           ''),
    'nu':
          ('[chaine] Nusselt number which may be a function of the Reynolds number (Re) and the \n'
           'Prandtl number (Pr).',
           ''),
    'xinf':
          ('[floattant] Position of the inlet of the 1D mesh on the axis direction.', ''),
    'xsup':
          ('[floattant] Position of the outlet of the 1D mesh on the axis direction.', ''),
    'emissivite_pour_rayonnement_entre_deux_plaques_quasi_infinies':
          ('[floattant] Coefficient of emissivity for radiation between two quasi infinite plates.', ''),
    'reprise_correlation':
          ('[rien] Keyword in the case of a resuming calculation with this correlation.', ''),
  }
  _helpDict = check_append_dict('paroi_echange_contact_correlation_vef_Tru', _helpDict, condlim_base_Tru._helpDict)
##### end paroi_echange_contact_correlation_vef_Tru class

class spec_pdcr_base_Tru(objet_lecture_Tru): #####
  r"""
  Class to read the source term modelling the presence of a bundle of tubes in a flow.
  Cf=A Re-B.
  """
  # From: line 1033, in file 'TRAD2_example'
  _braces = 0
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = [
    ('ch_a', 'chaine_ba189d_Tru'),   #
    ('a', 'floattant_820d8b_Tru'),   #
    ('ch_b', 'chaine_7956cf_Tru'),   #
    ('b', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['ch_b', 'b'])
  _infoMain = ['TRAD2_example', 1033]
  _infoAttr = {'a': ['TRAD2_example', 1035],
   'b': ['TRAD2_example', 1037],
   'ch_a': ['TRAD2_example', 1034],
   'ch_b': ['TRAD2_example', 1036]}
  _attributesList = check_append_list('spec_pdcr_base_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('spec_pdcr_base_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('spec_pdcr_base_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'ch_a':
          ('[chaine(into=["a","cf"])] Keyword to be used to set law coefficient values for the \n'
           'coefficient of regular pressure losses.',
           ''),
    'a':
          ('[floattant] Value of a law coefficient for regular pressure losses.', ''),
    'ch_b':
          ('[chaine(into=["b"])] Keyword to be used to set law coefficient values for regular \n'
           'pressure losses.',
           ''),
    'b':
          ('[floattant] Value of a law coefficient for regular pressure losses.', ''),
  }
  _helpDict = check_append_dict('spec_pdcr_base_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end spec_pdcr_base_Tru class

class longitudinale_Tru(spec_pdcr_base_Tru): #####
  r"""
  Class to define the pressure loss in the direction of the tube bundle.
  """
  # From: line 1038, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('dir', 'chaine_ad85af_Tru'),   #
    ('dd', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1038]
  _infoAttr = {'dd': ['TRAD2_example', 1040], 'dir': ['TRAD2_example', 1039]}
  _attributesList = check_append_list('longitudinale_Tru', _attributesList, spec_pdcr_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('longitudinale_Tru', _attributesSynonyms, spec_pdcr_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('longitudinale_Tru', _infoAttr, spec_pdcr_base_Tru._infoAttr)
  _optionals.update(spec_pdcr_base_Tru._optionals)
  _helpDict = {
    'dir':
          ('[chaine(into=["x","y","z"])] Direction.', ''),
    'dd':
          ('[floattant] Tube bundle hydraulic diameter value.\nThis value is expressed in m.', ''),
  }
  _helpDict = check_append_dict('longitudinale_Tru', _helpDict, spec_pdcr_base_Tru._helpDict)
##### end longitudinale_Tru class

class transversale_Tru(spec_pdcr_base_Tru): #####
  r"""
  Class to define the pressure loss in the direction perpendicular to the tube bundle.
  """
  # From: line 1041, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('dir', 'chaine_ad85af_Tru'),   #
    ('dd', 'floattant_820d8b_Tru'),   #
    ('chaine_d', 'chaine_437a7b_Tru'),   #
    ('d', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1041]
  _infoAttr = {'chaine_d': ['TRAD2_example', 1044],
   'd': ['TRAD2_example', 1045],
   'dd': ['TRAD2_example', 1043],
   'dir': ['TRAD2_example', 1042]}
  _attributesList = check_append_list('transversale_Tru', _attributesList, spec_pdcr_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('transversale_Tru', _attributesSynonyms, spec_pdcr_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('transversale_Tru', _infoAttr, spec_pdcr_base_Tru._infoAttr)
  _optionals.update(spec_pdcr_base_Tru._optionals)
  _helpDict = {
    'dir':
          ('[chaine(into=["x","y","z"])] Direction.', ''),
    'dd':
          ('[floattant] Value of the tube bundle step.', ''),
    'chaine_d':
          ('[chaine(into=["d"])] Keyword to be used to set the value of the tube external diameter.', ''),
    'd':
          ('[floattant] Value of the tube external diameter.', ''),
  }
  _helpDict = check_append_dict('transversale_Tru', _helpDict, spec_pdcr_base_Tru._helpDict)
##### end transversale_Tru class

class perte_charge_reguliere_Tru(source_base_Tru): #####
  r"""
  Source term modelling the presence of a bundle of tubes in a flow.
  """
  # From: line 1046, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('spec', '<inherited>spec_pdcr_base_Tru'),   #
    ('zone_name', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'zone_name': ['name_of_zone']
  }
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1046]
  _infoAttr = {'spec': ['TRAD2_example', 1047], 'zone_name': ['TRAD2_example', 1048]}
  _attributesList = check_append_list('perte_charge_reguliere_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('perte_charge_reguliere_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('perte_charge_reguliere_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'spec':
          ('[spec_pdcr_base] Description of longitudinale or transversale type.', ''),
    'zone_name':
          ('[chaine] Name of the sub-area occupied by the tube bundle.\n'
           'A Sous_Zone (Sub-area) type object called zone_name should have been previously created.',
           ''),
  }
  _helpDict = check_append_dict('perte_charge_reguliere_Tru', _helpDict, source_base_Tru._helpDict)
##### end perte_charge_reguliere_Tru class

class pb_base_Tru(pb_gen_base_Tru): #####
  r"""
  Resolution of equations on a domain.
  A problem is defined by creating an object and assigning the problem type that the 
  user wishes to resolve.
  To enter values for the problem objects created, the Lire (Read) interpretor is used 
  with a data block.
  """
  # From: line 1569, in file 'TRAD2_example'
  _braces = -3
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = [
    ('milieu', '<inherited>milieu_base_Tru'),   #
    ('constituant', 'constituant_Tru'),   #
    ('postraitement', '<inherited>corps_postraitement_Tru'),   #
    ('postraitements', 'postraitements_Tru'),   #
    ('liste_de_postraitements', 'liste_post_ok_Tru'),   #
    ('liste_postraitements', 'liste_post_Tru'),   #
    ('sauvegarde', 'format_file_Tru'),   #
    ('sauvegarde_simple', 'format_file_Tru'),   #
    ('reprise', 'format_file_Tru'),   #
    ('resume_last_time', 'format_file_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'postraitement': ['post_processing'], 'postraitements': ['post_processings']
  }
  _optionals = set(['milieu',
   'constituant',
   'postraitement',
   'postraitements',
   'liste_de_postraitements',
   'liste_postraitements',
   'sauvegarde',
   'sauvegarde_simple',
   'reprise',
   'resume_last_time'])
  _infoMain = ['TRAD2_example', 1569]
  _infoAttr = {'constituant': ['TRAD2_example', 1571],
   'liste_de_postraitements': ['TRAD2_example', 1574],
   'liste_postraitements': ['TRAD2_example', 1575],
   'milieu': ['TRAD2_example', 1570],
   'postraitement': ['TRAD2_example', 1572],
   'postraitements': ['TRAD2_example', 1573],
   'reprise': ['TRAD2_example', 1578],
   'resume_last_time': ['TRAD2_example', 1579],
   'sauvegarde': ['TRAD2_example', 1576],
   'sauvegarde_simple': ['TRAD2_example', 1577]}
  _attributesList = check_append_list('pb_base_Tru', _attributesList, pb_gen_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_base_Tru', _attributesSynonyms, pb_gen_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_base_Tru', _infoAttr, pb_gen_base_Tru._infoAttr)
  _optionals.update(pb_gen_base_Tru._optionals)
  _helpDict = {
    'milieu':
          ('[milieu_base] The medium associated with the problem.', ''),
    'constituant':
          ('[constituant] Constituent.', ''),
    'postraitement':
          ('[corps_postraitement] One post-processing (without name).', ''),
    'postraitements':
          ('[postraitements] List of Postraitement objects (with name).', ''),
    'liste_de_postraitements':
          ('[liste_post_ok] This', ''),
    'liste_postraitements':
          ('[liste_post] This block defines the output files to be written during the computation.\n'
           'The output format is lata in order to use OpenDX to draw the results.\n'
           'This block can be divided in one or several sub-blocks that can be written at different \n'
           'frequencies and in different directories.\n'
           'Attention.\n'
           'The directory lata used in this example should be created before running the computation \n'
           'or the lata files will be lost.',
           ''),
    'sauvegarde':
          ('[format_file] Keyword used when calculation results are to be backed up.\n'
           'When a coupling is performed, the backup-recovery file name must be well specified \n'
           'for each problem.\n'
           'In this case, you must save to different files and correctly specify these files \n'
           'when resuming the calculation.',
           ''),
    'sauvegarde_simple':
          ('[format_file] The same keyword than Sauvegarde except, the last time step only is \nsaved.', ''),
    'reprise':
          ('[format_file] Keyword to resume a calculation based on the name_file file (see the \n'
           'class format_file).\n'
           'If format_reprise is xyz, the name_file file should be the .xyz file created by the \n'
           'previous calculation.\n'
           'With this file, it is possible to resume a parallel calculation on P processors, \n'
           'whereas the previous calculation has been run on N (N<>P) processors.\n'
           'Should the calculation be resumed, values for the tinit (see schema_temps_base) time \n'
           'fields are taken from the name_file file.\n'
           'If there is no backup corresponding to this time in the name_file, TRUST exits in \n'
           'error.',
           ''),
    'resume_last_time':
          ('[format_file] Keyword to resume a calculation based on the name_file file, resume \n'
           'the calculation at the last time found in the file (tinit is set to last time of saved \n'
           'files).',
           ''),
  }
  _helpDict = check_append_dict('pb_base_Tru', _helpDict, pb_gen_base_Tru._helpDict)
##### end pb_base_Tru class

class pb_post_Tru(pb_base_Tru): #####
  r"""
  not_set
  """
  # From: line 1049, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1049]
  _infoAttr = {}
  _attributesList = check_append_list('pb_post_Tru', _attributesList, pb_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_post_Tru', _attributesSynonyms, pb_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_post_Tru', _infoAttr, pb_base_Tru._infoAttr)
  _optionals.update(pb_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('pb_post_Tru', _helpDict, pb_base_Tru._helpDict)
##### end pb_post_Tru class

class info_med_Tru(objet_lecture_Tru): #####
  r"""
  not_set
  """
  # From: line 1050, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('file_med', 'chaine_Tru'),   #
    ('domaine', 'chaine_Tru'),   #
    ('pb_post', '<inherited>pb_post_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1050]
  _infoAttr = {'domaine': ['TRAD2_example', 1052],
   'file_med': ['TRAD2_example', 1051],
   'pb_post': ['TRAD2_example', 1053]}
  _attributesList = check_append_list('info_med_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('info_med_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('info_med_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'file_med':
          ('[chaine] Name of the MED file.', ''),
    'domaine':
          ('[chaine] Name of domain.', ''),
    'pb_post':
          ('[pb_post] not_set', ''),
  }
  _helpDict = check_append_dict('info_med_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end info_med_Tru class

class list_info_med_Tru(ListOfBase_Tru): #####
  """
  not_set
  """
  # From: line 1054, in file 'TRAD2_example'
  _braces = -1
  _comma = 1
  _allowedClasses = ["info_med_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 1054]
##### end list_info_med_Tru class

class pbc_med_Tru(pb_gen_base_Tru): #####
  r"""
  Allows to read med files and post-process them.
  """
  # From: line 1055, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('list_info_med', 'list_info_med_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1055]
  _infoAttr = {'list_info_med': ['TRAD2_example', 1056]}
  _attributesList = check_append_list('pbc_med_Tru', _attributesList, pb_gen_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pbc_med_Tru', _attributesSynonyms, pb_gen_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pbc_med_Tru', _infoAttr, pb_gen_base_Tru._infoAttr)
  _optionals.update(pb_gen_base_Tru._optionals)
  _helpDict = {
    'list_info_med':
          ('[list_info_med] not_set', ''),
  }
  _helpDict = check_append_dict('pbc_med_Tru', _helpDict, pb_gen_base_Tru._helpDict)
##### end pbc_med_Tru class

class decoupebord_pour_rayonnement_Tru(interprete_Tru): #####
  r"""
  To subdivide the external boundary of a domain into several parts (may be useful for 
  better accuracy when using radiation model in transparent medium).
  To specify the boundaries of the fine_domain_name domain to be splitted.
  These boundaries will be cut according the coarse mesh defined by either the keyword 
  domaine_grossier (each boundary face of the coarse mesh coarse_domain_name will be 
  used to group boundary faces of the fine mesh to define a new boundary), either by 
  the keyword nb_parts_naif (each boundary of the fine mesh is splitted into a partition 
  with nx*ny*nz elements), either by a geometric condition given by a formulae with 
  the keyword condition_geometrique.
  If used, the coarse_domain_name domain should have the same boundaries name of the 
  fine_domain_name domain.
  
  A mesh file (ASCII format, except if binaire option is specified) named by default 
  newgeom (or specified by the nom_fichier_sortie keyword) will be created and will 
  contain the fine_domain_name domain with the splitted boundaries named boundary_name%I 
  (where I is between from 0 and n-1).
  Furthermore, several files named boundary_name%I and boundary_name_xv will be created, 
  containing the definition of the subdived boundaries.
  newgeom will be used to calculate view factors with geom2ansys script whereas only 
  the boundary_name_xv files will be necessary for the radiation calculation.
  The file listb will contain the list of the boundaries boundary_name%I.
  """
  # From: line 1057, in file 'TRAD2_example'
  _braces = 1
  _synonyms = ['decoupebord_Tru']
  _attributesList = [
    ('domaine', 'ref_domaine_Tru'),   #
    ('domaine_grossier', 'ref_domaine_Tru'),   #
    ('nb_parts_naif', 'listentier_Tru'),   #
    ('nb_parts_geom', 'listentier_Tru'),   #
    ('bords_a_decouper', 'listchaine_Tru'),   #
    ('nom_fichier_sortie', 'chaine_Tru'),   #
    ('condition_geometrique', 'listchaine_Tru'),   #
    ('binaire', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['domaine_grossier',
   'nb_parts_naif',
   'nb_parts_geom',
   'nom_fichier_sortie',
   'condition_geometrique',
   'binaire'])
  _infoMain = ['TRAD2_example', 1057]
  _infoAttr = {'binaire': ['TRAD2_example', 1065],
   'bords_a_decouper': ['TRAD2_example', 1062],
   'condition_geometrique': ['TRAD2_example', 1064],
   'domaine': ['TRAD2_example', 1058],
   'domaine_grossier': ['TRAD2_example', 1059],
   'nb_parts_geom': ['TRAD2_example', 1061],
   'nb_parts_naif': ['TRAD2_example', 1060],
   'nom_fichier_sortie': ['TRAD2_example', 1063]}
  _attributesList = check_append_list('decoupebord_pour_rayonnement_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('decoupebord_pour_rayonnement_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('decoupebord_pour_rayonnement_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domaine':
          ('[ref_domaine] not_set', ''),
    'domaine_grossier':
          ('[ref_domaine] not_set', ''),
    'nb_parts_naif':
          ('[listentier] not_set', ''),
    'nb_parts_geom':
          ('[listentier] not_set', ''),
    'bords_a_decouper':
          ('[listchaine] not_set', ''),
    'nom_fichier_sortie':
          ('[chaine] not_set', ''),
    'condition_geometrique':
          ('[listchaine] not_set', ''),
    'binaire':
          ('[entier] not_set', ''),
  }
  _helpDict = check_append_dict('decoupebord_pour_rayonnement_Tru', _helpDict, interprete_Tru._helpDict)
##### end decoupebord_pour_rayonnement_Tru class

class methode_transport_deriv_Tru(objet_lecture_Tru): #####
  r"""
  Basic class for method of transport of interface.
  """
  # From: line 1066, in file 'TRAD2_example'
  _braces = 0
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1066]
  _infoAttr = {}
  _attributesList = check_append_list('methode_transport_deriv_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('methode_transport_deriv_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('methode_transport_deriv_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('methode_transport_deriv_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end methode_transport_deriv_Tru class

class loi_horaire_Tru(ConstrainBase_Tru): #####
  r"""
  to define the movement with a time-dependant law for the solid interface.
  """
  # From: line 1067, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('position', 'listchaine_Tru'),   #
    ('vitesse', 'listchaine_Tru'),   #
    ('rotation', 'listchaine_Tru'),   #
    ('derivee_rotation', 'listchaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['rotation', 'derivee_rotation'])
  _infoMain = ['TRAD2_example', 1067]
  _infoAttr = {'derivee_rotation': ['TRAD2_example', 1071],
   'position': ['TRAD2_example', 1068],
   'rotation': ['TRAD2_example', 1070],
   'vitesse': ['TRAD2_example', 1069]}
  _helpDict = {
    'position':
          ('[listchaine] not_set', ''),
    'vitesse':
          ('[listchaine] not_set', ''),
    'rotation':
          ('[listchaine] not_set', ''),
    'derivee_rotation':
          ('[listchaine] not_set', ''),
  }
##### end loi_horaire_Tru class

class methode_loi_horaire_Tru(methode_transport_deriv_Tru): #####
  r"""
  not_set
  """
  # From: line 1072, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['loi_horaire_Tru']
  _attributesList = [
    ('nom_loi', 'ref_loi_horaire_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1072]
  _infoAttr = {'nom_loi': ['TRAD2_example', 1073]}
  _attributesList = check_append_list('methode_loi_horaire_Tru', _attributesList, methode_transport_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('methode_loi_horaire_Tru', _attributesSynonyms, methode_transport_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('methode_loi_horaire_Tru', _infoAttr, methode_transport_deriv_Tru._infoAttr)
  _optionals.update(methode_transport_deriv_Tru._optionals)
  _helpDict = {
    'nom_loi':
          ('[ref_loi_horaire] not_set', ''),
  }
  _helpDict = check_append_dict('methode_loi_horaire_Tru', _helpDict, methode_transport_deriv_Tru._helpDict)
##### end methode_loi_horaire_Tru class

class problem_read_generic_Tru(pb_base_Tru): #####
  r"""
  The probleme_read_generic differs rom the rest of the TRUST code : The problem does 
  not state the number of equations that are enclosed in the problem.
  As the list of equations to be solved in the generic read problem is declared in 
  the data file and not pre-defined in the structure of the problem, each equation has 
  to be distinctively associated with the problem with the Associate keyword.
  """
  # From: line 1074, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1074]
  _infoAttr = {}
  _attributesList = check_append_list('problem_read_generic_Tru', _attributesList, pb_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('problem_read_generic_Tru', _attributesSynonyms, pb_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('problem_read_generic_Tru', _infoAttr, pb_base_Tru._infoAttr)
  _optionals.update(pb_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('problem_read_generic_Tru', _helpDict, pb_base_Tru._helpDict)
##### end problem_read_generic_Tru class

class temperature_Tru(traitement_particulier_base_Tru): #####
  r"""
  not_set
  """
  # From: line 1075, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('bord', 'chaine_Tru'),   #
    ('direction', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1075]
  _infoAttr = {'bord': ['TRAD2_example', 1076], 'direction': ['TRAD2_example', 1077]}
  _attributesList = check_append_list('temperature_Tru', _attributesList, traitement_particulier_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('temperature_Tru', _attributesSynonyms, traitement_particulier_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('temperature_Tru', _infoAttr, traitement_particulier_base_Tru._infoAttr)
  _optionals.update(traitement_particulier_base_Tru._optionals)
  _helpDict = {
    'bord':
          ('[chaine] not_set', ''),
    'direction':
          ('[entier] not_set', ''),
  }
  _helpDict = check_append_dict('temperature_Tru', _helpDict, traitement_particulier_base_Tru._helpDict)
##### end temperature_Tru class

class canal_Tru(traitement_particulier_base_Tru): #####
  r"""
  Keyword for statistics on a periodic plane channel.
  """
  # From: line 1078, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('dt_impr_moy_spat', 'floattant_820d8b_Tru'),   #
    ('dt_impr_moy_temp', 'floattant_820d8b_Tru'),   #
    ('debut_stat', 'floattant_820d8b_Tru'),   #
    ('fin_stat', 'floattant_820d8b_Tru'),   #
    ('pulsation_w', 'floattant_820d8b_Tru'),   #
    ('nb_points_par_phase', 'entier_3282e0_Tru'),   #
    ('reprise', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['dt_impr_moy_spat',
   'dt_impr_moy_temp',
   'debut_stat',
   'fin_stat',
   'pulsation_w',
   'nb_points_par_phase',
   'reprise'])
  _infoMain = ['TRAD2_example', 1078]
  _infoAttr = {'debut_stat': ['TRAD2_example', 1081],
   'dt_impr_moy_spat': ['TRAD2_example', 1079],
   'dt_impr_moy_temp': ['TRAD2_example', 1080],
   'fin_stat': ['TRAD2_example', 1082],
   'nb_points_par_phase': ['TRAD2_example', 1084],
   'pulsation_w': ['TRAD2_example', 1083],
   'reprise': ['TRAD2_example', 1085]}
  _attributesList = check_append_list('canal_Tru', _attributesList, traitement_particulier_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('canal_Tru', _attributesSynonyms, traitement_particulier_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('canal_Tru', _infoAttr, traitement_particulier_base_Tru._infoAttr)
  _optionals.update(traitement_particulier_base_Tru._optionals)
  _helpDict = {
    'dt_impr_moy_spat':
          ('[floattant] Period to print the spatial average (default value is 1e6).', ''),
    'dt_impr_moy_temp':
          ('[floattant] Period to print the temporal average (default value is 1e6).', ''),
    'debut_stat':
          ('[floattant] Time to start the temporal averaging (default value is 1e6).', ''),
    'fin_stat':
          ('[floattant] Time to end the temporal averaging (default value is 1e6).', ''),
    'pulsation_w':
          ('[floattant] Pulsation for phase averaging (in case of pulsating forcing term) (no \n'
           'default value).',
           ''),
    'nb_points_par_phase':
          ('[entier] Number of samples to represent phase average all along a period (no default \nvalue).',
           ''),
    'reprise':
          ('[chaine] val_moy_temp_xxxxxx.sauv : Keyword to resume a calculation with previous \n'
           'averaged quantities.\n'
           '\n'
           'Note that for thermal and turbulent problems, averages on temperature and turbulent \n'
           'viscosity are automatically calculated.\n'
           'To resume a calculation with phase averaging, val_moy_temp_xxxxxx.sauv_phase file \n'
           'is required on the directory where the job is submitted (this last file will be then \n'
           'automatically loaded by TRUST).',
           ''),
  }
  _helpDict = check_append_dict('canal_Tru', _helpDict, traitement_particulier_base_Tru._helpDict)
##### end canal_Tru class

class ec_Tru(traitement_particulier_base_Tru): #####
  r"""
  Keyword to print total kinetic energy into the referential linked to the domain (keyword 
  Ec).
  In the case where the domain is moving into a Galilean referential, the keyword Ec_dans_repere_fixe 
  will print total kinetic energy in the Galilean referential whereas Ec will print 
  the value calculated into the moving referential linked to the domain
  """
  # From: line 1086, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('ec', 'rien_Tru'),   #
    ('ec_dans_repere_fixe', 'rien_Tru'),   #
    ('periode', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['ec', 'ec_dans_repere_fixe', 'periode'])
  _infoMain = ['TRAD2_example', 1086]
  _infoAttr = {'ec': ['TRAD2_example', 1087],
   'ec_dans_repere_fixe': ['TRAD2_example', 1088],
   'periode': ['TRAD2_example', 1089]}
  _attributesList = check_append_list('ec_Tru', _attributesList, traitement_particulier_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('ec_Tru', _attributesSynonyms, traitement_particulier_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('ec_Tru', _infoAttr, traitement_particulier_base_Tru._infoAttr)
  _optionals.update(traitement_particulier_base_Tru._optionals)
  _helpDict = {
    'ec':
          ('[rien] not_set', ''),
    'ec_dans_repere_fixe':
          ('[rien] not_set', ''),
    'periode':
          ('[floattant] periode is the keyword to set the period of printing into the file datafile_Ec.son \n'
           'or datafile_Ec_dans_repere_fixe.son.',
           ''),
  }
  _helpDict = check_append_dict('ec_Tru', _helpDict, traitement_particulier_base_Tru._helpDict)
##### end ec_Tru class

class thi_Tru(traitement_particulier_base_Tru): #####
  r"""
  Keyword for a THI (Homogeneous Isotropic Turbulence) calculation.
  """
  # From: line 1090, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('init_ec', 'entier_3282e0_Tru'),   #
    ('val_ec', 'floattant_820d8b_Tru'),   #
    ('facon_init', 'entier_b08690_Tru'),   #
    ('calc_spectre', 'entier_b08690_Tru'),   #
    ('periode_calc_spectre', 'floattant_820d8b_Tru'),   #
    ('spectre_3d', 'entier_b08690_Tru'),   #
    ('spectre_1d', 'entier_b08690_Tru'),   #
    ('conservation_ec', 'rien_Tru'),   #
    ('longueur_boite', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'spectre_1d': ['flag1d'], 'spectre_3d': ['flag3d']
  }
  _optionals = set(['val_ec',
   'facon_init',
   'calc_spectre',
   'periode_calc_spectre',
   'spectre_3d',
   'spectre_1d',
   'conservation_ec',
   'longueur_boite'])
  _infoMain = ['TRAD2_example', 1090]
  _infoAttr = {'calc_spectre': ['TRAD2_example', 1094],
   'conservation_ec': ['TRAD2_example', 1098],
   'facon_init': ['TRAD2_example', 1093],
   'init_ec': ['TRAD2_example', 1091],
   'longueur_boite': ['TRAD2_example', 1099],
   'periode_calc_spectre': ['TRAD2_example', 1095],
   'spectre_1d': ['TRAD2_example', 1097],
   'spectre_3d': ['TRAD2_example', 1096],
   'val_ec': ['TRAD2_example', 1092]}
  _attributesList = check_append_list('thi_Tru', _attributesList, traitement_particulier_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('thi_Tru', _attributesSynonyms, traitement_particulier_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('thi_Tru', _infoAttr, traitement_particulier_base_Tru._infoAttr)
  _optionals.update(traitement_particulier_base_Tru._optionals)
  _helpDict = {
    'init_ec':
          ('[entier] Keyword to renormalize initial velocity so that kinetic energy equals to \n'
           'the value given by keyword val_Ec.',
           ''),
    'val_ec':
          ('[floattant] Keyword to impose a value for kinetic energy by velocity renormalizated \n'
           'if init_Ec value is 1.',
           ''),
    'facon_init':
          ('[entier(into=["0","1"])] Keyword to specify how kinetic energy is computed (0 or 1).', ''),
    'calc_spectre':
          ('[entier(into=["0","1"])] Calculate or not the spectrum of kinetic energy.\n'
           '\n'
           'Files called Sorties_THI are written with inside four columns :\n'
           '\n'
           'time:t global_kinetic_energy:Ec enstrophy:D skewness:S\n'
           '\n'
           'If calc_spectre is set to 1, a file Sorties_THI2_2 is written with three columns \n'
           ':\n'
           '\n'
           'time:t kinetic_energy_at_kc=32 enstrophy_at_kc=32\n'
           '\n'
           'If calc_spectre is set to 1, a file spectre_xxxxx is written with two columns at \n'
           'each time xxxxx :\n'
           '\n'
           'frequency:k energy:E(k).',
           ''),
    'periode_calc_spectre':
          ('[floattant] Period for calculating spectrum of kinetic energy', ''),
    'spectre_3d':
          ('[entier(into=["0","1"])] Calculate or not the 3D spectrum', ''),
    'spectre_1d':
          ('[entier(into=["0","1"])] Calculate or not the 1D spectrum', ''),
    'conservation_ec':
          ('[rien] If set to 1, velocity field will be changed as to have a constant kinetic energy \n'
           '(default 0)',
           ''),
    'longueur_boite':
          ('[floattant] Length of the calculation domain', ''),
  }
  _helpDict = check_append_dict('thi_Tru', _helpDict, traitement_particulier_base_Tru._helpDict)
##### end thi_Tru class

class chmoy_faceperio_Tru(traitement_particulier_base_Tru): #####
  r"""
  non documente
  """
  # From: line 1100, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('bloc', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1100]
  _infoAttr = {'bloc': ['TRAD2_example', 1101]}
  _attributesList = check_append_list('chmoy_faceperio_Tru', _attributesList, traitement_particulier_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('chmoy_faceperio_Tru', _attributesSynonyms, traitement_particulier_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('chmoy_faceperio_Tru', _infoAttr, traitement_particulier_base_Tru._infoAttr)
  _optionals.update(traitement_particulier_base_Tru._optionals)
  _helpDict = {
    'bloc':
          ('[bloc_lecture] not_set', ''),
  }
  _helpDict = check_append_dict('chmoy_faceperio_Tru', _helpDict, traitement_particulier_base_Tru._helpDict)
##### end chmoy_faceperio_Tru class

class extraire_plan_Tru(interprete_Tru): #####
  r"""
  This keyword extracts a plane mesh named domain_name (this domain should have been 
  declared before) from the mesh of the pb_name problem.
  The plane can be either a triangle (defined by the keywords Origine, Point1, Point2 
  and Triangle), either a regular quadrangle (with keywords Origine, Point1 and Point2), 
  or either a generalized quadrangle (with keywords Origine, Point1, Point2, Point3).
  The keyword Epaisseur specifies the thickness of volume around the plane which contains 
  the faces of the extracted mesh.
  The keyword via_extraire_surface will create a plan and use Extraire_surface algorithm.
  Inverse_condition_element keyword then will be used in the case where the plane is 
  a boundary not well oriented, and avec_certains_bords_pour_extraire_surface is the 
  option related to the Extraire_surface option named avec_certains_bords.
  """
  # From: line 1102, in file 'TRAD2_example'
  _braces = -3
  _synonyms = []
  _attributesList = [
    ('domaine', 'ref_domaine_Tru'),   #
    ('probleme', 'ref_pb_base_Tru'),   #
    ('epaisseur', 'floattant_820d8b_Tru'),   #
    ('origine', 'list_Tru'),   #
    ('point1', 'list_Tru'),   #
    ('point2', 'list_Tru'),   #
    ('point3', 'list_Tru'),   #
    ('triangle', 'rien_Tru'),   #
    ('via_extraire_surface', 'rien_Tru'),   #
    ('inverse_condition_element', 'rien_Tru'),   #
    ('avec_certains_bords_pour_extraire_surface', 'listchaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['point3',
   'triangle',
   'via_extraire_surface',
   'inverse_condition_element',
   'avec_certains_bords_pour_extraire_surface'])
  _infoMain = ['TRAD2_example', 1102]
  _infoAttr = {'avec_certains_bords_pour_extraire_surface': ['TRAD2_example', 1113],
   'domaine': ['TRAD2_example', 1103],
   'epaisseur': ['TRAD2_example', 1105],
   'inverse_condition_element': ['TRAD2_example', 1112],
   'origine': ['TRAD2_example', 1106],
   'point1': ['TRAD2_example', 1107],
   'point2': ['TRAD2_example', 1108],
   'point3': ['TRAD2_example', 1109],
   'probleme': ['TRAD2_example', 1104],
   'triangle': ['TRAD2_example', 1110],
   'via_extraire_surface': ['TRAD2_example', 1111]}
  _attributesList = check_append_list('extraire_plan_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('extraire_plan_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('extraire_plan_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domaine':
          ('[ref_domaine] domain_namme', ''),
    'probleme':
          ('[ref_pb_base] pb_name', ''),
    'epaisseur':
          ('[floattant] not_set', ''),
    'origine':
          ('[list] not_set', ''),
    'point1':
          ('[list] not_set', ''),
    'point2':
          ('[list] not_set', ''),
    'point3':
          ('[list] not_set', ''),
    'triangle':
          ('[rien] not_set', ''),
    'via_extraire_surface':
          ('[rien] not_set', ''),
    'inverse_condition_element':
          ('[rien] not_set', ''),
    'avec_certains_bords_pour_extraire_surface':
          ('[listchaine] not_set', ''),
  }
  _helpDict = check_append_dict('extraire_plan_Tru', _helpDict, interprete_Tru._helpDict)
##### end extraire_plan_Tru class

class extraire_domaine_Tru(interprete_Tru): #####
  r"""
  Keyword to create a new domain built with the domain elements of the pb_name problem 
  verifying the two conditions given by Condition_elements.
  The problem pb_name should have been discretized.
  """
  # From: line 1114, in file 'TRAD2_example'
  _braces = -3
  _synonyms = []
  _attributesList = [
    ('domaine', 'ref_domaine_Tru'),   #
    ('probleme', 'ref_pb_base_Tru'),   #
    ('condition_elements', 'chaine_Tru'),   #
    ('sous_zone', 'ref_sous_zone_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['condition_elements', 'sous_zone'])
  _infoMain = ['TRAD2_example', 1114]
  _infoAttr = {'condition_elements': ['TRAD2_example', 1117],
   'domaine': ['TRAD2_example', 1115],
   'probleme': ['TRAD2_example', 1116],
   'sous_zone': ['TRAD2_example', 1118]}
  _attributesList = check_append_list('extraire_domaine_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('extraire_domaine_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('extraire_domaine_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domaine':
          ('[ref_domaine] Domain in which faces are saved', ''),
    'probleme':
          ('[ref_pb_base] Problem from which faces should be extracted', ''),
    'condition_elements':
          ('[chaine] not_set', ''),
    'sous_zone':
          ('[ref_sous_zone] not_set', ''),
  }
  _helpDict = check_append_dict('extraire_domaine_Tru', _helpDict, interprete_Tru._helpDict)
##### end extraire_domaine_Tru class

class extraire_surface_Tru(interprete_Tru): #####
  r"""
  This keyword extracts a surface mesh named domain_name (this domain should have been 
  declared before) from the mesh of the pb_name problem.
  The surface mesh is defined by one or two conditions.
  The first condition is about elements with Condition_elements.
  For example: Condition_elements x*x+y*y+z*z<1
  
  Will define a surface mesh with external faces of the mesh elements inside the sphere 
  of radius 1 located at (0,0,0).
  The second condition Condition_faces is useful to give a restriction.NL2 By default, 
  the faces from the boundaries are not added to the surface mesh excepted if option 
  avec_les_bords is given (all the boundaries are added), or if the option avec_certains_bords 
  is used to add only some boundaries.
  """
  # From: line 1119, in file 'TRAD2_example'
  _braces = -3
  _synonyms = []
  _attributesList = [
    ('domaine', 'ref_domaine_Tru'),   #
    ('probleme', 'ref_pb_base_Tru'),   #
    ('condition_elements', 'chaine_Tru'),   #
    ('condition_faces', 'chaine_Tru'),   #
    ('avec_les_bords', 'rien_Tru'),   #
    ('avec_certains_bords', 'listchaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['condition_elements',
   'condition_faces',
   'avec_les_bords',
   'avec_certains_bords'])
  _infoMain = ['TRAD2_example', 1119]
  _infoAttr = {'avec_certains_bords': ['TRAD2_example', 1125],
   'avec_les_bords': ['TRAD2_example', 1124],
   'condition_elements': ['TRAD2_example', 1122],
   'condition_faces': ['TRAD2_example', 1123],
   'domaine': ['TRAD2_example', 1120],
   'probleme': ['TRAD2_example', 1121]}
  _attributesList = check_append_list('extraire_surface_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('extraire_surface_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('extraire_surface_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domaine':
          ('[ref_domaine] Domain in which faces are saved', ''),
    'probleme':
          ('[ref_pb_base] Problem from which faces should be extracted', ''),
    'condition_elements':
          ('[chaine] not_set', ''),
    'condition_faces':
          ('[chaine] not_set', ''),
    'avec_les_bords':
          ('[rien] not_set', ''),
    'avec_certains_bords':
          ('[listchaine] not_set', ''),
  }
  _helpDict = check_append_dict('extraire_surface_Tru', _helpDict, interprete_Tru._helpDict)
##### end extraire_surface_Tru class

class integrer_champ_med_Tru(interprete_Tru): #####
  r"""
  his keyword is used to calculate a flow rate from a velocity MED field read before.
  The method is either debit_total to calculate the flow rate on the whole surface, 
  either integrale_en_z to calculate flow rates between z=zmin and z=zmax on nb_tranche 
  surfaces.
  The output file indicates first the flow rate for the whole surface and then lists 
  for each tranche : the height z, the surface average value, the surface area and the 
  flow rate.
  For the debit_total method, only one tranche is considered.NL2 file :z Sum(u.dS)/Sum(dS) 
  Sum(dS) Sum(u.dS)
  """
  # From: line 1126, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('champ_med', 'ref_champ_fonc_med_Tru'),   #
    ('methode', 'chaine_5dc32f_Tru'),   #
    ('zmin', 'floattant_820d8b_Tru'),   #
    ('zmax', 'floattant_820d8b_Tru'),   #
    ('nb_tranche', 'entier_3282e0_Tru'),   #
    ('fichier_sortie', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['zmin', 'zmax', 'nb_tranche', 'fichier_sortie'])
  _infoMain = ['TRAD2_example', 1126]
  _infoAttr = {'champ_med': ['TRAD2_example', 1127],
   'fichier_sortie': ['TRAD2_example', 1132],
   'methode': ['TRAD2_example', 1128],
   'nb_tranche': ['TRAD2_example', 1131],
   'zmax': ['TRAD2_example', 1130],
   'zmin': ['TRAD2_example', 1129]}
  _attributesList = check_append_list('integrer_champ_med_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('integrer_champ_med_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('integrer_champ_med_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'champ_med':
          ('[ref_champ_fonc_med] not_set', ''),
    'methode':
          ('[chaine(into=["integrale_en_z","debit_total"])] to choose between the integral following \n'
           'z or over the entire height (debit_total corresponds to zmin=-DMAXFLOAT, ZMax=DMAXFLOAT, \n'
           'nb_tranche=1)',
           ''),
    'zmin':
          ('[floattant] not_set', ''),
    'zmax':
          ('[floattant] not_set', ''),
    'nb_tranche':
          ('[entier] not_set', ''),
    'fichier_sortie':
          ('[chaine] name of the output file, by default: integrale.', ''),
  }
  _helpDict = check_append_dict('integrer_champ_med_Tru', _helpDict, interprete_Tru._helpDict)
##### end integrer_champ_med_Tru class

class convection_amont_Tru(convection_deriv_Tru): #####
  r"""
  Keyword for upwind scheme for VDF or VEF discretizations.
  In VEF discretization equivalent to generic amont for TRUST version 1.5 or later.
  The previous upwind scheme can be used with the obsolete in future amont_old keyword.
  """
  # From: line 1133, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['amont_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1133]
  _infoAttr = {}
  _attributesList = check_append_list('convection_amont_Tru', _attributesList, convection_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_amont_Tru', _attributesSynonyms, convection_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_amont_Tru', _infoAttr, convection_deriv_Tru._infoAttr)
  _optionals.update(convection_deriv_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('convection_amont_Tru', _helpDict, convection_deriv_Tru._helpDict)
##### end convection_amont_Tru class

class convection_amont_old_Tru(convection_deriv_Tru): #####
  r"""
  Only for VEF discretization, obsolete keyword, see amont.
  """
  # From: line 1134, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['amont_old_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1134]
  _infoAttr = {}
  _attributesList = check_append_list('convection_amont_old_Tru', _attributesList, convection_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_amont_old_Tru', _attributesSynonyms, convection_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_amont_old_Tru', _infoAttr, convection_deriv_Tru._infoAttr)
  _optionals.update(convection_deriv_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('convection_amont_old_Tru', _helpDict, convection_deriv_Tru._helpDict)
##### end convection_amont_old_Tru class

class convection_centre_Tru(convection_deriv_Tru): #####
  r"""
  For VDF and VEF discretizations.
  """
  # From: line 1135, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['centre_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1135]
  _infoAttr = {}
  _attributesList = check_append_list('convection_centre_Tru', _attributesList, convection_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_centre_Tru', _attributesSynonyms, convection_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_centre_Tru', _infoAttr, convection_deriv_Tru._infoAttr)
  _optionals.update(convection_deriv_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('convection_centre_Tru', _helpDict, convection_deriv_Tru._helpDict)
##### end convection_centre_Tru class

class convection_centre4_Tru(convection_deriv_Tru): #####
  r"""
  For VDF and VEF discretizations.
  """
  # From: line 1136, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['centre4_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1136]
  _infoAttr = {}
  _attributesList = check_append_list('convection_centre4_Tru', _attributesList, convection_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_centre4_Tru', _attributesSynonyms, convection_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_centre4_Tru', _infoAttr, convection_deriv_Tru._infoAttr)
  _optionals.update(convection_deriv_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('convection_centre4_Tru', _helpDict, convection_deriv_Tru._helpDict)
##### end convection_centre4_Tru class

class convection_centre_old_Tru(convection_deriv_Tru): #####
  r"""
  Only for VEF discretization.
  """
  # From: line 1137, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['centre_old_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1137]
  _infoAttr = {}
  _attributesList = check_append_list('convection_centre_old_Tru', _attributesList, convection_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_centre_old_Tru', _attributesSynonyms, convection_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_centre_old_Tru', _infoAttr, convection_deriv_Tru._infoAttr)
  _optionals.update(convection_deriv_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('convection_centre_old_Tru', _helpDict, convection_deriv_Tru._helpDict)
##### end convection_centre_old_Tru class

class convection_di_l2_Tru(convection_deriv_Tru): #####
  r"""
  Only for VEF discretization.
  """
  # From: line 1138, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['di_l2_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1138]
  _infoAttr = {}
  _attributesList = check_append_list('convection_di_l2_Tru', _attributesList, convection_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_di_l2_Tru', _attributesSynonyms, convection_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_di_l2_Tru', _infoAttr, convection_deriv_Tru._infoAttr)
  _optionals.update(convection_deriv_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('convection_di_l2_Tru', _helpDict, convection_deriv_Tru._helpDict)
##### end convection_di_l2_Tru class

class bloc_ef_Tru(objet_lecture_Tru): #####
  r"""
  not_set
  """
  # From: line 1139, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('mot1', 'chaine_e80920_Tru'),   #
    ('val1', 'entier_8bf71a_Tru'),   #
    ('mot2', 'chaine_e80920_Tru'),   #
    ('val2', 'entier_8bf71a_Tru'),   #
    ('mot3', 'chaine_e80920_Tru'),   #
    ('val3', 'entier_8bf71a_Tru'),   #
    ('mot4', 'chaine_e80920_Tru'),   #
    ('val4', 'entier_8bf71a_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1139]
  _infoAttr = {'mot1': ['TRAD2_example', 1140],
   'mot2': ['TRAD2_example', 1142],
   'mot3': ['TRAD2_example', 1144],
   'mot4': ['TRAD2_example', 1146],
   'val1': ['TRAD2_example', 1141],
   'val2': ['TRAD2_example', 1143],
   'val3': ['TRAD2_example', 1145],
   'val4': ['TRAD2_example', 1147]}
  _attributesList = check_append_list('bloc_ef_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('bloc_ef_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('bloc_ef_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'mot1':
          ('[chaine(into=["transportant_bar","transporte_bar","filtrer_resu","antisym"])] not_set', ''),
    'val1':
          ('[entier(into=[0,1])] not_set', ''),
    'mot2':
          ('[chaine(into=["transportant_bar","transporte_bar","filtrer_resu","antisym"])] not_set', ''),
    'val2':
          ('[entier(into=[0,1])] not_set', ''),
    'mot3':
          ('[chaine(into=["transportant_bar","transporte_bar","filtrer_resu","antisym"])] not_set', ''),
    'val3':
          ('[entier(into=[0,1])] not_set', ''),
    'mot4':
          ('[chaine(into=["transportant_bar","transporte_bar","filtrer_resu","antisym"])] not_set', ''),
    'val4':
          ('[entier(into=[0,1])] not_set', ''),
  }
  _helpDict = check_append_dict('bloc_ef_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end bloc_ef_Tru class

class convection_ef_Tru(convection_deriv_Tru): #####
  r"""
  For VEF calculations, a centred convective scheme based on Finite Elements formulation 
  can be called through the following data:NL2
  
  Convection { EF transportant_bar val transporte_bar val antisym val filtrer_resu 
  val }NL2
  
  This scheme is 2nd order accuracy (and get better the property of kinetic energy 
  conservation).
  Due to possible problems of instabilities phenomena, this scheme has to be coupled 
  with stabilisation process (see Source_Qdm_lambdaup).These two last data are equivalent 
  from a theoretical point of view in variationnal writing to : div(( u.
  grad ub , vb) - (u.
  grad vb, ub)), where vb corresponds to the filtered reference test functions.NL2
  
  Remark:NL2 This class requires to define a filtering operator : see solveur_bar
  """
  # From: line 1148, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['ef_Tru']
  _attributesList = [
    ('mot1', 'chaine_a3dc34_Tru'),   #
    ('bloc_ef', 'bloc_ef_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['mot1', 'bloc_ef'])
  _infoMain = ['TRAD2_example', 1148]
  _infoAttr = {'bloc_ef': ['TRAD2_example', 1150], 'mot1': ['TRAD2_example', 1149]}
  _attributesList = check_append_list('convection_ef_Tru', _attributesList, convection_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_ef_Tru', _attributesSynonyms, convection_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_ef_Tru', _infoAttr, convection_deriv_Tru._infoAttr)
  _optionals.update(convection_deriv_Tru._optionals)
  _helpDict = {
    'mot1':
          ('[chaine(into=["defaut_bar"])] equivalent to transportant_bar 0 transporte_bar 1 filtrer_resu \n'
           '1 antisym 1',
           ''),
    'bloc_ef':
          ('[bloc_ef] not_set', ''),
  }
  _helpDict = check_append_dict('convection_ef_Tru', _helpDict, convection_deriv_Tru._helpDict)
##### end convection_ef_Tru class

class convection_muscl3_Tru(convection_deriv_Tru): #####
  r"""
  Keyword for a scheme using a ponderation between muscl and center schemes in VEF.
  """
  # From: line 1151, in file 'TRAD2_example'
  _braces = 1
  _synonyms = ['muscl3_Tru']
  _attributesList = [
    ('alpha', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['alpha'])
  _infoMain = ['TRAD2_example', 1151]
  _infoAttr = {'alpha': ['TRAD2_example', 1152]}
  _attributesList = check_append_list('convection_muscl3_Tru', _attributesList, convection_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_muscl3_Tru', _attributesSynonyms, convection_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_muscl3_Tru', _infoAttr, convection_deriv_Tru._infoAttr)
  _optionals.update(convection_deriv_Tru._optionals)
  _helpDict = {
    'alpha':
          ('[floattant] To weight the scheme centering with the factor double (between 0 (full \n'
           'centered) and 1 (muscl), by default 1).',
           ''),
  }
  _helpDict = check_append_dict('convection_muscl3_Tru', _helpDict, convection_deriv_Tru._helpDict)
##### end convection_muscl3_Tru class

class sous_zone_valeur_Tru(objet_lecture_Tru): #####
  r"""
  Two words.
  """
  # From: line 1153, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('sous_zone', 'ref_sous_zone_Tru'),   #
    ('valeur', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1153]
  _infoAttr = {'sous_zone': ['TRAD2_example', 1154], 'valeur': ['TRAD2_example', 1155]}
  _attributesList = check_append_list('sous_zone_valeur_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('sous_zone_valeur_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('sous_zone_valeur_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'sous_zone':
          ('[ref_sous_zone] sous zone', ''),
    'valeur':
          ('[floattant] value', ''),
  }
  _helpDict = check_append_dict('sous_zone_valeur_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end sous_zone_valeur_Tru class

class listsous_zone_valeur_Tru(ListOfBase_Tru): #####
  """
  List of groups of two words.
  """
  # From: line 1156, in file 'TRAD2_example'
  _braces = 0
  _comma = 0
  _allowedClasses = ["sous_zone_valeur_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 1156]
##### end listsous_zone_valeur_Tru class

class convection_ef_stab_Tru(convection_deriv_Tru): #####
  r"""
  Keyword for a VEF convective scheme.
  """
  # From: line 1157, in file 'TRAD2_example'
  _braces = 1
  _synonyms = ['ef_stab_Tru']
  _attributesList = [
    ('alpha', 'floattant_820d8b_Tru'),   #
    ('test', 'entier_3282e0_Tru'),   #
    ('tdivu', 'rien_Tru'),   #
    ('old', 'rien_Tru'),   #
    ('volumes_etendus', 'rien_Tru'),   #
    ('volumes_non_etendus', 'rien_Tru'),   #
    ('amont_sous_zone', 'ref_sous_zone_Tru'),   #
    ('alpha_sous_zone', 'listsous_zone_valeur_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['alpha',
   'test',
   'tdivu',
   'old',
   'volumes_etendus',
   'volumes_non_etendus',
   'amont_sous_zone',
   'alpha_sous_zone'])
  _infoMain = ['TRAD2_example', 1157]
  _infoAttr = {'alpha': ['TRAD2_example', 1158],
   'alpha_sous_zone': ['TRAD2_example', 1165],
   'amont_sous_zone': ['TRAD2_example', 1164],
   'old': ['TRAD2_example', 1161],
   'tdivu': ['TRAD2_example', 1160],
   'test': ['TRAD2_example', 1159],
   'volumes_etendus': ['TRAD2_example', 1162],
   'volumes_non_etendus': ['TRAD2_example', 1163]}
  _attributesList = check_append_list('convection_ef_stab_Tru', _attributesList, convection_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_ef_stab_Tru', _attributesSynonyms, convection_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_ef_stab_Tru', _infoAttr, convection_deriv_Tru._infoAttr)
  _optionals.update(convection_deriv_Tru._optionals)
  _helpDict = {
    'alpha':
          ('[floattant] To weight the scheme centering with the factor double (between 0 (full \n'
           'centered) and 1 (mix between upwind and centered), by default 1).\n'
           'For scalar equation, it is adviced to use alpha=1 and for the momentum equation, \n'
           'alpha=0.2 is adviced.',
           ''),
    'test':
          ('[entier] Developer option to compare old and new version of EF_stab', ''),
    'tdivu':
          ('[rien] To have the convective operator calculated as div(TU)-TdivU(=UgradT).', ''),
    'old':
          ('[rien] To use old version of EF_stab scheme (default no).', ''),
    'volumes_etendus':
          ('[rien] Option for the scheme to use the extended volumes (default, yes).', ''),
    'volumes_non_etendus':
          ('[rien] Option for the scheme to not use the extended volumes (default, no).', ''),
    'amont_sous_zone':
          ('[ref_sous_zone] Option to degenerate EF_stab scheme into Amont (upwind) scheme in \n'
           'the sub zone of name sz_name.\n'
           'The sub zone may be located arbitrarily in the domain but the more often this option \n'
           'will be activated in a zone where EF_stab scheme generates instabilities as for free \n'
           'outlet for example.',
           ''),
    'alpha_sous_zone':
          ('[listsous_zone_valeur] Option to change locally the alpha value on N sub-zones named \n'
           'sub_zone_name_I.\n'
           'Generally, it is used to prevent from a local divergence by increasing locally the \n'
           'alpha parameter.',
           ''),
  }
  _helpDict = check_append_dict('convection_ef_stab_Tru', _helpDict, convection_deriv_Tru._helpDict)
##### end convection_ef_stab_Tru class

class convection_generic_Tru(convection_deriv_Tru): #####
  r"""
  Keyword for generic calling of upwind and muscl convective scheme in VEF discretization.
  For muscl scheme, limiters and order for fluxes calculations have to be specified.
  The available limiters are : minmod - vanleer -vanalbada - chakravarthy - superbee, 
  and the order of accuracy is 1 or 2.
  Note that chakravarthy is a non-symmetric limiter and superbee may engender results 
  out of physical limits.
  By consequence, these two limiters are not recommended.
  
  Examples:
  
  convection { generic amont }NL2 convection { generic muscl minmod 1 }NL2 convection 
  { generic muscl vanleer 2 }NL2
  
  In case of results out of physical limits with muscl scheme (due for instance to 
  strong non-conformal velocity flow field), user can redefine in data file a lower 
  order and a smoother limiter, as : convection { generic muscl minmod 1 }
  """
  # From: line 1166, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['generic_Tru']
  _attributesList = [
    ('type', 'chaine_1691a8_Tru'),   #
    ('limiteur', 'chaine_c002ae_Tru'),   #
    ('ordre', 'entier_6d15af_Tru'),   #
    ('alpha', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['limiteur', 'ordre', 'alpha'])
  _infoMain = ['TRAD2_example', 1166]
  _infoAttr = {'alpha': ['TRAD2_example', 1170],
   'limiteur': ['TRAD2_example', 1168],
   'ordre': ['TRAD2_example', 1169],
   'type': ['TRAD2_example', 1167]}
  _attributesList = check_append_list('convection_generic_Tru', _attributesList, convection_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_generic_Tru', _attributesSynonyms, convection_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_generic_Tru', _infoAttr, convection_deriv_Tru._infoAttr)
  _optionals.update(convection_deriv_Tru._optionals)
  _helpDict = {
    'type':
          ('[chaine(into=["amont","muscl","centre"])] type of scheme', ''),
    'limiteur':
          ('[chaine(into=["minmod","vanleer","vanalbada","chakravarthy","superbee"])] type of \nlimiter', ''),
    'ordre':
          ('[entier(into=[1,2,3])] order of accuracy', ''),
    'alpha':
          ('[floattant] alpha', ''),
  }
  _helpDict = check_append_dict('convection_generic_Tru', _helpDict, convection_deriv_Tru._helpDict)
##### end convection_generic_Tru class

class convection_kquick_Tru(convection_deriv_Tru): #####
  r"""
  Only for VEF discretization.
  """
  # From: line 1171, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['kquick_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1171]
  _infoAttr = {}
  _attributesList = check_append_list('convection_kquick_Tru', _attributesList, convection_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_kquick_Tru', _attributesSynonyms, convection_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_kquick_Tru', _infoAttr, convection_deriv_Tru._infoAttr)
  _optionals.update(convection_deriv_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('convection_kquick_Tru', _helpDict, convection_deriv_Tru._helpDict)
##### end convection_kquick_Tru class

class convection_muscl_Tru(convection_deriv_Tru): #####
  r"""
  Keyword for muscl scheme in VEF discretization equivalent to generic muscl vanleer 
  2 for the 1.5 version or later.
  The previous muscl scheme can be used with the obsolete in future muscl_old keyword.
  """
  # From: line 1172, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['muscl_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1172]
  _infoAttr = {}
  _attributesList = check_append_list('convection_muscl_Tru', _attributesList, convection_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_muscl_Tru', _attributesSynonyms, convection_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_muscl_Tru', _infoAttr, convection_deriv_Tru._infoAttr)
  _optionals.update(convection_deriv_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('convection_muscl_Tru', _helpDict, convection_deriv_Tru._helpDict)
##### end convection_muscl_Tru class

class convection_muscl_old_Tru(convection_deriv_Tru): #####
  r"""
  Only for VEF discretization.
  """
  # From: line 1173, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['muscl_old_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1173]
  _infoAttr = {}
  _attributesList = check_append_list('convection_muscl_old_Tru', _attributesList, convection_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_muscl_old_Tru', _attributesSynonyms, convection_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_muscl_old_Tru', _infoAttr, convection_deriv_Tru._infoAttr)
  _optionals.update(convection_deriv_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('convection_muscl_old_Tru', _helpDict, convection_deriv_Tru._helpDict)
##### end convection_muscl_old_Tru class

class convection_muscl_new_Tru(convection_deriv_Tru): #####
  r"""
  Only for VEF discretization.
  """
  # From: line 1174, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['muscl_new_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1174]
  _infoAttr = {}
  _attributesList = check_append_list('convection_muscl_new_Tru', _attributesList, convection_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_muscl_new_Tru', _attributesSynonyms, convection_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_muscl_new_Tru', _infoAttr, convection_deriv_Tru._infoAttr)
  _optionals.update(convection_deriv_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('convection_muscl_new_Tru', _helpDict, convection_deriv_Tru._helpDict)
##### end convection_muscl_new_Tru class

class convection_negligeable_Tru(convection_deriv_Tru): #####
  r"""
  For VDF and VEF discretizations.
  Suppresses the convection operator.
  """
  # From: line 1175, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['negligeable_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1175]
  _infoAttr = {}
  _attributesList = check_append_list('convection_negligeable_Tru', _attributesList, convection_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_negligeable_Tru', _attributesSynonyms, convection_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_negligeable_Tru', _infoAttr, convection_deriv_Tru._infoAttr)
  _optionals.update(convection_deriv_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('convection_negligeable_Tru', _helpDict, convection_deriv_Tru._helpDict)
##### end convection_negligeable_Tru class

class convection_quick_Tru(convection_deriv_Tru): #####
  r"""
  Only for VDF discretization.
  """
  # From: line 1176, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['quick_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1176]
  _infoAttr = {}
  _attributesList = check_append_list('convection_quick_Tru', _attributesList, convection_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_quick_Tru', _attributesSynonyms, convection_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_quick_Tru', _infoAttr, convection_deriv_Tru._infoAttr)
  _optionals.update(convection_deriv_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('convection_quick_Tru', _helpDict, convection_deriv_Tru._helpDict)
##### end convection_quick_Tru class

class diffusion_negligeable_Tru(diffusion_deriv_Tru): #####
  r"""
  the diffusivity will not taken in count
  """
  # From: line 1177, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['negligeable_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1177]
  _infoAttr = {}
  _attributesList = check_append_list('diffusion_negligeable_Tru', _attributesList, diffusion_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('diffusion_negligeable_Tru', _attributesSynonyms, diffusion_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('diffusion_negligeable_Tru', _infoAttr, diffusion_deriv_Tru._infoAttr)
  _optionals.update(diffusion_deriv_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('diffusion_negligeable_Tru', _helpDict, diffusion_deriv_Tru._helpDict)
##### end diffusion_negligeable_Tru class

class bloc_diffusion_standard_Tru(objet_lecture_Tru): #####
  r"""
  grad_Ubar 1 makes the gradient calculated through the filtered values of velocity 
  (P1-conform).NL2 nu 1 (respectively nut 1) takes the molecular viscosity (eddy viscosity) 
  into account in the velocity gradient part of the diffusion expression.
  
  nu_transp 1 (respectively nut_transp 1) takes the molecular viscosity (eddy viscosity) 
  into account according in the TRANSPOSED velocity gradient part of the diffusion expression.NL2 
  filtrer_resu 1 allows to filter the resulting diffusive fluxes contribution.
  """
  # From: line 1178, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('mot1', 'chaine_84289f_Tru'),   #
    ('val1', 'entier_8bf71a_Tru'),   #
    ('mot2', 'chaine_84289f_Tru'),   #
    ('val2', 'entier_8bf71a_Tru'),   #
    ('mot3', 'chaine_84289f_Tru'),   #
    ('val3', 'entier_8bf71a_Tru'),   #
    ('mot4', 'chaine_84289f_Tru'),   #
    ('val4', 'entier_8bf71a_Tru'),   #
    ('mot5', 'chaine_84289f_Tru'),   #
    ('val5', 'entier_8bf71a_Tru'),   #
    ('mot6', 'chaine_84289f_Tru'),   #
    ('val6', 'entier_8bf71a_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1178]
  _infoAttr = {'mot1': ['TRAD2_example', 1179],
   'mot2': ['TRAD2_example', 1181],
   'mot3': ['TRAD2_example', 1183],
   'mot4': ['TRAD2_example', 1185],
   'mot5': ['TRAD2_example', 1187],
   'mot6': ['TRAD2_example', 1189],
   'val1': ['TRAD2_example', 1180],
   'val2': ['TRAD2_example', 1182],
   'val3': ['TRAD2_example', 1184],
   'val4': ['TRAD2_example', 1186],
   'val5': ['TRAD2_example', 1188],
   'val6': ['TRAD2_example', 1190]}
  _attributesList = check_append_list('bloc_diffusion_standard_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('bloc_diffusion_standard_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('bloc_diffusion_standard_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'mot1':
          ('[chaine(into=["grad_ubar","nu","nut","nu_transp","nut_transp","filtrer_resu"])] not_set', ''),
    'val1':
          ('[entier(into=[0,1])] not_set', ''),
    'mot2':
          ('[chaine(into=["grad_ubar","nu","nut","nu_transp","nut_transp","filtrer_resu"])] not_set', ''),
    'val2':
          ('[entier(into=[0,1])] not_set', ''),
    'mot3':
          ('[chaine(into=["grad_ubar","nu","nut","nu_transp","nut_transp","filtrer_resu"])] not_set', ''),
    'val3':
          ('[entier(into=[0,1])] not_set', ''),
    'mot4':
          ('[chaine(into=["grad_ubar","nu","nut","nu_transp","nut_transp","filtrer_resu"])] not_set', ''),
    'val4':
          ('[entier(into=[0,1])] not_set', ''),
    'mot5':
          ('[chaine(into=["grad_ubar","nu","nut","nu_transp","nut_transp","filtrer_resu"])] not_set', ''),
    'val5':
          ('[entier(into=[0,1])] not_set', ''),
    'mot6':
          ('[chaine(into=["grad_ubar","nu","nut","nu_transp","nut_transp","filtrer_resu"])] not_set', ''),
    'val6':
          ('[entier(into=[0,1])] not_set', ''),
  }
  _helpDict = check_append_dict('bloc_diffusion_standard_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end bloc_diffusion_standard_Tru class

class difusion_p1b_Tru(diffusion_deriv_Tru): #####
  r"""
  not_set
  """
  # From: line 1191, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['p1b_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1191]
  _infoAttr = {}
  _attributesList = check_append_list('difusion_p1b_Tru', _attributesList, diffusion_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('difusion_p1b_Tru', _attributesSynonyms, diffusion_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('difusion_p1b_Tru', _infoAttr, diffusion_deriv_Tru._infoAttr)
  _optionals.update(diffusion_deriv_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('difusion_p1b_Tru', _helpDict, diffusion_deriv_Tru._helpDict)
##### end difusion_p1b_Tru class

class diffusion_p1ncp1b_Tru(diffusion_deriv_Tru): #####
  r"""
  not_set
  """
  # From: line 1192, in file 'TRAD2_example'
  _braces = 1
  _synonyms = ['p1ncp1b_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1192]
  _infoAttr = {}
  _attributesList = check_append_list('diffusion_p1ncp1b_Tru', _attributesList, diffusion_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('diffusion_p1ncp1b_Tru', _attributesSynonyms, diffusion_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('diffusion_p1ncp1b_Tru', _infoAttr, diffusion_deriv_Tru._infoAttr)
  _optionals.update(diffusion_deriv_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('diffusion_p1ncp1b_Tru', _helpDict, diffusion_deriv_Tru._helpDict)
##### end diffusion_p1ncp1b_Tru class

class diffusion_stab_Tru(diffusion_deriv_Tru): #####
  r"""
  keyword allowing consistent and stable calculations even in case of obtuse angle meshes.
  """
  # From: line 1193, in file 'TRAD2_example'
  _braces = 1
  _synonyms = ['stab_Tru']
  _attributesList = [
    ('standard', 'entier_3282e0_Tru'),   #
    ('info', 'entier_3282e0_Tru'),   #
    ('new_jacobian', 'entier_3282e0_Tru'),   #
    ('nu', 'entier_3282e0_Tru'),   #
    ('nut', 'entier_3282e0_Tru'),   #
    ('nu_transp', 'entier_3282e0_Tru'),   #
    ('nut_transp', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['standard', 'info', 'new_jacobian', 'nu', 'nut', 'nu_transp', 'nut_transp'])
  _infoMain = ['TRAD2_example', 1193]
  _infoAttr = {'info': ['TRAD2_example', 1195],
   'new_jacobian': ['TRAD2_example', 1196],
   'nu': ['TRAD2_example', 1197],
   'nu_transp': ['TRAD2_example', 1199],
   'nut': ['TRAD2_example', 1198],
   'nut_transp': ['TRAD2_example', 1200],
   'standard': ['TRAD2_example', 1194]}
  _attributesList = check_append_list('diffusion_stab_Tru', _attributesList, diffusion_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('diffusion_stab_Tru', _attributesSynonyms, diffusion_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('diffusion_stab_Tru', _infoAttr, diffusion_deriv_Tru._infoAttr)
  _optionals.update(diffusion_deriv_Tru._optionals)
  _helpDict = {
    'standard':
          ('[entier] to recover the same results as calculations made by standard laminar diffusion \n'
           'operator.\n'
           'However, no stabilization technique is used and calculations may be unstable when \n'
           'working with obtuse angle meshes (by default 0)',
           ''),
    'info':
          ('[entier] developer option to get the stabilizing ratio (by default 0)', ''),
    'new_jacobian':
          ('[entier] when implicit time schemes are used, this option defines a new jacobian that \n'
           'may be more suitable to get stationary solutions (by default 0)',
           ''),
    'nu':
          ('[entier] (respectively nut 1) takes the molecular viscosity (resp.\n'
           'eddy viscosity) into account in the velocity gradient part of the diffusion expression \n'
           '(by default nu=1 and nut=1)',
           ''),
    'nut':
          ('[entier] not_set', ''),
    'nu_transp':
          ('[entier] (respectively nut_transp 1) takes the molecular viscosity (resp.\n'
           'eddy viscosity) into account in the transposed velocity gradient part of the diffusion \n'
           'expression (by default nu_transp=0 and nut_transp=1)',
           ''),
    'nut_transp':
          ('[entier] not_set', ''),
  }
  _helpDict = check_append_dict('diffusion_stab_Tru', _helpDict, diffusion_deriv_Tru._helpDict)
##### end diffusion_stab_Tru class

class diffusion_standard_Tru(diffusion_deriv_Tru): #####
  r"""
  A new keyword, intended for LES calculations, has been developed to optimise and parameterise 
  each term of the diffusion operator.
  Remark:NL2
  
  1.
  This class requires to define a filtering operator : see solveur_barNL2 2.
  The former (original) version: diffusion { } -which omitted some of the term of the 
  diffusion operator- can be recovered by using the following parameters in the new 
  class :NL2 diffusion { standard grad_Ubar 0 nu 1 nut 1 nu_transp 0 nut_transp 1 filtrer_resu 
  0}.
  """
  # From: line 1201, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['standard_Tru']
  _attributesList = [
    ('mot1', 'chaine_a3dc34_Tru'),   #
    ('bloc_diffusion_standard', 'bloc_diffusion_standard_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['mot1', 'bloc_diffusion_standard'])
  _infoMain = ['TRAD2_example', 1201]
  _infoAttr = {'bloc_diffusion_standard': ['TRAD2_example', 1203],
   'mot1': ['TRAD2_example', 1202]}
  _attributesList = check_append_list('diffusion_standard_Tru', _attributesList, diffusion_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('diffusion_standard_Tru', _attributesSynonyms, diffusion_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('diffusion_standard_Tru', _infoAttr, diffusion_deriv_Tru._infoAttr)
  _optionals.update(diffusion_deriv_Tru._optionals)
  _helpDict = {
    'mot1':
          ('[chaine(into=["defaut_bar"])] equivalent to grad_Ubar 1 nu 1 nut 1 nu_transp 1 nut_transp \n'
           '1 filtrer_resu 1',
           ''),
    'bloc_diffusion_standard':
          ('[bloc_diffusion_standard] not_set', ''),
  }
  _helpDict = check_append_dict('diffusion_standard_Tru', _helpDict, diffusion_deriv_Tru._helpDict)
##### end diffusion_standard_Tru class

class reaction_Tru(objet_lecture_Tru): #####
  r"""
  Keyword to describe reaction:
  
  w =K pow(T,beta) exp(-Ea/( R T)) $\Pi$ pow(Reactif_i,activitivity_i).
  
  If K_inv >0,
  
  w= K pow(T,beta) exp(-Ea/( R T)) ( $\Pi$ pow(Reactif_i,activitivity_i) - Kinv/exp(-c_r_Ea/(R 
  T)) $\Pi$ pow(Produit_i,activitivity_i ))
  """
  # From: line 1204, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('reactifs', 'chaine_Tru'),   #
    ('produits', 'chaine_Tru'),   #
    ('constante_taux_reaction', 'floattant_820d8b_Tru'),   #
    ('coefficients_activites', 'bloc_lecture_Tru'),   #
    ('enthalpie_reaction', 'floattant_820d8b_Tru'),   #
    ('energie_activation', 'floattant_820d8b_Tru'),   #
    ('exposant_beta', 'floattant_820d8b_Tru'),   #
    ('contre_reaction', 'floattant_820d8b_Tru'),   #
    ('contre_energie_activation', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['constante_taux_reaction',
   'coefficients_activites',
   'contre_reaction',
   'contre_energie_activation'])
  _infoMain = ['TRAD2_example', 1204]
  _infoAttr = {'coefficients_activites': ['TRAD2_example', 1208],
   'constante_taux_reaction': ['TRAD2_example', 1207],
   'contre_energie_activation': ['TRAD2_example', 1213],
   'contre_reaction': ['TRAD2_example', 1212],
   'energie_activation': ['TRAD2_example', 1210],
   'enthalpie_reaction': ['TRAD2_example', 1209],
   'exposant_beta': ['TRAD2_example', 1211],
   'produits': ['TRAD2_example', 1206],
   'reactifs': ['TRAD2_example', 1205]}
  _attributesList = check_append_list('reaction_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('reaction_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('reaction_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'reactifs':
          ('[chaine] LHS of equation (ex CH4+2*O2)', ''),
    'produits':
          ('[chaine] RHS of equation (ex CO2+2*H20)', ''),
    'constante_taux_reaction':
          ('[floattant] constante of cinetic K', ''),
    'coefficients_activites':
          ('[bloc_lecture] coefficients od ativity (exemple { CH4 1 O2 2 })', ''),
    'enthalpie_reaction':
          ('[floattant] DH', ''),
    'energie_activation':
          ('[floattant] Ea', ''),
    'exposant_beta':
          ('[floattant] Beta', ''),
    'contre_reaction':
          ('[floattant] K_inv', ''),
    'contre_energie_activation':
          ('[floattant] c_r_Ea', ''),
  }
  _helpDict = check_append_dict('reaction_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end reaction_Tru class

class reactions_Tru(ListOfBase_Tru): #####
  """
  list of reactions
  """
  # From: line 1214, in file 'TRAD2_example'
  _braces = 1
  _comma = 1
  _allowedClasses = ["reaction_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 1214]
##### end reactions_Tru class

class chimie_Tru(ConstrainBase_Tru): #####
  r"""
  Keyword to describe the chmical reactions
  """
  # From: line 1215, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('reactions', 'reactions_Tru'),   #
    ('modele_micro_melange', 'entier_3282e0_Tru'),   #
    ('constante_modele_micro_melange', 'floattant_820d8b_Tru'),   #
    ('espece_en_competition_micro_melange', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['modele_micro_melange',
   'constante_modele_micro_melange',
   'espece_en_competition_micro_melange'])
  _infoMain = ['TRAD2_example', 1215]
  _infoAttr = {'constante_modele_micro_melange': ['TRAD2_example', 1218],
   'espece_en_competition_micro_melange': ['TRAD2_example', 1219],
   'modele_micro_melange': ['TRAD2_example', 1217],
   'reactions': ['TRAD2_example', 1216]}
  _helpDict = {
    'reactions':
          ('[reactions] list of reactions', ''),
    'modele_micro_melange':
          ('[entier] modele_micro_melange (0 by default)', ''),
    'constante_modele_micro_melange':
          ('[floattant] constante of modele (1 by default)', ''),
    'espece_en_competition_micro_melange':
          ('[chaine] espece in competition in reactions', ''),
  }
##### end chimie_Tru class

class rectify_mesh_Tru(interprete_Tru): #####
  r"""
  Keyword to raffine a mesh
  """
  # From: line 1220, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['orienter_simplexes_Tru']
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1220]
  _infoAttr = {'domain_name': ['TRAD2_example', 1221]}
  _attributesList = check_append_list('rectify_mesh_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('rectify_mesh_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('rectify_mesh_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of domain.', ''),
  }
  _helpDict = check_append_dict('rectify_mesh_Tru', _helpDict, interprete_Tru._helpDict)
##### end rectify_mesh_Tru class

class verifier_simplexes_Tru(interprete_Tru): #####
  r"""
  Keyword to raffine a simplexes
  """
  # From: line 1222, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1222]
  _infoAttr = {'domain_name': ['TRAD2_example', 1223]}
  _attributesList = check_append_list('verifier_simplexes_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('verifier_simplexes_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('verifier_simplexes_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of domain.', ''),
  }
  _helpDict = check_append_dict('verifier_simplexes_Tru', _helpDict, interprete_Tru._helpDict)
##### end verifier_simplexes_Tru class

class verifier_qualite_raffinements_Tru(interprete_Tru): #####
  r"""
  not_set
  """
  # From: line 1224, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('domain_names', 'vect_nom_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1224]
  _infoAttr = {'domain_names': ['TRAD2_example', 1225]}
  _attributesList = check_append_list('verifier_qualite_raffinements_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('verifier_qualite_raffinements_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('verifier_qualite_raffinements_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_names':
          ('[vect_nom] not_set', ''),
  }
  _helpDict = check_append_dict('verifier_qualite_raffinements_Tru', _helpDict, interprete_Tru._helpDict)
##### end verifier_qualite_raffinements_Tru class

class schema_adams_bashforth_order_2_Tru(schema_temps_base_Tru): #####
  r"""
  not_set
  """
  # From: line 1226, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1226]
  _infoAttr = {}
  _attributesList = check_append_list('schema_adams_bashforth_order_2_Tru', _attributesList, schema_temps_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('schema_adams_bashforth_order_2_Tru', _attributesSynonyms, schema_temps_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('schema_adams_bashforth_order_2_Tru', _infoAttr, schema_temps_base_Tru._infoAttr)
  _optionals.update(schema_temps_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('schema_adams_bashforth_order_2_Tru', _helpDict, schema_temps_base_Tru._helpDict)
##### end schema_adams_bashforth_order_2_Tru class

class schema_adams_bashforth_order_3_Tru(schema_temps_base_Tru): #####
  r"""
  not_set
  """
  # From: line 1227, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1227]
  _infoAttr = {}
  _attributesList = check_append_list('schema_adams_bashforth_order_3_Tru', _attributesList, schema_temps_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('schema_adams_bashforth_order_3_Tru', _attributesSynonyms, schema_temps_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('schema_adams_bashforth_order_3_Tru', _infoAttr, schema_temps_base_Tru._infoAttr)
  _optionals.update(schema_temps_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('schema_adams_bashforth_order_3_Tru', _helpDict, schema_temps_base_Tru._helpDict)
##### end schema_adams_bashforth_order_3_Tru class

class schema_adams_moulton_order_2_Tru(schema_implicite_base_Tru): #####
  r"""
  not_set
  """
  # From: line 1228, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('facsec_max', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['facsec_max'])
  _infoMain = ['TRAD2_example', 1228]
  _infoAttr = {'facsec_max': ['TRAD2_example', 1229]}
  _attributesList = check_append_list('schema_adams_moulton_order_2_Tru', _attributesList, schema_implicite_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('schema_adams_moulton_order_2_Tru', _attributesSynonyms, schema_implicite_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('schema_adams_moulton_order_2_Tru', _infoAttr, schema_implicite_base_Tru._infoAttr)
  _optionals.update(schema_implicite_base_Tru._optionals)
  _helpDict = {
    'facsec_max':
          ('[floattant] Maximum ratio allowed between time step and stability time returned by \n'
           'CFL condition.\n'
           'The initial ratio given by facsec keyword is changed during the calculation with \n'
           "the implicit scheme but it couldn't be higher than facsec_max value.NL2 Warning: Some \n"
           'implicit schemes do not permit high facsec_max, example Schema_Adams_Moulton_order_3 \n'
           'needs facsec=facsec_max=1.\n'
           '\n'
           'Advice:NL2 The calculation may start with a facsec specified by the user and increased \n'
           'by the algorithm up to the facsec_max limit.\n'
           'But the user can also choose to specify a constant facsec (facsec_max will be set \n'
           'to facsec value then).\n'
           'Faster convergence has been seen and depends on the kind of calculation: NL2-Hydraulic \n'
           'only or thermal hydraulic with forced convection and low coupling between velocity \n'
           'and temperature (Boussinesq value beta low), facsec between 20-30NL2-Thermal hydraulic \n'
           'with forced convection and strong coupling between velocity and temperature (Boussinesq \n'
           'value beta high), facsec between 90-100 NL2-Thermohydralic with natural convection, \n'
           'facsec around 300NL2 -Conduction only, facsec can be set to a very high value (1e8) \n'
           'as if the scheme was unconditionally stableNL2These values can also be used as rule \n'
           'of thumb for initial facsec with a facsec_max limit higher.',
           ''),
  }
  _helpDict = check_append_dict('schema_adams_moulton_order_2_Tru', _helpDict, schema_implicite_base_Tru._helpDict)
##### end schema_adams_moulton_order_2_Tru class

class schema_adams_moulton_order_3_Tru(schema_implicite_base_Tru): #####
  r"""
  not_set
  """
  # From: line 1230, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('facsec_max', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['facsec_max'])
  _infoMain = ['TRAD2_example', 1230]
  _infoAttr = {'facsec_max': ['TRAD2_example', 1231]}
  _attributesList = check_append_list('schema_adams_moulton_order_3_Tru', _attributesList, schema_implicite_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('schema_adams_moulton_order_3_Tru', _attributesSynonyms, schema_implicite_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('schema_adams_moulton_order_3_Tru', _infoAttr, schema_implicite_base_Tru._infoAttr)
  _optionals.update(schema_implicite_base_Tru._optionals)
  _helpDict = {
    'facsec_max':
          ('[floattant] Maximum ratio allowed between time step and stability time returned by \n'
           'CFL condition.\n'
           'The initial ratio given by facsec keyword is changed during the calculation with \n'
           "the implicit scheme but it couldn't be higher than facsec_max value.NL2 Warning: Some \n"
           'implicit schemes do not permit high facsec_max, example Schema_Adams_Moulton_order_3 \n'
           'needs facsec=facsec_max=1.\n'
           '\n'
           'Advice:NL2 The calculation may start with a facsec specified by the user and increased \n'
           'by the algorithm up to the facsec_max limit.\n'
           'But the user can also choose to specify a constant facsec (facsec_max will be set \n'
           'to facsec value then).\n'
           'Faster convergence has been seen and depends on the kind of calculation: NL2-Hydraulic \n'
           'only or thermal hydraulic with forced convection and low coupling between velocity \n'
           'and temperature (Boussinesq value beta low), facsec between 20-30NL2-Thermal hydraulic \n'
           'with forced convection and strong coupling between velocity and temperature (Boussinesq \n'
           'value beta high), facsec between 90-100 NL2-Thermohydralic with natural convection, \n'
           'facsec around 300NL2 -Conduction only, facsec can be set to a very high value (1e8) \n'
           'as if the scheme was unconditionally stableNL2These values can also be used as rule \n'
           'of thumb for initial facsec with a facsec_max limit higher.',
           ''),
  }
  _helpDict = check_append_dict('schema_adams_moulton_order_3_Tru', _helpDict, schema_implicite_base_Tru._helpDict)
##### end schema_adams_moulton_order_3_Tru class

class schema_backward_differentiation_order_2_Tru(schema_implicite_base_Tru): #####
  r"""
  not_set
  """
  # From: line 1232, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('facsec_max', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['facsec_max'])
  _infoMain = ['TRAD2_example', 1232]
  _infoAttr = {'facsec_max': ['TRAD2_example', 1233]}
  _attributesList = check_append_list('schema_backward_differentiation_order_2_Tru', _attributesList, schema_implicite_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('schema_backward_differentiation_order_2_Tru', _attributesSynonyms, schema_implicite_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('schema_backward_differentiation_order_2_Tru', _infoAttr, schema_implicite_base_Tru._infoAttr)
  _optionals.update(schema_implicite_base_Tru._optionals)
  _helpDict = {
    'facsec_max':
          ('[floattant] Maximum ratio allowed between time step and stability time returned by \n'
           'CFL condition.\n'
           'The initial ratio given by facsec keyword is changed during the calculation with \n'
           "the implicit scheme but it couldn't be higher than facsec_max value.NL2 Warning: Some \n"
           'implicit schemes do not permit high facsec_max, example Schema_Adams_Moulton_order_3 \n'
           'needs facsec=facsec_max=1.\n'
           '\n'
           'Advice:NL2 The calculation may start with a facsec specified by the user and increased \n'
           'by the algorithm up to the facsec_max limit.\n'
           'But the user can also choose to specify a constant facsec (facsec_max will be set \n'
           'to facsec value then).\n'
           'Faster convergence has been seen and depends on the kind of calculation: NL2-Hydraulic \n'
           'only or thermal hydraulic with forced convection and low coupling between velocity \n'
           'and temperature (Boussinesq value beta low), facsec between 20-30NL2-Thermal hydraulic \n'
           'with forced convection and strong coupling between velocity and temperature (Boussinesq \n'
           'value beta high), facsec between 90-100 NL2-Thermohydralic with natural convection, \n'
           'facsec around 300NL2 -Conduction only, facsec can be set to a very high value (1e8) \n'
           'as if the scheme was unconditionally stableNL2These values can also be used as rule \n'
           'of thumb for initial facsec with a facsec_max limit higher.',
           ''),
  }
  _helpDict = check_append_dict('schema_backward_differentiation_order_2_Tru', _helpDict, schema_implicite_base_Tru._helpDict)
##### end schema_backward_differentiation_order_2_Tru class

class schema_backward_differentiation_order_3_Tru(schema_implicite_base_Tru): #####
  r"""
  not_set
  """
  # From: line 1234, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('facsec_max', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['facsec_max'])
  _infoMain = ['TRAD2_example', 1234]
  _infoAttr = {'facsec_max': ['TRAD2_example', 1235]}
  _attributesList = check_append_list('schema_backward_differentiation_order_3_Tru', _attributesList, schema_implicite_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('schema_backward_differentiation_order_3_Tru', _attributesSynonyms, schema_implicite_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('schema_backward_differentiation_order_3_Tru', _infoAttr, schema_implicite_base_Tru._infoAttr)
  _optionals.update(schema_implicite_base_Tru._optionals)
  _helpDict = {
    'facsec_max':
          ('[floattant] Maximum ratio allowed between time step and stability time returned by \n'
           'CFL condition.\n'
           'The initial ratio given by facsec keyword is changed during the calculation with \n'
           "the implicit scheme but it couldn't be higher than facsec_max value.NL2 Warning: Some \n"
           'implicit schemes do not permit high facsec_max, example Schema_Adams_Moulton_order_3 \n'
           'needs facsec=facsec_max=1.\n'
           '\n'
           'Advice:NL2 The calculation may start with a facsec specified by the user and increased \n'
           'by the algorithm up to the facsec_max limit.\n'
           'But the user can also choose to specify a constant facsec (facsec_max will be set \n'
           'to facsec value then).\n'
           'Faster convergence has been seen and depends on the kind of calculation: NL2-Hydraulic \n'
           'only or thermal hydraulic with forced convection and low coupling between velocity \n'
           'and temperature (Boussinesq value beta low), facsec between 20-30NL2-Thermal hydraulic \n'
           'with forced convection and strong coupling between velocity and temperature (Boussinesq \n'
           'value beta high), facsec between 90-100 NL2-Thermohydralic with natural convection, \n'
           'facsec around 300NL2 -Conduction only, facsec can be set to a very high value (1e8) \n'
           'as if the scheme was unconditionally stableNL2These values can also be used as rule \n'
           'of thumb for initial facsec with a facsec_max limit higher.',
           ''),
  }
  _helpDict = check_append_dict('schema_backward_differentiation_order_3_Tru', _helpDict, schema_implicite_base_Tru._helpDict)
##### end schema_backward_differentiation_order_3_Tru class

class ef_Tru(discretisation_base_Tru): #####
  r"""
  Element Finite discretization.
  """
  # From: line 1236, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1236]
  _infoAttr = {}
  _attributesList = check_append_list('ef_Tru', _attributesList, discretisation_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('ef_Tru', _attributesSynonyms, discretisation_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('ef_Tru', _infoAttr, discretisation_base_Tru._infoAttr)
  _optionals.update(discretisation_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('ef_Tru', _helpDict, discretisation_base_Tru._helpDict)
##### end ef_Tru class

class diffusion_option_Tru(diffusion_deriv_Tru): #####
  r"""
  not_set
  """
  # From: line 1237, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['option_Tru']
  _attributesList = [
    ('bloc_lecture', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1237]
  _infoAttr = {'bloc_lecture': ['TRAD2_example', 1238]}
  _attributesList = check_append_list('diffusion_option_Tru', _attributesList, diffusion_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('diffusion_option_Tru', _attributesSynonyms, diffusion_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('diffusion_option_Tru', _infoAttr, diffusion_deriv_Tru._infoAttr)
  _optionals.update(diffusion_deriv_Tru._optionals)
  _helpDict = {
    'bloc_lecture':
          ('[bloc_lecture] not_set', ''),
  }
  _helpDict = check_append_dict('diffusion_option_Tru', _helpDict, diffusion_deriv_Tru._helpDict)
##### end diffusion_option_Tru class

class testeur_medcoupling_Tru(interprete_Tru): #####
  r"""
  not_set
  """
  # From: line 1239, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('pb_name', 'chaine_Tru'),   #
    ('field_name', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'field_name': ['filed_name']
  }
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1239]
  _infoAttr = {'field_name': ['TRAD2_example', 1241], 'pb_name': ['TRAD2_example', 1240]}
  _attributesList = check_append_list('testeur_medcoupling_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('testeur_medcoupling_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('testeur_medcoupling_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'pb_name':
          ('[chaine] Name of domain.', ''),
    'field_name':
          ('[chaine] Name of domain.', ''),
  }
  _helpDict = check_append_dict('testeur_medcoupling_Tru', _helpDict, interprete_Tru._helpDict)
##### end testeur_medcoupling_Tru class

class champ_front_normal_vef_Tru(front_field_base_Tru): #####
  r"""
  Field to define the normal vector field standard at the boundary in VEF discretization.
  """
  # From: line 1242, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('mot', 'chaine_588727_Tru'),   #
    ('vit_tan', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1242]
  _infoAttr = {'mot': ['TRAD2_example', 1243], 'vit_tan': ['TRAD2_example', 1244]}
  _attributesList = check_append_list('champ_front_normal_vef_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_normal_vef_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_normal_vef_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'mot':
          ('[chaine(into=["valeur_normale"])] Name of vector field.', ''),
    'vit_tan':
          ('[floattant] normal vector value (positive value for a vector oriented outside to inside).', ''),
  }
  _helpDict = check_append_dict('champ_front_normal_vef_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_normal_vef_Tru class

class loi_fermeture_base_Tru(ConstrainBase_Tru): #####
  r"""
  Class for appends fermeture to problem
  """
  # From: line 1247, in file 'TRAD2_example'
  _braces = -3
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1247]
  _infoAttr = {}
  _helpDict = {}
##### end loi_fermeture_base_Tru class

class loi_fermeture_test_Tru(loi_fermeture_base_Tru): #####
  r"""
  Loi for test only
  """
  # From: line 1245, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('coef', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['coef'])
  _infoMain = ['TRAD2_example', 1245]
  _infoAttr = {'coef': ['TRAD2_example', 1246]}
  _attributesList = check_append_list('loi_fermeture_test_Tru', _attributesList, loi_fermeture_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('loi_fermeture_test_Tru', _attributesSynonyms, loi_fermeture_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('loi_fermeture_test_Tru', _infoAttr, loi_fermeture_base_Tru._infoAttr)
  _optionals.update(loi_fermeture_base_Tru._optionals)
  _helpDict = {
    'coef':
          ('[floattant] coefficient', ''),
  }
  _helpDict = check_append_dict('loi_fermeture_test_Tru', _helpDict, loi_fermeture_base_Tru._helpDict)
##### end loi_fermeture_test_Tru class

class scalaire_impose_paroi_Tru(dirichlet_Tru): #####
  r"""
  Imposed temperature condition at the wall called bord (edge).
  """
  # From: line 1248, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('ch', '<inherited>front_field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1248]
  _infoAttr = {'ch': ['TRAD2_example', 1249]}
  _attributesList = check_append_list('scalaire_impose_paroi_Tru', _attributesList, dirichlet_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('scalaire_impose_paroi_Tru', _attributesSynonyms, dirichlet_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('scalaire_impose_paroi_Tru', _infoAttr, dirichlet_Tru._infoAttr)
  _optionals.update(dirichlet_Tru._optionals)
  _helpDict = {
    'ch':
          ('[front_field_base] Boundary field type.', ''),
  }
  _helpDict = check_append_dict('scalaire_impose_paroi_Tru', _helpDict, dirichlet_Tru._helpDict)
##### end scalaire_impose_paroi_Tru class

class bloc_pdf_model_Tru(objet_lecture_Tru): #####
  r"""
  not_set
  """
  # From: line 1250, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('eta', 'floattant_820d8b_Tru'),   #
    ('temps_relaxation_coefficient_pdf', 'floattant_820d8b_Tru'),   #
    ('echelle_relaxation_coefficient_pdf', 'floattant_820d8b_Tru'),   #
    ('local', 'rien_Tru'),   #
    ('vitesse_imposee_data', '<inherited>field_base_Tru'),   #
    ('vitesse_imposee_fonction', 'troismots_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['temps_relaxation_coefficient_pdf',
   'echelle_relaxation_coefficient_pdf',
   'local',
   'vitesse_imposee_data',
   'vitesse_imposee_fonction'])
  _infoMain = ['TRAD2_example', 1250]
  _infoAttr = {'echelle_relaxation_coefficient_pdf': ['TRAD2_example', 1253],
   'eta': ['TRAD2_example', 1251],
   'local': ['TRAD2_example', 1254],
   'temps_relaxation_coefficient_pdf': ['TRAD2_example', 1252],
   'vitesse_imposee_data': ['TRAD2_example', 1255],
   'vitesse_imposee_fonction': ['TRAD2_example', 1256]}
  _attributesList = check_append_list('bloc_pdf_model_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('bloc_pdf_model_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('bloc_pdf_model_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'eta':
          ('[floattant] penalization coefficient', ''),
    'temps_relaxation_coefficient_pdf':
          ('[floattant] time relaxation on the forcing term to help', ''),
    'echelle_relaxation_coefficient_pdf':
          ('[floattant] time relaxation on the forcing term to help convergence', ''),
    'local':
          ('[rien] rien whether the prescribed velocity is expressed in the global or local basis', ''),
    'vitesse_imposee_data':
          ('[field_base] Prescribed velocity as a field', ''),
    'vitesse_imposee_fonction':
          ('[troismots] Prescribed velocity as a set of ananlytical component', ''),
  }
  _helpDict = check_append_dict('bloc_pdf_model_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end bloc_pdf_model_Tru class

class interprete_geometrique_base_Tru(interprete_Tru): #####
  r"""
  Class for interpreting a data file
  """
  # From: line 1364, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1364]
  _infoAttr = {}
  _attributesList = check_append_list('interprete_geometrique_base_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('interprete_geometrique_base_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('interprete_geometrique_base_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('interprete_geometrique_base_Tru', _helpDict, interprete_Tru._helpDict)
##### end interprete_geometrique_base_Tru class

class create_domain_from_sub_domain_Tru(interprete_geometrique_base_Tru): #####
  r"""
  This keyword fills the domain domaine_final with the subdomaine par_sous_zone from 
  the domain domaine_init.
  It is very useful when meshing several mediums with Gmsh.
  Each medium will be defined as a subdomaine into Gmsh.
  A MED mesh file will be saved from Gmsh and read with Lire_Med keyword by the TRUST 
  data file.
  And with this keyword, a domain will be created for each medium in the TRUST data 
  file.
  """
  # From: line 1394, in file 'TRAD2_example'
  _braces = 1
  _synonyms = ['create_domain_from_sub_domains_Tru', 'create_domain_from_sous_zone_Tru']
  _attributesList = [
    ('domaine_final', 'ref_domaine_Tru'),   #
    ('par_sous_zone', 'chaine_Tru'),   #
    ('domaine_init', 'ref_domaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['domaine_final', 'par_sous_zone'])
  _infoMain = ['TRAD2_example', 1394]
  _infoAttr = {'domaine_final': ['TRAD2_example', 1395],
   'domaine_init': ['TRAD2_example', 1397],
   'par_sous_zone': ['TRAD2_example', 1396]}
  _attributesList = check_append_list('create_domain_from_sub_domain_Tru', _attributesList, interprete_geometrique_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('create_domain_from_sub_domain_Tru', _attributesSynonyms, interprete_geometrique_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('create_domain_from_sub_domain_Tru', _infoAttr, interprete_geometrique_base_Tru._infoAttr)
  _optionals.update(interprete_geometrique_base_Tru._optionals)
  _helpDict = {
    'domaine_final':
          ('[ref_domaine] new domain in which faces are stored', ''),
    'par_sous_zone':
          ('[chaine] a sub-area allowing to choose the elements', ''),
    'domaine_init':
          ('[ref_domaine] initial domain', ''),
  }
  _helpDict = check_append_dict('create_domain_from_sub_domain_Tru', _helpDict, interprete_geometrique_base_Tru._helpDict)
##### end create_domain_from_sub_domain_Tru class

class create_domain_from_sous_zone_Tru(create_domain_from_sub_domain_Tru): #####
  r"""
  kept for backward compatibility.
  please use Create_domain_from_sub_domain
  """
  # From: line 1257, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['create_domain_from_sub_domains_Tru', 'create_domain_from_sub_domain_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1257]
  _infoAttr = {}
  _attributesList = check_append_list('create_domain_from_sous_zone_Tru', _attributesList, create_domain_from_sub_domain_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('create_domain_from_sous_zone_Tru', _attributesSynonyms, create_domain_from_sub_domain_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('create_domain_from_sous_zone_Tru', _infoAttr, create_domain_from_sub_domain_Tru._infoAttr)
  _optionals.update(create_domain_from_sub_domain_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('create_domain_from_sous_zone_Tru', _helpDict, create_domain_from_sub_domain_Tru._helpDict)
##### end create_domain_from_sous_zone_Tru class

class champ_front_debit_massique_Tru(front_field_base_Tru): #####
  r"""
  This field is used to define a flow rate field using the density
  """
  # From: line 1258, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('ch', '<inherited>front_field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1258]
  _infoAttr = {'ch': ['TRAD2_example', 1259]}
  _attributesList = check_append_list('champ_front_debit_massique_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_debit_massique_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_debit_massique_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'ch':
          ('[front_field_base] uniform field in space to define the flow rate.\n'
           'It could be, for example, champ_front_uniforme, ch_front_input_uniform or '
           'champ_front_fonc_txyz \n'
           'that depends only on time.',
           ''),
  }
  _helpDict = check_append_dict('champ_front_debit_massique_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_debit_massique_Tru class

class champ_front_debit_Tru(front_field_base_Tru): #####
  r"""
  This field is used to define a flow rate field instead of a velocity field for a Dirichlet 
  boundary condition on Navier-Stokes equations.
  """
  # From: line 1260, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('ch', '<inherited>front_field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1260]
  _infoAttr = {'ch': ['TRAD2_example', 1261]}
  _attributesList = check_append_list('champ_front_debit_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_debit_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_debit_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'ch':
          ('[front_field_base] uniform field in space to define the flow rate.\n'
           'It could be, for example, champ_front_uniforme, ch_front_input_uniform or '
           'champ_front_fonc_txyz \n'
           'that depends only on time.',
           ''),
  }
  _helpDict = check_append_dict('champ_front_debit_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_debit_Tru class

class champ_front_composite_Tru(front_field_base_Tru): #####
  r"""
  Composite front field.
  Used in multiphase problems to associate data to each phase.
  """
  # From: line 1262, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('dim', 'entier_3282e0_Tru'),   #
    ('bloc', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1262]
  _infoAttr = {'bloc': ['TRAD2_example', 1264], 'dim': ['TRAD2_example', 1263]}
  _attributesList = check_append_list('champ_front_composite_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_composite_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_composite_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'dim':
          ('[entier] Number of field components.', ''),
    'bloc':
          ('[bloc_lecture] Values Various pieces of the field, defined per phase.\n'
           'Part 1 goes to phase 1, etc...',
           ''),
  }
  _helpDict = check_append_dict('champ_front_composite_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_composite_Tru class

class champ_front_xyz_debit_Tru(front_field_base_Tru): #####
  r"""
  This field is used to define a flow rate field with a velocity profil which will be 
  normalized to match the flow rate chosen.
  """
  # From: line 1265, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('velocity_profil', '<inherited>front_field_base_Tru'),   #
    ('flow_rate', '<inherited>front_field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['velocity_profil'])
  _infoMain = ['TRAD2_example', 1265]
  _infoAttr = {'flow_rate': ['TRAD2_example', 1267],
   'velocity_profil': ['TRAD2_example', 1266]}
  _attributesList = check_append_list('champ_front_xyz_debit_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_xyz_debit_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_xyz_debit_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'velocity_profil':
          ('[front_field_base] velocity_profil 0 velocity field to define the profil of velocity.', ''),
    'flow_rate':
          ('[front_field_base] flow_rate 1 uniform field in space to define the flow rate.\n'
           'It could be, for example, champ_front_uniforme, ch_front_input_uniform or champ_front_fonc_t',
           ''),
  }
  _helpDict = check_append_dict('champ_front_xyz_debit_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_xyz_debit_Tru class

class champ_front_fonc_t_Tru(front_field_base_Tru): #####
  r"""
  Boundary field that depends only on time.
  """
  # From: line 1268, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('val', 'listchaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1268]
  _infoAttr = {'val': ['TRAD2_example', 1269]}
  _attributesList = check_append_list('champ_front_fonc_t_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_fonc_t_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_fonc_t_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'val':
          ('[listchaine] Values of field components (mathematical expressions).', ''),
  }
  _helpDict = check_append_dict('champ_front_fonc_t_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_fonc_t_Tru class

class reduction_0d_Tru(champ_post_de_champs_post_Tru): #####
  r"""
  To calculate the min, max, sum, average, weighted sum, weighted average, weighted 
  sum by porosity, weighted average by porosity, euclidian norm, normalized euclidian 
  norm, L1 norm, L2 norm of a field.
  """
  # From: line 1270, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['champ_post_reduction_0d_Tru']
  _attributesList = [
    ('methode', 'chaine_d249f7_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1270]
  _infoAttr = {'methode': ['TRAD2_example', 1271]}
  _attributesList = check_append_list('reduction_0d_Tru', _attributesList, champ_post_de_champs_post_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('reduction_0d_Tru', _attributesSynonyms, champ_post_de_champs_post_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('reduction_0d_Tru', _infoAttr, champ_post_de_champs_post_Tru._infoAttr)
  _optionals.update(champ_post_de_champs_post_Tru._optionals)
  _helpDict = {
    'methode':
          ('[chaine(into=["min","max","moyenne","average","moyenne_ponderee","weighted_average","somme","sum","somme_ponderee","weighted_sum","somme_ponderee_porosite","weighted_sum_porosity","euclidian_norm","normalized_euclidian_norm","l1_norm","l2_norm","valeur_a_gauche","left_value"])] \n'
           'name of the reduction method:\n'
           '\n'
           '- min for the minimum value,\n'
           '\n'
           '- max for the maximum value,\n'
           '\n'
           '- average (or moyenne) for a mean,\n'
           '\n'
           '- weighted_average (or moyenne_ponderee) for a mean ponderated by integration volumes, \n'
           'e.g: cell volumes for temperature and pressure in VDF, volumes around faces for velocity \n'
           'and temperature in VEF,\n'
           '\n'
           '- sum (or somme) for the sum of all the values of the field,\n'
           '\n'
           '- weighted_sum (or somme_ponderee) for a weighted sum (integral),\n'
           '\n'
           '- weighted_average_porosity (or moyenne_ponderee_porosite) and weighted_sum_porosity \n'
           '(or somme_ponderee_porosite) for the mean and sum weighted by the volumes of the elements, \n'
           'only for ELEM localisation,\n'
           '\n'
           '- euclidian_norm for the euclidian norm,\n'
           '\n'
           '- normalized_euclidian_norm for the euclidian norm normalized,\n'
           '\n'
           '- L1_norm for norm L1,\n'
           '\n'
           '- L2_norm for norm L2',
           ''),
  }
  _helpDict = check_append_dict('reduction_0d_Tru', _helpDict, champ_post_de_champs_post_Tru._helpDict)
##### end reduction_0d_Tru class

class champ_front_fonc_txyz_Tru(front_field_base_Tru): #####
  r"""
  Boundary field which is not constant in space and in time.
  """
  # From: line 1302, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('val', 'listchaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1302]
  _infoAttr = {'val': ['TRAD2_example', 1303]}
  _attributesList = check_append_list('champ_front_fonc_txyz_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_fonc_txyz_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_fonc_txyz_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'val':
          ('[listchaine] Values of field components (mathematical expressions).', ''),
  }
  _helpDict = check_append_dict('champ_front_fonc_txyz_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_fonc_txyz_Tru class

class champ_front_xyz_tabule_Tru(champ_front_fonc_txyz_Tru): #####
  r"""
  Space dependent field on the boundary, tabulated as a function of time.
  """
  # From: line 1272, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('val', 'listchaine_Tru'),   #
    ('bloc', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1272]
  _infoAttr = {'bloc': ['TRAD2_example', 1274], 'val': ['TRAD2_example', 1273]}
  _attributesList = check_append_list('champ_front_xyz_tabule_Tru', _attributesList, champ_front_fonc_txyz_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_xyz_tabule_Tru', _attributesSynonyms, champ_front_fonc_txyz_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_xyz_tabule_Tru', _infoAttr, champ_front_fonc_txyz_Tru._infoAttr)
  _optionals.update(champ_front_fonc_txyz_Tru._optionals)
  _helpDict = {
    'val':
          ('[listchaine] Values of field components (mathematical expressions).', ''),
    'bloc':
          ('[bloc_lecture] {nt1 t2 t3 ....tn u1 [v1 w1 ...] u2 [v2 w2 ...] u3 [v3 w3 ...] ...\n'
           'un [vn wn ...] }\n'
           '\n'
           'Values are entered into a table based on n couples (ti, ui) if nb_comp value is 1.\n'
           'The value of a field at a given time is calculated by linear interpolation from this \n'
           'table.',
           ''),
  }
  _helpDict = check_append_dict('champ_front_xyz_tabule_Tru', _helpDict, champ_front_fonc_txyz_Tru._helpDict)
##### end champ_front_xyz_tabule_Tru class

class champ_front_fonction_Tru(front_field_base_Tru): #####
  r"""
  boundary field that is function of another field
  """
  # From: line 1275, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('dim', 'entier_3282e0_Tru'),   #
    ('inco', 'chaine_Tru'),   #
    ('expression', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1275]
  _infoAttr = {'dim': ['TRAD2_example', 1276],
   'expression': ['TRAD2_example', 1278],
   'inco': ['TRAD2_example', 1277]}
  _attributesList = check_append_list('champ_front_fonction_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_fonction_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_fonction_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'dim':
          ('[entier] Number of field components.', ''),
    'inco':
          ('[chaine] Name of the field (for example: temperature).', ''),
    'expression':
          ('[chaine] keyword to use a analytical expression like 10.*EXP(-0.1*val) where val be \n'
           'the keyword for the field.',
           ''),
  }
  _helpDict = check_append_dict('champ_front_fonction_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_fonction_Tru class

class champ_front_tabule_Tru(front_field_base_Tru): #####
  r"""
  Constant field on the boundary, tabulated as a function of time.
  """
  # From: line 1279, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('nb_comp', 'entier_3282e0_Tru'),   #
    ('bloc', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1279]
  _infoAttr = {'bloc': ['TRAD2_example', 1281], 'nb_comp': ['TRAD2_example', 1280]}
  _attributesList = check_append_list('champ_front_tabule_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_tabule_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_tabule_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'nb_comp':
          ('[entier] Number of field components.', ''),
    'bloc':
          ('[bloc_lecture] {nt1 t2 t3 ....tn u1 [v1 w1 ...] u2 [v2 w2 ...] u3 [v3 w3 ...] ...\n'
           'un [vn wn ...] }\n'
           '\n'
           'Values are entered into a table based on n couples (ti, ui) if nb_comp value is 1.\n'
           'The value of a field at a given time is calculated by linear interpolation from this \n'
           'table.',
           ''),
  }
  _helpDict = check_append_dict('champ_front_tabule_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_tabule_Tru class

class predefini_Tru(champ_generique_base_Tru): #####
  r"""
  This keyword is used to post process predefined postprocessing fields.
  """
  # From: line 1282, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('pb_champ', 'deuxmots_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1282]
  _infoAttr = {'pb_champ': ['TRAD2_example', 1283]}
  _attributesList = check_append_list('predefini_Tru', _attributesList, champ_generique_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('predefini_Tru', _attributesSynonyms, champ_generique_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('predefini_Tru', _infoAttr, champ_generique_base_Tru._infoAttr)
  _optionals.update(champ_generique_base_Tru._optionals)
  _helpDict = {
    'pb_champ':
          ('[deuxmots] { Pb_champ nom_pb nom_champ } : nom_pb is the problem name and nom_champ \n'
           'is the selected field name.\n'
           'The available keywords for the field name are: energie_cinetique_totale, '
           'energie_cinetique_elem, \n'
           'viscosite_turbulente, viscous_force_x, viscous_force_y, viscous_force_z, pressure_force_x, \n'
           'pressure_force_y, pressure_force_z, total_force_x, total_force_y, total_force_z, '
           'viscous_force, \n'
           'pressure_force, total_force',
           ''),
  }
  _helpDict = check_append_dict('predefini_Tru', _helpDict, champ_generique_base_Tru._helpDict)
##### end predefini_Tru class

class champ_front_tabule_lu_Tru(champ_front_tabule_Tru): #####
  r"""
  Constant field on the boundary, tabulated from a specified column file.
  Lines starting with # are ignored.
  """
  # From: line 1284, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('nb_comp', 'entier_3282e0_Tru'),   #
    ('column_file', 'chaine_Tru'),   #
  ]
  _suppressed = ['bloc']
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1284]
  _infoAttr = {'column_file': ['TRAD2_example', 1286], 'nb_comp': ['TRAD2_example', 1285]}
  _attributesList = check_append_list('champ_front_tabule_lu_Tru', _attributesList, champ_front_tabule_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_tabule_lu_Tru', _attributesSynonyms, champ_front_tabule_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_tabule_lu_Tru', _infoAttr, champ_front_tabule_Tru._infoAttr)
  _optionals.update(champ_front_tabule_Tru._optionals)
  _helpDict = {
    'nb_comp':
          ('[entier] Number of field components.', ''),
    'column_file':
          ('[chaine] Name of the column file.', ''),
  }
  _helpDict = check_append_dict('champ_front_tabule_lu_Tru', _helpDict, champ_front_tabule_Tru._helpDict)
##### end champ_front_tabule_lu_Tru class

class champ_front_musig_Tru(champ_front_composite_Tru): #####
  r"""
  MUSIG front field.
  Used in multiphase problems to associate data to each phase.
  """
  # From: line 1288, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('bloc', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = ['dim']
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1288]
  _infoAttr = {'bloc': ['TRAD2_example', 1290]}
  _attributesList = check_append_list('champ_front_musig_Tru', _attributesList, champ_front_composite_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_musig_Tru', _attributesSynonyms, champ_front_composite_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_musig_Tru', _infoAttr, champ_front_composite_Tru._infoAttr)
  _optionals.update(champ_front_composite_Tru._optionals)
  _helpDict = {
    'bloc':
          ('[bloc_lecture] Not set', ''),
  }
  _helpDict = check_append_dict('champ_front_musig_Tru', _helpDict, champ_front_composite_Tru._helpDict)
##### end champ_front_musig_Tru class

class champ_front_bruite_Tru(front_field_base_Tru): #####
  r"""
  Field which is variable in time and space in a random manner.
  """
  # From: line 1291, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('nb_comp', 'entier_3282e0_Tru'),   #
    ('bloc', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1291]
  _infoAttr = {'bloc': ['TRAD2_example', 1293], 'nb_comp': ['TRAD2_example', 1292]}
  _attributesList = check_append_list('champ_front_bruite_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_bruite_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_bruite_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'nb_comp':
          ('[entier] Number of field components.', ''),
    'bloc':
          ('[bloc_lecture] { [N val L val ] Moyenne m_1.....[m_i ] Amplitude A_1.....[A_ i ]}: \n'
           'Random nois: If N and L are not defined, the ith component of the field varies randomly \n'
           'around an average value m_i with a maximum amplitude A_i.\n'
           '\n'
           'White noise: If N and L are defined, these two additional parameters correspond to \n'
           'L, the domain length and N, the number of nodes in the domain.\n'
           'Noise frequency will be between 2*Pi/L and 2*Pi*N/(4*L).\n'
           '\n'
           'For example, formula for velocity: u=U0(t) v=U1(t)Uj(t)=Mj+2*Aj*bruit_blanc where \n'
           'bruit_blanc (white_noise) is the formula given in the mettre_a_jour (update) method \n'
           'of the Champ_front_bruite (noise_boundary_field) (Refer to the Champ_front_bruite.cpp \n'
           'file).',
           ''),
  }
  _helpDict = check_append_dict('champ_front_bruite_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_bruite_Tru class

class champ_input_p0_composite_Tru(champ_input_base_Tru): #####
  r"""
  Field used to define a classical champ input p0 field (for ICoCo), but with a predefined 
  field for the initial state.
  """
  # From: line 1294, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('initial_field', '<inherited>field_base_Tru'),   #
    ('input_field', '<inherited>champ_input_p0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['initial_field', 'input_field'])
  _infoMain = ['TRAD2_example', 1294]
  _infoAttr = {'initial_field': ['TRAD2_example', 1295],
   'input_field': ['TRAD2_example', 1296]}
  _attributesList = check_append_list('champ_input_p0_composite_Tru', _attributesList, champ_input_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_input_p0_composite_Tru', _attributesSynonyms, champ_input_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_input_p0_composite_Tru', _infoAttr, champ_input_base_Tru._infoAttr)
  _optionals.update(champ_input_base_Tru._optionals)
  _helpDict = {
    'initial_field':
          ('[field_base] The field used for initialization', ''),
    'input_field':
          ('[champ_input_p0] The input field for ICoCo', ''),
  }
  _helpDict = check_append_dict('champ_input_p0_composite_Tru', _helpDict, champ_input_base_Tru._helpDict)
##### end champ_input_p0_composite_Tru class

class champ_fonc_fonction_txyz_Tru(champ_fonc_fonction_Tru): #####
  r"""
  this refers to a field that is a function of another field and time and/or space coordinates
  """
  # From: line 1301, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1301]
  _infoAttr = {}
  _attributesList = check_append_list('champ_fonc_fonction_txyz_Tru', _attributesList, champ_fonc_fonction_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_fonc_fonction_txyz_Tru', _attributesSynonyms, champ_fonc_fonction_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_fonc_fonction_txyz_Tru', _infoAttr, champ_fonc_fonction_Tru._infoAttr)
  _optionals.update(champ_fonc_fonction_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('champ_fonc_fonction_txyz_Tru', _helpDict, champ_fonc_fonction_Tru._helpDict)
##### end champ_fonc_fonction_txyz_Tru class

class interpolation_Tru(champ_post_de_champs_post_Tru): #####
  r"""
  To create a field which is an interpolation of the field given by the keyword source.
  """
  # From: line 1304, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['champ_post_interpolation_Tru']
  _attributesList = [
    ('localisation', 'chaine_Tru'),   #
    ('methode', 'chaine_Tru'),   #
    ('domaine', 'chaine_Tru'),   #
    ('optimisation_sous_maillage', 'chaine_910cb5_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['methode', 'domaine', 'optimisation_sous_maillage'])
  _infoMain = ['TRAD2_example', 1304]
  _infoAttr = {'domaine': ['TRAD2_example', 1307],
   'localisation': ['TRAD2_example', 1305],
   'methode': ['TRAD2_example', 1306],
   'optimisation_sous_maillage': ['TRAD2_example', 1308]}
  _attributesList = check_append_list('interpolation_Tru', _attributesList, champ_post_de_champs_post_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('interpolation_Tru', _attributesSynonyms, champ_post_de_champs_post_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('interpolation_Tru', _infoAttr, champ_post_de_champs_post_Tru._infoAttr)
  _optionals.update(champ_post_de_champs_post_Tru._optionals)
  _helpDict = {
    'localisation':
          ('[chaine] type_loc indicate where is done the interpolation (elem for element or som \nfor node).',
           ''),
    'methode':
          ('[chaine] The optional keyword methode is limited to calculer_champ_post for the moment.', ''),
    'domaine':
          ('[chaine] the domain name where the interpolation is done (by default, the calculation \ndomain)',
           ''),
    'optimisation_sous_maillage':
          ('[chaine(into=["default","yes","no",])] not_set', ''),
  }
  _helpDict = check_append_dict('interpolation_Tru', _helpDict, champ_post_de_champs_post_Tru._helpDict)
##### end interpolation_Tru class

class champ_composite_Tru(champ_don_base_Tru): #####
  r"""
  Composite field.
  Used in multiphase problems to associate data to each phase.
  """
  # From: line 1309, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('dim', 'entier_3282e0_Tru'),   #
    ('bloc', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1309]
  _infoAttr = {'bloc': ['TRAD2_example', 1311], 'dim': ['TRAD2_example', 1310]}
  _attributesList = check_append_list('champ_composite_Tru', _attributesList, champ_don_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_composite_Tru', _attributesSynonyms, champ_don_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_composite_Tru', _infoAttr, champ_don_base_Tru._infoAttr)
  _optionals.update(champ_don_base_Tru._optionals)
  _helpDict = {
    'dim':
          ('[entier] Number of field components.', ''),
    'bloc':
          ('[bloc_lecture] Values Various pieces of the field, defined per phase.\n'
           'Part 1 goes to phase 1, etc...',
           ''),
  }
  _helpDict = check_append_dict('champ_composite_Tru', _helpDict, champ_don_base_Tru._helpDict)
##### end champ_composite_Tru class

class champ_fonc_tabule_morceaux_Tru(champ_don_base_Tru): #####
  r"""
  Field defined by tabulated data in each sub-domaine.
  It makes possible the definition of a field which is a function of other fields.
  """
  # From: line 1323, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['champ_tabule_morceaux_Tru']
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
    ('nb_comp', 'entier_3282e0_Tru'),   #
    ('data', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1323]
  _infoAttr = {'data': ['TRAD2_example', 1326],
   'domain_name': ['TRAD2_example', 1324],
   'nb_comp': ['TRAD2_example', 1325]}
  _attributesList = check_append_list('champ_fonc_tabule_morceaux_Tru', _attributesList, champ_don_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_fonc_tabule_morceaux_Tru', _attributesSynonyms, champ_don_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_fonc_tabule_morceaux_Tru', _infoAttr, champ_don_base_Tru._infoAttr)
  _optionals.update(champ_don_base_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of the domain.', ''),
    'nb_comp':
          ('[entier] Number of field components.', ''),
    'data':
          ('[bloc_lecture] { Defaut val_def sous_domaine_1 val_1 ...\n'
           'sous_domaine_i val_i } By default, the value val_def is assigned to the field.\n'
           'It takes the sous_domaine_i identifier Sous_Domaine (sub_area) type object function, \n'
           'val_i.\n'
           'Sous_Domaine (sub_area) type objects must have been previously defined if the operator \n'
           'wishes to use a champ_fonc_tabule_morceaux type object.',
           ''),
  }
  _helpDict = check_append_dict('champ_fonc_tabule_morceaux_Tru', _helpDict, champ_don_base_Tru._helpDict)
##### end champ_fonc_tabule_morceaux_Tru class

class champ_fonc_tabule_morceaux_interp_Tru(champ_fonc_tabule_morceaux_Tru): #####
  r"""
  Field defined by tabulated data in each sub-domaine.
  It makes possible the definition of a field which is a function of other fields.
  Here we use MEDCoupling to interpolate fields between the two domains.
  """
  # From: line 1312, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('problem_name', 'ref_pb_base_Tru'),   #
  ]
  _suppressed = ['domain_name']
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1312]
  _infoAttr = {'problem_name': ['TRAD2_example', 1314]}
  _attributesList = check_append_list('champ_fonc_tabule_morceaux_interp_Tru', _attributesList, champ_fonc_tabule_morceaux_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_fonc_tabule_morceaux_interp_Tru', _attributesSynonyms, champ_fonc_tabule_morceaux_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_fonc_tabule_morceaux_interp_Tru', _infoAttr, champ_fonc_tabule_morceaux_Tru._infoAttr)
  _optionals.update(champ_fonc_tabule_morceaux_Tru._optionals)
  _helpDict = {
    'problem_name':
          ('[ref_pb_base] Name of the problem.', ''),
  }
  _helpDict = check_append_dict('champ_fonc_tabule_morceaux_interp_Tru', _helpDict, champ_fonc_tabule_morceaux_Tru._helpDict)
##### end champ_fonc_tabule_morceaux_interp_Tru class

class champ_fonc_fonction_txyz_morceaux_Tru(champ_don_base_Tru): #####
  r"""
  Field defined by analytical functions in each sub-domaine.
  It makes possible the definition of a field that depends on the time and the space.
  """
  # From: line 1315, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('problem_name', 'ref_pb_base_Tru'),   #
    ('inco', 'chaine_Tru'),   #
    ('nb_comp', 'entier_3282e0_Tru'),   #
    ('data', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1315]
  _infoAttr = {'data': ['TRAD2_example', 1319],
   'inco': ['TRAD2_example', 1317],
   'nb_comp': ['TRAD2_example', 1318],
   'problem_name': ['TRAD2_example', 1316]}
  _attributesList = check_append_list('champ_fonc_fonction_txyz_morceaux_Tru', _attributesList, champ_don_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_fonc_fonction_txyz_morceaux_Tru', _attributesSynonyms, champ_don_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_fonc_fonction_txyz_morceaux_Tru', _infoAttr, champ_don_base_Tru._infoAttr)
  _optionals.update(champ_don_base_Tru._optionals)
  _helpDict = {
    'problem_name':
          ('[ref_pb_base] Name of the problem.', ''),
    'inco':
          ('[chaine] Name of the field (for example: temperature).', ''),
    'nb_comp':
          ('[entier] Number of field components.', ''),
    'data':
          ('[bloc_lecture] { Defaut val_def sous_domaine_1 val_1 ...\n'
           'sous_domaine_i val_i } By default, the value val_def is assigned to the field.\n'
           'It takes the sous_domaine_i identifier Sous_Domaine (sub_area) type object function, \n'
           'val_i.\n'
           'Sous_Domaine (sub_area) type objects must have been previously defined if the operator \n'
           'wishes to use a champ_fonc_fonction_txyz_morceaux type object.',
           ''),
  }
  _helpDict = check_append_dict('champ_fonc_fonction_txyz_morceaux_Tru', _helpDict, champ_don_base_Tru._helpDict)
##### end champ_fonc_fonction_txyz_morceaux_Tru class

class champ_musig_Tru(champ_composite_Tru): #####
  r"""
  MUSIG field.
  Used in multiphase problems to associate data to each phase.
  """
  # From: line 1320, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('bloc', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = ['dim']
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1320]
  _infoAttr = {'bloc': ['TRAD2_example', 1322]}
  _attributesList = check_append_list('champ_musig_Tru', _attributesList, champ_composite_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_musig_Tru', _attributesSynonyms, champ_composite_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_musig_Tru', _infoAttr, champ_composite_Tru._infoAttr)
  _optionals.update(champ_composite_Tru._optionals)
  _helpDict = {
    'bloc':
          ('[bloc_lecture] Not set', ''),
  }
  _helpDict = check_append_dict('champ_musig_Tru', _helpDict, champ_composite_Tru._helpDict)
##### end champ_musig_Tru class

class runge_kutta_rationnel_ordre_2_Tru(schema_temps_base_Tru): #####
  r"""
  This is the Runge-Kutta rational scheme of second order.
  The method is described in the note: Wambeck - Rational Runge-Kutta methods for solving 
  systems of ordinary differential equations, at the link: https://link.springer.com/article/10.1007/BF02252381.
  Although rational methods require more computational work than linear ones, they 
  can have some other properties, such as a stable behaviour with explicitness, which 
  make them preferable.
  The CFD application of this RRK2 scheme is described in the note: https://link.springer.com/content/pdf/10.1007\%2F3-540-13917-6_112.pdf.
  """
  # From: line 1327, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1327]
  _infoAttr = {}
  _attributesList = check_append_list('runge_kutta_rationnel_ordre_2_Tru', _attributesList, schema_temps_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('runge_kutta_rationnel_ordre_2_Tru', _attributesSynonyms, schema_temps_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('runge_kutta_rationnel_ordre_2_Tru', _infoAttr, schema_temps_base_Tru._infoAttr)
  _optionals.update(schema_temps_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('runge_kutta_rationnel_ordre_2_Tru', _helpDict, schema_temps_base_Tru._helpDict)
##### end runge_kutta_rationnel_ordre_2_Tru class

class runge_kutta_ordre_2_Tru(schema_temps_base_Tru): #####
  r"""
  This is a low-storage Runge-Kutta scheme of second order that uses 2 integration points.
  The method is presented by Williamson (case 1) in https://www.sciencedirect.com/science/article/pii/0021999180900339
  """
  # From: line 1328, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1328]
  _infoAttr = {}
  _attributesList = check_append_list('runge_kutta_ordre_2_Tru', _attributesList, schema_temps_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('runge_kutta_ordre_2_Tru', _attributesSynonyms, schema_temps_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('runge_kutta_ordre_2_Tru', _infoAttr, schema_temps_base_Tru._infoAttr)
  _optionals.update(schema_temps_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('runge_kutta_ordre_2_Tru', _helpDict, schema_temps_base_Tru._helpDict)
##### end runge_kutta_ordre_2_Tru class

class runge_kutta_ordre_3_Tru(schema_temps_base_Tru): #####
  r"""
  This is a low-storage Runge-Kutta scheme of third order that uses 3 integration points.
  The method is presented by Williamson (case 7) in https://www.sciencedirect.com/science/article/pii/0021999180900339
  """
  # From: line 1329, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1329]
  _infoAttr = {}
  _attributesList = check_append_list('runge_kutta_ordre_3_Tru', _attributesList, schema_temps_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('runge_kutta_ordre_3_Tru', _attributesSynonyms, schema_temps_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('runge_kutta_ordre_3_Tru', _infoAttr, schema_temps_base_Tru._infoAttr)
  _optionals.update(schema_temps_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('runge_kutta_ordre_3_Tru', _helpDict, schema_temps_base_Tru._helpDict)
##### end runge_kutta_ordre_3_Tru class

class runge_kutta_ordre_4_Tru(schema_temps_base_Tru): #####
  r"""
  This is a low-storage Runge-Kutta scheme of fourth order that uses 3 integration points.
  The method is presented by Williamson (case 17) in https://www.sciencedirect.com/science/article/pii/0021999180900339
  """
  # From: line 1330, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['runge_kutta_ordre_4_d3p_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1330]
  _infoAttr = {}
  _attributesList = check_append_list('runge_kutta_ordre_4_Tru', _attributesList, schema_temps_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('runge_kutta_ordre_4_Tru', _attributesSynonyms, schema_temps_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('runge_kutta_ordre_4_Tru', _infoAttr, schema_temps_base_Tru._infoAttr)
  _optionals.update(schema_temps_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('runge_kutta_ordre_4_Tru', _helpDict, schema_temps_base_Tru._helpDict)
##### end runge_kutta_ordre_4_Tru class

class runge_kutta_ordre_2_classique_Tru(schema_temps_base_Tru): #####
  r"""
  This is a classical Runge-Kutta scheme of second order that uses 2 integration points.
  """
  # From: line 1331, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1331]
  _infoAttr = {}
  _attributesList = check_append_list('runge_kutta_ordre_2_classique_Tru', _attributesList, schema_temps_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('runge_kutta_ordre_2_classique_Tru', _attributesSynonyms, schema_temps_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('runge_kutta_ordre_2_classique_Tru', _infoAttr, schema_temps_base_Tru._infoAttr)
  _optionals.update(schema_temps_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('runge_kutta_ordre_2_classique_Tru', _helpDict, schema_temps_base_Tru._helpDict)
##### end runge_kutta_ordre_2_classique_Tru class

class runge_kutta_ordre_3_classique_Tru(schema_temps_base_Tru): #####
  r"""
  This is a classical Runge-Kutta scheme of third order that uses 3 integration points.
  """
  # From: line 1332, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1332]
  _infoAttr = {}
  _attributesList = check_append_list('runge_kutta_ordre_3_classique_Tru', _attributesList, schema_temps_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('runge_kutta_ordre_3_classique_Tru', _attributesSynonyms, schema_temps_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('runge_kutta_ordre_3_classique_Tru', _infoAttr, schema_temps_base_Tru._infoAttr)
  _optionals.update(schema_temps_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('runge_kutta_ordre_3_classique_Tru', _helpDict, schema_temps_base_Tru._helpDict)
##### end runge_kutta_ordre_3_classique_Tru class

class runge_kutta_ordre_4_classique_Tru(schema_temps_base_Tru): #####
  r"""
  This is a classical Runge-Kutta scheme of fourth order that uses 4 integration points.
  """
  # From: line 1333, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1333]
  _infoAttr = {}
  _attributesList = check_append_list('runge_kutta_ordre_4_classique_Tru', _attributesList, schema_temps_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('runge_kutta_ordre_4_classique_Tru', _attributesSynonyms, schema_temps_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('runge_kutta_ordre_4_classique_Tru', _infoAttr, schema_temps_base_Tru._infoAttr)
  _optionals.update(schema_temps_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('runge_kutta_ordre_4_classique_Tru', _helpDict, schema_temps_base_Tru._helpDict)
##### end runge_kutta_ordre_4_classique_Tru class

class runge_kutta_ordre_4_classique_3_8_Tru(schema_temps_base_Tru): #####
  r"""
  This is a classical Runge-Kutta scheme of fourth order that uses 4 integration points 
  and the 3/8 rule.
  """
  # From: line 1334, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1334]
  _infoAttr = {}
  _attributesList = check_append_list('runge_kutta_ordre_4_classique_3_8_Tru', _attributesList, schema_temps_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('runge_kutta_ordre_4_classique_3_8_Tru', _attributesSynonyms, schema_temps_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('runge_kutta_ordre_4_classique_3_8_Tru', _infoAttr, schema_temps_base_Tru._infoAttr)
  _optionals.update(schema_temps_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('runge_kutta_ordre_4_classique_3_8_Tru', _helpDict, schema_temps_base_Tru._helpDict)
##### end runge_kutta_ordre_4_classique_3_8_Tru class

class schema_euler_implicite_Tru(schema_implicite_base_Tru): #####
  r"""
  This is the Euler implicit scheme.
  """
  # From: line 1335, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['scheme_euler_implicit_Tru']
  _attributesList = [
    ('facsec_max', 'floattant_820d8b_Tru'),   #
    ('resolution_monolithique', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['facsec_max', 'resolution_monolithique'])
  _infoMain = ['TRAD2_example', 1335]
  _infoAttr = {'facsec_max': ['TRAD2_example', 1336],
   'resolution_monolithique': ['TRAD2_example', 1337]}
  _attributesList = check_append_list('schema_euler_implicite_Tru', _attributesList, schema_implicite_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('schema_euler_implicite_Tru', _attributesSynonyms, schema_implicite_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('schema_euler_implicite_Tru', _infoAttr, schema_implicite_base_Tru._infoAttr)
  _optionals.update(schema_implicite_base_Tru._optionals)
  _helpDict = {
    'facsec_max':
          ('[floattant] 1 Maximum ratio allowed between time step and stability time returned \n'
           'by CFL condition.\n'
           'The initial ratio given by facsec keyword is changed during the calculation with \n'
           "the implicit scheme but it couldn't be higher than facsec_max value.NL2 Warning: Some \n"
           'implicit schemes do not permit high facsec_max, example Schema_Adams_Moulton_order_3 \n'
           'needs facsec=facsec_max=1.\n'
           '\n'
           'Advice:NL2 The calculation may start with a facsec specified by the user and increased \n'
           'by the algorithm up to the facsec_max limit.\n'
           'But the user can also choose to specify a constant facsec (facsec_max will be set \n'
           'to facsec value then).\n'
           'Faster convergence has been seen and depends on the kind of calculation: NL2-Hydraulic \n'
           'only or thermal hydraulic with forced convection and low coupling between velocity \n'
           'and temperature (Boussinesq value beta low), facsec between 20-30NL2-Thermal hydraulic \n'
           'with forced convection and strong coupling between velocity and temperature (Boussinesq \n'
           'value beta high), facsec between 90-100 NL2-Thermohydralic with natural convection, \n'
           'facsec around 300NL2 -Conduction only, facsec can be set to a very high value (1e8) \n'
           'as if the scheme was unconditionally stableNL2These values can also be used as rule \n'
           'of thumb for initial facsec with a facsec_max limit higher.',
           ''),
    'resolution_monolithique':
          ('[bloc_lecture] Activate monolithic resolution for coupled problems.\n'
           'Solves together the equations corresponding to the application domains in the given \n'
           'order.\n'
           'All aplication domains of the coupled equations must be given to determine the order \n'
           'of resolution.\n'
           'If the monolithic solving is not wanted for a specific application domain, an underscore \n'
           'can be added as prefix.\n'
           'For example, resolution_monolithique { dom1 { dom2 dom3 } _dom4 } will solve in a \n'
           'single matrix the equations having dom1 as application domain, then the equations \n'
           'having dom2 or dom3 as application domain in a single matrix, then the equations having \n'
           'dom4 as application domain in a sequential way (not in a single matrix).',
           ''),
  }
  _helpDict = check_append_dict('schema_euler_implicite_Tru', _helpDict, schema_implicite_base_Tru._helpDict)
##### end schema_euler_implicite_Tru class

class champ_front_fonc_xyz_Tru(front_field_base_Tru): #####
  r"""
  Boundary field which is not constant in space.
  """
  # From: line 1338, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('val', 'listchaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1338]
  _infoAttr = {'val': ['TRAD2_example', 1339]}
  _attributesList = check_append_list('champ_front_fonc_xyz_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_fonc_xyz_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_fonc_xyz_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'val':
          ('[listchaine] Values of field components (mathematical expressions).', ''),
  }
  _helpDict = check_append_dict('champ_front_fonc_xyz_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_fonc_xyz_Tru class

class champ_fonc_reprise_Tru(champ_don_base_Tru): #####
  r"""
  This field is used to read a data field in a save file (.xyz or .sauv) at a specified 
  time.
  It is very useful, for example, to run a thermohydraulic calculation with velocity 
  initial condition read into a save file from a previous hydraulic calculation.
  """
  # From: line 1340, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('format', 'chaine_db4169_Tru'),   #
    ('filename', 'chaine_Tru'),   #
    ('pb_name', 'ref_pb_base_Tru'),   #
    ('champ', 'chaine_Tru'),   #
    ('fonction', 'fonction_champ_reprise_Tru'),   #
    ('temps', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'temps': ['time']
  }
  _optionals = set(['format', 'fonction'])
  _infoMain = ['TRAD2_example', 1340]
  _infoAttr = {'champ': ['TRAD2_example', 1344],
   'filename': ['TRAD2_example', 1342],
   'fonction': ['TRAD2_example', 1345],
   'format': ['TRAD2_example', 1341],
   'pb_name': ['TRAD2_example', 1343],
   'temps': ['TRAD2_example', 1346]}
  _attributesList = check_append_list('champ_fonc_reprise_Tru', _attributesList, champ_don_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_fonc_reprise_Tru', _attributesSynonyms, champ_don_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_fonc_reprise_Tru', _infoAttr, champ_don_base_Tru._infoAttr)
  _optionals.update(champ_don_base_Tru._optionals)
  _helpDict = {
    'format':
          ('[chaine(into=["binaire","formatte","xyz","single_hdf"])] Type of file (the file format).\n'
           'If xyz format is activated, the .xyz file from the previous calculation will be given \n'
           'for filename, and if formatte or binaire is choosen, the .sauv file of the previous \n'
           'calculation will be specified for filename.\n'
           'In the case of a parallel calculation, if the mesh partition does not changed between \n'
           'the previous calculation and the next one, the binaire format should be preferred, \n'
           'because is faster than the xyz format.\n'
           'If single_hdf is used, the same constraints/advantages as binaire apply, but a single \n'
           '(HDF5) file is produced on the filesystem instead of having one file per processor.',
           ''),
    'filename':
          ('[chaine] Name of the save file.', ''),
    'pb_name':
          ('[ref_pb_base] Name of the problem.', ''),
    'champ':
          ('[chaine] Name of the problem unknown.\n'
           'It may also be the temporal average of a problem unknown (like moyenne_vitesse, '
           'moyenne_temperature,...)',
           ''),
    'fonction':
          ('[fonction_champ_reprise] Optional keyword to apply a function on the field being read \n'
           'in the save file (e.g.\n'
           'to read a temperature field in Celsius units and convert it for the calculation on \n'
           'Kelvin units, you will use: fonction 1 273.+val )',
           ''),
    'temps':
          ('[chaine] Time of the saved field in the save file or last_time.\n'
           'If you give the keyword last_time instead, the last time saved in the save file will \n'
           'be used.',
           ''),
  }
  _helpDict = check_append_dict('champ_fonc_reprise_Tru', _helpDict, champ_don_base_Tru._helpDict)
##### end champ_fonc_reprise_Tru class

class ecriturelecturespecial_Tru(interprete_Tru): #####
  r"""
  Class to write or not to write a .xyz file on the disk at the end of the calculation.
  """
  # From: line 1347, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('type', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1347]
  _infoAttr = {'type': ['TRAD2_example', 1348]}
  _attributesList = check_append_list('ecriturelecturespecial_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('ecriturelecturespecial_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('ecriturelecturespecial_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'type':
          ('[chaine] If set to 0, no xyz file is created.\n'
           'If set to EFichierBin, it uses prior 1.7.0 way of reading xyz files (now LecFicDiffuseBin).\n'
           'If set to EcrFicPartageBin, it uses prior 1.7.0 way of writing xyz files (now '
           'EcrFicPartageMPIIO).',
           ''),
  }
  _helpDict = check_append_dict('ecriturelecturespecial_Tru', _helpDict, interprete_Tru._helpDict)
##### end ecriturelecturespecial_Tru class

class multiplefiles_Tru(interprete_Tru): #####
  r"""
  Change MPI rank limit for multiple files during I/O
  """
  # From: line 1349, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('type', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1349]
  _infoAttr = {'type': ['TRAD2_example', 1350]}
  _attributesList = check_append_list('multiplefiles_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('multiplefiles_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('multiplefiles_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'type':
          ('[entier] New MPI rank limit', ''),
  }
  _helpDict = check_append_dict('multiplefiles_Tru', _helpDict, interprete_Tru._helpDict)
##### end multiplefiles_Tru class

class disable_tu_Tru(interprete_Tru): #####
  r"""
  Flag to disable the writing of the .TU files
  """
  # From: line 1351, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1351]
  _infoAttr = {}
  _attributesList = check_append_list('disable_tu_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('disable_tu_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('disable_tu_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('disable_tu_Tru', _helpDict, interprete_Tru._helpDict)
##### end disable_tu_Tru class

class refine_mesh_Tru(interprete_Tru): #####
  r"""
  not_set
  """
  # From: line 1352, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('domaine', 'ref_domaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1352]
  _infoAttr = {'domaine': ['TRAD2_example', 1353]}
  _attributesList = check_append_list('refine_mesh_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('refine_mesh_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('refine_mesh_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domaine':
          ('[ref_domaine] not_set', ''),
  }
  _helpDict = check_append_dict('refine_mesh_Tru', _helpDict, interprete_Tru._helpDict)
##### end refine_mesh_Tru class

class extrudebord_Tru(interprete_Tru): #####
  r"""
  Class to generate an extruded mesh from a boundary of a tetrahedral or an hexahedral 
  mesh.
  
  Warning: If the initial domain is a tetrahedral mesh, the boundary will be moved 
  in the XY plane then extrusion will be applied (you should maybe use the Transformer 
  keyword on the final domain to have the domain you really want).
  You can use the keyword Ecrire_Fichier_Meshtv to generate a meshtv file to visualize 
  your initial and final meshes.
  
  This keyword can be used for example to create a periodic box extracted from a boundary 
  of a tetrahedral or a hexaedral mesh.
  This periodic box may be used then to engender turbulent inlet flow condition for 
  the main domain.NL2 Note that ExtrudeBord in VEF generates 3 or 14 tetrahedra from 
  extruded prisms.
  """
  # From: line 1354, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('domaine_init', 'ref_domaine_Tru'),   #
    ('direction', 'listf_Tru'),   #
    ('nb_tranches', 'entier_3282e0_Tru'),   #
    ('domaine_final', 'chaine_Tru'),   #
    ('nom_bord', 'chaine_Tru'),   #
    ('hexa_old', 'rien_Tru'),   #
    ('trois_tetra', 'rien_Tru'),   #
    ('vingt_tetra', 'rien_Tru'),   #
    ('sans_passer_par_le2d', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['hexa_old', 'trois_tetra', 'vingt_tetra', 'sans_passer_par_le2d'])
  _infoMain = ['TRAD2_example', 1354]
  _infoAttr = {'direction': ['TRAD2_example', 1356],
   'domaine_final': ['TRAD2_example', 1358],
   'domaine_init': ['TRAD2_example', 1355],
   'hexa_old': ['TRAD2_example', 1360],
   'nb_tranches': ['TRAD2_example', 1357],
   'nom_bord': ['TRAD2_example', 1359],
   'sans_passer_par_le2d': ['TRAD2_example', 1363],
   'trois_tetra': ['TRAD2_example', 1361],
   'vingt_tetra': ['TRAD2_example', 1362]}
  _attributesList = check_append_list('extrudebord_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('extrudebord_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('extrudebord_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domaine_init':
          ('[ref_domaine] Initial domain with hexaedras or tetrahedras.', ''),
    'direction':
          ('[listf] Directions for the extrusion.', ''),
    'nb_tranches':
          ('[entier] Number of elements in the extrusion direction.', ''),
    'domaine_final':
          ('[chaine] Extruded domain.', ''),
    'nom_bord':
          ('[chaine] Name of the boundary of the initial domain where extrusion will be applied.', ''),
    'hexa_old':
          ('[rien] Old algorithm for boundary extrusion from a hexahedral mesh.', ''),
    'trois_tetra':
          ('[rien] To extrude in 3 tetrahedras instead of 14 tetrahedras.', ''),
    'vingt_tetra':
          ('[rien] To extrude in 20 tetrahedras instead of 14 tetrahedras.', ''),
    'sans_passer_par_le2d':
          ('[entier] Only for non-regression', ''),
  }
  _helpDict = check_append_dict('extrudebord_Tru', _helpDict, interprete_Tru._helpDict)
##### end extrudebord_Tru class

class polyedriser_Tru(interprete_Tru): #####
  r"""
  cast hexahedra into polyhedra so that the indexing of the mesh vertices is compatible 
  with PolyMAC_P0P1NC discretization.
  Must be used in PolyMAC_P0P1NC discretization if a hexahedral mesh has been produced 
  with TRUST's internal mesh generator.
  """
  # From: line 1365, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1365]
  _infoAttr = {'domain_name': ['TRAD2_example', 1366]}
  _attributesList = check_append_list('polyedriser_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('polyedriser_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('polyedriser_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of domain.', ''),
  }
  _helpDict = check_append_dict('polyedriser_Tru', _helpDict, interprete_Tru._helpDict)
##### end polyedriser_Tru class

class corriger_frontiere_periodique_Tru(interprete_Tru): #####
  r"""
  The Corriger_frontiere_periodique keyword is mandatory to first define the periodic 
  boundaries, to reorder the faces and eventually fix unaligned nodes of these boundaries.
  Faces on one side of the periodic domain are put first, then the faces on the opposite 
  side, in the same order.
  It must be run in sequential before mesh splitting.
  """
  # From: line 1367, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('domaine', 'chaine_Tru'),   #
    ('bord', 'chaine_Tru'),   #
    ('direction', 'list_Tru'),   #
    ('fichier_post', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['direction', 'fichier_post'])
  _infoMain = ['TRAD2_example', 1367]
  _infoAttr = {'bord': ['TRAD2_example', 1369],
   'direction': ['TRAD2_example', 1370],
   'domaine': ['TRAD2_example', 1368],
   'fichier_post': ['TRAD2_example', 1371]}
  _attributesList = check_append_list('corriger_frontiere_periodique_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('corriger_frontiere_periodique_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('corriger_frontiere_periodique_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domaine':
          ('[chaine] Name of domain.', ''),
    'bord':
          ('[chaine] the name of the boundary (which must contain two opposite sides of the domain)', ''),
    'direction':
          ('[list] defines the periodicity direction vector (a vector that points from one node \n'
           'on one side to the opposite node on the other side).\n'
           'This vector must be given if the automatic algorithm fails, that is:NL2 - when the \n'
           'node coordinates are not perfectly periodic\n'
           '\n'
           '- when the periodic direction is not aligned with the normal vector of the boundary \n'
           'faces',
           ''),
    'fichier_post':
          ('[chaine] .', ''),
  }
  _helpDict = check_append_dict('corriger_frontiere_periodique_Tru', _helpDict, interprete_Tru._helpDict)
##### end corriger_frontiere_periodique_Tru class

class bloc_pave_Tru(objet_lecture_Tru): #####
  r"""
  Class to create a pave.
  """
  # From: line 1372, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('origine', 'listf_Tru'),   #
    ('longueurs', 'listf_Tru'),   #
    ('nombre_de_noeuds', 'listentierf_Tru'),   #
    ('facteurs', 'listf_Tru'),   #
    ('symx', 'rien_Tru'),   #
    ('symy', 'rien_Tru'),   #
    ('symz', 'rien_Tru'),   #
    ('xtanh', 'floattant_820d8b_Tru'),   #
    ('xtanh_dilatation', 'entier_5e32c8_Tru'),   #
    ('xtanh_taille_premiere_maille', 'floattant_820d8b_Tru'),   #
    ('ytanh', 'floattant_820d8b_Tru'),   #
    ('ytanh_dilatation', 'entier_5e32c8_Tru'),   #
    ('ytanh_taille_premiere_maille', 'floattant_820d8b_Tru'),   #
    ('ztanh', 'floattant_820d8b_Tru'),   #
    ('ztanh_dilatation', 'entier_5e32c8_Tru'),   #
    ('ztanh_taille_premiere_maille', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['origine',
   'longueurs',
   'nombre_de_noeuds',
   'facteurs',
   'symx',
   'symy',
   'symz',
   'xtanh',
   'xtanh_dilatation',
   'xtanh_taille_premiere_maille',
   'ytanh',
   'ytanh_dilatation',
   'ytanh_taille_premiere_maille',
   'ztanh',
   'ztanh_dilatation',
   'ztanh_taille_premiere_maille'])
  _infoMain = ['TRAD2_example', 1372]
  _infoAttr = {'facteurs': ['TRAD2_example', 1376],
   'longueurs': ['TRAD2_example', 1374],
   'nombre_de_noeuds': ['TRAD2_example', 1375],
   'origine': ['TRAD2_example', 1373],
   'symx': ['TRAD2_example', 1377],
   'symy': ['TRAD2_example', 1378],
   'symz': ['TRAD2_example', 1379],
   'xtanh': ['TRAD2_example', 1380],
   'xtanh_dilatation': ['TRAD2_example', 1381],
   'xtanh_taille_premiere_maille': ['TRAD2_example', 1382],
   'ytanh': ['TRAD2_example', 1383],
   'ytanh_dilatation': ['TRAD2_example', 1384],
   'ytanh_taille_premiere_maille': ['TRAD2_example', 1385],
   'ztanh': ['TRAD2_example', 1386],
   'ztanh_dilatation': ['TRAD2_example', 1387],
   'ztanh_taille_premiere_maille': ['TRAD2_example', 1388]}
  _attributesList = check_append_list('bloc_pave_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('bloc_pave_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('bloc_pave_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'origine':
          ('[listf] Keyword to define the pave (block) origin, that is to say one of the 8 block \n'
           'points (or 4 in a 2D coordinate system).',
           ''),
    'longueurs':
          ('[listf] Keyword to define the block dimensions, that is to say knowing the origin, \n'
           'length along the axes.',
           ''),
    'nombre_de_noeuds':
          ('[listentierf] Keyword to define the discretization (nodenumber) in each direction.', ''),
    'facteurs':
          ('[listf] Keyword to define stretching factors for mesh discretization in each direction.\n'
           'This is a real number which must be positive (by default 1.0).\n'
           'A stretching factor other than 1 allows refinement on one edge in one direction.',
           ''),
    'symx':
          ('[rien] Keyword to define a block mesh that is symmetrical with respect to the YZ plane \n'
           '(respectively Y-axis in 2D) passing through the block centre.',
           ''),
    'symy':
          ('[rien] Keyword to define a block mesh that is symmetrical with respect to the XZ plane \n'
           '(respectively X-axis in 2D) passing through the block centre.',
           ''),
    'symz':
          ('[rien] Keyword defining a block mesh that is symmetrical with respect to the XY plane \n'
           'passing through the block centre.',
           ''),
    'xtanh':
          ('[floattant] Keyword to generate mesh with tanh (hyperbolic tangent) variation in the \n'
           'X-direction.',
           ''),
    'xtanh_dilatation':
          ('[entier(into=[-1,0,1])] Keyword to generate mesh with tanh (hyperbolic tangent) variation \n'
           'in the X-direction.\n'
           'xtanh_dilatation: The value may be -1,0,1 (0 by default): 0: coarse mesh at the middle \n'
           'of the channel and smaller near the walls -1: coarse mesh at the left side of the \n'
           'channel and smaller at the right side 1: coarse mesh at the right side of the channel \n'
           'and smaller near the left side of the channel.',
           ''),
    'xtanh_taille_premiere_maille':
          ('[floattant] Size of the first cell of the mesh with tanh (hyperbolic tangent) variation \n'
           'in the X-direction.',
           ''),
    'ytanh':
          ('[floattant] Keyword to generate mesh with tanh (hyperbolic tangent) variation in the \n'
           'Y-direction.',
           ''),
    'ytanh_dilatation':
          ('[entier(into=[-1,0,1])] Keyword to generate mesh with tanh (hyperbolic tangent) variation \n'
           'in the Y-direction.\n'
           'ytanh_dilatation: The value may be -1,0,1 (0 by default): 0: coarse mesh at the middle \n'
           'of the channel and smaller near the walls -1: coarse mesh at the bottom of the channel \n'
           'and smaller near the top 1: coarse mesh at the top of the channel and smaller near \n'
           'the bottom.',
           ''),
    'ytanh_taille_premiere_maille':
          ('[floattant] Size of the first cell of the mesh with tanh (hyperbolic tangent) variation \n'
           'in the Y-direction.',
           ''),
    'ztanh':
          ('[floattant] Keyword to generate mesh with tanh (hyperbolic tangent) variation in the \n'
           'Z-direction.',
           ''),
    'ztanh_dilatation':
          ('[entier(into=[-1,0,1])] Keyword to generate mesh with tanh (hyperbolic tangent) variation \n'
           'in the Z-direction.\n'
           'tanh_dilatation: The value may be -1,0,1 (0 by default): 0: coarse mesh at the middle \n'
           'of the channel and smaller near the walls -1: coarse mesh at the back of the channel \n'
           'and smaller near the front 1: coarse mesh at the front of the channel and smaller \n'
           'near the back.',
           ''),
    'ztanh_taille_premiere_maille':
          ('[floattant] Size of the first cell of the mesh with tanh (hyperbolic tangent) variation \n'
           'in the Z-direction.',
           ''),
  }
  _helpDict = check_append_dict('bloc_pave_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end bloc_pave_Tru class

class raffiner_isotrope_parallele_Tru(interprete_Tru): #####
  r"""
  Refine parallel mesh in parallel
  """
  # From: line 1389, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('name_of_initial_domaines', 'chaine_Tru'),   #
    ('name_of_new_domaines', 'chaine_Tru'),   #
    ('ascii', 'rien_Tru'),   #
    ('single_hdf', 'rien_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'name_of_initial_domaines': ['name_of_initial_zones'],
    'name_of_new_domaines': ['name_of_new_zones']
  }
  _optionals = set(['ascii', 'single_hdf'])
  _infoMain = ['TRAD2_example', 1389]
  _infoAttr = {'ascii': ['TRAD2_example', 1392],
   'name_of_initial_domaines': ['TRAD2_example', 1390],
   'name_of_new_domaines': ['TRAD2_example', 1391],
   'single_hdf': ['TRAD2_example', 1393]}
  _attributesList = check_append_list('raffiner_isotrope_parallele_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('raffiner_isotrope_parallele_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('raffiner_isotrope_parallele_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'name_of_initial_domaines':
          ('[chaine] name of initial Domaines', ''),
    'name_of_new_domaines':
          ('[chaine] name of new Domaines', ''),
    'ascii':
          ('[rien] writing Domaines in ascii format', ''),
    'single_hdf':
          ('[rien] writing Domaines in hdf format', ''),
  }
  _helpDict = check_append_dict('raffiner_isotrope_parallele_Tru', _helpDict, interprete_Tru._helpDict)
##### end raffiner_isotrope_parallele_Tru class

class modifydomaineaxi1d_Tru(interprete_Tru): #####
  r"""
  Convert a 1D mesh to 1D axisymmetric mesh
  """
  # From: line 1398, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['convert_1d_to_1daxi_Tru']
  _attributesList = [
    ('dom', 'chaine_Tru'),   #
    ('bloc', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1398]
  _infoAttr = {'bloc': ['TRAD2_example', 1400], 'dom': ['TRAD2_example', 1399]}
  _attributesList = check_append_list('modifydomaineaxi1d_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('modifydomaineaxi1d_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('modifydomaineaxi1d_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'dom':
          ('[chaine] not_set', ''),
    'bloc':
          ('[bloc_lecture] not_set', ''),
  }
  _helpDict = check_append_dict('modifydomaineaxi1d_Tru', _helpDict, interprete_Tru._helpDict)
##### end modifydomaineaxi1d_Tru class

class domaineaxi1d_Tru(domaine_Tru): #####
  r"""
  1D domain
  """
  # From: line 1401, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1401]
  _infoAttr = {}
  _attributesList = check_append_list('domaineaxi1d_Tru', _attributesList, domaine_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('domaineaxi1d_Tru', _attributesSynonyms, domaine_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('domaineaxi1d_Tru', _infoAttr, domaine_Tru._infoAttr)
  _optionals.update(domaine_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('domaineaxi1d_Tru', _helpDict, domaine_Tru._helpDict)
##### end domaineaxi1d_Tru class

class interpolation_ibm_base_Tru(ConstrainBase_Tru): #####
  r"""
  Base class for all the interpolation methods available in the Immersed Boundary Method 
  (IBM).
  """
  # From: line 1402, in file 'TRAD2_example'
  _braces = 0
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = [
    ('impr', 'rien_Tru'),   #
    ('nb_histo_boxes_impr', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['impr', 'nb_histo_boxes_impr'])
  _infoMain = ['TRAD2_example', 1402]
  _infoAttr = {'impr': ['TRAD2_example', 1403],
   'nb_histo_boxes_impr': ['TRAD2_example', 1404]}
  _helpDict = {
    'impr':
          ('[rien] To print IBM-related data', ''),
    'nb_histo_boxes_impr':
          ('[entier] number of histogram boxes for printed data', ''),
  }
##### end interpolation_ibm_base_Tru class

class interpolation_ibm_aucune_Tru(interpolation_ibm_base_Tru): #####
  r"""
  Immersed Boundary Method (IBM): no interpolation.
  """
  # From: line 1405, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['ibm_aucune_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1405]
  _infoAttr = {}
  _attributesList = check_append_list('interpolation_ibm_aucune_Tru', _attributesList, interpolation_ibm_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('interpolation_ibm_aucune_Tru', _attributesSynonyms, interpolation_ibm_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('interpolation_ibm_aucune_Tru', _infoAttr, interpolation_ibm_base_Tru._infoAttr)
  _optionals.update(interpolation_ibm_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('interpolation_ibm_aucune_Tru', _helpDict, interpolation_ibm_base_Tru._helpDict)
##### end interpolation_ibm_aucune_Tru class

class interpolation_ibm_elem_fluid_Tru(interpolation_ibm_base_Tru): #####
  r"""
  Immersed Boundary Method (IBM): fluid element interpolation.
  """
  # From: line 1406, in file 'TRAD2_example'
  _braces = 1
  _synonyms = ['interpolation_ibm_element_fluide_Tru', 'ibm_element_fluide_Tru']
  _attributesList = [
    ('points_fluides', '<inherited>field_base_Tru'),   #
    ('points_solides', '<inherited>field_base_Tru'),   #
    ('elements_fluides', '<inherited>field_base_Tru'),   #
    ('correspondance_elements', '<inherited>field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1406]
  _infoAttr = {'correspondance_elements': ['TRAD2_example', 1410],
   'elements_fluides': ['TRAD2_example', 1409],
   'points_fluides': ['TRAD2_example', 1407],
   'points_solides': ['TRAD2_example', 1408]}
  _attributesList = check_append_list('interpolation_ibm_elem_fluid_Tru', _attributesList, interpolation_ibm_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('interpolation_ibm_elem_fluid_Tru', _attributesSynonyms, interpolation_ibm_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('interpolation_ibm_elem_fluid_Tru', _infoAttr, interpolation_ibm_base_Tru._infoAttr)
  _optionals.update(interpolation_ibm_base_Tru._optionals)
  _helpDict = {
    'points_fluides':
          ('[field_base] Node field giving the projection of the point below (points_solides) \n'
           'falling into the pure cell fluid',
           ''),
    'points_solides':
          ('[field_base] Node field giving the projection of the node on the immersed boundary', ''),
    'elements_fluides':
          ('[field_base] Node field giving the number of the element (cell) containing the pure \n'
           'fluid point',
           ''),
    'correspondance_elements':
          ('[field_base] Cell field giving the SALOME cell number', ''),
  }
  _helpDict = check_append_dict('interpolation_ibm_elem_fluid_Tru', _helpDict, interpolation_ibm_base_Tru._helpDict)
##### end interpolation_ibm_elem_fluid_Tru class

class interpolation_ibm_hybride_Tru(interpolation_ibm_elem_fluid_Tru): #####
  r"""
  Immersed Boundary Method (IBM): hybrid (fluid/mean gradient) interpolation.
  """
  # From: line 1411, in file 'TRAD2_example'
  _braces = 1
  _synonyms = ['ibm_hybride_Tru']
  _attributesList = [
    ('est_dirichlet', '<inherited>field_base_Tru'),   #
    ('elements_solides', '<inherited>field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1411]
  _infoAttr = {'elements_solides': ['TRAD2_example', 1413],
   'est_dirichlet': ['TRAD2_example', 1412]}
  _attributesList = check_append_list('interpolation_ibm_hybride_Tru', _attributesList, interpolation_ibm_elem_fluid_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('interpolation_ibm_hybride_Tru', _attributesSynonyms, interpolation_ibm_elem_fluid_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('interpolation_ibm_hybride_Tru', _infoAttr, interpolation_ibm_elem_fluid_Tru._infoAttr)
  _optionals.update(interpolation_ibm_elem_fluid_Tru._optionals)
  _helpDict = {
    'est_dirichlet':
          ('[field_base] Node field of booleans indicating whether the node belong to an element \n'
           'where the interface is',
           ''),
    'elements_solides':
          ('[field_base] Node field giving the element number containing the solid point', ''),
  }
  _helpDict = check_append_dict('interpolation_ibm_hybride_Tru', _helpDict, interpolation_ibm_elem_fluid_Tru._helpDict)
##### end interpolation_ibm_hybride_Tru class

class interpolation_ibm_mean_gradient_Tru(interpolation_ibm_base_Tru): #####
  r"""
  Immersed Boundary Method (IBM): mean gradient interpolation.
  """
  # From: line 1414, in file 'TRAD2_example'
  _braces = 1
  _synonyms = ['ibm_gradient_moyen_Tru', 'interpolation_ibm_gradient_moyen_Tru']
  _attributesList = [
    ('points_solides', '<inherited>field_base_Tru'),   #
    ('est_dirichlet', '<inherited>field_base_Tru'),   #
    ('correspondance_elements', '<inherited>field_base_Tru'),   #
    ('elements_solides', '<inherited>field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1414]
  _infoAttr = {'correspondance_elements': ['TRAD2_example', 1417],
   'elements_solides': ['TRAD2_example', 1418],
   'est_dirichlet': ['TRAD2_example', 1416],
   'points_solides': ['TRAD2_example', 1415]}
  _attributesList = check_append_list('interpolation_ibm_mean_gradient_Tru', _attributesList, interpolation_ibm_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('interpolation_ibm_mean_gradient_Tru', _attributesSynonyms, interpolation_ibm_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('interpolation_ibm_mean_gradient_Tru', _infoAttr, interpolation_ibm_base_Tru._infoAttr)
  _optionals.update(interpolation_ibm_base_Tru._optionals)
  _helpDict = {
    'points_solides':
          ('[field_base] Node field giving the projection of the node on the immersed boundary', ''),
    'est_dirichlet':
          ('[field_base] Node field of booleans indicating whether the node belong to an element \n'
           'where the interface is',
           ''),
    'correspondance_elements':
          ('[field_base] Cell field giving the SALOME cell number', ''),
    'elements_solides':
          ('[field_base] Node field giving the element number containing the solid point', ''),
  }
  _helpDict = check_append_dict('interpolation_ibm_mean_gradient_Tru', _helpDict, interpolation_ibm_base_Tru._helpDict)
##### end interpolation_ibm_mean_gradient_Tru class

class interpolation_ibm_power_law_tbl_Tru(interpolation_ibm_elem_fluid_Tru): #####
  r"""
  Immersed Boundary Method (IBM): power law interpolation.
  """
  # From: line 1419, in file 'TRAD2_example'
  _braces = 1
  _synonyms = ['ibm_power_law_tbl_Tru']
  _attributesList = [
    ('formulation_linear_pwl', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['formulation_linear_pwl'])
  _infoMain = ['TRAD2_example', 1419]
  _infoAttr = {'formulation_linear_pwl': ['TRAD2_example', 1420]}
  _attributesList = check_append_list('interpolation_ibm_power_law_tbl_Tru', _attributesList, interpolation_ibm_elem_fluid_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('interpolation_ibm_power_law_tbl_Tru', _attributesSynonyms, interpolation_ibm_elem_fluid_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('interpolation_ibm_power_law_tbl_Tru', _infoAttr, interpolation_ibm_elem_fluid_Tru._infoAttr)
  _optionals.update(interpolation_ibm_elem_fluid_Tru._optionals)
  _helpDict = {
    'formulation_linear_pwl':
          ('[entier] Choix formulation lineaire ou non', ''),
  }
  _helpDict = check_append_dict('interpolation_ibm_power_law_tbl_Tru', _helpDict, interpolation_ibm_elem_fluid_Tru._helpDict)
##### end interpolation_ibm_power_law_tbl_Tru class

class interpolation_ibm_power_law_tbl_u_star_Tru(interpolation_ibm_mean_gradient_Tru): #####
  r"""
  Immersed Boundary Method (IBM): law u star.
  """
  # From: line 1421, in file 'TRAD2_example'
  _braces = 1
  _synonyms = ['ibm_power_law_tbl_u_star_Tru']
  _attributesList = [
    ('points_solides', '<inherited>field_base_Tru'),   #
    ('est_dirichlet', '<inherited>field_base_Tru'),   #
    ('correspondance_elements', '<inherited>field_base_Tru'),   #
    ('elements_solides', '<inherited>field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1421]
  _infoAttr = {'correspondance_elements': ['TRAD2_example', 1424],
   'elements_solides': ['TRAD2_example', 1425],
   'est_dirichlet': ['TRAD2_example', 1423],
   'points_solides': ['TRAD2_example', 1422]}
  _attributesList = check_append_list('interpolation_ibm_power_law_tbl_u_star_Tru', _attributesList, interpolation_ibm_mean_gradient_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('interpolation_ibm_power_law_tbl_u_star_Tru', _attributesSynonyms, interpolation_ibm_mean_gradient_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('interpolation_ibm_power_law_tbl_u_star_Tru', _infoAttr, interpolation_ibm_mean_gradient_Tru._infoAttr)
  _optionals.update(interpolation_ibm_mean_gradient_Tru._optionals)
  _helpDict = {
    'points_solides':
          ('[field_base] Node field giving the projection of the node on the immersed boundary', ''),
    'est_dirichlet':
          ('[field_base] Node field of booleans indicating whether the node belong to an element \n'
           'where the interface is',
           ''),
    'correspondance_elements':
          ('[field_base] Cell field giving the SALOME cell number', ''),
    'elements_solides':
          ('[field_base] Node field giving the element number containing the solid point', ''),
  }
  _helpDict = check_append_dict('interpolation_ibm_power_law_tbl_u_star_Tru', _helpDict, interpolation_ibm_mean_gradient_Tru._helpDict)
##### end interpolation_ibm_power_law_tbl_u_star_Tru class

class partition_multi_Tru(interprete_Tru): #####
  r"""
  allows to partition multiple domains in contact with each other in parallel: necessary 
  for resolution monolithique in implicit schemes and for all coupled problems using 
  PolyMAC_P0P1NC.
  By default, this keyword is commented in the reference test cases.
  """
  # From: line 1426, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['decouper_multi_Tru']
  _attributesList = [
    ('aco', 'chaine_e3a651_Tru'),   #
    ('domaine1', 'chaine_ae5ad3_Tru'),   #
    ('dom', 'ref_domaine_Tru'),   #
    ('blocdecoupdom1', 'bloc_decouper_Tru'),   #
    ('domaine2', 'chaine_ae5ad3_Tru'),   #
    ('dom2', 'ref_domaine_Tru'),   #
    ('blocdecoupdom2', 'bloc_decouper_Tru'),   #
    ('acof', 'chaine_af6447_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1426]
  _infoAttr = {'aco': ['TRAD2_example', 1427],
   'acof': ['TRAD2_example', 1434],
   'blocdecoupdom1': ['TRAD2_example', 1430],
   'blocdecoupdom2': ['TRAD2_example', 1433],
   'dom': ['TRAD2_example', 1429],
   'dom2': ['TRAD2_example', 1432],
   'domaine1': ['TRAD2_example', 1428],
   'domaine2': ['TRAD2_example', 1431]}
  _attributesList = check_append_list('partition_multi_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('partition_multi_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('partition_multi_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'aco':
          ('[chaine(into=["{"])] Opening curly bracket.', ''),
    'domaine1':
          ('[chaine(into=["domaine"])] not set.', ''),
    'dom':
          ('[ref_domaine] Name of the first domain to be cut.', ''),
    'blocdecoupdom1':
          ('[bloc_decouper] Partition bloc for the first domain.', ''),
    'domaine2':
          ('[chaine(into=["domaine"])] not set.', ''),
    'dom2':
          ('[ref_domaine] Name of the second domain to be cut.', ''),
    'blocdecoupdom2':
          ('[bloc_decouper] Partition bloc for the second domain.', ''),
    'acof':
          ('[chaine(into=["}"])] Closing curly bracket.', ''),
  }
  _helpDict = check_append_dict('partition_multi_Tru', _helpDict, interprete_Tru._helpDict)
##### end partition_multi_Tru class

class partitionneur_fichier_med_Tru(partitionneur_deriv_Tru): #####
  r"""
  Partitioning a domain using a MED file containing an integer field providing for each 
  element the processor number on which the element should be located.
  """
  # From: line 1435, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['fichier_med_Tru']
  _attributesList = [
    ('file', 'chaine_Tru'),   #
    ('field', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1435]
  _infoAttr = {'field': ['TRAD2_example', 1437], 'file': ['TRAD2_example', 1436]}
  _attributesList = check_append_list('partitionneur_fichier_med_Tru', _attributesList, partitionneur_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('partitionneur_fichier_med_Tru', _attributesSynonyms, partitionneur_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('partitionneur_fichier_med_Tru', _infoAttr, partitionneur_deriv_Tru._infoAttr)
  _optionals.update(partitionneur_deriv_Tru._optionals)
  _helpDict = {
    'file':
          ('[chaine] file name of the MED file to load', ''),
    'field':
          ('[chaine] field name of the integer field to load', ''),
  }
  _helpDict = check_append_dict('partitionneur_fichier_med_Tru', _helpDict, partitionneur_deriv_Tru._helpDict)
##### end partitionneur_fichier_med_Tru class

class partitionneur_sous_dom_Tru(partitionneur_deriv_Tru): #####
  r"""
  Given a global partition of a global domain, 'sous-domaine' allows to produce a conform 
  partition of a sub-domain generated from the bigger one using the keyword create_domain_from_sous_domaine.
  The sub-domain will be partitionned in a conform fashion with the global domain.
  """
  # From: line 1438, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['sous_dom_Tru']
  _attributesList = [
    ('fichier', 'chaine_Tru'),   #
    ('fichier_ssz', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1438]
  _infoAttr = {'fichier': ['TRAD2_example', 1439], 'fichier_ssz': ['TRAD2_example', 1440]}
  _attributesList = check_append_list('partitionneur_sous_dom_Tru', _attributesList, partitionneur_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('partitionneur_sous_dom_Tru', _attributesSynonyms, partitionneur_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('partitionneur_sous_dom_Tru', _infoAttr, partitionneur_deriv_Tru._infoAttr)
  _optionals.update(partitionneur_deriv_Tru._optionals)
  _helpDict = {
    'fichier':
          ('[chaine] fichier', ''),
    'fichier_ssz':
          ('[chaine] fichier sous zonne', ''),
  }
  _helpDict = check_append_dict('partitionneur_sous_dom_Tru', _helpDict, partitionneur_deriv_Tru._helpDict)
##### end partitionneur_sous_dom_Tru class

class partitionneur_sous_domaines_Tru(partitionneur_deriv_Tru): #####
  r"""
  This algorithm will create one part for each specified subdomaine/domain.
  All elements contained in the first subdomaine/domain are put in the first part, 
  all remaining elements contained in the second subdomaine/domain in the second part, 
  etc...
  
  If all elements of the current domain are contained in the specified subdomaines/domain, 
  then N parts are created, otherwise, a supplemental part is created with the remaining 
  elements.
  
  If no subdomaine is specified, all subdomaines defined in the domain are used to 
  split the mesh.
  """
  # From: line 1441, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['partitionneur_sous_zones_Tru']
  _attributesList = [
    ('sous_zones', 'listchaine_Tru'),   #
    ('domaines', 'listchaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['sous_zones', 'domaines'])
  _infoMain = ['TRAD2_example', 1441]
  _infoAttr = {'domaines': ['TRAD2_example', 1443], 'sous_zones': ['TRAD2_example', 1442]}
  _attributesList = check_append_list('partitionneur_sous_domaines_Tru', _attributesList, partitionneur_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('partitionneur_sous_domaines_Tru', _attributesSynonyms, partitionneur_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('partitionneur_sous_domaines_Tru', _infoAttr, partitionneur_deriv_Tru._infoAttr)
  _optionals.update(partitionneur_deriv_Tru._optionals)
  _helpDict = {
    'sous_zones':
          ('[listchaine] N SUBZONE_NAME_1 SUBZONE_NAME_2 ...', ''),
    'domaines':
          ('[listchaine] N DOMAIN_NAME_1 DOMAIN_NAME_2 ...', ''),
  }
  _helpDict = check_append_dict('partitionneur_sous_domaines_Tru', _helpDict, partitionneur_deriv_Tru._helpDict)
##### end partitionneur_sous_domaines_Tru class

class partition_Tru(interprete_Tru): #####
  r"""
  Class for parallel calculation to cut a domain for each processor.
  By default, this keyword is commented in the reference test cases.
  """
  # From: line 1444, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['decouper_Tru']
  _attributesList = [
    ('domaine', 'ref_domaine_Tru'),   #
    ('bloc_decouper', 'bloc_decouper_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1444]
  _infoAttr = {'bloc_decouper': ['TRAD2_example', 1446], 'domaine': ['TRAD2_example', 1445]}
  _attributesList = check_append_list('partition_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('partition_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('partition_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domaine':
          ('[ref_domaine] Name of the domain to be cut.', ''),
    'bloc_decouper':
          ('[bloc_decouper] Description how to cut a domain.', ''),
  }
  _helpDict = check_append_dict('partition_Tru', _helpDict, interprete_Tru._helpDict)
##### end partition_Tru class

class bloc_decouper_Tru(objet_lecture_Tru): #####
  r"""
  Auxiliary class to cut a domain.
  """
  # From: line 1447, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('partitionneur', '<inherited>partitionneur_deriv_Tru'),   #
    ('larg_joint', 'entier_3282e0_Tru'),   #
    ('nom_zones', 'chaine_Tru'),   #
    ('ecrire_decoupage', 'chaine_Tru'),   #
    ('ecrire_lata', 'chaine_Tru'),   #
    ('nb_parts_tot', 'entier_3282e0_Tru'),   #
    ('periodique', 'listchaine_Tru'),   #
    ('reorder', 'entier_3282e0_Tru'),   #
    ('single_hdf', 'rien_Tru'),   #
    ('print_more_infos', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'nom_zones': ['zones_name'], 'partitionneur': ['partition_tool']
  }
  _optionals = set(['partitionneur',
   'larg_joint',
   'nom_zones',
   'ecrire_decoupage',
   'ecrire_lata',
   'nb_parts_tot',
   'periodique',
   'reorder',
   'single_hdf',
   'print_more_infos'])
  _infoMain = ['TRAD2_example', 1447]
  _infoAttr = {'ecrire_decoupage': ['TRAD2_example', 1451],
   'ecrire_lata': ['TRAD2_example', 1452],
   'larg_joint': ['TRAD2_example', 1449],
   'nb_parts_tot': ['TRAD2_example', 1453],
   'nom_zones': ['TRAD2_example', 1450],
   'partitionneur': ['TRAD2_example', 1448],
   'periodique': ['TRAD2_example', 1454],
   'print_more_infos': ['TRAD2_example', 1457],
   'reorder': ['TRAD2_example', 1455],
   'single_hdf': ['TRAD2_example', 1456]}
  _attributesList = check_append_list('bloc_decouper_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('bloc_decouper_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('bloc_decouper_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'partitionneur':
          ('[partitionneur_deriv] Defines the partitionning algorithm (the effective C++ object \n'
           "used is 'Partitionneur_ALGORITHM_NAME').",
           ''),
    'larg_joint':
          ('[entier] This keyword specifies the thickness of the virtual ghost domaine (data known \n'
           'by one processor though not owned by it).\n'
           'The default value is 1 and is generally correct for all algorithms except the QUICK \n'
           'convection scheme that require a thickness of 2.\n'
           'Since the 1.5.5 version, the VEF discretization imply also a thickness of 2 (except \n'
           'VEF P0).\n'
           'Any non-zero positive value can be used, but the amount of data to store and exchange \n'
           'between processors grows quickly with the thickness.',
           ''),
    'nom_zones':
          ('[chaine] Name of the files containing the different partition of the domain.\n'
           'The files will be :\n'
           '\n'
           'name_0001.Zones\n'
           '\n'
           'name_0002.Zones\n'
           '\n'
           '...\n'
           '\n'
           'name_000n.Zones.\n'
           'If this keyword is not specified, the geometry is not written on disk (you might \n'
           "just want to generate a 'ecrire_decoupage' or 'ecrire_lata').",
           ''),
    'ecrire_decoupage':
          ('[chaine] After having called the partitionning algorithm, the resulting partition \n'
           'is written on disk in the specified filename.\n'
           'See also partitionneur Fichier_Decoupage.\n'
           'This keyword is useful to change the partition numbers: first, you write the partition \n'
           'into a file with the option ecrire_decoupage.\n'
           "This file contains the domaine number for each element's mesh.\n"
           'Then you can easily permute domaine numbers in this file.\n'
           'Then read the new partition to create the .Zones files with the Fichier_Decoupage \n'
           'keyword.',
           ''),
    'ecrire_lata':
          ('[chaine] not_set', ''),
    'nb_parts_tot':
          ('[entier] Keyword to generates N .Domaine files, instead of the default number M obtained \n'
           'after the partitionning algorithm.\n'
           'N must be greater or equal to M.\n'
           'This option might be used to perform coupled parallel computations.\n'
           'Supplemental empty domaines from M to N-1 are created.\n'
           'This keyword is used when you want to run a parallel calculation on several domains \n'
           'with for example, 2 processors on a first domain and 10 on the second domain because \n'
           'the first domain is very small compare to second one.\n'
           'You will write Nb_parts 2 and Nb_parts_tot 10 for the first domain and Nb_parts 10 \n'
           'for the second domain.',
           ''),
    'periodique':
          ('[listchaine] N BOUNDARY_NAME_1 BOUNDARY_NAME_2 ...\n'
           ': N is the number of boundary names given.\n'
           'Periodic boundaries must be declared by this method.\n'
           'The partitionning algorithm will ensure that facing nodes and faces in the periodic \n'
           'boundaries are located on the same processor.',
           ''),
    'reorder':
          ('[entier] If this option is set to 1 (0 by default), the partition is renumbered in \n'
           'order that the processes which communicate the most are nearer on the network.\n'
           'This may slighlty improves parallel performance.',
           ''),
    'single_hdf':
          ('[rien] Optional keyword to enable you to write the partitioned domaines in a single \n'
           'file in hdf5 format.',
           ''),
    'print_more_infos':
          ('[entier] If this option is set to 1 (0 by default), print infos about number of remote \n'
           'elements (ghosts) and additional infos about the quality of partitionning.\n'
           'Warning, it slows down the cutting operations.',
           ''),
  }
  _helpDict = check_append_dict('bloc_decouper_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end bloc_decouper_Tru class

class partitionneur_union_Tru(partitionneur_deriv_Tru): #####
  r"""
  Let several local domains be generated from a bigger one using the keyword create_domain_from_sous_domaine, 
  and let their partitions be generated in the usual way.
  Provided the list of partition files for each small domain, the keyword 'union' will 
  partition the global domain in a conform fashion with the smaller domains.
  """
  # From: line 1458, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['union_Tru']
  _attributesList = [
    ('liste', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1458]
  _infoAttr = {'liste': ['TRAD2_example', 1459]}
  _attributesList = check_append_list('partitionneur_union_Tru', _attributesList, partitionneur_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('partitionneur_union_Tru', _attributesSynonyms, partitionneur_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('partitionneur_union_Tru', _infoAttr, partitionneur_deriv_Tru._infoAttr)
  _optionals.update(partitionneur_deriv_Tru._optionals)
  _helpDict = {
    'liste':
          ('[bloc_lecture] List of the partition files with the following syntaxe: {sous_domaine1 \n'
           'decoupage1 ...\n'
           'sous_domaineim decoupageim } where sous_domaine1 ...\n'
           'sous_zomeim are small domains names and decoupage1 ...\n'
           'decoupageim are partition files.',
           ''),
  }
  _helpDict = check_append_dict('partitionneur_union_Tru', _helpDict, partitionneur_deriv_Tru._helpDict)
##### end partitionneur_union_Tru class

class parametre_equation_base_Tru(objet_lecture_Tru): #####
  r"""
  Basic class for parametre_equation
  """
  # From: line 1568, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1568]
  _infoAttr = {}
  _attributesList = check_append_list('parametre_equation_base_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('parametre_equation_base_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('parametre_equation_base_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('parametre_equation_base_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end parametre_equation_base_Tru class

class parametre_diffusion_implicite_Tru(parametre_equation_base_Tru): #####
  r"""
  To specify additional parameters for the equation when using impliciting diffusion
  """
  # From: line 1460, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('crank', 'entier_8bf71a_Tru'),   #
    ('preconditionnement_diag', 'entier_8bf71a_Tru'),   #
    ('niter_max_diffusion_implicite', 'entier_3282e0_Tru'),   #
    ('seuil_diffusion_implicite', 'floattant_820d8b_Tru'),   #
    ('solveur', '<inherited>solveur_sys_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['crank',
   'preconditionnement_diag',
   'niter_max_diffusion_implicite',
   'seuil_diffusion_implicite',
   'solveur'])
  _infoMain = ['TRAD2_example', 1460]
  _infoAttr = {'crank': ['TRAD2_example', 1461],
   'niter_max_diffusion_implicite': ['TRAD2_example', 1463],
   'preconditionnement_diag': ['TRAD2_example', 1462],
   'seuil_diffusion_implicite': ['TRAD2_example', 1464],
   'solveur': ['TRAD2_example', 1465]}
  _attributesList = check_append_list('parametre_diffusion_implicite_Tru', _attributesList, parametre_equation_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('parametre_diffusion_implicite_Tru', _attributesSynonyms, parametre_equation_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('parametre_diffusion_implicite_Tru', _infoAttr, parametre_equation_base_Tru._infoAttr)
  _optionals.update(parametre_equation_base_Tru._optionals)
  _helpDict = {
    'crank':
          ('[entier(into=[0,1])] Use (1) or not (0, default) a Crank Nicholson method for the \n'
           'diffusion implicitation algorithm.\n'
           'Setting crank to 1 increases the order of the algorithm from 1 to 2.',
           ''),
    'preconditionnement_diag':
          ('[entier(into=[0,1])] The CG used to solve the implicitation of the equation diffusion \n'
           'operator is not preconditioned by default.\n'
           'If this option is set to 1, a diagonal preconditionning is used.\n'
           'Warning: this option is not necessarily more efficient, depending on the treated \n'
           'case.',
           ''),
    'niter_max_diffusion_implicite':
          ('[entier] Change the maximum number of iterations for the CG (Conjugate Gradient) algorithm \n'
           'when solving the diffusion implicitation of the equation.',
           ''),
    'seuil_diffusion_implicite':
          ('[floattant] Change the threshold convergence value used by default for the CG resolution \n'
           'for the diffusion implicitation of this equation.',
           ''),
    'solveur':
          ('[solveur_sys_base] Method (different from the default one, Conjugate Gradient) to \n'
           'solve the linear system.',
           ''),
  }
  _helpDict = check_append_dict('parametre_diffusion_implicite_Tru', _helpDict, parametre_equation_base_Tru._helpDict)
##### end parametre_diffusion_implicite_Tru class

class parametre_implicite_Tru(parametre_equation_base_Tru): #####
  r"""
  Keyword to change for this equation only the parameter of the implicit scheme used 
  to solve the problem.
  """
  # From: line 1466, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('seuil_convergence_implicite', 'floattant_820d8b_Tru'),   #
    ('seuil_convergence_solveur', 'floattant_820d8b_Tru'),   #
    ('solveur', '<inherited>solveur_sys_base_Tru'),   #
    ('resolution_explicite', 'rien_Tru'),   #
    ('equation_non_resolue', 'rien_Tru'),   #
    ('equation_frequence_resolue', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['seuil_convergence_implicite',
   'seuil_convergence_solveur',
   'solveur',
   'resolution_explicite',
   'equation_non_resolue',
   'equation_frequence_resolue'])
  _infoMain = ['TRAD2_example', 1466]
  _infoAttr = {'equation_frequence_resolue': ['TRAD2_example', 1472],
   'equation_non_resolue': ['TRAD2_example', 1471],
   'resolution_explicite': ['TRAD2_example', 1470],
   'seuil_convergence_implicite': ['TRAD2_example', 1467],
   'seuil_convergence_solveur': ['TRAD2_example', 1468],
   'solveur': ['TRAD2_example', 1469]}
  _attributesList = check_append_list('parametre_implicite_Tru', _attributesList, parametre_equation_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('parametre_implicite_Tru', _attributesSynonyms, parametre_equation_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('parametre_implicite_Tru', _infoAttr, parametre_equation_base_Tru._infoAttr)
  _optionals.update(parametre_equation_base_Tru._optionals)
  _helpDict = {
    'seuil_convergence_implicite':
          ('[floattant] Keyword to change for this equation only the value of seuil_convergence_implicite \n'
           'used in the implicit scheme.',
           ''),
    'seuil_convergence_solveur':
          ('[floattant] Keyword to change for this equation only the value of seuil_convergence_solveur \n'
           'used in the implicit scheme',
           ''),
    'solveur':
          ('[solveur_sys_base] Keyword to change for this equation only the solver used in the \n'
           'implicit scheme',
           ''),
    'resolution_explicite':
          ('[rien] To solve explicitly the equation whereas the scheme is an implicit scheme.', ''),
    'equation_non_resolue':
          ('[rien] Keyword to specify that the equation is not solved.', ''),
    'equation_frequence_resolue':
          ('[chaine] Keyword to specify that the equation is solved only every n time steps (n \n'
           'is an integer or given by a time-dependent function f(t)).',
           ''),
  }
  _helpDict = check_append_dict('parametre_implicite_Tru', _helpDict, parametre_equation_base_Tru._helpDict)
##### end parametre_implicite_Tru class

class neumann_homogene_Tru(condlim_base_Tru): #####
  r"""
  Homogeneous neumann boundary condition
  """
  # From: line 1475, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1475]
  _infoAttr = {}
  _attributesList = check_append_list('neumann_homogene_Tru', _attributesList, condlim_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('neumann_homogene_Tru', _attributesSynonyms, condlim_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('neumann_homogene_Tru', _infoAttr, condlim_base_Tru._infoAttr)
  _optionals.update(condlim_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('neumann_homogene_Tru', _helpDict, condlim_base_Tru._helpDict)
##### end neumann_homogene_Tru class

class sonde_Tru(objet_lecture_Tru): #####
  r"""
  Keyword is used to define the probes.
  Observations: the probe coordinates should be given in Cartesian coordinates (X, 
  Y, Z), including axisymmetric.
  """
  # From: line 1476, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('nom_sonde', 'chaine_Tru'),   #
    ('special', 'chaine_6cbaef_Tru'),   #
    ('nom_inco', 'chaine_Tru'),   #
    ('mperiode', 'chaine_cddc57_Tru'),   #
    ('prd', 'floattant_820d8b_Tru'),   #
    ('type', '<inherited>sonde_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['special'])
  _infoMain = ['TRAD2_example', 1476]
  _infoAttr = {'mperiode': ['TRAD2_example', 1480],
   'nom_inco': ['TRAD2_example', 1479],
   'nom_sonde': ['TRAD2_example', 1477],
   'prd': ['TRAD2_example', 1481],
   'special': ['TRAD2_example', 1478],
   'type': ['TRAD2_example', 1482]}
  _attributesList = check_append_list('sonde_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('sonde_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('sonde_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'nom_sonde':
          ('[chaine] Name of the file in which the values taken over time will be saved.\n'
           'The complete file name is nom_sonde.son.',
           ''),
    'special':
          ('[chaine(into=["grav","som","nodes","chsom","gravcl"])] Option to change the positions \n'
           'of the probes.\n'
           'Several options are available:\n'
           '\n'
           'grav : each probe is moved to the nearest cell center of the mesh;\n'
           '\n'
           'som : each probe is moved to the nearest vertex of the mesh\n'
           '\n'
           'nodes : each probe is moved to the nearest face center of the mesh;\n'
           '\n'
           'chsom : only available for P1NC sampled field.\n'
           'The values of the probes are calculated according to P1-Conform corresponding field.\n'
           '\n'
           'gravcl : Extend to the domain face boundary a cell-located segment probe in order \n'
           'to have the boundary condition for the field.\n'
           'For this type the extreme probe point has to be on the face center of gravity.',
           ''),
    'nom_inco':
          ('[chaine] Name of the sampled field.', ''),
    'mperiode':
          ('[chaine(into=["periode"])] Keyword to set the sampled field measurement frequency.', ''),
    'prd':
          ('[floattant] Period value.\n'
           'Every prd seconds, the field value calculated at the previous time step is written \n'
           'to the nom_sonde.son file.',
           ''),
    'type':
          ('[sonde_base] Type of probe.', ''),
  }
  _helpDict = check_append_dict('sonde_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end sonde_Tru class

class segmentfacesx_Tru(sonde_base_Tru): #####
  r"""
  Segment probe where points are moved to the nearest x faces
  """
  # From: line 1483, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('nbr', 'entier_3282e0_Tru'),   #
    ('point_deb', 'un_point_Tru'),   #
    ('point_fin', 'un_point_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1483]
  _infoAttr = {'nbr': ['TRAD2_example', 1484],
   'point_deb': ['TRAD2_example', 1485],
   'point_fin': ['TRAD2_example', 1486]}
  _attributesList = check_append_list('segmentfacesx_Tru', _attributesList, sonde_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('segmentfacesx_Tru', _attributesSynonyms, sonde_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('segmentfacesx_Tru', _infoAttr, sonde_base_Tru._infoAttr)
  _optionals.update(sonde_base_Tru._optionals)
  _helpDict = {
    'nbr':
          ('[entier] Number of probe points of the segment, evenly distributed.', ''),
    'point_deb':
          ('[un_point] First outer probe segment point.', ''),
    'point_fin':
          ('[un_point] Second outer probe segment point.', ''),
  }
  _helpDict = check_append_dict('segmentfacesx_Tru', _helpDict, sonde_base_Tru._helpDict)
##### end segmentfacesx_Tru class

class segmentfacesy_Tru(sonde_base_Tru): #####
  r"""
  Segment probe where points are moved to the nearest y faces
  """
  # From: line 1487, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('nbr', 'entier_3282e0_Tru'),   #
    ('point_deb', 'un_point_Tru'),   #
    ('point_fin', 'un_point_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1487]
  _infoAttr = {'nbr': ['TRAD2_example', 1488],
   'point_deb': ['TRAD2_example', 1489],
   'point_fin': ['TRAD2_example', 1490]}
  _attributesList = check_append_list('segmentfacesy_Tru', _attributesList, sonde_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('segmentfacesy_Tru', _attributesSynonyms, sonde_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('segmentfacesy_Tru', _infoAttr, sonde_base_Tru._infoAttr)
  _optionals.update(sonde_base_Tru._optionals)
  _helpDict = {
    'nbr':
          ('[entier] Number of probe points of the segment, evenly distributed.', ''),
    'point_deb':
          ('[un_point] First outer probe segment point.', ''),
    'point_fin':
          ('[un_point] Second outer probe segment point.', ''),
  }
  _helpDict = check_append_dict('segmentfacesy_Tru', _helpDict, sonde_base_Tru._helpDict)
##### end segmentfacesy_Tru class

class segmentfacesz_Tru(sonde_base_Tru): #####
  r"""
  Segment probe where points are moved to the nearest z faces
  """
  # From: line 1491, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('nbr', 'entier_3282e0_Tru'),   #
    ('point_deb', 'un_point_Tru'),   #
    ('point_fin', 'un_point_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1491]
  _infoAttr = {'nbr': ['TRAD2_example', 1492],
   'point_deb': ['TRAD2_example', 1493],
   'point_fin': ['TRAD2_example', 1494]}
  _attributesList = check_append_list('segmentfacesz_Tru', _attributesList, sonde_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('segmentfacesz_Tru', _attributesSynonyms, sonde_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('segmentfacesz_Tru', _infoAttr, sonde_base_Tru._infoAttr)
  _optionals.update(sonde_base_Tru._optionals)
  _helpDict = {
    'nbr':
          ('[entier] Number of probe points of the segment, evenly distributed.', ''),
    'point_deb':
          ('[un_point] First outer probe segment point.', ''),
    'point_fin':
          ('[un_point] Second outer probe segment point.', ''),
  }
  _helpDict = check_append_dict('segmentfacesz_Tru', _helpDict, sonde_base_Tru._helpDict)
##### end segmentfacesz_Tru class

class radius_Tru(sonde_base_Tru): #####
  r"""
  not_set
  """
  # From: line 1495, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('nbr', 'entier_3282e0_Tru'),   #
    ('point_deb', 'un_point_Tru'),   #
    ('radius', 'floattant_820d8b_Tru'),   #
    ('teta1', 'floattant_820d8b_Tru'),   #
    ('teta2', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1495]
  _infoAttr = {'nbr': ['TRAD2_example', 1496],
   'point_deb': ['TRAD2_example', 1497],
   'radius': ['TRAD2_example', 1498],
   'teta1': ['TRAD2_example', 1499],
   'teta2': ['TRAD2_example', 1500]}
  _attributesList = check_append_list('radius_Tru', _attributesList, sonde_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('radius_Tru', _attributesSynonyms, sonde_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('radius_Tru', _infoAttr, sonde_base_Tru._infoAttr)
  _optionals.update(sonde_base_Tru._optionals)
  _helpDict = {
    'nbr':
          ('[entier] Number of probe points of the segment, evenly distributed.', ''),
    'point_deb':
          ('[un_point] First outer probe segment point.', ''),
    'radius':
          ('[floattant] not_set', ''),
    'teta1':
          ('[floattant] not_set', ''),
    'teta2':
          ('[floattant] not_set', ''),
  }
  _helpDict = check_append_dict('radius_Tru', _helpDict, sonde_base_Tru._helpDict)
##### end radius_Tru class

class sondes_fichier_Tru(objet_lecture_Tru): #####
  r"""
  not_set
  """
  # From: line 1535, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('fichier', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'fichier': ['file']
  }
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1535]
  _infoAttr = {'fichier': ['TRAD2_example', 1536]}
  _attributesList = check_append_list('sondes_fichier_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('sondes_fichier_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('sondes_fichier_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'fichier':
          ('[chaine] name of file', ''),
  }
  _helpDict = check_append_dict('sondes_fichier_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end sondes_fichier_Tru class

class definition_champs_fichier_Tru(objet_lecture_Tru): #####
  r"""
  Keyword to read definition_champs from a file
  """
  # From: line 1537, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('fichier', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'fichier': ['file']
  }
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1537]
  _infoAttr = {'fichier': ['TRAD2_example', 1538]}
  _attributesList = check_append_list('definition_champs_fichier_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('definition_champs_fichier_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('definition_champs_fichier_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'fichier':
          ('[chaine] name of file containing the definition of advanced fields', ''),
  }
  _helpDict = check_append_dict('definition_champs_fichier_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end definition_champs_fichier_Tru class

class residuals_Tru(interprete_Tru): #####
  r"""
  To specify how the residuals will be computed.
  """
  # From: line 1565, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('norm', 'chaine_48a9fa_Tru'),   #
    ('relative', 'chaine_d2dddb_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['norm', 'relative'])
  _infoMain = ['TRAD2_example', 1565]
  _infoAttr = {'norm': ['TRAD2_example', 1566], 'relative': ['TRAD2_example', 1567]}
  _attributesList = check_append_list('residuals_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('residuals_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('residuals_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'norm':
          ('[chaine(into=["l2","max"])] allows to choose the norm we want to use (max norm by \n'
           'default).\n'
           'Possible to specify L2-norm.',
           ''),
    'relative':
          ('[chaine(into=["0","1","2"])] This is the old keyword seuil_statio_relatif_deconseille.\n'
           'If it is set to 1, it will normalize the residuals with the residuals of the first \n'
           '5 timesteps (default is 0).\n'
           'if set to 2, residual will be computed as R/(max-min).',
           ''),
  }
  _helpDict = check_append_dict('residuals_Tru', _helpDict, interprete_Tru._helpDict)
##### end residuals_Tru class

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
  # From: line 1580, in file 'TRAD2_example'
  _braces = 1
  _synonyms = ['lire_med_Tru']
  _attributesList = [
    ('convertalltopoly', 'rien_Tru'),   #
    ('domain', 'ref_domaine_Tru'),   #
    ('file', 'chaine_Tru'),   #
    ('mesh', 'chaine_Tru'),   #
    ('exclude_groups', 'listchaine_Tru'),   #
    ('include_additional_face_groups', 'listchaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'domain': ['domaine'],
    'exclude_groups': ['exclure_groupes'],
    'file': ['fichier'],
    'include_additional_face_groups': ['inclure_groupes_faces_additionnels'],
    'mesh': ['maillage']
  }
  _optionals = set(['convertalltopoly', 'mesh', 'exclude_groups', 'include_additional_face_groups'])
  _infoMain = ['TRAD2_example', 1580]
  _infoAttr = {'convertalltopoly': ['TRAD2_example', 1581],
   'domain': ['TRAD2_example', 1582],
   'exclude_groups': ['TRAD2_example', 1585],
   'file': ['TRAD2_example', 1583],
   'include_additional_face_groups': ['TRAD2_example', 1586],
   'mesh': ['TRAD2_example', 1584]}
  _attributesList = check_append_list('read_med_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('read_med_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('read_med_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'convertalltopoly':
          ('[rien] Option to convert mesh with mixed cells into polyhedral/polygonal cells', ''),
    'domain':
          ('[ref_domaine] Corresponds to the domain name.', ''),
    'file':
          ("[chaine] File (written in the MED format, with extension '.med') containing the mesh", ''),
    'mesh':
          ('[chaine] Name of the mesh in med file.\nIf not specified, the first mesh will be read.', ''),
    'exclude_groups':
          ('[listchaine] List of face groups to skip in the MED file.', ''),
    'include_additional_face_groups':
          ('[listchaine] List of face groups to read and register in the MED file.', ''),
  }
  _helpDict = check_append_dict('read_med_Tru', _helpDict, interprete_Tru._helpDict)
##### end read_med_Tru class

class champ_front_med_Tru(front_field_base_Tru): #####
  r"""
  Field allowing the loading of a boundary condition from a MED file using Champ_fonc_med
  """
  # From: line 1587, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('champ_fonc_med', '<inherited>field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1587]
  _infoAttr = {'champ_fonc_med': ['TRAD2_example', 1588]}
  _attributesList = check_append_list('champ_front_med_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_med_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_med_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'champ_fonc_med':
          ('[field_base] a champ_fonc_med loading the values of the unknown on a domain boundary', ''),
  }
  _helpDict = check_append_dict('champ_front_med_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_med_Tru class

class format_lata_to_med_Tru(objet_lecture_Tru): #####
  r"""
  not_set
  """
  # From: line 1589, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('mot', 'chaine_e57e1c_Tru'),   #
    ('format', 'chaine_f07ecc_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['format'])
  _infoMain = ['TRAD2_example', 1589]
  _infoAttr = {'format': ['TRAD2_example', 1591], 'mot': ['TRAD2_example', 1590]}
  _attributesList = check_append_list('format_lata_to_med_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('format_lata_to_med_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('format_lata_to_med_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'mot':
          ('[chaine(into=["format_post_sup"])] not_set', ''),
    'format':
          ('[chaine(into=["lml","lata","lata_v2","med"])] generated file post_med.data use format \n'
           '(MED or LATA or LML keyword).',
           ''),
  }
  _helpDict = check_append_dict('format_lata_to_med_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end format_lata_to_med_Tru class

class lata_to_other_Tru(interprete_Tru): #####
  r"""
  To convert results file written with LATA format to MED or LML format.
  Warning: Fields located at faces are not supported yet.
  """
  # From: line 1592, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('format', 'chaine_f07ecc_Tru'),   #
    ('file', 'chaine_Tru'),   #
    ('file_post', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['format'])
  _infoMain = ['TRAD2_example', 1592]
  _infoAttr = {'file': ['TRAD2_example', 1594],
   'file_post': ['TRAD2_example', 1595],
   'format': ['TRAD2_example', 1593]}
  _attributesList = check_append_list('lata_to_other_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('lata_to_other_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('lata_to_other_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'format':
          ('[chaine(into=["lml","lata","lata_v2","med"])] Results format (MED or LATA or LML keyword).', ''),
    'file':
          ('[chaine] LATA file to convert to the new format.', ''),
    'file_post':
          ('[chaine] Name of file post.', ''),
  }
  _helpDict = check_append_dict('lata_to_other_Tru', _helpDict, interprete_Tru._helpDict)
##### end lata_to_other_Tru class

class lata_to_med_Tru(interprete_Tru): #####
  r"""
  To convert results file written with LATA format to MED file.
  Warning: Fields located on faces are not supported yet.
  """
  # From: line 1596, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('format', 'format_lata_to_med_Tru'),   #
    ('file', 'chaine_Tru'),   #
    ('file_med', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['format'])
  _infoMain = ['TRAD2_example', 1596]
  _infoAttr = {'file': ['TRAD2_example', 1598],
   'file_med': ['TRAD2_example', 1599],
   'format': ['TRAD2_example', 1597]}
  _attributesList = check_append_list('lata_to_med_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('lata_to_med_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('lata_to_med_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'format':
          ('[format_lata_to_med] generated file post_med.data use format (MED or LATA or LML keyword).', ''),
    'file':
          ('[chaine] LATA file to convert to the new format.', ''),
    'file_med':
          ('[chaine] Name of the MED file.', ''),
  }
  _helpDict = check_append_dict('lata_to_med_Tru', _helpDict, interprete_Tru._helpDict)
##### end lata_to_med_Tru class

class ecrire_med_Tru(interprete_Tru): #####
  r"""
  Write a domain to MED format into a file.
  """
  # From: line 1600, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['write_med_Tru']
  _attributesList = [
    ('nom_dom', 'ref_domaine_Tru'),   #
    ('file', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1600]
  _infoAttr = {'file': ['TRAD2_example', 1602], 'nom_dom': ['TRAD2_example', 1601]}
  _attributesList = check_append_list('ecrire_med_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('ecrire_med_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('ecrire_med_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'nom_dom':
          ('[ref_domaine] Name of domain.', ''),
    'file':
          ('[chaine] Name of file.', ''),
  }
  _helpDict = check_append_dict('ecrire_med_Tru', _helpDict, interprete_Tru._helpDict)
##### end ecrire_med_Tru class

class ecrire_champ_med_Tru(interprete_Tru): #####
  r"""
  Keyword to write a field to MED format into a file.
  """
  # From: line 1603, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('nom_dom', 'ref_domaine_Tru'),   #
    ('nom_chp', 'ref_field_base_Tru'),   #
    ('file', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1603]
  _infoAttr = {'file': ['TRAD2_example', 1606],
   'nom_chp': ['TRAD2_example', 1605],
   'nom_dom': ['TRAD2_example', 1604]}
  _attributesList = check_append_list('ecrire_champ_med_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('ecrire_champ_med_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('ecrire_champ_med_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'nom_dom':
          ('[ref_domaine] domain name', ''),
    'nom_chp':
          ('[ref_field_base] field name', ''),
    'file':
          ('[chaine] file name', ''),
  }
  _helpDict = check_append_dict('ecrire_champ_med_Tru', _helpDict, interprete_Tru._helpDict)
##### end ecrire_champ_med_Tru class

class merge_med_Tru(interprete_Tru): #####
  r"""
  This keyword allows to merge multiple MED files produced during a parallel computation 
  into a single MED file.
  """
  # From: line 1607, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('med_files_base_name', 'chaine_Tru'),   #
    ('time_iterations', 'chaine_45470e_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1607]
  _infoAttr = {'med_files_base_name': ['TRAD2_example', 1608],
   'time_iterations': ['TRAD2_example', 1609]}
  _attributesList = check_append_list('merge_med_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('merge_med_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('merge_med_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'med_files_base_name':
          ('[chaine] Base name of multiple med files that should appear as base_name_xxxxx.med, \n'
           'where xxxxx denotes the MPI rank number.\n'
           "If you specify NOM_DU_CAS, it will automatically take the basename from your datafile's \n"
           'name.',
           ''),
    'time_iterations':
          ('[chaine(into=["all_times","last_time"])] Identifies whether to merge all time iterations \n'
           'present in the MED files or only the last one.',
           ''),
  }
  _helpDict = check_append_dict('merge_med_Tru', _helpDict, interprete_Tru._helpDict)
##### end merge_med_Tru class

class champ_fonc_med_Tru(field_base_Tru): #####
  r"""
  Field to read a data field in a MED-format file .med at a specified time.
  It is very useful, for example, to resume a calculation with a new or refined geometry.
  The field post-processed on the new geometry at med format is used as initial condition 
  for the resume.
  """
  # From: line 1610, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('use_existing_domain', 'rien_Tru'),   #
    ('last_time', 'rien_Tru'),   #
    ('decoup', 'chaine_Tru'),   #
    ('domain', 'chaine_Tru'),   #
    ('file', 'chaine_Tru'),   #
    ('field', 'chaine_Tru'),   #
    ('loc', 'chaine_8b7047_Tru'),   #
    ('time', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['use_existing_domain', 'last_time', 'decoup', 'loc', 'time'])
  _infoMain = ['TRAD2_example', 1610]
  _infoAttr = {'decoup': ['TRAD2_example', 1613],
   'domain': ['TRAD2_example', 1614],
   'field': ['TRAD2_example', 1616],
   'file': ['TRAD2_example', 1615],
   'last_time': ['TRAD2_example', 1612],
   'loc': ['TRAD2_example', 1617],
   'time': ['TRAD2_example', 1618],
   'use_existing_domain': ['TRAD2_example', 1611]}
  _attributesList = check_append_list('champ_fonc_med_Tru', _attributesList, field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_fonc_med_Tru', _attributesSynonyms, field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_fonc_med_Tru', _infoAttr, field_base_Tru._infoAttr)
  _optionals.update(field_base_Tru._optionals)
  _helpDict = {
    'use_existing_domain':
          ('[rien] whether to optimize the field loading by indicating that the field is supported \n'
           'by the same mesh that was initially loaded as the domain',
           ''),
    'last_time':
          ('[rien] to use the last time of the MED file instead of the specified time.\n'
           "Mutually exclusive with 'time' parameter.",
           ''),
    'decoup':
          ('[chaine] specify a partition file.', ''),
    'domain':
          ('[chaine] Name of the domain supporting the field.\n'
           'This is the name of the mesh in the MED file, and if this mesh was also used to create \n'
           "the TRUST domain, loading can be optimized with option 'use_existing_domain'.",
           ''),
    'file':
          ('[chaine] Name of the .med file.', ''),
    'field':
          ('[chaine] Name of field to load.', ''),
    'loc':
          ('[chaine(into=["som","elem"])] To indicate where the field is localised.\nDefault to \'elem\'.',
           ''),
    'time':
          ("[floattant] Timestep to load from the MED file.\nMutually exclusive with 'last_time' flag.", ''),
  }
  _helpDict = check_append_dict('champ_fonc_med_Tru', _helpDict, field_base_Tru._helpDict)
##### end champ_fonc_med_Tru class

class champ_fonc_med_table_temps_Tru(champ_fonc_med_Tru): #####
  r"""
  Field defined as a fixed spatial shape scaled by a temporal coefficient
  """
  # From: line 1619, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('table_temps', 'chaine_Tru'),   #
    ('table_temps_lue', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['table_temps', 'table_temps_lue'])
  _infoMain = ['TRAD2_example', 1619]
  _infoAttr = {'table_temps': ['TRAD2_example', 1620],
   'table_temps_lue': ['TRAD2_example', 1621]}
  _attributesList = check_append_list('champ_fonc_med_table_temps_Tru', _attributesList, champ_fonc_med_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_fonc_med_table_temps_Tru', _attributesSynonyms, champ_fonc_med_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_fonc_med_table_temps_Tru', _infoAttr, champ_fonc_med_Tru._infoAttr)
  _optionals.update(champ_fonc_med_Tru._optionals)
  _helpDict = {
    'table_temps':
          ('[chaine] Table containing the temporal coefficient used to scale the field', ''),
    'table_temps_lue':
          ('[chaine] Name of the file containing the values of the temporal coefficient used to \n'
           'scale the field',
           ''),
  }
  _helpDict = check_append_dict('champ_fonc_med_table_temps_Tru', _helpDict, champ_fonc_med_Tru._helpDict)
##### end champ_fonc_med_table_temps_Tru class

class champ_fonc_med_tabule_Tru(champ_fonc_med_Tru): #####
  r"""
  not_set
  """
  # From: line 1622, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1622]
  _infoAttr = {}
  _attributesList = check_append_list('champ_fonc_med_tabule_Tru', _attributesList, champ_fonc_med_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_fonc_med_tabule_Tru', _attributesSynonyms, champ_fonc_med_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_fonc_med_tabule_Tru', _infoAttr, champ_fonc_med_Tru._infoAttr)
  _optionals.update(champ_fonc_med_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('champ_fonc_med_tabule_Tru', _helpDict, champ_fonc_med_Tru._helpDict)
##### end champ_fonc_med_tabule_Tru class

class champ_fonc_interp_Tru(champ_don_base_Tru): #####
  r"""
  Field that is interpolated from a distant domain via MEDCoupling (remapper).
  """
  # From: line 1623, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('nom_champ', 'chaine_Tru'),   #
    ('pb_loc', 'chaine_Tru'),   #
    ('pb_dist', 'chaine_Tru'),   #
    ('dom_loc', 'chaine_Tru'),   #
    ('dom_dist', 'chaine_Tru'),   #
    ('default_value', 'chaine_Tru'),   #
    ('nature', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['dom_loc', 'dom_dist', 'default_value'])
  _infoMain = ['TRAD2_example', 1623]
  _infoAttr = {'default_value': ['TRAD2_example', 1629],
   'dom_dist': ['TRAD2_example', 1628],
   'dom_loc': ['TRAD2_example', 1627],
   'nature': ['TRAD2_example', 1630],
   'nom_champ': ['TRAD2_example', 1624],
   'pb_dist': ['TRAD2_example', 1626],
   'pb_loc': ['TRAD2_example', 1625]}
  _attributesList = check_append_list('champ_fonc_interp_Tru', _attributesList, champ_don_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_fonc_interp_Tru', _attributesSynonyms, champ_don_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_fonc_interp_Tru', _infoAttr, champ_don_base_Tru._infoAttr)
  _optionals.update(champ_don_base_Tru._optionals)
  _helpDict = {
    'nom_champ':
          ('[chaine] Name of the field (for example: temperature).', ''),
    'pb_loc':
          ('[chaine] Name of the local problem.', ''),
    'pb_dist':
          ('[chaine] Name of the distant problem.', ''),
    'dom_loc':
          ('[chaine] Name of the local domain.', ''),
    'dom_dist':
          ('[chaine] Name of the distant domain.', ''),
    'default_value':
          ('[chaine] Name of the distant domain.', ''),
    'nature':
          ('[chaine] Nature of the field (knowledge from MEDCoupling is required; IntensiveMaximum, \n'
           'IntensiveConservation, ...).',
           ''),
  }
  _helpDict = check_append_dict('champ_fonc_interp_Tru', _helpDict, champ_don_base_Tru._helpDict)
##### end champ_fonc_interp_Tru class

class echange_couplage_thermique_Tru(paroi_echange_global_impose_Tru): #####
  r"""
  Thermal coupling boundary condition
  """
  # From: line 1631, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('temperature_paroi', '<inherited>field_base_Tru'),   #
    ('flux_paroi', '<inherited>field_base_Tru'),   #
  ]
  _suppressed = ['text', 'h_imp', 'himpc', 'ch']
  _attributesSynonyms = {}
  _optionals = set(['temperature_paroi', 'flux_paroi'])
  _infoMain = ['TRAD2_example', 1631]
  _infoAttr = {'flux_paroi': ['TRAD2_example', 1637],
   'temperature_paroi': ['TRAD2_example', 1636]}
  _attributesList = check_append_list('echange_couplage_thermique_Tru', _attributesList, paroi_echange_global_impose_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('echange_couplage_thermique_Tru', _attributesSynonyms, paroi_echange_global_impose_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('echange_couplage_thermique_Tru', _infoAttr, paroi_echange_global_impose_Tru._infoAttr)
  _optionals.update(paroi_echange_global_impose_Tru._optionals)
  _helpDict = {
    'temperature_paroi':
          ('[field_base] Temperature', ''),
    'flux_paroi':
          ('[field_base] Wall heat flux', ''),
  }
  _helpDict = check_append_dict('echange_couplage_thermique_Tru', _helpDict, paroi_echange_global_impose_Tru._helpDict)
##### end echange_couplage_thermique_Tru class

class echange_interne_global_parfait_Tru(condlim_base_Tru): #####
  r"""
  Internal heat exchange boundary condition with perfect (infinite) exchange coefficient.
  """
  # From: line 1638, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['paroi_echange_interne_global_parfait_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1638]
  _infoAttr = {}
  _attributesList = check_append_list('echange_interne_global_parfait_Tru', _attributesList, condlim_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('echange_interne_global_parfait_Tru', _attributesSynonyms, condlim_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('echange_interne_global_parfait_Tru', _infoAttr, condlim_base_Tru._infoAttr)
  _optionals.update(condlim_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('echange_interne_global_parfait_Tru', _helpDict, condlim_base_Tru._helpDict)
##### end echange_interne_global_parfait_Tru class

class solide_Tru(milieu_base_Tru): #####
  r"""
  Solid with cp and/or rho non-uniform.
  """
  # From: line 1639, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('rho', '<inherited>field_base_Tru'),   #
    ('cp', '<inherited>field_base_Tru'),   #
    ('Lambda', '<inherited>field_base_Tru'),   #
    ('user_field', '<inherited>field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'Lambda': ['lambda_u']
  }
  _optionals = set(['rho', 'cp', 'Lambda', 'user_field'])
  _infoMain = ['TRAD2_example', 1639]
  _infoAttr = {'Lambda': ['TRAD2_example', 1642],
   'cp': ['TRAD2_example', 1641],
   'rho': ['TRAD2_example', 1640],
   'user_field': ['TRAD2_example', 1643]}
  _attributesList = check_append_list('solide_Tru', _attributesList, milieu_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('solide_Tru', _attributesSynonyms, milieu_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('solide_Tru', _infoAttr, milieu_base_Tru._infoAttr)
  _optionals.update(milieu_base_Tru._optionals)
  _helpDict = {
    'rho':
          ('[field_base] Density (kg.m-3).', ''),
    'cp':
          ('[field_base] Specific heat (J.kg-1.K-1).', ''),
    'Lambda':
          ('[field_base] Conductivity (W.m-1.K-1).', ''),
    'user_field':
          ('[field_base] user defined field.', ''),
  }
  _helpDict = check_append_dict('solide_Tru', _helpDict, milieu_base_Tru._helpDict)
##### end solide_Tru class

class echange_interne_global_impose_Tru(condlim_base_Tru): #####
  r"""
  Internal heat exchange boundary condition with global exchange coefficient.
  """
  # From: line 1644, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['paroi_echange_interne_global_impose_Tru']
  _attributesList = [
    ('h_imp', 'chaine_Tru'),   #
    ('ch', '<inherited>front_field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1644]
  _infoAttr = {'ch': ['TRAD2_example', 1646], 'h_imp': ['TRAD2_example', 1645]}
  _attributesList = check_append_list('echange_interne_global_impose_Tru', _attributesList, condlim_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('echange_interne_global_impose_Tru', _attributesSynonyms, condlim_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('echange_interne_global_impose_Tru', _infoAttr, condlim_base_Tru._infoAttr)
  _optionals.update(condlim_base_Tru._optionals)
  _helpDict = {
    'h_imp':
          ('[chaine] Global exchange coefficient value.\n'
           'The global exchange coefficient value is expressed in W.m-2.K-1.',
           ''),
    'ch':
          ('[front_field_base] Boundary field type.', ''),
  }
  _helpDict = check_append_dict('echange_interne_global_impose_Tru', _helpDict, condlim_base_Tru._helpDict)
##### end echange_interne_global_impose_Tru class

class pb_conduction_Tru(pb_base_Tru): #####
  r"""
  Resolution of the heat equation.
  """
  # From: line 1647, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('solide', '<inherited>solide_Tru'),   #
    ('conduction', '<inherited>conduction_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['solide', 'conduction'])
  _infoMain = ['TRAD2_example', 1647]
  _infoAttr = {'conduction': ['TRAD2_example', 1649], 'solide': ['TRAD2_example', 1648]}
  _attributesList = check_append_list('pb_conduction_Tru', _attributesList, pb_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_conduction_Tru', _attributesSynonyms, pb_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_conduction_Tru', _infoAttr, pb_base_Tru._infoAttr)
  _optionals.update(pb_base_Tru._optionals)
  _helpDict = {
    'solide':
          ('[solide] The medium associated with the problem.', ''),
    'conduction':
          ('[conduction] Heat equation.', ''),
  }
  _helpDict = check_append_dict('pb_conduction_Tru', _helpDict, pb_base_Tru._helpDict)
##### end pb_conduction_Tru class

class conduction_Tru(eqn_base_Tru): #####
  r"""
  Heat equation.
  """
  # From: line 1650, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1650]
  _infoAttr = {}
  _attributesList = check_append_list('conduction_Tru', _attributesList, eqn_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('conduction_Tru', _attributesSynonyms, eqn_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('conduction_Tru', _infoAttr, eqn_base_Tru._infoAttr)
  _optionals.update(eqn_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('conduction_Tru', _helpDict, eqn_base_Tru._helpDict)
##### end conduction_Tru class

class convection_btd_Tru(convection_deriv_Tru): #####
  r"""
  Only for EF discretization.
  """
  # From: line 1651, in file 'TRAD2_example'
  _braces = 1
  _synonyms = ['btd_Tru']
  _attributesList = [
    ('btd', 'floattant_820d8b_Tru'),   #
    ('facteur', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1651]
  _infoAttr = {'btd': ['TRAD2_example', 1652], 'facteur': ['TRAD2_example', 1653]}
  _attributesList = check_append_list('convection_btd_Tru', _attributesList, convection_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_btd_Tru', _attributesSynonyms, convection_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_btd_Tru', _infoAttr, convection_deriv_Tru._infoAttr)
  _optionals.update(convection_deriv_Tru._optionals)
  _helpDict = {
    'btd':
          ('[floattant] not_set', ''),
    'facteur':
          ('[floattant] not_set', ''),
  }
  _helpDict = check_append_dict('convection_btd_Tru', _helpDict, convection_deriv_Tru._helpDict)
##### end convection_btd_Tru class

class convection_supg_Tru(convection_deriv_Tru): #####
  r"""
  Only for EF discretization.
  """
  # From: line 1654, in file 'TRAD2_example'
  _braces = 1
  _synonyms = ['supg_Tru']
  _attributesList = [
    ('facteur', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1654]
  _infoAttr = {'facteur': ['TRAD2_example', 1655]}
  _attributesList = check_append_list('convection_supg_Tru', _attributesList, convection_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_supg_Tru', _attributesSynonyms, convection_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_supg_Tru', _infoAttr, convection_deriv_Tru._infoAttr)
  _optionals.update(convection_deriv_Tru._optionals)
  _helpDict = {
    'facteur':
          ('[floattant] not_set', ''),
  }
  _helpDict = check_append_dict('convection_supg_Tru', _helpDict, convection_deriv_Tru._helpDict)
##### end convection_supg_Tru class

class source_pdf_base_Tru(source_base_Tru): #####
  r"""
  Base class of the source term for the Immersed Boundary Penalized Direct Forcing method 
  (PDF)
  """
  # From: line 1743, in file 'TRAD2_example'
  _braces = 1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = [
    ('aire', '<inherited>field_base_Tru'),   #
    ('rotation', '<inherited>field_base_Tru'),   #
    ('transpose_rotation', 'rien_Tru'),   #
    ('modele', 'bloc_pdf_model_Tru'),   #
    ('interpolation', '<inherited>interpolation_ibm_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['transpose_rotation', 'interpolation'])
  _infoMain = ['TRAD2_example', 1743]
  _infoAttr = {'aire': ['TRAD2_example', 1744],
   'interpolation': ['TRAD2_example', 1748],
   'modele': ['TRAD2_example', 1747],
   'rotation': ['TRAD2_example', 1745],
   'transpose_rotation': ['TRAD2_example', 1746]}
  _attributesList = check_append_list('source_pdf_base_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('source_pdf_base_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('source_pdf_base_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'aire':
          ('[field_base] volumic field: a boolean for the cell (0 or 1) indicating if the obstacle \n'
           'is in the cell',
           ''),
    'rotation':
          ('[field_base] volumic field with 9 components representing the change of basis on cells \n'
           '(local to global).\n'
           'Used for rotating cases for example.',
           ''),
    'transpose_rotation':
          ('[rien] whether to transpose the basis change matrix.', ''),
    'modele':
          ('[bloc_pdf_model] model used for the Penalized Direct Forcing', ''),
    'interpolation':
          ('[interpolation_ibm_base] interpolation method', ''),
  }
  _helpDict = check_append_dict('source_pdf_base_Tru', _helpDict, source_base_Tru._helpDict)
##### end source_pdf_base_Tru class

class source_pdf_Tru(source_pdf_base_Tru): #####
  r"""
  Source term for Penalised Direct Forcing (PDF) method.
  """
  # From: line 1656, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1656]
  _infoAttr = {}
  _attributesList = check_append_list('source_pdf_Tru', _attributesList, source_pdf_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('source_pdf_Tru', _attributesSynonyms, source_pdf_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('source_pdf_Tru', _infoAttr, source_pdf_base_Tru._infoAttr)
  _optionals.update(source_pdf_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('source_pdf_Tru', _helpDict, source_pdf_base_Tru._helpDict)
##### end source_pdf_Tru class

class paroi_fixe_iso_genepi2_sans_contribution_aux_vitesses_sommets_Tru(paroi_fixe_Tru): #####
  r"""
  Boundary condition to obtain iso Geneppi2, without interest
  """
  # From: line 1657, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1657]
  _infoAttr = {}
  _attributesList = check_append_list('paroi_fixe_iso_genepi2_sans_contribution_aux_vitesses_sommets_Tru', _attributesList, paroi_fixe_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('paroi_fixe_iso_genepi2_sans_contribution_aux_vitesses_sommets_Tru', _attributesSynonyms, paroi_fixe_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('paroi_fixe_iso_genepi2_sans_contribution_aux_vitesses_sommets_Tru', _infoAttr, paroi_fixe_Tru._infoAttr)
  _optionals.update(paroi_fixe_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('paroi_fixe_iso_genepi2_sans_contribution_aux_vitesses_sommets_Tru', _helpDict, paroi_fixe_Tru._helpDict)
##### end paroi_fixe_iso_genepi2_sans_contribution_aux_vitesses_sommets_Tru class

class verifiercoin_Tru(interprete_Tru): #####
  r"""
  This keyword subdivides inconsistent 2D/3D cells used with VEFPreP1B discretization.
  Must be used before the mesh is discretized.
  NL1 The Read_file option can be used only if the file.decoupage_som was previously 
  created by TRUST.
  This option, only in 2D, reverses the common face at two cells (at least one is inconsistent), 
  through the nodes opposed.
  In 3D, the option has no effect.
  
  The expert_only option deactivates, into the VEFPreP1B divergence operator, the test 
  of inconsistent cells.
  """
  # From: line 1658, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('domain_name', 'ref_domaine_Tru'),   #
    ('bloc', 'verifiercoin_bloc_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'domain_name': ['dom']
  }
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1658]
  _infoAttr = {'bloc': ['TRAD2_example', 1660], 'domain_name': ['TRAD2_example', 1659]}
  _attributesList = check_append_list('verifiercoin_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('verifiercoin_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('verifiercoin_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'domain_name':
          ('[ref_domaine] Name of the domaine', ''),
    'bloc':
          ('[verifiercoin_bloc] not_set', ''),
  }
  _helpDict = check_append_dict('verifiercoin_Tru', _helpDict, interprete_Tru._helpDict)
##### end verifiercoin_Tru class

class verifiercoin_bloc_Tru(objet_lecture_Tru): #####
  r"""
  not_set
  """
  # From: line 1661, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('read_file', 'chaine_Tru'),   #
    ('expert_only', 'rien_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'read_file': ['filename', 'lire_fichier']
  }
  _optionals = set(['read_file', 'expert_only'])
  _infoMain = ['TRAD2_example', 1661]
  _infoAttr = {'expert_only': ['TRAD2_example', 1663], 'read_file': ['TRAD2_example', 1662]}
  _attributesList = check_append_list('verifiercoin_bloc_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('verifiercoin_bloc_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('verifiercoin_bloc_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'read_file':
          ('[chaine] name of the *.decoupage_som file', ''),
    'expert_only':
          ('[rien] to not check the mesh', ''),
  }
  _helpDict = check_append_dict('verifiercoin_bloc_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end verifiercoin_bloc_Tru class

class coarsen_operator_uniform_Tru(objet_lecture_Tru): #####
  r"""
  Object defining the uniform coarsening process of the given grid in IJK discretization
  """
  # From: line 1672, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('coarsen_operator_uniform', 'chaine_Tru'),   #
    ('aco', 'chaine_e3a651_Tru'),   #
    ('coarsen_i', 'chaine_dbffd3_Tru'),   #
    ('coarsen_i_val', 'entier_3282e0_Tru'),   #
    ('coarsen_j', 'chaine_41af74_Tru'),   #
    ('coarsen_j_val', 'entier_3282e0_Tru'),   #
    ('coarsen_k', 'chaine_c64c02_Tru'),   #
    ('coarsen_k_val', 'entier_3282e0_Tru'),   #
    ('acof', 'chaine_af6447_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['coarsen_operator_uniform',
   'coarsen_i',
   'coarsen_i_val',
   'coarsen_j',
   'coarsen_j_val',
   'coarsen_k',
   'coarsen_k_val'])
  _infoMain = ['TRAD2_example', 1672]
  _infoAttr = {'aco': ['TRAD2_example', 1674],
   'acof': ['TRAD2_example', 1681],
   'coarsen_i': ['TRAD2_example', 1675],
   'coarsen_i_val': ['TRAD2_example', 1676],
   'coarsen_j': ['TRAD2_example', 1677],
   'coarsen_j_val': ['TRAD2_example', 1678],
   'coarsen_k': ['TRAD2_example', 1679],
   'coarsen_k_val': ['TRAD2_example', 1680],
   'coarsen_operator_uniform': ['TRAD2_example', 1673]}
  _attributesList = check_append_list('coarsen_operator_uniform_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('coarsen_operator_uniform_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('coarsen_operator_uniform_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'coarsen_operator_uniform':
          ('[chaine] not_set', ''),
    'aco':
          ('[chaine(into=["{"])] opening curly brace', ''),
    'coarsen_i':
          ('[chaine(into=["coarsen_i"])] not_set', ''),
    'coarsen_i_val':
          ('[entier] Integer indicating the number by which we will divide the number of elements \n'
           'in the I direction (in order to obtain a coarser grid)',
           ''),
    'coarsen_j':
          ('[chaine(into=["coarsen_j"])] not_set', ''),
    'coarsen_j_val':
          ('[entier] Integer indicating the number by which we will divide the number of elements \n'
           'in the J direction (in order to obtain a coarser grid)',
           ''),
    'coarsen_k':
          ('[chaine(into=["coarsen_k"])] not_set', ''),
    'coarsen_k_val':
          ('[entier] Integer indicating the number by which we will divide the number of elements \n'
           'in the K direction (in order to obtain a coarser grid)',
           ''),
    'acof':
          ('[chaine(into=["}"])] closing curly brace', ''),
  }
  _helpDict = check_append_dict('coarsen_operator_uniform_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end coarsen_operator_uniform_Tru class

class coarsen_operators_Tru(ListOfBase_Tru): #####
  """
  not_set
  """
  # From: line 1682, in file 'TRAD2_example'
  _braces = 0
  _comma = 0
  _allowedClasses = ["coarsen_operator_uniform_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 1682]
##### end coarsen_operators_Tru class

class multigrid_solver_Tru(interprete_Tru): #####
  r"""
  Object defining a multigrid solver in IJK discretization
  """
  # From: line 1683, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('coarsen_operators', 'coarsen_operators_Tru'),   #
    ('ghost_size', 'entier_3282e0_Tru'),   #
    ('relax_jacobi', 'list_Tru'),   #
    ('pre_smooth_steps', 'listentier_Tru'),   #
    ('smooth_steps', 'listentier_Tru'),   #
    ('nb_full_mg_steps', 'listentier_Tru'),   #
    ('solveur_grossier', '<inherited>solveur_sys_base_Tru'),   #
    ('seuil', 'floattant_820d8b_Tru'),   #
    ('impr', 'rien_Tru'),   #
    ('solver_precision', 'chaine_c49d25_Tru'),   #
    ('iterations_mixed_solver', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['coarsen_operators',
   'ghost_size',
   'relax_jacobi',
   'pre_smooth_steps',
   'smooth_steps',
   'nb_full_mg_steps',
   'solveur_grossier',
   'seuil',
   'impr',
   'solver_precision',
   'iterations_mixed_solver'])
  _infoMain = ['TRAD2_example', 1683]
  _infoAttr = {'coarsen_operators': ['TRAD2_example', 1684],
   'ghost_size': ['TRAD2_example', 1685],
   'impr': ['TRAD2_example', 1692],
   'iterations_mixed_solver': ['TRAD2_example', 1694],
   'nb_full_mg_steps': ['TRAD2_example', 1689],
   'pre_smooth_steps': ['TRAD2_example', 1687],
   'relax_jacobi': ['TRAD2_example', 1686],
   'seuil': ['TRAD2_example', 1691],
   'smooth_steps': ['TRAD2_example', 1688],
   'solver_precision': ['TRAD2_example', 1693],
   'solveur_grossier': ['TRAD2_example', 1690]}
  _attributesList = check_append_list('multigrid_solver_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('multigrid_solver_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('multigrid_solver_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'coarsen_operators':
          ('[coarsen_operators] Definition of the number of grids that will be used, in addition \n'
           'to the finest (original) grid, followed by the list of the coarsen operators that \n'
           'will be applied to get those grids',
           ''),
    'ghost_size':
          ('[entier] Number of ghost cells known by each processor in each of the three directions', ''),
    'relax_jacobi':
          ('[list] Parameter between 0 and 1 that will be used in the Jacobi method to solve equation \n'
           'on each grid.\n'
           'Should be around 0.7',
           ''),
    'pre_smooth_steps':
          ('[listentier] First integer of the list indicates the numbers of integers that has \n'
           'to be read next.\n'
           'Following integers define the numbers of iterations done before solving the equation \n'
           'on each grid.\n'
           'For example, 2 7 8 means that we have a list of 2 integers, the first one tells us \n'
           'to perform 7 pre-smooth steps on the first grid, the second one tells us to perform \n'
           '8 pre-smooth steps on the second grid.\n'
           'If there are more than 2 grids in the solver, then the remaining ones will have as \n'
           'many pre-smooth steps as the last mentionned number (here, 8)',
           ''),
    'smooth_steps':
          ('[listentier] First integer of the list indicates the numbers of integers that has \n'
           'to be read next.\n'
           'Following integers define the numbers of iterations done after solving the equation \n'
           'on each grid.\n'
           'Same behavior as pre_smooth_steps',
           ''),
    'nb_full_mg_steps':
          ('[listentier] Number of multigrid iterations at each level', ''),
    'solveur_grossier':
          ('[solveur_sys_base] Name of the iterative solver that will be used to solve the system \n'
           'on the coarsest grid.\n'
           'This resolution must be more precise than the ones occurring on the fine grids.\n'
           'The threshold of this solver must therefore be lower than seuil defined above.',
           ''),
    'seuil':
          ('[floattant] Define an upper bound on the norm of the final residue (i.e.\n'
           'the one obtained after applying the multigrid solver).\n'
           'With hybrid precision, as long as we have not obtained a residue whose norm is lower \n'
           'than the imposed threshold, we keep applying the solver',
           ''),
    'impr':
          ('[rien] Flag to display some info on the resolution on eahc grid', ''),
    'solver_precision':
          ('[chaine(into=["mixed","double"])] Precision with which the variables at stake during \n'
           'the resolution of the system will be stored.\n'
           'We can have a simple or double precision or both.\n'
           'In the case of a hybrid precision, the multigrid solver is launched in simple precision, \n'
           'but the residual is calculated in double precision.',
           ''),
    'iterations_mixed_solver':
          ('[entier] Define the maximum number of iterations in mixed precision solver', ''),
  }
  _helpDict = check_append_dict('multigrid_solver_Tru', _helpDict, interprete_Tru._helpDict)
##### end multigrid_solver_Tru class

class test_sse_kernels_Tru(interprete_Tru): #####
  r"""
  Object to test the different kernel methods used in the multigrid solver in IJK discretization
  """
  # From: line 1695, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('nmax', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['nmax'])
  _infoMain = ['TRAD2_example', 1695]
  _infoAttr = {'nmax': ['TRAD2_example', 1696]}
  _attributesList = check_append_list('test_sse_kernels_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('test_sse_kernels_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('test_sse_kernels_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'nmax':
          ('[entier] Number of tests we want to perform', ''),
  }
  _helpDict = check_append_dict('test_sse_kernels_Tru', _helpDict, interprete_Tru._helpDict)
##### end test_sse_kernels_Tru class

class ijk_splitting_Tru(ConstrainBase_Tru): #####
  r"""
  Object to specify how the domain will be divided between processors in IJK discretization
  """
  # From: line 1697, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('ijk_grid_geometry', 'ref_ijk_grid_geometry_Tru'),   #
    ('nproc_i', 'entier_3282e0_Tru'),   #
    ('nproc_j', 'entier_3282e0_Tru'),   #
    ('nproc_k', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1697]
  _infoAttr = {'ijk_grid_geometry': ['TRAD2_example', 1698],
   'nproc_i': ['TRAD2_example', 1699],
   'nproc_j': ['TRAD2_example', 1700],
   'nproc_k': ['TRAD2_example', 1701]}
  _helpDict = {
    'ijk_grid_geometry':
          ('[ref_ijk_grid_geometry] the grid that will be splitted', ''),
    'nproc_i':
          ('[entier] the number of processors into which we will divide the grid following the \nI direction',
           ''),
    'nproc_j':
          ('[entier] the number of processors into which we will divide the grid following the \nJ direction',
           ''),
    'nproc_k':
          ('[entier] the number of processors into which we will divide the grid following the \nK direction',
           ''),
  }
##### end ijk_splitting_Tru class

class parallel_io_parameters_Tru(interprete_Tru): #####
  r"""
  Object to handle parallel files in IJK discretization
  """
  # From: line 1702, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('block_size_bytes', 'entier_3282e0_Tru'),   #
    ('block_size_megabytes', 'entier_3282e0_Tru'),   #
    ('writing_processes', 'entier_3282e0_Tru'),   #
    ('bench_ijk_splitting_write', 'chaine_Tru'),   #
    ('bench_ijk_splitting_read', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['block_size_bytes',
   'block_size_megabytes',
   'writing_processes',
   'bench_ijk_splitting_write',
   'bench_ijk_splitting_read'])
  _infoMain = ['TRAD2_example', 1702]
  _infoAttr = {'bench_ijk_splitting_read': ['TRAD2_example', 1707],
   'bench_ijk_splitting_write': ['TRAD2_example', 1706],
   'block_size_bytes': ['TRAD2_example', 1703],
   'block_size_megabytes': ['TRAD2_example', 1704],
   'writing_processes': ['TRAD2_example', 1705]}
  _attributesList = check_append_list('parallel_io_parameters_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('parallel_io_parameters_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('parallel_io_parameters_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'block_size_bytes':
          ('[entier] File writes will be performed by chunks of this size (in bytes).\n'
           'This parameter will not be taken into account if block_size_megabytes has been defined',
           ''),
    'block_size_megabytes':
          ('[entier] File writes will be performed by chunks of this size (in megabytes).\n'
           'The size should be a multiple of the GPFS block size or lustre stripping size (typically \n'
           'several megabytes)',
           ''),
    'writing_processes':
          ('[entier] This is the number of processes that will write concurrently to the file \n'
           'system (this must be set according to the capacity of the filesystem, set to 1 on \n'
           'small computers, can be up to 64 or 128 on very large systems).',
           ''),
    'bench_ijk_splitting_write':
          ('[chaine] Name of the splitting object we want to use to run a parallel write bench \n'
           '(optional parameter)',
           ''),
    'bench_ijk_splitting_read':
          ('[chaine] Name of the splitting object we want to use to run a parallel read bench \n'
           '(optional parameter)',
           ''),
  }
  _helpDict = check_append_dict('parallel_io_parameters_Tru', _helpDict, interprete_Tru._helpDict)
##### end parallel_io_parameters_Tru class

class ijk_grid_geometry_Tru(domaine_Tru): #####
  r"""
  Object to define the grid that will represent the domain of the simulation in IJK 
  discretization
  """
  # From: line 1708, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('perio_i', 'rien_Tru'),   #
    ('perio_j', 'rien_Tru'),   #
    ('perio_k', 'rien_Tru'),   #
    ('nbelem_i', 'entier_3282e0_Tru'),   #
    ('nbelem_j', 'entier_3282e0_Tru'),   #
    ('nbelem_k', 'entier_3282e0_Tru'),   #
    ('uniform_domain_size_i', 'floattant_820d8b_Tru'),   #
    ('uniform_domain_size_j', 'floattant_820d8b_Tru'),   #
    ('uniform_domain_size_k', 'floattant_820d8b_Tru'),   #
    ('origin_i', 'floattant_820d8b_Tru'),   #
    ('origin_j', 'floattant_820d8b_Tru'),   #
    ('origin_k', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['perio_i',
   'perio_j',
   'perio_k',
   'nbelem_i',
   'nbelem_j',
   'nbelem_k',
   'uniform_domain_size_i',
   'uniform_domain_size_j',
   'uniform_domain_size_k',
   'origin_i',
   'origin_j',
   'origin_k'])
  _infoMain = ['TRAD2_example', 1708]
  _infoAttr = {'nbelem_i': ['TRAD2_example', 1712],
   'nbelem_j': ['TRAD2_example', 1713],
   'nbelem_k': ['TRAD2_example', 1714],
   'origin_i': ['TRAD2_example', 1718],
   'origin_j': ['TRAD2_example', 1719],
   'origin_k': ['TRAD2_example', 1720],
   'perio_i': ['TRAD2_example', 1709],
   'perio_j': ['TRAD2_example', 1710],
   'perio_k': ['TRAD2_example', 1711],
   'uniform_domain_size_i': ['TRAD2_example', 1715],
   'uniform_domain_size_j': ['TRAD2_example', 1716],
   'uniform_domain_size_k': ['TRAD2_example', 1717]}
  _attributesList = check_append_list('ijk_grid_geometry_Tru', _attributesList, domaine_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('ijk_grid_geometry_Tru', _attributesSynonyms, domaine_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('ijk_grid_geometry_Tru', _infoAttr, domaine_Tru._infoAttr)
  _optionals.update(domaine_Tru._optionals)
  _helpDict = {
    'perio_i':
          ('[rien] flag to specify the border along the I direction is periodic', ''),
    'perio_j':
          ('[rien] flag to specify the border along the J direction is periodic', ''),
    'perio_k':
          ('[rien] flag to specify the border along the K direction is periodic', ''),
    'nbelem_i':
          ('[entier] the number of elements of the grid in the I direction', ''),
    'nbelem_j':
          ('[entier] the number of elements of the grid in the J direction', ''),
    'nbelem_k':
          ('[entier] the number of elements of the grid in the K direction', ''),
    'uniform_domain_size_i':
          ('[floattant] the size of the elements along the I direction', ''),
    'uniform_domain_size_j':
          ('[floattant] the size of the elements along the J direction', ''),
    'uniform_domain_size_k':
          ('[floattant] the size of the elements along the K direction', ''),
    'origin_i':
          ('[floattant] I-coordinate of the origin of the grid', ''),
    'origin_j':
          ('[floattant] J-coordinate of the origin of the grid', ''),
    'origin_k':
          ('[floattant] K-coordinate of the origin of the grid', ''),
  }
  _helpDict = check_append_dict('ijk_grid_geometry_Tru', _helpDict, domaine_Tru._helpDict)
##### end ijk_grid_geometry_Tru class

class option_vdf_Tru(interprete_Tru): #####
  r"""
  Class of VDF options.
  """
  # From: line 1721, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('traitement_coins', 'chaine_ad7267_Tru'),   #
    ('traitement_gradients', 'chaine_ad7267_Tru'),   #
    ('p_imposee_aux_faces', 'chaine_ad7267_Tru'),   #
    ('all_options', 'rien_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'all_options': ['toutes_les_options']
  }
  _optionals = set(['traitement_coins',
   'traitement_gradients',
   'p_imposee_aux_faces',
   'all_options'])
  _infoMain = ['TRAD2_example', 1721]
  _infoAttr = {'all_options': ['TRAD2_example', 1725],
   'p_imposee_aux_faces': ['TRAD2_example', 1724],
   'traitement_coins': ['TRAD2_example', 1722],
   'traitement_gradients': ['TRAD2_example', 1723]}
  _attributesList = check_append_list('option_vdf_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('option_vdf_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('option_vdf_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'traitement_coins':
          ('[chaine(into=["oui","non"])] Treatment of corners (yes or no).\n'
           'This option modifies slightly the calculations at the outlet of the plane channel.\n'
           'It supposes that the boundary continues after channel outlet (i.e.\n'
           'velocity vector remains parallel to the boundary).',
           ''),
    'traitement_gradients':
          ('[chaine(into=["oui","non"])] Treatment of gradient calculations (yes or no).\n'
           'This option modifies slightly the gradient calculation at the corners and activates \n'
           'also the corner treatment option.',
           ''),
    'p_imposee_aux_faces':
          ('[chaine(into=["oui","non"])] Pressure imposed at the faces (yes or no).', ''),
    'all_options':
          ('[rien] Activates all Option_VDF options.\n'
           'If used, must be used alone without specifying the other options, nor combinations.',
           ''),
  }
  _helpDict = check_append_dict('option_vdf_Tru', _helpDict, interprete_Tru._helpDict)
##### end option_vdf_Tru class

class champ_front_debit_qc_vdf_Tru(front_field_base_Tru): #####
  r"""
  This keyword is used to define a flow rate field for quasi-compressible fluids in 
  VDF discretization.
  The flow rate is kept constant during a transient.
  """
  # From: line 1726, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('dimension', 'entier_3282e0_Tru'),   #
    ('liste', 'bloc_lecture_Tru'),   #
    ('moyen', 'chaine_Tru'),   #
    ('pb_name', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'dimension': ['dim']
  }
  _optionals = set(['moyen'])
  _infoMain = ['TRAD2_example', 1726]
  _infoAttr = {'dimension': ['TRAD2_example', 1727],
   'liste': ['TRAD2_example', 1728],
   'moyen': ['TRAD2_example', 1729],
   'pb_name': ['TRAD2_example', 1730]}
  _attributesList = check_append_list('champ_front_debit_qc_vdf_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_debit_qc_vdf_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_debit_qc_vdf_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'dimension':
          ('[entier] Problem dimension', ''),
    'liste':
          ('[bloc_lecture] List of the mass flow rate values [kg/s/m2] with the following syntaxe: \n'
           '{ val1 ...\n'
           'valdim }',
           ''),
    'moyen':
          ('[chaine] Option to use rho mean value', ''),
    'pb_name':
          ('[chaine] Problem name', ''),
  }
  _helpDict = check_append_dict('champ_front_debit_qc_vdf_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_debit_qc_vdf_Tru class

class champ_front_debit_qc_vdf_fonc_t_Tru(front_field_base_Tru): #####
  r"""
  This keyword is used to define a flow rate field for quasi-compressible fluids in 
  VDF discretization.
  The flow rate could be constant or time-dependent.
  """
  # From: line 1731, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('dimension', 'entier_3282e0_Tru'),   #
    ('liste', 'bloc_lecture_Tru'),   #
    ('moyen', 'chaine_Tru'),   #
    ('pb_name', 'chaine_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'dimension': ['dim']
  }
  _optionals = set(['moyen'])
  _infoMain = ['TRAD2_example', 1731]
  _infoAttr = {'dimension': ['TRAD2_example', 1732],
   'liste': ['TRAD2_example', 1733],
   'moyen': ['TRAD2_example', 1734],
   'pb_name': ['TRAD2_example', 1735]}
  _attributesList = check_append_list('champ_front_debit_qc_vdf_fonc_t_Tru', _attributesList, front_field_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('champ_front_debit_qc_vdf_fonc_t_Tru', _attributesSynonyms, front_field_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('champ_front_debit_qc_vdf_fonc_t_Tru', _infoAttr, front_field_base_Tru._infoAttr)
  _optionals.update(front_field_base_Tru._optionals)
  _helpDict = {
    'dimension':
          ('[entier] Problem dimension', ''),
    'liste':
          ('[bloc_lecture] List of the mass flow rate values [kg/s/m2] with the following syntaxe: \n'
           '{ val1 ...\n'
           'valdim } where val1 ...\n'
           'valdim are constant or function of time.',
           ''),
    'moyen':
          ('[chaine] Option to use rho mean value', ''),
    'pb_name':
          ('[chaine] Problem name', ''),
  }
  _helpDict = check_append_dict('champ_front_debit_qc_vdf_fonc_t_Tru', _helpDict, front_field_base_Tru._helpDict)
##### end champ_front_debit_qc_vdf_fonc_t_Tru class

class echange_interne_impose_Tru(condlim_base_Tru): #####
  r"""
  Internal heat exchange boundary condition with exchange coefficient.
  """
  # From: line 1736, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['paroi_echange_interne_impose_Tru']
  _attributesList = [
    ('h_imp', 'chaine_Tru'),   #
    ('ch', '<inherited>front_field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1736]
  _infoAttr = {'ch': ['TRAD2_example', 1738], 'h_imp': ['TRAD2_example', 1737]}
  _attributesList = check_append_list('echange_interne_impose_Tru', _attributesList, condlim_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('echange_interne_impose_Tru', _attributesSynonyms, condlim_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('echange_interne_impose_Tru', _infoAttr, condlim_base_Tru._infoAttr)
  _optionals.update(condlim_base_Tru._optionals)
  _helpDict = {
    'h_imp':
          ('[chaine] Exchange coefficient value expressed in W.m-2.K-1.', ''),
    'ch':
          ('[front_field_base] Boundary field type.', ''),
  }
  _helpDict = check_append_dict('echange_interne_impose_Tru', _helpDict, condlim_base_Tru._helpDict)
##### end echange_interne_impose_Tru class

class echange_interne_parfait_Tru(condlim_base_Tru): #####
  r"""
  Internal heat exchange boundary condition with perfect (infinite) exchange coefficient.
  """
  # From: line 1739, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['paroi_echange_interne_parfait_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1739]
  _infoAttr = {}
  _attributesList = check_append_list('echange_interne_parfait_Tru', _attributesList, condlim_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('echange_interne_parfait_Tru', _attributesSynonyms, condlim_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('echange_interne_parfait_Tru', _infoAttr, condlim_base_Tru._infoAttr)
  _optionals.update(condlim_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('echange_interne_parfait_Tru', _helpDict, condlim_base_Tru._helpDict)
##### end echange_interne_parfait_Tru class

class travail_pression_Tru(source_base_Tru): #####
  r"""
  Source term which corresponds to the additional pressure work term that appears when 
  dealing with compressible multiphase fluids
  """
  # From: line 1740, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1740]
  _infoAttr = {}
  _attributesList = check_append_list('travail_pression_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('travail_pression_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('travail_pression_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('travail_pression_Tru', _helpDict, source_base_Tru._helpDict)
##### end travail_pression_Tru class

class convection_ale_Tru(convection_deriv_Tru): #####
  r"""
  A convective scheme for ALE (Arbitrary Lagrangian-Eulerian) framework.
  """
  # From: line 1741, in file 'TRAD2_example'
  _braces = 0
  _synonyms = ['ale_Tru']
  _attributesList = [
    ('opconv', 'bloc_convection_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1741]
  _infoAttr = {'opconv': ['TRAD2_example', 1742]}
  _attributesList = check_append_list('convection_ale_Tru', _attributesList, convection_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_ale_Tru', _attributesSynonyms, convection_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_ale_Tru', _infoAttr, convection_deriv_Tru._infoAttr)
  _optionals.update(convection_deriv_Tru._optionals)
  _helpDict = {
    'opconv':
          ('[bloc_convection] Choice between: amont and muscl\n\nExample: convection { ALE { amont } }', ''),
  }
  _helpDict = check_append_dict('convection_ale_Tru', _helpDict, convection_deriv_Tru._helpDict)
##### end convection_ale_Tru class

class terme_puissance_thermique_echange_impose_Tru(source_base_Tru): #####
  r"""
  Source term to impose thermal power according to formula : P = himp * (T - Text).
  Where T is the Trust temperature, Text is the outside temperature with which energy 
  is exchanged via an exchange coefficient himp
  """
  # From: line 1749, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('himp', '<inherited>field_base_Tru'),   #
    ('text', '<inherited>field_base_Tru'),   #
    ('pid_controler_on_targer_power', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['pid_controler_on_targer_power'])
  _infoMain = ['TRAD2_example', 1749]
  _infoAttr = {'himp': ['TRAD2_example', 1750],
   'pid_controler_on_targer_power': ['TRAD2_example', 1752],
   'text': ['TRAD2_example', 1751]}
  _attributesList = check_append_list('terme_puissance_thermique_echange_impose_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('terme_puissance_thermique_echange_impose_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('terme_puissance_thermique_echange_impose_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'himp':
          ('[field_base] the exchange coefficient', ''),
    'text':
          ('[field_base] the outside temperature', ''),
    'pid_controler_on_targer_power':
          ('[bloc_lecture] PID_controler_on_targer_power bloc with parameters target_power (required), \n'
           'Kp, Ki and Kd (at least one of them should be provided)',
           ''),
  }
  _helpDict = check_append_dict('terme_puissance_thermique_echange_impose_Tru', _helpDict, source_base_Tru._helpDict)
##### end terme_puissance_thermique_echange_impose_Tru class

class fluide_base_Tru(milieu_base_Tru): #####
  r"""
  Basic class for fluids.
  """
  # From: line 1753, in file 'TRAD2_example'
  _braces = -3
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = [
    ('indice', '<inherited>field_base_Tru'),   #
    ('kappa', '<inherited>field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['indice', 'kappa'])
  _infoMain = ['TRAD2_example', 1753]
  _infoAttr = {'indice': ['TRAD2_example', 1754], 'kappa': ['TRAD2_example', 1755]}
  _attributesList = check_append_list('fluide_base_Tru', _attributesList, milieu_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('fluide_base_Tru', _attributesSynonyms, milieu_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('fluide_base_Tru', _infoAttr, milieu_base_Tru._infoAttr)
  _optionals.update(milieu_base_Tru._optionals)
  _helpDict = {
    'indice':
          ('[field_base] Refractivity of fluid.', ''),
    'kappa':
          ('[field_base] Absorptivity of fluid (m-1).', ''),
  }
  _helpDict = check_append_dict('fluide_base_Tru', _helpDict, milieu_base_Tru._helpDict)
##### end fluide_base_Tru class

class fluide_incompressible_Tru(fluide_base_Tru): #####
  r"""
  Class for non-compressible fluids.
  """
  # From: line 1759, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('beta_th', '<inherited>field_base_Tru'),   #
    ('mu', '<inherited>field_base_Tru'),   #
    ('beta_co', '<inherited>field_base_Tru'),   #
    ('rho', '<inherited>field_base_Tru'),   #
    ('cp', '<inherited>field_base_Tru'),   #
    ('Lambda', '<inherited>field_base_Tru'),   #
    ('porosites', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'Lambda': ['lambda_u']
  }
  _optionals = set(['beta_th', 'mu', 'beta_co', 'rho', 'cp', 'Lambda', 'porosites'])
  _infoMain = ['TRAD2_example', 1759]
  _infoAttr = {'Lambda': ['TRAD2_example', 1765],
   'beta_co': ['TRAD2_example', 1762],
   'beta_th': ['TRAD2_example', 1760],
   'cp': ['TRAD2_example', 1764],
   'mu': ['TRAD2_example', 1761],
   'porosites': ['TRAD2_example', 1766],
   'rho': ['TRAD2_example', 1763]}
  _attributesList = check_append_list('fluide_incompressible_Tru', _attributesList, fluide_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('fluide_incompressible_Tru', _attributesSynonyms, fluide_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('fluide_incompressible_Tru', _infoAttr, fluide_base_Tru._infoAttr)
  _optionals.update(fluide_base_Tru._optionals)
  _helpDict = {
    'beta_th':
          ('[field_base] Thermal expansion (K-1).', ''),
    'mu':
          ('[field_base] Dynamic viscosity (kg.m-1.s-1).', ''),
    'beta_co':
          ('[field_base] Volume expansion coefficient values in concentration.', ''),
    'rho':
          ('[field_base] Density (kg.m-3).', ''),
    'cp':
          ('[field_base] Specific heat (J.kg-1.K-1).', ''),
    'Lambda':
          ('[field_base] Conductivity (W.m-1.K-1).', ''),
    'porosites':
          ('[bloc_lecture] Porosity (optional)', ''),
  }
  _helpDict = check_append_dict('fluide_incompressible_Tru', _helpDict, fluide_base_Tru._helpDict)
##### end fluide_incompressible_Tru class

class fluide_ostwald_Tru(fluide_incompressible_Tru): #####
  r"""
  Non-Newtonian fluids governed by Ostwald's law.
  The law applicable to stress tensor is:
  
  tau=K(T)*(D:D/2)**((n-1)/2)*D Where:
  
  D refers to the deformation tensor
  
  K refers to fluid consistency (may be a function of the temperature T)
  
  n refers to the fluid structure index n=1 for a Newtonian fluid, n<1 for a rheofluidifier 
  fluid, n>1 for a rheothickening fluid.
  """
  # From: line 1756, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('k', '<inherited>field_base_Tru'),   #
    ('n', '<inherited>field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['k', 'n'])
  _infoMain = ['TRAD2_example', 1756]
  _infoAttr = {'k': ['TRAD2_example', 1757], 'n': ['TRAD2_example', 1758]}
  _attributesList = check_append_list('fluide_ostwald_Tru', _attributesList, fluide_incompressible_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('fluide_ostwald_Tru', _attributesSynonyms, fluide_incompressible_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('fluide_ostwald_Tru', _infoAttr, fluide_incompressible_Tru._infoAttr)
  _optionals.update(fluide_incompressible_Tru._optionals)
  _helpDict = {
    'k':
          ('[field_base] Fluid consistency.', ''),
    'n':
          ('[field_base] Fluid structure index.', ''),
  }
  _helpDict = check_append_dict('fluide_ostwald_Tru', _helpDict, fluide_incompressible_Tru._helpDict)
##### end fluide_ostwald_Tru class

class pb_thermohydraulique_Tru(pb_base_Tru): #####
  r"""
  Resolution of thermohydraulic problem.
  """
  # From: line 1767, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_incompressible', '<inherited>fluide_incompressible_Tru'),   #
    ('fluide_ostwald', '<inherited>fluide_ostwald_Tru'),   #
    ('fluide_sodium_liquide', '<inherited>fluide_sodium_liquide_Tru'),   #
    ('fluide_sodium_gaz', '<inherited>fluide_sodium_gaz_Tru'),   #
    ('navier_stokes_standard', 'navier_stokes_standard_Tru'),   #
    ('convection_diffusion_temperature', 'convection_diffusion_temperature_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['fluide_incompressible',
   'fluide_ostwald',
   'fluide_sodium_liquide',
   'fluide_sodium_gaz',
   'navier_stokes_standard',
   'convection_diffusion_temperature'])
  _infoMain = ['TRAD2_example', 1767]
  _infoAttr = {'convection_diffusion_temperature': ['TRAD2_example', 1773],
   'fluide_incompressible': ['TRAD2_example', 1768],
   'fluide_ostwald': ['TRAD2_example', 1769],
   'fluide_sodium_gaz': ['TRAD2_example', 1771],
   'fluide_sodium_liquide': ['TRAD2_example', 1770],
   'navier_stokes_standard': ['TRAD2_example', 1772]}
  _attributesList = check_append_list('pb_thermohydraulique_Tru', _attributesList, pb_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_thermohydraulique_Tru', _attributesSynonyms, pb_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_thermohydraulique_Tru', _infoAttr, pb_base_Tru._infoAttr)
  _optionals.update(pb_base_Tru._optionals)
  _helpDict = {
    'fluide_incompressible':
          ('[fluide_incompressible] The fluid medium associated with the problem (only one possibility).', ''),
    'fluide_ostwald':
          ('[fluide_ostwald] The fluid medium associated with the problem (only one possibility).', ''),
    'fluide_sodium_liquide':
          ('[fluide_sodium_liquide] The fluid medium associated with the problem (only one possibility).', ''),
    'fluide_sodium_gaz':
          ('[fluide_sodium_gaz] The fluid medium associated with the problem (only one possibility).', ''),
    'navier_stokes_standard':
          ('[navier_stokes_standard] Navier-Stokes equations.', ''),
    'convection_diffusion_temperature':
          ('[convection_diffusion_temperature] Energy equation (temperature diffusion convection).', ''),
  }
  _helpDict = check_append_dict('pb_thermohydraulique_Tru', _helpDict, pb_base_Tru._helpDict)
##### end pb_thermohydraulique_Tru class

class pb_hydraulique_concentration_Tru(pb_base_Tru): #####
  r"""
  Resolution of Navier-Stokes/multiple constituent transport equations.
  """
  # From: line 1774, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_incompressible', '<inherited>fluide_incompressible_Tru'),   #
    ('constituant', 'constituant_Tru'),   #
    ('navier_stokes_standard', 'navier_stokes_standard_Tru'),   #
    ('convection_diffusion_concentration', 'convection_diffusion_concentration_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['constituant', 'navier_stokes_standard', 'convection_diffusion_concentration'])
  _infoMain = ['TRAD2_example', 1774]
  _infoAttr = {'constituant': ['TRAD2_example', 1776],
   'convection_diffusion_concentration': ['TRAD2_example', 1778],
   'fluide_incompressible': ['TRAD2_example', 1775],
   'navier_stokes_standard': ['TRAD2_example', 1777]}
  _attributesList = check_append_list('pb_hydraulique_concentration_Tru', _attributesList, pb_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_hydraulique_concentration_Tru', _attributesSynonyms, pb_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_hydraulique_concentration_Tru', _infoAttr, pb_base_Tru._infoAttr)
  _optionals.update(pb_base_Tru._optionals)
  _helpDict = {
    'fluide_incompressible':
          ('[fluide_incompressible] The fluid medium associated with the problem.', ''),
    'constituant':
          ('[constituant] Constituents.', ''),
    'navier_stokes_standard':
          ('[navier_stokes_standard] Navier-Stokes equations.', ''),
    'convection_diffusion_concentration':
          ('[convection_diffusion_concentration] Constituent transport vectorial equation (concentration \n'
           'diffusion convection).',
           ''),
  }
  _helpDict = check_append_dict('pb_hydraulique_concentration_Tru', _helpDict, pb_base_Tru._helpDict)
##### end pb_hydraulique_concentration_Tru class

class listeqn_Tru(ListOfBase_Tru): #####
  """
  List of equations.
  """
  # From: line 1779, in file 'TRAD2_example'
  _braces = 1
  _comma = 0
  _allowedClasses = ["eqn_base_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 1779]
##### end listeqn_Tru class

class pb_avec_passif_Tru(pb_base_Tru): #####
  r"""
  Class to create a classical problem with a scalar transport equation (e.g: temperature 
  or concentration) and an additional set of passive scalars (e.g: temperature or concentration) 
  equations.
  """
  # From: line 1780, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('equations_scalaires_passifs', 'listeqn_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1780]
  _infoAttr = {'equations_scalaires_passifs': ['TRAD2_example', 1781]}
  _attributesList = check_append_list('pb_avec_passif_Tru', _attributesList, pb_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_avec_passif_Tru', _attributesSynonyms, pb_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_avec_passif_Tru', _infoAttr, pb_base_Tru._infoAttr)
  _optionals.update(pb_base_Tru._optionals)
  _helpDict = {
    'equations_scalaires_passifs':
          ('[listeqn] Passive scalar equations.\n'
           'The unknowns of the passive scalar equation number N are named temperatureN or concentrationN \n'
           'or fraction_massiqueN.\n'
           'This keyword is used to define initial conditions and the post processing fields.\n'
           'This kind of problem is very useful to test in only one data file (and then only \n'
           'one calculation) different schemes or different boundary conditions for the scalar \n'
           'transport equation.',
           ''),
  }
  _helpDict = check_append_dict('pb_avec_passif_Tru', _helpDict, pb_base_Tru._helpDict)
##### end pb_avec_passif_Tru class

class pb_thermohydraulique_concentration_scalaires_passifs_Tru(pb_avec_passif_Tru): #####
  r"""
  Resolution of Navier-Stokes/energy/multiple constituent transport equations, with 
  the additional passive scalar equations.
  """
  # From: line 1782, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_incompressible', '<inherited>fluide_incompressible_Tru'),   #
    ('constituant', 'constituant_Tru'),   #
    ('navier_stokes_standard', 'navier_stokes_standard_Tru'),   #
    ('convection_diffusion_concentration', 'convection_diffusion_concentration_Tru'),   #
    ('convection_diffusion_temperature', 'convection_diffusion_temperature_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['constituant',
   'navier_stokes_standard',
   'convection_diffusion_concentration',
   'convection_diffusion_temperature'])
  _infoMain = ['TRAD2_example', 1782]
  _infoAttr = {'constituant': ['TRAD2_example', 1784],
   'convection_diffusion_concentration': ['TRAD2_example', 1786],
   'convection_diffusion_temperature': ['TRAD2_example', 1787],
   'fluide_incompressible': ['TRAD2_example', 1783],
   'navier_stokes_standard': ['TRAD2_example', 1785]}
  _attributesList = check_append_list('pb_thermohydraulique_concentration_scalaires_passifs_Tru', _attributesList, pb_avec_passif_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_thermohydraulique_concentration_scalaires_passifs_Tru', _attributesSynonyms, pb_avec_passif_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_thermohydraulique_concentration_scalaires_passifs_Tru', _infoAttr, pb_avec_passif_Tru._infoAttr)
  _optionals.update(pb_avec_passif_Tru._optionals)
  _helpDict = {
    'fluide_incompressible':
          ('[fluide_incompressible] The fluid medium associated with the problem.', ''),
    'constituant':
          ('[constituant] Constituents.', ''),
    'navier_stokes_standard':
          ('[navier_stokes_standard] Navier-Stokes equations.', ''),
    'convection_diffusion_concentration':
          ('[convection_diffusion_concentration] Constituent transport equations (concentration \n'
           'diffusion convection).',
           ''),
    'convection_diffusion_temperature':
          ('[convection_diffusion_temperature] Energy equations (temperature diffusion convection).', ''),
  }
  _helpDict = check_append_dict('pb_thermohydraulique_concentration_scalaires_passifs_Tru', _helpDict, pb_avec_passif_Tru._helpDict)
##### end pb_thermohydraulique_concentration_scalaires_passifs_Tru class

class pb_thermohydraulique_scalaires_passifs_Tru(pb_avec_passif_Tru): #####
  r"""
  Resolution of thermohydraulic problem, with the additional passive scalar equations.
  """
  # From: line 1788, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_incompressible', '<inherited>fluide_incompressible_Tru'),   #
    ('constituant', 'constituant_Tru'),   #
    ('navier_stokes_standard', 'navier_stokes_standard_Tru'),   #
    ('convection_diffusion_temperature', 'convection_diffusion_temperature_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['constituant', 'navier_stokes_standard', 'convection_diffusion_temperature'])
  _infoMain = ['TRAD2_example', 1788]
  _infoAttr = {'constituant': ['TRAD2_example', 1790],
   'convection_diffusion_temperature': ['TRAD2_example', 1792],
   'fluide_incompressible': ['TRAD2_example', 1789],
   'navier_stokes_standard': ['TRAD2_example', 1791]}
  _attributesList = check_append_list('pb_thermohydraulique_scalaires_passifs_Tru', _attributesList, pb_avec_passif_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_thermohydraulique_scalaires_passifs_Tru', _attributesSynonyms, pb_avec_passif_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_thermohydraulique_scalaires_passifs_Tru', _infoAttr, pb_avec_passif_Tru._infoAttr)
  _optionals.update(pb_avec_passif_Tru._optionals)
  _helpDict = {
    'fluide_incompressible':
          ('[fluide_incompressible] The fluid medium associated with the problem.', ''),
    'constituant':
          ('[constituant] Constituents.', ''),
    'navier_stokes_standard':
          ('[navier_stokes_standard] Navier-Stokes equations.', ''),
    'convection_diffusion_temperature':
          ('[convection_diffusion_temperature] Energy equations (temperature diffusion convection).', ''),
  }
  _helpDict = check_append_dict('pb_thermohydraulique_scalaires_passifs_Tru', _helpDict, pb_avec_passif_Tru._helpDict)
##### end pb_thermohydraulique_scalaires_passifs_Tru class

class pb_hydraulique_concentration_scalaires_passifs_Tru(pb_avec_passif_Tru): #####
  r"""
  Resolution of Navier-Stokes/multiple constituent transport equations with the additional 
  passive scalar equations.
  """
  # From: line 1793, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_incompressible', '<inherited>fluide_incompressible_Tru'),   #
    ('constituant', 'constituant_Tru'),   #
    ('navier_stokes_standard', 'navier_stokes_standard_Tru'),   #
    ('convection_diffusion_concentration', 'convection_diffusion_concentration_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['constituant', 'navier_stokes_standard', 'convection_diffusion_concentration'])
  _infoMain = ['TRAD2_example', 1793]
  _infoAttr = {'constituant': ['TRAD2_example', 1795],
   'convection_diffusion_concentration': ['TRAD2_example', 1797],
   'fluide_incompressible': ['TRAD2_example', 1794],
   'navier_stokes_standard': ['TRAD2_example', 1796]}
  _attributesList = check_append_list('pb_hydraulique_concentration_scalaires_passifs_Tru', _attributesList, pb_avec_passif_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_hydraulique_concentration_scalaires_passifs_Tru', _attributesSynonyms, pb_avec_passif_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_hydraulique_concentration_scalaires_passifs_Tru', _infoAttr, pb_avec_passif_Tru._infoAttr)
  _optionals.update(pb_avec_passif_Tru._optionals)
  _helpDict = {
    'fluide_incompressible':
          ('[fluide_incompressible] The fluid medium associated with the problem.', ''),
    'constituant':
          ('[constituant] Constituents.', ''),
    'navier_stokes_standard':
          ('[navier_stokes_standard] Navier-Stokes equations.', ''),
    'convection_diffusion_concentration':
          ('[convection_diffusion_concentration] Constituent transport equations (concentration \n'
           'diffusion convection).',
           ''),
  }
  _helpDict = check_append_dict('pb_hydraulique_concentration_scalaires_passifs_Tru', _helpDict, pb_avec_passif_Tru._helpDict)
##### end pb_hydraulique_concentration_scalaires_passifs_Tru class

class pb_thermohydraulique_concentration_Tru(pb_base_Tru): #####
  r"""
  Resolution of Navier-Stokes/energy/multiple constituent transport equations.
  """
  # From: line 1798, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_incompressible', '<inherited>fluide_incompressible_Tru'),   #
    ('constituant', 'constituant_Tru'),   #
    ('navier_stokes_standard', 'navier_stokes_standard_Tru'),   #
    ('convection_diffusion_concentration', 'convection_diffusion_concentration_Tru'),   #
    ('convection_diffusion_temperature', 'convection_diffusion_temperature_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['constituant',
   'navier_stokes_standard',
   'convection_diffusion_concentration',
   'convection_diffusion_temperature'])
  _infoMain = ['TRAD2_example', 1798]
  _infoAttr = {'constituant': ['TRAD2_example', 1800],
   'convection_diffusion_concentration': ['TRAD2_example', 1802],
   'convection_diffusion_temperature': ['TRAD2_example', 1803],
   'fluide_incompressible': ['TRAD2_example', 1799],
   'navier_stokes_standard': ['TRAD2_example', 1801]}
  _attributesList = check_append_list('pb_thermohydraulique_concentration_Tru', _attributesList, pb_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_thermohydraulique_concentration_Tru', _attributesSynonyms, pb_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_thermohydraulique_concentration_Tru', _infoAttr, pb_base_Tru._infoAttr)
  _optionals.update(pb_base_Tru._optionals)
  _helpDict = {
    'fluide_incompressible':
          ('[fluide_incompressible] The fluid medium associated with the problem.', ''),
    'constituant':
          ('[constituant] Constituents.', ''),
    'navier_stokes_standard':
          ('[navier_stokes_standard] Navier-Stokes equations.', ''),
    'convection_diffusion_concentration':
          ('[convection_diffusion_concentration] Constituent transport equations (concentration \n'
           'diffusion convection).',
           ''),
    'convection_diffusion_temperature':
          ('[convection_diffusion_temperature] Energy equation (temperature diffusion convection).', ''),
  }
  _helpDict = check_append_dict('pb_thermohydraulique_concentration_Tru', _helpDict, pb_base_Tru._helpDict)
##### end pb_thermohydraulique_concentration_Tru class

class pb_hydraulique_Tru(pb_base_Tru): #####
  r"""
  Resolution of the Navier-Stokes equations.
  """
  # From: line 1804, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_incompressible', '<inherited>fluide_incompressible_Tru'),   #
    ('navier_stokes_standard', 'navier_stokes_standard_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1804]
  _infoAttr = {'fluide_incompressible': ['TRAD2_example', 1805],
   'navier_stokes_standard': ['TRAD2_example', 1806]}
  _attributesList = check_append_list('pb_hydraulique_Tru', _attributesList, pb_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_hydraulique_Tru', _attributesSynonyms, pb_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_hydraulique_Tru', _infoAttr, pb_base_Tru._infoAttr)
  _optionals.update(pb_base_Tru._optionals)
  _helpDict = {
    'fluide_incompressible':
          ('[fluide_incompressible] The fluid medium associated with the problem.', ''),
    'navier_stokes_standard':
          ('[navier_stokes_standard] Navier-Stokes equations.', ''),
  }
  _helpDict = check_append_dict('pb_hydraulique_Tru', _helpDict, pb_base_Tru._helpDict)
##### end pb_hydraulique_Tru class

class neumann_paroi_adiabatique_Tru(neumann_homogene_Tru): #####
  r"""
  Adiabatic wall neumann boundary condition
  """
  # From: line 1807, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1807]
  _infoAttr = {}
  _attributesList = check_append_list('neumann_paroi_adiabatique_Tru', _attributesList, neumann_homogene_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('neumann_paroi_adiabatique_Tru', _attributesSynonyms, neumann_homogene_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('neumann_paroi_adiabatique_Tru', _infoAttr, neumann_homogene_Tru._infoAttr)
  _optionals.update(neumann_homogene_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('neumann_paroi_adiabatique_Tru', _helpDict, neumann_homogene_Tru._helpDict)
##### end neumann_paroi_adiabatique_Tru class

class pb_hydraulique_turbulent_Tru(pb_base_Tru): #####
  r"""
  Resolution of Navier-Stokes equations with turbulence modelling.
  """
  # From: line 1808, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_incompressible', '<inherited>fluide_incompressible_Tru'),   #
    ('navier_stokes_turbulent', 'navier_stokes_turbulent_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1808]
  _infoAttr = {'fluide_incompressible': ['TRAD2_example', 1809],
   'navier_stokes_turbulent': ['TRAD2_example', 1810]}
  _attributesList = check_append_list('pb_hydraulique_turbulent_Tru', _attributesList, pb_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_hydraulique_turbulent_Tru', _attributesSynonyms, pb_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_hydraulique_turbulent_Tru', _infoAttr, pb_base_Tru._infoAttr)
  _optionals.update(pb_base_Tru._optionals)
  _helpDict = {
    'fluide_incompressible':
          ('[fluide_incompressible] The fluid medium associated with the problem.', ''),
    'navier_stokes_turbulent':
          ('[navier_stokes_turbulent] Navier-Stokes equations as well as the associated turbulence \n'
           'model equations.',
           ''),
  }
  _helpDict = check_append_dict('pb_hydraulique_turbulent_Tru', _helpDict, pb_base_Tru._helpDict)
##### end pb_hydraulique_turbulent_Tru class

class pb_hydraulique_concentration_turbulent_Tru(pb_base_Tru): #####
  r"""
  Resolution of Navier-Stokes/multiple constituent transport equations, with turbulence 
  modelling.
  """
  # From: line 1811, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_incompressible', '<inherited>fluide_incompressible_Tru'),   #
    ('constituant', 'constituant_Tru'),   #
    ('navier_stokes_turbulent', 'navier_stokes_turbulent_Tru'),   #
    ('convection_diffusion_concentration_turbulent', 'convection_diffusion_concentration_turbulent_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['constituant',
   'navier_stokes_turbulent',
   'convection_diffusion_concentration_turbulent'])
  _infoMain = ['TRAD2_example', 1811]
  _infoAttr = {'constituant': ['TRAD2_example', 1813],
   'convection_diffusion_concentration_turbulent': ['TRAD2_example', 1815],
   'fluide_incompressible': ['TRAD2_example', 1812],
   'navier_stokes_turbulent': ['TRAD2_example', 1814]}
  _attributesList = check_append_list('pb_hydraulique_concentration_turbulent_Tru', _attributesList, pb_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_hydraulique_concentration_turbulent_Tru', _attributesSynonyms, pb_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_hydraulique_concentration_turbulent_Tru', _infoAttr, pb_base_Tru._infoAttr)
  _optionals.update(pb_base_Tru._optionals)
  _helpDict = {
    'fluide_incompressible':
          ('[fluide_incompressible] The fluid medium associated with the problem.', ''),
    'constituant':
          ('[constituant] Constituents.', ''),
    'navier_stokes_turbulent':
          ('[navier_stokes_turbulent] Navier-Stokes equations as well as the associated turbulence \n'
           'model equations.',
           ''),
    'convection_diffusion_concentration_turbulent':
          ('[convection_diffusion_concentration_turbulent] Constituent transport equations (concentration \n'
           'diffusion convection) as well as the associated turbulence model equations.',
           ''),
  }
  _helpDict = check_append_dict('pb_hydraulique_concentration_turbulent_Tru', _helpDict, pb_base_Tru._helpDict)
##### end pb_hydraulique_concentration_turbulent_Tru class

class pb_thermohydraulique_turbulent_Tru(pb_base_Tru): #####
  r"""
  Resolution of thermohydraulic problem, with turbulence modelling.
  """
  # From: line 1816, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_incompressible', '<inherited>fluide_incompressible_Tru'),   #
    ('navier_stokes_turbulent', 'navier_stokes_turbulent_Tru'),   #
    ('convection_diffusion_temperature_turbulent', '<inherited>convection_diffusion_temperature_turbulent_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1816]
  _infoAttr = {'convection_diffusion_temperature_turbulent': ['TRAD2_example', 1819],
   'fluide_incompressible': ['TRAD2_example', 1817],
   'navier_stokes_turbulent': ['TRAD2_example', 1818]}
  _attributesList = check_append_list('pb_thermohydraulique_turbulent_Tru', _attributesList, pb_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_thermohydraulique_turbulent_Tru', _attributesSynonyms, pb_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_thermohydraulique_turbulent_Tru', _infoAttr, pb_base_Tru._infoAttr)
  _optionals.update(pb_base_Tru._optionals)
  _helpDict = {
    'fluide_incompressible':
          ('[fluide_incompressible] The fluid medium associated with the problem.', ''),
    'navier_stokes_turbulent':
          ('[navier_stokes_turbulent] Navier-Stokes equations as well as the associated turbulence \n'
           'model equations.',
           ''),
    'convection_diffusion_temperature_turbulent':
          ('[convection_diffusion_temperature_turbulent] Energy equation (temperature diffusion \n'
           'convection) as well as the associated turbulence model equations.',
           ''),
  }
  _helpDict = check_append_dict('pb_thermohydraulique_turbulent_Tru', _helpDict, pb_base_Tru._helpDict)
##### end pb_thermohydraulique_turbulent_Tru class

class pb_thermohydraulique_turbulent_scalaires_passifs_Tru(pb_avec_passif_Tru): #####
  r"""
  Resolution of thermohydraulic problem, with turbulence modelling and with the additional 
  passive scalar equations.
  """
  # From: line 1820, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_incompressible', '<inherited>fluide_incompressible_Tru'),   #
    ('constituant', 'constituant_Tru'),   #
    ('navier_stokes_turbulent', 'navier_stokes_turbulent_Tru'),   #
    ('convection_diffusion_temperature_turbulent', '<inherited>convection_diffusion_temperature_turbulent_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['constituant',
   'navier_stokes_turbulent',
   'convection_diffusion_temperature_turbulent'])
  _infoMain = ['TRAD2_example', 1820]
  _infoAttr = {'constituant': ['TRAD2_example', 1822],
   'convection_diffusion_temperature_turbulent': ['TRAD2_example', 1824],
   'fluide_incompressible': ['TRAD2_example', 1821],
   'navier_stokes_turbulent': ['TRAD2_example', 1823]}
  _attributesList = check_append_list('pb_thermohydraulique_turbulent_scalaires_passifs_Tru', _attributesList, pb_avec_passif_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_thermohydraulique_turbulent_scalaires_passifs_Tru', _attributesSynonyms, pb_avec_passif_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_thermohydraulique_turbulent_scalaires_passifs_Tru', _infoAttr, pb_avec_passif_Tru._infoAttr)
  _optionals.update(pb_avec_passif_Tru._optionals)
  _helpDict = {
    'fluide_incompressible':
          ('[fluide_incompressible] The fluid medium associated with the problem.', ''),
    'constituant':
          ('[constituant] Constituents.', ''),
    'navier_stokes_turbulent':
          ('[navier_stokes_turbulent] Navier-Stokes equations as well as the associated turbulence \n'
           'model equations.',
           ''),
    'convection_diffusion_temperature_turbulent':
          ('[convection_diffusion_temperature_turbulent] Energy equations (temperature diffusion \n'
           'convection) as well as the associated turbulence model equations.',
           ''),
  }
  _helpDict = check_append_dict('pb_thermohydraulique_turbulent_scalaires_passifs_Tru', _helpDict, pb_avec_passif_Tru._helpDict)
##### end pb_thermohydraulique_turbulent_scalaires_passifs_Tru class

class pb_thermohydraulique_especes_turbulent_qc_Tru(pb_avec_passif_Tru): #####
  r"""
  Resolution of turbulent thermohydraulic problem under low Mach number with passive 
  scalar equations.
  """
  # From: line 1825, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_quasi_compressible', '<inherited>fluide_quasi_compressible_Tru'),   #
    ('navier_stokes_turbulent_qc', 'navier_stokes_turbulent_qc_Tru'),   #
    ('convection_diffusion_chaleur_turbulent_qc', '<inherited>convection_diffusion_chaleur_turbulent_qc_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1825]
  _infoAttr = {'convection_diffusion_chaleur_turbulent_qc': ['TRAD2_example', 1828],
   'fluide_quasi_compressible': ['TRAD2_example', 1826],
   'navier_stokes_turbulent_qc': ['TRAD2_example', 1827]}
  _attributesList = check_append_list('pb_thermohydraulique_especes_turbulent_qc_Tru', _attributesList, pb_avec_passif_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_thermohydraulique_especes_turbulent_qc_Tru', _attributesSynonyms, pb_avec_passif_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_thermohydraulique_especes_turbulent_qc_Tru', _infoAttr, pb_avec_passif_Tru._infoAttr)
  _optionals.update(pb_avec_passif_Tru._optionals)
  _helpDict = {
    'fluide_quasi_compressible':
          ('[fluide_quasi_compressible] The fluid medium associated with the problem.', ''),
    'navier_stokes_turbulent_qc':
          ('[navier_stokes_turbulent_qc] Navier-Stokes equations under low Mach number as well \n'
           'as the associated turbulence model equations.',
           ''),
    'convection_diffusion_chaleur_turbulent_qc':
          ('[convection_diffusion_chaleur_turbulent_qc] Energy equation under low Mach number \n'
           'as well as the associated turbulence model equations.',
           ''),
  }
  _helpDict = check_append_dict('pb_thermohydraulique_especes_turbulent_qc_Tru', _helpDict, pb_avec_passif_Tru._helpDict)
##### end pb_thermohydraulique_especes_turbulent_qc_Tru class

class pb_hydraulique_concentration_turbulent_scalaires_passifs_Tru(pb_avec_passif_Tru): #####
  r"""
  Resolution of Navier-Stokes/multiple constituent transport equations, with turbulence 
  modelling and with the additional passive scalar equations.
  """
  # From: line 1829, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_incompressible', '<inherited>fluide_incompressible_Tru'),   #
    ('constituant', 'constituant_Tru'),   #
    ('navier_stokes_turbulent', 'navier_stokes_turbulent_Tru'),   #
    ('convection_diffusion_concentration_turbulent', 'convection_diffusion_concentration_turbulent_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['constituant',
   'navier_stokes_turbulent',
   'convection_diffusion_concentration_turbulent'])
  _infoMain = ['TRAD2_example', 1829]
  _infoAttr = {'constituant': ['TRAD2_example', 1831],
   'convection_diffusion_concentration_turbulent': ['TRAD2_example', 1833],
   'fluide_incompressible': ['TRAD2_example', 1830],
   'navier_stokes_turbulent': ['TRAD2_example', 1832]}
  _attributesList = check_append_list('pb_hydraulique_concentration_turbulent_scalaires_passifs_Tru', _attributesList, pb_avec_passif_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_hydraulique_concentration_turbulent_scalaires_passifs_Tru', _attributesSynonyms, pb_avec_passif_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_hydraulique_concentration_turbulent_scalaires_passifs_Tru', _infoAttr, pb_avec_passif_Tru._infoAttr)
  _optionals.update(pb_avec_passif_Tru._optionals)
  _helpDict = {
    'fluide_incompressible':
          ('[fluide_incompressible] The fluid medium associated with the problem.', ''),
    'constituant':
          ('[constituant] Constituents.', ''),
    'navier_stokes_turbulent':
          ('[navier_stokes_turbulent] Navier-Stokes equations as well as the associated turbulence \n'
           'model equations.',
           ''),
    'convection_diffusion_concentration_turbulent':
          ('[convection_diffusion_concentration_turbulent] Constituent transport equations (concentration \n'
           'diffusion convection) as well as the associated turbulence model equations.',
           ''),
  }
  _helpDict = check_append_dict('pb_hydraulique_concentration_turbulent_scalaires_passifs_Tru', _helpDict, pb_avec_passif_Tru._helpDict)
##### end pb_hydraulique_concentration_turbulent_scalaires_passifs_Tru class

class pb_thermohydraulique_concentration_turbulent_scalaires_passifs_Tru(pb_avec_passif_Tru): #####
  r"""
  Resolution of Navier-Stokes/energy/multiple constituent transport equations, with 
  turbulence modelling and with the additional passive scalar equations.
  """
  # From: line 1834, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_incompressible', '<inherited>fluide_incompressible_Tru'),   #
    ('constituant', 'constituant_Tru'),   #
    ('navier_stokes_turbulent', 'navier_stokes_turbulent_Tru'),   #
    ('convection_diffusion_concentration_turbulent', 'convection_diffusion_concentration_turbulent_Tru'),   #
    ('convection_diffusion_temperature_turbulent', '<inherited>convection_diffusion_temperature_turbulent_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['constituant',
   'navier_stokes_turbulent',
   'convection_diffusion_concentration_turbulent',
   'convection_diffusion_temperature_turbulent'])
  _infoMain = ['TRAD2_example', 1834]
  _infoAttr = {'constituant': ['TRAD2_example', 1836],
   'convection_diffusion_concentration_turbulent': ['TRAD2_example', 1838],
   'convection_diffusion_temperature_turbulent': ['TRAD2_example', 1839],
   'fluide_incompressible': ['TRAD2_example', 1835],
   'navier_stokes_turbulent': ['TRAD2_example', 1837]}
  _attributesList = check_append_list('pb_thermohydraulique_concentration_turbulent_scalaires_passifs_Tru', _attributesList, pb_avec_passif_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_thermohydraulique_concentration_turbulent_scalaires_passifs_Tru', _attributesSynonyms, pb_avec_passif_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_thermohydraulique_concentration_turbulent_scalaires_passifs_Tru', _infoAttr, pb_avec_passif_Tru._infoAttr)
  _optionals.update(pb_avec_passif_Tru._optionals)
  _helpDict = {
    'fluide_incompressible':
          ('[fluide_incompressible] The fluid medium associated with the problem.', ''),
    'constituant':
          ('[constituant] Constituents.', ''),
    'navier_stokes_turbulent':
          ('[navier_stokes_turbulent] Navier-Stokes equations as well as the associated turbulence \n'
           'model equations.',
           ''),
    'convection_diffusion_concentration_turbulent':
          ('[convection_diffusion_concentration_turbulent] Constituent transport equations (concentration \n'
           'diffusion convection) as well as the associated turbulence model equations.',
           ''),
    'convection_diffusion_temperature_turbulent':
          ('[convection_diffusion_temperature_turbulent] Energy equations (temperature diffusion \n'
           'convection) as well as the associated turbulence model equations.',
           ''),
  }
  _helpDict = check_append_dict('pb_thermohydraulique_concentration_turbulent_scalaires_passifs_Tru', _helpDict, pb_avec_passif_Tru._helpDict)
##### end pb_thermohydraulique_concentration_turbulent_scalaires_passifs_Tru class

class pb_thermohydraulique_concentration_turbulent_Tru(pb_base_Tru): #####
  r"""
  Resolution of Navier-Stokes/energy/multiple constituent transport equations, with 
  turbulence modelling.
  """
  # From: line 1840, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_incompressible', '<inherited>fluide_incompressible_Tru'),   #
    ('constituant', 'constituant_Tru'),   #
    ('navier_stokes_turbulent', 'navier_stokes_turbulent_Tru'),   #
    ('convection_diffusion_concentration_turbulent', 'convection_diffusion_concentration_turbulent_Tru'),   #
    ('convection_diffusion_temperature_turbulent', '<inherited>convection_diffusion_temperature_turbulent_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['constituant',
   'navier_stokes_turbulent',
   'convection_diffusion_concentration_turbulent',
   'convection_diffusion_temperature_turbulent'])
  _infoMain = ['TRAD2_example', 1840]
  _infoAttr = {'constituant': ['TRAD2_example', 1842],
   'convection_diffusion_concentration_turbulent': ['TRAD2_example', 1844],
   'convection_diffusion_temperature_turbulent': ['TRAD2_example', 1845],
   'fluide_incompressible': ['TRAD2_example', 1841],
   'navier_stokes_turbulent': ['TRAD2_example', 1843]}
  _attributesList = check_append_list('pb_thermohydraulique_concentration_turbulent_Tru', _attributesList, pb_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_thermohydraulique_concentration_turbulent_Tru', _attributesSynonyms, pb_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_thermohydraulique_concentration_turbulent_Tru', _infoAttr, pb_base_Tru._infoAttr)
  _optionals.update(pb_base_Tru._optionals)
  _helpDict = {
    'fluide_incompressible':
          ('[fluide_incompressible] The fluid medium associated with the problem.', ''),
    'constituant':
          ('[constituant] Constituents.', ''),
    'navier_stokes_turbulent':
          ('[navier_stokes_turbulent] Navier-Stokes equations as well as the associated turbulence \n'
           'model equations.',
           ''),
    'convection_diffusion_concentration_turbulent':
          ('[convection_diffusion_concentration_turbulent] Constituent transport equations (concentration \n'
           'diffusion convection) as well as the associated turbulence model equations.',
           ''),
    'convection_diffusion_temperature_turbulent':
          ('[convection_diffusion_temperature_turbulent] Energy equation (temperature diffusion \n'
           'convection) as well as the associated turbulence model equations.',
           ''),
  }
  _helpDict = check_append_dict('pb_thermohydraulique_concentration_turbulent_Tru', _helpDict, pb_base_Tru._helpDict)
##### end pb_thermohydraulique_concentration_turbulent_Tru class

class pb_thermohydraulique_turbulent_qc_Tru(pb_base_Tru): #####
  r"""
  Resolution of turbulent thermohydraulic problem under low Mach number.
  
  Warning : Available for VDF and VEF P0/P1NC discretization only.
  """
  # From: line 1846, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_quasi_compressible', '<inherited>fluide_quasi_compressible_Tru'),   #
    ('navier_stokes_turbulent_qc', 'navier_stokes_turbulent_qc_Tru'),   #
    ('convection_diffusion_chaleur_turbulent_qc', '<inherited>convection_diffusion_chaleur_turbulent_qc_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1846]
  _infoAttr = {'convection_diffusion_chaleur_turbulent_qc': ['TRAD2_example', 1849],
   'fluide_quasi_compressible': ['TRAD2_example', 1847],
   'navier_stokes_turbulent_qc': ['TRAD2_example', 1848]}
  _attributesList = check_append_list('pb_thermohydraulique_turbulent_qc_Tru', _attributesList, pb_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_thermohydraulique_turbulent_qc_Tru', _attributesSynonyms, pb_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_thermohydraulique_turbulent_qc_Tru', _infoAttr, pb_base_Tru._infoAttr)
  _optionals.update(pb_base_Tru._optionals)
  _helpDict = {
    'fluide_quasi_compressible':
          ('[fluide_quasi_compressible] The fluid medium associated with the problem.', ''),
    'navier_stokes_turbulent_qc':
          ('[navier_stokes_turbulent_qc] Navier-Stokes equations under low Mach number as well \n'
           'as the associated turbulence model equations.',
           ''),
    'convection_diffusion_chaleur_turbulent_qc':
          ('[convection_diffusion_chaleur_turbulent_qc] Energy equation under low Mach number \n'
           'as well as the associated turbulence model equations.',
           ''),
  }
  _helpDict = check_append_dict('pb_thermohydraulique_turbulent_qc_Tru', _helpDict, pb_base_Tru._helpDict)
##### end pb_thermohydraulique_turbulent_qc_Tru class

class pb_hydraulique_melange_binaire_turbulent_qc_Tru(pb_base_Tru): #####
  r"""
  Resolution of a turbulent binary mixture problem for a quasi-compressible fluid with 
  an iso-thermal condition.
  """
  # From: line 1850, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_quasi_compressible', '<inherited>fluide_quasi_compressible_Tru'),   #
    ('navier_stokes_turbulent_qc', 'navier_stokes_turbulent_qc_Tru'),   #
    ('convection_diffusion_espece_binaire_turbulent_qc', '<inherited>convection_diffusion_espece_binaire_turbulent_qc_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1850]
  _infoAttr = {'convection_diffusion_espece_binaire_turbulent_qc': ['TRAD2_example', 1853],
   'fluide_quasi_compressible': ['TRAD2_example', 1851],
   'navier_stokes_turbulent_qc': ['TRAD2_example', 1852]}
  _attributesList = check_append_list('pb_hydraulique_melange_binaire_turbulent_qc_Tru', _attributesList, pb_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_hydraulique_melange_binaire_turbulent_qc_Tru', _attributesSynonyms, pb_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_hydraulique_melange_binaire_turbulent_qc_Tru', _infoAttr, pb_base_Tru._infoAttr)
  _optionals.update(pb_base_Tru._optionals)
  _helpDict = {
    'fluide_quasi_compressible':
          ('[fluide_quasi_compressible] The fluid medium associated with the problem.', ''),
    'navier_stokes_turbulent_qc':
          ('[navier_stokes_turbulent_qc] Navier-Stokes equation for a quasi-compressible fluid \n'
           'as well as the associated turbulence model equations.',
           ''),
    'convection_diffusion_espece_binaire_turbulent_qc':
          ('[convection_diffusion_espece_binaire_turbulent_qc] Species conservation equation for \n'
           'a quasi-compressible fluid as well as the associated turbulence model equations.',
           ''),
  }
  _helpDict = check_append_dict('pb_hydraulique_melange_binaire_turbulent_qc_Tru', _helpDict, pb_base_Tru._helpDict)
##### end pb_hydraulique_melange_binaire_turbulent_qc_Tru class

class paroi_decalee_robin_Tru(condlim_base_Tru): #####
  r"""
  This keyword is used to designate a Robin boundary condition (a.u+b.du/dn=c) associated 
  with the Pironneau methodology for the wall laws.
  The value of given by the delta option is the distance between the mesh (where symmetry 
  boundary condition is applied) and the fictious wall.
  This boundary condition needs the definition of the dedicated source terms (Source_Robin 
  or Source_Robin_Scalaire) according the equations used.
  """
  # From: line 1854, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('delta', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1854]
  _infoAttr = {'delta': ['TRAD2_example', 1855]}
  _attributesList = check_append_list('paroi_decalee_robin_Tru', _attributesList, condlim_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('paroi_decalee_robin_Tru', _attributesSynonyms, condlim_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('paroi_decalee_robin_Tru', _infoAttr, condlim_base_Tru._infoAttr)
  _optionals.update(condlim_base_Tru._optionals)
  _helpDict = {
    'delta':
          ('[floattant] not_set', ''),
  }
  _helpDict = check_append_dict('paroi_decalee_robin_Tru', _helpDict, condlim_base_Tru._helpDict)
##### end paroi_decalee_robin_Tru class

class modele_turbulence_scal_nul_Tru(modele_turbulence_scal_base_Tru): #####
  r"""
  Nul scalar turbulence model (turbulent diffusivity = 0) which can be used with a turbulent 
  problem.
  """
  # From: line 1856, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['null_Tru']
  _attributesList = []
  _suppressed = ['turbulence_paroi']
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1856]
  _infoAttr = {}
  _attributesList = check_append_list('modele_turbulence_scal_nul_Tru', _attributesList, modele_turbulence_scal_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('modele_turbulence_scal_nul_Tru', _attributesSynonyms, modele_turbulence_scal_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('modele_turbulence_scal_nul_Tru', _infoAttr, modele_turbulence_scal_base_Tru._infoAttr)
  _optionals.update(modele_turbulence_scal_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('modele_turbulence_scal_nul_Tru', _helpDict, modele_turbulence_scal_base_Tru._helpDict)
##### end modele_turbulence_scal_nul_Tru class

class modele_turbulence_hyd_nul_Tru(modele_turbulence_hyd_deriv_Tru): #####
  r"""
  Nul turbulence model (turbulent viscosity = 0) which can be used with a turbulent 
  problem.
  """
  # From: line 1858, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['null_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1858]
  _infoAttr = {}
  _attributesList = check_append_list('modele_turbulence_hyd_nul_Tru', _attributesList, modele_turbulence_hyd_deriv_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('modele_turbulence_hyd_nul_Tru', _attributesSynonyms, modele_turbulence_hyd_deriv_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('modele_turbulence_hyd_nul_Tru', _infoAttr, modele_turbulence_hyd_deriv_Tru._infoAttr)
  _optionals.update(modele_turbulence_hyd_deriv_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('modele_turbulence_hyd_nul_Tru', _helpDict, modele_turbulence_hyd_deriv_Tru._helpDict)
##### end modele_turbulence_hyd_nul_Tru class

class navier_stokes_turbulent_Tru(navier_stokes_standard_Tru): #####
  r"""
  Navier-Stokes equations as well as the associated turbulence model equations.
  """
  # From: line 1859, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('modele_turbulence', '<inherited>modele_turbulence_hyd_deriv_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['modele_turbulence'])
  _infoMain = ['TRAD2_example', 1859]
  _infoAttr = {'modele_turbulence': ['TRAD2_example', 1860]}
  _attributesList = check_append_list('navier_stokes_turbulent_Tru', _attributesList, navier_stokes_standard_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('navier_stokes_turbulent_Tru', _attributesSynonyms, navier_stokes_standard_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('navier_stokes_turbulent_Tru', _infoAttr, navier_stokes_standard_Tru._infoAttr)
  _optionals.update(navier_stokes_standard_Tru._optionals)
  _helpDict = {
    'modele_turbulence':
          ('[modele_turbulence_hyd_deriv] Turbulence model for Navier-Stokes equations.', ''),
  }
  _helpDict = check_append_dict('navier_stokes_turbulent_Tru', _helpDict, navier_stokes_standard_Tru._helpDict)
##### end navier_stokes_turbulent_Tru class

class navier_stokes_turbulent_qc_Tru(navier_stokes_turbulent_Tru): #####
  r"""
  Navier-Stokes equations under low Mach number as well as the associated turbulence 
  model equations.
  """
  # From: line 1861, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1861]
  _infoAttr = {}
  _attributesList = check_append_list('navier_stokes_turbulent_qc_Tru', _attributesList, navier_stokes_turbulent_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('navier_stokes_turbulent_qc_Tru', _attributesSynonyms, navier_stokes_turbulent_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('navier_stokes_turbulent_qc_Tru', _infoAttr, navier_stokes_turbulent_Tru._infoAttr)
  _optionals.update(navier_stokes_turbulent_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('navier_stokes_turbulent_qc_Tru', _helpDict, navier_stokes_turbulent_Tru._helpDict)
##### end navier_stokes_turbulent_qc_Tru class

class convection_diffusion_espece_multi_turbulent_qc_Tru(eqn_base_Tru): #####
  r"""
  not_set
  """
  # From: line 1862, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('modele_turbulence', '<inherited>modele_turbulence_scal_base_Tru'),   #
    ('espece', 'espece_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['modele_turbulence'])
  _infoMain = ['TRAD2_example', 1862]
  _infoAttr = {'espece': ['TRAD2_example', 1864],
   'modele_turbulence': ['TRAD2_example', 1863]}
  _attributesList = check_append_list('convection_diffusion_espece_multi_turbulent_qc_Tru', _attributesList, eqn_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_diffusion_espece_multi_turbulent_qc_Tru', _attributesSynonyms, eqn_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_diffusion_espece_multi_turbulent_qc_Tru', _infoAttr, eqn_base_Tru._infoAttr)
  _optionals.update(eqn_base_Tru._optionals)
  _helpDict = {
    'modele_turbulence':
          ('[modele_turbulence_scal_base] Turbulence model to be used.', ''),
    'espece':
          ('[espece] not_set', ''),
  }
  _helpDict = check_append_dict('convection_diffusion_espece_multi_turbulent_qc_Tru', _helpDict, eqn_base_Tru._helpDict)
##### end convection_diffusion_espece_multi_turbulent_qc_Tru class

class convection_diffusion_temperature_turbulent_Tru(eqn_base_Tru): #####
  r"""
  Energy equation (temperature diffusion convection) as well as the associated turbulence 
  model equations.
  """
  # From: line 1865, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('modele_turbulence', '<inherited>modele_turbulence_scal_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['modele_turbulence'])
  _infoMain = ['TRAD2_example', 1865]
  _infoAttr = {'modele_turbulence': ['TRAD2_example', 1866]}
  _attributesList = check_append_list('convection_diffusion_temperature_turbulent_Tru', _attributesList, eqn_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_diffusion_temperature_turbulent_Tru', _attributesSynonyms, eqn_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_diffusion_temperature_turbulent_Tru', _infoAttr, eqn_base_Tru._infoAttr)
  _optionals.update(eqn_base_Tru._optionals)
  _helpDict = {
    'modele_turbulence':
          ('[modele_turbulence_scal_base] Turbulence model for the energy equation.', ''),
  }
  _helpDict = check_append_dict('convection_diffusion_temperature_turbulent_Tru', _helpDict, eqn_base_Tru._helpDict)
##### end convection_diffusion_temperature_turbulent_Tru class

class convection_diffusion_chaleur_qc_Tru(eqn_base_Tru): #####
  r"""
  Temperature equation for a quasi-compressible fluid.
  """
  # From: line 2009, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('mode_calcul_convection', 'chaine_385ae2_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['mode_calcul_convection'])
  _infoMain = ['TRAD2_example', 2009]
  _infoAttr = {'mode_calcul_convection': ['TRAD2_example', 2010]}
  _attributesList = check_append_list('convection_diffusion_chaleur_qc_Tru', _attributesList, eqn_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_diffusion_chaleur_qc_Tru', _attributesSynonyms, eqn_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_diffusion_chaleur_qc_Tru', _infoAttr, eqn_base_Tru._infoAttr)
  _optionals.update(eqn_base_Tru._optionals)
  _helpDict = {
    'mode_calcul_convection':
          ('[chaine(into=["ancien","divut_moins_tdivu","divrhout_moins_tdivrhou"])] Option to \n'
           'set the form of the convective operatorNL2 divrhouT_moins_Tdivrhou (the default since \n'
           '1.6.8): rho.u.gradT = div(rho.u.T )- Tdiv(rho.u.1) NL2ancien: u.gradT = div(u.T) - \n'
           'T.div(u)\n'
           '\n'
           'divuT_moins_Tdivu : u.gradT = div(u.T) - Tdiv(u.1)',
           ''),
  }
  _helpDict = check_append_dict('convection_diffusion_chaleur_qc_Tru', _helpDict, eqn_base_Tru._helpDict)
##### end convection_diffusion_chaleur_qc_Tru class

class convection_diffusion_chaleur_turbulent_qc_Tru(convection_diffusion_chaleur_qc_Tru): #####
  r"""
  Temperature equation for a quasi-compressible fluid as well as the associated turbulence 
  model equations.
  """
  # From: line 1867, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('modele_turbulence', '<inherited>modele_turbulence_scal_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['modele_turbulence'])
  _infoMain = ['TRAD2_example', 1867]
  _infoAttr = {'modele_turbulence': ['TRAD2_example', 1868]}
  _attributesList = check_append_list('convection_diffusion_chaleur_turbulent_qc_Tru', _attributesList, convection_diffusion_chaleur_qc_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_diffusion_chaleur_turbulent_qc_Tru', _attributesSynonyms, convection_diffusion_chaleur_qc_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_diffusion_chaleur_turbulent_qc_Tru', _infoAttr, convection_diffusion_chaleur_qc_Tru._infoAttr)
  _optionals.update(convection_diffusion_chaleur_qc_Tru._optionals)
  _helpDict = {
    'modele_turbulence':
          ('[modele_turbulence_scal_base] Turbulence model for the temperature (energy) conservation \n'
           'equation.',
           ''),
  }
  _helpDict = check_append_dict('convection_diffusion_chaleur_turbulent_qc_Tru', _helpDict, convection_diffusion_chaleur_qc_Tru._helpDict)
##### end convection_diffusion_chaleur_turbulent_qc_Tru class

class convection_diffusion_concentration_turbulent_Tru(convection_diffusion_concentration_Tru): #####
  r"""
  Constituent transport equations (concentration diffusion convection) as well as the 
  associated turbulence model equations.
  """
  # From: line 1869, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('modele_turbulence', '<inherited>modele_turbulence_scal_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['modele_turbulence'])
  _infoMain = ['TRAD2_example', 1869]
  _infoAttr = {'modele_turbulence': ['TRAD2_example', 1870]}
  _attributesList = check_append_list('convection_diffusion_concentration_turbulent_Tru', _attributesList, convection_diffusion_concentration_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_diffusion_concentration_turbulent_Tru', _attributesSynonyms, convection_diffusion_concentration_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_diffusion_concentration_turbulent_Tru', _infoAttr, convection_diffusion_concentration_Tru._infoAttr)
  _optionals.update(convection_diffusion_concentration_Tru._optionals)
  _helpDict = {
    'modele_turbulence':
          ('[modele_turbulence_scal_base] Turbulence model to be used in the constituent transport \n'
           'equations.\n'
           'The only model currently available is Schmidt.',
           ''),
  }
  _helpDict = check_append_dict('convection_diffusion_concentration_turbulent_Tru', _helpDict, convection_diffusion_concentration_Tru._helpDict)
##### end convection_diffusion_concentration_turbulent_Tru class

class convection_diffusion_espece_binaire_qc_Tru(eqn_base_Tru): #####
  r"""
  Species conservation equation for a binary quasi-compressible fluid.
  """
  # From: line 2011, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 2011]
  _infoAttr = {}
  _attributesList = check_append_list('convection_diffusion_espece_binaire_qc_Tru', _attributesList, eqn_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_diffusion_espece_binaire_qc_Tru', _attributesSynonyms, eqn_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_diffusion_espece_binaire_qc_Tru', _infoAttr, eqn_base_Tru._infoAttr)
  _optionals.update(eqn_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('convection_diffusion_espece_binaire_qc_Tru', _helpDict, eqn_base_Tru._helpDict)
##### end convection_diffusion_espece_binaire_qc_Tru class

class convection_diffusion_espece_binaire_turbulent_qc_Tru(convection_diffusion_espece_binaire_qc_Tru): #####
  r"""
  Species conservation equation for a binary quasi-compressible fluid as well as the 
  associated turbulence model equations.
  """
  # From: line 1871, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('modele_turbulence', '<inherited>modele_turbulence_scal_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['modele_turbulence'])
  _infoMain = ['TRAD2_example', 1871]
  _infoAttr = {'modele_turbulence': ['TRAD2_example', 1872]}
  _attributesList = check_append_list('convection_diffusion_espece_binaire_turbulent_qc_Tru', _attributesList, convection_diffusion_espece_binaire_qc_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_diffusion_espece_binaire_turbulent_qc_Tru', _attributesSynonyms, convection_diffusion_espece_binaire_qc_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_diffusion_espece_binaire_turbulent_qc_Tru', _infoAttr, convection_diffusion_espece_binaire_qc_Tru._infoAttr)
  _optionals.update(convection_diffusion_espece_binaire_qc_Tru._optionals)
  _helpDict = {
    'modele_turbulence':
          ('[modele_turbulence_scal_base] Turbulence model for the species conservation equation.', ''),
  }
  _helpDict = check_append_dict('convection_diffusion_espece_binaire_turbulent_qc_Tru', _helpDict, convection_diffusion_espece_binaire_qc_Tru._helpDict)
##### end convection_diffusion_espece_binaire_turbulent_qc_Tru class

class sets_Tru(simpler_Tru): #####
  r"""
  Stability-Enhancing Two-Step solver which is useful for a multiphase problem.
  Ref : J.
  H.
  MAHAFFY, A stability-enhancing two-step method for fluid flow calculations, Journal 
  of Computational Physics, 46, 3, 329 (1982).
  """
  # From: line 1873, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('criteres_convergence', 'bloc_criteres_convergence_Tru'),   #
    ('iter_min', 'entier_3282e0_Tru'),   #
    ('seuil_convergence_implicite', 'floattant_820d8b_Tru'),   #
    ('nb_corrections_max', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['criteres_convergence',
   'iter_min',
   'seuil_convergence_implicite',
   'nb_corrections_max'])
  _infoMain = ['TRAD2_example', 1873]
  _infoAttr = {'criteres_convergence': ['TRAD2_example', 1874],
   'iter_min': ['TRAD2_example', 1875],
   'nb_corrections_max': ['TRAD2_example', 1877],
   'seuil_convergence_implicite': ['TRAD2_example', 1876]}
  _attributesList = check_append_list('sets_Tru', _attributesList, simpler_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('sets_Tru', _attributesSynonyms, simpler_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('sets_Tru', _infoAttr, simpler_Tru._infoAttr)
  _optionals.update(simpler_Tru._optionals)
  _helpDict = {
    'criteres_convergence':
          ('[bloc_criteres_convergence] Set the convergence thresholds for each unknown (i.e: \n'
           'alpha, temperature, velocity and pressure).\n'
           'The default values are respectively 0.01, 0.1, 0.01 and 100',
           ''),
    'iter_min':
          ('[entier] Number of minimum iterations', ''),
    'seuil_convergence_implicite':
          ('[floattant] Convergence criteria.', ''),
    'nb_corrections_max':
          ('[entier] Maximum number of corrections performed by the PISO algorithm to achieve \n'
           'the projection of the velocity field.\n'
           'The algorithm may perform less corrections then nb_corrections_max if the accuracy \n'
           'of the projection is sufficient.\n'
           '(By default nb_corrections_max is set to 21).',
           ''),
  }
  _helpDict = check_append_dict('sets_Tru', _helpDict, simpler_Tru._helpDict)
##### end sets_Tru class

class bloc_criteres_convergence_Tru(bloc_lecture_Tru): #####
  r"""
  Not set
  """
  # From: line 1878, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1878]
  _infoAttr = {}
  _attributesList = check_append_list('bloc_criteres_convergence_Tru', _attributesList, bloc_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('bloc_criteres_convergence_Tru', _attributesSynonyms, bloc_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('bloc_criteres_convergence_Tru', _infoAttr, bloc_lecture_Tru._infoAttr)
  _optionals.update(bloc_lecture_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('bloc_criteres_convergence_Tru', _helpDict, bloc_lecture_Tru._helpDict)
##### end bloc_criteres_convergence_Tru class

class ice_Tru(sets_Tru): #####
  r"""
  Implicit Continuous-fluid Eulerian solver which is useful for a multiphase problem.
  Robust pressure reduction resolution.
  """
  # From: line 1879, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('pression_degeneree', 'entier_3282e0_Tru'),   #
    ('reduction_pression', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'reduction_pression': ['pressure_reduction']
  }
  _optionals = set(['pression_degeneree', 'reduction_pression'])
  _infoMain = ['TRAD2_example', 1879]
  _infoAttr = {'pression_degeneree': ['TRAD2_example', 1880],
   'reduction_pression': ['TRAD2_example', 1881]}
  _attributesList = check_append_list('ice_Tru', _attributesList, sets_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('ice_Tru', _attributesSynonyms, sets_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('ice_Tru', _infoAttr, sets_Tru._infoAttr)
  _optionals.update(sets_Tru._optionals)
  _helpDict = {
    'pression_degeneree':
          ('[entier] Set to 1 if the pressure field is degenerate (ex.\n'
           ': incompressible fluid with no imposed-pressure BCs).\n'
           'Default: autodetected',
           ''),
    'reduction_pression':
          ('[entier] Set to 1 if the user wants a resolution with a pressure reduction.\n'
           'Otherwise, the flag is to be set to 0 so that the complete matrix is considered.\n'
           'The default value of this flag is 1.',
           ''),
  }
  _helpDict = check_append_dict('ice_Tru', _helpDict, sets_Tru._helpDict)
##### end ice_Tru class

class criteres_convergence_Tru(interprete_Tru): #####
  r"""
  convergence criteria
  """
  # From: line 1882, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('aco', 'chaine_e3a651_Tru'),   #
    ('inco', 'chaine_Tru'),   #
    ('val', 'floattant_820d8b_Tru'),   #
    ('acof', 'chaine_af6447_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['inco', 'val'])
  _infoMain = ['TRAD2_example', 1882]
  _infoAttr = {'aco': ['TRAD2_example', 1883],
   'acof': ['TRAD2_example', 1886],
   'inco': ['TRAD2_example', 1884],
   'val': ['TRAD2_example', 1885]}
  _attributesList = check_append_list('criteres_convergence_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('criteres_convergence_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('criteres_convergence_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'aco':
          ('[chaine(into=["{"])] Opening curly bracket.', ''),
    'inco':
          ('[chaine] Unknown (i.e: alpha, temperature, velocity and pressure)', ''),
    'val':
          ('[floattant] Convergence threshold', ''),
    'acof':
          ('[chaine(into=["}"])] Closing curly bracket.', ''),
  }
  _helpDict = check_append_dict('criteres_convergence_Tru', _helpDict, interprete_Tru._helpDict)
##### end criteres_convergence_Tru class

class frottement_interfacial_Tru(source_base_Tru): #####
  r"""
  Source term which corresponds to the phases friction at the interface
  """
  # From: line 1887, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('a_res', 'floattant_820d8b_Tru'),   #
    ('dv_min', 'floattant_820d8b_Tru'),   #
    ('exp_res', 'entier_3282e0_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['a_res', 'dv_min', 'exp_res'])
  _infoMain = ['TRAD2_example', 1887]
  _infoAttr = {'a_res': ['TRAD2_example', 1888],
   'dv_min': ['TRAD2_example', 1889],
   'exp_res': ['TRAD2_example', 1890]}
  _attributesList = check_append_list('frottement_interfacial_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('frottement_interfacial_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('frottement_interfacial_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {
    'a_res':
          ('[floattant] void fraction at which the gas velocity is forced to approach liquid velocity \n'
           '(default alpha_evanescence*100)',
           ''),
    'dv_min':
          ('[floattant] minimal relative velocity used to linearize interfacial friction at low \nvelocities',
           ''),
    'exp_res':
          ('[entier] exponent that callibrates intensity of velocity convergence (default 2)', ''),
  }
  _helpDict = check_append_dict('frottement_interfacial_Tru', _helpDict, source_base_Tru._helpDict)
##### end frottement_interfacial_Tru class

class vitesse_relative_base_Tru(source_base_Tru): #####
  r"""
  Basic class for drift-velocity source term between a liquid and a gas phase
  """
  # From: line 1891, in file 'TRAD2_example'
  _braces = 0
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1891]
  _infoAttr = {}
  _attributesList = check_append_list('vitesse_relative_base_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('vitesse_relative_base_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('vitesse_relative_base_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('vitesse_relative_base_Tru', _helpDict, source_base_Tru._helpDict)
##### end vitesse_relative_base_Tru class

class flux_interfacial_Tru(source_base_Tru): #####
  r"""
  Source term of mass transfer between phases connected by the saturation object defined 
  in saturation_xxxx
  """
  # From: line 1892, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1892]
  _infoAttr = {}
  _attributesList = check_append_list('flux_interfacial_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('flux_interfacial_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('flux_interfacial_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('flux_interfacial_Tru', _helpDict, source_base_Tru._helpDict)
##### end flux_interfacial_Tru class

class vitesse_derive_base_Tru(vitesse_relative_base_Tru): #####
  r"""
  Source term which corresponds to the drift-velocity between a liquid and a gas phase
  """
  # From: line 1893, in file 'TRAD2_example'
  _braces = 0
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1893]
  _infoAttr = {}
  _attributesList = check_append_list('vitesse_derive_base_Tru', _attributesList, vitesse_relative_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('vitesse_derive_base_Tru', _attributesSynonyms, vitesse_relative_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('vitesse_derive_base_Tru', _infoAttr, vitesse_relative_base_Tru._infoAttr)
  _optionals.update(vitesse_relative_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('vitesse_derive_base_Tru', _helpDict, vitesse_relative_base_Tru._helpDict)
##### end vitesse_derive_base_Tru class

class source_travail_pression_elem_base_Tru(source_base_Tru): #####
  r"""
  Source term which corresponds to the additional pressure work term that appears when 
  dealing with compressible multiphase fluids
  """
  # From: line 1894, in file 'TRAD2_example'
  _braces = 0
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1894]
  _infoAttr = {}
  _attributesList = check_append_list('source_travail_pression_elem_base_Tru', _attributesList, source_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('source_travail_pression_elem_base_Tru', _attributesSynonyms, source_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('source_travail_pression_elem_base_Tru', _infoAttr, source_base_Tru._infoAttr)
  _optionals.update(source_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('source_travail_pression_elem_base_Tru', _helpDict, source_base_Tru._helpDict)
##### end source_travail_pression_elem_base_Tru class

class fluide_reel_base_Tru(fluide_base_Tru): #####
  r"""
  Class for real fluids.
  """
  # From: line 1923, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1923]
  _infoAttr = {}
  _attributesList = check_append_list('fluide_reel_base_Tru', _attributesList, fluide_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('fluide_reel_base_Tru', _attributesSynonyms, fluide_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('fluide_reel_base_Tru', _infoAttr, fluide_base_Tru._infoAttr)
  _optionals.update(fluide_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('fluide_reel_base_Tru', _helpDict, fluide_base_Tru._helpDict)
##### end fluide_reel_base_Tru class

class fluide_stiffened_gas_Tru(fluide_reel_base_Tru): #####
  r"""
  Class for Stiffened Gas
  """
  # From: line 1895, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('gamma', 'floattant_820d8b_Tru'),   #
    ('pinf', 'floattant_820d8b_Tru'),   #
    ('mu', 'floattant_820d8b_Tru'),   #
    ('Lambda', 'floattant_820d8b_Tru'),   #
    ('cv', 'floattant_820d8b_Tru'),   #
    ('q', 'floattant_820d8b_Tru'),   #
    ('q_prim', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['gamma', 'pinf', 'mu', 'Lambda', 'cv', 'q', 'q_prim'])
  _infoMain = ['TRAD2_example', 1895]
  _infoAttr = {'Lambda': ['TRAD2_example', 1899],
   'cv': ['TRAD2_example', 1900],
   'gamma': ['TRAD2_example', 1896],
   'mu': ['TRAD2_example', 1898],
   'pinf': ['TRAD2_example', 1897],
   'q': ['TRAD2_example', 1901],
   'q_prim': ['TRAD2_example', 1902]}
  _attributesList = check_append_list('fluide_stiffened_gas_Tru', _attributesList, fluide_reel_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('fluide_stiffened_gas_Tru', _attributesSynonyms, fluide_reel_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('fluide_stiffened_gas_Tru', _infoAttr, fluide_reel_base_Tru._infoAttr)
  _optionals.update(fluide_reel_base_Tru._optionals)
  _helpDict = {
    'gamma':
          ('[floattant] Heat capacity ratio (Cp/Cv)', ''),
    'pinf':
          ('[floattant] Stiffened gas pressure constant (if set to zero, the state law becomes \n'
           'identical to that of perfect gases)',
           ''),
    'mu':
          ('[floattant] Dynamic viscosity', ''),
    'Lambda':
          ('[floattant] Thermal conductivity', ''),
    'cv':
          ('[floattant] Thermal capacity at constant volume', ''),
    'q':
          ('[floattant] Reference energy', ''),
    'q_prim':
          ('[floattant] Model constant', ''),
  }
  _helpDict = check_append_dict('fluide_stiffened_gas_Tru', _helpDict, fluide_reel_base_Tru._helpDict)
##### end fluide_stiffened_gas_Tru class

class saturation_base_Tru(ConstrainBase_Tru): #####
  r"""
  Basic class for a liquid-gas interface (used in pb_multiphase)
  """
  # From: line 1912, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1912]
  _infoAttr = {}
  _helpDict = {}
##### end saturation_base_Tru class

class saturation_constant_Tru(saturation_base_Tru): #####
  r"""
  Class for saturation constant
  """
  # From: line 1903, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('p_sat', 'floattant_820d8b_Tru'),   #
    ('t_sat', 'floattant_820d8b_Tru'),   #
    ('lvap', 'floattant_820d8b_Tru'),   #
    ('hlsat', 'floattant_820d8b_Tru'),   #
    ('hvsat', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['p_sat', 't_sat', 'lvap', 'hlsat', 'hvsat'])
  _infoMain = ['TRAD2_example', 1903]
  _infoAttr = {'hlsat': ['TRAD2_example', 1907],
   'hvsat': ['TRAD2_example', 1908],
   'lvap': ['TRAD2_example', 1906],
   'p_sat': ['TRAD2_example', 1904],
   't_sat': ['TRAD2_example', 1905]}
  _attributesList = check_append_list('saturation_constant_Tru', _attributesList, saturation_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('saturation_constant_Tru', _attributesSynonyms, saturation_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('saturation_constant_Tru', _infoAttr, saturation_base_Tru._infoAttr)
  _optionals.update(saturation_base_Tru._optionals)
  _helpDict = {
    'p_sat':
          ('[floattant] Define the saturation pressure value (this is a required parameter)', ''),
    't_sat':
          ('[floattant] Define the saturation temperature value (this is a required parameter)', ''),
    'lvap':
          ('[floattant] Latent heat of vaporization', ''),
    'hlsat':
          ('[floattant] Liquid saturation enthalpy', ''),
    'hvsat':
          ('[floattant] Vapor saturation enthalpy', ''),
  }
  _helpDict = check_append_dict('saturation_constant_Tru', _helpDict, saturation_base_Tru._helpDict)
##### end saturation_constant_Tru class

class fluide_sodium_gaz_Tru(fluide_reel_base_Tru): #####
  r"""
  Class for Fluide_sodium_liquide
  """
  # From: line 1909, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('p_ref', 'floattant_820d8b_Tru'),   #
    ('t_ref', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['p_ref', 't_ref'])
  _infoMain = ['TRAD2_example', 1909]
  _infoAttr = {'p_ref': ['TRAD2_example', 1910], 't_ref': ['TRAD2_example', 1911]}
  _attributesList = check_append_list('fluide_sodium_gaz_Tru', _attributesList, fluide_reel_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('fluide_sodium_gaz_Tru', _attributesSynonyms, fluide_reel_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('fluide_sodium_gaz_Tru', _infoAttr, fluide_reel_base_Tru._infoAttr)
  _optionals.update(fluide_reel_base_Tru._optionals)
  _helpDict = {
    'p_ref':
          ('[floattant] Use to set the pressure value in the closure law.\n'
           'If not specified, the value of the pressure unknown will be used',
           ''),
    't_ref':
          ('[floattant] Use to set the temperature value in the closure law.\n'
           'If not specified, the value of the temperature unknown will be used',
           ''),
  }
  _helpDict = check_append_dict('fluide_sodium_gaz_Tru', _helpDict, fluide_reel_base_Tru._helpDict)
##### end fluide_sodium_gaz_Tru class

class fluide_sodium_liquide_Tru(fluide_reel_base_Tru): #####
  r"""
  Class for Fluide_sodium_liquide
  """
  # From: line 1913, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('p_ref', 'floattant_820d8b_Tru'),   #
    ('t_ref', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['p_ref', 't_ref'])
  _infoMain = ['TRAD2_example', 1913]
  _infoAttr = {'p_ref': ['TRAD2_example', 1914], 't_ref': ['TRAD2_example', 1915]}
  _attributesList = check_append_list('fluide_sodium_liquide_Tru', _attributesList, fluide_reel_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('fluide_sodium_liquide_Tru', _attributesSynonyms, fluide_reel_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('fluide_sodium_liquide_Tru', _infoAttr, fluide_reel_base_Tru._infoAttr)
  _optionals.update(fluide_reel_base_Tru._optionals)
  _helpDict = {
    'p_ref':
          ('[floattant] Use to set the pressure value in the closure law.\n'
           'If not specified, the value of the pressure unknown will be used',
           ''),
    't_ref':
          ('[floattant] Use to set the temperature value in the closure law.\n'
           'If not specified, the value of the temperature unknown will be used',
           ''),
  }
  _helpDict = check_append_dict('fluide_sodium_liquide_Tru', _helpDict, fluide_reel_base_Tru._helpDict)
##### end fluide_sodium_liquide_Tru class

class liste_mil_Tru(ListOfBase_Tru): #####
  """
  MUSIG medium made of several sub mediums.
  """
  # From: line 1916, in file 'TRAD2_example'
  _braces = -1
  _comma = 0
  _allowedClasses = ["milieu_base_Tru"]
  _synonyms = []
  _infoMain = ['TRAD2_example', 1916]
##### end liste_mil_Tru class

class saturation_sodium_Tru(saturation_base_Tru): #####
  r"""
  Class for saturation sodium
  """
  # From: line 1920, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('p_ref', 'floattant_820d8b_Tru'),   #
    ('t_ref', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['p_ref', 't_ref'])
  _infoMain = ['TRAD2_example', 1920]
  _infoAttr = {'p_ref': ['TRAD2_example', 1921], 't_ref': ['TRAD2_example', 1922]}
  _attributesList = check_append_list('saturation_sodium_Tru', _attributesList, saturation_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('saturation_sodium_Tru', _attributesSynonyms, saturation_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('saturation_sodium_Tru', _infoAttr, saturation_base_Tru._infoAttr)
  _optionals.update(saturation_base_Tru._optionals)
  _helpDict = {
    'p_ref':
          ('[floattant] Use to fix the pressure value in the closure law.\n'
           'If not specified, the value of the pressure unknown will be used',
           ''),
    't_ref':
          ('[floattant] Use to fix the temperature value in the closure law.\n'
           'If not specified, the value of the temperature unknown will be used',
           ''),
  }
  _helpDict = check_append_dict('saturation_sodium_Tru', _helpDict, saturation_base_Tru._helpDict)
##### end saturation_sodium_Tru class

class pb_multiphase_Tru(pb_base_Tru): #####
  r"""
  A problem that allows the resolution of N-phases with 3*N equations
  """
  # From: line 1925, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('milieu_composite', 'bloc_lecture_Tru'),   #
    ('milieu_musig', 'bloc_lecture_Tru'),   #
    ('correlations', 'bloc_lecture_Tru'),   #
    ('qdm_multiphase', '<inherited>qdm_multiphase_Tru'),   #
    ('masse_multiphase', '<inherited>masse_multiphase_Tru'),   #
    ('energie_multiphase', '<inherited>energie_multiphase_Tru'),   #
    ('energie_cinetique_turbulente', '<inherited>energie_cinetique_turbulente_Tru'),   #
    ('echelle_temporelle_turbulente', '<inherited>echelle_temporelle_turbulente_Tru'),   #
    ('energie_cinetique_turbulente_wit', '<inherited>energie_cinetique_turbulente_wit_Tru'),   #
    ('taux_dissipation_turbulent', '<inherited>taux_dissipation_turbulent_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['milieu_composite',
   'milieu_musig',
   'correlations',
   'energie_cinetique_turbulente',
   'echelle_temporelle_turbulente',
   'energie_cinetique_turbulente_wit',
   'taux_dissipation_turbulent'])
  _infoMain = ['TRAD2_example', 1925]
  _infoAttr = {'correlations': ['TRAD2_example', 1928],
   'echelle_temporelle_turbulente': ['TRAD2_example', 1933],
   'energie_cinetique_turbulente': ['TRAD2_example', 1932],
   'energie_cinetique_turbulente_wit': ['TRAD2_example', 1934],
   'energie_multiphase': ['TRAD2_example', 1931],
   'masse_multiphase': ['TRAD2_example', 1930],
   'milieu_composite': ['TRAD2_example', 1926],
   'milieu_musig': ['TRAD2_example', 1927],
   'qdm_multiphase': ['TRAD2_example', 1929],
   'taux_dissipation_turbulent': ['TRAD2_example', 1935]}
  _attributesList = check_append_list('pb_multiphase_Tru', _attributesList, pb_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_multiphase_Tru', _attributesSynonyms, pb_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_multiphase_Tru', _infoAttr, pb_base_Tru._infoAttr)
  _optionals.update(pb_base_Tru._optionals)
  _helpDict = {
    'milieu_composite':
          ('[bloc_lecture] The composite medium associated with the problem.', ''),
    'milieu_musig':
          ('[bloc_lecture] The composite medium associated with the problem.', ''),
    'correlations':
          ('[bloc_lecture] List of correlations used in specific source terms (i.e.\n'
           'interfacial flux, interfacial friction, ...)',
           ''),
    'qdm_multiphase':
          ('[qdm_multiphase] Momentum conservation equation for a multi-phase problem where the \n'
           'unknown is the velocity',
           ''),
    'masse_multiphase':
          ('[masse_multiphase] Mass consevation equation for a multi-phase problem where the unknown \n'
           'is the alpha (void fraction)',
           ''),
    'energie_multiphase':
          ('[energie_multiphase] Internal energy conservation equation for a multi-phase problem \n'
           'where the unknown is the temperature',
           ''),
    'energie_cinetique_turbulente':
          ('[energie_cinetique_turbulente] Turbulent kinetic Energy conservation equation for \n'
           'a turbulent mono/multi-phase problem (available in TrioCFD)',
           ''),
    'echelle_temporelle_turbulente':
          ('[echelle_temporelle_turbulente] Turbulent Dissipation time scale equation for a turbulent \n'
           'mono/multi-phase problem (available in TrioCFD)',
           ''),
    'energie_cinetique_turbulente_wit':
          ('[energie_cinetique_turbulente_wit] Bubble Induced Turbulent kinetic Energy equation \n'
           'for a turbulent multi-phase problem (available in TrioCFD)',
           ''),
    'taux_dissipation_turbulent':
          ('[taux_dissipation_turbulent] Turbulent Dissipation frequency equation for a turbulent \n'
           'mono/multi-phase problem (available in TrioCFD)',
           ''),
  }
  _helpDict = check_append_dict('pb_multiphase_Tru', _helpDict, pb_base_Tru._helpDict)
##### end pb_multiphase_Tru class

class pb_multiphase_hem_Tru(pb_multiphase_Tru): #####
  r"""
  A problem that allows the resolution of 2-phases mechanicaly and thermally coupled 
  with 3 equations
  """
  # From: line 1924, in file 'TRAD2_example'
  _braces = -1
  _synonyms = ['pb_hem_Tru']
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1924]
  _infoAttr = {}
  _attributesList = check_append_list('pb_multiphase_hem_Tru', _attributesList, pb_multiphase_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_multiphase_hem_Tru', _attributesSynonyms, pb_multiphase_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_multiphase_hem_Tru', _infoAttr, pb_multiphase_Tru._infoAttr)
  _optionals.update(pb_multiphase_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('pb_multiphase_hem_Tru', _helpDict, pb_multiphase_Tru._helpDict)
##### end pb_multiphase_hem_Tru class

class energie_cinetique_turbulente_Tru(eqn_base_Tru): #####
  r"""
  Turbulent kinetic Energy conservation equation for a turbulent mono/multi-phase problem 
  (available in TrioCFD)
  """
  # From: line 1936, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1936]
  _infoAttr = {}
  _attributesList = check_append_list('energie_cinetique_turbulente_Tru', _attributesList, eqn_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('energie_cinetique_turbulente_Tru', _attributesSynonyms, eqn_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('energie_cinetique_turbulente_Tru', _infoAttr, eqn_base_Tru._infoAttr)
  _optionals.update(eqn_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('energie_cinetique_turbulente_Tru', _helpDict, eqn_base_Tru._helpDict)
##### end energie_cinetique_turbulente_Tru class

class echelle_temporelle_turbulente_Tru(eqn_base_Tru): #####
  r"""
  Turbulent Dissipation time scale equation for a turbulent mono/multi-phase problem 
  (available in TrioCFD)
  """
  # From: line 1937, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1937]
  _infoAttr = {}
  _attributesList = check_append_list('echelle_temporelle_turbulente_Tru', _attributesList, eqn_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('echelle_temporelle_turbulente_Tru', _attributesSynonyms, eqn_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('echelle_temporelle_turbulente_Tru', _infoAttr, eqn_base_Tru._infoAttr)
  _optionals.update(eqn_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('echelle_temporelle_turbulente_Tru', _helpDict, eqn_base_Tru._helpDict)
##### end echelle_temporelle_turbulente_Tru class

class energie_cinetique_turbulente_wit_Tru(eqn_base_Tru): #####
  r"""
  Bubble Induced Turbulent kinetic Energy equation for a turbulent multi-phase problem 
  (available in TrioCFD)
  """
  # From: line 1938, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1938]
  _infoAttr = {}
  _attributesList = check_append_list('energie_cinetique_turbulente_wit_Tru', _attributesList, eqn_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('energie_cinetique_turbulente_wit_Tru', _attributesSynonyms, eqn_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('energie_cinetique_turbulente_wit_Tru', _infoAttr, eqn_base_Tru._infoAttr)
  _optionals.update(eqn_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('energie_cinetique_turbulente_wit_Tru', _helpDict, eqn_base_Tru._helpDict)
##### end energie_cinetique_turbulente_wit_Tru class

class taux_dissipation_turbulent_Tru(eqn_base_Tru): #####
  r"""
  Turbulent Dissipation frequency equation for a turbulent mono/multi-phase problem 
  (available in TrioCFD)
  """
  # From: line 1939, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1939]
  _infoAttr = {}
  _attributesList = check_append_list('taux_dissipation_turbulent_Tru', _attributesList, eqn_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('taux_dissipation_turbulent_Tru', _attributesSynonyms, eqn_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('taux_dissipation_turbulent_Tru', _infoAttr, eqn_base_Tru._infoAttr)
  _optionals.update(eqn_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('taux_dissipation_turbulent_Tru', _helpDict, eqn_base_Tru._helpDict)
##### end taux_dissipation_turbulent_Tru class

class masse_multiphase_Tru(eqn_base_Tru): #####
  r"""
  Mass consevation equation for a multi-phase problem where the unknown is the alpha 
  (void fraction)
  """
  # From: line 1940, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1940]
  _infoAttr = {}
  _attributesList = check_append_list('masse_multiphase_Tru', _attributesList, eqn_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('masse_multiphase_Tru', _attributesSynonyms, eqn_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('masse_multiphase_Tru', _infoAttr, eqn_base_Tru._infoAttr)
  _optionals.update(eqn_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('masse_multiphase_Tru', _helpDict, eqn_base_Tru._helpDict)
##### end masse_multiphase_Tru class

class energie_multiphase_Tru(eqn_base_Tru): #####
  r"""
  Internal energy conservation equation for a multi-phase problem where the unknown 
  is the temperature
  """
  # From: line 1941, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1941]
  _infoAttr = {}
  _attributesList = check_append_list('energie_multiphase_Tru', _attributesList, eqn_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('energie_multiphase_Tru', _attributesSynonyms, eqn_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('energie_multiphase_Tru', _infoAttr, eqn_base_Tru._infoAttr)
  _optionals.update(eqn_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('energie_multiphase_Tru', _helpDict, eqn_base_Tru._helpDict)
##### end energie_multiphase_Tru class

class qdm_multiphase_Tru(eqn_base_Tru): #####
  r"""
  Momentum conservation equation for a multi-phase problem where the unknown is the 
  velocity
  """
  # From: line 1942, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('solveur_pression', '<inherited>solveur_sys_base_Tru'),   #
    ('evanescence', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['solveur_pression', 'evanescence'])
  _infoMain = ['TRAD2_example', 1942]
  _infoAttr = {'evanescence': ['TRAD2_example', 1944],
   'solveur_pression': ['TRAD2_example', 1943]}
  _attributesList = check_append_list('qdm_multiphase_Tru', _attributesList, eqn_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('qdm_multiphase_Tru', _attributesSynonyms, eqn_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('qdm_multiphase_Tru', _infoAttr, eqn_base_Tru._infoAttr)
  _optionals.update(eqn_base_Tru._optionals)
  _helpDict = {
    'solveur_pression':
          ('[solveur_sys_base] Linear pressure system resolution method.', ''),
    'evanescence':
          ('[bloc_lecture] Management of the vanishing phase (when alpha tends to 0 or 1)', ''),
  }
  _helpDict = check_append_dict('qdm_multiphase_Tru', _helpDict, eqn_base_Tru._helpDict)
##### end qdm_multiphase_Tru class

class fluide_dilatable_base_Tru(fluide_base_Tru): #####
  r"""
  Basic class for dilatable fluids.
  """
  # From: line 2014, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 2014]
  _infoAttr = {}
  _attributesList = check_append_list('fluide_dilatable_base_Tru', _attributesList, fluide_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('fluide_dilatable_base_Tru', _attributesSynonyms, fluide_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('fluide_dilatable_base_Tru', _infoAttr, fluide_base_Tru._infoAttr)
  _optionals.update(fluide_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('fluide_dilatable_base_Tru', _helpDict, fluide_base_Tru._helpDict)
##### end fluide_dilatable_base_Tru class

class fluide_quasi_compressible_Tru(fluide_dilatable_base_Tru): #####
  r"""
  Quasi-compressible flow with a low mach number assumption; this means that the thermo-dynamic 
  pressure (used in state law) is uniform in space.
  """
  # From: line 1945, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('sutherland', 'bloc_sutherland_Tru'),   #
    ('pression', 'floattant_820d8b_Tru'),   #
    ('loi_etat', '<inherited>loi_etat_base_Tru'),   #
    ('traitement_pth', 'chaine_6c41af_Tru'),   #
    ('traitement_rho_gravite', 'chaine_7e6e04_Tru'),   #
    ('temps_debut_prise_en_compte_drho_dt', 'floattant_820d8b_Tru'),   #
    ('omega_relaxation_drho_dt', 'floattant_820d8b_Tru'),   #
    ('Lambda', '<inherited>field_base_Tru'),   #
    ('mu', '<inherited>field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'Lambda': ['lambda_u']
  }
  _optionals = set(['sutherland',
   'pression',
   'loi_etat',
   'traitement_pth',
   'traitement_rho_gravite',
   'temps_debut_prise_en_compte_drho_dt',
   'omega_relaxation_drho_dt',
   'Lambda',
   'mu'])
  _infoMain = ['TRAD2_example', 1945]
  _infoAttr = {'Lambda': ['TRAD2_example', 1953],
   'loi_etat': ['TRAD2_example', 1948],
   'mu': ['TRAD2_example', 1954],
   'omega_relaxation_drho_dt': ['TRAD2_example', 1952],
   'pression': ['TRAD2_example', 1947],
   'sutherland': ['TRAD2_example', 1946],
   'temps_debut_prise_en_compte_drho_dt': ['TRAD2_example', 1951],
   'traitement_pth': ['TRAD2_example', 1949],
   'traitement_rho_gravite': ['TRAD2_example', 1950]}
  _attributesList = check_append_list('fluide_quasi_compressible_Tru', _attributesList, fluide_dilatable_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('fluide_quasi_compressible_Tru', _attributesSynonyms, fluide_dilatable_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('fluide_quasi_compressible_Tru', _infoAttr, fluide_dilatable_base_Tru._infoAttr)
  _optionals.update(fluide_dilatable_base_Tru._optionals)
  _helpDict = {
    'sutherland':
          ('[bloc_sutherland] Sutherland law for viscosity and for conductivity.', ''),
    'pression':
          ('[floattant] Initial thermo-dynamic pressure used in the assosciated state law.', ''),
    'loi_etat':
          ('[loi_etat_base] The state law that will be associated to the Quasi-compressible fluid.', ''),
    'traitement_pth':
          ('[chaine(into=["edo","constant","conservation_masse"])] Particular treatment for the \n'
           'thermodynamic pressure Pth ; there are three possibilities:\n'
           '\n'
           "1) with the keyword 'edo' the code computes Pth solving an O.D.E.\n"
           '; in this case, the mass is not strictly conserved (it is the default case for quasi \n'
           'compressible computation):\n'
           '\n'
           "2) the keyword 'conservation_masse' forces the conservation of the mass (closed geometry \n"
           'or with periodic boundaries condition)\n'
           '\n'
           "3) the keyword 'constant' makes it possible to have a constant Pth ; it's the good \n"
           'choice when the flow is open (e.g.\n'
           'with pressure boundary conditions).\n'
           '\n'
           'It is possible to monitor the volume averaged value for temperature and density, \n'
           'plus Pth evolution in the .evol_glob file.',
           ''),
    'traitement_rho_gravite':
          ('[chaine(into=["standard","moins_rho_moyen"])] It may be :1) \\`standard\\` : the gravity \n'
           'term is evaluted with rho*g (It is the default).\n'
           '2) \\`moins_rho_moyen\\` : the gravity term is evaluated with (rho-rhomoy) *g.\n'
           'Unknown pressure is then P*=P+rhomoy*g*z.\n'
           'It is useful when you apply uniforme pressure boundary condition like P*=0.',
           ''),
    'temps_debut_prise_en_compte_drho_dt':
          ('[floattant] While time<value, dRho/dt is set to zero (Rho, volumic mass).\n'
           'Useful for some calculation during the first time steps with big variation of temperature \n'
           'and volumic mass.',
           ''),
    'omega_relaxation_drho_dt':
          ('[floattant] Optional option to have a relaxed algorithm to solve the mass equation.\n'
           'value is used (1 per default) to specify omega.',
           ''),
    'Lambda':
          ('[field_base] Conductivity (W.m-1.K-1).', ''),
    'mu':
          ('[field_base] Dynamic viscosity (kg.m-1.s-1).', ''),
  }
  _helpDict = check_append_dict('fluide_quasi_compressible_Tru', _helpDict, fluide_dilatable_base_Tru._helpDict)
##### end fluide_quasi_compressible_Tru class

class bloc_sutherland_Tru(objet_lecture_Tru): #####
  r"""
  Sutherland law for viscosity mu(T)=mu0*((T0+C)/(T+C))*(T/T0)**1.5 and (optional) for 
  conductivity lambda(T)=mu0*Cp/Prandtl*((T0+Slambda)/(T+Slambda))*(T/T0)**1.5
  """
  # From: line 1955, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('problem_name', 'ref_pb_base_Tru'),   #
    ('mu0', 'chaine_23652b_Tru'),   #
    ('mu0_val', 'floattant_820d8b_Tru'),   #
    ('t0', 'chaine_86c49e_Tru'),   #
    ('t0_val', 'floattant_820d8b_Tru'),   #
    ('slambda', 'chaine_269f2a_Tru'),   #
    ('s', 'floattant_820d8b_Tru'),   #
    ('c', 'chaine_96adb9_Tru'),   #
    ('c_val', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['slambda', 's'])
  _infoMain = ['TRAD2_example', 1955]
  _infoAttr = {'c': ['TRAD2_example', 1963],
   'c_val': ['TRAD2_example', 1964],
   'mu0': ['TRAD2_example', 1957],
   'mu0_val': ['TRAD2_example', 1958],
   'problem_name': ['TRAD2_example', 1956],
   's': ['TRAD2_example', 1962],
   'slambda': ['TRAD2_example', 1961],
   't0': ['TRAD2_example', 1959],
   't0_val': ['TRAD2_example', 1960]}
  _attributesList = check_append_list('bloc_sutherland_Tru', _attributesList, objet_lecture_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('bloc_sutherland_Tru', _attributesSynonyms, objet_lecture_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('bloc_sutherland_Tru', _infoAttr, objet_lecture_Tru._infoAttr)
  _optionals.update(objet_lecture_Tru._optionals)
  _helpDict = {
    'problem_name':
          ('[ref_pb_base] Name of problem.', ''),
    'mu0':
          ('[chaine(into=["mu0"])] not_set', ''),
    'mu0_val':
          ('[floattant] not_set', ''),
    't0':
          ('[chaine(into=["t0"])] not_set', ''),
    't0_val':
          ('[floattant] not_set', ''),
    'slambda':
          ('[chaine(into=["slambda"])] not_set', ''),
    's':
          ('[floattant] not_set', ''),
    'c':
          ('[chaine(into=["c"])] not_set', ''),
    'c_val':
          ('[floattant] not_set', ''),
  }
  _helpDict = check_append_dict('bloc_sutherland_Tru', _helpDict, objet_lecture_Tru._helpDict)
##### end bloc_sutherland_Tru class

class loi_etat_base_Tru(ConstrainBase_Tru): #####
  r"""
  Basic class for state laws used with a dilatable fluid.
  """
  # From: line 2015, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 2015]
  _infoAttr = {}
  _helpDict = {}
##### end loi_etat_base_Tru class

class loi_etat_gaz_parfait_base_Tru(loi_etat_base_Tru): #####
  r"""
  Basic class for perfect gases state laws used with a dilatable fluid.
  """
  # From: line 2012, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 2012]
  _infoAttr = {}
  _attributesList = check_append_list('loi_etat_gaz_parfait_base_Tru', _attributesList, loi_etat_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('loi_etat_gaz_parfait_base_Tru', _attributesSynonyms, loi_etat_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('loi_etat_gaz_parfait_base_Tru', _infoAttr, loi_etat_base_Tru._infoAttr)
  _optionals.update(loi_etat_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('loi_etat_gaz_parfait_base_Tru', _helpDict, loi_etat_base_Tru._helpDict)
##### end loi_etat_gaz_parfait_base_Tru class

class perfect_gaz_qc_Tru(loi_etat_gaz_parfait_base_Tru): #####
  r"""
  Class for perfect gas state law used with a quasi-compressible fluid.
  """
  # From: line 1965, in file 'TRAD2_example'
  _braces = 1
  _synonyms = ['gaz_parfait_qc_Tru']
  _attributesList = [
    ('cp', 'floattant_820d8b_Tru'),   #
    ('cv', 'floattant_820d8b_Tru'),   #
    ('gamma', 'floattant_820d8b_Tru'),   #
    ('prandtl', 'floattant_820d8b_Tru'),   #
    ('rho_constant_pour_debug', '<inherited>field_base_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['cv', 'gamma', 'rho_constant_pour_debug'])
  _infoMain = ['TRAD2_example', 1965]
  _infoAttr = {'cp': ['TRAD2_example', 1966],
   'cv': ['TRAD2_example', 1967],
   'gamma': ['TRAD2_example', 1968],
   'prandtl': ['TRAD2_example', 1969],
   'rho_constant_pour_debug': ['TRAD2_example', 1970]}
  _attributesList = check_append_list('perfect_gaz_qc_Tru', _attributesList, loi_etat_gaz_parfait_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('perfect_gaz_qc_Tru', _attributesSynonyms, loi_etat_gaz_parfait_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('perfect_gaz_qc_Tru', _infoAttr, loi_etat_gaz_parfait_base_Tru._infoAttr)
  _optionals.update(loi_etat_gaz_parfait_base_Tru._optionals)
  _helpDict = {
    'cp':
          ('[floattant] Specific heat at constant pressure (J/kg/K).', ''),
    'cv':
          ('[floattant] Specific heat at constant volume (J/kg/K).', ''),
    'gamma':
          ('[floattant] Cp/Cv', ''),
    'prandtl':
          ('[floattant] Prandtl number of the gas Pr=mu*Cp/lambda', ''),
    'rho_constant_pour_debug':
          ('[field_base] For developers to debug the code with a constant rho.', ''),
  }
  _helpDict = check_append_dict('perfect_gaz_qc_Tru', _helpDict, loi_etat_gaz_parfait_base_Tru._helpDict)
##### end perfect_gaz_qc_Tru class

class binaire_gaz_parfait_qc_Tru(loi_etat_gaz_parfait_base_Tru): #####
  r"""
  Class for perfect gas binary mixtures state law used with a quasi-compressible fluid 
  under the iso-thermal and iso-bar assumptions.
  """
  # From: line 1971, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('molar_mass1', 'floattant_820d8b_Tru'),   #
    ('molar_mass2', 'floattant_820d8b_Tru'),   #
    ('mu1', 'floattant_820d8b_Tru'),   #
    ('mu2', 'floattant_820d8b_Tru'),   #
    ('temperature', 'floattant_820d8b_Tru'),   #
    ('diffusion_coeff', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1971]
  _infoAttr = {'diffusion_coeff': ['TRAD2_example', 1977],
   'molar_mass1': ['TRAD2_example', 1972],
   'molar_mass2': ['TRAD2_example', 1973],
   'mu1': ['TRAD2_example', 1974],
   'mu2': ['TRAD2_example', 1975],
   'temperature': ['TRAD2_example', 1976]}
  _attributesList = check_append_list('binaire_gaz_parfait_qc_Tru', _attributesList, loi_etat_gaz_parfait_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('binaire_gaz_parfait_qc_Tru', _attributesSynonyms, loi_etat_gaz_parfait_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('binaire_gaz_parfait_qc_Tru', _infoAttr, loi_etat_gaz_parfait_base_Tru._infoAttr)
  _optionals.update(loi_etat_gaz_parfait_base_Tru._optionals)
  _helpDict = {
    'molar_mass1':
          ('[floattant] Molar mass of species 1 (in kg/mol).', ''),
    'molar_mass2':
          ('[floattant] Molar mass of species 2 (in kg/mol).', ''),
    'mu1':
          ('[floattant] Dynamic viscosity of species 1 (in kg/m.s).', ''),
    'mu2':
          ('[floattant] Dynamic viscosity of species 2 (in kg/m.s).', ''),
    'temperature':
          ('[floattant] Temperature (in Kelvin) which will be constant during the simulation since \n'
           'this state law only works for iso-thermal conditions.',
           ''),
    'diffusion_coeff':
          ('[floattant] Diffusion coefficient assumed the same for both species (in m2/s).', ''),
  }
  _helpDict = check_append_dict('binaire_gaz_parfait_qc_Tru', _helpDict, loi_etat_gaz_parfait_base_Tru._helpDict)
##### end binaire_gaz_parfait_qc_Tru class

class loi_etat_gaz_reel_base_Tru(loi_etat_base_Tru): #####
  r"""
  Basic class for real gases state laws used with a dilatable fluid.
  """
  # From: line 2013, in file 'TRAD2_example'
  _braces = -1
  _read_type = True  # This class and its children needs to read their type when parsed in the dataset!
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 2013]
  _infoAttr = {}
  _attributesList = check_append_list('loi_etat_gaz_reel_base_Tru', _attributesList, loi_etat_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('loi_etat_gaz_reel_base_Tru', _attributesSynonyms, loi_etat_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('loi_etat_gaz_reel_base_Tru', _infoAttr, loi_etat_base_Tru._infoAttr)
  _optionals.update(loi_etat_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('loi_etat_gaz_reel_base_Tru', _helpDict, loi_etat_base_Tru._helpDict)
##### end loi_etat_gaz_reel_base_Tru class

class rhot_gaz_reel_qc_Tru(loi_etat_gaz_reel_base_Tru): #####
  r"""
  Class for real gas state law used with a quasi-compressible fluid.
  """
  # From: line 1978, in file 'TRAD2_example'
  _braces = 0
  _synonyms = []
  _attributesList = [
    ('bloc', 'bloc_lecture_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1978]
  _infoAttr = {'bloc': ['TRAD2_example', 1979]}
  _attributesList = check_append_list('rhot_gaz_reel_qc_Tru', _attributesList, loi_etat_gaz_reel_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('rhot_gaz_reel_qc_Tru', _attributesSynonyms, loi_etat_gaz_reel_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('rhot_gaz_reel_qc_Tru', _infoAttr, loi_etat_gaz_reel_base_Tru._infoAttr)
  _optionals.update(loi_etat_gaz_reel_base_Tru._optionals)
  _helpDict = {
    'bloc':
          ('[bloc_lecture] Description.', ''),
  }
  _helpDict = check_append_dict('rhot_gaz_reel_qc_Tru', _helpDict, loi_etat_gaz_reel_base_Tru._helpDict)
##### end rhot_gaz_reel_qc_Tru class

class espece_Tru(interprete_Tru): #####
  r"""
  not_set
  """
  # From: line 1980, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('mu', '<inherited>field_base_Tru'),   #
    ('cp', '<inherited>field_base_Tru'),   #
    ('masse_molaire', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 1980]
  _infoAttr = {'cp': ['TRAD2_example', 1982],
   'masse_molaire': ['TRAD2_example', 1983],
   'mu': ['TRAD2_example', 1981]}
  _attributesList = check_append_list('espece_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('espece_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('espece_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'mu':
          ('[field_base] Species dynamic viscosity value (kg.m-1.s-1).', ''),
    'cp':
          ('[field_base] Species specific heat value (J.kg-1.K-1).', ''),
    'masse_molaire':
          ('[floattant] Species molar mass.', ''),
  }
  _helpDict = check_append_dict('espece_Tru', _helpDict, interprete_Tru._helpDict)
##### end espece_Tru class

class multi_gaz_parfait_qc_Tru(loi_etat_gaz_parfait_base_Tru): #####
  r"""
  Class for perfect gas multi-species mixtures state law used with a quasi-compressible 
  fluid.
  """
  # From: line 1984, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('sc', 'floattant_820d8b_Tru'),   #
    ('prandtl', 'floattant_820d8b_Tru'),   #
    ('cp', 'floattant_820d8b_Tru'),   #
    ('dtol_fraction', 'floattant_820d8b_Tru'),   #
    ('correction_fraction', 'rien_Tru'),   #
    ('ignore_check_fraction', 'rien_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['cp', 'dtol_fraction', 'correction_fraction', 'ignore_check_fraction'])
  _infoMain = ['TRAD2_example', 1984]
  _infoAttr = {'correction_fraction': ['TRAD2_example', 1989],
   'cp': ['TRAD2_example', 1987],
   'dtol_fraction': ['TRAD2_example', 1988],
   'ignore_check_fraction': ['TRAD2_example', 1990],
   'prandtl': ['TRAD2_example', 1986],
   'sc': ['TRAD2_example', 1985]}
  _attributesList = check_append_list('multi_gaz_parfait_qc_Tru', _attributesList, loi_etat_gaz_parfait_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('multi_gaz_parfait_qc_Tru', _attributesSynonyms, loi_etat_gaz_parfait_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('multi_gaz_parfait_qc_Tru', _infoAttr, loi_etat_gaz_parfait_base_Tru._infoAttr)
  _optionals.update(loi_etat_gaz_parfait_base_Tru._optionals)
  _helpDict = {
    'sc':
          ('[floattant] Schmidt number of the gas Sc=nu/D (D: diffusion coefficient of the mixing).', ''),
    'prandtl':
          ('[floattant] Prandtl number of the gas Pr=mu*Cp/lambda', ''),
    'cp':
          ('[floattant] Specific heat at constant pressure of the gas Cp.', ''),
    'dtol_fraction':
          ('[floattant] Delta tolerance on mass fractions for check testing (default value 1.e-6).', ''),
    'correction_fraction':
          ('[rien] To force mass fractions between 0.\nand 1.', ''),
    'ignore_check_fraction':
          ('[rien] Not to check if mass fractions between 0.\nand 1.', ''),
  }
  _helpDict = check_append_dict('multi_gaz_parfait_qc_Tru', _helpDict, loi_etat_gaz_parfait_base_Tru._helpDict)
##### end multi_gaz_parfait_qc_Tru class

class rhot_gaz_parfait_qc_Tru(loi_etat_gaz_parfait_base_Tru): #####
  r"""
  Class for perfect gas used with a quasi-compressible fluid where the state equation 
  is defined as rho = f(T).
  """
  # From: line 1991, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('cp', 'floattant_820d8b_Tru'),   #
    ('prandtl', 'floattant_820d8b_Tru'),   #
    ('rho_xyz', '<inherited>field_base_Tru'),   #
    ('rho_t', 'chaine_Tru'),   #
    ('t_min', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['prandtl', 'rho_xyz', 'rho_t', 't_min'])
  _infoMain = ['TRAD2_example', 1991]
  _infoAttr = {'cp': ['TRAD2_example', 1992],
   'prandtl': ['TRAD2_example', 1993],
   'rho_t': ['TRAD2_example', 1995],
   'rho_xyz': ['TRAD2_example', 1994],
   't_min': ['TRAD2_example', 1996]}
  _attributesList = check_append_list('rhot_gaz_parfait_qc_Tru', _attributesList, loi_etat_gaz_parfait_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('rhot_gaz_parfait_qc_Tru', _attributesSynonyms, loi_etat_gaz_parfait_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('rhot_gaz_parfait_qc_Tru', _infoAttr, loi_etat_gaz_parfait_base_Tru._infoAttr)
  _optionals.update(loi_etat_gaz_parfait_base_Tru._optionals)
  _helpDict = {
    'cp':
          ('[floattant] Specific heat at constant pressure of the gas Cp.', ''),
    'prandtl':
          ('[floattant] Prandtl number of the gas Pr=mu*Cp/lambda', ''),
    'rho_xyz':
          ('[field_base] Defined with a Champ_Fonc_xyz to define a constant rho with time (space \n'
           'dependent)',
           ''),
    'rho_t':
          ('[chaine] Expression of T used to calculate rho.\n'
           'This can lead to a variable rho, both in space and in time.',
           ''),
    't_min':
          ('[floattant] Temperature may, in some cases, locally and temporarily be very small \n'
           '(and negative) even though computation converges.\n'
           'T_min keyword allows to set a lower limit of temperature (in Kelvin, -1000 by default).\n'
           'WARNING: DO NOT USE THIS KEYWORD WITHOUT CHECKING CAREFULY YOUR RESULTS!',
           ''),
  }
  _helpDict = check_append_dict('rhot_gaz_parfait_qc_Tru', _helpDict, loi_etat_gaz_parfait_base_Tru._helpDict)
##### end rhot_gaz_parfait_qc_Tru class

class pb_hydraulique_melange_binaire_qc_Tru(pb_base_Tru): #####
  r"""
  Resolution of a binary mixture problem for a quasi-compressible fluid with an iso-thermal 
  condition.
  
  Keywords for the unknowns other than pressure, velocity, fraction_massique are :
  
  masse_volumique : density
  
  pression : reduced pressure
  
  pression_tot : total pressure.
  """
  # From: line 1997, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_quasi_compressible', '<inherited>fluide_quasi_compressible_Tru'),   #
    ('constituant', 'constituant_Tru'),   #
    ('navier_stokes_qc', 'navier_stokes_qc_Tru'),   #
    ('convection_diffusion_espece_binaire_qc', '<inherited>convection_diffusion_espece_binaire_qc_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['constituant'])
  _infoMain = ['TRAD2_example', 1997]
  _infoAttr = {'constituant': ['TRAD2_example', 1999],
   'convection_diffusion_espece_binaire_qc': ['TRAD2_example', 2001],
   'fluide_quasi_compressible': ['TRAD2_example', 1998],
   'navier_stokes_qc': ['TRAD2_example', 2000]}
  _attributesList = check_append_list('pb_hydraulique_melange_binaire_qc_Tru', _attributesList, pb_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_hydraulique_melange_binaire_qc_Tru', _attributesSynonyms, pb_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_hydraulique_melange_binaire_qc_Tru', _infoAttr, pb_base_Tru._infoAttr)
  _optionals.update(pb_base_Tru._optionals)
  _helpDict = {
    'fluide_quasi_compressible':
          ('[fluide_quasi_compressible] The fluid medium associated with the problem.', ''),
    'constituant':
          ('[constituant] The various constituants associated to the problem.', ''),
    'navier_stokes_qc':
          ('[navier_stokes_qc] Navier-Stokes equation for a quasi-compressible fluid.', ''),
    'convection_diffusion_espece_binaire_qc':
          ('[convection_diffusion_espece_binaire_qc] Species conservation equation for a binary \n'
           'quasi-compressible fluid.',
           ''),
  }
  _helpDict = check_append_dict('pb_hydraulique_melange_binaire_qc_Tru', _helpDict, pb_base_Tru._helpDict)
##### end pb_hydraulique_melange_binaire_qc_Tru class

class pb_thermohydraulique_qc_Tru(pb_base_Tru): #####
  r"""
  Resolution of thermo-hydraulic problem for a quasi-compressible fluid.
  
  Keywords for the unknowns other than pressure, velocity, temperature are :
  
  masse_volumique : density
  
  enthalpie : enthalpy
  
  pression : reduced pressure
  
  pression_tot : total pressure.
  """
  # From: line 2002, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_quasi_compressible', '<inherited>fluide_quasi_compressible_Tru'),   #
    ('navier_stokes_qc', 'navier_stokes_qc_Tru'),   #
    ('convection_diffusion_chaleur_qc', '<inherited>convection_diffusion_chaleur_qc_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 2002]
  _infoAttr = {'convection_diffusion_chaleur_qc': ['TRAD2_example', 2005],
   'fluide_quasi_compressible': ['TRAD2_example', 2003],
   'navier_stokes_qc': ['TRAD2_example', 2004]}
  _attributesList = check_append_list('pb_thermohydraulique_qc_Tru', _attributesList, pb_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_thermohydraulique_qc_Tru', _attributesSynonyms, pb_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_thermohydraulique_qc_Tru', _infoAttr, pb_base_Tru._infoAttr)
  _optionals.update(pb_base_Tru._optionals)
  _helpDict = {
    'fluide_quasi_compressible':
          ('[fluide_quasi_compressible] The fluid medium associated with the problem.', ''),
    'navier_stokes_qc':
          ('[navier_stokes_qc] Navier-Stokes equation for a quasi-compressible fluid.', ''),
    'convection_diffusion_chaleur_qc':
          ('[convection_diffusion_chaleur_qc] Temperature equation for a quasi-compressible fluid.', ''),
  }
  _helpDict = check_append_dict('pb_thermohydraulique_qc_Tru', _helpDict, pb_base_Tru._helpDict)
##### end pb_thermohydraulique_qc_Tru class

class navier_stokes_qc_Tru(navier_stokes_standard_Tru): #####
  r"""
  Navier-Stokes equation for a quasi-compressible fluid.
  """
  # From: line 2006, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 2006]
  _infoAttr = {}
  _attributesList = check_append_list('navier_stokes_qc_Tru', _attributesList, navier_stokes_standard_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('navier_stokes_qc_Tru', _attributesSynonyms, navier_stokes_standard_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('navier_stokes_qc_Tru', _infoAttr, navier_stokes_standard_Tru._infoAttr)
  _optionals.update(navier_stokes_standard_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('navier_stokes_qc_Tru', _helpDict, navier_stokes_standard_Tru._helpDict)
##### end navier_stokes_qc_Tru class

class convection_diffusion_espece_multi_qc_Tru(eqn_base_Tru): #####
  r"""
  Species conservation equation for a multi-species quasi-compressible fluid.
  """
  # From: line 2007, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('espece', 'espece_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['espece'])
  _infoMain = ['TRAD2_example', 2007]
  _infoAttr = {'espece': ['TRAD2_example', 2008]}
  _attributesList = check_append_list('convection_diffusion_espece_multi_qc_Tru', _attributesList, eqn_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_diffusion_espece_multi_qc_Tru', _attributesSynonyms, eqn_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_diffusion_espece_multi_qc_Tru', _infoAttr, eqn_base_Tru._infoAttr)
  _optionals.update(eqn_base_Tru._optionals)
  _helpDict = {
    'espece':
          ('[espece] Assosciate a species (with its properties) to the equation', ''),
  }
  _helpDict = check_append_dict('convection_diffusion_espece_multi_qc_Tru', _helpDict, eqn_base_Tru._helpDict)
##### end convection_diffusion_espece_multi_qc_Tru class

class pb_thermohydraulique_especes_qc_Tru(pb_avec_passif_Tru): #####
  r"""
  Resolution of thermo-hydraulic problem for a multi-species quasi-compressible fluid.
  """
  # From: line 2016, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_quasi_compressible', '<inherited>fluide_quasi_compressible_Tru'),   #
    ('navier_stokes_qc', 'navier_stokes_qc_Tru'),   #
    ('convection_diffusion_chaleur_qc', '<inherited>convection_diffusion_chaleur_qc_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 2016]
  _infoAttr = {'convection_diffusion_chaleur_qc': ['TRAD2_example', 2019],
   'fluide_quasi_compressible': ['TRAD2_example', 2017],
   'navier_stokes_qc': ['TRAD2_example', 2018]}
  _attributesList = check_append_list('pb_thermohydraulique_especes_qc_Tru', _attributesList, pb_avec_passif_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_thermohydraulique_especes_qc_Tru', _attributesSynonyms, pb_avec_passif_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_thermohydraulique_especes_qc_Tru', _infoAttr, pb_avec_passif_Tru._infoAttr)
  _optionals.update(pb_avec_passif_Tru._optionals)
  _helpDict = {
    'fluide_quasi_compressible':
          ('[fluide_quasi_compressible] The fluid medium associated with the problem.', ''),
    'navier_stokes_qc':
          ('[navier_stokes_qc] Navier-Stokes equation for a quasi-compressible fluid.', ''),
    'convection_diffusion_chaleur_qc':
          ('[convection_diffusion_chaleur_qc] Temperature equation for a quasi-compressible fluid.', ''),
  }
  _helpDict = check_append_dict('pb_thermohydraulique_especes_qc_Tru', _helpDict, pb_avec_passif_Tru._helpDict)
##### end pb_thermohydraulique_especes_qc_Tru class

class pb_thermohydraulique_especes_wc_Tru(pb_avec_passif_Tru): #####
  r"""
  Resolution of thermo-hydraulic problem for a multi-species weakly-compressible fluid.
  """
  # From: line 2020, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_weakly_compressible', '<inherited>fluide_weakly_compressible_Tru'),   #
    ('navier_stokes_wc', 'navier_stokes_wc_Tru'),   #
    ('convection_diffusion_chaleur_wc', '<inherited>convection_diffusion_chaleur_wc_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 2020]
  _infoAttr = {'convection_diffusion_chaleur_wc': ['TRAD2_example', 2023],
   'fluide_weakly_compressible': ['TRAD2_example', 2021],
   'navier_stokes_wc': ['TRAD2_example', 2022]}
  _attributesList = check_append_list('pb_thermohydraulique_especes_wc_Tru', _attributesList, pb_avec_passif_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_thermohydraulique_especes_wc_Tru', _attributesSynonyms, pb_avec_passif_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_thermohydraulique_especes_wc_Tru', _infoAttr, pb_avec_passif_Tru._infoAttr)
  _optionals.update(pb_avec_passif_Tru._optionals)
  _helpDict = {
    'fluide_weakly_compressible':
          ('[fluide_weakly_compressible] The fluid medium associated with the problem.', ''),
    'navier_stokes_wc':
          ('[navier_stokes_wc] Navier-Stokes equation for a weakly-compressible fluid.', ''),
    'convection_diffusion_chaleur_wc':
          ('[convection_diffusion_chaleur_wc] Temperature equation for a weakly-compressible fluid.', ''),
  }
  _helpDict = check_append_dict('pb_thermohydraulique_especes_wc_Tru', _helpDict, pb_avec_passif_Tru._helpDict)
##### end pb_thermohydraulique_especes_wc_Tru class

class fluide_weakly_compressible_Tru(fluide_dilatable_base_Tru): #####
  r"""
  Weakly-compressible flow with a low mach number assumption; this means that the thermo-dynamic 
  pressure (used in state law) can vary in space.
  """
  # From: line 2024, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('loi_etat', '<inherited>loi_etat_base_Tru'),   #
    ('sutherland', 'bloc_sutherland_Tru'),   #
    ('traitement_pth', 'chaine_c53c45_Tru'),   #
    ('Lambda', '<inherited>field_base_Tru'),   #
    ('mu', '<inherited>field_base_Tru'),   #
    ('pression_thermo', 'floattant_820d8b_Tru'),   #
    ('pression_xyz', '<inherited>field_base_Tru'),   #
    ('use_total_pressure', 'entier_3282e0_Tru'),   #
    ('use_hydrostatic_pressure', 'entier_3282e0_Tru'),   #
    ('use_grad_pression_eos', 'entier_3282e0_Tru'),   #
    ('time_activate_ptot', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {
    'Lambda': ['lambda_u']
  }
  _optionals = set(['loi_etat',
   'sutherland',
   'traitement_pth',
   'Lambda',
   'mu',
   'pression_thermo',
   'pression_xyz',
   'use_total_pressure',
   'use_hydrostatic_pressure',
   'use_grad_pression_eos',
   'time_activate_ptot'])
  _infoMain = ['TRAD2_example', 2024]
  _infoAttr = {'Lambda': ['TRAD2_example', 2028],
   'loi_etat': ['TRAD2_example', 2025],
   'mu': ['TRAD2_example', 2029],
   'pression_thermo': ['TRAD2_example', 2030],
   'pression_xyz': ['TRAD2_example', 2031],
   'sutherland': ['TRAD2_example', 2026],
   'time_activate_ptot': ['TRAD2_example', 2035],
   'traitement_pth': ['TRAD2_example', 2027],
   'use_grad_pression_eos': ['TRAD2_example', 2034],
   'use_hydrostatic_pressure': ['TRAD2_example', 2033],
   'use_total_pressure': ['TRAD2_example', 2032]}
  _attributesList = check_append_list('fluide_weakly_compressible_Tru', _attributesList, fluide_dilatable_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('fluide_weakly_compressible_Tru', _attributesSynonyms, fluide_dilatable_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('fluide_weakly_compressible_Tru', _infoAttr, fluide_dilatable_base_Tru._infoAttr)
  _optionals.update(fluide_dilatable_base_Tru._optionals)
  _helpDict = {
    'loi_etat':
          ('[loi_etat_base] The state law that will be associated to the Weakly-compressible fluid.', ''),
    'sutherland':
          ('[bloc_sutherland] Sutherland law for viscosity and for conductivity.', ''),
    'traitement_pth':
          ('[chaine(into=["constant"])] Particular treatment for the thermodynamic pressure Pth \n'
           '; there is currently one possibility:\n'
           '\n'
           "1) the keyword 'constant' makes it possible to have a constant Pth but not uniform \n"
           "in space ; it's the good choice when the flow is open (e.g.\n"
           'with pressure boundary conditions).',
           ''),
    'Lambda':
          ('[field_base] Conductivity (W.m-1.K-1).', ''),
    'mu':
          ('[field_base] Dynamic viscosity (kg.m-1.s-1).', ''),
    'pression_thermo':
          ('[floattant] Initial thermo-dynamic pressure used in the assosciated state law.', ''),
    'pression_xyz':
          ('[field_base] Initial thermo-dynamic pressure used in the assosciated state law.\n'
           'It should be defined with as a Champ_Fonc_xyz.',
           ''),
    'use_total_pressure':
          ('[entier] Flag (0 or 1) used to activate and use the total pressure in the assosciated \n'
           'state law.\n'
           'The default value of this Flag is 0.',
           ''),
    'use_hydrostatic_pressure':
          ('[entier] Flag (0 or 1) used to activate and use the hydro-static pressure in the assosciated \n'
           'state law.\n'
           'The default value of this Flag is 0.',
           ''),
    'use_grad_pression_eos':
          ('[entier] Flag (0 or 1) used to specify whether or not the gradient of the thermo-dynamic \n'
           'pressure will be taken into account in the source term of the temperature equation \n'
           '(case of a non-uniform pressure).\n'
           'The default value of this Flag is 1 which means that the gradient is used in the \n'
           'source.',
           ''),
    'time_activate_ptot':
          ('[floattant] Time (in seconds) at which the total pressure will be used in the assosciated \n'
           'state law.',
           ''),
  }
  _helpDict = check_append_dict('fluide_weakly_compressible_Tru', _helpDict, fluide_dilatable_base_Tru._helpDict)
##### end fluide_weakly_compressible_Tru class

class multi_gaz_parfait_wc_Tru(loi_etat_gaz_parfait_base_Tru): #####
  r"""
  Class for perfect gas multi-species mixtures state law used with a weakly-compressible 
  fluid.
  """
  # From: line 2036, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('species_number', 'entier_3282e0_Tru'),   #
    ('diffusion_coeff', '<inherited>field_base_Tru'),   #
    ('molar_mass', '<inherited>field_base_Tru'),   #
    ('mu', '<inherited>field_base_Tru'),   #
    ('cp', '<inherited>field_base_Tru'),   #
    ('prandtl', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 2036]
  _infoAttr = {'cp': ['TRAD2_example', 2041],
   'diffusion_coeff': ['TRAD2_example', 2038],
   'molar_mass': ['TRAD2_example', 2039],
   'mu': ['TRAD2_example', 2040],
   'prandtl': ['TRAD2_example', 2042],
   'species_number': ['TRAD2_example', 2037]}
  _attributesList = check_append_list('multi_gaz_parfait_wc_Tru', _attributesList, loi_etat_gaz_parfait_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('multi_gaz_parfait_wc_Tru', _attributesSynonyms, loi_etat_gaz_parfait_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('multi_gaz_parfait_wc_Tru', _infoAttr, loi_etat_gaz_parfait_base_Tru._infoAttr)
  _optionals.update(loi_etat_gaz_parfait_base_Tru._optionals)
  _helpDict = {
    'species_number':
          ('[entier] Number of species you are considering in your problem.', ''),
    'diffusion_coeff':
          ('[field_base] Diffusion coefficient of each species, defined with a Champ_uniforme \n'
           'of dimension equals to the species_number.',
           ''),
    'molar_mass':
          ('[field_base] Molar mass of each species, defined with a Champ_uniforme of dimension \n'
           'equals to the species_number.',
           ''),
    'mu':
          ('[field_base] Dynamic viscosity of each species, defined with a Champ_uniforme of dimension \n'
           'equals to the species_number.',
           ''),
    'cp':
          ('[field_base] Specific heat at constant pressure of the gas Cp, defined with a Champ_uniforme \n'
           'of dimension equals to the species_number..',
           ''),
    'prandtl':
          ('[floattant] Prandtl number of the gas Pr=mu*Cp/lambda.', ''),
  }
  _helpDict = check_append_dict('multi_gaz_parfait_wc_Tru', _helpDict, loi_etat_gaz_parfait_base_Tru._helpDict)
##### end multi_gaz_parfait_wc_Tru class

class perfect_gaz_wc_Tru(loi_etat_gaz_parfait_base_Tru): #####
  r"""
  Class for perfect gas state law used with a weakly-compressible fluid.
  """
  # From: line 2043, in file 'TRAD2_example'
  _braces = 1
  _synonyms = ['gaz_parfait_wc_Tru']
  _attributesList = [
    ('cp', 'floattant_820d8b_Tru'),   #
    ('cv', 'floattant_820d8b_Tru'),   #
    ('gamma', 'floattant_820d8b_Tru'),   #
    ('prandtl', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['cv', 'gamma'])
  _infoMain = ['TRAD2_example', 2043]
  _infoAttr = {'cp': ['TRAD2_example', 2044],
   'cv': ['TRAD2_example', 2045],
   'gamma': ['TRAD2_example', 2046],
   'prandtl': ['TRAD2_example', 2047]}
  _attributesList = check_append_list('perfect_gaz_wc_Tru', _attributesList, loi_etat_gaz_parfait_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('perfect_gaz_wc_Tru', _attributesSynonyms, loi_etat_gaz_parfait_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('perfect_gaz_wc_Tru', _infoAttr, loi_etat_gaz_parfait_base_Tru._infoAttr)
  _optionals.update(loi_etat_gaz_parfait_base_Tru._optionals)
  _helpDict = {
    'cp':
          ('[floattant] Specific heat at constant pressure (J/kg/K).', ''),
    'cv':
          ('[floattant] Specific heat at constant volume (J/kg/K).', ''),
    'gamma':
          ('[floattant] Cp/Cv', ''),
    'prandtl':
          ('[floattant] Prandtl number of the gas Pr=mu*Cp/lambda', ''),
  }
  _helpDict = check_append_dict('perfect_gaz_wc_Tru', _helpDict, loi_etat_gaz_parfait_base_Tru._helpDict)
##### end perfect_gaz_wc_Tru class

class binaire_gaz_parfait_wc_Tru(loi_etat_gaz_parfait_base_Tru): #####
  r"""
  Class for perfect gas binary mixtures state law used with a weakly-compressible fluid 
  under the iso-thermal and iso-bar assumptions.
  """
  # From: line 2048, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('molar_mass1', 'floattant_820d8b_Tru'),   #
    ('molar_mass2', 'floattant_820d8b_Tru'),   #
    ('mu1', 'floattant_820d8b_Tru'),   #
    ('mu2', 'floattant_820d8b_Tru'),   #
    ('temperature', 'floattant_820d8b_Tru'),   #
    ('diffusion_coeff', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 2048]
  _infoAttr = {'diffusion_coeff': ['TRAD2_example', 2054],
   'molar_mass1': ['TRAD2_example', 2049],
   'molar_mass2': ['TRAD2_example', 2050],
   'mu1': ['TRAD2_example', 2051],
   'mu2': ['TRAD2_example', 2052],
   'temperature': ['TRAD2_example', 2053]}
  _attributesList = check_append_list('binaire_gaz_parfait_wc_Tru', _attributesList, loi_etat_gaz_parfait_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('binaire_gaz_parfait_wc_Tru', _attributesSynonyms, loi_etat_gaz_parfait_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('binaire_gaz_parfait_wc_Tru', _infoAttr, loi_etat_gaz_parfait_base_Tru._infoAttr)
  _optionals.update(loi_etat_gaz_parfait_base_Tru._optionals)
  _helpDict = {
    'molar_mass1':
          ('[floattant] Molar mass of species 1 (in kg/mol).', ''),
    'molar_mass2':
          ('[floattant] Molar mass of species 2 (in kg/mol).', ''),
    'mu1':
          ('[floattant] Dynamic viscosity of species 1 (in kg/m.s).', ''),
    'mu2':
          ('[floattant] Dynamic viscosity of species 2 (in kg/m.s).', ''),
    'temperature':
          ('[floattant] Temperature (in Kelvin) which will be constant during the simulation since \n'
           'this state law only works for iso-thermal conditions.',
           ''),
    'diffusion_coeff':
          ('[floattant] Diffusion coefficient assumed the same for both species (in m2/s).', ''),
  }
  _helpDict = check_append_dict('binaire_gaz_parfait_wc_Tru', _helpDict, loi_etat_gaz_parfait_base_Tru._helpDict)
##### end binaire_gaz_parfait_wc_Tru class

class pb_thermohydraulique_wc_Tru(pb_base_Tru): #####
  r"""
  Resolution of thermo-hydraulic problem for a weakly-compressible fluid.
  
  Keywords for the unknowns other than pressure, velocity, temperature are :
  
  masse_volumique : density
  
  pression : reduced pressure
  
  pression_tot : total pressure
  
  pression_hydro : hydro-static pressure
  
  pression_eos : pressure used in state equation.
  """
  # From: line 2055, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_weakly_compressible', '<inherited>fluide_weakly_compressible_Tru'),   #
    ('navier_stokes_wc', 'navier_stokes_wc_Tru'),   #
    ('convection_diffusion_chaleur_wc', '<inherited>convection_diffusion_chaleur_wc_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 2055]
  _infoAttr = {'convection_diffusion_chaleur_wc': ['TRAD2_example', 2058],
   'fluide_weakly_compressible': ['TRAD2_example', 2056],
   'navier_stokes_wc': ['TRAD2_example', 2057]}
  _attributesList = check_append_list('pb_thermohydraulique_wc_Tru', _attributesList, pb_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_thermohydraulique_wc_Tru', _attributesSynonyms, pb_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_thermohydraulique_wc_Tru', _infoAttr, pb_base_Tru._infoAttr)
  _optionals.update(pb_base_Tru._optionals)
  _helpDict = {
    'fluide_weakly_compressible':
          ('[fluide_weakly_compressible] The fluid medium associated with the problem.', ''),
    'navier_stokes_wc':
          ('[navier_stokes_wc] Navier-Stokes equation for a weakly-compressible fluid.', ''),
    'convection_diffusion_chaleur_wc':
          ('[convection_diffusion_chaleur_wc] Temperature equation for a weakly-compressible fluid.', ''),
  }
  _helpDict = check_append_dict('pb_thermohydraulique_wc_Tru', _helpDict, pb_base_Tru._helpDict)
##### end pb_thermohydraulique_wc_Tru class

class pb_hydraulique_melange_binaire_wc_Tru(pb_base_Tru): #####
  r"""
  Resolution of a binary mixture problem for a weakly-compressible fluid with an iso-thermal 
  condition.
  
  Keywords for the unknowns other than pressure, velocity, fraction_massique are :
  
  masse_volumique : density
  
  pression : reduced pressure
  
  pression_tot : total pressure
  
  pression_hydro : hydro-static pressure
  
  pression_eos : pressure used in state equation.
  """
  # From: line 2059, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = [
    ('fluide_weakly_compressible', '<inherited>fluide_weakly_compressible_Tru'),   #
    ('navier_stokes_wc', 'navier_stokes_wc_Tru'),   #
    ('convection_diffusion_espece_binaire_wc', '<inherited>convection_diffusion_espece_binaire_wc_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 2059]
  _infoAttr = {'convection_diffusion_espece_binaire_wc': ['TRAD2_example', 2062],
   'fluide_weakly_compressible': ['TRAD2_example', 2060],
   'navier_stokes_wc': ['TRAD2_example', 2061]}
  _attributesList = check_append_list('pb_hydraulique_melange_binaire_wc_Tru', _attributesList, pb_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('pb_hydraulique_melange_binaire_wc_Tru', _attributesSynonyms, pb_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('pb_hydraulique_melange_binaire_wc_Tru', _infoAttr, pb_base_Tru._infoAttr)
  _optionals.update(pb_base_Tru._optionals)
  _helpDict = {
    'fluide_weakly_compressible':
          ('[fluide_weakly_compressible] The fluid medium associated with the problem.', ''),
    'navier_stokes_wc':
          ('[navier_stokes_wc] Navier-Stokes equation for a weakly-compressible fluid.', ''),
    'convection_diffusion_espece_binaire_wc':
          ('[convection_diffusion_espece_binaire_wc] Species conservation equation for a binary \n'
           'weakly-compressible fluid.',
           ''),
  }
  _helpDict = check_append_dict('pb_hydraulique_melange_binaire_wc_Tru', _helpDict, pb_base_Tru._helpDict)
##### end pb_hydraulique_melange_binaire_wc_Tru class

class convection_diffusion_chaleur_wc_Tru(eqn_base_Tru): #####
  r"""
  Temperature equation for a weakly-compressible fluid.
  """
  # From: line 2063, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 2063]
  _infoAttr = {}
  _attributesList = check_append_list('convection_diffusion_chaleur_wc_Tru', _attributesList, eqn_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_diffusion_chaleur_wc_Tru', _attributesSynonyms, eqn_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_diffusion_chaleur_wc_Tru', _infoAttr, eqn_base_Tru._infoAttr)
  _optionals.update(eqn_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('convection_diffusion_chaleur_wc_Tru', _helpDict, eqn_base_Tru._helpDict)
##### end convection_diffusion_chaleur_wc_Tru class

class convection_diffusion_espece_multi_wc_Tru(eqn_base_Tru): #####
  r"""
  Species conservation equation for a multi-species weakly-compressible fluid.
  """
  # From: line 2064, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 2064]
  _infoAttr = {}
  _attributesList = check_append_list('convection_diffusion_espece_multi_wc_Tru', _attributesList, eqn_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_diffusion_espece_multi_wc_Tru', _attributesSynonyms, eqn_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_diffusion_espece_multi_wc_Tru', _infoAttr, eqn_base_Tru._infoAttr)
  _optionals.update(eqn_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('convection_diffusion_espece_multi_wc_Tru', _helpDict, eqn_base_Tru._helpDict)
##### end convection_diffusion_espece_multi_wc_Tru class

class navier_stokes_wc_Tru(navier_stokes_standard_Tru): #####
  r"""
  Navier-Stokes equation for a weakly-compressible fluid.
  """
  # From: line 2065, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 2065]
  _infoAttr = {}
  _attributesList = check_append_list('navier_stokes_wc_Tru', _attributesList, navier_stokes_standard_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('navier_stokes_wc_Tru', _attributesSynonyms, navier_stokes_standard_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('navier_stokes_wc_Tru', _infoAttr, navier_stokes_standard_Tru._infoAttr)
  _optionals.update(navier_stokes_standard_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('navier_stokes_wc_Tru', _helpDict, navier_stokes_standard_Tru._helpDict)
##### end navier_stokes_wc_Tru class

class convection_diffusion_espece_binaire_wc_Tru(eqn_base_Tru): #####
  r"""
  Species conservation equation for a binary weakly-compressible fluid.
  """
  # From: line 2066, in file 'TRAD2_example'
  _braces = -1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 2066]
  _infoAttr = {}
  _attributesList = check_append_list('convection_diffusion_espece_binaire_wc_Tru', _attributesList, eqn_base_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('convection_diffusion_espece_binaire_wc_Tru', _attributesSynonyms, eqn_base_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('convection_diffusion_espece_binaire_wc_Tru', _infoAttr, eqn_base_Tru._infoAttr)
  _optionals.update(eqn_base_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('convection_diffusion_espece_binaire_wc_Tru', _helpDict, eqn_base_Tru._helpDict)
##### end convection_diffusion_espece_binaire_wc_Tru class

class option_polymac_Tru(interprete_Tru): #####
  r"""
  Class of PolyMAC options.
  """
  # From: line 2067, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('use_osqp', 'rien_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['use_osqp'])
  _infoMain = ['TRAD2_example', 2067]
  _infoAttr = {'use_osqp': ['TRAD2_example', 2068]}
  _attributesList = check_append_list('option_polymac_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('option_polymac_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('option_polymac_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'use_osqp':
          ('[rien] Flag to use the old formulation of the M2 matrix provided by the OSQP library', ''),
  }
  _helpDict = check_append_dict('option_polymac_Tru', _helpDict, interprete_Tru._helpDict)
##### end option_polymac_Tru class

class option_polymac_p0_Tru(interprete_Tru): #####
  r"""
  Class of PolyMAC_P0 options.
  """
  # From: line 2069, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('interp_ve1', 'rien_Tru'),   #
    ('traitement_axi', 'rien_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['interp_ve1', 'traitement_axi'])
  _infoMain = ['TRAD2_example', 2069]
  _infoAttr = {'interp_ve1': ['TRAD2_example', 2070],
   'traitement_axi': ['TRAD2_example', 2071]}
  _attributesList = check_append_list('option_polymac_p0_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('option_polymac_p0_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('option_polymac_p0_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'interp_ve1':
          ('[rien] Flag to enable a first order velocity face-to-element interpolation (the default \n'
           'value is 0 which means a second order interpolation)',
           ''),
    'traitement_axi':
          ('[rien] Flag used to relax the time-step stability criterion in case of a thin slice \n'
           'geometry while modelling an axi-symetrical case',
           ''),
  }
  _helpDict = check_append_dict('option_polymac_p0_Tru', _helpDict, interprete_Tru._helpDict)
##### end option_polymac_p0_Tru class

class op_conv_ef_stab_polymac_p0p1nc_elem_Tru(interprete_Tru): #####
  r"""
  Class Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem
  """
  # From: line 2072, in file 'TRAD2_example'
  _braces = 1
  _synonyms = ['op_conv_ef_stab_polymac_p0_elem_Tru']
  _attributesList = [
    ('alpha', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['alpha'])
  _infoMain = ['TRAD2_example', 2072]
  _infoAttr = {'alpha': ['TRAD2_example', 2073]}
  _attributesList = check_append_list('op_conv_ef_stab_polymac_p0p1nc_elem_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('op_conv_ef_stab_polymac_p0p1nc_elem_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('op_conv_ef_stab_polymac_p0p1nc_elem_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'alpha':
          ('[floattant] parametre ajustant la stabilisation de 0 (schema centre) a 1 (schema amont)', ''),
  }
  _helpDict = check_append_dict('op_conv_ef_stab_polymac_p0p1nc_elem_Tru', _helpDict, interprete_Tru._helpDict)
##### end op_conv_ef_stab_polymac_p0p1nc_elem_Tru class

class op_conv_ef_stab_polymac_p0_face_Tru(interprete_Tru): #####
  r"""
  Class Op_Conv_EF_Stab_PolyMAC_P0_Face
  """
  # From: line 2074, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 2074]
  _infoAttr = {}
  _attributesList = check_append_list('op_conv_ef_stab_polymac_p0_face_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('op_conv_ef_stab_polymac_p0_face_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('op_conv_ef_stab_polymac_p0_face_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('op_conv_ef_stab_polymac_p0_face_Tru', _helpDict, interprete_Tru._helpDict)
##### end op_conv_ef_stab_polymac_p0_face_Tru class

class op_conv_ef_stab_polymac_p0p1nc_face_Tru(interprete_Tru): #####
  r"""
  Class Op_Conv_EF_Stab_PolyMAC_P0P1NC_Face
  """
  # From: line 2075, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = []
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set([])
  _infoMain = ['TRAD2_example', 2075]
  _infoAttr = {}
  _attributesList = check_append_list('op_conv_ef_stab_polymac_p0p1nc_face_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('op_conv_ef_stab_polymac_p0p1nc_face_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('op_conv_ef_stab_polymac_p0p1nc_face_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {}
  _helpDict = check_append_dict('op_conv_ef_stab_polymac_p0p1nc_face_Tru', _helpDict, interprete_Tru._helpDict)
##### end op_conv_ef_stab_polymac_p0p1nc_face_Tru class

class op_conv_ef_stab_polymac_face_Tru(interprete_Tru): #####
  r"""
  Class Op_Conv_EF_Stab_PolyMAC_Face_PolyMAC
  """
  # From: line 2076, in file 'TRAD2_example'
  _braces = 1
  _synonyms = []
  _attributesList = [
    ('alpha', 'floattant_820d8b_Tru'),   #
  ]
  _suppressed = []
  _attributesSynonyms = {}
  _optionals = set(['alpha'])
  _infoMain = ['TRAD2_example', 2076]
  _infoAttr = {'alpha': ['TRAD2_example', 2077]}
  _attributesList = check_append_list('op_conv_ef_stab_polymac_face_Tru', _attributesList, interprete_Tru._attributesList, _suppressed)
  _attributesSynonyms = check_append_dict('op_conv_ef_stab_polymac_face_Tru', _attributesSynonyms, interprete_Tru._attributesSynonyms)
  _infoAttr = check_append_dict('op_conv_ef_stab_polymac_face_Tru', _infoAttr, interprete_Tru._infoAttr)
  _optionals.update(interprete_Tru._optionals)
  _helpDict = {
    'alpha':
          ('[floattant] parametre ajustant la stabilisation de 0 (schema centre) a 1 (schema amont)', ''),
  }
  _helpDict = check_append_dict('op_conv_ef_stab_polymac_face_Tru', _helpDict, interprete_Tru._helpDict)
##### end op_conv_ef_stab_polymac_face_Tru class


##### automatic classes

class chaine_db4169_Tru(BaseChaineInList_Tru):
  """
  automatic class 0 key chaine_db4169 from format_file.format ->
  chaine(into=["binaire","formatte","xyz","single_hdf"])
  """  
  _allowedList = ["binaire","formatte","xyz","single_hdf"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_db4169_Tru class

class entier_3282e0_Tru(BaseEntier_Tru):
  """
  automatic class 1 key entier_3282e0 from deuxentiers.int1 ->
  entier
  """  
  pass
##### end entier_3282e0_Tru class

class floattant_820d8b_Tru(BaseFloattant_Tru):
  """
  automatic class 2 key floattant_820d8b from floatfloat.a ->
  floattant
  """  
  pass
##### end floattant_820d8b_Tru class

class chaine_a96986_Tru(BaseChaineInList_Tru):
  """
  automatic class 3 key chaine_a96986 from champ_a_post.localisation ->
  chaine(into=["elem","som","faces"])
  """  
  _allowedList = ["elem","som","faces"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_a96986_Tru class

class chaine_7d729e_Tru(BaseChaineInList_Tru):
  """
  automatic class 4 key chaine_7d729e from champs_posts.format ->
  chaine(into=["binaire","formatte"])
  """  
  _allowedList = ["binaire","formatte"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_7d729e_Tru class

class chaine_824559_Tru(BaseChaineInList_Tru):
  """
  automatic class 5 key chaine_824559 from champs_posts.mot ->
  chaine(into=["dt_post","nb_pas_dt_post"])
  """  
  _allowedList = ["dt_post","nb_pas_dt_post"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_824559_Tru class

class chaine_38aea9_Tru(BaseChaineInList_Tru):
  """
  automatic class 6 key chaine_38aea9 from stats_serie_posts.mot ->
  chaine(into=["dt_integr"])
  """  
  _allowedList = ["dt_integr"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_38aea9_Tru class

class listf_Tru(ListOfFloatDim_Tru):
  """
  automatic class 7 key listf from un_point.pos ->
  listf
  """  
  pass
##### end listf_Tru class

class entier_6f2a33_Tru(BaseEntierInList_Tru):
  """
  automatic class 8 key entier_6f2a33 from circle.direction ->
  entier(into=[0,1,2])
  """  
  _allowedList = [0,1,2]
  _defaultValue = _allowedList[0]
  pass
##### end entier_6f2a33_Tru class

class rien_Tru(Rien_Tru):
  """
  automatic class 9 key rien from champ_post_operateur_eqn.sans_solveur_masse ->
  rien
  """  
  pass
##### end rien_Tru class

class chaine_deccae_Tru(BaseChaineInList_Tru):
  """
  automatic class 10 key chaine_deccae from transformation.methode ->
  chaine(into=["produit_scalaire","norme","vecteur","formule","composante"])
  """  
  _allowedList = ["produit_scalaire","norme","vecteur","formule","composante"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_deccae_Tru class

class listchaine_Tru(ListOfChaine_Tru):
  """
  automatic class 11 key listchaine from transformation.expression ->
  listchaine
  """  
  pass
##### end listchaine_Tru class

class ref_domaine_Tru(BaseChaine_Tru):
  """
  automatic class 12 key ref_domaine from extraction.domaine ->
  ref_domaine
  """  
  pass
##### end ref_domaine_Tru class

class chaine_10bcdf_Tru(BaseChaineInList_Tru):
  """
  automatic class 13 key chaine_10bcdf from extraction.methode ->
  chaine(into=["trace","champ_frontiere"])
  """  
  _allowedList = ["trace","champ_frontiere"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_10bcdf_Tru class

class chaine_0bf8d2_Tru(BaseChaineInList_Tru):
  """
  automatic class 14 key chaine_0bf8d2 from morceau_equation.option ->
  chaine(into=["stabilite","flux_bords","flux_surfacique_bords"])
  """  
  _allowedList = ["stabilite","flux_bords","flux_surfacique_bords"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_0bf8d2_Tru class

class chaine_e3346e_Tru(BaseChaineInList_Tru):
  """
  automatic class 15 key chaine_e3346e from type_un_post.type ->
  chaine(into=["postraitement","post_processing"])
  """  
  _allowedList = ["postraitement","post_processing"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_e3346e_Tru class

class chaine_1feca2_Tru(BaseChaineInList_Tru):
  """
  automatic class 16 key chaine_1feca2 from type_postraitement_ft_lata.type ->
  chaine(into=["postraitement_ft_lata","postraitement_lata"])
  """  
  _allowedList = ["postraitement_ft_lata","postraitement_lata"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_1feca2_Tru class

class ref_pb_gen_base_Tru(BaseChaine_Tru):
  """
  automatic class 17 key ref_pb_gen_base from discretize.problem_name ->
  ref_pb_gen_base
  """  
  pass
##### end ref_pb_gen_base_Tru class

class ref_discretisation_base_Tru(BaseChaine_Tru):
  """
  automatic class 18 key ref_discretisation_base from discretize.dis ->
  ref_discretisation_base
  """  
  pass
##### end ref_discretisation_base_Tru class

class chaine_ad85af_Tru(BaseChaineInList_Tru):
  """
  automatic class 19 key chaine_ad85af from defbord_3.dir ->
  chaine(into=["x","y","z"])
  """  
  _allowedList = ["x","y","z"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_ad85af_Tru class

class chaine_52b4a3_Tru(BaseChaineInList_Tru):
  """
  automatic class 20 key chaine_52b4a3 from defbord_3.eq ->
  chaine(into=["="])
  """  
  _allowedList = ["="]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_52b4a3_Tru class

class chaine_4aae18_Tru(BaseChaineInList_Tru):
  """
  automatic class 21 key chaine_4aae18 from defbord_3.inf1 ->
  chaine(into=["<="])
  """  
  _allowedList = ["<="]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_4aae18_Tru class

class chaine_036e4c_Tru(BaseChaineInList_Tru):
  """
  automatic class 22 key chaine_036e4c from defbord_3.dir2 ->
  chaine(into=["x","y"])
  """  
  _allowedList = ["x","y"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_036e4c_Tru class

class chaine_241470_Tru(BaseChaineInList_Tru):
  """
  automatic class 23 key chaine_241470 from defbord_3.dir3 ->
  chaine(into=["y","z"])
  """  
  _allowedList = ["y","z"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_241470_Tru class

class chaine_43a355_Tru(BaseChaineInList_Tru):
  """
  automatic class 24 key chaine_43a355 from raccord.type1 ->
  chaine(into=["local","distant"])
  """  
  _allowedList = ["local","distant"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_43a355_Tru class

class chaine_c39562_Tru(BaseChaineInList_Tru):
  """
  automatic class 25 key chaine_c39562 from raccord.type2 ->
  chaine(into=["homogene"])
  """  
  _allowedList = ["homogene"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_c39562_Tru class

class listentier_Tru(ListOfInt_Tru):
  """
  automatic class 26 key listentier from maillerparallel.nb_nodes ->
  listentier
  """  
  pass
##### end listentier_Tru class

class entier_8bf71a_Tru(BaseEntierInList_Tru):
  """
  automatic class 27 key entier_8bf71a from vef.changement_de_base_p1bulle ->
  entier(into=[0,1])
  """  
  _allowedList = [0,1]
  _defaultValue = _allowedList[0]
  pass
##### end entier_8bf71a_Tru class

class entier_216c6a_Tru(BaseEntierInList_Tru):
  """
  automatic class 28 key entier_216c6a from dimension.dim ->
  entier(into=[2,3])
  """  
  _allowedList = [2,3]
  _defaultValue = _allowedList[0]
  pass
##### end entier_216c6a_Tru class

class listchainef_Tru(ListOfChaineDim_Tru):
  """
  automatic class 29 key listchainef from transformer.formule ->
  listchainef
  """  
  pass
##### end listchainef_Tru class

class ref_pb_base_Tru(BaseChaine_Tru):
  """
  automatic class 30 key ref_pb_base from un_pb.mot ->
  ref_pb_base
  """  
  pass
##### end ref_pb_base_Tru class

class chaine_f0532c_Tru(BaseChaineInList_Tru):
  """
  automatic class 31 key chaine_f0532c from bords_ecrire.chaine ->
  chaine(into=["bords"])
  """  
  _allowedList = ["bords"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_f0532c_Tru class

class chaine_e3a651_Tru(BaseChaineInList_Tru):
  """
  automatic class 32 key chaine_e3a651 from bloc_convection.aco ->
  chaine(into=["{"])
  """  
  _allowedList = ["{"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_e3a651_Tru class

class chaine_af6447_Tru(BaseChaineInList_Tru):
  """
  automatic class 33 key chaine_af6447 from bloc_convection.acof ->
  chaine(into=["}"])
  """  
  _allowedList = ["}"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_af6447_Tru class

class chaine_a28892_Tru(BaseChaineInList_Tru):
  """
  automatic class 34 key chaine_a28892 from op_implicite.implicite ->
  chaine(into=["implicite"])
  """  
  _allowedList = ["implicite"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_a28892_Tru class

class chaine_2a7c96_Tru(BaseChaineInList_Tru):
  """
  automatic class 35 key chaine_2a7c96 from op_implicite.mot ->
  chaine(into=["solveur"])
  """  
  _allowedList = ["solveur"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_2a7c96_Tru class

class chaine_39eae7_Tru(BaseChaineInList_Tru):
  """
  automatic class 36 key chaine_39eae7 from navier_stokes_standard.methode_calcul_pression_initiale ->
  chaine(into=["avec_les_cl","avec_sources","avec_sources_et_operateurs","sans_rien"])
  """  
  _allowedList = ["avec_les_cl","avec_sources","avec_sources_et_operateurs","sans_rien"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_39eae7_Tru class

class list_Tru(ListOfFloat_Tru):
  """
  automatic class 37 key list from penalisation_l2_ftd_lec.val ->
  list
  """  
  pass
##### end list_Tru class

class chaine_d75b13_Tru(BaseChaineInList_Tru):
  """
  automatic class 38 key chaine_d75b13 from paroi_knudsen_non_negligeable.name_champ_1 ->
  chaine(into=["vitesse_paroi","k"])
  """  
  _allowedList = ["vitesse_paroi","k"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_d75b13_Tru class

class chaine_917c90_Tru(BaseChaineInList_Tru):
  """
  automatic class 39 key chaine_917c90 from frontiere_ouverte.var_name ->
  chaine(into=["t_ext","c_ext","y_ext","k_eps_ext","fluctu_temperature_ext","flux_chaleur_turb_ext","v2_ext","a_ext","tau_ext","k_ext","omega_ext"])
  """  
  _allowedList = ["t_ext","c_ext","y_ext","k_eps_ext","fluctu_temperature_ext","flux_chaleur_turb_ext","v2_ext","a_ext","tau_ext","k_ext","omega_ext"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_917c90_Tru class

class entier_a44528_Tru(BaseEntierInList_Tru):
  """
  automatic class 40 key entier_a44528 from init_par_partie.n_comp ->
  entier(into=[1])
  """  
  _allowedList = [1]
  _defaultValue = _allowedList[0]
  pass
##### end entier_a44528_Tru class

class listentierf_Tru(ListOfIntDim_Tru):
  """
  automatic class 41 key listentierf from champ_front_fonc_pois_tube.r_loc_mult ->
  listentierf
  """  
  pass
##### end listentierf_Tru class

class chaine_36f0dc_Tru(BaseChaineInList_Tru):
  """
  automatic class 42 key chaine_36f0dc from champ_front_tangentiel_vef.mot ->
  chaine(into=["vitesse_tangentielle"])
  """  
  _allowedList = ["vitesse_tangentielle"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_36f0dc_Tru class

class entier_6f194b_Tru(BaseEntierInList_Tru):
  """
  automatic class 43 key entier_6f194b from form_a_nb_points.nb ->
  entier(into=[4])
  """  
  _allowedList = [4]
  _defaultValue = _allowedList[0]
  pass
##### end entier_6f194b_Tru class

class entier_fd157e_Tru(BaseEntierInRange_Tru):
  """
  automatic class 44 key entier_fd157e from form_a_nb_points.dir1 ->
  entier(max=2)
  """  
  _allowedRange = [0, 2]
  _defaultValue = _allowedRange[0]
  pass
##### end entier_fd157e_Tru class

class entier_b08690_Tru(BaseEntierInList_Tru):
  """
  automatic class 45 key entier_b08690 from gmres.controle_residu ->
  entier(into=["0","1"])
  """  
  _allowedList = [0,1]
  _defaultValue = _allowedList[0]
  pass
##### end entier_b08690_Tru class

class chaine_f07ecc_Tru(BaseChaineInList_Tru):
  """
  automatic class 46 key chaine_f07ecc from postraiter_domaine.format ->
  chaine(into=["lml","lata","lata_v2","med"])
  """  
  _allowedList = ["lml","lata","lata_v2","med"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_f07ecc_Tru class

class ref_sous_zone_Tru(BaseChaine_Tru):
  """
  automatic class 47 key ref_sous_zone from postraiter_domaine.sous_zone ->
  ref_sous_zone
  """  
  pass
##### end ref_sous_zone_Tru class

class chaine_a32a8b_Tru(BaseChaineInList_Tru):
  """
  automatic class 48 key chaine_a32a8b from bloc_origine_cotes.name ->
  chaine(into=["origine"])
  """  
  _allowedList = ["origine"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_a32a8b_Tru class

class chaine_a7556e_Tru(BaseChaineInList_Tru):
  """
  automatic class 49 key chaine_a7556e from bloc_origine_cotes.name2 ->
  chaine(into=["cotes"])
  """  
  _allowedList = ["cotes"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_a7556e_Tru class

class chaine_1d8e3b_Tru(BaseChaineInList_Tru):
  """
  automatic class 50 key chaine_1d8e3b from bloc_couronne.name3 ->
  chaine(into=["ri"])
  """  
  _allowedList = ["ri"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_1d8e3b_Tru class

class chaine_08814c_Tru(BaseChaineInList_Tru):
  """
  automatic class 51 key chaine_08814c from bloc_couronne.name4 ->
  chaine(into=["re"])
  """  
  _allowedList = ["re"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_08814c_Tru class

class chaine_56e2aa_Tru(BaseChaineInList_Tru):
  """
  automatic class 52 key chaine_56e2aa from bloc_tube.name2 ->
  chaine(into=["dir"])
  """  
  _allowedList = ["dir"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_56e2aa_Tru class

class chaine_8c4850_Tru(BaseChaineInList_Tru):
  """
  automatic class 53 key chaine_8c4850 from bloc_tube.name5 ->
  chaine(into=["hauteur"])
  """  
  _allowedList = ["hauteur"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_8c4850_Tru class

class chaine_1ca9d5_Tru(BaseChaineInList_Tru):
  """
  automatic class 54 key chaine_1ca9d5 from acceleration.option ->
  chaine(into=["terme_complet","coriolis_seul","entrainement_seul"])
  """  
  _allowedList = ["terme_complet","coriolis_seul","entrainement_seul"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_1ca9d5_Tru class

class chaine_d74614_Tru(BaseChaineInList_Tru):
  """
  automatic class 55 key chaine_d74614 from fonction_champ_reprise.mot ->
  chaine(into=["fonction"])
  """  
  _allowedList = ["fonction"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_d74614_Tru class

class chaine_13672b_Tru(BaseChaineInList_Tru):
  """
  automatic class 56 key chaine_13672b from perte_charge_singuliere.dir ->
  chaine(into=["kx","ky","kz","k"])
  """  
  _allowedList = ["kx","ky","kz","k"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_13672b_Tru class

class chaine_7347ec_Tru(BaseChaineInList_Tru):
  """
  automatic class 57 key chaine_7347ec from moyenne_volumique.localisation ->
  chaine(into=["elem","som"])
  """  
  _allowedList = ["elem","som"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_7347ec_Tru class

class chaine_ba189d_Tru(BaseChaineInList_Tru):
  """
  automatic class 58 key chaine_ba189d from spec_pdcr_base.ch_a ->
  chaine(into=["a","cf"])
  """  
  _allowedList = ["a","cf"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_ba189d_Tru class

class chaine_7956cf_Tru(BaseChaineInList_Tru):
  """
  automatic class 59 key chaine_7956cf from spec_pdcr_base.ch_b ->
  chaine(into=["b"])
  """  
  _allowedList = ["b"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_7956cf_Tru class

class chaine_437a7b_Tru(BaseChaineInList_Tru):
  """
  automatic class 60 key chaine_437a7b from transversale.chaine_d ->
  chaine(into=["d"])
  """  
  _allowedList = ["d"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_437a7b_Tru class

class ref_loi_horaire_Tru(BaseChaine_Tru):
  """
  automatic class 61 key ref_loi_horaire from methode_loi_horaire.nom_loi ->
  ref_loi_horaire
  """  
  pass
##### end ref_loi_horaire_Tru class

class ref_champ_fonc_med_Tru(BaseChaine_Tru):
  """
  automatic class 62 key ref_champ_fonc_med from integrer_champ_med.champ_med ->
  ref_champ_fonc_med
  """  
  pass
##### end ref_champ_fonc_med_Tru class

class chaine_5dc32f_Tru(BaseChaineInList_Tru):
  """
  automatic class 63 key chaine_5dc32f from integrer_champ_med.methode ->
  chaine(into=["integrale_en_z","debit_total"])
  """  
  _allowedList = ["integrale_en_z","debit_total"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_5dc32f_Tru class

class chaine_e80920_Tru(BaseChaineInList_Tru):
  """
  automatic class 64 key chaine_e80920 from bloc_ef.mot1 ->
  chaine(into=["transportant_bar","transporte_bar","filtrer_resu","antisym"])
  """  
  _allowedList = ["transportant_bar","transporte_bar","filtrer_resu","antisym"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_e80920_Tru class

class chaine_a3dc34_Tru(BaseChaineInList_Tru):
  """
  automatic class 65 key chaine_a3dc34 from convection_ef.mot1 ->
  chaine(into=["defaut_bar"])
  """  
  _allowedList = ["defaut_bar"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_a3dc34_Tru class

class chaine_1691a8_Tru(BaseChaineInList_Tru):
  """
  automatic class 66 key chaine_1691a8 from convection_generic.type ->
  chaine(into=["amont","muscl","centre"])
  """  
  _allowedList = ["amont","muscl","centre"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_1691a8_Tru class

class chaine_c002ae_Tru(BaseChaineInList_Tru):
  """
  automatic class 67 key chaine_c002ae from convection_generic.limiteur ->
  chaine(into=["minmod","vanleer","vanalbada","chakravarthy","superbee"])
  """  
  _allowedList = ["minmod","vanleer","vanalbada","chakravarthy","superbee"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_c002ae_Tru class

class entier_6d15af_Tru(BaseEntierInList_Tru):
  """
  automatic class 68 key entier_6d15af from convection_generic.ordre ->
  entier(into=[1,2,3])
  """  
  _allowedList = [1,2,3]
  _defaultValue = _allowedList[0]
  pass
##### end entier_6d15af_Tru class

class chaine_84289f_Tru(BaseChaineInList_Tru):
  """
  automatic class 69 key chaine_84289f from bloc_diffusion_standard.mot1 ->
  chaine(into=["grad_ubar","nu","nut","nu_transp","nut_transp","filtrer_resu"])
  """  
  _allowedList = ["grad_ubar","nu","nut","nu_transp","nut_transp","filtrer_resu"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_84289f_Tru class

class chaine_588727_Tru(BaseChaineInList_Tru):
  """
  automatic class 70 key chaine_588727 from champ_front_normal_vef.mot ->
  chaine(into=["valeur_normale"])
  """  
  _allowedList = ["valeur_normale"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_588727_Tru class

class chaine_d249f7_Tru(BaseChaineInList_Tru):
  """
  automatic class 71 key chaine_d249f7 from reduction_0d.methode ->
  chaine(into=["min","max","moyenne","average","moyenne_ponderee","weighted_average","somme","sum","somme_ponderee","weighted_sum","somme_ponderee_porosite","weighted_sum_porosity","euclidian_norm","normalized_euclidian_norm","l1_norm","l2_norm","valeur_a_gauche","left_value"])
  """  
  _allowedList = ["min","max","moyenne","average","moyenne_ponderee","weighted_average","somme","sum","somme_ponderee","weighted_sum","somme_ponderee_porosite","weighted_sum_porosity","euclidian_norm","normalized_euclidian_norm","l1_norm","l2_norm","valeur_a_gauche","left_value"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_d249f7_Tru class

class chaine_910cb5_Tru(BaseChaineInList_Tru):
  """
  automatic class 72 key chaine_910cb5 from interpolation.optimisation_sous_maillage ->
  chaine(into=["default","yes","no",])
  """  
  _allowedList = ["default","yes","no",]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_910cb5_Tru class

class entier_5e32c8_Tru(BaseEntierInList_Tru):
  """
  automatic class 73 key entier_5e32c8 from bloc_pave.xtanh_dilatation ->
  entier(into=[-1,0,1])
  """  
  _allowedList = [-1,0,1]
  _defaultValue = _allowedList[0]
  pass
##### end entier_5e32c8_Tru class

class chaine_ae5ad3_Tru(BaseChaineInList_Tru):
  """
  automatic class 74 key chaine_ae5ad3 from partition_multi.domaine1 ->
  chaine(into=["domaine"])
  """  
  _allowedList = ["domaine"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_ae5ad3_Tru class

class chaine_6cbaef_Tru(BaseChaineInList_Tru):
  """
  automatic class 75 key chaine_6cbaef from sonde.special ->
  chaine(into=["grav","som","nodes","chsom","gravcl"])
  """  
  _allowedList = ["grav","som","nodes","chsom","gravcl"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_6cbaef_Tru class

class chaine_cddc57_Tru(BaseChaineInList_Tru):
  """
  automatic class 76 key chaine_cddc57 from sonde.mperiode ->
  chaine(into=["periode"])
  """  
  _allowedList = ["periode"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_cddc57_Tru class

class chaine_8faf53_Tru(BaseChaineInList_Tru):
  """
  automatic class 77 key chaine_8faf53 from postraitement.format ->
  chaine(into=["lml","lata","lata_v2","med","med_major"])
  """  
  _allowedList = ["lml","lata","lata_v2","med","med_major"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_8faf53_Tru class

class chaine_e7fecf_Tru(BaseChaineInList_Tru):
  """
  automatic class 78 key chaine_e7fecf from postraitement.parallele ->
  chaine(into=["simple","multiple","mpi-io"])
  """  
  _allowedList = ["simple","multiple","mpi-io"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_e7fecf_Tru class

class chaine_48a9fa_Tru(BaseChaineInList_Tru):
  """
  automatic class 79 key chaine_48a9fa from residuals.norm ->
  chaine(into=["l2","max"])
  """  
  _allowedList = ["l2","max"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_48a9fa_Tru class

class chaine_d2dddb_Tru(BaseChaineInList_Tru):
  """
  automatic class 80 key chaine_d2dddb from residuals.relative ->
  chaine(into=["0","1","2"])
  """  
  _allowedList = ["0","1","2"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_d2dddb_Tru class

class chaine_e57e1c_Tru(BaseChaineInList_Tru):
  """
  automatic class 81 key chaine_e57e1c from format_lata_to_med.mot ->
  chaine(into=["format_post_sup"])
  """  
  _allowedList = ["format_post_sup"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_e57e1c_Tru class

class ref_field_base_Tru(BaseChaine_Tru):
  """
  automatic class 82 key ref_field_base from ecrire_champ_med.nom_chp ->
  ref_field_base
  """  
  pass
##### end ref_field_base_Tru class

class chaine_45470e_Tru(BaseChaineInList_Tru):
  """
  automatic class 83 key chaine_45470e from merge_med.time_iterations ->
  chaine(into=["all_times","last_time"])
  """  
  _allowedList = ["all_times","last_time"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_45470e_Tru class

class chaine_8b7047_Tru(BaseChaineInList_Tru):
  """
  automatic class 84 key chaine_8b7047 from champ_fonc_med.loc ->
  chaine(into=["som","elem"])
  """  
  _allowedList = ["som","elem"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_8b7047_Tru class

class chaine_dbffd3_Tru(BaseChaineInList_Tru):
  """
  automatic class 85 key chaine_dbffd3 from coarsen_operator_uniform.coarsen_i ->
  chaine(into=["coarsen_i"])
  """  
  _allowedList = ["coarsen_i"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_dbffd3_Tru class

class chaine_41af74_Tru(BaseChaineInList_Tru):
  """
  automatic class 86 key chaine_41af74 from coarsen_operator_uniform.coarsen_j ->
  chaine(into=["coarsen_j"])
  """  
  _allowedList = ["coarsen_j"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_41af74_Tru class

class chaine_c64c02_Tru(BaseChaineInList_Tru):
  """
  automatic class 87 key chaine_c64c02 from coarsen_operator_uniform.coarsen_k ->
  chaine(into=["coarsen_k"])
  """  
  _allowedList = ["coarsen_k"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_c64c02_Tru class

class chaine_c49d25_Tru(BaseChaineInList_Tru):
  """
  automatic class 88 key chaine_c49d25 from multigrid_solver.solver_precision ->
  chaine(into=["mixed","double"])
  """  
  _allowedList = ["mixed","double"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_c49d25_Tru class

class ref_ijk_grid_geometry_Tru(BaseChaine_Tru):
  """
  automatic class 89 key ref_ijk_grid_geometry from ijk_splitting.ijk_grid_geometry ->
  ref_ijk_grid_geometry
  """  
  pass
##### end ref_ijk_grid_geometry_Tru class

class chaine_ad7267_Tru(BaseChaineInList_Tru):
  """
  automatic class 90 key chaine_ad7267 from option_vdf.traitement_coins ->
  chaine(into=["oui","non"])
  """  
  _allowedList = ["oui","non"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_ad7267_Tru class

class chaine_6c41af_Tru(BaseChaineInList_Tru):
  """
  automatic class 91 key chaine_6c41af from fluide_quasi_compressible.traitement_pth ->
  chaine(into=["edo","constant","conservation_masse"])
  """  
  _allowedList = ["edo","constant","conservation_masse"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_6c41af_Tru class

class chaine_7e6e04_Tru(BaseChaineInList_Tru):
  """
  automatic class 92 key chaine_7e6e04 from fluide_quasi_compressible.traitement_rho_gravite ->
  chaine(into=["standard","moins_rho_moyen"])
  """  
  _allowedList = ["standard","moins_rho_moyen"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_7e6e04_Tru class

class chaine_23652b_Tru(BaseChaineInList_Tru):
  """
  automatic class 93 key chaine_23652b from bloc_sutherland.mu0 ->
  chaine(into=["mu0"])
  """  
  _allowedList = ["mu0"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_23652b_Tru class

class chaine_86c49e_Tru(BaseChaineInList_Tru):
  """
  automatic class 94 key chaine_86c49e from bloc_sutherland.t0 ->
  chaine(into=["t0"])
  """  
  _allowedList = ["t0"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_86c49e_Tru class

class chaine_269f2a_Tru(BaseChaineInList_Tru):
  """
  automatic class 95 key chaine_269f2a from bloc_sutherland.slambda ->
  chaine(into=["slambda"])
  """  
  _allowedList = ["slambda"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_269f2a_Tru class

class chaine_96adb9_Tru(BaseChaineInList_Tru):
  """
  automatic class 96 key chaine_96adb9 from bloc_sutherland.c ->
  chaine(into=["c"])
  """  
  _allowedList = ["c"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_96adb9_Tru class

class chaine_385ae2_Tru(BaseChaineInList_Tru):
  """
  automatic class 97 key chaine_385ae2 from convection_diffusion_chaleur_qc.mode_calcul_convection ->
  chaine(into=["ancien","divut_moins_tdivu","divrhout_moins_tdivrhou"])
  """  
  _allowedList = ["ancien","divut_moins_tdivu","divrhout_moins_tdivrhou"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_385ae2_Tru class

class chaine_c53c45_Tru(BaseChaineInList_Tru):
  """
  automatic class 98 key chaine_c53c45 from fluide_weakly_compressible.traitement_pth ->
  chaine(into=["constant"])
  """  
  _allowedList = ["constant"]
  _defaultValue = _allowedList[0]
  pass
##### end chaine_c53c45_Tru class

    
def initXyzClasses():
  """case of CLFX.reset() done may be useful to redo appendAllXyzClasses"""
  if "acceleration_Tru" not in CLFX.getAllXyzClassesNames():
    CLFX.appendAllXyzClasses(packagespy_classes)

# useful for future import on this file
packagespy_classes = [
  acceleration_Tru,
  amgx_Tru,
  analyse_angle_Tru,
  associate_Tru,
  axi_Tru,
  bidim_axi_Tru,
  binaire_gaz_parfait_qc_Tru,
  binaire_gaz_parfait_wc_Tru,
  bloc_comment_Tru,
  bloc_convection_Tru,
  bloc_couronne_Tru,
  bloc_criteres_convergence_Tru,
  bloc_decouper_Tru,
  bloc_diffusion_Tru,
  bloc_diffusion_standard_Tru,
  bloc_ef_Tru,
  bloc_lec_champ_init_canal_sinal_Tru,
  bloc_lecture_Tru,
  bloc_lecture_poro_Tru,
  bloc_origine_cotes_Tru,
  bloc_pave_Tru,
  bloc_pdf_model_Tru,
  bloc_sutherland_Tru,
  bloc_tube_Tru,
  bord_Tru,
  bord_base_Tru,
  bords_ecrire_Tru,
  boundary_field_inward_Tru,
  boussinesq_concentration_Tru,
  boussinesq_temperature_Tru,
  calcul_Tru,
  calculer_moments_Tru,
  canal_Tru,
  canal_perio_Tru,
  centre_de_gravite_Tru,
  ch_front_input_Tru,
  ch_front_input_uniforme_Tru,
  champ_a_post_Tru,
  champ_composite_Tru,
  champ_don_base_Tru,
  champ_don_lu_Tru,
  champ_fonc_fonction_Tru,
  champ_fonc_fonction_txyz_Tru,
  champ_fonc_fonction_txyz_morceaux_Tru,
  champ_fonc_interp_Tru,
  champ_fonc_med_Tru,
  champ_fonc_med_table_temps_Tru,
  champ_fonc_med_tabule_Tru,
  champ_fonc_reprise_Tru,
  champ_fonc_t_Tru,
  champ_fonc_tabule_Tru,
  champ_fonc_tabule_morceaux_Tru,
  champ_fonc_tabule_morceaux_interp_Tru,
  champ_front_bruite_Tru,
  champ_front_calc_Tru,
  champ_front_composite_Tru,
  champ_front_contact_vef_Tru,
  champ_front_debit_Tru,
  champ_front_debit_massique_Tru,
  champ_front_debit_qc_vdf_Tru,
  champ_front_debit_qc_vdf_fonc_t_Tru,
  champ_front_fonc_pois_ipsn_Tru,
  champ_front_fonc_pois_tube_Tru,
  champ_front_fonc_t_Tru,
  champ_front_fonc_txyz_Tru,
  champ_front_fonc_xyz_Tru,
  champ_front_fonction_Tru,
  champ_front_lu_Tru,
  champ_front_med_Tru,
  champ_front_musig_Tru,
  champ_front_normal_vef_Tru,
  champ_front_pression_from_u_Tru,
  champ_front_recyclage_Tru,
  champ_front_tabule_Tru,
  champ_front_tabule_lu_Tru,
  champ_front_tangentiel_vef_Tru,
  champ_front_uniforme_Tru,
  champ_front_xyz_debit_Tru,
  champ_front_xyz_tabule_Tru,
  champ_generique_base_Tru,
  champ_init_canal_sinal_Tru,
  champ_input_base_Tru,
  champ_input_p0_Tru,
  champ_input_p0_composite_Tru,
  champ_musig_Tru,
  champ_ostwald_Tru,
  champ_post_de_champs_post_Tru,
  champ_post_operateur_base_Tru,
  champ_post_operateur_eqn_Tru,
  champ_post_statistiques_base_Tru,
  champ_som_lu_vdf_Tru,
  champ_som_lu_vef_Tru,
  champ_tabule_temps_Tru,
  champ_uniforme_morceaux_Tru,
  champ_uniforme_morceaux_tabule_temps_Tru,
  champs_a_post_Tru,
  champs_posts_Tru,
  chimie_Tru,
  chmoy_faceperio_Tru,
  cholesky_Tru,
  circle_3_Tru,
  circle_Tru,
  class_generic_Tru,
  coarsen_operator_uniform_Tru,
  coarsen_operators_Tru,
  comment_Tru,
  condinit_Tru,
  condinits_Tru,
  condlim_base_Tru,
  condlimlu_Tru,
  condlims_Tru,
  conduction_Tru,
  constituant_Tru,
  convection_ale_Tru,
  convection_amont_Tru,
  convection_amont_old_Tru,
  convection_btd_Tru,
  convection_centre4_Tru,
  convection_centre_Tru,
  convection_centre_old_Tru,
  convection_deriv_Tru,
  convection_di_l2_Tru,
  convection_diffusion_chaleur_qc_Tru,
  convection_diffusion_chaleur_turbulent_qc_Tru,
  convection_diffusion_chaleur_wc_Tru,
  convection_diffusion_concentration_Tru,
  convection_diffusion_concentration_turbulent_Tru,
  convection_diffusion_espece_binaire_qc_Tru,
  convection_diffusion_espece_binaire_turbulent_qc_Tru,
  convection_diffusion_espece_binaire_wc_Tru,
  convection_diffusion_espece_multi_qc_Tru,
  convection_diffusion_espece_multi_turbulent_qc_Tru,
  convection_diffusion_espece_multi_wc_Tru,
  convection_diffusion_temperature_Tru,
  convection_diffusion_temperature_turbulent_Tru,
  convection_ef_Tru,
  convection_ef_stab_Tru,
  convection_generic_Tru,
  convection_kquick_Tru,
  convection_muscl3_Tru,
  convection_muscl_Tru,
  convection_muscl_new_Tru,
  convection_muscl_old_Tru,
  convection_negligeable_Tru,
  convection_quick_Tru,
  convection_supg_Tru,
  coriolis_Tru,
  corps_postraitement_Tru,
  correlation_Tru,
  corriger_frontiere_periodique_Tru,
  coupled_problem_Tru,
  create_domain_from_sous_zone_Tru,
  create_domain_from_sub_domain_Tru,
  criteres_convergence_Tru,
  darcy_Tru,
  debog_Tru,
  debut_bloc_Tru,
  decoupebord_pour_rayonnement_Tru,
  decouper_bord_coincident_Tru,
  defbord_3_Tru,
  defbord_Tru,
  definition_champ_Tru,
  definition_champs_Tru,
  definition_champs_fichier_Tru,
  deuxentiers_Tru,
  deuxmots_Tru,
  diffusion_deriv_Tru,
  diffusion_negligeable_Tru,
  diffusion_option_Tru,
  diffusion_p1ncp1b_Tru,
  diffusion_stab_Tru,
  diffusion_standard_Tru,
  difusion_p1b_Tru,
  dilate_Tru,
  dimension_Tru,
  dirac_Tru,
  dirichlet_Tru,
  disable_tu_Tru,
  discretisation_base_Tru,
  discretiser_domaine_Tru,
  discretize_Tru,
  distance_paroi_Tru,
  divergence_Tru,
  domain_Tru,
  domaine_Tru,
  domaineaxi1d_Tru,
  dp_impose_Tru,
  dt_calc_dt_calc_Tru,
  dt_calc_dt_fixe_Tru,
  dt_calc_dt_min_Tru,
  dt_impr_ustar_mean_only_Tru,
  dt_start_Tru,
  ec_Tru,
  ecart_type_Tru,
  echange_couplage_thermique_Tru,
  echange_interne_global_impose_Tru,
  echange_interne_global_parfait_Tru,
  echange_interne_impose_Tru,
  echange_interne_parfait_Tru,
  echelle_temporelle_turbulente_Tru,
  ecrire_champ_med_Tru,
  ecrire_fichier_formatte_Tru,
  ecrire_fichier_xyz_valeur_param_Tru,
  ecrire_med_Tru,
  ecriturelecturespecial_Tru,
  ef_Tru,
  energie_cinetique_turbulente_Tru,
  energie_cinetique_turbulente_wit_Tru,
  energie_multiphase_Tru,
  entierfloat_Tru,
  entree_temperature_imposee_h_Tru,
  epsilon_Tru,
  eqn_base_Tru,
  espece_Tru,
  euler_scheme_Tru,
  execute_parallel_Tru,
  export_Tru,
  extract_2d_from_3d_Tru,
  extract_2daxi_from_3d_Tru,
  extraction_Tru,
  extraire_domaine_Tru,
  extraire_plan_Tru,
  extraire_surface_Tru,
  extrudebord_Tru,
  extrudeparoi_Tru,
  extruder_Tru,
  extruder_en20_Tru,
  extruder_en3_Tru,
  field_base_Tru,
  field_func_txyz_Tru,
  field_func_xyz_Tru,
  fin_bloc_Tru,
  floatfloat_Tru,
  fluide_base_Tru,
  fluide_dilatable_base_Tru,
  fluide_incompressible_Tru,
  fluide_ostwald_Tru,
  fluide_quasi_compressible_Tru,
  fluide_reel_base_Tru,
  fluide_sodium_gaz_Tru,
  fluide_sodium_liquide_Tru,
  fluide_stiffened_gas_Tru,
  fluide_weakly_compressible_Tru,
  flux_interfacial_Tru,
  fonction_champ_reprise_Tru,
  forchheimer_Tru,
  form_a_nb_points_Tru,
  format_file_Tru,
  format_lata_to_med_Tru,
  fourfloat_Tru,
  front_field_base_Tru,
  frontiere_ouverte_Tru,
  frontiere_ouverte_concentration_imposee_Tru,
  frontiere_ouverte_fraction_massique_imposee_Tru,
  frontiere_ouverte_gradient_pression_impose_Tru,
  frontiere_ouverte_gradient_pression_impose_vefprep1b_Tru,
  frontiere_ouverte_gradient_pression_libre_vef_Tru,
  frontiere_ouverte_gradient_pression_libre_vefprep1b_Tru,
  frontiere_ouverte_pression_imposee_Tru,
  frontiere_ouverte_pression_imposee_orlansky_Tru,
  frontiere_ouverte_pression_moyenne_imposee_Tru,
  frontiere_ouverte_rho_u_impose_Tru,
  frontiere_ouverte_temperature_imposee_Tru,
  frontiere_ouverte_vitesse_imposee_Tru,
  frontiere_ouverte_vitesse_imposee_sortie_Tru,
  frottement_interfacial_Tru,
  gcp_ns_Tru,
  gen_Tru,
  gmres_Tru,
  gradient_Tru,
  ice_Tru,
  ijk_grid_geometry_Tru,
  ijk_splitting_Tru,
  ilu_Tru,
  implicite_Tru,
  imprimer_flux_Tru,
  imprimer_flux_sum_Tru,
  info_med_Tru,
  init_par_partie_Tru,
  integrer_champ_med_Tru,
  internes_Tru,
  interpolation_Tru,
  interpolation_ibm_aucune_Tru,
  interpolation_ibm_base_Tru,
  interpolation_ibm_elem_fluid_Tru,
  interpolation_ibm_hybride_Tru,
  interpolation_ibm_mean_gradient_Tru,
  interpolation_ibm_power_law_tbl_Tru,
  interpolation_ibm_power_law_tbl_u_star_Tru,
  interprete_geometrique_base_Tru,
  lata_to_med_Tru,
  lata_to_other_Tru,
  leap_frog_Tru,
  lecture_bloc_moment_base_Tru,
  lire_ideas_Tru,
  list_bloc_mailler_Tru,
  list_bord_Tru,
  list_info_med_Tru,
  list_list_nom_Tru,
  list_nom_Tru,
  list_nom_virgule_Tru,
  list_stat_post_Tru,
  list_un_pb_Tru,
  listchamp_generique_Tru,
  listdeuxmots_sacc_Tru,
  liste_mil_Tru,
  liste_post_Tru,
  liste_post_ok_Tru,
  liste_sonde_tble_Tru,
  listeqn_Tru,
  listobj_Tru,
  listpoints_Tru,
  listsous_zone_valeur_Tru,
  loi_etat_base_Tru,
  loi_etat_gaz_parfait_base_Tru,
  loi_etat_gaz_reel_base_Tru,
  loi_fermeture_base_Tru,
  loi_fermeture_test_Tru,
  loi_horaire_Tru,
  longitudinale_Tru,
  mailler_Tru,
  mailler_base_Tru,
  maillerparallel_Tru,
  masse_multiphase_Tru,
  merge_med_Tru,
  methode_loi_horaire_Tru,
  methode_transport_deriv_Tru,
  milieu_base_Tru,
  modele_turbulence_hyd_deriv_Tru,
  modele_turbulence_hyd_nul_Tru,
  modele_turbulence_scal_base_Tru,
  modele_turbulence_scal_nul_Tru,
  modif_bord_to_raccord_Tru,
  modifydomaineaxi1d_Tru,
  mor_eqn_Tru,
  morceau_equation_Tru,
  moyenne_Tru,
  moyenne_volumique_Tru,
  multi_gaz_parfait_qc_Tru,
  multi_gaz_parfait_wc_Tru,
  multigrid_solver_Tru,
  multiplefiles_Tru,
  navier_stokes_qc_Tru,
  navier_stokes_standard_Tru,
  navier_stokes_turbulent_Tru,
  navier_stokes_turbulent_qc_Tru,
  navier_stokes_wc_Tru,
  nettoiepasnoeuds_Tru,
  neumann_Tru,
  neumann_homogene_Tru,
  neumann_paroi_adiabatique_Tru,
  nom_Tru,
  nom_anonyme_Tru,
  nom_postraitement_Tru,
  numero_elem_sur_maitre_Tru,
  objet_lecture_Tru,
  op_conv_ef_stab_polymac_face_Tru,
  op_conv_ef_stab_polymac_p0_face_Tru,
  op_conv_ef_stab_polymac_p0p1nc_elem_Tru,
  op_conv_ef_stab_polymac_p0p1nc_face_Tru,
  op_implicite_Tru,
  optimal_Tru,
  option_polymac_Tru,
  option_polymac_p0_Tru,
  option_vdf_Tru,
  orientefacesbord_Tru,
  parallel_io_parameters_Tru,
  parametre_diffusion_implicite_Tru,
  parametre_equation_base_Tru,
  parametre_implicite_Tru,
  paroi_Tru,
  paroi_adiabatique_Tru,
  paroi_contact_Tru,
  paroi_contact_fictif_Tru,
  paroi_decalee_robin_Tru,
  paroi_defilante_Tru,
  paroi_echange_contact_correlation_vdf_Tru,
  paroi_echange_contact_correlation_vef_Tru,
  paroi_echange_contact_vdf_Tru,
  paroi_echange_externe_impose_Tru,
  paroi_echange_externe_impose_h_Tru,
  paroi_echange_global_impose_Tru,
  paroi_fixe_Tru,
  paroi_fixe_iso_genepi2_sans_contribution_aux_vitesses_sommets_Tru,
  paroi_flux_impose_Tru,
  paroi_knudsen_non_negligeable_Tru,
  paroi_temperature_imposee_Tru,
  partition_Tru,
  partition_multi_Tru,
  partitionneur_deriv_Tru,
  partitionneur_fichier_decoupage_Tru,
  partitionneur_fichier_med_Tru,
  partitionneur_metis_Tru,
  partitionneur_partition_Tru,
  partitionneur_sous_dom_Tru,
  partitionneur_sous_domaines_Tru,
  partitionneur_sous_zones_Tru,
  partitionneur_tranche_Tru,
  partitionneur_union_Tru,
  pave_Tru,
  pb_avec_passif_Tru,
  pb_base_Tru,
  pb_conduction_Tru,
  pb_gen_base_Tru,
  pb_hydraulique_Tru,
  pb_hydraulique_concentration_Tru,
  pb_hydraulique_concentration_scalaires_passifs_Tru,
  pb_hydraulique_concentration_turbulent_Tru,
  pb_hydraulique_concentration_turbulent_scalaires_passifs_Tru,
  pb_hydraulique_melange_binaire_qc_Tru,
  pb_hydraulique_melange_binaire_turbulent_qc_Tru,
  pb_hydraulique_melange_binaire_wc_Tru,
  pb_hydraulique_turbulent_Tru,
  pb_multiphase_Tru,
  pb_multiphase_hem_Tru,
  pb_post_Tru,
  pb_thermohydraulique_Tru,
  pb_thermohydraulique_concentration_Tru,
  pb_thermohydraulique_concentration_scalaires_passifs_Tru,
  pb_thermohydraulique_concentration_turbulent_Tru,
  pb_thermohydraulique_concentration_turbulent_scalaires_passifs_Tru,
  pb_thermohydraulique_especes_qc_Tru,
  pb_thermohydraulique_especes_turbulent_qc_Tru,
  pb_thermohydraulique_especes_wc_Tru,
  pb_thermohydraulique_qc_Tru,
  pb_thermohydraulique_scalaires_passifs_Tru,
  pb_thermohydraulique_turbulent_Tru,
  pb_thermohydraulique_turbulent_qc_Tru,
  pb_thermohydraulique_turbulent_scalaires_passifs_Tru,
  pb_thermohydraulique_wc_Tru,
  pbc_med_Tru,
  penalisation_l2_ftd_Tru,
  penalisation_l2_ftd_lec_Tru,
  perfect_gaz_qc_Tru,
  perfect_gaz_wc_Tru,
  periodic_Tru,
  perte_charge_anisotrope_Tru,
  perte_charge_circulaire_Tru,
  perte_charge_directionnelle_Tru,
  perte_charge_isotrope_Tru,
  perte_charge_reguliere_Tru,
  perte_charge_singuliere_Tru,
  petsc_Tru,
  pilote_icoco_Tru,
  piso_Tru,
  plan_Tru,
  point_Tru,
  points_Tru,
  polyedriser_Tru,
  polymac_Tru,
  polymac_p0_Tru,
  polymac_p0p1nc_Tru,
  porosites_Tru,
  position_like_Tru,
  postraitement_Tru,
  postraitement_base_Tru,
  postraitements_Tru,
  postraiter_domaine_Tru,
  precisiongeom_Tru,
  precond_base_Tru,
  precondsolv_Tru,
  predefini_Tru,
  problem_read_generic_Tru,
  puissance_thermique_Tru,
  qdm_multiphase_Tru,
  raccord_Tru,
  radioactive_decay_Tru,
  radius_Tru,
  raffiner_anisotrope_Tru,
  raffiner_isotrope_Tru,
  raffiner_isotrope_parallele_Tru,
  reaction_Tru,
  reactions_Tru,
  read_Tru,
  read_file_Tru,
  read_file_bin_Tru,
  read_med_Tru,
  read_tgrid_Tru,
  read_unsupported_ascii_file_from_icem_Tru,
  rectify_mesh_Tru,
  redresser_hexaedres_vdf_Tru,
  reduction_0d_Tru,
  refchamp_Tru,
  refine_mesh_Tru,
  regroupebord_Tru,
  remove_elem_Tru,
  remove_elem_bloc_Tru,
  remove_invalid_internal_boundaries_Tru,
  reorienter_tetraedres_Tru,
  reorienter_triangles_Tru,
  resequencing_Tru,
  residuals_Tru,
  rhot_gaz_parfait_qc_Tru,
  rhot_gaz_reel_qc_Tru,
  rocalution_Tru,
  rotation_Tru,
  runge_kutta_ordre_2_Tru,
  runge_kutta_ordre_2_classique_Tru,
  runge_kutta_ordre_3_Tru,
  runge_kutta_ordre_3_classique_Tru,
  runge_kutta_ordre_4_Tru,
  runge_kutta_ordre_4_classique_3_8_Tru,
  runge_kutta_ordre_4_classique_Tru,
  runge_kutta_rationnel_ordre_2_Tru,
  saturation_base_Tru,
  saturation_constant_Tru,
  saturation_sodium_Tru,
  scalaire_impose_paroi_Tru,
  scatter_Tru,
  scattermed_Tru,
  sch_cn_ex_iteratif_Tru,
  sch_cn_iteratif_Tru,
  schema_adams_bashforth_order_2_Tru,
  schema_adams_bashforth_order_3_Tru,
  schema_adams_moulton_order_2_Tru,
  schema_adams_moulton_order_3_Tru,
  schema_backward_differentiation_order_2_Tru,
  schema_backward_differentiation_order_3_Tru,
  schema_euler_implicite_Tru,
  schema_implicite_base_Tru,
  schema_predictor_corrector_Tru,
  schema_temps_base_Tru,
  segment_Tru,
  segmentfacesx_Tru,
  segmentfacesy_Tru,
  segmentfacesz_Tru,
  segmentpoints_Tru,
  sets_Tru,
  simple_Tru,
  simpler_Tru,
  solide_Tru,
  solv_gcp_Tru,
  solve_Tru,
  solveur_implicite_base_Tru,
  solveur_lineaire_std_Tru,
  solveur_sys_base_Tru,
  solveur_u_p_Tru,
  sonde_Tru,
  sonde_base_Tru,
  sonde_tble_Tru,
  sondes_Tru,
  sondes_fichier_Tru,
  sortie_libre_temperature_imposee_h_Tru,
  source_base_Tru,
  source_constituant_Tru,
  source_generique_Tru,
  source_pdf_Tru,
  source_pdf_base_Tru,
  source_qdm_Tru,
  source_qdm_lambdaup_Tru,
  source_robin_Tru,
  source_robin_scalaire_Tru,
  source_th_tdivu_Tru,
  source_travail_pression_elem_base_Tru,
  sources_Tru,
  sous_zone_Tru,
  sous_zone_valeur_Tru,
  spec_pdcr_base_Tru,
  ssor_Tru,
  ssor_bloc_Tru,
  stat_post_correlation_Tru,
  stat_post_deriv_Tru,
  stat_post_ecart_type_Tru,
  stat_post_moyenne_Tru,
  stat_post_t_deb_Tru,
  stat_post_t_fin_Tru,
  stats_posts_Tru,
  stats_serie_posts_Tru,
  supprime_bord_Tru,
  symetrie_Tru,
  system_Tru,
  taux_dissipation_turbulent_Tru,
  tayl_green_Tru,
  temperature_Tru,
  temperature_imposee_paroi_Tru,
  terme_puissance_thermique_echange_impose_Tru,
  test_solveur_Tru,
  test_sse_kernels_Tru,
  testeur_Tru,
  testeur_medcoupling_Tru,
  tetraedriser_Tru,
  tetraedriser_homogene_Tru,
  tetraedriser_homogene_compact_Tru,
  tetraedriser_homogene_fin_Tru,
  tetraedriser_par_prisme_Tru,
  thi_Tru,
  tparoi_vef_Tru,
  traitement_particulier_Tru,
  traitement_particulier_base_Tru,
  transformation_Tru,
  transformer_Tru,
  transversale_Tru,
  travail_pression_Tru,
  triangulate_Tru,
  trianguler_fin_Tru,
  trianguler_h_Tru,
  troisf_Tru,
  troismots_Tru,
  turbulence_paroi_base_Tru,
  turbulence_paroi_scalaire_base_Tru,
  twofloat_Tru,
  type_postraitement_ft_lata_Tru,
  type_un_post_Tru,
  un_pb_Tru,
  un_point_Tru,
  un_postraitement_Tru,
  un_postraitement_spec_Tru,
  uniform_field_Tru,
  valeur_totale_sur_volume_Tru,
  vdf_Tru,
  vect_nom_Tru,
  vef_Tru,
  verifier_qualite_raffinements_Tru,
  verifier_simplexes_Tru,
  verifiercoin_Tru,
  verifiercoin_bloc_Tru,
  vitesse_derive_base_Tru,
  vitesse_relative_base_Tru,
  volume_Tru,
  write_Tru,
  write_file_Tru,
  ########## self._automatic_classes,
  chaine_db4169_Tru,
  entier_3282e0_Tru,
  floattant_820d8b_Tru,
  chaine_a96986_Tru,
  chaine_7d729e_Tru,
  chaine_824559_Tru,
  chaine_38aea9_Tru,
  listf_Tru,
  entier_6f2a33_Tru,
  rien_Tru,
  chaine_deccae_Tru,
  listchaine_Tru,
  ref_domaine_Tru,
  chaine_10bcdf_Tru,
  chaine_0bf8d2_Tru,
  chaine_e3346e_Tru,
  chaine_1feca2_Tru,
  ref_pb_gen_base_Tru,
  ref_discretisation_base_Tru,
  chaine_ad85af_Tru,
  chaine_52b4a3_Tru,
  chaine_4aae18_Tru,
  chaine_036e4c_Tru,
  chaine_241470_Tru,
  chaine_43a355_Tru,
  chaine_c39562_Tru,
  listentier_Tru,
  entier_8bf71a_Tru,
  entier_216c6a_Tru,
  listchainef_Tru,
  ref_pb_base_Tru,
  chaine_f0532c_Tru,
  chaine_e3a651_Tru,
  chaine_af6447_Tru,
  chaine_a28892_Tru,
  chaine_2a7c96_Tru,
  chaine_39eae7_Tru,
  list_Tru,
  chaine_d75b13_Tru,
  chaine_917c90_Tru,
  entier_a44528_Tru,
  listentierf_Tru,
  chaine_36f0dc_Tru,
  entier_6f194b_Tru,
  entier_fd157e_Tru,
  entier_b08690_Tru,
  chaine_f07ecc_Tru,
  ref_sous_zone_Tru,
  chaine_a32a8b_Tru,
  chaine_a7556e_Tru,
  chaine_1d8e3b_Tru,
  chaine_08814c_Tru,
  chaine_56e2aa_Tru,
  chaine_8c4850_Tru,
  chaine_1ca9d5_Tru,
  chaine_d74614_Tru,
  chaine_13672b_Tru,
  chaine_7347ec_Tru,
  chaine_ba189d_Tru,
  chaine_7956cf_Tru,
  chaine_437a7b_Tru,
  ref_loi_horaire_Tru,
  ref_champ_fonc_med_Tru,
  chaine_5dc32f_Tru,
  chaine_e80920_Tru,
  chaine_a3dc34_Tru,
  chaine_1691a8_Tru,
  chaine_c002ae_Tru,
  entier_6d15af_Tru,
  chaine_84289f_Tru,
  chaine_588727_Tru,
  chaine_d249f7_Tru,
  chaine_910cb5_Tru,
  entier_5e32c8_Tru,
  chaine_ae5ad3_Tru,
  chaine_6cbaef_Tru,
  chaine_cddc57_Tru,
  chaine_8faf53_Tru,
  chaine_e7fecf_Tru,
  chaine_48a9fa_Tru,
  chaine_d2dddb_Tru,
  chaine_e57e1c_Tru,
  ref_field_base_Tru,
  chaine_45470e_Tru,
  chaine_8b7047_Tru,
  chaine_dbffd3_Tru,
  chaine_41af74_Tru,
  chaine_c64c02_Tru,
  chaine_c49d25_Tru,
  ref_ijk_grid_geometry_Tru,
  chaine_ad7267_Tru,
  chaine_6c41af_Tru,
  chaine_7e6e04_Tru,
  chaine_23652b_Tru,
  chaine_86c49e_Tru,
  chaine_269f2a_Tru,
  chaine_96adb9_Tru,
  chaine_385ae2_Tru,
  chaine_c53c45_Tru
]

# needs explicit class factory appends for *_generated_*.py, as precaution
# avoid automatic as implicit
# initXyzClasses()  # avoid this automatic as implicit

  
# number of classes: 621
# number of automatic classes: 99
# number of automatic identical classes: 25

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


# Some lists must read their size upfront, some not ... I can not figure out a proper
# rule for this ...
try:
  listpoints_Tru._read_size = True
  vect_nom_Tru._read_size = True
except:
  pass

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
  listeqn_Tru._read_size = False
  listeqn_Tru.toDatasetTokens = toDSToken_hack
except:
  pass
