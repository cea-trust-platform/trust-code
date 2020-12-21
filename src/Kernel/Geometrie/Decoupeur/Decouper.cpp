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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Decouper.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie/Decoupeur
// Version:     /main/21
//
//////////////////////////////////////////////////////////////////////////////
#include <Decouper.h>
#include <Domaine.h>
#include <DomaineCutter.h>
#include <Postraitement_lata.h>
#include <EChaine.h>
#include <SFichier.h>
#include <Param.h>

Implemente_instanciable(Decouper,"Decouper|Partition",Interprete);

Sortie& Decouper::printOn(Sortie& os) const
{
  exit();
  return os;
}

Entree& Decouper::readOn(Entree& is)
{
  exit();
  return is;
}

// Description:
//  Cherche le domaine de nom "le_nom_domaine" parmi les objets connus
//  de l'interprete et renvoie une ref a ce domaine.
// Exception: si le domaine n'a pas ete trouve, exit.
const Domaine& Decouper::find_domain(const Nom& le_nom_domaine)
{
  const Domaine * domaine_ptr = 0;

  if (objet_existant(le_nom_domaine))
    {
      const Objet_U& objet_domaine = objet(le_nom_domaine);
      if (sub_type(Domaine, objet_domaine))
        domaine_ptr = & ref_cast(Domaine, objet_domaine);
    }

  if (domaine_ptr == 0)
    {
      Cerr << "Error in Decouper::interpreter\n "
           << " Syntax:\n Decouper DOMAINNAME { ... }\n ";
      Cerr << le_nom_domaine << " is not a domain known" << finl;
      Process::exit();
    }
  return *domaine_ptr;
}

// Description: lecture du partitionneur dans le jeu de donnees
//  (lecture du type et des parametres et initialisation de d_part)
static void lire_partitionneur(DERIV(Partitionneur_base) & d_part,
                               const Domaine& domaine, Entree& is)
{
  Nom n;
  is >> n;
  Nom type_partitionneur("Partitionneur_");
  type_partitionneur += n;
  Cerr << " Creation of a partitioner of type: " << type_partitionneur << finl;
  d_part.typer(type_partitionneur);
  // En cas d'echec (si le nom du type est invalide) : exit
  if (! d_part.non_nul())
    Process::exit();

  // Initialisation des parametres du partitionneur
  Partitionneur_base& part = d_part.valeur();
  part.associer_domaine(domaine);
  is >> part;
}

static void ecrire_fichier_decoupage(const Nom& nom_fichier_decoupage,
                                     const ArrOfInt& elem_part,
                                     int& nb_parts_tot)
{
  Cerr << "Writing of the splitting array at the format ArrOfInt ascii\n"
       << " in the file " << nom_fichier_decoupage
       << "\n(for each element, number of the destination processor)" << finl;
  SFichier file; // Fichier ascii
  if (! file.ouvrir(nom_fichier_decoupage))
    {
      Cerr << " Error in the opening of the file." << finl;
      Process::exit();
    }
  file << elem_part;
  file << nb_parts_tot;
}

static void postraiter_decoupage(const Nom& nom_fichier_lata,
                                 const Domaine& domaine,
                                 const ArrOfInt& elem_part)
{
  Cerr << "Postprocessing of the splitting at the lata format: " << nom_fichier_lata << finl;
  Postraitement_lata::Format format = Postraitement_lata::BINAIRE;
  Postraitement_lata::ecrire_entete(nom_fichier_lata,
                                    "Discretisation_inconnue",
                                    format);
  Postraitement_lata::ecrire_zone(nom_fichier_lata,
                                  domaine.zone(0),
                                  format);

  Postraitement_lata::ecrire_temps(nom_fichier_lata,
                                   0.,
                                   format);
  const int n = elem_part.size_array();
  DoubleTab data(n);
  for (int i = 0; i < n; i++)
    data(i) = elem_part[i];
  Postraitement_lata::ecrire_champ(nom_fichier_lata,
                                   Nom("partition"),
                                   Postraitement_lata::CHAMP,
                                   'I',
                                   domaine.le_nom(),
                                   "pb",
                                   0., /* temps */
                                   Postraitement_base::ELEMENTS,
                                   data,
                                   format);

  Postraitement_lata::finir_lata(nom_fichier_lata);
}

static void ecrire_sous_zones(const Nom& nom_zones_decoup,
                              const Decouper::ZonesFileOutputType format,
                              const Domaine& domaine,
                              ArrOfInt& elem_part,
                              const int nb_parties,
                              const int epaisseur_joint,
                              const int reorder,
                              const Noms& bords_periodiques)
{
  DomaineCutter cutter;
  cutter.initialiser(domaine, elem_part, nb_parties, epaisseur_joint,
                     bords_periodiques);
  // Reflexion provisoire:
  // Les joints sont construits dans ecrire_zones -> construire_sous_domaine
  // Donc apres renumerotation des PEs et donc de elem_part, il faudrait
  // reinitialiser ? Ou bien, tout renumeroter apres le calcul des joints...
  // Cela parait mieux...
  cutter.ecrire_zones(nom_zones_decoup, format, elem_part, reorder);
}

