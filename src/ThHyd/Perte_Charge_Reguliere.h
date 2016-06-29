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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Perte_Charge_Reguliere.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Perte_Charge_Reguliere_included
#define Perte_Charge_Reguliere_included



#include <math.h>
#include <Perte_Charge.h>
#include <DoubleVect.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Perte_Charge_Reguliere
//    Cette classe derivee de Perte_Charge est utiliseelorsque l'on veut
//    modeliser une zone de l'ecoulement dans laquelle des obstacles sont
//    immerges (faisceau de tubes par exemple)
// .SECTION voir aussi
//    Perte_Charge Perte_Charge_Singuliere
//////////////////////////////////////////////////////////////////////////////
class Perte_Charge_Reguliere : public Perte_Charge
{

public :
  inline double correction_direction(DoubleVect& , int) const ;
  inline double calculer_Cf_blasius(double ) const;
  Entree& lire_donnees(Entree& );
  inline double D() const;
  inline double d() const;
  inline double a() const;
  inline double b() const;
  inline double CF() const;

protected :

  double D_;
  double d_;
  double a_;
  double b_;
  double Cf_;

  int couronne_tube ;
  int Cf_utilisateur ;
  double xo,yo,zo;
  int dir[3] ;
  double h0, h ;

};


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline double Perte_Charge_Reguliere::D() const
{
  return D_;
}


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline double Perte_Charge_Reguliere::d() const
{
  return d_;
}


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline double Perte_Charge_Reguliere::a() const
{
  return a_;
}


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline double Perte_Charge_Reguliere::b() const
{
  return b_;
}

inline double Perte_Charge_Reguliere::CF() const
{
  return Cf_;
}

// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline double Perte_Charge_Reguliere::calculer_Cf_blasius(double Re) const
{
  double Cf = 0.;
  if (Re > 1.e-12)
    Cf= a_*pow(Re,-b_);
  return Cf;
}

inline double Perte_Charge_Reguliere::correction_direction(DoubleVect& xp, int k) const
{
  double cor_dir = 1 ;

  if (couronne_tube == 1)
    {
      int dimension = xp.size() ;

      double x1 = (xp(0) - xo)*dir[0] ;
      double y1 = (xp(1) - yo)*dir[1] ;
      double z1 = 0. ;
      if (dimension == 3 ) z1 = (xp(2) - zo)*dir[2] ;
      double scale = sqrt(x1*x1 + y1*y1 + z1*z1);

      //          if (k == 0 ) cor_dir = x1/scale ;
      //          if (k == 1 ) cor_dir = y1/scale ;
      //         if (k == 2 ) cor_dir = z1/scale ;
      if (k == 0 ) cor_dir = dabs(x1)/scale ;
      if (k == 1 ) cor_dir = dabs(y1)/scale ;
      if (k == 2 ) cor_dir = dabs(z1)/scale ;

    }

  return cor_dir;
}


#endif
