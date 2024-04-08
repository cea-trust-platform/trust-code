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

#include <Operateur_Diff.h>
#include <Verif_Cl.h>

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
void TRUSTProblem_List_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::typer_lire_milieu(Entree& is)
{
  this->lire_resize_medium(is);
  add_concentration_equations(); // XXX
  this->create_constituants_echaines();

  auto& list_eqs = this->list_eq_concentration_.get_stl_list();

  int ind_list = 0;
  for (auto &&itr : list_eqs)
    {
      itr.associer_milieu_base(this->mil_constituants_[ind_list]);
      ind_list++;
    }

  // Milieux lus ... Lets go ! On discretise les equations
  const int nb_eq_mere = _DERIVED_TYPE_::nombre_d_equations();
  Cerr << "Discretization of the equations of problem " << _DERIVED_TYPE_::que_suis_je() << " ..." <<  finl;
  for (int i = 0; i < this->nombre_d_equations(); i++)
    {
      this->equation(i).associer_domaine_dis(_DERIVED_TYPE_::domaine_dis());
      this->equation(i).discretiser();

      if (i >= nb_eq_mere && this->nb_consts_ > 1)
        this->rename_equation_unknown(i - nb_eq_mere, this->list_eq_concentration_[i - nb_eq_mere]); // Et oui ;)
    }

  // remontee de l'inconnue vers le milieu
  for (int i = 0; i < this->nombre_d_equations(); i++)
    this->equation(i).associer_milieu_equation();

  // On discretise les milieux !
  this->equation(0).milieu().discretiser((*this), _DERIVED_TYPE_::discretisation()); // NS

  ind_list = 0;
  for (auto &itr : list_eqs)
    {
      Nom nom_const = "coefficient_diffusion";
      if(this->nb_consts_ > 1) nom_const += Nom(ind_list);
      ref_cast(_MEDIUM_TYPE_, itr.milieu()).discretiser_multi_concentration(nom_const, (*this), _DERIVED_TYPE_::discretisation()); // Conc
      ind_list++;
    }
}

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
void TRUSTProblem_List_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::add_concentration_equations()
{
  for (int i = 0; i < this->nb_consts_; i++)
    {
      this->list_eq_concentration_.add(_EQUATION_TYPE_());
      _EQUATION_TYPE_ &eqn = this->list_eq_concentration_.dernier();
      Cerr << "Adding equation " << eqn.que_suis_je() << " to the list ... " << finl;
      eqn.associer_pb_base(*this);
      eqn.associer_sch_tps_base(_DERIVED_TYPE_::schema_temps());
    }
}

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
Entree& TRUSTProblem_List_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::lire_equations(Entree& is, Motcle& dernier_mot)
{
  Nom un_nom;
  const int nb_eq_mere = _DERIVED_TYPE_::nombre_d_equations(), nb_eq = this->nombre_d_equations();
  Cerr << "Reading of the equations ..." << finl;

  for (int i = 0; i < nb_eq_mere; i++)
    {
      is >> un_nom;
      is >> _DERIVED_TYPE_::getset_equation_by_name(un_nom);
    }

  for (int i = nb_eq_mere; i < nb_eq; i++)
    {
      is >> un_nom; /* poubelle */
      Motcle un_nom_maj = Motcle(un_nom), eq_nom_maj = this->list_eq_concentration_.front().que_suis_je();
      if (un_nom_maj != eq_nom_maj)
        {
          Cerr << "What() ?? Error in TRUSTProblem_List_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::lire_equations !!!" << finl;
          Cerr << "You defined " << this->nb_consts_ << " diffusion coefficients so we are supposed to read " << this->nb_consts_ << " equations ..." << finl;
          Cerr << "We expected to read " << eq_nom_maj << " and not " << un_nom_maj << " !!!" << finl;
          Cerr << "Correct your data file or call the 911 !!!" << finl;
          Process::exit();
        }

      is >> this->list_eq_concentration_[i - nb_eq_mere];
    }

  is >> dernier_mot;
  return is;
}

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
void TRUSTProblem_List_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::associer_milieu_base(const Milieu_base& mil)
{
  if (sub_type(_MEDIUM_TYPE_, mil)) { /* Do nothing */ }
  else _DERIVED_TYPE_::associer_milieu_base(mil);
}

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
int TRUSTProblem_List_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::verifier()
{
  _DERIVED_TYPE_::verifier();
  const Domaine_Cl_dis& domaine_Cl_hydr = this->equation(0).domaine_Cl_dis();

  auto& list_eqs = this->list_eq_concentration_.get_stl_list();
  for (auto &itr : list_eqs)
    {
      const Domaine_Cl_dis& domaine_Cl_co = itr.domaine_Cl_dis();
      tester_compatibilite_hydr_concentration(domaine_Cl_hydr, domaine_Cl_co);
    }

  return 1;
}

#endif /* Pb_List_Concentration_Gen_TPP_included */
