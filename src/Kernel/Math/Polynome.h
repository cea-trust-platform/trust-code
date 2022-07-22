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

#ifndef Polynome_included
#define Polynome_included

#include <TRUSTTab.h>


/*! @brief Polynome a n variables, n <= 4 Implementation des coefficients a l'aide d'un DoubleTab
 *
 */
class Polynome : public Objet_U
{
  Declare_instanciable(Polynome);

public :
  inline Polynome(int n1);
  inline Polynome(int n1, int n2);
  inline Polynome(int n1, int n2, int n3);
  inline Polynome(int n1, int n2, int n3, int n4);
  inline Polynome(const DoubleTab& );
  inline int degre(int ) const;
  inline double& coeff(int n1);
  inline double& coeff(int n1, int n2);
  inline double& coeff(int n1, int n2, int n3);
  inline double& coeff(int n1, int n2, int n3, int n4);
  inline double coeff(int n1) const;
  inline double coeff(int n1, int n2) const;
  inline double coeff(int n1, int n2, int n3) const;
  inline double coeff(int n1, int n2, int n3, int n4) const;
  double operator()(double x1) const;
  double operator()(double x1, double x2) const;
  double operator()(double x1, double x2, double x3) const;
  double operator()(double x1, double x2, double x3, double x4) const;

  void derive(int =0);
  void integre(int =0);

  double derive(double x1) const;
  double derive(double x1, double x2) const;
  double derive(double x1, double x2, double x3) const;
  double derive(double x1, double x2, double x3, double x4) const;

  double integre(double x1) const;
  double integre(double x1, double x2) const;
  double integre(double x1, double x2, double x3) const;
  double integre(double x1, double x2, double x3, double x4) const;

  Polynome& operator +=(const Polynome&);
  Polynome& operator -=(const Polynome&);
  Polynome& operator *=(const Polynome&);
  Polynome& operator *=(double);
  Polynome& operator /=(double);

private :
  DoubleTab coeff_;
};



/*! @brief Construction d'un polynome a une variable de degre n1
 *
 * @param (int n1) degre du polynome
 */
inline Polynome::Polynome(int n1) : coeff_(++n1) {}

/*! @brief Construction d'un polynome a 2 variables de degres n1 et n2
 *
 * @param (int n1) degre de la premiere variable
 * @param (int n2) degre de la deuxieme variable
 */
inline Polynome::Polynome(int n1, int n2) : coeff_(++n1, ++n2) {}

/*! @brief Construction d'un polynome a 3 variables de degres n1, n2 et n3
 *
 * @param (int n1) degre de la premiere variable
 * @param (int n2) degre de la deuxieme variable
 * @param (int n3) degre de la troisieme variable
 */
inline Polynome::Polynome(int n1, int n2, int n3) : coeff_(++n1, ++n2, ++n3) {}

/*! @brief Construction d'un polynome a 4 variables de degres n1, n2, n3 et n4
 *
 * @param (int n1) degre de la premiere variable
 * @param (int n2) degre de la deuxieme variable
 * @param (int n3) degre de la troisieme variable
 * @param (int n4) degre de la quatrieme variable
 */
inline Polynome::Polynome(int n1, int n2, int n3, int n4) : coeff_(++n1, ++n2, ++n3, ++n4) {}

/*! @brief Construction d'un polynome a partir du tableau de ses coefficients
 *
 * @param (const DoubleTab& t) le tableau des coefficients du polynome (1 a 4 dimensions)
 */
inline Polynome::Polynome(const DoubleTab& t) : coeff_(t) {}

/*! @brief Retourne le degre du polynome par rapport a la ieme variable
 *
 * @param (int i) l'indice de la variable consideree
 * @return (int) degre du polynome
 */
inline int Polynome::degre(int i) const
{
  return coeff_.dimension(i)-1;
}

/*! @brief Retourne le coefficient du terme de degre n1
 *
 * @param (int n1) degre
 * @return (double&) coefficient
 */
inline double& Polynome::coeff(int n1)
{
  return coeff_(n1);
}

/*! @brief Retourne le coefficient du terme de degre n1,n2
 *
 * @param (int n1) degre
 * @param (int n2) degre
 * @return (double&) coefficient
 */
inline double& Polynome::coeff(int n1, int n2)
{
  return coeff_(n1, n2);
}

/*! @brief Retourne le coefficient du terme de degre n1,n2,n3
 *
 * @param (int n1) degre
 * @param (int n2) degre
 * @param (int n3) degre
 * @return (double&) coefficient
 */
inline double& Polynome::coeff(int n1, int n2, int n3)
{
  return coeff_(n1, n2, n3);
}

/*! @brief Retourne le coefficient du terme de degre n1,n2,n3,n4
 *
 * @param (int n1) degre
 * @param (int n2) degre
 * @param (int n3) degre
 * @param (int n4) degre
 * @return (double&) coefficient
 */
inline double& Polynome::coeff(int n1, int n2, int n3, int n4)
{
  return coeff_(n1, n2, n3, n4);
}

/*! @brief Retourne le coefficient du terme de degre n1
 *
 * @param (int n1) degre
 * @return (double) coefficient
 */
inline double Polynome::coeff(int n1) const
{
  return coeff_(n1);
}

/*! @brief Retourne le coefficient du terme de degre n1,n2
 *
 * @param (int n1) degre
 * @param (int n2) degre
 * @return (double) coefficient
 */
inline double Polynome::coeff(int n1, int n2) const
{
  return coeff_(n1, n2);
}

/*! @brief Retourne le coefficient du terme de degre n1,n2,n3
 *
 * @param (int n1) degre
 * @param (int n2) degre
 * @param (int n3) degre
 * @return (double) coefficient
 */
inline double Polynome::coeff(int n1, int n2, int n3) const
{
  return coeff_(n1, n2, n3);
}

/*! @brief Retourne le coefficient du terme de degre n1,n2,n3,n4
 *
 * @param (int n1) degre
 * @param (int n2) degre
 * @param (int n3) degre
 * @param (int n4) degre
 * @return (double) coefficient
 */
inline double Polynome::coeff(int n1, int n2, int n3, int n4) const
{
  return coeff_(n1, n2, n3, n4);
}
#endif        //Polynome_H

