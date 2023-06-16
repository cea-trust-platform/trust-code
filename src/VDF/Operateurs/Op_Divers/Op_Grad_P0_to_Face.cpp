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

#include <Check_espace_virtuel.h>
#include <Op_Grad_P0_to_Face.h>
#include <Pb_Multiphase.h>
#include <Statistiques.h>
#include <Domaine_Cl_VDF.h>
#include <Periodique.h>
#include <Neumann_paroi.h>
#include <Neumann_homogene.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Echange_impose_base.h>

extern Stat_Counter_Id gradient_counter_;

Implemente_instanciable(Op_Grad_P0_to_Face,"Op_Grad_P0_to_Face",Op_Grad_VDF_Face_base);

Sortie& Op_Grad_P0_to_Face::printOn(Sortie& s) const { return s << que_suis_je(); }
Entree& Op_Grad_P0_to_Face::readOn(Entree& s) { return s; }

void Op_Grad_P0_to_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const { return; }

void Op_Grad_P0_to_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  /*  if (sub_type(Pb_Multiphase, equation().probleme()))
      {
        Cerr << "Op_Grad_P0_to_Face::" << __func__ << " is not yet compatible with Pb_Multiphase !" << finl;
        Cerr << "You should instead use Op_Grad_VDF_Face ... Otherwise you should add the contripution of alpha to the secmem ..." << finl;
        Process::exit();
      }
  */
  statistiques().begin_count(gradient_counter_);
  const DoubleTab& inco = semi_impl.count("pression") ? semi_impl.at("pression") : equation().inconnue().valeur().valeurs();
  assert_espace_virtuel_vect(inco);
  const Domaine_VDF& zvdf = le_dom_vdf.valeur();
  const Domaine_Cl_VDF& zclvdf = la_zcl_vdf.valeur();
  const DoubleVect& face_surfaces = zvdf.face_surfaces();
  const DoubleTab& xv = zvdf.xv();

  double dist;
  int n0, n1, ori, N = inco.line_size(), k;

  // Boucle sur les bords pour traiter les conditions aux limites
  for (int n_bord = 0; n_bord<zvdf.nb_front_Cl(); n_bord++)
    for (k=0; k<N; k++)
      {
        const Cond_lim& la_cl = zclvdf.les_conditions_limites(n_bord);
        const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
        const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();

        if (sub_type(Periodique,la_cl.valeur())) // Correction periodicite
          for (int num_face = ndeb; num_face < nfin; num_face++)
            {
              n0 = face_voisins(num_face,0), n1 = face_voisins(num_face,1);
              dist = volume_entrelaces(num_face) / face_surfaces(num_face);
              secmem(num_face, k) -= (inco(n1, k) - inco(n0, k))/dist;
            }
        else if (sub_type(Dirichlet,la_cl.valeur())) // Cas CL Dirichlet
          for (int num_face = ndeb, num_face_cl=0; num_face < nfin; num_face++, num_face_cl++)
            {
              n0 = face_voisins(num_face,0);
              if (n0<0) n0 = face_voisins(num_face,1);
              ori = orientation(num_face);
              secmem(num_face, k) -= (inco(n0, k) - ref_cast(Dirichlet, la_cl.valeur()).val_imp(num_face_cl, k))/(xp(n0,ori)- xv(num_face,ori));
            }
        else if (sub_type(Dirichlet_homogene,la_cl.valeur())) // Cas Dirichlet homogene, i.e. valeur nulle a la paroi
          for (int num_face = ndeb; num_face < nfin; num_face++)
            {
              n0 = face_voisins(num_face,0);
              if (n0<0) n0 = face_voisins(num_face,1);
              ori = orientation(num_face);
              secmem(num_face, k) -= inco(n0, k)/(xp(n0,ori)- xv(num_face,ori));
            }
        else if (sub_type(Echange_impose_base,la_cl.valeur())) // Cas Echange_impose_base
          for (int num_face = ndeb, num_face_cl=0; num_face < nfin; num_face++, num_face_cl++)
            {
              n0 = face_voisins(num_face,0);
              if (n0<0) n0 = face_voisins(num_face,1);
              if ( face_voisins(num_face,0) >= 0 ) secmem(num_face, k) -= ( inco(n0, k) - ref_cast(Echange_impose_base, la_cl.valeur()).T_ext(num_face_cl, k))*ref_cast(Echange_impose_base, la_cl.valeur()).h_imp_grad(num_face_cl, k); // Si bien oriente
              else                                 secmem(num_face, k) += ( inco(n0, k) - ref_cast(Echange_impose_base, la_cl.valeur()).T_ext(num_face_cl, k))*ref_cast(Echange_impose_base, la_cl.valeur()).h_imp_grad(num_face_cl, k); // Si oriente a envers
            }
        else if (sub_type(Neumann_paroi,la_cl.valeur())) // Cas Neumann_paroi
          for (int num_face = ndeb, num_face_cl=0; num_face < nfin; num_face++, num_face_cl++)
            secmem(num_face, k) -= ref_cast(Neumann_paroi, la_cl.valeur()).flux_impose(num_face_cl, k); // Si bien oriente
        else if  (! sub_type(Neumann_homogene,la_cl.valeur())) // En Neumann homogene, i.e. symetrie, la derivee a la face est nulle => on fait rien
          for (int num_face = ndeb; num_face < nfin; num_face++)
            {
              n0 = face_voisins(num_face,0);
              if (n0<0) n0 = face_voisins(num_face,1);

              ori = orientation(num_face);
              int face_opposee = zvdf.elem_faces(n0,ori);
              if (face_opposee == num_face) face_opposee = zvdf.elem_faces(n0,ori+dimension);

              n1 = face_voisins(face_opposee,0);
              if ((n1<0) || ((n1==n0) && face_voisins(face_opposee,1)>=0)) n1 = face_voisins(face_opposee,1);

              if (n1!=n0) secmem(num_face, k) -= (inco(n1, k)-inco(n0, k)) / (xp(n1,ori)- xp(n0,ori));
            }
      }

  // Boucle sur les faces internes
  for (int num_face = zvdf.premiere_face_int(); num_face < zvdf.nb_faces(); num_face++)
    for (k=0; k<N; k++)
      {
        n0 = face_voisins(num_face,0), n1 = face_voisins(num_face,1), ori = orientation(num_face);
        secmem(num_face, k) -= (inco(n1, k)-inco(n0, k))/(xp(n1,ori)- xp(n0,ori));
      }

  secmem.echange_espace_virtuel();
  statistiques().end_count(gradient_counter_);
}
