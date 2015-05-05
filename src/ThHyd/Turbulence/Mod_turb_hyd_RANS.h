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
// File:        Mod_turb_hyd_RANS.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Mod_turb_hyd_RANS_included
#define Mod_turb_hyd_RANS_included

#include <Mod_turb_hyd_base.h>

class Equation_base;
class Transport_K_Eps_base;


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Mod_turb_hyd_RANS
//    Classe de base des modeles de type RANS
// .SECTION voir aussi
//    Mod_turb_hyd_base
//////////////////////////////////////////////////////////////////////////////
class Mod_turb_hyd_RANS : public Mod_turb_hyd_base
{

  Declare_base_sans_constructeur(Mod_turb_hyd_RANS);

public:

  Mod_turb_hyd_RANS();
  void set_param(Param& param);
  virtual int nombre_d_equations() const=0;
  virtual Transport_K_Eps_base& eqn_transp_K_Eps()=0;
  virtual const Transport_K_Eps_base& eqn_transp_K_Eps() const=0;
  virtual void completer();

  void verifie_loi_paroi();
  int sauvegarder(Sortie& os) const;
  int reprendre(Entree& is);

  virtual const Equation_base& equation_k_eps(int) const=0 ;

  inline double get_Prandtl_K() const;
  inline double get_Prandtl_Eps() const;
  inline double get_LeEPS_MIN() const;
  inline double get_LeK_MIN() const;

  //Methodes de l interface des champs postraitables
  /////////////////////////////////////////////////////
  //Methode creer_champ non codee a surcharger si necessaire
  //virtual void creer_champ(const Motcle& motlu);
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const;
  /////////////////////////////////////////////////////

protected:

  double Prandtl_K, Prandtl_Eps;
  double LeEPS_MIN, LeK_MIN ;

};

inline double Mod_turb_hyd_RANS::get_Prandtl_K() const
{
  return Prandtl_K;
}

inline double Mod_turb_hyd_RANS::get_Prandtl_Eps() const
{
  return Prandtl_Eps;
}

inline double Mod_turb_hyd_RANS::get_LeEPS_MIN() const
{
  return LeEPS_MIN;
}

inline double Mod_turb_hyd_RANS::get_LeK_MIN() const
{
  return LeK_MIN;
}

#endif
