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
// File:        Cast.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Cast_H
#define Cast_H

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Informations de types :
//      static_type_info(T) : renvoie le type_info de la classe T.
//      ptr_type_info(p) : renvoie le type_info de l'objet pointe par p.
//      ref_type_info(r) : renvoie le type_info de l'objet r ou de l'alias r
//      sub_type(T,r) : renvoie 1 ssi r est une instance d'une classe fille de T; 0 sinon.
//    Les cast securises :
//      ptr_cast(T,p) : renvoie (T*)(p) ssi *p est d'un sous type de T; 0 sinon.
//      ref_cast(T,r) : : renvoie (T&)(r) ssi r est d'un sous type de T; 0 sinon.
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////

class Type_info;

extern int* bad_ref_cast(const Type_info* t1, const Type_info* t2);

#define static_type_info(T)                        \
  T::info()
#define ptr_type_info(p)                        \
  ((p)->get_info())
#define ref_type_info(r)                        \
  ((r).get_info())

#define same_type(T,r)                                \
  (((r).get_info())->same(T::info()))
#define sub_type(T,r)                                        \
  (T::info_obj.can_cast((r).get_info()) ? 1 : 0)
#define ptr_cast(T,p)                                                \
  (T::info_obj.can_cast((p)->get_info())                        \
   ? 0 :bad_ref_cast(T::info(),(p)->get_info()), (T*)(p))
#define ref_cast(T,r)                                                        \
  (T::info_obj.can_cast((r).get_info())                                        \
   ? 0 :bad_ref_cast(T::info(),(r).get_info()), T::self_cast(r))
#define ref_cast_non_const(T,r)                                        \
  (T::info_obj.can_cast((r).get_info())                                \
   ? 0 :bad_ref_cast(T::info(),(r).get_info()), (T&)(r))
#endif
