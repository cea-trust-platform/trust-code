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
// File:        Assembleur_P_CoviMAC.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Solveurs
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#include <Assembleur_P_CoviMAC.h>
#include <Zone_Cl_CoviMAC.h>
#include <Zone_CoviMAC.h>
#include <Neumann_sortie_libre.h>
#include <Op_Grad_CoviMAC_Face.h>
#include <Matrice_Bloc_Sym.h>
#include <Matrice_Diagonale.h>
#include <Array_tools.h>
#include <Debog.h>
#include <Static_Int_Lists.h>
#include <Operateur_Grad.h>
#include <Schema_Temps_base.h>
#include <Piso.h>

#include <Navier_Stokes_std.h>
#include <Matrice_Morse_Sym.h>
#include <Matrix_tools.h>

#include <Pb_Multiphase.h>

Implemente_instanciable(Assembleur_P_CoviMAC,"Assembleur_P_CoviMAC",Assembleur_base);

Sortie& Assembleur_P_CoviMAC::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom() ;
}

Entree& Assembleur_P_CoviMAC::readOn(Entree& s )
{
  return Assembleur_base::readOn(s);
}

int Assembleur_P_CoviMAC::assembler(Matrice& la_matrice)
{
  DoubleVect rien;
  return assembler_mat(la_matrice,rien,1,1);
}

int  Assembleur_P_CoviMAC::assembler_mat(Matrice& la_matrice,const DoubleVect& diag,int incr_pression,int resoudre_en_u)
{
  set_resoudre_increment_pression(incr_pression);
  set_resoudre_en_u(resoudre_en_u);
  la_matrice.typer("Matrice_Morse");
  Matrice_Morse& mat = ref_cast(Matrice_Morse, la_matrice.valeur());

  const Zone_CoviMAC& zone = ref_cast(Zone_CoviMAC, la_zone_CoviMAC.valeur());
  const Op_Grad_CoviMAC_Face& grad = ref_cast(Op_Grad_CoviMAC_Face, ref_cast(Navier_Stokes_std, equation()).operateur_gradient().valeur());
  grad.update_grad();
  const DoubleTab& fgrad_c = grad.fgrad_c;
  const IntTab& f_e = zone.face_voisins(), &fgrad_d = grad.fgrad_d, &fgrad_e = grad.fgrad_e;
  const DoubleVect& pf = zone.porosite_face(), &fs = zone.face_surfaces();
  int i, j, e, eb, f, ne = zone.nb_elem(), ne_tot = zone.nb_elem_tot();

  //en l'absence de CLs en pression, on ajoute P(0) = 0 sur le process 0
  has_P_ref=0;
  for (int n_bord=0; n_bord<la_zone_CoviMAC->nb_front_Cl(); n_bord++)
    if (sub_type(Neumann_sortie_libre, la_zone_Cl_CoviMAC->les_conditions_limites(n_bord).valeur()) )
      has_P_ref=1;

  /* 1. stencil de la matrice en pression : seulement au premier passage */
  if (!stencil_done)
    {
      IntTrav stencil(0, 2);
      stencil.set_smart_resize(1);
      for (f = 0; f < zone.nb_faces(); f++) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < ne) for (j = fgrad_d(f); j < fgrad_d(f + 1); j++)
              if ((eb = fgrad_e(j)) < ne_tot) stencil.append_line(e, eb);

      tableau_trier_retirer_doublons(stencil);
      Matrix_tools::allocate_morse_matrix(ne_tot, ne_tot, stencil, mat);
      mat.sort_stencil();
      tab1.ref_array(mat.get_set_tab1()), tab2.ref_array(mat.get_set_tab2());
      stencil_done = 1;
    }
  else //sinon, on recycle
    {
      mat.get_set_tab1().ref_array(tab1);
      mat.get_set_tab2().ref_array(tab2);
      mat.get_set_coeff().resize(tab2.size());
      mat.set_nb_columns(ne_tot);
    }

  /* 2. remplissage des coefficients : style Op_Diff_PolyMAC_Elem */
  for (f = 0; f < zone.nb_faces(); f++) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < ne) for (j = fgrad_d(f); j < fgrad_d(f + 1); j++) if ((eb = fgrad_e(j)) < ne_tot)
            mat(e, eb) += (i ? 1 : -1) * pf(f) * fs(f) * fgrad_c(j, 0);

  if (!has_P_ref && !Process::me()) mat(0, 0) *= 2;

  return 1;
}

