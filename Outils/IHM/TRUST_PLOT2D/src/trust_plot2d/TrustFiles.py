import numpy as np

class TrustFile(object):
  """ Parsing and value extraction from a Trust file. Assumption: all valid (=completly written) line
  of data terminate with a new line char """
  
  _VOID_ARR = np.array([], dtype=np.float32)
  
  def __init__(self, filePath, refFilePath):
    """ 
    @param filePath Trust file to parse
    @param refFilePath file serving as a reference to know if the main file has been reset. This 
    is typically the stop file. Can be None for test purposes (in this case only size of the main file
    will be inspected).
    """
    self._filePath = filePath
    self._refPath = refFilePath
    self._lastSize = -1
    self._lastRefDate = -1
    self._reset()     
      
  def _reset(self):
    self._entries = {}                # key = entry_name, val=col_index (or component numb for segs)
    self._taxis = TrustFile._VOID_ARR # time axis values
    self._yvalues = TrustFile._VOID_ARR           # a np.array, col index is in _entries
    self._lastIndex = {}              # position of the last fetched data in the yvalues arrays. key=entry_name, vals=index
    self._lastSeek = 0                # position of the last fetched data in the file
    self._data = ""                   # last chunk of read data
    self._ncols = 0                   # numb of cols in the data
  
  def isLogY(self):
    return False
  
  def getXLabel(self):
    raise NotImplementedError
  
  def getEntries(self):
    """ 
    @return a list of file entries (i.e. what can be ploted from the file)
    """
    status = self._queryAndUpdateStatus(update=False)
    if status == "invalid":
      return []
    if not len(self._entries):
      hdr = self._populateFromHeader()
      self._reloadEntries(hdr)
    return self._entries.keys()
 
  def getValues(self, entryName):
    """
    Get all current valid values from the file.
    @return two np arrays (x and y)
    """
    fileReady = self._getValuesInternal()
    return self._extractValues(fileReady, entryName)
  
  def getNewValues(self, entryName):
    """
    Return values to be appended to the last retrieved ones to get the full set of data.
    Note: after a file reset, it will return None, None on an entryName as long as getValues() hasn't been called once. 
    @return two (potentially void) np arrays (x and y)
    @return None, None if the file has been reset or has become invalid (getValues() must be invoked again on this entry)
    """
    fileReady = self._getValuesInternal()
    return self._extractNewValues(fileReady, entryName)      

  def _getValuesInternal(self):
    """ @return False if file is not ready """ 
    status = self._queryAndUpdateStatus(update=True)
