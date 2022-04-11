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
// File:        Op_Diff_PolyMAC_V2_Elem.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////
#include <Op_Diff_PolyMAC_V2_Elem.h>
#include <Pb_Multiphase.h>
#include <Schema_Temps_base.h>
#include <Zone_PolyMAC_V2.h>
#include <Zone_Cl_PolyMAC.h>
#include <TRUSTLists.h>
#include <Dirichlet.h>
#include <Neumann_paroi.h>
#include <Echange_contact_PolyMAC_V2.h>
#include <Echange_externe_impose.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Champ_P0_PolyMAC_V2.h>
#include <Champ_front_calc.h>
#include <Modele_turbulence_scal_base.h>
#include <Synonyme_info.h>
#include <communications.h>
#include <MD_Vector_base.h>
#include <cmath>
#include <functional>
#include <Flux_parietal_base.h>

Implemente_instanciable_sans_constructeur( Op_Diff_PolyMAC_V2_Elem          , "Op_Diff_PolyMAC_V2_Elem|Op_Diff_PolyMAC_V2_var_Elem"                                , Op_Diff_PolyMAC_V2_base ) ;
Implemente_instanciable( Op_Dift_PolyMAC_V2_Elem          , "Op_Dift_PolyMAC_V2_P0_PolyMAC_V2|Op_Dift_PolyMAC_V2_var_P0_PolyMAC_V2"                    , Op_Diff_PolyMAC_V2_Elem ) ;
Implemente_ref(Op_Diff_PolyMAC_V2_Elem);

Op_Diff_PolyMAC_V2_Elem::Op_Diff_PolyMAC_V2_Elem()
{
  declare_support_masse_volumique(1);
}

Sortie& Op_Diff_PolyMAC_V2_Elem::printOn( Sortie& os ) const
{
  return Op_Diff_PolyMAC_V2_base::printOn( os );
}

Sortie& Op_Dift_PolyMAC_V2_Elem::printOn( Sortie& os ) const
{
  return Op_Diff_PolyMAC_V2_base::printOn( os );
}

Entree& Op_Diff_PolyMAC_V2_Elem::readOn( Entree& is )
{
  return Op_Diff_PolyMAC_V2_base::readOn( is );
}

Entree& Op_Dift_PolyMAC_V2_Elem::readOn( Entree& is )
{
  return Op_Diff_PolyMAC_V2_base::readOn( is );
}

void Op_Diff_PolyMAC_V2_Elem::completer()
{
  Op_Diff_PolyMAC_V2_base::completer();
  const Equation_base& eq = equation();
  const Champ_P0_PolyMAC_V2& ch = ref_cast(Champ_P0_PolyMAC_V2, eq.inconnue().valeur());
  const Zone_PolyMAC_V2& zone = la_zone_poly_.valeur();
  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 1)
    Cerr << "Op_Diff_PolyMAC_V2_Elem : largeur de joint insuffisante (minimum 1)!" << finl, Process::exit();
  flux_bords_.resize(zone.premiere_face_int(), ch.valeurs().line_size());

  /* tableau q_pi */
  if (sub_type(Energie_Multiphase, eq))
    q_pi_.resize(0, ch.valeurs().line_size(), ch.valeurs().line_size()), zone.zone().creer_tableau_elements(q_pi_);
}

/* construction de s_dist : sommets du porbleme coincidant avec des sommets de problemes distants */
void Op_Diff_PolyMAC_V2_Elem::init_s_dist() const
{
  if (s_dist_init_) return; //deja fait
  const Conds_lim& cls = la_zcl_poly_->les_conditions_limites();
  for (int i = 0; i < cls.size(); i++) if (sub_type(Echange_contact_PolyMAC_V2, cls[i].valeur()))
      {
        const Echange_contact_PolyMAC_V2& cl = ref_cast(Echange_contact_PolyMAC_V2, cls[i].valeur());
        const Op_Diff_PolyMAC_V2_Elem *o_diff = &cl.o_diff.valeur();
        cl.init_fs_dist();
        for (auto &s_sb : cl.s_dist) s_dist[s_sb.first][o_diff] = s_sb.second;
      }
  s_dist_init_ = 1;
}

