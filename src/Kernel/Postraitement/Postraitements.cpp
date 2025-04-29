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

#include <Postraitements.h>
#include <Postraitement.h>

Implemente_instanciable(Postraitements,"Postraitements|Post_processings",LIST(OWN_PTR(Postraitement_base)));
// XD postraitements listobj postraitements -1 un_postraitement 0 Keyword to use several results files. List of objects of post-processing (with name).

// XD type_postraitement_ft_lata objet_lecture nul 0 not_set
// XD attr type chaine(into=["postraitement_ft_lata","postraitement_lata"]) type 0 not_set
// XD attr nom chaine nom 0 Name of the post-processing.
// XD attr bloc chaine bloc 0 not_set

// XD un_postraitement_spec objet_lecture nul 0 An object of post-processing (with type +name).
// XD attr type_un_post type_un_post type_un_post 1 not_set
// XD attr type_postraitement_ft_lata type_postraitement_ft_lata type_postraitement_ft_lata 1 not_set

// XD liste_post listobj liste_post -1 un_postraitement_spec 0 Keyword to use several results files. List of objects of post-processing (with name)

// XD liste_post_ok listobj liste_post_ok -1 nom_postraitement 0 Keyword to use several results files. List of objects of post-processing (with name)

Entree& Postraitements::readOn(Entree& s)
{
  Process::exit("Postraitements::readOn should not be used");
  return s;
}

Sortie& Postraitements::printOn(Sortie& s) const
{
  for (const auto& itr : *this)
    {
      const Postraitement_base& post = itr.valeur(); // valeur() car OWN_PTR
      s << post;
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

int Postraitements::lire_postraitements(Entree& is, const Motcle& motlu, const Probleme_base& mon_pb)
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
      OWN_PTR(Postraitement_base) & post = add( OWN_PTR(Postraitement_base)() );
      if (mon_pb.que_suis_je() == "Pb_STT")
        post.typer("Postraitement_STT");
      else if (mon_pb.que_suis_je().debute_par("Probleme_FT_Disc_gen"))
        post.typer("Postraitement_ft_lata");
      else if (mon_pb.que_suis_je().contient("_IJK"))
        post.typer("Postprocessing_IJK");
      else if (mon_pb.que_suis_je().debute_par("Probleme_FT_Disc_gen"))
        post.typer("Postraitement_ft_lata");
      else
        post.typer("Postraitement");
      post->associer_nom_et_pb_base("neant", mon_pb);
      is >> post.valeur();
    }
  else if (lerang == 1 || lerang == 2 || lerang == 3 )
    {
      // Lecture d'une liste
      // Lire l'accolade
      //Nom post_which_contains_statistic("");
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

          OWN_PTR(Postraitement_base) & post = add( OWN_PTR(Postraitement_base)() );
          post.typer(type);
          post->associer_nom_et_pb_base(nom_du_post, mon_pb);
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
  for (auto& itr : *this)
    {
      Postraitement_base& post = itr.valeur();
      post.postraiter(1); // On force le postraitement
    }
}

void Postraitements::traiter_postraitement()
{
  for (auto& itr : *this)
    {
      Postraitement_base& post = itr.valeur();
      post.postraiter(0); // Postraitement si intervalle de temps ecoule
    }
}

void Postraitements::mettre_a_jour(double temps)
{
  for (auto& itr : *this)
    {
      Postraitement_base& post = itr.valeur();
      post.mettre_a_jour(temps);
    }
}

void Postraitements::resetTime(double t, const std::string dirname)
{
  for (auto& itr : *this)
    {
      Postraitement_base& post = itr.valeur();
      post.resetTime(t, dirname);
    }
}

void Postraitements::init()
{
  for (auto& itr : *this)
    {
      Postraitement_base& post = itr.valeur();
      post.init();
    }
}

void Postraitements::finir()
{
  for (auto& itr : *this)
    {
      Postraitement_base& post = itr.valeur();
      post.finir();
    }
}

int Postraitements::sauvegarder(Sortie& os) const
{
  int bytes = 0;
  for (const auto& itr : *this)
    {
      const Postraitement_base& post = itr.valeur();
      bytes += post.sauvegarder(os);
    }
  return bytes;
}

int Postraitements::reprendre(Entree& is)
{
  for (auto& itr : *this)
    {
      Postraitement_base& post = itr.valeur();
      post.reprendre(is);
    }
  return 1;
}

void Postraitements::completer()
{
  for (auto& itr : *this)
    {
      Postraitement_base& post = itr.valeur();
      post.completer();
    }
}

void Postraitements::completer_sondes()
{
  for (auto& itr : *this)
    {
      Postraitement_base& post = itr.valeur();
      post.completer_sondes();
    }
}
