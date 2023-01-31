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

#include <TRUST_Deriv.h>

Implemente_instanciable_sans_constructeur_ni_destructeur(TRUST_Deriv_Objet_U, "TRUST_Deriv_Objet_U", Objet_U_ptr);

Sortie& TRUST_Deriv_Objet_U::printOn(Sortie& os) const { return Objet_U_ptr::printOn(os); }

Entree& TRUST_Deriv_Objet_U::readOn(Entree& is) { return Objet_U_ptr::readOn(is); }

TRUST_Deriv_Objet_U::~TRUST_Deriv_Objet_U() { detach(); }

TRUST_Deriv_Objet_U::TRUST_Deriv_Objet_U() : Objet_U_ptr(), pointeur_(nullptr) { }

TRUST_Deriv_Objet_U::TRUST_Deriv_Objet_U(const TRUST_Deriv_Objet_U& t) : Objet_U_ptr(), pointeur_(nullptr)
{
  if (t.non_nul()) recopie(t.valeur());
}

TRUST_Deriv_Objet_U::TRUST_Deriv_Objet_U(const Objet_U& t) : Objet_U_ptr(), pointeur_(nullptr)
{
  recopie(t);
}

const TRUST_Deriv_Objet_U& TRUST_Deriv_Objet_U::operator=(const Objet_U& t)
{
  detach();
  recopie(t);
  return *this;
}

const TRUST_Deriv_Objet_U& TRUST_Deriv_Objet_U::operator=(const TRUST_Deriv_Objet_U& t)
{
  detach();
  if (t.non_nul()) recopie(t.valeur());
  else set_Objet_U_ptr(nullptr);
  return *this;
}

const Type_info& TRUST_Deriv_Objet_U::get_info_ptr() const
{
  const Type_info *type_info = Objet_U::info();
  return *type_info; /* type de base accepte par la ref */
}

void TRUST_Deriv_Objet_U::set_Objet_U_ptr(Objet_U *objet)
{
  Objet_U_ptr::set_Objet_U_ptr(objet);
  if (objet)  pointeur_ = (Objet_U*) objet;
  else pointeur_ = nullptr;
}

void TRUST_Deriv_Objet_U::deplace(TRUST_Deriv_Objet_U& deriv_obj)
{
  detach();
  Objet_U& objet = deriv_obj.valeur();
  set_Objet_U_ptr(&objet);
  deriv_obj.set_Objet_U_ptr(nullptr);
}

int TRUST_Deriv_Objet_U::reprendre(Entree& is)
{
  return valeur().reprendre(is);
}

int TRUST_Deriv_Objet_U::sauvegarder(Sortie& os) const
{
  return valeur().sauvegarder(os);
}
