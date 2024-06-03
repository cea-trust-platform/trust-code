/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <EcrFicPartageMPIIO.h>
#include <Format_Post_Lata.h>
#include <EcrFicPartageBin.h>
#include <communications.h>
#include <Fichier_Lata.h>
#include <EFichier.h>
#include <sys/stat.h>
#include <Param.h>
#include <string> // Necessaire avec xlC pour std::getline

Implemente_instanciable_sans_constructeur(Format_Post_Lata,"Format_Post_Lata",Format_Post_base);

#define _LATA_INT_TYPE_ int

/*! @brief Constructeur par defaut: format_ ASCII et options_para_ = SINGLE_FILE
 *
 */
Format_Post_Lata::Format_Post_Lata()
{
  reset();
}

/*! @brief Remet l'objet dans l'etat obtenu par le constructeur par defaut.
 *
 */
void Format_Post_Lata::reset()
{
  lata_basename_ = "??";
  format_ = ASCII;
  options_para_ = SINGLE_FILE;
  status = RESET;
  temps_courant_ = -1.;
  deja_fait_ = 0;
  tinit_ = -1.;
  file_existe_ = 0;
}

Sortie& Format_Post_Lata::printOn(Sortie& os) const
{
  Process::exit("Format_Post_Lata::printOn : error");
  return os;
}

/*! @brief Lecture des parametres du postraitement au format "jeu de donnees" Le format attendu est le suivant:
 *
 *   {
 *        nom_fichier nom                       champ obligatoire
 *      [ format   ascii|binaire ]              valeur par defaut : ascii
 *      [ parallel single_file|multiple_files ] valeur par defaut : single_file
 *   }
 *
 */
Entree& Format_Post_Lata::readOn(Entree& is)
{
  assert(status == RESET);
  Format_Post_base::readOn(is);
  status = INITIALIZED;
  return is;
}

void Format_Post_Lata::set_param(Param& param)
{
  Cerr << "Format_Post_Lata::set_param: Not implemented." << finl;
  Process::exit();
}

int Format_Post_Lata::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  Cerr << "Format_Post_Lata::lire_motcle_non_standard: Not implemented." << finl;
  Process::exit();
  return 0;
}

/*! @brief Renvoie l'extension conventionnelle des fichiers lata : ".lata"
 *
 */
const char * Format_Post_Lata::extension_lata()
{
  static const char * ext = ".lata";
  return ext;
}

/*! @brief Renvoie le nom d'un fichier sans le path : on enleve les caracteres avant le dernier /
 *
 */
const char * Format_Post_Lata::remove_path(const char * filename)
{
  int i = (int)strlen(filename);
  while (i > 0 && filename[i-1] != '/')
    i--;
  return filename + i;
}

/*! @brief Ouvre le fichier maitre en mode ERASE et ecrit l'entete du fichier lata (sur le processeur maitre seulement).
 *
 * void Format_Post_Lata::ecrire_entete_lata()
 *
 */
int Format_Post_Lata::ecrire_entete(const double temps_courant, const int reprise, const int est_le_premier_post)
{
  ecrire_entete_lata(lata_basename_, options_para_, format_, est_le_premier_post);
  return 1;
}

int Format_Post_Lata::completer_post(const Domaine&, const int , const Nature_du_champ&, const int , const Noms&, const Motcle&, const Nom&)
{
  return 1;
}

int Format_Post_Lata::preparer_post(const Nom& , const int , const int , const double )
{
  return 1;
}

/*! @brief fichier est un fichier lata de donnees (pas le fichier maitre) on y ecrit le tableau tab tel quel (en binaire ou ascii et sur un ou
 *
 *   plusieurs fichiers en parallel).
 *   nb_colonnes est rempli avec le produit des tab.dimension(i) pour i>0
 *
 */
