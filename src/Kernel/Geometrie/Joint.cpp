/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Joint.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/35
//
//////////////////////////////////////////////////////////////////////////////

#include <Joint.h>
#include <Domaine.h>

Implemente_instanciable(Joint,"Joint",Frontiere);

// ***************************************************************
//  Implementation de la classe Joint
// ***************************************************************

// Description:
//    Ecrit le joint sur un flot de sortie.
//    On ecrit:
//      - la frontiere
//      - le PE voisin
//      - l'epaisseur
//      - les sommets
Sortie& Joint::printOn(Sortie& s ) const
{
  Frontiere::printOn(s) ;
  s << "format_joint_250507" << finl;
  s << PEvoisin_ << finl;
  s << epaisseur_ << finl;
  s << joint_item(Joint::SOMMET).items_communs() << finl;
  s << joint_item(Joint::ELEMENT).items_distants() << finl;
  return s ;
}

// Description:
//    Lit un joint a partir d'un flot d'entree.
Entree& Joint::readOn(Entree& s)
{
  Frontiere::readOn(s) ;
  Nom format;
  s >> format;
  if (format!="format_joint_250507")
    {
      if (Process::je_suis_maitre())
        {
          Cerr << "The format of .Zones is not recognized because probably too old." << finl;
          Cerr << "Split your mesh with an executable which is more recent" << finl;
          Cerr << "than the version 1.5.2 build 240507." << finl;
        }
      exit();
    }
  s >> PEvoisin_;
  s >> epaisseur_;
  s >> set_joint_item(Joint::SOMMET).set_items_communs();
  s >> set_joint_item(Joint::ELEMENT).set_items_distants();
  return s ;
}

// Description:
void Joint::dimensionner(int i)
{
  faces().dimensionner(i);
  faces().voisins() = -1;
  faces().les_sommets() = -1;
}


// Description:
//    Fixe le numero du PE voisin.
void Joint::affecte_PEvoisin(int num)
{
  PEvoisin_ = num;
}

// Description:
//    Fixe l'epaisseur du joint
void Joint::affecte_epaisseur(int ep)
{
  epaisseur_ = ep;
}

// Description:
//    Renvoie PEvoisin (numero de la zone voisine)
int Joint::PEvoisin() const
{
  return PEvoisin_;
}

// Description:
//    Renvoie l'epaisseur du joint.
int Joint::epaisseur() const
{
  return epaisseur_;
}


// Description:
//    Ajoute des faces a la frontiere (au joint)
//    voir Frontiere::ajouter_faces(const IntTab&)
// Precondition:
// Parametre: IntTab& sommets
//    Signification: tableau contenant les numeros des sommets
//                   des face a ajouter
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Joint::ajouter_faces(const IntTab& sommets)
{
  faces().ajouter(sommets);
  faces().voisins() = -1;
}

const IntTab& Joint::renum_virt_loc() const
{
  return joint_item(SOMMET).renum_items_communs();;
}

const ArrOfInt& Joint::esp_dist_elems() const
{
  return joint_item(ELEMENT).items_distants();
}

const ArrOfInt& Joint::esp_dist_sommets() const
{
  return joint_item(SOMMET).items_distants();
}

const ArrOfInt&  Joint::esp_dist_faces() const
{
  return joint_item(FACE).items_distants();
}


// Description: Renvoie les informations de joint pour un type d'item
//  geometrique donne, pour remplissage des structures. Ces structures
//  sont generalement remplies par la classe Scatter. Exceptions notables:
//  Zone_VDF et Zone_VF pour la renumerotation des faces.
Joint_Items& Joint::set_joint_item(Type_Item item)
{
  switch(item)
    {
    case SOMMET:
      return joint_sommets_;
      break;
    case ELEMENT:
      return joint_elements_;
      break;
    case FACE:
      return joint_faces_;
      break;
    case ARETE:
      return joint_aretes_;
      break;
    case FACE_FRONT:
      return joint_faces_front_;
      break;
    default:
      Cerr << "Error in Joint::set_joint_item, invalid item number : " << (int)item << finl;
      exit();
    }
  return joint_sommets_; // never arrive here
}

// Description: Renvoie les informations de joint pour le type demande.
const Joint_Items& Joint::joint_item(Type_Item item) const
{
  switch(item)
    {
    case SOMMET:
      return joint_sommets_;
      break;
    case ELEMENT:
      return joint_elements_;
      break;
    case FACE:
      return joint_faces_;
      break;
    case ARETE:
      return joint_aretes_;
      break;
    case FACE_FRONT:
      return joint_faces_front_;
      break;
    default:
      Cerr << "Error in Joint::set_joint_item, invalid item number : " << (int)item << finl;
      exit();
    }
  return joint_sommets_; // never arrive here
}

