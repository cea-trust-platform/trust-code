/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Champ_Generique_Reduction_0D.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Champ_Generique_Reduction_0D_included
#define Champ_Generique_Reduction_0D_included

#include <Champ_Gen_de_Champs_Gen.h>

//
// .DESCRIPTION class Champ_Generique_Reduction_0D
//

// Champ destine a post-traiter un champ reduit a une dimension 0D
// On construit un champ prenant en tout point de l espace la valeur reduite
// La classe porte le type de methode pour realiser la reduction
//// Syntaxe a respecter pour jdd
//
// "nom_champ" Reduction_0D { methode "type_methode"
//                source "type_champ_gen" { ...source ref_Champ { Pb_champ "nom_pb" "nom_champ_discret" } }
//                [ nom_source "nom_source" ]
//                [ source_reference "source_reference" ]
//                [ sources_reference "list_nom_virgule" ]
//                [ sources "listchamp_generique" ]
//               }
// "nom_champ" fixe par utilisateur sera le nom du champ generique
// "type_champ_gen" type d'un champ generique
// "type_methode" indique le type de reduction demandee
// Les options possibles sont disponibles dans $TRUST_ROOT/src/Kernel/Champs/Champ_Generique_Reduction_0D.cpp

class Champ_Generique_Reduction_0D : public Champ_Gen_de_Champs_Gen
{

  Declare_instanciable(Champ_Generique_Reduction_0D);

public:

  void set_param(Param& param);
  void completer(const Postraitement_base& post);
  virtual const Noms get_property(const Motcle& query) const;
  virtual const Champ_base&  get_champ(Champ& espace_stockage) const;
  virtual const Champ_base&  get_champ_without_evaluation(Champ& espace_stockage) const;
  void nommer_source();
  void extraire(double& val_extraites, const DoubleVect& val_source, const int& composante_VDF=-1) const;

protected:

  Motcle methode_; //Type de reduction : min, max, moyenne ou somme
  int numero_proc_; // numero du proc contenant la maille la plus a gauche
  int numero_elem_;    // numero local de la maille la plus a gauche sur numero_proc_
  mutable DoubleVect volume_controle_; //Tableau de travail
};

#endif

