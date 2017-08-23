/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Objet_U.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/29
//
//////////////////////////////////////////////////////////////////////////////

#include <Objet_U.h>
#include <Interprete_bloc.h>
#include <Lire.h>

int Objet_U::dimension=0;
double Objet_U::precision_geom=1e-10;
int Objet_U::format_precision_geom=11;
int Objet_U::axi=0;
int Objet_U::bidim_axi=0;
int Objet_U::static_obj_counter_=0;
Interprete* Objet_U::l_interprete=0;

Type_info Objet_U::info_obj("Objet_U");

// Permet de debugger en remontant a la creation d'un objet donne:
static int object_id_to_track = -1;

// Fonction utilitaire pour intercepter la creation d'un objet
// ATTENTION: depuis gcc3, le compilo genere plusieurs routines pour
//  chaque constructeur: au moins Objet_U::Objet_U() et Objet_U::Objet_U$base()
//  Cela rend difficile l'utilisatino de breakpoints dans ces routines.
//  Poser le breakpoint ici:
static void object_trap(int obj_id)
{
  Cerr << "Objet_U::Objet_U() : object_id_=" << obj_id << finl;
}

// Description:
//   Constructeur par defaut : attribue un numero d'identifiant unique
//   a l'objet (object_id_), et enregistre l'objet en "memoire"
//   en lui donnant un numero _num_obj_. L'object_id_ et tres
//   pratique pour debugger...
Objet_U::Objet_U() :  Process(),
  object_id_(static_obj_counter_++)
{
  int obj_id = object_id_;
  if (obj_id == object_id_to_track)
    {
      object_trap(obj_id);
    }
  Memoire& memoire = Memoire::Instance();
  _num_obj_ = memoire.add(this);
}

// Description:
//   Constructeur par copie. Cree un nouveau numero d'objet
//   (ne copie pas le numero de l'autre objet !)
Objet_U::Objet_U(const Objet_U&) : Process(),
  object_id_(static_obj_counter_++)
{
  int obj_id = object_id_;
  if (obj_id == object_id_to_track)
    {
      object_trap(obj_id);
    }
  Memoire& memoire = Memoire::Instance();
  _num_obj_ = memoire.add(this);
}
// Description:
//  Destructeur, supprime l'objet de la liste d'objets enregistres
//  dans "memoire".
// Effets de bord:
//     Suppression de l'objet dans la memoire de TRUST
// Postcondition:
Objet_U::~Objet_U()
{
  Memoire& memoire = Memoire::Instance();
  memoire.suppr(_num_obj_);
  _num_obj_ = -2; // Paranoia
}

// Description:
//   Operateur= : ne fait rien (on conserve le numero et l'identifiant)
const Objet_U& Objet_U::operator=(const Objet_U&)
{
  return *this;
}

// Description:
//   Renvoie l'indice de l'objet dans Memoire::data
int Objet_U::numero() const
{
  return _num_obj_;
}

// Description:
//   Renvoie l'identifiant unique de l'objet object_id_
int Objet_U::get_object_id() const
{
  return object_id_;
}

// Description:
//     renvoie la chaine identifiant la classe.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: const Nom&
//    Signification: Nom identifiant la classe de l'objet
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const Nom& Objet_U::que_suis_je() const
{
  return get_info()->name();
}


// Description:
//     Ecriture de l'objet sur un flot de sortie
//     Methode a surcharger
// Precondition:
// Parametre: Sortie& s
//    Signification: flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
//    A surcharger. Le flot de sortie n'est pas modifie dans cette implementation
// Postcondition:
Sortie& Objet_U::printOn(Sortie& s) const
{
  return s;
}


// Description:
//     Lecture d'un Objet_U sur un flot d'entree
//     Methode a surcharger
// Precondition:
// Parametre: Entree& s
//    Signification: flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree& s
//    Signification: flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
//    A surcharger. Le flot de sortie n'est pas modifie dans cette implementation
// Postcondition:
Entree& Objet_U::readOn(Entree& s)
{
  return s;
}
// Description:
//     Lecture des parametres de type non simple d'un objet_U
//     a partir d'un flot d'entree.
// Precondition:
// Parametre: Motcle& motlu
//    Signification: le nom du terme a lire
//    Valeurs par defaut:
//    Acces: entree
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
int Objet_U::lire_motcle_non_standard(const Motcle& motlu, Entree& is)
{
  Cerr<<"lire_motcle_non_standard not coded by "<<que_suis_je()<<finl;
  exit();
  return -1;
}

// Description:
//     Reprise d'un Objet_U sur un flot d'entree
//     Methode a surcharger : retourne toujours 1 dans cette implementation
// Precondition:
// Parametre: Entree&
//    Signification: flot d'entree a utiliser pour la reprise
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: int
//    Signification: code de retour
//    Contraintes:
// Exception:
// Effets de bord:
//     Modification de l'Objet_U
//     Methode a surcharger. Ne modifie pas l'objet et retourne toujours 1 dans cette implementation
// Postcondition:
int Objet_U::reprendre(Entree& )
{
  return 1;
}


