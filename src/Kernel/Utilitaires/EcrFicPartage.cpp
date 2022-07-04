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

#include <EcrFicPartage.h>
#include <Statistiques.h>
#include <PE_Groups.h>
#include <Comm_Group.h>
#include <communications.h>

extern Stat_Counter_Id IO_EcrireFicPartageBin_counter_;

Implemente_instanciable_sans_constructeur_ni_destructeur(EcrFicPartage,"EcrFicPartage",SFichier);
Entree& EcrFicPartage::readOn(Entree& s)
{
  throw;
  return s;
}

Sortie& EcrFicPartage::printOn(Sortie& s) const
{
  throw;
  return s;
}
EcrFicPartage::EcrFicPartage() : SFichier()
{
  obuffer_ptr_ = new OBuffer;
  set_bin(0);
}

// Description:
//    Ouvre le fichier avec les parametres mode et prot donnes
//    Ces parametres sont les parametres de la methode open standard
EcrFicPartage::EcrFicPartage(const char* name,IOS_OPEN_MODE mode)
{
  obuffer_ptr_ = new OBuffer;
  set_bin(0);

  ouvrir(name, mode);
}

inline OBuffer& EcrFicPartage::get_obuffer()
{
  assert(obuffer_ptr_);
  return *obuffer_ptr_;
}

// Description:
//    Ouvre le fichier avec les parametres mode et prot donnes
//    Ces parametres sont les parametres de la methode open standard
// Precondition:
//    Tous les processeurs du groupe doivent executer ceci simultanement !
int EcrFicPartage::ouvrir(const char* name,IOS_OPEN_MODE mode)
{
  // Verification sanitaire : tous les processeurs sont la ?
  barrier();

  int ok = 1;
  if(je_suis_maitre())
    {
#ifdef FILESYSTEM_NON_GLOBAL
      nom_fic_ = pwd();
      nom_fic_ += "/";
      nom_fic_ += name;
#else
      nom_fic_ = name;
#endif
      // Seul le maitre ouvre le fichier
      ok = SFichier::ouvrir((const char *)nom_fic_, mode);
    }
  syncfile();

  // Modif B.Math. 22/09/2004: tous les processeurs passent par le buffer,
  //  y compris le maitre.
  get_obuffer().new_buffer();
  return ok;
}


// Description:
//     ferme le fichier
// Precondition:
//     Tous les processeurs doivent executer ceci simultanement !
EcrFicPartage::~EcrFicPartage()
{
  close();
  delete obuffer_ptr_;
  obuffer_ptr_ = 0;
}

void EcrFicPartage::close()
{
  // Verification sanitaire : tout le monde est la ?
  barrier();
  const int buflen = get_obuffer().len();
  if(buflen > 0)
    {
      Cerr << "***** WARNING : EcrFicPartage::close() ******* "<<nom_fic_
           << "\non PE " << me() << " the buffer is not empty\n"
           << "  (Missing syncfile) : one makes a last syncfile" << finl;
      Cerr<<get_obuffer().str()<<finl;
    }
#ifndef NDEBUG
  // Y a-t-il un processeur sur lequel il reste des donnees
  const int maxbuflen = mp_sum(buflen);
  if (maxbuflen > 0)
    syncfile();
#endif
  if(je_suis_maitre())
    SFichier::close();
#ifndef NDEBUG
  if (maxbuflen > 0)
    {
      Cerr<<"It missed a syncfile somewhere"<<finl;
      Cerr<<"Indeed, maxbuflen =" << maxbuflen << finl;
      Cerr<<"GF prefers to stop the calculation to correct "<<finl;
      exit();
    }
#endif
}
// Description:
//    Permet au processus appelant de bloquer en attente de la ressource commune a tous les processus qui est le fichier partage.
//    Si le processus appelant cette methode n'est pas le premier, il atend du processus precedent l'endroit ou il doit se positionner dans le fichier pour effectuer sa prochaine ecriture.
//    Cette methode est systematiquement appelee avant toute nouvelle ecriture dans le fichier.
// Retour: Sortie&
//    Signification: *this
Sortie& EcrFicPartage::lockfile()
{
  return *this;
}


// Description:
//    Permet de debloquer la ressource critique pour leprocessus suivant.
//    Le processus appelant, sauf si c'est le premier processus du groupe, envoie
//    sa position courante au precessus suivant dans le groupe. Cette methode est
//    a appeler apres chaque ecriture dans le fichier.
// Retour: Sortie&
//    Signification: *this
Sortie& EcrFicPartage::unlockfile()
{
  return *this;
}

