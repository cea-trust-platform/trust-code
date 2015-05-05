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
// File:        Transport_Flux_Chaleur_Turbulente.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Transport_Flux_Chaleur_Turbulente.h>
#include <Modele_turbulence_scal_Fluctuation_Temperature.h>
#include <Discret_Thermique.h>
#include <Les_Pb_Turb.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Transport_Flux_Chaleur_Turbulente,"Transport_Flux_Chaleur_Turbulente",Convection_Diffusion_std);

Transport_Flux_Chaleur_Turbulente::Transport_Flux_Chaleur_Turbulente()
{
  /*
    Noms& nom=champs_compris_.liste_noms_compris();
    nom.dimensionner(1);
    nom[0]="Flux_Chaleur_Turbulente";
  */
}
// printOn et readOn

Sortie& Transport_Flux_Chaleur_Turbulente::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

// Description:
//  cf Convection_Diffusion_std::readOn(is)
Entree& Transport_Flux_Chaleur_Turbulente::readOn(Entree& is )
{
  Convection_Diffusion_std::readOn(is);
  if (les_sources.est_vide())
    {
      // Ajout automatique du terme source
      Source t;
      Source& so=les_sources.add(t);
      const Probleme_base& pb = probleme();
      Cerr << "Construction and typing for the source term of the Transport_Flux_Chaleur_Turbulente transport equation." << finl;
      if (sub_type(Pb_Thermohydraulique_Turbulent,pb))
        {
          Nom typ = "Source_Transport_Flux_Chaleur_Turbulente";
          so.typer(typ,*this);
        }
      so->associer_eqn(*this);
    }
  return is;
}

void Transport_Flux_Chaleur_Turbulente::set_param(Param& param)
{
  Convection_Diffusion_std::set_param(param);
}

int Transport_Flux_Chaleur_Turbulente::lire_motcle_non_standard(const Motcle& mot, Entree& is)
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

// Description:
// surcharge de la methode d'Equation_base.
// renvoie le nombre d'operateurs.
int Transport_Flux_Chaleur_Turbulente::nombre_d_operateurs() const
{
  return 2;
}

// Description:
// surcharge de la methode d'Equation_base.
// renvoie le ieme operateur.
const Operateur& Transport_Flux_Chaleur_Turbulente::operateur(int i) const
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
        Cerr << "Error for Transport_Flux_Chaleur_Turbulente::operateur(int i)" << finl;
        Cerr << "Transport_Flux_Chaleur_Turbulente has " << nombre_d_operateurs() <<" operators "<<finl;
        Cerr << "and you are trying to access the " << i <<" th one."<<finl;
        exit();
      }
    }
  return terme_diffusif;
}

// Description:
// surcharge de la methode d'Equation_base.
// renvoie le ieme operateur.
Operateur& Transport_Flux_Chaleur_Turbulente::operateur(int i)
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
        Cerr << "Error for Transport_Flux_Chaleur_Turbulente::operateur(int i)" << finl;
        Cerr << "Transport_Flux_Chaleur_Turbulente has " << nombre_d_operateurs() <<" operators "<<finl;
        Cerr << "and you are trying to access the " << i <<" th one."<<finl;
        exit();
      }
    }
  return terme_diffusif;
}

void Transport_Flux_Chaleur_Turbulente::completer()
{
  Cerr << "Method Transport_Flux_Chaleur_Turbulente::completer() " << finl;
  les_sources.completer();
  inconnue().associer_eqn(*this);
  for(int i=0; i<nombre_d_operateurs(); i++)
    {
      operateur(i).completer();
    }
  la_zone_Cl_dis->completer();
  // Supression car le completer fait appel au 2eme completer desormais
  //la_zone_Cl_dis->completer(zone_dis());
}

void Transport_Flux_Chaleur_Turbulente::discretiser()
{
  const Discret_Thermique& dis = ref_cast(Discret_Thermique,discretisation());
  Cerr <<que_suis_je()<<" equation discretization" << finl;
  dis.Flux_Chaleur_Turb(schema_temps(),zone_dis(),le_Flux_Chaleur_Turbulente);
  champs_compris_.ajoute_champ(le_Flux_Chaleur_Turbulente);
  Equation_base::discretiser();
}

void Transport_Flux_Chaleur_Turbulente::associer_milieu_base(const Milieu_base& un_milieu)
{
  le_fluide = ref_cast(Fluide_Incompressible, un_milieu) ;
}

const Milieu_base& Transport_Flux_Chaleur_Turbulente::milieu() const
{
  if(!le_fluide.non_nul())
    {
      Cerr << "No fluid has been associated to"
           << "Transport_Flux_Chaleur_Turbulente equation." << finl;
      exit();
    }
  return le_fluide.valeur();
}

Milieu_base& Transport_Flux_Chaleur_Turbulente::milieu()
{
  if(!le_fluide.non_nul())
    {
      Cerr << "No fluid has been associated to"
           << "Transport_Flux_Chaleur_Turbulente equation." << finl;
      exit();
    }
  return le_fluide.valeur();
}



void Transport_Flux_Chaleur_Turbulente::associer_modele_turbulence(const Modele_turbulence_scal_Fluctuation_Temperature& modele)
{
  const Equation_base& eqn_therm = modele.equation();
  Equation_base::associer_pb_base(eqn_therm.probleme());
  Equation_base::associer_sch_tps_base(eqn_therm.schema_temps());
  Equation_base::associer_zone_dis(eqn_therm.zone_dis());
  associer_milieu_base(eqn_therm.milieu());
  associer_vitesse(eqn_therm.inconnue());
  mon_modele_fluctu = modele;
  discretiser();
}

const Motcle& Transport_Flux_Chaleur_Turbulente::domaine_application() const
{
  static Motcle domaine = "Thermique";
  return domaine;
}

int Transport_Flux_Chaleur_Turbulente::controler_grandeur()
{
  static const double LIM = 1.e+2;
  DoubleTab& chaleur = le_Flux_Chaleur_Turbulente.valeurs();
  int size= chaleur.dimension(0);
  for (int n=0; n<size; n++)
    {
      if ( (chaleur(n) >= LIM) || (chaleur(n) <= -LIM) )
        chaleur(n) = 1.e-3;
    }
  return 1;
}
