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

#ifndef IJK_Field_tools_included
#define IJK_Field_tools_included

#include <Vect.h>
#include <Static_Int_Lists.h>
#include <TRUSTLists.h>
#include <TRUSTVect.h>
#include <TRUSTArray.h>
#include <IJK_Splitting.h>
#include <IJ_layout.h>
#include <IJK_Field_local_template.h>
#include <IJK_Field_template.h>
#include <IJKArray_with_ghost.h>
#include <IJK_communications.h>

// pour IJK_Lata_writer.cpp. TODO : FIXME : to do enum class !!!!!
#define DIRECTION_I 0
#define DIRECTION_J 1
#define DIRECTION_K 2

// .Description
// Useless class but for some reason, verifie_pere only seems to work if it's here ??
class IJK_Field_tools : public Objet_U
{
  Declare_instanciable(IJK_Field_tools);
};


template<typename _TYPE_, typename _TYPE_ARRAY_>
double norme_ijk(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x);

template<typename _TYPE_, typename _TYPE_ARRAY_>
_TYPE_ prod_scal_ijk(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& x, const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& y);

template<typename _TYPE_, typename _TYPE_ARRAY_>
double somme_ijk(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& residu);

template<typename _TYPE_, typename _TYPE_ARRAY_>
_TYPE_ max_ijk(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>& residu);

using IJK_Field_local = IJK_Field_local_double;
using IJK_Field = IJK_Field_double;
using VECT(IJK_Field) = VECT(IJK_Field_double);

#include <IJK_Field_tools.tpp>

#endif
