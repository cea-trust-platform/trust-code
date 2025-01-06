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

#include <Objet_U.h>
#include <Nom.h>

#ifndef LATATOOLS
#include <Memoire.h>
#include <Interprete_bloc.h>
#include <Lire.h>

int Objet_U::dimension=0;
int Objet_U::format_precision_geom=11;
int Objet_U::axi=0;
int Objet_U::bidim_axi=0;
int Objet_U::static_obj_counter_=0;
int Objet_U::DEACTIVATE_SIGINT_CATCH=0;
Interprete* Objet_U::l_interprete=0;

int Objet_U::disable_TU=0; // Flag to disable or not the writing of the .TU files
bool Objet_U::stat_per_proc_perf_log=false; // Flag to enable the writing of the statistics detailed per processor in _csv.TU file
Type_info Objet_U::info_obj("Objet_U");

// Permet de debugger en remontant a la creation d'un objet donne:
static int object_id_to_track = -1;

// Fonction utilitaire pour intercepter la creation d'un objet
// ATTENTION: depuis gcc3, le compilo genere plusieurs routines pour
//  chaque constructeur: au moins Objet_U::Objet_U() et Objet_U::Objet_U$base()
//  Cela rend difficile l'utilisation de breakpoints dans ces routines.
//  Poser le breakpoint ici:
static void object_trap(int obj_id)
{
  Cerr << "Objet_U::Objet_U() : object_id_=" << obj_id << finl;
}

/*! @brief Constructeur par defaut : attribue un numero d'identifiant unique a l'objet (object_id_), et enregistre l'objet en "memoire"
 *
 *    en lui donnant un numero _num_obj_. L'object_id_ et tres
 *    pratique pour debugger...
 *
 */
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

/*! @brief Constructeur par copie.
 *
 * Cree un nouveau numero d'objet (ne copie pas le numero de l'autre objet !)
 *
 */
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

/*! @brief Operateur= : ne fait rien (on conserve le numero et l'identifiant)
 *
 */
const Objet_U& Objet_U::operator=(const Objet_U&)
{
  return *this;
}

/*! @brief Renvoie l'identifiant unique de l'objet object_id_
 *
 */
int Objet_U::get_object_id() const
{
  return object_id_;
}

/*! @brief renvoie la chaine identifiant la classe.
 *
 * @return (const Nom&) Nom identifiant la classe de l'objet
 */
const Nom& Objet_U::que_suis_je() const
{
  return get_info()->name();
}

/*! @brief Lecture des parametres de type non simple d'un objet_U a partir d'un flot d'entree.
 *
 * @param (Motcle& motlu) le nom du terme a lire
 * @param (Entree& is) un flot d'entree
 * @return (int) negative value if the keyword is not understood, otherwise positive value.
 */
int Objet_U::lire_motcle_non_standard(const Motcle& motlu, Entree& is)
{
  Cerr << "The method " << __func__ << " must be overloaded in " << que_suis_je() << " !!!!" << finl;
  Process::exit();
  return -1;
}

/*! @brief Donne des informations sur le type de l'Objet_U
 *
 * @return (const Type_info*) structure regroupant les informations sur le type de l'Objet_U
 */
const Type_info*  Objet_U::get_info() const
{
  return &info_obj;
}


/*! @brief Donne des informations sur le type de l'Objet_U
 *
 * @return (const Type_info*) structure regroupant les informations sur le type de l'Objet_U
 */
const Type_info*  Objet_U::info()
{
  return &info_obj;
}

/*! @brief Renvoie une reference constante vers le nom du cas.
 *
 * Cette methode est statique.
 *
 */
const Nom& Objet_U::nom_du_cas()
{
  return get_set_nom_du_cas();
}

/*! @brief Renvoie une reference non constante vers le nom du cas (pour pouvoir le modifier).
 *
 *   Cette methode est statique.
 *
 */
Nom& Objet_U::get_set_nom_du_cas()
{
  // Cet objet statique est construit au premier appel
  // a la fonction.
  static Nom nom_du_cas_;
  return nom_du_cas_;
}

/*! @brief methode ajoutee pour caster en python
 *
 */
const Objet_U& Objet_U::self_cast(const Objet_U& obj)
{
  return ref_cast_non_const(Objet_U,obj);
}
Objet_U& Objet_U::self_cast(Objet_U& obj)
{
  return ref_cast_non_const(Objet_U,obj);
}

/*! @brief Change le numero interne de l'Objet_U
 *
 * @param (const int* const new_ones) tableau compose des nouveaux numeros, indexe par les anciens numeros
 * @return (int) le nouveau numero affecte a l'Objet_U
 */
