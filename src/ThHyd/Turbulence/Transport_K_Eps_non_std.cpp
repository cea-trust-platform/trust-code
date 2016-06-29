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
// File:        Transport_K_Eps_non_std.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/4
//
//////////////////////////////////////////////////////////////////////////////

#include <Transport_K_Eps_non_std.h>
#include <Fluide_Incompressible.h>
#include <Probleme_base.h>
#include <Param.h>

Implemente_base(Transport_K_Eps_non_std,"Transport_K_Eps_non_std",Transport_K_Eps_base);

// Description:
// Precondition:
// Parametre: Sortie& is
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Transport_K_Eps_non_std::printOn(Sortie& is) const
{
  return is << que_suis_je() << "\n";

}

// Description:
//    Simple appel a Equation_base::readOn(Entree&)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Transport_K_Eps_non_std::readOn(Entree& is)
{
  Transport_K_Eps_base::readOn(is);
  return is;
}

void Transport_K_Eps_non_std::set_param(Param& param)
{
  Transport_K_Eps_base::set_param(param);
}

int Transport_K_Eps_non_std::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="diffusion")
    {
      Cerr << "Reading and typing of the diffusion operator : " << finl;
      terme_diffusif.associer_diffusivite(diffusivite_pour_transport());
      is >> terme_diffusif;
      return 1;
    }
  else if (mot=="convection")
    {
      Cerr << "Reading and typing of the convection operator : " << finl;
      const Champ_base& vit_transp = vitesse_pour_transport();
      associer_vitesse(vit_transp);
      terme_convectif.associer_vitesse(vit_transp);
      is >> terme_convectif;
      return 1;
    }
  else
    return Transport_K_Eps_base::lire_motcle_non_standard(mot,is);
  return 1;
}

// Retour: int
// Signification: le nombre d'operateurs de l'equation
// Contraintes: toujours egal a 2
int Transport_K_Eps_non_std::nombre_d_operateurs() const
{
  return 2;
}

// Description:
// renvoie terme_diffusif si i=0
// renvoie terme_convectif si i=1
// exit si i>1
const Operateur& Transport_K_Eps_non_std::operateur(int i) const
{
  switch(i)
    {
    case 0:
      return terme_diffusif;
    case 1:
      return terme_convectif;
    default :
      Cerr << "Error for "<<que_suis_je()<<"::operateur("<<i<<") !! " << finl;
      Cerr << que_suis_je()<<" has " << nombre_d_operateurs() <<" operators "<<finl;
      Cerr << "and you are trying to access the " << i <<" th one."<< finl;
      exit();
    }
  // Pour les compilos!!
  return terme_diffusif;
}

// Description:
// renvoie terme_diffusif si i=0
// renvoie terme_convectif si i=1
// exit si i>1
Operateur& Transport_K_Eps_non_std::operateur(int i)
{
  switch(i)
    {
    case 0:
      return terme_diffusif;
    case 1:
      return terme_convectif;
    default :
      Cerr << "Error for "<<que_suis_je()<<"::operateur("<<i<<") !! " << finl;
      Cerr << que_suis_je()<<" has " << nombre_d_operateurs() <<" operators "<<finl;
      Cerr << "and you are trying to access the " << i <<" th one."<< finl;
      exit();
    }
  // Pour les compilos!!
  return terme_diffusif;
}

const Champ_Don& Transport_K_Eps_non_std::diffusivite_pour_transport()
{
  Fluide_Incompressible& fluide_inc = ref_cast(Fluide_Incompressible,le_fluide.valeur());
  return fluide_inc.viscosite_cinematique();
}

const Champ_base& Transport_K_Eps_non_std::vitesse_pour_transport()
{
  const Champ_base& vitesse_transportante = probleme().equation(0).inconnue();
  return vitesse_transportante;
}
