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
// File:        Op_Diff_PolyMAC_Elem.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////
#include <Op_Diff_PolyMAC_Elem.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Zone_PolyMAC.h>
#include <Zone_Cl_PolyMAC.h>
#include <IntLists.h>
#include <DoubleLists.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Neumann_paroi.h>
#include <Echange_contact_PolyMAC.h>
#include <Echange_externe_impose.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Champ_P0_PolyMAC.h>
#include <Champ_front_calc.h>
#include <Modele_turbulence_scal_base.h>
#include <Synonyme_info.h>
#include <communications.h>
#include <cmath>

Implemente_instanciable( Op_Diff_PolyMAC_Elem, "Op_Diff_PolyMAC_Elem|Op_Dift_PolyMAC_P0_PolyMAC", Op_Diff_PolyMAC_base ) ;
Add_synonym(Op_Diff_PolyMAC_Elem, "Op_Diff_PolyMAC_var_Elem");
Add_synonym(Op_Diff_PolyMAC_Elem, "Op_Dift_PolyMAC_var_P0_PolyMAC");


Sortie& Op_Diff_PolyMAC_Elem::printOn( Sortie& os ) const
{
  Op_Diff_PolyMAC_base::printOn( os );
  return os;
}

Entree& Op_Diff_PolyMAC_Elem::readOn( Entree& is )
{
  Op_Diff_PolyMAC_base::readOn( is );
  return is;
}

void Op_Diff_PolyMAC_Elem::completer()
{
  Op_Diff_PolyMAC_base::completer();
  const Champ_P0_PolyMAC& ch = ref_cast(Champ_P0_PolyMAC, equation().inconnue().valeur());
  const Zone_PolyMAC& zone = la_zone_poly_.valeur();
  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 1)
    Cerr << "Op_Diff_PolyMAC_Elem : largeur de joint insuffisante (minimum 1)!" << finl, Process::exit();
  ch.init_cl();
  int nb_comp = (equation().que_suis_je() == "Transport_K_Eps") ? 2 : 1;
  flux_bords_.resize(zone.premiere_face_int(), nb_comp);

  if (que_suis_je() == "Op_Diff_PolyMAC_Elem") return;
  const RefObjU& modele_turbulence = equation().get_modele(TURBULENCE);
  const Modele_turbulence_scal_base& mod_turb = ref_cast(Modele_turbulence_scal_base,modele_turbulence.valeur());
  const Champ_Fonc& lambda_t = mod_turb.conductivite_turbulente();
  associer_diffusivite_turbulente(lambda_t);
}

void Op_Diff_PolyMAC_Elem::dimensionner(Matrice_Morse& mat) const
{
  const Champ_P0_PolyMAC& ch = ref_cast(Champ_P0_PolyMAC, equation().inconnue().valeur());
  const Zone_PolyMAC& zone = la_zone_poly_.valeur();
  const IntTab& e_f = zone.elem_faces();
  int i, j, k, l, e, f, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), n, N = ch.valeurs().line_size();

  zone.init_m2();

  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);
  for (e = 0; e < zone.nb_elem_tot(); e++)
    {
      //dependance en les Te : diagonale -> faces autour de chaque element
      if (e < zone.nb_elem()) for (n = 0; n < N; n++) stencil.append_line(N * e + n, N * e + n);
      for (i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) for (n = 0; f < zone.nb_faces() && n < N; n++)
          stencil.append_line(N * (ne_tot + f) + n, N * e + n);

      //dependence en les Tf
      for (j = 0, k = zone.m2d(e); k < zone.m2d(e + 1); j++, k++) for (f = e_f(e, j), l = zone.w2i(k); l < zone.w2i(k + 1); l++)
          {
            //blocs superieurs : divergence
            for (n = 0; e < zone.nb_elem() && n < N; n++) stencil.append_line(N * e + n, N * (ne_tot + e_f(e, zone.w2j(l))) + n);

            //blocs inferieurs : continuite
            for (n = 0; f < zone.nb_faces() && n < N; n++) stencil.append_line(N * (ne_tot + f) + n, N * (ne_tot + e_f(e, zone.w2j(l))) + n);
          }
    }

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(N * (ne_tot + nf_tot), N * (ne_tot + nf_tot), stencil, mat);
}

