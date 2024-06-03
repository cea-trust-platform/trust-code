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

#include <LecFicDistribue.h>
#include <communications.h>
#include <Nom.h>

Implemente_instanciable_sans_constructeur_ni_destructeur(LecFicDistribue,"LecFicDistribue",EFichier);

Entree& LecFicDistribue::readOn(Entree& s)
{
  throw;
}

Sortie& LecFicDistribue::printOn(Sortie& s) const
{
  throw;
}

LecFicDistribue::LecFicDistribue(int)
{
  bin_=0;
}
LecFicDistribue::LecFicDistribue():EFichier()
{}
LecFicDistribue::~LecFicDistribue()
{
  EFichier::close();
}

/*! @brief Constructeur Ouvre le fichier avec les parametres mode et prot donnes
 *
 *     Ces parametres sont les parametres de la methode open standard
 *
 * @param (const char* name) nom du fichier
 * @param (int mode) parametre passe a open
 * @param (int prot) parametre passe a open
 */
LecFicDistribue::LecFicDistribue(const char* name, IOS_OPEN_MODE mode)
{
  LecFicDistribue::ouvrir(name, mode);
}


/*! @brief Ouvre le fichier avec les parametres mode et prot donnes Ces parametres sont les parametres de la methode open standard
 *
 * @param (const char* name) nom du fichier
 * @param (int mode) parametre passe a open
 * @param (int prot) parametre passe a open
 * @return (Entree&) *this
 */
int LecFicDistribue::ouvrir(const char* name, IOS_OPEN_MODE mode)
{
  Nom nom_fic(name);
  if(Process::nproc()>1)
    nom_fic=nom_fic.nom_me(Process::me());

#ifdef FILESYSTEM_NON_GLOBAL
  Nom localisation;
  if(Process::je_suis_maitre())
    localisation = pwd();
  envoyer_broadcast(localisation, 0);

  nom_fic = localisation + "/" + nom_fic;
#endif

  if (!EFichier::ouvrir((const char*)nom_fic,mode))
    {
      Cerr << "File " << nom_fic << " not found or could not be opened." << finl;
      return 0;
    }
  else
    {
      Process::Journal() << "File " << nom_fic << " is opened." << finl;
      return 1;
    }
}

