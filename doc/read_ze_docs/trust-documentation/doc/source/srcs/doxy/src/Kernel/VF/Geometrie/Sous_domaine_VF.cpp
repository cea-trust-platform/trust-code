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

#include <Sous_Domaine.h>
#include <Sous_domaine_VF.h>

Implemente_instanciable(Sous_domaine_VF,"Sous_domaine_VF",Sous_domaine_dis_base);
Sortie& Sous_domaine_VF::printOn(Sortie& os) const
{

  Cerr << "Sous_domaine_VF::printOn" << finl;

  os << "les_faces : " << finl;
  les_faces_.ecrit(os);

  os << "premiere_face_bord_0 : " << finl;
  os << premiere_face_bord_0_ << finl;

  os << "premiere_face_bord_1 : " << finl;
  os << premiere_face_bord_1_ << finl;

  os << "premiere_face_bord : " << finl;
  os << premiere_face_bord_ << finl;

  return os ;
}


Entree& Sous_domaine_VF::readOn(Entree& is)
{
  Cerr << "Sous_domaine_VF::readOn" << finl;

  les_faces_.lit(is);
  is >> premiere_face_bord_0_;
  is >> premiere_face_bord_1_;
  is >> premiere_face_bord_;
  return is ;
}

////////////////////////////////////////////////////////////////
//                                                            //
//           Fonction principale : discretiser                //
//                                                            //
////////////////////////////////////////////////////////////////

void Sous_domaine_VF::discretiser()
{
  Cerr << "Sous_domaine_VF::discretiser : discretization of the sub-domain "
       << le_sous_domaine->le_nom() << "... ";

  le_dom_VF=ref_cast(Domaine_VF,le_dom_dis.valeur());

  int nb_faces_tot=le_dom_VF->nb_faces_tot();

  // Stockages temporaires :
  IntTab faces_internes;
  faces_internes.resize(nb_faces_tot);
  int nb_faces_internes=0;

  IntTab faces_bord_0;
  faces_bord_0.resize(nb_faces_tot);
  int nb_faces_bord_0=0;

  IntTab faces_bord_1;
  faces_bord_1.resize(nb_faces_tot);
  int nb_faces_bord_1=0;

  IntTab faces_bord;
  faces_bord.resize(nb_faces_tot);
  int nb_faces_bord=0;

  IntTab elem_app;
  elem_app.resize(le_dom_VF->nb_elem_tot());
  elem_app=0;

  // Remplissage de elem_app
  for (int i=0; i<le_sous_domaine->nb_elem_tot(); i++)
    elem_app[le_sous_domaine.valeur()(i)]=1;

  // Boucle sur les faces
  for (int i=0; i<nb_faces_tot; i++)
    {
      int elem0,elem1;
      // Quels elements lui sont voisins ?
      elem0=le_dom_VF->face_voisins(i,0);
      elem1=le_dom_VF->face_voisins(i,1);
      // Remplissage des tableaux temporaires.
      if (elem1==-1)                           // face au bord du domaine
        {
          if (elem_app[elem0])                         // dans le sous_domaine
            {
              faces_bord(nb_faces_bord++)=i;
            }
        }
      else if (elem0==-1)                      // face au bord du domaine
        {
          if (elem_app[elem1])                         // dans le sous_domaine
            {
              faces_bord(nb_faces_bord++)=i;
            }
        }
      else                                     // face interne au domaine
        {
          if (elem_app[elem0] && elem_app[elem1])          // interne a le sous_domaine
            {
              faces_internes(nb_faces_internes++)=i;
            }
          else if (elem_app[elem0])                    // seul elem0 est dans le sous-domaine
            {
              faces_bord_0(nb_faces_bord_0++)=i;
            }
          else if (elem_app[elem1])                    // seul elem1 est dans le sous-domaine
            {
              faces_bord_1(nb_faces_bord_1++)=i;
            }
        }
    }

  // Regroupement des resultats
  premiere_face_bord_0_ = nb_faces_internes;
  premiere_face_bord_1_ = premiere_face_bord_0_+nb_faces_bord_0;
  premiere_face_bord_   = premiere_face_bord_1_+nb_faces_bord_1;
  les_faces_.resize(premiere_face_bord_+nb_faces_bord);
  for (int i=0; i<nb_faces_internes; i++)
    les_faces_(i)=faces_internes(i);
  for (int i=0; i<nb_faces_bord_0; i++)
    les_faces_(premiere_face_bord_0_+i)=faces_bord_0(i);
  for (int i=0; i<nb_faces_bord_1; i++)
    les_faces_(premiere_face_bord_1_+i)=faces_bord_1(i);
  for (int i=0; i<nb_faces_bord; i++)
    les_faces_(premiere_face_bord_+i)=faces_bord(i);

  // Calcul des volumes entrelaces
  const DoubleVect& volumes=le_dom_VF->volumes();
  volumes_entrelaces_.resize(nb_faces_bord_0+nb_faces_bord_1);
  for (int i=0; i<nb_faces_bord_0; i++)
    {
      volumes_entrelaces_(i)=volumes(le_dom_VF->face_voisins(les_faces_(premiere_face_bord_0_+i),0));
    }
  for (int i=0; i<nb_faces_bord_1; i++)
    {
      volumes_entrelaces_(nb_faces_bord_0+i)=volumes(le_dom_VF->face_voisins(les_faces_(premiere_face_bord_1_+i),1));
    }
  Cerr << "OK" << finl;

  // Affichage du volume des sous domaines (ca peut etre pratique)
  double volume_sous_domaine=0;
  for (int j=0; j<le_sous_domaine->nb_elem_tot(); j++)
    {
      // On ne compte que les mailles reelles
      if (le_sous_domaine.valeur()(j)<le_dom_VF->domaine().nb_elem())
        volume_sous_domaine+=volumes(le_sous_domaine.valeur()(j));
    }
  volume_sous_domaine=mp_sum(volume_sous_domaine);
  if (je_suis_maitre())
    Cerr << "Volume of sub-domain " << le_sous_domaine->le_nom() << ": " << volume_sous_domaine << finl;
}
