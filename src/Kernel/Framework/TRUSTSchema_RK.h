/****************************************************************************
* Copyright (c) 2022, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        TRUSTSchema_RK.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/63
//
//////////////////////////////////////////////////////////////////////////////

#ifndef TRUSTSchema_RK_included
#define TRUSTSchema_RK_included

#include<Schema_Temps_base.h>
#include <Equation.h>

template<bool B, typename T>
using enable_if_t = typename std::enable_if<B, T>::type;

using ARR1 = std::array<double, 1>; // OK je sais mais bon ... ne demande pas alors :-)
using ARR2 = std::array<double, 2>;
using ARR3 = std::array<double, 3>;
using ARR4 = std::array<double, 4>;

enum class Ordre_RK { UN , RATIO_DEUX , DEUX_CLASSIQUE , DEUX_WILLIAMSON , TROIS_CLASSIQUE , TROIS_WILLIAMSON , QUATRE_CLASSIQUE , QUATRE_CLASSIQUE_3_8 , QUATRE_WILLIAMSON };

template <Ordre_RK _ORDRE_ >
class TRUSTSchema_RK : public Schema_Temps_base
{
  // Renvoie le nombre de valeurs temporelles a conserver. Ici : n et n+1, donc 2.
  int nb_valeurs_temporelles() const override { return 2 ; }

  // Renvoie le nombre de valeurs temporelles futures. Ici : n+1, donc 1.
  int nb_valeurs_futures() const override { return 1 ; }

  // Renvoie le le temps a la i-eme valeur future. Ici : t(n+1)
  double temps_futur(int i) const override
  {
    assert(i == 1);
    return temps_courant() + pas_de_temps();
  }

  // Renvoie le le temps le temps que doivent rendre les champs a l'appel de valeurs(). Ici : t(n+1)
  double temps_defaut() const override { return temps_courant() + pas_de_temps(); }

  // a surcharger si utile
  void completer() override { /* Do nothing */ }

  friend class RK3_FT; // pour trio
  int faire_un_pas_de_temps_eqn_base(Equation_base& eq) override { return faire_un_pas_de_temps_eqn_base_generique<_ORDRE_>(eq); } // SFINAE :-)

protected:
  static constexpr int NW = 100;

  inline void print_warning(const int nw)
  {
    Cerr << finl << "**** Advice (printed only on the first " << nw << " time steps) ****" << finl;
    Cerr << "You are using Runge Kutta schema ! If you wish to increase the time step, try facsec between 1 and 2/3/4 (depends on the order of the scheme)." << finl;
  }

