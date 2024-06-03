/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Liste_bloc_included
#define Liste_bloc_included

#include <TRUST_Deriv.h>

/*! @brief La classe Liste_bloc et Liste_bloc_curseur represente une liste de Deriv<Objet_U> et un curseur associe.
 *
 */
class Liste_bloc: public Objet_U
{
  Declare_instanciable_sans_constructeur_ni_destructeur(Liste_bloc);
public:
  Liste_bloc() { suivant_ = this; }
  ~Liste_bloc() override;

  inline int est_vide() const { return suivant_ == this; }
  inline Objet_U& valeur() { return data.valeur(); }
  inline const Objet_U& valeur() const { return data.valeur(); }
  inline int est_dernier() const { return (est_vide() || (!suivant_)); }
  inline Objet_U* operator ->() { return &(valeur()); }
  inline const Objet_U* operator ->() const { return &(valeur()); }
  inline const Liste_bloc& suivant() const { return *suivant_; }
  inline Liste_bloc& suivant() { return *suivant_; }

  Objet_U& operator[](int);
  Objet_U& add_deplace(DerObjU&);

  void vide();
  Liste_bloc& dernier();

protected:
  DerObjU data;
  Liste_bloc *suivant_ = nullptr;
};

class Liste_bloc_curseur
{
public :
  inline Liste_bloc_curseur(const Liste_bloc& a_list) : curseur((Liste_bloc*) &a_list) { if(a_list.est_vide()) curseur = nullptr; }
  inline const Objet_U& valeur() const { return curseur->valeur(); }
  inline Objet_U& valeur() { return curseur->valeur(); }
  inline Objet_U* operator ->() { return curseur->operator->(); }
  inline const Objet_U* operator ->() const { return curseur->operator->(); }
  inline const Liste_bloc& list() const { return *curseur; }
  inline Liste_bloc& list() { return *curseur; }
  inline operator bool() const { return (curseur != nullptr); }
  inline void operator ++()
  {
    if (curseur->est_dernier()) curseur = nullptr;
    else curseur = &(curseur->suivant());
  }

  inline Liste_bloc& operator=(const Liste_bloc& a_list)
  {
    curseur = (Liste_bloc*) (&a_list);
    return *curseur;
  }

private :
  Liste_bloc* curseur = nullptr;
};

#endif /* Liste_bloc_included */
