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
// File:        Terme_Source_Canal_perio.h
// Directory:   $TRUST_ROOT/src/ThHyd/Incompressible/Sources
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////



#ifndef Terme_Source_Canal_perio_included
#define Terme_Source_Canal_perio_included

#include <Source_base.h>
#include <SFichier.h>
class Probleme_base;

// .DESCRIPTION
// Source term to keep a constant flow rate in a channel with periodic boundary conditions

class Terme_Source_Canal_perio : public Source_base
{

  Declare_base(Terme_Source_Canal_perio);

public:
  void associer_pb(const Probleme_base& ) override;
  void mettre_a_jour(double temps) override {}
  DoubleTab& calculer(DoubleTab& resu) const override;

protected :
  Entree& lire_donnees(Entree& );
  void completer() override;

  // This one is overridden depending on discret.
  virtual void calculer_debit(double&) const=0;

  ArrOfDouble source() const;
  void write_flow_rate(const Nom& ext_nom_source, double debit) const;
  double compute_heat_flux() const;
  // This one is overridden for P0 fields in VDF
  virtual ArrOfDouble source_convection_diffusion(double debit_e) const;

  ArrOfDouble dir_source_;
  int direction_ecoulement_;
  int velocity_weighting_;
  Nom bord_periodique_;
  double surface_bord_;
  double h,coeff,u_etoile;
  // ajout pour eviter des statics
  mutable int deb_;
  mutable double source_;
  mutable double debnm1_;
  mutable double debit_ref_;
  mutable double dernier_temps_calc_ ;
  int is_debit_impose_;
  double debit_impose_;

private:
  mutable SFichier flow_rate_file_;
  mutable SFichier pressure_gradient_file_;
  mutable SFichier restart_file_;
};


#endif
