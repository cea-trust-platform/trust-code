/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Operateurs_Statistique_tps.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Statistiques_temps
// Version:     /main/45
//
//////////////////////////////////////////////////////////////////////////////

#include <Operateurs_Statistique_tps.h>
#include <EcritureLectureSpecial.h>
#include <Postraitement.h>
#include <Op_Correlation.h>
#include <Champ_Generique_Statistiques_base.h>

Implemente_liste(Operateur_Statistique_tps);
Implemente_instanciable_sans_constructeur(Operateurs_Statistique_tps,"Operateurs_Statistique_tps",LIST(Operateur_Statistique_tps));

Operateurs_Statistique_tps::Operateurs_Statistique_tps(): tstat_deb_(-123.), tstat_dernier_calcul_(-123.), tstat_fin_(-123.), lserie_(-123),dt_integr_serie_(-123.) {}

// Description:
//    Imprime le type et le nom de la liste d'operateurs
//    sur un flot de sortie.
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Operateurs_Statistique_tps::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

inline const Operateur_Statistique_tps& recherche(Operateurs_Statistique_tps& op, const Nom& champ, const Motcle& motlu)
{
  LIST_CURSEUR(Operateur_Statistique_tps) curseur=op;
  while(curseur)
    {
      if ( curseur->valeur().le_nom() == champ )
        return curseur.valeur();
      ++curseur;
    }
  Cerr << "The post_processing request " << motlu << " of a variable" << finl;
  Cerr << "must be preceded by the post-processing request of the average of the same variable." << finl;
  Process::exit();
  return curseur.valeur();
}

// Description:
//    Lit les specifications d'une liste d'operateurs statistiques a
//    partir d'un flot d'entree.
// Precondition: la liste d'operateurs doit avoir un postraitement associe
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Exception: mot clef ne correspond pas a un champ inconnu du probleme
// Effets de bord:
// Postcondition:
Entree& Operateurs_Statistique_tps::readOn(Entree& s)
{
  return s;
}

int Operateurs_Statistique_tps::sauvegarder(Sortie& os) const
{
  // en mode ecriture special seul le maitre ecrit l'entete
  int a_faire,special;
  EcritureLectureSpecial::is_ecriture_special(special,a_faire);

  if (a_faire)
    {
      Nom mon_ident(que_suis_je());
      mon_ident += mon_post->probleme().domaine().le_nom();
      double temps = mon_post->probleme().schema_temps().temps_courant();
      mon_ident += Nom(temps,"%e");
      os << mon_ident << finl;
      os << que_suis_je() << finl;
      os << size() << finl;
      os << Nom(tstat_deb_,"%e") << finl;
      os << Nom(tstat_dernier_calcul_,"%e") << finl ;
    }
  CONST_LIST_CURSEUR(Operateur_Statistique_tps) curseur=*this;
  int bytes=0;
  while(curseur)
    {
      bytes += curseur->sauvegarder(os);
      ++curseur;
    }
  if (a_faire) os.flush();
  return bytes;
}


int Operateurs_Statistique_tps::reprendre(Entree& is)
{
  //Cerr << "Operateurs_Statistique_tps::reprendre" << finl;
  if (mon_post.non_nul())
    {
      Nom bidon;
      is >> bidon;
      if (bidon=="fin")
        {
          // Ce test evite un beau segmentation fault a la lecture
          // du deuxieme bidon lors d'une sauvegarde/reprise au format binaire
          Cerr << "The end of the restarting file is reached." << finl;
          Cerr << "This file does not contain statistics." << finl;
          Cerr << "The restarting time tinit must therefore be lower" << finl;
          Cerr << "than the statistics begining time t_deb." << finl;
          exit();
        }
      is >> bidon;
      int n;
      is >> n;
      double tstat_deb_sauv,temps_derniere_mise_a_jour_stats;
      is >> tstat_deb_sauv;
      is >> temps_derniere_mise_a_jour_stats;
      //Cerr << "temps_derniere_mise_a_jour_stats" << temps_derniere_mise_a_jour_stats << finl;
      double tinit = mon_post->probleme().schema_temps().temps_courant();

      // Plusieurs cas possibles:
      if (inf_strict(tinit,temps_derniere_mise_a_jour_stats,1.e-5))
        {
          Cerr << "The initial time " << tinit << " of the restarting calculation is lower" << finl;
          Cerr << "than the last statistics update time " << temps_derniere_mise_a_jour_stats << finl;
          Cerr << "One is not able to treat this case." << finl;
          exit();
        }
      else if (!est_egal(tstat_deb_sauv,tstat_deb_))
        {
          // t_deb est modifie : on refait une statistique sans reprendre dans certains cas
          if (inf_strict(tstat_deb_,tinit,1.e-5))
            {
              Cerr << "t_deb has been modified to carry out a new statistics calculation without restarting" << finl;
              Cerr << "but t_deb is lower than tinit : it is not possible." << finl;
              exit();
            }
          Cerr << "Statistics are not restarted and therefore the statistics calculation" << finl;
          Cerr << "will restart a t_deb =" << tstat_deb_ << finl;
          LIST_CURSEUR(Operateur_Statistique_tps) curseur=*this;
          Nom bidon2;
          double dbidon;
          DoubleTab tab_bidon;
          while(curseur)   // On saute les champs
            {
              is >> bidon2 >> bidon2;
              is >> dbidon;
              tab_bidon.reset(); // sinon erreur sur la taille dans lit()
              tab_bidon.jump(is);
              ++curseur;
            }
        }
      else // tinit=>temps_derniere_mise_a_jour_stats : on fait la reprise
        {
          Nom bidon2;
          LIST_CURSEUR(Operateur_Statistique_tps) curseur=*this;
          while(curseur)
            {
              is >> bidon2 >> bidon2; // On saute l'identificateur et le type des champs
              curseur->reprendre(is);
              ++curseur;
            }
          // On modifie l'attribut tstat_deb_ et l'attribut t_debut_ des champs
          // pour tenir compte de la reprise
          tstat_deb_ = tstat_deb_sauv;
          curseur=*this;
          while(curseur)
            {
              curseur->fixer_tstat_deb(tstat_deb_,temps_derniere_mise_a_jour_stats);
              ++curseur;
            }
        }
    }
  else  // lecture pour sauter le bloc
    {
      int n;
      is >> n;
      double tstat_deb_sauv,temps_derniere_mise_a_jour_stats;
      is >> tstat_deb_sauv;
      is >> temps_derniere_mise_a_jour_stats;
      Nom bidon;
      double dbidon;
      DoubleTab tab_bidon;
      for (int i=0; i<n; i++)
        {
          is >> bidon >> bidon;
          is >> dbidon;
          tab_bidon.reset(); // sinon erreur sur la taille dans lit()
          tab_bidon.jump(is);
        }
    }
  return 1;
}
