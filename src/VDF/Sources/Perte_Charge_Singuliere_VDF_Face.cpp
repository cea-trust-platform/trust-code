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
// File:        Perte_Charge_Singuliere_VDF_Face.cpp
// Directory:   $TRUST_ROOT/src/VDF/Sources
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Perte_Charge_Singuliere_VDF_Face.h>
#include <Zone_VDF.h>
#include <Champ_Face.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Motcle.h>
#include <Matrice_Morse.h>

Implemente_instanciable(Perte_Charge_Singuliere_VDF_Face,"Perte_Charge_Singuliere_VDF_Face",Perte_Charge_VDF_Face);

//// printOn
//

Sortie& Perte_Charge_Singuliere_VDF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Perte_Charge_Singuliere_VDF_Face::readOn(Entree& s)
{
  Perte_Charge_Singuliere::lire_donnees(s);
  remplir_num_faces(s);
  return s;
}


/////////////////////////////////////////////////////////////////////
//
//                    Implementation des fonctions
//
//               de la classe Perte_Charge_Singuliere_VDF_Face
//
////////////////////////////////////////////////////////////////////

void Perte_Charge_Singuliere_VDF_Face::remplir_num_faces(Entree& s)
{
  const Domaine& le_domaine = equation().probleme().domaine();
  const Zone_VDF& zone_VDF = ref_cast(Zone_VDF,equation().zone_dis().valeur());
  int taille_bloc = zone_VDF.nb_elem();
  num_faces.resize(taille_bloc);
  lire_surfaces(s,le_domaine,zone_VDF,num_faces);
  int nfac = num_faces.size();
  int nfac_tot = mp_sum(num_faces.size());
  if (nfac_tot==0)
    {
      Cerr << "Erreur a la lecture des donnees de la perte de charge singuliere :" << finl;
      Cerr << " la surface sur laquelle vous avez defini une perte de charge" << finl;
      Cerr << " ne contient entierement aucune des faces de la geometrie " <<  finl;
      exit();
    }

  if (nfac != 0)
    {
      int ori = zone_VDF.orientation(num_faces[0]);
      if (ori != direction_perte_charge())
        {
          Cerr << "Erreur a la lecture des donnees de la perte de charge singuliere : " << finl;
          Cerr << " l'orientation de la surface est differente de la direction" << finl;
          Cerr << " de la perte de charge" << finl;
          exit();
        }
    }
}


void Perte_Charge_Singuliere_VDF_Face::contribuer_a_avec(const DoubleTab&, Matrice_Morse& matrice) const
{

  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const DoubleVect& volumes_entrelaces = zone_VDF.volumes_entrelaces();
  const DoubleVect& porosite_surf = zone_VDF.porosite_face();
  const DoubleTab& vit = la_vitesse->valeurs();
  int ndeb_faces_int = zone_VDF.premiere_face_int();

  int nb_faces = num_faces.size();
  int numfa;
  double Ck;
  double Ud; // vitesse debitante

  if (axi)
    for (int i=0; i<nb_faces; i++)
      {
        /*
          numfa = num_faces[i];
          U = vit[numfa];

          if (numfa < ndeb_faces_int)
          Ck = -0.5*K()/la_zone_VDF->dist_norm_bord_axi(numfa);
          else
          Ck = -0.5*K()/la_zone_VDF->dist_norm_axi(numfa);

          //resu[numfa] += Ck*U*dabs(U)*volumes_entrelaces[numfa]*porosite_surf[numfa];
          */
      }
  else
    for (int i=0; i<nb_faces; i++)
      {
        numfa = num_faces[i];
        // U = vit[numfa];

        Ud = vit[numfa]*porosite_surf[numfa];

        if (numfa < ndeb_faces_int)
          Ck = -0.5*K()/la_zone_VDF->dist_norm_bord(numfa);
        else
          Ck = -0.5*K()/la_zone_VDF->dist_norm(numfa);

        //resu[numfa] += Ck*Ud*dabs(Ud)*volumes_entrelaces[numfa]*porosite_surf[numfa];
        matrice(numfa,numfa)-=Ck*porosite_surf[numfa]*dabs(Ud)*volumes_entrelaces[numfa]*porosite_surf[numfa];

      }


}
DoubleTab& Perte_Charge_Singuliere_VDF_Face::ajouter_(const DoubleTab& inco,DoubleTab& resu) const
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const DoubleVect& volumes_entrelaces = zone_VDF.volumes_entrelaces();
  const DoubleVect& porosite_surf = zone_VDF.porosite_face();
  const DoubleTab& vit = la_vitesse->valeurs();
  int ndeb_faces_int = zone_VDF.premiere_face_int();

  int nb_faces = num_faces.size();
  int numfa;
  double Ck,U;
  double Ud; // vitesse debitante

  if (axi)
    for (int i=0; i<nb_faces; i++)
      {
        numfa = num_faces[i];
        U = vit[numfa];

        if (numfa < ndeb_faces_int)
          Ck = -0.5*K()/la_zone_VDF->dist_norm_bord_axi(numfa);
        else
          Ck = -0.5*K()/la_zone_VDF->dist_norm_axi(numfa);

        resu[numfa] += Ck*U*dabs(U)*volumes_entrelaces[numfa]*porosite_surf[numfa];
      }
  else
    for (int i=0; i<nb_faces; i++)
      {
        numfa = num_faces[i];
        U = inco[numfa]*porosite_surf[numfa];

        Ud = vit[numfa]*porosite_surf[numfa];

        if (numfa < ndeb_faces_int)
          Ck = -0.5*K()/la_zone_VDF->dist_norm_bord(numfa);
        else
          Ck = -0.5*K()/la_zone_VDF->dist_norm(numfa);

        resu[numfa] += Ck*U*dabs(Ud)*volumes_entrelaces[numfa]*porosite_surf[numfa];

      }
  return resu;
}