#    print "getValues stat", status, " ", self._filePath
    if status == "invalid":
      return False
    if not len(self._entries):   # if there's been a reset()
      hdr = self._populateFromHeader()
      self._reloadEntries(hdr=hdr)
    # Try to retreive values only if entries are OK:
    if not len(self._entries):
      # File is not ready:
      return False
    if not self._yvalues.shape[0]:
      self._reloadValues()
    elif status != "same":
      self._completeValues()  
    if not self._yvalues.shape[0]:
      # File is not ready:
      return False
    return True

  def _extractValues(self, fileReady, entryName):
    """ Extract values from the internal array """
    if not fileReady:
      # Two void arrays
      return TrustFile._VOID_ARR, TrustFile._VOID_ARR
    else:
      idx = self._taxis.shape[0]
      self._lastIndex[entryName] = idx
      col = self._entries[entryName]
      return self._taxis, self._yvalues[:, col]

  def _extractNewValues(self, fileReady, entryName):
    """ Extract NEW values from the internal array """
    if not fileReady:
      return None, None
    else:
      idx = self._lastIndex[entryName]
      if idx == -1:
        # File was reset previously, and for this entry, getValues() has not been called again yet
        return None, None
      col = self._entries[entryName]
      x, y = self._taxis[idx:], self._yvalues[idx:, col]
      self._lastIndex[entryName] += x.shape[0]
      return x, y
  
  def _populateFromHeader(self, hdr=None):
    """ Fill meta data from file header. If hdr=None, the method should reread the header itself.
    @return hdr if it could be read properly, None otherwise
    """
    return hdr

  def _reloadEntries(self, hdr=None):
    """ Reload entries exposed by a file
    @param hdr if None the header has to be re-read by the method 
    """
    pass

  def _reloadValues(self):
    """ Reload all data (not header) from a file.
    Use numpy.loadtxt for efficiency. """
    self._yvalues = self._readOn()
    if self._yvalues.shape[0]:
      self._taxis = self._yvalues[:,0]
    else:
      self._taxis = TrustFile._VOID_ARR

  def _completeValues(self):
    """ Complete the internal array with the newly read values """
    lines = self._readOn()
    if lines.shape[0]:
      self._taxis = np.append(self._taxis, lines[:,0])
      if self._yvalues.shape[0]:
        self._yvalues = np.vstack([self._yvalues, lines])
      else:
        self._yvalues = lines        
   
  def _queryAndUpdateStatus(self, update=True):    
    """ 
    @param update True if the internal status of the file should be updated
    @return "append" : if new data has been added (or if first query on a valid file)
                "reset"  : if file has been reset
                "invalid": if the file cannot be accessed anymore or is invalid
                "same"   : if the file has not changed 
    """
    import os
    try:
      ret = "invalid"
      if self._refPath is not None:   
        t = os.stat(self._refPath).st_mtime
      else:
        # for test - will emulate a single reset at startup
        t = -2
      if t != self._lastRefDate:
        ret = "reset"
        if update:
          self._lastRefDate = t
          self._reset()
      siz = os.stat(self._filePath).st_size
      if ret == "reset":
        if update:     self._lastSize = siz
        return ret
      if siz < self._lastSize:
        if update:     
          self._lastSize = siz
          self._reset()
        print "WARNING: file is smaller but reference file has not changed! Will reset. (%s)" % self._filePath
        return "reset"
      elif siz == self._lastSize:
        return "same"
      else:
        if update:     
          self._lastSize = siz
        return "append"
    except OSError:
      if update:
        self._lastRefDate = -1
        self._lastSize = -1
        self._reset()
      return "invalid"

  def _parseData(self, data):
    arrPy = data.split("\n")
    remainData, npArr = "", TrustFile._VOID_ARR
    if len(arrPy):
      remainData = arrPy[-1]
    else:
      return remainData, npArr
    i = 0
    # Skip header:
    for a in arrPy[:-1]:
      if a[0] == "#":
        i += 1
      else:
        break
    # No data, just a header:
    if len(arrPy[i:-1]) == 0:
      return remainData, npArr
    arrData = " ".join(arrPy[i:-1])
    arrData = np.fromstring(arrData, dtype=np.float32, sep=' ')
    if arrData.shape[0] % self._ncols:
      raise Exception("Misformatted file: the line below has an invalid numb of cols:\n%s" % a)
    npArr = arrData.reshape((-1, self._ncols))
    return remainData, npArr

  def _readOn(self):
    """ Suppose the file can be read (_queryAndUpdateStatus has been invoked) 
    @return a list of NumPy arrays.
    """
    try:
      with open(self._filePath, "r") as f:
        f.seek(self._lastSeek)
        data = f.read()
        self._lastSeek += len(data)
        # Now try to extract lines:
        self._data += data
        self._data, lines = self._parseData(self._data)
        return lines
    except IOError:
      return None
    
  @classmethod
  def _ReadHeader(cls, filePth, nLines):
    """ Read header (nLines-1 first lines) + one data line (to know numb of compo.) """
    ret = []
    try:
      with open(filePth) as f:
        for _ in range(nLines):
          l = f.readline().strip()
          if l.startswith("#"):
            l = l[1:].strip()
          if l != "":
            ret.append(l.split())        
    except:
      return None
    return ret