private:
  static constexpr double SQRT2 = sqrt(2.), SQRT2_2 = SQRT2 / 2.;

  // RK low storage : See Williamson RK series https://www.sciencedirect.com/science/article/pii/0021999180900339
  static constexpr ARR2 A2 = { 0.0, SQRT2 - 2. };
  static constexpr ARR2 B2 = { SQRT2_2, SQRT2_2 };

  static constexpr ARR3 A3 = { 0.0, -5. / 9., -153. / 128. };
  static constexpr ARR3 B3 = { 1. / 3., 15. / 16., 8. / 15. };

  static constexpr ARR3 A4 = { 0.0, -1. /2. , -2. };
  static constexpr ARR3 B4 = { 1. / 2., 1. , 1. / 6. };

  // RK CLASSIQUE : see https://en.wikipedia.org/wiki/List_of_Runge%E2%80%93Kutta_methods
  static constexpr std::array<ARR1, 1> BUTCHER_2 = { { { 1. / 2. } } }; /* RK2 */
  static constexpr std::array<ARR2, 2> BUTCHER_3 = { { { 1. / 2., 0. }, { -1., 2. } } }; /* RK3 */
  static constexpr std::array<ARR3, 3> BUTCHER_4 = { { { 1. / 2., 0. , 0.}, { 0., 1. / 2., 0.}, { 0. , 0., 1. } } }; /* RK4 */
  static constexpr std::array<ARR3, 3> BUTCHER_4_3_8 = { { { 1. / 3., 0. , 0.}, { -1. / 3., 1., 0.}, { 1. , -1., 1. } } }; /* RK4 3/8 rule */

  static constexpr std::array<ARR4, 4> BUTCHER_TAB = { {
      { 0., 1., 0., 0. }, /* RK2 classique*/
      { 1. / 6., 2. / 3., 1. / 6., 0. }, /* RK3 classique*/
      { 1. / 6., 1. / 3., 1. / 3., 1. / 6. }, /* RK4 classique*/
      { 1. / 8., 3. / 8., 3. / 8., 1. / 8. } /* RK4 3/8 rule */
    }
  };

  // SFINAE template functions
  template<Ordre_RK _O_ = _ORDRE_, int NB>
  enable_if_t<_O_ == Ordre_RK::DEUX_WILLIAMSON, std::array<double, NB>>
  inline const get_a() { return A2; }

  template<Ordre_RK _O_ = _ORDRE_, int NB>
  enable_if_t<_O_ != Ordre_RK::DEUX_WILLIAMSON, std::array<double, NB>>
  inline const get_a() { return _ORDRE_ == Ordre_RK::TROIS_WILLIAMSON ? A3 : A4; }

  template<Ordre_RK _O_ = _ORDRE_, int NB>
  enable_if_t<_O_ == Ordre_RK::DEUX_WILLIAMSON, std::array<double, NB>>
  inline const get_b() { return B2; }

  template<Ordre_RK _O_ = _ORDRE_, int NB>
  enable_if_t<_O_ != Ordre_RK::DEUX_WILLIAMSON, std::array<double, NB>>
  inline const get_b() { return _ORDRE_ == Ordre_RK::TROIS_WILLIAMSON ? B3 : B4; }

  template<Ordre_RK _O_ = _ORDRE_>
  enable_if_t<_O_ == Ordre_RK::DEUX_WILLIAMSON || _O_ == Ordre_RK::TROIS_WILLIAMSON || _O_ == Ordre_RK::QUATRE_WILLIAMSON, int>
  faire_un_pas_de_temps_eqn_base_generique(Equation_base& eq);

  template<Ordre_RK _O_ = _ORDRE_, int NB>
  enable_if_t<_O_ == Ordre_RK::DEUX_CLASSIQUE, std::array<std::array<double, NB>, NB> >
  inline const get_butcher_coeff() { return BUTCHER_2; }

  template<Ordre_RK _O_ = _ORDRE_, int NB>
  enable_if_t<_O_ == Ordre_RK::TROIS_CLASSIQUE, std::array<std::array<double, NB>, NB> >
  inline const get_butcher_coeff() { return BUTCHER_3; }

  template<Ordre_RK _O_ = _ORDRE_, int NB>
  enable_if_t<_O_ == Ordre_RK::QUATRE_CLASSIQUE || _O_ == Ordre_RK::QUATRE_CLASSIQUE_3_8, std::array<std::array<double, NB>, NB> >
  inline const get_butcher_coeff() { return _O_ == Ordre_RK::QUATRE_CLASSIQUE ? BUTCHER_4 : BUTCHER_4_3_8; }

  template<Ordre_RK _O_ = _ORDRE_>
  enable_if_t<_O_ == Ordre_RK::DEUX_CLASSIQUE || _O_ == Ordre_RK::TROIS_CLASSIQUE || _O_ == Ordre_RK::QUATRE_CLASSIQUE || _O_ == Ordre_RK::QUATRE_CLASSIQUE_3_8, int>
  faire_un_pas_de_temps_eqn_base_generique(Equation_base& eq);

  // DANGER : SHOULD NOT GO HERE
  template<Ordre_RK _O_ = _ORDRE_> enable_if_t<_O_ == Ordre_RK::UN || _O_ == Ordre_RK::RATIO_DEUX, int>
  faire_un_pas_de_temps_eqn_base_generique(Equation_base& eq) { throw; } // From VTABLE
};

// XXX : CAN BE REMOVED WHEN WE PASS TO C++17
// see https://stackoverflow.com/questions/40690260/undefined-reference-error-for-static-constexpr-member
template <Ordre_RK _ORDRE_ >
constexpr std::array<ARR1, 1> TRUSTSchema_RK<_ORDRE_>::BUTCHER_2;

template <Ordre_RK _ORDRE_ >
constexpr std::array<ARR2, 2> TRUSTSchema_RK<_ORDRE_>::BUTCHER_3;

template <Ordre_RK _ORDRE_ >
constexpr std::array<ARR3, 3> TRUSTSchema_RK<_ORDRE_>::BUTCHER_4;

template <Ordre_RK _ORDRE_ >
constexpr std::array<ARR3, 3> TRUSTSchema_RK<_ORDRE_>::BUTCHER_4_3_8;

template <Ordre_RK _ORDRE_ >
constexpr std::array<ARR4, 4> TRUSTSchema_RK<_ORDRE_>::BUTCHER_TAB;

#include <TRUSTSchema_RK.tpp>

#endif /* TRUSTSchema_RK_included */
