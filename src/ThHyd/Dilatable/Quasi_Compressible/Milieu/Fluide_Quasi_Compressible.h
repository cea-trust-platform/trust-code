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

#ifndef Fluide_Quasi_Compressible_included
#define Fluide_Quasi_Compressible_included

#include <Fluide_Dilatable_base.h>
#include <EDO_Pression_th.h>

class Zone_Cl_dis;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Fluide_Quasi_Compressible
//    Cette classe represente un d'un fluide quasi compressible
//    c'est a dire heritant de fluide dilatable
// .SECTION voir aussi
//     Milieu_base Fluide_Dilatable_base
//
//////////////////////////////////////////////////////////////////////////////

class Fluide_Quasi_Compressible : public Fluide_Dilatable_base
{
  Declare_instanciable_sans_constructeur(Fluide_Quasi_Compressible);
public :
  Fluide_Quasi_Compressible();
  void set_param(Param& param) override;
  void completer(const Probleme_base&) override;
  void preparer_pas_temps() override;
  void prepare_pressure_edo() override;
  void write_mean_edo(double) override;
  void checkTraitementPth(const Zone_Cl_dis& zone_cl) override;
  void discretiser(const Probleme_base& pb, const  Discretisation_base& dis) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;

  // Methodes inlines
  inline void Resoudre_EDO_PT();
  inline void secmembre_divU_Z(DoubleTab& ) const override;
  inline int get_traitement_rho_gravite() const { return traitement_rho_gravite_; }
  inline double masse_totale(double P,const DoubleTab& T) { return EDO_Pth_->masse_totale( P, T); }

protected :
  int traitement_rho_gravite_; // 0 : gravite = rho*g, 1 : gravite =(rho-rho_moy)*g
  double temps_debut_prise_en_compte_drho_dt_,omega_drho_dt_;
  EDO_Pression_th EDO_Pth_;
  Nom output_file_;
  mutable DoubleTab tab_W_old_;

private :
  void completer_edo(const Probleme_base& );
  void remplir_champ_pression_tot(int n, const DoubleTab& PHydro, DoubleTab& PTot) override;
};

inline void Fluide_Quasi_Compressible::Resoudre_EDO_PT()
{
  Pth_n = Pth_;

  if (traitement_PTh != 2)
    {
      Pth_ = EDO_Pth_->resoudre(Pth_);

      if (Pth_<=0)
        {
          Cerr<<"Error : the pressure calculated by Resoudre_EDO_PT method is negative : "<< Pth_ << finl;
          abort();
        }

      EDO_Pth_->mettre_a_jour_CL(Pth_);
    }
}

//Calcule W=-dZ/dt, 2nd membre de l'equation div(rhoU) = W
inline void Fluide_Quasi_Compressible::secmembre_divU_Z(DoubleTab& tab_W) const
{
  double temps=vitesse_->temps();
  if (temps>temps_debut_prise_en_compte_drho_dt_)
    {
      eos_tools_->secmembre_divU_Z(tab_W);
      if (tab_W_old_.size_totale() > 0) // Relaxation eventuelle:
        {
          tab_W *= omega_drho_dt_;
          tab_W.ajoute_sans_ech_esp_virt(1-omega_drho_dt_, tab_W_old_);
        }
      tab_W_old_ = tab_W;
    }
  else
    {
      if (je_suis_maitre())
        Cerr<<" The drho_dt term is not taken into account for time smaller than " <<temps_debut_prise_en_compte_drho_dt_<<finl;
      tab_W=0;
    }
}

#endif /* Fluide_Quasi_Compressible_included */
