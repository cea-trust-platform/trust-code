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

#include <Frontiere_ouverte_rho_u_impose.h>
#include <Fluide_Dilatable_base.h>
#include <Equation_base.h>
#include <Motcle.h>
#include <Frontiere_dis_base.h>

Implemente_instanciable(Frontiere_ouverte_rho_u_impose,"Frontiere_ouverte_rho_u_impose",Entree_fluide_vitesse_imposee_libre);

Sortie& Frontiere_ouverte_rho_u_impose::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Frontiere_ouverte_rho_u_impose::readOn(Entree& s)
{
  return Entree_fluide_vitesse_imposee_libre::readOn(s);
}

void Frontiere_ouverte_rho_u_impose::completer()
{
  le_fluide = ref_cast(Fluide_Dilatable_base,ma_zone_cl_dis->equation().milieu());
}

int Frontiere_ouverte_rho_u_impose::compatible_avec_eqn(const Equation_base& eqn) const
{
  if (!Entree_fluide_vitesse_imposee_libre::compatible_avec_eqn(eqn)) return 0;
  if (!sub_type(Fluide_Dilatable_base,ma_zone_cl_dis->equation().milieu())) return 0;

  return 1;
}

double Frontiere_ouverte_rho_u_impose::val_imp_au_temps(double temps, int i) const
{
  Cerr << "Acces a une condition limite en rho.u sans preciser la composante" << finl;
  Process::exit();
  return 0;
}

double Frontiere_ouverte_rho_u_impose::val_imp_au_temps(double temps, int i,int j) const
{
  double rho_u, rho;
  int ndeb=le_champ_front->frontiere_dis().frontiere().num_premiere_face();
  const DoubleTab& tab_rho_u=le_champ_front->valeurs_au_temps(temps);
  assert(tab_rho_u.nb_dim()==2);

  if (tab_rho_u.dimension(0)==1)
    rho_u=tab_rho_u(0,j);
  else
    rho_u=tab_rho_u(i,j);

  rho=le_fluide->rho_face_np1()(i+ndeb);
  return rho_u/rho;
}
