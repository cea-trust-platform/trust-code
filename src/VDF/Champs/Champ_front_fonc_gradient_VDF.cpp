/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Champ_front_fonc_gradient_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Champs
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////


#include <Champ_front_fonc_gradient_VDF.h>
#include <distances_VDF.h>
#include <Paroi_Knudsen_non_negligeable.h>
#include <DoubleTrav.h>
#include <Champ_Inc.h>

Implemente_instanciable(Champ_front_fonc_gradient_VDF,"Champ_front_fonc_gradient_VDF",Champ_front_fonc_gradient);

Sortie& Champ_front_fonc_gradient_VDF::printOn(Sortie& os) const
{
  return Champ_front_fonc_gradient::printOn(os);
}


Entree& Champ_front_fonc_gradient_VDF::readOn(Entree& s)
{
  fixer_nb_comp(dimension);
  return s;
}

// Description:
// Mise a jour du temps

void Champ_front_fonc_gradient_VDF::mettre_a_jour(double temps)
{
  // Cerr << "Champ_front_fonc_gradient_VDF::mettre_a_jour" << finl;
  const Front_VF& la_front=ref_cast(Front_VF, frontiere_dis());
  int nb_faces = la_front.nb_faces();
  int ndeb = la_front.num_premiere_face();

  DoubleTab& tab=valeurs_au_temps(temps);

  const Zone_VDF& la_zone_VDF = ref_cast(Zone_VDF,zone_dis());
  const IntVect& orientation = la_zone_VDF.orientation();
  // On recupere la CL et les champs associes
  const Paroi_Knudsen_non_negligeable& la_condition_limite=ref_cast(Paroi_Knudsen_non_negligeable,condition_limite(la_front.le_nom()));
  const DoubleTab& la_inconnue_paroi_=la_condition_limite.vitesse_paroi();
  const DoubleTab& k_=la_condition_limite.k();
  const DoubleVect& la_inconnue=inconnue->valeurs();
  int elem,iro,ori,i,j;
  //  double inconnue_moyenne1,inconnue_moyenne2;

  DoubleTab la_inconnue_paroi(nb_faces,dimension);
  DoubleVect k(nb_faces);
  DoubleVect inconnue_moyenne(dimension-1);
  if (la_inconnue_paroi_.dimension(0)==1)
    for (i=0; i<nb_faces; i++)
      for (j=0; j<dimension; j++)
        la_inconnue_paroi(i,j)=la_inconnue_paroi_(0,j);
  else
    for (i=0; i<nb_faces; i++)
      for (j=0; j<dimension; j++)
        la_inconnue_paroi(i,j)=la_inconnue_paroi_(i,j);
  if (k_.dimension(0)==1)
    for (i=0; i<nb_faces; i++)
      k(i)=k_(0,0);
  else
    for (i=0; i<nb_faces; i++)
      k(i)=k_(i,0);
  for (i=0; i<nb_faces; i++)
    {
      int num_face=ndeb+i;
      double k_sur_e;
      if (axi)
        k_sur_e=k(i)/la_zone_VDF.dist_norm_bord_axi(num_face);
      else
        k_sur_e=k(i)/la_zone_VDF.dist_norm_bord(num_face);
      elem = la_zone_VDF.face_voisins(num_face,0);
      if ( elem == -1 )
        elem = la_zone_VDF.face_voisins(num_face,1);
      // Calcul de la inconnue a la paroi en fonction
      // de l'orientation
      ori = orientation(num_face);
      tab(i,ori) = la_inconnue_paroi(i,ori);
      if (dimension==2)
        {
          moy_2D_vit(la_inconnue,elem,ori,la_zone_VDF,inconnue_moyenne[0]);
          iro=1-ori;
          tab(i,iro)=(la_inconnue_paroi(i,iro)+k_sur_e*inconnue_moyenne[0])/(1+k_sur_e);
        }
      else if (dimension==3)
        {
          moy_3D_vit(la_inconnue,elem,ori,la_zone_VDF,inconnue_moyenne[0],inconnue_moyenne[1]);
          if (ori==0)
            {
              iro=1;
              tab(i,iro)=(la_inconnue_paroi(i,iro)+k_sur_e*inconnue_moyenne[0])/(1+k_sur_e);
              iro=2;
              tab(i,iro)=(la_inconnue_paroi(i,iro)+k_sur_e*inconnue_moyenne[1])/(1+k_sur_e);
            }
          else if (ori==1)
            {
              iro=0;
              tab(i,iro)=(la_inconnue_paroi(i,iro)+k_sur_e*inconnue_moyenne[0])/(1+k_sur_e);
              iro=2;
              tab(i,iro)=(la_inconnue_paroi(i,iro)+k_sur_e*inconnue_moyenne[1])/(1+k_sur_e);
            }
          else if (ori==2)
            {
              iro=0;
              tab(i,iro)=(la_inconnue_paroi(i,iro)+k_sur_e*inconnue_moyenne[0])/(1+k_sur_e);
              iro=1;
              tab(i,iro)=(la_inconnue_paroi(i,iro)+k_sur_e*inconnue_moyenne[1])/(1+k_sur_e);
            }
        }
    }
  tab.echange_espace_virtuel();
}
