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
/////////////////////////////////////////////////////////////////////////////
//
// File      : Champ_Generique_Champ.h
// Directory : $BASIC_ROOT/src
//
/////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Generique_Champ_included
#define Champ_Generique_Champ_included

#include <Champ_Generique_refChamp.h>
#include <Champ.h>

/////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION : class Champ_Generique_Champ
//
// <Description of class Champ_Generique_Champ>
//
/////////////////////////////////////////////////////////////////////////////

class Champ_Generique_Champ : public Champ_Generique_refChamp
{

  Declare_instanciable( Champ_Generique_Champ ) ;
  void set_param(Param& param);
  int lire_motcle_non_standard(const Motcle&, Entree&);
  virtual const Champ_base& get_champ(Champ& espace_stockage) const;
  const Champ_base& get_ref_champ_base() const;
  virtual void reset();
  void mettre_a_jour(double temps);
public :

protected :
  Champ champ_;
};

#endif /* Champ_Generique_Champ_included */