int Format_Post_Lata::write_doubletab(Fichier_Lata& fichier, const DoubleTab& tab, int& nb_colonnes, const Options_Para& option)
{
  int line_size = 1;
  int nb_lignes = tab.dimension(0);
  const int nb_dim = tab.nb_dim();
  for (int i = 1; i < nb_dim; i++)
    line_size *= tab.dimension(i);

  int nb_lignes_tot = 0;
  const int tab_size = line_size * nb_lignes;
  int nb_octets = tab_size * (int) sizeof(float);
  switch(option)
    {
    case Format_Post_Lata::SINGLE_FILE_MPIIO:
    case Format_Post_Lata::SINGLE_FILE:
      nb_lignes_tot = Process::mp_sum(nb_lignes);
      // En parallele, tous les tableaux doivent avoir le meme nombre de colonnes (ou etre vides).
      nb_colonnes = Process::mp_max(line_size);
      nb_octets = nb_colonnes * nb_lignes_tot * (int) sizeof(float);
      assert(nb_lignes == 0 || line_size == nb_colonnes);
      break;
    case Format_Post_Lata::MULTIPLE_FILES:
      nb_lignes_tot = nb_lignes;
      nb_colonnes = line_size;
      break;
    default:
      Cerr << "Format_Post_Lata_write_tab: error nb_lignes_tot" << finl;
      Process::exit();
    }

  SFichier& sfichier = fichier.get_SFichier();

  // Debut de bloc fortran
  if (fichier.is_master())
    sfichier << nb_octets << finl;

  // Ecriture des donnees.
  if (sub_type(EcrFicPartageMPIIO, sfichier))
    {
      auto *tmp = new float[tab_size]; // No ArrOfFloat in TRUST
      const double *data = tab.addr();
      for (int i = 0; i < tab_size; i++)
        tmp[i] = (float) data[i];       // downcast to float
      sfichier.put(tmp, tab_size, line_size);
      delete[] tmp;
      // Fin de bloc fortran
      if (fichier.is_master())
        sfichier << nb_octets << finl;
    }
  else
    {
      // On convertit le tout en float par paquet de N valeurs Buffer dont la taille est un multiple de line_size:
      const int N = 16384;
      int bufsize = (N / line_size + 1) * line_size;
      float *tmp = new float[bufsize];
      const double *data = tab.addr();
      for (int i = 0; i < tab_size; i += bufsize)
        {
          int j_max = bufsize;
          if (j_max > tab_size - i)
            j_max = tab_size - i;

          // Conversion du bloc en float:
          for (int j = 0; j < j_max; j++)
            tmp[j] = (float) data[i + j];

          // Ecriture avec retour a la ligne a chaque ligne du tableau
          sfichier.put(tmp, j_max, line_size);
        }
      delete[] tmp;
      fichier.syncfile();
      // Fin de bloc fortran
      if (fichier.is_master())
        sfichier << nb_octets << finl;
      fichier.syncfile();
    }

  return nb_lignes_tot;
}

/*! @brief Ecriture d'un tableau d'entiers dans le fichier fourni.
 *
 * Les valeurs ecrites sont les valeurs du tableau auquelles ont ajoute "decalage". Cette valeur est utilisee pour passer en numerotation
 *   fortran (ajouter 1), ou pour passer en numerotation globale (ajouter le nombre d'elements sur les processeurs precedents).
 *
 *  On renvoie dans nb_colonnes la somme des dimension(i) pour i>0.
 *  Valeur de retour: somme des dimension(0) ecrits (selon que tous les processeurs ecrivent sur le meme fichier ou pas).
 */
int Format_Post_Lata::write_inttab(Fichier_Lata& fichier, int decalage, int decalage_partiel, const IntTab& tab, int& nb_colonnes, const Options_Para& option)
{
  assert(decalage_partiel >= decalage);
  assert((decalage == 0) || (decalage == 1));
  int nb_lignes = 0;
  int line_size = 1;
  int i;
  nb_lignes = tab.dimension(0);
  const int nb_dim = tab.nb_dim();
  for (i = 1; i < nb_dim; i++)
    line_size *= tab.dimension(i);

  int nb_lignes_tot = 0;

  const int tab_size = line_size * nb_lignes;
  int nb_octets = tab_size * (int) sizeof(_LATA_INT_TYPE_);
  switch(option)
    {
    case Format_Post_Lata::SINGLE_FILE_MPIIO:
    case Format_Post_Lata::SINGLE_FILE:
      nb_lignes_tot = Process::mp_sum(nb_lignes);
      // En parallele, tous les tableaux doivent avoir le meme nombre de colonnes (ou etre vides).
      nb_colonnes = Process::mp_max(line_size);
      nb_octets = nb_colonnes * nb_lignes_tot * (int) sizeof(_LATA_INT_TYPE_);
      assert(nb_lignes == 0 || line_size == nb_colonnes);
      break;
    case Format_Post_Lata::MULTIPLE_FILES:
      nb_lignes_tot = nb_lignes;
      nb_colonnes = line_size;
      break;
    default:
      Cerr << "Format_Post_Lata_write_tab: error nb_lignes_tot" << finl;
      exit();
    }

  SFichier& sfichier = fichier.get_SFichier();

  // Debut de bloc fortran
  if (fichier.is_master())
    sfichier << nb_octets << finl;
  int erreurs = 0;

  // Ecriture des donnees.
  if (sub_type(EcrFicPartageMPIIO, sfichier))
    {
      // On convertit le tout en _INT_TYPE_
      _LATA_INT_TYPE_ *tmp = new _LATA_INT_TYPE_[tab_size];
      const int *data = tab.addr();
      for (i = 0; i < tab_size; i++)
        {
          // valeur a ecrire (conversion en numerotation fortran si besoin)
          int x = data[i];
          if (x > -1)
            x += decalage_partiel;
          else
            x += decalage;

          // conversion en type int pour ecriture
          _LATA_INT_TYPE_ y = (_LATA_INT_TYPE_) x;
          // reconversion en int pour comparaison
          int z = (int) y;
          if (x != z)
            erreurs++;
          tmp[i] = y;
        }
      sfichier.put(tmp, tab_size, line_size);
      delete[] tmp;
      // Fin de bloc fortran
      if (fichier.is_master())
        sfichier << nb_octets << finl;
    }
  else
    {
      // On convertit le tout en _INT_TYPE_ par paquet de N valeurs
      // Buffer dont la taille est un multiple de line_size:
      const int N = 16384;
      int bufsize = (N / line_size + 1) * line_size;
      _LATA_INT_TYPE_ *tmp = new _LATA_INT_TYPE_[bufsize];
      const int *data = tab.addr();
      for (i = 0; i < tab_size; i += bufsize)
        {
          int j;
          int j_max = bufsize;
          if (j_max > tab_size - i)
            j_max = tab_size - i;

          // Conversion du bloc en int_type:
          for (j = 0; j < j_max; j++)
            {
              // valeur a ecrire (conversion en numerotation fortran si besoin)
              int x = data[i + j];
              if (x > -1)
                x += decalage_partiel;
              else
                x += decalage;

              // conversion en type int pour ecriture
              _LATA_INT_TYPE_ y = (_LATA_INT_TYPE_) x;
              // reconversion en int pour comparaison
              int z = (int) y;
              if (x != z)
                erreurs++;
              tmp[j] = y;
            }
          // Ecriture avec retour a la ligne a chaque ligne du tableau
          sfichier.put(tmp, j_max, line_size);
        }
      delete[] tmp;
      fichier.syncfile();
      // Fin de bloc fortran
      if (fichier.is_master())
        sfichier << nb_octets << finl;
      fichier.syncfile();
    }
  if (erreurs > 0)
    {
      Cerr << "Error in Format_Post_Lata::write_inttab\n" << " Some integers were truncated\n" << " Recompile the code with #define _LATA_INT_TYPE_ entier" << finl;
      Process::exit();
    }
  return nb_lignes_tot;
}

