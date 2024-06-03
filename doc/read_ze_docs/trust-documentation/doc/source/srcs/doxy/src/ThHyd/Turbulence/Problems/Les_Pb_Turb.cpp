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

#include <Les_Pb_Turb.h>
#include <Verif_Cl.h>

/*
 * XXX XXX XXX
 * Elie Saikali
 *
 * Attention fichier masque dans trio pour tester les equations RANS ...
 */
int Pb_Hydraulique_Concentration_Turbulent::verifier()
{
  const Domaine_Cl_dis& domaine_Cl_hydr = eq_hydraulique.domaine_Cl_dis();
  const Domaine_Cl_dis& domaine_Cl_co = eq_concentration.domaine_Cl_dis();
  tester_compatibilite_hydr_concentration(domaine_Cl_hydr,domaine_Cl_co);
  return 1;
}

int Pb_Hydraulique_Melange_Binaire_Turbulent_QC::verifier()
{
  const Domaine_Cl_dis& domaine_Cl_hydr = eq_hydraulique.domaine_Cl_dis();
  const Domaine_Cl_dis& domaine_Cl_fm = eq_frac_mass.domaine_Cl_dis();
  tester_compatibilite_hydr_fraction_massique(domaine_Cl_hydr,domaine_Cl_fm);
  return 1;
}

int Pb_Hydraulique_Turbulent::verifier()
{
  return 1;
}

int Pb_Thermohydraulique_Concentration_Turbulent::verifier()
{
  const Domaine_Cl_dis& domaine_Cl_hydr = eq_hydraulique.domaine_Cl_dis();
  const Domaine_Cl_dis& domaine_Cl_th = eq_thermique.domaine_Cl_dis();
  const Domaine_Cl_dis& domaine_Cl_co = eq_concentration.domaine_Cl_dis();
  tester_compatibilite_hydr_thermique(domaine_Cl_hydr,domaine_Cl_th);
  tester_compatibilite_hydr_concentration(domaine_Cl_hydr,domaine_Cl_co);
  return 1;
}

int Pb_Thermohydraulique_Turbulent_QC::verifier()
{
  const Domaine_Cl_dis& domaine_Cl_hydr = eq_hydraulique.domaine_Cl_dis();
  const Domaine_Cl_dis& domaine_Cl_th = eq_thermique.domaine_Cl_dis();
  tester_compatibilite_hydr_thermique(domaine_Cl_hydr,domaine_Cl_th);
  return 1;
}

int Pb_Thermohydraulique_Turbulent::verifier()
{
  const Domaine_Cl_dis& domaine_Cl_hydr = eq_hydraulique.domaine_Cl_dis();
  const Domaine_Cl_dis& domaine_Cl_th = eq_thermique.domaine_Cl_dis();
  tester_compatibilite_hydr_thermique(domaine_Cl_hydr,domaine_Cl_th);
  return 1;
}
