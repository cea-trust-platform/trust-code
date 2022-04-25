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
// File:        Pb_Dilatable_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Common/Problems
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Schema_Euler_explicite.h>
#include <Schema_Euler_Implicite.h>
#include <Fluide_Dilatable_base.h>
#include <Schema_RK_Williamson.h>
#include <Loi_Fermeture_base.h>
#include <Pb_Dilatable_base.h>
#include <Probleme_Couple.h>
#include <Equation_base.h>
#include <stat_counters.h>
#include <Pred_Cor.h>
#include <Domaine.h>
#include <Debog.h>
#include <RRK2.h>

Implemente_base(Pb_Dilatable_base,"Pb_Dilatable_base",Pb_Fluide_base);

Sortie& Pb_Dilatable_base::printOn(Sortie& os) const { return Pb_Fluide_base::printOn(os); }

Entree& Pb_Dilatable_base::readOn(Entree& is) { return Pb_Fluide_base::readOn(is); }

void Pb_Dilatable_base::associer_milieu_base(const Milieu_base& mil)
{
  Pb_Fluide_base::associer_milieu_base(mil);
  le_fluide_ =  ref_cast(Fluide_Dilatable_base,mil);
}

void Pb_Dilatable_base::associer_sch_tps_base(const Schema_Temps_base& sch)
{
  if (!sub_type(Schema_Euler_explicite,sch) && !sub_type(Schema_Euler_Implicite,sch)
      && !sub_type(RRK2,sch) && !sub_type(RK2,sch) && !sub_type(RK3,sch) && !sub_type(RK4,sch))
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
  Pb_Fluide_base::associer_sch_tps_base(sch);
}

void Pb_Dilatable_base::preparer_calcul()
{
  Fluide_Dilatable_base& le_fluide = ref_cast(Fluide_Dilatable_base,milieu());
  if (le_fluide.type_fluide()=="Gaz_Reel") equation(1).inconnue()->nommer("temperature");
  if (le_fluide.type_fluide()=="Melange_Binaire") equation(1).inconnue()->nommer("fraction_massique");

  le_fluide.completer(*this);
  le_fluide.preparer_calcul();
  Pb_Fluide_base::preparer_calcul();
  le_fluide.calculer_masse_volumique();
  le_fluide.preparer_calcul();
}

bool Pb_Dilatable_base::initTimeStep(double dt)
{
  bool ok = Pb_Fluide_base::initTimeStep(dt);
  le_fluide_->preparer_pas_temps();
  return ok;
}

void Pb_Dilatable_base::mettre_a_jour(double temps)
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

bool Pb_Dilatable_base::iterateTimeStep(bool& converged)
{
  Debog::set_nom_pb_actuel(le_nom());
  Schema_Temps_base& sch=schema_temps();
  double temps_present=sch.temps_courant();
  double temps_futur=temps_present+sch.pas_de_temps();

  //1. Solve all the equations except the first one (Navier Stokes, solved later at //6)
  for (int i=1; i<nombre_d_equations(); i++)
    {
      sch.faire_un_pas_de_temps_eqn_base(equation(i));
      statistiques().begin_count(mettre_a_jour_counter_);
      equation(i).milieu().mettre_a_jour(temps_futur);
      equation(i).inconnue().mettre_a_jour(temps_futur);
      statistiques().end_count(mettre_a_jour_counter_);
    }

  statistiques().begin_count(mettre_a_jour_counter_);

  //2. Compute temperature-dependent coefficients
  le_fluide_->calculer_coeff_T();

  //3. Compute volumic mass (update Cp)
  le_fluide_->calculer_masse_volumique();

  //4. Solve EDO equation (pressure) if needed (ie. QC)
  solve_pressure_thermo();

  //5. Compute volumic mass
  le_fluide_->calculer_masse_volumique();
  statistiques().end_count(mettre_a_jour_counter_);

  //6. Solve Navier Stokes equation
  sch.faire_un_pas_de_temps_eqn_base(equation(0));
  statistiques().begin_count(mettre_a_jour_counter_);
  equation(0).milieu().mettre_a_jour(temps_futur);
  equation(0).inconnue().mettre_a_jour(temps_futur);
  statistiques().end_count(mettre_a_jour_counter_);

  // Update pressure fields (total/thermo/hydro) if necessary
  update_pressure_fields(temps_futur);

  // on recule les inconnues (le pb mettra a jour les equations)
  for (int i=0; i<nombre_d_equations(); i++)
    equation(i).inconnue().reculer();

  // Calculs coeffs echange sur l'instant sur lequel doivent agir les operateurs.
  double tps=schema_temps().temps_defaut();
  for(int i=0; i<nombre_d_equations(); i++)
    equation(i).zone_Cl_dis()->calculer_coeffs_echange(tps);

  converged=true;
  return true;
}

void Pb_Dilatable_base::update_pressure_fields(double t)
{
  le_fluide_->update_pressure_fields(t);
}