void Op_Diff_PolyMAC_V2_Elem::init_som_ext() const
{
  if (som_ext_init_) return; //deja fait
  const Zone_PolyMAC_V2& zone = la_zone_poly_.valeur();
  int i, j, k, s, sb, s_l, iop, e, f, ok;

  /* remplissage de op_ext : de proche en proche */
  op_ext = { this };
  init_s_dist();
  auto s_dist_full = s_dist;
  for (std::set<const Op_Diff_PolyMAC_V2_Elem *> op_ext_tbd = { this }; op_ext_tbd.size();)
    {
      const Op_Diff_PolyMAC_V2_Elem *op = *op_ext_tbd.begin();
      op_ext_tbd.erase(op_ext_tbd.begin());
      //elargissement de op_ext
      const Conds_lim& cls = op->equation().zone_Cl_dis()->les_conditions_limites();
      for (i = 0; i < cls.size(); i++) if (sub_type(Echange_contact_PolyMAC_V2, cls[i].valeur()))
          {
            const Echange_contact_PolyMAC_V2& cl = ref_cast(Echange_contact_PolyMAC_V2, cls[i].valeur());
            const Op_Diff_PolyMAC_V2_Elem *o_diff = &cl.o_diff.valeur();
            if (std::find(op_ext.begin(), op_ext.end(), o_diff) == op_ext.end())
              op_ext.push_back(o_diff), op_ext_tbd.insert(o_diff);
          }
      //elargissement de s_dist_full : aargh...
      op->init_s_dist();
      if (op != this) for (auto && s_op_sb : s_dist_full) if (s_op_sb.second.count(op)) for (auto && op_sc : op->s_dist[s_op_sb.second[op]])
              if (!s_op_sb.second.count(op_sc.first)) s_op_sb.second[op_sc.first] = op_sc.second;
    }

  /* construction de som_ext_{d, e, f} */
  som_ext.set_smart_resize(1), som_ext_d.resize(0, 2), som_ext_d.set_smart_resize(1), som_ext_d.append_line(0, 0);
  som_ext_pe.resize(0, 2), som_ext_pf.resize(0, 4), som_ext_pe.set_smart_resize(1), som_ext_pf.set_smart_resize(1);
  std::set<std::array<int, 2>> s_pe; // (pb, el)
  std::set<std::array<int, 4>> s_pf; // (pb1, f1, pb2, f2)
  std::vector<std::reference_wrapper<const Zone_PolyMAC_V2>> zones;
  std::vector<std::reference_wrapper<const IntTab>> fcl, e_f, f_s;
  for (auto &&op : op_ext) zones.push_back(std::ref(ref_cast(Zone_PolyMAC_V2, op->equation().zone_dis().valeur())));
  for (auto &&op : op_ext) fcl.push_back(std::ref(ref_cast(Champ_P0_PolyMAC_V2, op->equation().inconnue().valeur()).fcl()));
  for (auto &&zo : zones) zo.get().init_som_elem(), e_f.push_back(std::ref(zo.get().elem_faces())), f_s.push_back(std::ref(zo.get().face_sommets()));

  /* autres CLs (hors Echange_contact) devant etre traitees par som_ext : Echange_impose_base, tout si Pb_Multiphase avec Flux_parietal_base */
  const Conds_lim& cls = equation().zone_Cl_dis()->les_conditions_limites();
  int has_flux = sub_type(Energie_Multiphase, equation()) && ref_cast(Pb_Multiphase, equation().probleme()).has_correlation("flux_parietal");
  for (i = 0; i < cls.size(); i++) if (has_flux || sub_type(Echange_impose_base, cls[i].valeur()))
      for (j = 0; j < ref_cast(Front_VF, cls[i]->frontiere_dis()).nb_faces(); j++)
        for (f = ref_cast(Front_VF, cls[i]->frontiere_dis()).num_face(j), k = 0; k < f_s[0].get().dimension(1) && (s = f_s[0](f, k)) >= 0; k++)
          if (!s_dist_full.count(s)) s_dist_full[s] = { }; //dans le std::map, mais pas d'operateurs distants!

  for (auto &&s_op_sb : s_dist_full) if ((s = s_op_sb.first) < zone.nb_som())
      {
        //elements
        for (i = 0; i < zone.som_elem.get_list_size(s); i++)
          {
            std::array<int, 2> arr = { 0, zone.som_elem(s, i) };
            s_pe.insert({arr}); //cote local
          }
        for (auto &&op_sb : s_op_sb.second) //cotes distants
          {
            const Zone_PolyMAC_V2& o_zone = ref_cast(Zone_PolyMAC_V2, op_sb.first->equation().zone_dis().valeur());
            for (iop = std::find(op_ext.begin(), op_ext.end(), op_sb.first) - op_ext.begin(), i = 0; i < o_zone.som_elem.get_list_size(op_sb.second); i++)
              {
                std::array<int, 2> arr = { iop, o_zone.som_elem(op_sb.second, i) };
                s_pe.insert({arr});
              }
          }
        //faces : celles des elements de s_pe
        for (auto && iop_e : s_pe) for (iop = iop_e[0], e = iop_e[1], s_l = iop ? s_op_sb.second.at(op_ext[iop]) : s, i = 0; i < e_f[iop].get().dimension(1) && (f = e_f[iop](e, i)) >= 0; i++)
            {
              for (ok = 0, j = 0; !ok && j < f_s[iop].get().dimension(1) && (sb = f_s[iop](f, j)) >= 0; j++) ok |= sb == s_l;
              if (!ok || fcl[iop](f, 0) != 3) continue; //face ne touchant pas le sommet ou non Echange_contact
              const Echange_contact_PolyMAC_V2& cl = ref_cast(Echange_contact_PolyMAC_V2, op_ext[iop]->equation().zone_Cl_dis()->les_conditions_limites()[fcl[iop](f, 1)].valeur());
              int o_iop = std::find(op_ext.begin(), op_ext.end(), &cl.o_diff.valeur()) - op_ext.begin(), o_f = cl.f_dist(fcl[iop](f, 2)); //operateur / face de l'autre cote
              std::array<int, 4> arr = { iop < o_iop ? iop : o_iop, iop < o_iop ? f : o_f, iop < o_iop ? o_iop : iop, iop < o_iop ? o_f : f};
              s_pf.insert({arr}); //stocke dans l'ordre
            }
        int mix = 0; //melange-t-on les composantes autour de ce sommet? oui si une equation Energie_Multiphase avec correlation de flux parietal est presente autour
        for (auto &&pe : s_pe) som_ext_pe.append_line(pe[0], pe[1]), mix |= sub_type(Energie_Multiphase, op_ext[pe[0]]->equation()) && ref_cast(Pb_Multiphase, op_ext[pe[0]]->equation().probleme()).has_correlation("flux_parietal");
        for (auto &&pf : s_pf) som_ext_pf.append_line(pf[0], pf[1], pf[2], pf[3]);
        som_ext.append_line(s), som_mix.append_line(mix), som_ext_d.append_line(som_ext_pe.dimension(0), som_ext_pf.dimension(0)), s_pe.clear(), s_pf.clear();
      }
  som_ext_init_ = 1;
}