void Op_Diff_PolyMAC_Elem::get_items_croises(const Probleme_base& autre_pb, extra_item_t& extra_items) const
{
  const Conds_lim& cls = la_zcl_poly_->les_conditions_limites();
  for (int i = 0; i < cls.size(); i++) if (sub_type(Echange_contact_PolyMAC, cls[i].valeur()))
      {
        const Echange_contact_PolyMAC& cl = ref_cast(Echange_contact_PolyMAC, cls[i].valeur());
        if (cl.nom_autre_pb() != autre_pb.le_nom()) continue; //not our problem
        for (auto && kv : cl.extra_items) extra_items[kv.first] = -1; //on ajoute les items dont la CL a besoin
      }
}

void Op_Diff_PolyMAC_Elem::dimensionner_termes_croises(Matrice_Morse& matrice, const Probleme_base& autre_pb, const extra_item_t& extra_items, int nl, int nc) const
{
  const Champ_P0_PolyMAC& ch = ref_cast(Champ_P0_PolyMAC, equation().inconnue().valeur());
  const Zone_PolyMAC& zone = la_zone_poly_.valeur();
  const Conds_lim& cls = la_zcl_poly_->les_conditions_limites();
  int i, j, k, l, f, n, N = ch.valeurs().line_size(), ne_tot = zone.nb_elem_tot();

  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);
  for (i = 0; i < cls.size(); i++) if (sub_type(Echange_contact_PolyMAC, cls[i].valeur()))
      {
        const Echange_contact_PolyMAC& cl = ref_cast(Echange_contact_PolyMAC, cls[i].valeur());
        if (cl.nom_autre_pb() != autre_pb.le_nom()) continue; //not our problem
        /* mise a jour de remote_item a l'aide de extra_items */
        for (auto &&kv : cl.extra_items) cl.remote_item(kv.second[0], kv.second[1]) = extra_items.at(kv.first);

        /* stencil */
        const Front_VF& fvf = ref_cast(Front_VF, cl.frontiere_dis());
        for (j = 0; j < cl.remote_item.dimension(0); j++)
          for (k = 0, f = fvf.num_face(j); k < cl.remote_item.dimension(1) && (l = cl.remote_item(j, k)) >= 0; k++)
            for (n = 0; n < N; n++) stencil.append_line(N * (ne_tot + f) + n, N * l + n);
      }

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(nl, nc, stencil, matrice);
}

void Op_Diff_PolyMAC_Elem::contribuer_termes_croises(const DoubleTab& inco, Matrice_Morse& matrice, const Probleme_base& autre_pb) const
{
  const Champ_P0_PolyMAC& ch = ref_cast(Champ_P0_PolyMAC, equation().inconnue().valeur());
  const Zone_PolyMAC& zone = la_zone_poly_.valeur();
  const Conds_lim& cls = la_zcl_poly_->les_conditions_limites();
  int i, j, k, l, f, n, N = ch.valeurs().line_size(), ne_tot = zone.nb_elem_tot();

  for (i = 0; i < cls.size(); i++) if (sub_type(Echange_contact_PolyMAC, cls[i].valeur()) && ref_cast(Echange_contact_PolyMAC, cls[i].valeur()).monolithic)
    ref_cast_non_const(Echange_contact_PolyMAC, cls[i].valeur()).update_coeffs(equation().schema_temps().temps_courant());

  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);
  for (i = 0; i < cls.size(); i++) if (sub_type(Echange_contact_PolyMAC, cls[i].valeur()))
      {
        const Echange_contact_PolyMAC& cl = ref_cast(Echange_contact_PolyMAC, cls[i].valeur());
        if (cl.nom_autre_pb() != autre_pb.le_nom()) continue; //not our problem
        const Front_VF& fvf = ref_cast(Front_VF, cl.frontiere_dis());
        for (j = 0; j < fvf.nb_faces(); j++)
          for (k = 0, f = fvf.num_face(j); k < cl.remote_item.dimension(1) && (l = cl.remote_item(j, k)) >= 0; k++) for (n = 0; n < N; n++)
              matrice(N * (ne_tot + f) + n, N * l + n) += cl.remote_coeff(j, k + 1);
      }
}

