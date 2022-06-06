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

#include <Sondes.h>
#include <LecFicDiffuse_JDD.h>
#include <Postraitement.h>

Implemente_liste(Sonde);
Implemente_instanciable(Sondes,"Sondes|Probes",LIST(Sonde));
Implemente_liste(Champ);

// Description:
//    Lit une liste de sondes a partir d'un flot d'entree
//    Format:
//    {
//      [LIRE UNE SONDE AUTANT DE FOIS QUE NECESSAIRE]
//    }
// Precondition: la liste des sondes doit avoir un postraitement associe
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Effets de bord:
// Postcondition:
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

  bool depuisFichier = false;
  Nom nom_fichier;

  if (motlu == "FICHIER")
    {
      Cerr<<"Warning: Sondes { " << motlu << " .... } no more allowed, use Sondes_fichier { " << motlu << " .... }" <<finl;
      exit();
      depuisFichier = true;
      s >> nom_fichier;
    }
  else if (motlu == accolade_fermee)
    {
      Cerr << "Error while reading the probes in the postprocessing" << finl;
      Cerr << "You have not defined any probe" << finl;
      exit();
    }
  set_noms_champs_postraitables();
  if (depuisFichier)
    {
      lire_fichier(nom_fichier);
      s >> motlu;
      if (motlu != accolade_fermee)
        {
          Cerr << "Error while reading the probes in the postprocessing" << finl;
          Cerr << "We expected } to end to read the probes" << finl;
          exit();
        }
    }
  else
    {
      while (motlu != accolade_fermee)
        {
          Sonde une_sonde(motlu);
          une_sonde.associer_post(mon_post.valeur());
          s >> une_sonde;
          add(une_sonde);
          s >> motlu;
        }
    }
  Cerr << "End of reading probes " << finl;
  return s;
}

void Sondes::lire_fichier(const Nom& nom_fichier)
{
  set_noms_champs_postraitables();
  LecFicDiffuse_JDD f(nom_fichier);
  Motcle motlu;

  if (!f.good())
    {
      Cerr << "Cannot open the file " << nom_fichier << finl;
      Process::exit();
    }

  f >> motlu;
  while (!f.eof())
    {
      Sonde une_sonde(motlu);
      une_sonde.associer_post(mon_post.valeur());
      f >> une_sonde;
      add(une_sonde);
      f >> motlu;
    }
  f.close();
}

// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Sondes::printOn(Sortie& s ) const
{
  return s ;
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
  LIST_CURSEUR(Sonde) curseur=*this;
  while(curseur)
    {
      curseur->completer();
      ++curseur;
    }
}

// Description:
//    Effectue le postraitement sur chacune des sondes de
//    la liste.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Sondes::postraiter()
{
  LIST_CURSEUR(Sonde) curseur=*this;
  while(curseur)
    {
      curseur->postraiter();
      ++curseur;
    }
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
      Champ espace_stockage;
      const Champ_base& ma_source = ref_cast(Champ_base, mon_champ->get_champ(espace_stockage));
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

// Description:
//    Effectue une mise a jour en temps de chacune
//    des sondes de la liste.
// Precondition:
// Parametre: double temps
//    Signification: le temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: double tinit
//    Signification: le temps initial des sondes
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Sondes::mettre_a_jour(double temps, double tinit)
{
  LIST_CURSEUR(Sonde) curseur=*this;
  while(curseur)
    {
      curseur->mettre_a_jour(temps,tinit);
      ++curseur;
    }
  clear_cache();
}


// Description:
//    Associe un postraitement a la liste des sondes.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la liste des sondes a un postraitement associe
void Sondes::associer_post(const Postraitement& post)
{
  mon_post = post;
}