int Assembleur_P_CoviMAC::modifier_solution(DoubleTab& pression)
{
  Debog::verifier("pression dans modifier solution in",pression);
  // Projection :
  double press_0;
  if(!has_P_ref)
    {
      //abort();
      // On prend la pression minimale comme pression de reference
      // afin d'avoir la meme pression de reference en sequentiel et parallele
      press_0=DMAXFLOAT;
      int n,nb_elem=la_zone_CoviMAC.valeur().zone().nb_elem();
      for(n=0; n<nb_elem; n++)
        if (pression[n] < press_0)
          press_0 = pression[n];
      press_0 = Process::mp_min(press_0);

      for(n=0; n<nb_elem; n++)
        pression[n] -=press_0;

      pression.echange_espace_virtuel();
    }
  return 1;
}

const Zone_dis_base& Assembleur_P_CoviMAC::zone_dis_base() const
{
  return la_zone_CoviMAC.valeur();
}

const Zone_Cl_dis_base& Assembleur_P_CoviMAC::zone_Cl_dis_base() const
{
  return la_zone_Cl_CoviMAC.valeur();
}

void Assembleur_P_CoviMAC::associer_zone_dis_base(const Zone_dis_base& la_zone_dis)
{
  la_zone_CoviMAC = ref_cast(Zone_CoviMAC,la_zone_dis);
}

void Assembleur_P_CoviMAC::associer_zone_cl_dis_base(const Zone_Cl_dis_base& la_zone_Cl_dis)
{
  la_zone_Cl_CoviMAC = ref_cast(Zone_Cl_CoviMAC, la_zone_Cl_dis);
}

void Assembleur_P_CoviMAC::completer(const Equation_base& Eqn)
{
  mon_equation=Eqn;
  stencil_done = 0;
}

/* equation sum_k alpha_k = 1 en Pb_Multiphase */
void Assembleur_P_CoviMAC::dimensionner_continuite(matrices_t matrices) const
{
  int e, n, N = ref_cast(Pb_Multiphase, equation().probleme()).nb_phases(), ne_tot = la_zone_CoviMAC->nb_elem_tot();
  IntTrav stencil(0, 2);
  stencil.set_smart_resize(1);
  for (e = 0; e < la_zone_CoviMAC->nb_elem(); e++) for (n = 0; n < N; n++) stencil.append_line(e, N * e + n);
  Matrix_tools::allocate_morse_matrix(ne_tot, N * ne_tot, stencil, *matrices.at("alpha"));
}

void Assembleur_P_CoviMAC::assembler_continuite(matrices_t matrices, DoubleTab& secmem) const
{
  const DoubleTab& alpha = ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().valeurs();
  Matrice_Morse& mat = *matrices.at("alpha");
  const DoubleVect& ve = la_zone_CoviMAC->volumes(), &pe = la_zone_CoviMAC->porosite_elem();
  int e, n, N = alpha.line_size();
  /* second membre : on multiplie par porosite * volume pour que le systeme en P soit symetrique en cartesien */
  for (e = 0; e < la_zone_CoviMAC->nb_elem(); e++)
    for (secmem(e) = -pe(e) * ve(e), n = 0; n < N; n++) secmem(e) += pe(e) * ve(e) * alpha(e, n);
  /* matrice */
  for (e = 0; e < la_zone_CoviMAC->nb_elem(); e++) for (n = 0; n < N; n++) mat(e, N * e + n) = -pe(e) * ve(e);
}
