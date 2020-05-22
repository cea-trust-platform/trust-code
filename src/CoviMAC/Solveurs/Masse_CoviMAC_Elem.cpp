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
// File:        Masse_CoviMAC_Elem.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Solveurs
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#include <Masse_CoviMAC_Elem.h>
#include <Zone_Cl_CoviMAC.h>
#include <Zone_CoviMAC.h>
#include <Debog.h>
#include <Champ_P0_CoviMAC.h>
#include <Equation_base.h>
#include <Conds_lim.h>
#include <Neumann_paroi.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Operateur.h>
#include <Op_Diff_negligeable.h>
#include <Echange_impose_base.h>
#include <ConstDoubleTab_parts.h>

Implemente_instanciable(Masse_CoviMAC_Elem,"Masse_CoviMAC_Elem",Solveur_Masse_base);


//     printOn()
/////

Sortie& Masse_CoviMAC_Elem::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Masse_CoviMAC_Elem::readOn(Entree& s)
{
  return s ;
}


////////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Masse_CoviMAC_Elem
//
////////////////////////////////////////////////////////////////


DoubleTab& Masse_CoviMAC_Elem::appliquer_impl(DoubleTab& sm) const
{
  const Zone_CoviMAC& zone_CoviMAC = la_zone_CoviMAC.valeur();
  const DoubleVect& volumes = zone_CoviMAC.volumes();
  const DoubleVect& porosite_elem = zone_CoviMAC.porosite_elem();
  int nb_elem = zone_CoviMAC.nb_elem();
  if(nb_elem==0)
    {
      sm.echange_espace_virtuel();
      return sm;
    }
  int nb_comp = sm.size()/nb_elem;
  int nb_dim=sm.nb_dim();
  assert((nb_comp*nb_elem == sm.size())||(nb_dim==3));
  if (nb_dim == 1)
    for (int num_elem=0; num_elem<nb_elem; num_elem++)
      sm(num_elem) /= (volumes(num_elem)*porosite_elem(num_elem));
  else if (nb_dim == 2)
    {
      for (int num_elem=0; num_elem<nb_elem; num_elem++)
        for (int k=0; k<nb_comp; k++)
          sm(num_elem,k) /= (volumes(num_elem)*porosite_elem(num_elem));
    }
  else if (sm.nb_dim() == 3)
    {
      //int d0=sm.dimension(0);
      int d1=sm.dimension(1);
      int d2=sm.dimension(2);
      for (int num_elem=0; num_elem<nb_elem; num_elem++)
        for (int k=0; k<d1; k++)
          for (int d=0; d<d2; d++)
            sm(num_elem,k,d) /= (volumes(num_elem)*porosite_elem(num_elem));
    }
  else
    {
      Cerr<< "Masse_CoviMAC_Elem::appliquer ne peut pas s'appliquer a un DoubleTab a "<<sm.nb_dim()<<" dimensions"<<finl;
      assert(0);
      exit();
    }
  sm.echange_espace_virtuel();
  return sm;
}

void Masse_CoviMAC_Elem::associer_zone_dis_base(const Zone_dis_base& la_zone_dis_base)
{
  la_zone_CoviMAC = ref_cast(Zone_CoviMAC, la_zone_dis_base);
}

void Masse_CoviMAC_Elem::associer_zone_cl_dis_base(const Zone_Cl_dis_base& la_zone_Cl_dis_base)
{
  la_zone_Cl_CoviMAC = ref_cast(Zone_Cl_CoviMAC, la_zone_Cl_dis_base);
}

void Masse_CoviMAC_Elem::dimensionner(Matrice_Morse& mat) const
{
  const Zone_CoviMAC& zone = la_zone_CoviMAC.valeur();

  /* diagonale sur les elems reels */
  int i, N = equation().inconnue().valeurs().line_size(), ntot = equation().inconnue().valeurs().size_totale();
  IntTab stencil(0, 2);
  stencil.set_smart_resize(1);
  for (i = 0; i < N * zone.nb_elem(); i++) stencil.append_line(i, i);
  for (i = N * zone.nb_elem_tot(); i < N * (zone.nb_elem_tot() + zone.premiere_face_int()); i++) stencil.append_line(i, i);
  Matrix_tools::allocate_morse_matrix(ntot, ntot, stencil, mat);
}

