"""
Update TRUST dataset to move medium block inside of problem, along with gravity.

Authors: A Bruneton, E Saikali
"""

# All keywords below in lower case!!
PB_LIST = ["pb_conduction",
           "pb_hydraulique", "pb_thermohydraulique", 
           "pb_thermohydraulique_qc", "pb_thermohydraulique_wc",
           "pb_hydraulique_melange_binaire_qc","pb_hydraulique_melange_binaire_wc",
           "pb_thermohydraulique_especes_wc",
           "pb_hydraulique_turbulent","pb_thermohydraulique_turbulent"] # exclude pb_couple, pb_multiphase, pb_med
MIL_LIST = ["solide", 
            "fluide_incompressible", 
            "fluide_quasi_compressible", "fluide_weakly_compressible"]
GRAVITE = ["gravite", "gravity"] # Strong assumption, gravity field is always called gravite/gravity

class ScriptModifier(object):
    def __init__(self):
        self.tabToken = []     # The list of tokens, with case, tabs and line returns preserved
        self.tabTokenLow = []  # Same as above, but all lowercase, and no tabs, no LR. Comment tokens are also empty.
        self.mil = {"decl": -1, "ass": -1, 
                    "start": -1, "end": -1, "name": "", "type": ""}   # decl is 'Solide mil',  ass is 'associer mil pb', start is begining of 'read mil ...', end is end of read block
        self.grav = {"decl": -1, "ass": -1, 
                     "start": -1, "end": -1 , "name": "", "type": ""}
        self.milType = ""
        self.pbIdxStart = -1
        
    def getObjName(self, key):
        """ Get name of an object as in 
               Pb_conduction pb_x
            Given key 'Pb_conduction' will return 'pb_x' and index of the token.
        """
        try:
           i = self.tabTokenLow.index(key)
           ii = self.getNext(i, 1)
           return self.tabToken[ii].strip(), i
        except: 
           return "", -1
 
    def findReadBlock(self, nam):
        """ Return start index of a 'read XXXX { ...' block
        """
        for i, t in enumerate(self.tabTokenLow):
            if self.tabTokenLow[i] in ["read","lire"]:
                ii = self.getNext(i, 1)
                if self.tabToken[ii].strip() == nam:
                    return i
        return -1

    def tokenize(self, fNameI):
        """ Split dataset on spaces, tabs and line returns, but keep the line returns in the original tokens
        to be able to reproduce a dataset with them at the end.
        """
        import re
        with open(fNameI, "r") as fIn:
            txtIn = fIn.read()
            tmp = re.split(' ', txtIn)
            for t in tmp:
                ts = t.split('\t')
                if len(ts) > 1:
                    ts2 = [a + "\t" for a in ts[:-1]]
                    ts2.append(ts[-1])
                else:
                    ts2 = ts
                ts4 = []
                for t2 in ts2:    
                    ts3 = t2.split('\n')
                    if len(ts3) > 1:
                        ts4 = [a + "\n" for a in ts3[:-1]]
                        ts4.append(ts3[-1])
                    else:
                        ts4 = ts3
                    self.tabToken.extend(ts4)  
            self.tabTokenLow = [t.lower().strip() for t in self.tabToken]

        # Now validate the tokens, and empty slots of tabTokenLow where we have comments
        valid = True
        for i, t in enumerate(self.tabTokenLow):
            if t in ["/*", "#"] and valid: 
                valid = False
                self.tabTokenLow[i] = ""
                continue
            if not valid: self.tabTokenLow[i] = ""
            if t in ["*/", "#"] and not valid: valid = True
        #print(self.tabTokenLow)
    
    def unTokenize(self, data, fNameO):
        """ Inverse operation of self.tokenize() :-)
        """
        out, acc = [], []
        for d in data + ['']:
            if d.endswith('\n') or d.endswith('\t'):
                acc.append(d)
            else:
                acc.append(d)
                out.append(''.join(acc))
                acc = []
        d = ' '.join(out)
        with open(fNameO, "w") as fOut:
            fOut.write(d)
        
    
    def getNext(self, start, off):
        """ Get index of (valid) token located 'off' slots after 'start', skipping
        void tokens (like the empty string ...) and comments. This supports negative offsets.
        This always return the index of a valid non empty token.
        """
        mult = -1 if off < 0 else 1
        cnt, cnt2 = 0, 0
        while cnt < abs(off):
            cnt2 = cnt2+1
            if self.tabTokenLow[start+mult*cnt2] == "": continue
            cnt = cnt+1
        return start+mult*cnt2

    def getNextJustAfter(self, start, off):
        """ Same as above, but instead of returning index of next valid token, 
        return index just passed the off-1 token.
        This allows for a better handling of comments and spaces.
        """
        i = self.getNext(start, off)
        j = i-1
        if self.tabTokenLow[j] != "":    return i
        while self.tabTokenLow[j] == "": j = j-1
        return j+1

    def findAssociate(self, nam):
        """ Locate the 'associer ...' directive
        """
        for i, t in enumerate(self.tabTokenLow):
            if t in ["associate", "associer"] and self.tabToken[self.getNext(i,2)].strip() == nam:
                return i
        return -1
   
    def checkName(self, name):
        """ Check if the name is a valid medium name, otherwise probably means that we are
        inspecting the medium block which is already inside the problem
        """
        if name == "{" or name == "#" or name == "/":
            return False
        return True
   
    def identifyMilieu(self):
        """ Locate declaration, reading and association of medium
        """
        # Spot problem type
        pbName = ""
        for p in PB_LIST: 
            pbName, _ = self.getObjName(p)
            if pbName != "": break
        if self.getObjName("probleme_couple")[1] != -1 or pbName == "":
            print("Problem type not supported - nothing done")
            return False
        # Spot milieu block of data
        for m in MIL_LIST:
            self.mil["name"], idx = self.getObjName(m)
            if self.mil["name"] != "":
                if not self.checkName(self.mil["name"]):
                    print("It seems that your datafile is already in the new 'milieu' syntax!")
                    return False
                self.mil["type"] = m
                self.mil["decl"] = idx
                print("Milieu '%s' of type '%s' found at index %d" % (self.mil["name"], m, idx))
                break
        if self.mil["name"] == "": 
            raise ValueError("Milieu not found!!")
        self.mil["start"] = self.findReadBlock(self.mil["name"])
        if self.mil["start"] == -1:
            print("Block 'read milieu' not found!!")
            return False
        startI, cntP = self.getNext(self.mil["start"], 2), 0
        for j in range(startI, len(self.tabToken)):
            if self.tabTokenLow[j] == "{":  cntP = cntP+1
            if self.tabTokenLow[j] == "}":  cntP = cntP-1
            if cntP == 0: 
                self.mil["end"] = self.getNextJustAfter(j,1)
                break
        
        # Find associate
        self.mil["ass"] = self.findAssociate(self.mil["name"])
        if self.mil["ass"] == -1:
            print("WHAAAT? No medium association found ...")
            return False
        # Find begining of problem:
        self.pbIdxStart = self.getNext(self.findReadBlock(pbName), 2)
        return True
      
    def readDimension(self):
        """ Read dimension
        """
        try:
            idx = self.tabTokenLow.index("dimension")
            if not self.isValid[idx]: return -1
            return int(self.tabTokenLow[self.getNext(idx, 1)])
        except:
            print("WHAAT?? 'dimension' keyword not found in dataset!!")
            return -1
      
    def identifyGravity(self):
        """ Find out position (if any) of gravity field in dataset
        """
        grav_nam = ""
        for g in GRAVITE:
            try:
                self.grav["decl"] = self.tabTokenLow.index(g)
                grav_nam = g
            except:
                pass
        if self.grav["decl"] == -1:
            print("Gravity field not found!")                
            return False
        self.grav["decl"] = self.getNext(self.grav["decl"], -1) # Capture field type like 'Champ_front_uniforme' written before keyword 'gravite'
        self.grav["type"] = self.tabTokenLow[self.grav["decl"]]

        self.grav["start"] = self.findReadBlock(grav_nam)
        dim = self.readDimension()
        if dim == -1:  # No dimension keyword in dataset; maybe included in another dataset
            dim = int(self.tabToken[self.getNext(self.grav["start"],2)])
            print("Guessing dimension from gravite: ", dim)
        self.grav["end"] = self.getNextJustAfter(self.grav["start"], 3+dim)
        
        # Find associate
        self.grav["ass"] = self.findAssociate(grav_nam)
        # Sorry Adrien :)
        if self.grav["ass"] == -1:
        	return False
        milName2 = self.tabToken[self.getNext(self.grav["ass"], 1)]
        
        if milName2 != self.mil["name"]:
            raise ValueError("Not implemented or problem : milieu name in associate is not matching for gravity")
        return True
          
    def indent(self, tkLst):
        """ Indent some lines """
        res = []
        for t in tkLst:
            if t.endswith("\n"): 
                res.append(t)
                res.append("  ")
            else:
                res.append(t)  
        return res
              
    def outputData(self, fNameO):
        """ Produce final output file.
        """
        tt = self.tabToken
        # Order things we will need to skip in newly created data file
        skip = [ (self.mil["decl"],self.getNextJustAfter(self.mil["decl"], 2)),
                 (self.mil["start"],self.mil["end"]),
                 (self.mil["ass"],self.getNextJustAfter(self.mil["ass"], 3))
               ]
        if self.grav["ass"] != -1:
            skip.extend([ (self.grav["decl"],self.getNextJustAfter(self.grav["decl"], 2)),
                          (self.grav["start"],self.grav["end"]),
                          (self.grav["ass"],self.getNextJustAfter(self.grav["ass"], 3))
                        ])
            startDeclG = self.getNext(self.grav["start"], 2)
            # Create gravity block:
            gravTxt = ["\n", "     gravite", self.grav["type"]] + self.tabToken[startDeclG:self.grav["end"]] 
            #print("Gravite: ", gravTxt)
        else:  # gravity can be completely ignored
            gravTxt = [""]      
        # Be sure to remove various blocks in the right order:
        skip.sort()
        # Create medium block
        justAfterAcc = self.getNextJustAfter(self.mil["start"], 3)  # After the '{' in 'read mil ...'
        milImplI = self.getNextJustAfter(self.mil["start"], 2)  # skip 'read mil'
        mil = ['\n', self.mil["type"]] + tt[milImplI:justAfterAcc] + gravTxt + tt[justAfterAcc:self.mil["end"]] + ['\n']
        mil = self.indent(mil)
        #print("Milieu: ", mil)

        # Create final token list by skiping what's given in 'skip' and inserting medium in problem:
        newData = tt[:skip[0][0]]           
        prev = skip[0][1]
        for ss, se in skip:
            newData.extend(tt[prev:ss])
            prev = se
        
        pbStartInside = self.getNextJustAfter(self.pbIdxStart, 1)  # After the '{' in the initial 'read pb', i.e. first token for first equation.
        newData.extend(tt[se:pbStartInside])
        # Put milieu inside pb:
        newData.extend(['  ' + self.milType] + mil)
        newData.extend(tt[pbStartInside:])
        self.unTokenize(newData, fNameO)
            
if __name__ == "__main__":
    import sys
    
    fNameI, fNameO = sys.argv[1], sys.argv[2]
    sm = ScriptModifier()
    sm.tokenize(fNameI)
    if sm.identifyMilieu():
        sm.identifyGravity()
        sm.outputData(fNameO)
        print("File '%s' written!" % fNameO)
    else:
        print("Nothing done.")
    
