#!/usr/bin/env python3

"""
Update TRUST dataset to use new 'Lire_MED' syntax.

Authors: A Bruneton, E Saikali
"""
from tparser import TRUSTParser, TObject

class LireMEDBlock(TObject):
    """ A single Lire_MED block
    """    
    def __init__(self):
        super().__init__()
        self.file = ""
        self.mesh = ""
        self.dom = ""
        self.catp = False # ConvertAllToPoly
    
    def createNewLireMED(self):
        lm =  ["Lire_MED", "{", "\n"]
        lm += ["   domain", self.dom, "\n"]
        lm += ["   file", self.file, "\n"]
#        if self.mesh != "--any--":
#            lm += ["   mesh", self.mesh, "\n"]
        if self.catp:
            lm += ["   convertAllToPoly\n"]
        lm += ["}\n"]
        return lm

class LireMEDConverter(TRUSTParser):
    def __init__(self):
        super().__init__()
        self.lire_med = []
        
    def loadLireMED(self):
        off = 0
        # Google translate :-)
        for i, t in enumerate(self.tabTokenLow):
            if t == "read_med":  self.tabTokenLow[i] = "lire_med"
        # Now for the real processing:
        while True:   # Yummi :-)
            lmb = LireMEDBlock()
            _, idx = self.getObjName("lire_med", off)
            lmb.start = idx
            idx = self.getNext(idx, 1)            
            firstArgL = self.tabTokenLow[idx]
            if firstArgL == "{":
                print("It seems your dataset already has the correct Lire_MED format!")
                return False
            if lmb.start != -1:
                # "Syntax: Lire_MED [ vef|convertAllToPoly  ] [ family_names_from_group_names | short_family_names ] domaine_name mesh_name filename.med" << finl;
                if firstArgL == "convertalltopoly":
                    lmb.catp = True
                    idx = self.getNext(idx, 1)
                    firstArgL = self.tabTokenLow[idx]
                if firstArgL == "family_names_from_group_names":
                    # Skip silently
                    idx = self.getNext(idx, 1)
                    firstArgL = self.tabTokenLow[idx]
                if firstArgL == "short_family_names":
                    print("WHAAT? Using keyword 'short_family_names' - clean this up!")
                    # Skip silently
                    idx = self.getNext(idx, 1)
                    firstArgL = self.tabTokenLow[idx]
                # Domain - case sensitive
                lmb.dom = self.tabToken[idx]
                idx = self.getNext(idx, 1)
                # Mesh name - case sensitive
                lmb.mesh = self.tabToken[idx].strip()
                idx = self.getNext(idx, 1)
                # File name - case sensitive
                lmb.file = self.tabToken[idx].strip()

                lmb.end = self.getNextJustAfter(idx, 1)
                self.lire_med.append(lmb)
                off = idx
            else:
                break
        return len(self.lire_med) != 0
        
    def outputData(self, fNameO):
        """ Write everything out.
        """
        tt = self.tabToken       
        newData, prev = [], 0
        for lmb in self.lire_med:
            newData.extend(tt[prev:lmb.start])
            # New Lire_MED block
            lm = lmb.createNewLireMED()
            newData.extend(lm)
            prev = lmb.end
        # Finish writing:
        newData.extend(tt[prev:])
        self.unTokenize(newData, fNameO)
        
if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print("Usage: convert_liremed.py <input_file.data> <output_file.data>")
        sys.exit(-1)
    fNameI, fNameO = sys.argv[1], sys.argv[2]
    dm = LireMEDConverter()
    dm.tokenize(fNameI)
    if dm.loadLireMED():
        dm.outputData(fNameO)    
        print("File '%s' written!" % fNameO)
    else:
        print("Nothing done.")
        sys.exit(-1)
 
        

