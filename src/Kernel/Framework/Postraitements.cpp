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

#include <Postraitements.h>
#include <Postraitement_base.h>
#include <List_Nom.h>
#include <Postraitement.h>
Implemente_deriv(Postraitement_base);
Implemente_liste(DERIV(Postraitement_base));
Implemente_instanciable(Postraitements,"Postraitements|Post_processings",LIST(DERIV(Postraitement_base)));

Entree& Postraitements::readOn(Entree& s)
{
  Cerr << "Postraitements::readOn should not be used" << finl;
  exit();
  return s;
}

Sortie& Postraitements::printOn(Sortie& s) const
{
  CONST_LIST_CURSEUR(DERIV(Postraitement_base)) curseur = *this;
  while(curseur)
    {
      const Postraitement_base& post = curseur.valeur().valeur();
      s << post;
      ++curseur;
    }
  return s;
}

// Lecture des postraitements dans le jeu de donnees TRUST
// Au sein du probleme, la syntaxe est la suivante:
//
// ------------- Syntaxe 1 : Un postraitement standard
//                           (syntaxe historique)
// Lire pb {
//   ... (les equations)
//   ...
//   Postraitement {
//     ...            (lecture au format readOn de Postraitement_std)
//   }
//
// ------------- Syntaxe 2 : Plusieurs postraitements standard
//                           (syntaxe historique)
// Lire pb {
//   ... (les equations)
//   ...
//   Postraitements {
//     nom_postraitement_1 {                   note: nom arbitraire
//       ...            (lecture au format readOn de Postraitement_std)
//     }
//     nom_postraitement_2 {
//       ...            (lecture au format readOn de Postraitement_std)
//     }
//     ...
//   }
// ------------- Syntaxe 3 : Postraitements standard ou non standard
//               (pour utiliser le postraitement standard, utiliser
//                type_postraitement = "Postraitement_std")
//
// Lire pb {
//   ... (les equations)
//   ...
//   Liste_Postraitements {
//     type_postraitement nom_postraitement {
//                      (type_postraitement est un le nom d'un type derive
//                       de Postraitement_base, par ex Postraitement)
//       ...            (lecture au format readOn du type demande)
//     }
//     type_postraitement nom_postraitement {
//       ...            (lecture au format readOn du type demande)
//     }
//     ...
//   }
//
// La lecture commence par celle du mot cle Postraitement,
// Postraitements ou Liste_postraitement, et on lit jusqu'a la
// derniere accolade incluse.

// Renvoie 1 si le motlu est compris (lecture d'un bloc de postraitement),
//         0 sinon.

