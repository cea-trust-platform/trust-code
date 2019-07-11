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
// File:        Op_Conv_ALE.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_Conv_ALE_included
#define Op_Conv_ALE_included

#include <Operateur_Conv.h>
#include <Ref_Champ_Inc_base.h>


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class Op_Conv_ALE : public Operateur_Conv_base
{
  Declare_base(Op_Conv_ALE);

public :

  void associer_vitesse(const Champ_base& vit );
  virtual DoubleTab& ajouter(const DoubleTab&, DoubleTab& ) const;
  virtual DoubleTab& calculer(const DoubleTab&, DoubleTab& ) const;
  virtual DoubleTab& ajouterALE(const DoubleTab&, DoubleTab& ) const=0;


protected :
  Operateur_Conv op_conv;
  virtual void associer(const Zone_dis&,
                        const Zone_Cl_dis&,
                        const Champ_Inc& inco) ;//Classe abstraite de Operateur base
  REF(Domaine) dom;
  REF(Champ_Inc_base) la_vitesse;
};

#endif
