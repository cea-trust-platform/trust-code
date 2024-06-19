/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem.h>
#include <Champ_Elem_PolyMAC_P0P1NC.h>
#include <Champ_Face_PolyMAC_P0P1NC.h>
#include <Masse_PolyMAC_P0P1NC_Elem.h>
#include <Convection_Diffusion_std.h>
#include <Discretisation_base.h>
#include <Dirichlet_homogene.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Schema_Temps_base.h>
#include <Domaine_Poly_base.h>
#include <Option_PolyMAC_P0.h>
#include <Pb_Multiphase.h>
#include <Probleme_base.h>
#include <Statistiques.h>
#include <Matrix_tools.h>
#include <Milieu_base.h>
#include <Array_tools.h>
#include <TRUSTLists.h>
#include <Dirichlet.h>
#include <Symetrie.h>
#include <Param.h>
#include <cfloat>
#include <vector>
#include <cmath>

extern Stat_Counter_Id convection_counter_;

Implemente_instanciable(Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem, "Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem|Op_Conv_EF_Stab_PolyMAC_P0_Elem", Op_Conv_PolyMAC_base);
Implemente_instanciable_sans_constructeur(Op_Conv_Amont_PolyMAC_P0P1NC_Elem, "Op_Conv_Amont_PolyMAC_P0P1NC_Elem|Op_Conv_Amont_PolyMAC_P0_Elem", Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem);
Implemente_instanciable_sans_constructeur(Op_Conv_Centre_PolyMAC_P0P1NC_Elem, "Op_Conv_Centre_PolyMAC_P0P1NC_Elem|Op_Conv_Centre_PolyMAC_P0_Elem", Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem);

Op_Conv_Amont_PolyMAC_P0P1NC_Elem::Op_Conv_Amont_PolyMAC_P0P1NC_Elem() { alpha_ = 1.0; }
Op_Conv_Centre_PolyMAC_P0P1NC_Elem::Op_Conv_Centre_PolyMAC_P0P1NC_Elem() { alpha_ = 0.0; }

// XD Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem interprete Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem 1 Class Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem
Sortie& Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem::printOn(Sortie& os) const { return Op_Conv_PolyMAC_base::printOn(os); }
Sortie& Op_Conv_Amont_PolyMAC_P0P1NC_Elem::printOn(Sortie& os) const { return Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem::printOn(os); }
Sortie& Op_Conv_Centre_PolyMAC_P0P1NC_Elem::printOn(Sortie& os) const { return Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem::printOn(os); }
Entree& Op_Conv_Amont_PolyMAC_P0P1NC_Elem::readOn(Entree& is) { return Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem::readOn(is); }
Entree& Op_Conv_Centre_PolyMAC_P0P1NC_Elem::readOn(Entree& is) { return Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem::readOn(is); }

Entree& Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem::readOn(Entree& is)
{
  Op_Conv_PolyMAC_base::readOn(is);
  if (que_suis_je().debute_par("Op_Conv_EF_Stab")) //on n'est pas dans Op_Conv_Amont/Centre
    {
      Param param(que_suis_je());
      param.ajouter("alpha", &alpha_);            // XD_ADD_P double parametre ajustant la stabilisation de 0 (schema centre) a 1 (schema amont)
      param.lire_avec_accolades_depuis(is);
    }

  if (sub_type(Masse_Multiphase, equation())) //convection dans Masse_Multiphase -> champs de debit / titre
    {
      const Pb_Multiphase& pb = ref_cast(Pb_Multiphase, equation().probleme());
      noms_cc_phases_.dimensionner(pb.nb_phases()), cc_phases_.resize(pb.nb_phases());
      noms_vd_phases_.dimensionner(pb.nb_phases()), vd_phases_.resize(pb.nb_phases());
      noms_x_phases_.dimensionner(pb.nb_phases()), x_phases_.resize(pb.nb_phases());
      for (int i = 0; i < pb.nb_phases(); i++)
        {
          noms_cc_phases_[i] = Nom("debit_") + pb.nom_phase(i);
          noms_vd_phases_[i] = Nom("vitesse_debitante_") + pb.nom_phase(i);
          noms_x_phases_[i] = Nom("titre_") + pb.nom_phase(i);
        }
    }
  return is;
}

void Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem::preparer_calcul()
{
  Op_Conv_PolyMAC_base::preparer_calcul();

  /* au cas ou... */
  const Domaine_Poly_base& domaine = le_dom_poly_.valeur();
  equation().init_champ_convecte();
  flux_bords_.resize(domaine.premiere_face_int(), (le_champ_inco.non_nul() ? le_champ_inco->valeurs() : equation().inconnue().valeurs()).line_size());

  if (domaine.domaine().nb_joints() && domaine.domaine().joint(0).epaisseur() < 2)
    {
      Cerr << "Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem : largeur de joint insuffisante (minimum 2)!" << finl;
      Process::exit();
    }
}

double Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem::calculer_dt_stab() const
{
  double dt = 1e10;
  const Domaine_Poly_base& domaine = le_dom_poly_.valeur();
  const DoubleVect& fs = domaine.face_surfaces(), &pf = equation().milieu().porosite_face(), &ve = domaine.volumes(), &pe = equation().milieu().porosite_elem();
  const DoubleTab& vit = vitesse_->valeurs(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ?
                          &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr;
  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins(), &fcl = ref_cast(Champ_Elem_PolyMAC, equation().inconnue().valeur()).fcl();
  int i, e, f, n, N = std::min(vit.line_size(), equation().inconnue().valeurs().line_size());
  DoubleTrav flux(N); //somme des flux pf * |f| * vf

  for (e = 0; e < domaine.nb_elem(); e++)
    {
      for (flux = 0, i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0 ; i++)
        if (!Option_PolyMAC_P0::traitement_axi || (Option_PolyMAC_P0::traitement_axi && !(fcl(f,0) == 4 || fcl(f,0) == 5)) )
          for (n = 0; n < N; n++)
            flux(n) += pf(f) * fs(f) * std::max((e == f_e(f, 1) ? 1 : -1) * vit(f, n), 0.); //seul le flux entrant dans e compte

      for (n = 0; n < N; n++)
        if ((!alp || (*alp)(e, n) > 1e-3) && std::abs(flux(n)) > 1e-12 /* eviter les valeurs 'tres proches de 0 mais pas completement nulles' */)
          dt = std::min(dt, pe(e) * ve(e) / flux(n));
    }

  return Process::mp_min(dt);
}

void Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem::dimensionner_blocs(matrices_t mats, const tabs_t& semi_impl) const
{
  const Domaine_Poly_base& domaine = le_dom_poly_.valeur();
  const IntTab& f_e = domaine.face_voisins(), &fcl_v = ref_cast(Champ_Face_base, vitesse_.valeur()).fcl();
  int i, j, e, eb, f, n, N = equation().inconnue().valeurs().line_size();
  const Champ_Inc_base& cc = equation().champ_convecte();

  for (auto &&i_m : mats)
    if (i_m.first == "vitesse" || (cc.derivees().count(i_m.first) && !semi_impl.count(cc.le_nom().getString())))
      {
        Matrice_Morse mat;
        IntTab stencil(0, 2);

        int m, M = equation().probleme().get_champ(i_m.first.c_str()).valeurs().line_size();
        if (i_m.first == "vitesse") /* vitesse */
          {
            for (f = 0; f < domaine.nb_faces_tot(); f++)
              if (fcl_v(f, 0) < 2)
                for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
                  if (e < domaine.nb_elem())
                    for (n = 0; n < N; n++)
                      stencil.append_line(N * e + n, M * f + n * (M > 1));
          }
        else
          for (f = 0; f < domaine.nb_faces_tot(); f++)
            for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
              if (e < domaine.nb_elem()) /* inconnues scalaires */
                for (j = 0; j < 2 && (eb = f_e(f, j)) >= 0; j++)
                  for (n = 0, m = 0; n < N; n++, m += (M > 1))
                    stencil.append_line(N * e + n, M * eb + m);

        tableau_trier_retirer_doublons(stencil);
        Matrix_tools::allocate_morse_matrix(equation().inconnue().valeurs().size_totale(), i_m.first == "vitesse" ? vitesse_->valeurs().size_totale() : cc.derivees().at(i_m.first).size_totale(),
                                            stencil, mat);
        i_m.second->nb_colonnes() ? *i_m.second += mat : *i_m.second = mat;
      }
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
void Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem::ajouter_blocs(matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  statistiques().begin_count(convection_counter_);
  const Domaine_Poly_base& domaine = le_dom_poly_.valeur();
  const IntTab& f_e = domaine.face_voisins(), &fcl = ref_cast(Champ_Inc_P0_base, equation().inconnue().valeur()).fcl(), &fcl_v = ref_cast(Champ_Face_base, vitesse_.valeur()).fcl();
  const DoubleVect& fs = domaine.face_surfaces(), &pf = equation().milieu().porosite_face();
  const Champ_Inc_base& cc = le_champ_inco.non_nul() ? le_champ_inco->valeur() : equation().champ_convecte();
  const std::string& nom_cc = cc.le_nom().getString();
  const DoubleTab& vit = vitesse_->valeurs(), &vcc = semi_impl.count(nom_cc) ? semi_impl.at(nom_cc) : cc.valeurs(), bcc = cc.valeur_aux_bords();
  int i, j, e, eb, f, n, m, N = vcc.line_size(), Mv = vit.line_size(), M;

  Matrice_Morse *m_vit = mats.count("vitesse") ? mats.at("vitesse") : nullptr;
  std::vector<std::tuple<const DoubleTab*, Matrice_Morse*, int>> d_cc; //liste des derivees de cc a renseigner : couples (derivee de cc, matrice, nb de compos de la variable)
  if (!semi_impl.count(nom_cc))
    for (auto &i_m : mats)
      if (cc.derivees().count(i_m.first))
        d_cc.push_back(std::make_tuple(&cc.derivees().at(i_m.first), i_m.second, equation().probleme().get_champ(i_m.first.c_str()).valeurs().line_size()));

  DoubleTrav dv_flux(N), dc_flux(2, N); //derivees du flux convectif a la face par rapport a la vitesse / au champ convecte amont / aval

  /* convection aux faces internes (fcl(f, 0) == 0), de Neumann_val_ext ou de Dirichlet */
  for (f = 0; f < domaine.nb_faces(); f++)
    if (!fcl(f, 0) || (fcl(f, 0) > 4 && fcl(f, 0) < 7))
      {
        for (dv_flux = 0, dc_flux = 0, i = 0; i < 2; i++)
          for (e = f_e(f, i), n = 0, m = 0; n < N; n++, m += (Mv > 1))
            {
              double v = vit(f, m) ? vit(f, m) : DBL_MIN, fac = pf(f) * fs(f) * (1. + (v * (i ? -1 : 1) > 0 ? 1. : -1) * alpha_) / 2;
              dv_flux(n) += fac * (e >= 0 ? vcc(e, n) : bcc(f, n)); //f est reelle -> indice trivial dans bcc
              dc_flux(i, n) = e >= 0 ? fac * vit(f, m) : 0;
            }

        //second membre
        for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
          if (e < domaine.nb_elem())
            for (n = 0, m = 0; n < N; n++, m += (Mv > 1))
              secmem(e, n) -= (i ? -1 : 1) * dv_flux(n) * vit(f, m);
        //derivees : vitesse
        if (m_vit && fcl_v(f, 0) < 2)
          for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
            if (e < domaine.nb_elem())
              for (n = 0, m = 0; n < N; n++, m += (Mv > 1))
                (*m_vit)(N * e + n, Mv * f + m) += (i ? -1 : 1) * dv_flux(n);
        //derivees : champ convecte
        for (auto &&d_m_i : d_cc)
          for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
            if (e < domaine.nb_elem())
              for (j = 0; j < 2 && (eb = f_e(f, j)) >= 0; j++)
                for (n = 0, m = 0, M = std::get<2>(d_m_i); n < N; n++, m += (M > 1))
                  (*std::get<1>(d_m_i))(N * e + n, M * eb + m) += (i ? -1 : 1) * dc_flux(j, n) * (*std::get<0>(d_m_i))(eb, m);
      }
  statistiques().end_count(convection_counter_);
}

void Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  Op_Conv_PolyMAC_base::get_noms_champs_postraitables(nom,opt);
  Noms noms_compris;

  if (sub_type(Masse_Multiphase, equation()))
    {
      const Pb_Multiphase& pb = ref_cast(Pb_Multiphase, equation().probleme());

      for (int i = 0; i < pb.nb_phases(); i++)
        {
          noms_compris.add(noms_cc_phases_[i]);
          noms_compris.add(noms_vd_phases_[i]);
          noms_compris.add(noms_x_phases_[i]);
        }
    }
  if (opt==DESCRIPTION)
    Cerr<<" Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem : "<< noms_compris <<finl;
  else
    nom.add(noms_compris);
}

void Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem::creer_champ(const Motcle& motlu)
{
  Op_Conv_PolyMAC_base::creer_champ(motlu);
  int i = noms_cc_phases_.rang(motlu), j = noms_vd_phases_.rang(motlu), k = noms_x_phases_.rang(motlu);
  if (i >= 0 && !cc_phases_[i].non_nul())
    {
      equation().discretisation().discretiser_champ("vitesse", equation().domaine_dis(), noms_cc_phases_[i], "kg/m2/s", dimension, 1, 0, cc_phases_[i]);
      champs_compris_.ajoute_champ(cc_phases_[i]);
    }
  if (j >= 0 && !vd_phases_[j].non_nul())
    {
      equation().discretisation().discretiser_champ("vitesse", equation().domaine_dis(), noms_vd_phases_[j], "m/s", dimension, 1, 0, vd_phases_[j]);
      champs_compris_.ajoute_champ(vd_phases_[j]);
    }
  if (k >= 0 && !x_phases_[k].non_nul())
    {
      equation().discretisation().discretiser_champ("temperature", equation().domaine_dis(), noms_x_phases_[k], "m/s", 1, 1, 0, x_phases_[k]);
      champs_compris_.ajoute_champ(x_phases_[k]);
    }
}

void Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem::mettre_a_jour(double temps)
{
  Op_Conv_PolyMAC_base::mettre_a_jour(temps);
  const Domaine_Poly_base& domaine = le_dom_poly_.valeur();
  const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces();
  const Champ_Inc_base& cc = le_champ_inco.non_nul() ? le_champ_inco.valeur() : equation().champ_convecte();
  const DoubleVect& pf = equation().milieu().porosite_face(), &pe = equation().milieu().porosite_elem(), &fs = domaine.face_surfaces(), &ve = domaine.volumes();
  const DoubleTab& vit = vitesse_->valeurs(), &vcc = cc.valeurs(), bcc = cc.valeur_aux_bords(), &xv = domaine.xv(), &xp = domaine.xp();
  DoubleTab balp;
  if (vd_phases_.size())
    balp = equation().inconnue().valeur().valeur_aux_bords();

  int i, e, f, d, D = dimension, n, m, N = vcc.line_size(), M = vit.line_size();
  DoubleTrav cc_f(N); //valeur du champ convecte aux faces
  /* flux aux bords */
  for (f = 0; f < domaine.premiere_face_int(); f++)
    {
      for (cc_f = 0, i = 0; i < 2; i++)
        for (e = f_e(f, i), n = 0, m = 0; n < N; n++, m += (M > 1))
          cc_f(n) += (1. + (vit(f, m) * (i ? -1 : 1) >= 0 ? 1. : -1.) * alpha_) / 2 * (e >= 0 ? vcc(e, n) : bcc(f, n));

      for (n = 0, m = 0; n < N; n++, m += (M > 1))
        flux_bords_(f, n) = pf(f) * fs(f) * vit(f, m) * cc_f(n);
    }

  if (cc_phases_.size())
    for (n = 0, m = 0; n < N; n++, m += (M > 1))
      if (cc_phases_[n].non_nul()) /* mise a jour des champs de debit */
        {
          Champ_Face_PolyMAC_P0P1NC& c_ph = ref_cast(Champ_Face_PolyMAC_P0P1NC, cc_phases_[n].valeur());
          DoubleTab& v_ph = c_ph.valeurs();
          for (f = 0; f < domaine.nb_faces(); v_ph(f) *= vit(f, m) * pf(f), f++)
            for (v_ph(f) = 0, i = 0; i < 2; i++)
              v_ph(f) += (1. + (vit(f, m) * (i ? -1 : 1) >= 0 ? 1. : -1.) * alpha_) / 2 * ((e = f_e(f, i)) >= 0 ? vcc(e, n) : bcc(f, n));
          c_ph.changer_temps(temps);
        }

  if (vd_phases_.size())
    for (n = 0, m = 0; n < N; n++, m += (M > 1))
      if (vd_phases_[n].non_nul()) /* mise a jour des champs de vitesse debitante */
        {
          const DoubleTab& alp = equation().inconnue().valeurs();
          Champ_Face_PolyMAC_P0P1NC& c_ph = ref_cast(Champ_Face_PolyMAC_P0P1NC, vd_phases_[n].valeur());
          DoubleTab& v_ph = c_ph.valeurs();
          /* on remplit la partie aux faces, puis on demande au champ d'interpoler aux elements */
          for (f = 0; f < domaine.nb_faces(); v_ph(f) *= vit(f, m) * pf(f), f++)
            for (v_ph(f) = 0, i = 0; i < 2; i++)
              v_ph(f) += (1. + (vit(f, m) * (i ? -1 : 1) >= 0 ? 1. : -1.) * alpha_) / 2 * ((e = f_e(f, i)) >= 0 ? alp(e, n) : balp(f, n));
          c_ph.changer_temps(temps);
        }

  DoubleTrav G(N), v(N, D);
  double Gt;
  if (x_phases_.size())
    for (e = 0; e < domaine.nb_elem(); e++) //titre : aux elements
      {
        for (v = 0, i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++)
          for (n = 0; n < N; n++)
            for (d = 0; d < D; d++)
              v(n, d) += fs(f) * pf(f) * (xv(f, d) - xp(e, d)) * (e == f_e(f, 0) ? 1 : -1) * vit(f, n) / (pe(e) * ve(e));
        for (Gt = 0, n = 0; n < N; Gt += G(n), n++)
          G(n) = vcc(e, n) * sqrt(domaine.dot(&v(n, 0), &v(n, 0)));
        for (n = 0; n < N; n++)
          if (x_phases_[n].non_nul())
            x_phases_[n]->valeurs()(e) = Gt ? G(n) / Gt : 0;
      }
  if (x_phases_.size())
    for (n = 0; n < N; n++)
      if (x_phases_[n].non_nul())
        x_phases_[n]->changer_temps(temps);
}

