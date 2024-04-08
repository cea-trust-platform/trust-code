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

#ifndef Pb_Concentration_Gen_included
#define Pb_Concentration_Gen_included

#include <Convection_Diffusion_Concentration.h>
#include <Pb_Thermohydraulique.h>
#include <Champ_Uniforme.h>
#include <Constituant.h>
#include <EChaine.h>
#include <sstream>
#include <iomanip>

template <typename _DERIVED_TYPE_, typename _EQUATION_TYPE_, typename _MEDIUM_TYPE_>
class Pb_Concentration_Gen : public _DERIVED_TYPE_
{
protected:
  std::vector<_MEDIUM_TYPE_> mil_constituants_;
  LIST(_EQUATION_TYPE_) list_eq_concentration_;
  int nb_consts_ = -123;

  Sortie& printOn(Sortie& os) const override { return _DERIVED_TYPE_::printOn(os); }

  inline int nb_equations_multi() { return list_eq_concentration_.size(); }
  inline int nb_equations_multi() const { return list_eq_concentration_.size(); }

public:
  int nombre_d_equations() const override
  {
    return (_DERIVED_TYPE_::nombre_d_equations() + nb_equations_multi());
  }

  const Equation_base& equation(int i) const override
  {
    if (i >= nombre_d_equations())
      Process::exit("Error in Pb_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::equation => wrong equation number !");

    const int nb_eq_mere = _DERIVED_TYPE_::nombre_d_equations();
    if (i < nb_eq_mere)
      return _DERIVED_TYPE_::equation(i);
    else
      return list_eq_concentration_(i - nb_eq_mere);
  }

  Equation_base& equation(int i) override
  {
    if (i >= nombre_d_equations())
      Process::exit("Error in Pb_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::equation => wrong equation number !");

    const int nb_eq_mere = _DERIVED_TYPE_::nombre_d_equations();
    if (i < nb_eq_mere)
      return _DERIVED_TYPE_::equation(i);
    else
      return list_eq_concentration_(i - nb_eq_mere);
  }

  void rename_equation_unknown(const int i, _EQUATION_TYPE_& eqn)
  {
    if (nb_consts_ > 1)
      {
        Nom nom_inco = "concentration", nom_eq = eqn.que_suis_je();
        nom_inco += Nom(i), nom_eq += Nom(i);
        Cerr << "The unknown name of the " << eqn.que_suis_je() << " equation " << i << " is modified => " << nom_inco << finl;
        eqn.inconnue()->nommer(nom_inco);
        Cerr << "The " << eqn.que_suis_je() << " equation name, of number " << i << ", is modified => " << nom_eq << finl;
        eqn.nommer(nom_eq);
      }
  }

  Entree& lire_resize_medium(Entree& is)
  {
    _DERIVED_TYPE_::milieu_vect().resize(2);
    for (int i = 0; i < 2; i++) is >> _DERIVED_TYPE_::milieu_vect()[i];
    _DERIVED_TYPE_::associer_milieu_base(_DERIVED_TYPE_::milieu_vect().front().valeur()); // NS : On l'associe a chaque equations (methode virtuelle pour chaque pb ...)

    /*
     * XXX : Elie Saikali :
     *  - le_milieu_[0] => Fluide incompressible et le_milieu_[1] => constituants (n compos).
     *  - le_milieu_[1] pas associe a l'equation car n compos
     *  - this->mil_constituants_ contient le milieu associe a chaque equation
     */
    const Constituant& les_consts = ref_cast(_MEDIUM_TYPE_, _DERIVED_TYPE_::milieu_vect().back().valeur());
    nb_consts_ = les_consts.nb_constituants();
    mil_constituants_.resize(nb_consts_);

    if (!sub_type(Champ_Uniforme, les_consts.diffusivite_constituant().valeur()))
      {
        Cerr << "Error in Pb_Concentration_Gen<_DERIVED_TYPE_, _EQUATION_TYPE_, _MEDIUM_TYPE_>::lire_resize_medium. You can not use a diffusion coefficient of type " << les_consts.diffusivite_constituant()->que_suis_je() << " !!!" << finl;
        Cerr << "We only accept uniform fields for the moment ... Fix your data set or call the 911 !!!" << finl;
        Process::exit();
      }
    return is;
  }

  void create_constituants_echaines()
  {
    const Constituant& les_consts = ref_cast(_MEDIUM_TYPE_, _DERIVED_TYPE_::milieu_vect().back().valeur());
    const DoubleTab& vals = les_consts.diffusivite_constituant()->valeurs();

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
  }
};

#endif /* Pb_Concentration_Gen_included */
