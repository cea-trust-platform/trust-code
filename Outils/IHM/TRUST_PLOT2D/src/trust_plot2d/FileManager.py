import TrustFiles

class FileManager(object):
  """ Class caching the TrustFiles """
  def __init__(self, testMode=False):
    self._testMode = testMode
    self._cache = {}
    
  def getFileObject(self, fullPath, refPath):
    import os
    if not os.path.isfile(fullPath):
      return None, ""
    shortName = os.path.split(fullPath)[1]
    if not self._cache.has_key(fullPath):
      self.__addFile(fullPath, refPath)
    return self._cache[fullPath], shortName
    
  def __addFile(self, fName, refPath):
    """ Add file object to the internal cache
    """
    if not self._testMode:
      obj = TrustFiles.BuildFromPath(fName, refPath)
    else:
      with open(fName) as f:
        obj = id(f)
    self._cache[fName] = obj  
        
