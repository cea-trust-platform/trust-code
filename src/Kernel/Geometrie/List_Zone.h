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

#ifndef List_Zone_included
#define List_Zone_included

#include <Motcle.h>
#include <liste.h>
#include <Zone.h>

/*
 *
 * ADRIEN ADRIEN ADRIEN : C'EST TOI QUI VA VIRER CETTE CLASSE :) FAIS TOI PLAISIR AHAHAHA
 *
 */
class List_Zone: public liste
{
  Declare_instanciable(List_Zone);
public:
  List_Zone(const Zone& t) : liste(t) { }
  List_Zone(const List_Zone& t) : liste(t) { }

  Zone& operator[](int i) { return static_cast<Zone&>(liste::operator[](i)); }
  const Zone& operator[](int i) const { return static_cast<const Zone&>(liste::operator[](i)); }
  Zone& operator[](const Nom& nom) { return static_cast<Zone&>(liste::operator[](nom)); }
  const Zone& operator[](const Nom& nom) const { return static_cast<const Zone&>(liste::operator[](nom)); }

  Zone& operator()(int i) { return operator[](i); }
  const Zone& operator()(int i) const { return operator[](i); }
  Zone& operator()(const Nom& n) { return operator[](n); }
  const Zone& operator()(const Nom& n) const { return operator[](n); }

  Zone& valeur() { return static_cast<Zone&>(liste::valeur()); }
  const Zone& valeur() const { return static_cast<const Zone&>(liste::valeur()); }

  List_Zone& operator=(const Zone& t) { return static_cast<List_Zone&>(liste::operator=(t)); }
  List_Zone& operator=(const List_Zone& a_list) { return static_cast<List_Zone&>(liste::operator=(a_list)); }
  List_Zone& add(const List_Zone& a_list) {  return static_cast<List_Zone&>(liste::add(a_list)); }
  Zone& add(const Zone& t) { return static_cast<Zone&>(liste::add((const Objet_U&) t)); }

  void supprimer() { liste::supprimer(); }
  List_Zone& suivant() { return static_cast<List_Zone&>(liste::suivant()); }

  int est_egal_a(const Objet_U&) const override { return 0; }
  void suppr(const Zone& t) { liste::suppr(t); }
};


class List_Zone_Curseur : public liste_curseur
{
public:
  List_Zone_Curseur(const List_Zone& a_list) : liste_curseur(a_list) { }

  void operator=(const List_Zone& a_list) { liste_curseur::operator=(a_list); }

  Zone& valeur() { return static_cast<Zone&>(liste_curseur::valeur()); }
  const Zone& valeur() const { return static_cast<const Zone&>(liste_curseur::valeur()); }

  Zone* operator ->() { return static_cast<Zone*>(liste_curseur::operator ->()); }
  const Zone* operator ->() const { return static_cast<const Zone*>(liste_curseur::operator ->()); }

  List_Zone& list() { return static_cast<List_Zone&>(liste_curseur::list()); }
  const List_Zone& list() const { return static_cast<const List_Zone&>(liste_curseur::list()); }
};

#endif /* List_Zone_included */
