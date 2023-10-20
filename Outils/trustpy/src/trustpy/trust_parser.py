#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Module 'tparser'
Allows the manipulation of a TRUST dataset.
Authors: A Bruneton, E Saikali
"""

class TObject(object):
    """ An object in the dataset. To be derived. Mostly used in the dataset conversion scripts, not
    in trustpy stuff. 
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

class TRUSTEndOfStreamException(Exception):
    """ Raised when the parser reaches the end of the stream, and ones tries to consume
    more tokens. """
    def __init__(self, msg="Unexpected end of file (or stream)!!"):
        Exception.__init__(self, msg)

class TRUSTTokens(object):
    """ Handy class representing a slice of a TRUSTStream (see below).
    Allows to synchronously deal with lower case and original tokens.
    """
    def __init__(self, low=[], orig=[]):
        self._orig = orig[:]
        self._low = low[:]

    def orig(self):
        return self._orig

    def low(self):
        return self._low

    @classmethod
    def Join(cls, lst):
        """ Merge a list of TRUSTTokens objects into a single TRUSTTokens object """
        l_low, l_orig = [], []
        for ttk in lst:
            l_low.extend(ttk._low)
            l_orig.extend(ttk._orig)
        return TRUSTTokens(l_low, l_orig)

class TRUSTStream(object):
    """ Handy class for scanning the tokens once the parsing has been done.
        Not needed internally but more user friendly for external clients of the parser.
        The parser produces a list of 'original' tokens (as were found in the original dataset)
        and a list of 'lower' tokens, which are stripped, lower-case version of the original tokens, and where the comments
        are compelty removed. 
        The two lists are always synchronized (at the same position) and have equal lengths.
    """
    def __init__(self, parser=None, file_nam="??"):
        self.file_nam = file_nam  # For tracking purposes only
        self.tok = []        # Original version, as found in the dataset
        self.tokLow = []     # Lower-case, stripped version, with comments blanked out
        self.lineNum = []    # Line number of the token (for error display)
        if not parser is None:
          self.tok = parser.tabToken[:]       # Better take copies ...
          self.tokLow = parser.tabTokenLow[:]
          self.lineNum = parser.lineNum[:]
        self.idx = 0            # Current position in the stream
        # Internal stuff:
        self._probing = False    # whether we are trying to move ahead in the stream, but have not validated the move yet.
        self._probeIdx = 0       # probing position
        self._prevIdx = -1
        self._states = {}        # see  save() / restore() methods

    def clone(self):
        ret = TRUSTStream()
        ret.file_nam = self.file_nam
        ret.tok = self.tok[:]
        ret.tokLow = self.tokLow[:]
        ret.lineNum = self.lineNum[:]
        ret.idx = self.idx
        ret._probing = self._probing
        ret._probeIdx = self._probeIdx
        ret._prevIdx = self._prevIdx
        ret._states = dict(self._states)
        return ret

    def __len__(self):
        return len(self.tok)

    def pos(self):
        """ Where are we in the stream? """
        return self.idx

    def setPos(self, i):
        self.idx = i

    def currentLine(self):
        """ Return the line number of the current token in the data file """
        if self.eof():
            return self.lineNum[-1]
        return self.lineNum[self.idx]

    def fileName(self):
        return self.file_nam

    def eof(self):
        """ Have we reached end of stream? """
        # If only blank tokens left, this is EOF:
        try:
            self.probeNextLow()
            self._probing = False
            return False
        except TRUSTEndOfStreamException as e:
            return True

    def nextLow(self):
        """ Returns the next non-blank token (lower case) and advance the stream just **after** it. """
        self._probing = False
        s = self.probeNextLow()
        self.validateNext()
        return s

    def probeNextLow(self):
        """ Same as nextLow() but without moving the stream forward. This allows the user to 'peek' into the stream.
        If happy with the result, one can finalize the move with validateNext() below. """
        self._probing = True
        for i, val in enumerate(self.tokLow[self.idx:]):
            if val != "":
                self._probeIdx = self.idx + i + 1
                return val
        self._probing = False
        raise TRUSTEndOfStreamException()

    def validateNext(self):
        """ Validate the forward move initiated with probeNextLow() """
        assert self._probing
        self._prevIdx = self.idx
        self.idx = self._probeIdx
        self._probing = False
        return self.idx

    def lastReadTokens(self):
        """ Return the last tokens read tokens when invoking nextLow() or validateNext().
        This returns a couple: 1. part of the list in lowercase, no-comment mode, 2. full original version
        """
        if self._prevIdx < 0:
            return []
        return TRUSTTokens(low=self.tokLow[self._prevIdx:self.idx],
                           orig=self.tok[self._prevIdx:self.idx])

    def save(self, tag):
        """ Save the current state of the stream in a internal dict under key 'tag'. This state can be restored
        using the restore() method. This allows moving forward 'just to try' and then coming back. """
        state = (self.idx, self._probing, self._probeIdx)
        self._states[tag] = state

    def restore(self, tag):
        """ See save(). """
        if tag not in self._states:
            raise Exception(f"Can not restore() state, invalid tag '{tag}'!!")
        self.idx, self._probing, self._probeIdx = self._states[tag]

    def dropTail(self, idx):
        """ Remove end of stream after index 'idx' (which is itself preserved). Useful for pruning blank stuff at the end of a file for example. """
        self.tok = self.tok[:idx+1]
        self.tokLow = self.tokLow[:idx+1]
        self.lineNum = self.lineNum[:idx+1]

