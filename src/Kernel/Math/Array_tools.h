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
// File:        Array_tools.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <TRUSTTab.h>
#include <arch.h>

// Description: Methode outil pour retirer les doublons dans un tableau.
void array_trier_retirer_doublons(ArrOfInt& array);

// Description: Methode outil pour calculer l'intersection entre deux listes d'entiers.
void array_calculer_intersection(ArrOfInt& liste1, const ArrOfInt& liste2);

// Description: Methode outil pour calculer la difference entre deux listes d'entiers triees
void array_retirer_elements(ArrOfInt& sorted_array, const ArrOfInt& sorted_elements_list);

// Description: Methode outil pour chercher une valeur dans un tableau trie
int array_bsearch(const ArrOfInt& tab, int valeur);

// Description: Tri lexicographique d'un tableau
int tri_lexicographique_tableau(IntTab& tab);

// Description: Tri indirect (on trie le tableau index qui contient des numeros de lignes dans tab)
int tri_lexicographique_tableau_indirect(const IntTab& tab, ArrOfInt& index);

// Description: Methode outil pour retirer les doublons dans un tableau.
void tableau_trier_retirer_doublons(IntTab& tab);

// Description: Methode outil pour trouver les doublons (permet de retirer les doublons
//  sans changer l'ordre des elements)
void calculer_renum_sans_doublons(const IntTab& tab, ArrOfInt& renum, ArrOfInt& items_a_garder);

// Description: Set the smart_resize flag, preallocates memory for the given size, and resize to zero
void array_smart_allocate(ArrOfInt& array, const int allocate_size);
void array_smart_allocate(ArrOfDouble& array, const int allocate_size);

// Description: Increase size of "dest" array and copy "src" at the end of "dest"
void append_array_to_array(ArrOfInt& dest, const ArrOfInt& src);

// Description: Increase size of "dest" array and copy "src" at the end of "dest"
void append_array_to_array(ArrOfDouble& dest, const ArrOfDouble& src);