/*! @brief Initialisation de la classe avec des parametres par defaut (format ASCII, SINGLE_FILE)
 *
 */
int Format_Post_Lata::initialize_by_default(const Nom& file_basename)
{
  assert(status == RESET);
  initialize_lata(file_basename, ASCII, SINGLE_FILE);
  return 1;
}

int Format_Post_Lata::initialize(const Nom& file_basename, const int format, const Nom& option_para)
{
  assert(status == RESET);
  // Changement du format LATA (par defaut binaire)
  format_ = BINAIRE;
  if (format == 0)
    format_ = ASCII;

  if (Motcle(option_para) == "MPI-IO")
    options_para_ = SINGLE_FILE_MPIIO;
  else if (Motcle(option_para) == "SIMPLE")
    options_para_ = SINGLE_FILE;
  else if (Motcle(option_para) == "MULTIPLE")
    options_para_ = MULTIPLE_FILES;
  else
    {
      Cerr << "The option " << option_para << " for lata format for the parallel is not correct." << finl;
      Process::exit();
    }

  initialize_lata(file_basename, format_, options_para_);

  return 1;
}

/*! @brief Initialisation de la classe, ouverture du fichier et ecriture de l'entete.
 *
 */
int Format_Post_Lata::initialize_lata(const Nom& file_basename, const Format format, const Options_Para options_para)
{
  assert(status == RESET);
  lata_basename_ = file_basename;
  format_ = format;
  options_para_ = options_para;
  status = INITIALIZED;
  return 1;
}

/*! @brief Modification du nom du fichier de postraitement.
 *
 */
int Format_Post_Lata::modify_file_basename(const Nom file_basename, const int a_faire, const double tinit)
{
  assert(a_faire == 0 || a_faire == 1 || a_faire == 2);

  Nom post_file;
  post_file = file_basename + extension_lata();
  // On verifie que le fichier maitre existe et a une entete correcte
  file_existe_ = 0;
  if (Process::je_suis_maitre())
    {
      struct stat f;
      if (stat(post_file, &f) == 0)
        {
          EFichier tmp(post_file);
          Nom cle;
          tmp >> cle;
          if (cle.debute_par("LATA"))
            {
              if (tinit == -1)
                file_existe_ = 1;
              else
                {
                  tmp >> cle;
                  while (!tmp.eof() && cle != "TEMPS")
                    tmp >> cle;
                  if (cle == "TEMPS")
                    {
                      double temps;
                      tmp >> temps;
                      // On verifie le temps du fichier de reprise
                      if (temps < tinit)
                        file_existe_ = 1;
                    }
                }
            }
          tmp.close();
        }
    }
  // Point de synchronisation necessaire:
  file_existe_ = (int) Process::mp_max(file_existe_);
  if (Process::je_suis_maitre() && file_existe_ && deja_fait_ == 0 && a_faire)
    {
      Nom before_restart;
      before_restart = file_basename + ".before_restart" + extension_lata();
      rename(post_file, before_restart);
      Cerr << "File " << post_file << " is moved to " << before_restart << " with times<=tinit=" << tinit << finl;
      reconstruct(post_file, before_restart, tinit);
    }
  deja_fait_ = 1;
  // On attend tous les processeurs (prudence excessive?)
  Process::barrier();
  if (file_existe_)
    {
      if (a_faire == 0)
        lata_basename_ = file_basename;
      else
        {
          lata_basename_ = file_basename + ".after_restart";
          if (tinit == -1)
            finir_sans_reprise(post_file);
        }
    }
  return 1;
}

