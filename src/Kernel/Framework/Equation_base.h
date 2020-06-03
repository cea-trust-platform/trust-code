/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Equation_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/49
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Equation_base_included
#define Equation_base_included

#include <Ref_Probleme_base.h>
#include <Ref_Zone_dis.h>
#include <Zone_Cl_dis.h>
#include <Ref_Schema_Temps_base.h>
#include <Solveur_Masse.h>
#include <Sources.h>
#include <Parametre_equation.h>
#include <Champs_Fonc.h>
#include <DoubleList.h>
#include <Ref_Objet_U.h>
#include <Liste_Noms.h>
#include <Parser_U.h>
#include <Matrice_Morse.h>
#include <vector>
#include <map>
#include <MD_Vector_tools.h>

class Operateur;
class DoubleTab;
class DoubleTrav;
class Discretisation_base;
class Motcle;
class Milieu_base;
class Champ_Inc;
class Param;

enum Type_modele { TURBULENCE };
//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Equation_base
//     Le role d'une equation est le calcul d'un ou plusieurs champs.
//     Cette classe est la base de la hierarchie des equations.
//     Ses membres sont les attributs et les methodes communs
//     a toutes les classes qui representent des equations.
//     Une equation est modelisee de la facon suivante:
//
//         M * dU_h/dt + Somme_i(Op_i(U_h)) = Somme(Sources);
//
//     M est la matrice masse representee par un objet "Solveur_Masse"
//     U_h est l'inconnue representee par un objet "Champ_Inc"
//     Op_i est le i-eme operateur de l'equation represente par un objet
//          "Operateur"
//     Sources sont les termes sources (eventuellement inexistant) de
//             l'equation represente par des objets "Source".
//     Une equation est lie a un probleme par une reference contenue
//     dans le membre REF(Probleme_base) mon_probleme.
//
// .SECTION voir aussi
//     Equation
//     Classe abstraite dont toutes les equations doivent deriver.
//     Methodes abstraites:
//       int nombre_d_operateurs() const
//       const Operateur& operateur(int) const
//       Operateur& operateur(int)
//       const Champ_Inc& inconnue() const
//       Champ_Inc& inconnue()
//       void associer_milieu_base(const Milieu_base&)
//       const Milieu_base& milieu() const
//       Milieu_base& milieu()
//       Entree& lire(const Motcle&, Entree&) [protegee]
//////////////////////////////////////////////////////////////////////////////

Declare_liste(RefObjU);

class Equation_base : public Champs_compris_interface, public Objet_U
{
  Declare_base(Equation_base);

public :
  // Methode surchargee de Objet_U:
  void nommer(const Nom& nom);
  // MODIF ELI LAUCOIN (22/11/2007) : je rajoute un avancer et un reculer
  virtual void avancer(int i=1);
  virtual void reculer(int i=1);
  // FIN MODIF ELI LAUCOIN (22/11/2007)

  virtual int nombre_d_operateurs() const =0;
  virtual int nombre_d_operateurs_tot() const;
  virtual const Operateur& operateur(int) const =0;
  virtual Operateur& operateur(int) =0;
  virtual const Operateur& operateur_fonctionnel(int) const;
  virtual Operateur& operateur_fonctionnel(int);
  virtual const Champ_Inc& inconnue() const =0;
  virtual Champ_Inc& inconnue() =0;
  virtual void associer_milieu_base(const Milieu_base&)=0;
  virtual const Milieu_base& milieu() const =0;
  virtual Milieu_base& milieu() =0;
  virtual int sauvegarder(Sortie&) const;
  virtual int reprendre(Entree&);
  int limpr() const;
  virtual void imprimer(Sortie& os) const;
  virtual int impr(Sortie& os) const;

