/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Op_DiffF22_VDF_Elem.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_DiffF22_VDF_Elem.h>
#include <Champ_P0_VDF.h>
#include <Modele_turbulence_hyd_K_Eps_V2.h>

Implemente_instanciable_sans_constructeur(Op_DiffF22_VDF_Elem,"Op_DiffF22_VDF_const_P0_VDF",Op_Diff_VDF_base);

implemente_It_VDF_Elem(Eval_DiffF22_VDF_const_Elem)


//// printOn
//

Sortie& Op_DiffF22_VDF_Elem::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Op_DiffF22_VDF_Elem::readOn(Entree& s )
{
  return s ;
}

double Op_DiffF22_VDF_Elem::calculer_dt_stab() const
{
  double dt_stab;
  double coef,alpha;
  alpha = (diffusivite().valeurs())(0,0);
  const Zone_VDF& zone_VDF = iter.zone();

  // Calcul du pas de temps de stabilite :
  //
  //  - La  diffusivite est uniforme donc :
  //
  //     dt_stab = 1/(2*diffusivite*Max(coef(elem)))
  //
  //     avec:
  //           coef = 1/(dx*dx) + 1/(dy*dy) + 1/(dz*dz)
  //
  //           i decrivant l'ensemble des elements du maillage
  //
  // Rq : On ne balaie pas l'ensemble des elements puisque
  //      le max de coeff est atteint sur l'element qui realise
  //      a la fois le min de dx le min de dy et le min de dz

  coef = 1/(zone_VDF.h_x()*zone_VDF.h_x())
         + 1/(zone_VDF.h_y()*zone_VDF.h_y());

  if (dimension == 3)
    coef += 1/(zone_VDF.h_z()*zone_VDF.h_z());
  if (alpha==0)
    dt_stab = DMAXFLOAT;
  else
    dt_stab = 0.5/(alpha*coef);

  return dt_stab;
}


// Description:
// complete l'iterateur et l'evaluateur
void Op_DiffF22_VDF_Elem::associer(const Zone_dis& zone_dis,
                                   const Zone_Cl_dis& zone_cl_dis,
                                   const Champ_Inc& ch_diffuse)
{
  //Cerr << "Dans void Op_DiffF22_VDF_Elem::associer eqn = " << equation() << finl;
  const Champ_P0_VDF& inco = ref_cast(Champ_P0_VDF,ch_diffuse.valeur());
  const Zone_VDF& zvdf = ref_cast(Zone_VDF,zone_dis.valeur());
  const Zone_Cl_VDF& zclvdf = ref_cast(Zone_Cl_VDF,zone_cl_dis.valeur());
  iter.associer(zvdf, zclvdf, *this);

  Eval_DiffF22_VDF_const_Elem& eval_diff = (Eval_DiffF22_VDF_const_Elem&) iter.evaluateur();
  eval_diff.associer_zones(zvdf, zclvdf );          // Evaluateur_VDF::associer_zones
  eval_diff.associer_inconnue(inco );        // Eval_VDF_Elem::associer_inconnue
}


// Description:
// associe le champ de diffusivite a l'evaluateur
void Op_DiffF22_VDF_Elem::associer_diffusivite(const Champ_base& ch_diff)
{
  Eval_DiffF22_VDF_const_Elem& eval_diff = (Eval_DiffF22_VDF_const_Elem&) iter.evaluateur();
  eval_diff.associer(ch_diff);                // Eval_DiffF22_VDF::associer
  const EqnF22base& mon_eqn = ref_cast(EqnF22base,equation());

  const Champ_Inc& K_eps =  mon_eqn.modele_turbulence().eqn_transp_K_Eps().inconnue();
  const Champ_Inc& v2 =  mon_eqn.modele_turbulence().eqn_V2().inconnue();
  eval_diff.associer_keps(K_eps,v2);
}



const Champ_base& Op_DiffF22_VDF_Elem::diffusivite() const
{
  const Eval_DiffF22_VDF_const_Elem& eval_diff = (const Eval_DiffF22_VDF_const_Elem&) iter.evaluateur();
  return eval_diff.diffusivite();
}
