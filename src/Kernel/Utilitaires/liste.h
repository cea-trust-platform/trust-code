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

#ifndef liste_included
#define liste_included

#include <TRUST_Deriv.h>

/*! @brief Les classes liste et liste_curseur representent une liste de Deriv<Objet_U> et un curseur associe.
 *
 *             List<T> et List_Curseur<T> en heritent.
 *
 */
class liste : public Objet_U
{

  Declare_instanciable_sans_constructeur_ni_destructeur(liste);

public :
  inline liste();
  inline liste(const liste&);
  inline liste(const Objet_U&);
  ~liste() override;
  int size() const;
  int rang(const char* const) const;
  int contient(const char* const) const;
  void suppr(const char* const);
  inline int est_vide() const;
  inline int est_dernier() const;
  void vide();
  friend class liste_curseur;
  friend int operator==(const liste&, const liste&);
  friend int operator!=(const liste&, const liste&);

protected :
  inline Objet_U& operator()(int);
  inline const Objet_U& operator()(int) const;
  inline Objet_U& operator()(const Nom&);
  inline const Objet_U& operator()(const Nom&) const;
  inline Objet_U& valeur();
  inline const Objet_U& valeur() const;
  inline liste& operator +=(const Objet_U&);
  inline liste& operator +=(const liste&);
  inline liste& operator -=(const Objet_U&);
  inline const liste& suivant() const;
  inline liste& suivant();
  inline Objet_U* operator ->();
  inline const Objet_U* operator ->() const;

  Objet_U& operator[](int);
  const Objet_U& operator[](int) const;
  Objet_U& operator[](const Nom&);
  const Objet_U& operator[](const Nom&) const;
  Objet_U& add(const Objet_U&);
  Objet_U& add_deplace(DerObjU&);
  Objet_U& inserer(const Objet_U&);
  liste& add(const liste&);
  liste& inserer(const liste&);
  void supprimer();
  Objet_U& add_if_not(const Objet_U&);
  liste& search(const Objet_U&) const;
  liste& search(const char* const) const;
  int contient(const Objet_U&) const;
  int rang(const Objet_U&) const;
  void suppr(const Objet_U&);
  liste& operator=(const liste&);
  const liste& dernier() const;
  liste& dernier();
  DerObjU data;
  liste* suivant_;
};


/*! @brief Les classes liste et liste_curseur representent une liste de Deriv<Objet_U> et un curseur associe.
 *
 *             List<T> et List_Curseur<T> en heritent.
 *
 */
class liste_curseur
{
public :
  inline liste_curseur(const liste&);
  inline operator bool() const;
  inline void operator ++();
  inline const Objet_U& valeur() const;
  inline Objet_U& valeur();
  inline Objet_U* operator ->();
  inline const Objet_U* operator ->() const;
  inline liste& operator=(const liste&);
  inline const liste& list() const;
  inline liste& list() ;
private :
  liste* curseur ;
};


/*! @brief Constructeur par defaut Construit une liste vide
 *
 */
inline liste::liste()
{
  suivant_=this;
}

/*! @brief Constructeur Construit une liste singleton
 *
 * @param (const Objet_U& ob) l'Objet_U a inserer dans la liste
 */
inline liste::liste(const Objet_U& ob) : Objet_U(ob),data(ob), suivant_(0)
{
}

/*! @brief Constructeur Ajoute les elements contenus dans la liste passe en parametre
 *
 * @param (const liste& list) la liste des elements a inserer dans la liste construite
 */
inline liste::liste(const liste& a_list): Objet_U(a_list)
{
  suivant_=this;
  add(a_list);
}

/*! @brief Teste si la liste est vide
 *
 * @return (int) 1 si le liste est vide, 0 sinon
 */
inline int liste::est_vide() const
{
  return suivant_==this;
}

/*! @brief Teste si la liste comporte des elements suivants ou non.
 *
 * Retourne 1 si la liste est vide
 *
 * @return (int) 1 si la liste est le dernier element d'une liste chainee
 */
inline int liste::est_dernier() const
{
  return ( est_vide() || (!suivant_) );
}

/*! @brief Retourne le ieme element de la liste Sort en erreur si l'indice deborde de la liste
 *
 * @param (int i) l'indice de l'element desire
 * @return (Objet_U&) le ieme element de la liste
 */
inline Objet_U& liste::operator()(int i)
{
  return operator[](i);
}

/*! @brief Retourne le ieme element de la liste Sort en erreur si l'indice deborde de la liste
 *
 * @param (int i) l'indice de l'element desire
 * @return (const Objet_U&) le ieme element de la liste
 */
inline const Objet_U& liste::operator()(int i) const
{
  return operator[](i);
}

/*! @brief Retourne le premier element de la liste dont le Nom est nom Sort en erreur si aucun Objet_U n'est trouve
 *
 * @param (const Nom& nom) le nom de l'element desire
 * @return (Objet_U&) le premier element de la liste dont le Nom est nom
 */
inline Objet_U& liste::operator()(const Nom& nom)
{
  return operator[](nom);
}

