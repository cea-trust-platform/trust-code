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

#include <Linear_algebra_tools_impl.h>
#include <Op_Grad_PolyMAC_Face.h>
#include <Masse_ajoutee_base.h>
#include <Masse_PolyMAC_Face.h>
#include <Dirichlet_homogene.h>
#include <Champ_Face_PolyMAC.h>
#include <Schema_Temps_base.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Champ_Uniforme.h>
#include <Equation_base.h>
#include <Pb_Multiphase.h>
#include <Domaine_PolyMAC.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <Matrice33.h>
#include <Dirichlet.h>
#include <Symetrie.h>
#include <Vecteur3.h>
#include <Debog.h>
#include <Piso.h>

Implemente_instanciable(Masse_PolyMAC_Face, "Masse_PolyMAC_Face", Masse_PolyMAC_base);

Sortie& Masse_PolyMAC_Face::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Masse_PolyMAC_Face::readOn(Entree& s) { return s ; }

void Masse_PolyMAC_Face::completer()
{
  Solveur_Masse_Face_proto::associer_masse_proto(*this,le_dom_PolyMAC.valeur());
}

// XXX : a voir si on peut utiliser Solveur_Masse_Face_proto::appliquer_impl_proto ...
DoubleTab& Masse_PolyMAC_Face::appliquer_impl(DoubleTab& sm) const
{
  const Domaine_PolyMAC& domaine = le_dom_PolyMAC.valeur();
  const IntTab& f_e = domaine.face_voisins();
  const DoubleVect& pf = equation().milieu().porosite_face();
  int i, e, f, n, N = equation().inconnue().valeurs().line_size();
  const DoubleTab *a_r = sub_type(QDM_Multiphase, equation()) ? &ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.champ_conserve().passe() : NULL,
                   &vfd = domaine.volumes_entrelaces_dir();
  double fac;

  //vitesses aux faces
  for (f = 0; f < domaine.nb_faces(); f++)
    for (n = 0; n < N; n++)
      {
        for (fac = 0, i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) fac += vfd(f, i) * (a_r ? (*a_r)(e, n) : 1);
        sm(f, n) /= pf(f) * fac; //vitesse calculee
      }

  sm.echange_espace_virtuel();
  return sm;
}

void Masse_PolyMAC_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  IntTrav sten(0, 2);
  sten.set_smart_resize(1);
  Solveur_Masse_Face_proto::dimensionner_blocs_proto(matrices, semi_impl, true /* allocate too */, sten);
}

void Masse_PolyMAC_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, double dt, const tabs_t& semi_impl, int resoudre_en_increments) const
{
  Solveur_Masse_Face_proto::ajouter_blocs_proto( matrices, secmem,  dt,semi_impl, resoudre_en_increments);
}

//sert a imposer les CLs de Dirichlet en multiphase (ou la variation de P_bord ne permet de corriger que v_melange)
DoubleTab& Masse_PolyMAC_Face::corriger_solution(DoubleTab& x, const DoubleTab& y, int incr) const
{
  const Domaine_PolyMAC& domaine = le_dom_PolyMAC.valeur();
  const Conds_lim& cls = le_dom_Cl_PolyMAC->les_conditions_limites();
  const IntTab& fcl = ref_cast(Champ_Face_PolyMAC, equation().inconnue().valeur()).fcl();
  const DoubleTab& nf = domaine.face_normales(), &vit = equation().inconnue().valeurs();
  const DoubleVect& fs = domaine.face_surfaces();
  int f, n, N = x.line_size(), d, D = dimension;

  for (f = 0; f < domaine.nb_faces_tot(); f++)
    if (fcl(f, 0) == 2 || fcl(f, 0) == 4)
      for (n = 0; n < N; n++) x(f, n) = incr ? -vit(f, n) : 0; //Dirichlet homogene / Symetrie: on revient a 0
    else if (fcl(f, 0) == 3)
      for (n = 0; n < N; n++)
        for (x(f, n) = incr ? -vit(f, n) : 0, d = 0; d < D; d++) //Dirichlet : valeur de la CL
          x(f, n) += nf(f, d) / fs(f) * ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), N * d + n);

  return x;
}
