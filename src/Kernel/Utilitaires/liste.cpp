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

#include <liste.h>

Implemente_instanciable_sans_constructeur_ni_destructeur(liste,"liste",Objet_U);

// Description:
//     Lecture d'une liste sur un flot d'entree
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& liste::readOn(Entree& is)
{
  vide();
  Nom accouverte="{";
  Nom accfermee="}";
  Nom virgule=",";
  Nom nom;
  is >> nom;
  if(nom ==(const char*)"vide") return is;
  if(nom!=accouverte)
    {
      Cerr << "We expected a { and not " << nom << finl;
      exit();
    }
  while(nom!=accfermee)
    {
      is >> nom;
      if (nom !=  accfermee)
        {
          add(nom);
          is >> nom;
        }
      if((nom!=accfermee)&&(nom!=virgule))
        {
          Cerr << "We expected a } or a , and not " << nom << finl;
          exit();
        }
    }
  return is;
}


// Description:
//     Ecriture d'une liste sur un flot de sortie
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& liste::printOn(Sortie& os) const
{
  Nom accouverte="{";
  Nom accfermee="}";
  Nom virgule=",";
  Nom blanc=" ";
  if (est_vide()) os << "vide" << finl;
  else
    {
      os << accouverte << finl;
      liste_curseur curseur(*this);
      while(curseur)
        {
          os << curseur.valeur() ;
          if (!curseur.list().est_dernier())
            os << blanc << virgule << finl;
          ++curseur;
        }
      os <<finl << accfermee << finl;
    }
  return os;
}

// Description:
//     Renvoie le dernier element de la liste
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: liste&
//    Signification: le dernier element de la liste
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
liste& liste::dernier()
{
  if (est_dernier()) return *this;
  liste_curseur curseur(*this);
  while(curseur)
    if (curseur.list().est_dernier()) break;
    else ++curseur;
  return curseur.list();

}

// Description:
//     Renvoie le dernier element de la liste
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: const liste&
//    Signification: le dernier element de la liste
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const liste& liste::dernier() const
{
  if (est_dernier()) return *this;
  liste_curseur curseur(*this);
  while(curseur)
    if (curseur.list().est_dernier()) break;
    else ++curseur;
  return curseur.list();

}

// Description:
//     Renvoie la taille de la liste
//     Une liste vide est de taille nulle
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: nombre d'elements de la liste
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int liste::size() const
{
  if(est_vide()) return 0;
  if(est_dernier()) return 1;
  int compteur=1;
  liste_curseur curseur(*suivant_);
  while(curseur)
    {
      ++compteur;
      ++curseur;
    }
  return compteur;
}

// Description:
//     Destructeur de liste
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
liste::~liste()
{
  vide();
  suivant_=0;
}

// Description:
//     Vide la liste
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void liste::vide()
{
  if (!est_vide())
    {
      if(suivant_)
        {
          suivant_->vide();
          delete suivant_;
        }
      data=DerObjU();
    }
  suivant_=this;
}

// Description:
//     Operateur d'acces au ieme element de la liste
// Precondition:
// Parametre: int i
//    Signification: l'indice de l'element a trouver
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Objet_U&
//    Signification: le ieme element de la liste
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Objet_U& liste::operator[](int i)
{
  liste_curseur curseur(*this);
  if (!curseur)
    {
      Cerr << que_suis_je() << " : empty list !" << finl;
      exit();
    }
  while(curseur && i--)
    ++curseur;
  if(i!=-1)
    {
      Cerr << que_suis_je() << " : overflow of list " << finl;
      assert(i==-1);
      exit();
    }
  return curseur.valeur();
}

// Description:
//     Operateur d'acces au ieme element de la liste
// Precondition:
// Parametre: int i
//    Signification: l'indice de l'element a trouver
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: const Objet_U&
//    Signification: le ieme element de la liste
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const Objet_U& liste::operator[](int i) const
{
  liste_curseur curseur(*this);
  while(curseur && i--)
    ++curseur;
  if(i!=-1)
    {
      Cerr << que_suis_je() << " : overflow of list " << finl;
      assert(i==-1);
      exit();
    }
  return curseur.valeur();
}

