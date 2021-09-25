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
// File:        Fluide_Weakly_Compressible.h
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Weakly_Compressible/Milieu
// Version:     /main/29
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Fluide_Weakly_Compressible_included
#define Fluide_Weakly_Compressible_included

#include <Fluide_Dilatable_base.h>
#include <Ref_Probleme_base.h>

class Zone_Cl_dis;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Fluide_Weakly_Compressible
//    Cette classe represente un d'un fluide faiblement compressible
//    c'est a dire heritant de Fluide_Dilatable_base
// .SECTION voir aussi
//     Milieu_base Fluide_Dilatable_base
//////////////////////////////////////////////////////////////////////////////

class Fluide_Weakly_Compressible : public Fluide_Dilatable_base
{
  Declare_instanciable_sans_constructeur(Fluide_Weakly_Compressible);

public :
  Fluide_Weakly_Compressible();
  virtual void set_param(Param& param);
  virtual void completer(const Probleme_base&);
  virtual void preparer_pas_temps();
  virtual void prepare_pressure_edo();
  virtual void write_mean_edo(double);
  virtual void checkTraitementPth(const Zone_Cl_dis&);
  virtual void discretiser(const Probleme_base& pb, const  Discretisation_base& dis);
  virtual void abortTimeStep();
  virtual void calculer_pression_tot();
  virtual void update_pressure_fields(double );
  virtual int lire_motcle_non_standard(const Motcle&, Entree&);

  // Methodes inlines
  inline Probleme_base& get_problem() { return le_probleme_.valeur(); }
  inline const Champ_Don& pression_hydro() const { return pression_hydro_; }
  inline Champ_Don& pression_hydro() { return pression_hydro_; }
  inline const Champ_Don& pression_eos() const { return pression_eos_; }
  inline Champ_Don& pression_eos() { return pression_eos_; }
  inline const Champ_Don& fraction_massique_nonresolue() const { return unsolved_species_; }
  inline Champ_Don& fraction_massique_nonresolue() { return unsolved_species_; }
  inline const DoubleTab& pression_th_tab() { return Pth_tab_; } // Tab Pression thermodynamique
  inline const DoubleTab& pression_thn_tab() { return Pth_n_tab_; } // Tab Pression thermodynamique a l'etape precedente
  inline void secmembre_divU_Z(DoubleTab& tab_W) const { eos_tools_->secmembre_divU_Z(tab_W); }
  inline void set_pression_th_tab(DoubleTab& Pth_tab) { Pth_n_tab_ = Pth_tab_ = Pth_tab; }
  inline void set_resume_flag() { sim_resumed_ = 1; }
  inline bool use_total_pressure() { return use_total_pressure_; }
  inline bool use_hydrostatic_pressure() { return use_hydrostatic_pressure_; }
  inline bool use_pth_xyz() { return Pth_xyz_.non_nul(); }
  inline bool use_total_hydro_pressure() { return (use_total_pressure_||use_hydrostatic_pressure_); }
  inline bool use_saved_data() { return sim_resumed_; }

protected:
  Champ_Don Pth_xyz_,pression_hydro_,pression_eos_,unsolved_species_;
  DoubleTab Pth_tab_, Pth_n_tab_,P_NS_;
  int use_total_pressure_, use_hydrostatic_pressure_, sim_resumed_;
  double time_activate_ptot_;
  REF(Probleme_base) le_probleme_;

private:
  void calculer_pression_hydro();
  void initialiser_pth();
  void initialiser_pth_xyz();
  void initialiser_pth_for_EOS(const Probleme_base& pb);
  void remplir_champ_pression_tot(int n, const DoubleTab& PHydro, DoubleTab& PTot);
  void remplir_champ_pression_for_EOS();
};

#endif /* Fluide_Weakly_Compressible_included */
