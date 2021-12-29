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
// File:        Op_Diff_VDF_Face_base.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Op_Diff_Dift
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_VDF_Face_base.h>
#include <Fluide_Incompressible.h>
#include <Eval_VDF_Face.h>
#include <Eval_Diff_VDF.h>
#include <Champ_Inc.h>
#include <SFichier.h>

Implemente_base(Op_Diff_VDF_Face_base,"Op_Diff_VDF_Face_base",Op_Diff_VDF_base);

Sortie& Op_Diff_VDF_Face_base::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Diff_VDF_Face_base::readOn(Entree& s ) { return s ; }

double Op_Diff_VDF_Face_base::calculer_dt_stab() const { return Op_Diff_VDF_Face_base::calculer_dt_stab(iter.zone()); }

double Op_Diff_VDF_Face_base::calculer_dt_stab(const Zone_VDF& zone_VDF) const
{
  // Calcul du pas de temps de stabilite :
  //
  //
  //  - La diffusivite n'est pas uniforme donc:
  //
  //     dt_stab = Min (1/(2*diffusivite(elem)*coeff(elem))
  //
  //     avec :
  //            coeff =  1/(dx*dx) + 1/(dy*dy) + 1/(dz*dz)
  //
  //            i decrivant l'ensemble des elements du maillage
  //
  //  Rq: en hydraulique on cherche le Max sur les elements du maillage
  //      initial (comme en thermique) et non le Max sur les volumes de Qdm.
  double coef=0;
  const Champ_base& ch_diffu = has_champ_masse_volumique() ? diffusivite() : diffusivite_pour_pas_de_temps();
  const DoubleTab& diffu = ch_diffu.valeurs();
  const double Cdiffu = sub_type(Champ_Uniforme, ch_diffu);

  // Si la diffusivite est variable, ce doit etre un champ aux elements.
  assert(Cdiffu || diffu.size() == zone_VDF.nb_elem());
  const int nb_elem = zone_VDF.nb_elem();
  for (int elem = 0; elem < nb_elem; elem++)
    {
      double diflo = 0.;
      for (int i = 0; i < dimension; i++)
        {
          const double h = zone_VDF.dim_elem(elem, i);
          diflo += 1. / (h * h);
        }
      const int k = Cdiffu ? 0 : elem;
      double alpha = diffu(k, 0);
      for (int ncomp = 1; ncomp < diffu.dimension(1); ncomp++)
        alpha = max(alpha, diffu(k, ncomp));

      diflo *= alpha;
      if (has_champ_masse_volumique())
        {
          const DoubleTab& rho = get_champ_masse_volumique().valeurs();
          diflo/= rho(elem);
        }
      coef = max(coef,diflo);
    }

  double dt_stab = (coef==0 ? DMAXFLOAT : 0.5/coef);
  return Process::mp_min(dt_stab);
}
