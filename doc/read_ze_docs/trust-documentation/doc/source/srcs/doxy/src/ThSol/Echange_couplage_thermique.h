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

#ifndef Echange_couplage_thermique_included
#define Echange_couplage_thermique_included

#include <Echange_global_impose.h>
#include <Lecture_Champ.h>

/*! @brief : class Echange_couplage_thermique
 *
 */
class Echange_couplage_thermique : public Echange_global_impose
{
  Declare_instanciable( Echange_couplage_thermique ) ;
public :
  void completer() override;
  void set_temps_defaut(double temps) override;
  void changer_temps_futur(double temps, int i) override;
  int avancer(double temps) override;
  int reculer(double temps) override;
  void mettre_a_jour(double temps) override;
  int initialiser(double temps) override;

  double champ_exterieur(int i,int j, const Champ_front& champ_ext) const override;
  inline double champ_exterieur(int i, const Champ_front& champ_ext) const override { return champ_exterieur(i, 0, champ_ext); };

  inline double flux_exterieur_impose(int i) const override { return couplage_*champ_exterieur(i,phi_ext()); }
  inline double flux_exterieur_impose(int i,int j) const override { return couplage_*champ_exterieur(i,j,phi_ext()); };

  inline Champ_front& T_p() { return T_ext(); }
  inline const Champ_front& T_p() const { return T_ext(); }

  bool reprise() const { return reprise_; }
  inline Champ_front& a_p() { return coeff_ap; }
  inline const Champ_front& a_p() const { return coeff_ap; }

  inline Champ_front& s_p() { return coeff_sp; }
  inline const Champ_front& s_p() const { return coeff_sp; }

  // Coefficient d'echange effectif
  inline Champ_front& h_eff() { return coeff_heff; }
  inline const Champ_front& h_eff() const { return coeff_heff; }

  // Température effective
  inline Champ_front& T_eff() { return temperature_Teff; }
  inline const Champ_front& T_eff() const { return temperature_Teff; }

protected :
  double couplage_ = -1.;
  Champ_front coeff_ap;
  Champ_front coeff_sp;
  Champ_front coeff_heff;
  Champ_front temperature_Teff;
  Lecture_Champ lec_champs;
  bool reprise_ = false, divise_par_rho_cp_ = true;
};

#endif /* Echange_couplage_thermique_included */
