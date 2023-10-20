# -*- coding: utf-8 -*-

# %% LICENSE_SALOME_CEA_BEGIN
# see trustpy/LICENCE file
# %% LICENSE_END

"""
Reference data for test
"""

def buildCurlyExpec(mod, bidon=True):
  """ Reference for:

    '# Some stupid test #
    read_MED_bidon 
      {
         mesh ze_mesh_name
         file a/complicated/path/to.med
         exclude_groups 2 toto titi
         convertAllToPoly
      }'

  """
  if bidon:
    LM = mod.read_med_bidon_Tru
  else:
    LM = mod.read_med_Tru
  BC = mod.BaseChaine_Tru
  exp = LM()
  exp.mesh, exp.file = "ze_mesh_name", "a/complicated/path/to.med"
  exp.exclude_groups = mod.ListOfChaine_Tru()
  exp.exclude_groups.extend([BC("toto"), BC("titi")])
  exp.convertalltopoly = True
  return exp

def buildForwardExpec(mod):
  """ Reference data for
  
    lire_MED_bidon   rmed
    coucou cb
    
    Read  rmed 
      {
         mesh ze_mesh_name
         file a/complicated/path/to.med
      }
    
    read    cb {   }

  """
  # First and 2nd item are forward declarations
  FD, BC = mod.Declaration_Tru, mod.BaseChaine_Tru
  LM = mod.read_med_bidon_Tru
  exp0 = FD(); exp0.cls_nam = "lire_med_bidon_Tru"; exp0.identifier = "rmed"
  exp1 = FD(); exp1.cls_nam = "coucou_Tru"; exp1.identifier = "cb"
  exp2 = LM()
  exp2.mesh, exp2.file = "ze_mesh_name", "a/complicated/path/to.med"
  exp3 = mod.coucou_Tru()
  return exp0, exp1, exp2, exp3

def buildMinimalExpec(mod):
  """  Reference data for 
  
  '    # Some stupid test #
    champ_uniforme gravite 
    nom coucou
    read gravite 2 28 32   # Keyword with no brace #
    read coucou toto
    lire_MED
      {
         mesh ze_mesh_name
         file a/complicated/path/to.med
         exclude_groups 2 toto titi
         convertAllToPoly
      }'
    
  """
  FD = mod.Declaration_Tru
  UF, LT, BF, N = mod.uniform_field_Tru, mod.list_Tru, mod.BaseFloattant_Tru, mod.nom_Tru
  # First and 2nd item are forward declarations:
  exp0 = FD(); exp0.cls_nam = "champ_uniforme_Tru"; exp0.identifier = "gravite"
  exp1 = FD(); exp1.cls_nam = "nom_Tru"; exp1.identifier = "coucou"
  # Second item reading of the uniform field:
  exp2, l = UF(), LT()
  l.extend([BF(28), BF(32)])
  exp2.val = l
  # Third item is a nom:
  exp3 = N()
  exp3.mot = "toto"
  # Fourth item is a read_med:
  exp4 = buildCurlyExpec(mod, bidon=False)
  return [exp0, exp1, exp2, exp3, exp4]

def buildSondesExpec(mod):
  """ Reference data for
    sonde_pression pression periode 0.005 points 2 0.13 0.105 0.13 0.115
  """
  S, PS, P, LF, LP = mod.sonde_Tru, mod.points_Tru, mod.un_point_Tru, mod.ListOfFloatDim_Tru, mod.listpoints_Tru
  exp = S()
  exp.mperiode = "periode"
  exp.nom_inco = "pression"
  exp.nom_sonde = "sonde_pression"
  exp.prd = 0.005
  ps = PS()
  pt1, pt2 = P(), P()
  pt1.pos = LF([0.13, 0.105])
  pt2.pos = LF([0.13, 0.115])
  listpt = LP()
  listpt.extend([pt1, pt2])
  ps.points = listpt
  exp.type = ps
  return exp

def buildBordExpec(mod):
  """ Reference data for
      Bord periox X = 0.  0. <= Y <= 2.0
  """
  B, DB = mod.bord_Tru, mod.defbord_2_Tru
  exp = B()
  exp.nom = "periox"
  db = DB()
  db.edge_1 = "X = 0."
  db.edge_2 = "0. <= Y <= 2.0"
  exp.defbord = db
  return exp

def buildSolveurPressionExpec(mod):
  """ Reference data for
      petsc cholesky {   nImp c est un blOc leCtuRe }
  """
  P, BL = mod.petsc_Tru, mod.bloc_lecture_Tru
  p = P()
  b = BL()
  # Note the space to start with:
  b.bloc_lecture = " {   nImp c est un blOc leCtuRe }"
  p.option_solveur = b
  p.solveur = "cholesky"
  return p
