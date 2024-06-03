/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Convection_Diffusion_std.h>
#include <Probleme_base.h>
#include <Milieu_base.h>
#include <Param.h>

Implemente_base(Convection_Diffusion_std,"Convection_Diffusion_standard",Equation_base);

/*! @brief Simple appel a Equation_base::printOn(Sortie&)
 *
 * @param (Sortie& is) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Convection_Diffusion_std::printOn(Sortie& is) const
{
  return Equation_base::printOn(is);
}


/*! @brief cf Equation_base::readOn(Entree&)
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 * @throws terme diffusif non specifie
 * @throws terme convectif non specifie
 */
Entree& Convection_Diffusion_std::readOn(Entree& is)
{
  Equation_base::readOn(is);
  return is;
}

void Convection_Diffusion_std::set_param(Param& param)
{
  Equation_base::set_param(param);
  param.ajouter_non_std("diffusion",(this));
  param.ajouter_non_std("convection",(this));
}


int Convection_Diffusion_std::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="diffusion")
    {
      Cerr << "Reading and typing of the diffusion operator : " << finl;
      terme_diffusif.associer_diffusivite(diffusivite_pour_transport());
      is >> terme_diffusif;
      terme_diffusif.associer_diffusivite_pour_pas_de_temps(diffusivite_pour_pas_de_temps());
      return 1;
    }
  else if (mot=="convection")
    {
      Cerr << "Reading and typing of the convection operator : " << finl;
      const Champ_base& ch_vitesse_transportante = vitesse_pour_transport();
      associer_vitesse(ch_vitesse_transportante);
      terme_convectif.associer_vitesse(ch_vitesse_transportante);
      is >> terme_convectif;
      return 1;
    }
  else
    return Equation_base::lire_motcle_non_standard(mot,is);
}
/*! @brief Renvoie le nombre d'operateurs de l'equation: 2 pour une equation de diffusion.
 *
 * @return (int) le nombre d'operateurs de l'equation
 */
int Convection_Diffusion_std::nombre_d_operateurs() const
{
  return 2;
}

/*! @brief Renvoie l'operateur specifie par son index: renvoie terme_diffusif si i = 0
 *
 *      renvoie terme_convectif si i = 1
 *      exit si i>1
 *     (version const)
 *
 * @param (int i) l'index de l'operateur a renvoyer
 * @return (Operateur&) l'operateur specifie
 * @throws l'equation n'a pas plus de 2 operateurs
 */
const Operateur& Convection_Diffusion_std::operateur(int i) const
{
  switch(i)
    {
    case 0:
      return terme_diffusif;
    case 1:
      return terme_convectif;
    default :
      Cerr << "Error for Convection_Diffusion_std::operateur(int i)" << finl;
      Cerr << "Convection_Diffusion_std has " << nombre_d_operateurs() <<" operators "<<finl;
      Cerr << "and you are trying to access the " << i <<" th one."<< finl;
      exit();
    }
  // Pour les compilos!!
  return terme_diffusif;
}

/*! @brief Renvoie l'operateur specifie par son index: renvoie terme_diffusif si i = 0
 *
 *      renvoie terme_convectif si i = 1
 *      exit si i>1
 *
 * @param (int i) l'index de l'operateur a renvoyer
 * @return (Operateur&) l'operateur specifie
 * @throws l'equation n'a pas plus de 2 operateurs
 */
Operateur& Convection_Diffusion_std::operateur(int i)
{
  switch(i)
    {
    case 0:
      return terme_diffusif;
    case 1:
      return terme_convectif;
    default :
      Cerr << "Error for Convection_Diffusion_std::operateur(int i)" << finl;
      Cerr << "Convection_Diffusion_std has " << nombre_d_operateurs() <<" operators "<<finl;
      Cerr << "and you are trying to access the " << i <<" th one."<< finl;
      exit();
    }
  // Pour les compilos!!
  return terme_diffusif;
}

const Champ_Don& Convection_Diffusion_std::diffusivite_pour_transport() const
{
  return milieu().diffusivite();
}

const Champ_base& Convection_Diffusion_std::diffusivite_pour_pas_de_temps() const
{
  return terme_diffusif.diffusivite();
}

const Champ_base& Convection_Diffusion_std::vitesse_pour_transport() const
{
  return probleme().get_champ("vitesse");
}

// E. Saikali : Methodes utiles pour un heritage V
int Convection_Diffusion_std::sauvegarder_base(Sortie& os) const
{
  return Equation_base::sauvegarder(os);
}
int Convection_Diffusion_std::reprendre_base(Entree& is)
{
  return Equation_base::reprendre(is);
}
