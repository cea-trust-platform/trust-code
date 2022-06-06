/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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

#include <Champ_Fonc_P1NC.h>
#include <Zone_VEF.h>

Implemente_instanciable(Champ_Fonc_P1NC,"Champ_Fonc_P1NC",Champ_Fonc_base);


//     printOn()
/////

Sortie& Champ_Fonc_P1NC::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Champ_Fonc_P1NC::readOn(Entree& s)
{
  return s ;
}

////////////////////////////////////////////////////////////////////
//
//   Implementation de fonctions de la classe Champ_Fonc_P1NC
//
////////////////////////////////////////////////////////////////////

// Description :
// Renvoie la Zone_VEF
const Zone_dis_base& Champ_Fonc_P1NC::zone_dis_base() const
{
  return la_zone_VEF.valeur();
}

// Description :
// Verifie que z_dis est une Zone_VEF
// cree l'association.
void Champ_Fonc_P1NC::associer_zone_dis_base(const Zone_dis_base& z_dis)
{
  la_zone_VEF=ref_cast(Zone_VEF, z_dis);
}

// Description :
//
int Champ_Fonc_P1NC::fixer_nb_valeurs_nodales(int nb_noeuds)
{
  assert(nb_noeuds == zone_vef().nb_faces());
  const MD_Vector& md = zone_vef().md_vector_faces();
  creer_tableau_distribue(md);

  Champ_P1NC_implementation::fixer_nb_valeurs_nodales(nb_noeuds);

  return nb_noeuds;
}

// Description :
// sans action !
void Champ_Fonc_P1NC::mettre_a_jour(double t)
{
  Champ_Fonc_base::mettre_a_jour(t);
}

// Description :
//Voir Champ_base::trace
DoubleTab& Champ_Fonc_P1NC::trace(const Frontiere_dis_base& fr, DoubleTab& x, double tps,int distant) const
{
  return Champ_P1NC_implementation::trace(fr, valeurs(), x,distant);
}

int Champ_Fonc_P1NC::imprime(Sortie& os, int ncomp) const
{
  imprime_P1NC(os,ncomp);
  return 1;
}
