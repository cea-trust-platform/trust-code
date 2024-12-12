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

#include <Convection_Diffusion_Concentration.h>
#include <Frontiere_dis_base.h>
#include <Navier_Stokes_std.h>
#include <Probleme_base.h>
#include <Discret_Thyd.h>
#include <Constituant.h>
#include <EChaine.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Convection_Diffusion_Concentration,"Convection_Diffusion_Concentration",Convection_Diffusion_std);
// XD convection_diffusion_concentration eqn_base convection_diffusion_concentration -1 Constituent transport vectorial equation (concentration diffusion convection).

Convection_Diffusion_Concentration::Convection_Diffusion_Concentration():nb_constituants_(-1), masse_molaire_(-1.) { }

Sortie& Convection_Diffusion_Concentration::printOn(Sortie& is) const { return Convection_Diffusion_std::printOn(is); }

/*! @brief Verifie si l'equation a une concentration et un constituant associe et appelle Convection_Diffusion_std::readOn(Entree&).
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree& is) le flot d'entree modifie
 */
Entree& Convection_Diffusion_Concentration::readOn(Entree& is)
{
  assert(la_concentration.non_nul());
  Convection_Diffusion_std::readOn(is);
  if (terme_convectif.op_non_nul())
    {
      Nom nom="Convection_";
      nom+=inconnue().le_nom(); // On ajoute le nom de l'inconnue pour prevoir une equation de scalaires passifs
      terme_convectif.set_fichier(nom);
      terme_convectif.set_description((Nom)"Convective mass transfer rate=Integral(-C*u*ndS)[m"+(Nom)(dimension+bidim_axi)+".Mol.s-1]");
    }
  else
    {
      EChaine ech("{ negligeable }");
      ech >> terme_convectif;
    }
  if (terme_diffusif.op_non_nul())
    {
      Nom nom="Diffusion_";
      nom+=inconnue().le_nom();
      terme_diffusif.set_fichier(nom);
      terme_diffusif.set_description((Nom)"Diffusion mass transfer rate=Integral(alpha*grad(C)*ndS) [m"+(Nom)(dimension+bidim_axi)+".Mol.s-1]");
    }
  else
    {
      EChaine ech("{ negligeable }");
      ech >> terme_diffusif;
    }
  return is;
}

// renvoit la masse_molaire, celle ci doit avoir ete lue avant
const double& Convection_Diffusion_Concentration::masse_molaire() const
{
  assert(masse_molaire_>0);
  return masse_molaire_;
}

void Convection_Diffusion_Concentration::set_param(Param& param)
{
  Convection_Diffusion_std::set_param(param);
  param.ajouter_non_std("nom_inconnue",(this)); // XD_ADD_P chaine Keyword Nom_inconnue will rename the unknown of this equation with the given name. In the postprocessing part, the concentration field will be accessible with this name. This is usefull if you want to track more than one concentration (otherwise, only the concentration field in the first concentration equation can be accessed).
  param.ajouter_non_std("alias",(this)); // XD_ADD_P chaine not_set
  param.ajouter("masse_molaire",&masse_molaire_); // XD_ADD_P floattant not_set
  param.ajouter_non_std("is_multi_scalar|is_multi_scalar_diffusion", (this)); // XD_ADD_P rien Flag to activate the multi_scalar diffusion operator
}

int Convection_Diffusion_Concentration::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="nom_inconnue")
    {
      Motcle nom; // Question: veut-on le mettre en majuscules ?
      is >> nom;
      Cerr << "The unknow of a Convection_Diffusion_Concentration equation is renamed"
           << "\n Old name : " << inconnue().le_nom()
           << "\n New name : " << nom << finl;
      inconnue().nommer(nom);
      champs_compris_.ajoute_champ(la_concentration);
      return 1;
    }
  else if (mot=="alias")
    {
      Motcle nom; // Question: veut-on le mettre en majuscules ?
      is >> nom;
      Cerr << "nom_inconnue: On renomme l'equation et son inconnue"
           << "\n Ancien nom : " << inconnue().le_nom()
           << "\n Nouveau nom : " << nom << finl;
      inconnue().nommer(nom);
      champs_compris_.ajoute_champ(la_concentration);
      return 1;
    }
  else if (mot=="is_multi_scalar" || mot=="is_multi_scalar_diffusion")
    {
      diffusion_multi_scalaire_ = true;
      return 1;
    }
  else
    return Convection_Diffusion_std::lire_motcle_non_standard(mot,is);
}

