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

#include <Champ_Morceaux.h>
#include <Discretisation_base.h>
#include <Champ_Fonc_MED.h>
#include <Champ_Uniforme.h>
#include <Probleme_base.h>
#include <Domaine_dis_base.h>
#include <Param.h>

Implemente_instanciable(Champ_Morceaux,"Champ_Morceaux",Champ_Fonc_P0_base);
// XD champ_morceaux champ_fonc_p0_base champ_morceaux 0 Field defined by subfields on a list of subdomains. Each field must be defined on the whole domain.
// XD   attr problem_name ref_Pb_base problem_name 0 Name of the problem.
// XD   attr data bloc_lecture data 0 { Defaut field_def sous_domaine_1 field_1 ... sous_domaine_i field_i } By default, the field field_def is assigned to the field. It takes the sous_domaine_i identifier Sous_Domaine (sub_area) type object field, field_i. Sous_Domaine (sub_area) type objects must have been previously defined if the operator wishes to use a champ_morceaux type object.

Sortie& Champ_Morceaux::printOn(Sortie& os) const { return os; }

Entree& Champ_Morceaux::readOn(Entree& is)
{
  nb_compo_ = -1;
  // first keywords : domain and components
  Nom nom;
  is >> nom;
  pb_ = ref_cast(Probleme_base, Interprete::objet(nom));
  associer_domaine_dis_base(pb_->domaine_dis());
  IntTrav filled; //to check if subdomains cover the whole domain
  pb_->domaine().creer_tableau_elements(filled);
  //loop over subdomains
  is >> nom;
  if (nom != "{")
    Process::exit(que_suis_je() + " : { expected instead of " + nom);
  for (is >> nom; nom != "}"; is >> nom)
    {
      if (nom == "default") nom = "defaut";
      if (nom != "defaut")
        {
          const Sous_Domaine& sdom = pb_->domaine().ss_domaine(nom);
          for (int i = 0; i < sdom.nb_elem_tot(); i++)
            filled(sdom(i)) = 1;
        }
      if (field_.count(nom.getString()))
        Process::exit(que_suis_je() + " : the sudomain " + nom + "has been specified twice!");
      is >> field_[nom.getString()];
      if (nb_compo_ < 0)
        nb_compo_ = field_[nom.getString()]->nb_comp();
      else if (field_[nom.getString()]->nb_comp() != nb_compo_)
        Process::exit(que_suis_je() + " : the field on " + nom + " has " + Nom(field_[nom.getString()]->nb_comp()) + " components instead of " + Nom(nb_compo_) + " !");
    }
  //either check that there no missing elements or add them to the "default_vals" list
  if (!field_.count("defaut") && mp_min_vect(filled) == 0)
    Process::exit(que_suis_je() + " : some pieces of the field are missing!");
  else for (int e = 0; e < pb_->domaine().nb_elem(); e++)
      if (!filled(e))
        default_vals_.append_line(e);
  valeurs_.resize(0, nb_compo_);
  noms_compo_.dimensionner(nb_compo_);
  pb_->domaine().creer_tableau_elements(valeurs_);

  // store xp in each sub-zone (some fields need positions for valeur_aux, ex : champ_fonc_xyz)
  const DoubleTab& xp_dom = ref_cast(Domaine_VF, pb_->domaine_dis()).xp();
  for (auto &&kv : field_)
    {
      const IntVect& elems = kv.first == "defaut" ? default_vals_ : pb_->domaine().ss_domaine(kv.first).les_elems();
      xp_ssz_[kv.first].resize(elems.size(), dimension);
      for (int i = 0; i < elems.size(); i++)
        for (int d = 0; d < dimension; d++)
          xp_ssz_[kv.first](i, d) = xp_dom(elems(i), d);
    }

  return is;
}

void Champ_Morceaux::mettre_a_jour(double temps)
{
  for (auto &&kv : field_)
    {
      kv.second->mettre_a_jour(temps);
      const IntVect& elems = kv.first == "defaut" ? default_vals_ : pb_->domaine().ss_domaine(kv.first).les_elems();
      DoubleTrav vals(elems.size(), nb_compo_);

      kv.second->valeur_aux_elems(xp_ssz_[kv.first], elems, vals);
      for (int i = 0; i < elems.size(); i++)
        for (int e = elems(i), n = 0; n < nb_compo_; n++)
          valeurs_(e, n) = vals(i, n);
    }
  valeurs_.echange_espace_virtuel();
}

int Champ_Morceaux::initialiser(const double t)
{
  int res = 1;
  for (auto &&kv : field_)
    res &= kv.second->initialiser(t);
  mettre_a_jour(t);
  return res;
}

double Champ_Morceaux::changer_temps(const double t)
{
  for (auto &&kv : field_)
    kv.second->changer_temps(t);
  return t;
}

const Nom& Champ_Morceaux::fixer_unite(const Nom& unit)
{
  for (auto &&kv : field_)
    kv.second->fixer_unite(unit);
  return unit;
}

const Nom& Champ_Morceaux::fixer_unite(int i, const Nom& unit)
{
  for (auto &&kv : field_)
    kv.second->fixer_unite(i, unit);
  return unit;
}

void Champ_Morceaux::associer_domaine_dis_base(const Domaine_dis_base& zdb)
{
  Champ_Fonc_P0_base::associer_domaine_dis_base(zdb);
  for (auto &&kv : field_)
    kv.second->associer_domaine_dis_base(zdb);
}
