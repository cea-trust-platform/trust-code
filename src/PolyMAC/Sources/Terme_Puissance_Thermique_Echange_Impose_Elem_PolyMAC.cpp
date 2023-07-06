/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Terme_Puissance_Thermique_Echange_Impose_Elem_PolyMAC.h>
#include <Fluide_Incompressible.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Domaine_PolyMAC.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Terme_Puissance_Thermique_Echange_Impose_Elem_PolyMAC,"Terme_Puissance_Thermique_Echange_Impose_Elem_PolyMAC",Source_base);

Sortie& Terme_Puissance_Thermique_Echange_Impose_Elem_PolyMAC::printOn(Sortie& s ) const
{
  return s << que_suis_je();
}

Entree& Terme_Puissance_Thermique_Echange_Impose_Elem_PolyMAC::readOn(Entree& s )
{
  Param param(que_suis_je());
  param.ajouter("himp",&himp_,Param::REQUIRED);
  param.ajouter("Text",&Text_,Param::REQUIRED);
  param.lire_avec_accolades_depuis(s);
  set_fichier("Terme_Puissance_Thermique_Echange_Impose");
  set_description("Power (W)");
  return s ;
}

void Terme_Puissance_Thermique_Echange_Impose_Elem_PolyMAC::mettre_a_jour(double temps)
{
  const Domaine_VF& domaine = le_dom_PolyMAC.valeur();
  const DoubleVect& volumes = domaine.volumes();
  const DoubleTab& himp = himp_.valeur().valeurs();
  const DoubleTab& Text = Text_.valeur().valeurs();
  const DoubleTab& T = equation().inconnue().valeurs();
  int nb_elem = le_dom_PolyMAC.valeur().nb_elem(), c_h = himp.dimension(0) == 1, c_T = Text.dimension(0) == 1, n, N = T.line_size();

  bilan().resize(N), bilan() = 0;

  for (int e = 0; e < nb_elem; e++)
    for (n = 0; n < N; n++)
      bilan()(n) += himp(!c_h * e, n) * volumes(e) * (Text(!c_T * e, n) - T(e, n));

  himp_.mettre_a_jour(temps);
  Text_.mettre_a_jour(temps);
}


/////////////////////////////////////////////////////////////////////
//
//                    Implementation des fonctions
//
//               de la classe Terme_Puissance_Thermique_Echange_Impose_Elem_PolyMAC
//
////////////////////////////////////////////////////////////////////

void Terme_Puissance_Thermique_Echange_Impose_Elem_PolyMAC::associer_domaines(const Domaine_dis& domaine_dis,
                                                                              const Domaine_Cl_dis& domaine_Cl_dis)
{
  le_dom_PolyMAC = ref_cast(Domaine_PolyMAC, domaine_dis.valeur());
  le_dom_Cl_PolyMAC = ref_cast(Domaine_Cl_PolyMAC, domaine_Cl_dis.valeur());
}


DoubleTab& Terme_Puissance_Thermique_Echange_Impose_Elem_PolyMAC::ajouter(DoubleTab& resu )  const
{
  const Domaine_VF&     domaine               = le_dom_PolyMAC.valeur();
  const DoubleVect& volumes = domaine.volumes();
  const DoubleTab& himp = himp_.valeur().valeurs();
  const DoubleTab& Text = Text_.valeur().valeurs();
  const DoubleTab& T = equation().inconnue().valeurs();
  int nb_elem=le_dom_PolyMAC.valeur().nb_elem(), c_h = himp.dimension(0) == 1, c_T = Text.dimension(0) == 1, n, N = T.line_size();

  for (int e = 0; e < nb_elem; e++)
    for (n = 0; n < N; n++)
      resu(e, n) -= volumes(e) * himp(!c_h * e, n) * (T(e, n) - Text(!c_T * e, n));

  return resu;
}
DoubleTab& Terme_Puissance_Thermique_Echange_Impose_Elem_PolyMAC::calculer(DoubleTab& resu) const
{
  resu=0;
  ajouter(resu);
  return resu;
}
void Terme_Puissance_Thermique_Echange_Impose_Elem_PolyMAC::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  const Domaine_VF&     domaine               = le_dom_PolyMAC.valeur();
  const DoubleVect& volumes = domaine.volumes();
  const DoubleTab& himp = himp_.valeur().valeurs();
  int nb_elem=le_dom_PolyMAC.valeur().nb_elem(), c_h = himp.dimension(0) == 1, n, N = himp.line_size();

  for (int e = 0, i = 0; e < nb_elem; e++)
    for (n = 0; n < N; n++, i++)
      matrice(i, i) += volumes(e) * himp(!c_h * e, n);
}