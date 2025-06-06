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

#include <Schema_Temps_base.h>
#include <Champ_Don_base.h>
#include <Probleme_base.h>
#include <Op_Moyenne.h>

Implemente_instanciable(Op_Moyenne, "Op_Moyenne", Operateur_Statistique_tps_base);

Sortie& Op_Moyenne::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Op_Moyenne::readOn(Entree& s)
{
  return s;
}

Entree& Op_Moyenne::lire_ch_moyenne(Entree& is)
{
  return is >> ch_moyenne_convergee_;
}

void Op_Moyenne::completer(const Probleme_base& Pb, const Nom& prefix)
{
  Nom nom_pour_post = "Moyenne_";

  const OBS_PTR(Champ_Generique_base) &mon_champ = integrale_champ_.le_champ();
  const Noms noms = mon_champ->get_property("nom");
  nom_pour_post += Motcle(noms[0]);

  integrale_champ_.le_champ_calcule().nommer(nom_pour_post);

  Nom pdi_name = prefix + nom_pour_post;
  integrale_champ_.le_champ_calcule().set_pdi_name(pdi_name);

  // Dimensionnement du champ integrale_champ a la meme taille que mon_champ
  OWN_PTR(Champ_base) espace_stockage_source;
  const Champ_base& source = mon_champ->get_champ(espace_stockage_source);
  const DoubleTab& tab1 = source.valeurs();

  // La methode valeurs pour cette classe est surchargee et renvoie
  // integrale_champ->valeurs(). Il n'y a pas de stockage en memoire
  // de la moyenne. On stocke l'integrale en temps et on postraite la moyenne
  // Copie de la structure du vecteur, initialisation a zero.
  valeurs().copy(tab1, RESIZE_OPTIONS::NOCOPY_NOINIT);
  if (!ch_moyenne_convergee_.non_nul())
    valeurs() = 0.;
  int nb_comp = source.nb_comp();
  integrale_champ_.le_champ_calcule().associer_domaine_dis_base(Pb.domaine_dis());
  integrale_champ_.le_champ_calcule().fixer_nb_comp(nb_comp);
  if (nb_comp == 1)
    integrale_champ_.le_champ_calcule().fixer_unite(source.unite());
  else
    integrale_champ_.le_champ_calcule().fixer_unites(source.unites());

  integrale_champ_.le_champ_calcule().changer_temps(Pb.schema_temps().temps_courant());
}

DoubleTab Op_Moyenne::calculer_valeurs() const
{
  double dt = dt_integration();
  DoubleTrav moyenne(valeurs());
  moyenne = valeurs();
  if (dt > 0)
    moyenne /= dt;
  return moyenne;
}
