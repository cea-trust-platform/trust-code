/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef Pb_Cloned_Concentration_Gen_TPP_included
#define Pb_Cloned_Concentration_Gen_TPP_included

#include <Op_Diff_negligeable.h>
#include <Champ_Uniforme.h>
#include <Operateur_Diff.h>
#include <Constituant.h>
#include <Verif_Cl.h>
#include <EChaine.h>
#include <sstream>
#include <iomanip>

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
Entree& Pb_Cloned_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::readOn(Entree& is)
{
  /* Step 1 : Add first equation to list ! */
  list_eq_concentration_.add(_EQUATION_TYPE_());
  _EQUATION_TYPE_& eqn = list_eq_concentration_.dernier();
  Cerr << "Adding the first " << eqn.que_suis_je() << " to the list ... " << finl;
  eqn.associer_pb_base(*this);
  eqn.associer_sch_tps_base(_DERIVED_TYPE_::schema_temps());
  eqn.associer_domaine_dis(_DERIVED_TYPE_::domaine_dis());

  /* Step 2 : Go to Probleme_base::readOn !! */
  return _DERIVED_TYPE_::readOn(is);
}

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
void Pb_Cloned_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::typer_lire_milieu(Entree& is)
{
  _DERIVED_TYPE_::milieu_vect().resize(2);
  for (int i = 0; i < 2; i++) is >> _DERIVED_TYPE_::milieu_vect()[i];
  associer_milieu_base(_DERIVED_TYPE_::milieu_vect().front().valeur()); // NS : On l'associe a chaque equations (methode virtuelle pour chaque pb ...)

  /*
   * XXX : Elie Saikali :
   *  - le_milieu_[0] => Fluide incompressible et le_milieu_[1] => constituants (n compos).
   *  - le_milieu_[1] pas associe a l'equation car n compos
   *  - mil_constituants_ contient le milieu associe a chaque equation
   */
  const Constituant& les_consts = ref_cast(_MEDIUM_TYPE_, _DERIVED_TYPE_::milieu_vect().back().valeur());
  const DoubleTab& vals = les_consts.diffusivite_constituant()->valeurs();
  nb_consts_ = les_consts.nb_constituants();
  mil_constituants_.resize(nb_consts_);

  if (!sub_type(Champ_Uniforme, les_consts.diffusivite_constituant().valeur()))
    {
      Cerr << "Error in Pb_Cloned_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::typer_lire_milieu. You can not use a diffusion coefficient of type " << les_consts.diffusivite_constituant()->que_suis_je() << " !!!" << finl;
      Cerr << "We only accept uniform fields for the moment ... Fix your data set !!!" << finl;
      Process::exit();
    }

  for (int i = 0; i < nb_consts_; i++)
    {
      std::ostringstream oss;
      oss << std::scientific << std::setprecision(15) << vals(0, i); // Setting precision to 3 decimal places

      Nom str = "{ coefficient_diffusion Champ_Uniforme 1 ";
      str += oss.str().c_str();
      str += " }";

      EChaine const1(str);
      const1 >> mil_constituants_[i];
    }

  associer_milieu_base(mil_constituants_[0]); // 1er eq concentration pour le moment

  // Milieux lus ... Lets go ! On discretise les equations
  _DERIVED_TYPE_::discretiser_equations();

  // remontee de l'inconnue vers le milieu
  for (int i = 0; i < nombre_d_equations(); i++)
    equation(i).associer_milieu_equation();

  // On discretise les milieux !
  equation(0).milieu().discretiser((*this), _DERIVED_TYPE_::discretisation()); // NS

  const Nom nom_const = nb_consts_ > 1 ? "coefficient_diffusion0" : "coefficient_diffusion";
  ref_cast(_MEDIUM_TYPE_, list_eq_concentration_.dernier().milieu()).discretiser_multi_concentration(nom_const, (*this), _DERIVED_TYPE_::discretisation()); // Conc
}

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
Entree& Pb_Cloned_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::lire_equations(Entree& is, Motcle& dernier_mot)
{
  rename_equation_unknown(0);
  _DERIVED_TYPE_::lire_equations(is, dernier_mot);
  clone_equations();
  return is;
}

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
void Pb_Cloned_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::rename_equation_unknown(const int i)
{
  if (nb_consts_ > 1)
    {
      _EQUATION_TYPE_& eqn = list_eq_concentration_.dernier();
      Nom nom_inco = "concentration", nom_eq = "Convection_Diffusion_Concentration";
      nom_inco += Nom(i), nom_eq += Nom(i);
      Cerr << "The unknown name of the Convection_Diffusion_Concentration equation " << i << " is modified => " << nom_inco << finl;
      eqn.inconnue()->nommer(nom_inco);
      Cerr << "The Convection_Diffusion_Concentration equation name, of number " << i << ", is modified => " << nom_eq << finl;
      eqn.nommer(nom_eq);
    }
}

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
void Pb_Cloned_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::clone_equations()
{
  if (nb_consts_ > 1)
    {
      Cerr << "Cloning " << nb_consts_ << " concentration equations in progress ..." << finl;
      for (int i = 1; i < nb_consts_; i++)
        {
          list_eq_concentration_.add(list_eq_concentration_.front());
          _EQUATION_TYPE_& eqn = list_eq_concentration_.dernier();
          Cerr << "Adding another "<< eqn.que_suis_je() << " to the list ... " << finl;
          eqn.associer_pb_base(*this);
          eqn.associer_sch_tps_base(_DERIVED_TYPE_::schema_temps());
          eqn.associer_domaine_dis(_DERIVED_TYPE_::domaine_dis());

          associer_milieu_base(mil_constituants_[i]); // 1er eq concentration pour le moment
          eqn.associer_milieu_equation();
          const Nom nom_const = Nom("coefficient_diffusion") + Nom(i);
          ref_cast(_MEDIUM_TYPE_, eqn.milieu()).discretiser_multi_concentration(nom_const, (*this), _DERIVED_TYPE_::discretisation()); // Conc

          rename_equation_unknown(i); // XXX

          eqn.get_champ_compris().clear_champs_compris();
          eqn.get_champ_compris().ajoute_champ(eqn.inconnue());

          // XXX : Elie Saikali : that is REQUIRED !!!! Otherwise we use the operators of the original equation !!!
          for (int op = 0; op < eqn.nombre_d_operateurs(); op++)
            {
              eqn.operateur(op).associer_eqn(eqn);
              eqn.operateur(op).l_op_base().associer_eqn(eqn);
            }

          Operateur_Diff& op_diff = dynamic_cast<Operateur_Diff&>(eqn.operateur(0));
          op_diff.associer_diffusivite(eqn.diffusivite_pour_transport());
          op_diff->associer_diffusivite(eqn.diffusivite_pour_transport());
          op_diff.associer_diffusivite_pour_pas_de_temps(eqn.diffusivite_pour_pas_de_temps());
          op_diff->associer_diffusivite_pour_pas_de_temps(eqn.diffusivite_pour_pas_de_temps());

          Nom nom_fich = "Diffusion_";
          nom_fich += eqn.inconnue().le_nom();
          op_diff.set_fichier(nom_fich);
          op_diff.set_description((Nom) "Diffusion mass transfer rate=Integral(alpha*grad(C)*ndS) [m" + (Nom) (Objet_U::dimension + Objet_U::bidim_axi) + ".Mol.s-1]");

          Operateur_Conv& op_conv = dynamic_cast<Operateur_Conv&>(eqn.operateur(1));
          const Champ_base& ch_vitesse_transportante = eqn.vitesse_pour_transport();
          eqn.associer_vitesse(ch_vitesse_transportante);
          op_conv->associer_vitesse(ch_vitesse_transportante);

          // Pareil ici !!!
          eqn.solv_masse().associer_eqn(eqn);
          eqn.solv_masse()->associer_eqn(eqn);
        }
    }
}

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
const Equation_base& Pb_Cloned_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::equation(int i) const
{
  if (i >= nombre_d_equations())
    Process::exit("Error in Pb_Cloned_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::equation => wrong equation number !");

  const int nb_eq_mere = _DERIVED_TYPE_::nombre_d_equations();
  if (i < nb_eq_mere)
    return _DERIVED_TYPE_::equation(i);
  else
    return list_eq_concentration_(i - nb_eq_mere);
}

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
Equation_base& Pb_Cloned_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::equation(int i)
{
  if (i >= nombre_d_equations())
    Process::exit("Error in Pb_Cloned_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::equation => wrong equation number !");

  const int nb_eq_mere = _DERIVED_TYPE_::nombre_d_equations();
  if (i < nb_eq_mere)
    return _DERIVED_TYPE_::equation(i);
  else
    return list_eq_concentration_(i - nb_eq_mere);
}

