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

#include <Probleme_base_interface_proto.h>
#include <ICoCoExceptions.h>
#include <Probleme_base.h>
#include <Postraitement.h>
#include <Equation.h>
#include <Debog.h>

using ICoCo::WrongArgument;
using ICoCo::WrongContext;

void Probleme_base_interface_proto::initialize_impl(Probleme_base& pb)
{
  if (initialized)
    throw WrongContext(pb.le_nom().getChar(), "initialize", "initialize should not have been called");

  Debog::set_nom_pb_actuel(pb.le_nom());
  pb.preparer_calcul();
  pb.domaine().initialiser(0, pb.domaine_dis(), pb); // Pour le cas de geometries variables (ex. ALE)
  // on initialise le schema en temps avant le postraitement
  // ainsi les sources qui dependent du pas de temps fonctionnent
  pb.schema_temps().initialize();
  pb.init_postraitements();

  initialized = true;
}

double Probleme_base_interface_proto::presentTime_impl(const Probleme_base& pb) const
{
  if (!initialized)
    throw WrongContext(pb.le_nom().getChar(), "presentTime", "initialize should have been called");
  if (terminated)
    throw WrongContext(pb.le_nom().getChar(), "presentTime", "terminate should not have been called");

  Debog::set_nom_pb_actuel(pb.le_nom());
  return pb.schema_temps().temps_courant();
}

bool Probleme_base_interface_proto::initTimeStep_impl(Probleme_base& pb, double dt)
{
  if (!initialized)
    throw WrongContext(pb.le_nom().getChar(), "initTimeStep", "initialize should have been called");
  if (terminated)
    throw WrongContext(pb.le_nom().getChar(), "initTimeStep", "terminate should not have been called");
  if (dt_defined)
    throw WrongContext(pb.le_nom().getChar(), "initTimeStep", "initTimeStep should not have been called");
  if (dt <= 0)
    throw WrongArgument(pb.le_nom().getChar(), "initTimeStep", "dt", "dt should be a positive real number");

  Debog::set_nom_pb_actuel(pb.le_nom());

  bool ok = pb.schema_temps().initTimeStep(dt);
  pb.domaine().set_dt(dt);
  pb.milieu().initTimeStep(dt);
  for (int i = 0; i < pb.nombre_d_equations(); i++)
    ok = ok && pb.equation(i).initTimeStep(dt);

  // Calculs coeffs echange sur l'instant sur lequel doivent agir les operateurs.
  double tps = pb.schema_temps().temps_defaut();
  for (int i = 0; i < pb.nombre_d_equations(); i++)
    pb.equation(i).domaine_Cl_dis()->calculer_coeffs_echange(tps);

  dt_validated = false;
  dt_defined = true;
  return ok;
}

bool Probleme_base_interface_proto::solveTimeStep_impl(Probleme_base& pb)
{
  if (!dt_defined)
    throw WrongContext(pb.le_nom().getChar(), "solveTimeStep", "initTimeStep should have been called");

  Debog::set_nom_pb_actuel(pb.le_nom());

  bool ok = pb.solveTimeStep_pbU(); // call mother's method

  // Calculs coeffs echange sur l'instant sur lequel doivent agir les operateurs.
  double tps = pb.schema_temps().temps_defaut();
  for (int i = 0; i < pb.nombre_d_equations(); i++)
    pb.equation(i).domaine_Cl_dis()->calculer_coeffs_echange(tps);

  return ok;
}

bool Probleme_base_interface_proto::iterateTimeStep_impl(Probleme_base& pb, bool& converged)
{
  if (!dt_defined)
    throw WrongContext(pb.le_nom().getChar(), "iterateTimeStep", "initTimeStep should have been called");

  // Update the domain
  pb.domaine().mettre_a_jour(pb.schema_temps().temps_defaut(),pb.domaine_dis(),pb);

  Debog::set_nom_pb_actuel(pb.le_nom());
  bool ok = pb.schema_temps().iterateTimeStep(converged);

  // Calculs coeffs echange sur l'instant sur lequel doivent agir les operateurs.
  double tps = pb.schema_temps().temps_defaut();
  for (int i = 0; i < pb.nombre_d_equations(); i++)
    pb.equation(i).domaine_Cl_dis()->calculer_coeffs_echange(tps);

  return ok;
}

void Probleme_base_interface_proto::validateTimeStep_impl(Probleme_base& pb)
{
  if (!dt_defined)
    throw WrongContext(pb.le_nom().getChar(), "validateTimeStep", "initTimeStep should have been called");

  Debog::set_nom_pb_actuel(pb.le_nom());
  pb.schema_temps().validateTimeStep();
  pb.allocation();

  dt_defined = false;
  dt_validated = true;
}

