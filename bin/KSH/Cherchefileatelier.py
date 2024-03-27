#!/usr/bin/env python
"""
This script takes as a input a list of header files (.h, .tpp), and will return all the
TRUST cpp files using this include.
Thus, when a header is overloaded in a BALTIK, the corresponding impacted TRUST cpp files 
will also be recompiled.
"""

import os
import sys

ENV=os.getenv("TRUST_ROOT")

def short_path(root_dir, ze_dir):
    return ze_dir.replace(os.path.join(root_dir, ""), "")

# Get list of TRUST source directories:
dirs=[]
src_dir = os.path.join(ENV, "src")
for dirpath, dirnames, filenames in os.walk(src_dir):
    if dirnames:
        dirs.append(short_path(ENV, dirpath))

argv=sys.argv[1:]

i=-1
for j in range(len(argv)):
    if argv[j]=='%%':
        i=j
        break
    pass

if (i>0):
    dirs=argv[:i]
    argv=argv[i+1:]
    pass


def create_dico(dirs):
    import os,re
    pattern="[\s]*#[\s]*include[\s]*<(?P<fi>[\S]+\.h)[\s]*>"
    expr=re.compile(pattern)

    pattern2="[\s]*#[\s]*include[\s]*<(?P<fi>[\S]+\.tpp)[\s]*>"
    expr2=re.compile(pattern2)

    srcs=[]
    for d in dirs:
        root=ENV+"/"+d

        for fi in os.listdir(root):
            fifull=os.path.join(root,fi)
            if os.path.splitext(fi)[1] in [".cxx",".cpp",".h",".hxx",".tpp"]:
                srcs.append((fi,fifull))
                pass
            pass
        pass

    dico={}
    dico_h={}
    dico_cpp={}
    dico_tpp={}
    dico[".h"]=dico_h
    dico[".cpp"]=dico_cpp
    dico[".tpp"]=dico_tpp
    for fi,fifull in srcs:
        # print fifull
        #f=open(fifull,"r")
        f=open(fifull,"r", encoding='utf-8')

        lines=f.readlines()
        lines1=list(filter(expr2.match,lines))
        res=[expr2.match(line).group("fi") for line in lines1]
        for r in res:
            titi=os.path.splitext(fi)[1]
            f=fi
            if (titi==".cpp"): f=fifull
            try:
                dico[titi][r].append(f)
            except:
                dico[titi][r]=[f]
        lines2=list(filter(expr.match,lines))
        res2=[expr.match(line).group("fi") for line in lines2]
        for r in res2:
            titi=os.path.splitext(fi)[1]
            f=fi
            if (titi==".cpp"): f=fifull
            try:
                dico[titi][r].append(f)
            except:
                dico[titi][r]=[f]

    return [dico_tpp,dico_h,dico_cpp]


#print "IIIII",sys.argv
files_in=[]
files_cpp=[]

for fil in argv:
    file2=os.path.basename(fil)
    if (file2[-2:]==".h"):
        files_in.append(file2)
    elif (file2[-4:]==".tpp"):
        files_in.append(file2)
    else:
        files_cpp.append(file2)
        pass
    pass
#print files_in

if (len(files_in)):
    dico=create_dico(dirs)
else:
    import sys
    sys.exit(0)
    pass

lena=1
while (lena):
    files_h=[]
    for file2 in files_in:
        try:
            for f in dico[0][file2]:
                if not f in files_in:
                    files_h.append(f)
        except KeyError:
            pass
    for file2 in files_in:
        try:
            for f in dico[1][file2]:
                if not f in files_in:
                    files_h.append(f)
        except KeyError:
            pass
    lena=len(files_h)
    for f in files_h:    files_in.append(f)
    # print files_in
    pass

#print files_in
lcpp=[]
for f in files_in :
    try :
        for cpp in dico[1][f]:
            if not(cpp in lcpp):
                namecpp=os.path.basename(cpp)
                if not (namecpp in files_cpp) and (namecpp[-4:]==".cpp"):
                    lcpp.append(cpp) # .replace(ENV,"$TRUST_ROOT"))
                else:
                    # print namecpp," dans l'atelier"
                    pass
            pass
        pass
    except KeyError:
        pass

for f in files_in :
    try :
        for cpp in dico[2][f]:
            if not(cpp in lcpp):
                namecpp=os.path.basename(cpp)
                if not (namecpp in files_cpp):
                    lcpp.append(cpp) # .replace(ENV,"$TRUST_ROOT"))
                else:
                    # print namecpp," dans l'atelier"
                    pass
            pass
        pass
    except KeyError:
        pass
#lcpp=[lcpp[1]]
# GF on remet le chemin en dur pour cmake 3.
sys.stdout.write(';'.join(lcpp))        # .replace(ENV,"${TRUST_ROOT}"))
#print ''
if 0:
    for c in lcpp:
        cmd="ln "+ c + " ."
        print(cmd)

#print len(lcpp)
