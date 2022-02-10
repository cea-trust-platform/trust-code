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
// File:        O_U_Ptr.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef O_U_Ptr_included
#define O_U_Ptr_included
#include <Objet_U.h>
#include <Memoire.h>

class Nom;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Pointeur sur un Objet_U.
//    Le constructeur par defaut construit un pointeur "nul".
//
// .SECTION voir aussi
//     Ref_ Deriv_ Memoire_
//////////////////////////////////////////////////////////////////////////////
class O_U_Ptr : public Objet_U
{
  Declare_base_sans_constructeur_ni_destructeur(O_U_Ptr);

public:
  int        non_nul() const;
  const Nom&    le_nom() const override;
  // Renvoie le Type_info du type de base accepte par le pointeur
  // (l'objet pointe derive obligatoirement de ce type).
  virtual const Type_info& get_info_ptr() const = 0;

protected:
  ~O_U_Ptr() override;
  O_U_Ptr();

  virtual void      set_O_U_Ptr(Objet_U *);
  virtual Objet_U * get_O_U_Ptr() const;
  int            get_ptr_cle() const;
  int            get_ptr_object_id() const;

  void              recopie(const Objet_U&);
  int            change_num(const int* const) override; // renumerotation des objets

  Objet_U *         get_O_U_Ptr_check() const;
  int            check_O_U_Ptr_type(const Objet_U * ptr) const;

private:
  O_U_Ptr(const O_U_Ptr&);
  const O_U_Ptr& operator=(const O_U_Ptr&);

  // cle_ est le numero de cle de l'objet en reference.
  // Si cle_ == -1, le pointeur est "nul".
  // L'objet peut etre recupere par la_memoire().objet_u(cle_)
  // Attention, la cle_ peut changer au cours du deroulement du
  // programme (voir Memoire::comprimer())
  int    cle_;
  // ptr_object_id_ est l'object_id_ de l'objet en reference.
  // Permet de tester facilement si on pointe sur l'objet que
  // l'on croit.
  int    ptr_object_id_;
};

#endif // O_U_Ptr_H