void Probleme_base_interface_proto::terminate_impl(Probleme_base& pb)
{
  if (!initialized)
    throw WrongContext(pb.le_nom().getChar(), "terminate", "initialize should have been called");
  if (terminated)
    throw WrongContext(pb.le_nom().getChar(), "terminate", "terminate should not have been called");
  if (dt_defined)
    throw WrongContext(pb.le_nom().getChar(), "terminate", "time step should be validated or aborted first");

  Debog::set_nom_pb_actuel(pb.le_nom());
  pb.finir();
  pb.schema_temps().terminate();

  terminated = true;
}

double Probleme_base_interface_proto::computeTimeStep_impl(const Probleme_base& pb, bool& stop) const
{
  if (!initialized)
    throw WrongContext(pb.le_nom().getChar(), "computeTimeStep", "initialize should have been called");
  if (terminated)
    throw WrongContext(pb.le_nom().getChar(), "computeTimeStep", "terminate should not have been called");

  Debog::set_nom_pb_actuel(pb.le_nom());
  double dt = pb.schema_temps().computeTimeStep(stop);
  return dt;
}

bool Probleme_base_interface_proto::isStationary_impl(const Probleme_base& pb) const
{
  if (!dt_validated)
    throw WrongContext(pb.le_nom().getChar(), "isStationary", "validateTimeStep should have been called");
  if (terminated)
    throw WrongContext(pb.le_nom().getChar(), "isStationary", "terminate should not have been called");

  return pb.schema_temps().isStationary();
}

void Probleme_base_interface_proto::abortTimeStep_impl(Probleme_base& pb)
{
  if (!dt_defined)
    throw WrongContext(pb.le_nom().getChar(), "abortTimeStep", "initTimeStep should have been called");

  pb.schema_temps().abortTimeStep();
  pb.milieu().abortTimeStep();
  dt_defined = false;
}

bool Probleme_base_interface_proto::updateGivenFields_impl(Probleme_base& pb)
{
  Debog::set_nom_pb_actuel(pb.le_nom());
  bool ok = true;
  for (int i = 0; i < pb.nombre_d_equations(); i++)
    ok = ok && pb.equation(i).updateGivenFields();
  return ok;
}

// Adds the input fields names this pb understands to noms
void Probleme_base_interface_proto::getInputFieldsNames_impl(const Probleme_base& pb,Noms& noms) const
{
  int n = input_fields.size();
  Noms nouveaux_noms(n);
  for (int i = 0; i < n; i++)
    nouveaux_noms[i] = input_fields[i]->le_nom();
  // GF pour contourner un bug dans add() si vecteur null
  if (n)
    noms.add(nouveaux_noms);
}

// Adds the input fields names this pb understands to noms
void Probleme_base_interface_proto::getOutputFieldsNames_impl(const Probleme_base& pb, Noms& noms) const
{
  for (int i = 0; i < pb.postraitements().size(); i++)
    {
      if (sub_type(Postraitement, pb.postraitements()(i).valeur()))
        {
          const Liste_Champ_Generique& liste_champ = ref_cast(Postraitement,pb.postraitements()(i).valeur()).champs_post_complet();
          for (int ii = 0; ii < liste_champ.size(); ii++)
            noms.add(liste_champ(ii).valeur().get_nom_post());
        }
    }
}

REF(Field_base) Probleme_base_interface_proto::findInputField_impl(const Probleme_base& pb, const Nom& name) const
{
  // WEC : there should be a better way to scan the list
  for (int i = 0; i < input_fields.size(); i++)
    if (input_fields[i]->le_nom() == name) return input_fields[i];

  REF(Field_base) ch;
  return ch;
}

REF(Champ_Generique_base) Probleme_base_interface_proto::findOutputField_impl(const Probleme_base& pb, const Nom& name) const
{
  REF(Champ_Generique_base) ch;
  if (pb.comprend_champ_post(name)) ch = pb.get_champ_post(name);
  return ch;
}

void Probleme_base_interface_proto::addInputField_impl(Probleme_base& pb, Field_base& f)
{
  const Nom& name = f.le_nom();
  for (int i = 0; i < input_fields.size(); i++)
    {
      if (input_fields[i]->le_nom() == name)
        throw WrongContext(name.getChar(), "addInputField", "Field with this name is already in list");
    }
  input_fields.add(f);
}

double Probleme_base_interface_proto::futureTime_impl(const Probleme_base& pb) const
{
  if (!dt_defined)
    throw WrongContext(pb.le_nom().getChar(), "futureTime", "initTimeStep should have been called");
  int i = pb.schema_temps().nb_valeurs_futures();
  return pb.schema_temps().temps_futur(i);
}

