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

#ifndef Partitionneur_Fichier_Decoupage_included
#define Partitionneur_Fichier_Decoupage_included

#include <Partitionneur_base.h>
#include <TRUST_Ref.h>

class Domaine;


/*! @brief Partition d'un domaine a partir d'un fichier disque contenant, pour chaque element, le numero du processeur auquel cet element est attache.
 *
 *   Le fichier doit contenir un tableau au format ArrOfInt
 *    (nombre n de valeurs suivi des valeurs).
 *   La taille du tableau doit etre egale au nombre d'elements
 *   du domaine a decouper et chaque valeur du tableau indique
 *   sur quel processeur cet element doit etre place.
 *
 */

class Partitionneur_Fichier_Decoupage : public Partitionneur_base
{
  Declare_instanciable(Partitionneur_Fichier_Decoupage);
public:
  void set_param(Param& param) override;
  void associer_domaine(const Domaine& domaine) override;
  void initialiser(const char *filename);
  void construire_partition(IntVect& elem_part, int& nb_parts_tot) const override;

protected:
  // Parametres du partitionneur
  REF(Domaine) ref_domaine_;
  Nom filename_;
  int corriger_partition_;
};
#endif
