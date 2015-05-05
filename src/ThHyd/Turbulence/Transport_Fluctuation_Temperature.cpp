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
// File:        Transport_Fluctuation_Temperature.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Transport_Fluctuation_Temperature.h>
#include <Modele_turbulence_scal_Fluctuation_Temperature.h>
#include <Discret_Thermique.h>
#include <Les_Pb_Turb.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Transport_Fluctuation_Temperature,"Transport_Fluctuation_Temperature",Convection_Diffusion_std);

Transport_Fluctuation_Temperature::Transport_Fluctuation_Temperature()
{
  /*
    Noms& nom=champs_compris_.liste_noms_compris();
    nom.dimensionner(3);
    nom[0]="variance_temperature";
    nom[1]="taux_dissipation_temperature";
    nom[2]="Fluctu_Temperature";
  */
}

// printOn et readOn

Sortie& Transport_Fluctuation_Temperature::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

// Description:
//  cf Convection_Diffusion_std::readOn(is)
Entree& Transport_Fluctuation_Temperature::readOn(Entree& is )
{
  Convection_Diffusion_std::readOn(is);
  if (les_sources.est_vide())
    {
      // Ajout automatique du terme source
      Source t;
      Source& so=les_sources.add(t);
      const Probleme_base& pb = probleme();
      Cerr << "Construction and typing for the source term of the Transport_Fluctuation_Temperature transport equation." << finl;
      if (sub_type(Pb_Thermohydraulique_Turbulent,pb))
        {
          Nom typ = "Source_Transport_Fluctuation_Temperature";
          so.typer(typ,*this);
        }
      so->associer_eqn(*this);
    }
  return is;
}

void Transport_Fluctuation_Temperature::set_param(Param& param)
{
  Convection_Diffusion_std::set_param(param);
}

int Transport_Fluctuation_Temperature::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="diffusion")
    {
      Cerr << "Reading and typing of the diffusion operator : " << finl;
      is >> terme_diffusif;
      return 1;
    }
  else
    return Convection_Diffusion_std::lire_motcle_non_standard(mot,is);
  return 1;
}

void Transport_Fluctuation_Temperature::discretiser()
{
  const Discret_Thermique& dis = ref_cast(Discret_Thermique,discretisation());
  Cerr << "Transport_Fluctuation_Temperature equation discretization" << finl;
  dis.Fluctu_Temperature(schema_temps(),zone_dis(),le_champ_Fluctu_Temperature);
  champs_compris_.ajoute_champ(le_champ_Fluctu_Temperature);
  Equation_base::discretiser();
}

void Transport_Fluctuation_Temperature::completer()
{
  // Source t;
  //   Source& so=les_sources.add(t);
  les_sources.completer();
  inconnue().associer_eqn(*this);
  for(int i=0; i<nombre_d_operateurs(); i++)
    {
      operateur(i).completer();
    }
  la_zone_Cl_dis->completer();
  // Suppression car le completer fait desormais appel au 2eme completer
  //la_zone_Cl_dis->completer(zone_dis());
}

// Description:
// surcharge de la methode d'Equation_base.
// renvoie le nombre d'operateurs.
int Transport_Fluctuation_Temperature::nombre_d_operateurs() const
{
  return 2;
}

// Description:
// surcharge de la methode d'Equation_base.
// renvoie le ieme operateur.
const Operateur& Transport_Fluctuation_Temperature::operateur(int i) const
{
  assert(i>=0);
  assert(i<nombre_d_operateurs());

  switch(i)
    {
    case 0:
      {
        return  terme_diffusif;
      }
    case 1:
      {
        return  terme_convectif;
      }
    default :
      {
        Cerr << "Error for Transport_Fluctuation_Temperature::operateur(int i)" << finl;
        Cerr << "Transport_Fluctuation_Temperature has " << nombre_d_operateurs() <<" operators "<<finl;
        Cerr << "and you are trying to access the " << i <<" th one."<< finl;
        exit();
      }
    }
  return terme_diffusif;
}

// Description:
// surcharge de la methode d'Equation_base.
// renvoie le ieme operateur.
Operateur& Transport_Fluctuation_Temperature::operateur(int i)
{
  assert(i>=0);
  assert(i<nombre_d_operateurs());

  switch(i)
    {
    case 0:
      {
        return  terme_diffusif;
      }
    case 1:
      {
        return  terme_convectif;
      }
    default :
      {
        Cerr << "Error for Transport_Fluctuation_Temperature::operateur(int i)" << finl;
        Cerr << "Transport_Fluctuation_Temperature has " << nombre_d_operateurs() <<" operators "<<finl;
        Cerr << "and you are trying to access the " << i <<" th one."<< finl;
        exit();
      }
    }
  return terme_diffusif;
}

// Description:
// association entre  Transport_Fluctuation_Temperature et Milieu_base.
// affecte le_Milieu_base.
void Transport_Fluctuation_Temperature::associer_milieu_base(const Milieu_base& un_milieu)
{
  le_fluide = ref_cast(Fluide_Incompressible, un_milieu) ;
}

const Milieu_base& Transport_Fluctuation_Temperature::milieu() const
{
  if(!le_fluide.non_nul())
    {
      Cerr << "No fluid has been associated to"
           << "Transport_Fluctuation_Temperature equation." << finl;
      exit();
    }
  return le_fluide.valeur();
}

Milieu_base& Transport_Fluctuation_Temperature::milieu()
{
  if(!le_fluide.non_nul())
    {
      Cerr << "No fluid has been associated to"
           << "Transport_Fluctuation_Temperature equation." << finl;
      exit();
    }
  return le_fluide.valeur();
}


const Motcle& Transport_Fluctuation_Temperature::domaine_application() const
{
  static Motcle domaine = "Thermique";
  return domaine;
}

void Transport_Fluctuation_Temperature::associer_modele_turbulence(const Modele_turbulence_scal_Fluctuation_Temperature& modele)
{
  const Equation_base& eqn_therm = modele.equation();
  Equation_base::associer_pb_base(eqn_therm.probleme());
  Equation_base::associer_sch_tps_base(eqn_therm.schema_temps());
  Equation_base::associer_zone_dis(eqn_therm.zone_dis());
  associer_milieu_base(eqn_therm.milieu());
  mon_modele_Fluctu_Temp=modele;
  discretiser();
}

int Transport_Fluctuation_Temperature::controler_grandeur()
{
  static const double LIM = 1.e-10;
  DoubleTab& fluctuation = le_champ_Fluctu_Temperature.valeurs();
  int size=fluctuation.dimension(0);
  for (int n=0; n<size; n++)
    {
      if ( fluctuation(n,0) <= LIM)
        fluctuation(n,0) = 0.;
      if ( fluctuation(n,1) <= LIM)
        fluctuation(n,1)= 0.;
    }
  return 1;
}