DoubleTab& Masse_CoviMAC_Elem::ajouter_masse(double dt, DoubleTab& secmem, const DoubleTab& inco, int penalisation) const
{
  const Zone_CoviMAC& zone = la_zone_CoviMAC.valeur();
  const Champ_P0_CoviMAC& ch = ref_cast(Champ_P0_CoviMAC, equation().inconnue().valeur());
  const Conds_lim& cls = la_zone_Cl_CoviMAC->les_conditions_limites();
  const IntTab& f_e = zone.face_voisins();
  const DoubleVect& ve = zone.volumes(), &pe = zone.porosite_elem(), &fs = zone.face_surfaces();
  int e, f, n, N = inco.line_size();
  DoubleTrav coef(zone.porosite_elem());
  coef = 1.;
  if (has_coefficient_temporel_) appliquer_coef(coef);

  ch.init_cl();
  //elements : "vraie" masse
  for (e = 0; e < zone.nb_elem(); e++) for (n = 0; n < N; n++)
      secmem.addr()[N * e + n] += coef(e) * pe(e) * ve(e) * inco.addr()[N * e + n] / dt;

  //CLs : ligne val = val_imp pour Dirichlet, flux pour Neumann et Echange_externe_impose
  for (f = 0; f < zone.premiere_face_int(); f++)
    if (ch.icl(f, 0) < 3) for (e = f_e(f, ch.icl(f, 0) < 2), n = 0; n < N; n++) //Echange_impose_base
        secmem(N * e + n) += fs(f) * ref_cast(Echange_impose_base, cls[ch.icl(f, 1)].valeur()).h_imp(ch.icl(f, 2), n)
                             * ref_cast(Echange_impose_base, cls[ch.icl(f, 1)].valeur()).T_ext(ch.icl(f, 2), n);
    else if (ch.icl(f, 0) == 3) continue; //monolithique
    else if (ch.icl(f, 0) < 6) for (e = f_e(f, 1), n = 0; n < N; n++) //Neumann
        secmem(N * e + n) += fs(f) * ref_cast(Neumann, cls[ch.icl(f, 1)].valeur()).flux_impose(ch.icl(f, 2), n);
    else if (ch.icl(f, 0) < 7) for (e = f_e(f, 1), n = 0; n < N; n++) //Dirichlet non homogene
        secmem(N * e + n) = ref_cast(Dirichlet, cls[ch.icl(f, 1)].valeur()).val_imp(ch.icl(f, 2), n);

  return secmem;
}

Matrice_Base& Masse_CoviMAC_Elem::ajouter_masse(double dt, Matrice_Base& matrice, int penalisation) const
{
  const Zone_CoviMAC& zone = la_zone_CoviMAC.valeur();
  const Champ_P0_CoviMAC& ch = ref_cast(Champ_P0_CoviMAC, equation().inconnue().valeur());
  const Conds_lim& cls = la_zone_Cl_CoviMAC->les_conditions_limites();
  const IntTab& f_e = zone.face_voisins();
  const DoubleVect& ve = zone.volumes(), &pe = zone.porosite_elem(), &fs = zone.face_surfaces();
  Matrice_Morse& mat = ref_cast(Matrice_Morse, matrice);
  int e, f, n, N = ch.valeurs().line_size();
  DoubleTrav coef(zone.porosite_elem());
  coef = 1.;
  if (has_coefficient_temporel_) appliquer_coef(coef);

  ch.init_cl();
  //elements : "vraie" masse
  for (e = 0; e < zone.nb_elem(); e++) for (n = 0; n < N; n++)
      mat(N * e + n, N * e + n) += coef(e) * pe(e) * ve(e) / dt;

  //CLs : ligne val = val_imp pour Dirichlet, flux pour Neumann et Echange_externe_impose
  for (f = 0; f < zone.premiere_face_int(); f++)
    {
      /* contribution de Echange_impose_base */
      if (ch.icl(f, 0) < 3) for (e = f_e(f, ch.icl(f, 0) < 2), n = 0; n < N; n++) //Echange_impose_base
          mat(N * e + n, N * e + n) += fs(f) * ref_cast(Echange_impose_base, cls[ch.icl(f, 1)].valeur()).h_imp(ch.icl(f, 2), n);
      /* si Dirichlet ou Echange_global_impose : ligne val= . */
      if (ch.icl(f, 0) == 2 || ch.icl(f, 0) > 5) for (e = f_e(f, 1), n = 0; n < N; n++) mat(N * e + n, N * e + n) = 1;
    }

  return matrice;
}

void Masse_CoviMAC_Elem::appliquer_coef(DoubleVect& coef) const
{
  if (has_coefficient_temporel_)
    {
      REF(Champ_base) ref_coeff;
      ref_coeff = equation().get_champ(name_of_coefficient_temporel_);

      DoubleTab values;
      if (sub_type(Champ_Inc_base,ref_coeff.valeur()))
        {
          const Champ_Inc_base& coeff = ref_cast(Champ_Inc_base,ref_coeff.valeur());
          ConstDoubleTab_parts val_parts(coeff.valeurs());
          values.ref(val_parts[0]);

        }
      else if (sub_type(Champ_Fonc_base,ref_coeff.valeur()))
        {
          const Champ_Fonc_base& coeff = ref_cast(Champ_Fonc_base,ref_coeff.valeur());
          values.ref(coeff.valeurs());

        }
      else if (sub_type(Champ_Don_base,ref_coeff.valeur()))
        {
          DoubleTab nodes;
          equation().inconnue().valeur().remplir_coord_noeuds(nodes);
          ref_coeff.valeur().valeur_aux(nodes,values);
        }
      tab_multiply_any_shape(coef, values, VECT_REAL_ITEMS);
    }
}
