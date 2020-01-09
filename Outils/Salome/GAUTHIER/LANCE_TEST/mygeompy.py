
# --

from xsalome_pirate import getXSalomeSession
salome = getXSalomeSession(modules=['GEOM','SMESH'], logger=0)

# --

def inBatch():
    if str(__import__).find('import_hook') >= 0:
        return 0
    return 1

# --

if inBatch():
    from batchmode_geompy import *
    def updateStudy():
        return
    pass
else:
    from geompy import *
    def updateStudy():
        salome.sg.updateObjBrowser(1)
        return
    pass

# --

#AJOUT GF


def MakePointStruct(x,y,z):
    return  BasicOp.MakePointXYZ(x,y,z)

def MakeDirection(point):
    OO = BasicOp.MakePointXYZ(0, 0, 0)
    return BasicOp.MakeVectorTwoPnt(OO,point)
def MakeAxisStruct(x0,y0,z0,x,y,z):
    O1=BasicOp.MakePointXYZ(x0, y0, z0)
    point=BasicOp.MakePointXYZ(x0+x,y0+y,z0+z)
    return  BasicOp.MakeVectorTwoPnt(O1,point)


def MakeEllipsoide(x,y,z,r_x,r_y,r_z):
    print("MakeElliposide(x_c,y_c,z_c,r_x,r_y,r_z)")
    r_minor=min(r_x,r_y,r_z)
    r_major=max(r_x,r_y,r_z)
    n_minor=0
    n_major=0
    if (r_minor == r_x):
        n_minor+=1
    elif (r_major == r_x):
        n_major+=1
    else:
        print("r_x!=r_minor!=r_major")
        1/0
        pass
    if (r_minor == r_y):
        n_minor+=1
    elif (r_major == r_y):
        n_major+=1
    else:
        print("r_y!=r_minor!=r_major")
        1/0
        pass
    if (r_minor == r_z):
        n_minor+=1
    elif (r_major == r_z):
        n_major+=1
    else:
        print("r_z!=r_minor!=r_major")
        1/0
        pass

    el=MakeEllipse(MakePointStruct(0,0,0),MakeDirection(MakePointStruct(1,0,0)),r_major,r_minor)
    el=MakeWire([el])
    el=MakeFace(el,1)
    # addToStudy(el,"ellipse")
    if (n_minor==2):
        plane=MakePlane(MakePointStruct(0,2*r_major,0),MakeDirection(MakePointStruct(1,0,0)),4*r_major);
    else:
        plane=MakePlane(MakePointStruct(0,0,2*r_major),MakeDirection(MakePointStruct(1,0,0)),4*r_major);
        pass
    res=MakeCut(el,plane)
    face=SubShapeAll(res,ShapeType["FACE"])[0]
    from math import asin
    pi=asin(1.)*2.
    if (n_minor==2):
        res=MakeRevolution(face,MakeAxisStruct(0,0,0,0,0,1),2*pi)
    else:
        res=MakeRevolution(face,MakeAxisStruct(0,0,0,0,1,0),2*pi)
        pass
    # addToStudy(face,"face")


    # addToStudy(res,"res_intermediaire")
    # dans le cas n_minor==2 on a un ellipse de grand axe en z
    # dans le cas n_major==2 on a un ellipse de petit axe en y
    dir=None
    if (n_minor==2):
        if (r_x==r_major):
            dir=MakeAxisStruct(0,0,0,0,1,0)
        elif (r_y==r_major):
            dir=MakeAxisStruct(0,0,0,1,0,0)
            pass
        pass
    else:
        # rmajor==2
        if (r_x==r_minor):
            dir=MakeAxisStruct(0,0,0,0,0,1)
        elif (r_z==r_minor):
            dir=MakeAxisStruct(0,0,0,1,0,0)
            pass
        pass
    if (dir):
        res=MakeRotation(res,dir,pi/2.)
        pass
    res=MakeTranslation(res,x,y,z)
    # addToStudy(res,"res")
    return res
    pass
