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

#include <Perte_Charge_Reguliere_VEF_P1NC.h>
#include <Domaine_VEF.h>
#include <Sous_Domaine.h>
#include <Fluide_Incompressible.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Domaine.h>
#include <Champ_Uniforme.h>
#include <Matrice_Morse.h>

Implemente_instanciable(Perte_Charge_Reguliere_VEF_P1NC,"Perte_Charge_Reguliere_VEF_P1NC",Perte_Charge_VEF_Face);



//// printOn
//

Sortie& Perte_Charge_Reguliere_VEF_P1NC::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Perte_Charge_Reguliere_VEF_P1NC::readOn(Entree& s )
{
  Cerr << " Perte_Charge_Reguliere_VEF_P1NC::readOn " << finl  ;
  Perte_Charge_Reguliere::lire_donnees(s);
  //Nom nom_sous_domaine;
  s >> nom_sous_domaine;

  return s ;
}


/////////////////////////////////////////////////////////////////////
//
//                    Implementation des fonctions
//
//               de la classe Perte_Charge_Reguliere_VEF_P1NC
//
////////////////////////////////////////////////////////////////////

void Perte_Charge_Reguliere_VEF_P1NC::remplir_num_faces(Nom& un_nom_sous_domaine)
{
  Cerr << " Perte_Charge_Reguliere_VEF_P1NC::remplir_num_faces " << finl;
  const Domaine& le_domaine = equation().probleme().domaine();
  const Domaine_VEF& domaine_VEF = le_dom_VEF.valeur();

  const IntTab& elem_faces = domaine_VEF.elem_faces();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  const DoubleVect& volumes = domaine_VEF.volumes();
  const Sous_Domaine& le_sous_domaine = le_domaine.ss_domaine(un_nom_sous_domaine);
  int nb_poly_ss_domaine = le_sous_domaine.nb_elem_tot();

  int nfe=domaine_VEF.domaine().nb_faces_elem();
  int nfac;
  int num_poly;

  num_faces.resize(nb_poly_ss_domaine*4);
  corr_front_ss.resize(nb_poly_ss_domaine*4);

  corr_front_ss = 1. ;
  nfac = 0;

  IntVect num_loc(domaine_VEF.nb_elem_tot());
  IntVect fait(domaine_VEF.nb_faces_tot());
  fait = -1;
  num_loc = -1;

  int num_poly_vois;
  int num_elem,num_face, face_loc;

  for (num_elem=0; num_elem<nb_poly_ss_domaine; num_elem++)
    {
      num_loc[le_sous_domaine(num_elem)] = num_elem;
    }

  for (num_elem=0; num_elem<nb_poly_ss_domaine; num_elem++)
    {
      num_poly = le_sous_domaine(num_elem);
      for  (face_loc = 0; face_loc < nfe ; face_loc++)
        {
          num_face =  elem_faces(num_poly,face_loc);
          if (fait(num_face) ==-1)
            {
              fait(num_face) = 1 ;
              num_faces[nfac++] = num_face;
              for (int i=0; i<2; i++ )
                {
                  num_poly_vois = face_voisins(num_face,i);
                  if (num_poly_vois != -1)
                    if (num_loc[num_poly_vois] == -1)  // le poly voisin n'est pas dans le sous_domaine
                      {
                        corr_front_ss[nfac-1] = volumes(num_poly)/(volumes(num_poly)+volumes(num_poly_vois)) ;
                      }
                }
            }
        }
    }
  num_faces.resize(nfac);
  corr_front_ss.resize(nfac);
}

