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

#include <Frontiere_dis_base.h>
#include <Discret_Thermique.h>
#include <Probleme_base.h>
#include <Conduction.h>
#include <Solide.h>
#include <Param.h>

Implemente_instanciable(Conduction,"Conduction",Equation_base);
// XD Conduction eqn_base Conduction -1 Heat equation.

Sortie& Conduction::printOn(Sortie& s ) const
{
  return s << que_suis_je() << finl;
}

Entree& Conduction::readOn(Entree& is )
{
  Equation_base::readOn(is);
  terme_diffusif.set_fichier("Diffusion_chaleur");
  terme_diffusif.set_description((Nom)"Conduction heat transfer rate=Integral(lambda*grad(T)*ndS) [W] if SI units used");
  return is;
}

void Conduction::set_param(Param& param)
{
  Equation_base::set_param(param);
  param.ajouter_non_std("diffusion",(this));
  param.ajouter_condition("is_read_diffusion","The diffusion operator must be read, select negligeable type if you want to neglect it.");
  param.ajouter_non_std("Traitement_particulier",(this));
}

//  Modification par rapport a Conduction::lire_motcle_non_standard()!
int Conduction::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  //Motcle motlu;
  if (mot=="diffusion")
    {
      Cerr << "Reading and typing of the diffusion operator : " << finl;

      terme_diffusif.associer_diffusivite(diffusivite_pour_transport());
      is >> terme_diffusif;
      // le champ pour le dt_stab est le meme que celui de l'operateur. On prend toujours la diffusivite()
      // comme dans le cas de Conduction
      terme_diffusif.associer_diffusivite_pour_pas_de_temps(milieu().diffusivite());
      solveur_masse->set_name_of_coefficient_temporel("rho_cp_comme_T");

      return 1;
    }
  else if (mot=="Traitement_particulier")
    {
      Cerr << "Reading and typing of the Traitement_particulier object : " << finl;
      Nom type="Traitement_particulier_";
      Motcle motbidon;
      is >> motbidon;
      if (motbidon == "{")
        {
          Motcle le_cas;
          is >> le_cas;
          if (le_cas == "}")
            le_cas ="";
          else
            {
              type+= le_cas;
              type+= "_";
            }
          Nom discr=discretisation().que_suis_je();
          if (discr == "VEFPreP1B")
            discr = "VEF";
          type+=discr;
          Cerr << type << finl;
          le_traitement_particulier.typer(type);
          le_traitement_particulier->associer_eqn(*this);
          le_traitement_particulier->lire(is);
          le_traitement_particulier->preparer_calcul_particulier();
          return 1;
        }
      else
        {
          Cerr << "Error while reading Traitement_particulier" << finl;
          Cerr << "A { is expected." << finl;
          exit();
          return -1;
        }
    }
  else
    return Equation_base::lire_motcle_non_standard(mot,is);
}

// retourne la *conductivite* et non la diffusivite comme dans Conduction
const Champ_Don_base& Conduction::diffusivite_pour_transport() const
{
  return milieu().conductivite();
}

const Champ_base& Conduction::diffusivite_pour_pas_de_temps() const
{
  return terme_diffusif.diffusivite();
}

/*! @brief Associe un milieu physique a l'equation, le milieu est en fait caste en Solide.
 *
 * @param (Milieu_base& un_milieu)
 */
void Conduction::associer_milieu_base(const Milieu_base& le_milieu)
{
  associer_solide(ref_cast(Solide,le_milieu));
}

/*! @brief Associe le milieu solide a l'equation.
 *
 * @param (Solide& un_solide) le milieu solide a associer a l'equation
 */
void Conduction::associer_solide(const Solide& un_solide)
{
  le_solide = un_solide;
}

/*! @brief Renvoie le nombre d'operateurs de l'equation pour l'equation de conduction standart renvoie toujours 1.
 *
 * @return (int) le nombre d'operateurs de l'equation
 */
int Conduction::nombre_d_operateurs() const
{
  return 1;
}

/*! @brief Renvoie l'operateur d'index specifie de l'equation.
 *
 * Renvoie terme_diffusif si i=0
 *      exit si i>0
 *     (version const)
 *
 * @param (int i) index de l'operateur a renvoyer
 * @return (Operateur&) l'operateur d'index specifie (uniquement terme_diffusif)
 * @throws l'equation de conduction standard ne contient
 * qu'un operateur
 */
const Operateur& Conduction::operateur(int i) const
{
  if (i == 0)
    return terme_diffusif;
  else
    {
      Cerr << "Conduction::operateur("<<i<<") !! " << finl;
      Cerr << "Equation Conduction has only one operator." << finl;
      exit();
    }
  // Pour les compilos!!
  return terme_diffusif;
}

/*! @brief Renvoie l'operateur d'index specifie de l'equation.
 *
 * Renvoie terme_diffusif si i=0
 *      exit si i>0
 *
 * @param (int i) index de l'operateur a renvoyer
 * @return (Operateur&) l'operateur d'index specifie (uniquement terme_diffusif)
 * @throws l'equation de conduction standard ne contient
 * qu'un operateur
 */
Operateur& Conduction::operateur(int i)
{
  if (i == 0)
    return terme_diffusif;
  else
    {
      Cerr << "Conduction::operateur("<<i<<") !! " << finl;
      Cerr << "Equation Conduction has only one operator." << finl;
      exit();
    }
  // Pour les compilos!!
  return terme_diffusif;
}

/*! @brief Discretise l'equation
 *
 */
