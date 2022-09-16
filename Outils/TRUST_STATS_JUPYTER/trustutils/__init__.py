""" This is the trustutils package providing functionalities for writing
validation forms with the Jupyter notebook

Victor Banon Garcia / Adrien Bruneton
CEA Saclay - DM2S/STMF/LGLS
03/2021
"""

# Check that we are under proper TRUST environment
import os

if os.environ.get("TRUST_ROOT", "") == "":
    print("TRUST environment not properly loaded!!! Source TRUST first.")
    raise Exception("TRUST environment not properly loaded!!! Source TRUST first.")
del os

from .files import TrustFile, SonFile, SonPOINTFile, SonSEGFile, OutFile, CSVFile, DTEVFile
from .jupyter import run

try:
    from .jupyter import plot
except:
    print("WARNING: jupyter.plot functionality not available (missing matplotlib?")
    print("WARNING: Try to import jupyter.plot directly to see missing packages.")

try:
    from .jupyter import widget
    from .visitutils import tools_for_visit as visit
    from .visitutils import message
except:
    print("WARNING: jupyter.widget functionality not available ... please see README.md.")
    # Widget might not always be available!!
    pass
