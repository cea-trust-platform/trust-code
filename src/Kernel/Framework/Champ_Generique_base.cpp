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
// File:        Champ_Generique_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Generique_base.h>
#include <Ref_Champ_Generique_base.h>
#include <Champs_compris.h>
#include <Interprete.h>
#include <Probleme_base.h>
#include <Param.h>

Implemente_base(Champ_Generique_base,"Champ_Generique_base",Objet_U);


// Description:
// Imprime sur un flot de sortie.
// Parametre: Sortie& os
// Signification: un flot de sortie
// Retour: Sortie&
// Signification: le flot de sortie modifie
Sortie& Champ_Generique_base::printOn(Sortie& os) const
{
  return os;
}

Entree& Champ_Generique_base::readOn(Entree& is)
{
  Cerr<<"Reading data for a "<<que_suis_je()<<" field." <<finl;
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  return is;
}

int Champ_Generique_base::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  Cerr << mot << " is not a keyword understood by " << que_suis_je() << " in lire_motcle_non_standard"<< finl;
  exit();
  return -1;
}

// Description:
//  Renvoie la dimension de l'espace dans lequel le champ est defini.
//  Il s'agit du nombre de composantes des coordonnees qu'il faut fournir dans get_xyz_values.
//  (par exemple, un champ defini sur une surface peut etre de dimension 3 si les
//   coordonnees des sommets sont 3D, ou de dimension 2 s'il s'agit de coordonnees curvilignes
//   le long de la surface)
int Champ_Generique_base::get_dimension() const
{
  throw Champ_Generique_erreur("NOT_IMPLEMENTED");
  // On n'arrive jamais ici
  return get_dimension();
}

// Description:
//  Renvoie le temps du Champ_Generique.
double Champ_Generique_base::get_time() const
{
  throw Champ_Generique_erreur("NOT_IMPLEMENTED");
  // On n'arrive jamais ici
  return get_time();
}

//Renvoie le probleme qui porte le champ cible
const Probleme_base& Champ_Generique_base::get_ref_pb_base() const
{
  throw Champ_Generique_erreur("NOT_IMPLEMENTED");
  return get_ref_pb_base();
}

//Renvoie la discretisation associee au probleme
const Discretisation_base& Champ_Generique_base::get_discretisation() const
{
  const Objet_U& ob = interprete().objet(nom_pb_);
  const Probleme_base& pb = ref_cast(Probleme_base,ob);
  const Discretisation_base& discr = pb.discretisation();
  return discr;
}

//Renvoie la directive (champ_elem, champ_sommets, champ_face ou pression)
//pour lancer la discretisation de l espace de stockage rendu par get_champ()
const Motcle Champ_Generique_base::get_directive_pour_discr() const
{
  throw Champ_Generique_erreur("NOT_IMPLEMENTED");
  return get_directive_pour_discr();
}

void Champ_Generique_base::nommer(const Nom& nom)
{
  nom_post_ = nom;
}

const Nom& Champ_Generique_base::get_nom_post() const
{
  return nom_post_;
}

// Description:
//  Renvoie la liste des "query" possibles pour le champ.
void Champ_Generique_base::get_property_names(Motcles& list) const
{
  throw Champ_Generique_erreur("NOT_IMPLEMENTED");
}

// Description:
//  Renvoie la propriete demandee. Exemples:
//  "DISCRETISATION" : type du champ discret (P0, P1, P1NC, etc...)
//  "ELEMENT_TYPE" : type de l'element de plus grande dimension ({TRIANGLE}, {TETRAHEDRA}, {QUAD}, {HEXA}, etc)
//  "DYNAMIC_MESH" : le maillage est-il dynamique ou pas ({STATIC}, {DYNAMIC})
//  "NAME" : nom du champ
//  "COMPONENT_NAMES" : nom des composantes du champ ({K,EPSILON} ou {VITESSE_X,VITESSE_Y,VITESSE_Z})
//  "BOUNDARY_NAMES" : nom des bords
//  "COORDINATES" : systeme de coordonnees des noeuds ({X}, {X,Y}, {X,Y,Z}, {R,THETA}, etc...)
// Exceptions:
//  - GenericFieldError::INVALID : query non comprise
const Noms Champ_Generique_base::get_property(const Motcle& query) const
{
  throw Champ_Generique_erreur("NOT_IMPLEMENTED");
  // On n'arrive jamais ici
  return get_property(query);
}


