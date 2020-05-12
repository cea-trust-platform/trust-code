/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Masse_CoviMAC_Face.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Solveurs
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Masse_CoviMAC_Face.h>
#include <Equation_base.h>
#include <Zone_Cl_CoviMAC.h>
#include <Zone_CoviMAC.h>
#include <Champ_Face_CoviMAC.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Debog.h>

Implemente_instanciable(Masse_CoviMAC_Face,"Masse_CoviMAC_Face",Solveur_Masse_base);


//     printOn()
/////

Sortie& Masse_CoviMAC_Face::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Masse_CoviMAC_Face::readOn(Entree& s)
{
  return s ;
}


///////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Masse_CoviMAC_Face
//
//////////////////////////////////////////////////////////////


DoubleTab& Masse_CoviMAC_Face::appliquer_impl(DoubleTab& sm) const
{
  assert(la_zone_CoviMAC.non_nul());
  assert(la_zone_Cl_CoviMAC.non_nul());
  const Zone_CoviMAC& zone_CoviMAC = la_zone_CoviMAC.valeur();
  const DoubleVect& porosite_face = zone_CoviMAC.porosite_face();
  const DoubleVect& volumes_entrelaces = zone_CoviMAC.volumes_entrelaces();

  int nb_faces = zone_CoviMAC.nb_faces();
  int num_face,ndeb,nfin;

  if (sm.dimension(0) != nb_faces)
    {
      Cerr << "Masse_CoviMAC_Face::appliquer :  erreur dans la taille de sm" << finl;
      exit();
    }

  if (sm.nb_dim() == 1)
    {
      // Boucle sur les faces joint

      // Boucle sur les bords
      // Sur les faces qui portent des conditions aux limites de Dirichlet ou de Symetrie
      // la vitesse normale reste egale a sa valeur initiale.
      // Donc sur ces faces vpoint doit rester a 0.

      for (int n_bord=0; n_bord<zone_CoviMAC.nb_front_Cl(); n_bord++)
        {

          // pour chaque Condition Limite on regarde son type

          const Cond_lim& la_cl = la_zone_Cl_CoviMAC->les_conditions_limites(n_bord);
          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = la_front_dis.num_premiere_face();
          nfin = ndeb + la_front_dis.nb_faces();

          if ( (sub_type(Dirichlet,la_cl.valeur()))
               ||
               (sub_type(Dirichlet_homogene,la_cl.valeur()))
             )
            // Pour les faces de Dirichlet on met sm a 0
            for (num_face=ndeb; num_face<nfin; num_face++)
              sm[num_face] = 0;
          else if (sub_type(Symetrie,la_cl.valeur()))
            // Pour les faces de Symetrie on met vpoint a 0
            for (num_face=ndeb; num_face<nfin; num_face++)
              sm[num_face] = 0;
          else
            for (num_face=ndeb; num_face<nfin; num_face++)
              sm[num_face] /= (volumes_entrelaces(num_face)*porosite_face(num_face));

        }

      // Boucle sur les faces internes

      ndeb = zone_CoviMAC.premiere_face_int();
      for (num_face=ndeb; num_face<nb_faces; num_face++)
        {
          sm[num_face] /= (volumes_entrelaces(num_face)*porosite_face(num_face));
        }
    }
  else
    {
      assert(sm.nb_dim()==2); // sinon on ne fait pas ce qu'il faut
      int nb_comp = sm.dimension(1);
      int ncomp;

      // Boucle sur les faces joint

      // Boucle sur les bords
      // Sur les faces qui portent des conditions aux limites de Dirichlet ou de Symetrie
      // la vitesse normale reste egale a sa valeur initiale.
      // Donc sur ces faces vpoint doit rester a 0.

      for (int n_bord=0; n_bord<zone_CoviMAC.nb_front_Cl(); n_bord++)
        {

          // pour chaque Condition Limite on regarde son type

          const Cond_lim& la_cl = la_zone_Cl_CoviMAC->les_conditions_limites(n_bord);
          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = la_front_dis.num_premiere_face();
          nfin = ndeb + la_front_dis.nb_faces();

          if ( (sub_type(Dirichlet,la_cl.valeur()))
               ||
               (sub_type(Dirichlet_homogene,la_cl.valeur()))
             )
            // Pour les faces de Dirichlet on met sm a 0
            for (num_face=ndeb; num_face<nfin; num_face++)
              for (ncomp=0; ncomp<nb_comp; ncomp++)
                sm(num_face,ncomp) = 0;
          else if (sub_type(Symetrie,la_cl.valeur()))
            // Pour les faces de Symetrie on met vpoint a 0
            for (num_face=ndeb; num_face<nfin; num_face++)
              for (ncomp=0; ncomp<nb_comp; ncomp++)
                sm(num_face,ncomp) = 0;
          else
            for (num_face=ndeb; num_face<nfin; num_face++)
              for (ncomp=0; ncomp<nb_comp; ncomp++)
                sm(num_face,ncomp)
                /= (volumes_entrelaces(num_face)*porosite_face(num_face));

        }

      // Boucle sur les faces internes

      ndeb = zone_CoviMAC.premiere_face_int();
      for (num_face=ndeb; num_face<nb_faces; num_face++)
        for (ncomp=0; ncomp<nb_comp; ncomp++)
          sm(num_face,ncomp) /= (volumes_entrelaces(num_face)*porosite_face(num_face));
    }
  //sm.echange_espace_virtuel();
  //Debog::verifier("Masse_CoviMAC_Face::appliquer sm",sm);
  return sm;
}

DoubleTab& Masse_CoviMAC_Face::ajouter_masse(double dt, DoubleTab& secmem, const DoubleTab& inco, int penalisation) const
{
  Solveur_Masse_base::ajouter_masse(dt, secmem, inco, penalisation);

  /* pour les CL a vitesse imposee */

  const Zone_CoviMAC& zone = la_zone_CoviMAC;
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const Conds_lim& cls = la_zone_Cl_CoviMAC->les_conditions_limites();
  const DoubleTab& tf = zone.face_tangentes();
  int f, r;

  ch.init_cl();

  for (f = 0; f < zone.premiere_face_int(); f++) if (ch.icl(f, 0) == 3) /* Dirichlet seulement */
      for (r = 0, secmem(f) = 0; r < dimension; r++) secmem(f) += tf(f, r) * ref_cast(Dirichlet, cls[ch.icl(f, 1)].valeur()).val_imp(ch.icl(f, 2), r);

  return secmem;
}

Matrice_Base& Masse_CoviMAC_Face::ajouter_masse(double dt, Matrice_Base& matrice, int penalisation) const
{
  Solveur_Masse_base::ajouter_masse(dt, matrice, penalisation);

  /* pour les CL a vitesse imposee */

  const Zone_CoviMAC& zone = la_zone_CoviMAC;
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());

  ch.init_cl();

  Matrice_Morse& mat = ref_cast(Matrice_Morse, matrice);
  for (int f = 0; f < zone.premiere_face_int(); f++) if (ch.icl(f, 0) > 1) mat(f, f) = 1; /* tout sauf interne et Neumann */

  return matrice;
}

//
void Masse_CoviMAC_Face::associer_zone_dis_base(const Zone_dis_base& la_zone_dis_base)
{
  la_zone_CoviMAC = ref_cast(Zone_CoviMAC, la_zone_dis_base);
}

void Masse_CoviMAC_Face::associer_zone_cl_dis_base(const Zone_Cl_dis_base& la_zone_Cl_dis_base)
{
  la_zone_Cl_CoviMAC = ref_cast(Zone_Cl_CoviMAC, la_zone_Cl_dis_base);
}

