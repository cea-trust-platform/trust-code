import sys
import os

def usage():
   print("Extract list of test cases used in a Jupyter Notebook.")
   print("This is done by extracting first all Python cells from the notebook and then replacing calls to 'runCases'")
   print("by a specific instruction outputing the test case list into a special file (./list_cases_jy)")
   print("   python get_nr_cases_jup.py <notebook.ipynb>")
   print("")
   print("No input notebook specified!")

def extract_cases(book):
   import json
   import re

   with open(book, "r") as f:
     root = json.loads(f.read())
     # Now parse tree to retrieve all Python up to the invocation of "runCases()":
     cells = root.get("cells", [])
     done, s = False, ""
     for c in cells:
       s += "\n"
       if done: break
       if c["cell_type"] == "code":
         if "source" in c:
           for loc in c["source"]:
             # for now, skip shell code lines:
             if loc.strip().startswith("!"):
               continue
             if ".runCases(" in loc:
               done = True
               s += re.sub(r"^(.*).runCases\([^\)]*\)(.*)$", "\\1.extractNRCases()\\2", loc)
               break
             s += loc
   # Now execute this code - it will perform whatever action the user wants to have as 'prepare' steps
   # and it will finally print out the list of test cases.
   exec(s, globals())

if __name__ == "__main__":
   if len(sys.argv) != 2:
     usage()
     sys.exit(-1)
   extract_cases(sys.argv[1])

