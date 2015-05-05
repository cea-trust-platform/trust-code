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
// File:        NettoieNoeuds.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#ifndef NettoieNoeuds_included
#define NettoieNoeuds_included




//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe NettoieNoeuds
//    x->alpha x
// .SECTION voir aussi
//    Interprete Pave
//    Actuellement le seul tyep d'objet reconnu par Trio-U pour mailler
//    une domaine est l'objet Pave
//////////////////////////////////////////////////////////////////////////////
#include <Interprete_geometrique_base.h>
class Domaine;
class NettoieNoeuds : public Interprete_geometrique_base
{
  Declare_instanciable(NettoieNoeuds);
public :
  Entree& interpreter_(Entree&);
  static void nettoie(Domaine&);
  static void verifie_noeuds(const Domaine&);
  static int NettoiePasNoeuds;
};

#endif

