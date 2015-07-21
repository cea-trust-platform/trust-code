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
// File:        Interprete_bloc.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <Interprete_bloc.h>
#include <Type_Verifie.h>

Implemente_instanciable_sans_constructeur_ni_destructeur(Interprete_bloc,"Interprete_bloc",liste);
Implemente_ref(Interprete_bloc);

// Voir Interprete_bloc::interprete_courant()
static REF(Interprete_bloc) interprete_courant_;

// Description: renvoie l'interprete_bloc en train d'etre
//  lu dans le jeu de donnees. On change d'interprete courant
//  quand on cree et on detruit un objet de type Interprete_bloc
//  (par exemple quand on entre ou qu'on sort d'un bloc { })
Interprete_bloc& Interprete_bloc::interprete_courant()
{
  return interprete_courant_.valeur();
}

Interprete_bloc::Interprete_bloc()
{
  // S'il existe un interprete courant, il devient le pere
  // de l'interprete en cours de construction:
  if (interprete_courant_.non_nul())
    pere_ = interprete_courant_;

  interprete_courant_ = *this;
}

Interprete_bloc::~Interprete_bloc()
{
  // Restaure la valeur de l'interprete courant precedent
  interprete_courant_ = pere_;
}

Sortie& Interprete_bloc::printOn(Sortie& os) const
{
  exit();
  return os;
}

Entree& Interprete_bloc::readOn(Entree& is)
{
  exit();
  return is;
}

// Description:
//  Interpretation d'un bloc d'instructions prises dans l'entree is.
//  Si le bloc commence par {, on suppose que l'accolade a deja ete lue.
//  Le bloc se termine soit par }, soit par la fin du fichier, soit par
//  le mot FIN en fonction de bloc_type (voir mon_main.cpp par exemple)
//  Si verifie_sans_interpreter!=0, on ne cree aucun objet et on ne
//  lance aucun interprete, on se contente de verifier la coherence des
//  accolades (meme nombre de { que de }).
Entree& Interprete_bloc::interpreter_bloc(Entree& is,
                                          Bloc_Type bloc_type,
                                          int verifie_sans_interpreter)
{
  // Le niveau de sortie des messages journal
  const int jlevel = 3;
  Journal(jlevel) << "Interprete_bloc::interpreter_bloc bloc_type=" << (int)bloc_type << finl;
  Motcle motlu;
  is >> motlu;
  while(1)
    {
      // Est-ce qu'on est a la fin du bloc ?
      if (is.eof())
        {
          // Certains fichiers .data ne se terminent pas par FIN, on accepte aussi d'arriver a eof
          // sans avoir trouve "FIN".
          if (bloc_type == BLOC_EOF || bloc_type == FIN)
            {
              Journal(jlevel) << "Interprete_bloc: end of file => end of bloc" << finl;
              break;
            }
          else
            {
              Cerr << "Error in Interprete_bloc: unexpected end of file\n"
                   << "check for missig \"}\" or missing FIN keyword"
                   << finl;
              exit();
            }
        }
      // Autres fins possibles:
      if (motlu == "}" || motlu == "FIN|END")
        {
          if ((motlu == "}" && bloc_type == ACCOLADE)
              || (motlu == "FIN|END" && bloc_type == FIN))
            {
              Journal(jlevel) << "Interprete_bloc: reading " << motlu << " => end of bloc" << finl;
              break;
            }
          else
            {
              if (motlu == "}")
                Cerr << "Error in Interprete_bloc: extra \"}\" in data file" << finl;
              else
                Cerr << "Error in Interprete_bloc: unexpected FIN, check for missing \"}\""
                     << finl;
              exit();
            }
        }

      // Est-ce que c'est un commentaire ?
      if (motlu == "#")
        {
          Nom commentaire("# ");
          Nom nom_lu;
          int countleft = 8; // On retient les 8 premiers mots du commentaire
          do
            {
              is >> nom_lu;
              if (is.eof())
                break;
              if ((countleft--) > 0)
                {
                  commentaire += nom_lu;
                  commentaire += " ";
                }
            }
          while (nom_lu != "#");
          if (is.eof())
            {
              Cerr << "Error in Interprete_bloc: end of file while reading a comment :\n"
                   << commentaire << "..." << finl;
            }
          // Prochain mot a interpreter:
          motlu = nom_lu;
        }
      else if (motlu == "{")
        {
          Journal(jlevel) << "Interprete_bloc: reading { => creating new Interprete_bloc"
                          << finl;
          // Est-ce que c'est un debut de bloc ?
          // Nouvel interprete:
          Interprete_bloc inter;
          inter.interpreter_bloc(is, ACCOLADE, verifie_sans_interpreter);
        }
      else
        {
          if (verifie_sans_interpreter)
            {
              // Si on veut seulement verifier le nombre d'accolades,
              // il suffit d'ignorer le motcle... on ouvre recursivement
              // de nouveaux interprete si on rencontre { et on les ferme
              // si on trouve }. S'il manque une } a la fin, on obtiendra le message
              // "check for missing }" et s'il y en a une en trop on aura
              // le message "extra }"
              Journal(jlevel) << "Interprete_bloc: just checking {} => ignore keyword "
                              << motlu << finl;
              verifie(motlu);
            }
          else
            {
              // Dans le bloc suivant, s'il y a une erreur de lecture sur is, c'est une erreur:
              is.set_error_action(Entree::ERROR_EXIT);
              int export_object = 0;
              if (motlu == "export")
                {
                  Journal(jlevel) << "Exporting object, reading object type..." << finl;
                  export_object = 1;
                  is >> motlu;
                }
              // Le mot cle doit etre un type d'objet instanciable
              Journal(jlevel) << "Interprete_bloc: reading " << motlu
                              << " => trying to instanciate object" << finl;
              DerObjU objet;
              objet.typer(motlu);

              if (sub_type(Interprete, objet.valeur()))
                {
                  // Si c'est un interprete, on appelle la methode interprete de l'objet:
                  Journal(jlevel) << " Calling object.interpreter()" << finl;
                  Interprete& inter = ref_cast(Interprete, objet.valeur());
                  inter.interpreter(is);
                }
              else
                {
                  // Ce n'est pas un interprete, on lit le nom de l'objet et on le stocke
                  Journal(jlevel) << " Reading object name" << finl;
                  Nom nom_objet;
                  is >> nom_objet;
                  Journal(jlevel) << " Storing object " << nom_objet << " of type " << motlu << finl;
                  if (!export_object || !pere_.non_nul())
                    ajouter(nom_objet, objet);
                  else
                    pere_.valeur().ajouter(nom_objet, objet);
                }
              is.set_error_action(Entree::ERROR_CONTINUE);
            }
        }
      is >> motlu;
    }
  return is;
}

