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

#ifndef Pb_List_Concentration_Gen_TPP_included
#define Pb_List_Concentration_Gen_TPP_included

#include <Op_Diff_negligeable.h>
#include <Champ_Uniforme.h>
#include <Operateur_Diff.h>
#include <Constituant.h>
#include <Verif_Cl.h>
#include <EChaine.h>
#include <sstream>
#include <iomanip>

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
void Pb_List_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::typer_lire_milieu(Entree& is)
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

  add_concentration_equations(); // XXX

  if (!sub_type(Champ_Uniforme, les_consts.diffusivite_constituant().valeur()))
    {
      Cerr << "Error in Pb_List_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::typer_lire_milieu. You can not use a diffusion coefficient of type " << les_consts.diffusivite_constituant()->que_suis_je() << " !!!" << finl;
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

  auto& list_eqs = list_eq_concentration_.get_stl_list();

  int ind_list = 0;
  for (auto &&itr : list_eqs)
    {
      itr.associer_milieu_base(mil_constituants_[ind_list]);
      ind_list++;
    }

  // Milieux lus ... Lets go ! On discretise les equations
  const int nb_eq_mere = _DERIVED_TYPE_::nombre_d_equations();
  Cerr << "Discretization of the equations of problem " << _DERIVED_TYPE_::que_suis_je() << " ..." <<  finl;
  for (int i = 0; i < nombre_d_equations(); i++)
    {
      equation(i).associer_domaine_dis(_DERIVED_TYPE_::domaine_dis());
      equation(i).discretiser();

      if (i >= nb_eq_mere && nb_consts_ > 1)
        rename_equation_unknown(i - nb_eq_mere); // Et oui ;)
    }

  // remontee de l'inconnue vers le milieu
  for (int i = 0; i < nombre_d_equations(); i++)
    equation(i).associer_milieu_equation();

  // On discretise les milieux !
  equation(0).milieu().discretiser((*this), _DERIVED_TYPE_::discretisation()); // NS

  ind_list = 0;
  for (auto &itr : list_eqs)
    {
      Nom nom_const = "coefficient_diffusion";
      if(nb_consts_ > 1) nom_const += Nom(ind_list);
      ref_cast(_MEDIUM_TYPE_, itr.milieu()).discretiser_multi_concentration(nom_const, (*this), _DERIVED_TYPE_::discretisation()); // Conc
      ind_list++;
    }
}

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
void Pb_List_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::add_concentration_equations()
{
  for (int i = 0; i < nb_consts_; i++)
    {
      list_eq_concentration_.add(_EQUATION_TYPE_());
      _EQUATION_TYPE_ &eqn = list_eq_concentration_.dernier();
      Cerr << "Adding equation " << eqn.que_suis_je() << " to the list ... " << finl;
      eqn.associer_pb_base(*this);
      eqn.associer_sch_tps_base(_DERIVED_TYPE_::schema_temps());
    }
}

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
void Pb_List_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::rename_equation_unknown(const int i)
{
  if (nb_consts_ > 1)
    {
      _EQUATION_TYPE_& eqn = list_eq_concentration_[i];
      Nom nom_inco = "concentration", nom_eq = "Convection_Diffusion_Concentration";
      nom_inco += Nom(i), nom_eq += Nom(i);
      Cerr << "The unknown name of the Convection_Diffusion_Concentration equation " << i << " is modified => " << nom_inco << finl;
      eqn.inconnue()->nommer(nom_inco);
      Cerr << "The Convection_Diffusion_Concentration equation name, of number " << i << ", is modified => " << nom_eq << finl;
      eqn.nommer(nom_eq);
    }
}

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
Entree& Pb_List_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::lire_equations(Entree& is, Motcle& dernier_mot)
{
  Nom un_nom;
  const int nb_eq_mere = _DERIVED_TYPE_::nombre_d_equations(), nb_eq = nombre_d_equations();
  Cerr << "Reading of the equations ..." << finl;

  for (int i = 0; i < nb_eq_mere; i++)
    {
      is >> un_nom;
      is >> _DERIVED_TYPE_::getset_equation_by_name(un_nom);
    }

  for (int i = nb_eq_mere; i < nb_eq; i++)
    {
      is >> un_nom; /* poubelle */
      Motcle un_nom_maj = Motcle(un_nom), eq_nom_maj = list_eq_concentration_.front().que_suis_je();
      if (un_nom_maj != eq_nom_maj)
        {
          Cerr << "What() ?? Error in Pb_List_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::lire_equations !!!" << finl;
          Cerr << "You defined " << nb_consts_ << " diffusion coefficients so we are supposed to read " << nb_consts_ << " equations ..." << finl;
          Cerr << "We expected to read " << eq_nom_maj << " and not " << un_nom_maj << " !!!" << finl;
          Cerr << "Correct your data file or call the 911 !!!" << finl;
          Process::exit();
        }

      is >> list_eq_concentration_[i - nb_eq_mere];
    }

  is >> dernier_mot;
  return is;
}

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
const Equation_base& Pb_List_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::equation(int i) const
{
  if (i >= nombre_d_equations())
    Process::exit("Error in Pb_List_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::equation => wrong equation number !");

  const int nb_eq_mere = _DERIVED_TYPE_::nombre_d_equations();
  if (i < nb_eq_mere)
    return _DERIVED_TYPE_::equation(i);
  else
    return list_eq_concentration_(i - nb_eq_mere);
}

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
Equation_base& Pb_List_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::equation(int i)
{
  if (i >= nombre_d_equations())
    Process::exit("Error in Pb_List_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::equation => wrong equation number !");

  const int nb_eq_mere = _DERIVED_TYPE_::nombre_d_equations();
  if (i < nb_eq_mere)
    return _DERIVED_TYPE_::equation(i);
  else
    return list_eq_concentration_(i - nb_eq_mere);
}

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
void Pb_List_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::associer_milieu_base(const Milieu_base& mil)
{
  if (sub_type(_MEDIUM_TYPE_, mil)) { /* Do nothing */ }
  else _DERIVED_TYPE_::associer_milieu_base(mil);
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
int Pb_List_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::verifier()
{
  _DERIVED_TYPE_::verifier();
  const Domaine_Cl_dis& domaine_Cl_hydr = equation(0).domaine_Cl_dis();

  auto& list_eqs = list_eq_concentration_.get_stl_list();
  for (auto &itr : list_eqs)
    {
      const Domaine_Cl_dis& domaine_Cl_co = itr.domaine_Cl_dis();
      tester_compatibilite_hydr_concentration(domaine_Cl_hydr, domaine_Cl_co);
    }

  return 1;
}

#endif /* Pb_List_Concentration_Gen_TPP_included */
