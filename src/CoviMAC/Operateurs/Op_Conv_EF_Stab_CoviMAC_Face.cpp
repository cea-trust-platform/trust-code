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
// File:        Op_Conv_EF_Stab_CoviMAC_Face.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Conv_EF_Stab_CoviMAC_Face.h>
#include <Pb_Multiphase.h>
#include <Schema_Temps_base.h>
#include <Zone_CoviMAC.h>
#include <Zone_Cl_CoviMAC.h>
#include <IntLists.h>
#include <DoubleLists.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Champ_Face_CoviMAC.h>
#include <Op_Grad_CoviMAC_Face.h>
#include <Param.h>
#include <cmath>
#include <Masse_ajoutee_base.h>

Implemente_instanciable( Op_Conv_EF_Stab_CoviMAC_Face, "Op_Conv_EF_Stab_CoviMAC_Face_CoviMAC", Op_Conv_CoviMAC_base ) ;
Implemente_instanciable( Op_Conv_Amont_CoviMAC_Face, "Op_Conv_Amont_CoviMAC_Face_CoviMAC", Op_Conv_EF_Stab_CoviMAC_Face ) ;
Implemente_instanciable( Op_Conv_Centre_CoviMAC_Face, "Op_Conv_Centre_CoviMAC_Face_CoviMAC", Op_Conv_EF_Stab_CoviMAC_Face ) ;

// XD Op_Conv_EF_Stab_CoviMAC_Face interprete Op_Conv_EF_Stab_CoviMAC_Face 1 Class Op_Conv_EF_Stab_CoviMAC_Face
Sortie& Op_Conv_EF_Stab_CoviMAC_Face::printOn( Sortie& os ) const
{
  Op_Conv_CoviMAC_base::printOn( os );
  return os;
}

Sortie& Op_Conv_Amont_CoviMAC_Face::printOn( Sortie& os ) const
{
  Op_Conv_CoviMAC_base::printOn( os );
  return os;
}

Sortie& Op_Conv_Centre_CoviMAC_Face::printOn( Sortie& os ) const
{
  Op_Conv_CoviMAC_base::printOn( os );
  return os;
}

Entree& Op_Conv_EF_Stab_CoviMAC_Face::readOn( Entree& is )
{
  Op_Conv_CoviMAC_base::readOn( is );
  Param param(que_suis_je());
  param.ajouter("alpha", &alpha);            // XD_ADD_P double parametre ajustant la stabilisation de 0 (schema centre) a 1 (schema amont)
  param.lire_avec_accolades_depuis(is);
  return is;
}

Entree& Op_Conv_Amont_CoviMAC_Face::readOn( Entree& is )
{
  Op_Conv_CoviMAC_base::readOn( is );
  alpha = 1;
  return is;
}

Entree& Op_Conv_Centre_CoviMAC_Face::readOn( Entree& is )
{
  Op_Conv_CoviMAC_base::readOn( is );
  alpha = 0;
  return is;
}

void Op_Conv_EF_Stab_CoviMAC_Face::completer()
{
  Op_Conv_CoviMAC_base::completer();
  /* au cas ou... */
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  zone.init_equiv();

  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 2)
    Cerr << "Op_Conv_EF_Stab_CoviMAC_Face : largeur de joint insuffisante (minimum 2)!" << finl, Process::exit();
  porosite_f.ref(zone.porosite_face());
  porosite_e.ref(zone.porosite_elem());
}

