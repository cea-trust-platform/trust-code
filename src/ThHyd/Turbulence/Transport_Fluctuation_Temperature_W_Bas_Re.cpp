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
// File:        Transport_Fluctuation_Temperature_W_Bas_Re.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Transport_Fluctuation_Temperature_W_Bas_Re.h>
#include <Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re.h>
#include <Les_Pb_Turb.h>
#include <Param.h>

Implemente_instanciable(Transport_Fluctuation_Temperature_W_Bas_Re,"Transport_Fluctuation_Temperature_W_Bas_Re",Transport_Fluctuation_Temperature_W);

// printOn et readOn

Sortie& Transport_Fluctuation_Temperature_W_Bas_Re::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

// Description:
//  cf Transport_Fluctuation_Temperature_W::readOn(is)
Entree& Transport_Fluctuation_Temperature_W_Bas_Re::readOn(Entree& is )
{
  Transport_Fluctuation_Temperature_W::readOn(is);
  if (les_sources.est_vide())
    {
      // Ajout automatique du terme source
      Source t;
      Source& so=les_sources.add(t);
      const Probleme_base& pb = probleme();
      Cerr << "Construction and typing for the source term of the Transport_Fluctuation_Temperature_W_Bas_Re transport equation." << finl;
      if (sub_type(Pb_Thermohydraulique_Turbulent,pb))
        {
          Nom typ = "Source_Transport_Fluctuation_Temperature_W_Bas_Re";
          so.typer(typ,*this);
        }
      else
        {
          Cerr << "A problem of type " <<que_suis_je()<<" cannot be considered "<<finl;
          Cerr << "with a Transport_Fluctuation_Temperature_W_Bas_Re equation." <<finl;
          exit();
        }
      so->associer_eqn(*this);
    }
  return is;
}

void Transport_Fluctuation_Temperature_W_Bas_Re::set_param(Param& param)
{
  Transport_Fluctuation_Temperature_W::set_param(param);
}

int Transport_Fluctuation_Temperature_W_Bas_Re::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="diffusion")
    {
      Cerr << "Reading and typing of the diffusion operator : " << finl;
      is >> terme_diffusif;
      return 1;
    }
  else
    return Transport_Fluctuation_Temperature_W::lire_motcle_non_standard(mot,is);
  return 1;
}

// Description:
// surcharge de la methode d'Equation_base.
// renvoie le ieme operateur.
const Operateur& Transport_Fluctuation_Temperature_W_Bas_Re::operateur(int i) const
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
        Cerr << "Error for Transport_Fluctuation_Temperature_W_Bas_Re::operateur(int i)" << finl;
        Cerr << "Transport_Fluctuation_Temperature_W_Bas_Re has " << nombre_d_operateurs() <<" operators "<<finl;
        Cerr << "and you are trying to access the " << i <<" th one."<< finl;
        exit();
      }
    }
  return terme_diffusif;
}

// Description:
// surcharge de la methode d'Equation_base.
// renvoie le ieme operateur.
Operateur& Transport_Fluctuation_Temperature_W_Bas_Re::operateur(int i)
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
        Cerr << "Error for Transport_Fluctuation_Temperature_W_Bas_Re::operateur(int i)" << finl;
        Cerr << "Transport_Fluctuation_Temperature_W_Bas_Re has " << nombre_d_operateurs() <<" operators "<<finl;
        Cerr << "and you are trying to access the " << i <<" th one."<< finl;
        exit();
      }
    }
  return terme_diffusif;
}

void Transport_Fluctuation_Temperature_W_Bas_Re::associer_modele_turbulence(const Modele_turbulence_scal_Fluctuation_Temperature_W_Bas_Re& modele)
{
  const Equation_base& eqn_therm = modele.equation();
  Equation_base::associer_pb_base(eqn_therm.probleme());
  Equation_base::associer_sch_tps_base(eqn_therm.schema_temps());
  Equation_base::associer_zone_dis(eqn_therm.zone_dis());
  associer_milieu_base(eqn_therm.milieu());
  mon_modele_Fluctu_Temp = modele;
  discretiser();
}


