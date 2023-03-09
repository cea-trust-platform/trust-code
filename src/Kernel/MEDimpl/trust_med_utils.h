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

#ifndef med_utils_included
#define med_utils_included

#include <Process.h>
#include <Nom.h>
#include <TRUSTTabs_forward.h>
#include <med++.h>
#include <medcoupling++.h>
#ifdef MEDCOUPLING_
#   include <NormalizedGeometricTypes>
#endif


////
//// Useful MED-related fonctions
////

class Char_ptr;
class Entree;
class Nom;

inline void med_non_installe()
{
  Process::exit("This version has not been built with MED library.");
}

void lire_nom_med(Nom& nom_champ, Entree& is);
void test_version(Nom& nom) ;
void dimensionne_char_ptr_taille(Char_ptr& nom, int taille_d_un_mot, int nb=1);
void traite_nom_fichier_med(Nom& nom_fic);
void read_med_field_names(const Nom& nom_fic, Noms& noms_chps, ArrOfDouble& temps_sauv);
void conn_trust_to_med(IntTab& les_elems2, Nom& type_elem, bool toMED);

#ifdef MED_
med_geometry_type type_geo_trio_to_type_med(const Nom& type_elem);
med_geometry_type type_geo_trio_to_type_med(const Nom& type_elem_,med_axis_type& rep);

#ifdef MEDCOUPLING_
INTERP_KERNEL::NormalizedCellType type_geo_trio_to_type_medcoupling(const Nom& type_elem_, int& mesh_dimension);
#endif

#endif

#endif