void Op_Diff_PolyMAC_Elem::calculer_flux_bord(const DoubleTab& inco) const
{
  abort();
}

// Description:
// ajoute la contribution de la diffusion au second membre resu
// renvoie resu
DoubleTab& Op_Diff_PolyMAC_Elem::ajouter(const DoubleTab& inco,  DoubleTab& resu) const
{
  const Champ_P0_PolyMAC& ch = ref_cast(Champ_P0_PolyMAC, equation().inconnue().valeur());
  const Zone_PolyMAC& zone = la_zone_poly_.valeur();
  const Conds_lim& cls = la_zcl_poly_->les_conditions_limites();
  const IntTab& e_f = zone.elem_faces();
  const DoubleVect& fs = zone.face_surfaces(), &ve = zone.volumes();
  const DoubleTab& xp = zone.xp(), &xv = zone.xv();
  int i, j, k, e, f, fb, ne_tot = zone.nb_elem_tot(), n, N = inco.line_size(), N_nu = nu_.line_size();
  double fac;

  update_nu(equation().schema_temps().temps_courant());
  for (i = 0; i < cls.size(); i++) if (sub_type(Echange_contact_PolyMAC, cls[i].valeur()) && ref_cast(Echange_contact_PolyMAC, cls[i].valeur()).monolithic)
    ref_cast_non_const(Echange_contact_PolyMAC, cls[i].valeur()).update_coeffs(equation().schema_temps().temps_courant());
    
  DoubleTrav nu_ef(e_f.dimension(1), N), mff(N), mfe(N), mee(N);
  flux_bords_ = 0;
  for (e = 0; e < ne_tot; e++)
    {
      int n_f = zone.m2d(e + 1) - zone.m2d(e); //nombre de faces de l'element e
      /* 1. diffusivites dans l'element e : nu_ef */
      for (i = 0; i < n_f; i++)
        {
          f = e_f(e, i);
          /* diffusivite de chaque composante dans la direction (xf - xe) */
          if (N_nu == N) for (n = 0; n < N; n++) nu_ef(i, n) = nu_.addr()[N * e + n]; //isotrope
          else if (N_nu == N * dimension) for (n = 0; n < N; n++) for (j = 0, nu_ef(i, n) = 0; j < dimension; j++) //anisotrope diagonal
                nu_ef(i, n) += nu_.addr()[dimension * (N * e + n) + j] * std::pow(xv(f, j) - xp(e, j), 2);
          else if (N_nu == N * dimension * dimension) for (n = 0; n < N; n++) for (j = 0, nu_ef(i, n) = 0; j < dimension; j++) for (k = 0; k < dimension; k++)
                  nu_ef(i, n) += nu_.addr()[dimension * (dimension * (N * e + n) + j) + k] * (xv(f, j) - xp(e, j)) * (xv(f, k) - xp(e, k)); //anisotrope complet
          else abort();
          for (n = 0, fac = nu_fac_.addr()[f] * (N_nu > N ? 1. / zone.dot(&xv(f, 0), &xv(f, 0), &xp(e, 0), &xp(e, 0)) : 1); n < N; n++) nu_ef(i, n) *= fac;
        }

      /* operateur : divergence pour les lignes aux elements, continuite pour les lignes aux faces */
      for (i = 0, mee = 0; i < n_f; i++, mee += mfe)
        {
          for (f = e_f(e, i), j = zone.w2i(zone.m2d(e) + i), mfe = 0; j < zone.w2i(zone.m2d(e) + i + 1); j++, mfe += mff)
            {
              for (fb = e_f(e, zone.w2j(j)), n = 0, fac = fs(f) * fs(fb) / ve(e) * zone.w2c(j); n < N; n++) mff(n) = fac * nu_ef(zone.w2j(j), n);
              for (n = 0; ch.icl(f, 0) < 6 && n < N; n++) resu.addr()[N * (ne_tot + f) + n] -= mff(n) * inco.addr()[N * (ne_tot + fb) + n];
              for (n = 0; f < zone.premiere_face_int() && n < N; n++) flux_bords_(f, n) -= mff(n) * inco.addr()[N * (ne_tot + fb) + n];
              for (n = 0; n < N; n++) resu.addr()[N * e + n] += mff(n) * inco.addr()[N * (ne_tot + fb) + n];
            }
          for (n = 0; ch.icl(f, 0) < 6 && n < N; n++) resu.addr()[N * (ne_tot + f) + n] += mfe(n) * inco.addr()[N * e + n];
          for (n = 0; f < zone.premiere_face_int() && n < N; n++) flux_bords_(f, n) += mfe(n) * inco.addr()[N * e + n];

          //Echange_impose_base
          if (ch.icl(f, 0) > 0 && ch.icl(f, 0) < 2 && f < zone.nb_faces()) for (n = 0; n < N; n++)
              resu.addr()[N * (ne_tot + f) + n] -= fs(f) * ref_cast(Echange_impose_base, cls[ch.icl(f, 1)].valeur()).h_imp(ch.icl(f, 2), n)
                                                   * (inco.addr()[N * (ch.icl(f, 0) == 1 ? ne_tot + f : e) + n] - ref_cast(Echange_impose_base, cls[ch.icl(f, 1)].valeur()).T_ext(ch.icl(f, 2), n));
          else if (ch.icl(f, 0) == 3 && f < zone.nb_faces()) for (n = 0; n < N; n++) //paroi_contact gere en monolithique
              resu.addr()[N * (ne_tot + f) + n] -= inco.addr()[N * (ne_tot + f) + n] * ref_cast(Echange_contact_PolyMAC, cls[ch.icl(f, 1)].valeur()).remote_coeff(ch.icl(f, 2), 0)
                                                   + ref_cast(Echange_contact_PolyMAC, cls[ch.icl(f, 1)].valeur()).remote_contrib(ch.icl(f, 2), 0);
        }
      for (n = 0; n < N; n++) resu.addr()[N * e + n] -= mee(n) * inco.addr()[N * e + n];
    }

  return resu;
}

