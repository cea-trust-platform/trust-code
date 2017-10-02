import MEDLoader as mc

file_name = "etat_interm_3D_cas_test.med"

f = mc.ReadFieldCell(file_name, "maillage_2D", 0, "VITESSE_ELEM_dom_0_3D", 0, -1)
f.writeVTK("etat_interm.vtu")

