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
// File:        Op_Diff_CoviMAC_Elem.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////
#include <Op_Diff_CoviMAC_Elem.h>
#include <Pb_Multiphase.h>
#include <Schema_Temps_base.h>
#include <Zone_CoviMAC.h>
#include <Zone_Cl_CoviMAC.h>
#include <IntLists.h>
#include <DoubleLists.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Neumann_paroi.h>
#include <Echange_contact_CoviMAC.h>
#include <Echange_externe_impose.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Champ_P0_CoviMAC.h>
#include <Champ_front_calc.h>
#include <Modele_turbulence_scal_base.h>
#include <Synonyme_info.h>
#include <communications.h>
#include <MD_Vector_base.h>
#include <cmath>

Implemente_instanciable_sans_constructeur( Op_Diff_CoviMAC_Elem          , "Op_Diff_CoviMAC_Elem|Op_Diff_CoviMAC_var_Elem"                                , Op_Diff_CoviMAC_base ) ;
Implemente_instanciable( Op_Dift_CoviMAC_Elem          , "Op_Dift_CoviMAC_P0_CoviMAC|Op_Dift_CoviMAC_var_P0_CoviMAC"                    , Op_Diff_CoviMAC_Elem ) ;
Implemente_instanciable( Op_Diff_Nonlinear_CoviMAC_Elem, "Op_Diff_nonlinear_CoviMAC_Elem|Op_Diff_nonlinear_CoviMAC_var_Elem"            , Op_Diff_CoviMAC_Elem ) ;
Implemente_instanciable( Op_Dift_Nonlinear_CoviMAC_Elem, "Op_Dift_CoviMAC_P0_CoviMAC_nonlinear|Op_Dift_CoviMAC_var_P0_CoviMAC_nonlinear", Op_Diff_CoviMAC_Elem ) ;

Op_Diff_CoviMAC_Elem::Op_Diff_CoviMAC_Elem()
{
  declare_support_masse_volumique(1);
}

Sortie& Op_Diff_CoviMAC_Elem::printOn( Sortie& os ) const
{
  return Op_Diff_CoviMAC_base::printOn( os );
}

Sortie& Op_Dift_CoviMAC_Elem::printOn( Sortie& os ) const
{
  return Op_Diff_CoviMAC_base::printOn( os );
}

Sortie& Op_Diff_Nonlinear_CoviMAC_Elem::printOn( Sortie& os ) const
{
  return Op_Diff_CoviMAC_base::printOn( os );
}

Sortie& Op_Dift_Nonlinear_CoviMAC_Elem::printOn( Sortie& os ) const
{
  return Op_Diff_CoviMAC_base::printOn( os );
}

Entree& Op_Diff_CoviMAC_Elem::readOn( Entree& is )
{
  return Op_Diff_CoviMAC_base::readOn( is );
}

Entree& Op_Diff_Nonlinear_CoviMAC_Elem::readOn( Entree& is )
{
  return Op_Diff_CoviMAC_base::readOn( is );
}

Entree& Op_Dift_CoviMAC_Elem::readOn( Entree& is )
{
  return Op_Diff_CoviMAC_base::readOn( is );
}

Entree& Op_Dift_Nonlinear_CoviMAC_Elem::readOn( Entree& is )
{
  return Op_Diff_CoviMAC_base::readOn( is );
}

void Op_Diff_CoviMAC_Elem::completer()
{
  Op_Diff_CoviMAC_base::completer();
  const Equation_base& eq = equation();
  const Champ_P0_CoviMAC& ch = ref_cast(Champ_P0_CoviMAC, eq.inconnue().valeur());
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 1)
    Cerr << "Op_Diff_CoviMAC_Elem : largeur de joint insuffisante (minimum 1)!" << finl, Process::exit();
  flux_bords_.resize(zone.premiere_face_int(), ch.valeurs().line_size());

  const RefObjU& modele_turbulence = eq.get_modele(TURBULENCE);
  if (modele_turbulence.non_nul())
    {
      const Modele_turbulence_scal_base& mod_turb = ref_cast(Modele_turbulence_scal_base,modele_turbulence.valeur());
      const Champ_Fonc& lambda_t = mod_turb.conductivite_turbulente();
      associer_diffusivite_turbulente(lambda_t);
    }
}

