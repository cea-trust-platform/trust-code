/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Roue.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#include <Roue.h>

// NOTE : OF 19/6/98
// Pas de commentaires pour Roue_Ptr, ce n'est pas une classe...

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
Roue_ptr::Roue_ptr() : ptr(new(Roue))
{
}


// Description:
// Constructeur par recopie. Copie de l'ensemble de la roue.
// Utile pour les constructeurs par recopie des champs.
// (ce constructeur est appele quand on fait un Champ = un autre Champ,
//  la version precedente copie la reference, d'ou plantage a la destruction)
Roue_ptr::Roue_ptr(const Roue_ptr& x)
{
  ptr=new Roue(x.valeur());
}

// Description:
// Constructeur par recopie. Copie de l'ensemble de la roue.
Roue_ptr& Roue_ptr::operator=(const Roue_ptr& x)
{
  Cerr << "We pass through Roue_ptr=Roue_ptr" << endl;
  assert(0);
  ptr=new Roue(x.valeur());
  return *this;
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
Roue_ptr::Roue_ptr(Roue& x):ptr(&x)
{
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
Roue_ptr::~Roue_ptr()
{
  if(ptr)
    {
      delete ptr;
      ptr=0;
    }
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
Roue& Roue_ptr::operator[](int i)
{
  return ptr->futur(i);
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
const Roue& Roue_ptr::operator[](int i) const
{
  return ptr->futur(i);
}


// Description:
//    Constructeur
//    Construit une roue avec une seule case, au temps t=0
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
Roue::Roue() : temps_(0), nb_cases_(1), valeurs_(), passe_(this), futur_(this)
{
}


// Description:
//    Constructeur par copie
//    Le temps, les valeurs et les valeurs des Roues futures sont copies
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
Roue::Roue(const Roue& roue) : temps_(roue.temps_), nb_cases_(1), valeurs_(roue.valeurs_), passe_(this), futur_(this)
{
  fixer_nb_cases(roue.nb_cases_);
  for(int i=1; i<nb_cases_-1; i++)
    futur(i).valeurs_ = roue.futur(i).valeurs_;
}


// Description:
//    Desctructeur
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
Roue::~Roue()
{
  supprimer_case(nb_cases_);
  assert(nb_cases_==1);
  assert(futur_.ptr==this);
  assert(passe_.ptr==this);
  assert(valeurs_.size()==0);
  futur_.ptr=passe_.ptr=0;
}

// Description:
//    Retourne la Roue correspondant a la ieme case future
// Precondition:
// Parametre: int i
//    Signification: l'indice i de la ieme case future
//    Valeurs par defaut:
//    Contraintes: 1 <= i <= nombre de cases
//    Acces: entree
// Retour: const Roue&
//    Signification: reference sur la ieme Roue future
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const Roue& Roue::futur(int i) const
{
  assert(i>=0);
  assert(i<=nb_cases_);
  if(i==0)
    return *this;
  else
    return futur_->futur(--i);
}


// Description:
//    Retourne la Roue correspondant a la ieme case future
// Precondition:
// Parametre: int i
//    Signification: l'indice i de la ieme case future
//    Valeurs par defaut:
//    Contraintes: 1 <= i <= nombre de cases
//    Acces: entree
// Retour: const Roue&
//    Signification:  la ieme Roue future
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Roue& Roue::futur(int i)
{
  assert(i>=0);
  assert(i<=nb_cases_);
  if(i==0)
    return *this;
  else
    return futur_->futur(--i);
}


// Description:
//    Retourne la Roue correspondant a la ieme case passee
// Precondition:
// Parametre: int i
//    Signification: l'indice i de la ieme case passee
//    Valeurs par defaut:
//    Contraintes: 1 <= i <= nombre de cases
//    Acces: entree
// Retour: const Roue&
//    Signification: reference sur la ieme Roue passee
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const Roue& Roue::passe(int i) const
{
  assert(i>=0);
  assert(i<=nb_cases_);
  if(i==0)
    return *this;
  else
    return passe_->passe(--i);
}


// Description:
//    Retourne la Roue correspondant a la ieme case passee
// Precondition:
// Parametre: int i
//    Signification: l'indice i de la ieme case passee
//    Valeurs par defaut:
//    Contraintes: 1 <= i <= nombre de cases
//    Acces: entree
// Retour: const Roue&
//    Signification: la ieme Roue passee
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Roue& Roue::passe(int i)
{
  assert(i>=0);
  assert(i<=nb_cases_);
  if(i==0)
    return*this;
  else
    return passe_->passe(--i);
}

//    Roue::Roue(const Roue& roue) : passe_(roue.passe_),
//    futur_(roue.passe_), temps_(roue.temps_)
//    {
//       valeurs_.ref(roue.valeurs_);
//    }


// Description:
//    Dimensionne (1D) les valeurs de la Roue et des ses roues futures
// Precondition:
// Parametre: int nb_val
//    Signification: nombre de valeurs
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
//    Redimensionnement des valeurs de la Roue et de ses Roues futures
// Postcondition:
void Roue::dimensionner(int nb_val)
{
  if (valeurs_.size() != nb_val )
    {
      valeurs_.resize(nb_val);
      for(int j=1; j<nb_cases_; j++)
        futur(j).valeurs_.resize(nb_val);
    }
}


// Description:
//    Dimensionne (2D) les valeurs de la Roue et des ses roues futures
// Precondition:
// Parametre: int nb_val
//    Signification: dimension 1 des valeurs
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int nb_ncomp
//    Signification: dimension 2 des valeurs
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
//    Redimensionnement des valeurs de la Roue et de ses Roues futures
// Postcondition:
void Roue::dimensionner(int nb_val, int nb_comp)
{
  if ((valeurs_.size() != nb_val*nb_comp )||(valeurs_.nb_dim()!=2) ||  (nb_comp!=valeurs_.dimension(1)))
    {
      valeurs_.resize(nb_val, nb_comp);
      for(int j=1; j<nb_cases_; j++)
        futur(j).valeurs_.resize(nb_val, nb_comp);
    }
}


// Description:
//    Change le nombre de cases de la Roue
// Precondition:
// Parametre: int nb_case
//    Signification: le nouveau nombre de cases
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nouveau nombre de cases
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Roue::fixer_nb_cases(int nb_case)
{
  if(nb_case==nb_cases_)
    return nb_cases_;
  int nb_case_a_ajouter=nb_case-nb_cases_;
  if(nb_case_a_ajouter>0)
    ajouter_case(nb_case_a_ajouter);
  else
    supprimer_case(-nb_case_a_ajouter);
  return nb_cases_;
}


// Description:
//    Ajoute n cases a la Roue
// Precondition:
// Parametre: int n
//    Signification: le nombre de cases a ajouter
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
//
//   Avant
//          ----->
//   (this)         futur_
//          <-----
//
//   Apres
//          ----->        ----->
//   (this)         futur_        fut_(=futur_ d'avant)
//          <-----        <-----
//
//
//
//
void Roue::ajouter_case(int n)
{
  for (int n_to_add = 0; n_to_add < n; n_to_add++)
    {
      Roue * old_futur = futur_.ptr;
      Roue * new_futur = new Roue;
      this     ->futur_.ptr = new_futur;
      old_futur->passe_.ptr = new_futur;
      new_futur->futur_.ptr = old_futur;
      new_futur->passe_.ptr = this;
      // On met a jour nb_cases_ de toutes les cases:
      {
        const int new_nb_cases = nb_cases_ + 1;
        Roue * ptr_roue = this;
        for(int i = 0; i < new_nb_cases; i++, ptr_roue = ptr_roue->futur_.ptr)
          ptr_roue->nb_cases_ = new_nb_cases;
      }
    }
}

// Description:
//    Supprime n cases de la Roue
// Precondition:
// Parametre: int n
//    Signification: nombre de cases a supprimer
//    Valeurs par defaut:
//    Contraintes: n < nombre de cases
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
//   Avant
//          ----->        ----->
//   (this)         futur_        fut_
//          <-----        <-----
//
//   Apres
//          ----->
//   (this)         fut_
//          <-----
//
//
//
void Roue::supprimer_case(int n)
{
  for (int n_to_kill = 0; n_to_kill < n; n_to_kill++)
    {
      if (nb_cases_ == 1)
        {
          assert(futur_.ptr == this);
          assert(passe_.ptr == this);
          valeurs_.reset();
          temps_=0;
        }
      else
        {
          // Sauvegarde du futur du futur:
          Roue * new_futur = futur_->futur_.ptr;
          // Destruction du futur:
          {
            Roue * to_kill = futur_.ptr;
            to_kill->nb_cases_ = 1;
            to_kill->futur_.ptr = to_kill->passe_.ptr = to_kill;
            delete to_kill;
          }
          // On referme la liste chainee:
          new_futur->passe_.ptr = this;
          futur_.ptr = new_futur;

          const int new_nb_cases = nb_cases_ - 1;
          if(new_nb_cases == 1)
            passe_.ptr=this;

          // On met a jour nb_cases_ de toutes les cases:
          {
            Roue * ptr_roue = this;
            for(int i = 0; i < new_nb_cases; i++, ptr_roue = ptr_roue->futur_.ptr)
              ptr_roue->nb_cases_ = new_nb_cases;
          }
        }
    }
}


// Description:
//    Affectation d'une Roue
//    Les attibuts modifies sont le temps, les valeurs, le nombre de cases et les valeurs des cases futures
// Precondition:
// Parametre: const Roue& roue
//    Signification: la Roue a copier
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Roue&
//    Signification: la Roue modifiee
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Roue& Roue::operator=(const Roue& roue)
{
  temps_ = roue.temps_;
  valeurs_ = roue.valeurs_;
  fixer_nb_cases(roue.nb_cases_);
  for(int i=1; i<nb_cases_-1; i++)
    futur(i).valeurs_ = roue.futur(i).valeurs_;
  return *this;
}

