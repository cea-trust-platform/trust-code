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
// File:        Force_Centrifuge_VDF_Face_Axi.cpp
// Directory:   $TRUST_ROOT/src/VDF/Axi/Sources
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#include <Force_Centrifuge_VDF_Face_Axi.h>
#include <Zone_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Champ_Face.h>
#include <Les_Cl.h>
#include <DoubleTrav.h>
#include <Equation_base.h>
#include <Champ_Inc.h>

Implemente_instanciable(Force_Centrifuge_VDF_Face_Axi,"Force_Centrifuge_VDF_Face_Axi",Source_base);

//// printOn
//

Sortie& Force_Centrifuge_VDF_Face_Axi::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Force_Centrifuge_VDF_Face_Axi::readOn(Entree& s )
{
  return s ;
}


/////////////////////////////////////////////////////////////////////
//
//                    Implementation des fonctions
//
//               de la classe Force_Centrifuge_VDF_Face_Axi
//
////////////////////////////////////////////////////////////////////

void Force_Centrifuge_VDF_Face_Axi::associer_pb(const Probleme_base& )
{
  Cerr << "Force_Centrifuge_VDF_Face_Axi::associer_pb" << finl;
}

void Force_Centrifuge_VDF_Face_Axi::associer_zones(const Zone_dis& zone_dis,
                                                   const Zone_Cl_dis& zone_Cl_dis)
{
  const Zone_VDF& zvdf = ref_cast(Zone_VDF, zone_dis.valeur());
  const Zone_Cl_VDF& zclvdf = ref_cast(Zone_Cl_VDF, zone_Cl_dis.valeur());
  la_zone_VDF = zvdf;
  la_zone_Cl_VDF = zclvdf;
  elem_faces.ref(zvdf.elem_faces());
  orientation.ref(zvdf.orientation());
  xp.ref(zvdf.xp());
  xv.ref(zvdf.xv());
  volume_entrelaces.ref(zvdf.volumes_entrelaces());
  porosite_surf.ref(zvdf.porosite_face());
}

DoubleTab& Force_Centrifuge_VDF_Face_Axi::ajouter(DoubleTab& resu) const
{
  const Zone_VDF& zvdf = la_zone_VDF.valeur();
  const Zone_Cl_VDF& zclvdf = la_zone_Cl_VDF.valeur();
  const DoubleTab& vitesse = la_vitesse->valeurs();

  int nb_elem = zvdf.nb_elem();
  int nb_faces = zvdf.nb_faces();
  DoubleTrav vit_sum(nb_faces);
  vit_sum =0;
  double U,V,coef;
  int fac0,fac1,fac2,fac3;

  // Boucle sur les elements pour calculer vit_sum

  for (int num_elem=0; num_elem<nb_elem; num_elem++)
    {
      fac0 = elem_faces(num_elem,0);
      fac1 = elem_faces(num_elem,1);
      fac2 = elem_faces(num_elem,dimension);
      fac3 = elem_faces(num_elem,1+dimension);

      V = 0.5*(vitesse(fac1)+vitesse(fac3));
      U = 0.5*(vitesse(fac0)+vitesse(fac2));

      vit_sum(fac0) += 0.5*(V*V);
      vit_sum(fac2) += 0.5*(V*V);

      vit_sum(fac1) += 0.5*(U*V);
      vit_sum(fac3) += 0.5*(U*V);
    }

  // Boucle sur les conditions limites pour traiter les faces de bord

  int ndeb,nfin,ori,num_face;

  for (int n_bord=0; n_bord<zvdf.nb_front_Cl(); n_bord++)
    {

      // pour chaque Condition Limite on regarde son type
      // Si face de Dirichlet ou de Symetrie on ne fait rien
      // Si face de Neumann on calcule la contribution au terme source

      const Cond_lim& la_cl = zclvdf.les_conditions_limites(n_bord);

      if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          for (num_face=ndeb; num_face<nfin; num_face++)
            {
              ori = orientation(num_face);
              if (ori == 0)
                {
                  coef = volume_entrelaces(num_face)*porosite_surf(num_face)/xv(num_face,0);
                  resu(num_face) += 0.5*(vit_sum(num_face)*coef);
                }
              else if (ori == 1)
                {
                  coef = volume_entrelaces(num_face)*porosite_surf(num_face)/xv(num_face,0);
                  resu(num_face) -= 0.5*(vit_sum(num_face)*coef);
                }
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

  ndeb = zvdf.premiere_face_int();
  nfin = zvdf.nb_faces();

  for (num_face=ndeb; num_face<nfin; num_face++)
    {
      ori = orientation(num_face);
      if (ori == 0)
        {
          coef = volume_entrelaces(num_face)*porosite_surf(num_face)/xv(num_face,0);
          resu(num_face) += 0.5*(vit_sum(num_face)*coef);
        }
      else if (ori == 1)
        {
          coef = volume_entrelaces(num_face)*porosite_surf(num_face)/xv(num_face,0);
          resu(num_face) -= 0.5*(vit_sum(num_face)*coef);
        }
    }
  return resu;
}

DoubleTab& Force_Centrifuge_VDF_Face_Axi::calculer(DoubleTab& resu) const
{
  resu=0;
  ajouter(resu);
  return resu;
}

void Force_Centrifuge_VDF_Face_Axi::completer()
{
  Source_base::completer();
  la_vitesse = ref_cast(Champ_Face,equation().inconnue().valeur());

}



