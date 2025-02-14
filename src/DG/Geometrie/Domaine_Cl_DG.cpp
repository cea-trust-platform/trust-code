/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Dirichlet_entree_fluide_leaves.h>
#include <Champ_front_softanalytique.h>
#include <Dirichlet_paroi_defilante.h>
#include <Dirichlet_paroi_fixe.h>
#include <Discretisation_base.h>
#include <Domaine_Cl_DG.h>
//#include <Champ_Face_DG.h>
#include <Dirichlet_homogene.h>
#include <Champ_Inc_P0_base.h>
#include <Domaine_DG.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Matrice_Morse.h>
#include <Periodique.h>
#include <Symetrie.h>
#include <Debog.h>

Implemente_instanciable(Domaine_Cl_DG, "Domaine_Cl_DG", Domaine_Cl_dis_base);

Sortie& Domaine_Cl_DG::printOn(Sortie& os) const { return os; }

Entree& Domaine_Cl_DG::readOn(Entree& is) { return Domaine_Cl_dis_base::readOn(is); }

/*! @brief Need to investigate
 *
 */
void Domaine_Cl_DG::completer(const Domaine_dis_base& )
{
  modif_perio_fait_ = 0;
}


/*! @brief Need to investigate: a priori, nothing to do for DG
 *
 */
void Domaine_Cl_DG::imposer_cond_lim(Champ_Inc_base& ch, double temps)
{
//  DoubleTab& ch_tab = ch.valeurs(temps);
  if (sub_type(Champ_Inc_P0_base, ch)) { /* Do nothing */ }
  else if (ch.nature_du_champ() == scalaire) { /* Do nothing */ }
  else
    {
      Cerr << "Le type de Champ_Inc " << ch.que_suis_je() << " n'est pas prevu en DG family " << finl;
      Process::exit();
    }
//  ch_tab.echange_espace_virtuel();
//  Debog::verifier("Domaine_Cl_DG::imposer_cond_lim ch_tab", ch_tab);
}

int Domaine_Cl_DG::nb_faces_sortie_libre() const
{
  Process::exit("Function Domaine_Cl_DG::nb_faces_sortie_libre should not be used for DG");
  return -1000000;
}

int Domaine_Cl_DG::nb_bord_periodicite() const
{
  int compteur = 0;
  for (int cl = 0; cl < les_conditions_limites_.size(); cl++)
    {
      if (sub_type(Periodique, les_conditions_limites_[cl].valeur()))
        compteur++;
    }
  return compteur;
}

int Domaine_Cl_DG::initialiser(double temps)
{
  Domaine_Cl_dis_base::initialiser(temps);

  if (nb_bord_periodicite() > 0)
      Process::exit("Periodic conditions are not coded yet for DG");
  return 1;
}

Domaine_VF& Domaine_Cl_DG::domaine_vf()
{
  return ref_cast(Domaine_VF, domaine_dis());
}

const Domaine_VF& Domaine_Cl_DG::domaine_vf() const
{
  return ref_cast(Domaine_VF, domaine_dis());
}