/*! @brief Retourne le premier element de la liste dont le Nom est nom Sort en erreur si aucun Objet_U n'est trouve
 *
 * @param (const Nom& nom) le nom de l'element desire
 * @return (Objet_U&) le premier element de la liste dont le Nom est nom
 */
inline const Objet_U& liste::operator()(const Nom& nom) const
{
  return operator[](nom);
}

/*! @brief Retourne l'Objet_U associe a la cellule de la liste
 *
 * @return (Objet_U&) reference sur l'Objet_U courant
 */
inline Objet_U& liste::valeur()
{
  return data.valeur();
}

/*! @brief Retourne l'Objet_U associe a la cellule de la liste
 *
 * @return (const Objet_U&) reference sur l'Objet_U courant
 */
inline const Objet_U& liste::valeur() const
{
  return data.valeur();
}

/*! @brief Ajoute un Objet_U a la fin de la liste
 *
 * @param (const Objet_U& ob) l'Objet_U a ajouter
 */
inline liste& liste::operator +=(const Objet_U& ob)
{
  add(ob);
  return *this;
}

/*! @brief Ajoute une list d'Objet_U a la fin de la liste
 *
 * @param (const liste& list) la liste d'Objet_U a ajouter
 */
inline liste& liste::operator +=(const liste& a_list)
{
  return add(a_list);
}

/*! @brief Supprime un Objet_U de la liste Affiche un message d'erreur si l'objet n'est pas trouve
 *
 */
inline liste& liste::operator -=(const Objet_U& ob)
{
  suppr(ob);
  return *this;
}

/*! @brief Retourne la liste composee des elements suivants de la liste consideree
 *
 * @return (liste&) liste des elements suivants
 */
inline liste& liste::suivant()
{
  return *suivant_;
}

/*! @brief Retourne la liste composee des elements suivants de la liste consideree
 *
 * @return (const liste&) liste des elements suivants
 */
inline const liste& liste::suivant() const
{
  return *suivant_;
}


/*! @brief Retourne un pointeur sur l'Objet_U premier element de la liste
 *
 * @return (Objet_U*) pointeur sur l'Objet_U premier element de la liste
 */
inline Objet_U* liste::operator ->()
{
  return(&(valeur()));
}


/*! @brief Retourne un pointeur sur l'Objet_U premier element de la liste
 *
 * @return (const Objet_U*) pointeur sur l'Objet_U premier element de la liste
 */
inline const Objet_U* liste::operator ->() const
{
  return(&(valeur()));
}

/*! @brief Constructeur Construit une liste_curseur a partir d'une liste
 *
 * @param (const liste& list) la liste a utiliser
 */
inline liste_curseur::liste_curseur(const liste& a_list)
  : curseur((liste*) &a_list)
{
  if(a_list.est_vide())
    curseur=0;
}

/*! @brief Teste si le curseur est non nul
 *
 * @return (int) 1 si le curseur est non nul, 0 si il est nul
 */
inline liste_curseur::operator bool() const
{
  return (curseur!=0);
}

/*! @brief Avance le curseur dans la liste Si le curseur est sur le dernier element, il devient nul
 *
 */
inline void liste_curseur::operator ++()
{
  if (curseur->est_dernier()) curseur=0;
  else curseur=&(curseur->suivant());
}

/*! @brief Retourne l'Objet_U associe au curseur de la liste
 *
 * @return (const Objet_U&) l'Objet_U associe au curseur
 */
inline const Objet_U& liste_curseur::valeur() const
{
  return curseur->valeur();
}

/*! @brief Retourne l'Objet_U associe au curseur de la liste
 *
 * @return (Objet_U&) l'Objet_U associe au curseur
 */
inline Objet_U& liste_curseur::valeur()
{
  return curseur->valeur();
}

/*! @brief Retourne un pointeur sur l'Objet_U du curseur de la liste
 *
 * @return (Objet_U*) pointeur sur l'Objet_U du curseur
 */
inline Objet_U* liste_curseur::operator ->()
{
  return curseur->operator->();
}

/*! @brief Retourne un pointeur sur l'Objet_U du curseur de la liste
 *
 * @return (const Objet_U*) pointeur sur l'Objet_U du curseur
 */
inline const Objet_U* liste_curseur::operator ->() const
{
  return curseur->operator->();
}

/*! @brief Affectation d'une liste a une liste_curseur
 *
 * @param (const liste& list) la liste a affecter
 * @return (liste&) la liste affectee
 */
inline liste& liste_curseur::operator=(const liste& a_list)
{
  curseur=(liste*) (&a_list);
  return *curseur;
}

/*! @brief Retourne la liste associee a la liste_curseur
 *
 * @return (const liste&) la liste associee
 */
inline const liste& liste_curseur::list() const
{
  return *curseur;
}

/*! @brief Retourne la liste associee a la liste_curseur
 *
 * @return (liste&) la liste associee
 */
inline liste& liste_curseur::list()
{
  return *curseur;
}

#endif