/*! @brief Associe un milieu au probleme, Si le milieu est de type
 *
 *       - Fluide_Incompressible, il sera associe a l'equation de l'hydraulique et a l'equation d'energie.
 *       - Constituant, il sera associe a l'equation de convection-diffusion
 *     Un autre type de milieu provoque une erreur
 *
 * @param (Milieu_base& mil) le milieu physique a associer au probleme
 * @throws mauvais type de milieu physique
 */
template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
void Pb_Cloned_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::associer_milieu_base(const Milieu_base& mil)
{
  if (sub_type(_MEDIUM_TYPE_, mil))
    list_eq_concentration_.dernier().associer_milieu_base(mil);
  else
    _DERIVED_TYPE_::associer_milieu_base(mil);
}

/*! @brief Teste la compatibilite des equations de convection-diffusion et de l'hydraulique.
 *
 * Le test se fait sur les conditions
 *     aux limites discretisees de chaque equation.
 *     Appels aux fonctions de librairie hors classe:
 *       tester_compatibilite_hydr_thermique(const Domaine_Cl_dis&,const Domaine_Cl_dis&)
 *       tester_compatibilite_hydr_concentration(const Domaine_Cl_dis&,const Domaine_Cl_dis&)
 *
 * @return (int) code de retour propage
 */
template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
int Pb_Cloned_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::verifier()
{
  _DERIVED_TYPE_::verifier();
  const Domaine_Cl_dis& domaine_Cl_hydr = equation(0).domaine_Cl_dis();
  const Domaine_Cl_dis& domaine_Cl_co = list_eq_concentration_.front().domaine_Cl_dis();
  return tester_compatibilite_hydr_concentration(domaine_Cl_hydr, domaine_Cl_co);
}

#endif /* Pb_Cloned_Concentration_Gen_TPP_included */
