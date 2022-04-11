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
// File:        Roue.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Roue_included
#define Roue_included

#include <TRUSTTabs_forward.h>
class Roue;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     Pointeur sur une roue.
//     La roue est creee dans le constructeur, detruite dans le destructeur.
//     Elle est recopiee dans le constructeur par recopie
//     et dans l'operateur d'affectation d'un Roue_ptr a un autre.
//     Dans tous les autres cas seuls les pointeurs sont manipules.
// .SECTION voir aussi
//     Champ_Inc
//////////////////////////////////////////////////////////////////////////////

struct Roue_ptr
{


  inline Roue* operator ->() const;
  inline Roue* operator ->();
  Roue_ptr(const Roue_ptr& );
  Roue_ptr& operator =(const Roue_ptr& x);
  Roue_ptr(Roue& );
  Roue_ptr();
  inline Roue_ptr(Roue*);
  ~Roue_ptr();
  Roue* ptr;
  inline Roue& valeur()
  {
    return *ptr;
  }
  inline const Roue& valeur() const
  {
    return *ptr;
  }
  inline void annule()
  {
    ptr=0;
  }
  inline operator bool() const
  {
    return (ptr!=0);
  }
  inline Roue& operator =(Roue& x)
  {
    ptr=&x;
    return *ptr;
  }
  inline Roue* operator =(Roue* x)
  {
    return ptr=x;
  }

  Roue& operator[](int i);
  const Roue& operator[](int i) const;
};


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     Classe Roue utilisee dans Champ_Inc_Base.
//     Permet de gerer le nombre de valeurs du temps pour lesquels le champ
//     doit rester en memoire.
// .SECTION voir aussi
//     Champ_Inc
//////////////////////////////////////////////////////////////////////////////


class Roue
{
public :
  inline const DoubleTab& valeurs() const;
  inline DoubleTab& valeurs();
  Roue();
  Roue(const Roue&);
  ~Roue();
  void dimensionner(int nb_val);
  void dimensionner(int nb_val, int nb_comp);
  int nb_cases() const;
  int fixer_nb_cases(int );
  void ajouter_case(int n=1);
  void supprimer_case(int n=1);
  inline double temps() const;
  inline double changer_temps(const double t);
  const Roue& futur(int i =1) const;
  Roue& futur(int i =1);
  const Roue& passe(int i =1) const;
  Roue& passe(int i =1);
  Roue& operator=(const Roue&);
  inline void avancer(Roue_ptr& ptr)
  {
    ptr=ptr->futur_.valeur();
  };
  inline void reculer(Roue_ptr& ptr)
  {
    ptr=ptr->passe_.valeur();
  };

private :

  double temps_;
  int nb_cases_;
  DoubleTab valeurs_;
  Roue_ptr passe_;
  Roue_ptr futur_;
};

// Description:
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
inline Roue_ptr::Roue_ptr(Roue *x): ptr(x) {}

// Description:
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
inline Roue* Roue_ptr::operator ->() const
{
  return ptr;
}

// Description:
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
inline Roue* Roue_ptr::operator ->()
{
  return ptr;
}

// Description:
//    Retourne le tableau des valeurs de la Roue
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: const DoubleTab&
//    Signification: reference constante sur le tableau des valeurs
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline const DoubleTab& Roue::valeurs() const
{
  return valeurs_;
}


// Description:
//    Retourne le tableau des valeurs de la Roue
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: const DoubleTab&
//    Signification: reference sur le tableau des valeurs
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline DoubleTab& Roue::valeurs()
{
  return valeurs_;
}


// Description:
//    Retoune la valeur du temps de la Roue
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: le temps
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline double Roue::temps() const
{
  return temps_;
}


// Description:
//    Change le temps de la Roue
// Precondition:
// Parametre: const double& t
//    Signification: le nouveau temps
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: le nouveau temps
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline double Roue::changer_temps(const double t)
{
  return temps_=t;
}
// Description:
//    Retourne le nombre de cases de la Roue
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de cases de la Roue
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int Roue::nb_cases() const
{
  return nb_cases_;
}

#endif