//Description:
//on assemble la matrice.
void Op_Diff_PolyMAC_Elem::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  const Champ_P0_PolyMAC& ch = ref_cast(Champ_P0_PolyMAC, equation().inconnue().valeur());
  const Zone_PolyMAC& zone = la_zone_poly_.valeur();
  const Conds_lim& cls = la_zcl_poly_->les_conditions_limites();
  const IntTab& e_f = zone.elem_faces();
  const DoubleVect& fs = zone.face_surfaces(), &ve = zone.volumes();
  const DoubleTab& xp = zone.xp(), &xv = zone.xv();
  int i, j, k, e, f, fb, ne_tot = zone.nb_elem_tot(), n, N = inco.line_size(), N_nu = nu_.line_size();
  double fac;

  update_nu(equation().schema_temps().temps_courant());
  for (i = 0; i < cls.size(); i++) if (sub_type(Echange_contact_PolyMAC, cls[i].valeur()) && ref_cast(Echange_contact_PolyMAC, cls[i].valeur()).monolithic)
    ref_cast_non_const(Echange_contact_PolyMAC, cls[i].valeur()).update_coeffs(equation().schema_temps().temps_courant());
  DoubleTrav nu_ef(e_f.dimension(1), N), mff(N), mfe(N), mee(N);
  for (e = 0; e < ne_tot; e++)
    {
      int n_f = zone.m2d(e + 1) - zone.m2d(e); //nombre de faces de l'element e
      /* 1. diffusivites dans l'element e : nu_ef */
      for (i = 0; i < n_f; i++)
        {
          f = e_f(e, i);
          /* diffusivite de chaque composante dans la direction (xf - xe) */
          if (N_nu == N) for (n = 0; n < N; n++) nu_ef(i, n) = nu_.addr()[N * e + n]; //isotrope
          else if (N_nu == N * dimension) for (n = 0; n < N; n++) for (j = 0, nu_ef(i, n) = 0; j < dimension; j++) //anisotrope diagonal
                nu_ef(i, n) += nu_.addr()[dimension * (N * e + n) + j] * std::pow(xv(f, j) - xp(e, j), 2);
          else if (N_nu == N * dimension * dimension) for (n = 0; n < N; n++) for (j = 0, nu_ef(i, n) = 0; j < dimension; j++) for (k = 0; k < dimension; k++)
                  nu_ef(i, n) += nu_.addr()[dimension * (dimension * (N * e + n) + j) + k] * (xv(f, j) - xp(e, j)) * (xv(f, k) - xp(e, k)); //anisotrope complet
          else abort();
          for (n = 0, fac = nu_fac_.addr()[f] * (N_nu > N ? 1. / zone.dot(&xv(f, 0), &xv(f, 0), &xp(e, 0), &xp(e, 0)) : 1); n < N; n++) nu_ef(i, n) *= fac;
        }

      /* operateur : divergence pour les lignes aux elements, continuite pour les lignes aux faces */
      for (i = 0, mee = 0; i < n_f; i++, mee += mfe)
        {
          for (f = e_f(e, i), j = zone.w2i(zone.m2d(e) + i), mfe = 0; j < zone.w2i(zone.m2d(e) + i + 1); j++, mfe += mff)
            {
              for (fb = e_f(e, zone.w2j(j)), n = 0, fac = fs(f) * fs(fb) / ve(e) * zone.w2c(j); n < N; n++) mff(n) = fac * nu_ef(zone.w2j(j), n);
              for (n = 0; f < zone.nb_faces() && ch.icl(f, 0) < 6 && ch.icl(fb, 0) < 6 && n < N; n++) matrice(N * (ne_tot + f) + n, N * (ne_tot + fb) + n) += mff(n);
              for (n = 0; e < zone.nb_elem() && ch.icl(fb, 0) < 6 && n < N; n++) matrice(N * e + n, N * (ne_tot + fb) + n) -= mff(n);
            }
          for (n = 0; f < zone.nb_faces() && ch.icl(f, 0) < 6 && n < N; n++) matrice(N * (ne_tot + f) + n, N * e + n) -= mfe(n);

          //Echange_impose_base
          if (ch.icl(f, 0) > 0 && ch.icl(f, 0) < 2 && f < zone.nb_faces()) for (n = 0; n < N; n++)
              matrice(N * (ne_tot + f) + n, N * (ch.icl(f, 0) == 1 ? ne_tot + f : e) + n) += fs(f) * ref_cast(Echange_impose_base, cls[ch.icl(f, 1)].valeur()).h_imp(ch.icl(f, 2), n);
          else if (ch.icl(f, 0) == 3 && f < zone.nb_faces()) for (n = 0; n < N; n++) //paroi_contact gere en monolithique
              matrice(N * (ne_tot + f) + n, N * (ne_tot + f) + n) += ref_cast(Echange_contact_PolyMAC, cls[ch.icl(f, 1)].valeur()).remote_coeff(ch.icl(f, 2), 0);
        }
      for (n = 0; e < zone.nb_elem() && n < N; n++) matrice(N * e + n, N * e + n) += mee(n);
    }
  i++;
}

void Op_Diff_PolyMAC_Elem::modifier_pour_Cl(Matrice_Morse& la_matrice, DoubleTab& secmem) const
{
  //en principe, rien a faire!
  return;
}

//Description:
//on ajoute la contribution du second membre.
void Op_Diff_PolyMAC_Elem::contribuer_au_second_membre(DoubleTab& resu) const
{
  abort();
}
