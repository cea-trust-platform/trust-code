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
// File:        Fluide_Quasi_Compressible.h
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible
// Version:     /main/29
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Fluide_Quasi_Compressible_included
#define Fluide_Quasi_Compressible_included

#include <Fluide_Incompressible.h>
#include <Loi_Etat.h>
#include <EDO_Pression_th.h>
#include <Ref_Champ_Inc.h>
#include <Champ_Inc.h>

class Probleme_base;
class Zone_Cl_dis;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Fluide_Quasi_Compressible
//    Cette classe represente un d'un fluide quasi ccompressible
//    c'est a dire heritant de l'incompressible, mais faiblement compressible
// .SECTION voir aussi
//     Milieu_base Fluide_Incompressible
//////////////////////////////////////////////////////////////////////////////
class Fluide_Quasi_Compressible : public Fluide_Incompressible
{
  Declare_instanciable_sans_constructeur(Fluide_Quasi_Compressible);

public :

  Fluide_Quasi_Compressible();
  void set_param(Param& param);
  int lire_motcle_non_standard(const Motcle&, Entree&);
  void verifier_coherence_champs(int& err,Nom& message);
  void creer_champs_non_lus();
  void mettre_a_jour(double );
  int initialiser(const double& temps);
  virtual void discretiser(const Probleme_base& pb, const  Discretisation_base& dis);
  void completer(const Probleme_base&);
  inline const Nom type_fluide() const;
  void set_Cp(double);
  void preparer_calcul();
  void preparer_pas_temps();
  virtual void abortTimeStep();

  inline const Loi_Etat& loi_etat() const;
  //Ajout de la methode en non const pour pouvoir modifier cahmps_compris_
  //au cours de la discretisation
  inline Loi_Etat& loi_etat();

  inline void initialiser_inco_ch();
  inline double pression_th() const;
  inline double pression_thn() const;
  inline double pression_th1() const;
  inline void set_pression_th(double);
  inline const Champ_Inc& inco_chaleur() const;
  inline Champ_Inc& inco_chaleur();
  inline const Champ_Inc& vitesse() const;
  inline const Champ_Don& pression_tot() const;
  inline Champ_Don& pression_tot();
  const DoubleTab& temperature() const;
  Champ_Don& ch_temperature();
  const Champ_Don& ch_temperature() const;

  inline const Champ_Don& mu_sur_Schmidt() const;
  inline Champ_Don& mu_sur_Schmidt();

  void calculer_pression_tot();
  inline double calculer_H(double) const;
  //Methodes de l interface des champs postraitables
  //////////////////////////////////////////////////////
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const;
  /////////////////////////////////////////////////////



  inline void calculer_coeff_T();
  inline void calculer_Cp();
  inline void calculer_lambda();
  inline void calculer_mu();
  inline void calculer_nu();
  inline void calculer_alpha();
  inline void calculer_mu_sur_Sc();
  inline void Resoudre_EDO_PT();
  inline void calculer_masse_volumique();

  inline const DoubleTab& rho_discvit() const;
  inline const  DoubleTab& rho_n() const;
  inline const  DoubleTab& rho_np1() const;
  inline const  DoubleTab& rho_face_n() const;
  inline const  DoubleTab& rho_face_np1() const;
  inline void calculer_rho_face(const DoubleTab& );
  inline void divu_discvit(DoubleTab&, DoubleTab&);
  inline void secmembre_divU_Z(DoubleTab& ) const;
  inline double moyenne_vol(const DoubleTab&) const;
  inline double masse_totale(double P,const DoubleTab& T);
  inline int getTraitementPth() const
  {
    return traitement_PTh;
  }
  void checkTraitementPth(const Zone_Cl_dis&);
  inline int get_traitement_rho_gravite() const
  {
    return traitement_rho_gravite_;
  }
protected :
  Loi_Etat loi_etat_;
  EDO_Pression_th EDO_Pth_;
  int traitement_PTh; // flag pour le traitement de la pression thermo
  // 0 = resolution classique de l'edo
  // 1 = pression calculee pour conserver la masse
  // 2 = pression laissee cste.
  int traitement_rho_gravite_; // flag pour le traitement de rho pour la gravite
  // 0 std: gravite = rho*g
  //1 modifie: gravite =(rho-rho_moy)*g

  REF(Champ_Inc) inco_chaleur_;
  REF(Champ_Inc) vitesse_;
  REF(Champ_Inc) pression_;
  Champ_Don pression_tot_;
  double Pth_;  //Pression thermodynamique
  double Pth_n;  //Pression thermodynamique a l'etape precedente
  double Pth1; //Pression thermodynamique calculee pour conserver la masse
  double temps_debut_prise_en_compte_drho_dt_;
  Champ_Don mu_sur_Sc,rho_gaz,rho_comme_v,rho_cp;
  mutable DoubleTab tab_W_old_;
  double omega_drho_dt_; // Facteur de relaxation sur drho_dt


};