// Description:
//  Renvoie le type des entites geometriques sur auxquelles les valeurs discretes
//  sont attachees (NODE pour un champ P1, FACE pour un champ P1NC, ELEMENT pour un
//  champ P0, etc). Il est conseille d'utiliser la syntaxe get_localisation() sans
//  parametre, sauf si on sait quoi faire pour les champs multi-supports.
//  Voir aussi get_nb_localisations()
// Parametre : index
// Signification : index de la localisation demandee (pour les champs multi-support).
//  Si index = -1 : si le champ est multi-support on leve une exception, sinon on renvoie le support.
//  Si index >= 0 : on renvoie le i-ieme support.
// Valeur par defaut : -1
// Exceptions:
//  - GenericFieldError::INVALID : le champ n'est pas discretise sur ces entites geometriques
//    (il peut s'agir d'un champ analytique ou d'un champ multi-localisation), ou le champ
//    est multi-support alors qu'on a demande index = -1, ou le support "index" n'existe pas.
Entity Champ_Generique_base::get_localisation(const int index) const
{
  throw Champ_Generique_erreur("NOT_IMPLEMENTED");
  // On n'arrive jamais ici
  return get_localisation(index);
}

int Champ_Generique_base::get_nb_localisations() const
{
  throw Champ_Generique_erreur("NOT_IMPLEMENTED");
  // On n'arrive jamais ici
  return get_nb_localisations();
}

// Description:
//  Renvoie une reference au tableau de valeurs discretes s'il existe en memoire.
//  La reference est valide au moins jusqu'au prochain appel a une methode non const.
//  Le maillage sur lequel ces valeurs sont definies est accessible par les methodes
//  get_node_coordinates(), get_connectivity(), get_field_type().
// Exceptions:
//  - GenericFieldError::XYZ_ONLY : les valeurs ne sont accessibles que par get_xyz_values
//  - GenericFieldError::NO_REF : les valeurs ne sont pas stockees en memoire,
//     il faut utiliser get_copy_values();
//  - GenericFieldError::MESH_ONLY : le champ ne porte pas de valeurs, il sert uniquement a decrire une geometrie
const DoubleTab& Champ_Generique_base::get_ref_values() const
{
  // Implementation par defaut : exception NO_REF
  throw Champ_Generique_erreur("NO_REF");
  // On n'arrive jamais ici
  return get_ref_values();
}

// Description:
//  Remplit le tableau values avec les valeurs discretes du champ (cree une copie).
// Exceptions:
//  - GenericFieldError::XYZ_ONLY : les valeurs ne sont accessibles que par get_xyz_values
//  - GenericFieldError::MESH_ONLY : le champ ne porte pas de valeurs, il sert uniquement a decrire une geometrie
void Champ_Generique_base::get_copy_values(DoubleTab& values) const
{
  throw Champ_Generique_erreur("NOT_IMPLEMENTED");
}

// Description:
//  Calcule la valeur ponctuelle du champ aux coordonnees donnees dans coords et
//  les met dans values. validity_flag est rempli avec 1 si la valeur est valide
//  (coordonnees a l'interieur du domaine), 0 sinon.
/// Attention en parallele:
//  Chaque processeur traite le tableau coords qui lui est fourni : on peut soit faire
//  calculer le meme tableau a tout le monde: dans ce cas, validity_flag indique sur
//  chaque processeur quelles valeurs chaque processeur a pu calculer, soit on sait par
//  avance quelles coordonnees sont calculable par chaque processeur et on donne au processeur
//  local uniquement des coordonnees qu'il possede. Si une meme coordonnee est demandee
//  a plusieurs processeurs, on ne garantit pas que tous donnent le meme resultat. En general,
//  un seul aura le validity_flag mis pour cette coordonnee.
// Exceptions:
//  - GenericFieldError::NOT_IMPLEMENTED : le paresseux n'a pas code les methodes d'interpolation
//   il faut se debrouiller avec le maillage et les valeurs discretes.
//  - GenericFieldError::MESH_ONLY : le champ ne porte pas de valeurs, il sert uniquement a decrire une geometrie
void Champ_Generique_base::get_xyz_values(const DoubleTab& coords, DoubleTab& values, ArrOfBit& validity_flag) const
{
  throw Champ_Generique_erreur("NOT_IMPLEMENTED");
}

// Description:
//  Renvoie une reference au tableau des coordonnees des sommets du maillage support
//  du champ, s'il existe. Le tableau a toujours deux dimensions :
//   dimension(0) = nombre de sommets reels
//   dimension(1) = get_dimension() (dimension de l'espace dans lequel est defini le champ)
//  En parallele, le tableau est un tableau distribue avec items communs.
// Exceptions:
//  - GenericFieldError::INVALID : le tableau n'existe pas (champ analytique ...)
//  - GenericFieldError::NO_REF : les valeurs ne sont pas stockees en memoire,
//     il faut utiliser get_copy_coordinates();
const DoubleTab& Champ_Generique_base::get_ref_coordinates() const
{
  // Implementation par defaut : exception NO_REF
  throw Champ_Generique_erreur("NO_REF");
  // On n'arrive jamais ici
  return get_ref_coordinates();
}


void Champ_Generique_base::get_copy_coordinates(DoubleTab&) const
{
  throw Champ_Generique_erreur("NO_REF");
}

