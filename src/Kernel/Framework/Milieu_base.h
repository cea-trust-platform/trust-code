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

#ifndef Milieu_base_included
#define Milieu_base_included

#include <Champ.h>
#include <Champ_Don.h>
#include <Champ_Fonc.h>
#include <Champ_Inc_base.h>
#include <Ref_Champ_Don_base.h>
#include <Champs_compris.h>
#include <Champs_compris_interface.h>
#include <Interface_blocs.h>

class Champ_Don;
class Motcle;
class Discretisation_base;
class Probleme_base;
class Param;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Milieu_base
//     Cette classe est la base de la hierarchie des milieux (physiques)
//     Elle regroupe les fonctionnalites (communes) et les proprietes physiques
//     d'un milieu. Cette classe contient les principaux champs donnees
//     caracterisant le milieu:
//        - masse volumique       (rho)
//        - diffusivite           (alpha)
//        - conductivite          (lambda)
//        - capacite calorifique  (Cp)
//        - dilatabilite          (beta_th)
// .SECTION voir aussi
//     Milieu Solide Fluide_Incompressible Constituant
//     Classe abstraite dont tous les milieux physiques doivent deriver.
//     Methodes abstraites:
//       void tester_champs_lus()
//       void mettre_a_jour(double temps)
//       void initialiser()
//////////////////////////////////////////////////////////////////////////////
class Milieu_base : public Champs_compris_interface, public Objet_U
{
  Declare_base_sans_constructeur(Milieu_base);

public:

  Milieu_base();
  virtual void set_param(Param& param);
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  virtual void preparer_calcul();
  virtual void verifier_coherence_champs(int& err,Nom& message);
  virtual void creer_champs_non_lus();
  virtual void discretiser(const Probleme_base& pb, const  Discretisation_base& dis);
  virtual int is_rayo_semi_transp() const;
  virtual int is_rayo_transp() const;
  virtual void mettre_a_jour(double temps);
  virtual bool initTimeStep(double dt);
  virtual void abortTimeStep();
  virtual int initialiser(const double temps);
  virtual void associer_gravite(const Champ_Don_base& );
  virtual const Champ& masse_volumique() const;
  virtual Champ&       masse_volumique();
  virtual const Champ_Don& diffusivite() const;
  virtual Champ_Don&       diffusivite();
  virtual const Champ_Don& conductivite() const;
  virtual Champ_Don&       conductivite();
  virtual const Champ_Don& capacite_calorifique() const;
  virtual Champ_Don&       capacite_calorifique();
  virtual const Champ_Don& beta_t() const;
  virtual Champ_Don&       beta_t();
  virtual int                a_gravite() const;
  virtual const Champ_Don_base& gravite() const;
  virtual Champ_Don_base&       gravite();
  int                associer_(Objet_U&) override;
  virtual int est_deja_associe();
  void nommer(const Nom&) override;
  const Nom& le_nom() const override;

  //Methodes de l interface des champs postraitables
  //////////////////////////////////////////////////////
  void creer_champ(const Motcle& motlu) override;
  const Champ_base& get_champ(const Motcle& nom) const override;
  void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const override;
  /////////////////////////////////////////////////////
  virtual void update_rho_cp(double temps);

  // equations associees au milieu
  virtual void associer_equation(const Equation_base* eqn) const;
  virtual const Equation_base& equation(const std::string& nom_inc) const;

  //controle du domaine de validite des inconnues des equations associees au milieu
  //par defaut renvoie 1 (OK)
  virtual int check_unknown_range() const
  {
    return 1;
  }

  void set_id_composite(const int i);

  int id_composite = -1;

protected:

  Champ rho; //peut etre un Champ_Don ou un Champ_Inc
  Champ_Don alpha;
  Champ_Don lambda;
  Champ_Don Cp;
  Champ_Don beta_th;
  Champ_Fonc rho_cp_elem_,rho_cp_comme_T_;
  Nom nom_;
  REF(Champ_Don_base) g;

  enum Type_rayo { NONRAYO, TRANSP, SEMITRANSP };

  Type_rayo indic_rayo_;

  void ecrire(Sortie& ) const;
  void creer_alpha();
  virtual void calculer_alpha();

  mutable int deja_associe;


  Champs_compris champs_compris_;

  mutable std::map<std::string, const Equation_base *> equation_;

  void creer_derivee_rho();
};


#endif
