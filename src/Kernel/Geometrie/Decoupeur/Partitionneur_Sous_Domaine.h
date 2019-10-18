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
// File:        Partitionneur_Sous_Domaine.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie/Decoupeur
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Partitionneur_Sous_Domaine_included
#define Partitionneur_Sous_Domaine_included

#include <Partitionneur_base.h>
#include <Ref_Domaine.h>


// .DESCRIPTION
//  Decoupeur permettant de decouper des sous-domaines (se recouvrant potentiellement)  crees par
//  Create_domain_from_sous_zone de maniere "conforme" : le sous-domaine est decoupe de maniere
//  "conforme" avec le domaine complet.
//
//  Utilisation:
//      - creer un domaine global, et deux sous-zones (qui se recouvrent partiellement) pour Domaine1 et Domaine2
//      - decouper le domaine global et ecrire le fichier de decoupe
//      - creer Domaine1 et Domaine2 par Create_domain_from_sous_zone
//      - decouper D1 et D2 par Partitionneur sous_domaine en prenant pour source le decoupage du domaine global.
//
//  Syntaxe:
//    Decouper dom_N
//    {
//        partitionneur sous_domaine
//        {
//             fichier     decoup/domaine_glob.txt
//             fichier_ssz sous_zone_dom_N.file
//        }
//        Nom_Zones decoup/dom_N
//    }
//
// .SECTION voir aussi
//    Partitionneur_Union Create_domain_from_sous_zone
class Partitionneur_Sous_Domaine : public Partitionneur_base
{
  Declare_instanciable(Partitionneur_Sous_Domaine);
public:
  void set_param(Param& param);
  void associer_domaine(const Domaine& dom) { };
  void construire_partition(ArrOfInt& elem_part, int& nb_parts_tot) const;

protected:
  // Parametres du partitionneur
  Nom filename_;      ///! Nom du fichier de decoupe globale
  Nom filename_ssz_;  ///! Nom du fichier de sous-zones
};
#endif
