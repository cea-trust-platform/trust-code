/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Op_Diff_CoviMAC_Face.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_CoviMAC_Face.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Zone_CoviMAC.h>
#include <Zone_Cl_CoviMAC.h>
#include <IntLists.h>
#include <DoubleLists.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Champ_Face_CoviMAC.h>
#include <Champ_Uniforme.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Mod_turb_hyd_base.h>
#include <Synonyme_info.h>
#include <MD_Vector_base.h>
#include <Op_Grad_CoviMAC_Face.h>
#include <Pb_Multiphase.h>

Implemente_instanciable( Op_Diff_CoviMAC_Face, "Op_Diff_CoviMAC_Face|Op_Dift_CoviMAC_Face_CoviMAC", Op_Diff_CoviMAC_base ) ;
Add_synonym(Op_Diff_CoviMAC_Face, "Op_Diff_CoviMAC_var_Face");
Add_synonym(Op_Diff_CoviMAC_Face, "Op_Dift_CoviMAC_var_Face_CoviMAC");

Sortie& Op_Diff_CoviMAC_Face::printOn( Sortie& os ) const
{
  Op_Diff_CoviMAC_base::printOn( os );
  return os;
}

Entree& Op_Diff_CoviMAC_Face::readOn( Entree& is )
{
  Op_Diff_CoviMAC_base::readOn( is );
  return is;
}

void Op_Diff_CoviMAC_Face::completer()
{
  Op_Diff_CoviMAC_base::completer();
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Equation_base& eq = equation();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, eq.inconnue().valeur());
  ch.init_cl(), zone.init_equiv();
  flux_bords_.resize(zone.premiere_face_int(), dimension * ch.valeurs().line_size());
  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 1)
    Cerr << "Op_Diff_CoviMAC_Face : largeur de joint insuffisante (minimum 1)!" << finl, Process::exit();
  porosite_e.ref(zone.porosite_elem());
  porosite_f.ref(zone.porosite_face());


  const RefObjU& modele_turbulence = equation().get_modele(TURBULENCE);
  if (modele_turbulence.non_nul())
    {
      const Mod_turb_hyd_base& mod_turb = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur());
      const Champ_Fonc& alpha_t = mod_turb.viscosite_turbulente();
      associer_diffusivite_turbulente(alpha_t);
    }
}

