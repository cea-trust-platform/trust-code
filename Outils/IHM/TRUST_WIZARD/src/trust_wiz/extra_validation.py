#
# Author : A. Bruneton
#

import os
import MEDLoader

def getMCTypeMap():
  """ Helper function returning mapping between the MEDCoupling type enum and human-readable names.  
    @return a dict of (int -> str) , like {3: NORM_TRI3} 
  """
  import MEDCoupling as mc
  return dict( (v, k) for k,v in mc.__dict__.items() if k[:5] == "NORM_" )

class TrustExtraValidation(object):
  """ Extra validation rules for a Trust dataset.
  All methods must return a string, which is void if no error was spotted. 
  Methods validating arrays (like velocity) must accept both the full array (useful to check its length).
  """  
  def __init__(self):
    self._filePath = ""
    self._meshName = ""
    self._dim = -1
    self._groups = []
    self._geotypes = []
  
  def validateNonVoidObj(self, json_bit):
    if len(json_bit) == 0:
      return "Empty entry, or entry set to 'None'! Make a choice!"
    return ""
  
  def validateNonNone(self, json_bit):
    if json_bit is None or json_bit == "None":
      return "Entry set to 'None'! Make a choice!"
    return ""
  
  def validateFile(self, json_bit):
    filePath = json_bit  # data is a simple string here
    if not os.path.isfile(filePath):
      return "Non existing file: '%s'!" % filePath
    self._filePath = str(filePath)
    return ""
  
  def validateMesh(self, json_bit):
    self._meshName = str(json_bit)
    try:
      mesh = MEDLoader.ReadMeshFromFile(self._filePath, self._meshName)
      self._dim = mesh.getCoords().getNumberOfComponents()
      self._groups = MEDLoader.GetMeshGroupsNames(self._filePath, self._meshName)
      mc_typ_map = getMCTypeMap()
      self._geotypes = [ mc_typ_map[t] for t in mesh.getAllGeoTypes() ]
    except:
      return "Could not read mesh '%s' in file '%s'" % (self._meshName, self._filePath)
    return ""

  def validateDim(self, json_bit):
    if self._dim != json_bit:
      return "Invalid mesh dimension %d (found %d)!" % (json_bit, self._dim)
    return ""

  def validateGeoTypes(self, json_bit):
    geotypes = json_bit
    if set(geotypes) != set(self._geotypes):
      return ( "Incoherent geometric types: %s is not what is found in mesh " % str(geotypes) + 
               "'%s' (found: %s)" % (self._meshName, str(self._geotypes))  )
    return ""

  def validateBC(self, json_bit):
    if not isinstance(json_bit, list):
      return "Not a list??? Should not happen"
    for l in json_bit:
      if not len(l):
        return "Some boundary conditions are not set!"
    return ""
  
  def validateVelocity(self, json_bit):
    if not isinstance(json_bit, list):
      return ""  # Could check type but JSon will do it too
    if len(json_bit) != self._dim:
      return "Invalid number of components in velocity!"
    return ""
  