inline const Loi_Etat& Fluide_Quasi_Compressible::loi_etat() const
{
  return loi_etat_;
}

inline Loi_Etat& Fluide_Quasi_Compressible::loi_etat()
{
  return loi_etat_;
}

inline const Nom Fluide_Quasi_Compressible::type_fluide() const
{
  return loi_etat_->type_fluide();
}

inline void Fluide_Quasi_Compressible::initialiser_inco_ch()
{
  loi_etat_->initialiser_inco_ch();
}

inline double Fluide_Quasi_Compressible::pression_th() const
{
  return Pth_;
}
inline double Fluide_Quasi_Compressible::pression_thn() const
{
  return Pth_n;
}
inline double Fluide_Quasi_Compressible::pression_th1() const
{
  return Pth1;
}

inline void Fluide_Quasi_Compressible::set_pression_th(double Pth)
{
  Pth_n = Pth_ = Pth;
}

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
}
inline void Fluide_Quasi_Compressible::calculer_Cp()
{
  loi_etat_->calculer_Cp();
}
inline void Fluide_Quasi_Compressible::calculer_lambda()
{
  loi_etat_->calculer_lambda();
}
inline void Fluide_Quasi_Compressible::calculer_mu()
{
  loi_etat_->calculer_mu();
}
inline void Fluide_Quasi_Compressible::calculer_nu()
{
  loi_etat_->calculer_nu();
}
inline void Fluide_Quasi_Compressible::calculer_alpha()
{
  loi_etat_->calculer_alpha();
}

inline void Fluide_Quasi_Compressible::calculer_mu_sur_Sc()
{

  loi_etat_-> calculer_mu_sur_Sc();
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
inline void Fluide_Quasi_Compressible::calculer_masse_volumique()
{
  loi_etat_->calculer_masse_volumique();
}

inline const Champ_Inc& Fluide_Quasi_Compressible::inco_chaleur() const
{
  return inco_chaleur_.valeur();
}
inline Champ_Inc& Fluide_Quasi_Compressible::inco_chaleur()
{
  return inco_chaleur_.valeur();
}
inline const Champ_Inc& Fluide_Quasi_Compressible::vitesse() const
{
  return vitesse_.valeur();
}
inline const Champ_Don& Fluide_Quasi_Compressible::pression_tot() const
{
  return pression_tot_;
}
inline Champ_Don& Fluide_Quasi_Compressible::pression_tot()
{
  return pression_tot_;
}
inline const Champ_Don& Fluide_Quasi_Compressible::mu_sur_Schmidt() const
{
  return mu_sur_Sc;
}

inline Champ_Don& Fluide_Quasi_Compressible::mu_sur_Schmidt()
{
  return mu_sur_Sc;
}

//renvoie rho avec la meme discretisation que la vitesse
inline const DoubleTab& Fluide_Quasi_Compressible::rho_discvit() const
{
  return EDO_Pth_->rho_discvit();
}
inline const  DoubleTab& Fluide_Quasi_Compressible::rho_n() const
{
  return loi_etat_->rho_n();
}
inline const  DoubleTab& Fluide_Quasi_Compressible::rho_np1() const
{
  return loi_etat_->rho_np1();
}
inline const  DoubleTab& Fluide_Quasi_Compressible::rho_face_n() const
{
  return EDO_Pth_->rho_face_n();
}
inline const  DoubleTab& Fluide_Quasi_Compressible::rho_face_np1() const
{
  return EDO_Pth_->rho_face_np1();
}
inline void Fluide_Quasi_Compressible::calculer_rho_face(const DoubleTab& tab_rho)
{
  EDO_Pth_->calculer_rho_face_np1(tab_rho);
}
//Calcule div(u) avec la meme discretisation que la vitesse
inline void Fluide_Quasi_Compressible::divu_discvit(DoubleTab& secmem1, DoubleTab& secmem2)
{
  EDO_Pth_->divu_discvit(secmem1,secmem2);
}
inline double Fluide_Quasi_Compressible::moyenne_vol(const DoubleTab& A) const
{
  return EDO_Pth_->moyenne_vol(A);
}

//Calcule la pression en conservant la masse
inline double Fluide_Quasi_Compressible::masse_totale(double P,const DoubleTab& T)
{
  return EDO_Pth_->masse_totale( P, T);
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

inline double Fluide_Quasi_Compressible::calculer_H(double h)  const
{
  return loi_etat_->calculer_H(Pth_,h);
}
#endif