int Postraitements::lire_postraitements(Entree& is, const Motcle& motlu,
                                        const Probleme_base& mon_pb)
{
  Motcles motcles(4);
  motcles[0] = "Postraitement|Post_processing";
  motcles[1] = "Postraitements|Post_processings";
  motcles[2] = "liste_postraitements";
  motcles[3] = "liste_de_postraitements";

  int lerang = motcles.search(motlu);

  // a cause du cas Liste_Postraitements (rang=2)
  //=> obligation de creer la liste de nom de fichier associes au post
  LIST(Nom) list_nom_post;
  int compteur = 0 ;

  if (lerang == 0)
    {
      // Creation et lecture d'un postraitement unique standard
      DERIV(Postraitement_base) & post = add( DERIV(Postraitement_base)() );
      post.typer("Postraitement");
      post.valeur().associer_nom_et_pb_base("neant", mon_pb);
      is >> post.valeur();
    }
  else if (lerang == 1 || lerang == 2 || lerang == 3 )
    {
      // Lecture d'une liste
      // Lire l'accolade
      Nom post_which_contains_statistic("");
      Motcle motlu2;
      is >> motlu2;
      if (motlu2 != "{")
        {
          Cerr << "Error while reading the list of Postraitements," << finl;
          Cerr << " we expected a {" << finl;
          exit();
        }
      is >> motlu2;
      while (motlu2 != "}")
        {
          Motcle type, nom_du_post;
          switch (lerang)
            {
            case 1:
              {
                type = "Postraitement";
                nom_du_post = motlu2;
                break;
              }
            case 2:
              {
                Cerr<<" Warning liste_postraitements obsolete option" <<finl;
                // Le premier mot est le type, le deuxieme est le nom
                type = motlu2;
                is >> nom_du_post;
                break;
              }
            case 3:
              {
                nom_du_post = motlu2;
                is >> type ;
                break;
              }
            default:
              {
                Cerr<< "Error in Postraitements"<<finl;
                exit();
              }
            }
          list_nom_post.add_if_not(nom_du_post);

          DERIV(Postraitement_base) & post = add( DERIV(Postraitement_base)() );
          post.typer(type);
          post.valeur().associer_nom_et_pb_base(nom_du_post, mon_pb);
          is >> post.valeur();
          /*
          // Check if statistic block is defined several times
          // Not supported yet cause, backup files will contains several
          // statistic blocks with same name BUT different contents -> error
          // We should change the .sauv format and generalize the statistic block name
          if (ref_cast(Postraitement,post.valeur()).stat_demande() || ref_cast(Postraitement,post.valeur()).stat_demande_definition_champs())
          {
            if (post_which_contains_statistic!="")
            {
               Cerr << "Several statistic blocks in a same problem is not supported yet." << finl;
               Cerr << "It seems you defined statistics in the post processing block " << nom_du_post << finl;
               Cerr << "whereas some statistics has been defined already the in the block " << post_which_contains_statistic << finl;
               Cerr << "Check your data file by regrouping statistics in a same prostprocessing block." << finl;
               exit();
            }
                  post_which_contains_statistic=nom_du_post;
                }
          */
          is >> motlu2;
          compteur++;
        }

      // test de verification pour eviter ecrasement de donnnes au fur et a mesure de leur enregistrement
      // a cause du cas Liste_Postraitements => obligation de creer la liste de nom de fichier associes au post
      if (list_nom_post.size()!=compteur)
        {
          Cerr << "You can use the same name for the storing data file in two different blocks of post-processing" << finl;
          Cerr << "Check your datafile." << finl;
          exit();
        }
    }
  else
    {
      return 0;
    }
  return 1;
}

void Postraitements::postraiter()
{
  LIST_CURSEUR(DERIV(Postraitement_base)) curseur = *this;
  while(curseur)
    {
      Postraitement_base& post = curseur.valeur().valeur();
      post.postraiter(1); // On force le postraitement
      ++curseur;
    }
}

void Postraitements::traiter_postraitement()
{
  LIST_CURSEUR(DERIV(Postraitement_base)) curseur = *this;
  while(curseur)
    {
      Postraitement_base& post = curseur.valeur().valeur();
      post.postraiter(0); // Postraitement si intervalle de temps ecoule
      ++curseur;
    }
}

void Postraitements::mettre_a_jour(double temps)
{
  LIST_CURSEUR(DERIV(Postraitement_base)) curseur = *this;
  while(curseur)
    {
      Postraitement_base& post = curseur.valeur().valeur();
      post.mettre_a_jour(temps);
      ++curseur;
    }
}

void Postraitements::init()
{
  LIST_CURSEUR(DERIV(Postraitement_base)) curseur = *this;
  while(curseur)
    {
      Postraitement_base& post = curseur.valeur().valeur();
      post.init();
      ++curseur;
    }
}

void Postraitements::finir()
{
  LIST_CURSEUR(DERIV(Postraitement_base)) curseur = *this;
  while(curseur)
    {
      Postraitement_base& post = curseur.valeur().valeur();
      post.finir();
      ++curseur;
    }
}

int Postraitements::sauvegarder(Sortie& os) const
{
  int bytes = 0;
  CONST_LIST_CURSEUR(DERIV(Postraitement_base)) curseur = *this;
  while(curseur)
    {
      const Postraitement_base& post = curseur.valeur().valeur();
      bytes += post.sauvegarder(os);
      ++curseur;
    }
  return bytes;
}

int Postraitements::reprendre(Entree& is)
{
  LIST_CURSEUR(DERIV(Postraitement_base)) curseur = *this;
  while(curseur)
    {
      Postraitement_base& post = curseur.valeur().valeur();
      post.reprendre(is);
      ++curseur;
    }
  return 1;
}

void Postraitements::completer()
{
  LIST_CURSEUR(DERIV(Postraitement_base)) curseur = *this;
  while(curseur)
    {
      Postraitement_base& post = curseur.valeur().valeur();
      post.completer();
      ++curseur;
    }
}
