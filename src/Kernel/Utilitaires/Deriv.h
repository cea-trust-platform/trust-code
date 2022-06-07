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
// .DESCRIPTION
//
//  Soit une classe Type_base qui derive de Objet_U.
//  DERIV(T) est une petite classe contenant un pointeur vers une instance de Type_base,
//  ou de n'importe quelle classe derivee de Type_base qui est creee
//  dynamiquement lors a l'execution.
//
//  Definition de la classe DERIV:
//   class Type_base; // Une declaration forward de Type_base suffit
//Declare_deriv(Type_base);;
//
//  Implementation de la classe DERIV:
//   class Type_base : Un_Type_Derive_de_Objet_U
//   { ... } ;
//Implemente_deriv(Type_base);//IlfautquelaclasseType_basesoitdefinie;
//
//  Creation d'un objet de type DERIV(Type_base) :
//   DERIV(Type_base) deriv_type; // deriv_type est encore un pointeur nul
//
//  On suppose que la classe Type_Derive derive de Type_base et est instanciable:
//   class Type_Derive : Type_base
//   {
//Declare_instanciable(Type_Derive);;
//     ...
//   };
//
//  Creation d'une instance de la classe Type_Derive qui derive de Type_base:
//   deriv_type.typer("Type_Derive");
//
//  Acces a l'instance de Type_Derive:
//   Type_Derive & objet_derive = deriv_type.valeur();
//   const Type_Derive & objet_derive = deriv_type.valeur();
//
//  L'instance de Type_Derive est detruite si on appelle a nouveau "typer()"
//  ou si l'objet deriv_type est detruit.
//
//  Exemples :
//   Equation_base     Operateur_Diff_base  ... ( <=> Type_base )
//   Navier_Stokes_std Op_Diff_VDF_var_Face     ( <=> Type_derive )
//   Equation          Operateur_Diff           ( <=> DERIV(Type_base) )
//
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////

#ifndef Deriv_H
#define Deriv_H

#define DERIV(_TYPE_) name2(Deriv_,_TYPE_)

#ifdef Template_Version
// B. Mathieu, 07/2004 : il faudrait mettre a jour la version template
// si on veut l'utiliser. J'ai modifie MacDeriv.h mais pas TpltDeriv.h.
#error "La version template de DERIV n'a pas ete mise a jour."
#include <Deriv.h>
#define declare_deriv(_TYPE_)                        \
  typedef Deriv<_TYPE_> DERIV(_TYPE_);
#define implemente_deriv(_TYPE_)
#else
#include <MacDeriv.h>
#endif
#endif