class SonFile(TrustFile):
  _COMPO_NAMES = ["X", "Y", "Z"]
  
  def __init__(self, filePath, refPath):
    TrustFile.__init__(self, filePath, refPath)
    self._reset()

  def _reset(self):
    TrustFile._reset(self)
    self._dim = -1                       # Space dimension of the points 
    self._ncompo = -1                    # Numb of components of the field
    self._points = TrustFile._VOID_ARR   # The points themselves
    self._field = ""    # Field name (VITESSE, etc ...)

  @classmethod
  def BuildFromHeader(cls, filePath, refPath):
    """ Build either a SonPOINTFile or SonSEGFile depending on the header
    """
    try:
      hdr = TrustFile._ReadHeader(filePath,5)
      if hdr is None: 
        return None
      typ = hdr[3][1] 
      if typ == "POINTS" or typ == "NUMERO_ELEM_SUR_MAITRE":
        ret = SonPOINTFile(filePath, refPath)
      elif typ == "SEGMENT":
        ret = SonSEGFile(filePath, refPath)
      else:
        raise Exception("Invalid SON file type!")
    except:
      return None, None
    ret._populateFromHeader(hdr)
    return ret, hdr

  def _populateFromHeader(self, hdr=None):
    if hdr is None:
      hdr = TrustFile._ReadHeader(self._filePath,5)
    try:
      # Field name:
      self._field = hdr[2][1]
      # Points:
      dim = 1
      if len(hdr[1]) > 2:
        if hdr[1][3] == "y=": dim += 1
        if len(hdr[1]) > 5:
          if hdr[1][5] == "z=": dim += 1
      self._dim = dim
      # Store point coords:
      if (len(hdr[1])-1) % 2 or ((len(hdr[1])-1)/2) % dim:
        return None 
      npoints = (len(hdr[1])-1)/2/dim
      pts = []
      for i in range(npoints):
        pt = [float(a) for a in hdr[1][2+i*2*dim:2+(i+1)*2*dim:2]]
        pts.append(pt)
      # And finally number of components:
      if (len(hdr[4]) - 1) % npoints:
        return None
      self._ncompo = (len(hdr[4]) - 1) / npoints
      self._points = np.array(pts)
      self._ncols = len(hdr[4])
      return hdr
    except:
      self._reset()
      # A partial/misformatted file is ignored:
      return None
     
  def _displayPoint(self, pt):
    pts = ["%s=%g" % (SonFile._COMPO_NAMES[i], pt[i]) for i in range(len(pt))]
    return ",".join(pts)
  
  def _reloadEntries(self, hdr=None):
    if hdr is None:
      hdr = TrustFile._ReadHeader(self._filePath, 5)
    #self._entries[hdr[1][0]] = 0  # do not take time
    col = 1
    for p in self._points:
      sp = self._displayPoint(p)
      for cnum in range(self._ncompo):
        if self._ncompo > 1:
          e = "%s_%s (%s)" % (self._field, SonFile._COMPO_NAMES[cnum], sp)
        else:
          e = "%s (%s)" % (self._field, sp)
        self._entries[e] = col
        col += 1
    # Reset last index:
    self._lastIndex = dict( zip(self._entries.keys(), [-1] * len(self._entries)) )

class SonPOINTFile(SonFile):
  def __init__(self, filePath, refPath):
    SonFile.__init__(self, filePath, refPath)

  def getXLabel(self):
    return "Time"

