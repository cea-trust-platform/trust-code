from triou import *
for cl in getXClasses():
    if cl.readacc_==0:
        for prop in cl.getAllInitXAttributes():
            if (isinstance(prop,Tr_Attribute) and (prop.optional==1)):
                print "optionel avec readacc=0",cl.name_,prop.name
                pass
            pass
        pass
    else:
        for prop in cl.getAllInitXAttributes():
            if (isinstance(prop,Tr_Attribute) and (prop.optional==0)):
                print "non optionel avec readacc=1",cl.name_,prop.name
                pass
            pass
        pass

    pass
