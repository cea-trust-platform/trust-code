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

#include <Op_Grad_PolyMAC_Face.h>
#include <Neumann_sortie_libre.h>
#include <Check_espace_virtuel.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Champ_Elem_PolyMAC.h>
#include <Navier_Stokes_std.h>
#include <Schema_Temps_base.h>
#include <Probleme_base.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <TRUSTTrav.h>
#include <Dirichlet.h>

Implemente_instanciable(Op_Grad_PolyMAC_Face, "Op_Grad_PolyMAC_Face", Operateur_Grad_base);

Sortie& Op_Grad_PolyMAC_Face::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Op_Grad_PolyMAC_Face::readOn(Entree& s) { return s; }

void Op_Grad_PolyMAC_Face::associer(const Domaine_dis& domaine_dis, const Domaine_Cl_dis& domaine_Cl_dis, const Champ_Inc&)
{
  const Domaine_PolyMAC& zPolyMAC_P0P1NC = ref_cast(Domaine_PolyMAC, domaine_dis.valeur());
  const Domaine_Cl_PolyMAC& zclPolyMAC_P0P1NC = ref_cast(Domaine_Cl_PolyMAC, domaine_Cl_dis.valeur());
  ref_domaine = zPolyMAC_P0P1NC;
  ref_zcl = zclPolyMAC_P0P1NC;
  porosite_surf.ref(equation().milieu().porosite_face());
  face_voisins.ref(zPolyMAC_P0P1NC.face_voisins());
}

void Op_Grad_PolyMAC_Face::dimensionner(Matrice_Morse& mat) const
{
  if (has_interface_blocs())
    {
      Operateur_Grad_base::dimensionner(mat);
      return;
    }

  const Domaine_PolyMAC& zPolyMAC_P0P1NC = ref_domaine.valeur();
  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);
  for (int f = 0; f < zPolyMAC_P0P1NC.nb_faces(); f++)
    for (int i = 0, e; i < 2 && (e = zPolyMAC_P0P1NC.face_voisins(f, i)) >= 0; i++)
      stencil.append_line(f, e);
  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(zPolyMAC_P0P1NC.nb_faces_tot(), zPolyMAC_P0P1NC.nb_elem_tot(), stencil, mat);
}

DoubleTab& Op_Grad_PolyMAC_Face::ajouter(const DoubleTab& inco, DoubleTab& resu) const
{
  if (has_interface_blocs()) return Operateur_Grad_base::ajouter(inco, resu);

  assert_espace_virtuel_vect(inco);
  const Domaine_PolyMAC& zPolyMAC_P0P1NC = ref_domaine.valeur();
  const Domaine_Cl_PolyMAC& zclPolyMAC_P0P1NC = ref_zcl.valeur();
  const DoubleVect& face_surfaces = zPolyMAC_P0P1NC.face_surfaces();

  double coef;
  int n0, n1;

  // Boucle sur les bords pour traiter les conditions aux limites
  int ndeb, nfin, num_face;
  for (int n_bord = 0; n_bord < zPolyMAC_P0P1NC.nb_front_Cl(); n_bord++)
    {

      // pour chaque Condition Limite on regarde son type
      // Si face de Dirichlet ou de Symetrie on ne fait rien
      // Si face de Neumann on calcule la contribution au terme source

      const Cond_lim& la_cl = zclPolyMAC_P0P1NC.les_conditions_limites(n_bord);
      if (sub_type(Neumann_sortie_libre, la_cl.valeur()))
        {
          const Neumann_sortie_libre& la_cl_typee = ref_cast(Neumann_sortie_libre, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          for (num_face = ndeb; num_face < nfin; num_face++)
            {
              double P_imp = la_cl_typee.flux_impose(num_face - ndeb);
              n0 = face_voisins(num_face, 0);
              if (n0 != -1)
                {
                  coef = face_surfaces(num_face) * porosite_surf(num_face);
                  resu(num_face) += (coef * (P_imp - inco(n0)));
                }
              else
                {
                  n1 = face_voisins(num_face, 1);
                  coef = face_surfaces(num_face) * porosite_surf(num_face);
                  resu(num_face) += (coef * (inco(n1) - P_imp));
                }
            }
        }
      // Fin de la boucle for
    }

  // Boucle sur les faces internes
  for (num_face = zPolyMAC_P0P1NC.premiere_face_int(); num_face < zPolyMAC_P0P1NC.nb_faces(); num_face++)
    {
      n0 = face_voisins(num_face, 0);
      n1 = face_voisins(num_face, 1);
      coef = face_surfaces(num_face) * porosite_surf(num_face);
      resu(num_face) += coef * (inco(n1) - inco(n0));
    }
  resu.echange_espace_virtuel();
  return resu;
}

DoubleTab& Op_Grad_PolyMAC_Face::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  resu = 0.;
  return ajouter(inco, resu);
}

void Op_Grad_PolyMAC_Face::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& la_matrice) const
{
  if (has_interface_blocs())
    {
      Operateur_Grad_base::contribuer_a_avec(inco, la_matrice);
      return;
    }

  assert_espace_virtuel_vect(inco);
  const Domaine_PolyMAC& zPolyMAC_P0P1NC = ref_domaine.valeur();
  const Domaine_Cl_PolyMAC& zclPolyMAC_P0P1NC = ref_zcl.valeur();
  const DoubleVect& face_surfaces = zPolyMAC_P0P1NC.face_surfaces();

  double coef;
  int n0, n1;

  // Boucle sur les bords pour traiter les conditions aux limites
  int ndeb, nfin, num_face;
  for (int n_bord = 0; n_bord < zPolyMAC_P0P1NC.nb_front_Cl(); n_bord++)
    {

      // pour chaque Condition Limite on regarde son type
      // Si face de Dirichlet ou de Symetrie on ne fait rien
      // Si face de Neumann on calcule la contribution au terme source

      const Cond_lim& la_cl = zclPolyMAC_P0P1NC.les_conditions_limites(n_bord);
      if (sub_type(Neumann_sortie_libre, la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          for (num_face = ndeb; num_face < nfin; num_face++)
            {
              n0 = face_voisins(num_face, 0);
              if (n0 != -1)
                {
                  coef = face_surfaces(num_face) * porosite_surf(num_face);
                  la_matrice(num_face, n0) += coef;
                }
              else
                {
                  n1 = face_voisins(num_face, 1);
                  coef = face_surfaces(num_face) * porosite_surf(num_face);
                  la_matrice(num_face, n1) -= coef;
                }
            }
        }
      // Fin de la boucle for
    }

  // Boucle sur les faces internes
  for (num_face = zPolyMAC_P0P1NC.premiere_face_int(); num_face < zPolyMAC_P0P1NC.nb_faces(); num_face++)
    {
      n0 = face_voisins(num_face, 0);
      n1 = face_voisins(num_face, 1);
      coef = face_surfaces(num_face) * porosite_surf(num_face);
      la_matrice(num_face, n0) += coef;
      la_matrice(num_face, n1) -= coef;
    }
}

int Op_Grad_PolyMAC_Face::impr(Sortie& os) const
{
  return 0;
}
