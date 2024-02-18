#!/usr/bin/python

"""
This script scans a single dataset. See comments in tst_rw_all_datasets.py
"""

import os
import sys
import unittest

import trustpy.trust_utilities as TRUU
import trustpy.baseTru as BTRU
from trustpy.unittest_custom_methods_trust import UnittestCustomMethodsTrust

class SingleTest(UnittestCustomMethodsTrust):
    # Static variable - set in main() below:
    MODULE_NAME = None
    JDD_NAME = None

    def single_test(self, lml_or_data_nam, verbose=False):
        """ Test a single data set - this method is called also from tst_rw_all_datasets.py """
        from trustpy.trust_parser import TRUSTParser, TRUSTStream
        import xyzpy.classFactoryXyz as CLFX

        # Extract dir name, and build dataset file name
        if lml_or_data_nam.endswith(".lml.gz"):
            d = os.path.dirname(lml_or_data_nam)
            bas = os.path.split(d)[-1]
            data_nam = os.path.join(d, bas + ".data")
        else:
            data_nam = lml_or_data_nam

        # Resolve sym links (useful in Baltiks to have real test location):
        data_nam = os.path.realpath(data_nam)

        with open(data_nam) as f:
            data_ex = f.read()
        try:
            # Parse the TRUST data set provided in arg
            if verbose:
                print("\n")
                TRUU.log_info("About to parse dataset ...")
            tp = TRUSTParser()
            tp.tokenize(data_ex)
            if verbose: TRUU.log_info(" --> OK")
            stream = TRUSTStream(parser=tp, file_nam=data_nam)

            # Load it into the data model
            if verbose: TRUU.log_info("About to load it into the Python datamodel ...")
            tds_cls = CLFX.getXyzClassFromName("DataSet" + BTRU._TRIOU_SUFFIX)
            ds = tds_cls.ReadFromTokens(stream)
            if verbose: TRUU.log_info(" --> OK")

            # Test write out:
            if verbose: TRUU.log_info("About to test writing it from the Python datamodel ...")
            s = ''.join(ds.toDatasetTokens())
            if verbose: TRUU.log_info(" --> OK")
            data_ex_p = self.prune_after_end(data_ex)

            if verbose: TRUU.log_info("About to compare with original dataset ...")
            if not self.check_str_equality(s, data_ex_p).ok:
              TRUU.log_info("   -> This might come from a duplicated instruction in your dataset...")
              raise Exception
            if verbose: TRUU.log_info(" --> OK")
            return True
        except Exception as e:
            print(e)
            TRUU.log_error(f"   Dataset KO: {data_nam}\n\n\n\n\n")
            return False

    def test_one(self, mod, dataset):
        """ Test a single TRUST data file """
        # Import generated module:
        MODGEN = TRUU.import_generated_module(mod)
        # Init classes:
        BTRU.initXyzClasses()
        MODGEN.initXyzClasses()

        # Let's go for all the tests now:
        print("Testing data set %s ..."  % dataset, end='')
        if self.single_test(dataset, verbose=True):
            print("OK")
            return True
        return False

if __name__ == "__main__":
    import sys
    mod = os.environ.get("GEN_MOD", "")
    verb = int(os.environ.get("VERBOSE", "0"))
    if verb:
      TRUU._log_level = 4
    if not os.path.exists(mod):
        print("Unable to find generated module defined in env variable GEN_MOD: %s" % mod)
        sys.exit(-1)
    st = SingleTest()
    if not st.test_one(mod, sys.argv[1]):
      sys.exit(-1)
