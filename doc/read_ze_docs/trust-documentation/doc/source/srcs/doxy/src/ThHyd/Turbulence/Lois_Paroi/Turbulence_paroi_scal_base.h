/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Turbulence_paroi_scal_base_included
#define Turbulence_paroi_scal_base_included

#include <Champs_compris_interface.h>
#include <Champs_compris.h>
#include <Champ_Fonc.h>
#include <TRUSTVects.h>
#include <Champ_Inc.h>
#include <TRUST_Ref.h>

class Modele_turbulence_scal_base;
class Probleme_base;
class EcrFicPartage;
class Domaine_Cl_dis;
class Champ_Don;
class Domaine_dis;
class Domaine_VF;

/*! @brief Classe Turbulence_paroi_scal_base Classe de base pour la hierarchie des classes representant les modeles
 *
 *     de calcul des grandeurs turbulentes aux voisinages des parois.
 *     La methode principale a implenter dans les classes filles est calculer_scal
 *     qui doit se charger de calculer et remplir le tableau equivalent_distance_ des distances equivalentes
 *     en paroi.
 *
 * @sa Classe abstraite, Methodes abstraites
 */
class Turbulence_paroi_scal_base: public Champs_compris_interface, public Objet_U
{

  Declare_base_sans_constructeur(Turbulence_paroi_scal_base);

public:
  Turbulence_paroi_scal_base() : nb_impr_(0), calcul_ldp_en_flux_impose_(0), Prdt_sur_kappa_(2.12) { }
  inline void associer_modele(const Modele_turbulence_scal_base&);
  virtual void associer(const Domaine_dis&, const Domaine_Cl_dis&)=0;
  virtual void completer() { }
  virtual int init_lois_paroi() =0;
  inline int calculer_scal(Champ_Fonc&);
  virtual int calculer_scal(Champ_Fonc_base&) =0;
  virtual void imprimer_nusselt(Sortie&) const
  {
    Cerr << "imprimer_nusselt non code pour " << que_suis_je() << finl;
  }

  void creer_champ(const Motcle& motlu) override;
  const Champ_base& get_champ(const Motcle& nom) const override;
  void get_noms_champs_postraitables(Noms& nom, Option opt = NONE) const override;

  // Ecriture dans un fichier separe de u_star, Cisaillement_paroi etc...
  void ouvrir_fichier_partage(EcrFicPartage&, const Nom&) const;
  inline const int& get_flag_calcul_ldp_en_flux_impose() const { return calcul_ldp_en_flux_impose_; }

  virtual bool use_equivalent_distance() const;
  virtual DoubleVect& equivalent_distance_name(DoubleVect& d_eq, const Nom& nom_bord) const = 0;

  inline const DoubleVects& equivalent_distance() const { return equivalent_distance_; }
  inline const DoubleVect& equivalent_distance(int bord) const
  {
    assert(bord < equivalent_distance_.size());
    return equivalent_distance_[bord];
  }
  inline double equivalent_distance(int bord, int face) const
  {
    assert(bord < equivalent_distance_.size());
    assert(face < equivalent_distance_[bord].size());
    return equivalent_distance_[bord](face);
  }

  inline const DoubleVects& tab_equivalent_distance() const { return equivalent_distance_; }
  inline int tab_equivalent_distance_size() { return equivalent_distance_.size(); }

  inline const DoubleVect& tab_equivalent_distance(int bord) const
  {
    assert(bord < equivalent_distance_.size());
    return equivalent_distance_[bord];
  }

protected:
  REF(Modele_turbulence_scal_base) mon_modele_turb_scal;
  mutable int nb_impr_;        // Compteur d'impression
  int calcul_ldp_en_flux_impose_; // flag defenissant si on utilise la ldp en flux impose 0 par defaut
  double Prdt_sur_kappa_;         // Constante dans la loi de paroi
  inline double T_plus(double y_plus, double Pr);

  DoubleVects equivalent_distance_;
  // tableau des distances equivalentes sur chaque bord
  // calculees lors de l'application des lois
  // de paroi et a utiliser dans le calcul des
  // coefficients d'echange a la paroi
  // En VDF contrairement au VEF, on n'a pas besoin des contributions des faces de
  // bord virtuelles car le scalaire est au centre des elements en VDF alors
  // qu'il est au centre des faces en VEF
  // Comme il faut tenir compte des faces de virtuelles de bord
  // en VEF pour l'application dans l'operateur de diffusion
  // de equivalent_distance_, on utilise tableau par bord et
  // chaque tableau est dimensionne au nombre de faces de
  // bord totales (reelles+virtuelles)

private:

  Champs_compris champs_compris_;

};

// Loi analytique avec raccordement des comportements
// asymptotiques de la temperature adimensionnee T+
// avec Kader:
// sous-couche conductrice : T+=Pr y+
// domaine logarithmique : T+=2.12*ln(y+)+Beta
inline double Turbulence_paroi_scal_base::T_plus(double y_plus, double Pr)
{
  double Gamma = (0.01 * pow(Pr * y_plus, 4.)) / (1. + 5. * pow(Pr, 3.) * y_plus);
  double Beta = pow(3.85 * pow(Pr, 1. / 3.) - 1.3, 2.) + Prdt_sur_kappa_ * log(Pr);
  return Pr * y_plus * exp(-Gamma) + (Prdt_sur_kappa_ * log(1. + y_plus) + Beta) * exp(-1. / (Gamma + 1e-20));
}

/*! @brief Associe un modele de turbulence a l'objet.
 *
 * @param (Modele_turbulence_hyd_base& le_modele) le modele de turbulence hydraulique a associer a l'objet
 */
inline void Turbulence_paroi_scal_base::associer_modele(const Modele_turbulence_scal_base& le_modele)
{
  mon_modele_turb_scal = le_modele;
}

/*! @brief Simple appel a int calculer_scal(Champ_Fonc_base& ).
 *
 * @param (Champ_Fonc& ch)
 * @return (int) code de retour propage
 */
inline int Turbulence_paroi_scal_base::calculer_scal(Champ_Fonc& ch)
{
  return calculer_scal(ch.valeur());
}

#endif
