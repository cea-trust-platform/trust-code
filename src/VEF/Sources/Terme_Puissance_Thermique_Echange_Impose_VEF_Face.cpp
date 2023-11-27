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

#include <Terme_Puissance_Thermique_Echange_Impose_VEF_Face.h>
#include <Fluide_Incompressible.h>
#include <Discretisation_base.h>
#include <Probleme_base.h>
#include <Domaine_VEF.h>
#include <Champ_P1NC.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Terme_Puissance_Thermique_Echange_Impose_VEF_Face,"Terme_Puissance_Thermique_Echange_Impose_VEF_P1NC",Source_base);

Sortie& Terme_Puissance_Thermique_Echange_Impose_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je();
}

Entree& Terme_Puissance_Thermique_Echange_Impose_VEF_Face::readOn(Entree& s )
{
  Param param(que_suis_je());
  param.ajouter("himp",&himp_,Param::REQUIRED);
  param.ajouter("Text",&Text_,Param::REQUIRED);
  param.lire_avec_accolades_depuis(s);
  set_fichier("Terme_Puissance_Thermique_Echange_Impose");
  set_description("Power (W)");
  return s ;
}

void Terme_Puissance_Thermique_Echange_Impose_VEF_Face::mettre_a_jour(double temps)
{
  int nb_faces = le_dom_VEF.valeur().nb_faces();
  const Domaine_VEF& domaine = ref_cast(Domaine_VEF, le_dom_VEF.valeur());
  const IntTab& face_voisins = domaine.face_voisins();
  const DoubleVect& volumes_entrelaces = domaine.volumes_entrelaces();
  const DoubleTab& himp = himp_.valeur().valeurs();
  const DoubleTab& Text = Text_.valeur().valeurs();
  const DoubleTab& T = equation().inconnue().valeurs();
  const DoubleVect& volumes_elements = domaine.volumes();

  bilan()(0) = 0;

  for (int num_face = 0; num_face < nb_faces; num_face++)
    {
      const double vol = volumes_entrelaces(num_face);


      const int elem0 = face_voisins(num_face,0);
      const int elem1 = face_voisins(num_face,1);
      double h0 = 0, h1 = 0, text0 = 0, text1 = 0, vol0 = 0, vol1 = 0;
      if (elem0 >= 0)
        {
          h0 = himp.addr()[elem0 * (himp.dimension(0) > 1)];
          text0 = Text.addr()[elem0 * (Text.dimension(0) > 1)];
          vol0 = volumes_elements(elem0);
        }
      if (elem1 >= 0)
        {
          h1 = himp.addr()[elem1 * (himp.dimension(0) > 1)];
          text1 = Text.addr()[elem1 * (Text.dimension(0) > 1)];
          vol1 = volumes_elements(elem1);
        }
      double hm = (h0 * vol0 + h1 * vol1) / (vol0 + vol1);
      double htextm = (h0 * text0 * vol0 + h1 * text1 * vol1) / (vol0 + vol1);

      double c = (domaine.faces_doubles()[num_face]==1) ? 0.5 : 1 ;
      bilan()(0) -= c * (hm*T(num_face)-htextm) * vol;

    }
  himp_.mettre_a_jour(temps);
  Text_.mettre_a_jour(temps);
}

void Terme_Puissance_Thermique_Echange_Impose_VEF_Face::associer_domaines(const Domaine_dis& domaine_dis,
                                                                          const Domaine_Cl_dis& domaine_Cl_dis)
{
  Cerr << " Terme_Puissance_Thermique_Echange_Impose_VEF_Face::associer_domaines " << finl ;
  le_dom_VEF = ref_cast(Domaine_VEF, domaine_dis.valeur());
  le_dom_Cl_VEF = ref_cast(Domaine_Cl_VEF, domaine_Cl_dis.valeur());
}


