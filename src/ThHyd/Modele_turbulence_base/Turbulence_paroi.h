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

#ifndef Turbulence_paroi_included
#define Turbulence_paroi_included

#include <Turbulence_paroi_base.h>
#include <TRUST_Vector.h>
#include <TRUST_Deriv.h>

class Pb_Hydraulique;
class Probleme_base;

/*! @brief Classe Turbulence_paroi Classe generique de la hierarchie des turbulences au niveau de la
 *
 *     paroi, un objet Turbulence_paroi peut referencer n'importe quel
 *     objet derivant Turbulence_paroi_base.
 *     La plupart des methodes appellent les methodes de l'objet Probleme
 *     sous-jacent via la methode valeur() declaree grace a la macro
 *
 * @sa Turbulence_paroi_base
 */
class Turbulence_paroi : public DERIV(Turbulence_paroi_base)
{

  Declare_instanciable(Turbulence_paroi);

public:

  //Turbulence_paroi(const Turbulence_paroi_base& x):DERIV(Turbulence_paroi_base)(x) {}
  // inline Turbulence_paroi& operator=(const Turbulence_paroi_base& paroi_base);
  void associer_modele(const Mod_turb_hyd_base& );
  inline int init_lois_paroi();
  inline int calculer_hyd(Champ_Inc& );
  inline int calculer_hyd_BiK(Champ_Inc& , Champ_Inc& );
  inline int calculer_hyd(Champ_Fonc& , Champ_Fonc& );
  inline int calculer_hyd(Champ_Inc_base& );
  inline int calculer_hyd(Champ_Fonc_base& ,Champ_Fonc_base& );
  inline void imprimer_ustar(Sortie& ) const;
  inline void imprimer_ustar_mean_only(Sortie&, int, const LIST(Nom)&, const Nom&) const;
  inline void imprimer_premiere_ligne_ustar(int, const LIST(Nom)&, const Nom& ) const;
  // rajout pour prendre en compte Cisaillement_paroi dans la classe
  // generique
  inline const DoubleTab& Cisaillement_paroi() const ;

protected:

  REF2(Mod_turb_hyd_base) mon_modele_turb_hyd;
};

/*! @brief Operateur d'affectation d'un objet Turbulence_paroi_base dans un objet Turbulence_paroi.
 *
 * @param (Turbulence_paroi_base& paroi_base) la partie droite de l'affectation
 * @return (Turbulence_paroi&) le resultat de l'affectation (*this)
 */
/*inline Turbulence_paroi& Turbulence_paroi::operator=(const Turbulence_paroi_base& paroi_base)
  {
  DERIV(Turbulence_paroi_base)::operator=(paroi_base);
  return *this;
  }*/


/*! @brief Appel a l'objet sous-jacent.
 *
 * Initialise les lois de parois.
 *
 * @return (int) code de retour propage
 */
inline int Turbulence_paroi::init_lois_paroi()
{
  return valeur().init_lois_paroi();
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * Imprime les u* de la loi de paroi
 *
 * @return code de retour propage
 */
inline void Turbulence_paroi::imprimer_ustar(Sortie& os) const
{
  valeur().imprimer_ustar(os);
}


/*! @brief Appel a l'objet sous-jacent.
 *
 * Imprime les u* moyens de la loi de paroi
 *
 * @return code de retour propage
 */
inline void Turbulence_paroi::imprimer_ustar_mean_only(Sortie& os, int boundaries_, const LIST(Nom)& boundaries_list, const Nom& nom_fichier_) const
{
  valeur().imprimer_ustar_mean_only(os, boundaries_, boundaries_list, nom_fichier_);
}


/*! @brief Appel a l'objet sous-jacent.
 *
 * Imprime la premiere ligne du fichier ***ustar_mean_only.out
 *
 */
inline void Turbulence_paroi::imprimer_premiere_ligne_ustar(int boundaries_, const LIST(Nom)& boundaries_list, const Nom& nom_fichier_ ) const
{
  valeur().imprimer_premiere_ligne_ustar(boundaries_, boundaries_list, nom_fichier_);
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * @param (Champ_Inc& ch)
 * @return (int) code de retour propage
 */
inline int Turbulence_paroi::calculer_hyd(Champ_Inc& ch)
{
  return valeur().calculer_hyd(ch);
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * @param (Champ_Fonc& ch1)
 * @param (Champ_Fonc& ch2)
 * @return (int) code de retour propage
 */
inline int Turbulence_paroi::calculer_hyd_BiK(Champ_Inc& ch1 , Champ_Inc& ch2 )
{
  return valeur().calculer_hyd_BiK(ch1->valeurs(),ch2->valeurs());
}


/*! @brief Appel a l'objet sous-jacent
 *
 * @param (Champ_Fonc& ch1)
 * @param (Champ_Fonc& ch2)
 * @return (int) code de retour propage
 */
inline int Turbulence_paroi::calculer_hyd(Champ_Fonc& ch1,Champ_Fonc& ch2)
{
  return valeur().calculer_hyd(ch1,ch2);
}

/*! @brief Appel a l'objet sous-jacent.
 *
 * @param (Champ_Inc_base& ch)
 * @return (int) code de retour propage
 */
inline int Turbulence_paroi::calculer_hyd(Champ_Inc_base& ch)
{
  return valeur().calculer_hyd(ch.valeurs());
}


/*! @brief Appel a l'objet sous-jacent.
 *
 * @param (Champ_Fonc_base& ch1)
 * @param (Champ_Fonc_base& ch2)
 * @return (int) code de retour propage
 */
inline int Turbulence_paroi::calculer_hyd(Champ_Fonc_base& ch1,Champ_Fonc_base& ch2)
{
  return valeur().calculer_hyd(ch1.valeurs(),ch2.valeurs());
}


#endif
