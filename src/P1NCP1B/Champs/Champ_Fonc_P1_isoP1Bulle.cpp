/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Champ_Fonc_P1_isoP1Bulle.cpp
// Directory:   $TRUST_ROOT/src/P1NCP1B/Champs
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////


#include <Champ_Fonc_P1_isoP1Bulle.h>
#include <Zone_VEF_PreP1b.h>

Implemente_instanciable(Champ_Fonc_P1_isoP1Bulle,"Champ_Fonc_P1_isoP1Bulle",Champ_Fonc_base);


//     printOn()
/////

Sortie& Champ_Fonc_P1_isoP1Bulle::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Champ_Fonc_P1_isoP1Bulle::readOn(Entree& s)
{
  return s ;
}

int Champ_Fonc_P1_isoP1Bulle::fixer_nb_valeurs_nodales(int n)
{
  // On ne doit pas specifier nb_ddl !
  assert(n < 0);
  const Zone_VEF_PreP1b& zvef = zone_vef();
  const MD_Vector& md = zvef.md_vector_p1b();
  creer_tableau_distribue(md);
  return n;
}

////////////////////////////////////////////////////////////////////
//
//   Implementation de fonctions de la classe Champ_Fonc_P1_isoP1Bulle
//
////////////////////////////////////////////////////////////////////

// Description :
// Renvoie la Zone_VEF
const Zone_dis_base& Champ_Fonc_P1_isoP1Bulle::zone_dis_base() const
{
  return la_zone_VEFP1B.valeur();
}

// Description :
// Verifie que z_dis est une Zone_VEF
// cree l'association.
void Champ_Fonc_P1_isoP1Bulle::associer_zone_dis_base(const Zone_dis_base& z_dis)
{
  la_zone_VEFP1B=ref_cast(Zone_VEF_PreP1b, z_dis);
}

// Description :
// sans action !
void Champ_Fonc_P1_isoP1Bulle::mettre_a_jour(double t)
{
  Champ_Fonc_base::mettre_a_jour(t);
}

// Description :
//Voir Champ_base::trace
DoubleTab& Champ_Fonc_P1_isoP1Bulle::trace(const Frontiere_dis_base& fr, DoubleTab& x, double tps) const
{
  return Champ_P1iP1B_implementation::trace(fr, valeurs(), x);
}

void Champ_Fonc_P1_isoP1Bulle::completer(const Zone_Cl_dis_base& zcl)
{
  Champ_P1iP1B_implementation::completer(zcl);
}
