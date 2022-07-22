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

#include <Polynome.h>

Implemente_instanciable(Polynome,"Polynome",Objet_U);



/*! @brief Ecriture sur un flot de sortie Ecrit le tableau des coefficients
 *
 * @param (Sortie& os) le flot de sortie a utiliser
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Polynome::printOn(Sortie& os) const
{
  os << coeff_;
  return os;
}


/*! @brief Lecture dans un flot d'entree Lecture du tableau des coefficients
 *
 * @param (Entree& is) le flot d'entree a utiliser
 * @return (Entree& le flot d'entree modifie)
 */
Entree& Polynome::readOn(Entree& is)
{
  is >> coeff_;
  return is;
}


/*! @brief Calcul de la valeur du polynome au point x1
 *
 * @param (double x1) point
 * @return (double) valeur du polynome
 */
double Polynome::operator()(double x1) const
{
  int n=coeff_.dimension(0);
  int i=0;
  double xi=1;
  double sum=0;
  while(n--)
    {
      sum+=coeff_(i++)*xi;
      xi*=x1;
    }
  return sum;
}

/*! @brief Calcul de la valeur du polynome au point x1,x2
 *
 * @param (double x1) point
 * @param (double x2) point
 * @return (double) valeur du polynome
 */
double Polynome::operator()(double x1, double x2) const
{
  int n1=coeff_.dimension(0);
  int n2=coeff_.dimension(1);
  int i=0;
  int j=0;
  double xi=1;
  double xj=1;
  double sum=0;
  while(n1--)
    {
      xj=1.;
      n2=coeff_.dimension(1);
      j=0;
      while(n2--)
        {
          sum+=coeff_(i,j++)*xj*xi;
          xj*=x2;
        }
      xi*=x1;
      i++;
    }
  return sum;
}

/*! @brief Calcul de la valeur du polynome au point x1,x2,x3
 *
 * @param (double x1) point
 * @param (double x2) point
 * @param (double x3) point
 * @return (double) valeur du polynome
 */
double Polynome::operator()(double x1, double x2, double x3) const
{
  int n1=coeff_.dimension(0);
  int n2=coeff_.dimension(1);
  int n3=coeff_.dimension(2);
  int i=0;
  int j=0;
  int k=0;
  double xi=1;
  double xj=1;
  double xk=1;
  double sum=0;
  while(n1--)
    {
      xj=1.;
      n2=coeff_.dimension(1);
      j=0;
      while(n2--)
        {
          xk=1.;
          n3=coeff_.dimension(2);
          k=0;
          while(n3--)
            {
              sum+=coeff_(i,j,k++)*xk*xj*xi;
              xk*=x3;
            }
          xj*=x2;
          j++;
        }
      xi*=x1;
      i++;
    }
  return sum;
}

/*! @brief Calcul de la valeur du polynome au point x1,x2,x3,x4
 *
 * @param (double x1) point
 * @param (double x2) point
 * @param (double x3) point
 * @param (double x4) point
 * @return (double) valeur du polynome
 */
double Polynome::operator()(double x1, double x2, double x3, double x4) const
{
  int n1=coeff_.dimension(0);
  int n2=coeff_.dimension(1);
  int n3=coeff_.dimension(2);
  int n4=coeff_.dimension(3);
  int i=0;
  int j=0;
  int k=0;
  int l=0;
  double xi=1;
  double xj=1;
  double xk=1;
  double xl=1;
  double sum=0;
  while(n1--)
    {
      xj=1.;
      n2=coeff_.dimension(1);
      j=0;
      while(n2--)
        {
          xk=1.;
          n3=coeff_.dimension(2);
          k=0;
          while(n3--)
            {
              xl=1.;
              n4=coeff_.dimension(3);
              l=0;
              while(n4--)
                {
                  sum+=coeff_(i,j,k,l++)*xl*xk*xj*xi;
                  xl*=x4;
                }
              xk*=x3;
              k++;
            }
          xj*=x2;
          j++;
        }
      xi*=x1;
      i++;
    }
  return sum;
}


/*! @brief Non code.
 *
 * Ne fait rien
 *
 */
void Polynome::derive(int )
{
}

/*! @brief Non code.
 *
 * Ne fait rien
 *
 */
void Polynome::integre(int )
{
}


/*! @brief Non code.
 *
 * retourne 0.
 *
 */
double Polynome::derive(double ) const
{
  return 0;
}

/*! @brief Non code.
 *
 * retourne 0.
 *
 */
double Polynome::derive(double , double ) const
{
  return 0;
}

/*! @brief Non code.
 *
 * retourne 0.
 *
 */
double Polynome::derive(double , double , double ) const
{
  return 0;
}

/*! @brief Non code.
 *
 * retourne 0.
 *
 */
double Polynome::derive(double , double , double , double ) const
{
  return 0;
}


/*! @brief Non code.
 *
 * retourne 0.
 *
 */
double Polynome::integre(double ) const
{
  return 0;
}

/*! @brief Non code.
 *
 * retourne 0.
 *
 */
double Polynome::integre(double , double ) const
{
  return 0;
}

/*! @brief Non code.
 *
 * retourne 0.
 *
 */
double Polynome::integre(double , double , double ) const
{
  return 0;
}

/*! @brief Non code.
 *
 * retourne 0.
 *
 */
double Polynome::integre(double , double , double , double ) const
{
  return 0;
}


/*! @brief Non code.
 *
 * Ne fait rien. Retourne *this.
 *
 */
Polynome& Polynome::operator +=(const Polynome&)
{
  return *this;
}

/*! @brief Non code.
 *
 * Ne fait rien. Retourne *this.
 *
 */
Polynome& Polynome::operator -=(const Polynome&)
{
  return *this;
}

/*! @brief Non code.
 *
 * Ne fait rien. Retourne *this.
 *
 */
Polynome& Polynome::operator *=(const Polynome&)
{
  return *this;
}

/*! @brief Non code.
 *
 * Ne fait rien. Retourne *this.
 *
 */
Polynome& Polynome::operator *=(double)
{
  return *this;
}

/*! @brief Non code.
 *
 * Ne fait rien. Retourne *this.
 *
 */
Polynome& Polynome::operator /=(double)
{
  return *this;
}
