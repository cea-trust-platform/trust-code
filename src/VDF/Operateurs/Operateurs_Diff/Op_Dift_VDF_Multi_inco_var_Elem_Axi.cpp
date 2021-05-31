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
// File:        Op_Dift_VDF_Multi_inco_var_Elem_Axi.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Operateurs_Diff
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Dift_VDF_Multi_inco_var_Elem_Axi.h>
#include <Champ_P0_VDF.h>
#include <Modele_turbulence_scal_base.h>

Implemente_instanciable_sans_constructeur(Op_Dift_VDF_Multi_inco_var_Elem_Axi,"Op_Dift_VDF_Multi_inco_var_P0_VDF_Axi",Op_Dift_VDF_base);
implemente_It_VDF_Elem(Eval_Dift_VDF_Multi_inco_var_Elem_Axi)

Sortie& Op_Dift_VDF_Multi_inco_var_Elem_Axi::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

Entree& Op_Dift_VDF_Multi_inco_var_Elem_Axi::readOn(Entree& s )
{
  return s ;
}

////////////////////////////////////////////////////////////////
//
//   Fonctions de la classe Op_Dift_VDF_Multi_inco_var_Elem_Axi
//
////////////////////////////////////////////////////////////////

// Description:
// complete l'iterateur et l'evaluateur
void Op_Dift_VDF_Multi_inco_var_Elem_Axi::associer(const Zone_dis& zone_dis,
                                                   const Zone_Cl_dis& zone_cl_dis,
                                                   const Champ_Inc& ch_diffuse)
{
  const Champ_P0_VDF& inco = ref_cast(Champ_P0_VDF,ch_diffuse.valeur());
  const Zone_VDF& zvdf = ref_cast(Zone_VDF,zone_dis.valeur());
  const Zone_Cl_VDF& zclvdf = ref_cast(Zone_Cl_VDF,zone_cl_dis.valeur());
  iter.associer(zvdf, zclvdf, *this);

  Eval_Dift_VDF_Multi_inco_var_Elem_Axi& eval_diff_turb =
    dynamic_cast<Eval_Dift_VDF_Multi_inco_var_Elem_Axi&> (iter.evaluateur());
  eval_diff_turb.associer_zones(zvdf, zclvdf );
  eval_diff_turb.associer_inconnue(inco );
}


// Description:
// associe le champ de diffusivite a l'evaluateur
void Op_Dift_VDF_Multi_inco_var_Elem_Axi::associer_diffusivite(const Champ_base& ch_diff)
{
  Eval_Dift_VDF_Multi_inco_var_Elem_Axi& eval_diff_turb =
    dynamic_cast<Eval_Dift_VDF_Multi_inco_var_Elem_Axi&> (iter.evaluateur());
  eval_diff_turb.associer(ch_diff);
}

const Champ_base& Op_Dift_VDF_Multi_inco_var_Elem_Axi::diffusivite() const
{
  const Eval_Dift_VDF_Multi_inco_var_Elem_Axi& eval_diff_turb =
    dynamic_cast<const Eval_Dift_VDF_Multi_inco_var_Elem_Axi&> (iter.evaluateur());
  return eval_diff_turb.get_diffusivite();
}

void Op_Dift_VDF_Multi_inco_var_Elem_Axi::associer_diffusivite_turbulente(const Champ_Fonc& diff_turb)
{
  Op_Diff_Turbulent_base::associer_diffusivite_turbulente(diff_turb);
  Evaluateur_VDF& eval = iter.evaluateur();
  Eval_Dift_VDF_Multi_inco_var_Elem_Axi& eval_diff_turb =
    dynamic_cast<Eval_Dift_VDF_Multi_inco_var_Elem_Axi&> (eval);
  eval_diff_turb.associer_diff_turb(diff_turb);
}

void Op_Dift_VDF_Multi_inco_var_Elem_Axi::associer_loipar(const Turbulence_paroi_scal& loi_paroi)
{
  //loipar = loi_paroi;
  Evaluateur_VDF& eval = iter.evaluateur();
  Eval_Dift_VDF_Multi_inco_var_Elem_Axi& eval_diff_turb =
    dynamic_cast<Eval_Dift_VDF_Multi_inco_var_Elem_Axi&> (eval);
  eval_diff_turb.associer_loipar(loi_paroi);
}

void Op_Dift_VDF_Multi_inco_var_Elem_Axi::completer()
{
  Op_Dift_VDF_base::completer();
  const RefObjU& modele_turbulence = equation().get_modele(TURBULENCE);
  const Modele_turbulence_scal_base& mod_turb = ref_cast(Modele_turbulence_scal_base,modele_turbulence.valeur());
  const Champ_Fonc& lambda_t = mod_turb.conductivite_turbulente();
  associer_diffusivite_turbulente(lambda_t);
  const Turbulence_paroi_scal& loipar = mod_turb.loi_paroi();
  associer_loipar(loipar);
}

double Op_Dift_VDF_Multi_inco_var_Elem_Axi::calculer_dt_stab() const
{
  const Zone_VDF& zone_VDF = iter.zone();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const DoubleTab& alpha = diffusivite().valeurs();
  const DoubleVect& alpha_t = diffusivite_turbulente()->valeurs();
  const int D = dimension;
  // Calcul du pas de temps de stabilite :
  //
  //
  //  - La diffusivite laminaire est variable
  //  - La diffusivite turbulente est variable
  //
  //     dt_stab = Min (1/(2*(Max(diffu1(i),diffu2(i),....)+diff_turb(i))*coeff(elem))
  //
  //     avec :
  //            diffu1(i),diffu2(i) ...: diffusivites des differents constituants
  //                                     sur l'element
  //            coeff =  1/(dx*dx) + 1/(dy*dy) + 1/(dz*dz)
  //
  //            i decrivant l'ensemble des elements du maillage
  //

  double coef = -1.e10;
  DoubleVect numfa(2 * D), h(D);

  for (int e = 0; e < zone_VDF.nb_elem(); e++)
    {
      for (int i = 0; i < 2 * D; i++)
        numfa(i) = elem_faces(e, i);
      for (int d = 0; d < D; d++)
        h(d) = zone_VDF.dist_face_axi(numfa(d), numfa(d + D), d);
      double alpha_lam = alpha(e, 0), invh = 0.;
      for (int k = 1; k < alpha.dimension(1); k++)
        alpha_lam = max(alpha_lam, alpha(e, k));
      for (int d = 0; d < D; d++)
        invh += 1. / (h(d) * h(d));
      const double alpha_local = (alpha_lam + alpha_t(e)) * invh;
      coef = max(coef, alpha_local);
    }

  const double dt_stab = 1. / (2. * (coef + DMINFLOAT));

  return dt_stab;
}

//
// Fonctions inline de la classe Op_Dift_VDF_Multi_inco_Elem_Axi
//
//// Op_Dift_VDF_Multi_inco_Elem_Axi
//
Op_Dift_VDF_Multi_inco_var_Elem_Axi::Op_Dift_VDF_Multi_inco_var_Elem_Axi():
  Op_Dift_VDF_base(It_VDF_Elem(Eval_Dift_VDF_Multi_inco_var_Elem_Axi)())
{
}
