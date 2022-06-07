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

#ifndef DomaineAxi1d_included
#define DomaineAxi1d_included

#include <Domaine.h>
#include <Champ.h>
#include <Ref_DoubleTab.h>

/////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION : class DomaineAxi1d
//
// <Description of class DomaineAxi1d>
//
/////////////////////////////////////////////////////////////////////////////

class DomaineAxi1d : public Domaine
{

  Declare_instanciable( DomaineAxi1d ) ;

public :

  const Champ& champ_origine() const;
  const Champ& champ_origine();
  const DoubleTab& origine_repere();
  const DoubleTab& origine_repere() const;
  void associer_origine_repere(const DoubleTab& orig);
  inline double origine_repere(int i,int j);
  inline double origine_repere(int i,int j) const;


protected :

  Champ champ_orig;
  Ref_DoubleTab ref_origine_;
};

inline double DomaineAxi1d::origine_repere(int i,int j)
{
  return ref_origine_.valeur()(i,j);
}

inline double DomaineAxi1d::origine_repere(int i,int j) const
{
  return ref_origine_.valeur()(i,j);
}

#endif /* DomaineAxi1d_included */
