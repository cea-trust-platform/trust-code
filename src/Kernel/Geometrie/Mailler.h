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
// File:        Mailler.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Mailler_included
#define Mailler_included




//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Mailler
//    Un mailleur par agglomeration de zones (paves pour le moment)
//    Structure du jeu de donnee (en dimension 2) :
//    Mailler dom
//    {
//    [Epsilon eps]
//    Pave nompave1
//    {
//       Origine OX OY
//          Longueurs LX LY
//          Nombre_de_Noeuds NX NY
//    }
//    {
//       Bord nom_bord1 X = X0 Y0 <= Y <= Y1
//       Bord nom_bord2 X = X0 Y1 <= Y <= Y2
//       Bord nom_bord2 Y = Y0 X1 <= X <= X2
//       ...
//    } ,
//    Pave nompave2 ...
//    }
//    Deux points seront confondus des que la distance entre eux est
//    inferieure a Epsilon.
// .SECTION voir aussi
//    Interprete Pave
//    Actuellement le seul tyep d'objet reconnu par Trio-U pour mailler
//    une domaine est l'objet Pave
//////////////////////////////////////////////////////////////////////////////
#include <Interprete_geometrique_base.h>
class Mailler : public Interprete_geometrique_base
{
  Declare_instanciable(Mailler);
public :
  Entree& interpreter_(Entree&);
};
#endif
