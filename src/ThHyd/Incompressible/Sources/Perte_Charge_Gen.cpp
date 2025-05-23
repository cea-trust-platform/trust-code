/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Multiplicateur_diphasique_base.h>
#include <EcritureLectureSpecial.h>
#include <Fluide_Incompressible.h>
#include <Perte_Charge_Gen.h>
#include <Pb_Multiphase.h>
#include <Avanc.h>
#include <Param.h>

Implemente_base(Perte_Charge_Gen, "Perte_Charge_Gen", Source_base);

Sortie& Perte_Charge_Gen::printOn(Sortie& s) const { return s << que_suis_je() << endl; }

Entree& Perte_Charge_Gen::readOn(Entree& is)
{
  Param param(que_suis_je());
  Cerr << que_suis_je() << "::readOn " << finl;
  lambda.setNbVar(4 + dimension);
  direction_perte_charge_ = -1;
  regul_ = 0;
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  Cerr << "Interpretation de la fonction " << lambda.getString() << " ... ";
  lambda.parseString();
  Cerr << " Ok" << finl;
  if (diam_hydr->nb_comp() != 1)
    {
      Cerr << "Il faut definir le champ diam_hydr a une composante" << finl;
      exit();
    }
  return is;
}

void Perte_Charge_Gen::set_param(Param& param)
{
  param.ajouter_non_std("lambda", (this), Param::REQUIRED);
  param.ajouter("diam_hydr", &diam_hydr, Param::REQUIRED);
  param.ajouter_non_std("sous_domaine|sous_zone", (this));
  param.ajouter("implicite", &implicite_);
  param.ajouter_non_std("regul", (this));
}

int Perte_Charge_Gen::sauvegarder(Sortie& os) const
{
  int a_faire, special;
  EcritureLectureSpecial::is_ecriture_special(special, a_faire);
  if (a_faire)
    {
      double temps = equation().inconnue().temps();
      Nom ident_k = Nom("K") + equation().probleme().le_nom() + identifiant_ + Nom(temps, "%e");
      os << ident_k << finl;
      os << "constante" << finl;
      os << K_;
      os << flush;
      Cerr << "Saving K at time : " << Nom(temps, "%e")  << " with value " << K_ << finl;
    }
  return 8;//un double
}

int Perte_Charge_Gen::reprendre(Entree& is)
{
  Nom ident_k = Nom("K") + equation().probleme().le_nom() + identifiant_ + Nom(equation().schema_temps().temps_courant(), equation().probleme().reprise_format_temps());
  avancer_fichier(is, ident_k);
  is >> K_;
  Cerr << "Resuming with the value K = " << K_ << finl;
  return 1;
}

int Perte_Charge_Gen::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot == "lambda")
    {
      Nom tmp;
      is >> tmp;
      lambda.setString(tmp);
      lambda.addVar("Re");
      lambda.addVar("t");
      lambda.addVar("x");
      if (dimension > 1)
        lambda.addVar("y");
      if (dimension > 2)
        lambda.addVar("z");
      return 1;
    }
  else if (mot == "sous_domaine")
    {
      is >> nom_sous_domaine;
      sous_domaine = true;
      return 1;
    }
  else if (mot == "regul")
    {
      lire_regul(is);
      num_faces.resize(ref_cast(Domaine_VF, equation().domaine_dis()).nb_elem());
      lire_surfaces(is, equation().domaine_dis().domaine(), equation().domaine_dis(), num_faces, sgn, 0);
      return 1;
    }
  else // non compris
    {
      Cerr << "Mot cle \"" << mot << "\" non compris lors de la lecture d'un " << que_suis_je() << finl;
      Process::exit();
    }
  return -1;
}

void Perte_Charge_Gen::completer()
{
  Source_base::completer();
  if (regul_) //fichier de sortie si regulation
    {
      bilan().resize(3); //K deb cible
      identifiant_ = sous_domaine ? nom_sous_domaine : le_dom_vf_->domaine().le_nom();
      set_fichier(Nom("DP_") + identifiant_);
      set_description(Nom("Regulation du Ksing de la surface ") + identifiant_);
      Noms col_names;
      col_names.add("K");
      col_names.add("Flow_rate");
      col_names.add("Target_Flow_rate");
      set_col_names(col_names);
    }
  if (sous_domaine)
    le_sous_domaine = equation().probleme().domaine().ss_domaine(nom_sous_domaine);
}

void Perte_Charge_Gen::mettre_a_jour(double t)
{
  if (regul_)
    update_K(equation(), calculate_Q(equation(), num_faces, sgn), bilan());
}

void Perte_Charge_Gen::associer_pb(const Probleme_base& pb)
{
  la_vitesse =  equation().inconnue();
  le_fluide = ref_cast(Fluide_base, equation().milieu());
}

void Perte_Charge_Gen::associer_domaines(const Domaine_dis_base& domaine_dis, const Domaine_Cl_dis_base& domaine_Cl_dis)
{
  le_dom_vf_ = ref_cast(Domaine_VF, domaine_dis);
  le_dom_Cl_dis_ = domaine_Cl_dis;
}
