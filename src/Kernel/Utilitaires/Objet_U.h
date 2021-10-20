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
// File:        Objet_U.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/21
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Objet_U_included
#define Objet_U_included

//     On rencontre des fous
//     qui #define min max ou throw!
#undef min
#undef max
#undef throw

#include <assert.h>
#include <DecBaseInst.h>
#include <EntreeSortie.h>
#include <Process.h>

class Type_info;
class Interprete;
class Motcle;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Objet_U
//     Cette classe est la classe de base des Objets de TRUST
//     Dans les classes derivees de Objet_U, on ajoute toujours
//      une macro Declare_instanciable ou Declare_base qui donne aux
//      objets les proprietes suivantes:
//     Un Objet_U peut etre lu dans une Entree ou ecrit dans une Sortie
//      (entree/sortie standard, jeu de donnees .data, fichier disque,
//       buffer memoire, buffer de communication parallele).
//       Il faut donc toujours implementer les methodes readOn et printOn
//     Un Objet_U d'un type quelconque peut etre instancie grace a une
//      chaine de caracteres qui l'identifie (que_suis_je()), voir DERIV::typer
//     Un Objet_U peut etre "sauvegarde" ou "repris" sur disque (au sens sauvegarde
//      et reprise d'un calcul). Ces operations sont differentes de readOn/printOn
//      car elles permettent eventuellement une redistribution des donnees paralleles
//      ou des changements de version.
//     Un Objet_U, s'il est "Declare_instanciable", peut etre cree dynamiquement et lu
//      dans le jeu de donnees TRUST (a travers readOn). Il possede alors le nom
//      (le_nom()) attribue dans le jeu de donnees (voir les classes Interprete_bloc et Lire)
//     Un Objet_U fait l'objet d'une gestion memoire speciale par le noyau permettant
//      le debuggage et l'optimisation (operations specifiques a la creation,
//      a la destruction et a la copie)
// .SECTION voir aussi
//      Memoire Ref_ Deriv_ Process
//     Classe abstraite
//////////////////////////////////////////////////////////////////////////////
class Objet_U : public Process
{
public:
  friend Entree& operator>>(Entree&, Objet_U&);
  friend class Sortie;

  virtual       ~Objet_U();
  int        numero() const;
  int        get_object_id() const;
  static int dimension;
  static double precision_geom;
  static int format_precision_geom;
  static int axi;
  static int bidim_axi;
  static int DEACTIVATE_SIGINT_CATCH; // flag to not enter the overloaded function signal_callback_handler
  static const Nom& nom_du_cas();
  static Nom& get_set_nom_du_cas();

  static Type_info         info_obj;
  virtual const Type_info* get_info() const;
  static const Type_info*  info();

  virtual const Nom& le_nom() const;
  const Nom&         que_suis_je() const;
  virtual void       nommer(const Nom&);
  const char*        le_type() const;

  virtual int    est_egal_a(const Objet_U&) const;
  friend int     operator ==(const Objet_U&, const Objet_U&);
  friend int     operator !=(const Objet_U&, const Objet_U&);
  virtual int    duplique()  const =0;
  virtual unsigned  taille_memoire() const =0;
  virtual int    change_num(const int* const );
  virtual Sortie&   printOn(Sortie& ) const;
  virtual Entree&   readOn(Entree& ) ;
  virtual int lire_motcle_non_standard(const Motcle& motlu, Entree& is);
  virtual int    reprendre(Entree& ) ;
  virtual int    sauvegarder(Sortie& ) const;
  virtual int    associer_(Objet_U&) ;
  const Interprete& interprete() const;
  Interprete& interprete();
  /* methode rajoutee pour caster en python */
  static const Objet_U& self_cast(const Objet_U&);
  static Objet_U& self_cast( Objet_U&);

  static int disable_TU; // Flag to disable the writing of the .TU files
protected:
  Objet_U();
  Objet_U(const Objet_U&);
  const Objet_U& operator=(const Objet_U&);

private:
  // Numero de l'objet (indice de l'objet dans Memoire::data)
  // Ce numero peut changer entre la construction et la destruction.
  int _num_obj_;
  // Identifiant unique de l'objet (attribue par le constructeur,
  // et jamais modifie ensuite).
  const int object_id_;

  // Compteur d'objets crees (incremente par le constructeur).
  static int static_obj_counter_;
  static Interprete* l_interprete;
};

int operator==(const Objet_U& x, const Objet_U& y) ;
int operator!=(const Objet_U& x, const Objet_U& y) ;

#endif


