//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Iterateur_Source_EF.h>
#include <Puissance_Thermique_EF.h>
#include <Source_Qdm_EF.h>
#include <Source_Scalaire_EF.h>
void instancie_src_EF_Sources() {
Cerr << "src_EF_Sources" << finl;
Iterateur_Source_EF inst1;verifie_pere(inst1);
Puissance_Thermique_EF inst2;verifie_pere(inst2);
Source_Qdm_EF inst3;verifie_pere(inst3);
Source_Scalaire_EF inst4;verifie_pere(inst4);
}
