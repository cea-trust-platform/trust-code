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

#include <Masse_PolyMAC_Elem.h>
#include <Domaine_Cl_PolyMAC.h>
#include <Champ_Elem_PolyMAC.h>
#include <Domaine_PolyMAC.h>
#include <TRUSTTab_parts.h>
#include <Equation_base.h>
#include <Neumann_paroi.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <Milieu_base.h>
#include <Conds_lim.h>

Implemente_instanciable(Masse_PolyMAC_Elem, "Masse_PolyMAC_Elem", Masse_PolyMAC_base);

Sortie& Masse_PolyMAC_Elem::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Masse_PolyMAC_Elem::readOn(Entree& s) { return s; }

//ne touche que la partie "elements"
DoubleTab& Masse_PolyMAC_Elem::appliquer_impl(DoubleTab& sm) const
{
  const Domaine_PolyMAC& domaine_PolyMAC = le_dom_PolyMAC.valeur();
  const DoubleVect& volumes = domaine_PolyMAC.volumes();
  const DoubleVect& porosite_elem = equation().milieu().porosite_elem();

  const int nb_elem = domaine_PolyMAC.nb_elem(), nb_dim = sm.nb_dim();
  if (nb_elem == 0)
    {
      sm.echange_espace_virtuel();
      return sm;
    }

  if (nb_dim == 2)
    {
      const int nb_comp = sm.line_size(); //sm.dimension_tot(0)/nb_elem;
      for (int num_elem = 0; num_elem < nb_elem; num_elem++)
        for (int k = 0; k < nb_comp; k++)
          sm(num_elem, k) /= (volumes(num_elem) * porosite_elem(num_elem));
    }
  else if (sm.nb_dim() == 3)
    {
      const int d1 = sm.dimension(1), d2 = sm.dimension(2);
      for (int num_elem = 0; num_elem < nb_elem; num_elem++)
        for (int k = 0; k < d1; k++)
          for (int d = 0; d < d2; d++)
            sm(num_elem, k, d) /= (volumes(num_elem) * porosite_elem(num_elem));
    }
  else
    {
      Cerr << "Masse_PolyMAC_Elem::appliquer ne peut pas s'appliquer a un DoubleTab a " << sm.nb_dim() << " dimensions" << endl;
      Process::exit();
    }
  sm.echange_espace_virtuel();
  return sm;
}

//Masse_PolyMAC_Elem est responsable des parties de la matrice n'impliquant pas la diffusion
void Masse_PolyMAC_Elem::dimensionner(Matrice_Morse& matrix) const
{
  const Domaine_PolyMAC& domaine = le_dom_PolyMAC.valeur();
  const Champ_Elem_PolyMAC& ch = ref_cast(Champ_Elem_PolyMAC, equation().inconnue());
  int e, f, ne_tot = domaine.nb_elem_tot(), nf_tot = domaine.nb_faces_tot(), n, N = ch.valeurs().line_size();
  const bool only_ne = (matrix.nb_lignes() == ne_tot);

  domaine.init_m2(), ch.fcl();
  IntTab indice(0, 2);

  //partie superieure : diagonale
  for (e = 0; e < domaine.nb_elem(); e++)
    for (n = 0; n < N; n++)
      indice.append_line(N * e + n, N * e + n);
  //partie inferieure : diagonale pour les CLs de Dirichlet
  for (f = 0; !only_ne && f < domaine.nb_faces(); f++)
    if (no_diff_ || ch.fcl()(f, 0) > 5)
      for (n = 0; n < N; n++)
        indice.append_line(N * (ne_tot + f) + n, N * (ne_tot + f) + n);

  tableau_trier_retirer_doublons(indice);
  Matrix_tools::allocate_morse_matrix(N * (ne_tot + !only_ne * nf_tot), N * (ne_tot + !only_ne * nf_tot), indice, matrix);
}

DoubleTab& Masse_PolyMAC_Elem::ajouter_masse(double dt, DoubleTab& secmem, const DoubleTab& inco, int penalisation) const
{
  const Domaine_PolyMAC& domaine = le_dom_PolyMAC.valeur();
  const Champ_Elem_PolyMAC& ch = ref_cast(Champ_Elem_PolyMAC, equation().inconnue());
  const Conds_lim& cls = le_dom_Cl_PolyMAC->les_conditions_limites();
  const DoubleVect& ve = domaine.volumes(), &pe = equation().milieu().porosite_elem(), &fs = domaine.face_surfaces();
  int e, f, ne_tot = domaine.nb_elem_tot(), n, N = inco.line_size();
  DoubleVect coef(equation().milieu().porosite_elem());
  coef = 1.;
  if (has_coefficient_temporel_) appliquer_coef(coef);

  ch.fcl();
  //partie superieure : diagonale
  for (e = 0; e < domaine.nb_elem(); e++)
    for (n = 0; n < N; n++)
      secmem(e, n) += coef(e) * pe(e) * ve(e) * inco(e, n) / dt;

  //partie inferieure : valeur imposee pour les CLs de Neumann / Dirichlet / Echange_Impose
  for (f = 0; secmem.dimension_tot(0) > ne_tot && f < domaine.nb_faces(); f++)
    if (ch.fcl()(f, 0) == 4)
      for (n = 0; n < N; n++) //Neumann_paroi
        secmem(N * (ne_tot + f) + n) -= fs(f) * ref_cast(Neumann_paroi, cls[ch.fcl()(f, 1)].valeur()).flux_impose(ch.fcl()(f, 2), n);
    else if (ch.fcl()(f, 0) == 6)
      for (n = 0; n < N; n++) //Dirichlet
        secmem(N * (ne_tot + f) + n) += ref_cast(Dirichlet, cls[ch.fcl()(f, 1)].valeur()).val_imp(ch.fcl()(f, 2), n);

  return secmem;
}

Matrice_Base& Masse_PolyMAC_Elem::ajouter_masse(double dt, Matrice_Base& matrice, int penalisation) const
{
  const Domaine_PolyMAC& domaine = le_dom_PolyMAC.valeur();
  const Champ_Elem_PolyMAC& ch = ref_cast(Champ_Elem_PolyMAC, equation().inconnue());
  const DoubleVect& ve = domaine.volumes(), &pe = equation().milieu().porosite_elem();
  int e, f, ne_tot = domaine.nb_elem_tot(), n, N = ch.valeurs().line_size();
  Matrice_Morse& mat = ref_cast(Matrice_Morse, matrice);
  DoubleVect coef(equation().milieu().porosite_elem());
  coef = 1.;
  if (has_coefficient_temporel_) appliquer_coef(coef);

  domaine.init_m2(), ch.fcl();
  //partie superieure : diagonale
  for (e = 0; e < domaine.nb_elem(); e++)
    for (n = 0; n < N; n++)
      mat(N * e + n, N * e + n) += coef(e) * pe(e) * ve(e) / dt; //diagonale

  //partie inferieure : 1 pour les flux imposes par CLs aux faces (si diffusion) ou pour toutes les faces (sinon)
  for (f = 0; mat.nb_lignes() > N * ne_tot && f < domaine.nb_faces(); f++)
    if (ch.fcl()(f, 0) > 5 || no_diff_)
      for (n = 0; n < N; n++) //Dirichlet ou Dirichlet_homogene
        mat(N * (ne_tot + f) + n, N * (ne_tot + f) + n) += 1;

  return matrice;
}
