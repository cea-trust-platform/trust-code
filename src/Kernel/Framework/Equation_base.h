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

#ifndef Equation_base_included
#define Equation_base_included

#include <Ref_Schema_Temps_base.h>
#include <Parametre_equation.h>
#include <Ref_Probleme_base.h>
#include <MD_Vector_tools.h>
#include <Interface_blocs.h>
#include <Solveur_Masse.h>
#include <Matrice_Morse.h>
#include <Ref_Zone_dis.h>
#include <Champs_Fonc.h>
#include <Ref_Objet_U.h>
#include <Zone_Cl_dis.h>
#include <List_Noms.h>
#include <TRUSTList.h>
#include <TRUSTTrav.h>
#include <TRUSTTab_parts.h>
#include <Parser_U.h>
#include <Sources.h>
#include <vector>

class Discretisation_base;
class Milieu_base;
class Operateur;
class Champ_Inc;
class Motcle;
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
  void nommer(const Nom& nom) override;
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
  int sauvegarder(Sortie&) const override;
  int reprendre(Entree&) override;
  int limpr() const;
  virtual void imprimer(Sortie& os) const;
  virtual int impr(Sortie& os) const;
  virtual void associer_milieu_equation();

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
  inline const Nom& le_nom() const override;
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
  virtual void dimensionner_matrice(Matrice_Morse& mat_morse); //memorise le stencil de la matrice apres le 1er appel
  virtual void dimensionner_matrice_sans_mem(Matrice_Morse& mat_morse); //methode interne appellee par celle ci-dessus

  // ajoute les contributions des operateurs et des sources
  virtual void assembler( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem) ;
  // modifie la matrice et le second mmebre en fonction des CL
  virtual void modifier_pour_Cl( Matrice_Morse& mat_morse,DoubleTab& secmem) const;
  // assemble, ajoute linertie,et modifie_pour_cl.
  virtual void assembler_avec_inertie( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem) ;
  virtual void dimensionner_termes_croises(Matrice_Morse& matrice, const Probleme_base& autre_pb, int nl, int nc);
  virtual void ajouter_termes_croises(const DoubleTab& inco, const Probleme_base& autre_pb, const DoubleTab& autre_inco, DoubleTab& resu) const;
  virtual void contribuer_termes_croises(const DoubleTab& inco, const Probleme_base& autre_pb, const DoubleTab& autre_inco, Matrice_Morse& matrice) const;

  /*
    interface {dimensionner/ajouter/assembler}_blocs
    specificites : - has_interface_blocs() renvoie 1 si tous les termes de l'equation supportent cette interface
                   - dimensionner_blocs() non memoize (a gerer par l'appelant) / appelable sur des matrices non vides
                   - assembler_blocs() utilise les valeurs des inconnues/champs a l'instant present (genre inconnue().valeurs())
                   - assembler_blocs_*() raisonne en increments : M.dInco = S -> attention aux seuils des solveurs
                   - certaines variables (ensemble semi_impl) peuvent etre traitees en "semi-implicite"
                     (on utilise des valeurs predites, pas de derivees renseignees)
  */
  virtual int  has_interface_blocs() const;
  virtual void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const;
  virtual void assembler_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const;
  virtual void assembler_blocs_avec_inertie(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {});

  /* methodes auxiliaires de l'interface _blocs : champ conserve par l'equation et ses valeurs sur les CLs de type Dirichlet ou Neumann_val_ext
     par defaut, champ_conserve = coefficient_temporel * inconnue
     ce champ est mutable pour que le schema en temps puisse le mettre a jour
  */
  Champ_Inc_base& champ_conserve() const //le champ  : autant de valeurs spatiales / temporelles que l'inconnue
  {
    return champ_conserve_.valeur();
  }
  int has_champ_conserve() const
  {
    return champ_conserve_.non_nul();
  }

  void init_champ_conserve() const; //a appeller dans le completer() des operateurs/sources qui auront besoin de champ_conserve_
  /* fonction de calcul par defaut de champ_conserve */
  static void calculer_champ_conserve(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv);
  /* renvoie le nom du champ conserve et la fonction pour le calculer -> a surcharger  */
  virtual std::pair<std::string, fonc_calc_t> get_fonc_champ_conserve() const
  {
    return { inconnue().le_nom().getString(), calculer_champ_conserve};
  }

  virtual Champ_Inc_base& champ_convecte() const //par defaut le champ conserve
  {
    return champ_conserve_.valeur();
  }
  virtual int has_champ_convecte() const
  {
    return champ_conserve_.non_nul();
  }
  virtual void init_champ_convecte() const
  {
    init_champ_conserve();
  };
  //mise a jour de champ_conserve / champ_convecte : appele par Probleme_base::mettre_a_jour() apres avoir mis a jour le milieu
  //si reset = 1, force le calcul de toutes les valeurs temporelles (et pas seulement de la valeur courante)
  virtual void mettre_a_jour_champs_conserves(double temps, int reset = 0);

  //Methodes de l interface des champs postraitables
  /////////////////////////////////////////////////////
  void creer_champ(const Motcle& motlu) override;
  const Champ_base& get_champ(const Motcle& nom) const override;
  void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const override;
  /////////////////////////////////////////////////////

  virtual const Motcle& domaine_application() const;
  virtual void verifie_ch_init_nb_comp(const Champ_Inc_base& ch_ref, const int nb_comp) const;
  virtual void verifie_ch_init_nb_comp_cl(const Champ_Inc_base& ch_ref, const int nb_comp, const Cond_lim_base& cl) const
  {
    verifie_ch_init_nb_comp(ch_ref, nb_comp);
  }

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
  virtual int equation_non_resolue() const;
  int disable_equation_residual() const { return disable_equation_residual_; };

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


  inline void set_residuals(DoubleTab r)
  {
    if(field_residu_.non_nul())
      {
        if (field_residu_.valeur().valeurs().dimension_tot(0) == r.dimension_tot(0)) field_residu_.valeur().valeurs() = r;
        else
          {
            DoubleTab_parts parts(r);
            if (parts[0].dimension_tot(0) == field_residu_.valeur().valeurs().dimension_tot(0)) field_residu_.valeur().valeurs() = parts[0];
          }
      }
  };
  virtual int positive_unkown() {return 0;};

protected :

  Nom nom_;
  Solveur_Masse solveur_masse;
  Sources les_sources;
  REF(Schema_Temps_base) le_schema_en_temps;
  REF(Zone_dis) la_zone_dis;
  Zone_Cl_dis la_zone_Cl_dis;
  REF(Probleme_base) mon_probleme;
  virtual void set_param(Param& titi);
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
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

  //memoization of the matrix for PolyMAC
  mutable Matrice_Morse matrice_stockee;
  mutable int matrice_init;

  //pour l'interface assembler_blocs
  mutable Champ_Inc champ_conserve_;
  mutable Champ_Inc champ_convecte_;

  // pour une positivation du terme en fin d'iteration si necessaire
  // renvoie 1 pour un champ positif, 0 pour un champ negatif

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
  Champ_Fonc field_residu_;

  mutable DoubleTab NULL_;
  mutable Parser_U equation_non_resolue_;
  int disable_equation_residual_ = 0;

  // For multistep methods, store previous dI/dt(n), dI/dt(n-1),...
  Champ_Inc derivee_en_temps_;
  int calculate_time_derivative_;

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