// Description:
//  Provoque l'ecriture sur disque des donnees accumulees sur les differents processeurs
//  depuis le dernier appel a syncfile().
//  Les donnees sont ecrites dans l'ordre croissant des processeurs.
//  Cette fonction doit etre appelee le meme nombre de fois sur tous les processeurs !
//
//  Exemple:
//   processeur 0:                         processeur 1:
//    file << "pe0 : 1" << finl;            file << "pe1 : 1" << finl;
//    file << "pe0 : 2" << finl;            file << "pe1 : 2" << finl;
//    file.syncfile();                            file.syncfile();
//    file << "pe0 : 3" << finl;            file << "pe1 : 3" << finl;
//    file << "pe0 : 4" << finl;
//    file.syncfile();                      file.syncfile();
//    file << "pe0 : end" << finl;          // le processeur 1 n'ecrit pas de donnees
//    file.syncfile();                      file.syncfile();
//
//  Contenu du fichier :
//    pe0 : 1
//    pe0 : 2
//    pe1 : 1
//    pe1 : 2
//    pe0 : 3
//    pe0 : 4
//    pe1 : 3
//    pe0 : end
Sortie& EcrFicPartage::syncfile()
{
  // En mode ascii, les donnees sont converties en ascii lors de l'ecriture dans
  //  le buffer. Une deuxieme conversion a lieu lors de l'ecriture sur disque
  //  (selon le systeme d'exploitation, '\n' est code differemment par exemple)
  // Donc, en ascii, on utilisera
  //   file << buffer;
  // et en binaire
  //   file.write(buffer, size);
  // Or file << buffer determine la longueur du buffer en cherchant de caractere '\0'
  // Par consequent, en ascii, il faut que le buffer finisse par un caractere '\0'
  // que l'on ajoute ici:

  if (! bin_)
    {
      get_obuffer().put_null_char();
    }

  const Comm_Group& group = PE_Groups::current_group();
  if(je_suis_maitre())
    {
      int p;
      const int nb_proc = nproc();
      for(p=0; p<nb_proc; p++)
        {
          const char * buffer_data = 0;
          char * allocated_buffer = 0;
          int buf_size;

          // On recupere les donnees du processeur p, soit directement (p==me()),
          //  soit par communication :
          if (p == me())
            {
              // Ecriture de mes propres donnees : je les prends dans le buffer.
              // Ce pointeur peut etre nul:
              buffer_data = get_obuffer().str();
              buf_size = get_obuffer().len();
            }
          else
            {
              // Ecriture des donnees d'un autre processeur, je les recupere.
              int dummy = 0;
              envoyer(dummy, p, 100); // Faire coucou au processeur p pour qu'il envoie ses donnees
              recevoir(buf_size, p, 100);
              if (buf_size > 0)
                {
                  buffer_data = allocated_buffer = new char[buf_size];
                  group.recv(p, allocated_buffer, buf_size, 100);
                }
            }
          // Ecriture dans le fichier disque
          if (buf_size > 0)
            {
              assert(buffer_data);
              ostream& os = get_ostream();
              if (bin_)
                {
                  // Ecriture binaire sans conversion :
                  statistiques().begin_count(IO_EcrireFicPartageBin_counter_);
                  os.write(buffer_data, buf_size);
                  statistiques().end_count(IO_EcrireFicPartageBin_counter_, buf_size);
                }
              else
                {
                  // On verifie que le buffer finit bien par un caractere 0 :
                  assert(buffer_data[buf_size-1] == 0);
                  // Ecriture de buffer_data comme une chaine
                  // (conversion des \n sur certains systemes, etc...)
                  os << buffer_data;
                }
            }
          if (allocated_buffer)
            delete[] allocated_buffer;
        }
      // Force a tout ecrire sur le disque tout de suite:
      // (appel a la fonction flush de bas niveau, pas celle de trio).
      get_ostream().flush();
    }
  else
    {
      // Envoi du buffer au processeur maitre:
      // On attend qu'il demande les donnees pour ne pas engorger le reseau :
      // (sinon tous les processeurs envoient simultanement leurs donnees)
      int dummy;
      recevoir(dummy, 0, 100);
      int buf_size = get_obuffer().len();
      envoyer(buf_size, 0, 100);
      // Si la taille est non nulle, on envoie le buffer :
      if (buf_size > 0)
        {
          const char * buffer_data = get_obuffer().str();
          group.send(0, buffer_data, buf_size, 100);
        }
    }

  // On vide le buffer :
  get_obuffer().new_buffer();
  return *this;
}

void EcrFicPartage::precision(int i) { get_obuffer().precision(i); }

int EcrFicPartage::get_precision() { return get_obuffer().get_precision(); }

Sortie& EcrFicPartage::operator <<(const char* ob)
{
  get_obuffer() << ob;
  return *this;
}

Sortie& EcrFicPartage::operator <<(const std::string& str) { return operator_template<std::string>(str);}
Sortie& EcrFicPartage::operator <<(const Separateur& s) { return operator_template<Separateur>(s);}
Sortie& EcrFicPartage::operator <<(const Objet_U& ob) { return operator_template<Objet_U>(ob);}
Sortie& EcrFicPartage::operator <<(const int ob) { return operator_template<int>(ob);}
Sortie& EcrFicPartage::operator <<(const unsigned int ob) { return operator_template<unsigned int>(ob);}
Sortie& EcrFicPartage::operator <<(const float ob) { return operator_template<float>(ob);}
Sortie& EcrFicPartage::operator <<(const double ob) { return operator_template<double>(ob);}

#ifndef INT_is_64_
Sortie& EcrFicPartage::operator <<(const long ob) { return operator_template<long>(ob);}
Sortie& EcrFicPartage::operator <<(const unsigned long ob) { return operator_template<unsigned long>(ob);}
#endif

int EcrFicPartage::put(const unsigned* ob, int n, int pas) { return put_template<unsigned>(ob,n,pas); }
int EcrFicPartage::put(const int* ob, int n, int pas) { return put_template<int>(ob,n,pas); }
int EcrFicPartage::put(const float* ob, int n, int pas) { return put_template<float>(ob,n,pas); }
int EcrFicPartage::put(const double* ob, int n, int pas) { return put_template<double>(ob,n,pas); }

#ifndef INT_is_64_
int EcrFicPartage::put(const long* ob, int n, int pas) { return put_template<long>(ob,n,pas); }
#endif

int EcrFicPartage::set_bin(int bin)
{
  assert(bin==0 || bin==1);
  bin_ = bin;
  get_obuffer().set_bin(bin_);
  return bin_;
}

Sortie& EcrFicPartage::flush() { return (*this); }
