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
// File:        Masse_VDF_Face.cpp
// Directory:   $TRUST_ROOT/src/VDF/Solveurs
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Masse_VDF_Face.h>
#include <Zone_Cl_VDF.h>
#include <Zone_VDF.h>
#include <Les_Cl.h>
#include <Debog.h>

Implemente_instanciable(Masse_VDF_Face,"Masse_VDF_Face",Solveur_Masse_base);


//     printOn()
/////

Sortie& Masse_VDF_Face::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Masse_VDF_Face::readOn(Entree& s)
{
  return s ;
}


///////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Masse_VDF_Face
//
//////////////////////////////////////////////////////////////


DoubleTab& Masse_VDF_Face::appliquer_impl(DoubleTab& sm) const
{
  assert(la_zone_VDF.non_nul());
  assert(la_zone_Cl_VDF.non_nul());
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const DoubleVect& porosite_face = zone_VDF.porosite_face();
  const DoubleVect& volumes_entrelaces = zone_VDF.volumes_entrelaces();

  int nb_faces = zone_VDF.nb_faces();
  int num_face,ndeb,nfin;

  if (sm.dimension(0) != nb_faces)
    {
      Cerr << "Masse_VDF_Face::appliquer :  erreur dans la taille de sm" << finl;
      exit();
    }

  if (sm.nb_dim() == 1)
    {
      // Boucle sur les faces joint

      // Boucle sur les bords
      // Sur les faces qui portent des conditions aux limites de Dirichlet ou de Symetrie
      // la vitesse normale reste egale a sa valeur initiale.
      // Donc sur ces faces vpoint doit rester a 0.

      for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
        {

          // pour chaque Condition Limite on regarde son type

          const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);
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

      ndeb = zone_VDF.premiere_face_int();
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

      for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
        {

          // pour chaque Condition Limite on regarde son type

          const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);
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

      ndeb = zone_VDF.premiere_face_int();
      for (num_face=ndeb; num_face<nb_faces; num_face++)
        for (ncomp=0; ncomp<nb_comp; ncomp++)
          sm(num_face,ncomp) /= (volumes_entrelaces(num_face)*porosite_face(num_face));
    }
  //sm.echange_espace_virtuel();
  //Debog::verifier("Masse_VDF_Face::appliquer sm",sm);
  return sm;
}

//
void Masse_VDF_Face::associer_zone_dis_base(const Zone_dis_base& la_zone_dis_base)
{
  la_zone_VDF = ref_cast(Zone_VDF, la_zone_dis_base);
}

void Masse_VDF_Face::associer_zone_cl_dis_base(const Zone_Cl_dis_base& la_zone_Cl_dis_base)
{
  la_zone_Cl_VDF = ref_cast(Zone_Cl_VDF, la_zone_Cl_dis_base);
}

