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

#ifndef Turbulence_paroi_base_included
#define Turbulence_paroi_base_included


#include <Champ_Inc.h>
#include <Champ_Fonc.h>
#include <Ref_Mod_turb_hyd_base.h>
#include <Champs_compris.h>
#include <Champs_compris_interface.h>
#include <List_Nom.h>

class Mod_turb_hyd;
class Zone_dis;
class Zone_Cl_dis;
class Probleme_base;
class EcrFicPartage;
class Param;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Turbulence_paroi_base
//    Classe de base pour la hierarchie des classes representant les modeles
//    de calcul des grandeurs turbulentes aux voisinages des parois.
//    Contient une reference a un modele de turbulence.
// .SECTION voir aussi
//    Paroi_std_hyd_VDF Paroi_std_scal_hyd_VDF
//    Classe abstraite
//    Methodes abstraites
//      void associer(const Zone_dis&, const Zone_Cl_dis&)
//      int init_lois_paroi()
//      int calculer_hyd(DoubleTab& )
//      int calculer_hyd(DoubleTab& , DoubleTab& )
//////////////////////////////////////////////////////////////////////////////
class Turbulence_paroi_base : public Champs_compris_interface, public Objet_U
{

  Declare_base_sans_constructeur(Turbulence_paroi_base);

public :

  Turbulence_paroi_base();
  virtual void set_param(Param& param);
  inline void associer_modele(const Mod_turb_hyd_base& );
  virtual void associer(const Zone_dis&, const Zone_Cl_dis&)=0;
  virtual void completer() {};
  virtual int init_lois_paroi() =0;
  inline int calculer_hyd(Champ_Inc& );
  inline int calculer_hyd(Champ_Fonc& , Champ_Fonc& );
  virtual int calculer_hyd(DoubleTab& ) =0;
  virtual int calculer_hyd(DoubleTab& , DoubleTab& ) =0;
  inline virtual DoubleTab& corriger_derivee_impl(DoubleTab& d) const
  {
    return d;
  };
  inline virtual void imprimer_ustar(Sortie& ) const ;
  inline virtual void imprimer_ustar_mean_only(Sortie&, int, const LIST(Nom)&, const Nom&) const ;
  inline virtual void imprimer_premiere_ligne_ustar(int, const LIST(Nom)&, const Nom&) const;
  // rajout pour prendre en compte Cisaillement_paroi dans la classe
  // de base

  inline const DoubleTab& Cisaillement_paroi() const;
  inline const DoubleVect& tab_u_star() const;
  inline double tab_u_star(int face) const;
  inline const DoubleVect& tab_d_plus() const;
  inline double tab_d_plus(int face) const;

  //OC 01/2006: ajout de la fonctionnalite sauvegarde/reprise : utile pour TBLE pour l'instant.
  int sauvegarder(Sortie&) const override
  {
    return 0;
  };
  int reprendre(Entree& ) override
  {
    return 0;
  };

  //Methodes de l interface des champs postraitables
  //////////////////////////////////////////////////////
  void creer_champ(const Motcle& motlu) override;
  const Champ_base& get_champ(const Motcle& nom) const override;
  void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const override;
  /////////////////////////////////////////////////////
  // Ecriture dans un fichier separe de u_star, Cisaillement_paroi etc...
  void ouvrir_fichier_partage(EcrFicPartage&,const Nom&) const;
  void ouvrir_fichier_partage(EcrFicPartage&, const Nom&, const Nom&) const;
  // indique si on utilise le cisaillement ou non
  virtual bool use_shear() const; // Generalement true sauf par exemple pour loi paroi_negligeable_XXX

protected:

  REF(Mod_turb_hyd_base) mon_modele_turb_hyd;
  DoubleTab Cisaillement_paroi_;         //valeurs des contraintes tangentielles aux
  // parois calculees localement a partir de u*
  DoubleVect tab_u_star_;                // valeurs des u* calculees localement
  DoubleVect tab_d_plus_;                // valeurs des d+ calculees localement
  mutable Champ_Fonc champ_u_star_;                                // Champ pour postraitement
  mutable int nb_impr_;                        // Compteur d'impression
  mutable int nb_impr0_;                        // Compteur d'impression

protected:

  Champs_compris champs_compris_;
};


inline void Turbulence_paroi_base::imprimer_ustar(Sortie& ) const
{
}

inline void Turbulence_paroi_base::imprimer_ustar_mean_only(Sortie&, int, const LIST(Nom)&, const Nom&) const
{
}

inline void Turbulence_paroi_base::imprimer_premiere_ligne_ustar(int, const LIST(Nom)&, const Nom& ) const
{
}
// Description:
//    Associe un modele de turbulence a l'objet.
// Precondition:
// Parametre: Mod_turb_hyd_base& le_modele
//    Signification: le modele de turbulence hydraulique
//                   a associer a l'objet
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: l'objet a un modele de turbulence associe
inline void Turbulence_paroi_base::associer_modele(const Mod_turb_hyd_base& le_modele)
{
  mon_modele_turb_hyd = le_modele;
}


// Description:
//    Simple appel a int calculer_hyd(DoubleTab& ).
// Precondition:
// Parametre: Champ_Inc& ch
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: code de retour propage
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int Turbulence_paroi_base::calculer_hyd(Champ_Inc& ch)
{
  return calculer_hyd(ch.valeurs());
}


// Description:
//    Simple appel a int calculer_hyd(DoubleTab&, DoubleTab&).
// Precondition:
// Parametre: Champ_Inc& ch1
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Parametre: Champ_Inc& ch2
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: code de retour propage
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int Turbulence_paroi_base::calculer_hyd(Champ_Fonc& ch1,Champ_Fonc& ch2)
{
  return calculer_hyd(ch1.valeurs(),ch2.valeurs());
}

inline const DoubleTab& Turbulence_paroi_base::Cisaillement_paroi() const
{
  return Cisaillement_paroi_;
}

inline const DoubleVect& Turbulence_paroi_base::tab_u_star() const
{
  return tab_u_star_;
}

inline double Turbulence_paroi_base::tab_u_star(int face) const
{
  return tab_u_star_(face);
}

inline const DoubleVect& Turbulence_paroi_base::tab_d_plus() const
{
  return tab_d_plus_;
}

inline double Turbulence_paroi_base::tab_d_plus(int face) const
{
  return tab_d_plus_(face);
}

inline void erreur_non_convergence()
{
  Cerr << "TRUST stopped cause in the function calculer_u_plus of wall law." << finl;
  Cerr << "The iterative process of u* did not converge" << finl;
  if (Process::nproc()>1) Cerr << " on the processor" << Process::me();
  else Cerr << ".";
  Cerr << finl << "The hydraulic calculation is may be diverging." << finl;
  Process::exit();
}
#endif