void Op_Diff_CoviMAC_Elem::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  update_phif(); //calcul de (nf.nu.grad T)
  const std::string nom_inco = equation().inconnue().le_nom().getString();
  if (semi_impl.count(nom_inco)) return; //semi-implicite -> rien a dimensionner
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins();
  int i, j, e, e_s, p_s, f, fb, m, n, n_sten = 0;
  std::vector<Matrice_Morse *> mat(op_ext.size());
  for (i = 0; i < (int) op_ext.size(); i++) //une matrice potentielle a remplir par operateur de op_ext
    mat[i] = matrices.count(i ? nom_inco + "_" + op_ext[i]->equation().probleme().le_nom().getString() : nom_inco) ?
             matrices.at(i ? nom_inco + "_" + op_ext[i]->equation().probleme().le_nom().getString() : nom_inco) : NULL;

  std::vector<int> N(op_ext.size()); //nombre de composantes par probleme de op_ext
  std::vector<IntTrav> stencil(op_ext.size()); //stencils par matrice
  for (i = 0; i < (int) op_ext.size(); i++)
    stencil[i].resize(0, 2), stencil[i].set_smart_resize(1), N[i] = op_ext[i]->equation().inconnue().valeurs().line_size();

  IntTrav tpfa(0, N[0]); //pour suivre quels flux sont a deux points
  zone.creer_tableau_faces(tpfa), tpfa = 1;

  Cerr << "Op_Diff_CoviMAC_Elem::dimensionner() : ";
  for (f = 0; f < zone.nb_faces(); f++) for (i = 0; i < 2; i++) if ((e = f_e(f, i)) >= 0 && e < zone.nb_elem()) //stencil a l'element e
        {
          for (j = phif_d(f, 0); j < phif_d(f + 1, 0); j++) if ((e_s = phif_e(j)) < zone.nb_elem_tot()) //partie "simple" (phif_e / phif_c) : ne melange pas les composantes
              for (n = 0; n < N[0]; n++) stencil[0].append_line(N[0] * e + n, N[0] * e_s + n), tpfa(f, n) &= (e_s == f_e(f, 0) || e_s == f_e(f, 1));
          for (j = phif_d(f, 1); j < phif_d(f + 1, 1); j++) //partie "complexe" (phif_pe / phif_pc) : melange les composantes et les problemes
            if (mat[p_s = phif_pe(j, 0)] && (e_s = phif_pe(j, 1)) < op_ext[p_s]->equation().zone_dis()->nb_elem_tot()) for (n = 0; n < N[0]; n++)
                {
                  tpfa(f, n) &= (p_s == 0 && (e_s == f_e(f, 0) || e_s == f_e(f, 1))) //amont/aval
                                || ((fb = zone.fbord(f)) >= 0 && p_s == pe_ext(fb, 0) && e_s == pe_ext(fb, 1)); //aval si Echange_contact
                  for (m = 0; m < N[p_s]; m++) stencil[p_s].append_line(N[0] * e + n, N[i] * e_s + m);
                }
        }

  for (i = 0; i < (int) op_ext.size(); i++) if (mat[i])
      {
        tableau_trier_retirer_doublons(stencil[i]);
        Matrice_Morse mat2;
        Matrix_tools::allocate_morse_matrix(N[0] * zone.nb_elem_tot(), N[i] * op_ext[i]->equation().zone_dis()->nb_elem_tot(), stencil[i], mat2);
        mat[i]->nb_colonnes() ? *mat[i] += mat2 : *mat[i] = mat2;
      }

  for (auto &&st : stencil) n_sten += st.dimension(0); //n_sten : nombre total de points du stencil de l'operateur
  Cerr << "width " << Process::mp_sum(n_sten) * 1. / (N[0] * zone.zone().md_vector_elements().valeur().nb_items_seq_tot())
       << " " << mp_somme_vect(tpfa) * 100. / (N[0] * zone.md_vector_faces().valeur().nb_items_seq_tot()) << "% TPFA " << finl;
}