// Description:
//  Renvoie le tableau de connectivite entre l'entite geometrique index1 et l'entite index2.
//  Par exemple
//   get_ref_connectivity(ELEM, NODE) = Zone::mes_elems
//   get_ref_connectivity(ELEM, FACE) = ZoneVF::elem_faces_
//   get_ref_connectivity(FACE, ELEM) = ZoneVF::face_voisins_
//  Le tableau a toujours deux dimensions :
//   dimension(0) = nombre d'entites "index1" reelles
//   dimension(1) = nombre d'entites "index2" connectees a chaque entite "index1"
//  En parallele, le tableau est un tableau distribue avec items communs
// Exceptions:
//  - GenericFieldError::INVALID : le tableau n'existe pas (champ analytique ...)
//  - GenericFieldError::NO_REF : les valeurs ne sont pas stockees en memoire,
//     il faut utiliser get_copy_connectivity();
const IntTab& Champ_Generique_base::get_ref_connectivity(Entity index1, Entity index2) const
{
  throw Champ_Generique_erreur("NOT_IMPLEMENTED");
  // On n'arrive jamais ici
  return get_ref_connectivity(index1,index2);
}

void Champ_Generique_base::get_copy_connectivity(Entity index1, Entity index2, IntTab&) const
{
  throw Champ_Generique_erreur("NOT_IMPLEMENTED");
}

// Description:
// Renvoie une ref au domaine sur lequel sera evalue l espace de stockage.
// Par defaut le domaine associe au probleme.
const Domaine& Champ_Generique_base::get_ref_domain() const
{
  const Objet_U& ob = interprete().objet(nom_pb_);
  const Probleme_base& pb = ref_cast(Probleme_base,ob);
  const Domaine& dom = pb.domaine_dis().domaine();
  return dom;
}


// Description:
// Cree une copie du domaine sur lequel sera evalue l espace de stockage.
// Par defaut le domaine associe au probleme.
void Champ_Generique_base::get_copy_domain(Domaine& domain) const
{
  const Domaine& dom = get_ref_domain();
  domain = dom;
}


// Description:
// Renvoie une ref a la zone_discretisee du domaine
// sur lequel sera evalue l espace de stockage.
// Par defaut le domaine associe au probleme.
const Zone_dis_base& Champ_Generique_base::get_ref_zone_dis_base() const
{
  const Objet_U& ob = interprete().objet(nom_pb_);
  const Probleme_base& pb = ref_cast(Probleme_base,ob);
  const Zone_dis_base& zone_dis = pb.domaine_dis().zone_dis(0).valeur();
  return zone_dis;
}

//Description:
//  Renvoie une ref a la zone_cl_discretisee de l equation portant le champ cible.
const Zone_Cl_dis_base& Champ_Generique_base::get_ref_zcl_dis_base() const
{
  throw Champ_Generique_erreur("INVALID");
  // On n'arrive jamais ici
  return get_ref_zcl_dis_base();
}

const Champ_Generique_base& Champ_Generique_base::get_champ_post(const Motcle& nom) const
{
  REF(Champ_Generique_base) ref_champ;

  Motcle nom_champ;
  const Noms nom_champ_post = get_property("nom");
  nom_champ = Motcle(nom_champ_post[0]);

  if (nom_champ==nom)
    {
      ref_champ = *this;
      ref_champ->fixer_identifiant_appel(nom);
      return ref_champ;
    }
  const Noms syno = get_property("synonyms");
  int nb_syno = syno.size();
  for (int i=0; i<nb_syno; i++)
    {
      nom_champ = Motcle(syno[i]);
      if (nom_champ==nom)
        {
          ref_champ = *this;
          ref_champ->fixer_identifiant_appel(nom);
          return ref_champ;
        }
    }

  {
    const Noms composantes = get_property("composantes");
    int nb_composantes = composantes.size();

    for (int i=0; i<nb_composantes; i++)
      {
        nom_champ = Motcle(composantes[i]);
        if (nom_champ==nom)
          {
            ref_champ = *this;
            ref_champ->fixer_identifiant_appel(nom);
            return ref_champ;
          }
      }
  }

  throw Champs_compris_erreur();
  return ref_champ;
}

int Champ_Generique_base::comprend_champ_post(const Motcle& identifiant) const
{
  Motcle nom_champ;
  const Noms nom_champ_post = get_property("nom");
  nom_champ = Motcle(nom_champ_post[0]);

  if (nom_champ==identifiant)
    return 1;
  else
    {
      const Noms composantes = get_property("composantes");
      int nb_composantes = composantes.size();

      for (int i=0; i<nb_composantes; i++)
        {
          nom_champ = Motcle(composantes[i]);
          if (nom_champ==identifiant)
            return 1;
        }
    }
  return 0;
}