// Copie du debut du fichier before_restart dans post_file (jusqu'au TEMPS=tinit)
int Format_Post_Lata::reconstruct(const Nom post_file, const Nom before_restart, const double tinit)
{
  EFichier LataOld(before_restart);
  SFichier LataNew(post_file);
  LataNew.setf(ios::scientific);
  LataNew.precision(8);
  std::string line;
  Nom mot;
  double temps;
  tinit_ = tinit;
  while (!LataOld.eof())
    {
      LataOld >> mot; // je lis le premier mot de la ligne dans .before_restart.lata
      if (mot != "FIN") // Si le mot n'est pas FIN
        {
          if (mot == "TEMPS") // Si le mot suivant est TEMPS
            {
              LataOld >> temps;
              if (temps == tinit) // Si le temps trouvee = tinit alors j'arrete
                break;
              std::getline(LataOld.get_ifstream(), line); // je lis le reste de la ligne
              LataNew.get_ofstream() << mot << " " << temps << line << std::endl; // et j'ecris le tout dans le lata
            }
          else
            {
              std::getline(LataOld.get_ifstream(), line); // je lis le reste de la ligne
              LataNew.get_ofstream() << mot << line << std::endl; // et j'ecris le tout dans le lata
            }
        }
      else
        {
          break; // Sinon j'arrete
        }
    }
  LataNew.close();
  LataOld.close();
  Cerr << "File " << post_file << " is rebuilt but truncated to times<tinit=" << tinit << finl;
  return 1;
}

int Format_Post_Lata::finir_sans_reprise(const Nom file_basename)
{
  Nom post_file;
  post_file = lata_basename_ + extension_lata();
  if (Process::je_suis_maitre())
    {
      struct stat f;
      if (stat(post_file, &f) == 0)
        {
          EFichier Lata(file_basename);
          SFichier LataRep(post_file, ios::app);
          LataRep.setf(ios::scientific);
          LataRep.precision(8);
          std::string line;
          Nom mot;
          double temps;
          while (!Lata.eof())
            {
              Lata >> mot; // je lis le premier mot de la ligne dans .lata
              if (mot == "TEMPS") // Si le mot suivant n'est pas TEMPS
                {
                  Lata >> temps;
                  if (temps == tinit_) // Si le temps trouvee = tinit alors
                    {
                      std::getline(Lata.get_ifstream(), line);
                      LataRep.get_ofstream() << mot << " " << temps << line << std::endl; // je peux commencer a ecrire
                      break;
                    }
                }
            }
          while (!Lata.eof())
            {
              std::getline(Lata.get_ifstream(), line); // je lis le reste du .lata
              LataRep.get_ofstream() << line << std::endl; // et j'ecris le tout dans le after_restart.lata
            }
          Lata.close();
          LataRep.close();
          Cerr << "File " << post_file << " is built with times from tinit=" << tinit_ << finl;
        }
    }
  return 1;
}

/*! @brief Low level routine to write a mesh into a LATA file.
 *
 * Also called directly in TrioCFD, by Postraitement_ft_lata for interface writing.
 */
