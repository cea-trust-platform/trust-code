/****************************************************************************
* Copyright (c) 2015, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Reynolds_maille_Champ_Face.cpp
// Directory:   $TRUST_ROOT/src/VDF/Champs
// Version:     /main/4
//
//////////////////////////////////////////////////////////////////////////////

#include <Reynolds_maille_Champ_Face.h>
#include <Zone_VDF.h>
#include <Champ_Face.h>
#include <Champ_Don.h>

Implemente_instanciable(Reynolds_maille_Champ_Face,"Reynolds_maille_Champ_Face",Champ_Fonc_Face);


//     printOn()
/////

Sortie& Reynolds_maille_Champ_Face::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Reynolds_maille_Champ_Face::readOn(Entree& s)
{
  return s ;
}

void Reynolds_maille_Champ_Face::associer_champ(const Champ_Face& la_vitesse, const Champ_Don& la_viscosite_cinematique)
{
  vitesse_= la_vitesse;
  nu_ = la_viscosite_cinematique;
}

// Methode de calcul de la valeur sur une face d'un champ uniforme ou non a plusieurs composantes
inline double valeur(const DoubleTab& champ, const int& face, const int& compo, const Zone_VDF& la_zone_VDF)
{
  if (champ.dimension(0)==1)
    return champ(0,compo); // Champ uniforme
  else
    {
      int elem0 = la_zone_VDF.face_voisins(face,0);
      int elem1 = la_zone_VDF.face_voisins(face,1);
      if (elem1<0) elem1 = elem0; // face frontiere
      if (champ.nb_dim()==1)
        return 0.5*(champ(elem0)+champ(elem1));
      else
        return 0.5*(champ(elem0,compo)+champ(elem1,compo));
    }
}

void Reynolds_maille_Champ_Face::mettre_a_jour(double tps)
{
  const int& nb_faces = zone_vdf().nb_faces();
  DoubleTab& re = valeurs(); // Reynolds de maille
  // Boucle sur les faces
  for (int face=0; face<nb_faces; face++)
    {
      // Calcul de la viscosite face
      double nu_face = valeur(nu_->valeurs(), face, 0, zone_vdf());
      // Calcul de la taille de maille entourant la face
      double taille_maille = zone_vdf().volumes_entrelaces()(face) / zone_vdf().face_surfaces(face);
      // Calcul du Reynolds de maille
      re(face) = dabs(vitesse_->valeurs()(face)) * taille_maille / nu_face; // Reynolds_maille = |Uface| * taille_maille / nu_face
    }

  changer_temps(tps);
  Champ_Fonc_base::mettre_a_jour(tps);
}

