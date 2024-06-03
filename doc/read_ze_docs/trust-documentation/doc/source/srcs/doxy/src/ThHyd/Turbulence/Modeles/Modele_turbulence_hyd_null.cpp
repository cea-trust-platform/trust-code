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

#include <Modele_turbulence_hyd_null.h>
#include <EcritureLectureSpecial.h>
#include <Discretisation_base.h>
#include <Schema_Temps_base.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Domaine.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Modele_turbulence_hyd_null, "Modele_turbulence_hyd_null", Modele_turbulence_hyd_base);
// XD modele_turbulence_hyd_nul modele_turbulence_hyd_deriv null -1 Nul turbulence model (turbulent viscosity = 0) which can be used with a turbulent problem.

Modele_turbulence_hyd_null::Modele_turbulence_hyd_null()
{
  declare_support_masse_volumique(1);
}

Entree& Modele_turbulence_hyd_null::readOn(Entree& is)
{
  // Creation d'une loi de paroi nulle:
  const Equation_base& eqn = equation();
  const Nom& discr = eqn.discretisation().que_suis_je();
  loipar.associer_modele(*this);
  if (discr == "VEF" || discr == "VEFPreP1B") loipar.typer("negligeable_VEF");
  else if (discr == "VDF") loipar.typer("negligeable_VDF");
  else if (discr == "EF") loipar.typer("negligeable_EF");
  else
    {
      Cerr << "Erreur dans Modele_turbulence_hyd_null::readOn\n";
      Cerr << " la discretisation " << discr << " n'est pas prise en charge";
      Cerr << finl;
      exit();
    }
  loipar.valeur().associer_modele(*this);
  loipar.valeur().associer(eqn.domaine_dis(), eqn.domaine_Cl_dis());

  // Pas envie de debugger XDATA ... je penalise tt le monde alors
  Param param(que_suis_je());
  param.lire_avec_accolades_depuis(is);
  return is;
}

Sortie& Modele_turbulence_hyd_null::printOn(Sortie& os) const
{
  return os << que_suis_je() << " " << le_nom();
}

int Modele_turbulence_hyd_null::sauvegarder(Sortie& os) const
{
  // en mode ecriture special seul le maitre ecrit l'entete
  int write, special;
  EcritureLectureSpecial::is_ecriture_special(special, write);
  if (write)
    {
      Nom mon_ident(que_suis_je());
      mon_ident += equation().probleme().domaine().le_nom();
      double temps = equation().schema_temps().temps_courant();
      mon_ident += Nom(temps, "%e");
      os << mon_ident << finl;
      os << que_suis_je() << finl;
      os.flush();
    }
  return 0;
}
