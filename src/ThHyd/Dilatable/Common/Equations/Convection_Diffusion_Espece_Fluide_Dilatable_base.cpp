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

#include <Convection_Diffusion_Espece_Fluide_Dilatable_base.h>
#include <Fluide_Dilatable_base.h>
#include <Neumann_sortie_libre.h>
#include <Discret_Thyd.h>

Implemente_base(Convection_Diffusion_Espece_Fluide_Dilatable_base,"Convection_Diffusion_Espece_Fluide_Dilatable_base",Convection_Diffusion_Fluide_Dilatable_base);

Sortie& Convection_Diffusion_Espece_Fluide_Dilatable_base::printOn(Sortie& is) const
{
  return Convection_Diffusion_Fluide_Dilatable_base::printOn(is);
}

Entree& Convection_Diffusion_Espece_Fluide_Dilatable_base::readOn(Entree& is)
{
  return Convection_Diffusion_Fluide_Dilatable_base::readOn(is);
}

int Convection_Diffusion_Espece_Fluide_Dilatable_base::preparer_calcul()
{
  Convection_Diffusion_Fluide_Dilatable_base::preparer_calcul();

  // remplissage de la zone cl modifiee avec 1 partout au bord...
  zcl_modif_=(zone_Cl_dis());

  Conds_lim& condlims=zcl_modif_.valeur().les_conditions_limites();
  int nb=condlims.size();
  for (int i=0; i<nb; i++)
    {
      // pour chaque condlim on recupere le champ_front et on met 1
      // meme si la cond lim est un flux (dans ce cas la convection restera nullle.)
      DoubleTab& T=condlims[i].valeur().champ_front().valeurs();
      T=1.;
      if (sub_type(Neumann_sortie_libre,condlims[i].valeur()))
        ref_cast(Neumann_sortie_libre,condlims[i].valeur()).tab_ext()=1;
    }
  zcl_modif_.les_conditions_limites().set_modifier_val_imp(0);
  return 1;
}

void Convection_Diffusion_Espece_Fluide_Dilatable_base::discretiser()
{
  int nb_valeurs_temp = schema_temps().nb_valeurs_temporelles();
  double temps = schema_temps().temps_courant();
  const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
  Cerr << "Massic fraction equation discretization ..." << finl;
  //On utilise temperature pour la directive car discretisation identique
  dis.discretiser_champ("temperature",zone_dis(),"fraction_massique","sans_dimension",
                        1 /* nb_composantes */,nb_valeurs_temp,temps,l_inco_ch);
  Equation_base::discretiser();
  Cerr << "Convection_Diffusion_Espece_Fluide_Dilatable_base::discretiser() ok" << finl;
}

void Convection_Diffusion_Espece_Fluide_Dilatable_base::calculer_div_u_ou_div_rhou(DoubleTab& Div) const
{
  Convection_Diffusion_Fluide_Dilatable_Proto::calculer_div_rho_u_impl(Div,*this);
}

int Convection_Diffusion_Espece_Fluide_Dilatable_base::sauvegarder(Sortie& os) const
{
  int bytes=0;
  bytes += Equation_base::sauvegarder(os);
  return bytes;
}

int Convection_Diffusion_Espece_Fluide_Dilatable_base::reprendre(Entree& is)
{
  Convection_Diffusion_Fluide_Dilatable_base::reprendre(is);
  return 1;
}

// Description:
//    Renvoie le nom du domaine d'application de l'equation.
//    Ici "Thermique".
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Motcle&
//    Signification: le nom du domaine d'application de l'equation
//    Contraintes: toujours egal a "Thermique"
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Motcle& Convection_Diffusion_Espece_Fluide_Dilatable_base::domaine_application() const
{
  static Motcle domaine ="Fraction_massique";
  return domaine;
}
