/****************************************************************************
* Copyright (c) 2025, CEA
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

#ifndef Echange_externe_radiatif_included
#define Echange_externe_radiatif_included

#include <Cond_lim_base.h>

/*! @brief classe Echange_externe_radiatif: // TODO Yannick FIXME
 *
 * @sa Cond_lim_base
 */
class Echange_externe_radiatif : public Cond_lim_base
{
  Declare_instanciable(Echange_externe_radiatif);
public:
  /*! @brief Renvoie le champ T_ext de temperature imposee a la frontiere.
   *
   * @return (Champ_front_base&) le champ T_ext de temperature imposee a la frontiere
   */
  inline Champ_front_base& T_ext() { return le_champ_front; }
  inline const Champ_front_base& T_ext() const { return le_champ_front; }

  inline Champ_front_base& emissivite() { return emissivite_; }
  inline const Champ_front_base& emissivite() const  { return emissivite_; }

  double emissivite(int num) const;
  double emissivite(int num,int k) const;

  double T_ext(int num) const;
  double T_ext(int num,int k) const;

  void verifie_ch_init_nb_comp() const override;

  void mettre_a_jour(double ) override;
  int initialiser(double temps) override;
  int a_mettre_a_jour_ss_pas_dt() override { return 1; }

  // ajout de methode pour ne pas operer directement sur champ_front
  void set_temps_defaut(double temps) override;
  void fixer_nb_valeurs_temporelles(int nb_cases) override;

  void changer_temps_futur(double temps,int i) override;
  int avancer(double temps) override;
  int reculer(double temps) override;
  void associer_fr_dis_base(const Frontiere_dis_base& ) override ;

protected :
  OWN_PTR(Champ_front_base) emissivite_;
};

#endif /* Echange_externe_radiatif_included */