int Format_Post_Lata::ecrire_domaine_low_level(const Nom& id_domaine, const DoubleTab& sommets, const IntTab& elements, const Motcle& type_element)
{
  const int dim = sommets.dimension(1);
  Motcle type_elem(type_element);

  // GF Pour assuerer la lecture avec le plugin lata
  if (type_element == "PRISME") type_elem = "PRISM6";

  int nb_som_tot, nb_elem_tot;  // Nombre de sommets/elements dans le fichier

  // Construction du nom du fichier de geometrie
  Nom basename_geom(lata_basename_), extension_geom(extension_lata());

  if (un_seul_fichier_lata_) extension_geom += "_single";
  else
    {
      extension_geom += ".";
      extension_geom += id_domaine;
      extension_geom += ".";
      char str_temps[100] = "0.0";
      if (temps_courant_ >= 0.)
        snprintf(str_temps, 100, "%.10f", temps_courant_);
      extension_geom += Nom(str_temps);
    }

  Nom nom_fichier_geom;
  int decalage_sommets = 1, decalage_elements = 1;

  {
    Fichier_Lata fichier_geom(basename_geom, extension_geom, offset_elem_ < 0 ? Fichier_Lata::ERASE : Fichier_Lata::APPEND, format_, options_para_);

    nom_fichier_geom = fichier_geom.get_filename();
    int nb_col;

    if (un_seul_fichier_lata_)
      if (fichier_geom.is_master())
        offset_som_ = fichier_geom.get_SFichier().get_ofstream().tellp();

    // Coordonnees des sommets
    if (axi)
      {
        DoubleTab sommets2(sommets);
        int ns = sommets2.dimension_tot(0);
        for (int s = 0; s < ns; s++)
          {
            double r = sommets(s, 0), theta = sommets(s, 1);
            sommets2(s, 0) = r * cos(theta);
            sommets2(s, 1) = r * sin(theta);
          }
        nb_som_tot = write_doubletab(fichier_geom, sommets2, nb_col, options_para_);
      }
    else
      nb_som_tot = write_doubletab(fichier_geom, sommets, nb_col, options_para_);

    assert(nb_som_tot == 0 || nb_col == dim);

    // Elements : Les indices de sommets, elements et autres dans le fichier lata commencent a 1 :
    if (options_para_ == SINGLE_FILE || options_para_ == SINGLE_FILE_MPIIO)
      {
        // Tous les processeurs ecrivent dans un fichier unique, il faut renumeroter les indices pour passer en numerotation globale.
        // Le processeur 0 numerote ses sommets de 1 a n0
        // Le processeur 1 numerote ses sommets de n0+1 a n0+n1, etc...
        // Decalage a ajouter aux indices pour avoir une numerotation globale.
        const int nbsom = sommets.dimension(0);
        decalage_sommets += mppartial_sum(nbsom);
        const int nbelem = elements.dimension(0);
        decalage_elements += mppartial_sum(nbelem);
      }

    if (un_seul_fichier_lata_)
      {
        if (fichier_geom.is_master())
          offset_elem_ = fichier_geom.get_SFichier().get_ofstream().tellp();

        nb_elem_tot = write_inttab(fichier_geom, 1, decalage_sommets, elements, nb_col, options_para_);
      }
    else
      {
        Fichier_Lata fichier_geom_elem(basename_geom, extension_geom + Nom(".elem"), Fichier_Lata::ERASE, format_, options_para_);
        nb_elem_tot = write_inttab(fichier_geom_elem, 1, decalage_sommets, elements, nb_col, options_para_);
      }
  }

  {
    // Attention, il faut refermer le fichier avant d'appeler ecrire_item_int qui va ouvrir a nouveau le fichier.
    Fichier_Lata_maitre fichier_lata(lata_basename_, extension_lata(), Fichier_Lata::APPEND, options_para_);
    SFichier& sfichier = fichier_lata.get_SFichier();

    if (fichier_lata.is_master())
      {
        sfichier << "GEOM " << id_domaine;
        sfichier << " type_elem=" << type_elem << finl;

        // SOMMETS support
        sfichier << "CHAMP SOMMETS " << remove_path(nom_fichier_geom);
        sfichier << " geometrie=" << id_domaine;
#ifdef INT_is_64_
        // The string "INT64\n" is written in clear text at the begining of each sub-file when we are 64bits.
        // This makes 6 bytes that we have to skip to get to the core of the (binary) data.
        sfichier << " file_offset=6";
        if (un_seul_fichier_lata_) Process::exit("Single_lata option is not yet ported to 64 bits executable ! Use format lata instead or contact TRUST support team !");
#endif
        sfichier << " size=" << nb_som_tot;
        sfichier << " composantes=" << dim;
        if (un_seul_fichier_lata_)
          sfichier << " file_offset=" << (int)offset_som_ << finl;
        else
          sfichier << finl;

        // ELEMENTS support
        sfichier << "CHAMP ELEMENTS " << remove_path(nom_fichier_geom);
        if (!un_seul_fichier_lata_) sfichier << ".elem";
        sfichier << " geometrie=" << id_domaine;
#ifdef INT_is_64_
        // The string "INT64\n" is written in clear text at the begining of each sub-file when we are 64bits.
        // This makes 6 bytes that we have to skip to get to the core of the (binary) data.
        sfichier << " file_offset=6";
#endif
        sfichier << " size=" << nb_elem_tot << " composantes=" << elements.dimension(1);
        if (un_seul_fichier_lata_)
          sfichier << " file_offset=" << (int)offset_elem_;
        switch(sizeof(_LATA_INT_TYPE_))
          {
          case 4:
            sfichier << " format=INT32" << finl;
            break;
          case 8:
            sfichier << " format=INT64" << finl;
            break;
          default:
            Cerr << "Error in Format_Post_Lata::ecrire_entete\n" << " sizeof(int) not supported" << finl;
            Process::exit();
          }

      }
    fichier_lata.syncfile();
  }

  // En mode parallele, on ecrit en plus des fichiers contenant les donnees paralleles sur les sommets, les elements et les faces...
  if (Process::nproc() > 1)
    if (options_para_ == SINGLE_FILE || options_para_ == SINGLE_FILE_MPIIO)
      {
        IntTab data(1,2);
        data(0, 0) = decalage_sommets;
        data(0, 1) = sommets.dimension(0);
        ecrire_item_int("JOINTS_SOMMETS",
                        id_domaine,
                        "", /* id_domaine */
                        "", /* localisation */
                        "", /* reference */
                        data,
                        0); /* reference_size */
        data(0, 0) = decalage_elements;
        data(0, 1) = elements.dimension(0);
        ecrire_item_int("JOINTS_ELEMENTS",
                        id_domaine,
                        "", /* id_domaine */
                        "", /* localisation */
                        "", /* reference */
                        data,
                        0); /* reference_size */
      }
  return 1;
}

