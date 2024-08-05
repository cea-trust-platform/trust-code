/****************************************************************************
* Copyright (c) 2024, CEA
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
  // first keywords : domain and components
  Nom nom;
  is >> nom;
  dom = ref_cast(Domaine, Interprete::objet(nom));
  IntTrav filled; //to check if subdomains cover the whole domain
  dom->creer_tableau_elements(filled);
  //loop over subdomains
  is >> nom;
  if (nom != "{")
    Process::exit(que_suis_je() + " : { expected instead of " + nom);
  for (is >> nom; nom != "}"; is >> nom)
    {
      if (nom == "default") nom = "defaut";
      if (nom != "defaut")
        {
          const Sous_Domaine& sdom = dom->ss_domaine(nom);
          for (int i = 0; i < sdom.nb_elem_tot(); i++)
            filled(sdom(i)) = 1;
        }
      if (field.count(nom.getString()))
        Process::exit(que_suis_je() + " : the sudomain " + nom + "has been specified twice!");
      is >> field[nom.getString()];
      if (nb_comp < 0)
        nb_comp = field[nom.getString()]->nb_comp();
      else if (field[nom.getString()]->nb_comp() != nb_comp)
        Process::exit(que_suis_je() + " : the field on " + nom + " has " + Nom(field[nom.getString()]->nb_comp()) + " components instead of " + Nom(nb_comp) + " !");
    }
  //either check that there no missing elements or add them to the "default_vals" list
  if (!field.count("defaut") && mp_min_vect(filled) == 0)
    Process::exit(que_suis_je() + " : some pieces of the field are missing!");
  else for (int e = 0; e < dom->nb_elem(); e++)
      if (!filled(e))
        default_vals.append_line(e);
  valeurs_.resize(0, nb_comp);
  noms_compo_.dimensionner(nb_comp);
  dom->creer_tableau_elements(valeurs_);
  return is;
}

void Champ_Morceaux::mettre_a_jour(double temps)
{
  for (auto &&kv : field)
    {
      kv.second->mettre_a_jour(temps);
      const IntVect& elems = kv.first == "defaut" ? default_vals : dom->ss_domaine(kv.first).les_elems();
      DoubleTrav xp, vals(elems.size(), nb_comp);
      kv.second->valeur_aux_elems(xp, elems, vals);
      for (int i = 0; i < elems.size(); i++)
        for (int e = elems(i), n = 0; n < nb_comp; n++)
          valeurs_(e, n) = vals(i, n);
    }
  valeurs_.echange_espace_virtuel();
}

int Champ_Morceaux::initialiser(const double t)
{
  int res = 1;
  for (auto &&kv : field)
    res &= kv.second->initialiser(t);
  return res;
}

double Champ_Morceaux::changer_temps(const double t)
{
  for (auto &&kv : field)
    kv.second->changer_temps(t);
  return t;
}

const Nom& Champ_Morceaux::fixer_unite(const Nom& unit)
{
  for (auto &&kv : field)
    kv.second->fixer_unite(unit);
  return unit;
}

const Nom& Champ_Morceaux::fixer_unite(int i, const Nom& unit)
{
  for (auto &&kv : field)
    kv.second->fixer_unite(i, unit);
  return unit;
}

void Champ_Morceaux::associer_domaine_dis_base(const Domaine_dis_base& zdb)
{
  Champ_Fonc_P0_base::associer_domaine_dis_base(zdb);
  for (auto &&kv : field)
    kv.second->associer_domaine_dis_base(zdb);
}
