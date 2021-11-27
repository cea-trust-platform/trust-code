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
// File:        Op_Grad_P0_to_Face.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Operateurs_Divers
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Grad_P0_to_Face.h>
#include <Champ_P0_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Periodique.h>
#include <Navier_Stokes_std.h>
#include <Probleme_base.h>
#include <Option_VDF.h>
#include <Schema_Temps_base.h>
#include <SFichier.h>
#include <Check_espace_virtuel.h>

Implemente_instanciable(Op_Grad_P0_to_Face,"Op_Grad_P0_to_Face",Operateur_Grad_base);


//// printOn
//

Sortie& Op_Grad_P0_to_Face::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Grad_P0_to_Face::readOn(Entree& s)
{
  return s ;
}



// Description:
void Op_Grad_P0_to_Face::associer(const Zone_dis& zone_dis,
                                  const Zone_Cl_dis& zone_Cl_dis,
                                  const Champ_Inc& )
{
  const Zone_VDF& zvdf = ref_cast(Zone_VDF, zone_dis.valeur());
  const Zone_Cl_VDF& zclvdf = ref_cast(Zone_Cl_VDF, zone_Cl_dis.valeur());
  la_zone_vdf = zvdf;
  la_zcl_vdf = zclvdf;

  porosite_surf.ref(zvdf.porosite_face());
  volume_entrelaces.ref(zvdf.volumes_entrelaces());
  face_voisins.ref(zvdf.face_voisins());
  orientation.ref(zvdf.orientation());
  xp.ref(zvdf.xp());

}

DoubleTab& Op_Grad_P0_to_Face::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  assert_espace_virtuel_vect(inco);
  //  Debog::verifier("Op_Grad_P0_to_Face::ajouter, inco 0 :",inco);
  const Zone_VDF& zvdf = la_zone_vdf.valeur();
  const Zone_Cl_VDF& zclvdf = la_zcl_vdf.valeur();
  const DoubleVect& face_surfaces = zvdf.face_surfaces();

  //  double coef;
  double dist;
  int n0, n1, ori;

  // Boucle sur les bords pour traiter les conditions aux limites

  //  int nb_elems = zvdf.nb_elem();
  //  const DoubleTab& xv = zvdf.xv();

  int ndeb, nfin, num_face;
  for (int n_bord=0; n_bord<zvdf.nb_front_Cl(); n_bord++)
    {

      const Cond_lim& la_cl = zclvdf.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      ndeb = le_bord.num_premiere_face();
      nfin = ndeb + le_bord.nb_faces();

      // Correction periodicite :
      if (sub_type(Periodique,la_cl.valeur()))
        {
          for (num_face=ndeb; num_face<nfin; num_face++)
            {
              n0 = face_voisins(num_face,0);
              n1 = face_voisins(num_face,1);
              // coef = volume_entrelaces(num_face)*porosite_surf(num_face);
              dist = volume_entrelaces(num_face)/face_surfaces(num_face);
              resu(num_face) += ((inco(n1) - inco(n0)))/dist;
            }
        }
      else
        {
          for (num_face=ndeb; num_face<nfin; num_face++)
            {
              n0 = face_voisins(num_face,0);
              if (n0<0) n0=face_voisins(num_face,1);
              ori = orientation(num_face);
              int face_opposee=zvdf.elem_faces(n0,ori);
              if (face_opposee==num_face) face_opposee=zvdf.elem_faces(n0,ori+dimension);
              n1 = face_voisins(face_opposee,0);
              if (n1==n0) n1=face_voisins(face_opposee,1);
              resu(num_face) += (inco(n1)-inco(n0))/(xp(n1,ori)- xp(n0,ori));
            }
        }
      // Fin de la boucle for
    }
  // Boucle sur les faces internes

  for (num_face=zvdf.premiere_face_int(); num_face<zvdf.nb_faces(); num_face++)
    {
      n0 = face_voisins(num_face,0);
      n1 = face_voisins(num_face,1);
      ori = orientation(num_face);

      // coef = volume_entrelaces(num_face)*porosite_surf(num_face);
      resu(num_face) += (inco(n1)-inco(n0))/(xp(n1,ori)- xp(n0,ori));
    }

  resu.echange_espace_virtuel();
  return resu;
}

DoubleTab& Op_Grad_P0_to_Face::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  resu=0;
  return ajouter(inco,resu);
}

int Op_Grad_P0_to_Face::impr(Sortie& os) const
{
  Cerr << "Op_Grad_P0_to_Face::impr is not coded !" << finl;
  exit();
  return 1;
}