class TRUSTParser(object):
    """ Main class allowing the parsing of a TRUST dataset.
        
    """
    def __init__(self):
        self.tabToken = []     # The list of tokens, with case, tabs and line returns preserved
        self.tabTokenLow = []  # Same as above, but all lowercase, and no tabs, no LR. Comment tokens are also empty.
        self.lineNum = []      # A list with exactly the same size as self.tabToken indicating on which line in the original dataset the token is found

    def _mergeQuoted(self):
        """ Merge back tokens containing quotes so that
            ['ab', 'cd', '"some', ' text', ' in', ' quotes"'] 
            becomes
            ['ab', 'cd', '"some text in quotes"'] 
        """
        # Merge tokens containing quotes " (used for example in things like 'system "rm -rf toto"')
        in_quot, prev_idx,  = False, -1
        tt, ttl, lst, lstl = [], [], [], []
        for i, (t, tl) in enumerate(zip(self.tabToken, self.tabTokenLow)):
            # If there is an even number of quotes in the token we are already fine, like in '"dom"'
            if tl.count('"') % 2 != 0:
                in_quot = not in_quot
                if not in_quot:  # branch when we just left the quoted mode:
                    tt.append(''.join(lst + [t]))
                    ttl.append(''.join(lstl + [tl]))
                    lst, lstl = [], []
                    continue
            if in_quot:
                lst.append(t)
                lstl.append(tl)
            else:
                tt.append(t)
                ttl.append(tl)
        self.tabToken, self.tabTokenLow = tt, ttl

    def tokenize(self, txtIn):
        """ Split dataset on spaces, tabs and line returns, but keep the spaces, line returns and tabs, in the original tokens
        to be able to reproduce a dataset with them at the end. The two following members are set:
        self.tabToken is the initial stream of tokens, with all spaces, tabs and line returns preserved (if you join it on '' you retrieve
        the initial string)
        self.tabTokenLow is the lower-case, stripped version of self.tabToken
        Once the tokenisation has been performed, a TRUSTStream object can be used to work with the tokens.
        """
        # The idea: split on <space>, then '<tab>', then '<ret>' and put back the separator at the **begining** of the token.
        # (will help in tools like packagepy where we might want to substitute tokens)
        def innerSplit(tab, char):
            ret = []
            for t in tab:
                ts = t.split(char)
                if len(ts) > 1:
                    ts2 = [ts[0]] + [char + a for a in ts[1:]]
                else:
                    ts2 = ts
                ret.extend(ts2)
            return ret
        a = innerSplit([txtIn], ' ')
        a = innerSplit(a, '\t')
        a = innerSplit(a, '\n')
        self.tabToken = a
        self.tabTokenLow = [t.lower().strip() for t in self.tabToken]
        self._mergeQuoted()

        # Extract line numbers (based on '\n'):
        lin = 1
        for t in self.tabToken:
            self.lineNum.append(lin)
            lin += t.count("\n")

        # Now validate the tokens, and empty slots of tabTokenLow where we have comments
        # Rules: - inside a pair of '#' everything is ignored (even '/*' or '*/')
        #        - if comment starts with '/*' then '#' is ignored and opening and closing '*/' must match
        inHash, starSlashCnt = False, 0
        for i, t in enumerate(self.tabTokenLow):
            emptyOnce = False
            if t == "#" and starSlashCnt == 0:
                inHash, emptyOnce = not inHash, True
            if t == "/*" and not inHash:
                starSlashCnt += 1
            if t == "*/" and not inHash:
                if starSlashCnt == 0:
                  raise ValueError("Invalid closing '*/' at line #%d" % self.lineNum[i])
                starSlashCnt -= 1
                emptyOnce = True
            if starSlashCnt > 0 or inHash or emptyOnce:
                self.tabTokenLow[i] = ""

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


    ##############################################################################################
    ##
    ## From here on, the methods are just used in old conversion scripts, not in 'trustpy' package.
    ## Typically this is used for the scripts allowing to move the 'medium' block into the problem.
    ##
    ##############################################################################################

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
            nam, idx = self.getObjName(mainKW, off)
            data["start"] = idx
            if data["start"] != -1:
                idx = self.getNext(idx, 1)
                currTok = self.tabTokenLow[idx]
                if currTok == "{":
                    print("It seems your dataset already has the correct '%s' format!" % keywords[0])
                    return False, ret_lst
                idxRead = self.findReadBlock(nam)
                # The keyword is loaded with 'lire' syntax: type nom_obj <other other>.....<other other> lire nom_obj
                if idxRead != -1:
                    idx = self.getNext(idxRead, 2)
                    data["start"] = idx
                    data["name"] = nam
                    currTok = self.tabTokenLow[idx]
                #print("tabTokenLow", self.tabTokenLow[idx-1:idx+1])
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