// Description:
//     Sauvegarde d'un Objet_U sur un flot de sortie
//     Methode a surcharger : retourne toujours 1 dans cette implementation
// Precondition:
// Parametre: Sortie&
//    Signification: flot de sortie a utiliser pour la sauvegarde
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: int
//    Signification: code de retour
//    Contraintes:
// Exception:
// Effets de bord:
//     Ne doit pas modifier l'objet ???
// Postcondition:
int Objet_U::sauvegarder(Sortie& ) const
{
  return 0;
}

// Description:
//     Donne le nom de l'Objet_U
//     Methode a surcharger : renvoie "neant" dans cette implementation
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: le nom de l'Objet_U
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const Nom& Objet_U::le_nom() const
{
  static Nom inconnu="neant";
  return inconnu;
}



// Description:
//     Donne des informations sur le type de l'Objet_U
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: const Type_info*
//    Signification: structure regroupant les informations sur le type de l'Objet_U
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const Type_info*  Objet_U::get_info() const
{
  return &info_obj;
}


// Description:
//     Donne des informations sur le type de l'Objet_U
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: const Type_info*
//    Signification: structure regroupant les informations sur le type de l'Objet_U
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const Type_info*  Objet_U::info()
{
  return &info_obj;
}

// Description:
//  Renvoie une reference constante vers le nom du cas.
//  Cette methode est statique.
const Nom& Objet_U::nom_du_cas()
{
  return get_set_nom_du_cas();
}

// Description:
//  Renvoie une reference non constante vers le nom du cas
//  (pour pouvoir le modifier).
//  Cette methode est statique.
Nom& Objet_U::get_set_nom_du_cas()
{
  // Cet objet statique est construit au premier appel
  // a la fonction.
  static Nom nom_du_cas_;
  return nom_du_cas_;
}

// Description:
//  Renvoie 1 si l'objet x et *this sont une seule et meme instance
//  (meme adresse en memoire).
int Objet_U::est_egal_a(const Objet_U& x) const
{
  int resultat;
  if (&x==this)
    resultat = 1;
  else
    resultat = 0;
  return resultat;
}
// Description:
// methode ajoutee pour caster en python
const Objet_U& Objet_U::self_cast(const Objet_U& obj)
{
  return ref_cast_non_const(Objet_U,obj);
}
Objet_U& Objet_U::self_cast(Objet_U& obj)
{
  return ref_cast_non_const(Objet_U,obj);
}

// Description:
//     Change le numero interne de l'Objet_U
// Precondition:
// Parametre: const int* const new_ones
//    Signification: tableau compose des nouveaux numeros, indexe par les anciens numeros
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nouveau numero affecte a l'Objet_U
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Objet_U::change_num(const int* const new_ones)
{
  return _num_obj_=new_ones[_num_obj_];
}


// Description:
//     Donne le nom du type de l'Objet_U
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: const char*
//    Signification: chaine de caracteres representant le type de l'Objet_U
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const char* Objet_U::le_type() const
{
  return get_info()->name();
}

// Description:
//     Associe l'Objet_U a un autre Objet_U
//     Methode virtuelle a surcharger
// Precondition:
// Parametre: Objet_U&
//    Signification: l'Objet_U avec lequel doit s'effectuer l'association
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: code de retour
//    Contraintes:
// Exception:
// Effets de bord:
//     Cette methode virtuelle ne fait rien et retourne toujours 0
//     On peut supposer que l'association peut modifier les deux Objet_U concernes
// Postcondition:
int Objet_U::associer_(Objet_U&)
{
  return 0;
}

static Interprete& interprete_bidon()
{
  static Lire bidon;
  return bidon;
}
// BM : a supprimer a terme (utiliser Interprete::objet)
const Interprete& Objet_U::interprete() const
{
  return interprete_bidon();
}

// BM : a supprimer a terme (utiliser Interprete::objet)
Interprete& Objet_U::interprete()
{
  return interprete_bidon();
}


// Description:
//     Donne un nom a l'Objet_U
//     Methode virtuelle a surcharger
// Precondition:
// Parametre: const Nom&
//    Signification: le nom a affectuer a l'Objet_U
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
//     Cette implementation ne fait rien
// Postcondition:
void Objet_U::nommer(const Nom&)
{
}

// Description:
//   retourne x.est_egal_a(y)
// Precondition:
// Parametre: const Objet_U& x
//    Signification: Objet_U a utiliser pour la comparaison
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: const Objet_U& y
//    Signification: Objet_U a utiliser pour la comparaison
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: int
//    Signification: 1 si les deux Objet_U sont egaux, 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int operator==(const Objet_U& x, const Objet_U& y)
{
  return x.est_egal_a(y);
}

// Description:
//     Comparaison de deux Objet_U x et y
//     Renvoie 1 - x.est_egal_a(y);
// Precondition:
// Parametre: const Objet_U& x
//    Signification: Objet_U a utiliser pour la comparaison
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Parametre: const Objet_U& y
//    Signification: Objet_U a utiliser pour la comparaison
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: int
//    Signification: 1 si les deux Objet_U sont differents, 0 s'ils sont egaux
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int operator!=(const Objet_U& x, const Objet_U& y)
{
  return (1-(x.est_egal_a(y)));
}

