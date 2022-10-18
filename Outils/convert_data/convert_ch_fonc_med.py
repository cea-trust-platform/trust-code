#!/usr/bin/env python3

"""
Update TRUST dataset to use new 'Champ_Fonc_MED' (and 'Champ_Fonc_MED_tabule') syntax.

Authors: A Bruneton
"""
from tparser import TRUSTParser

def str_1(a):
    return str(a), 1
    
class ChampFoncMEDConverter(TRUSTParser):
    """
    Former syntax: 
        Champ_fonc_MED [ use_existing_domain ] [ last_time ] [ option ] filename domain_name field_name location time
        
    New syntax:
       Champ_fonc_MED {
            domain <domain_name>    // required - domain name
            file <file.med>         // required - MED file 
            field <field_name>      // required - name of the field to load
            loc [elem|som]          // required for now, but should soon become optional since this can be detected most of the time
            use_existing_domain     // optional - use existing domain already loaded
            last_time               // optional - flag to take last time step
            time <float>            // optional - mutually exclusive with last_time
            decoup <file>           // optional - splitting file for efficient parallel MED loading
       }
    """  
    LIST_PARAMS = [("table_temps", None),  # see overriden _handleOneParam() - to take care of Champ_Fonc_MED_table_temps
                  ("use_existing_domain", bool),
                  ("last_time", bool),
                  ("decoup", None),        # see overriden _handleOneParam()
                  ("file", str),
                  ("dom", str),
                  ("field", str),
                  ("loc", str),
                  ("time", str)
                  ]

    def __init__(self):
        super().__init__()
        self.lire_med = []

    def _handleOneParam(self, param_nam, typ, idx):
        """ Override because of funny-looking 'decoup' option
        """
        tok = self.tabTokenLow[idx]
        if param_nam == "decoup":
            if tok == "decoup":
                i = self.getNext(idx, 1)
                val = self.tabToken[i].strip()  # Name of the 'decoup' file
                return val, 2
            else:
                return "", 0
        # Handle specific "table_temps" parameter in Champ_Fonc_MED_table_temps:
        elif param_nam == "table_temps":
            if tok == "table_temps":
                i = self.getNext(idx, 1)
                if self.tabTokenLow[i] != "{":
                    raise Exception("Expected '{' after 'table_temps'!")
                e = self.findBlockEnd(i)
                tim_tab = self.tabToken[i:e+1]
                # Count numb of valid tokens betw i and e:
                cnt = 0
                for j in range(i, e+1):
                    if self.tabTokenLow[j] != "": cnt=cnt+1
                return tim_tab, cnt+1 # +1 for the param name itself
            else:
                return "", 0
        else:
            return TRUSTParser._handleOneParam(self, param_nam, typ, idx)
    
    def loadChampFoncMED(self, kw):
        ok, self.lire_med = self.loadNoCurlyBraceGeneric([kw])
        return ok

    def computeIndent(self, startIdx, kw):
        idx = startIdx
        # Scan backward to the previous line return and count how many char this makes:
        cnt = 0
        while True:
            idx = idx-1
            if idx < 0: break
            tok = self.tabToken[idx]
            if "\n" in tok: 
                pos = tok.rindex("\n")
                cnt += len(tok) - pos -1
                break
            cnt += len(tok) + 1  # +1 because of spaces between tokens that have been removed when we did the initial 'split'
        return [" " * (cnt + len(kw +" {"))]
    
    def createNewBlock(self, it, kw):
        """ Create the new block of data
        @param it a dictionnary
        @param kw is either "Champ_Fonc_MED" or "Champ_Fonc_MED_tabule"
        """
        withLR = True   # Whether to create a new block with line returns
        
        lr = ["\n"] if withLR else []
        dom, file, field, loc, tim = it["dom"], it["file"], it["field"], it["loc"], it["time"]
        if it.get("name", "") != "":
            indent_kw = ""
            lm =  ["{"]
        else:
            indent_kw = kw
            lm =  [kw, "{"]
        indent = self.computeIndent(it["start"], indent_kw) if withLR else []
        
        lm += ["domain", dom] + lr
        lm += indent + ["file", file] + lr
        lm += indent + ["field", field] + lr
        if loc != "elem":
            lm += indent + ["loc", loc] + lr
        if it["use_existing_domain"]:
            lm += indent + ["use_existing_domain"] + lr
        if it["last_time"]:
            lm += indent + ["last_time"] + lr
        else:
            lm += indent + ["time", tim] + lr
        if it.get("decoup", "") != "":
            lm += indent + ["decoup", it["decoup"]] + lr                
        tim_tab = it.get("table_temps", [])
        if len(tim_tab):
            lm += indent + ["table_temps"] + tim_tab[:-1] + [tim_tab[-1].strip()] + lr
        lm += indent + ["}\n"]
        return lm
        
    def outputDataChFonc(self, kw):
        """ Write everything out.
        """
        tt = self.tabToken       
        newData, prev = [], 0
        for lmb in self.lire_med:
            newData.extend(tt[prev:lmb["start"]])
            # New Champ_Fonc_MED block
            lm = self.createNewBlock(lmb, kw)
            newData.extend(lm)
            prev = lmb["end"]
        # Finish writing:
        newData.extend(tt[prev:])
        return self.unTokenize(newData)
        
if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print("Usage: convert_ch_fonc_med.py <input_file.data> <output_file.data>")
        sys.exit(-1)
    fNameI, fNameO, newTxt = sys.argv[1], sys.argv[2], ""
    dm = ChampFoncMEDConverter()
    # Load file as text:
    with open(fNameI, "r") as fIn:
        txtIn = fIn.read()
    dm.tokenize(txtIn)
    # Handle the three keywords:    
    for kw in ["Champ_Fonc_MED", "Champ_Fonc_MED_tabule", "Champ_Fonc_MED_table_temps"]:
        if dm.loadChampFoncMED(kw):
            newTxt = dm.outputDataChFonc(kw)    
            dm.tokenize(newTxt)
        else:
            print("Nothing done for '%s'." % kw)
    # Write out if needed:
    if newTxt != "":
        with open(fNameO, "w") as fOut:
            fOut.write(newTxt)
            print("File '%s' written." % fNameO)
    else:
        print("Nothing done.")
        sys.exit(-1)

    
