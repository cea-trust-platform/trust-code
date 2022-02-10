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
// File:        Operateur_Diff.h
// Directory:   $TRUST_ROOT/src/Kernel/Operateurs
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Operateur_Diff_included
#define Operateur_Diff_included

#include <Operateur.h>
#include <Ref_Champ_base.h>

class Operateur_Diff_base;
Declare_deriv(Operateur_Diff_base);

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Operateur_Diff
//    Classe generique de la hierarchie des operateurs representant un terme
//    de diffusion. Un objet Operateur_Diff peut referencer n'importe quel
//    objet derivant de Operateur_Diff_base.
// .SECTION voir aussi
//    Operateur_Diff_base Operateur
//////////////////////////////////////////////////////////////////////////////

class Operateur_Diff  : public Operateur,
  public DERIV(Operateur_Diff_base)
{
  Declare_instanciable(Operateur_Diff);
public:
  Operateur_base&   l_op_base() override;
  const Operateur_base& l_op_base() const override;
  DoubleTab&        ajouter(const DoubleTab&, DoubleTab& ) const override;
  DoubleTab&        calculer(const DoubleTab&, DoubleTab& ) const override;
  void              associer_diffusivite(const Champ_base&);
  void              associer_diffusivite_pour_pas_de_temps(const Champ_base&);
  const Champ_base& diffusivite() const;
  void              typer() override;
  void              typer(const Nom&);
  inline int op_non_nul() const override;

protected:
  REF(Champ_base) la_diffusivite;
};
inline int Operateur_Diff::op_non_nul() const
{
  if (non_nul())
    return 1;
  else
    return 0;
}
#endif
