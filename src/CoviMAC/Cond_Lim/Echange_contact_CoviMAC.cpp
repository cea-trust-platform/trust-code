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
// File:        Echange_contact_CoviMAC.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Cond_Lim
// Version:     /main/32
//
//////////////////////////////////////////////////////////////////////////////

#include <Echange_contact_CoviMAC.h>
#include <Champ_front_calc.h>
#include <Probleme_base.h>
#include <Champ_Uniforme.h>
#include <Schema_Euler_Implicite.h>
#include <Milieu_base.h>

#include <Zone_CoviMAC.h>
#include <Equation_base.h>
#include <Champ_P0_CoviMAC.h>
#include <Operateur.h>
#include <Op_Diff_CoviMAC_Elem.h>
#include <Front_VF.h>

#include <cmath>

Implemente_instanciable(Echange_contact_CoviMAC,"Paroi_Echange_contact_CoviMAC",Echange_externe_impose);
using namespace MEDCoupling;

Sortie& Echange_contact_CoviMAC::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Echange_contact_CoviMAC::readOn(Entree& s )
{
  Nom nom_bord;
  Motcle nom_champ;
  s >> nom_autre_pb_ >> nom_bord_ >> nom_champ_ >> invh_paroi_;
  invh_paroi_ = invh_paroi_ > 1e8 ? 0 : 1. / invh_paroi_;
  T_ext().typer("Ch_front_var_instationnaire_dep");
  T_ext()->fixer_nb_comp(1);
  return s;
}

int Echange_contact_CoviMAC::initialiser(double temps)
{
  Echange_externe_impose::completer();
  Champ_front_calc ch;
  ch.creer(nom_autre_pb_, nom_bord_, nom_champ_);
  fvf = ref_cast(Front_VF, frontiere_dis()), o_fvf = ref_cast(Front_VF, ch.front_dis()); //frontieres
  const Equation_base& eqn = zone_Cl_dis().equation(), &o_eqn = ch.equation(); //equations
  i_fvf = eqn.zone_dis()->rang_frontiere(fvf.le_nom()), i_o_fvf = o_eqn.zone_dis()->rang_frontiere(nom_bord_);

  int i_op = -1, o_i_op = -1, i; //indice de l'operateur de diffusion dans l'autre equation
  for (i = 0; i < eqn.nombre_d_operateurs(); i++)
    if (sub_type(Op_Diff_CoviMAC_base, eqn.operateur(i).l_op_base())) i_op = i;
  if (i_op < 0) Process::exit(le_nom() + " : no diffusion operator found in " + eqn.probleme().le_nom() + " !");
  for (i = 0; i < o_eqn.nombre_d_operateurs(); i++)
    if (sub_type(Op_Diff_CoviMAC_base, o_eqn.operateur(i).l_op_base())) o_i_op = i;
  if (o_i_op < 0) Process::exit(le_nom() + " : no diffusion operator found in " + o_eqn.probleme().le_nom() + " !");
  diff = ref_cast(Op_Diff_CoviMAC_base, eqn.operateur(i_op).l_op_base());
  o_diff = ref_cast(Op_Diff_CoviMAC_base, o_eqn.operateur(o_i_op).l_op_base());

  //controle du nombre de faces
  int nf_tot = fvf->nb_faces_tot(), diff_nf = Process::mp_max(abs(nf_tot - o_fvf->nb_faces_tot()));
  if (diff_nf) Process::exit(le_nom() + " : inconsistent face count with " + o_fvf->le_nom() + " (max " + Nom(diff_nf) + " )!");
  fe_init_ = 0;
  return 1;
}

