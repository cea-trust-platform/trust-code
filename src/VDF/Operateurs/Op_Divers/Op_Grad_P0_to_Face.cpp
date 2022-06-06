/****************************************************************************
* Copyright (c) 2022, CEA
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

#include <Check_espace_virtuel.h>
#include <Op_Grad_P0_to_Face.h>
#include <Zone_Cl_VDF.h>
#include <Periodique.h>
#include <Statistiques.h>

extern Stat_Counter_Id gradient_counter_;

Implemente_instanciable(Op_Grad_P0_to_Face,"Op_Grad_P0_to_Face",Op_Grad_VDF_Face_base);

Sortie& Op_Grad_P0_to_Face::printOn(Sortie& s) const { return s << que_suis_je(); }
Entree& Op_Grad_P0_to_Face::readOn(Entree& s) { return s; }

void Op_Grad_P0_to_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  return;
}

void Op_Grad_P0_to_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  statistiques().begin_count(gradient_counter_);
  const DoubleTab& inco = semi_impl.count("pression") ? semi_impl.at("pression") : equation().inconnue().valeur().valeurs();
  assert_espace_virtuel_vect(inco);
  const Zone_VDF& zvdf = la_zone_vdf.valeur();
  const Zone_Cl_VDF& zclvdf = la_zcl_vdf.valeur();
  const DoubleVect& face_surfaces = zvdf.face_surfaces();

  double dist;
  int n0, n1, ori;

  // Boucle sur les bords pour traiter les conditions aux limites
  for (int n_bord = 0; n_bord<zvdf.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zclvdf.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur())) // Correction periodicite
        {
          for (int num_face = ndeb; num_face < nfin; num_face++)
            {
              n0 = face_voisins(num_face,0), n1 = face_voisins(num_face,1);
              dist = volume_entrelaces(num_face) / face_surfaces(num_face);
              secmem(num_face) += ((inco(n1) - inco(n0)))/dist;
            }
        }
      else
        {
          for (int num_face = ndeb; num_face < nfin; num_face++)
            {
              n0 = face_voisins(num_face,0);
              if (n0<0) n0 = face_voisins(num_face,1);

              ori = orientation(num_face);
              int face_opposee = zvdf.elem_faces(n0,ori);
              if (face_opposee == num_face) face_opposee = zvdf.elem_faces(n0,ori+dimension);

              n1 = face_voisins(face_opposee,0);
              if (n1==n0) n1 = face_voisins(face_opposee,1);

              secmem(num_face) += (inco(n1)-inco(n0)) / (xp(n1,ori)- xp(n0,ori));
            }
        }
    }

  // Boucle sur les faces internes
  for (int num_face = zvdf.premiere_face_int(); num_face < zvdf.nb_faces(); num_face++)
    {
      n0 = face_voisins(num_face,0), n1 = face_voisins(num_face,1), ori = orientation(num_face);
      secmem(num_face) += (inco(n1)-inco(n0))/(xp(n1,ori)- xp(n0,ori));
    }

  secmem.echange_espace_virtuel();
  statistiques().end_count(gradient_counter_);

}
