file=open("gencles.py","r")
f2=open("gencl.py","w")
line=file.readline()
f2.write(line)
line=file.readline()
f2.write(line)
line=file.readline()
def add_class(*args,**kwargs):
    # print kwargs
    return kwargs["name"]
    

dico={}
list=[]
for ob in 'suppress_param','objet_u','listobj_impl','chaine','floattant','entier','rien','listentier','listentierf','listchaine','listchainef','list','listf':
    dico[ob]=[1,"yy"]
    pass
while (line):
    # print line

    es=line.replace("gen_class","add_class")
    #  print dir(line)
    exec("name="+es)
    if name in dico.keys():
        if line!=dico[name][1]:
	    message="classe deja dans dico "+name+" avec "+dico[name][1]+"\n et maintenant "+line
            #raise Exception (message)
    	    print "WARNING", message
        pass
    list.append(name)
    dico[name]=[0,line]
    #   f2.write(line)
    line=file.readline()
    
    pass

def get_deps(*args,**kwargs):
    # print kwargs
    deps=[]
    deps.append( kwargs["pere"])
    for mot in kwargs["list_mot"]:
        # print mot
        dep=mot[1].split('(')[0]
        dep=dep.split('ref_')[-1]
        deps.append(dep)
        ret=mot[1].split("default=")
        if (len(ret)>1):
            de=ret[1].split('()')
            
            if (len(de)>1):
                deps.append(de[0])
                # print  kwargs["name"],de[0]
                pass
            pass
        pass
    if "class_type" in kwargs.keys():
        deps.append(kwargs["class_type"])
    return deps

def gen_class(cl,dico):
    val=dico[cl]
    
    if val[0]==-1:
        
        raise Exception ("on cycle "+cl)
    elif val[0]==0:
        # print "debut",cl
        es=val[1].replace("gen_class","get_deps")
        dico[cl]=[-1,val[1]]
        exec("listdeps="+es)
        for cl_d in listdeps:
            if (cl_d!=cl):
                gen_class(cl_d,dico)
                pass
            pass
        f2.write(val[1])
        dico[cl]=[1,val[1]]
        # print "fin",cl
    else:
        #  print cl," deja genere"
        pass
    pass

for cl in list:
    # print 'ici',cl
    gen_class(cl,dico)
   
    pass


print "ok termine ",dico["solve"]
