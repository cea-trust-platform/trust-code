import os
ENV=os.getenv("TRUST_ROOT")

dirs=[]
f=open(ENV+"/env/rep.TRUST","r")
titi=1

while (titi):
    titi=f.readline().strip()
    if (titi!=''):    dirs.append(titi)
    pass



import sys

argv=sys.argv[1:]
# print argv
# sys.stderr.write(' '.join(argv))

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
#print dirs,"oooo", argv



def create_dico(dirs):
    import os,re
    pattern="[\s]*#[\s]*include[\s]*<(?P<fi>[\S]+\.h)[\s]*>"
    expr=re.compile(pattern)

    srcs=[]
    for d in dirs:
        root=ENV+"/"+d

        for fi in os.listdir(root):
            fifull=os.path.join(root,fi)
            if os.path.splitext(fi)[1] in [".cxx",".cpp",".h",".hxx"]:
                srcs.append((fi,fifull))
                pass
            pass
        pass

    dico={}
    dico_h={}
    dico_cpp={}
    dico[".h"]=dico_h
    dico[".cpp"]=dico_cpp
    for fi,fifull in srcs:
        # print fifull
        f=open(fifull,"r")

        lines=f.readlines()
        lines=list(filter(expr.match,lines))
        res=[expr.match(line).group("fi") for line in lines]
        for r in res:
            titi=os.path.splitext(fi)[1]
            f=fi
            if (titi==".cpp"): f=fifull
            try:
                dico[titi][r].append(f)
            except:
                dico[titi][r]=[f]
                pass


        pass

    return [dico_h,dico_cpp]
def create_dico_old(dirs):
    import os
    f=open('deps','w')
    f.close()
    for d0 in dirs:

        d=ENV+"/"+d0
        # d=d0
        cmd="egrep  '^#include' "+d+"/*.cpp "+d+"/*.h >>deps 2>/dev/null"
        # print cmd
        res=os.system(cmd)
        # print "res",res
        # print dir(res)



        pass



    f=open("deps","r")
    titi=f.readline()
    dico_h={}
    dico_cpp={}
    dico=[dico_h,dico_cpp]
    while (titi):


        r=titi.split(':')
        r2=r[1].split('<')[1].split('>')[0].strip()
        file=r[0]
        ind=0
        if (file[-4:]==".cpp"):
            ind=1
        else:
            file=file.split('/')[-1]
            pass
        # print file2 , " iiiiiii",r2
        try:
            dico[ind][r2].append(file)
        except KeyError:
            dico[ind][r2]=[file]
            pass

        titi=f.readline()
        pass

    os.remove("deps")
    # print dico
    return dico




#print "IIIII",sys.argv
files_in=[]
files_cpp=[]

for fil in argv:
    file2=os.path.basename(fil)
    if (file2[-2:]==".h"):
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
                    pass
                pass
        except KeyError:
            pass
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
