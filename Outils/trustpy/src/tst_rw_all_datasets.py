#!/usr/bin/python

"""
This script scans all datasets in the test directory of the TRUST installation and performs two things
for each of them:
    - loading the dataset in the 'trustpy' data model
    - writing back from this data model the data file, and checking the result is strictly identical.

TRUST datasets are retrieved from the folders where a .lml.gz file is found.
"""

import os
import sys
import unittest

import trustpy.trust_utilities as TRUU
import trustpy.baseTru as BTRU
from trustpy.unittest_custom_methods_trust import UnittestCustomMethodsTrust

class TestCase(unittest.TestCase, UnittestCustomMethodsTrust):
    # Static variable - set in main() below:
    MODULE_NAME = None

    def get_all_data_names(self):
        """ Retrieve all .data file names from the places where we find a .lml.gz file """
        import glob
        trust_root = os.environ.get("TRUST_ROOT", None)
        if trust_root is None:
          raise Exception("TRUST_ROOT not defined! Have you sourced TRUST?")
        tst_dir = os.path.join(trust_root, "tests")
        # Find all datasets:
        pattern = os.path.abspath(os.path.join(tst_dir, "**/*.lml.gz"))
        g = glob.glob(pattern, recursive=True)
        TRUU.log_info("Number of test cases found: %d" % len(g))
        return g

    def single_test(self, lml_nam):
        """ Test a single data set """
        from trustpy.trust_parser import TRUSTParser, TRUSTStream
        import xyzpy.classFactoryXyz as CLFX

        # Extract dir name, and build dataset file name
        d = os.path.dirname(lml_nam)
        bas = os.path.split(d)[-1]
        data_nam = os.path.join(d, bas + ".data")
        with open(data_nam) as f:
            data_ex = f.read()
        try:
            # Parse the TRUST data set provided in arg
            tp = TRUSTParser()
            tp.tokenize(data_ex)
            stream = TRUSTStream(parser=tp, file_nam=data_nam)
            # Load it into the data model
            tds_cls = CLFX.getXyzClassFromName("DataSet" + BTRU._TRIOU_SUFFIX)
            ds = tds_cls.ReadFromTokens(stream)
            # Test write out:
            s = ''.join(ds.toDatasetTokens())
            data_ex_p = self.prune_after_end(data_ex)
            self.assertTrue(self.check_str_equality(s, data_ex_p).ok)
            return True
        except Exception as e:
            print(e)
            TRUU.log_error(f"   Dataset KO: {data_nam}\n\n\n\n\n")
            return False

    def test_all(self):
        """ Test all TRUST data files! """
        ok = True
        # Import generated module:
        mod_pth, mod_nam = os.path.split(TestCase.MODULE_NAME)
        mod_nam = os.path.splitext(mod_nam)[0]
        sys.path.append(mod_pth)
        res = {'res_import': None}
        cmd = "import %s as res_import" % mod_nam
        exec(cmd, res)  # need '.' as currentdir in PYTHONPATH to get import OK
        MODGEN = res['res_import']

        # Init classes:
        BTRU.initXyzClasses()
        MODGEN.initXyzClasses()

        # Let's go for all the tests now:
        all_data = self.get_all_data_names()
        for i, e in enumerate(all_data):
            print("Testing data set %s ..."  % e, end='')
            if self.single_test(e):
                print("OK")
            else:
                ok = False
        self.assertTrue(ok)

if __name__ == "__main__":
    mod = os.environ.get("GEN_MOD", "")
    if not os.path.exists(mod):
        print("Unable to find generated module defined in env variable GEN_MOD: %s" % mod)
        sys.exit(-1)
    TestCase.MODULE_NAME = mod
    unittest.main()