/*! @brief Associe un milieu physique a l'equation, le milieu est en fait caste en Constituant et associe a l'equation.
 *
 * @param (Milieu_base& un_milieu)
 * @throws diffusivite du constituant dans le fluide non definie
 */
void Convection_Diffusion_Concentration::associer_milieu_base(const Milieu_base& un_milieu)
{
  const Constituant& un_constituant = ref_cast(Constituant,un_milieu);
  associer_constituant(un_constituant);
}

const Champ_Don_base& Convection_Diffusion_Concentration::diffusivite_pour_transport() const
{
  return constituant().diffusivite_constituant();
}

/*! @brief Discretise l'equation.
 *
 */
void Convection_Diffusion_Concentration::discretiser()
{
  assert(le_constituant.non_nul());
  const Discret_Thyd& dis = ref_cast(Discret_Thyd, discretisation());
  Cerr << "Transport concentration(s) equation discretization " << finl;
  nb_constituants_ = constituant().nb_constituants();
  dis.concentration(schema_temps(), domaine_dis(), la_concentration, nb_constituants_);
  champs_compris_.ajoute_champ(la_concentration);
  Equation_base::discretiser();
}

/*! @brief Renvoie le milieu physique de l'equation.
 *
 * (un Constituant upcaste en Milieu_base)
 *     (version const)
 *
 * @return (Milieu_base&) le Constituant upcaste en Milieu_base
 */
const Milieu_base& Convection_Diffusion_Concentration::milieu() const
{
  return constituant();
}


/*! @brief Renvoie le milieu physique de l'equation.
 *
 * (un Constituant upcaste en Milieu_base)
 *
 * @return (Milieu_base&) le Constituant upcaste en Milieu_base
 */
Milieu_base& Convection_Diffusion_Concentration::milieu()
{
  return constituant();
}


/*! @brief Renvoie le constituant (si il a ete associe).
 *
 * (version const)
 *
 * @return (Constituant&) le constituant associe a l'equation
 */
const Constituant& Convection_Diffusion_Concentration::constituant() const
{
  if(!le_constituant.non_nul())
    {
      Cerr << "You forgot to associate the constituent to the problem named " << probleme().le_nom() << finl;
      Process::exit();
    }
  return le_constituant.valeur();
}


/*! @brief Renvoie le constituant (si il a ete associe).
 *
 * @return (Constituant&) le constituant associe a l'equation
 */
Constituant& Convection_Diffusion_Concentration::constituant()
{
  if(!le_constituant.non_nul())
    {
      Cerr << "No constituant has been associated "
           << "with a Convection_Diffusion_Concentration equation." << finl;
      exit();
    }
  return le_constituant.valeur();
}

int Convection_Diffusion_Concentration::preparer_calcul()
{
  Equation_base::preparer_calcul();
  double temps=schema_temps().temps_courant();
  constituant().initialiser(temps);
  return 1;
}

void Convection_Diffusion_Concentration::mettre_a_jour(double temps)
{
  Equation_base::mettre_a_jour(temps);
  constituant().mettre_a_jour(temps);
}

/*! @brief Impression des flux sur les bords sur un flot de sortie.
 *
 * Appelle Equation_base::impr(Sortie&)
 *
 * @param (Sortie& os) un flot de sortie
 * @return (int) code de retour propage
 */
int Convection_Diffusion_Concentration::impr(Sortie& os) const
{
  return Equation_base::impr(os);
}

/*! @brief Renvoie 1 si le mot clef specifie designe un type de champ inconnue de l'equation.
 *
 *     Renvoie 1 si mot = "concentration"
 *     Renvoie 0 sinon
 *     Si la methode renvoie 1 ch_ref fait reference au champ, dont
 *     le type a ete specifie.
 *
 * @param (Motcle& mot) le type du champ dont on veut recuperer la reference
 * @param (OBS_PTR(Champ_base)& ch_ref) la reference sur le champ du type specifie
 * @return (int) renvoie 1 si le champ a ete trouve, 0 sinon
 */
inline int string2int(char* digit, int& result)
{
  result = 0;

  //--- Convert each digit char and add into result.
  while (*digit >= '0' && *digit <='9')
    {
      result = (result * 10) + (*digit - '0');
      digit++;
    }

  //--- Check that there were no non-digits at end.
  if (*digit != 0)
    {
      return 0;
    }

  return 1;
}



/*! @brief Renvoie le nom du domaine d'application de l'equation.
 *
 * Ici "Concentration".
 *
 * @return (Motcle&) le nom du domaine d'application de l'equation
 */
const Motcle& Convection_Diffusion_Concentration::domaine_application() const
{
  static Motcle domaine = "Concentration";
  return domaine;
}

