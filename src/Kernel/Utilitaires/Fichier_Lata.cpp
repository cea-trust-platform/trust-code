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

#include <EcrFicPartageMPIIO.h>
#include <EcrFicPartageBin.h>
#include <Fichier_Lata.h>

/*! @brief Construit un fichier de type EcrFicPartage(Bin) ou EcrFicPrive(Bin), binaire ou pas selon le parametre "format".
 *
 *   Si parallel==MULTIPLE_FILES, le fichier est de type EcrFicPrive(Bin).
 *     Dans ce cas, chaque processeur ouvre un fichier different, dont
 *     le nom est "basename_XXXXXextension", ou XXXXX est egal a Process::me().
 *     Tous les processeurs renverront is_master() == 1.
 *   Si parallel==SINGLE_FILE est non nul, le fichier est de type EcrFicPartage(Bin).
 *     Seul le processeur maitre ouvre le fichier, le nom du fichier est
 *     "basenameextension".
 *     is_master() renverra 1 sur le maitre, 0 sur les autres processeurs.
 *
 * @param (basename) debut du nom du fichier
 * @param (extension) fin du nom du fichier
 * @param (mode_append) Si mode_append==ERASE, on ouvre en mode ecriture, si mode_append==APPEND, on ouvre en mode append.
 * @param (format) Determine si on ouvre en binaire ou pas. (valeurs possibles: Format_Post_Lata::ASCII ou Format_Post_Lata::BINAIRE)
 * @param (parallel) fichier unique partage ou plusieurs fichiers prives...
 */
Fichier_Lata::Fichier_Lata(const char * basename, const char * extension,
                           Mode mode_append,
                           Format_Post_Lata::Format format,
                           Format_Post_Lata::Options_Para parallel) :
  filename_(""),
  fichier_(0),
  is_parallel_(0)
{
  char s[20] = "";

  switch(parallel)
    {
    case Format_Post_Lata::SINGLE_FILE_MPIIO:
    case Format_Post_Lata::SINGLE_FILE:
      {
        is_parallel_ = 1;
        filename_ = basename;
        filename_ += extension;
        // Pour un calcul sequentiel, on ouvre un fichier SFichier
        // pour ne pas bufferiser en memoire
        if  (Process::is_sequential())
          fichier_ = new SFichier;
        else
          {
            if (format == Format_Post_Lata::BINAIRE && parallel == Format_Post_Lata::SINGLE_FILE_MPIIO)
              fichier_ = new EcrFicPartageMPIIO;
            else
              fichier_ = new EcrFicPartage;
          }
        break;
      }
    case Format_Post_Lata::MULTIPLE_FILES:
      {
        is_parallel_ = 0;
        fichier_ = new SFichier;
        const int moi = Process::me();
        snprintf(s, 20, "_%05d", (True_int)moi);
        break;
      }
    default:
      Cerr << "Fichier_Lata::Fichier_Lata: parallel option not supported " << (int)parallel << finl;
      Process::exit();
    }
  filename_ = basename;
  filename_ += s;
  filename_ += extension;

  switch(format)
    {
    case Format_Post_Lata::ASCII:
      fichier_->set_bin(0);
      break;
    case Format_Post_Lata::BINAIRE:
      fichier_->set_bin(1);
      break;
    default:
      Cerr << "Fichier_Lata::Fichier_Lata: format not supported " << (int)format << finl;
      Process::exit();
    }


  IOS_OPEN_MODE mode = ios::out;
  switch(mode_append)
    {
    case ERASE:
      mode = ios::out;
      break;
    case APPEND:
      mode = ios::out | ios::app;
      break;
    default:
      Cerr << "Fichier_Lata::Fichier_Lata: open mode not supported " << (int)mode_append << finl;
      Process::exit();
    }
  //if (Process::je_suis_maitre() || parallel==Format_Post_Lata::MULTIPLE_FILES)
  {
    int ok = fichier_->ouvrir(filename_, mode);
    if (!ok)
      {
        Cerr << "Error in Fichier_Lata::Fichier_Lata\n"
             << " Error while opening file : " << filename_
             << finl;
        Process::exit();
      }
    fichier_->setf(ios::scientific);
    fichier_->precision(8);
  }
}

Fichier_Lata::~Fichier_Lata()
{
  if (fichier_)
    {
      delete fichier_;
      fichier_ = 0;
    }
}

SFichier& Fichier_Lata::get_SFichier()
{
  assert(fichier_);
  return *fichier_;
}

/*! @brief Renvoie le nom du fichier avec le path
 *
 */
const Nom& Fichier_Lata::get_filename() const
{
  return filename_;
}

/*! @brief Si le fichier est de type partage, renvoie 1 si me() est egal au master du groupe et 0 sinon,
 *
 *   Si le fichier est prive, renvoie 1 sur tous les processeurs.
 *
 */
int Fichier_Lata::is_master() const
{
  int resu = 0;
  if (is_parallel_ == 0)
    {
      // Execution sequentielle, fichiers prives chaque processeur est maitre
      resu = 1;
    }
  else
    {
      // Execution parallele : un seul maitre
      if (Process::je_suis_maitre())
        resu = 1;
    }
  return resu;
}

/*! @brief Si le fichier est de type partage, appelle la methode syncfile(), sinon ne fait rien.
 *
 */
void Fichier_Lata::syncfile()
{
  if (is_parallel_ && Process::is_parallel())
    fichier_->syncfile();
}

Fichier_Lata_maitre::Fichier_Lata_maitre(const char * basename,
                                         const char * extension,
                                         Mode mode_append,
                                         Format_Post_Lata::Options_Para parallel) :
  Fichier_Lata(basename, extension,
               mode_append, Format_Post_Lata::ASCII, parallel)
{
  fichier_->setf(ios::scientific);
  // On peut changer la precision du fichier maitre a cet endroit:
  fichier_->precision(8);
}
