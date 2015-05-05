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
// File:        Champ_Generique_modifier_pour_QC.h
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Champ_Generique_modifier_pour_QC_included
#define Champ_Generique_modifier_pour_QC_included

#include <Champ_Gen_de_Champs_Gen.h>
#include <Ref_Milieu_base.h>

// .DESCRIPTION class Champ_Generique_modifier_pour_QC
// Champ destine a post-traiter un champ d un probleme en quasi-compressible
// que l on souhaite multiplier ou diviser par la masse volumique (rho).
// La classe porte une REF au milieu qui doit etre de type Fluide_Quasi_Compressible.

//// Syntaxe a respecter pour jdd
//
// "nom_champ" modifier_pour_QC {
//                 [division]
//                source "type_champ_gen" { ... source ref_Champ { Pb_champ "nom_pb" "nom_champ_discret" } }
//               }
// "nom_champ" fixe par utilisateur sera le nom du champ generique
// "division" active la division du champ par rho sinon le champ est multiplie par rho
// "type_champ_gen" type d'un champ generique

class Champ_Generique_modifier_pour_QC : public Champ_Gen_de_Champs_Gen
{
  Declare_instanciable_sans_constructeur(Champ_Generique_modifier_pour_QC);

public:

  Champ_Generique_modifier_pour_QC();
  void set_param(Param& param);
  void completer(const Postraitement_base& post);
  virtual const Champ_base&  get_champ(Champ& espace_stockage) const;
  void nommer_source();

protected:

  int diviser_;
  REF(Milieu_base) mon_milieu_;
};

#endif

