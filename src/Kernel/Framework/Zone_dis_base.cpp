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
// File:        Zone_dis_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Zone_dis_base.h>
#include <Frontiere_dis_base.h>
#include <Domaine.h>

Implemente_base(Zone_dis_base,"Zone_dis_base",Objet_U);


// Description:
//    Surcharge Objet_U::printOn(Sortie&)
//    NE FAIT RIEN
//    A surcharger dans les classes derivees.
//    Imprime la zone discretisee sur un flot de sortie
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Zone_dis_base::printOn(Sortie& os) const
{
  return os ;
}


// Description:
//    Surcharge Objet_U::readOn(Sortie&)
//    NE FAIT RIEN
//    A surcharger dans les classes derivees.
//    Lit une zone discretisee a partir d'un flot d'entree
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Zone_dis_base::readOn(Entree& is)
{
  return is ;
}

// Description:
//    Associe une Zone a l'objet.
// Precondition:
// Parametre: Zone& une_zone
//    Signification: la zone a associee a la zone discretisee
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Zone_dis_base::associer_zone(const Zone& une_zone)
{
  la_zone=une_zone;
}

// Description:
//    Associe un Domaine_dis a l'objet.
// Precondition:
// Parametre: Domaine_dis& un_domaine_dis
//    Signification: le domaine discretise a associer.
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Zone_dis_base::associer_domaine_dis(const Domaine_dis& un_domaine_dis)
{
  le_domaine_dis=un_domaine_dis;
}

// Description:
//    Renvoie la frontiere de Nom nom.
//    (On indexe les frontiere avec leur nom)
// Precondition:
// Parametre: Nom& nom
//    Signification: le nom de la frontiere a indexer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Frontiere_dis_base&
//    Signification: la frontiere discretisee indexee
//    Contraintes:
// Exception: frontiere de nom inconnu leve par rang_frontiere(Nom&)
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Frontiere_dis_base& Zone_dis_base::frontiere_dis(const Nom& nom) const
{
  return frontiere_dis(rang_frontiere(nom));
}

Frontiere_dis_base& Zone_dis_base::frontiere_dis(const Nom& nom)
{
  return frontiere_dis(rang_frontiere(nom));
}

// Description:
//    Renvoie le rang de la frontiere de Nom nom
//    Renvoie -1 si aucune frontiere ne s'appelle nom.
// Precondition:
// Parametre: Nom& nom
//    Signification: le nom de la frontiere dont cherche le rang
//    Valeurs par defaut:
//    Contraintes:  reference constante
//    Acces: entree
// Retour: int
//    Signification: le rang de la frontiere si elle existe -1 sinon
//    Contraintes:
// Exception: pas de frontiere de Nom nom trouvee
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Zone_dis_base::rang_frontiere(const Nom& nom) const
{
  for(int i=0; i<nb_front_Cl(); i++)
    if (frontiere_dis(i).le_nom()==nom)
      return i;
  Cerr << "We did not find the boundary name " << nom << " on the domain " << zone().domaine().le_nom() << finl;
  Cerr << "Check the given boundary name or the domain." << finl;
  exit();
  return -1;
}

int Zone_dis_base::rang_frontiere(const Nom& nom)
{
  for(int i=0; i<nb_front_Cl(); i++)
    if (frontiere_dis(i).le_nom()==nom)
      return i;
  Cerr << "We did not find the boundary name " << nom << " on the domain " << zone().domaine().le_nom() << finl;
  Cerr << "Check the given boundary name or the domain." << finl;
  exit();
  return -1;
}
// Description:
//    Ecriture des noms des bords sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Zone_dis_base::ecrire_noms_bords(Sortie& os) const
{
  zone().ecrire_noms_bords(os);
}

void Zone_dis_base::creer_elements_fictifs(const Zone_Cl_dis_base&)
{
  Cerr << "Zone_dis_base::creer_elements_fictifs should be overloaded by "<< que_suis_je() <<finl;
  assert(0);
  exit();
}

IntTab& Zone_dis_base::face_sommets()
{
  Cerr << que_suis_je() << "does not implement the method face_sommets()"
       << finl;
  exit();
  throw;
  return face_sommets();
}

const IntTab& Zone_dis_base::face_sommets() const
{
  Cerr << que_suis_je() << "does not implement the method face_sommets()"
       << finl;
  exit();
  throw;
  return face_sommets();
}

IntTab& Zone_dis_base::face_voisins()
{
  Cerr << que_suis_je() << "does not implement the method face_voisins()"
       << finl;
  exit();
  throw;
  return face_voisins();
}

const IntTab& Zone_dis_base::face_voisins() const
{
  Cerr << que_suis_je() << "does not implement the method face_voisins()"
       << finl;
  exit();
  throw;
  return face_voisins();
}
