//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
// TRUST_NO_INDENT
//
#include <verifie_pere.h>
#include <Traitement_particulier_NS_Brech_VEF.h>
#include <Traitement_particulier_NS_EC_VEF.h>
#include <Traitement_particulier_NS_VEF.h>
#include <Traitement_particulier_NS_canal_VEF.h>
#include <Traitement_particulier_NS_chmoy_faceperio_VEF.h>
#include <Traitement_particulier_NS_temperature_VEF.h>
void instancie_src_VEF() {
Cerr << "src_VEF" << finl;
Traitement_particulier_NS_Brech_VEF inst1;verifie_pere(inst1);
Traitement_particulier_NS_EC_VEF inst2;verifie_pere(inst2);
Traitement_particulier_NS_VEF inst3;verifie_pere(inst3);
Traitement_particulier_NS_canal_VEF inst4;verifie_pere(inst4);
Traitement_particulier_NS_chmoy_faceperio_VEF inst5;verifie_pere(inst5);
Traitement_particulier_NS_temperature_VEF inst6;verifie_pere(inst6);
}
