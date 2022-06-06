/****************************************************************************
* Copyright (c) 2022, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

#ifndef Champ_val_tot_sur_vol_base_included
#define Champ_val_tot_sur_vol_base_included

#include <Champ_Uniforme_Morceaux.h>

class Zone_dis_base;
class Zone_Cl_dis_base;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Champ_val_tot_sur_vol_base
//     Classe de base derivee de Champ_Uniforme_Morceaux qui represente les champs
//     dont on veut une evaluation exprimee par val_lue_loc/(Somme_vol_poro_loc)
//
//     val_lue_loc designe la valeur lue pour une localisation de l espace (sous zone ou domaine par defaut)
//     cas VDF : Somme_vol_poro_loc exprime la sommation de vol_element*poro_volumique
//               pour les elements contenus dans la localisation loc
//     cas VEF : Somme_vol_poro_loc exprime la sommation de vol_entrelaces*poro_surface
//                 pour les faces contenues dans la localisation loc
//     Somme_vol_poro_loc est evalue par la methode eval_contrib_loc() des classes derivees
//
// La syntaxe utilisateur a respecter est la suivante :
//               Valeur_totale_sur_volume nom_domaine nb_comp { defaut val_lue_dom ...zonei val_lue_szi ... }
//               nom_domaine : nom du domaine de calcul
//                 nb_comp     : nombre de composantes du champ
//                 val_lue_dom : valeur lue pour le domaine par defaut
//                 val_lue_szi : valeur lue pour la sous zone zonei
//
//
//////////////////////////////////////////////////////////////////////////////
class Champ_val_tot_sur_vol_base : public Champ_Uniforme_Morceaux
{

  Declare_base(Champ_val_tot_sur_vol_base);

public:

  void evaluer(const Zone_dis_base& zdis,const Zone_Cl_dis_base& zcldis);
  virtual DoubleVect& eval_contrib_loc(const Zone_dis_base& zdis,const Zone_Cl_dis_base& zcldis,
                                       DoubleVect& vol)=0;
};

#endif
