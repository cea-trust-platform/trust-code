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

#include <Champ_Front_Fonction.h>
#include <EChaine.h>
#include <Champ_Inc_base.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Domaine_VF.h>
#include <Front_VF.h>

Implemente_instanciable(Champ_Front_Fonction,"Champ_Front_Fonction",Ch_front_var_instationnaire_dep);
// XD champ_front_fonction front_field_base champ_front_fonction 0 boundary field that is function of another field
// XD attr dim entier dim 0 Number of field components.
// XD attr inco chaine inco 0 Name of the field (for example: temperature).
// XD attr expression chaine expression 0 keyword to use a analytical expression like 10.*EXP(-0.1*val) where val be the keyword for the field.


/*! @brief NE FAIT RIEN
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Champ_Front_Fonction::printOn(Sortie& os) const
{
  return os;
}

/*! @brief Lecture du Champ a partir d'un flot d'entree, (On ne sait traiter que les champs scalaires.
 *
 * )
 *      exemple:
 *      Champ_Front_Fonction
 *      1 (nombre de composantes)
 *      temperature  (ch est fonction de la temperature)
 *      val*6 (expression du parser)
 *
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Champ_Front_Fonction::readOn(Entree& is)
{


  int nbcomp;
  nbcomp=lire_dimension(is,que_suis_je());
  fixer_nb_comp(nbcomp);
  is >> nom_champ_parametre_;

  Cerr<<"The analytical fonction is read... "<<finl;
  la_table.lire_fxyzt(is, nbcomp);
  return is;
}
void Champ_Front_Fonction::mettre_a_jour(double temps)
{
  const Champ_base& ch=ref_pb.valeur().get_champ(nom_champ_parametre_);
  const Front_VF& fvf = ref_cast(Front_VF, frontiere_dis());
  const Domaine_VF& zvf = ref_cast(Domaine_VF, frontiere_dis().domaine_dis());
  DoubleTab& tab=valeurs_au_temps(temps);
  DoubleTab tab_ch(tab), pos(tab.dimension_tot(0), dimension);
  ch.trace(frontiere_dis(),tab_ch,temps,0);
  assert(tab.dimension(0)==tab_ch.dimension(0));
  for (int i = 0, j; i < fvf.nb_faces_tot(); i++)
    for (j = 0; j < dimension; j++) pos(i, j) = zvf.xv(fvf.num_face(i), j);
  la_table.valeurs(tab_ch, pos, temps, tab);
}

int Champ_Front_Fonction::initialiser(double temps, const Champ_Inc_base& inco)
{
  if (!Champ_front_var_instationnaire::initialiser(temps,inco))
    return 0;
  ref_pb=inco.equation().probleme();
  mettre_a_jour(temps);
  return 1;
}
Champ_front_base& Champ_Front_Fonction::affecter_(const Champ_front_base& ch)
{
  Cerr<<__FILE__<<(int)__LINE__<<" not coded" <<finl;
  exit();
  return (*this);
}



