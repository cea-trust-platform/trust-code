/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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



// Description:
//    Ecriture sur un flot de sortie
//    Ecrit le tableau des coefficients
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& Polynome::printOn(Sortie& os) const
{
  os << coeff_;
  return os;
}


// Description:
//    Lecture dans un flot d'entree
//    Lecture du tableau des coefficients
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree& le flot d'entree modifie
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Polynome::readOn(Entree& is)
{
  is >> coeff_;
  return is;
}


// Description:
//    Calcul de la valeur du polynome au point x1
// Precondition:
// Parametre: double x1
//    Signification: point
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: valeur du polynome
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
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

// Description:
//    Calcul de la valeur du polynome au point x1,x2
// Precondition:
// Parametre: double x1
//    Signification: point
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: double x2
//    Signification: point
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: valeur du polynome
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
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

// Description:
//    Calcul de la valeur du polynome au point x1,x2,x3
// Precondition:
// Parametre: double x1
//    Signification: point
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: double x2
//    Signification: point
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: double x3
//    Signification: point
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: valeur du polynome
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
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

// Description:
//    Calcul de la valeur du polynome au point x1,x2,x3,x4
// Precondition:
// Parametre: double x1
//    Signification: point
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: double x2
//    Signification: point
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: double x3
//    Signification: point
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: double x4
//    Signification: point
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: valeur du polynome
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
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


// Description:
//     Non code. Ne fait rien
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Polynome::derive(int )
{
}

// Description:
//     Non code. Ne fait rien
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Polynome::integre(int )
{
}


// Description:
//     Non code. retourne 0.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
double Polynome::derive(double ) const
{
  return 0;
}

// Description:
//     Non code. retourne 0.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
double Polynome::derive(double , double ) const
{
  return 0;
}

// Description:
//     Non code. retourne 0.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
double Polynome::derive(double , double , double ) const
{
  return 0;
}

// Description:
//     Non code. retourne 0.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
double Polynome::derive(double , double , double , double ) const
{
  return 0;
}


// Description:
//     Non code. retourne 0.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
double Polynome::integre(double ) const
{
  return 0;
}

// Description:
//     Non code. retourne 0.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
double Polynome::integre(double , double ) const
{
  return 0;
}

// Description:
//     Non code. retourne 0.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
double Polynome::integre(double , double , double ) const
{
  return 0;
}

// Description:
//     Non code. retourne 0.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
double Polynome::integre(double , double , double , double ) const
{
  return 0;
}


// Description:
//     Non code. Ne fait rien. Retourne *this.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Polynome& Polynome::operator +=(const Polynome&)
{
  return *this;
}

// Description:
//     Non code. Ne fait rien. Retourne *this.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Polynome& Polynome::operator -=(const Polynome&)
{
  return *this;
}

// Description:
//     Non code. Ne fait rien. Retourne *this.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Polynome& Polynome::operator *=(const Polynome&)
{
  return *this;
}

// Description:
//     Non code. Ne fait rien. Retourne *this.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Polynome& Polynome::operator *=(double)
{
  return *this;
}

// Description:
//     Non code. Ne fait rien. Retourne *this.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Polynome& Polynome::operator /=(double)
{
  return *this;
}