DoubleTab& Terme_Puissance_Thermique_Echange_Impose_VEF_Face::ajouter(DoubleTab& resu )  const
{
  int nb_faces=le_dom_VEF.valeur().nb_faces();
  const Domaine_VF&     domaine               = le_dom_VEF.valeur();
  const IntTab&      face_voisins       = domaine.face_voisins();
  const DoubleVect& volumes_entrelaces = domaine.volumes_entrelaces();
  const DoubleTab& himp = himp_.valeur().valeurs();
  const DoubleTab& Text = Text_.valeur().valeurs();

  const DoubleTab& T = equation().inconnue().valeurs();
  const DoubleVect& volumes_elements = domaine.volumes();

  for (int num_face=0; num_face<nb_faces; num_face++)
    {
      const double vol = volumes_entrelaces(num_face);


      const int elem0 = face_voisins(num_face,0);
      const int elem1 = face_voisins(num_face,1);
      double h0 = 0, h1 = 0, text0=0, text1=0,vol0 = 0, vol1 = 0;
      if (elem0 >= 0)
        {
          h0 = himp.addr()[elem0 * (himp.dimension(0) > 1)];
          text0 = Text.addr()[elem0 * (Text.dimension(0) > 1)];
          vol0 = volumes_elements(elem0);
        }
      if (elem1 >= 0)
        {
          h1 = himp.addr()[elem1 * (himp.dimension(0) > 1)];
          text1 = Text.addr()[elem1 * (Text.dimension(0) > 1)];
          vol1 = volumes_elements(elem1);
        }
      double hm = (h0 * vol0 + h1 * vol1) / (vol0 + vol1);
      double htextm = (h0 * text0 * vol0 + h1 * text1 * vol1) / (vol0 + vol1);

      resu(num_face) -= (hm*T(num_face)-htextm) * vol;

    }


  return resu;
}
DoubleTab& Terme_Puissance_Thermique_Echange_Impose_VEF_Face::calculer(DoubleTab& resu) const
{
  resu=0;
  ajouter(resu);
  return resu;
}
void Terme_Puissance_Thermique_Echange_Impose_VEF_Face::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{


  int nb_faces=le_dom_VEF.valeur().nb_faces();
  const Domaine_VF&     domaine               = le_dom_VEF.valeur();
  const IntTab&      face_voisins       = domaine.face_voisins();
  const DoubleVect& volumes_entrelaces = domaine.volumes_entrelaces();
  const DoubleTab& himp = himp_.valeur().valeurs();
  // const DoubleTab& Text = Text_.valeur().valeurs();
  // const DoubleTab& T = equation().inconnue().valeurs();
  const DoubleVect& volumes_elements = domaine.volumes();




  for (int num_face=0; num_face<nb_faces; num_face++)
    {
      const double vol = volumes_entrelaces(num_face);


      const int elem0 = face_voisins(num_face,0);
      const int elem1 = face_voisins(num_face,1);
      double h0 = 0, h1 = 0,vol0 = 0, vol1 = 0;
      if (elem0 >= 0)
        {
          h0 = himp.addr()[elem0 * (himp.dimension(0) > 1)];
          //  text0 = text(elem0);
          vol0 = volumes_elements(elem0);
        }
      if (elem1 >= 0)
        {
          h1 = himp.addr()[elem1 * (himp.dimension(0) > 1)];
          // text1 = text(elem0);
          vol1 = volumes_elements(elem1);
        }
      double hm = (h0 * vol0 + h1 * vol1) / (vol0 + vol1);
      // double textm = (text0 * vol0 + text1 * vol1) / (vol0 + vol1);

      matrice(num_face,num_face) += hm* vol;

    }
}

int Terme_Puissance_Thermique_Echange_Impose_VEF_Face::initialiser(double temps)
{
  Nom nom_Himp = himp_->le_nom() != "??" ? himp_->le_nom() : "Himp";
  equation().discretisation().nommer_completer_champ_physique(equation().domaine_dis(), nom_Himp, "", himp_.valeur(), equation().probleme());
  Nom nom_Text = Text_->le_nom() != "??" ? Text_->le_nom() : "Text";
  equation().discretisation().nommer_completer_champ_physique(equation().domaine_dis(), nom_Text, "", Text_.valeur(), equation().probleme());
  himp_->initialiser(temps);
  Text_->initialiser(temps);
  return Source_base::initialiser(temps);
}
