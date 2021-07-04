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
#include <Loi_Etat.h>
#include <EDO_Pression_th.h>

class Probleme_base;
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
  void set_param(Param& param);
  int lire_motcle_non_standard(const Motcle&, Entree&);
  void creer_champs_non_lus();
  void mettre_a_jour(double );
  int initialiser(const double& temps);
  virtual void discretiser(const Probleme_base& pb, const  Discretisation_base& dis);
  void completer(const Probleme_base&);
  void preparer_calcul();
  void preparer_pas_temps();
  virtual void abortTimeStep();
  const DoubleTab& temperature() const;
  Champ_Don& ch_temperature();
  const Champ_Don& ch_temperature() const;
  void calculer_pression_tot();
  virtual void checkTraitementPth(const Zone_Cl_dis&);

  //Methodes de l interface des champs postraitables
  //////////////////////////////////////////////////////
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const;
  /////////////////////////////////////////////////////

  inline const Loi_Etat& loi_etat() const { return loi_etat_; }
  inline Loi_Etat&  loi_etat() { return loi_etat_; }
  inline const Nom type_fluide() const { return loi_etat_->type_fluide(); }
  inline void initialiser_inco_ch() { loi_etat_->initialiser_inco_ch();  }
  inline void calculer_Cp() { loi_etat_->calculer_Cp(); }
  inline void calculer_lambda() { loi_etat_->calculer_lambda(); }
  inline void calculer_mu() { loi_etat_->calculer_mu(); }
  inline void calculer_nu() { loi_etat_->calculer_nu(); }
  inline void calculer_alpha() { loi_etat_->calculer_alpha(); }
  inline void calculer_mu_sur_Sc() { loi_etat_-> calculer_mu_sur_Sc(); }
  inline void calculer_nu_sur_Sc() { loi_etat_-> calculer_nu_sur_Sc(); }
  inline void calculer_masse_volumique() { loi_etat_->calculer_masse_volumique(); }
  //renvoie rho avec la meme discretisation que la vitesse
  inline const DoubleTab& rho_discvit() const  { return EDO_Pth_->rho_discvit();  }
  inline const  DoubleTab& rho_n() const { return loi_etat_->rho_n(); }
  inline const  DoubleTab& rho_np1() const { return loi_etat_->rho_np1(); }
  inline const  DoubleTab& rho_face_n() const { return EDO_Pth_->rho_face_n(); }
  inline const  DoubleTab& rho_face_np1() const  { return EDO_Pth_->rho_face_np1(); }
  inline void calculer_rho_face(const DoubleTab& tab_rho) { EDO_Pth_->calculer_rho_face_np1(tab_rho); }
  //Calcule div(u) avec la meme discretisation que la vitesse
  inline void divu_discvit(DoubleTab& secmem1, DoubleTab& secmem2) { EDO_Pth_->divu_discvit(secmem1,secmem2); }
  inline double moyenne_vol(const DoubleTab& A) const { return EDO_Pth_->moyenne_vol(A); }
  //Calcule la pression en conservant la masse
  inline double masse_totale(double P,const DoubleTab& T) { return EDO_Pth_->masse_totale( P, T); }
  inline double calculer_H(double hh) const { return loi_etat_->calculer_H(Pth_,hh); }
  inline int getTraitementPth() const { return traitement_PTh; }
  inline int get_traitement_rho_gravite() const { return traitement_rho_gravite_; }
  inline void calculer_coeff_T();
  inline void Resoudre_EDO_PT();
  inline void secmembre_divU_Z(DoubleTab& ) const;

protected :
  Loi_Etat loi_etat_;
  EDO_Pression_th EDO_Pth_;
  int traitement_rho_gravite_; // flag pour le traitement de rho pour la gravite
  // 0 std: gravite = rho*g
  //1 modifie: gravite =(rho-rho_moy)*g

  double temps_debut_prise_en_compte_drho_dt_;
  double omega_drho_dt_; // Facteur de relaxation sur drho_dt
  Nom output_file_; // Fichier evolution (anciennement "evol_glob")

};

inline void Fluide_Quasi_Compressible::calculer_coeff_T()
{
  loi_etat_->remplir_T();
  calculer_Cp();
  calculer_mu();
  calculer_lambda();
  //calculer_mu();
  calculer_nu();
  calculer_alpha();
  calculer_mu_sur_Sc();
  calculer_nu_sur_Sc();
}

inline void Fluide_Quasi_Compressible::Resoudre_EDO_PT()
{
  //Corrections pour utiliser la pression qui permet de conserver la masse
  //Pth1 = EDO_Pth_->masse_totale(double P,const DoubleTab& T);
  //Cerr<<"Pth_n,Pth_,Pth1="<<Pth_n<<","<<Pth_<<","<<Pth1<<finl;
  Pth_n = Pth_;
  //Corrections pour utiliser la pression qui permet de conserver la masse
  //Pth_n=Pth1;
  Pth_ = EDO_Pth_->resoudre(Pth_);
  //Corrections pour utiliser la pression qui permet de conserver la masse
  //Pth_ = EDO_Pth_->resoudre(Pth_n);
  //Cerr<<"Pth_n,Pth_,Pth1="<<Pth_n<<","<<Pth_<<","<<Pth1<<finl;
  //Corrections pour calculer la pression
  //Pth1 = EDO_Pth_->resoudre(Pth_);
  if (Pth_<=0)
    {
      Cerr<<"Error : the pressure calculated by Resoudre_EDO_PT method is negative : "<<Pth_<<finl;
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