class SonSEGFile(SonFile):
  """ For a segment file, getNewValues() will return None, None everytime the last line has changed.
   Hence by default the API of a seg file return  """
  def __init__(self,filePath, refPath):
    SonFile.__init__(self, filePath, refPath)
    self._reset()

  def _reset(self):
    SonFile._reset(self)
    self._start = []
    self._end = []
    self._orient = ""                  #  "x", "y", "z"  or "curvi."
    self._xaxis = TrustFile._VOID_ARR  # X axis for seg plots
    self._segentries = {}

  def _populateFromHeader(self, hdr=None):
    hdr = SonFile._populateFromHeader(self, hdr)
    if not hdr is None:
      l = [0.0] * (2 * self._dim)
      l2 = [float(hdr[3][i]) for i in range(2,2+2*self._dim)]
      start, end = l2[:self._dim], l2[self._dim:] 
      self.setXTremePoints(start, end)
      self._computeXAxis()
      return hdr
    else:
      return None

  def _computeCurvilinear(self):
    """ Compute curvilinear distances """
    d = self._points[1:] - self._points[:-1]
    dist = np.sqrt(np.sum(d*d, axis=1))
    curvi = np.zeros((self._points.shape[0],), dtype=np.float32)
    curvi[1:] = np.cumsum(dist)
    return curvi

  def _computeXAxis(self):
    if self._orient == "curvi.":
      self._xaxis = self._computeCurvilinear()
    else:
      mp = {"x": 0, "y": 1, "z": 2}
      self._xaxis = self._points[:, mp[self._orient]]

  def setXTremePoints(self, start, end):
    x1, x2 = start[0], end[0]
    if self._dim > 1:   y1, y2 = start[1], end[1]
    else:               y1, y2 = 0.0, 0.0
    if self._dim > 2:   z1, z2 = start[2], end[2]
    else:               z1, z2 = 0.0, 0.0
    if x1 == x2 and y1 == y2:
      self._orient = "z"
    elif x1 == x2 and z1 == z2:
      self._orient = "y"
    elif y1 == y2 and z1 == z2:
      self._orient = "x"
    else:
      self._orient = "curvi."
    self._start, self._end = start, end

  def getXLabel(self):
    return "Coordinate (%s)" % self._orient 

  def _extractValues(self, fileReady, entryName):
    """ Override to handle seg entries. """
    if self._entries.has_key(entryName):
      return SonFile._extractValues(self, fileReady, entryName)
    elif self._segentries.has_key(entryName):
      return self._extractValuesSeg(fileReady, entryName)
    else:
      raise Exception("Invalid column name! '%s'" % entryName)

  def _extractNewValues(self, fileReady, entryName):
    """ Override to handle seg entries - the behaviour is also to return None, None everytime a new seg is available. """
    if self._entries.has_key(entryName):
      return SonFile._extractNewValues(self, fileReady, entryName)
    elif self._segentries.has_key(entryName):
      return self._extractNewValuesSeg(fileReady, entryName)
    else:
      raise Exception("Invalid column name! '%s'" % entryName)
     
  def _extractValuesSeg(self, fileReady, entryName):
    if not fileReady:
      return TrustFile._VOID_ARR, TrustFile._VOID_ARR
    else:
      col = self._segentries[entryName]  
      self._lastIndex[entryName] = 1  # 1, no more new data available
      x, y = self._xaxis, self._yvalues[-1, 1+col::self._ncompo]
      return x, y

  def _extractNewValuesSeg(self, fileReady, entryName):
    if not fileReady:
      return None, None
    else:
      # Segment data - getNewValues()'s behaviour is to indicate with (None, None) that a new seg is available
      alreadyFetched = self._lastIndex[entryName]
      if not alreadyFetched:
        x, y = None, None
      else:
        x, y = TrustFile._VOID_ARR, TrustFile._VOID_ARR   # Nothing has changed
      return x, y

  def getEntriesSeg(self):
    """ Return only seg entries (i.e. not normal point entries) """
    entr = self.getEntries()  # make sure getEntries logic is applied
    return self._segentries.keys()

  def getPointEntries(self, segName):
    self.getEntries()  # make sure getEntries logic is applied
    compoNum = self._segentries[segName]
    l = []
    for e in self._entries:
      if self._ncompo == 1 or (self._entries[e] - compoNum) % self._ncompo:
        l.append(e)
    return l

  def getLastTime(self):
    if self._yvalues.shape[0]:
      return self._yvalues[-1,0]
    else:
      return 0.0

  def _reloadEntries(self, hdr=None):
    """ Override - append seg entries which are stored with neg indices """
    SonFile._reloadEntries(self, hdr)
    start = self._displayPoint(self._start)
    end = self._displayPoint(self._end)
    for cnum in range(self._ncompo):
      if self._ncompo == 1:
        e = "%s (SEG [%s] -> [%s])" % (self._field, start, end)
      else:
        e = "%s_%s (SEG [%s] -> [%s])" % (self._field, SonFile._COMPO_NAMES[cnum], start, end) 
      self._segentries[e] = cnum
      self._lastIndex[e] = 0     # 0, new data available, not fetched yet 
  
  def _completeValues(self):
    SonFile._completeValues(self)
    for s in self._segentries.keys():
      self._lastIndex[s] = 0     # 0, new data available, not fetched yet