/* identification des elements / faces de l'autre cote de la frontiere, avec offsets */
void Echange_contact_CoviMAC::init_fe_dist() const
{
  if (fe_init_) return; //deja fait
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC, fvf->zone_dis()), &o_zone = ref_cast(Zone_CoviMAC, o_fvf->zone_dis());
  const IntTab& o_f_e = o_zone.face_voisins();
  const DoubleTab& xv = zone.xv(), &o_xv = o_zone.xv();

  int i, f, nf_tot = o_fvf->nb_faces_tot(), d, D = dimension;
  fe_dist_.resize(nf_tot, 2);

  DoubleTrav xyz(nf_tot, D), o_xyz(nf_tot, D); //positions locales/distantes -> pour calcul de correspondance
  for (i = 0; i < nf_tot; i++) for (d = 0; d < D; d++)   xyz(i, d) =   xv(  fvf->num_face(i), d);
  for (i = 0; i < nf_tot; i++) for (d = 0; d < D; d++) o_xyz(i, d) = o_xv(o_fvf->num_face(i), d);
#ifdef MEDCOUPLING_
  MCAuto<DataArrayDouble> dad(DataArrayDouble::New()), o_dad(DataArrayDouble::New());
  dad->useExternalArrayWithRWAccess(xyz.addr(), nf_tot, D), o_dad->useExternalArrayWithRWAccess(o_xyz.addr(), nf_tot, D);
  //point de o_xyz le plus proche de chaque point de xyz
  MCAuto<DataArrayInt> idx(o_dad->findClosestTupleId(dad));

  for (i = 0; i < nf_tot; i++) //remplissage : (face distante, elem distant)
    f = o_fvf->num_face(idx->getIJ(i, 0)), fe_dist_(i, 0) = f, fe_dist_(i, 1) = o_f_e(f, 0);
#else
  Process::exit("Echange_contact_CoviMAC : MEDCoupling is required!");
#endif
  fe_init_ = 1;
}

void Echange_contact_CoviMAC::harmonic_points(DoubleTab& xh, DoubleTab& whm) const
{
  int i, f, o_f, fb, o_fb, e, o_e, nf_tot = fvf->nb_faces_tot(), d, D = dimension, i_mono, n, m,
                                   N = diff->equation().inconnue().valeurs().line_size(),
                                   oN = o_diff->equation().inconnue().valeurs().line_size(), M = max(N, oN);
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC, fvf->zone_dis()), &o_zone = ref_cast(Zone_CoviMAC, o_fvf->zone_dis());
  const IntTab& f_e = zone.face_voisins(), &pe_ext = diff->pe_ext, &fe_d = fe_dist();
  const DoubleTab& xv = zone.xv(), &o_xv = o_zone.xv(), &xp = zone.xp(), &o_xp = o_zone.xp(), &nf = zone.face_normales(),
                   &nu = diff->nu(), &invh =  diff->invh(), &o_nu = o_diff->nu(), &o_invh = o_diff->invh();
  const DoubleVect& fs = zone.face_surfaces();
  DoubleTrav def(2), xef(2, D), lambda(2, M), lambda_t(2, M, D), xh_c(D), wh_c(2, M);
  double i3[3][3] = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }}, den;

  for (i = 0; i < nf_tot; i++) if ((o_f = fe_d(i, 0)) >= 0) /* il faut que la face / element de l'autre cote soient presents */
      {
        f = fvf->num_face(i), e = f_e(f, 0), o_e = fe_d(i, 1), fb = zone.fbord(f), o_fb = o_zone.fbord(o_f), i_mono = pe_ext(fb, 2);
        //distances / projections
        for (def(0) = zone.dot(&xv(f, 0), &nf(f, 0), &xp(e, 0)) / fs(f), d = 0; d < D; d++)
          xef(0, d) = xp(e, d) + def(0) * nf(f, d) / fs(f);
        for (def(1) = - zone.dot(&o_xv(o_f, 0), &nf(f, 0), &o_xp(o_e, 0)) / fs(f), d = 0; d < D; d++)
          xef(1, d) = o_xp(o_e, d) - def(1) * nf(f, d) / fs(f);
        //diffusivites de chaque cote : partie normale, partie transverse
        for (n = 0; n <  N; n++) for (lambda(0, n) = zone.nu_dot(&nu, e, n, &nf(f, 0), &nf(f, 0)) / (fs(f) * fs(f)), d = 0; d < D; d++)
            lambda_t(0, n, d) = (zone.nu_dot(&nu, e, n, i3[d], &nf(f, 0)) - lambda(0, n) * nf(f, d)) / fs(f);
        for (n = 0; n < oN; n++) for (lambda(1, n) = zone.nu_dot(&o_nu, o_e, n, &nf(f, 0), &nf(f, 0)) / (fs(f) * fs(f)), d = 0; d < D; d++)
            lambda_t(1, n, d) = (zone.nu_dot(&o_nu, o_e, n, i3[d], &nf(f, 0)) - lambda(1, n) * nf(f, d)) / fs(f);

        //xh_c / wh_c : point harmonique commun, correspondant a la temperature de paroi du cote du pb courant
        den = 0, xh_c = 0, wh_c = 0; //denominateur des expressions, position, poids des temperatures amont/aval
        for (n = 0; n <  N; n++) for (den += (wh_c(0, n) = lambda(0, n) / (def(0) + invh(fb, n) * lambda(0, n))), d = 0; d < D; d++)
            xh_c(d) += (lambda(0, n) * xef(0, d) + def(0) * lambda_t(0, n, d)) / (def(0) + invh(fb, n) * lambda(0, n));
        for (n = 0; n < oN; n++) for (den += (wh_c(1, n) = lambda(1, n) / (def(1) + (o_invh(o_fb, n) + invh_paroi_) * lambda(1, n))), d = 0; d < D; d++)
            xh_c(d) += (lambda(1, n) * xef(1, d) - def(1) * lambda_t(1, n, d)) / (def(1) + (o_invh(o_fb, n) + invh_paroi_) * lambda(1, n));
        xh_c /= den, wh_c /= den; //normalisation

        for (n = 0; n < N; n++) for (d = 0; d < D; d++) //position du point pour chaque composante -> dans xh (tableau sur toutes les faces)
            xh(f, n, d) = (def(0) * xh_c(d) + invh(fb, n) * (lambda(0, n) * xef(0, d) + def(0) * lambda_t(0, n, d))) / (def(0) + invh(fb, n) * lambda(0, n));
        for (n = 0; n < N; n++) //poids de chaque cote -> dans whm
          {
            for (m = 0; m <  N; m++) whm(i_mono, n, m, 0) = (def(0) * wh_c(0, m) + (m == n) * invh(fb, n) * lambda(0, n)) / (def(0) + invh(fb, n) * lambda(0, n));
            for (m = 0; m < oN; m++) whm(i_mono, n, m, 1) = def(0) * wh_c(1, m) / (def(0) + invh(fb, n) * lambda(0, n));
          }
      }
}

