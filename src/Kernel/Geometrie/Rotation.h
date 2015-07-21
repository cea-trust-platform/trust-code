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
// File:        Rotation.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Rotation_included
#define Rotation_included

#include <Transformer.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Rotation
//    Applique une rotation autour d un axe aligne avec un des axes du repere.
//    Structure du jeu de donnee :
//    Rotation nom_domaine nom_axe coord1 coord2 angle
//    nom_domaine   : nom du domaine auquelon applique une rotation
//    nom_axe       : axe (X, Y ou Z) avec lequel est aligne l axe de rotation
//    coord1 coord2 : coordonnees (sens dtrigonometrique direct) du centre de rotation
//                      dans le plan orthogonal a l axe de rotation
//////////////////////////////////////////////////////////////////////////////

class Rotation : public Transformer
{
  Declare_instanciable(Rotation);
public :
  Entree& interpreter_(Entree&);
};
#endif