// Description:
//  Renvoie l'Objet_U correspondant a nom contenu dans cet interprete_bloc
//  Si l'objet n'existe pas, exit() (on ne cherche pas dans le pere).
Objet_U& Interprete_bloc::objet_local(const Nom& nom)
{
  const int i = les_noms_.search(nom);
  if (i < 0)
    {
      Cerr << "Internal error in Interprete::objet_local() : object " << nom
           << " does not exist" << finl;
      exit();
    }
  return operator[](i);
}

// Description: renvoie un drapeau indiquant si un objet de ce nom
//  est enregistre dans cet inteprete (ne teste pas le pere).
int Interprete_bloc::objet_local_existant(const Nom& nom)
{
  const int i = les_noms_.search(nom);
  return (i >= 0);
}

// Description: Ajoute l'objet ob a la liste des objets de l'interprete,
//  et nomme l'objet avec nom. Si l'objet existe deja, exit()
Objet_U& Interprete_bloc::ajouter(const Nom& nom, DerObjU& ob)
{
  Journal(3) << "Interprete::ajouter(" << nom << ") de type "
             << ob.valeur().que_suis_je() << finl;
  if (les_noms_.search(nom) >= 0)
    {
      Cerr << "Error in Interprete::ajouter: object " << nom << " already exists." << finl;
      exit();
    }
  les_noms_.add(nom);
  Objet_U& obu = add_deplace(ob);
  obu.nommer(nom);
  return obu;
}

// Description: cherche l'objet demande dans l'Interprete_bloc
//  courant (Interprete_bloc::interprete_courant()) et dans tous
//  ses peres successifs. Si l'objet n'existe pas, exit()
Objet_U& Interprete_bloc::objet_global(const Nom& nom)
{
  REF(Interprete_bloc) ptr(interprete_courant());
  while (ptr.non_nul())
    {
      Interprete_bloc& interp = ptr.valeur();
      if (interp.objet_local_existant(nom))
        {
          Objet_U& objet = interp.objet_local(nom);
          return objet;
        }
      ptr = interp.pere_;
    }
  // Objet non trouve !
  Cerr << "Error in Interprete::objet: object " << nom
       << " does not exist." << finl;
  Process::exit();
  return objet_global(nom); // Pour le compilo
}

// Description: renvoie un drapeau indiquant si un objet de ce
//  nom existe dans inteprete_courant() ou l'un de ses parents.
int Interprete_bloc::objet_global_existant(const Nom& nom)
{
  REF(Interprete_bloc) ptr(interprete_courant());
  while (ptr.non_nul())
    {
      Interprete_bloc& interp = ptr.valeur();
      if (interp.objet_local_existant(nom))
        return 1;
      ptr = interp.pere_;
    }
  // Objet non trouve !
  return 0;
}
