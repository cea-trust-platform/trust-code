#!/usr/bin/env python3
"""
A script to move XD tags from the TRAD_2.org file into the relevant CPP files!
"""

import os, glob, re

class TR2Loader(object):
    def __init__(self):
        tr = os.environ["TRUST_ROOT"]
        self.trad2_pth = os.path.join(tr, "Outils", "TRIOXDATA", "XTriou", "TRAD_2.org")
        self.trad2 = []
        self.trad2_lines = {}  # Key: a (lowercase) class name, value: (k, l) start and end line of the block in TRAD_2.org file
        self.found = set()      # Keys found in TRAD2_org file and moved to CPP
        exp = r"implemente_[^(]+\([^,]+,[ ]*\"([^,]+)\",[^)]+\)"
        self.ce = re.compile(exp, re.IGNORECASE)
        self.dbg = False

    def loadTRAD2_org(self):
        """ Load the TRAD_2.org file internally.
        """
        with open(self.trad2_pth) as f:
            self.trad2 = f.readlines()
            cnt = 0
            curr, currL = "", -1
            for l in self.trad2 + [""]:
                ll = l.strip()
                if not ll.startswith("attr"):
                    if currL != -1:  # not first pass
                        # print(f"Registering {curr} in ({currL}, {cnt})")
                        self.trad2_lines[curr] = (currL, cnt)
                    curr = ll.split(' ')[0].lower()
                    currL = cnt
                cnt = cnt+1

    def _completeOut(self, lin, lout):
        """ Complete CPP file with part of the text from TRAD_2.org
        """
        m = self.ce.match(lin)
        if self.dbg:
          print("about to match?")
          print(lin)
        if m:
            if self.dbg: print("@@@@@@@ MATCH")
            cls_n = m.group(1)
            if self.dbg: print(cls_n)
            for a in cls_n.split("|"):
                a = a.lower()
                if a in self.trad2_lines:
                    # print(f"Class {a} match!!")
                    self.found.add(a)
                    lstart, lend = self.trad2_lines[a]
                    slc = self.trad2[lstart:lend]
                    for s in slc:
                        lout.append("// XD " + s.strip() + "\n")
                    lout.append("\n")


    def processFiles(self):
        """ Process all CPP files in TRUST to see if XD tags can be moved from TRAD_2.org into the CPP file itself
        """
        tr = os.environ["TRUST_ROOT"]
        src = os.path.join(tr, "src", "**", "*.cpp")
        # Load all .cpp files having a "implemente_instanciable" directive
        files = glob.glob(src, recursive=True)

        for fn in files:
            # if "Champ_Don_base." in fn:
            #   print(f"Handling file {fn}")
            #   self.dbg = True
            # else:
            #   self.dbg = False
            with open(fn, "r") as f:
                lins = f.readlines()
                lout = []
                for l in lins:
                    lout.append(l)
                    ll = l.strip().lower()
                    if "implemente_" in ll:
                        self._completeOut(ll, lout)
            ###
            #continue
            ##
            if len(lins) != len(lout):
                with open(fn, "w") as f:
                    print(f"Re-writing file {fn} ...")
                    f.write(''.join(lout))

    def writeNewTrad2(self):
        """ Re-write TRAD_2.org removing what has been moved into the CPP files
        """
        skip, newData = [], []
        for f in self.found:
            ls, le = self.trad2_lines[f]
            skip.append((ls, le))
        skip.sort()
        newData = self.trad2[:skip[0][0]]
        prev = skip[0][1]
        for ss, se in skip:
            newData.extend(self.trad2[prev:ss])
            prev = se
        newData.extend(self.trad2[se:])

        with open(self.trad2_pth, "w") as f:
            f.write(''.join(newData))

if __name__ == "__main__":
    import os
    if "TRUST_ROOT" not in os.environ:
        raise ValueError("TRUST env not loaded??")
    trl = TR2Loader()
    print("Loading TRAD_2.org ...")
    trl.loadTRAD2_org()
    print("Processing cpp files ...")
    trl.processFiles()
    print("Re-writing TRAD_2.org ...")
    trl.writeNewTrad2()
    print("All done!")