class OutFile(TrustFile):
  _COMPO_NAMES = ["X", "Y", "Z"]
  
  def __init__(self, filePath, refPath):
    TrustFile.__init__(self, filePath, refPath)
    self._reset()

  def _reset(self):
    TrustFile._reset(self)
    self._dim = -1                       # Space dimension of the points 
    self._ncompo = -1                    # Numb of components of the field
    self._field = ""    # Field name (VITESSE, etc ...)


  def getXLabel(self):
    return "Time"
  def _populateFromHeader(self, hdr=None):
    if hdr is None:
      hdr = TrustFile._ReadHeader(self._filePath,4)
    try:
    
      # print "uuuuuuuuuu",hdr
      # Field name:
      self._fied="Unknown"
      for x in  hdr[1]:
        if x.find("Integral")> -1:
          self._field = x.split("=")[-1]
      nb_co=len(hdr[2])-1
      # print "uu",nb_co
      dim=1
      lg=len(hdr[3])
      if (lg >1):
        # dim=(len(hdr[3])-1)/nb_co soucis avec perio
        if (hdr[3][3]=='Fx'):
          dim=2
        else:
          assert(hdr[3][3]=='Fz')
          dim=3
      
      #if (self._field=="Integral(P*ndS)"):
      #  print "pb fichier pression"
      #  nb_co-=1
      # nb_co-=1 # on a total par erreur dans les label
      # print "yyyyyyyyyy",(len(hdr[3])-1)/dim, nb_co 
      # print "dim",dim,nb_co
      if dim==0:
        dim=1
      self._dim=dim
      self._ncols = dim*(nb_co)+1
      # self._ncols = len(hdr[4])
      
      #self._entries["Time"]=0
      col=1
      xold=None
      for c in xrange(nb_co):
        x=hdr[2][c+1]
        # dans le cas periodique le nom du bord est double
        if x==xold:
          x+="_bis"
        xold=x
        if dim==1:
          self._entries[self._field+" "+x]=col
          col+=1
        else:
          for i in xrange(self._dim):
            # print"uuu", i,x
            self._entries[self._field+" "+x+OutFile._COMPO_NAMES[i]]=col
            col+=1
      # print "uuu",self._entries
      return hdr
    except:
      self._reset()
      # A partial/misformatted file is ignored:
      return None
       
  def _reloadEntries(self, hdr=None):
    if hdr is None:
      self._populateFromHeader(hdr)
      # print "uuuuuuuuuuuu"
      #1/0
    
    self._lastIndex = dict( zip(self._entries.keys(), [-1] * len(self._entries)) )


class CSVFile(TrustFile):
  def __init__(self,filePath, nbclos,refPath=None):
    TrustFile.__init__(self, filePath, refPath)
    self._ncols = nbclos
    self._ncols_sa = nbclos
  def reset(self):
    print "reset"
    pass
  def getXLabel(self):
    return "??"
 
  def _populateFromHeader(self, hdr=None):
    self._ncols =  self._ncols_sa
    if (hdr is None):
      h1=[]
      for i in xrange(self._ncols):
        h1.append("c%d"%i)
      hdr=[h1]
     
    for i, col in enumerate(hdr[0]):
      self._entries[col] = i
    return hdr
 
  def _reloadEntries(self, hdr=None):
   
    if hdr is None:
      print "euh...;"
      1/0
    # Adrien ???
    self._lastIndex = dict( zip(self._entries.keys(), [-1] * len(self._entries)) )
    # print "QQ",self._lastIndex
    # print "uuu",self._entries.keys()



class DTEVFile(TrustFile):
  def __init__(self,filePath, refPath):
    TrustFile.__init__(self, filePath, refPath)

  def isLogY(self):
    return True
 
  def getXLabel(self):
    return "Time"
 
  def _populateFromHeader(self, hdr=None):
    if hdr is None:
      hdr = TrustFile._ReadHeader(self._filePath,2)
    self._ncols = len(hdr[0])
    return hdr
 
  def _reloadEntries(self, hdr=None):
    if len(hdr[0]) >= 6:
      for i, col in enumerate(hdr[0]):
        self._entries[col] = i
    self._lastIndex = dict( zip(self._entries.keys(), [-1] * len(self._entries)) )
       
def BuildFromPath(pth, ref_path):
  """ Object factory. """
  if pth[-4:] == ".son":
    ret, hdr = SonFile.BuildFromHeader(pth, ref_path)
    return ret
  elif pth[-6:] == ".dt_ev":
    return DTEVFile(pth, ref_path)
  elif pth[-4:] == ".out":
      return OutFile(pth, ref_path)
 
  else:
    raise ValueError("Invalid result file type!")