// Description:
//  Syntaxe du jeu de donnees :
//
//  Decouper NOMDOMAINE
//  {
//      [ options du partitionneur ]
//  }
// Voir la doc utilisateur TRUST.
Entree& Decouper::interpreter(Entree& is)
{
  Cerr << "Decouper : Splitting of a domain" << finl;

  // Lecture du nom du domaine a decouper:
  int epaisseur_joint = 1;
  Nom nom_zones_decoup("?");
  Nom nom_fichier_decoupage("?");
  Nom nom_fichier_lata("?");
  int format_binaire = 1;
  int format_hdf = 0;
  int nb_parts_tot = -1;
  int reorder = 0;
  Noms liste_bords_periodiques;

  is >> nom_domaine;
  Cerr << " Domain name to split : " << nom_domaine << finl;
  const Domaine& domaine = find_domain(nom_domaine);
  // Avant de decouper on imprime des infos
  domaine.imprimer();

  Param param(que_suis_je());
  param.ajouter_non_std("partitionneur|partition_tool",(this),Param::REQUIRED);
  param.ajouter("larg_joint",&epaisseur_joint);
  param.ajouter_condition("value_of_larg_joint_ge_1","The joint thickness must greater or equal to 1.");
  param.ajouter("nom_zones|zones_name",&nom_zones_decoup);
  param.ajouter("ecrire_decoupage",&nom_fichier_decoupage);
  param.ajouter("ecrire_lata",&nom_fichier_lata);
  param.ajouter("nb_parts_tot",&nb_parts_tot);
  param.ajouter("reorder",&reorder);
  param.ajouter_flag("single_hdf",&format_hdf);
  param.ajouter("periodique",&liste_bords_periodiques);
  param.lire_avec_accolades_depuis(is);

  Partitionneur_base& partitionneur = deriv_partitionneur.valeur();

  // On recupere le resultat: un tableau qui donne pour chaque element
  // le numero de la partie a laquelle il appartient.
  ArrOfInt elem_part;
  partitionneur.declarer_bords_periodiques(liste_bords_periodiques);
  partitionneur.construire_partition(elem_part,nb_parts_tot);

  // Calcul du nombre de parties generees par le partitionneur
  int nb_parties = 0;
  if (elem_part.size_array() > 0)
    nb_parties = max_array(elem_part) + 1;
  Cerr << "The partitioner has generated " << nb_parties << " parts." << finl;

  // Prise en compte de la directive nb_parts_tot
  if (nb_parts_tot >= 0)
    {
      if (nb_parties > nb_parts_tot)
        {
          Cerr << "Error: nb_parts_tot is less than the number of parts generated by the partitioner."
               << finl;
          exit();
        }
      Cerr << "Number of parts requested : " << nb_parts_tot
           << "\nGeneration of " << nb_parts_tot - nb_parties << " empty parts." << finl;
      nb_parties = nb_parts_tot;
    }

  if (nom_fichier_decoupage != "?")
    ecrire_fichier_decoupage(nom_fichier_decoupage, elem_part, nb_parts_tot);

  if (nom_fichier_lata != "?")
    postraiter_decoupage(nom_fichier_lata, domaine, elem_part);

  if (nom_zones_decoup != "?")
    {
      Decouper::ZonesFileOutputType typ;
      if (format_binaire) typ = BINARY_MULTIPLE;
      if (format_hdf) typ = HDF5_SINGLE;
      ecrire_sous_zones(nom_zones_decoup, typ,
                        domaine, elem_part, nb_parties, epaisseur_joint, reorder,
                        liste_bords_periodiques);
    }

  Cout << "\nQuality of partitioning --------------------------------------------" << finl;
  Cout << "\nTotal number of elements = " << elem_part.size_array() << finl;

  DoubleVect A(nb_parties);
  for (int i = 0; i < elem_part.size_array(); i++)
    A(elem_part[i]) = A(elem_part[i]) + 1;

  Cout << "Number of Zones : " << A.size_array() << finl;
  Cout << "Max number of elements by Zones = " << local_max_vect(A) << finl;

  double load_imbalance = double(local_max_vect(A));
  load_imbalance *= nb_parties;
  load_imbalance /= elem_part.size_array();

  Cout << "Load imbalance = " << load_imbalance << "\n" << finl;

  Cerr << "End of the interpreter Decouper" << finl;
  return is;
}

int Decouper::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  Motcle motlu;
  if (mot=="partitionneur|partition_tool")
    {
      Cerr<<"domaine = "<<nom_domaine<<finl;
      const Domaine& domaine = find_domain(nom_domaine);
      lire_partitionneur(deriv_partitionneur,domaine,is);
      return 1;
    }
  else
    {
      Cerr << mot << " is not a keyword understood by " << que_suis_je() << " in lire_motcle_non_standard"<< finl;
      exit();
    }
  return -1;
}
