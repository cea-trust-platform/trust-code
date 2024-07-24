import unittest
import pathlib
import pydantic
from trustpy.trad2_pydantic import generate_pydantic


class TestBase(unittest.TestCase):
    
    @classmethod
    def setUpClass(cls):
        if cls is TestBase:
            raise unittest.SkipTest("Base class")
        super().setUpClass()

    def setUp(self):        
        generate_pydantic(trad2_filename=f"src/trustpy/test/{self.TRAD2_FILE}", output_filename="pydantic_schema.py")
    
    def tearDown(self):
        import os
        os.unlink("pydantic_schema.py")
        return super().tearDown()

    def test_instantiate_all_classes(self):

        # reload the generated pydantic schema
        import pydantic_schema
        import importlib
        importlib.reload(pydantic_schema)

        # list of all schema classes
        all_classes = []        
        for keyword in pydantic_schema.__dict__.values():
            if isinstance(keyword, type) and issubclass(keyword, pydantic.BaseModel):
                all_classes.append(keyword)
        self.assertGreater(len(all_classes), 2)

        for cls in all_classes[1:]:
            with self.subTest(cls, arg=cls):
                if cls is pydantic_schema.champ_fonc_fonction_txyz:                    
                    self.assertFalse(True, "TODO champ_fonc_fonction_txyz")                    
                cls() # try to instantiate


class Test_myTRAD2(TestBase):
    def setUp(self):
        self.TRAD2_FILE = "myTRAD2"
        super().setUp()


class Test_TRAD_2_adr_simple(TestBase):
    def setUp(self):
        self.TRAD2_FILE = "TRAD_2_adr_simple"
        super().setUp()

class Test_TRAD_2_GENEPI_V17(TestBase):
    def setUp(self):
        self.TRAD2_FILE = "TRAD_2_GENEPI_V17"
        super().setUp()

class Test_TRAD_2_v1_5_cv(TestBase):
    def setUp(self):
        self.TRAD2_FILE = "TRAD_2_v1.5_cv"
        super().setUp()

class Test_TRAD_2_v192(TestBase):
    def setUp(self):
        self.TRAD2_FILE = "TRAD_2_v192"
        super().setUp()


if __name__ == "__main__":
    unittest.main()