void Conduction::discretiser()
{
  const Discret_Thermique& dis=ref_cast(Discret_Thermique, discretisation());
  Cerr << "Conduction equation discretization" << finl;
  dis.temperature(schema_temps(), domaine_dis(), la_temperature);
  champs_compris_.ajoute_champ(la_temperature);
  Equation_base::discretiser();
}


/*! @brief Renvoie le milieu physique associe a l'equation.
 *
 * Ici Solide upcaste en Milieu_base.
 *     (version const)
 *
 * @return (Milieu_base&) le milieu physique associe a l'equation (Solide upcaste en Milieu_base)
 */
const Milieu_base& Conduction::milieu() const
{
  return solide();
}

/*! @brief Renvoie le milieu physique associe a l'equation.
 *
 * Ici Solide upcaste en Milieu_base.
 *
 * @return (Milieu_base&) le milieu physique associe a l'equation (Solide upcaste en Milieu_base)
 */
Milieu_base& Conduction::milieu()
{
  return solide();
}

/*! @brief Renvoie le milieu solide associe a l'equation.
 *
 * (version const)
 *
 * @return (Solide&) le milieu solide associe a l'equation
 * @throws pas de milieu solide associe a l'equation
 */
const Solide& Conduction::solide() const
{
  if(le_solide.est_nul())
    {
      Cerr << "You forgot to associate the solid to the problem named " << probleme().le_nom() << finl;
      Process::exit();
    }
  return le_solide.valeur();
}

void Conduction::creer_champ(const Motcle& motlu)
{
  if (motlu == "temperature_paroi" || motlu == "wall_temperature")
    {
      if (temperature_paroi_.est_nul())
        {
          const Discret_Thermique& dis = ref_cast(Discret_Thermique, discretisation());
          dis.t_paroi(domaine_dis(), domaine_Cl_dis(), la_temperature, temperature_paroi_);
          champs_compris_.ajoute_champ(temperature_paroi_);
        }
    }

  Equation_base::creer_champ(motlu);
}

bool Conduction::has_champ(const Motcle& nom, OBS_PTR(Champ_base) &ref_champ) const
{
  if (nom == "temperature_paroi" || nom == "wall_temperature")
    {
      ref_champ = Conduction::get_champ(nom);
      return true;
    }

  if (Equation_base::has_champ(nom, ref_champ))
    return true;

  if (le_traitement_particulier.non_nul())
    if (le_traitement_particulier->has_champ(nom, ref_champ))
      return true;

  return false; /* rien trouve */
}

bool Conduction::has_champ(const Motcle& nom) const
{
  if (nom == "temperature_paroi" || nom == "wall_temperature")
    return true;

  if (Equation_base::has_champ(nom))
    return true;

  if (le_traitement_particulier.non_nul())
    if (le_traitement_particulier->has_champ(nom))
      return true;

  return false; /* rien trouve */
}

const Champ_base& Conduction::get_champ(const Motcle& nom) const
{
  OBS_PTR(Champ_base) ref_champ;

  if (nom == "temperature_paroi" || nom == "wall_temperature")
    {
      double temps_init = schema_temps().temps_init();
      Champ_Fonc_base& ch_tp = ref_cast_non_const(Champ_Fonc_base, temperature_paroi_.valeur());
      if (((ch_tp.temps() != la_temperature->temps()) || (ch_tp.temps() == temps_init)) && ((la_temperature->mon_equation_non_nul())))
        ch_tp.mettre_a_jour(la_temperature->temps());
      return champs_compris_.get_champ(nom);
    }

  if (Equation_base::has_champ(nom, ref_champ))
    return ref_champ;

  if (le_traitement_particulier.non_nul())
    if (le_traitement_particulier->has_champ(nom, ref_champ))
      return ref_champ;

  throw std::runtime_error(std::string("Field ") + nom.getString() + std::string(" not found !"));
}

void Conduction::get_noms_champs_postraitables(Noms& nom, Option opt) const
{
  Equation_base::get_noms_champs_postraitables(nom, opt);

  Noms noms_compris = champs_compris_.liste_noms_compris();
  noms_compris.add("TEMPERATURE_PAROI");
  noms_compris.add("WALL_TEMPERATURE");

  if (opt == DESCRIPTION)
    Cerr << "Conduction : " << noms_compris << finl;
  else
    nom.add(noms_compris);

  if (le_traitement_particulier.non_nul())
    le_traitement_particulier->get_noms_champs_postraitables(nom, opt);
}

/*! @brief Renvoie le milieu solide associe a l'equation.
 *
 * @return (Solide&) le milieu solide associe a l'equation
 * @throws pas de milieu solide associe a l'equation
 */
Solide& Conduction::solide()
{
  if(!le_solide.non_nul())
    {
      Cerr << "A solide medium has not been associated to a Conduction equation"<<finl;
      exit();
    }
  return le_solide.valeur();
}

/*! @brief Imprime le terme diffusif sur un flot de sortie.
 *
 * @param (Sortie& os) un flot de sortie
 * @return (int) renvoie toujours 1
 */
int Conduction::impr(Sortie& os) const
{
  return Equation_base::impr(os);
}

/*! @brief Renvoie le nom du domaine d'application de l'equation.
 *
 * Ici "Thermique".
 *
 * @return (Motcle&) le nom du domaine d'application de l'equation
 */
const Motcle& Conduction::domaine_application() const
{
  static Motcle domaine = "Thermique";
  return domaine;
}

void Conduction::mettre_a_jour(double temps)
{
  Equation_base::mettre_a_jour(temps);

  if (le_traitement_particulier.non_nul())
    le_traitement_particulier->post_traitement_particulier();
}