/*! @brief voir Format_Post_base::ecrire_domaine On accepte l'ecriture d'un domaine dans un pas de temps, mais
 *
 *   les id_domaines doivent etre tous distincts.
 *   Ecrit le fichier "basename(_XXXXX).lata.nom_domaine", qui contient la liste des sommets et la liste des elements.
 *   Si le PE est maitre, ouvre le fichier maitre en mode APPEND et ajoute une reference a ce fichier.
 */
int Format_Post_Lata::ecrire_domaine(const Domaine& domaine,const int est_le_premier_post)
{
  if (status == RESET)
    {
      Cerr << "Error in Format_Post_Lata::ecrire_domaine\n" << " status = RESET. Uninitialized object" << finl;
      Process::exit();
    }
  Motcle type_elem = domaine.type_elem().valeur().que_suis_je();

  ecrire_domaine_low_level(domaine.le_nom(), domaine.les_sommets(), domaine.les_elems(), type_elem);

  // Si on a des frontieres domaine, on les ecrit egalement
  const LIST(REF(Domaine)) bords= domaine.domaines_frontieres();
  for (int i=0; i<bords.size(); i++)
    ecrire_domaine(bords[i].valeur(),est_le_premier_post);

  return 1; // ok tout va bien
}

/*! @brief commence l'ecriture d'un nouveau pas de temps En l'occurence pour le format LATA:
 *
 *   Ouvre le fichier maitre en mode APPEND et ajoute une ligne
 *    "TEMPS xxxxx" si ce temps n'a pas encore ete ecrit
 *
 */
int Format_Post_Lata::ecrire_temps(const double temps)
{
  ecrire_temps_lata(temps,temps_courant_,lata_basename_,status,options_para_);
  return 1;
}

/*! @brief voir Format_Post_base::ecrire_champ
 *
 */
