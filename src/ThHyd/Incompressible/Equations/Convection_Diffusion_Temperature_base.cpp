/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Convection_Diffusion_Temperature_base.h>
#include <Probleme_base.h>
#include <Discret_Thyd.h>

Implemente_base(Convection_Diffusion_Temperature_base, "Convection_Diffusion_Temperature_base", Convection_Diffusion_std);

Sortie& Convection_Diffusion_Temperature_base::printOn(Sortie& is) const { return Convection_Diffusion_std::printOn(is); }

Entree& Convection_Diffusion_Temperature_base::readOn(Entree& is) { return Convection_Diffusion_std::readOn(is); }

const Fluide_base& Convection_Diffusion_Temperature_base::fluide() const
{
  if(le_fluide.est_nul())
    {
      Cerr << "You forgot to associate the fluid to the problem named " << probleme().le_nom() << finl;
      Process::exit();
    }
  return le_fluide.valeur();
}


/*! @brief Renvoie le fluide associe a l'equation.
 *
 * @return (Fluide_base&) le fluide associe a l'equation
 * @throws pas de fluide associe a l'eqaution
 */
Fluide_base& Convection_Diffusion_Temperature_base::fluide()
{
  if(le_fluide.est_nul())
    {
      Cerr << "You forgot to associate the fluid to the problem named " << probleme().le_nom() << finl;
      Process::exit();
    }
  return le_fluide.valeur();
}

/*! @brief Associe un milieu physique a l'equation, le milieu est en fait caste en Fluide_base.
 *
 * @param (Milieu_base& un_milieu)
 * @throws le milieu n'est pas un Fluide_base
 */
void Convection_Diffusion_Temperature_base::associer_milieu_base(const Milieu_base& un_milieu)
{
  if (sub_type(Fluide_base,un_milieu)) associer_fluide(ref_cast(Fluide_base, un_milieu));
  else Process::exit(que_suis_je() + " : le fluide " + un_milieu.que_suis_je() + " n'est pas de type Fluide_base!");
}

void Convection_Diffusion_Temperature_base::creer_champ(const Motcle& motlu)
{
  Convection_Diffusion_std::creer_champ(motlu);

  if (motlu == "temperature_paroi" || motlu == "wall_temperature")
    {
      if (temperature_paroi_.est_nul())
        {
          const Discret_Thermique& dis = ref_cast(Discret_Thermique, discretisation());
          dis.t_paroi(domaine_dis(), domaine_Cl_dis(), inconnue(), temperature_paroi_);
          champs_compris_.ajoute_champ(temperature_paroi_);
        }
    }
}

bool Convection_Diffusion_Temperature_base::has_champ(const Motcle& nom, OBS_PTR(Champ_base)& ref_champ) const
{
  if (nom == "temperature_paroi" || nom == "wall_temperature")
    {
      ref_champ = Convection_Diffusion_Temperature_base::get_champ(nom);
      return true;
    }

  if (Convection_Diffusion_std::has_champ(nom))
    return Convection_Diffusion_std::has_champ(nom, ref_champ);

  return false; /* rien trouve */
}

bool Convection_Diffusion_Temperature_base::has_champ(const Motcle& nom) const
{
  if (nom == "temperature_paroi" || nom == "wall_temperature")
    return true;

  if (Convection_Diffusion_std::has_champ(nom))
    return true;

  return false; /* rien trouve */
}

const Champ_base& Convection_Diffusion_Temperature_base::get_champ(const Motcle& nom) const
{
  if (nom == "temperature_paroi" || nom == "wall_temperature")
    {
      double temps_init = schema_temps().temps_init();
      Champ_Fonc_base& ch_tp = ref_cast_non_const(Champ_Fonc_base, temperature_paroi_.valeur());
      if (((ch_tp.temps() != inconnue().temps()) || (ch_tp.temps() == temps_init)) && ((inconnue().mon_equation_non_nul())))
        ch_tp.mettre_a_jour(inconnue().temps());
      return champs_compris_.get_champ(nom);
    }

  OBS_PTR(Champ_base) ref_champ;

  if (Convection_Diffusion_std::has_champ(nom, ref_champ))
    return ref_champ;

  throw std::runtime_error(std::string("Field ") + nom.getString() + std::string(" not found !"));
}

void Convection_Diffusion_Temperature_base::get_noms_champs_postraitables(Noms& nom, Option opt) const
{
  Convection_Diffusion_std::get_noms_champs_postraitables(nom, opt);

  Noms noms_compris = champs_compris_.liste_noms_compris();
  noms_compris.add("TEMPERATURE_PAROI");
  noms_compris.add("WALL_TEMPERATURE");

  if (opt == DESCRIPTION)
    Cerr << que_suis_je() << " : " << noms_compris << finl;
  else
    nom.add(noms_compris);
}
