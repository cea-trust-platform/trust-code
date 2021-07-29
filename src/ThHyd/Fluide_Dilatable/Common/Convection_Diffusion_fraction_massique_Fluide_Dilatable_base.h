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
// File:        Convection_Diffusion_fraction_massique_Fluide_Dilatable_base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Common
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Convection_Diffusion_fraction_massique_Fluide_Dilatable_base_included
#define Convection_Diffusion_fraction_massique_Fluide_Dilatable_base_included

#include <Convection_Diffusion_std.h>
#include <Ref_Fluide_Dilatable_base.h>

class Fluide_Dilatable_base;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Convection_Diffusion_fraction_massique_Fluide_Dilatable_base
//     Cas particulier de Convection_Diffusion_std pour un fluide dilatable
//     quand le scalaire subissant le transport est la fraction massique
// .SECTION voir aussi
//     Conv_Diffusion_std
//////////////////////////////////////////////////////////////////////////////

class Convection_Diffusion_fraction_massique_Fluide_Dilatable_base : public Convection_Diffusion_std
{
  Declare_base(Convection_Diffusion_fraction_massique_Fluide_Dilatable_base);

public :
  void associer_fluide(const Fluide_Dilatable_base& );
  void discretiser();
  void associer_milieu_base(const Milieu_base& );
  int sauvegarder(Sortie&) const;
  int reprendre(Entree&);
  const Milieu_base& milieu() const;
  const Fluide_Dilatable_base& fluide() const;
  const Champ_Don& diffusivite_pour_transport();
  Fluide_Dilatable_base& fluide();
  Milieu_base& milieu();

  virtual void completer();
  virtual void set_param(Param& titi);
  virtual int lire_motcle_non_standard(const Motcle&, Entree&);
  virtual int preparer_calcul();
  virtual int impr(Sortie& os) const;
  virtual const Motcle& domaine_application() const;

  // Methodes virtuelles pure
  virtual void assembler( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem) = 0;
  virtual const Champ_base& diffusivite_pour_pas_de_temps() = 0;
  virtual DoubleTab& derivee_en_temps_inco(DoubleTab& ) = 0;

  // Methodes inlines
  inline const Champ_Inc& inconnue() const { return l_inco_ch; }
  inline Champ_Inc& inconnue() { return l_inco_ch; }

protected :
  Champ_Inc l_inco_ch;
  Zone_Cl_dis zcl_modif_;
  REF(Fluide_Dilatable_base) le_fluide;
};

#endif /* Convection_Diffusion_fraction_massique_Fluide_Dilatable_base_included */