// Description:
//     Operateur d'acces a l'element de la liste qui est egal a nom
// Precondition:
// Parametre: const Nom& nom
//    Signification: le nom de l'element a trouver
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Objet_U&
//    Signification: l'element de la liste qui est egal a nom
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Objet_U& liste::operator[](const Nom& nom)
{
  liste_curseur curseur(*this);
  while(curseur)
    if( (curseur->le_nom()) == nom) break;
    else ++curseur;
  if(!curseur)
    {
      Cerr << "We have not found an object with name " << nom << finl;
      Cerr << "The known names are : " << finl;
      liste_curseur curseur2(*this);

      while(curseur2)
        {
          Cerr << curseur2->le_nom() << " ; " ;
          ++curseur2;
        }
      Cerr << finl;
      exit();
    }
  return curseur.valeur();
}

// Description:
//     Operateur d'acces a l'element de la liste qui est egal a nom
// Precondition:
// Parametre: const Nom& nom
//    Signification: le nom de l'element a trouver
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: const Objet_U&
//    Signification: l'element de la liste qui est egal a nom
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const Objet_U& liste::operator[](const Nom& nom) const
{
  liste_curseur curseur(*this);
  while(curseur)
    if( (curseur->le_nom()) == nom) break;
    else ++curseur;
  if(!curseur)
    {
      Cerr << "We have not found an object with name " << nom << finl;
      Cerr << "The known names are : ";
      liste_curseur curseur2(*this);

      while(curseur2)
        {
          Cerr << curseur2->le_nom() << " ; " ;
          ++curseur2;
        }
      exit();
    }
  return curseur.valeur();
}

// Description:
//     Operateur d'affectation de liste
// Precondition:
// Parametre: const liste& list
//    Signification: la liste a affecter
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
//    Les anciens elements sont perdus
// Postcondition:
liste& liste::operator=(const liste& list)
{
  vide();
  add(list);
  return *this;
}

//////////
// Description:
// Ajout d'un Objet_U a la liste
// to_add est libere en sortie
//
Objet_U& liste::add_deplace(DerObjU& to_add)
{
  if( est_vide())
    {
      data.deplace(to_add);
      suivant_=0;
      return valeur();
    }
  else
    {
      if(est_dernier())
        {
          liste* liste_ptr=new liste;
          if(!liste_ptr)
            {
              Cerr << "Run out of memory " << finl;
              exit();
            }
          liste_ptr->add_deplace(to_add);
          suivant_=liste_ptr;
          return suivant_->valeur();
        }
      else
        return dernier().add_deplace(to_add);
    }
}


// Description:
//     Ajout d'un Objet_U a la liste
// Precondition:
// Parametre: const Objet_U& to_add
//    Signification: L'Objet_U a ajouter
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Objet_U&
//    Signification: L'Objet_U ajoute
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Objet_U& liste::add(const Objet_U& to_add)
{
  if( est_vide())
    {
      data=to_add;
      suivant_=0;
      return valeur();
    }
  else
    {
      if(est_dernier())
        {
          suivant_=new liste(to_add);
          if(!suivant_)
            {
              Cerr << "Run out of memory " << finl;
              exit();
            }
          return suivant_->valeur();
        }
      else
        return dernier().add(to_add);
    }
}

// Description:
//     Ajout d'une liste a la liste
// Precondition:
// Parametre: const liste& list
//    Signification: la liste a ajouter
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: liste&
//    Signification: la liste comportant tous les elements
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
liste& liste::add(const liste& list)
{
  liste_curseur curseur(list);
  while(curseur)
    {
      Objet_U& to_add=curseur.valeur();
      add(to_add);
      ++curseur;
    }
  return *this;
}

// Description:
//     Ajout d'un Objet_U a la liste ssi il n'existe pas deja
// Precondition:
// Parametre: const Objet_U& to_add
//    Signification: l'Objet_U a ajouter
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Objet_U&
//    Signification: L'Objet_U existant deja dans la liste ou celui ajoute
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Objet_U& liste::add_if_not(const Objet_U& to_add)
{
  if( est_vide())
    {
      data=to_add;
      suivant_=0;
      return valeur();
    }
  else
    {
      liste_curseur curseur=*this;
      while(curseur)
        {
          if(curseur.valeur()==to_add)
            return curseur.valeur();
          if(curseur.list().est_dernier())
            return(curseur.list().add(to_add));
          ++curseur;
        }
    }
  // On ne devrait pas passer la :
  Cerr << "Error in a list for add_if_not " << finl;
  return valeur();
}