void Echange_contact_CoviMAC::fgrad(DoubleTab& phif_w, IntTab& phif_d, IntTab& phif_e, DoubleTab& phif_c, IntTab& phif_pe, DoubleTab& phif_pc) const
{
  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC, fvf->zone_dis()), &o_zone = ref_cast(Zone_CoviMAC, o_fvf->zone_dis());
  int i_f, i, il, ic, j, k, l, f, o_f, fb, o_fb, e, o_e, e_s, f_s, f_sb, m, m_s, n, n_max, on_max, nw, infoo = 0;
  int d, D = dimension, ne_tot = zone.nb_elem_tot(), o_ne_tot = o_zone.nb_elem_tot();
  int N = phif_w.dimension(1), oN = o_diff->equation().inconnue().valeurs().line_size(), M = max(N, oN);
  const IntTab& f_e = zone.face_voisins(), &o_f_e = o_zone.face_voisins(), &fe_d = fe_dist(), &pe_ext = diff->pe_ext, &o_pe_ext = o_diff->pe_ext;
  const DoubleTab& xv = zone.xv(), &xp = zone.xp(), &o_xp = o_zone.xp(), &nf = zone.face_normales(), &o_nf = o_zone.face_normales(),
                   &nu = diff->nu(), &invh = diff->invh(), &o_nu = o_diff->nu(), &o_invh = o_diff->invh(), &xh = diff->xh(), &o_xh = o_diff->xh(),
                    &wh = diff->wh(), &o_wh = o_diff->wh(), &whm = diff->whm(), &o_whm = o_diff->whm();
  const DoubleVect& fs = zone.face_surfaces(), &o_fs = o_zone.face_surfaces();
  double i3[3][3] = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }};
  char trans = 'N';

  //traduction des indices de probleme dans o_diff en indices de problemes dans diff (avec extension de op_ext si necessaire)
  std::vector<int> trad;
  for (auto op : o_diff->op_ext)
    {
      trad.push_back(std::find(diff->op_ext.begin(), diff->op_ext.end(), op) - diff->op_ext.begin());
      if (trad.back() == (int) diff->op_ext.size()) diff->op_ext.push_back(op);
    }

  //verification : phif_d doit etre rempli jusqu'a la premiere face de la CL, on ne doit pas demander les faces virtuelles
  assert(phif_d.dimension(0) == fvf->num_face(0) + 1 && phif_w.dimension(0) == zone.nb_faces());

  std::vector<std::pair<int, int>> p_e; //stencil aux elemens : { { pb, elem/bord }, ... }
  DoubleTrav base, A , B, lambda(2, M), r_int(2, M), h(2, M), xef(2, N, D), interp, tphi, W(1); //base de champs possibles, systeme A.x = B,
  base.set_smart_resize(1), A.set_smart_resize(1), B.set_smart_resize(1), interp.set_smart_resize(1), tphi.set_smart_resize(1), W.set_smart_resize(1);
  for (i_f = 0; i_f < fvf->nb_faces(); i_f++, phif_d.append_line(phif_e.dimension(0), phif_pe.dimension(0)))
    {
      f = fvf->num_face(i_f), e = f_e(f, 0), o_f = fe_d(i_f, 0), o_e = fe_d(i_f, 1), fb = zone.fbord(f), o_fb = o_zone.fbord(o_f);

      const int *fa = &zone.fsten_f(zone.fsten_d(f)), *o_fa = &o_zone.fsten_f(o_zone.fsten_d(o_f)); //debuts des stencils de chaque cote
      int n_f = zone.fsten_d(f + 1) - zone.fsten_d(f), o_n_f = o_zone.fsten_d(o_f + 1) - o_zone.fsten_d(o_f), n_tf = n_f + o_n_f + 2,
          nrhs = N + oN, ng = D + N + oN - 2, nl = ng + 1, nc = N * (n_f + 1) + oN * (o_n_f + 1), nnz, hdiag, ok = 1;

      /* verification qu'on dispose de tous les points harmoniques dont on a besoin */
      for (i = 0; i <   n_f; i++) ok &= (  wh(  fa[i], 0) >= 0);
      for (i = 0; i < o_n_f; i++) ok &= (o_wh(o_fa[i], 0) >= 0);
      if (!ok) continue;

      double h_max[2] = { 0, }, h_tot[2] = { 0, }, dist[2] = { zone.dist_norm_bord(f), o_zone.dist_norm_bord(o_f) }; //coeff d'echange max de chaque cote, distances
      B.resize(nrhs, nc), A.resize(nc, nl);

      /* nf.lambda.nf de chaque cote + identification des phases dominantes (-> diffusant le plus) */
      const double *xe; //coordonnees de l'amont/aval
      for (i = 0; i < 2; i++) for (xe = i ? &o_xp(o_e, 0) : &xp(e, 0), n = 0; n < (i ? oN : N); n++)
          {
            lambda(i, n) = zone.nu_dot(i ? &o_nu : &nu, i ? o_e : e, n, &nf(f, 0), &nf(f, 0)) / (fs(f) * fs(f));
            r_int(i, n) = dist[i] / lambda(i, n);
            for (d = 0; d < D; d++) xef(i, n, d) = xe[d] + (i ? -1 : 1) * r_int(i, n) * zone.nu_dot(i ? &o_nu : &nu, i ? o_e : e, n, &nf(f, 0), i3[d]) / fs(f);
            h(i, n) = 1. / (r_int(i, n) + (i ? o_invh(o_fb, n): invh(fb, n)) + invh_paroi_ / 2), h_tot[i] += h(i, n);
            if (h(i, n) > h_max[i]) h_max[i] = h(i, n), *(i ? &on_max : &n_max) = n;
          }

      /* base de gradients amont/aval */
      //D premiers : diffusion dans les phases dominantes
      base.resize(ng, 2, M, D), base = 0;
      for (i = 0; i < D; i++) for (j = 0; j < 2; j++) for (d = 0; d < D; d++)
            base(i, j, j ? on_max : n_max, d) = i3[i][d] + (j ? -1 : 1) * (zone.nu_dot(&o_nu, o_e, on_max, &nf(f, 0), i3[i]) - zone.nu_dot(&nu, e, n_max, &nf(f, 0), i3[i]))
                                                * nf(f, d) / (2 * fs(f) * fs(f) * lambda(j, j ? on_max : n_max));
      //N + oN - 2 suivants : gradient normal dans la phase n (non dominante) d'un cote, compense par la phase dominante de l'autre
      for (i--, n = 0; n <  N; n++) if (n != n_max) for (i++, j = 0; j < 2; j++) for (d = 0; d < D; d++)
              base(i, j, j ? on_max : n, d) = nf(f, d) / fs(f) * (j ? lambda(0, n) / lambda(1, on_max) : 1);
      for (n = 0; n < oN; n++) if (n != on_max) for (i++, j = 0; j < 2; j++) for (d = 0; d < D; d++)
              base(i, j, j ? n : n_max, d)  = nf(f, d) / fs(f) * (j ? 1 : lambda(1, n) / lambda(0, n_max));

      /* seconds membres : pour les N + oN interpolations a calculer, une equation par gradient possible + une pour la constante */
      for (B = 0, i = 0, j = 0; i < 2; i++) for (n = 0; n < (i ? oN : N); n++, j++) for (k = 0; k < nl; k++)
            B(j, k) = k < ng ? zone.dot(&xef(i, n, 0), &base(k, i, n, 0), i ? &o_xh(o_f, n, 0) : &xh(f, n, 0)) : 0;

      /* poids par defaut : */
      interp.resize(2, M, n_tf, M); //ordre des colonnes : faces de zone.fsten_f -> xh local -> faces de o_zone.fsten_f -> xh distant
      for (interp = 0, i = 0; i < 2; i++) for (n = 0; n < (i ? oN : N); n++) interp(i, n, i ? n_tf - 1 : n_f, n) = 1;
      if (std::pow(1e6 * local_max_abs_vect(B), D - 1) > fs(f)) //pts projetes distants des pts harmoniques -> recherche d'interpolation
        {
          /* matrice */
          for (i = 0, ic = 0; i <= n_f; i++) for (n = 0; n < N; n++, ic++) for (il = 0; il <= ng; il++) //cote interne
                A(ic, il) = il < ng ? zone.dot(&xh(i < n_f ? fa[i] : f, n, 0), &base(il, 0, n, 0), &xv(f, 0))
                            + (invh(fb, n) + invh_paroi_ / 2) * zone.nu_dot(&nu, e, n, &nf(f, 0), &base(il, 0, n, 0)) / fs(f) : 1;
          for (i = 0; i <= n_f; i++) for (n = 0; n < oN; n++, ic++) for (il = 0; il <= ng; il++) //cote externe
                A(ic, il) = il < ng ? zone.dot(&o_xh(i < o_n_f ? o_fa[i] : o_f, n, 0), &base(il, 0, n, 0), &xv(f, 0))
                            - (o_invh(o_fb, n) + invh_paroi_ / 2) * zone.nu_dot(&o_nu, o_e, n, &nf(f, 0), &base(il, 0, n, 0)) / fs(f) : 1;

          /* resolution */
          nw = -1, F77NAME(dgels)(&trans, &nl, &nc, &nrhs, &A(0, 0), &nl, &B(0, 0), &nc, &W(0), &nw, &infoo);
          W.resize(nw = W(0)), F77NAME(dgels)(&trans, &nl, &nc, &nrhs, &A(0, 0), &nl, &B(0, 0), &nc, &W(0), &nw, &infoo);

          /* correction de interp (avec ecretage) */
          for (i = 0, j = 0; i < 2; i++) for (n = 0; n < (i ? oN : N); n++, j++) for (k = 0, il = 0; k < n_tf; k++)
                for (m = 0; m < (k > n_f ? oN : N); m++, il++) interp(i, n, k, m) += dabs(B(j, il)) > 1e-8 ? B(j, il) : 0;
        }

      /* stencil aux elements */
      for (p_e.clear(), i = 0; i < n_f; i++) for (f_s = fa[i], f_sb = zone.fbord(f_s), j = 0; j < 2; j++) //cote interne
          if ((e_s = f_e(f_s, j)) >= 0) p_e.push_back({ 0, e_s }); //element normal
          else if (f_sb >= 0 && (e_s = pe_ext(f_sb, 1)) >= 0) p_e.push_back({ pe_ext(f_sb, 0), e_s }); //de l'autre cote d'un Echange_contact
          else if (f_sb >= 0) p_e.push_back({ 0, ne_tot + f_s }); //CL non Echange_contact -> dependance en les valeurs aux bords
      for (i = 0; i < o_n_f; i++) for (f_s = o_fa[i], f_sb = o_zone.fbord(f_s), j = 0; j < 2; j++) //cote externe
          if ((e_s = o_f_e(f_s, j)) >= 0) p_e.push_back({ trad[0], e_s }); //element normal
          else if (f_sb >= 0 && (e_s = o_pe_ext(f_sb, 1)) >= 0) p_e.push_back({ trad[o_pe_ext(f_sb, 0)], e_s }); //de l'autre cote d'un Echange_contact
          else if (f_sb >= 0) p_e.push_back({ trad[0], o_ne_tot + f_s }); //CL non Echange_contact -> dependance en les valeurs aux bords
      std::sort(p_e.begin(), p_e.end()), p_e.erase(std::unique(p_e.begin(), p_e.end()), p_e.end()); //classement + deduplication

      /* tphi : parties amont/aval du flux total */
      tphi.resize(p_e.size(), M, 2), tphi = 0;
      //dependances points amont/aval
      for (i = 0; i < 2; i++) for (k = std::lower_bound(p_e.begin(), p_e.end(), std::make_pair(i ? trad[0] : 0, i ? o_e : e)) - p_e.begin(), n = 0; n < N; n++)
          tphi(k, n, i) += (i ? 1 : -1) / r_int(i, n);

      //dependances cote local
      for (i = 0; i <= n_f; i++) for (f_s = i < n_f ? fa[i] : f, f_sb = zone.fbord(f_s), j = 0; j < 2; j++)
          if ((e_s = f_e(f_s, j) >= 0 ? f_e(f_s, j) : f_sb >= 0 && pe_ext(f_sb, 0) >= 0 ? pe_ext(f_sb, 1) : -1) >= 0) //il existe un element source
            {
              k = std::lower_bound(p_e.begin(), p_e.end(), std::make_pair(f_e(f_s, j) >= 0 ? 0 : pe_ext(f_sb, 0), e_s)) - p_e.begin();
              if (f_sb < 0 || pe_ext(f_sb, 0) < 0) for (n = 0; n < N; n++) for (m = 0; m < M; m++) for (l = 0; l < 2; l++) //pt harmonique normal : dependance diagonale
                      tphi(k, m, l) += (l ? -1 : 1) / r_int(l, n) * interp(l, n, i, m) * (j ? 1 - wh(f_s, m) : wh(f_s, m));
              else for (n = 0; n < N; n++) for (m = 0; m < M; m++) for (m_s = 0; m_s < whm.dimension(2); m_s++) for (l = 0; l < 2; l++) //pt harmonique d'Echange_contact : melange...
                        tphi(k, m_s, l) += (l ? -1 : 1) / r_int(l, n) * interp(l, n, i, m) * whm(pe_ext(f_s, 2), m, m_s, j);
            }
          else for (k = std::find(p_e.begin(), p_e.end(), std::make_pair(0, ne_tot + f_s)) - p_e.begin(), n = 0; n < N; n++) //pas d'elem source -> dependance en la CL
              for (m = 0; m < M; m++) for (l = 0; l < 2; l++)
                  tphi(k, m, l) += (l ? -1 : 1) / r_int(l, n) * interp(l, n, i, m)
                                   * (wh(f_s, m) < 1 ? 1 - wh(f_s, m) : - zone.dist_norm_bord(f_s) * fs(f_s) * fs(f_s) / zone.nu_dot(&nu, f_e(f_s, 0), m, &nf(f_s, 0), &nf(f_s, 0)));

      //dependances cote distant
      for (i = n_f + 1; i < n_tf; i++) for (f_s = i < n_tf - 1 ? o_fa[i - (n_f + 1)] : o_f, f_sb = o_zone.fbord(f_s), j = 0; j < 2; j++)
          if ((e_s = o_f_e(f_s, j) >= 0 ? o_f_e(f_s, j) : f_sb >= 0 && o_pe_ext(f_sb, 0) >= 0 ? o_pe_ext(f_sb, 1) : -1) >= 0) //il existe un element source
            {
              k = std::lower_bound(p_e.begin(), p_e.end(), std::make_pair(trad[o_f_e(f_s, j) >= 0 ? 0 : o_pe_ext(f_sb, 0)], e_s)) - p_e.begin();
              if (f_sb < 0 || o_pe_ext(f_sb, 0) < 0) for (n = 0; n < oN; n++) for (m = 0; m < M; m++) for (l = 0; l < 2; l++) //pt harmonique normal : dependance diagonale
                      tphi(k, m, l) += (l ? -1 : 1) / r_int(l, n) * interp(l, n, i, m) * (j ? 1 - o_wh(f_s, m) : o_wh(f_s, m));
              else for (n = 0; n < oN; n++) for (m = 0; m < M; m++) for (m_s = 0; m_s < o_whm.dimension(2); m_s++) for (l = 0; l < 2; l++) //pt harmonique d'Echange_contact : melange...
                        tphi(k, m_s, l) += (l ? -1 : 1) / r_int(l, n) * interp(l, n, i, m) * o_whm(o_pe_ext(f_s, 2), m, m_s, j);
            }
          else for (k = std::find(p_e.begin(), p_e.end(), std::make_pair(trad[0], o_ne_tot + f_s)) - p_e.begin(), n = 0; n < oN; n++) //pas d'elem source -> dependance en la CL
              for (m = 0; m < M; m++) for (l = 0; l < 2; l++)
                  tphi(k, m, l) += (l ? -1 : 1) / r_int(l, n) * interp(l, n, i, m)
                                   * (o_wh(f_s, m) < 1 ? 1 - o_wh(f_s, m) : - o_zone.dist_norm_bord(f_s) * o_fs(f_s) * o_fs(f_s) / o_zone.nu_dot(&o_nu, o_f_e(f_s, 0), m, &o_nf(f_s, 0), &o_nf(f_s, 0)));

      /* remplissage */
      for (n = 0; n < N; n++) phif_w(f, n) = h_tot[1] / (h_tot[0] + h_tot[1]); //poids amont/aval : par le flux total
      for (i = 0; i < (int) p_e.size(); i++)
        {
          for (nnz = 0, hdiag = 0, n = 0; n < N; n++) for (m = 0; m < M; m++) for (j = 0; j < 2; j++) if (h(0, n) / h_tot[0] * tphi(i, m, j) != 0) nnz++, hdiag += (n != m);
          if (!nnz) continue; //on peut sauter cette dependance
          if (p_e[i].first == 0 && hdiag == 0) //coeffs diagonaux et probleme local -> phif_e / phif_c
            for (phif_e.append_line(p_e[i].second), phif_c.resize((k = phif_c.dimension(0)) + 1, N, 2), n = 0; n < N; n++) for (j = 0; j < 2; j++)
                phif_c(k, n, j) = h(0, n) / h_tot[0] * tphi(i, n, j);
          else for (phif_pe.append_line(p_e[i].first, p_e[i].second), phif_pc.resize((k = phif_pc.dimension(0)) + 1, N, M, 2), n = 0; n < N; n++) for (m = 0; m < M; m++) for (j = 0; j < 2; j++)
                  phif_pc(k, n, m, j) = h(0, n) / h_tot[0] * tphi(i, m, j); //sinon -> phif_pe / phif_pc
        }
    }
}
