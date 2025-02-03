/****************************************************************************
* Copyright (c) 2025, CEA
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
#include <Option_PolyMAC.h>
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
// XD Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem interprete Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem 1 Class Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem

Op_Conv_Amont_PolyMAC_P0P1NC_Elem::Op_Conv_Amont_PolyMAC_P0P1NC_Elem() { alpha_ = 1.0; }
Op_Conv_Centre_PolyMAC_P0P1NC_Elem::Op_Conv_Centre_PolyMAC_P0P1NC_Elem() { alpha_ = 0.0; }

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
      noms_cc_phases_.dimensionner(pb.nb_phases());
      cc_phases_.resize(pb.nb_phases());

      noms_vd_phases_.dimensionner(pb.nb_phases());
      vd_phases_.resize(pb.nb_phases());

      noms_x_phases_.dimensionner(pb.nb_phases());
      x_phases_.resize(pb.nb_phases());

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
  const Domaine_Poly_base& domaine = le_dom_poly_.valeur();
  const DoubleVect& fs = domaine.face_surfaces(), &pf = equation().milieu().porosite_face(),
                    &ve = domaine.volumes(), &pe = equation().milieu().porosite_elem();
  const DoubleTab& vit = vitesse_->valeurs(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ?
                          &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue().passe() : nullptr;

  const IntTab& e_f = domaine.elem_faces(), &f_e = domaine.face_voisins(), &fcl = ref_cast(Champ_Elem_PolyMAC, equation().inconnue()).fcl();
  const int N = std::min(vit.line_size(), equation().inconnue().valeurs().line_size());

  DoubleTrav flux(N); //somme des flux pf * |f| * vf
  double dt = 1.e10;

  for (int e = 0; e < domaine.nb_elem(); e++)
    {
      flux = 0.;
      const double poro_vol = pe(e) * ve(e);
      for (int i = 0; i < e_f.dimension(1); i++)
        {
          const int f = e_f(e, i);
          if (f < 0) continue;

          if (!Option_PolyMAC::TRAITEMENT_AXI || (Option_PolyMAC::TRAITEMENT_AXI && !(fcl(f, 0) == 4 || fcl(f, 0) == 5)))
            for (int n = 0; n < N; n++)
              flux(n) += pf(f) * fs(f) * std::max((e == f_e(f, 1) ? 1 : -1) * vit(f, n), 0.); //seul le flux entrant dans e compte
        }

      for (int n = 0; n < N; n++)
        if ((!alp || (*alp)(e, n) > 1e-3) && std::abs(flux(n)) > 1e-12 /* eviter les valeurs 'tres proches de 0 mais pas completement nulles' */)
          dt = std::min(dt, poro_vol / flux(n));
    }

  return Process::mp_min(dt);
}

void Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem::dimensionner_blocs(matrices_t mats, const tabs_t& semi_impl) const
{
  const Domaine_Poly_base& domaine = le_dom_poly_.valeur();
  const Champ_Inc_base& cc = equation().champ_convecte();

  const IntTab& f_e = domaine.face_voisins(), &fcl_v = ref_cast(Champ_Face_base, vitesse_.valeur()).fcl();
  const int N = equation().inconnue().valeurs().line_size();

  for (const auto &i_m : mats)
    {
      // Verification des conditions sur "vitesse" ou les derivees semi-implicites
      if (i_m.first == "vitesse" || (cc.derivees().count(i_m.first) && !semi_impl.count(cc.le_nom().getString())))
        {
          Matrice_Morse mat;
          IntTab stencil(0, 2);

          int M = equation().probleme().get_champ(i_m.first.c_str()).valeurs().line_size();

          if (i_m.first == "vitesse")
            {
              // Traitement specifique pour "vitesse"
              for (int f = 0; f < domaine.nb_faces_tot(); f++)
                if (fcl_v(f, 0) < 2) // CL mais pas dirichlet ou face interne
                  for (int i = 0; i < 2; i++)
                    {
                      int e = f_e(f, i);
                      if (e < 0) continue;

                      if (e < domaine.nb_elem())
                        for (int n = 0; n < N; n++)
                          stencil.append_line(N * e + n, M * f + n * (M > 1));
                    }
            }
          else
            {
              // Traitement pour les inconnues scalaires
              for (int f = 0; f < domaine.nb_faces_tot(); f++)
                for (int i = 0; i < 2; i++)
                  {
                    int e = f_e(f, i);
                    if (e < 0) continue;

                    if (e < domaine.nb_elem())
                      for (int j = 0; j < 2; j++)
                        {
                          int eb = f_e(f, j);
                          if (eb < 0) continue;

                          int m = 0;
                          for (int n = 0; n < N; n++, m += (M > 1))
                            stencil.append_line(N * e + n, M * eb + m);
                        }
                  }
            }

          // Suppression des doublons et allocation de la matrice
          tableau_trier_retirer_doublons(stencil);
          Matrix_tools::allocate_morse_matrix(equation().inconnue().valeurs().size_totale(),
                                              (i_m.first == "vitesse" ? vitesse_->valeurs().size_totale() : cc.derivees().at(i_m.first).size_totale()),
                                              stencil, mat);

          // Mise a jour de la matrice dans la collection
          if (i_m.second->nb_colonnes())
            *i_m.second += mat;
          else
            *i_m.second = mat;
        }
    }
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
void Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem::ajouter_blocs(matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  statistiques().begin_count(convection_counter_);

  const Domaine_Poly_base& domaine = le_dom_poly_.valeur();
  const Champ_Inc_base& cc = le_champ_inco.non_nul() ? le_champ_inco.valeur() : equation().champ_convecte();
  const std::string& nom_cc = cc.le_nom().getString();
  Matrice_Morse *m_vit = mats.count("vitesse") ? mats.at("vitesse") : nullptr;

  const IntTab& f_e = domaine.face_voisins(),
                &fcl = ref_cast(Champ_Inc_P0_base, equation().inconnue()).fcl(),
                 &fcl_v = ref_cast(Champ_Face_base, vitesse_.valeur()).fcl();

  const DoubleVect& fs = domaine.face_surfaces(), &pf = equation().milieu().porosite_face();
  const DoubleTab& vit = vitesse_->valeurs(), &vcc = semi_impl.count(nom_cc) ? semi_impl.at(nom_cc) : cc.valeurs(), bcc = cc.valeur_aux_bords();
  const int N = vcc.line_size(), Mv = vit.line_size();

  std::vector<std::tuple<const DoubleTab*, Matrice_Morse*, int>> d_cc; //liste des derivees de cc a renseigner : couples (derivee de cc, matrice, nb de compos de la variable)

  if (!semi_impl.count(nom_cc))
    for (auto &i_m : mats)
      if (cc.derivees().count(i_m.first))
        d_cc.push_back(std::make_tuple(&cc.derivees().at(i_m.first), i_m.second, equation().probleme().get_champ(i_m.first.c_str()).valeurs().line_size()));

  DoubleTrav dv_flux(N), dc_flux(2, N); //derivees du flux convectif a la face par rapport a la vitesse / au champ convecte amont / aval

  // Convection aux faces internes, Neumann_val_ext ou Dirichlet
  for (int f = 0; f < domaine.nb_faces(); f++)
    {
      // Verification de la condition pour traiter cette face
      if (fcl(f, 0) == 0 || (fcl(f, 0) > 4 && fcl(f, 0) < 7))
        {
          dv_flux = 0.;
          dc_flux = 0.;

          // Calcul de dv_flux et dc_flux
          for (int i = 0; i < 2; i++)
            {
              int e = f_e(f, i);
              int m = 0;

              for (int n = 0; n < N; n++, m += (Mv > 1))
                {
                  const double vit_f = vit(f, m);
                  const double v = vit_f ? vit_f : DBL_MIN;
                  const double fac = pf(f) * fs(f) * (1. + (v * (i ? -1 : 1) > 0 ? 1. : -1) * alpha_) / 2;

                  dv_flux(n) += fac * (e >= 0 ? vcc(e, n) : bcc(f, n));  // f est reelle -> indice trivial dans bcc
                  dc_flux(i, n) = e >= 0 ? fac * vit_f : 0;
                }
            }

          // Mise a jour du second membre
          for (int i = 0; i < 2; i++)
            {
              int e = f_e(f, i);
              if (e < 0) continue;

              if (e < domaine.nb_elem())
                {
                  int m = 0;
                  for (int n = 0; n < N; n++, m += (Mv > 1))
                    secmem(e, n) -= (i ? -1 : 1) * dv_flux(n) * vit(f, m);
                }
            }

          // Mise a jour des derivees : vitesse
          if (m_vit && fcl_v(f, 0) < 2)
            for (int i = 0; i < 2; i++)
              {
                int e = f_e(f, i);
                if (e < 0) continue;

                if (e < domaine.nb_elem())
                  {
                    int m = 0;
                    for (int n = 0; n < N; n++, m += (Mv > 1))
                      (*m_vit)(N * e + n, Mv * f + m) += (i ? -1 : 1) * dv_flux(n);
                  }
              }

          // Mise a jour des derivees : champ convecte
          for (auto &&d_m_i : d_cc)
            {
              int M = std::get<2>(d_m_i);
              for (int i = 0; i < 2; i++)
                {
                  int e = f_e(f, i);
                  if (e < 0) continue;

                  if (e < domaine.nb_elem())
                    for (int j = 0; j < 2; j++)
                      {
                        int eb = f_e(f, j);
                        if (eb < 0) continue;

                        int m = 0;
                        for (int n = 0; n < N; n++, m += (M > 1))
                          (*std::get<1>(d_m_i))(N * e + n, M * eb + m) += (i ? -1 : 1) * dc_flux(j, n) * (*std::get<0>(d_m_i))(eb, m);
                      }
                }
            }
        }
    }
  statistiques().end_count(convection_counter_);
}

void Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem::get_noms_champs_postraitables(Noms& nom, Option opt) const
{
  Op_Conv_PolyMAC_base::get_noms_champs_postraitables(nom, opt);
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
  if (opt == DESCRIPTION)
    Cerr << " Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem : " << noms_compris << finl;
  else
    nom.add(noms_compris);
}

void Op_Conv_EF_Stab_PolyMAC_P0P1NC_Elem::creer_champ(const Motcle& motlu)
{
  Op_Conv_PolyMAC_base::creer_champ(motlu);

  const int i = noms_cc_phases_.rang(motlu), j = noms_vd_phases_.rang(motlu), k = noms_x_phases_.rang(motlu);

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
  const Champ_Inc_base& cc = le_champ_inco.non_nul() ? le_champ_inco.valeur() : equation().champ_convecte();

  const IntTab& f_e = domaine.face_voisins(), &e_f = domaine.elem_faces();

  const DoubleVect& pf = equation().milieu().porosite_face(), &pe = equation().milieu().porosite_elem(),
                    &fs = domaine.face_surfaces(), &ve = domaine.volumes();

  const DoubleTab& vit = vitesse_->valeurs(), &vcc = cc.valeurs(),
                   &bcc = cc.valeur_aux_bords(), &xv = domaine.xv(), &xp = domaine.xp();

  DoubleTrav balp;

  if (vd_phases_.size())
    balp = equation().inconnue().valeur_aux_bords();

  const int  D = dimension,  N = vcc.line_size(), M = vit.line_size();
  DoubleTrav cc_f(N); //valeur du champ convecte aux faces

  /* flux aux bords */
  for (int f = 0; f < domaine.premiere_face_int(); f++)
    {
      cc_f = 0.;
      for (int i = 0; i < 2; i++)
        {
          int e = f_e(f, i);
          int m = 0;
          for (int n = 0; n < N; n++, m += (M > 1))
            cc_f(n) += (1. + (vit(f, m) * (i ? -1 : 1) >= 0 ? 1. : -1.) * alpha_) / 2 * (e >= 0 ? vcc(e, n) : bcc(f, n));
        }

      int m = 0;
      for (int n = 0; n < N; n++, m += (M > 1))
        flux_bords_(f, n) = pf(f) * fs(f) * vit(f, m) * cc_f(n);
    }

  if (cc_phases_.size())
    {
      int m = 0;
      for (int n = 0; n < N; n++, m += (M > 1))
        if (cc_phases_[n].non_nul()) /* mise a jour des champs de debit */
          {
            Champ_Face_PolyMAC_P0P1NC& c_ph = ref_cast(Champ_Face_PolyMAC_P0P1NC, cc_phases_[n].valeur());
            DoubleTab& v_ph = c_ph.valeurs();

            for (int f = 0; f < domaine.nb_faces(); f++)
              {
                v_ph(f) = 0.;

                for (int i = 0; i < 2; i++)
                  {
                    int e = f_e(f, i);
                    double signe = (vit(f, m) * (i ? -1 : 1) >= 0) ? 1. : -1.;
                    double facteur = (1. + signe * alpha_) / 2.;

                    v_ph(f) += facteur * (e >= 0 ? vcc(e, n) : bcc(f, n));
                  }

                v_ph(f) *= vit(f, m) * pf(f);
              }

            c_ph.changer_temps(temps);
          }
    }

  if (vd_phases_.size())
    {
      int m = 0;
      for (int n = 0; n < N; n++, m += (M > 1))
        if (vd_phases_[n].non_nul()) /* mise a jour des champs de vitesse debitante */
          {
            const DoubleTab& alp = equation().inconnue().valeurs();
            Champ_Face_PolyMAC_P0P1NC& c_ph = ref_cast(Champ_Face_PolyMAC_P0P1NC, vd_phases_[n].valeur());
            DoubleTab& v_ph = c_ph.valeurs();

            /* on remplit la partie aux faces, puis on demande au champ d'interpoler aux elements */
            for (int f = 0; f < domaine.nb_faces(); f++)
              {
                v_ph(f) = 0.;

                for (int i = 0; i < 2; i++)
                  {
                    int e = f_e(f, i);
                    double signe = (vit(f, m) * (i ? -1 : 1) >= 0) ? 1. : -1.;
                    double facteur = (1. + signe * alpha_) / 2.;

                    v_ph(f) += facteur * (e >= 0 ? alp(e, n) : balp(f, n));

                  }
                v_ph(f) *= vit(f, m) * pf(f);
              }

            c_ph.changer_temps(temps);
          }
    }

  DoubleTrav G(N), v(N, D);
  double Gt;

  if (x_phases_.size())
    for (int e = 0; e < domaine.nb_elem(); e++) // titre : aux elements
      {
        v = 0.;
        for (int i = 0; i < e_f.dimension(1); i++)
          {
            int f = e_f(e, i);
            if (f < 0) continue;

            for (int n = 0; n < N; n++)
              for (int d = 0; d < D; d++)
                v(n, d) += fs(f) * pf(f) * (xv(f, d) - xp(e, d)) * (e == f_e(f, 0) ? 1 : -1) * vit(f, n) / (pe(e) * ve(e));
          }

        Gt = 0.;
        for (int n = 0; n < N; n++)
          {
            G(n) = vcc(e, n) * sqrt(domaine.dot(&v(n, 0), &v(n, 0)));
            Gt += G(n);
          }

        for (int n = 0; n < N; n++)
          if (x_phases_[n].non_nul())
            x_phases_[n]->valeurs()(e) = Gt ? G(n) / Gt : 0.;
      }

  if (x_phases_.size())
    for (int n = 0; n < N; n++)
      if (x_phases_[n].non_nul())
        x_phases_[n]->changer_temps(temps);
}

