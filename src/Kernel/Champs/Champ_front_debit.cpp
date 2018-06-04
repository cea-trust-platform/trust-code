/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Champ_front_debit.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////


#include <Champ_front_debit.h>
#include <Champ_Don.h>
#include <Champ_Inc_base.h>
#include <Equation_base.h>
#include <Milieu_base.h>
#include <Zone_VF.h>
#include <DoubleTrav.h>
Implemente_instanciable(Champ_front_debit,"Champ_front_debit",Champ_front_normal);
// XD champ_front_debit front_field_base champ_front_debit 0 This field is used to define a flow rate field instead of a velocity field for a Dirichlet boundary condition on Navier-Stokes equations.

Sortie& Champ_front_debit::printOn(Sortie& os) const
{
  return Champ_front_normal::printOn(os);
}

Entree& Champ_front_debit::readOn(Entree& is)
{
  is >> champ_debit_; // XD attr ch front_field_base ch 0 uniform field in space to define the flow rate. It could be, for example, champ_front_uniforme, ch_front_input_uniform or champ_front_fonc_txyz that depends only on time.
  fixer_nb_comp(dimension);
  return is;
}

// Description:
// Mise a jour du temps
int Champ_front_debit::initialiser(double tps, const Champ_Inc_base& inco)
{
  if (!Champ_front_normal::initialiser(tps,inco))
    return 0;
  champ_debit_.valeur().initialiser(tps,inco);
  initialiser_coefficient(inco);

  const Zone_VF& zone_VF = ref_cast(Zone_VF,zone_dis());
  const Front_VF& le_bord= ref_cast(Front_VF,frontiere_dis());
  const IntTab& face_voisins = zone_VF.face_voisins();

  // on traite les faces sur la frontiere
  DoubleTab vecteur_norm(le_bord.nb_faces(),dimension);

  // Dimensionnement du tableau
  normales_divisees_par_aire_=les_valeurs[0].valeurs();

  double aire=0.0;        // Aire de la frontiere
  int premiere_face=le_bord.num_premiere_face();
  for(int i=0; i<le_bord.nb_faces(); i++)
    {
      int face = i + premiere_face;
      int elem = face_voisins(face,0);
      int signe = -1;
      if (elem == -1)
        {
          elem = face_voisins(face,1);
          signe = 1;
        }

      double dS=0;
      for (int j=0; j<dimension; j++)
        dS += zone_VF.face_normales(face,j) * zone_VF.face_normales(face,j);

      dS = sqrt(dS);
      aire += dS * zone_VF.porosite_face(face);
      for(int j=0; j<dimension; j++)
        normales_divisees_par_aire_(i,j)=signe*zone_VF.face_normales(face,j)/dS;
    }
  aire=mp_sum(aire);
  normales_divisees_par_aire_/=aire;
  // Remplissage de tous les instants
  int nb_cases=les_valeurs->nb_cases();
  for (int t=0; t<nb_cases; t++)
    {
      DoubleTab& tab=les_valeurs[t].valeurs();
      tab=normales_divisees_par_aire_;
      if (tab.size_array())
        {
          // Allows weighting by rho in Champ_front_debit_massique
          double debit=champ_debit_.valeur().valeurs()(0,0) * coeff_;
          tab*=debit;
        }
    }
  return 1;
}

void Champ_front_debit::initialiser_coefficient(const Champ_Inc_base& inco)
{
  coeff_ = 1.0;
}

void Champ_front_debit::associer_fr_dis_base(const Frontiere_dis_base& fr)
{
  champ_debit_.valeur().associer_fr_dis_base(fr);
  Champ_front_normal::associer_fr_dis_base(fr);
}
void Champ_front_debit::set_temps_defaut(double temps)
{
  champ_debit_.valeur().set_temps_defaut(temps);
  Champ_front_normal::set_temps_defaut(temps);

}
void Champ_front_debit::fixer_nb_valeurs_temporelles(int nb_cases)
{
  champ_debit_.valeur().fixer_nb_valeurs_temporelles(nb_cases);
  Champ_front_normal::fixer_nb_valeurs_temporelles(nb_cases);
}

void Champ_front_debit::changer_temps_futur(double temps,int i)
{
  Champ_front_normal::changer_temps_futur(temps,i);
  champ_debit_.valeur().changer_temps_futur(temps,i);
}

// Description:
//    Tourne la roue de la CL
int Champ_front_debit::avancer(double temps)
{
  Champ_front_normal::avancer(temps);
  return champ_debit_.valeur().avancer(temps);
}

// Description:
//    Tourne la roue de la CL
int Champ_front_debit::reculer(double temps)
{
  Champ_front_normal::reculer(temps);
  return champ_debit_.valeur().reculer(temps);
}

void Champ_front_debit::mettre_a_jour(double temps)
{
  champ_debit_.valeur().mettre_a_jour(temps);

  DoubleTab& tab=valeurs_au_temps(temps);
  if (tab.size_array())
    {
      // Allows weighting by rho in Champ_front_debit_massique
      double debit=champ_debit_.valeur().valeurs_au_temps(temps)(0,0) * coeff_;
      tab=normales_divisees_par_aire_;
      tab*=debit;
    }
}
