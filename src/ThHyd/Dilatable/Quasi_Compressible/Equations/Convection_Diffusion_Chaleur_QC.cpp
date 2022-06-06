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

#include <Convection_Diffusion_Chaleur_QC.h>
#include <Fluide_Quasi_Compressible.h>
#include <Op_Conv_negligeable.h>
#include <Frontiere_dis_base.h>
#include <Navier_Stokes_std.h>
#include <Probleme_base.h>
#include <Discret_Thyd.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Convection_Diffusion_Chaleur_QC,"Convection_Diffusion_Chaleur_QC",Convection_Diffusion_Chaleur_Fluide_Dilatable_base);
// XD convection_diffusion_chaleur_QC eqn_base convection_diffusion_chaleur_QC -1 Temperature equation for a quasi-compressible fluid.
// XD attr mode_calcul_convection chaine(into=["ancien","divuT_moins_Tdivu","divrhouT_moins_Tdivrhou"]) mode_calcul_convection 1 Option to set the form of the convective operatorNL2 divrhouT_moins_Tdivrhou (the default since 1.6.8): rho.u.gradT = div(rho.u.T )- Tdiv(rho.u.1) NL2ancien: u.gradT = div(u.T) - T.div(u) NL2 divuT_moins_Tdivu : u.gradT = div(u.T) - Tdiv(u.1)

Convection_Diffusion_Chaleur_QC::Convection_Diffusion_Chaleur_QC():mode_convection_(2) { }

Sortie& Convection_Diffusion_Chaleur_QC::printOn(Sortie& is) const
{
  return Convection_Diffusion_Chaleur_Fluide_Dilatable_base::printOn(is);
}

Entree& Convection_Diffusion_Chaleur_QC::readOn(Entree& is)
{
  Convection_Diffusion_Chaleur_Fluide_Dilatable_base::readOn(is);
  return is;
}

void Convection_Diffusion_Chaleur_QC::set_param(Param& param)
{
  Convection_Diffusion_Chaleur_Fluide_Dilatable_base::set_param(param);
  param.ajouter_non_std("mode_calcul_convection",(this));
}

int Convection_Diffusion_Chaleur_QC::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="convection")
    {
      Convection_Diffusion_Chaleur_Fluide_Dilatable_base::lire_motcle_non_standard(mot,is);

      //l'equation de la chaleur en quasi compressible contient un terme source dP/dt
      Cerr << "Source term creation of the energy equation :"<< finl;
      Source t;
      Source& so=les_sources.add(t);
      Nom type_so = "Source_QC_Chaleur_";
      Nom disc = discretisation().que_suis_je();
      if (disc=="VEFPreP1B") disc = "VEF";
      type_so += disc;
      so.typer_direct(type_so);
      so->associer_eqn(*this);
      Cerr<<so->que_suis_je()<<finl;
      Cerr << "Typing of the convection operator : ";
      return 1;
    }
  else if (mot=="mode_calcul_convection")
    {
      if (terme_convectif.non_nul())
        {
          Cerr<<" The option "<<mot<<" must be indicated before the convection operator."<<finl;
          Cerr<<" Please modify your data set."<<finl;
          exit();
        }
      Motcles modes(3);
      modes[0]="ancien"; // Default mode before 1.6.8 (wrong in VEF)
      modes[1]="divuT_moins_Tdivu"; // OK in VDF and VEF
      modes[2]="divrhouT_moins_Tdivrhou"; // Default mode after 1.6.8 (better)
      Motcle mot2;
      is>>mot2;
      mode_convection_=modes.search(mot2);
      if (mode_convection_==0)
        {
          Cerr << "WARNING! This mode is not supported anymore." << finl;
        }
      if (mode_convection_==-1)
        {
          Cerr<<" Allowed keywords for option mode_calcul_convection are :"<<finl;
          Cerr<<modes<<finl;
          exit();
        }
      return 1;
    }
  else
    return Convection_Diffusion_Chaleur_Fluide_Dilatable_base::lire_motcle_non_standard(mot,is);
  return 1;
}

const Champ_base& Convection_Diffusion_Chaleur_QC::vitesse_pour_transport() const
{
  if (mode_convection_==2) return Convection_Diffusion_Chaleur_Fluide_Dilatable_base::vitesse_pour_transport();
  else
    {
      const Probleme_base& pb = probleme();
      const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std,pb.equation(0));
      return eqn_hydr.inconnue() /* u */ ;
    }
}

void Convection_Diffusion_Chaleur_QC::calculer_div_u_ou_div_rhou(DoubleTab& Div) const
{
  // No convective operator:
  if (sub_type(Op_Conv_negligeable,operateur(1).l_op_base()))
    {
      Div=0;
      return;
    }

  if (mode_convection_!=0)
    Convection_Diffusion_Fluide_Dilatable_Proto::calculer_div_rho_u_impl(Div,*this);
  else
    {
      // Compute Div([rho]u) with Divergence operator
      const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std,probleme().equation(0));
      const DoubleTab& pression = eqn_hydr.pression().valeurs();
      const Champ_Inc& vitesse = eqn_hydr.inconnue();

      // Div = Div([rho]u) located on pressure nodes:
      DoubleTab Div_on_pressure_nodes(pression);
      eqn_hydr.operateur_divergence().calculer(vitesse.valeurs(),Div_on_pressure_nodes);

      // Test on the discretization:
      if (vitesse.valeurs().line_size() > 1) // VEF
        {
          // VEF (temperature and pressure are not the same) =>  Div extrapolated on velocity nodes:
          DoubleTab Div_on_temperature_nodes(inconnue().valeurs());
          Div_on_pressure_nodes.echange_espace_virtuel();
          ref_cast_non_const(Fluide_Quasi_Compressible,le_fluide.valeur()).divu_discvit(Div_on_pressure_nodes,Div_on_temperature_nodes);
          const int nsom = Div_on_temperature_nodes.dimension_tot(0);
          for (int i=0 ; i<nsom ; i++) Div(i)=Div_on_temperature_nodes(i);
        }
      else// VDF (temperature and pressure nodes are the same):
        Div = Div_on_pressure_nodes;

      Div *= -1;
    }
}

int Convection_Diffusion_Chaleur_QC::preparer_calcul()
{
  Convection_Diffusion_Chaleur_Fluide_Dilatable_base::preparer_calcul();
  return mode_convection_ == 0 ? 1 :
         Convection_Diffusion_Chaleur_Fluide_Dilatable_base::remplir_cl_modifiee();
}
