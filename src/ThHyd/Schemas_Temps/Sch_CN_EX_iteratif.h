/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Sch_CN_EX_iteratif.h
// Directory:   $TRUST_ROOT/src/ThHyd/Schemas_Temps
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Sch_CN_EX_iteratif_included
#define Sch_CN_EX_iteratif_included

#include <Sch_CN_iteratif.h>
//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Sch_CN_EX_iteratif
//
//     Ce schema en temps implemente quelques astuces en plus de Sch_CN_iteratif pour
//     le stabiliser au-dela de son domaine de stabilite naturel (facsec<2).
//
//     Un facteur d'amortissement des iterations est defini : omega.
//     Il ameliore la stabilite, mais deterore la qualite de la resolution :
//     Aux petits nombres d'iterations, les derivees temporelles sont sous-estimees
//     et les lois de conservation ne sont plus necessairement satisfaites.
//
//     Pour augmenter le pas de temps, les equations autres que Navier-Stokes sont resolues
//     via le calcul de plusieurs pas de temps d'Euler explicite.
//     A chaque iteration de Sch_CN_iteratif, les n pas de temps d'Euler explicite sont recalcules.
//
//     Ce schema est adapte aux cas industriels de type gros calculs hydrauliques LES avec
//     thermique couplee a du solide
//
// .SECTION voir aussi
//     Sch_CN_iteratif
//////////////////////////////////////////////////////////////////////////////
class Sch_CN_EX_iteratif : public Sch_CN_iteratif
{
  Declare_instanciable(Sch_CN_EX_iteratif);

public :

  virtual void set_param(Param& titi);
  void mettre_a_jour_dt_stab();

protected :

  virtual bool iterateTimeStepOnEquation(int i,bool& converged);
  virtual bool iterateTimeStepOnNS(int i,bool& converged);
  virtual bool iterateTimeStepOnOther(int i,bool& converged);

  double omega;
  virtual void ajuster_facsec(type_convergence cv);

};

#endif