int Format_Post_Lata::ecrire_champ(const Domaine& domaine, const Noms& unite_, const Noms& noms_compo, int ncomp, double temps, const Nom& id_du_champ, const Nom& id_du_domaine,
                                   const Nom& localisation, const Nom& nature, const DoubleTab& valeurs)
{
  Motcle id_du_champ_modifie(id_du_champ), iddomaine(id_du_domaine);

  //On utilise prefix avec un argument en majuscule
  if ((Motcle) localisation == "SOM")
    {
      id_du_champ_modifie.prefix(id_du_domaine);
      id_du_champ_modifie.prefix(iddomaine);
      id_du_champ_modifie.prefix("_SOM_");
    }
  else if ((Motcle) localisation == "ELEM")
    {
      id_du_champ_modifie.prefix(id_du_domaine);
      id_du_champ_modifie.prefix(iddomaine);
      id_du_champ_modifie.prefix("_ELEM_");
    }
  else if ((Motcle) localisation == "FACES")
    {
      id_du_champ_modifie.prefix(id_du_domaine);
      id_du_champ_modifie.prefix(iddomaine);
      id_du_champ_modifie.prefix("_FACES_");
    }
  Nom& id_champ = id_du_champ_modifie;

  // Construction du nom du fichier
  Nom basename_champ(lata_basename_), extension_champ(extension_lata());

  if (un_seul_fichier_lata_) extension_champ += "_single";
  else
    {
      extension_champ += ".";
      extension_champ += id_champ;
      extension_champ += ".";
      extension_champ += localisation;
      extension_champ += ".";
      extension_champ += id_du_domaine;
      extension_champ += ".";
      char str_temps[100] = "0.0";
      if (temps >= 0.)
        snprintf(str_temps, 100, "%.10f", temps);
      extension_champ += str_temps;
    }

  Nom filename_champ;
  int size_tot, nb_compo;
  {
    Fichier_Lata fichier_champ(basename_champ, extension_champ, offset_elem_ < 0 ? Fichier_Lata::ERASE : Fichier_Lata::APPEND, format_, options_para_);

    // XXX Elie Saikali : attention offset ici avant write_doubletab ! sinon decalage d'un champ !
    if (un_seul_fichier_lata_)
      if (fichier_champ.is_master())
        offset_elem_ = fichier_champ.get_SFichier().get_ofstream().tellp();

    filename_champ = fichier_champ.get_filename();
    size_tot = write_doubletab(fichier_champ, valeurs, nb_compo, options_para_);
  }

  // Ouverture du fichier .lata en mode append.
  // Ajout de la reference au champ
  Fichier_Lata_maitre fichier(lata_basename_, extension_lata(), Fichier_Lata::APPEND, options_para_);
  SFichier& sfichier = fichier.get_SFichier();
  if (fichier.is_master())
    {
      sfichier << "Champ " << id_champ << " ";
      sfichier << remove_path(filename_champ);
      sfichier << " geometrie=" << id_du_domaine;

#ifdef INT_is_64_
      // The string "INT64\n" is written in clear text at the begining of each sub-file when we are 64bits.
      // This makes 6 bytes that we have to skip to get to the core of the (binary) data.
      sfichier << " file_offset=6";
#endif

      sfichier << " localisation=" << localisation;
      sfichier << " size=" << size_tot;
      sfichier << " nature=" << nature;
      sfichier << " noms_compo=" << noms_compo[0];
      for (int k = 1; k < noms_compo.size(); k++)
        sfichier << "," << noms_compo[k];

      sfichier << " composantes=" << nb_compo;

      if (un_seul_fichier_lata_)
        sfichier << " file_offset=" << (int)offset_elem_ << finl;
      else
        sfichier << finl;
    }
  fichier.syncfile();

  return 1;
}

/*! @brief voir Format_Post_base::ecrire_champ ATTENTION: si "reference" est non vide on ajoute 1 a toutes les
 *
 *    valeurs pour passer en numerotation fortran, et si de plus on ecrit un fichier lata unique pour tous les processeurs, on ajoute un
 *    decalage a toutes les valeurs (renumerotation des indices pour passer en numerotation globale, voir le codage de ecrire_domaine par exemple)
 *
 */
int Format_Post_Lata::ecrire_item_int(const Nom& id_item, const Nom& id_du_domaine, const Nom& id_domaine, const Nom& localisation, const Nom& reference, const IntVect& val, const int reference_size)
{
  // Construction du nom du fichier
  Nom basename_champ(lata_basename_), extension_champ(extension_lata());

  if (un_seul_fichier_lata_) extension_champ += "_single";
  else
    {
      extension_champ += ".";
      //extension_champ += id_champ;
      extension_champ += id_item;
      extension_champ += ".";
      extension_champ += localisation;
      extension_champ += ".";
      extension_champ += id_du_domaine;
      extension_champ += ".";
      char str_temps[100] = "0.0";
      if (temps_courant_ >= 0.)
        snprintf(str_temps, 100, "%.10f", temps_courant_);
      extension_champ += Nom(str_temps);
    }

  Nom filename_champ;
  int size_tot, nb_compo;
  const IntTab& valeurs = static_cast<const IntTab&>(val);
  {
    Fichier_Lata fichier_champ(basename_champ, extension_champ, offset_elem_ < 0 ? Fichier_Lata::ERASE : Fichier_Lata::APPEND, format_, options_para_);

    if (un_seul_fichier_lata_)
      if (fichier_champ.is_master())
        offset_elem_ = fichier_champ.get_SFichier().get_ofstream().tellp();

    filename_champ = fichier_champ.get_filename();
    // On suppose que si reference est non vide, c'est un indice dans un autre tableau, donc numerotation fortran :
    int decal = 0;
    int decal_partiel = 0;
    if (reference != "")
      {
        decal = 1;
        decal_partiel = 1;
        if (options_para_ == SINGLE_FILE || options_para_ == SINGLE_FILE_MPIIO)
          {
            // Tous les processeurs ecrivent dans un fichier unique, il faut renumeroter les indices pour passer en numerotation globale.
            // Decalage a ajouter aux indices pour avoir une numerotation globale.
            decal_partiel += mppartial_sum(reference_size);
          }
      }
    size_tot = write_inttab(fichier_champ, decal, decal_partiel, valeurs, nb_compo, options_para_);
  }

  {
    // Ouverture du fichier .lata en mode append. Ajout de la reference au champ
    Fichier_Lata_maitre fichier(lata_basename_, extension_lata(), Fichier_Lata::APPEND, options_para_);
    SFichier& sfichier = fichier.get_SFichier();
    if (fichier.is_master())
      {
        sfichier << "Champ " << id_item << " ";
        sfichier << remove_path(filename_champ);
        sfichier << " geometrie=" << id_du_domaine;

#ifdef INT_is_64_
        // The string "INT64\n" is written in clear text at the begining of each sub-file when we are 64bits.
        // This makes 6 bytes that we have to skip to get to the core of the (binary) data.
        sfichier << " file_offset=6";
#endif

        if (localisation != "") sfichier << " localisation=" << localisation;
        sfichier << " size=" << size_tot;
        sfichier << " composantes=" << nb_compo;
        if (reference != "") sfichier << " reference=" << reference;

        if (un_seul_fichier_lata_)
          sfichier << " file_offset=" << (int)offset_elem_;

        const int sz = (int) sizeof(_LATA_INT_TYPE_);
        switch(sz)
          {
          case 4:
            sfichier << " format=int32";
            break;
          case 8:
            sfichier << " format=int64";
            break;
          default:
            Cerr << "Error in Format_Post_Lata::ecrire_champ_lata\n" << " Integer type not supported: size=" << sz << finl;
            exit();
          }
        sfichier << finl;
      }
    fichier.syncfile();
  }

  // Astuce pour les donnees paralleles des faces:
  if ((id_item == "FACES" && Process::nproc() > 1) && (options_para_ == SINGLE_FILE || options_para_ == SINGLE_FILE_MPIIO))
    {
      const int n = valeurs.dimension(0);
      IntTab data(1,2);
      data(0, 0) = 1 + mppartial_sum(n);
      data(0, 1) = n;
      ecrire_item_int("JOINTS_FACES",
                      id_du_domaine,
                      "", /* id_domaine */
                      "", /* localisation */
                      "", /* reference */
                      data,
                      0); /* reference_size */
    }
  return 1;
}

