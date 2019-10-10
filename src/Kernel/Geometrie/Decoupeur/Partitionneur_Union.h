/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Partitionneur_Union.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie/Decoupeur
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Partitionneur_Union_included
#define Partitionneur_Union_included

#include <Partitionneur_base.h>
#include <Ref_Domaine.h>
#include <map>
#include <string>

// .DESCRIPTION
// Decoupeur permettant de decouper un domaine global de maniere conforme avec un ensemble de sous-domaines
// deja decoupes. C'est l'operation inverse de Partitionneur_Sous_Domaine.
//
// Syntaxe:
//    partitionneur union
//    {
//      <sous-zone 1>  <fichier ecrire_decoupage du sous-domaine 1>
//      ...
//      <sous-zone N>  <fichier ecrire_decoupage du sous-domaine N>
//    }
//
// .SECTION voir aussi
//    Partitionneur_Sous_Domaine Create_domain_from_sous_zone
class Partitionneur_Union : public Partitionneur_base
{
  Declare_instanciable(Partitionneur_Union);
public:
  void set_param(Param& param) { };
  void associer_domaine(const Domaine& dom);
  void initialiser(const char *filename, const char *filename_ssz);
  void construire_partition(ArrOfInt& elem_part, int& nb_parts_tot) const;

protected:
  // Parametres du partitionneur
  REF(Domaine) ref_domaine_;
  std::map<std::string, std::string> fic_ssz; //fic_ssz[nom de la sous zone] = { fichier de decoupage }
};
#endif