DoubleTab& Perte_Charge_Reguliere_VEF_P1NC::ajouter(DoubleTab& resu) const
{
  //Cerr << " Perte_Charge_Reguliere_VEF_P1NC::ajouter " << finl;
  const Domaine_VEF& domaine_VEF = ref_cast(Domaine_VEF,equation().domaine_dis().valeur());
  const DoubleVect& volumes_entrelaces = domaine_VEF.volumes_entrelaces();
  const DoubleVect& porosite_face = equation().milieu().porosite_face();
  const DoubleTab& vit = la_vitesse->valeurs();
  const IntTab& face_voisins = domaine_VEF.face_voisins();

  double U_abs;
  double Cf,CK,Reynolds;
  int numfa,n0,n1;

  const Champ_Don& nu = le_fluide->viscosite_cinematique();
  const DoubleTab& visco = nu.valeurs();
  double d_visco=-1;
  int l_visco_unif=0;

  if (sub_type(Champ_Uniforme,nu.valeur()))
    {
      const Champ_Uniforme& ch_nu = ref_cast(Champ_Uniforme,nu.valeur());
      d_visco = ch_nu(0,0);
      l_visco_unif = 1;
    }


  if (couronne_tube == 1)
    {
      Cerr << " PQ : 17/06/08 : Cette option a ete supprimee dans Perte_Charge_Reguliere_VEF_P1NC" << finl;
      Cerr << " Possibilite de retrouver la meme fonctionnalite a l'aide du mot cle : " << finl;
      Cerr << "  Perte_charge_anisotrope (voir doc) " << finl;
      exit();
    }

  int premiere_face_int=domaine_VEF.premiere_face_int();
  int direction = direction_perte_charge();
  int nb_faces = num_faces.size();
  for (int i=0; i<nb_faces; i++)
    {
      numfa = num_faces[i];

      if (!l_visco_unif)
        {
          n0 = face_voisins(numfa,0);
          n1 = face_voisins(numfa,1);
          if (numfa < premiere_face_int)
            {
              if (n0 != -1)
                d_visco = visco[n0];
              else
                d_visco = visco[n1];
            }
          else
            d_visco = 0.5*(visco[n0]+visco[n1]);
        }
      // GF La vitesse pour caluler le Rey ne depend pas de la porosite
      // et on utilise la norme
      U_abs=0;
      for (int j =0; j<dimension; j++ )
        U_abs += vit(numfa,j)*vit(numfa,j);


      U_abs = sqrt(U_abs) ;


      Reynolds = U_abs*D()/d_visco;

      if (!Cf_utilisateur)
        Cf = calculer_Cf_blasius(Reynolds);
      else
        Cf = CF();

      CK = -0.5*Cf/D();

      resu(numfa,direction) += CK*U_abs*vit(numfa,direction)*volumes_entrelaces(numfa)*porosite_face(numfa)*corr_front_ss(i);

    }

  return resu;
}

void  Perte_Charge_Reguliere_VEF_P1NC::contribuer_a_avec(const DoubleTab&, Matrice_Morse& matrice) const
{
  //Cerr << " Perte_Charge_Reguliere_VEF_P1NC::ajouter " << finl;
  const Domaine_VEF& domaine_VEF = ref_cast(Domaine_VEF,equation().domaine_dis().valeur());
  const DoubleVect& volumes_entrelaces = domaine_VEF.volumes_entrelaces();
  const DoubleVect& porosite_face = equation().milieu().porosite_face();
  const DoubleTab& vit = la_vitesse->valeurs();
  const IntTab& face_voisins = domaine_VEF.face_voisins();

  double U_abs;
  double Cf,CK,Reynolds;
  int numfa,n0,n1;

  const Champ_Don& nu = le_fluide->viscosite_cinematique();
  const DoubleTab& visco = nu.valeurs();
  double d_visco=-1;
  int l_visco_unif=0;

  if (sub_type(Champ_Uniforme,nu.valeur()))
    {
      const Champ_Uniforme& ch_nu = ref_cast(Champ_Uniforme,nu.valeur());
      d_visco = ch_nu(0,0);
      l_visco_unif = 1;
    }


  if (couronne_tube == 1)
    {
      Cerr << " PQ : 17/06/08 : Cette option a ete supprimee dans Perte_Charge_Reguliere_VEF_P1NC" << finl;
      Cerr << " Possibilite de retrouver la meme fonctionnalite a l'aide du mot cle : " << finl;
      Cerr << "  Perte_charge_anisotrope (voir doc) " << finl;
      exit();
    }

  int premiere_face_int=domaine_VEF.premiere_face_int();
  int direction = direction_perte_charge();
  int nb_faces = num_faces.size();
  for (int i=0; i<nb_faces; i++)
    {
      numfa = num_faces[i];

      if (!l_visco_unif)
        {
          n0 = face_voisins(numfa,0);
          n1 = face_voisins(numfa,1);
          if (numfa < premiere_face_int)
            {
              if (n0 != -1)
                d_visco = visco[n0];
              else
                d_visco = visco[n1];
            }
          else
            d_visco = 0.5*(visco[n0]+visco[n1]);
        }
      // GF La vitesse pour caluler le Rey ne depend pas de la porosite
      // et on utilise la norme
      U_abs=0;
      for (int j =0; j<dimension; j++ )
        U_abs += vit(numfa,j)*vit(numfa,j);


      U_abs = sqrt(U_abs) ;


      Reynolds = U_abs*D()/d_visco;

      if (!Cf_utilisateur)
        Cf = calculer_Cf_blasius(Reynolds);
      else
        Cf = CF();

      CK = -0.5*Cf/D();

      //       resu(numfa,dir) += CK*U_abs*vit(numfa,dir)*volumes_entrelaces(numfa)*porosite_face(numfa)*corr_front_ss(i);
      int n0bis=numfa*dimension+direction;
      matrice.coef(n0bis,n0bis)-=CK*U_abs*volumes_entrelaces(numfa)*porosite_face(numfa)*corr_front_ss(i);

    }


}

DoubleTab& Perte_Charge_Reguliere_VEF_P1NC::calculer(DoubleTab& resu) const
{
  resu = 0;
  return ajouter(resu);
}


void Perte_Charge_Reguliere_VEF_P1NC::completer()
{
  Source_base::completer();
  remplir_num_faces(nom_sous_domaine);

}

