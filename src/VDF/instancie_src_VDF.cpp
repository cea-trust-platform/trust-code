//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Option_VDF.h>
#include <Traitement_particulier_NS_EC_VDF.h>
#include <Traitement_particulier_NS_Pression_VDF.h>
#include <Traitement_particulier_NS_Profils_VDF.h>
#include <Traitement_particulier_NS_Profils_thermo_VDF.h>
#include <Traitement_particulier_NS_canal_VDF.h>
#include <Traitement_particulier_Solide_canal_VDF.h>
void instancie_src_VDF() {
Cerr << "src_VDF" << finl;
Option_VDF inst1;verifie_pere(inst1);
Traitement_particulier_NS_EC_VDF inst2;verifie_pere(inst2);
Traitement_particulier_NS_Pression_VDF inst3;verifie_pere(inst3);
Traitement_particulier_NS_Profils_VDF inst4;verifie_pere(inst4);
Traitement_particulier_NS_Profils_thermo_VDF inst5;verifie_pere(inst5);
Traitement_particulier_NS_canal_VDF inst6;verifie_pere(inst6);
Traitement_particulier_Solide_canal_VDF inst7;verifie_pere(inst7);
}
