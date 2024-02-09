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

from tst_rw_one_dataset import SingleTest

class TestCase(unittest.TestCase, UnittestCustomMethodsTrust):
    # Static variable - set in main() below:
    MODULE_NAME = None

    # Make single_test part of this class too:
    single_test = SingleTest.single_test

    def get_all_data_names(self):
        """ Retrieve all .data file names from the places where we find a .lml.gz file """
        import glob
        tr = os.environ.get("TRUST_ROOT", None)
        pd = os.environ.get("project_directory", None)
        if tr is None:
          raise Exception("TRUST_ROOT not defined! Have you sourced TRUST?")
        if not pd is None: # Baltik!
          tst_dir = os.path.join(pd, "build", "tests", "all")
        else:
          tst_dir = os.path.join(tr, "tests")
        # Find all datasets:
        pattern = os.path.abspath(os.path.join(tst_dir, "**/*.lml.gz"))
        g = glob.glob(pattern, recursive=True)
        TRUU.log_info("Number of test cases found: %d" % len(g))
        return g

    def test_all(self):
        """ Test all TRUST data files! """
        ok = True

        # Import generated module:
        MODGEN = TRUU.import_generated_module(TestCase.MODULE_NAME)

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
