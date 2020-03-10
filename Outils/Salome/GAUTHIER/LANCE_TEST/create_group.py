import mygeompy
geom=mygeompy
def create_defaut(name,calcul,list_grp_def,type="FACE"):
    if type=="FACE":
        all_faces_id=geom.GetFreeFacesIDs(calcul)
        # print all_faces_id

        all_faces= [ geom.GetSubShape(calcul,[ x ] ) for x in all_faces_id ]
        # print all_faces

    else:
        all_faces=geom.SubShapeAll(calcul, geom.ShapeType[type])
        pass
    defaut=create_group_from(name,calcul,all_faces,type)
    for grp in list_grp_def:
        for id in grp.GetSubShapeIndices():
            geom.RemoveObject(defaut,id)
            pass
        pass
    list_grp_def.append(defaut)
    return defaut,list_grp_def
def create_defaut_old(name,calcul,list_grp_def,type="FACE"):
    defaut = geom.CreateGroup(calcul, geom.ShapeType[type])
    all_faces=geom.SubShapeAllSorted(calcul, geom.ShapeType[type])
    all_faces_id=[]
    for face in all_faces:
        all_faces_id.append(geom.GetSubShapeID(calcul,face))
        pass
    for grp in list_grp_def:
        for id in grp.GetSubShapeIndices():
            all_faces_id.remove(id)
            pass
        pass
    for id in all_faces_id:
        geom.AddObject(defaut, id)
        pass
    defaut.SetName(name)
    idd=geom.addToStudyInFather(calcul,defaut,name)

    list_grp_def.append(defaut)
    return defaut,list_grp_def
def create_group_from(name,calcul,list_elem,type="FACE"):
    """ cree le groupe de nnom name a partir des shape de list_elem sur calcul"""
    new=geom.CreateGroup(calcul, geom.ShapeType[type])
    geom.UnionList(new, list_elem)
    new.SetName(name)
    idd=geom.addToStudyInFather(calcul,new,name)
    return new

def create_bord_near(name,calcul,pos,list_grp_def,type="FACE"):
    """ cree le groupe de nnom name sur la face proche de  x,y,z sur calcul"""
    list_face=[]
    for vec in pos:
        if type=="FACE":
            face=geom.GetFaceNearPoint(calcul,geom.MakePointStruct(vec[0],vec[1],vec[2]))
        else:
            face=geom.GetEdgeNearPoint(calcul,geom.MakePointStruct(vec[0],vec[1],vec[2]))
            pass
        list_face.append(face)
        print(face)
        pass
    new=create_group_from(name,calcul,list_face,type)
    list_grp_def.append(new)
    return new,list_grp_def
