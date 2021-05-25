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
// File:        Op_Conv_EF_Stab_CoviMAC_Elem.h
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_Conv_EF_Stab_CoviMAC_Elem_included
#define Op_Conv_EF_Stab_CoviMAC_Elem_included

#include <Op_Conv_CoviMAC_base.h>
#include <Matrice_Morse.h>

/////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION : class Op_Conv_EF_Stab_CoviMAC_Elem
//
// <Description of class Op_Conv_EF_Stab_CoviMAC_Elem>
//
/////////////////////////////////////////////////////////////////////////////

class Op_Conv_EF_Stab_CoviMAC_Elem : public Op_Conv_CoviMAC_base
{

  Declare_instanciable( Op_Conv_EF_Stab_CoviMAC_Elem ) ;

public :
  void preparer_calcul();
  void modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const { };
  double calculer_dt_stab() const;

  /* interface ajouter_blocs */
  int has_interface_blocs() const
  {
    return 1;
  };
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const;
  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const;

  void check_multiphase_compatibility() const { }; //of course

  void creer_champ(const Motcle& motlu);

  void mettre_a_jour(double temps);

  double alpha; //alpha = 0 -> centre, alpha = 1 -> amont
protected :

  /* si operateur de convection de Masse_Multiphase */
  std::vector<Champ_Inc> cc_phases_; //flux massiques (kg/m2/s)
  Motcles noms_cc_phases_; //leurs noms
  std::vector<Champ_Inc> vd_phases_; //vitesses debitantes
  Motcles noms_vd_phases_; //leurs noms
  std::vector<Champ_Inc> x_phases_; //titres par phase
  Motcles noms_x_phases_; //leurs noms
};

class Op_Conv_Amont_CoviMAC_Elem : public Op_Conv_EF_Stab_CoviMAC_Elem
{
  Declare_instanciable( Op_Conv_Amont_CoviMAC_Elem ) ;
};

class Op_Conv_Centre_CoviMAC_Elem : public Op_Conv_EF_Stab_CoviMAC_Elem
{
  Declare_instanciable( Op_Conv_Centre_CoviMAC_Elem ) ;
};

#endif /* Op_Conv_EF_Stab_CoviMAC_Elem_included */
