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

#include <Terme_Puissance_Thermique_Echange_Impose_Elem_base.h>
#include <Schema_Temps_base.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Zone_VF.h>
#include <Param.h>

Implemente_base(Terme_Puissance_Thermique_Echange_Impose_Elem_base, "Terme_Puissance_Thermique_Echange_Impose_Elem_base", Source_base);

// XD terme_puissance_thermique_echange_impose source_base terme_puissance_thermique_echange_impose 1 Source term to impose thermal power according to formula : P = himp * (T - Text). Where T is the Trust temperature, Text is the outside temperature with which energy is exchanged via an exchange coefficient himp
// XD attr himp field_base himp 0 the exchange coefficient
// XD attr Text field_base Text 0 the outside temperature
// XD attr PID_controler_on_targer_power bloc_lecture PID_controler_on_targer_power 1 PID_controler_on_targer_power bloc with parameters target_power (required), Kp, Ki and Kd (at least one of them should be provided)

Sortie& Terme_Puissance_Thermique_Echange_Impose_Elem_base::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Terme_Puissance_Thermique_Echange_Impose_Elem_base::readOn(Entree& s)
{
  Param param(que_suis_je());
  param.ajouter("himp", &himp_, Param::REQUIRED);
  param.ajouter("Text", &Text_, Param::REQUIRED);
  param.ajouter_non_std("PID_controler_on_targer_power", (this));
  param.lire_avec_accolades_depuis(s);
  set_fichier("Terme_Puissance_Thermique_Echange_Impose");
  set_description("Power (W)");
  Noms col_names;
  col_names.add("Power");
  col_names.add("DT_regul");
  set_col_names(col_names);
  return s;
}

int Terme_Puissance_Thermique_Echange_Impose_Elem_base::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot == "PID_controler_on_targer_power")
    {
      regul_ = 1;
      Nom pow_str, Kp_str = "0", Ki_str = "0", Kd_str = "0";
      Param param("PID_controler_on_targer_power");
      param.ajouter("target_power", &pow_str, Param::REQUIRED);
      param.ajouter("Kp", &Kp_str);
      param.ajouter("Ki", &Ki_str);
      param.ajouter("Kd", &Kd_str);
      param.lire_avec_accolades(is);
      // check that at least one coefficient is read
      if (Kp_str == "0" && Ki_str == "0" && Kd_str == "0")
        {
          Cerr << "PID controler : you should provide at least one coefficient among Kp, Ki and Kd!" << finl;
          Process::exit();
        }
      pow_cible_.setNbVar(1), Kp_.setNbVar(1), Ki_.setNbVar(1), Kd_.setNbVar(1);
      pow_cible_.setString(pow_str), Kp_.setString(Kp_str), Ki_.setString(Ki_str), Kd_.setString(Kd_str);
      pow_cible_.addVar("t"), Kp_.addVar("t"), Ki_.addVar("t"), Kd_.addVar("t");
      pow_cible_.parseString(), Kp_.parseString(), Ki_.parseString(), Kd_.parseString();
      return 1;
    }
  return 1;
}

void Terme_Puissance_Thermique_Echange_Impose_Elem_base::mettre_a_jour(double temps)
{
  const Zone_VF& zone = le_dom.valeur();
  const DoubleVect& volumes = zone.volumes();
  const DoubleTab& himp = himp_.valeur().valeurs();
  const DoubleTab& Text = Text_.valeur().valeurs();
  const DoubleTab& T = equation().inconnue().valeurs();
  int nb_elem = le_dom.valeur().nb_elem(), c_h = himp.dimension(0) == 1, c_T = Text.dimension(0) == 1, n, N = T.line_size();

  bilan().resize(N + 1), bilan() = 0;

  for (int e = 0; e < nb_elem; e++)
    for (n = 0; n < N; n++)
      bilan()(n) += himp(!c_h * e, n) * volumes(e) * (DT_regul_ + Text(!c_T * e, n) - T(e, n));

  if (regul_) pid_process();
  //pour le fichier de suivi : seulement sur le maitre, car Source_base::imprimer() fait une somme sur les procs
  if (!Process::me()) bilan()(N) = DT_regul_;

  himp_.mettre_a_jour(temps);
  Text_.mettre_a_jour(temps);
}

void Terme_Puissance_Thermique_Echange_Impose_Elem_base::pid_process()
{
  const double ptot = mp_somme_vect(bilan()),t = equation().probleme().schema_temps().temps_courant(), dt = equation().probleme().schema_temps().pas_de_temps();
  pow_cible_.setVar(0, t), Ki_.setVar(0, t);
  const double pow_cible = pow_cible_.eval(), Kp = Kp_.eval(), Ki = Ki_.eval(), Kd = Kd_.eval(), error = (ptot - pow_cible) / pow_cible;
  if (dt < equation().probleme().schema_temps().pas_temps_min())
    {
      p_error = error;
      return; // dt vaut 0 a l'initialisation
    }
  DT_regul_ += Kp * error + dt * Ki * error + Kd * (error - p_error) / dt;
  p_error = error;
}

void Terme_Puissance_Thermique_Echange_Impose_Elem_base::associer_domaines(const Zone_dis& zone_dis, const Zone_Cl_dis& zone_Cl_dis)
{
  le_dom = ref_cast(Zone_VF, zone_dis.valeur());
  le_dom_Cl = ref_cast(Zone_Cl_dis_base, zone_Cl_dis.valeur());
}

void Terme_Puissance_Thermique_Echange_Impose_Elem_base::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Zone_VF& zone = le_dom.valeur();
  const DoubleVect& volumes = zone.volumes();
  const DoubleTab& himp = himp_.valeur().valeurs();
  const DoubleTab& Text = Text_.valeur().valeurs();
  const DoubleTab& T = equation().inconnue().valeurs();
  int nb_elem = le_dom.valeur().nb_elem(), c_h = himp.dimension(0) == 1, c_T = Text.dimension(0) == 1, n, N = T.line_size();
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  Matrice_Morse *mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : NULL;

  for (int e = 0; e < nb_elem; e++)
    for (n = 0; n < N; n++)
      {
        secmem(e, n) -= volumes(e) * himp(!c_h * e, n) * (T(e, n) - DT_regul_ - Text(!c_T * e, n));
        if (mat)
          (*mat)(N * e + n, N * e + n) += volumes(e) * himp(!c_h * e, n);
      }
}