double Op_Diff_PolyMAC_V2_Elem::calculer_dt_stab() const
{
  const Zone_PolyMAC_V2& zone = la_zone_poly_.valeur();
  const IntTab& e_f = zone.elem_faces();
  const DoubleTab& nf = zone.face_normales(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe() : (has_champ_masse_volumique() ? &get_champ_masse_volumique().valeurs() : NULL),
                    &diffu = diffusivite_pour_pas_de_temps().valeurs(), &lambda = diffusivite().valeurs();
  const DoubleVect& pe = zone.porosite_elem(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes();
  update_nu();

  int i, e, f, n, N = equation().inconnue().valeurs().dimension(1), cD = diffu.dimension(0) == 1, cL = lambda.dimension(0) == 1;
  double dt = 1e10;
  DoubleTrav flux(N);
  for (e = 0; e < zone.nb_elem(); e++)
    {
      for (flux = 0, i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) for (n = 0; n < N; n++)
          flux(n) += zone.nu_dot(&nu_, e, n, &nf(f, 0), &nf(f, 0)) / vf(f);
      for (n = 0; n < N; n++) if ((!alp || (*alp)(e, n) > 1e-3) && flux(n)) /* sous 0.5e-6, on suppose que l'evanescence fait le job */
          dt = std::min(dt, pe(e) * ve(e) * (alp ? (*alp)(e, n) : 1) * (lambda(!cL * e, n) / diffu(!cD * e, n)) / flux(n));
      if (dt < 0) abort();
    }
  return Process::mp_min(dt);
}

void Op_Diff_PolyMAC_V2_Elem::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  init_som_ext(), update_phif(!nu_constant_); //calcul de (nf.nu.grad T) : si nu variable, stencil complet
  const std::string nom_inco = equation().inconnue().le_nom().getString();
  if (semi_impl.count(nom_inco)) return; //semi-implicite -> rien a dimensionner
  const Zone_PolyMAC_V2& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins();
  int i, j, k, e, e_s, p_s, f, m, n, n_sten = 0;
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

  Cerr << "Op_Diff_PolyMAC_V2_Elem::dimensionner() : ";
  //avec fgrad : parties hors Echange_contact (ne melange ni les problemes, ni les composantes)
  for (f = 0; f < zone.nb_faces(); f++) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) //stencil a l'element e
        for (j = phif_d(f); j < phif_d(f + 1); j++) if ((e_s = phif_e(j)) < zone.nb_elem_tot()) for (n = 0; n < N[0]; n++)
              stencil[0].append_line(N[0] * e + n, N[0] * e_s + n), tpfa(f, n) &= e_s == f_e(f, 0) || e_s == f_e(f, 1) || phif_c(j, n) == 0;

  //avec som_ext : partie Echange_contact -> melange toutes les composantes si som_mix = 1
  for (i = 0; i < som_ext.dimension(0); i++) for (j = som_ext_d(i, 0); j < som_ext_d(i + 1, 0); j++) if (!som_ext_pe(j, 0) && (e = som_ext_pe(j, 1)) < zone.nb_elem())
        for (k = som_ext_d(i, 0); k < som_ext_d(i + 1, 0); k++) for (p_s = som_ext_pe(k, 0), e_s = som_ext_pe(k, 1), n = 0; n < N[0]; n++)
            for (m = (som_mix(i) ? 0 : n); m < (som_mix(i) ? N[p_s] : n + 1); m++) stencil[p_s].append_line(N[0] * e + n, N[p_s] * e_s + m);

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

void Op_Diff_PolyMAC_V2_Elem::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  init_som_ext(), update_phif();
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  int i, i_eq, i_s, il, j, k, k1, k2, kb, l, e, eb, f, fb, s, sb, sp, m, n, M, n_ext = op_ext.size(), p, pb, n_e, n_ef, nc, nl, n_m, d, db, D = dimension, sgn, sgn_l, nw, un = 1, rk, infoo, it, cv, nonlinear;
  std::vector<Matrice_Morse *> mat(n_ext); //matrices
  std::vector<int> N; //composantes
  std::vector<std::reference_wrapper<const Zone_PolyMAC_V2>> zone; //zones
  std::vector<std::reference_wrapper<const Conds_lim>> cls; //conditions aux limites
  std::vector<std::reference_wrapper<const IntTab>> fcl, e_f, f_e, f_s; //tableaux "fcl", "elem_faces", "faces_voisins"
  std::vector<std::reference_wrapper<const DoubleVect>> fs; //surfaces
  std::vector<std::reference_wrapper<const DoubleTab>> inco, nf, xp, xs, xv, diffu; //inconnues, normales aux faces, positions elems / faces / sommets
  for (i = 0, M = 0; i < n_ext; M = std::max(M, N[i]), i++)
    {
      std::string nom_mat = i ? nom_inco + "_" + op_ext[i]->equation().probleme().le_nom().getString() : nom_inco;
      mat[i] = !semi_impl.count(nom_inco) && matrices.count(nom_mat) ? matrices.at(nom_mat) : NULL;
      zone.push_back(std::ref(ref_cast(Zone_PolyMAC_V2, op_ext[i]->equation().zone_dis().valeur())));
      f_e.push_back(std::ref(zone[i].get().face_voisins())), e_f.push_back(std::ref(zone[i].get().elem_faces())), f_s.push_back(std::ref(zone[i].get().face_sommets()));
      fs.push_back(std::ref(zone[i].get().face_surfaces())), nf.push_back(std::ref(zone[i].get().face_normales()));
      xp.push_back(std::ref(zone[i].get().xp())), xv.push_back(std::ref(zone[i].get().xv())), xs.push_back(std::ref(zone[i].get().zone().domaine().coord_sommets()));
      cls.push_back(std::ref(op_ext[i]->equation().zone_Cl_dis().les_conditions_limites()));
      diffu.push_back(ref_cast(Op_Diff_PolyMAC_V2_Elem, *op_ext[i]).nu());
      const Champ_Inc& ch_inc = op_ext[i]->has_champ_inco() ? op_ext[i]->mon_inconnue() : op_ext[i]->equation().inconnue();
      const Champ_P0_PolyMAC_V2& ch = ref_cast(Champ_P0_PolyMAC_V2, ch_inc.valeur());
      inco.push_back(std::ref(semi_impl.count(nom_mat) ? semi_impl.at(nom_mat) : ch.valeurs()));
      N.push_back(inco[i].get().line_size()), fcl.push_back(std::ref(ch.fcl()));
    }
  const Zone_PolyMAC_V2& zone0 = zone[0];
  q_pi_ = 0; //remise a zero du flux paroi-interface

  /* avec phif : flux hors Echange_contact -> mat[0] seulement */
  DoubleTrav flux(N[0]);
  for (f = 0; f < zone0.nb_faces(); f++)
    {
      for (flux = 0, i = phif_d(f); i < phif_d(f + 1); i++)
        if ((fb = (eb = phif_e(i)) - zone0.nb_elem_tot()) < 0) //element
          {
            for (n = 0; n < N[0]; n++) flux(n) += phif_c(i, n) * fs[0](f) * inco[0](eb, n);
            if (mat[0]) for (j = 0; j < 2 && (e = f_e[0](f, j)) >= 0; j++) if (e < zone[0].get().nb_elem()) for (n = 0; n < N[0]; n++) //derivees
                    (*mat[0])(N[0] * e + n, N[0] * eb + n) += (j ? 1 : -1) * phif_c(i, n) * fs[0](f);
          }
        else if (fcl[0](fb, 0) == 1 || fcl[0](fb, 0) == 2) for (n = 0; n < N[0]; n++) //Echange_impose_base
            flux(n) += (phif_c(i, n) ? phif_c(i, n) * fs[0](f) * ref_cast(Echange_impose_base, cls[0].get()[fcl[0](fb, 1)].valeur()).T_ext(fcl[0](fb, 2), n) : 0);
        else if (fcl[0](fb, 0) == 4) for (n = 0; n < N[0]; n++) //Neumann non homogene
            flux(n) += (phif_c(i, n) ? phif_c(i, n) * fs[0](f) * ref_cast(Neumann_paroi, cls[0].get()[fcl[0](fb, 1)].valeur()).flux_impose(fcl[0](fb, 2), n) : 0);
        else if (fcl[0](fb, 0) == 6) for (n = 0; n < N[0]; n++) //Dirichlet
            flux(n) += (phif_c(i, n) ? phif_c(i, n) * fs[0](f) * ref_cast(Dirichlet, cls[0].get()[fcl[0](fb, 1)].valeur()).val_imp(fcl[0](fb, 2), n) : 0);

      for (j = 0; j < 2 && (e = f_e[0](f, j)) >= 0; j++) if (e < zone[0].get().nb_elem()) for (n = 0; n < N[0]; n++) //second membre -> amont/aval
            secmem(e, n) += (j ? -1 : 1) * flux(n);
      if (f < zone0.premiere_face_int()) for (n = 0; n < N[0]; n++) flux_bords_(f, n) = flux(n); //flux aux bords
    }

  /* avec som_ext : flux autour des sommets affectes par des Echange_contact */
  double vol_s, i3[3][3] = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }}, eps = 1e-8, eps_g = 1e-6, fac[3];
  std::vector<std::array<int, 2>> s_pe, s_pf; //listes { probleme, elements/bord}, { probleme, face } autour du sommet
  std::map<std::array<int, 2>, std::array<int, 2>> m_pf; //connections Echange_contact : m_pf[{pb1, f1}] = { pb2, f2 }
  std::vector<double> surf_fs, vol_es; //surfaces partielles des faces connectees au sommet (meme ordre que s_f)
  std::vector<std::array<std::array<double, 3>,2>> vec_fs;//pour chaque facette, base de (D-1) vecteurs permettant de la parcourir
  std::vector<std::vector<int>> se_f; /* se_f[i][.] : faces connectees au i-eme element connecte au sommet s */
  std::vector<int> type_f; //type_f[i] : type de la face i (numerotation du tableau fcl)
  IntTrav i_efs, i_e, i_eq_flux, i_eq_cont, i_eq_pbm, piv(1); //indices dans les matrices : i_efs(i, j, n) -> composante n de la face j de l'elem i dans s_pe, i_e(i, n) -> indice de la phase n de l'elem i de s_pe
  //i_eq_{flux,cont}(i, n) -> n-ieme equation de flux/de continuite a la face i de s_pf
  //i_eq_pbm(i_efs(i, j, n)) -> n-ieme equation "flux = correlation" a la face j de l'elem i de s_pe (seulement si Pb_Multiphase)
  DoubleTrav A, B, mA, mB, Ff, Fec, Qf, Qec, Tefs, C, X, Y, W(1), S, x_fs;
  i_efs.set_smart_resize(1), i_e.set_smart_resize(1), i_eq_flux.set_smart_resize(1), i_eq_cont.set_smart_resize(1), i_eq_pbm.set_smart_resize(1);
  A.set_smart_resize(1), B.set_smart_resize(1), Ff.set_smart_resize(1), Fec.set_smart_resize(1), C.set_smart_resize(1), X.set_smart_resize(1);
  Y.set_smart_resize(1), piv.set_smart_resize(1), W.set_smart_resize(1), x_fs.set_smart_resize(1), Tefs.set_smart_resize(1), S.set_smart_resize(1);
  mA.set_smart_resize(1), mB.set_smart_resize(1), Qf.set_smart_resize(1), Qec.set_smart_resize(1);

  for (i_s = 0; i_s < som_ext.dimension(0); i_s++) if ((s = som_ext(i_s)) < zone0.nb_som())
      {
        /* (pb, elem) connectes a s -> avec som_ext_pe (deja classes) */
        for (s_pe.clear(), n_e = 0, i = som_ext_d(i_s, 0); i < som_ext_d(i_s + 1, 0); i++, n_e++) s_pe.push_back({{ som_ext_pe(i, 0), som_ext_pe(i, 1)}});
        /* m_pf : correspondances par parois contact */
        for (m_pf.clear(), i = som_ext_d(i_s, 1); i < som_ext_d(i_s + 1, 1); i++) for (j = 0; j < 2; j++)
            m_pf[ {{ som_ext_pf(i, j ? 2 : 0), som_ext_pf(i, j ? 3 : 1) }}] = {{ som_ext_pf(i, j ? 0 : 2), som_ext_pf(i, j ? 1 : 3) }};

        /* faces, leurs surfaces partielles */
        for (s_pf.clear(), surf_fs.clear(), vec_fs.clear(), se_f.resize(std::max(int(se_f.size()), n_e)), i = 0; i < n_e; i++)
          for (se_f[i].clear(), p = s_pe[i][0], e = s_pe[i][1], sp = p ? s_dist[s].at(op_ext[p]) : s, j = 0; j < e_f[p].get().dimension(1) && (f = e_f[p](e, j)) >= 0; j++)
            {
              for (k = 0, sb = 0; k < f_s[p].get().dimension(1) && (sb = f_s[p](f, k)) >= 0; k++) if (sb == sp) break;
              if (sb != sp) continue; /* face de e non connectee a s -> on saute */
              se_f[i].push_back(f); //faces connectees a (p, e)
              //couple (p, f) de la face : si la face est un Echange_contact, alors on choisit le couple du pb d'indice le plus bas
              std::array<int, 2> pf0 = {{ p, f }}, pf = m_pf.count(pf0) && m_pf[pf0] < pf0 ? m_pf[pf0] : pf0;
              if ((l = std::lower_bound(s_pf.begin(), s_pf.end(), pf) - s_pf.begin()) == (int) s_pf.size() || s_pf[l] != pf) /* si (p, f) n'est pas dans s_pf, on l'ajoute */
                {
                  s_pf.insert(s_pf.begin() + l, pf); //(pb, face) -> dans s_pf
                  if (D < 3) surf_fs.insert(surf_fs.begin() + l, fs[p](f) / 2), vec_fs.insert(vec_fs.begin() + l, {{{ xs[p](sp, 0) - xv[p](f, 0), xs[p](sp, 1) - xv[p](f, 1), 0 }, { 0, 0, 0 }}}); //2D -> facile
                  else for (surf_fs.insert(surf_fs.begin() + l, 0), vec_fs.insert(vec_fs.begin() + l, {{{ 0, 0, 0}, {0, 0, 0 }}}), m = 0; m < 2; m++) //3D -> deux sous-triangles
                  {
                    if (m == 1 || k > 0) sb = f_s[p](f, m ? (k + 1 < f_s[p].get().dimension(1) && f_s[p](f, k + 1) >= 0 ? k + 1 : 0) : k - 1); //sommet suivant (m = 1) ou precedent avec k > 0 -> facile
                    else for (n = f_s[p].get().dimension(1) - 1; (sb = f_s[p](f, n)) == -1; ) n--; //sommet precedent avec k = 0 -> on cherche a partir de la fin
                    auto v = zone0.cross(D, D, &xs[p](sp, 0), &xs[p](sb, 0), &xv[p](f, 0), &xv[p](f, 0));//produit vectoriel (xs - xf)x(xsb - xf)
                    surf_fs[l] += std::fabs(zone0.dot(&v[0], &nf[p](f, 0))) / fs[p](f) / 4; //surface a ajouter
                    for (d = 0; d < D; d++) vec_fs[l][m][d] = (xs[p](sp, d) + xs[p](sb, d)) / 2 - xv[p](f, d); //vecteur face -> arete
                  }
                }
            }
        int n_f = s_pf.size(); //nombre de faces

        /* conversion de se_f en indices dans s_f */
        for (i = 0; i < n_e; i++) for (p = s_pe[i][0], j = 0; j < (int) se_f[i].size(); j++)
            {
              std::array<int, 2> pf0 = { p, se_f[i][j] }, pf = m_pf.count(pf0) && m_pf[pf0] < pf0 ? m_pf[pf0] : pf0;
              se_f[i][j] = std::lower_bound(s_pf.begin(), s_pf.end(), pf) - s_pf.begin();
            }

        /* volumes */
        for (vol_es.resize(n_e), vol_s = 0, i = 0; i < n_e; vol_s += vol_es[i], i++) for (p = s_pe[i][0], e = s_pe[i][1], vol_es[i] = 0, j = 0; j < (int) se_f[i].size(); j++)
            k = se_f[i][j], pb = s_pf[k][0], f = s_pf[k][1], vol_es[i] += surf_fs[k] * std::fabs(zone0.dot(&xp[p](e, 0), &nf[pb](f, 0), &xv[pb](f, 0))) / fs[pb](f) / D;

        /* inconnues en paroi (i_efs), aux elements (i_e). On alloues toutes les composantes si som_mix = 1, une seule sinon */
        int mix = som_mix(i_s), Nm = mix ? 1 : N[s_pe[0][0]], t_eq, t_e, t_ec; //nombre total d'equations/variables aux faces, nombre total de variables aux elements, t_ec = t_e + 1, nombres divises par Nl
        for (n_ef = 0, i = 0; i < n_e; i++) n_ef = std::max(n_ef, int(se_f[i].size()));//nombre max de faces par elem
        for (i_efs.resize(n_e, n_ef, 1 + M * mix), i_efs = -1, i = 0, t_eq = 0; i < n_e; i++) for (p = s_pe[i][0], e = s_pe[i][1], j = 0; j < (int) se_f[i].size(); j++)
            {
              for (k = se_f[i][j], n = 0; n < (mix ? N[p] : 1); n++, t_eq++) i_efs(i, j, n) = t_eq; //une temperature de paroi par phase
              //si face de bord d'un Pb_Multiphase (hors frontiere ouverte), une inconnue supplementaire : la Tparoi (dans ce cas, mix = 1)
              f = s_pf[k][0] == p && (e == f_e[p](s_pf[k][1], 0) || e == f_e[p](s_pf[k][1], 1)) ? s_pf[k][1] : m_pf.at(s_pf[k])[1]; //numero de face cote e
              if (sub_type(Energie_Multiphase, op_ext[p]->equation()) && ref_cast(Pb_Multiphase, op_ext[p]->equation().probleme()).has_correlation("flux_parietal") && fcl[p](f, 0) && fcl[p](f, 0) != 5)
                i_efs(i, j, M) = t_eq++;
            }
        for (i_e.resize(n_e, mix ? M : 1), i_e = -1, i = 0, t_e = 0, t_ec = 1; i < n_e; i++) for (p = s_pe[i][0], n = 0; n < (mix ? N[p] : 1); n++, t_e++, t_ec++) i_e(i, n) = t_e;

        /* equations */
        for (type_f.resize(n_f), i_eq_flux.resize(n_f, mix ? M : 1), i_eq_flux = -1, i_eq_cont.resize(n_f, mix ? M : 1), i_eq_cont = -1, i = 0, k = 0; i < n_f; i++)
          {
            int p1 = s_pf[i][0], p2 = m_pf.count(s_pf[i]) ? m_pf[s_pf[i]][0] : p1, p12[2] = { p1, p2}, n12[2] = { N[p1], N[p2] }; //nombres de composantes de chaque cote
            f = s_pf[i][1], type_f[i] = fcl[p1](f, 0); //type de face
            if (type_f[i] && type_f[i] != 5) for (j = 0; j < 2; j++) if (sub_type(Energie_Multiphase, op_ext[p12[j]]->equation()) //si flux parietal et CL non Neumann, il n'y a qu'une valeur en paroi
                                                                             && ref_cast(Pb_Multiphase, op_ext[p12[j]]->equation().probleme()).has_correlation("flux_parietal")) n12[j] = 1;
            if (n12[0] != n12[1]) Cerr << "Op_Diff_PolyMAC_V2_Elem : incompatibility between " << op_ext[p1]->equation().probleme().le_nom()
                                         << " and " << op_ext[p2]->equation().probleme().le_nom() << " ( " << n12[0] << " vs " << n12[1] << " components)!" << finl, Process::exit();
            //equations de flux : tous types sauf Dirichlet et Echange_impose_base
            if (type_f[i] != 6 && type_f[i] != 7 && type_f[i] != 1 && type_f[i] != 2) for (n = 0; n < (mix ? n12[0] : 1); n++) i_eq_flux(i, n) = k, k++;
            //equations de continuite : tous types sauf Neumann
            if (type_f[i] != 4 && type_f[i] != 5) for (n = 0; n < (mix ? n12[0] : 1); n++) i_eq_cont(i, n) = k, k++;
          }
        //si inconnues de paroi de Pb_Multiphase : equations dues aux correlations de flux (dans ce cas mix = 1)
        if (mix) for (i_eq_pbm.resize(t_eq), i_eq_pbm = -1, i = 0; i < n_e; i++) for (p = s_pe[i][0], j = 0; j < (int) se_f[i].size(); j++)
              if (i_efs(i, j, M) >= 0) for (n = 0; n < N[p]; n++) i_eq_pbm(i_efs(i, j, n)) = k, k++;
        assert(k == t_eq); //a-ton bien autant d'equations que d'inconnues?

        for (int essai = 0; essai < 3; essai++) /* essai 0 : MPFA O -> essai 1 : MPFA O avec x_fs mobiles -> essai 2 : MPFA symetrique (corecive, mais pas tres consistante) */
          {
            if (essai == 1) /* essai 1 : tentative de symmetrisation en deplacant les x_fs. Si mix = 1, on ne peut pas les deplacer independamment */
              {
                /* systeme lineaire */
                for (C.resize(Nm, nc = (D - 1) * n_f, nl = D * (D - 1) / 2 * t_e), Y.resize(Nm, n_m = std::max(nc, nl)), C = 0, Y = 0, i = 0, il = 0; i < n_e; i++)
                  for (p = s_pe[i][0], e = s_pe[i][1], d = 0; d < D; d++) for (db = 0; db < d; db++, il += !mix) for (n = 0; n < N[p]; n++, il += mix) for (j = 0; j < (int) se_f[i].size(); j++)
                          {
                            k = se_f[i][j], f = s_pf[k][0] == p ? s_pf[k][1] : m_pf[s_pf[k]][1], sgn = e == f_e[p](f, 0) ? 1 : -1; //indice de face, num dans le probleme courant, amont/aval
                            for (l = 0; l < D; l++) fac[l] = sgn * zone0.nu_dot(&diffu[p].get(), e, n, &nf[p](f, 0), i3[l]) * surf_fs[k] / fs[p](f) / vol_es[i]; //vecteur lambda_e nf sortant * facteur commun
                            Y(!mix * n, il) += fac[d] * (xv[p](f, db) - xp[p](e, db)) - fac[db] * (xv[p](f, d) - xp[p](e, d)); //second membre
                            for (l = 0; l < D - 1; l++) C(!mix * n, (D - 1) * k + l, il) += fac[db] * vec_fs[k][l][d] - fac[d] * vec_fs[k][l][db]; //matrice
                          }
                /* resolution -> DEGLSY */
                nw = -1, F77NAME(dgelsy)(&nl, &nc, &un, &C(0, 0, 0), &nl, &Y(0, 0), &n_m, &piv(0), &eps_g, &rk, &W(0), &nw, &infoo);
                for (W.resize(nw = W(0)), piv.resize(nc), n = 0; n < Nm; n++)
                  piv = 0, F77NAME(dgelsy)(&nl, &nc, &un, &C(n, 0, 0), &nl, &Y(n, 0), &n_m, &piv(0), &eps_g, &rk, &W(0), &nw, &infoo);
                /* x_fs = xf + corrections */
                for (x_fs.resize(Nm, n_f, D), n = 0; n < Nm; n++) for (i = 0; i < n_f; i++) for (p = s_pf[i][0], f = s_pf[i][1], d = 0; d < D; d++)
                      for (x_fs(n, i, d) = xv[p](f, d), k = 0; k < D - 1; k++) x_fs(n, i, d) += std::min(std::max(Y(n, (D - 1) * i + k), 0.), 0.5) * vec_fs[i][k][d];
              }

            A.resize(Nm, t_eq, t_eq), B.resize(Nm, t_ec = t_e + 1, t_eq), Ff.resize(Nm, t_eq, t_eq), Fec.resize(Nm, t_eq, t_ec); //systeme A.dT_efs = B.{dT_eb, 1}, flux sortant a chaque face
            if (mix) Qf.resize(n_e, t_eq, M, M), Qec.resize(n_e, t_ec, M, M); //si Pb_Multiphase : flux paroi-interface
            /* debut du Newton sur les T_efs : initialisation aux temperatures de mailles */
            for (Tefs.resize(Nm, t_eq), i = 0; i < n_e; i++) for (p = s_pe[i][0], e = s_pe[i][1], j = 0; j < (int) se_f[i].size(); j++)
                {
                  for (n = 0; n < N[p]; n++) Tefs(!mix * n, i_efs(i, j, mix * n)) = inco[p](e, n); //Tefs de chaque phase
                  if (mix && i_efs(i, j, M) >= 0) Tefs(0, i_efs(i, j, M)) = inco[p](e, 0); //Tparoi : on prend la temperature de la phase 0 faute de mieux
                }
            for (it = 0, nonlinear = 0, cv = 0; !cv && it < 10; it++) //Newton sur les Tefs. Si mix = 0 (pas de Pb_Multi), une seule iteration suffit
              {
                for (A = 0, B = 0, Ff = 0, Fec = 0, Qf = 0, Qec = 0, i = 0; i < n_e; i++)
                  {
                    p = s_pe[i][0], e = s_pe[i][1];
                    /* gradient dans e */
                    if (essai < 2) /* essais 0 et 1 : gradient consistant */
                      {
                        /* gradient dans (e, s) -> matrice / second membre M.x = Y du systeme (grad u)_i = sum_f b_{fi} (x_fs_i - x_e), avec x_fs le pt de continuite de u_fs */
                        C.resize(Nm, n_ef = se_f[i].size(), D), Y.resize(Nm, D, n_m = std::max(D, n_ef)), X.resize(Nm, n_ef, D);
                        for (n = 0; n < Nm; n++) for (j = 0; j < n_ef; j++) for (k = se_f[i][j], pb = s_pf[k][0], f = s_pf[k][1], d = 0; d < D; d++) C(n, j, d) = (essai ? x_fs(n, k, d) : xv[pb](f, d)) - xp[p](e, d);
                        for (Y = 0, n = 0; n < Nm; n++) for (d = 0; d < D; d++) Y(n, d, d) = 1;
                        nw = -1, F77NAME(dgelsy)(&D, &n_ef, &D, &C(0, 0, 0), &D, &Y(0, 0, 0), &n_m, &piv(0), &eps_g, &rk, &W(0), &nw, &infoo);
                        for (W.resize(nw = W(0)), piv.resize(n_m), n = 0; n < Nm; n++)
                          piv = 0, F77NAME(dgelsy)(&D, &n_ef, &D, &C(n, 0, 0), &D, &Y(n, 0, 0), &n_m, &piv(0), &eps_g, &rk, &W(0), &nw, &infoo);
                        for (n = 0; n < Nm; n++) for (j = 0; j < n_ef; j++) for (d = 0; d < D; d++) X(n, j, d) = Y(n, d, j); /* pour pouvoir utiliser nu_dot */
                      }
                    else for (n = 0; n < Nm; n++) for (j = 0; j < n_ef; j++) for (k = se_f[i][j], pb = s_pf[k][0], f = s_pf[k][1], sgn = pb == p && e == f_e[p](f, 0) ? 1 : -1, d = 0; d < D; d++)
                            X(n, j, d) = surf_fs[k] / vol_es[i] * sgn * nf[pb](f, d) / fs[pb](f); /* essai 2 : gradient non consistant */

                    /* remplissage de A, B, Ff, Fec */
                    for (j = 0; j < n_ef; j++)
                      {
                        k = se_f[i][j], f = s_pf[k][0] == p && (e == f_e[p](s_pf[k][1], 0) || e == f_e[p](s_pf[k][1], 1)) ? s_pf[k][1] : m_pf[s_pf[k]][1]; //indice de face, numero de face local
                        sgn_l = e == f_e[p](f, 0) ? 1 : -1, sgn = p == s_pf[k][0] && f == s_pf[k][1] ? sgn_l : -1; //orientation de la face locale, de la face globale

                        /* flux : remplit Ff / Fec (format standard) dans tous les cas, et les equations de A/B donnees par i_eq_flux / i_eq_cont (format LAPACK) */
                        for (l = 0; l < n_ef; l++) for (kb = se_f[i][l], fb = s_pf[kb][0] == p ? s_pf[kb][1] : m_pf[s_pf[kb]][1], n = 0; n < N[p]; n++)
                            {
                              double x = sgn_l * zone0.nu_dot(&diffu[p].get(), e, n, &nf[p](f, 0), &X(!mix * n, l, 0)) / fs[p](f), y = x * surf_fs[k];
                              /* stockage du flux sortant dans Ff / Fec */
                              Fec(!mix * n, i_efs(i, j, mix * n), t_e) += y * (Tefs(!mix * n, i_efs(i, l, mix * n)) - inco[p](e, n));
                              Ff(!mix * n, i_efs(i, j, mix * n), i_efs(i, l, mix * n)) += y, Fec(!mix * n, i_efs(i, j, mix * n), i_e(i, mix * n)) -= y;
                              /* equations de flux : on contribue a celle de la phase (si elle existe), ou a defaut a celle de la phase 0 */
                              if ((i_eq = i_eq_flux(k, mix * n)) >= 0 || (i_eq = i_eq_flux(k, 0)) >= 0)
                                B(!mix * n, t_e, i_eq) += x * (Tefs(!mix * n, i_efs(i, l, mix * n)) - inco[p](e, n)), A(!mix * n, i_efs(i, l, mix * n), i_eq) -= x, B(!mix * n, i_e(i, mix * n), i_eq) -= x;
                              /* equations de continuite : on y contribue si on est l'amont d'un Echange_contact pour prendre en compte son coeff d'echange */
                              if (sgn > 0 && (type_f[k] && type_f[k] < 4) && ((i_eq = i_eq_cont(k, mix * n)) >= 0 || (i_eq = i_eq_cont(k, 0)) >= 0))
                                {
                                  double invh = type_f[k] == 3 ? ref_cast(Echange_contact_PolyMAC_V2, cls[p].get()[fcl[p](f, 1)].valeur()).invh_paroi
                                                : 1. / ref_cast(Echange_impose_base, cls[p].get()[fcl[p](f, 1)].valeur()).h_imp(fcl[p](f, 1), !mix || i_eq == i_eq_cont(k, n) ? n : 0);
                                  B(!mix * n, t_e, i_eq) += invh * x * (Tefs(!mix * n, i_efs(i, l, mix * n)) - inco[p](e, n));
                                  A(!mix * n, i_efs(i, l, mix * n), i_eq) -= invh * x, B(!mix * n, i_e(i, mix * n), i_eq) -= invh * x;
                                }
                              /* si Pb_Multiphase : partie "flux" de l'equation sur la correlation */
                              if (mix && (i_eq = i_eq_pbm(i_efs(i, j, mix * n))) >= 0)
                                {
                                  B(!mix * n, t_e, i_eq) += x * (Tefs(!mix * n, i_efs(i, l, mix * n)) - inco[p](e, n));
                                  A(!mix * n, i_efs(i, l, mix * n), i_eq) -= x, B(!mix * n, i_e(i, mix * n), i_eq) -= x;
                                }
                            }

                        /* autres equations : continuite, correlations */
                        if (mix && i_efs(i, j, M) >= 0) /* inconnue de Tparoi -> Pb_Multiphase */
                          {
                            //equation de continuite : avec la Tparoi
                            if ((i_eq = i_eq_cont(k, 0)) >= 0) B(0, t_e, i_eq) += sgn * Tefs(0, i_efs(i, j, M)), A(0, i_efs(i, j, M), i_eq) -= sgn;
                            //equations sur les correlations
                            const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, op_ext[p]->equation().probleme());
                            const Flux_parietal_base& corr = ref_cast(Flux_parietal_base, pbm.get_correlation("Flux_parietal").valeur());
                            const DoubleTab& alpha = pbm.eq_masse.inconnue().passe(), &dh = zone[p].get().diametre_hydraulique_elem(), &press = pbm.eq_qdm.pression().passe(),
                                             &vit = pbm.eq_qdm.inconnue().passe(), &lambda = pbm.milieu().conductivite().passe(), &mu = ref_cast(Fluide_base, pbm.milieu()).viscosite_dynamique().passe(),
                                              &rho = pbm.milieu().masse_volumique().passe(), &Cp = pbm.milieu().capacite_calorifique().passe();
                            DoubleTrav qpk(N[p]), dTf_qpk(N[p], N[p]), dTp_qpk(N[p]), qpi(N[p], N[p]), dTf_qpi(N[p], N[p], N[p]), dTp_qpi(N[p], N[p]), nv(N[p]);
                            for (d = 0; d < D; d++) for (n = 0; n < N[p]; n++) nv(n) += std::pow(vit(zone[p].get().nb_faces_tot() + D * e + d, n), 2);
                            for (n = 0; n < N[p]; n++) nv(n) = sqrt(nv(n));
                            //appel : on n'est implicite qu'en les temperatures
                            corr.qp(N[p], f, dh(e), dh(e), &alpha(e, 0), &Tefs(0, i_efs(i, j, 0)), press(e), nv.addr(), Tefs(0, i_efs(i, j, M)), &lambda(e, 0), &mu(e, 0), &rho(e, 0), &Cp(e, 0),
                                    qpk.addr(), NULL, NULL, NULL, dTf_qpk.addr(), dTp_qpk.addr(), qpi.addr(), NULL, NULL, NULL, dTf_qpi.addr(), dTp_qpi.addr(), nonlinear);
                            /* on ajoute qpi(k, l) a la qpk(k) (sera ensuite retire par Flux_interfacial_PolyMAC_V2) */
                            for (k1 = 0; k1 < N[p]; k1++) for (k2 = k1 + 1; k2 < N[p]; k2++)
                                for (qpk(k1) += qpi(k1, k2), dTp_qpk(k1) += dTp_qpi(k1, k2), n = 0; n < N[p]; n++) dTf_qpk(k1, n) += dTf_qpi(k1, k2, n);

                            for (n = 0; n < N[p]; n++) //partie constante, derivees en Tp
                              i_eq = i_eq_pbm(i_efs(i, j, n)), B(0, t_e, i_eq) -= qpk(n), A(0, i_efs(i, j, M), i_eq) += dTp_qpk(n);
                            for (n = 0; n < N[p]; n++) for (i_eq = i_eq_pbm(i_efs(i, j, n)), m = 0; m < N[p]; m++) //derivees en Tf
                                A(0, i_efs(i, j, m), i_eq_pbm(i_efs(i, j, n))) += dTf_qpk(n, m);

                            /* contributions de q_pi au tableau de flux paroi-interface */
                            for (k1 = 0; k1 < N[p]; k1++) for (k2 = k1 + 1; k2 < N[p]; k2++) //partie constante, derivee en Tp
                                Qec(i, t_e, k1, k2) += surf_fs[k] * qpi(k1, k2), Qf(i, i_efs(i, j, M), k1, k2) += surf_fs[k] * dTp_qpi(k1, k2);
                            for (m = 0; m < N[p]; m++) for (k1 = 0; k1 < N[p]; k1++) for (k2 = k1 + 1; k2 < N[p]; k2++)
                                  Qf(i, i_efs(i, j, m), k1, k2) += surf_fs[k] * dTf_qpi(k1, k2, m); //derivees en Tf
                          }
                        else for (n = 0; n < N[p]; n++) if ((i_eq = i_eq_cont(k, mix * n)) >= 0) /* pas d'inconnue de Tparoi -> continuite composante par composante */
                              B(!mix * n, t_e, i_eq) += sgn * Tefs(!mix * n, i_efs(i, j, mix * n)), A(!mix * n, i_efs(i, j, mix * n), i_eq) -= sgn;

                        /* contributions CLs : aux equations de flux si Neumann, a celles de continuite si Dirichlet ou Echange_impose */
                        if (type_f[k] == 1 || type_f[k] == 2) for (n = 0; n < N[p]; n++) if ((i_eq = i_eq_cont(k, mix * n)) >= 0) //Echange_impose_base
                              B(!mix * n, t_e, i_eq) -= ref_cast(Echange_impose_base, cls[p].get()[fcl[p](f, 1)].valeur()).T_ext(fcl[p](f, 2), n);
                        if (type_f[k] == 6) for (n = 0; n < N[p]; n++) if ((i_eq = i_eq_cont(k, mix * n)) >= 0) //Dirichlet (non homogene)
                              B(!mix * n, t_e, i_eq) -= ref_cast(Dirichlet, cls[p].get()[fcl[p](f, 1)].valeur()).val_imp(fcl[p](f, 2), n);
                        if (type_f[k] == 4) for (n = 0; n < N[p]; n++) if ((i_eq = i_eq_flux(k, mix * n)) >= 0) //Neumann
                              B(!mix * n, t_e, i_eq) -= ref_cast(Neumann, cls[p].get()[fcl[p](f, 1)].valeur()).flux_impose(fcl[p](f, 2), n);
                      }
                  }
                /* resolution(s) -> DGELSY */
                nw = -1, F77NAME(dgelsy)(&t_eq, &t_eq, &t_ec, &A(0, 0, 0), &t_eq, &B(0, 0, 0), &t_eq, &piv(0), &eps, &rk, &W(0), &nw, &infoo);
                for (piv.resize(t_eq), W.resize(nw = W(0)), n = 0; n < Nm; n++)
                  piv = 0, F77NAME(dgelsy)(&t_eq, &t_eq, &t_ec, &A(n, 0, 0), &t_eq, &B(n, 0, 0), &t_eq, &piv(0), &eps, &rk, &W(0), &nw, &infoo);

                /* mise a jour des Tefs et convergence. Si nonlinear = 0, tout est lineaire -> pas besoin d'autres iterations */
                for (n = 0; n < Nm; n++) for (i = 0, cv = 1; i < t_eq; i++) Tefs(n, i) += B(n, t_e, i), cv &= !nonlinear || std::fabs(B(n, t_e, i)) < 1e-3;
              }
            if (!cv && essai < 2) continue; //T_efs pas converge avec flux non coercif -> on essaie de stabiliser
            else if (!cv) Cerr << "non-convergence des T_efs!" << finl, Process::exit();

            /* subbstitution dans des u_efs^n dans Fec et Qec */
            for (n = 0; n < Nm; n++) for (i = 0; i < t_eq; i++) for (j = 0; j < t_ec; j++) for (k = 0; k < t_eq; k++)
                    Fec(n, i, j) += Ff(n, i, k) * B(n, j, k);
            if (q_pi_.dimension(0))  for (i = 0; i < n_e; i++) for (j = 0; j < t_ec; j++) for (k = 0; k < t_eq; k++)
                    for (k1 = 0; k1 < M; k1++) for (k2 = k1 + 1; k2 < M; k2++) Qec(i, j, k1, k2) += Qf(i, k, k1, k2) * B(0, j, k);

            /* A : forme(s) bilineaire */
            if (essai == 2) break;//pas la peine pour VFSYM
            for (A.resize(Nm, t_e, t_e), A = 0, m = 0; m < Nm; m++) for (i = 0; i < n_e; i++) for (p = s_pe[i][0], e = s_pe[i][1], j = 0; j < (int) se_f[i].size(); j++)
                  for (n = 0; n < (mix ? N[p] : 1); n++) for (l = 0; l < t_e; l++) A(m, i_e(i, n), l) -= Fec(m, i_efs(i, j, n), l);
            /* symmetrisation */
            for (n = 0; n < Nm; n++) for (i = 0; i < t_e; i++) for (j = 0; j <= i; j++) A(n, i, j) = A(n, j, i) = (A(n, i, j) + A(n, j, i)) / 2;
            /* v.p. la plus petite : DSYEV */
            nw = -1, F77NAME(DSYEV)("N", "U", &t_e, &A(0, 0, 0), &t_e, S.addr(), &W(0), &nw, &infoo);
            for (W.resize(nw = W(0)), S.resize(t_e), cv = 1, n = 0; n < Nm; n++)
              F77NAME(DSYEV)("N", "U", &t_e, &A(n, 0, 0), &t_e, &S(0), &W(0), &nw, &infoo), cv &= S(0) > -1e-8 * vol_s;
            if (cv) break;
          }

        /* contributions aux flux et aux matrices */
        for (i = 0; i < n_e; i++) if (s_pe[i][0] == 0 && (e = s_pe[i][1]) < zone[0].get().nb_elem()) for (j = 0; j < (int) se_f[i].size(); j++)
              for (k = se_f[i][j], f = s_pf[k][0] ? m_pf[s_pf[k]][1] : s_pf[k][1], n = 0; n < N[0]; n++) //seulement celles du probleme courant
                {
                  secmem(e, n) += Fec(!mix * n, i_efs(i, j, mix * n), t_e); //partie constante
                  if (f < zone0.premiere_face_int()) flux_bords_(f, n) += Fec(!mix * n, i_efs(i, j, mix * n), t_e);
                  for (k = 0; k < n_e; k++) if (mat[p = s_pe[k][0]]) for (eb = s_pe[k][1], m = (mix ? 0 : n); m < (mix ? N[p] : n + 1); m++) //derivees : si on a la matrice
                        (*mat[p])(N[0] * e + n, N[p] * eb + m) -= Fec(!mix * n, i_efs(i, j, mix * n), i_e(k, mix * m));
                }
        /* contributions a q_pi : partie constante seulement pour le moment */
        if (q_pi_.dimension(0)) for (i = 0; i < n_e; i++) if (s_pe[i][0] == 0) for (e = s_pe[i][1], k1 = 0; k1 < N[0]; k1++) for (k2 = k1 + 1; k2 < N[0]; k2++)
                  q_pi_(e, k1, k2) += Qec(i, t_e, k1, k2);
      }
  q_pi_a_jour_ = 1; //on peut maintenant demander q_pi
}

const DoubleTab& Op_Diff_PolyMAC_V2_Elem::q_pi() const
{
  if (!q_pi_a_jour_) Cerr << "Op_Diff_PolyMAC_V2_Elem : attempt to access q_pi (nucleate heat flux) before ajouter_blocs() has been called!" << finl
                            << "Please call assembler_blocs() on Energie_Multiphase before calling it on Masse_Multiphase." << finl, Process::exit();
  return q_pi_;
}

void Op_Diff_PolyMAC_V2_Elem::mettre_a_jour(double t)
{
  Op_Diff_PolyMAC_V2_base::mettre_a_jour(t);
  q_pi_a_jour_ = 0; //q_pi devient inaccessible
}
