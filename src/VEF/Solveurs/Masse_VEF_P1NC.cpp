/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Masse_VEF_P1NC.cpp
// Directory:   $TRUST_ROOT/src/VEF/Solveurs
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////

#include <Masse_VEF_P1NC.h>
#include <Zone_Cl_VEF.h>
#include <Zone_VEF.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Periodique.h>
#include <Symetrie.h>
#include <Debog.h>
#include <Equation_base.h>
#include <Matrice_Morse.h>

Implemente_instanciable(Masse_VEF_P1NC,"Masse_VEF_P1NC",Solveur_Masse_base);


//     printOn()
/////

Sortie& Masse_VEF_P1NC::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Masse_VEF_P1NC::readOn(Entree& s)
{
  return s ;
}



///////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Masse_VEF_P1NC
//
//////////////////////////////////////////////////////////////


DoubleTab& Masse_VEF_P1NC::appliquer_impl(DoubleTab& sm) const
{
  const Zone_Cl_VEF& zone_Cl_VEF = ref_cast(Zone_Cl_VEF,la_zone_Cl_VEF.valeur());
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const DoubleVect& volumes_entrelaces = zone_VEF.volumes_entrelaces();
  const DoubleVect& volumes_entrelaces_Cl = zone_Cl_VEF.volumes_entrelaces_Cl();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const DoubleVect& porosite_face = zone_VEF.porosite_face();
  int nfa = zone_VEF.nb_faces();
  int num_std = zone_VEF.premiere_face_std();
  int num_int = zone_VEF.premiere_face_int();
  int face,comp,elem;
  int nbcomp = sm.line_size();
  if (nfa != sm.dimension(0))
    {
      Cerr << "erreur dans Masse_VEF_P1NC : ";
      Cerr << "nombre de faces :  " << nfa
           << " taille du second membre : " << sm.dimension(0) << finl;
      exit();
    }

  // On traite les faces standard qui ne portent pas de conditions aux limites

  for (face=num_std; face<nfa; face++)
    {
      //elem1 = face_voisins(face,0);
      //elem2 = face_voisins(face,1);
      for (comp=0; comp<nbcomp; comp++)
        sm(face,comp) /= (volumes_entrelaces(face)*porosite_face(face)) ;
    }
  // On traite les faces non standard
  // les faces des bord sont des faces non standard susceptibles de porter des C.L
  // les faces internes non standard ne portent pas de C.L

  // On traite les conditions aux limites

  for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {

      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num1 = le_bord.num_premiere_face();
      int num2 = num1 + le_bord.nb_faces();

      if ( (sub_type(Dirichlet,la_cl.valeur()))
           ||
           (sub_type(Dirichlet_homogene,la_cl.valeur()))
         )
        // Pour les faces de Dirichlet on met sm a 0
        for (face=num1; face<num2; face++)
          for (comp=0; comp<nbcomp; comp++)
            sm(face,comp) =0;
      else

        if ((sub_type(Symetrie,la_cl.valeur()))&&(zone_Cl_VEF.equation().inconnue()->nature_du_champ()==vectoriel))
          {
            const DoubleTab& normales = zone_VEF.face_normales();
            for (face=num1; face<num2; face++)
              {
                double psc=0;
                double surf=0;
                for (comp=0; comp<nbcomp; comp++)
                  {
                    psc+=sm(face,comp)*normales(face,comp);
                    surf+=normales(face,comp)*normales(face,comp);
                  }
                psc/=surf;
                for(comp=0; comp<dimension; comp++)
                  {
                    sm(face,comp)-=psc*normales(face,comp);
                    sm(face,comp) /= (volumes_entrelaces_Cl(face)*
                                      porosite_face(face));
                  }
              }
          }

        else

          for (face=num1; face<num2; face++)
            {
              elem = face_voisins(face,0);
              if ( elem == -1 )
                {
                  elem = face_voisins(face,1);
                }
              for (comp=0; comp<nbcomp; comp++)
                sm(face,comp) /= (volumes_entrelaces_Cl(face)*porosite_face(face));
            }
    }

  // On traite les faces internes non standard
  for (face=num_int; face<num_std; face++)
    {
      //elem1 = face_voisins(face,0);
      //elem2 = face_voisins(face,1);
      for (comp=0; comp<nbcomp; comp++)
        sm(face,comp) /= (volumes_entrelaces_Cl(face)*porosite_face(face)) ;

    }
  //sm.echange_espace_virtuel();
  //Debog::verifier("Masse_VEF_P1NC::appliquer, sm=",sm);
  return sm;
}


//
void Masse_VEF_P1NC::associer_zone_dis_base(const Zone_dis_base& la_zone_dis_base)
{
  la_zone_VEF = ref_cast(Zone_VEF, la_zone_dis_base);
}

void Masse_VEF_P1NC::associer_zone_cl_dis_base(const Zone_Cl_dis_base& la_zone_Cl_dis_base)
{
  la_zone_Cl_VEF = ref_cast(Zone_Cl_VEF, la_zone_Cl_dis_base);
}


Matrice_Base& Masse_VEF_P1NC::ajouter_masse(double dt, Matrice_Base& matrice, int penalisation) const
{
  if (penalisation||(la_zone_Cl_VEF.valeur().equation().inconnue()->nature_du_champ()!=vectoriel))
    return Solveur_Masse_base::ajouter_masse(dt,matrice,penalisation);
  // Sinon on modifie temporairement la nature du champ pour que appliquer_impl ne projette pas sur n.
  Champ_Inc_base& inco=ref_cast_non_const( Champ_Inc_base,la_zone_Cl_VEF.valeur().equation().inconnue().valeur());
  inco.fixer_nature_du_champ(multi_scalaire);
  Solveur_Masse_base::ajouter_masse(dt,matrice,penalisation);
  inco.fixer_nature_du_champ(vectoriel);
  return matrice;


}


DoubleTab& Masse_VEF_P1NC::ajouter_masse(double dt, DoubleTab& x, const DoubleTab& y, int penalisation) const
{
  if (penalisation||(la_zone_Cl_VEF.valeur().equation().inconnue()->nature_du_champ()!=vectoriel))
    return Solveur_Masse_base::ajouter_masse(dt,x,y,penalisation);

  // Sinon on modifie temporairement la nature du champ pour que appliquer_impl ne projette pas sur n.
  Champ_Inc_base& inco=ref_cast_non_const( Champ_Inc_base,la_zone_Cl_VEF.valeur().equation().inconnue().valeur());
  inco.fixer_nature_du_champ(multi_scalaire);
  Solveur_Masse_base::ajouter_masse(dt,x,y,penalisation);
  inco.fixer_nature_du_champ(vectoriel);
  return x;
}

DoubleTab& Masse_VEF_P1NC::corriger_solution(DoubleTab& x, const DoubleTab& y, int incr) const
{
  //  assert(penalisation_==1);

  return Solveur_Masse_base::corriger_solution( x,  y, incr) ;
}
