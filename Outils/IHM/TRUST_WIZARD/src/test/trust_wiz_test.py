# -*- coding: utf-8 -*-
#
# Author : A. Bruneton
#

import unittest, sys, os
import jig
from jig.loader import Loader
from jig import widgets

from pyqtside.QtGui import QApplication, QGridLayout, QWidget
qapp = QApplication(sys.argv)  

class TrustWizTest(unittest.TestCase):  
  def setUp(self):
    rootDir = os.environ.get("TRUST_WIZARD_ROOT_DIR")
    self._schema_path = os.path.join(rootDir, "share", "salome", "resources", "trust_wizard")
    self._data_path = self._schema_path
  
  def tearDown(self):
    pass

  def loadJSon(self, fileName):
    import json
    with open(os.path.join(self._data_path, fileName)) as f:
      data = json.loads(f.read())
    return data

  def testValidator(self):
    l = Loader("", "", "")
    l.setMasterSchema({"type": "object", "properties": {"tutu": {"type": "number"}}})
    g = l.buildGenerator()
    class StateObj:
      def __init__(self):
        self._last = None
      def customValid(self, x):
        self._last = "customValid"
        return "" if x > 0.0 else "error"
      def customValid2(self, x):
        self._last = "customValid2"
        return "" if x > 0.0 else "error"
    a = StateObj()
    g.registerCustomValidation("tutu", a.customValid, order=2)
    g.registerCustomValidation("tutu", a.customValid2, order=0)
    g.generate()
    v = g.getValidator()
    ret ,_ = v.validateInput(data={"tutu": 3.0}, highlight=False)
    self.assertEqual("", ret)
    self.assertEqual("customValid", a._last)
    ret, pth = v.validateInput(data={"tutu": -3.0}, highlight=False)
    self.assertEqual("error", ret)
    self.assertEqual([u'/', 'tutu'], pth)    

  def testLeafWidgets(self):    
    l = Loader(self._schema_path, "schema_leaf.json", "http://localhost/")
    l.loadAndValidateSchemas()
    g = l.buildGenerator()
    lay = QGridLayout()
    widget, _ = g.generate()
    cnt = 0
    for n in [u"tutu", u"tata", u"toto", u"combo1", u"combo2"]:
      w = widget.resolvePath([u"/", n])
      self.assertEqual([u"/", n], w.getPath())
      cnt += w._view.placeMeIn(lay, cnt)
    v = g.getValidator(widget)
    ret, pth = v.validateInput()
    self.assertEqual("", ret)
  
  def testComposedWidgets(self):
    l = Loader(self._schema_path, "schema_composed.json", "http://localhost/")
    l.loadAndValidateSchemas()
    g = l.buildGenerator()
    lay = QGridLayout()
    widget, _ = g.generate()
    widget._view.placeMeIn(lay, 0)
    pth = [u"/", u"ex_array_tab"]
    w = widget.resolvePath(pth)
    w.setNumberOfItems(0)
    w.setNumberOfItems(2)
    w.setNumberOfItems(1)
    cnt, pth = 0, [u"/", u"ex_radio", 1]
    w = widget.resolvePath(pth)
    self.assertEqual(pth, w.getPath())
    self.assertEqual(u"some string in radio", w.getTitle())
    w = widget.resolvePath([u"/", u"ex_radio"])
    w._radios[0].setChecked(True)  # Emulate user clicking the first radio
    w = widget.resolvePath([u"/", u"ex_radio_complex"])
    w._radios[0].setChecked(True)  
    cnt += w._view.placeMeIn(lay, cnt)
    v = g.getValidator(widget)
    ret, pth = v.validateInput()
    self.assertEqual("", ret)
      
  def testTrustSchemaJSON_NoneValues(self):
    """ Test schema without any custom widget nor any custom rule, but with GUI setting void numerical values """
    l = Loader(self._schema_path, "trust_schema.json", "trust_schemas:")
    l.loadAndValidateSchemas()
    g = l.buildGenerator()
    widget, _ = g.generate()
    vali = g.getValidator()
    data = self.loadJSon("ex_pb_thermo_hydrau.json")
    ret, _ = vali.validateInput(data=data, highlight=False)
    self.assertEqual("", ret)
    # Make a JSon error:
    data["thermo_hydrau_pb"]["domain"]["med_file"] = 3
    ret, pth = vali.validateInput(data=data, highlight=False)
    self.assertEqual("3 is not of type u'string'", ret)
    self.assertEqual([u'oneOf', 3, u'properties', u'thermo_hydrau_pb', u'properties', u'domain', u'properties', u'med_file', u'type'], pth)
    root = widget.resolvePath(["/"])
    root.onIndexChanged(3)  # select 'thermo_hydrau_pb'
    data2 = widget.getJSon()
    # Fill in dummy values to avoid non-valid default ones:
    self.assertEqual(1, len(data2['thermo_hydrau_pb']['gravity']))
    data2['thermo_hydrau_pb']['gravity'][0] = 9.8
    self.assertEqual(2, len(data2['thermo_hydrau_pb']['init_cond']['k-epsilon']))
    data2['thermo_hydrau_pb']['init_cond']['k-epsilon'] = [0.0,0.0]
    self.assertEqual(1, len(data2['thermo_hydrau_pb']['init_cond']['velocity']))
    data2['thermo_hydrau_pb']['init_cond']['velocity'][0] = 1.0
    self.assertEqual('', data2['thermo_hydrau_pb']['init_cond']['temperature'])
    data2['thermo_hydrau_pb']['init_cond']['temperature'] = 20.0
    # Idem with radio to click and combo to select:
    self.assertEqual(None, data2['thermo_hydrau_pb']['medium'])
    data2['thermo_hydrau_pb']['medium'] = {"preset": "Water 20 deg"}
    self.assertEqual('None', data2['thermo_hydrau_pb']['turb_model'])
    data2['thermo_hydrau_pb']['turb_model'] = "LM"
    ret, pth = vali.validateInput(data=data2, highlight=True)
    self.assertEqual("", ret)

  def testTrustDesktopHydrau(self):
    import os
    from trust_wiz.TrustWizDesktop import TrustWizDesktop
    
    data = {u'thermo_hydrau_pb': {u'bound_cond': [{u'bc_type': 'symetry',
                                      u'group_name': u'Groupe_1',
                                      u'k-epsilon': {},
                                      u'temperature': {},
                                      u'velocity': {}},
                                     {u'bc_type': 'wall',
                                      u'group_name': u'Groupe_2',
                                      u'temperature': None,
                                      u'velocity': 'adherent'}],
                     u'domain': {u'dimension': 3,
                                 u'geo_types': [u'NORM_TETRA4'],
                                 u'med_file': os.path.join(self._data_path,"Maillage_test.med") ,
                                 u'mesh_name': 'Maillage_1'},
                     u'gravity': ['', '', ''],
                     u'init_cond': {u'k-epsilon': [0.0, 0.0],
                                    u'temperature': '',
                                    u'velocity': ['', '', '']},
                     u'medium': {u'preset': 'Water 20 deg'},
                     u'turb_model': 'Laminar'}}
    desk = TrustWizDesktop(None)
    desk.createWidgetAndValidator(self._schema_path)
    g, widget = desk._generator, desk._dynWidget
    vali = g.getValidator()
    #
    # Pure data validation
    #
    ret, pth = vali.validateInput(data=data, highlight=False)
    self.assertNotEqual("", ret)  # A None remains
    self.assertEqual([u'/', 3, u'thermo_hydrau_pb', u'bound_cond', 1, 1, u'temperature'], pth)
    
    #
    # Now from the graphical interface:
    #
    w = widget.resolvePath([u'/', 3, u'thermo_hydrau_pb', u'bound_cond'])
    it = w.getItem()
    # Check hidden widgets:
    for w2 in it.findChildren("bc_type") + it.findChildren('group_name'):
      self.assertFalse(w2.isActive())
    w.setNumberOfItems(2)
    w = widget.resolvePath([u'/', 3, u'thermo_hydrau_pb', u'bound_cond', 1, 3])
    self.assertFalse(w.isActive())
    # Select thermo_hydrau problem type:
    widget.resolvePath([u'/']).onIndexChanged(1)
    self.assertFalse(w.isActive())
    widget.resolvePath([u'/', 3, u'thermo_hydrau_pb', u'bound_cond', 1]).onIndexChanged(5)  # Wall BC
    self.assertFalse(w.isActive())
    schem_pth = [u'oneOf', 3, u'properties', u'thermo_hydrau_pb', u'properties', u'bound_cond', u'items', u'oneOf', 5, u'allOf', 1, u'properties', u'temperature', u'oneOf', 1, u'properties', u'value', u'type']
    data_pth = [u'thermo_hydrau_pb', u'bound_cond', 1, u'temperature', u'value']
    w = vali._resolvePathInWidget(schem_pth, data_pth)
    self.assertEqual([u'/', 3, u'thermo_hydrau_pb', u'bound_cond', 1, 5, u'temperature', 1, u'value'], w.getPath())

  def testActiveWidgets(self):
    l = Loader(self._schema_path, "schema_composed.json", "http://localhost/")
    l.loadAndValidateSchemas()
    g = l.buildGenerator()
    widget, _ = g.generate()
    self.assertTrue(widget.resolvePath([u"/", u"ex_combo", 0]).isActive())
    self.assertFalse(widget.resolvePath([u"/", u"ex_combo", 1]).isActive())
    self.assertTrue(widget.resolvePath([u"/", u"ex_combo_complex", 0]).isActive())
    self.assertFalse(widget.resolvePath([u"/", u"ex_combo_complex", 1]).isActive())
    self.assertTrue(widget.resolvePath([u"/", u"ex_combo_complex", 1, "tata"]).isActive())
    
  def testTrustSchemaSolid(self):
    l = Loader(self._schema_path, "trust_schema.json", "trust_schemas:")
    l.loadAndValidateSchemas()
    g = l.buildGenerator()
    widget, _ = g.generate()
    vali = g.getValidator()
    data = self.loadJSon("ex_pb_solid.json")
    ret, _ = vali.validateInput(data=data, highlight=False)
    self.assertEqual("", ret)
  def testTrustSchemaHydrau(self):
    l = Loader(self._schema_path, "trust_schema.json", "trust_schemas:")
    l.loadAndValidateSchemas()
    g = l.buildGenerator()
    widget, _ = g.generate()
    vali = g.getValidator()
    data = self.loadJSon("ex_pb_hydrau.json")
    ret, _ = vali.validateInput(data=data, highlight=False)
    self.assertEqual("", ret)
    
  def testCustomValidSimple(self):
    l = Loader(self._schema_path, "trust_schema.json", "trust_schemas:")
    l.loadAndValidateSchemas()
    g = l.buildGenerator()
    def fileExists(pth):
      if not os.path.isfile(pth):
        return "NOT_EXIST"
      return ""
    g.registerCustomValidation("med_file", fileExists)
    _, _ = g.generate()
    vali = g.getValidator()
    data = self.loadJSon("ex_pb_thermo_hydrau.json")
    ret, pth = vali.validateInput(data=data, highlight=False)
    self.assertEqual("NOT_EXIST", ret)  # file in the example should not exist
    self.assertEqual([u"/",3, u"thermo_hydrau_pb", u"domain", u"med_file"], pth)
    
  def testAllExtraTrustValid(self):
    from trust_wiz.TrustWizDesktop import TrustWizDesktop
    data = {u'thermo_hydrau_pb': {u'bound_cond': [{u'bc_type': 'symetry',
                                      u'group_name': u'Groupe_1',
                                      u'k-epsilon': {},
                                      u'temperature': {},
                                      u'velocity': {}}],
                     u'domain': {u'dimension': 3,
                                 u'geo_types': [u'NORM_TETRA4'],
                                 u'med_file': os.path.join(self._data_path,"Maillage_test.med") ,
                                 u'mesh_name': 'Maillage_1'},
                     u'gravity': [0, 0, -9.8],
                     u'init_cond': {u'k-epsilon': [0.0, 0.0],
                                    u'temperature': 30.0,
                                    u'velocity': [0, 0, 0]},
                     u'medium': {u'preset': 'Water 20 deg'},
                     u'turb_model': 'Laminar'}}
    desk = TrustWizDesktop(None)
    desk.createWidgetAndValidator(self._schema_path)
    g, widget = desk._generator, desk._dynWidget
    vali = g.getValidator()
    ret, pth = vali.validateInput(data=data, highlight=False)
    self.assertEqual("", ret)
    ##
    ## Now testing all extra rules:
    ##
    #validateNonVoidObj(self, json_bit):
    ret, pth = vali.validateInput(data={}, highlight=False)
    self.assertEqual("Empty entry", ret[:11])
    self.assertEqual(['/'], pth)
    #validateNonNone(self, json_bit):
    prev, data['thermo_hydrau_pb']['medium'] = data['thermo_hydrau_pb']['medium'], None
    ret, pth = vali.validateInput(data=data, highlight=False)
    self.assertEqual("Entry set to", ret[:12])
    self.assertEqual([u'/', 3, u'thermo_hydrau_pb', u'medium'], pth)
    data['thermo_hydrau_pb']['medium'] = prev
    #validateFile(self, json_bit):
    prev, data['thermo_hydrau_pb']['domain']['med_file'] = data['thermo_hydrau_pb']['domain']['med_file'], "tutu@XYSXx.med"
    ret, pth = vali.validateInput(data=data, highlight=False)
    self.assertEqual("Non existing", ret[:12])
    self.assertEqual([u'/', 3, 'thermo_hydrau_pb', 'domain', 'med_file'], pth)
    data['thermo_hydrau_pb']['domain']['med_file'] = prev
    #validateMesh(self, json_bit):
    prev, data['thermo_hydrau_pb']['domain']['mesh_name'] = data['thermo_hydrau_pb']['domain']['mesh_name'], "@@@"
    ret, pth = vali.validateInput(data=data, highlight=False)
    self.assertEqual("Could not", ret[:9])
    self.assertEqual([u'/', 3, 'thermo_hydrau_pb', 'domain', 'mesh_name'], pth)
    data['thermo_hydrau_pb']['domain']['mesh_name'] = prev
    #validateDim(self, json_bit):
    prev, data['thermo_hydrau_pb']['domain']['dimension'] = data['thermo_hydrau_pb']['domain']['dimension'], 24
    ret, pth = vali.validateInput(data=data, highlight=False)
    self.assertEqual("Invalid mesh dimension", ret[:22])
    self.assertEqual([u'/', 3, 'thermo_hydrau_pb', 'domain', 'dimension'], pth)
    data['thermo_hydrau_pb']['domain']['dimension'] = prev
    #validateGeoTypes(self, json_bit):
    prev, data['thermo_hydrau_pb']['domain']['geo_types'] = data['thermo_hydrau_pb']['domain']['geo_types'], ['NORM_TOTO']
    ret, pth = vali.validateInput(data=data, highlight=False)
    self.assertEqual("Incoherent geometric", ret[:20])
    self.assertEqual([u'/', 3, 'thermo_hydrau_pb', 'domain', 'geo_types'], pth)
    data['thermo_hydrau_pb']['domain']['geo_types'] = prev
    #validateBC(self, json_bit):
    prev, data['thermo_hydrau_pb']['bound_cond'] = data['thermo_hydrau_pb']['bound_cond'], [{}]
    ret, pth = vali.validateInput(data=data, highlight=False)
    self.assertEqual("Some boundary", ret[:13])
    self.assertEqual([u'/', 3, 'thermo_hydrau_pb', 'bound_cond'], pth)
    data['thermo_hydrau_pb']['domain']['bound_cond'] = prev
    #validateVelocity(self, json_bit):
    prev, data['thermo_hydrau_pb']['init_cond']['velocity'] = data['thermo_hydrau_pb']['init_cond']['velocity'], [0.0]
    ret, pth = vali.validateInput(data=data, highlight=False)
    self.assertEqual("Invalid number", ret[:14])
    self.assertEqual([u'/', 3, 'thermo_hydrau_pb', 'init_cond', 'velocity'], pth)
    data['thermo_hydrau_pb']['init_cond']['velocity'] = prev

if __name__ == "__main__":
  suite = unittest.TestSuite()
  suite.addTest(TrustWizTest('testValidator'))
  suite.addTest(TrustWizTest('testLeafWidgets'))
  suite.addTest(TrustWizTest('testComposedWidgets'))
  suite.addTest(TrustWizTest('testTrustSchemaJSON_NoneValues'))
  suite.addTest(TrustWizTest('testTrustDesktopHydrau'))
  suite.addTest(TrustWizTest('testActiveWidgets'))
  suite.addTest(TrustWizTest('testTrustSchemaSolid'))
  suite.addTest(TrustWizTest('testTrustSchemaHydrau'))
  suite.addTest(TrustWizTest('testCustomValidSimple'))
  suite.addTest(TrustWizTest('testAllExtraTrustValid'))
  res = unittest.TextTestRunner().run(suite)
  if not res.wasSuccessful():
    sys.exit(1)
