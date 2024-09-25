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

#include <LecFicDiffuse_JDD.h>
#include <Postraitement.h>
#include <Sondes.h>

Implemente_instanciable(Sondes, "Sondes|Probes", LIST(OWN_PTR(Sonde)));
// XD sondes listobj nul 1 sonde 0 List of probes.


Sortie& Sondes::printOn(Sortie& s ) const { return s ; }

/*! @brief Lit une liste de sondes a partir d'un flot d'entree Format:
 *
 *     {
 *       [LIRE UNE SONDE AUTANT DE FOIS QUE NECESSAIRE]
 *     }
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 * @throws accolade ouvrante attendue
 */
Entree& Sondes::readOn(Entree& s )
{
  assert(mon_post.non_nul());

  Motcle motlu;
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");

  s >> motlu;

  if (motlu != accolade_ouverte)
    {
      Cerr << "Error while reading the probes in the postprocessing" << finl;
      Cerr << "We expected { to start to read the probes" << finl;
      exit();
    }
  s >> motlu;

  if (motlu == "FICHIER")
    {
      Cerr<<"Error: Sondes { " << motlu << " .... } no more allowed, use Sondes_fichier { " << motlu << " .... }" <<finl;
      Process::exit();
    }
  else if (motlu == accolade_fermee)
    {
      Cerr << "Error while reading the probes in the postprocessing" << finl;
      Cerr << "You have not defined any probe" << finl;
      exit();
    }
  set_noms_champs_postraitables();
  while (motlu != accolade_fermee)
    {
      OWN_PTR(Sonde) une_sonde;
      une_sonde.typer("Sonde");
      une_sonde->nommer(motlu);
      une_sonde->associer_post(mon_post.valeur());
      s >> une_sonde.valeur();
      add(une_sonde);
      s >> motlu;
    }
  Cerr << "End of reading probes " << finl;
  return s;
}

void Sondes::set_noms_champs_postraitables()
{
  // Reconstruit noms_champs_postraitables_
  if (mon_post.non_nul())
    {
      Noms noms;
      mon_post->probleme().get_noms_champs_postraitables(noms);
      noms_champs_postraitables_.dimensionner_force(noms.size());
      for (int i = 0; i < noms.size(); i++)
        noms_champs_postraitables_[i] = noms[i];
    }
}

void Sondes::completer()
{
  set_noms_champs_postraitables();
  for (auto &itr : *this) itr->completer();
}

/*! @brief Effectue le postraitement sur chacune des sondes de la liste.
 *
 */
void Sondes::postraiter()
{
  for (auto &itr : *this) itr->postraiter();

  clear_cache();
}

void Sondes::clear_cache()
{
  sourceList.vide();
  espaceStockageList.vide();
  sourceNoms.reset();
}

REF(Champ_base) Sondes::get_from_cache(REF(Champ_Generique_base)& mon_champ, const Nom& nom_champ_lu_)
{
  mon_champ->fixer_identifiant_appel(nom_champ_lu_);
  int num = sourceNoms.rang(nom_champ_lu_);
  if (num < 0)
    {
      OWN_PTR(Champ_base) espace_stockage;
      const Champ_base& ma_source = mon_champ->get_champ(espace_stockage);
      sourceList.add(ma_source);
      espaceStockageList.add(espace_stockage);
      sourceNoms.add(nom_champ_lu_);
      // Voir Champ_Generique_base pour la definition de l'espace stockage
      return espace_stockage.non_nul() ? espaceStockageList.dernier().valeur() : ma_source;
    }
  else if (espaceStockageList(num).non_nul())
    return espaceStockageList(num).valeur();
  else
    return sourceList(num);
}

/*! @brief Effectue une mise a jour en temps de chacune des sondes de la liste.
 *
 * @param (double temps) le temps de mise a jour
 * @param (double tinit) le temps initial des sondes
 */
void Sondes::mettre_a_jour(double temps, double tinit)
{
  for (auto &itr : *this) itr->mettre_a_jour(temps, tinit);

  clear_cache();
}

/*! @brief Associe un postraitement a la liste des sondes.
 *
 */
void Sondes::associer_post(const Postraitement& post)
{
  mon_post = post;
}

/*! @brief Modifie le parametrage des sondes mobiles
 *
 */
void Sondes::set_update_positions(bool update_positions_)
{
  update_positions=update_positions_;
}

/*! @brief Renvoie le parametrage des sondes mobiles
 *
 */
bool Sondes::get_update_positions()
{
  return update_positions;
}
