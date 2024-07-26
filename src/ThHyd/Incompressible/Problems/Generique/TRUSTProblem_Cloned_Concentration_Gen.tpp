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

#include <Operateur_Diff.h>
#include <Verif_Cl.h>

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
Entree& TRUSTProblem_Cloned_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::readOn(Entree& is)
{
  /* Step 1 : Add first equation to list ! */
  this->list_eq_concentration_.add(_EQUATION_TYPE_());
  _EQUATION_TYPE_& eqn = this->list_eq_concentration_.dernier();
  Cerr << "Adding the first " << eqn.que_suis_je() << " to the list ... " << finl;
  eqn.associer_pb_base(*this);
  eqn.associer_sch_tps_base(_DERIVED_TYPE_::schema_temps());
  eqn.associer_domaine_dis(_DERIVED_TYPE_::domaine_dis());

  /* Step 2 : Go to Probleme_base::readOn !! */
  return _DERIVED_TYPE_::readOn(is);
}

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
void TRUSTProblem_Cloned_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::typer_lire_milieu(Entree& is)
{
  this->lire_resize_medium(is);
  this->create_constituants_echaines();

  associer_milieu_base(this->mil_constituants_[0]); // 1er eq concentration pour le moment

  // Milieux lus ... Lets go ! On discretise les equations
  _DERIVED_TYPE_::discretiser_equations();

  // remontee de l'inconnue vers le milieu
  for (int i = 0; i < this->nombre_d_equations(); i++)
    this->equation(i).associer_milieu_equation();

  // On discretise les milieux !
  this->equation(0).milieu().discretiser((*this), _DERIVED_TYPE_::discretisation()); // NS

  const Nom nom_const = this->nb_consts_ > 1 ? "coefficient_diffusion0" : "coefficient_diffusion";
  ref_cast(_MEDIUM_TYPE_, this->list_eq_concentration_.dernier().milieu()).discretiser_multi_concentration(nom_const, (*this), _DERIVED_TYPE_::discretisation()); // Conc
}

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
Entree& TRUSTProblem_Cloned_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::lire_equations(Entree& is, Motcle& dernier_mot)
{
  this->rename_equation_unknown(0, this->list_eq_concentration_.dernier());
  _DERIVED_TYPE_::lire_equations(is, dernier_mot);
  clone_equations();
  return is;
}

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
void TRUSTProblem_Cloned_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::clone_equations()
{
  if (this->nb_consts_ > 1)
    {
      Cerr << "Cloning " << this->nb_consts_ << " concentration equations in progress ..." << finl;
      for (int i = 1; i < this->nb_consts_; i++)
        {
          this->list_eq_concentration_.add(this->list_eq_concentration_.front());
          _EQUATION_TYPE_& eqn = this->list_eq_concentration_.dernier();
          Cerr << "Adding another "<< eqn.que_suis_je() << " to the list ... " << finl;
          eqn.associer_pb_base(*this);
          eqn.associer_sch_tps_base(_DERIVED_TYPE_::schema_temps());
          eqn.associer_domaine_dis(_DERIVED_TYPE_::domaine_dis());

          associer_milieu_base(this->mil_constituants_[i]); // 1er eq concentration pour le moment
          eqn.associer_milieu_equation();
          const Nom nom_const = Nom("coefficient_diffusion") + Nom(i);
          ref_cast(_MEDIUM_TYPE_, eqn.milieu()).discretiser_multi_concentration(nom_const, (*this), _DERIVED_TYPE_::discretisation()); // Conc

          this->rename_equation_unknown(i, eqn); // XXX

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
          eqn.solv_masse()->associer_eqn(eqn);
        }
    }
}

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
void TRUSTProblem_Cloned_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::associer_milieu_base(const Milieu_base& mil)
{
  if (sub_type(_MEDIUM_TYPE_, mil))
    this->list_eq_concentration_.dernier().associer_milieu_base(mil);
  else
    _DERIVED_TYPE_::associer_milieu_base(mil);
}

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
int TRUSTProblem_Cloned_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::verifier()
{
  _DERIVED_TYPE_::verifier();
  const Domaine_Cl_dis& domaine_Cl_hydr = this->equation(0).domaine_Cl_dis();
  const Domaine_Cl_dis& domaine_Cl_co = this->list_eq_concentration_.front().domaine_Cl_dis();
  return tester_compatibilite_hydr_concentration(domaine_Cl_hydr, domaine_Cl_co);
}

#endif /* Pb_Cloned_Concentration_Gen_TPP_included */
