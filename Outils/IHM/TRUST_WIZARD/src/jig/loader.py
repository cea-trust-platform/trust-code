#
# Author : A. Bruneton
#

import os, json
import jsonschema as jss
from generator import Generator
from validator import Validator

class Loader(object):
  """ Class loading a master JSON schema and all the sub-schemas it references 
  This class also exposes the core JSON validator as offered by the 'jsonschema' module.
  """
  def __init__(self, schemaDir, masterName, rootUri, extraSubSchemaNames=[]):
    self._schemaDir = schemaDir
    self._masterName = masterName
    self._extraSubSchemaNames = extraSubSchemaNames
    self._rootUri = rootUri
    
    self._masterSchema = None
    self._generator = None
    self._validator = None
    self._store = {}    # Map URI to sub-schemas
    
  def getRootURI(self):
    return self._rootUri
    
  def getMasterSchema(self):
    return self._masterSchema
    
  def setMasterSchema(self, schema):
    """ Mainly for test purposes """
    self._masterSchema = schema
    self._jss_validator = jss.Draft4Validator(self._masterSchema)
    
  def getSubSchema(self, uri, rootURI):
    """ Lookup a specific URI within the schema """
    arr = uri.split('#')
    newRootURI = arr[0] 
    if newRootURI == "":
      newRootURI = rootURI
    if newRootURI == self._rootUri:
      sch = self._masterSchema.copy()
    else:
      sch = self._store[newRootURI].copy()
    arr2 = arr[1].split('/')
    for a in arr2[1:]:   # skip starting '/'
      try:
        sch = sch[a]
      except:
        raise ValueError("Unable to resolve URI '%s'" % uri)
    return sch, newRootURI
    
  def buildGenerator(self):
    if self._generator is None:
      if self._masterSchema is None:
        raise Exception("No master schema! Did you call loadAndValidateSchemas()?")
      self._generator = Generator(self)
    return self._generator
  
  def buildJSonValidator(self):
    """ Get a pure JSon validator. No extra validation allowed. """
    if self._masterSchema is None:
      raise Exception("No master schema! Did you call loadAndValidateSchemas()?")
    return Validator(self._jss_validator)
  
  def _extractRecursive(self, schema):
    ret = []
    if isinstance(schema, list):
      for l in schema:
        ret.extend(self._extractRecursive(l))
    elif isinstance(schema, dict):
      for k in schema:
        if k == "$ref":
          arr = schema[k].split("#")
          if len(arr) != 2:
            raise Exception("Invalid reference in one sub-schema: '%s'" % schema[k])
          arr2 = arr[0].split(self._rootUri)
          if arr[0] != "" and len(arr2) != 2:
            raise Exception("Invalid reference in one sub-schema: '%s'" % schema[k])
          if len(arr2) == 2:
            fileName = arr2[1]
            ret.extend([fileName])
          else:
            # Internal reference
            pass
        else:
          ret.extend(self._extractRecursive(schema[k]))
    return ret
  
  def _extractReferences(self):
    finalSet = set()
    workSet = set([self._masterName])
    while len(workSet):
      ref = workSet.pop()
      if ref not in finalSet:
        finalSet.add(ref)
        with open(os.path.join(self._schemaDir, ref)) as f:
          print "Parsing sub-schema %s" % ref
          schema = json.loads(f.read())
        # Look for refs:
        files = self._extractRecursive(schema)
        workSet.update(files)
    finalSet.remove(self._masterName)
    return list(finalSet)
    
  def loadAndValidateSchemas(self):
    print "Loading and validating master-schema: %s ..." % self._masterName
    with open(os.path.join(self._schemaDir, self._masterName)) as f:
      self._masterSchema = json.loads(f.read())
    jss.Draft4Validator.check_schema(self._masterSchema)
  
    self._store = {}
    
    # Add to the store all JSon files in the same dir:
    otherJson = self._extractReferences()    
    for s in otherJson + self._extraSubSchemaNames:
      print "Loading and validating sub-schema: %s ..." % s
      with open(os.path.join(self._schemaDir, s)) as f:
        sch = json.loads(f.read())
        self._store[self._rootUri + s] = sch
        jss.Draft4Validator.check_schema(sch)
    
    self._jss_resolver = jss.RefResolver(self._rootUri + self._masterName, self._masterSchema, store=self._store)
    self._jss_validator = jss.Draft4Validator(self._masterSchema, resolver=self._jss_resolver)
    
    return self._masterSchema

#  def flattenSchema(self, schema=None, rootURI=None):
#    """ Resolve all references and produce an equivalent schema"""
#    schema = schema or self._masterSchema
#    rootURI = rootURI or self._rootUri
#    ret = {}
#    for k,v in schema.items():
#      if k == "$ref":
#        subSchema, subRootURI = self.getSubSchema(schema[k], rootURI)
#        flat = self.flattenSchema(subSchema, subRootURI)
#      elif isinstance()
#        ret[k] = self.flattenSchema(schema[k])
