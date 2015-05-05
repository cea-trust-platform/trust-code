from upper_plenum3D import list_grp,geom,calcul,id_calcul,defaut
print list_grp

import smesh
tetra=smesh.Mesh(calcul,"essai")




# balayage des groupes --------------
for grp in list_grp:
    tetra.Group(grp)
    pass
import salome
import NETGENPlugin 
a=tetra.Netgen(True)
p=a.Parameters()
p.SetFineness(0)
#p.SetMaxSize(0.3)
tetra.Compute()
tetra_mesh = tetra.GetMesh()
tetra_edit = tetra_mesh.GetMeshEditor()
tetra_noeuds = tetra_edit.FindCoincidentNodes(1.e-5)
tetra_edit.MergeNodes(tetra_noeuds)

from os import system
system("rm -f ess.med mod_ess.med ess2d.med")
tetra.ExportMED("ess.med")
from change_name_fam import change_name_fam
change_name_fam("ess.med")
tetra.ExportUNV("ess.unv")
print "ok"
#system("$exec check")