void Op_Diff_CoviMAC_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces();
  const DoubleTab& nf = zone.face_normales();
  const DoubleVect& fs = zone.face_surfaces();

  const std::string& nom_inco = ch.le_nom().getString();
  if (!matrices.count(nom_inco) || semi_impl.count(nom_inco)) return; //semi-implicite ou pas de bloc diagonal -> rien a faire
  if (matrices.size() > 1 && !semi_impl.count("alpha_rho")) //implicite complet -> pas code
    Cerr << que_suis_je() << " : non-velocity derivatives not coded yet!" << finl, Process::exit();
  Matrice_Morse& mat = *matrices.at(nom_inco), mat2;

  int i, j, k, e, eb, f, fb, fc, n, N = ch.valeurs().line_size(), ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), d, D = dimension;

  IntTrav stencil(0, 2), tpfa(0, N);
  stencil.set_smart_resize(1), zone.creer_tableau_faces(tpfa), tpfa = 1;

  update_nu();
  /* stencils du flux : ceux (reduits) de update_nu si nu constant ou scalaire, ceux (complets) de la zone sinon */
  const IntTab& feb_d = nu_constant_ ? phif_d : zone.feb_d, &feb_j = nu_constant_ ? phif_j : zone.feb_j;
  Cerr << "Op_Diff_CoviMAC_Face::dimensionner() : ";

  for (f = 0; f < nf_tot; f++) if (ch.fcl(f, 0) > 2) //face de bord (hors Neumann / Symetrie) : flux a deux points
      {
        for (e = f_e(f, 0), i = 0; i < e_f.dimension(1) && (fb = e_f(e, i)) >= 0; i++) if (fb < zone.nb_faces() && ch.fcl(fb, 0) < 2) for (n = 0; n < N; n++)
              stencil.append_line(N * fb + n, N * fb + n); //face -> face
        if (e < zone.nb_elem()) for (d = 0; d < D; d++) for (n = 0; n < N; n++) stencil.append_line(N * (nf_tot + ne_tot * d + e) + n, N * (nf_tot + ne_tot * d + e) + n); //elem -> elem
      }
    else if (f_e(f, 0) >= 0 && f_e(f, 1) >= 0) for (i = 0; i < 2; i++) for (e = f_e(f, i), j = feb_d(f); j < feb_d(f + 1); j++) //faces internes -> flux multipoints
          {
            for (eb = feb_j(j), k = 0; k < e_f.dimension(1) && (fb = e_f(e, k)) >= 0; k++) if (ch.fcl(fb, 0) < 2 && fb < zone.nb_faces())
                {
                  if (j < feb_d(f) + 2 && (fc = zone.equiv(f, i, k)) >= 0 && ch.fcl(fc, 0) < 2) for (n = 0; n < N; n++) //equivalence : face-face
                      stencil.append_line(N * fb + n, N * (j == i + feb_d(f) ? fb : fc) + n);
                  for (d = 0; d < D; d++) if (dabs(nf(fb, d)) > 1e-6 * fs(fb)) for (n = 0; n < N; n++) //sinon : elem-face
                        stencil.append_line(N * fb + n, N * (nf_tot + D * (eb < ne_tot ? eb : f_e(eb - ne_tot, 0)) + d) + n);
                }
            if (e < zone.nb_elem()) for (d = 0; d < D; d++) for (n = 0; n < N; n++) //elem -> elem
                  stencil.append_line(N * (nf_tot + D * e + d) + n, N * (nf_tot + D * (eb < ne_tot ? eb : f_e(eb - ne_tot, 0)) + d) + n);
            for (n = 0; n < N; n++) if (j >= feb_d(f) + 2) tpfa(f, n) = 0;
          }

  tableau_trier_retirer_doublons(stencil);
  Cerr << "width " << Process::mp_sum(stencil.dimension(0)) * 1. / (N * (nf_tot + D * ne_tot))
       << " " << mp_somme_vect(tpfa) * 100. / (N * zone.md_vector_faces().valeur().nb_items_seq_tot()) << "% TPFA " << finl;
  Matrix_tools::allocate_morse_matrix(N * (nf_tot + ne_tot * D), N * (nf_tot + ne_tot * D), stencil, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
void Op_Diff_CoviMAC_Face::ajouter_blocs(matrices_t matrices, DoubleTab& resu, const tabs_t& semi_impl) const
{
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  Matrice_Morse *mat = matrices.count(nom_inco) && !semi_impl.count(nom_inco) ? matrices[nom_inco] : NULL; //facultatif
  const DoubleTab& inco = semi_impl.count(nom_inco) ? semi_impl.at(nom_inco) : equation().inconnue().valeurs();

  update_nu();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const ArrOfInt& i_bord = zone.ind_faces_virt_bord();
  const Conds_lim& cls = la_zcl_poly_->les_conditions_limites();
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces();
  const DoubleVect& fs = zone.face_surfaces(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes(), &pf = porosite_f, &pe = porosite_e;
  const DoubleTab& nf = zone.face_normales(), &xv = zone.xv(), &mu_f = ref_cast(Op_Grad_CoviMAC_Face, ref_cast(Navier_Stokes_std, equation()).operateur_gradient().valeur()).mu_f();
  int i, j, k, e, eb, f, fb, fc, fd, n, N = inco.line_size(), d, D = dimension, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot();
  double mult, t = equation().schema_temps().temps_courant(), f_eps;

  /* faces de bord : flux a deux points + valeurs aux bord */
  DoubleTrav vb(zone.nb_faces_tot(), D, N), dvb(zone.nb_faces_tot(), D, N), h_int(N), coeff(N);
  for (f = 0; f < zone.nb_faces_tot(); f++) if (ch.fcl(f, 0) > 2) //Dirichlet : remplissage de vb et contrib a la matrice
      {
        /* h_int : coefficent d'echange element-face */
        for (e = f_e(f, 0), fb = f < zone.nb_faces() ? f : i_bord[f - zone.nb_faces()], n = 0; n < N; n++)
          h_int(n) = zone.nu_dot(&nu_bord_, fb, n, N, &nf(f, 0), &nf(f, 0)) / (zone.dist_norm_bord(f) * fs(f) * fs(f));

        /* vb : seulement si Dirichlet non homogene */
        if (ch.fcl(f, 0) == 3)
          {
            const Champ_front_base& cfb = ref_cast(Dirichlet, cls[ch.fcl(f, 1)].valeur()).champ_front().valeur();
            const Champ_front_var_instationnaire *cfv = sub_type(Champ_front_var_instationnaire, cfb) ? &ref_cast(Champ_front_var_instationnaire, cfb) : NULL;
            for (d = 0; d < D; d++) for (n = 0; n < N; n++)
                vb(f, d, n) = !cfv || !cfv->valeur_au_temps_et_au_point_disponible() ? ref_cast(Dirichlet, cls[ch.fcl(f, 1)].valeur()).val_imp(ch.fcl(f, 2), N * d + n)
                              : cfv->valeur_au_temps_et_au_point(t, -1, phif_xb(f, n, 0), phif_xb(f, n, 1), D < 3 ? 0 : phif_xb(f, n, 2), N * d + n);
          }

        /* contributions */
        for (i = 0; i < e_f.dimension(1) && (fc = e_f(e, i)) >= 0; i++) if (fc < zone.nb_faces() && ch.fcl(fc, 0) < 2) for (n = 0; n < N; n++) //face->face
              {
                double dist_f = max(zone.dist_norm_bord(f), dabs(zone.dot(&xv(fc, 0), &nf(f, 0), &xv(f, 0))) / fs(f)); //fb peut etre plus loin du bord que e
                double fac = mu_f(fc, n, e != f_e(fc, 0)) * zone.nu_dot(&nu_bord_, fb, n, N, &nf(f, 0), &nf(f, 0)) / (fs(f) * dist_f) * vf(fc) / ve(e);
                resu.addr()[N * fc + n] -= fac * inco.addr()[N * fc + n];
                if (mat) (*mat)(N * fc + n, N * fc + n) += fac;
                for (d = 0; d < D; d++) resu.addr()[N * fc + n] += fac * nf(fc, d) / fs(fc) * vb(f, d, n);
              }
        if (e < zone.nb_elem()) for (d = 0; d < D; d++) for (n = 0; n < N; n++) //elem->elem (+ flux_bords si face reelle)
              {
                resu.addr()[N * (nf_tot + D * e + d) + n] -= fs(f) * h_int(n) * (inco.addr()[N * (nf_tot + D * e + d) + n] - vb(f, d, n));
                if (mat) (*mat)(N * (nf_tot + D * e + d) + n, N * (nf_tot + D * e + d) + n) += fs(f) * h_int(n);
              }
        if (f < zone.premiere_face_int()) for (d = 0; d < D; d++) for (n = 0; n < N; n++)
              flux_bords_(f, N * d + n) = - fs(f) * h_int(n) * (inco.addr()[N * (nf_tot + D * e + d) + n] - vb(f, d, n));
      }
    else if (ch.fcl(f, 0)) for (d = 0; d < D; d++) for (e = f_e(f, 0), n = 0; n < N; n++) //Neumann/Symetrie : vb / dvb seulement
          vb(f, d, n) = inco.addr()[N * (nf_tot + D * e + d) + n], dvb(f, d, n) = 1;

  /* faces internes : interpolation -> flux amont/aval -> combinaison convexe */
  for (f = 0; f < zone.nb_faces_tot(); f++) if (f_e(f, 0) >= 0 && f_e(f, 1) >= 0) for (f_eps = min(vf(f) / fs(f), eps), i = 0; i < 2; i++) for (e = f_e(f, i), j = phif_d(f); j < phif_d(f + 1); j++)
          {
            //elem -> face
            for (n = 0; n < N; n++) coeff(n) = (i ? 1 : -1) * fs(f) * (phif_w(f, n, 0) * phif_c(j, n, 0) + phif_w(f, n, 1) * phif_c(j, n, 1)); //|f|[nu grad v]_f
            for (eb = phif_j(j), k = 0; k < e_f.dimension(1) && (fb = e_f(e, k)) >= 0; k++) if (ch.fcl(fb, 0) < 2 && fb < zone.nb_faces())
                {
                  if (phif_d(f + 1) == phif_d(f) + 2 && (fc = zone.equiv(f, i, k)) >= 0) //equivalence : face-face
                    for (fd = (j == i + phif_d(f) ? fb : fc), mult = pf(fd) / pe(f_e(f, j - phif_d(f))) * (zone.dot(&nf(fb, 0), &nf(fd, 0)) > 0 ? 1 : -1), n = 0; n < N; n++)
                      {
                        double fac = mu_f(fb, n, e != f_e(fb, 0)) * vf(fb) / ve(e) * mult * coeff(n);
                        resu.addr()[N * fb + n] -= fac * inco.addr()[N * fd + n];
                        if (mat && ch.fcl(fd, 0) < 2) (*mat)(N * fb + n, N * fd + n) += fac;
                      }
                  else
                    {
                      for (d = 0; d < D; d++) if (dabs(nf(fb, d)) > 1e-6 * fs(fb)) for (n = 0; n < N; n++) //pas d'equivalence: elem -> face
                            {
                              double fac = mu_f(fb, n, e != f_e(fb, 0)) * vf(fb) / ve(e) * nf(fb, d) / fs(fb) * coeff(n) * (eb == e ? 1 - f_eps : 1);
                              resu.addr()[N * fb + n] -= fac * (eb < ne_tot ? inco.addr()[N * (nf_tot + D * eb + d) + n] : vb(eb - ne_tot, d, n));
                              if (mat && (eb < ne_tot || dvb(eb - ne_tot, d, n))) (*mat)(N * fb + n, N * (nf_tot + D * (eb < ne_tot ? eb : f_e(eb - ne_tot, 0)) + d) + n) += fac * (eb < ne_tot ? 1 : dvb(eb - ne_tot, d, n));
                            }
                      if (eb == e) for (n = 0; n < N; n++)
                          {
                            double fac = mu_f(fb, n, e != f_e(fb, 0)) * vf(fb) / ve(e) * coeff(n) * f_eps;
                            resu.addr()[N * fb + n] -= fac * inco.addr()[N * fb + n];
                            if (mat) (*mat)(N * fb + n, N * fb + n) += fac;
                          }
                    }
                }

            //elem -> elem
            if (e < zone.nb_elem()) for (d = 0; d < D; d++) for (n = 0; n < N; n++)
                  {
                    resu.addr()[N * (nf_tot + D * e + d) + n] -= coeff(n) * (eb < ne_tot ? inco.addr()[N * (nf_tot + D * eb + d) + n] : vb(eb - ne_tot, d, n));
                    if (mat && (eb < ne_tot || dvb(eb - ne_tot, d, n))) (*mat)(N * (nf_tot + D * e + d) + n, N * (nf_tot + D * (eb < ne_tot ? eb : f_e(eb - ne_tot, 0)) + d) + n) += coeff(n) * (eb < ne_tot ? 1 : dvb(eb - ne_tot, d, n));
                  }
          }
}
