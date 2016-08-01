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
// File:        Terme_Source_Qdm_VDF_Face.cpp
// Directory:   $TRUST_ROOT/src/VDF/Sources
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Source_Qdm_VDF_Face.h>
#include <Champ_Uniforme.h>
#include <Zone_Cl_dis.h>
#include <Zone_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Neumann_sortie_libre.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Periodique.h>

Implemente_instanciable(Terme_Source_Qdm_VDF_Face,"Source_Qdm_VDF_Face",Source_base);



//// printOn
//

Sortie& Terme_Source_Qdm_VDF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Terme_Source_Qdm_VDF_Face::readOn(Entree& s )
{
  s >> la_source;
  if (la_source->nb_comp() != dimension)
    {
      Cerr << "Erreur a la lecture du terme source de type " << que_suis_je() << finl;
      Cerr << "le champ source doit avoir " << dimension << " composantes" << finl;
      exit();
    }
  return s ;
}

void Terme_Source_Qdm_VDF_Face::associer_pb(const Probleme_base& )
{
  ;
}

void Terme_Source_Qdm_VDF_Face::associer_zones(const Zone_dis& zone_dis,
                                               const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VDF = ref_cast(Zone_VDF, zone_dis.valeur());
  la_zone_Cl_VDF = ref_cast(Zone_Cl_VDF, zone_Cl_dis.valeur());
}


DoubleTab& Terme_Source_Qdm_VDF_Face::ajouter(DoubleTab& resu) const
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const Zone_Cl_VDF& zone_Cl_VDF = la_zone_Cl_VDF.valeur();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntVect& orientation = zone_VDF.orientation();
  //  const DoubleTab& xv = zone_VDF.xv();
  const DoubleVect& porosite_surf = zone_VDF.porosite_face();
  const DoubleVect& volumes_entrelaces = zone_VDF.volumes_entrelaces();

  int ndeb,nfin,ncomp,num_face,elem1,elem2;
  double vol;

  if (sub_type(Champ_Uniforme,la_source.valeur()))
    {
      const DoubleVect& s = la_source->valeurs();

      // Boucle sur les conditions limites pour traiter les faces de bord

      for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
        {

          // pour chaque Condition Limite on regarde son type
          // Si face de Dirichlet ou de Symetrie on ne fait rien
          // Si face de Neumann on calcule la contribution au terme source

          const Cond_lim& la_cl = zone_Cl_VDF.les_conditions_limites(n_bord);

          if (sub_type(Periodique,la_cl.valeur()))
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();

              for (num_face=ndeb; num_face<nfin; num_face++)
                {
                  vol = volumes_entrelaces(num_face);
                  //  for (ncomp=0; ncomp<dimension ; ncomp++)
                  ncomp = orientation(num_face);
                  resu(num_face)+= s(ncomp)*vol;
                }
            }
          else if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
            {
              //              const Neumann_sortie_libre& la_cl_neumann = ref_cast(Neumann_sortie_libre,la_cl.valeur());
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();

              for (num_face=ndeb; num_face<nfin; num_face++)
                {
                  vol = volumes_entrelaces(num_face)*porosite_surf(num_face);
                  ncomp = orientation(num_face);
                  resu(num_face)+= s(ncomp)*vol;
                }

            }
          else if (sub_type(Symetrie,la_cl.valeur()))
            ;
          else if ( (sub_type(Dirichlet,la_cl.valeur()))
                    ||
                    (sub_type(Dirichlet_homogene,la_cl.valeur()))
                  )
            {
              // do nothing
              ;
            }
        }


      // Boucle sur les faces internes

      ndeb = zone_VDF.premiere_face_int();
      for (num_face =zone_VDF.premiere_face_int(); num_face<zone_VDF.nb_faces(); num_face++)
        {
          vol = volumes_entrelaces(num_face)*porosite_surf(num_face);
          ncomp = orientation(num_face);
          resu(num_face) += s(ncomp)*vol;

        }
    }
  else // le champ source n'est plus uniforme
    {
      const DoubleTab& s = la_source->valeurs();

      // Boucle sur les conditions limites pour traiter les faces de bord

      for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
        {

          // pour chaque Condition Limite on regarde son type
          // Si face de Dirichlet ou de Symetrie on ne fait rien
          // Si face de Neumann on calcule la contribution au terme source

          const Cond_lim& la_cl = zone_Cl_VDF.les_conditions_limites(n_bord);

          if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
            {

              //            const Neumann_sortie_libre& la_cl_neumann = ref_cast(Neumann_sortie_libre,la_cl.valeur());
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();

              for (num_face=ndeb; num_face<nfin; num_face++)
                {
                  vol = volumes_entrelaces(num_face)*porosite_surf(num_face);
                  ncomp = orientation(num_face);
                  elem1 = face_voisins(num_face,0);

                  if (elem1 != -1)
                    resu(num_face)+= s(elem1,ncomp)*vol;
                  else
                    {
                      elem2 = face_voisins(num_face,1);
                      resu(num_face)+= s(elem2,ncomp)*vol;
                    }
                }

            }
          else if (sub_type(Symetrie,la_cl.valeur()))
            ;
          else if ( (sub_type(Dirichlet,la_cl.valeur()))
                    ||
                    (sub_type(Dirichlet_homogene,la_cl.valeur()))
                  )
            ;
          else if (sub_type(Periodique,la_cl.valeur()))
            {
              double s_face;
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();
              for (num_face=ndeb; num_face<nfin; num_face++)
                {
                  vol = volumes_entrelaces(num_face)*porosite_surf(num_face);
                  ncomp = orientation(num_face);
                  s_face = 0.5*( s(face_voisins(num_face,0),ncomp) + s(face_voisins(num_face,1),ncomp) );
                  resu(num_face) += s_face*vol;
                }

            }
        }


      // Boucle sur les faces internes

      double s_face;
      ndeb = zone_VDF.premiere_face_int();
      for (num_face =zone_VDF.premiere_face_int(); num_face<zone_VDF.nb_faces(); num_face++)
        {

          vol = volumes_entrelaces(num_face)*porosite_surf(num_face);
          ncomp = orientation(num_face);
          s_face = 0.5*( s(face_voisins(num_face,0),ncomp) + s(face_voisins(num_face,1),ncomp) );
          resu(num_face) += s_face*vol;

        }
    }
  return resu;
}

DoubleTab& Terme_Source_Qdm_VDF_Face::calculer(DoubleTab& resu) const
{
  resu = 0;
  return ajouter(resu);
}

void Terme_Source_Qdm_VDF_Face::mettre_a_jour(double temps)
{
  la_source->mettre_a_jour(temps);
}
