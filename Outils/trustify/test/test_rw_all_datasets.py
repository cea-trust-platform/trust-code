#!/usr/bin/python

"""
This script scans all datasets in the test directory of the TRUST installation and performs two things
for each of them:
    - loading the dataset in the 'trustify' data model
    - writing back from this data model the data file, and checking the result is strictly identical.

TRUST datasets are retrieved from the folders where a .lml.gz file is found.

If an extra argument is passed on the command line, it is interpreted as a dataset, and only this dataset 
will be checked. In this last case either a valid path to a '.data' file can be passed, or a simple name 
like 'upwind' matching an existing TRUST test.
"""

import os
import sys
import unittest
from pathlib import Path

from trustify.misc_utilities import logger, UnitUtils

class TestCase(unittest.TestCase, UnitUtils):
    MODULE_NAME = None #: Static variable - set in main() below
    SINGLE_JDD = None  #: Only set when testing a single dataset

    def get_all_data_names(self):
        """ Retrieve all .data file names from the places where we find a .lml.gz file """
        import glob
        tr = os.environ.get("TRUST_ROOT", None)
        pd = os.environ.get("project_directory", None)
        from_trust = os.environ.get("TRUSTIFY_FROM_TRUST", None)
        if tr is None:
          raise Exception("TRUST_ROOT not defined! Have you sourced TRUST?")
        if not pd is None and from_trust is None: # Baltik!
          tst_dir = Path(pd) / "build" / "tests" / "all"
        else:
          tst_dir = Path(tr) / "tests"
        # Find all datasets ... must use glob.glob() and not Path.rglob() to follow symlinks ... shame.
        pattern = tst_dir / "**" / "*.lml.gz"
        g = glob.glob(str(pattern), recursive=True)
        # Convert to a list of .data
        ret = [Path(f).with_suffix('').with_suffix('.data') for f in g]

        # or just a single one if this was requested:
        if not self.SINGLE_JDD is None:
            p = Path(self.SINGLE_JDD)
            if p.exists():
                print(f" => Found case: {str(p)}")
                return [p]
            # Try to get the dataset from the glob:
            for f in ret:
                if f.stem == str(p.stem):
                    print(f" => Found case: {str(f)}")
                    return [f]
            msg = f"Test case '{self.SINGLE_JDD}' could not be found."
            logger.error(msg)
            raise Exception(msg)
        logger.info("Number of test cases found: %d" % len(ret))
        return ret

    def single_test(self, lml_or_data_nam, verbose=False):
        """ Test a single data set - this method is called also from tst_rw_all_datasets.py """
        from trustify.trust_parser import TRUSTParser, TRUSTStream
        from trustify.misc_utilities import ClassFactory, prune_after_end, check_str_equality

        # Extract dir name, and build dataset file name
        data_nam = lml_or_data_nam
        if lml_or_data_nam.suffixes == [".lml", ".gz"]:
            data_nam = lml_or_data_nam.with_suffix('').with_suffix(".data")

        # Resolve sym links (useful in Baltiks to have real test location):
        data_nam = data_nam.resolve()

        with open(data_nam) as f:
            data_ex = f.read()
        try:
            # Parse the TRUST data set provided in arg
            if verbose:
                print("\n")
                logger.info("About to parse dataset ...")
            tp = TRUSTParser()
            tp.tokenize(data_ex)
            if verbose: logger.info(" --> OK")
            stream = TRUSTStream(parser=tp, file_nam=data_nam)

            # Load it into the data model
            if verbose: logger.info("About to load it into the Python datamodel ...")
            tds_cls = ClassFactory.GetParserClassFromName("DataSet")
            ds = tds_cls.ReadFromTokens(stream)
            if verbose: logger.info(" --> OK")

            # Test write out:
            if verbose: logger.info("About to test writing it from the Python datamodel ...")
            s = ''.join(ds.toDatasetTokens())
            if verbose: logger.info(" --> OK")
            data_ex_p = prune_after_end(data_ex)

            if verbose: logger.info("About to compare with original dataset ...")
            if not check_str_equality(s, data_ex_p).ok:
              logger.info("   -> This might come from a duplicated instruction in your dataset...")
              raise Exception
            if verbose: logger.info(" --> OK")
            return True
        except Exception as e:
            print(e)
            #logger.error(f"   Dataset KO: {data_nam}\n\n\n\n\n")
            sys.exit(-1)
            return False

    def test_all(self):
        """ Test all TRUST data files! """
        # Retrieve datasets to be tested:
        all_data = self.get_all_data_names()

        # Import generated module:
        from trustify.misc_utilities import import_generated_module
        import_generated_module(TestCase.MODULE_NAME)

        # Let's go for all the tests now:
        ok = True
        for i, e in enumerate(all_data):
            print("Testing data set %s ..."  % e, end='')
            if self.single_test(e):
                print("OK")
            else:
                ok = False
        self.assertTrue(ok)

if __name__ == "__main__":
    from pathlib import Path
    mod = os.environ.get("GEN_MOD", "")
    if not Path(mod).exists():
        print("Unable to find generated module defined in env variable GEN_MOD: %s" % mod)
        sys.exit(-1)
    TestCase.MODULE_NAME = Path(mod)
    if len(sys.argv) > 1:
        TestCase.SINGLE_JDD = sys.argv.pop()
        t = TestCase()
        t.test_all()
    else:
        unittest.main()