int Objet_U::change_num(const int* const new_ones)
{
  return _num_obj_=new_ones[_num_obj_];
}


/*! @brief Donne le nom du type de l'Objet_U
 *
 * @return (const char*) chaine de caracteres representant le type de l'Objet_U
 */
const char* Objet_U::le_type() const
{
  return get_info()->name();
}

/*! @brief Associe l'Objet_U a un autre Objet_U Methode virtuelle a surcharger
 *
 * @param (Objet_U&) l'Objet_U avec lequel doit s'effectuer l'association
 * @return (int) code de retour
 */
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

/*! @brief retourne x.
 *
 * est_egal_a(y)
 *
 * @param (const Objet_U& x) Objet_U a utiliser pour la comparaison
 * @param (const Objet_U& y) Objet_U a utiliser pour la comparaison
 * @return (int) 1 si les deux Objet_U sont egaux, 0 sinon
 */
int operator==(const Objet_U& x, const Objet_U& y)
{
  return x.est_egal_a(y);
}

/*! @brief Comparaison de deux Objet_U x et y Renvoie 1 - x.
 *
 * est_egal_a(y);
 *
 * @param (const Objet_U& x) Objet_U a utiliser pour la comparaison
 * @param (const Objet_U& y) Objet_U a utiliser pour la comparaison
 * @return (int) 1 si les deux Objet_U sont differents, 0 s'ils sont egaux
 */
int operator!=(const Objet_U& x, const Objet_U& y)
{
  return (1-(x.est_egal_a(y)));
}

#endif   // LATATOOLS

double Objet_U::precision_geom = 1e-10;

/*! @brief Destructeur, supprime l'objet de la liste d'objets enregistres dans "memoire".
 *
 */
Objet_U::~Objet_U()
{
#ifndef LATATOOLS
  Memoire& memoire = Memoire::Instance();
  memoire.suppr(_num_obj_);
  _num_obj_ = -2; // Paranoia
#endif
}

/*! @brief Renvoie l'indice de l'objet dans Memoire::data
 *
 */
int Objet_U::numero() const
{
#ifndef LATATOOLS
  return _num_obj_;
#else
  return 0;
#endif
}

/*! @brief Ecriture de l'objet sur un flot de sortie Methode a surcharger
 *
 * @param (Sortie& s) flot de sortie
 * @return (Sortie&) flot de sortie modifie
 */
Sortie& Objet_U::printOn(Sortie& s) const
{
  return s;
}


/*! @brief Lecture d'un Objet_U sur un flot d'entree Methode a surcharger
 *
 * @param (Entree& s) flot d'entree
 * @return (Entree& s) flot d'entree modifie
 */
Entree& Objet_U::readOn(Entree& s)
{
  return s;
}

/*! @brief Renvoie 1 si l'objet x et *this sont une seule et meme instance (meme adresse en memoire).
 *
 */
int Objet_U::est_egal_a(const Objet_U& x) const
{
#ifndef LATATOOLS
  int resultat;
  if (&x==this)
    resultat = 1;
  else
    resultat = 0;
  return resultat;
#else
  return 0;
#endif
}

/*! @brief Donne le nom de l'Objet_U Methode a surcharger : renvoie "neant" dans cette implementation
 *
 * @return (Nom&) le nom de l'Objet_U
 */
const Nom& Objet_U::le_nom() const
{
  static Nom inconnu="neant";
  return inconnu;
}

/*! @brief Donne un nom a l'Objet_U Methode virtuelle a surcharger
 *
 * @param (const Nom&) le nom a affectuer a l'Objet_U
 */
void Objet_U::nommer(const Nom&)
{
}

/*! @brief Reprise d'un Objet_U sur un flot d'entree Methode a surcharger
 *
 * @param (Entree&) flot d'entree a utiliser pour la reprise
 * @return (int) code de retour
 */
int Objet_U::reprendre(Entree&)
{
#ifndef LATATOOLS
  Cerr << "The method " << __func__ << " must be overloaded in " << que_suis_je() << " !!!!" << finl;
  Process::exit();
#endif
  return 1;
}

/*! @brief Sauvegarde d'un Objet_U sur un flot de sortie Methode a surcharger
 *
 * @param (Sortie&) flot de sortie a utiliser pour la sauvegarde
 * @return (int) code de retour
 */
int Objet_U::sauvegarder(Sortie& ) const
{
#ifndef LATATOOLS
  Cerr << "The method " << __func__ << " must be overloaded in " << que_suis_je() << " !!!!" << finl;
  Process::exit();
#endif
  return 0;
}