int Format_Post_Lata::ecrire_entete_lata(const Nom& base_name, const Options_Para& option, const Format& format, const int est_le_premier_post)
{
  if (est_le_premier_post)
    {
      // Determination du format binaire:
      //  big endian => l'entier 32 bits "1" s'ecrit 0x00 0x00 0x00 0x01
      //  little endian =>                           0x01 0x00 0x00 0x00
      const unsigned int one = 1;
      const int big_endian = (*((unsigned char*) &one) == 0) ? 1 : 0;

      // Effacement du fichier .lata et ecriture de l'entete
      Fichier_Lata_maitre fichier(base_name, extension_lata(), Fichier_Lata::ERASE, option);

      SFichier& sfichier = fichier.get_SFichier();
      if (fichier.is_master())
        {
          sfichier << "LATA_V2.1 Trio_U Version1.6 or later" << finl;
          sfichier << Objet_U::nom_du_cas() << finl;
          sfichier << "Trio_U verbosity=0" << finl;

          sfichier << "Format ";
          switch(format)
            {
            case ASCII:
              sfichier << "ASCII,";
              break;
            case BINAIRE:
              sfichier << "BINAIRE,";
              if (big_endian)
                sfichier << "BIG_ENDIAN,";
              else
                sfichier << "LITTLE_ENDIAN,";
              break;
            default:
              Cerr << "Error in Format_Post_Lata::ecrire_entete\n" << " format not supported" << finl;
              exit();
            }
          switch(sizeof(_LATA_INT_TYPE_))
            {
            case 4:
              sfichier << "INT32,";
              break;
            case 8:
              sfichier << "INT64,";
              break;
            default:
              Cerr << "Error in Format_Post_Lata::ecrire_entete\n" << " sizeof(int) not supported" << finl;
              exit();
            }
          sfichier << "F_INDEXING,C_ORDERING,F_MARKERS_SINGLE,REAL32" << finl;
        }
      fichier.syncfile();

    }

  return 1;
}

int Format_Post_Lata::ecrire_temps_lata(const double temps, double& temps_format, const Nom& base_name, Status& stat, const Options_Para& option)
{
  assert(stat != RESET);
  // On ecrit le temps que si ce n'est pas le meme...
  if (stat != WRITING_TIME || temps_format != temps)
    {
      temps_format = temps;
      // Ouverture du fichier .lata en mode append
      Fichier_Lata_maitre fichier(base_name, extension_lata(), Fichier_Lata::APPEND, option);
      if (fichier.is_master())
        fichier.get_SFichier() << "TEMPS " << temps << finl;
      fichier.syncfile();
      stat = WRITING_TIME;
    }
  return 1;
}

int Format_Post_Lata::finir(const int est_le_dernier_post)
{
  if (est_le_dernier_post)
    {
      Fichier_Lata_maitre fichier(lata_basename_, extension_lata(), Fichier_Lata::APPEND, options_para_);
      SFichier& sfichier = fichier.get_SFichier();
      if (fichier.is_master()) sfichier << "FIN" << finl;
      fichier.syncfile();
    }
  return 1;
}