void Op_Diff_CoviMAC_Elem::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  update_phif();

  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  int i, j, k, e, e_s, f, f_s, m, n, M = phif_pc.dimension(2), n_ext = op_ext.size(), p;
  std::vector<Matrice_Morse *> mat(n_ext); //matrices
  std::vector<int> N(n_ext); //composantes
  std::vector<const Zone_CoviMAC *> zone(n_ext); //zones
  std::vector<const Conds_lim *> cls(n_ext); //conditions aux limites
  std::vector<const IntTab *> fcl(n_ext); //tableaux "fcl"
  std::vector<const DoubleTab *> inco(n_ext); //inconnues
  for (i = 0; i < n_ext; i++)
    {
      std::string nom_mat = i ? nom_inco + "_" + op_ext[i]->equation().probleme().le_nom().getString() : nom_inco;
      mat[i] = !semi_impl.count(nom_inco) && matrices.count(nom_mat) ? matrices.at(nom_mat) : NULL;
      zone[i] = &ref_cast(Zone_CoviMAC, op_ext[i]->equation().zone_dis().valeur());
      cls[i] = &op_ext[i]->equation().zone_Cl_dis().les_conditions_limites();
      const Champ_P0_CoviMAC& ch = ref_cast(Champ_P0_CoviMAC, op_ext[i]->equation().inconnue().valeur());
      fcl[i] = &ch.fcl(), inco[i] = &ch.valeurs(), N[i] = inco[i]->line_size();
    }
  const IntTab& e_f = zone[0]->elem_faces(), &f_e = zone[0]->face_voisins();
  const DoubleVect& fs = zone[0]->face_surfaces();

  /* derivee de la contribution en l'element e : cder(ider[{p, e}], compo d'arrivee, compo de depart) */
  std::map<std::array<int, 2>, int> ider;
  DoubleTrav cder, fac, flux(N[0]);
  cder.set_smart_resize(1), fac.set_smart_resize(1);
  for (e = 0; e < zone[0]->nb_elem(); cder.resize(0, N[0], M), ider.clear(), e++)
    {
      for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) /* remplissage des seconds membres et de ider / cder */
        {
          double prefac = fs(f) * (e == f_e(f, 0) ? 1 : -1);
          for (flux = 0, j = phif_d(f, 0); j < phif_d(f + 1, 0); j++) //indices/coeffs simples
            {
              e_s = phif_e(j), f_s = e_s - zone[0]->nb_elem_tot();
              for (fac.resize(N[0]), n = 0; n < N[0]; n++) fac(n) = prefac * (phif_w(f, n) * phif_c(j, n, 0) + (1 - phif_w(f, n)) * phif_c(j, n, 1));
              if (f_s < 0) //element
                {
                  for (n = 0; n < N[0]; n++) flux(n) += fac(n) * (*inco[0])(e_s, n);
                  if (!mat[0]) continue;
                  if (!ider.count({{ 0, e_s }})) cder.resize((ider[ {{ 0, e_s }}] = cder.dimension(0)) + 1, N[0], M);
                  for (k = ider[ {{ 0, e_s }}], n = 0; n < N[0]; n++) cder(k, n, n) += fac(n);
                }
              else if ((*fcl[0])(f_s, 0) == 1 || (*fcl[0])(f_s, 0) == 2) for (n = 0; n < N[0]; n++) //Echange_impose_base
                  flux(n) += fac(n) * ref_cast(Echange_impose_base, (*cls[0])[(*fcl[0])(f_s, 1)].valeur()).T_ext((*fcl[0])(f_s, 2), n);
              else if ((*fcl[0])(f_s, 0) == 4) for (n = 0; n < N[0]; n++) //Neumann non homogene
                  flux(n) += fac(n) * ref_cast(Neumann_paroi, (*cls[0])[(*fcl[0])(f_s, 1)].valeur()).flux_impose((*fcl[0])(f_s, 2), n);
              else if ((*fcl[0])(f_s, 0) == 6) for (n = 0; n < N[0]; n++) //Dirichlet
                  flux(n) += fac(n) * ref_cast(Dirichlet, (*cls[0])[(*fcl[0])(f_s, 1)].valeur()).val_imp((*fcl[0])(f_s, 2), n);
            }
          for (j = phif_d(f, 1); j < phif_d(f + 1, 1); j++) //indices/coeffs complexes
            {
              p = phif_pe(j, 0), e_s = phif_pe(j, 1), f_s = e_s - zone[p]->nb_elem_tot();
              for (fac.resize(N[0], N[p]), n = 0; n < N[0]; n++) for (m = 0; m < N[p]; m++)
                  fac(n, m) = prefac * (phif_w(f, n) * phif_pc(j, n, m, 0) + (1 - phif_w(f, n)) * phif_pc(j, n, m, 1));
              if (f_s < 0) //element
                {
                  for (n = 0; n < N[0]; n++) for (m = 0; m < N[p]; m++) flux(n) += fac(n, m) * (*inco[p])(e_s, m);
                  if (!mat[p]) continue;
                  if (!ider.count({{ p, e_s }})) cder.resize((ider[ {{ p, e_s }}] = cder.dimension(0)) + 1, N[0], M);
                  for (k = ider[ {{ p, e_s }}], n = 0; n < N[0]; n++) for (m = 0; m < N[p]; m++) cder(k, n, m) += fac(n, m);
                }
              else if ((*fcl[p])(f_s, 0) == 1 || (*fcl[p])(f_s, 0) == 2) for (n = 0; n < N[0]; n++) for (m = 0; m < N[p]; m++) //Echange_impose_base
                    flux(n) += fac(n, m) * ref_cast(Echange_impose_base, (*cls[p])[(*fcl[p])(f_s, 1)].valeur()).T_ext((*fcl[p])(f_s, 2), m);
              else if ((*fcl[p])(f_s, 0) == 4) for (n = 0; n < N[0]; n++) for (m = 0; m < N[p]; m++) //Neumann non homogene
                    flux(n) += fac(n, m) * ref_cast(Neumann_paroi, (*cls[p])[(*fcl[p])(f_s, 1)].valeur()).flux_impose((*fcl[p])(f_s, 2), m);
              else if ((*fcl[p])(f_s, 0) == 6) for (n = 0; n < N[0]; n++) for (m = 0; m < N[p]; m++) //Dirichlet
                    flux(n) += fac(n, m) * ref_cast(Dirichlet, (*cls[p])[(*fcl[p])(f_s, 1)].valeur()).val_imp((*fcl[p])(f_s, 2), m);
            }
          for (n = 0; n < N[0]; n++) secmem(e, n) += flux(n);
          if (f < zone[0]->premiere_face_int()) for (n = 0; n < N[0]; n++) flux_bords_(f, n) = flux(n);
        }
      /* remplissage des matrices a partir de ider / cder */
      for (n = 0; n < N[0]; n++) for (auto &&pe_k : ider) for (p = pe_k.first[0], e_s = pe_k.first[1], k = pe_k.second, m = 0; m < N[p]; m++) if (cder(k, n, m))
              (*mat[p])(N[0] * e + n, N[p] * e_s + m) -= cder(k, n, m);
    }
}
