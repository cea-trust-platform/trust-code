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

#ifndef Objet_U_ptr_included
#define Objet_U_ptr_included

#include <Objet_U.h>

class Nom;

/*! @brief Pointeur sur un Objet_U.
 *
 * Le constructeur par defaut construit un pointeur "nul".
 *
 *
 * @sa TRUST_Deriv
 */
class Objet_U_ptr: public Objet_U
{
  Declare_base_sans_constructeur_ni_destructeur(Objet_U_ptr);

public:
  static constexpr bool HAS_POINTER = true;
  Objet_U_ptr(const Objet_U_ptr&) = delete;
  const Objet_U_ptr& operator=(const Objet_U_ptr&) = delete;

  bool non_nul() const;
  bool est_nul() const;

  const Nom& le_nom() const override;
  Objet_U * typer(const char * nom_type);
  void detach();
#ifndef LATATOOLS
  int associer_(Objet_U& objet) override;
#endif

protected:
  ~Objet_U_ptr() override;
  Objet_U_ptr();

  virtual void set_Objet_U_ptr(Objet_U*);
  virtual Objet_U* get_Objet_U_ptr() const;

  void recopie(const Objet_U&);
#ifndef LATATOOLS
  int change_num(const int* const) override; // renumerotation des objets
#endif

  Objet_U* get_Objet_U_ptr_check() const;
  int check_Objet_U_ptr_type(const Objet_U *ptr) const;

  // Renvoie le Type_info du type de base accepte par le pointeur (l'objet pointe derive obligatoirement de ce type).
  virtual const Type_info& get_info_ptr() const = 0;

private:
  // cle_ est le numero de cle de l'objet en reference.
  // Si cle_ == -1, le pointeur est "nul".
  // L'objet peut etre recupere par la_memoire().objet_u(cle_)
  // Attention, la cle_ peut changer au cours du deroulement du programme (voir Memoire::comprimer())
  int cle_ = -1;
  // ptr_object_id_ est l'object_id_ de l'objet en reference.
  // Permet de tester facilement si on pointe sur l'objet que l'on croit.
  int ptr_object_id_ = -1;
};

#endif // Objet_U_ptr_H
