#!/usr/bin/env python3

"""
Update TRUST dataset to use new 'Lire_MED' syntax.

Authors: A Bruneton, E Saikali
"""
from tparser import TRUSTParser

class LireMEDConverter(TRUSTParser):
    """
    Former syntax: 
        Lire_MED [ vef|convertAllToPoly  ] [ family_names_from_group_names | short_family_names ] domaine_name mesh_name filename.med
        
    New syntax:
       lire_med {
          domaine dom
          file toto.med
          mesh the_mesh_in_file  // optional - if not there, first mesh taken.
          [convertAllToPoly]
       }
    """  
    LIST_PARAMS = [("convertalltopoly", bool),
                  ("family_names_from_group_names", bool),
                  ("short_family_names", bool),
                  ("dom", str), 
                  ("mesh", str), 
                  ("file", str)
                   ]

    def __init__(self):
        super().__init__()
        self.lire_med = []
    
    def loadLireMED(self):
        ok, self.lire_med = self.loadNoCurlyBraceGeneric(["lire_med", "read_med"])
        return ok

    def createNewLireMED(self, it):
        dom, file, mesh, catp = it["dom"], it["file"], it["mesh"], it["convertalltopoly"]
        lm =  ["Lire_MED", "{", "\n"]
        lm += ["   domain", dom, "\n"]
        lm += ["   file", file, "\n"]
#        if mesh != "--any--":
#            lm += ["   mesh", mesh, "\n"]
        if catp:
            lm += ["   convertAllToPoly\n"]
        lm += ["}\n"]
        return lm
        
    def outputData(self, fNameO):
        """ Write everything out.
        """
        tt = self.tabToken       
        newData, prev = [], 0
        for lmb in self.lire_med:
            newData.extend(tt[prev:lmb["start"]])
            # New Lire_MED block
            lm = self.createNewLireMED(lmb)
            newData.extend(lm)
            prev = lmb["end"]
        # Finish writing:
        newData.extend(tt[prev:])
        self.unTokenizeAndWrite(newData, fNameO)
        
if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print("Usage: convert_liremed.py <input_file.data> <output_file.data>")
        sys.exit(-1)
    fNameI, fNameO = sys.argv[1], sys.argv[2]
    dm = LireMEDConverter()
    dm.readAndTokenize(fNameI)
    if dm.loadLireMED():
        dm.outputData(fNameO)    
        print("File '%s' written!" % fNameO)
    else:
        print("Nothing done.")
        sys.exit(-1)
 
        