  virtual DoubleTab& derivee_en_temps_inco(DoubleTab& );
  virtual DoubleTab& derivee_en_temps_inco_transport(DoubleTab& derivee)
  {
    return derivee_en_temps_inco(derivee);
  };
  virtual DoubleTab& corriger_derivee_expl(DoubleTab& );
  virtual DoubleTab& corriger_derivee_impl(DoubleTab& );
  virtual void mettre_a_jour(double temps);
  virtual void abortTimeStep();
  virtual void valider_iteration();
  virtual int preparer_calcul();
  virtual bool initTimeStep(double dt);
  virtual bool updateGivenFields();
  virtual void discretiser();
  virtual void associer_pb_base(const Probleme_base&);
  virtual void completer();
  virtual double calculer_pas_de_temps() const;
  void calculer_pas_de_temps_locaux(DoubleTab&) const;  //Computation of local time: Vect of size number of faces of the domain
  Sources& sources();
  const Sources& sources() const;
  inline Solveur_Masse& solv_masse();
  inline const Solveur_Masse& solv_masse() const;
  Probleme_base& probleme();
  const Probleme_base& probleme() const;
  Schema_Temps_base& schema_temps();
  const Schema_Temps_base& schema_temps() const;
  virtual void associer_sch_tps_base(const Schema_Temps_base&);
  virtual void associer_zone_dis(const Zone_dis&);

  const Discretisation_base& discretisation() const;

  virtual inline Zone_Cl_dis& zone_Cl_dis();
  virtual inline const Zone_Cl_dis& zone_Cl_dis() const;
  Zone_dis& zone_dis();
  const Zone_dis& zone_dis() const;
  //
  inline const Nom& le_nom() const;
  inline DoubleVect& get_residu()
  {
    return residu_;
  };
  inline DoubleVect& residu_initial()
  {
    return residu_initial_;
  };
  void initialise_residu(int=0);
  virtual void imprime_residu(SFichier&);
  virtual Nom expression_residu();

  // methodes pour l'implicite
  virtual void dimensionner_matrice(Matrice_Morse& mat_morse);
  // ajoute les contributions des operateurs et des sources
  virtual void assembler( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem) ;
  // modifie la matrice et le second mmebre en fonction des CL
  virtual void modifier_pour_Cl( Matrice_Morse& mat_morse,DoubleTab& secmem) const;
  // assemble, ajoute linertie,et modifie_pour_cl.
  virtual void assembler_avec_inertie( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem) ;
  virtual void get_items_croises(const Probleme_base& autre_pb, extra_item_t& extra_items);
  virtual void dimensionner_termes_croises(Matrice_Morse& matrice, const Probleme_base& autre_pb, const extra_item_t& extra_items, int nl, int nc);
  virtual void ajouter_termes_croises(const DoubleTab& inco, const Probleme_base& autre_pb, const DoubleTab& autre_inco, DoubleTab& resu) const;
  virtual void contribuer_termes_croises(const DoubleTab& inco, const Probleme_base& autre_pb, const DoubleTab& autre_inco, Matrice_Morse& matrice) const;

  //Methodes de l interface des champs postraitables
  /////////////////////////////////////////////////////
  virtual void creer_champ(const Motcle& motlu);
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const;
  /////////////////////////////////////////////////////

  virtual const Motcle& domaine_application() const;
  void verifie_ch_init_nb_comp(const Champ_Inc_base& ch_ref, const int& nb_comp) const;
  DoubleTab& derivee_en_temps_conv(DoubleTab& , const DoubleTab& );
  void Gradient_conjugue_diff_impl(DoubleTrav& secmem, DoubleTab& solution)
  {
    return Gradient_conjugue_diff_impl(secmem,solution,0,NULL_);
  };
  void Gradient_conjugue_diff_impl(DoubleTrav& secmem, DoubleTab& solution, const DoubleTab& terme_mul)
  {
    return Gradient_conjugue_diff_impl(secmem,solution,terme_mul.dimension_tot(0),terme_mul);
  };
  inline Parametre_equation& parametre_equation()
  {
    return parametre_equation_ ;
  };
  inline const Parametre_equation& parametre_equation() const
  {
    return parametre_equation_ ;
  };
  virtual const REF(Objet_U)& get_modele(Type_modele type) const;
  int equation_non_resolue() const;

