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
// File:        Partitionneur_Tranche.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie/Decoupeur
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Partitionneur_Tranche_included
#define Partitionneur_Tranche_included

#include <Partitionneur_base.h>
#include <Ref_Domaine.h>
#include <ArrOfInt.h>

// .DESCRIPTION
//  Partitionneur de domaine en tranches paralleles aux directions
//  de l'espace. Voir construire_partition
class Partitionneur_Tranche : public Partitionneur_base
{
  Declare_instanciable(Partitionneur_Tranche);
public:
  void set_param(Param& param);
  void associer_domaine(const Domaine& domaine);
  void initialiser(const ArrOfInt& nb_tranches);
  void construire_partition(ArrOfInt& elem_part, int& nb_parts_tot) const;
  static void chercher_direction_perio(const Zone& zone,
                                       const Noms& liste_bords_perio,
                                       ArrOfInt& directions_perio);

private:
  // Parametres du partitionneur
  REF(Domaine) ref_domaine_;
  // Pour chaque dimension d'espace, 2 ou 3, nombre de tranches
  // a creer dans cette direction (>=1)
  ArrOfInt nb_tranches_;
};
#endif
