//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Ch_front_input_P1.h>
#include <Champ_Post_Operateur_Eqn.h>
#include <Champ_som_lu.h>
void instancie_src_Kernel_Champs_dis()
{
  Cerr << "src_Kernel_Champs_dis" << finl;
  Ch_front_input_P1 inst1;
  verifie_pere(inst1);
  Champ_Post_Operateur_Eqn inst2;
  verifie_pere(inst2);
  Champ_som_lu inst3;
  verifie_pere(inst3);
}
