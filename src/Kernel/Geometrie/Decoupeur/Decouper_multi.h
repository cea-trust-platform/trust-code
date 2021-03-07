/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Decouper_multi.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie/Decoupeur
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Decouper_multi_included
#define Decouper_multi_included

#include <Decouper.h>
#include <Raccord_base.h>
#include <Interprete.h>

// .DESCRIPTION
//  Interprete Decouper_multi. Decoupage simultane de plusieurs domaine
//  avec renseignement des sommets connectes par des Raccords (pour extension
//  des espaces virtuels)
class Decouper_multi : public Interprete
{
  Declare_instanciable(Decouper_multi);

public:
  Entree& interpreter(Entree& is);
  virtual int lire_motcle_non_standard(const Motcle&, Entree&);

private:
  std::map<std::string, Decouper> decoupeurs; //decoupeurs de chaque domaine
  std::vector<std::array<const Raccord_base *, 2>> racc_pairs; //liste de paires de raccords a connecter
  double tolerance = 1e-8; //tolerance geometrique
};

#endif
