/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef Domaine_forward
#define Domaine_forward

/*! This include file should be used in place of the former
 *
 *       class Domaine;
 *
 * forward declaration.
 *
 */

#include <arch.h>

template <typename _SIZE_> class Domaine_32_64;
using Domaine = Domaine_32_64<int>;
using Domaine_64 = Domaine_32_64<trustIdType>;

template <typename _SIZE_> class DomaineAxi1d_32_64;
using DomaineAxi1d = DomaineAxi1d_32_64<int>;
using DomaineAxi1d_64 = DomaineAxi1d_32_64<trustIdType>;

template <typename _SIZE_> class Sous_Domaine_32_64;
using Sous_Domaine = Sous_Domaine_32_64<int>;
using Sous_Domaine_64 = Sous_Domaine_32_64<trustIdType>;

template <typename _SIZE_> class Faces_32_64;
using Faces = Faces_32_64<int>;
using Faces_64 = Faces_32_64<trustIdType>;

template <typename _SIZE_> class Frontiere_32_64;
using Frontiere = Frontiere_32_64<int>;
using Frontiere_64 = Frontiere_32_64<trustIdType>;

template <typename _SIZE_> class Bord_32_64;
using Bord = Bord_32_64<int>;
using Bord_64 = Bord_32_64<trustIdType>;

template <typename _SIZE_> class Raccord_32_64;
using Raccord = Raccord_32_64<int>;
using Raccord_64 = Raccord_32_64<trustIdType>;

template <typename _SIZE_> class Joint_32_64;
using Joint = Joint_32_64<int>;
using Joint_64 = Joint_32_64<trustIdType>;

////////////////////////////////

template <typename _SIZE_> class Groupe_Faces_32_64;
using Groupe_Faces = Groupe_Faces_32_64<int>;
using Groupe_Faces_64 = Groupe_Faces_32_64<trustIdType>;

template <typename _SIZE_> class Bord_Interne_32_64;
using Bord_Interne = Bord_Interne_32_64<int>;
using Bord_Interne_64 = Bord_Interne_32_64<trustIdType>;

template <typename _SIZE_> class Bord_Interne_32_64;
using Bord_Interne = Bord_Interne_32_64<int>;
using Bord_Interne_64 = Bord_Interne_32_64<trustIdType>;

template <typename _SIZE_> class Faces_32_64;
using Faces = Faces_32_64<int>;
using Faces_64 = Faces_32_64<trustIdType>;

template <typename _SIZE_> class Raccords_32_64;
using Raccords = Raccords_32_64<int>;
using Raccords_64 = Raccords_32_64<trustIdType>;

template <typename _SIZE_> class Joints_32_64;
using Joints = Joints_32_64<int>;
using Joints_64 = Joints_32_64<trustIdType>;

/////////////////////////////////

template <typename _SIZE_> class Elem_geom_base_32_64;
using Elem_geom_base = Elem_geom_base_32_64<int>;
using Elem_geom_base_64 = Elem_geom_base_32_64<trustIdType>;


#endif