// Description:
//     Recherche d'un Objet_U dans la liste
// Precondition:
// Parametre: const Objet_U& obj
//    Signification: l'Objet_U a rechercher
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: liste&
//    Signification: L'element trouve dans la liste
//    Contraintes:
// Exception:
//    Sort en erreur si l'Objet_U n'est pas present dans la liste
// Effets de bord:
// Postcondition:
liste& liste::search(const Objet_U& obj) const
{
  liste_curseur curseur=*this;
  while(curseur)
    {
      if(curseur.valeur()==obj)
        return curseur.list();
      ++curseur;
    }
  Cerr << "We have not found an object "<< obj <<" !" << finl;
  exit();
  return *suivant_;
}

// Description:
//     Recherche d'un Objet_U de Nom ch dans la liste
// Precondition:
// Parametre: const char* const ch
//    Signification: le nom de l'Objet_U a rechercher
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: liste&
//    Signification: L'element de la liste trouve
//    Contraintes:
// Exception:
//    Sort en erreur si aucun Objet_U n'est trouve
// Effets de bord:
// Postcondition:
liste& liste::search(const char* const ch) const
{
  Nom nom(ch);
  liste_curseur curseur=*this;
  while(curseur)
    {
      if(curseur->le_nom()==nom)
        return curseur.list();
      ++curseur;
    }
  Cerr << "We have not found an object with name "<< nom <<" !" << finl;
  exit();
  return *suivant_;
}

// Description:
//     Verifie si un Objet_U appartient ou non a la liste
// Precondition:
// Parametre: const Objet_U& obj
//    Signification: L'Objet_U a rechercher
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si la liste contient l'Objet_U obj, 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int liste::contient(const Objet_U& obj) const
{
  liste_curseur curseur=*this;
  while(curseur)
    {
      if(curseur.valeur()==obj)
        return 1;
      ++curseur;
    }
  return 0;
}

// Description:
//     Verifie si un Objet_U de Nom ch appartient ou non a la liste
// Precondition:
// Parametre: const char* const ch
//    Signification: le nom de l'Objet_U a rechercher
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si la liste contient un Objet_U de Nom ch, 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int liste::contient(const char* const ch ) const
{
  Nom nom(ch);
  liste_curseur curseur=*this;
  while(curseur)
    {
      if(curseur->le_nom()==nom)
        return 1;
      ++curseur;
    }
  return 0;
}

// Description:
//     Renvoie le rang d'un Objet_U dans la liste
// Precondition:
// Parametre: const Objet_U& obj
//    Signification: l'Objet_U a rechercher
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le rang de L'Objet_U dans la liste, -1 si l'Objet_U n'est pas dans la liste
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int liste::rang(const Objet_U& obj) const
{
  liste_curseur curseur=*this;
  int i=0;
  while(curseur)
    {
      if(curseur.valeur()==obj)
        return i;
      ++i;
      ++curseur;
    }
  return -1;
}

// Description:
//     Renvoie le rang d'une chaine de caracteres contenue dans la liste
//     Recherche un Objet_U de Nom ch
// Precondition:
// Parametre: const char* const ch
//    Signification: le nom de l'Objet_U a rechercher
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le rang de l'Objet_U trouve ou -1 si aucun ne correspond
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int liste::rang(const char* const ch ) const
{
  Nom nom(ch);
  liste_curseur curseur=*this;
  int i=0;
  while(curseur)
    {
      if(curseur->le_nom()==nom)
        return i;
      ++i;
      ++curseur;
    }
  return -1;
}

// Description:
//     Supprime un Objet_U contenu dans la liste
// Precondition: Ne marche que si this est la tete de liste
// Parametre: const Objet_U& obj
//    Signification: l'Objet_U a supprimer de la liste
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void liste::suppr(const Objet_U& obj)
{
  if(valeur()==obj)
    {
      // L'objet recherche est le premier
      if(suivant_)
        {
          // La liste n'est pas un singleton
          liste* next=suivant_;
          suivant_=next->suivant_;
          data.deplace(next->data);
          next->suivant_=0;
          delete next;
          return;
        }
      else
        {
          vide();
          suivant_=this;
        }
      return;
    }
  // L'objet n'est pas en tete
  liste_curseur curseur_pre=*this;
  liste_curseur curseur=*suivant_;
  while(curseur)
    {
      if(curseur.valeur()==obj)
        {
          // On l'a trouve
          liste* next=curseur_pre.list().suivant_;
          curseur_pre.list().suivant_=curseur.list().suivant_;
          next->suivant_=0;
          delete next;
          return ;
        }
      ++curseur;
      ++curseur_pre;
    }
  Cerr << "WARNING while deleting in a list " << finl;
  Cerr << "We have not found an object == : " << obj << finl;
}

