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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Champ_Front_xyz_Tabule.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Front_xyz_Tabule.h>
#include <Domaine.h>
#include <Frontiere_dis_base.h>
#include <Zone_VF.h>
#include <DoubleTrav.h>
#include <Lecture_Table.h>

Implemente_instanciable( Champ_Front_xyz_Tabule, "Champ_Front_xyz_Tabule", Champ_front_txyz ) ;
// XD Champ_Front_xyz_Tabule champ_front_fonc_txyz Champ_Front_xyz_Tabule 0 Space dependent field on the boundary, tabulated as a function of time.
// XD attr val listchaine val 0 Values of field components (mathematical expressions).
// XD attr bloc bloc_lecture bloc 0 {nt1 t2 t3 ....tn u1 [v1 w1 ...] u2 [v2 w2 ...] u3 [v3 w3 ...] ... un [vn wn ...] } NL2 Values are entered into a table based on n couples (ti, ui) if nb_comp value is 1. The value of a field at a given time is calculated by linear interpolation from this table.

Sortie& Champ_Front_xyz_Tabule::printOn( Sortie& os ) const
{
  Champ_front_txyz::printOn( os );
  return os;
}

Entree& Champ_Front_xyz_Tabule::readOn( Entree& is )
{

  Champ_front_txyz::readOn( is );
  Lecture_Table lec_table;
  lec_table.lire_table(is,la_table);

  return is;
}


void Champ_Front_xyz_Tabule::mettre_a_jour(double temps)
{
  Champ_front_txyz::mettre_a_jour(temps);
  DoubleTab& tab=valeurs_au_temps(temps);
  double val_t = la_table.val(temps);
  tab*=val_t;
  tab.echange_espace_virtuel();
}

double Champ_Front_xyz_Tabule::valeur_au_temps_et_au_point(double temps,int som,double x,double y, double z, int k) const
{
  double val_xyz = Champ_front_txyz::valeur_au_temps_et_au_point(temps,som,x,y,z,k);
  double val_t = la_table.val(temps);
  return val_xyz*val_t;
}
