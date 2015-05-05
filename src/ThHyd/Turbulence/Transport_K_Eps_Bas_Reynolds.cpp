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
// File:        Transport_K_Eps_Bas_Reynolds.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/24
//
//////////////////////////////////////////////////////////////////////////////

#include <Transport_K_Eps_Bas_Reynolds.h>
#include <Modele_turbulence_hyd_K_Eps_Bas_Reynolds.h>
#include <Les_Pb_Turb.h>
#include <Modele_turbulence_scal_Fluctuation_Temperature_W.h>
#include <Param.h>

Implemente_instanciable(Transport_K_Eps_Bas_Reynolds,"Transport_K_Eps_Bas_Reynolds",Transport_K_Eps_non_std);

// printOn et readOn

Sortie& Transport_K_Eps_Bas_Reynolds::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Transport_K_Eps_Bas_Reynolds::readOn(Entree& is )
{
  // Lecture des attributs de l'equation
  Transport_K_Eps_non_std::readOn(is);
  return is;
}

void Transport_K_Eps_Bas_Reynolds::completer()
{
  // Ajout automatique du terme source
  if (les_sources.est_vide())
    {
      Source t;
      Source& so=les_sources.add(t);
      const Probleme_base& pb = probleme();
      Cerr << "Construction and typing for the source term of the Transport_K_Eps_Bas_Reynolds equation." << finl;
      if (sub_type(Pb_Hydraulique_Turbulent,pb) || sub_type(Pb_Thermohydraulique_Turbulent_QC,pb))
        {
          Nom typ = "Source_Transport_K_Eps_Bas_Reynolds";
          so.typer(typ,*this);
        }
      else if (sub_type(Pb_Thermohydraulique_Turbulent,pb))
        {
          const Equation_base& eqTemp = probleme().equation(1);
          const Modele_turbulence_scal_base& le_mod_turb_th = ref_cast(Modele_turbulence_scal_base,eqTemp.get_modele(TURBULENCE).valeur());
          if ( sub_type(Modele_turbulence_scal_Fluctuation_Temperature_W,le_mod_turb_th) )
            {
              Nom typ = "Source_Transport_K_Eps_Bas_Reynolds_anisotherme_W";
              so.typer(typ,*this);
            }
          else
            {
              Nom typ = "Source_Transport_K_Eps_Bas_Reynolds_anisotherme";
              so.typer(typ,*this);
            }
          //      Cerr << "pb = " << probleme().que_suis_je() << finl;
          //      Cerr << "Eq de temp = " << eqTemp.que_suis_je() << finl;
          //      Cerr << "modele turb en temperature = " <<  eqTemp.modele_turbulence() << finl;

        }
      else if (sub_type(Pb_Hydraulique_Concentration_Turbulent,pb))
        {
          Nom typ = "Source_Transport_K_Eps_Bas_Reynolds_aniso_concen";
          so.typer(typ,*this);
        }
      else if (sub_type(Pb_Thermohydraulique_Concentration_Turbulent,pb))
        {
          Nom typ = "Source_Transport_K_Eps_Bas_Reynolds_aniso_therm_concen";
          so.typer(typ,*this);
        }
      else
        {
          Cerr<<"The equation "<<que_suis_je()<<" cannot be associated to a problem "<<pb.que_suis_je()<<finl;
          abort();
        }
      so->associer_eqn(*this);
    }
  Equation_base::completer();
}

void Transport_K_Eps_Bas_Reynolds::associer_modele_turbulence(const Mod_turb_hyd_RANS& modele)
{
  const Equation_base& eqn_hydr = modele.equation();
  associer(eqn_hydr);
  associer_milieu_base(eqn_hydr.milieu());
  associer_vitesse(eqn_hydr.inconnue());
  mon_modele = ref_cast(Modele_turbulence_hyd_K_Eps_Bas_Reynolds,modele);
  discretiser();
}

void Transport_K_Eps_Bas_Reynolds::associer_milieu_base(const Milieu_base& un_milieu)
{
  le_fluide = ref_cast(Fluide_Incompressible, un_milieu);
}

const Motcle& Transport_K_Eps_Bas_Reynolds::domaine_application() const
{
  static Motcle domaine = "Transport_Keps_Bas_Re";
  return domaine;
}

