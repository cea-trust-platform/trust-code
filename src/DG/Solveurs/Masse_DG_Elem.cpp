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

#include <Masse_DG_Elem.h>
#include <Domaine_Cl_DG.h>
#include <Champ_Elem_DG.h>
#include <Domaine_DG.h>
#include <Option_DG.h>
#include <TRUSTTab_parts.h>
#include <Equation_base.h>
#include <Neumann_paroi.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <Milieu_base.h>
#include <Conds_lim.h>

Implemente_instanciable(Masse_DG_Elem, "Masse_DG_Elem", Masse_DG_base);

Sortie& Masse_DG_Elem::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Masse_DG_Elem::readOn(Entree& s) { return s; }

DoubleTab& Masse_DG_Elem::appliquer_impl(DoubleTab& sm) const
{
  const Champ_Elem_DG& ch = ref_cast(Champ_Elem_DG, equation().inconnue());

  const Matrice_Base& invM = ch.get_inv_mass_matrix();

  DoubleTab invSm;
  invSm.copy(sm, RESIZE_OPTIONS::COPY_INIT);

  invM.multvect(invSm, sm);

  return sm;
}

//Masse_DG_Elem est responsable des parties de la matrice n'impliquant pas la diffusion
void Masse_DG_Elem::dimensionner(Matrice_Morse& matrix) const
{

  // TODO DG construire matrice bloc avec toutes les contributions des differentes inconnues

  const Champ_Elem_DG& ch = ref_cast(Champ_Elem_DG, equation().inconnue());
  const Matrice_Morse& mass_matrix = ch.get_mass_matrix();

  matrix = mass_matrix; //Todo DG dans Equation_Base.cpp ligne 1863/1876 pourquoi avoir besoin d'additionner les matrices ?

}

DoubleTab& Masse_DG_Elem::ajouter_masse(double dt, DoubleTab& secmem, const DoubleTab& inco, int penalisation) const
{
  const Champ_Elem_DG& ch = ref_cast(Champ_Elem_DG, equation().inconnue());
  const Matrice_Morse& mass_matrix = ch.get_mass_matrix();

  secmem *= dt;
  mass_matrix.ajouter_multvect(inco, secmem);
  secmem /= dt;

  return secmem;
}

Matrice_Base& Masse_DG_Elem::ajouter_masse(double dt, Matrice_Base& matrice, int penalisation) const
{
  Matrice_Morse& mat = ref_cast(Matrice_Morse, matrice);
  const Champ_Elem_DG& ch = ref_cast(Champ_Elem_DG, equation().inconnue());
  const Matrice_Morse& mass_matrix = ch.get_mass_matrix();

  mat *= dt;
  mat += mass_matrix;
  mat /= dt;

  //todo DG for multi-component equation, adapt the mass matrix with bloc ?

  return matrice;
}
