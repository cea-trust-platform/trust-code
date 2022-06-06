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

#include <Solveur_Masse_EF.h>
#include <Zone_Cl_EF.h>
#include <Zone_EF.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Debog.h>
#include <Equation_base.h>


Implemente_instanciable(Solveur_Masse_EF,"Masse_EF",Solveur_Masse_base);


//     printOn()
/////

Sortie& Solveur_Masse_EF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Solveur_Masse_EF::readOn(Entree& s)
{
  return s ;
}



///////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Solveur_Masse_EF
//
//////////////////////////////////////////////////////////////


DoubleTab& Solveur_Masse_EF::appliquer_impl(DoubleTab& sm) const
{
  Debog::verifier("Solveur_Masse_EF::appliquer deb, sm=",sm);
  const Zone_Cl_EF& zone_Cl_EF = ref_cast(Zone_Cl_EF,la_zone_Cl_EF.valeur());
  const Zone_EF& zone_EF = la_zone_EF.valeur();
  const DoubleVect& volumes_sommets_thilde = zone_EF.volumes_sommets_thilde();

  //  const DoubleVect& porosite_sommet = zone_EF.porosite_sommet();
  int nfa = zone_EF.nb_som();
  int nbsom=zone_EF.nb_som();
  int face;
  if (nfa != sm.dimension(0))
    {
      Cerr << "erreur dans Solveur_Masse_EF : ";
      Cerr << "nombre de faces :  " << nfa
           << " taille du second membre : " << sm.dimension(0) << finl;
      exit();
    }
  const IntTab& faces_sommets=zone_EF.face_sommets();
  int nb_som_face=faces_sommets.dimension(1);
  if (sm.nb_dim() == 1)
    {
      // On traite les faces standard qui ne portent pas de conditions aux limites

      for (int som=0; som<nbsom; som++)
        {
          sm(som) /= (volumes_sommets_thilde(som));

        }

      // On traite les faces non standard
      // les faces des bord sont des faces non standard susceptibles de porter des C.L
      // les faces internes non standard ne portent pas de C.L

      // On traite les conditions aux limites
      int nb_cl=zone_Cl_EF.nb_cond_lim();
      for (int n_bord=0; n_bord<nb_cl; n_bord++)
        {

          const Cond_lim& la_cl = zone_Cl_EF.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();

          if ( (sub_type(Dirichlet,la_cl.valeur()))
               ||
               (sub_type(Dirichlet_homogene,la_cl.valeur()))
             )
            // Pour les faces de Dirichlet on met sm a 0
            for (face=num1; face<num2; face++)
              for (int s=0; s<nb_som_face; s++)
                {
                  int som=faces_sommets(face,s);
                  sm(som) =0;
                }
        }
    }


  else
    {
      int nbcomp = sm.dimension(1);
      // On traite les faces standard qui ne portent pas de conditions aux limites

      for (int som=0; som<nbsom; som++)
        {
          for (int comp=0; comp<nbcomp; comp++)
            sm(som,comp) /= (volumes_sommets_thilde(som));

        }

      // On traite les faces non standard
      // les faces des bord sont des faces non standard susceptibles de porter des C.L
      // les faces internes non standard ne portent pas de C.L

      // On traite les conditions aux limites
      if (zone_Cl_EF.equation().inconnue()->nature_du_champ()==vectoriel)
        zone_Cl_EF.imposer_symetrie(sm);
      int nb_cl=zone_Cl_EF.nb_cond_lim();
      for (int n_bord=0; n_bord<nb_cl; n_bord++)
        {

          const Cond_lim& la_cl = zone_Cl_EF.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();

          if ( (sub_type(Dirichlet,la_cl.valeur()))
               ||
               (sub_type(Dirichlet_homogene,la_cl.valeur()))
             )
            // Pour les faces de Dirichlet on met sm a 0
            for (face=num1; face<num2; face++)
              for (int s=0; s<nb_som_face; s++)
                {
                  int som=faces_sommets(face,s);
                  for (int comp=0; comp<nbcomp; comp++)
                    sm(som,comp) =0;
                }
        }
    }


  sm.echange_espace_virtuel();
  Debog::verifier("Solveur_Masse_EF::appliquer, sm=",sm);
  return sm;
}


//
void Solveur_Masse_EF::associer_zone_dis_base(const Zone_dis_base& la_zone_dis_base)
{
  la_zone_EF = ref_cast(Zone_EF, la_zone_dis_base);
}

void Solveur_Masse_EF::associer_zone_cl_dis_base(const Zone_Cl_dis_base& la_zone_Cl_dis_base)
{
  la_zone_Cl_EF = ref_cast(Zone_Cl_EF, la_zone_Cl_dis_base);
}




Matrice_Base& Solveur_Masse_EF::ajouter_masse(double dt, Matrice_Base& matrice, int penalisation) const
{
  if (penalisation||(la_zone_Cl_EF.valeur().equation().inconnue()->nature_du_champ()!=vectoriel))
    return Solveur_Masse_base::ajouter_masse(dt,matrice,penalisation);
  // Sinon on modifie temporairement la nature du champ pour que appliquer_impl ne projette pas sur n.
  Champ_Inc_base& inco=ref_cast_non_const( Champ_Inc_base,la_zone_Cl_EF.valeur().equation().inconnue().valeur());
  inco.fixer_nature_du_champ(multi_scalaire);
  Solveur_Masse_base::ajouter_masse(dt,matrice,penalisation);
  inco.fixer_nature_du_champ(vectoriel);
  return matrice;


}


DoubleTab& Solveur_Masse_EF::ajouter_masse(double dt, DoubleTab& x, const DoubleTab& y, int penalisation) const
{
  if (penalisation||(la_zone_Cl_EF.valeur().equation().inconnue()->nature_du_champ()!=vectoriel))
    return Solveur_Masse_base::ajouter_masse(dt,x,y,penalisation);

  // Sinon on modifie temporairement la nature du champ pour que appliquer_impl ne projette pas sur n.
  Champ_Inc_base& inco=ref_cast_non_const( Champ_Inc_base,la_zone_Cl_EF.valeur().equation().inconnue().valeur());
  inco.fixer_nature_du_champ(multi_scalaire);
  Solveur_Masse_base::ajouter_masse(dt,x,y,penalisation);
  inco.fixer_nature_du_champ(vectoriel);
  return x;
}
