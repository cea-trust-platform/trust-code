/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Sortie_libre_pression_imposee.h>
#include <Fluide_Incompressible.h>
#include <Navier_Stokes_std.h>
#include <Champ_Uniforme.h>
#include <Equation_base.h>

Implemente_instanciable_sans_constructeur(Sortie_libre_pression_imposee, "Frontiere_ouverte_pression_imposee", Neumann_sortie_libre);
// XD frontiere_ouverte_pression_imposee neumann frontiere_ouverte_pression_imposee -1 Imposed pressure condition at the open boundary called bord (edge). The imposed pressure field is expressed in Pa.
// XD attr ch front_field_base ch 0 Boundary field type.


Sortie_libre_pression_imposee::Sortie_libre_pression_imposee() : d_rho(-123.) { }

Sortie& Sortie_libre_pression_imposee::printOn(Sortie& s) const { return s << que_suis_je() << finl; }

Entree& Sortie_libre_pression_imposee::readOn(Entree& s)
{
  if (app_domains.size() == 0) app_domains = { Motcle("Hydraulique"), Motcle("indetermine") };

  s >> le_champ_front;
  le_champ_ext.typer("Champ_front_uniforme");
  le_champ_ext->valeurs().resize(1, dimension);
  return s;
}

/*! @brief Complete les conditions aux limites.
 *
 * Impose la masse volumique constante du milieu
 *     physique de l'equation a d_rho.
 *
 */
void Sortie_libre_pression_imposee::completer()
{
  const Milieu_base& mil = mon_dom_cl_dis->equation().milieu();
  if (sub_type(Fluide_Incompressible,mil) && mon_dom_cl_dis->equation().que_suis_je() != "QDM_Multiphase")
    {
      if (sub_type(Champ_Uniforme, mil.masse_volumique()))
        {
          const Champ_Uniforme& rho = ref_cast(Champ_Uniforme, mil.masse_volumique());
          d_rho = rho.valeurs()(0, 0);
        }
      else
        {
          d_rho = -1;

          // GF dans les cas a rho non const QC FT on ne doit pas diviser par rho on met d_rho=1
          d_rho = 1;
        }
    }
  else
    d_rho = 1;
}

/*! @brief Renvoie la valeur du flux impose sur la i-eme composante du champ representant le flux a la frontiere.
 *
 *     Le champ a la frontiere est considere constant sur tous les elements de la frontiere.
 *     La valeur du flux impose a la frontiere est egale a la valeur du champ (considere constant) a la frontiere divise par d_rho.
 *
 * @param (int i) indice suivant la premiere dimension du champ
 * @return (double) la valeur imposee sur la composante du champ specifiee
 * @throws deuxieme dimension du champ de frontiere superieur a 1
 */
double Sortie_libre_pression_imposee::flux_impose(int i) const
{
  return flux_impose(i,0);
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
double Sortie_libre_pression_imposee::flux_impose(int i, int j) const
{
  const Milieu_base& mil = mon_dom_cl_dis->equation().milieu();
  double rho_;
  assert(!est_egal(d_rho, -123.));
  if (d_rho == -1)
    {
      const Champ_base& rho = mil.masse_volumique();
      rho_ = rho.valeurs()(i);
    }
  else
    rho_ = d_rho;

  if (le_champ_front->valeurs().dimension(0) == 1)
    return le_champ_front->valeurs()(0, j) / rho_;
  else if (j < le_champ_front->valeurs().dimension(1))
    return le_champ_front->valeurs()(i, j) / rho_;
  else
    Cerr << "Sortie_libre_pression_imposee::flux_impose erreur" << finl;
  Process::exit();
  return 0.;
}
