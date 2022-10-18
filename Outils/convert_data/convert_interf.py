#!/usr/bin/env python3
""" Convert dataset containing a 'postraitement_ft_lata' block to the new format
"""
from tparser import TRUSTParser, TObject

class Flag(object):
    """ Just a type really """
    pass

class FTBlock(object):
    """ Class representing a single 'postraitement_ft_lata' block.
    """
    # The dictionnary of all possible parameters for FT post block:
    KEY_VAL = {"dt_post": str,    # Keep it as a string to copy it exactly as it was
                "nom_fichier": str,
                "print": Flag,
                "champs": None,
                "interfaces": None,
                "format": str
               }

    def __init__(self):
        self.postNam = ""
        self.postIdx = -1
        self.postEnd = -1
        self.data = {}       # Indexed by KEY_VAL
        self.champs_std = {}
        self.inter_name = ""
        self.champs_interf = {}
        self.format = ""

    def createNewFTBlock(self):
        """ Build the new postraitement_ft_lata block with the new format.
        """
        SPC = "   "
        ret = ["postraitement_ft_lata", self.postNam, "{", "\n"]
        #if not len(self.champs_std):
        #    self.champs_std["pressure"] = "elem"
        #f = self.data.get("format", "Lata")  # upper 'L' in Lata to avoid being replaced by 'lml' in lance_test
        #if f == "binaire": 
        if not len(self.champs_std) and "dt_post" in self.data:
            msg = "!!WARNING: for block '%s', dt_post option is present, but no regular field is being post-processed!\n" % self.postNam
            msg += "  In the new syntax, dt_post can not be specified for interface only.\n" 
            msg += "  You might want to add sth like 'champs dt_post xxx { pressure elem }' to ensure the original 'dt_post' is kept."
            print(msg)
        f = "Lata"
        ret.extend([SPC, "format", f, "\n"])
        if self.data.get("nom_fichier", "") != "": 
            ret.extend(["   ", "fichier", self.data["nom_fichier"], "\n"])
        if len(self.champs_std):
            # Sometimes no dt_post at all provided!! 
            dt_post = self.data.get("dt_post", "1.0"    )
            ret.extend([SPC, "champs", "dt_post", dt_post, "{", "\n"])
            for ch, loc in self.champs_std.items():
                ret.extend([SPC, SPC, ch, loc, "\n"])
            ret.extend([SPC, "}", "\n"])
        if self.inter_name != "":
            ret.extend([SPC, "interfaces", self.inter_name, "{", "\n"])
            for ch, loc in self.champs_interf.items():
                ret.extend([SPC, SPC, ch, loc, "\n"])
            ret.extend([SPC, "}", "\n"])
        return ret

class FTConvert(TRUSTParser):
    """ Main conversion class
    """
    def __init__(self):
        super().__init__()
        self.ft_blocks = []      # A list of FTBlocks
        
    def convertLoc(self, loc):
        LOC = {"sommets": "som", "elements": "elem", "faces": "faces"}
        if not loc in LOC:
            raise ValueError("Unknown localisation: %s" % loc)
        return LOC[loc]    
    
    def readChamps(self, dic, i):
        """ Read list of field names from block like:
                champs sommets
                {
                        vitesse
                        pressure
                }
        """
        t= self.tabTokenLow[self.getNext(i, 1)]
        if t == "dt_post":
            print("It seems that your data file already has the correct FT interface syntax!")
            return -1
        loc = self.convertLoc(self.tabTokenLow[self.getNext(i, 1)])
        startCh = self.getNext(i, 2)
        endCh = self.findBlockEnd(startCh)
        curr = self.getNext(startCh, 1)
        while curr < endCh:
            ch = self.tabToken[curr].strip() # no space but case preserved
            dic[ch] = loc
            curr = self.getNext(curr, 1)
        return endCh

    def readInterf(self, start, end, ftb):
        """ Read 'interfaces ...' block
        """
        i, endCh = start, -1
        while i < end:
            t = self.tabTokenLow[i]            
            # Read all the sub 'champs' parts:
            if t == "champs":
                endCh = self.readChamps(ftb.champs_interf, i)
                if endCh == -1:
                    return False
                i = endCh
            else:
                i = i+1
        return True

    def loadKeyValues(self, start, end, ftb):
        """ Fill FTBlock.data member with all the informations for a single block of post
        """
        i = start
        while i < end:
            t = self.tabTokenLow[i]
            if t in FTBlock.KEY_VAL:
                typ = FTBlock.KEY_VAL[t]
                if typ is Flag:
                    ftb.data[t] = True
                    i = self.getNext(i, 1)
                elif typ is None:
                    if t == "champs":
                        endC = self.readChamps(ftb.champs_std, i)
                        if endC == -1:
                            return False
                    elif t == "interfaces":
                        # name of the interface eq:
                        ftb.inter_name = self.tabTokenLow[self.getNext(i, 1)]
                        startIn = self.getNext(i, 2)
                        endC = self.findBlockEnd(startIn)
                        if not self.readInterf(startIn, endC, ftb):
                            return False
                    i = endC
                else: # float, str, int etc ...
                    nextI = self.getNext(i, 1)
                    ftb.data[t] = typ(self.tabToken[nextI].strip())
                    i = nextI
            else:
                if t == "fichier":  # Already new syntax
                    print("It seems that your data file already has the correct FT interface syntax!")
                    return False
                i = i+1 
        return True

    def loadPostFT(self):
        """ Load the dataset block 'postraitement_ft_lata { ... }'
        """
        off = 0
        while True:   # Yummi
            ftb = FTBlock()
            ftb.postNam, ftb.postIdx = self.getObjName("postraitement_ft_lata", off)
            if ftb.postIdx != -1:
                ftb.postEnd = self.findBlockEnd(self.getNext(ftb.postIdx, 2)) 
                if not self.loadKeyValues(ftb.postIdx, ftb.postEnd, ftb):
                    return False
                self.ft_blocks.append(ftb)
            if ftb.postIdx == -1:
                break
            off = ftb.postIdx+1
        if len(self.ft_blocks) == 0:
            print("No 'postraitement_ft_lata' block found!")
            return False
        return True
   
    def outputData(self, fNameO):
        """ Write everything out.
        """
        tt = self.tabToken
        # Replace FT blocks in order:      
        newData, prev = [], 0
        for ftb in self.ft_blocks:
            postIdx = ftb.postIdx
            newData.extend(tt[prev:postIdx])
            # New FT block
            ft = ftb.createNewFTBlock()
            newData.extend(ft)
            prev = ftb.postEnd
        # Finish writing:
        newData.extend(tt[prev:])
        self.unTokenizeAndWrite(newData, fNameO)

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print("Usage: convert_interf.py <input_file.data> <output_file.data>")
        sys.exit(-1)
    fNameI, fNameO = sys.argv[1], sys.argv[2]
    dm = FTConvert()
    dm.readAndTokenize(fNameI)
    if dm.loadPostFT():
        dm.outputData(fNameO)    
        print("File '%s' written!" % fNameO)
    else:
        print("Nothing done.")
        sys.exit(-1)
        
