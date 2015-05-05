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
// File:        Champ_front_fonc_gradient_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Champs
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////


#include <Champ_front_fonc_gradient_VEF.h>
#include <Zone_VEF.h>
#include <DoubleTrav.h>
#include <Champ_Inc.h>

Implemente_instanciable(Champ_front_fonc_gradient_VEF,"Champ_front_fonc_gradient_VEF",Champ_front_fonc_gradient);


Sortie& Champ_front_fonc_gradient_VEF::printOn(Sortie& os) const
{
  return Champ_front_fonc_gradient::printOn(os);
}


Entree& Champ_front_fonc_gradient_VEF::readOn(Entree& s)
{
  fixer_nb_comp(dimension);
  return s;
}

// Description:
// Mise a jour du temps

void Champ_front_fonc_gradient_VEF::mettre_a_jour(double temps)
{
  Cerr << "Champ_front_fonc_gradient_VEF::mettre_a_jour" << finl;
  Cerr << "Non encore code..." << finl;
  exit();

  const Zone_VEF& zone_VEF = ref_cast(Zone_VEF,zone_dis());
  const Front_VF& le_bord= ref_cast(Front_VF,frontiere_dis());
  int premiere=le_bord.num_premiere_face();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const IntTab& face_voisins = zone_VEF.face_voisins();

  DoubleTab& tab=valeurs_au_temps(temps);

  // on traite les faces sur la frontiere

  DoubleTab vecteur_tang(le_bord.nb_faces(),3);

  for(int i=0; i<le_bord.nb_faces(); i++)
    {
      int elem1=face_voisins(i+premiere,0);
      if (elem1 == -1)
        elem1=face_voisins(i+premiere,1);
      int signe = zone_VEF.oriente_normale(i+premiere,elem1);

      vecteur_tang(i,2) = 0.0;

      vecteur_tang (i,0) = -face_normales(i+premiere,1)*signe;

      vecteur_tang (i,1) =face_normales(i+premiere,0)*signe;

      double norme=0.0;

      for (int k=0; k<2; k++)
        norme += vecteur_tang(i,k)* vecteur_tang(i,k);

      norme = sqrt(norme);

      tab(i,2)=0.0;
      for(int j=0; j<2; j++)
        {
          tab(i+premiere,j)=vecteur_tang(i,j);
          tab(i+premiere,j)*=vit_tang;
          tab(i+premiere,j)=tab(i+premiere,j)/norme;
        }
    }
  tab.echange_espace_virtuel();
}