// Description:
//     Supprime l'Objet de Nom ch contenu dans la liste
// Precondition:
// Parametre: const char* const ch
//    Signification: le nom de l'Objet_U a supprimer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void liste::suppr(const char* const ch )
{
  Nom nom(ch);
  if(data.le_nom()==nom)
    {
      // L'objet recherche est le premier
      if(suivant_)
        {
          // La liste n'est pas un singleton
          liste* next=suivant_;
          suivant_=next->suivant_;
          data.deplace(next->data);
          next->suivant_=0;
          delete next;
          return;
        }
      else
        {
          vide();
          suivant_=this;
        }
      return;
    }
  // L'objet n'est pas en tete
  liste_curseur curseur_pre=*this;
  liste_curseur curseur=*suivant_;
  while(curseur)
    {
      if(curseur->le_nom()==nom)
        {
          // On l'a trouve
          liste* next=curseur_pre.list().suivant_;
          curseur_pre.list().suivant_=curseur.list().suivant_;
          next->suivant_=0;
          delete next;
          return ;
        }
      ++curseur;
      ++curseur_pre;
    }
  Cerr << "WARNING while deleting in a list " << finl;
  Cerr << "We have not found an object with name : " << ch << finl;
}

// Description:
//     Insertion d'un Objet_U dans la liste
// Precondition:
// Parametre: const Objet_U& ob
//    Signification: l'Objet_U a inserer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Objet_U&
//    Signification: L'Objet_U insere
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Objet_U& liste::inserer(const Objet_U& ob)
{
  if (est_vide() || est_dernier())
    return add(ob);
  else
    {
      liste* next=suivant_;
      suivant_=new liste(ob);
      suivant_->suivant_=next;
      return suivant_->valeur();
    }
}

// Description:
//     Insertion d'une liste
//     NON CODE - SORT SYSTEMATIQUEMENT EN ERREUR
// Precondition:
// Parametre: const liste&
//    Signification: la liste a inserer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
//    Sort toujours en erreur
// Effets de bord:
// Postcondition:
liste& liste::inserer(const liste&)
{
  Cerr << "liste::inserer(const liste&) : not coded !" << finl;
  exit();
  return *this;
}

// Description:
//     Supprime l'element suivant de la liste
//     Ne fait rien si la liste est vide ou si l'element courant est le dernier de la liste
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void liste::supprimer()
{
  if(est_vide()) return;
  if(est_dernier()) return;
  liste* old=suivant_;
  suivant_=suivant_->suivant_;
  old->suivant_=0;
  delete old;
}

// Description:
//     Operateur de comparaison de deux listes
//     La comparaison repose sur l'egalite des Objet_U des 2 listes
// Precondition:
// Parametre: const liste& list1
//    Signification: premiere liste a comparer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: const liste& list2
//    Signification: seconde liste a comparer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si les listes sont egales, 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int operator ==(const liste& list1 , const liste& list2)
{
  liste_curseur curseur1=list1;
  liste_curseur curseur2=list2;

  while(curseur1 && curseur2)
    if(curseur1.valeur() != curseur2.valeur())
      return 0;
    else
      {
        ++curseur1 ;
        ++curseur2;
      }
  if(!curseur1 && !curseur2)
    return 1;
  return 0;
}

// Description:
//     Operateur de difference entre deux listes
//     La comparaison repose sur l'egalite des Objet_U des 2 listes
// Precondition:
// Parametre: const liste& list1
//    Signification: premiere liste a comparer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: const liste& list2
//    Signification: seconde liste a comparer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si les listes sont differentes, 0 si elles egales
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int operator !=(const liste& list1 , const liste& list2)
{
  return !(list1==list2);
}

