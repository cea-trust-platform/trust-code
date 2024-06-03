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

#include <Courant_maille_Champ_Face.h>
#include <Schema_Temps_base.h>
#include <Champ_Face_VDF.h>
#include <Domaine_VDF.h>

Implemente_instanciable(Courant_maille_Champ_Face,"Courant_maille_Champ_Face",Champ_Fonc_Face_VDF);

Sortie& Courant_maille_Champ_Face::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Courant_maille_Champ_Face::readOn(Entree& s) { return s ; }

void Courant_maille_Champ_Face::associer_champ(const Champ_Face_VDF& la_vitesse, const Schema_Temps_base& sch)
{
  vitesse_ = la_vitesse;
  sch_ = sch;
}

// XXX : Elie Saikali : pas utilise ... commente
// Methode de calcul de la valeur sur une face d'un champ uniforme ou non a plusieurs composantes
//inline double valeur(const DoubleTab &champ, const int face, const int compo, const Domaine_VDF &le_dom_VDF)
//{
//  if (champ.dimension(0) == 1) return champ(0, compo); // Champ uniforme
//  else
//    {
//      int elem0 = le_dom_VDF.face_voisins(face, 0);
//      int elem1 = le_dom_VDF.face_voisins(face, 1);
//      if (elem1 < 0) elem1 = elem0; // face frontiere
//      return 0.5 * (champ(elem0, compo) + champ(elem1, compo));
//    }
//}

void Courant_maille_Champ_Face::mettre_a_jour(double tps)
{
  const int nb_faces = domaine_vdf().nb_faces();
  DoubleTab& co = valeurs(); // Courant de maille
  double dt = sch_->pas_de_temps();
  for (int face = 0; face < nb_faces; face++)
    {
      // Calcul de la taille de maille entourant la face
      double taille_maille = domaine_vdf().volumes_entrelaces()(face) / domaine_vdf().face_surfaces(face);
      // Calcul du Courant de maille
      co(face) = std::fabs(vitesse_->valeurs()(face)) * dt / taille_maille; // Courant_maille = |Uface| * dt / taille_maille
    }

  changer_temps(tps);
  Champ_Fonc_base::mettre_a_jour(tps);
}
