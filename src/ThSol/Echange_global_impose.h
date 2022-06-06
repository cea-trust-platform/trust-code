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

#ifndef Echange_global_impose_included
#define Echange_global_impose_included




#include <Echange_impose_base.h>
#include <Ref_Milieu_base.h>




//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Echange_global_impose
//    Cette classe represente le cas particulier de la classe
//    Echange_impose_base ou l'echange de chaleur total est calcule grace au
//    coefficient d'echange de chaleur global fourni par l'utilisateur.
//
//    ATTENTION, dans le cas des discretisations ou T est localisee au centre de l'element (VDF)
//    cela correspond a la modelisation d'une paroi d'echange dont l'epaisseur s'etend jusqu'a la moitie
//    de la premiere maille.
//
//     h_t   : coefficient d'echange total
//     h_imp_: coefficient d'echange global (donnee utilisateur)
//    On a: h_t = h_imp
//    Les champs h_imp et T_ext sont uniformes
//
// .SECTION voir aussi
//    Echange_impose_base Echange_externe_impose
//////////////////////////////////////////////////////////////////////////////
class Echange_global_impose : public Echange_impose_base
{

  Declare_instanciable(Echange_global_impose);
public:
  int compatible_avec_discr(const Discretisation_base& discr) const override;

  void completer() override;
  void set_temps_defaut(double temps) override;
  void changer_temps_futur(double temps, int i) override;
  int avancer(double temps) override;
  int reculer(double temps) override;
  void mettre_a_jour(double temps) override;
  int initialiser(double temps) override;

  inline Champ_front& derivee_phi_ext();
  inline const Champ_front& derivee_phi_ext() const;

  inline Champ_front& phi_ext();
  inline const Champ_front& phi_ext() const;

  virtual double champ_exterieur(int i,int j, const Champ_front& champ_ext) const;
  virtual double champ_exterieur(int i, const Champ_front& champ_ext) const;

  virtual double flux_exterieur_impose(int i) const;
  virtual double flux_exterieur_impose(int i,int j) const;

  virtual double derivee_flux_exterieur_imposee(int i) const;
  virtual double derivee_flux_exterieur_imposee(int i,int j) const;

  const bool& has_phi_ext() const { return phi_ext_lu_; };

protected :

  bool phi_ext_lu_;
  Champ_front derivee_phi_ext_;
  Champ_front phi_ext_;

};

inline const Champ_front& Echange_global_impose::derivee_phi_ext() const
{
  return derivee_phi_ext_;
}

inline Champ_front& Echange_global_impose::derivee_phi_ext()
{
  return derivee_phi_ext_;
}

inline const Champ_front& Echange_global_impose::phi_ext() const
{
  return phi_ext_;
}

inline Champ_front& Echange_global_impose::phi_ext()
{
  return phi_ext_;
}

#endif
