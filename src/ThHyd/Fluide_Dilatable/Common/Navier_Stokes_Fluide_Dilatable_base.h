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
// File:        Navier_Stokes_Fluide_Dilatable_base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Common
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Navier_Stokes_Fluide_Dilatable_base_included
#define Navier_Stokes_Fluide_Dilatable_base_included

#include <Champ_Inc.h>
#include <Ref_IntVect.h>
#include <Navier_Stokes_std.h>
class DoubleTab;
class Entree;
class Sortie;
class Motcle;
class Fluide_base;
class Matrice;
class Assembleur;
class Matrice_Morse;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Navier_Stokes_Fluide_Dilatable_base
//    Cette classe basse porte les termes de l'equation de la dynamique
//    pour un fluide sans modelisation de la turbulence.
//    On suppose l'hypothese de fluide dilatable.
//    Sous ces hypotheses, on utilise la forme suivante des equations de
//    Navier_Stokes:
//       DU/dt = div(terme visqueux) - gradP/rho + sources/rho
//       div U = W
//    avec DU/dt : derivee particulaire de la vitesse
//         rho   : masse volumique
//    Rq : l'implementation de la classe permet bien sur de negliger
//         certains termes de l'equation (le terme visqueux, le terme
//         convectif, tel ou tel terme source).
//    L'inconnue est le champ de vitesse.
// .SECTION voir aussi
//      Navier_Stokes_std
//
//////////////////////////////////////

class Navier_Stokes_Fluide_Dilatable_base : public Navier_Stokes_std
{
  Declare_base_sans_constructeur(Navier_Stokes_Fluide_Dilatable_base);
public :
  Navier_Stokes_Fluide_Dilatable_base();
  void discretiser();
  int preparer_calcul();
  const Champ_Don& diffusivite_pour_transport();
  const Champ_base& diffusivite_pour_pas_de_temps();
  const Champ_base& vitesse_pour_transport();
  DoubleTab& rho_vitesse(const DoubleTab& tab_rho,const DoubleTab& vitesse,DoubleTab& rhovitesse) const;

  // Methodes virtuelles
  virtual DoubleTab& derivee_en_temps_inco(DoubleTab& );
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void completer();
  virtual void assembler( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem) ;
  virtual void assembler_avec_inertie( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem) ;
  virtual int impr(Sortie& os) const;
  virtual bool initTimeStep(double dt);

  // Methodes inlines
  inline const Champ_Inc& rho_la_vitesse() const { return rho_la_vitesse_; }

protected:
  Champ_Inc rho_la_vitesse_;
  IntVect orientation_VDF_;
  DoubleTab tab_W; // RHS of div(rho.U)

private:
  mutable double cumulative_;
};

#endif /* Navier_Stokes_Fluide_Dilatable_base_included */
