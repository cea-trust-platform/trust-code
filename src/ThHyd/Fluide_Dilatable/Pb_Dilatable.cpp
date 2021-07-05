/****************************************************************************
 * Copyright (c) 2021, CEA
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
// File:        Pb_Dilatable.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Quasi_Compressible/Problems
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Pb_Dilatable.h>
#include <Equation_base.h>
#include <Schema_Euler_explicite.h>
#include <Schema_Euler_Implicite.h>
#include <RRK2.h>
#include <RK3.h>
#include <Pred_Cor.h>
#include <Debog.h>
#include <Domaine.h>
#include <Loi_Fermeture_base.h>
#include <Probleme_Couple.h>

Implemente_base(Pb_Dilatable,"Pb_Dilatable",Pb_qdm_fluide);

Sortie& Pb_Dilatable::printOn(Sortie& os) const
{
  return Pb_qdm_fluide::printOn(os);
}

Entree& Pb_Dilatable::readOn(Entree& is)
{
  return Pb_qdm_fluide::readOn(is);
}

void Pb_Dilatable::associer_milieu_base(const Milieu_base& mil)
{
  Pb_qdm_fluide::associer_milieu_base(mil);
}

void Pb_Dilatable::associer_sch_tps_base(const Schema_Temps_base& sch)
{
  if (!sub_type(Schema_Euler_explicite,sch) && !sub_type(Schema_Euler_Implicite,sch)
      && !sub_type(RRK2,sch) && !sub_type(RK3,sch))
    {
      Cerr << "TRUST can't solve a " << que_suis_je() << " with a " << sch.que_suis_je() << " time scheme." << finl;
      Process::exit();
    }
  if ( sub_type(Schema_Euler_Implicite,sch) && coupled_==1 )
    {
      Cerr << finl;
      Cerr << "Coupled problem with unique Euler implicit time scheme with " << que_suis_je() <<  "fluid are not allowed!" << finl;
      Cerr << "Choose another time scheme or set a time scheme for each of your problems." << finl;
      Process::exit();
    }
  Pb_qdm_fluide::associer_sch_tps_base(sch);
}

void Pb_Dilatable::preparer_calcul()
{
  Pb_qdm_fluide::preparer_calcul();
}

bool Pb_Dilatable::initTimeStep(double dt)
{
  return Pb_qdm_fluide::initTimeStep(dt);
}

void Pb_Dilatable::mettre_a_jour(double temps)
{
  Debog::set_nom_pb_actuel(le_nom());
  equation(1).mettre_a_jour(temps); // thermique
  equation(0).mettre_a_jour(temps); // NS

  for(int i=2; i<nombre_d_equations(); i++) // if species ...
    equation(i).mettre_a_jour(temps);

  les_postraitements.mettre_a_jour(temps);
  domaine().mettre_a_jour(temps,domaine_dis(),*this);
  LIST_CURSEUR(REF(Loi_Fermeture_base)) curseur = liste_loi_fermeture_;
  while (curseur)
    {
      Loi_Fermeture_base& loi=curseur.valeur().valeur();
      loi.mettre_a_jour(temps);
      ++curseur;
    }
}