  //pour les schemas en temps a pas multiples
  inline virtual const Champ_Inc& derivee_en_temps() const
  {
    return derivee_en_temps_;
  };
  inline virtual Champ_Inc& derivee_en_temps()
  {
    return derivee_en_temps_;
  }
  void set_calculate_time_derivative(int i)
  {
    calculate_time_derivative_=i;
  };
  int calculate_time_derivative() const
  {
    return calculate_time_derivative_;
  };

  inline const DoubleTab get_residuals() const
  {
    return residuals_;
  };

  inline void set_residuals(DoubleTab r)
  {
    residuals_ = r;
  };

protected :

  Nom nom_;
  Solveur_Masse solveur_masse;
  Sources les_sources;
  REF(Schema_Temps_base) le_schema_en_temps;
  REF(Zone_dis) la_zone_dis;
  Zone_Cl_dis la_zone_Cl_dis;
  REF(Probleme_base) mon_probleme;
  virtual void set_param(Param& titi);
  virtual int lire_motcle_non_standard(const Motcle&, Entree&);
  virtual Entree& lire_sources(Entree&);
  virtual Entree& lire_cond_init(Entree&);
  virtual Entree& lire_cl(Entree&);
  virtual int verif_Cl() const;
  mutable DoubleList dt_op_bak;
  //Methode lire avec signature specifique pour faire echouer
  //la compilation en cas de presence de l'ancienne methode lire
  //virtual Entree& lire(const Motcle&, Entree&)
  virtual void lire()
  {
    exit();
  };

  int sys_invariant_;
  int implicite_;
  Parametre_equation parametre_equation_;
  Champ_Fonc volume_maille;

  LIST(RefObjU) liste_modeles_; //Le premier element de la liste est le modele nul
  Champs_compris champs_compris_;
  Champs_Fonc list_champ_combi;

private :
  void Gradient_conjugue_diff_impl(DoubleTrav& secmem, DoubleTab& solution, int size_terme_mul, const DoubleTab& term_mul);

  void ecrire_fichier_xyz() const;
  ArrOfDouble dt_ecrire_fic_xyz;
  Motcles nom_champ_xyz;
  Liste_Noms noms_bord_xyz;
  IntVect nb_bords_post_xyz;
  int nombre_champ_xyz;
  int ecrit_champ_xyz_bin;

  //!SC: passage en protected (surcharge de get_champ dans Equation_Diphasique_base)
//  Champs_Fonc list_champ_combi;
  DoubleVect residu_;
  DoubleVect residu_initial_;
  // retourne le CHAMP (et non la norme) des residus de chaque inconnu du probleme
  DoubleTab residuals_;

  mutable DoubleTab NULL_;
  mutable Parser_U equation_non_resolue_;

  // For multistep methods, store previous dI/dt(n), dI/dt(n-1),...
  Champ_Inc derivee_en_temps_;
  int calculate_time_derivative_;

  //memoization of the matrix for PolyMAC
  mutable Matrice_Morse matrice_stockee;
  mutable int matrice_init;
};


// Description:
//    Renvoie le nom de l'equation.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: le nom de l'equation
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Nom& Equation_base::le_nom() const
{
  return nom_;
}


// Description:
//    Renvoie la zone des conditions aux limite discretisee
//    associee a l'equation
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Zone_Cl_dis&
//    Signification: Zone de condition aux limites discretisee
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Zone_Cl_dis& Equation_base::zone_Cl_dis()
{
  return la_zone_Cl_dis;
}

// Description:
//    Renvoie la zone des conditions aux limite discretisee
//    associee a l'equation
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Zone_Cl_dis&
//    Signification: Zone de condition aux limites discretisee
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Zone_Cl_dis& Equation_base::zone_Cl_dis() const
{
  return la_zone_Cl_dis;
}


// Description:
//    Renvoie le solveur de masse associe a l'equation.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Solveur_Masse&
//    Signification: le solveur de masse associe a l'equation
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Solveur_Masse& Equation_base::solv_masse()
{
  return solveur_masse;
}


// Description:
//    Renvoie le solveur de masse associe a l'equation.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Solveur_Masse&
//    Signification: le solveur de masse associe a l'equation
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Solveur_Masse& Equation_base::solv_masse() const
{
  return solveur_masse;
}

#endif

