/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Mod_turb_hyd_base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Modele_turbulence_base
// Version:     /main/40
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Mod_turb_hyd_base_included
#define Mod_turb_hyd_base_included


#define CMU 0.09

#include <Turbulence_paroi.h>
// generalisation du modele.
#include <Support_Champ_Masse_Volumique.h>
class Motcle;
class Zone_dis;
class Zone_Cl_dis;
class Equation_base;
class Schema_Temps_base;
class Param;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Mod_turb_hyd_base
//    Cette classe sert de base a la hierarchie des classes
//    qui representent un modele de turbulence pour les
//    equations de Navier-Stokes. Il existe deja deux classes derivees
//    qui representent le modele de turbulence (k,e) et le modele
//    de turbulence sous maille. Ces deux modeles ont en commun
//    le calcul d'une viscosite turbulente.
//
//    B.Mat. : la classe herite de support_champ_masse_volumique.
//     en Front-Tracking (et plus tard en qc), elle fournit le
//     mecanisme pour recuperer rho.
// .SECTION voir aussi
//    Mod_turb_hyd_ss_maille Modele_turbulence_hyd_K_Eps
//    Classe abstraite
//    Methode abstraite
//      void mettre_a_jour(double )
//      Entree& lire(const Motcle&, Entree&)
//////////////////////////////////////////////////////////////////////////////
class Mod_turb_hyd_base : public Objet_U,
  public Support_Champ_Masse_Volumique,
  public Champs_compris_interface
{

  Declare_base_sans_constructeur(Mod_turb_hyd_base);

public:
  inline Mod_turb_hyd_base();
  inline const Champ_Fonc& viscosite_turbulente() const;
  inline Equation_base& equation();
  inline const Equation_base& equation() const;
  inline const Turbulence_paroi& loi_paroi() const
  {
    return loipar;
  };
  inline       Turbulence_paroi& loi_paroi()
  {
    return loipar;
  };
  virtual void set_param(Param& param);
  virtual int lire_motcle_non_standard(const Motcle&, Entree&);
  virtual int preparer_calcul();
  virtual bool initTimeStep(double dt);
  virtual void mettre_a_jour(double ) =0;
  virtual void discretiser();
  void discretiser_visc_turb(const Schema_Temps_base&, Zone_dis&, Champ_Fonc&) const;
  void discretiser_corr_visc_turb(const Schema_Temps_base&, Zone_dis&, Champ_Fonc&) const;
  void discretiser_K(const Schema_Temps_base&, Zone_dis&, Champ_Fonc&) const;
  virtual void completer();
  void associer_eqn(const Equation_base& );
  virtual void associer(const Zone_dis& , const Zone_Cl_dis& );
  virtual int reprendre(Entree& );

  //Methodes de l interface des champs postraitables
  /////////////////////////////////////////////////////
  virtual void creer_champ(const Motcle& motlu);
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const;
  /////////////////////////////////////////////////////

  inline Champs_compris& champs_compris();

  virtual void imprimer(Sortie&) const;
  void a_faire(Sortie&) const;
  virtual int sauvegarder(Sortie&) const;

  int limpr_ustar(double , double, double, double ) const;
  inline double get_Cmu() const;

protected:

  double LeCmu;
  Champ_Fonc la_viscosite_turbulente;
  REF(Equation_base) mon_equation;
  Turbulence_paroi loipar;
  double dt_impr_ustar;
  double dt_impr_ustar_mean_only;
  int boundaries_;
  LIST(Nom) boundaries_list ;
  Nom nom_fichier_;
  void limiter_viscosite_turbulente();

  Champs_compris champs_compris_;
private :


  double XNUTM;
  int calcul_borne_locale_visco_turb_;
  double dt_diff_sur_dt_conv_;
  Champ_Fonc corr_visco_turb;
  DoubleVect borne_visco_turb;
};

inline Mod_turb_hyd_base::Mod_turb_hyd_base()
{
  LeCmu = CMU ;
  dt_impr_ustar=1.e20;
  dt_impr_ustar_mean_only=1.e20;
  boundaries_=0;
  nom_fichier_="";
  XNUTM = 1.E8 ;
  calcul_borne_locale_visco_turb_ = 0;
  dt_diff_sur_dt_conv_ = -1;
}
// Description:
//    Renvoie la viscosite turbulente.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Fonc&
//    Signification: le champ representant la viscosite turbulente
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Champ_Fonc& Mod_turb_hyd_base::viscosite_turbulente() const
{
  return la_viscosite_turbulente;
}


// Description:
//    Renvoie l'equation associee au modele de turbulence.
//    (c'est une equation du type Equation_base)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Equation_base&
//    Signification: l'equation associee au modele de turbulence
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline Equation_base& Mod_turb_hyd_base::equation()
{
  if (mon_equation.non_nul()==0)
    {
      Cerr << "\nError in Mod_turb_hyd_base::equation() : The equation is unknown !" << finl;
      Process::exit();
    }
  return mon_equation.valeur();
}

inline const Equation_base& Mod_turb_hyd_base::equation() const
{
  if (mon_equation.non_nul()==0)
    {
      Cerr << "\nError in Mod_turb_hyd_base::equation() : The equation is unknown !" << finl;
      Process::exit();
    }
  return mon_equation.valeur();
}

// Description:
//    Renvoie de la valeur de Cmu
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: la valeur Cmu
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline double Mod_turb_hyd_base::get_Cmu() const
{
  return LeCmu;
}

inline Champs_compris&  Mod_turb_hyd_base::champs_compris()
{
  return champs_compris_;
}
#endif

