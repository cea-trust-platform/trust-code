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

#include <Sortie_libre_pression_moyenne_imposee.h>
#include <Navier_Stokes_std.h>
#include <Champ_Uniforme.h>
#include <Milieu_base.h>
#include <TRUSTTab.h>
#include <Domaine_VF.h>

Implemente_instanciable_sans_constructeur(Sortie_libre_pression_moyenne_imposee, "Frontiere_ouverte_pression_moyenne_imposee", Neumann_sortie_libre);

Sortie_libre_pression_moyenne_imposee::Sortie_libre_pression_moyenne_imposee() : d_rho(-123.) { }

Sortie& Sortie_libre_pression_moyenne_imposee::printOn(Sortie& s) const { return s << que_suis_je() << finl; }

Entree& Sortie_libre_pression_moyenne_imposee::readOn(Entree& s)
{
  if (app_domains.size() == 0) app_domains = { Motcle("Hydraulique"), Motcle("indetermine") };

  s >> Pext_;
  le_champ_ext.typer("Champ_front_uniforme");
  le_champ_ext.valeurs().resize(1, dimension);
  le_champ_front.typer("Champ_front_fonc");
  le_champ_front->fixer_nb_comp(1);
  return s;
}

void Sortie_libre_pression_moyenne_imposee::completer()
{
  const Milieu_base& mil = mon_dom_cl_dis->equation().milieu();
  if (sub_type(Champ_Uniforme, mil.masse_volumique().valeur()))
    {
      const Champ_Uniforme& rho = ref_cast(Champ_Uniforme, mil.masse_volumique().valeur());
      d_rho = rho(0, 0);
    }
  else
    d_rho = -1;

  int i;
  ndeb_ = ref_cast(Front_VF,frontiere_dis()).num_premiere_face();
  nb_faces_ = ref_cast(Front_VF,frontiere_dis()).nb_faces();
  le_champ_front.valeurs().resize(nb_faces_, 1);
  DoubleTab& Pimp = le_champ_front.valeurs();
  for (i = 0; i < nb_faces_; i++)
    Pimp(i, 0) = Pext_;

  surfaces.resize(nb_faces_);

  Domaine_VF& zvf = ref_cast(Domaine_VF, mon_dom_cl_dis->domaine_dis().valeur());
  for (i = 0; i < nb_faces_; i++)
    {
      surfaces(i) = zvf.face_surfaces(i + ndeb_);
    }

  face_voisins = zvf.face_voisins();
}

/*! @brief Met a jour les conditions aux limites.
 *
 */
void Sortie_libre_pression_moyenne_imposee::mettre_a_jour(double temps)
{
  Cond_lim_base::mettre_a_jour(temps);
  const DoubleTab& tab_P = ref_cast(Navier_Stokes_std,mon_dom_cl_dis->equation()).pression().valeurs();

  int face, elem, facegl;
  DoubleTab& Pimp = le_champ_front.valeurs();

  double Ptot = 0, s, S = 0;

  for (face = 0; face < nb_faces_; face++)
    {
      facegl = face + ndeb_;
      elem = face_voisins(facegl, 0);
      if (elem == -1)
        elem = face_voisins(facegl, 1);
      s = surfaces(face);
      S += s;
      Ptot += s * tab_P(elem);
    }
  Ptot = mp_sum(Ptot);
  S = mp_sum(S);
  Ptot /= S;

  Cerr << "Sortie_libre_pression_moyenne_imposee  pmoy= " << Ptot << finl;

  for (face = 0; face < nb_faces_; face++)
    {
      facegl = face + ndeb_;
      elem = face_voisins(facegl, 0);
      if (elem == -1)
        elem = face_voisins(facegl, 1);
      Pimp(face, 0) = tab_P(elem) + (Pext_ - Ptot);
    }
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
double Sortie_libre_pression_moyenne_imposee::flux_impose(int i) const
{
  const Milieu_base& mil = mon_dom_cl_dis->equation().milieu();
  const Champ_base& rho = mil.masse_volumique().valeur();
  double rho_;
  assert(!est_egal(d_rho, -123.));
  if (d_rho == -1)
    rho_ = rho(i);
  else
    rho_ = d_rho;

  if (le_champ_front.valeurs().size() == 1)
    return le_champ_front(0, 0) / rho_;
  else if (le_champ_front.valeurs().dimension(1) == 1)
    return le_champ_front(i, 0) / rho_;
  else
    Cerr << "Neumann::flux_impose erreur" << finl;
  exit();
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
double Sortie_libre_pression_moyenne_imposee::flux_impose(int i, int j) const
{
  const Milieu_base& mil = mon_dom_cl_dis->equation().milieu();
  const Champ_base& rho = mil.masse_volumique().valeur();
  double rho_;
  assert(!est_egal(d_rho, -123.));
  if (d_rho == -1)
    rho_ = rho(i);
  else
    rho_ = d_rho;

  if (le_champ_front.valeurs().dimension(0) == 1)
    return le_champ_front(0, j) / rho_;
  else
    return le_champ_front(i, j) / rho_;
}

