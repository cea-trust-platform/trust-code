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
// File:        Convection_Diffusion_Chaleur_WC.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Weakly_Compressible/Equations
// Version:     /main/52
//
//////////////////////////////////////////////////////////////////////////////

#include <Convection_Diffusion_Fluide_Dilatable_Proto.h>
#include <Convection_Diffusion_Chaleur_WC.h>
#include <Discretisation_base.h>
#include <Op_Conv_negligeable.h>

Implemente_instanciable(Convection_Diffusion_Chaleur_WC,"Convection_Diffusion_Chaleur_WC",Convection_Diffusion_Chaleur_Fluide_Dilatable_base);
// XD convection_diffusion_chaleur_WC eqn_base convection_diffusion_chaleur_WC -1 Temperature equation for a weakly-compressible fluid.

Sortie& Convection_Diffusion_Chaleur_WC::printOn(Sortie& is) const
{
  return Convection_Diffusion_Chaleur_Fluide_Dilatable_base::printOn(is);
}

Entree& Convection_Diffusion_Chaleur_WC::readOn(Entree& is)
{
  Convection_Diffusion_Chaleur_Fluide_Dilatable_base::readOn(is);
  return is;
}

void Convection_Diffusion_Chaleur_WC::set_param(Param& param)
{
  Convection_Diffusion_Chaleur_Fluide_Dilatable_base::set_param(param);
}

int Convection_Diffusion_Chaleur_WC::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="convection")
    {
      Convection_Diffusion_Chaleur_Fluide_Dilatable_base::lire_motcle_non_standard(mot,is);

      //l'equation de la chaleur en quasi compressible contient un terme source dP_tot/dt
      Cerr << "Source term creation of the energy equation :"<< finl;
      Source t;
      Source& so=les_sources.add(t);
      Nom type_so = "Source_WC_Chaleur_";
      Nom disc = discretisation().que_suis_je();
      if (disc=="VEFPreP1B") disc = "VEF";
      type_so += disc;
      so.typer_direct(type_so);
      so->associer_eqn(*this);
      Cerr<<so->que_suis_je()<<finl;
      Cerr << "Typing of the convection operator : ";
      return 1;
    }
  else
    return Convection_Diffusion_Chaleur_Fluide_Dilatable_base::lire_motcle_non_standard(mot,is);
  return 1;
}

void Convection_Diffusion_Chaleur_WC::completer()
{
  Convection_Diffusion_Chaleur_Fluide_Dilatable_base::completer(); // en fait c'est Equation_base::completer() mais on sais pas un jour ...

  // initialiser l'operateur grad
  Op_Grad_WC_.associer_eqn(*this);
  Op_Grad_WC_.typer();
  Op_Grad_WC_.l_op_base().associer_eqn(*this);
  const Zone_dis& zdis = zone_dis();
  const Zone_Cl_dis& zcl = zone_Cl_dis();
  const Champ_Inc& inco = inconnue();
  Op_Grad_WC_->associer(zdis, zcl, inco);
}

const Champ_base& Convection_Diffusion_Chaleur_WC::vitesse_pour_transport()
{
  return Convection_Diffusion_Chaleur_Fluide_Dilatable_base::vitesse_pour_transport();
}

void Convection_Diffusion_Chaleur_WC::calculer_div_u_ou_div_rhou(DoubleTab& Div) const
{
  Convection_Diffusion_Fluide_Dilatable_Proto::calculer_div_rho_u_impl(Div,*this);
}

int Convection_Diffusion_Chaleur_WC::preparer_calcul()
{
  Convection_Diffusion_Chaleur_Fluide_Dilatable_base::preparer_calcul();
  return Convection_Diffusion_Chaleur_Fluide_Dilatable_base::remplir_cl_modifiee();
}

int Convection_Diffusion_Chaleur_WC::sauvegarder(Sortie& os) const
{
  return Convection_Diffusion_Fluide_Dilatable_Proto::Sauvegarder_WC(os,*this,le_fluide.valeur());
}

int Convection_Diffusion_Chaleur_WC::reprendre(Entree& is)
{
  double temps = schema_temps().temps_courant();
  return Convection_Diffusion_Fluide_Dilatable_Proto::Reprendre_WC(is,temps,*this,le_fluide.valeur(),l_inco_ch,probleme());
}
