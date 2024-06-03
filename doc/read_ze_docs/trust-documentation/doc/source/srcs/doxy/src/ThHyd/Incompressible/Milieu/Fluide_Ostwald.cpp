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
// Nouveau fluide. c'est un fluide incompressible non newtonien. Il derive donc
//   de fluide_Incompressible et possede quelques proprietes de plus (K, N)
//   mu depend de K, N, et de grad(U)

#include <Fluide_Ostwald.h>
#include <Probleme_base.h>
#include <Discret_Thyd.h>

#include <Navier_Stokes_std.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Fluide_Ostwald,"Fluide_Ostwald",Fluide_Incompressible);
// XD fluide_ostwald fluide_incompressible fluide_ostwald -1 Non-Newtonian fluids governed by Ostwald\'s law. The law applicable to stress tensor is: NL2 tau=K(T)*(D:D/2)**((n-1)/2)*D Where: NL2 D refers to the deformation tensor NL2 K refers to fluid consistency (may be a function of the temperature T) NL2 n refers to the fluid structure index n=1 for a Newtonian fluid, n<1 for a rheofluidifier fluid, n>1 for a rheothickening fluid.
// XD attr k field_base k 1 Fluid consistency.
// XD attr n field_base n 1 Fluid structure index.

Fluide_Ostwald::Fluide_Ostwald() { }

Sortie& Fluide_Ostwald::printOn(Sortie& s ) const
{
  s<<"{"<<finl;
  s<<"K"<<K_<<finl;
  //  s<<"beta_co"<<beta_co<<finl;
  s<<"N"<<N_<<finl;
  Milieu_base::ecrire(s);
  s<<"}"<<finl;
  return s;
}

Entree& Fluide_Ostwald::readOn(Entree& is)
{
  Fluide_Incompressible::readOn(is);
  return is;
}

void Fluide_Ostwald::set_param(Param& param)
{
  Fluide_Incompressible::set_param(param);
  param.ajouter_condition("is_read_rho","Density rho has not been read for a Fluide_Ostwald type medium.");
  param.ajouter("K",&K_,Param::REQUIRED);
  param.ajouter("n",&N_,Param::REQUIRED);
}

void  Fluide_Ostwald::discretiser(const Probleme_base& pb, const  Discretisation_base& dis)
{
  const Domaine_dis_base& domaine_dis=pb.equation(0).domaine_dis();
  const Discret_Thyd& dis2=ref_cast(Discret_Thyd, dis);

  // avec la signature de Ostwald....
  const Navier_Stokes_std& eqn_hydr=ref_cast(Navier_Stokes_std,pb.equation(0));
  dis2.proprietes_physiques_fluide_Ostwald(eqn_hydr.domaine_dis(),(*this),eqn_hydr,eqn_hydr.inconnue());
  Champ_Don& K = consistance();
  if (K.non_nul())
    {
      dis.nommer_completer_champ_physique(domaine_dis,"consistance","kg/m/s",K.valeur(),pb);
      champs_compris_.ajoute_champ(K.valeur());
    }

  Fluide_Incompressible::discretiser(pb,dis);
}

void Fluide_Ostwald::creer_champs_non_lus()
{
  Milieu_base::creer_champs_non_lus();
}

void Fluide_Ostwald::mettre_a_jour(double tps)
{
  K_.mettre_a_jour(tps);
  if (N_.non_nul() )
    N_.mettre_a_jour(tps);
  Fluide_Incompressible::mettre_a_jour(tps);
}

int Fluide_Ostwald::initialiser(const double temps)
{
  return Fluide_Incompressible::initialiser(temps);
}
