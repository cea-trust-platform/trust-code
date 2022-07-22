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

#include <Sortie_libre_pression_imposee_QC.h>
#include <Champ_Uniforme.h>
#include <Equation_base.h>
#include <Milieu_base.h>
#include <Motcle.h>

Implemente_instanciable_sans_constructeur(Sortie_libre_pression_imposee_QC,"Frontiere_ouverte_pression_totale_imposee",Neumann_sortie_libre);

Sortie_libre_pression_imposee_QC::Sortie_libre_pression_imposee_QC() : Pthn(1.e5), d_rho(-1) {}

/*! @brief Ecrit le type de l'objet sur un flot de sortie.
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Sortie_libre_pression_imposee_QC::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

/*! @brief Renvoie un booleen indiquant la compatibilite des conditions aux limites avec l'equation specifiee en parametre.
 *
 *     Des CL de type Sortie_libre_pression_imposee_QC sont compatibles
 *     avec une equation dont le domaine est l'hydraulique (Navier_Stokes)
 *     ou bien indetermine.
 *
 * @param (Equation_base& eqn) l'equation avec laquelle il faut verifier la compatibilite
 * @return (int) valeur booleenne, 1 si les CL sont compatibles avec l'equation 0 sinon
 */
int Sortie_libre_pression_imposee_QC::compatible_avec_eqn(const Equation_base& eqn) const
{
  Motcle dom_app=eqn.domaine_application();
  Motcle Hydraulique="Hydraulique";
  Motcle indetermine="indetermine";
  if ( (dom_app==Hydraulique) || (dom_app==indetermine) )
    return 1;
  else
    {
      err_pas_compatible(eqn);
      return 0;
    }
}

/*! @brief Lit le champ de frontiere a partir d'un flot d'entree.
 *
 * Type le_champ_ext en "Champ_front_uniforme".
 *     Redimmensionne le_champ_ext en fonction de la dimension
 *     de l'espace (dimension)
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree& s) le flot d'entree modifie
 */
Entree& Sortie_libre_pression_imposee_QC::readOn(Entree& s )
{
  s >> le_champ_front;
  le_champ_ext.typer("Champ_front_uniforme");
  le_champ_ext.valeurs().resize(1,dimension);
  return s ;
}


/*! @brief Complete les conditions aux limites.
 *
 * Impose la masse volumique constante du milieu
 *     physique de l'equation a d_rho.
 *
 */
void Sortie_libre_pression_imposee_QC::completer()
{
  const Milieu_base& mil=ma_zone_cl_dis->equation().milieu();
  if (sub_type(Champ_Uniforme,mil.masse_volumique().valeur()))
    {
      const Champ_Uniforme& rho=ref_cast(Champ_Uniforme,mil.masse_volumique().valeur());
      d_rho = rho(0,0);
    }
  else
    d_rho = -1;
}

/*! @brief Renvoie la valeur du flux impose sur la i-eme composante du champ representant le flux a la frontiere.
 *
 *     Le champ a la frontiere est considere constant sur tous
 *     les elements de la frontiere.
 *     La valeur du flux impose a la frontiere est egale
 *     a la valeur du champ (considere constant) a la frontiere divise par d_rho.
 *
 * @param (int i) indice suivant la premiere dimension du champ
 * @return (double) la valeur imposee sur la composante du champ specifiee
 * @throws deuxieme dimension du champ de frontiere superieur a 1
 */
double Sortie_libre_pression_imposee_QC::flux_impose(int i) const
{
  const Milieu_base& mil=ma_zone_cl_dis->equation().milieu();
  const Champ_base& rho=mil.masse_volumique().valeur();
  double rho_;
  if (d_rho==-1) rho_ = rho(i);
  else rho_ = d_rho;

  if (le_champ_front.valeurs().size()==1)
    return ( le_champ_front(0,0) - Pthn ) / rho_;
  else if (le_champ_front.valeurs().dimension(1)==1)
    return ( le_champ_front(i,0) - Pthn ) / rho_;
  else
    Cerr << "Neumann::flux_impose erreur" << finl;

  Process::exit();
  return 0.;
}

/*! @brief Renvoie la valeur du flux impose sur la (i,j)-eme composante du champ representant le flux a la frontiere.
 *
 *     Le champ a la frontiere n'est PAS constant sur tous les elements
 *     la frontiere.
 *
 * @param (int i) indice suivant la premiere dimension du champ
 * @param (int j) indice suivant la deuxieme dimension du champ
 * @return (double) la valeur imposee sur la composante du champ specifiee
 */
double Sortie_libre_pression_imposee_QC::flux_impose(int i,int j) const
{
  const Milieu_base& mil=ma_zone_cl_dis->equation().milieu();
  const Champ_base& rho=mil.masse_volumique().valeur();
  double rho_;

  if (d_rho==-1) rho_ = rho(i);
  else rho_ = d_rho;

  if (le_champ_front.valeurs().dimension(0)==1)
    return ( le_champ_front(0,j) - Pthn ) / rho_;
  else
    return ( le_champ_front(i,j) - Pthn ) / rho_;
}
