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
//////////////////////////////////////////////////////////////////////////////
//
// File:        EcrFicPartage.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef EcrFicPartage_included
#define EcrFicPartage_included

#include <SFichier.h>
#include <OBuffer.h>
#include <Nom.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Ecriture dans un fichier partage
//    Cette classe implemente les operateurs et les methodes virtuelles de la clase SFichier de la facon suivante :
//    un fichier physiquement localise sur le disque de la machine heberge la tache maitre de
//    l'application Trio-U (le processus de rang 0 dans le groupe "tous").
//    Chacun des processus ecrit ses donnees l'un apres l'autre, par ordre croissant de rang dans le groupe "tous"
//    La synchronisation est realisee par un systeme de semaphore.
//    Ce type de fichier est notamment utilise pour la creation d'un fichier de post-traitement unique.
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////

class OBuffer;

class EcrFicPartage : public SFichier
{
  // Chaque PE ecrit dans le meme() fichier
  Declare_instanciable_sans_constructeur_ni_destructeur(EcrFicPartage);
public:
  EcrFicPartage();
  EcrFicPartage(const char* name,IOS_OPEN_MODE mode=ios::out);
  int ouvrir(const char* name,IOS_OPEN_MODE mode=ios::out) override;
  ~EcrFicPartage() override;
  void close();
  Sortie& lockfile() override;
  Sortie& unlockfile() override;
  Sortie& syncfile() override;
  void precision(int) override;
  int get_precision() override;

  Sortie& flush() override;
  int set_bin(int bin) override;

  Sortie& operator <<(const char* ob) override;
  Sortie& operator <<(const Objet_U& ob) override;
  Sortie& operator <<(const std::string& ob) override;
  Sortie& operator <<(const Separateur& ) override;

  Sortie& operator <<(const unsigned ob) override;
  Sortie& operator <<(const int ob) override;
  Sortie& operator <<(const float ob) override;
  Sortie& operator <<(const double ob) override;
#ifndef INT_is_64_
  Sortie& operator <<(const long ob) override;
#endif

  int put(const unsigned* ob, int n, int pas) override;
  int put(const int* ob, int n, int pas) override;
  int put(const float* ob, int n, int pas) override;
  int put(const double* ob, int n, int pas) override;
#ifndef INT_is_64_
  int put(const long* ob, int n, int pas) override;
#endif

protected:
  Nom nom_fic_;

private:
  OBuffer& get_obuffer();
  OBuffer * obuffer_ptr_; // Pointeur : permet de ne pas inclure OBuffer.h

  template <typename _TYPE_>
  int put_template(const _TYPE_* ob, int n, int pas)
  {
    get_obuffer().put(ob,n,pas);
    return 1;
  }

  template <typename _TYPE_>
  Sortie& operator_template(const _TYPE_& ob)
  {
    get_obuffer() << ob;
    return *this;
  }
};

#endif /* EcrFicPartage_included */
