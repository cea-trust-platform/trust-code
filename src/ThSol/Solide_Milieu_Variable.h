/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Solide_Milieu_Variable.h
// Directory:   $TRUST_ROOT/src/ThSol
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Solide_Milieu_Variable_included
#define Solide_Milieu_Variable_included

#include <Milieu_base.h>
#include <Champ_Fonc.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Solide_Milieu_Variable
//    Cette classe represente un milieu solide et ses proprietes, avec la caracteristique que rho et Cp peuvent etre
//    non uniformes (lambda peut deja l'etre dans la classe Solide).
// .SECTION voir aussi
//    Milieu_base
//    Solide
//////////////////////////////////////////////////////////////////////////////
class Solide_Milieu_Variable : public Milieu_base
{
  Declare_instanciable(Solide_Milieu_Variable);

public:

  void set_param(Param& param);
  void verifier_coherence_champs(int& err,Nom& message);
  virtual void discretiser(const Probleme_base& pb, const  Discretisation_base& dis);
  void mettre_a_jour(double temps);
  int initialiser(const double& temps);
  void update_rho_cp(double temps);

  const Champ_base& get_rho_cp_comme_T() const
  {
    return rho_cp_comme_T_;
  };
  virtual const Champ_Don& capacite_calorifique() const
  {
    Cerr<<"Solide_Milieu_Variable::capacite_calorifique() not allowed"<<finl;
    abort();
    return capacite_calorifique();
  };
  virtual Champ_Don&       capacite_calorifique()
  {
    Cerr<<"Solide_Milieu_Variable::capacite_calorifique() not allowed"<<finl;
    abort();
    return capacite_calorifique();
  };

protected:
  Champ_Fonc rho_cp_elem_,rho_cp_comme_T_;

};

#endif
