//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
// TRUST_NO_INDENT
//
#include <verifie_pere.h>
#include <Perte_Charge_Reguliere_VDF_Face.h>
#include <Perte_Charge_Singuliere_VDF_Face.h>
#include <Source_Generique_VDF_Face.h>
#include <Source_Permeabilite_VDF.h>
#include <Terme_Boussinesq_VDF_Face.h>
#include <Terme_Puissance_Thermique_Echange_Impose_P0_VDF.h>
#include <Terme_Source_Acceleration_VDF_Face.h>
#include <Terme_Source_Canal_RANS_LES_VDF_Elem.h>
#include <Terme_Source_Canal_RANS_LES_VDF_Face.h>
#include <Terme_Source_Canal_perio_VDF_Face.h>
#include <Terme_Source_Canal_perio_VDF_P0.h>
#include <Terme_Source_Coriolis_VDF_Face.h>
#include <Terme_Source_Qdm_VDF_Face.h>
#include <Terme_Source_Solide_SWIFT_VDF.h>
#include <Terme_Source_inc_VDF_Face.h>
#include <Terme_Source_inc_th_VDF_Face.h>
void instancie_src_VDF_Sources() {
Cerr << "src_VDF_Sources" << finl;
Perte_Charge_Reguliere_VDF_Face inst1;verifie_pere(inst1);
Perte_Charge_Singuliere_VDF_Face inst2;verifie_pere(inst2);
Source_Generique_VDF_Face inst3;verifie_pere(inst3);
ModPerm_Cte inst4;verifie_pere(inst4);
ModPerm_Carman_Kozeny inst5;verifie_pere(inst5);
ModPerm_ErgunPourDarcy inst6;verifie_pere(inst6);
ModPerm_ErgunPourForch inst7;verifie_pere(inst7);
Terme_Boussinesq_VDF_Face inst8;verifie_pere(inst8);
Terme_Puissance_Thermique_Echange_Impose_P0_VDF inst9;verifie_pere(inst9);
Terme_Source_Acceleration_VDF_Face inst10;verifie_pere(inst10);
Terme_Source_Canal_RANS_LES_VDF_Elem inst11;verifie_pere(inst11);
Terme_Source_Canal_RANS_LES_VDF_Face inst12;verifie_pere(inst12);
Terme_Source_Canal_perio_VDF_Face inst13;verifie_pere(inst13);
Terme_Source_Canal_perio_QC_VDF_Face inst14;verifie_pere(inst14);
Terme_Source_Canal_perio_VDF_Front_Tracking inst15;verifie_pere(inst15);
Terme_Source_Canal_perio_VDF_P0 inst16;verifie_pere(inst16);
Terme_Source_Coriolis_VDF_Face inst17;verifie_pere(inst17);
Terme_Source_Coriolis_QC_VDF_Face inst18;verifie_pere(inst18);
Terme_Source_Qdm_VDF_Face inst19;verifie_pere(inst19);
Terme_Source_Solide_SWIFT_VDF inst20;verifie_pere(inst20);
Terme_Source_inc_VDF_Face inst21;verifie_pere(inst21);
Terme_Source_inc_th_VDF_Face inst22;verifie_pere(inst22);
}
