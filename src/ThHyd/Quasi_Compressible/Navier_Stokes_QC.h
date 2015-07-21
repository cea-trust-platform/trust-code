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
// File:        Navier_Stokes_QC.h
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Navier_Stokes_QC_included
#define Navier_Stokes_QC_included

#include <Navier_Stokes_std.h>
#include <Navier_Stokes_QC_impl.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Navier_Stokes_QC
//    Cette classe porte les termes de l'equation de la dynamique
//    pour un fluide sans modelisation de la turbulence.
//    On suppose l'hypothese de fluide quasi compressible.
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
//      Equation_base Pb_Thermohydraulique_QC Navier_Stokes_std
//////////////////////////////////////
////////////////////////////////////////
class Navier_Stokes_QC : public Navier_Stokes_std,public Navier_Stokes_QC_impl
{
  Declare_instanciable(Navier_Stokes_QC);

public :

  void completer();
  virtual int impr(Sortie& os) const;
  int preparer_calcul();

  const Champ_Don& diffusivite_pour_transport();
  const Champ_base& diffusivite_pour_pas_de_temps();
  const Champ_base& vitesse_pour_transport();

  DoubleTab& derivee_en_temps_inco(DoubleTab& );
  void assembler( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem) ;
  void assembler_avec_inertie( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem) ;

  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual bool initTimeStep(double dt);

  inline const Champ_Inc& rho_la_vitesse() const;
  void discretiser();
protected:



};
inline const Champ_Inc& Navier_Stokes_QC::rho_la_vitesse() const
{
  return rho_la_vitesse_;
}
#endif
