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

#ifndef Solv_Externe_included
#define Solv_Externe_included

#include <SolveurSys_base.h>
#include <ArrOfBit.h>
#include <TRUSTTab.h>

class Matrice_Morse;
class Matrice_Morse_Sym;

/*! Common stuff for several external solvers.
 *
 * Note: here we use trustIdType for potentially big identifiers, this maps to PetscInt type in Solv_Petsc class
 * (type equality between the both is checked when creating the solver).
 */
class Solv_Externe : public SolveurSys_base
{
  Declare_base_sans_constructeur_ni_destructeur(Solv_Externe);
public:
  Solv_Externe() : SolveurSys_base::SolveurSys_base(),
    nb_items_to_keep_(-1), nb_rows_(-1), nb_rows_tot_(-1),
    decalage_local_global_(-1),
    matrice_symetrique_(-1),
    secmem_sz_(-1)
  {}
  ~Solv_Externe() {}

protected:
  void construit_matrice_morse_intermediaire(const Matrice_Base&, Matrice_Morse& );
  void construit_renum(const DoubleVect&);
  void MorseSymToMorse(const Matrice_Morse_Sym& MS, Matrice_Morse& M);
  void Create_lhs_rhs_onDevice();
  template<typename ExecSpace>
  void Update_lhs_rhs(const DoubleVect& b, DoubleVect& x);
  template<typename ExecSpace>
  void Update_solution(DoubleVect& x);

  TIDTab renum_;                // Tableau de renumerotation globale lignes matrice TRUST -> matrice CSR
  IntTab index_;                // Tableau de renumerotation locale
  ArrOfBit items_to_keep_;      // Faut t'il conserver dans la matrice CSR la ligne item de la matrice TRUST ?
  ArrOfTID ix;                  // Tableau de travail pour remplissage Vec plus rapide
  int nb_items_to_keep_;        // Nombre local d'items a conserver
  int nb_rows_;                 // Nombre de lignes locales de la matrice TRUST
  trustIdType nb_rows_tot_;             // Nombre de lignes globales de la matrice TRUST
  trustIdType decalage_local_global_;   // Decalage numerotation local/global pour matrice CSR et vecteur
  int matrice_symetrique_;      // Drapeau sur la symetrie de la matrice
  int secmem_sz_;               // (Local) second member size
  ArrOfDouble lhs_;             // Premier membre sans les items communs
  ArrOfDouble rhs_;             // Second membre sans les items communs
};


#endif //TRUST_SOLV_EXTERNE_H
