"""
Reference data for tests
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
    LM = mod.Read_med_bidon
  else:
    LM = mod.Read_med
  exp = LM()
  exp.mesh, exp.file = "ze_mesh_name", "a/complicated/path/to.med"
  exp.exclude_groups = ["toto", "titi"]
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
  from trustify.misc_utilities import ClassFactory
  # First and 2nd item are forward declarations
  exp0 = mod.Declaration()
  exp0.ze_type = ClassFactory.GetPydClassFromName("Read_med_bidon")
  exp0.identifier = "rmed"
  exp1 = mod.Declaration()
  exp1.ze_type = ClassFactory.GetPydClassFromName("Coucou")
  exp1.identifier = "cb"
  exp2 = mod.Read()
  exp2.identifier = "rmed"
  lm = mod.Read_med_bidon()
  lm.mesh, lm.file = "ze_mesh_name", "a/complicated/path/to.med"
  exp2.obj = lm
  exp3 = mod.Read()
  exp3.identifier = "cb"
  exp3.obj = mod.Coucou()
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
  FD = mod.Declaration
  UF, N, RD = mod.Uniform_field, mod.Nom, mod.Read
  # First and 2nd item are forward declarations:
  exp0 = FD(); exp0.ze_type = UF; exp0.identifier = "gravite"
  exp1 = FD(); exp1.ze_type = N; exp1.identifier = "coucou"
  # Second item reading of the uniform field:
  u = UF(); u.val = [28, 32]
  exp2 = RD(); exp2.identifier = "gravite"; exp2.obj = u
  # Third item is a nom:
  n = N(); n.mot = "toto"
  exp3 = RD(); exp3.identifier = "coucou"; exp3.obj = n
  # Fourth item is a read_med:
  exp4 = buildCurlyExpec(mod, bidon=False)
  return [exp0, exp1, exp2, exp3, exp4]

def buildSondesExpec(mod):
  """ Reference data for
    sonde_pression pression periode 0.005 points 2 0.13 0.105 0.13 0.115
  """
  S, PS, P = mod.Sonde, mod.Points, mod.Un_point
  exp = S()
  exp.mperiode = "periode"
  exp.nom_inco = "pression"
  exp.nom_sonde = "sonde_pression"
  exp.prd = 0.005
  ps = PS()
  pt1, pt2 = P(), P()
  pt1.pos = [0.13, 0.105]
  pt2.pos = [0.13, 0.115]
  ps.points = [pt1, pt2]
  exp.type = ps
  return exp

def buildBordExpec(mod):
  """ Reference data for
      Bord periox X = 0.  0. <= Y <= 2.0
  """
  B, DB = mod.Bord, mod.Defbord_2
  exp = B()
  exp.nom = "periox"
  db = DB()
  db.dir, db.eq, db.pos = "x", "=", 0.
  db.pos2_min, db.inf1, db.dir2, db.inf2, db.pos2_max = 0., "<=", "y", "<=", 2.
  exp.defbord = db
  return exp

def buildSolveurPressionExpec(mod):
  """ Reference data for
      petsc cholesky {   nImp c est un blOc leCtuRe }
  """
  P, BL = mod.Petsc, mod.Bloc_lecture
  p = P()
  b = BL()
  # Note the space to start with:
  b.bloc_lecture = " {   nImp c est un blOc leCtuRe }"
  p.option_solveur = b
  p.solveur = "cholesky"
  return p
