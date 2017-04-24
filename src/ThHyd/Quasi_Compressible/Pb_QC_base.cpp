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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Pb_QC_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Pb_QC_base.h>
#include <Equation_base.h>
#include <Fluide_Quasi_Compressible.h>
#include <Schema_Euler_explicite.h>
#include <Pred_Cor.h>
#include <RRK2.h>
#include <RK3.h>
#include <Debog.h>
#include <Schema_Euler_Implicite.h>
#include <Domaine.h>
#include <Loi_Fermeture_base.h>
#include <Probleme_Couple.h>

Implemente_base(Pb_QC_base,"Pb_QC_base",Pb_qdm_fluide);

Sortie& Pb_QC_base::printOn(Sortie& os) const
{
  return Pb_qdm_fluide::printOn(os);
}


Entree& Pb_QC_base::readOn(Entree& is)
{
  return Pb_qdm_fluide::readOn(is);
}


void Pb_QC_base::associer_milieu_base(const Milieu_base& mil)
{
  if (sub_type(Fluide_Quasi_Compressible,mil))
    {
      Pb_qdm_fluide::associer_milieu_base(mil);
    }
  else
    {
      Cerr << "Un milieu de type " << mil.que_suis_je()
           << " ne peut etre associe a "<< finl
           << "un probleme quasi-compressible" << finl;
      exit();
    }
}


void Pb_QC_base::associer_sch_tps_base(const Schema_Temps_base& sch)
{
  if (!sub_type(Schema_Euler_explicite,sch)
      && !sub_type(Schema_Euler_Implicite,sch)
      && !sub_type(RRK2,sch)
      && !sub_type(RK3,sch))
    {
      Cerr << finl;
      Cerr << "TRUST can't solve a " << que_suis_je() << finl;
      Cerr << "with a " << sch.que_suis_je() << " time scheme." << finl;
      exit();
    }
  if ( sub_type(Schema_Euler_Implicite,sch) && coupled_==1 )
    {
      Cerr << finl;
      Cerr << "Coupled problem with unique Euler implicit time scheme with quasi-compressible fluid are not allowed!" << finl;
      Cerr << "Choose another time scheme or set a time scheme for each of your problems." << finl;
      exit();
    }
  Pb_qdm_fluide::associer_sch_tps_base(sch);
}


void Pb_QC_base::preparer_calcul()
{
  // WEC : Attention le double appel a le_fluide.preparer_calcul() est necessaire !
  // (je ne sais pas pourquoi, vu sur le cas test Champs_fonc_QC_rayo_semi_transp)
  Fluide_Quasi_Compressible& le_fluide = ref_cast(Fluide_Quasi_Compressible,milieu());

  //Le nom sera a nouveau modifie dans Loi_Etat_GR_rhoT::initialiser()
  if (le_fluide.type_fluide()=="Gaz_Reel")
    {
      equation(1).inconnue()->nommer("temperature");
    }

  le_fluide.completer(*this);
  le_fluide.preparer_calcul();
  Pb_qdm_fluide::preparer_calcul();
  le_fluide.calculer_masse_volumique();
  le_fluide.preparer_calcul();
  // encore utile ?
  le_fluide.calculer_masse_volumique();
  le_fluide.preparer_calcul();
}


bool Pb_QC_base::initTimeStep(double dt)
{
  bool ok=Pb_qdm_fluide::initTimeStep(dt);
  Fluide_Quasi_Compressible& le_fluide = ref_cast(Fluide_Quasi_Compressible,milieu());
  le_fluide.preparer_pas_temps();
  return ok;
}


void Pb_QC_base::mettre_a_jour(double temps)
{
  Debog::set_nom_pb_actuel(le_nom());
  equation(1).mettre_a_jour(temps);
  //   Schema_Temps_base& sch=schema_temps();

  // GF correction pour mettre a jour rho dans le cas ou l'on ne passe pas par
  // iterateTimeStep
  // WEC : ?? On y passe a tous les coups dans iterateTimeStep !!

  //   if (sub_type(Schema_Euler_Implicite,sch))
  //   {
  //     Fluide_Quasi_Compressible& le_fluide = ref_cast(Fluide_Quasi_Compressible,milieu());
  //     le_fluide.calculer_coeff_T();
  //     le_fluide.Resoudre_EDO_PT();
  //     le_fluide.calculer_masse_volumique();
  //     le_fluide.preparer_pas_temps();
  //   }

  equation(0).mettre_a_jour(temps);
  for(int i=2; i<nombre_d_equations(); i++)
    {
      equation(i).mettre_a_jour(temps);
    }

  // WEC : si ca pouvait venir ici, ce serait factorise avec Pb_base
  //   for(int i=0; i<nombre_d_equations(); i++)
  //     equation(i).mettre_a_jour(temps);
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

bool Pb_QC_base::iterateTimeStep(bool& converged)
{
  Debog::set_nom_pb_actuel(le_nom());

  Schema_Temps_base& sch=schema_temps();
  double temps_present=sch.temps_courant();
  double temps_futur=temps_present+sch.pas_de_temps();

  //1. Solve all the equations except the first one (Navier Stokes, solved later at //6)
  for (int i=1; i<nombre_d_equations(); i++)
    {
      sch.faire_un_pas_de_temps_eqn_base(equation(i));
      equation(i).milieu().mettre_a_jour(temps_futur);
      equation(i).inconnue().mettre_a_jour(temps_futur);
    }

  Fluide_Quasi_Compressible& le_fluide = ref_cast(Fluide_Quasi_Compressible,milieu());
  //2. Compute temperature-dependent coefficients
  le_fluide.calculer_coeff_T();

  //3. Compute volumic mass (update Cp)
  le_fluide.calculer_masse_volumique();

  //4. Solve EDO equation (pressure)
  le_fluide.Resoudre_EDO_PT();

  //5. Compute volumic mass
  le_fluide.calculer_masse_volumique();

  //6. Solve Navier Stokes equation
  sch.faire_un_pas_de_temps_eqn_base(equation(0));
  equation(0).milieu().mettre_a_jour(temps_futur);
  equation(0).inconnue().mettre_a_jour(temps_futur);

  for (int i=0; i<nombre_d_equations(); i++)
    {
      // on recule les inconnues (le pb mettra a jour les equations)
      equation(i).inconnue().reculer();
    }
  //FIN REMPLACEMENT PAR LE SCHEMA DE RESOLUTION PARTICULIER AU QUASI COMPRESSIBLE

  // Calculs coeffs echange sur l'instant sur lequel doivent agir les operateurs.
  double tps=schema_temps().temps_defaut();
  for(int i=0; i<nombre_d_equations(); i++)
    equation(i).zone_Cl_dis()->calculer_coeffs_echange(tps);

  converged=true;
  return true;
}
