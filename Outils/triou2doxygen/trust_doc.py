#!/usr/bin/env python3
""" This script can be used to convert the old TRUST documentation format into 
a clean standard doxygen format.
Hence something like:

        // Description:
        //    Renvoie la zone discretisee associee a l'equation.
        // Precondition: une zone discretisee doit avoir ete associee a l'equation.
        // Parametre:
        //    Signification:
        //    Valeurs par defaut:
        //    Contraintes:
        //    Acces:
        // Retour: Zone_dis&
        //    Signification: la zone discretisee asscoiee a l'equation
        //    Contraintes:
        // Exception: l'objet zone discretisee (Zone_dis) est invalide,
        //            probleme associe non discretise.
        // Effets de bord:
        // Postcondition:

is turned into

        /*! @brief Renvoie la zone discretisee associee a l'equation.
         *
         * @return (Zone_dis&) la zone discretisee asscoiee a l'equation 
         * @throws l'objet zone discretisee (Zone_dis) est invalide,
         * probleme associe non discretise. 
         */
"""

class DocConverter(object):

    ##
    ## .h and .hxx files
    ##
    DESC_CLS_TAG = ".DESCRIPTION"
    SA_CLS_TAG = ".SECTION voir aussi"

    ##
    ## .cpp files
    ##
    EMPTY_TAG = "XX_TOTO_XX"  # placeholder for text not attached to any tag
    # Outer tags:
    DESC_TAG = "Description:"
    PARAM_TAG = "Parametre:"
    PRE_TAG, POST_TAG = "Precondition:", "Postcondition:"
    EXCEP_TAG = "Exception:"
    BORD_TAG = "Effets de bord:"
    RET_TAG = "Retour:"

    # Collection of top level tags
    OUTER_TAGS = [DESC_CLS_TAG, SA_CLS_TAG, DESC_TAG, PARAM_TAG, PRE_TAG, POST_TAG, EXCEP_TAG, BORD_TAG, RET_TAG]

    # Inner tags:
    SIGNIF_TAG = "Signification:"
    ACCESS_TAG = "Acces:"
    DFLT_TAG = "Valeurs par defaut:"
    CONS_TAG = "Contraintes:"
    INNER_TAGS = [SIGNIF_TAG, ACCESS_TAG, DFLT_TAG, CONS_TAG]

    ACC_IN, ACC_OUT, ACC_IN_OUT = "entree", "sortie", "entree/sortie"

    def _recordEntry(self, currTag, currBuffer, res):
        if currTag in [self.PARAM_TAG, self.RET_TAG]:
            # Parameters/Return are nasty: they have nested tags - recurse!
            resParam = self.processBlock(currBuffer, self.INNER_TAGS)
            # Flatten list of lists:
            for k, v in resParam.items(): resParam[k] = v[0]
            res.setdefault(currTag, []).append(resParam)
        elif currTag in [self.DESC_CLS_TAG, self.DESC_TAG]:
            # All description text (be it class description or method description) are stored under the tag DESC_TAG
            res.setdefault(self.DESC_TAG, []).append(currBuffer)
        elif currTag != "":
            res.setdefault(currTag, []).append(currBuffer)
        else:  # Text attached to no sub tag, for example what is put before "Description:", or just after "Parametre:" before "Signification:"
            res.setdefault(self.EMPTY_TAG, []).append(currBuffer)

    def processBlock(self, blk, tagSet):
        """ Process a block of comments '//' and build a dictionary """
        res = {}
        curr, currT, first = [], "", False
        for lin in blk:
            lin = lin.strip()
            if lin == "": continue
            i = 0
            while i < len(lin) and lin[i] == "/": i+=1
            lin = lin[i:]
            for o in tagSet:
                stripL = lin.strip()
                if stripL.startswith(o):  # Switching tag, record previous entry
                    self._recordEntry(currT, curr, res)
                    curr, currT, first = [], o, True
                    break
            if currT != "":
                if first:
                    lin = stripL[len(currT):]
                    first = False
            curr.append(lin)
        # Last entry:
        self._recordEntry(currT, curr, res)

        return res

    def _generateBrief(self, txtLst):
        """ Extract the first sentence from a list of strings, i.e. 
            - first string ending with a dot '.', 
            - or first two lines (to handle the case of short descriptions cut on two lines ...)
         to generate the 'brief' part of doxygen
        """
        brief, long, lin = [], [], 0
        # Skip empty first lines
        while lin < len(txtLst) and txtLst[lin].strip() == "": lin+=1
        # Take the first line completely or up to the first point to make the 'brief' part
        firstL, lst = txtLst[lin:lin+2], txtLst[lin+2:]
        first = " ".join([f.strip() for f in firstL])
        if "." in first: i = first.index(".")
        else:            i = len(first)
        brief.append(first[:i+1].strip())
        l = first[i+1:].strip()
        if l != "": long.append(l)
        # All the rest goes to long unchanged
        long.extend(lst)
        return brief, long

    def formatToDoxy(self, dicRes):
        """ Format the dictionary to produce a nice Doxygen text 
        """
        sep = "\n * "
        brief, long = self._generateBrief(dicRes[self.DESC_TAG][0])
        brief_desc, long_desc = "", ""
        if brief != []:
            brief_desc = "@brief " + " ".join(brief)  # All on one line for brief description
        if long != []:
            long_desc = " * " + sep.join(long) + "\n *\n"

        # Description
        res = f"{brief_desc}\n *\n{long_desc}"

        # Parameters
        for p in dicRes.get(self.PARAM_TAG,[]):
            l = p.get(self.SIGNIF_TAG, [])
            signif = [s.strip() for s in l if s.strip() != ""]
            et = p.get(self.EMPTY_TAG, [])
            if signif != [] or et not in ([], ['']):
                in_out_k = ""
                if len(p.get(self.ACCESS_TAG, [])): in_out_k = p[self.ACCESS_TAG][0]
                in_out = {self.ACC_IN:"[in]", self.ACC_OUT:"[out]", self.ACC_IN_OUT:"[in,out]"}.get(in_out_k, "")
                if et not in ([], ['']):
                    supp = "(%s) " % p[self.EMPTY_TAG][0]
                else: supp = ""
                param_txt = supp + " ".join(signif)
                res += f" * @param{in_out} {param_txt} \n"

        # Return
        for ret in dicRes.get(self.RET_TAG,[]):
            l = ret.get(self.SIGNIF_TAG, [])
            signif = [s.strip() for s in l if s.strip() != ""]
            et = ret.get(self.EMPTY_TAG, [])
            if signif != [] or et not in ([], ['']):
                if et not in ([], ['']):
                    supp = "(%s) " % ret[self.EMPTY_TAG][0]
                else: supp = ""
                ret_txt = supp + " ".join(signif)
                res += " * @return %s \n" % ret_txt

        # Exceptions
        for ex in dicRes.get(self.EXCEP_TAG,[]):
            ex = [e.strip() for e in ex if e.strip() != ""]
            if ex != []:
                res += " * @throws %s \n" % sep.join(ex)

        # See also
        for sa in dicRes.get(self.SA_CLS_TAG,[]):
            sa = [s.strip() for s in sa if s.strip() != ""]
            if sa != []:
                res += f" * @sa %s \n" % ", ".join(sa)

        # Sometimes we have commented bits of code in the comments ...
        res = res.replace('/*', '//').replace('*/', '//')
        return "/*! " + res + " */\n"

    def convertFile(self, fNameIn, fNameOut):
        """ Main method: convert source file 'fNameIn' and produce 'fNameOut'
        """
        lins = open(fNameIn, "r").readlines()
        louts = []
        blk = []
        inComm = False
        for i, lin in enumerate(lins):
            isCom = lin.strip().startswith("//")
            if isCom:
                inComm = True
                blk.append(lin)
            if not isCom or i == len(lins)-1:  # handle last line too!
                if inComm:
                    inComm = False
                    dicRes = self.processBlock(blk, self.OUTER_TAGS)
                    #print(dicRes)
                    if self.DESC_TAG in dicRes:  # is it a custom doc block ? or just a lambda comment ?
                        res = self.formatToDoxy(dicRes)
                    else:
                        res = blk
                    louts.extend(res)
                    blk = []
                if not isCom or i != len(lins)-1:
                    louts.append(lin)

        with open(fNameOut, "w") as fout:
            fout.write("".join(louts))


def usage():
    print("Usage: trust_doc.py <file_in> <file_out>")
    print("")
    print("Convert the old TRUST documentation into standard Doxygen format")

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        usage()
        sys.exit(-1)
    fNameIn, fNameOut = sys.argv[1], sys.argv[2]
    cv = DocConverter()
    cv.convertFile(fNameIn, fNameOut)