double Op_Conv_EF_Stab_CoviMAC_Face::calculer_dt_stab() const
{
  double dt = 1e10;
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const DoubleVect& fs = zone.face_surfaces(), &pf = zone.porosite_face(), &ve = zone.volumes(), &pe = zone.porosite_elem(), &vf = zone.volumes_entrelaces();
  const DoubleTab& vit = vitesse_->valeurs(), &mu_f = ref_cast(Op_Grad_CoviMAC_Face, ref_cast(Navier_Stokes_std, equation()).operateur_gradient().valeur()).mu_f(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe() : NULL;
  const IntTab& e_f = zone.elem_faces(), &f_e = zone.face_voisins(), &fcl = ch.fcl();
  int i, e, f, n, N = vit.line_size();
  DoubleTrav flux(N), vol(N); //somme des flux pf * |f| * vf, volume minimal des mailles d'elements/faces affectes par ce flux

  for (e = 0; e < zone.nb_elem(); e++)
    {
      for (vol = pe(e) * ve(e), flux = 0, i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) for (n = 0; n < N; n++)
          {
            flux(n) += pf(f) * fs(f) * dabs(vit(f, n));
            if (0 && fcl(f, 0) < 2) vol(n) = min(vol(n), pf(f) * vf(f) / mu_f(f, n, e != f_e(f, 0))); //prise en compte de la contribution aux faces
          }
      for (n = 0; n < N; n++) if ((!alp || (*alp)(e, n) > 1e-3) && flux(n)) dt = min(dt, vol(n) / flux(n));
    }

  return Process::mp_min(dt);
}

void Op_Conv_EF_Stab_CoviMAC_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces(), &fcl = ch.fcl();
  const DoubleTab& nf = zone.face_normales();
  const DoubleVect& fs = zone.face_surfaces();

  const std::string& nom_inco = ch.le_nom().getString();
  if (!matrices.count(nom_inco) || semi_impl.count(nom_inco)) return; //pas de bloc diagonal ou semi-implicite -> rien a faire
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : NULL;
  const Masse_ajoutee_base *corr = pbm && pbm->has_correlation("masse_ajoutee") ? &ref_cast(Masse_ajoutee_base, pbm->get_correlation("masse_ajoutee").valeur()) : NULL;
  Matrice_Morse& mat = *matrices.at(nom_inco), mat2;

  int i, j, k, e, eb, f, fb, fc, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), m, n, N = equation().inconnue().valeurs().line_size(), d, D = dimension;

  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);

  /* agit uniquement aux elements; diagonale omise */
  for (f = 0; f < zone.nb_faces_tot(); f++) if (f_e(f, 0) >= 0 && (f_e(f, 1) >= 0 || fcl(f, 0) == 3))
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) for (j = 0; j < 2 && (eb = f_e(f, j)) >= 0; j++)
          {
            for (k = 0; k < e_f.dimension(1) && (fb = e_f(e, k)) >= 0; k++) if (fb < zone.nb_faces() && fcl(fb, 0) < 2)
                {
                  if ((fc = zone.equiv(f, i, k)) >= 0) //equivalence : face -> face
                    for (n = 0; fcl(fc, 0) < 2 && n < N; n++) for (m = (corr ? 0 : n); m < (corr ? N : n + 1); m++) stencil.append_line(N * fb + n, N * fc + m);
                  else if (f_e(f, 1) >= 0) for (d = 0; d < D; d++) //pas d'equivalence : elem -> face
                      if(dabs(nf(fb, d)) > 1e-6 * fs(fb)) for (n = 0; n < N; n++) for (m = (corr ? 0 : n); m < (corr ? N : n + 1); m++)
                            stencil.append_line(N * fb + n, N * (nf_tot + D * eb + d) + m); //elem -> face
                }
            for (d = 0; d < D; d++) for (n = 0; n < N; n++) for (m = (corr ? 0 : n); m < (corr ? N : n + 1); m++)  //elem->elem
                  stencil.append_line(N * (nf_tot + D * e + d) + n, N * (nf_tot + D * eb + d) + m);
          }

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(N * (nf_tot + D * ne_tot), N * (nf_tot + D * ne_tot), stencil, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
void Op_Conv_EF_Stab_CoviMAC_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const Conds_lim& cls = la_zcl_poly_.valeur().les_conditions_limites();
  const IntTab& f_e = zone.face_voisins(), &e_f = zone.elem_faces(), &fcl = ch.fcl();
  const DoubleTab& vit = ch.passe(), &nf = zone.face_normales(), &mu_f = ref_cast(Op_Grad_CoviMAC_Face, ref_cast(Navier_Stokes_std, equation()).operateur_gradient().valeur()).mu_f();
  const DoubleVect& fs = zone.face_surfaces(), &pe = porosite_e, &pf = porosite_f, &vf = zone.volumes_entrelaces(), &ve = zone.volumes();

  /* a_r : produit alpha_rho si Pb_Multiphase -> par semi_implicite, ou en recuperant le champ_conserve de l'equation de masse */
  const std::string& nom_inco = ch.le_nom().getString();
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : NULL;
  const Masse_ajoutee_base *corr = pbm && pbm->has_correlation("masse_ajoutee") ? &ref_cast(Masse_ajoutee_base, pbm->get_correlation("masse_ajoutee").valeur()) : NULL;
  const DoubleTab& inco = semi_impl.count(nom_inco) ? semi_impl.at(nom_inco) : ch.valeurs(),
                   *a_r = !pbm ? NULL : semi_impl.count("alpha_rho") ? &semi_impl.at("alpha_rho") : &pbm->eq_masse.champ_conserve().valeurs(),
                    *alp = pbm ? &pbm->eq_masse.inconnue().passe() : NULL, &rho = equation().milieu().masse_volumique().passe();
  Matrice_Morse *mat = matrices.count(nom_inco) && !semi_impl.count(nom_inco) ? matrices.at(nom_inco) : NULL;

  int i, j, k, e, eb, f, fb, fc, fd, nf_tot = zone.nb_faces_tot(), m, n, N = inco.line_size(), d, D = dimension, comp = !incompressible_;
  double mult;

  DoubleTrav dfac(2, N, N), masse(N, N);
  for (f = 0; f < zone.nb_faces_tot(); f++) if (f_e(f, 0) >= 0 && (f_e(f, 1) >= 0 || fcl(f, 0) == 1 || fcl(f, 0) == 3))
      {
        for (i = 0, dfac = 0; i < 2; i++)
          {
            //masse : diagonale + masse ajoutee si correlation
            for (masse = 0, e = f_e(f, f_e(f, i) >= 0 ? i : 0), n = 0; n < N; n++) masse(n, n) = a_r ? (*a_r)(e, n) : 1;
            if (corr) corr->ajouter(&(*alp)(e, 0), &rho(e, 0), masse);
            //contribution a dfac
            for (e = f_e(f, i), eb = f_e(f, i), n = 0; n < N; n++) for (m = 0; m < N; m++)
                dfac(fcl(f, 0) == 1 ? 0 : i, n, m) += fs(f) * vit(f, m) * pe(eb >= 0 ? eb : f_e(f, 0)) * masse(n, m)
                                                      * (1. + (vit(f, m) * (i ? -1 : 1) >= 0 ? 1. : vit(f, m) ? -1. : 0.) * alpha) / 2;
          }
        for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
          {
            for (k = 0; k < e_f.dimension(1) && (fb = e_f(e, k)) >= 0; k++) if (fb < zone.nb_faces() && fcl(fb, 0) < 2) //partie "faces"
                {
                  if ((fc = zone.equiv(f, i, k)) >= 0 || f_e(f, 1) < 0) for (j = 0; j < 2; j++) //equivalence : face fd -> face fb
                      {
                        eb = f_e(f, j), fd = (j == i ? fb : fc); //element/face sources
                        mult = (fd < 0 || zone.dot(&nf(fb, 0), &nf(fd, 0)) > 0 ? 1 : -1) * (fd >= 0 ? pf(fd) / pe(eb) : 1); //multiplicateur pour passer de vf a ve
                        for (n = 0; n < N; n++) for (m = 0; m < N; m++) if (dfac(j, n, m))
                              {
                                double fac = (i ? -1 : 1) * mu_f(fb, n, e != f_e(fb, 0)) * vf(fb) * dfac(j, n, m) / ve(e);
                                if (fd >= 0) secmem(fb, n) -= fac * mult * inco(fd, m); //autre face calculee
                                else for (d = 0; d < D; d++)  //CL de Dirichlet
                                    secmem(fb, n) -= fac * nf(fb, d) / fs(fb) * ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), N * d + m);
                                if (comp) secmem(fb, n) += fac * inco(fb, m); //partie v div(alpha rho v)
                                if (!mat) continue;
                                if (fd >= 0 && fcl(fd, 0) < 2) (*mat)(N * fb + n, N * fd + m) += fac * mult;
                                if (comp) (*mat)(N * fb + n, N * fb + m) -= fac;
                              }
                      }
                  else for (j = 0; j < 2; j++) for (eb = f_e(f, j), d = 0; d < D; d++) if (dabs(nf(fb, d)) > 1e-6 * fs(fb)) for (n = 0; n < N; n++) for (m = 0; m < N; m++) if (dfac(j, n, m))
                                {
                                  //pas d'equivalence : mu_f * n_f * operateur aux elements
                                  double fac = (i ? -1 : 1) * mu_f(fb, n, e != f_e(fb, 0)) * vf(fb) * dfac(j, n, m) / ve(e) * nf(fb, d) / fs(fb);
                                  secmem(fb, n) -= fac * inco(nf_tot + D * eb + d, m);
                                  if (comp) secmem(fb, n) += fac * inco(nf_tot + D * e + d, m);
                                  if (!mat || !fac) continue;
                                  (*mat)(N * fb + n, N * (nf_tot + D * eb + d) + m) += fac;
                                  if (comp) (*mat)(N * fb + n, N * (nf_tot + D * e + d) + m) -= fac;
                                }
                }
            for (j = 0; j < 2; j++) for (eb = f_e(f, j), d = 0; d < D; d++) for (n = 0; n < N; n++) for (m = 0; m < N; m++) if (dfac(j, n, m)) //partie "elem"
                      {
                        double fac = (i ? -1 : 1) * dfac(j, n, m);
                        secmem(nf_tot + D * e + d, n) -= fac * (eb >= 0 ? inco(nf_tot + D * eb + d, m)
                                                                : ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), N * d + m));
                        if (comp) secmem(nf_tot + D * e + d, n) += fac * inco(nf_tot + D * e + d, m); //partie v div(alpha rho v)
                        if (!mat) continue;
                        if (eb >= 0) (*mat)(N * (nf_tot + D * e + d) + n, N * (nf_tot + D * eb + d) + m) += fac;
                        if (comp) (*mat)(N * (nf_tot + D * e + d) + n, N * (nf_tot + D * e + d) + m) -= fac;
                      }
          }
      }
}
