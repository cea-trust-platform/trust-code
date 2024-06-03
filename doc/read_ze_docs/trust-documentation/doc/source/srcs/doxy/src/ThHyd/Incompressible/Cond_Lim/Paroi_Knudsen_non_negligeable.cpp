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

#include <Paroi_Knudsen_non_negligeable.h>
#include <Champ_front_fonc_gradient.h>
#include <Discretisation_base.h>

Implemente_instanciable(Paroi_Knudsen_non_negligeable, "Paroi_Knudsen_non_negligeable", Dirichlet_paroi_defilante);

Sortie& Paroi_Knudsen_non_negligeable::printOn(Sortie& s) const { return s << que_suis_je() << finl; }

Entree& Paroi_Knudsen_non_negligeable::readOn(Entree& s)
{
  if (app_domains.size() == 0) app_domains = { Motcle("Hydraulique"), Motcle("indetermine") };

  Cerr << "Paroi_Knudsen_non_negligeable::readOn" << finl;
  Motcle mot;
  Motcles les_motcles(3);
  {
    les_motcles[0] = "vitesse_paroi";
    les_motcles[1] = "k";
  }
  int nb_champs_lus = 0;
  while (nb_champs_lus < 2)
    {
      s >> mot;
      int rang = les_motcles.search(mot);
      if (rang == 0)
        s >> vitesse_paroi_;
      if (rang == 1)
        s >> k_;
      nb_champs_lus++;
    }

  le_champ_front.typer("Champ_front_fonc_gradient");
  le_champ_front->fixer_nb_comp(dimension);
  return s;
}

void Paroi_Knudsen_non_negligeable::completer()
{
  Cerr << "Paroi_Knudsen_non_negligeable::completer" << finl;
  Nom type = "Champ_front_fonc_gradient_";
  type += domaine_Cl_dis().equation().discretisation().que_suis_je();
  // Typage definitif en fonction de la discretisation
  Frontiere_dis_base& fr = le_champ_front.frontiere_dis();
  le_champ_front.typer(type);
  le_champ_front.associer_fr_dis_base(fr);
  // Paroi defilante : le champ_front est la vitesse de nombre
  // de composantes la dimension du pb ... Est ce utile de dimensionner
  // maintenant cela ?
  le_champ_front->fixer_nb_comp(dimension);
  // On associe l'inconnue:
  Champ_front_fonc_gradient& ch = ref_cast(Champ_front_fonc_gradient, le_champ_front.valeur());
  ch.associer_ch_inc_base(domaine_Cl_dis().equation().inconnue().valeur());
  Cerr << "Paroi_Knudsen_non_negligeable::completer OK" << finl;
}
