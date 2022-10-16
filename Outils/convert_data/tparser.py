"""
Module 'tparser'

Allows the manipulation of a TRUST dataset.

Authors: A Bruneton, E Saikali
"""

class TObject(object):
    """ An object in the dataset. To be derived. 
    """
    def __init__(self):
        self.decl = -1    # Token index of the declaration like 'Solide sol'
        self.start = -1   # Token index of the begining of the 'read' section
        self.end = -1     # Token index of the end of the 'read' section
        self.name = ""    # Name of the object, like 'sol'
        self.type = ""    # Type of the object, like 'Solide'
    
    def __str__(self):
        s = ""
        for k, v in self.__dict__.items():
            if not k.startswith("_"):
                s += "%s = %s\n" % (k, str(v))
        return s 

class TRUSTParser(object):
    def __init__(self):
        self.tabToken = []     # The list of tokens, with case, tabs and line returns preserved
        self.tabTokenLow = []  # Same as above, but all lowercase, and no tabs, no LR. Comment tokens are also empty.
        
    def getObjName(self, key, offset=0):
        """ Get name of an object as in 
               Pb_conduction pb_x
            Given key 'Pb_conduction' will return 'pb_x' and index of the token.
            offset can be used to start search after a given position.
        """
        try:
           i = self.tabTokenLow[offset:].index(key)
           i = i+offset
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

    def tokenize(self, txtIn):
        """ Split dataset on spaces, tabs and line returns, but keep the line returns in the original tokens
        to be able to reproduce a dataset with them at the end.
        """
        import re
        tmp = re.split(' ', txtIn)
        self.tabToken, self.tabTokenLow = [], []
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
    
    def readAndTokenize(self, fNameI):
        with open(fNameI, "r") as fIn:
            txtIn = fIn.read()
            self.tokenize(txtIn)    
    
    def unTokenize(self, data):
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
        d = ' '.join(out[:-1])
        return d
        
    def unTokenizeAndWrite(self, data, fNameO):
        d = self.unTokenize(data)
        with open(fNameO, "w") as fOut:
            fOut.write(d)
    
    def getNext(self, start, off):
        """ Get index of (valid) token located 'off' slots after 'start', skipping
        void tokens (like the empty string ...) and comments. This supports negative offsets.
        This always return the index of a valid non empty token.
        """
        mult = -1 if off < 0 else 1
        cnt, cnt2, valid = 0, 0, start
        while cnt < abs(off):
            cnt2 = cnt2+1
            # Too far away - stick to last valid token
            if start+mult*cnt2 >= len(self.tabTokenLow):
                return valid
            if self.tabTokenLow[start+mult*cnt2] == "":
                continue
            valid = start+mult*cnt2
            cnt = cnt+1
        return valid

    def getNextJustAfter(self, start, off):
        """ Same as above, but instead of returning index of next valid token, 
        return index just passed the off-1 token.
        This allows for a better handling of comments and spaces.
        """
        assert(off >= 1)
        i = self.getNext(start, off-1)
        return i+1

    def findBlockEnd(self, idx):
        """ Given an index corresponding to the start of a '{ ... }' block, 
        return the end index matching the closing '}' brace
        """
        if self.tabTokenLow[idx] != "{":
            raise ValueError("findBlockEnd() method called with index %d, but this does not correspond to an opening '{'!!" % idx)
        cntP = 0
        for j in range(idx, len(self.tabToken)):
            if self.tabTokenLow[j] == "{":  cntP = cntP+1
            if self.tabTokenLow[j] == "}":  cntP = cntP-1
            if cntP == 0: 
                return j

    def findAssociate(self, nam, offset=0):
        """ Locate an 'associer XXX nam' directive, potentially after a given position. 
        """
        for i, t in enumerate(self.tabTokenLow[offset:]):
            if t in ["associate", "associer"] and self.tabToken[self.getNext(i+offset,2)].strip() == nam:
                return i+offset
        return -1
   
    def readDimension(self):
        """ Read dimension
        """
        try:
            idx = self.tabTokenLow.index("dimension")
            return int(self.tabTokenLow[self.getNext(idx, 1)])
        except:
            print("WHAAT?? 'dimension' keyword not found in dataset!!")
            return -1

    def _handleOneParam(self, param_nam, typ, idx):
        """ Handle a single non-flag parameter when processing data with 
            loadNoCurlyBraceGeneric()
            @return the parsed value, and the number of tokens consummed.
        """
        val = typ(self.tabToken[idx].strip())  # preserve case! so do not 'typ(currTok)'
        shift = 1
        return val, shift
            
    def loadNoCurlyBraceGeneric(self, keywords):
        """ Load any TRUST keyword for which the syntax is defined by an (ordered) list of 
        flags and parameters.
        The class deriving TRUSTParser must have a member named LIST_PARAMS of the form:
            [("param_name1", type1),
             ("param_name2", type2), 
             ...]
        If 'type' is bool the parameter is considered as an optional flag.
        Otherwise, it is used to convert the string read in the datafile directly: 
            it must be a callable returning a tuple like
                (value, shift)
            value is the actual value to be stored, shift is the number of tokens consumed.
        @param keywords a list of keywords (for synonyms), for example ["Lire_MED", "Read_MED"]
        @return a list of dictionnary. Keys are the keys from LIST_PARAMS
        """
        # Handle synonyms by putting the first keyword everywhere - Google translate :-)
        keywords = [s.lower() for s in keywords]
        mainKW = keywords[0]
        for i, t in enumerate(self.tabTokenLow):
            if t in keywords[1:]:
                self.tabTokenLow[i] = mainKW
        # Now for the real processing:
        ret_lst, off, noMoreBool, idx2 = [], 0, False, 0
        while True:   # Yummi :-)
            data = {}
            _, idx = self.getObjName(mainKW, off)
            data["start"] = idx
            if data["start"] != -1:
                idx = self.getNext(idx, 1)
                currTok = self.tabTokenLow[idx]
                if currTok == "{":
                    print("It seems your dataset already has the correct '%s' format!" % keywords[0])
                    return False, ret_lst
                for param_nam, typ in self.LIST_PARAMS:
                    if noMoreBool and typ is bool:
                        raise ValueError("Error: the specified LIST_PARAMS contains a bool parameter after a non-bool parameter ... not supported yet. All flags should come first.")
                    if typ is bool:
                        if currTok == param_nam:
                            data[param_nam] = True
                            # Move to next token
                            idx, idx2 = self.getNext(idx, 1), self.getNextJustAfter(idx, 1)
                            currTok = self.tabTokenLow[idx]
                        else:
                            data[param_nam] = False
                    else:
                        data[param_nam], shift = self._handleOneParam(param_nam, typ, idx)
                        if shift > 0:
                            idx, idx2 = self.getNext(idx, shift), self.getNextJustAfter(idx, shift)
                            currTok = self.tabTokenLow[idx]
                # To avoid handling specifically the last iteration in the for loop
                data["end"] = idx2
                ret_lst.append(data)
                off = idx
            else:    # if ret["start"] != -1:
                break
        return (len(ret_lst) != 0, ret_lst)


