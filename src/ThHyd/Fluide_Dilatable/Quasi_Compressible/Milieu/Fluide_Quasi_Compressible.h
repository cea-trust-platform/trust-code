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
// File:        Fluide_Quasi_Compressible.h
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Quasi_Compressible/Milieu
// Version:     /main/29
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Fluide_Quasi_Compressible_included
#define Fluide_Quasi_Compressible_included

#include <Fluide_Dilatable.h>

class Zone_Cl_dis;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Fluide_Quasi_Compressible
//    Cette classe represente un d'un fluide quasi compressible
//    c'est a dire heritant de fluide dilatable
// .SECTION voir aussi
//     Milieu_base Fluide_Dilatable
//
//////////////////////////////////////////////////////////////////////////////

class Fluide_Quasi_Compressible : public Fluide_Dilatable
{
  Declare_instanciable_sans_constructeur(Fluide_Quasi_Compressible);

public :

  Fluide_Quasi_Compressible();

  virtual void set_param(Param& param);
  virtual void completer(const Probleme_base&);
  virtual void preparer_pas_temps();
  virtual void prepare_pressure_edo();
  virtual void write_mean_edo(double);
  virtual void checkTraitementPth(const Zone_Cl_dis& zone_cl);
  virtual void discretiser(const Probleme_base& pb, const  Discretisation_base& dis);
  virtual int lire_motcle_non_standard(const Motcle&, Entree&);

  // Methodes inlines
  inline const DoubleTab& rho_discvit() const  { return EDO_Pth_->rho_discvit();  }
  inline const DoubleTab& rho_face_n() const { return EDO_Pth_->rho_face_n(); }
  inline const DoubleTab& rho_face_np1() const  { return EDO_Pth_->rho_face_np1(); }
  inline void Resoudre_EDO_PT();
  inline void secmembre_divU_Z(DoubleTab& ) const;
  inline void calculer_rho_face(const DoubleTab& tab_rho) { EDO_Pth_->calculer_rho_face_np1(tab_rho); }
  inline void divu_discvit(DoubleTab& secmem1, DoubleTab& secmem2) { EDO_Pth_->divu_discvit(secmem1,secmem2); }
  inline int get_traitement_rho_gravite() const { return traitement_rho_gravite_; }
  inline double moyenne_vol(const DoubleTab& A) const { return EDO_Pth_->moyenne_vol(A); }
  inline double masse_totale(double P,const DoubleTab& T) { return EDO_Pth_->masse_totale( P, T); }

protected :
  int traitement_rho_gravite_; // 0 : gravite = rho*g, 1 : gravite =(rho-rho_moy)*g
  double temps_debut_prise_en_compte_drho_dt_,omega_drho_dt_;
  EDO_Pression_th EDO_Pth_;
  Nom output_file_;
  mutable DoubleTab tab_W_old_;
};

inline void Fluide_Quasi_Compressible::Resoudre_EDO_PT()
{
  Pth_n = Pth_;
  Pth_ = EDO_Pth_->resoudre(Pth_);

  if (Pth_<=0)
    {
      Cerr<<"Error : the pressure calculated by Resoudre_EDO_PT method is negative : "<< Pth_ << finl;
      abort();
    }

  EDO_Pth_->mettre_a_jour_CL(Pth_);
}

//Calcule W=-dZ/dt, 2nd membre de l'equation div(rhoU) = W
inline void Fluide_Quasi_Compressible::secmembre_divU_Z(DoubleTab& tab_W) const
{
  double temps=vitesse_->temps();
  if (temps>temps_debut_prise_en_compte_drho_dt_)
    {
      EDO_Pth_->secmembre_divU_Z(tab_W);
      // Relaxation eventuelle:
      // (BM: j'ai remplace le test size() != 0 car invalide sur champ P1bulle)
      if (tab_W_old_.size_totale() > 0)
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
