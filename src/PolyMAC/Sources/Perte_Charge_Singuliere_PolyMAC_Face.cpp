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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Perte_Charge_Singuliere_PolyMAC_Face.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Sources
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <Perte_Charge_Singuliere_PolyMAC_Face.h>
#include <Zone_PolyMAC_P0.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Pb_Multiphase.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Champ_Face_PolyMAC.h>
#include <Op_Grad_PolyMAC_Face.h>
#include <Motcle.h>
#include <Domaine.h>
#include <Matrice_Morse.h>
#include <Param.h>

Implemente_instanciable(Perte_Charge_Singuliere_PolyMAC_Face,"Perte_Charge_Singuliere_Face_PolyMAC|Perte_Charge_Singuliere_Face_PolyMAC_P0",Perte_Charge_PolyMAC_Face);

//// printOn
//

Sortie& Perte_Charge_Singuliere_PolyMAC_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Perte_Charge_Singuliere_PolyMAC_Face::readOn(Entree& s)
{
  Perte_Charge_Singuliere::lire_donnees(s);
  remplir_num_faces(s);
  if (regul_) //fichier de sortie si regulation
    {
      bilan().resize(3); //K deb cible
      set_fichier(Nom("K_") + identifiant_);
      set_description(Nom("Regulation du Ksing de la surface ") + identifiant_ + "\nt K deb cible");
    }
  return s;
}


/////////////////////////////////////////////////////////////////////
//
//                    Implementation des fonctions
//
//               de la classe Perte_Charge_Singuliere_PolyMAC_Face
//
////////////////////////////////////////////////////////////////////

void Perte_Charge_Singuliere_PolyMAC_Face::remplir_num_faces(Entree& s)
{
  const Domaine& le_domaine = equation().probleme().domaine();
  const Zone_Poly_base& zone_poly = ref_cast(Zone_Poly_base,equation().zone_dis().valeur());
  int taille_bloc = zone_poly.nb_elem();
  num_faces.resize(taille_bloc);
  lire_surfaces(s,le_domaine,zone_poly,num_faces, sgn);
  // int nfac_tot = mp_sum(num_faces.size());
  int nfac_max = (int)mp_max(num_faces.size()); // not to count several (number of processes) times the same face

  if(je_suis_maitre() && nfac_max == 0)
    {
      Cerr << "Error when defining the surface plane for the singular porosity :" << finl;
      Cerr << "No mesh faces has been found for the surface plane." << finl;
      Cerr << "Check the coordinate of the surface plane which should match mesh coordinates." << finl;
      exit();
    }
}

void Perte_Charge_Singuliere_PolyMAC_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Zone_Poly_base& zone = ref_cast(Zone_Poly_base, equation().zone_dis().valeur());
  const IntTab& f_e = zone.face_voisins(), &fcl = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur()).fcl();
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  if (!matrices.count(nom_inco) || !sub_type(Zone_PolyMAC_P0, zone)) return;
  Matrice_Morse& mat = *matrices.at(nom_inco), mat2;
  int i, j, e, f, n, N = equation().inconnue().valeurs().line_size(), d, D = dimension, nf_tot = zone.nb_faces_tot();
  DoubleTrav aar_f(N); //alpha * alpha * rho a chaque face
  IntTrav stencil(0, 2);
  stencil.set_smart_resize(1);

  for (i = 0; i < num_faces.size(); i++)
    if ((f = num_faces(i)) < zone.nb_faces() && fcl(f, 0) < 2)
      for (j = 0; j < 2 && (e = f_e(f, j)) >= 0; j++) //elem amont / aval si PolyMAC V2
        if (e < zone.nb_elem())
          for (d = 0; d < D; d++)
            for (n = 0; n < N; n++) stencil.append_line(N * (nf_tot + D * e + d) + n, N * f + n);
  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(equation().inconnue().valeurs().size_totale(), equation().inconnue().valeurs().size_totale(), stencil, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

void Perte_Charge_Singuliere_PolyMAC_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Zone_Poly_base& zone = ref_cast(Zone_Poly_base, equation().zone_dis().valeur());
  //const DoubleVect& volumes_entrelaces = zone_PolyMAC.volumes_entrelaces();
  const DoubleVect& pf = zone.porosite_face(), &fs = zone.face_surfaces(), &vf = zone.volumes_entrelaces();
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : NULL;
  const DoubleTab& vit = la_vitesse->valeurs(), &nf = zone.face_normales(), &vfd = zone.volumes_entrelaces_dir(), &xv = zone.xv(), &xp = zone.xp(),
                   *alpha = pbm ? &pbm->eq_masse.inconnue().passe() : NULL, *a_r = pbm ? &pbm->eq_masse.champ_conserve().passe() : NULL;
  const IntTab& f_e = zone.face_voisins(), &fcl = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur()).fcl();
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  Matrice_Morse *mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : NULL;
  int i, j, e, f, n, N = equation().inconnue().valeurs().line_size(), poly_v2 = sub_type(Zone_PolyMAC_P0, zone), semi = semi_impl.count(nom_inco), d, D = dimension, nf_tot = zone.nb_faces_tot();
  DoubleTrav aar_f(N); //alpha * alpha * rho a chaque face
  for (i = 0; i < num_faces.size(); i++)
    if ((f = num_faces(i)) < zone.nb_faces())
      {
        double fac = (direction_perte_charge() < 0 ? fs(f) : std::fabs(nf(f,direction_perte_charge()))) * pf(f) * K();
        if (pbm)
          for (aar_f = 0, j = 0; j < 2 && (e = f_e(f, j)) >= 0; j++)
            for (n = 0; n < N; n++) aar_f(n) += vfd(f, j) / vf(f) * (*a_r)(e, n) * (*alpha)(e, n);
        else aar_f = 1;

        if (!poly_v2 || fcl(f, 0) < 2) //contrib a la face : sauf si face de Dirichlet/Neumann en V2
          {
            for (n = 0; n < N; n++)  secmem(f, n) -= 0.5 * fac * aar_f(n) * vit(f, n) * std::fabs(vit(f, n));
            if (mat)
              for (n = 0; n < N; n++) (*mat)(N * f + n, N * f + n) += fac * aar_f(n) * std::fabs(vit(f, n));
          }
        if (poly_v2)
          for (j = 0; j < 2 && (e = f_e(f, j)) >= 0; j++)
            if (e < zone.nb_elem()) //elem amont / aval si PolyMAC V2
              {
                for (d = 0; d < D; d++)
                  for (n = 0; n < N; n++) /* second membre */
                    secmem(nf_tot + D * e + d, n) += (j ? -1 : 1) * fs(f) * (xv(f, d) - xp(e, d)) * 0.5 * fac * aar_f(n) * vit(f, n) * std::fabs(vit(f, n));
                if (mat && !semi && fcl(f, 0) < 2)
                  for (d = 0; d < D; d++)
                    for (n = 0; n < N; n++) /* derivee (pas possible en semi-implicite) */
                      (*mat)(N * (nf_tot + D * e + d) + n, N * f + n) -= (j ? -1 : 1) * fs(f) * (xv(f, d) - xp(e, d)) * fac * aar_f(n) * std::fabs(vit(f, n));
              }
      }
}

void Perte_Charge_Singuliere_PolyMAC_Face::mettre_a_jour(double temps)
{
  Perte_Charge_PolyMAC_Face::mettre_a_jour(temps);
  update_K(equation(), calculate_Q(equation(), num_faces, sgn), bilan());
}
