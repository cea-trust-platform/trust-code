#!/usr/bin/env python
# -*- coding: utf-8 -*-


"""
Example of a PACKAGESPY class to aggregate as attributes
some other user_defined PACKAGESPY classes
This is only for test
"""

import setenv_trustpy   # as PATH set to TRUSTPY_ROOT_DIR

import xyzpy.loggingXyz as LOG
logger = LOG.getLogger()   # used as global

import trustpy.baseTru as BTRU

class TRUSTAll_Tru(BTRU.ConstrainBase_Tru):
  """
  Class to set helpDict dynamically for create_model
  This is one class example inherited from ConstrainBase_Tru
  with attributes set as ALL TRUST classes
  """
  _helpDict = {}
  _attributesList = []

  # example of __init__
  # def __init__(self):
  #   super(TRUSTAll_Tru, self).__init__()
  #   self.setIsCast(True)

  # here could define save-restore-xml context menu
  def getActionsContextMenu(self):
    actions = super(TRUSTAll_Tru, self).getActionsContextMenu()
    #done on first item root unconditionnaly
    #controller = self.getController()
    #controllerActions = controller.getDefaultActions()
    #actions += controllerActions
    return actions

def create_model_all(trust_packagespy_generated=None, names=None):
  """
  example for a ConstrainBase_Tru with attributes as all TRUG classes
  trust_packagespy_generated is an imported *_generated*.py module file (variable user name file)
  example:
  >> import trust_packagespy_generated_v15 as TRUG   # import trust_packagespy_generated_v15.py
  >> create_model_all(trust_packagespy_generated=TRUG)
  """
  from trustpy.trust_utilities import PPF, PPFL, PPFC, filter_docstring
  import xyzpy.classFactoryXyz as CLFX

  # tricky... TRUG supposedly previously generated .py module file, but caller know what name and import it
  if trust_packagespy_generated is not None:
    TRUG = trust_packagespy_generated
  else:
    import trust_packagespy_generated as TRUG

  logger.debug("all TRUG packagespy classes :\n%s" % PPFC(TRUG.packagespy_classes))
  model = TRUSTAll_Tru()
  done = False
  max_i = 5000
  for i, c in enumerate(TRUG.packagespy_classes):
    # print("create_model name class %s" % c.__name__)
    # if "fermeture_genepi" in c.__name__:
    #   model_fermeture_genepi = c
    if i > max_i:
      if not done: logger.warning("limited number of classes displayed as %i" % max_i)
      done = True
      continue # TODO just limited for debug
    try:
      ci = c()
    except Exception as e:
      logger.warning("Problem instanciate class %s\n%s" % (c.__name__, e))
      continue
    # an attribute name with trailing index for example
    try:
      # if c.__name__ == "Conduction_Tru": help(c)
      attName = "%s_%i" % (ci.getNameAsAttributeAsRoot(), i)
    except Exception as e:
      logger.warning("Problem class %s\n%s" % (ci.__class__.__name__, e))
      # help(ci)

    model._attributesList.append((attName, ci.__class__.__name__))
    model.__setattr__(attName, ci)
    try:  # if leaf inexisting method _setAllAttributesListRecurse
      ci._setAllAttributesListRecurse(depth=2) # initiate attributes as branchs, avoid sous_zone
    except:
      pass
    # set inexisting tooltip for the model attribute from docstring of class by default
    if ci.__doc__ is not None:
      TRUSTAll_Tru._helpDict[attName] = (filter_docstring(ci.__doc__), "")
    else:
      TRUSTAll_Tru._helpDict[attName] = (filter_docstring("%s has no __doc__" % ci.__class__.__name__), "")

  # logger.info("model._helpDict :\n%s" % PPFL(TRUSTAll_Tru._helpDict))
  # help(ci)

  # make this class known, (but empty, but with _helpDict set)
  # memorize _helpDict once dynamically for TRUSTAll_Tru
  # and could be used by MVC functionalities
  CLFX.appendAllXyzClasses( [TRUSTAll_Tru] )
  return model



########################################
if __name__ == "__main__":
  logger.info("BEGIN main %s" % __file__)
  logger.info("END main")
