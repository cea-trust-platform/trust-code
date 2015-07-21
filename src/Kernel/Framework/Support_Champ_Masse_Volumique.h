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
// File:        Support_Champ_Masse_Volumique.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Support_Champ_Masse_Volumique_included
#define Support_Champ_Masse_Volumique_included

#include <Ref_Champ_base.h>

class Support_Champ_Masse_Volumique
{
public:
  Support_Champ_Masse_Volumique();
  virtual ~Support_Champ_Masse_Volumique();

  virtual void               associer_champ_masse_volumique(const Champ_base&);
  virtual void               dissocier_champ_masse_volumique();
  virtual int             has_champ_masse_volumique() const;
  virtual const Champ_base& get_champ_masse_volumique() const;
  inline const int& support_ok() const;
protected:
  virtual void               declare_support_masse_volumique(int ok);
private:
  // L'objet derive utilise bien le champ masse volumique.
  // Voir le constructeur.
  int          support_ok_;
  // Reference au champ de masse volumique a utiliser.
  // La ref peut etre nulle.
  REF(Champ_base) ref_champ_rho_;
};

inline const int& Support_Champ_Masse_Volumique::support_ok() const
{
  return support_ok_;
}

#endif

