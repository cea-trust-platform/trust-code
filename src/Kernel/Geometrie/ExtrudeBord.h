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
// File:        ExtrudeBord.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#ifndef ExtrudeBord_included
#define ExtrudeBord_included



//
// .DESCRIPTION class ExtrudeBord
/**
 * Realise une extrusion d'un bord.
 * ExtrudeBord peut extruder un bord maille soit en triangle soit en quadrangle
 *  - si  le bord est maille en quadrangle, le domaine extrude sera compose d'hexaedre
 *  - si  le bord est maille en triangle, le domaine extrude sera compose de tetraedres
 * La syntaxte est la suivante :
 *         ExtrudeBord
 *                 {
 *                 domaine_init         NOM_DOMAINE_DE_DEPART
 *                 direction         X Y Z
 *                 nb_tranches         N
 *                 domaine_final         NOM_DU_DOMAINE_EXTRUDE
 *                 nom_bord         NOM_BORD_A_EXTRUDER
 *                hexa_old  //mot cle a mettre pour utiliser l'ancienne version de l'extrusion des hexas
 *                 }
 *
 * (MODIF OC 12/2004)
 */
//
// .SECTION voir aussi
// Interprete

#include <Interprete_geometrique_base.h>

#include <DoubleVect.h>

class ExtrudeBord : public Interprete_geometrique_base
{
  Declare_instanciable_sans_constructeur(ExtrudeBord);

public :

  ExtrudeBord();
  Entree& interpreter_(Entree&) override;

private:

  void extruder_bord(Nom& nom_front, Nom& nom_dom2, DoubleVect& vect_dir, int nbpas);
  void extruder_hexa_old(Nom& nom_front, Nom& nom_dom2, DoubleVect& vect_dir, int nbpas);
  int hexa_old; // flag pour ancienne version de l'extrusion des hexas : 0 = ancienne version
  int Trois_Tetra; // flag pour extrusion en trois tetraedres plutot qu'en 14 (option par defaut)
  int Vingt_Tetra; // flag pour extrusion en vingr tetraedres plutot qu'en 14 (option par defaut)
  int en3D_;
};

#endif

