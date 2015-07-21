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
// File:        Zone.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/69
//
//////////////////////////////////////////////////////////////////////////////

#include <Zone.h>
#include <Domaine.h>
#include <Sous_Zone.h>
#include <Octree.h>
#include <communications.h>
#include <MD_Vector_tools.h>
#include <IntList.h>

Implemente_instanciable_sans_constructeur(Zone,"Zone",Objet_U);

Zone::Zone() :
  Moments_a_imprimer_(0)
{
  volume_total_ = -1.; // pas encore calcule
}

// Description:
//    Ecrit la Zone sur un flot de sortie.
//    On ecrit le nom, le type des elements, les elements
//    et les bords, les bords periodiques, les joints, les
//    raccords et les faces internes.
// Precondition:
// Parametre: Sortie& s
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
Sortie& Zone::printOn(Sortie& s ) const
{
  Cerr << "Writing of " << nb_elem() << " elements." << finl;
  s << nom << finl;
  s << elem << finl;
  s << mes_elems;
  s << mes_faces_bord;
  s << mes_faces_joint;
  s << mes_faces_raccord;
  s << mes_faces_int;
  return s;
}

template<class LIST_FRONTIERE> void check_frontiere(const LIST_FRONTIERE& list, const char * msg)
{
  int n = list.size();
  if (! is_parallel_object(n))
    {
      Cerr << " Fatal error: processors don't have the same number of boundaries " << msg << endl;
      Process::barrier();
      Process::exit();
    }
  for (int i = 0; i < n; i++)
    {
      const Nom& nom = list[i].le_nom();
      Cerr << "  Boundary " << msg << " : " << nom << finl;
      if (! is_parallel_object(nom))
        {
          Cerr << " Fatal error: processors don't have the same number of boundaries " << msg << endl;
          Process::barrier();
          Process::exit();
        }
    }
}

// S'il y a un proc qui a un type different de vide_OD, on type les faces sur tous
// les processeurs avec ce type:
static void corriger_type(Faces& faces)
{
  int typ = faces.type_face();
  const int pe = (faces.type_face() == vide_0D) ? Process::nproc()-1 : Process::me();
  const int min_pe = ::mp_min(pe);
  // Le processeur min_pe envoie son type a tous les autres
  int typ_commun = typ;
  envoyer_broadcast(typ_commun, min_pe);

  if (typ_commun != typ)
    {
      if (typ != vide_0D)
        {
          Cerr << "Error in Zone.cpp corriger_type: invalid boundary face type" << finl;
          Process::exit();
        }
      Type_Face tt = (Type_Face)typ_commun;
      faces.typer(tt);
      int n= faces.nb_som_faces();
      faces.les_sommets().resize(0, n);
    }
}

// Description:
//    Lit les objets constituant une Zone
//    a partir d'un flot d'entree. Une fois les objets
//    lus on les associe a la zone.
// Precondition:
// Parametre: Entree& s
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
Entree& Zone::readOn(Entree& s)
{
  s >> nom ;
  Cerr << " Reading zone " << le_nom() << finl;
  s >> elem;
  s >> mes_elems;
  s >> mes_faces_bord;
  s >> mes_faces_joint;
  s >> mes_faces_raccord;
  s >> mes_faces_int;

  // remplacer vide_0D par le bon type pour les procs qui n'ont pas de faces de bord:
  {
    int i;
    int n = nb_front_Cl();
    for (i = 0; i < n; i++)
      corriger_type(frontiere(i).faces());
  }

  if (mes_faces_bord.size()==0 && mes_faces_raccord.size()==0 && Process::nproc()==1)
    {
      Cerr << "Warning, the reread zone " << nom
           << " has no defined boundaries (none boundary or connector)." << finl;
    }
  mes_faces_bord.associer_zone(*this);
  mes_faces_joint.associer_zone(*this);
  mes_faces_raccord.associer_zone(*this);
  mes_faces_int.associer_zone(*this);
  elem.associer_zone(*this);
  fixer_premieres_faces_frontiere();

  const int nbelem = mp_sum(mes_elems.dimension(0));
  Cerr << "  Number of elements: " << nbelem << finl;
  // Verifications sanitaires:
  // On doit avoir le meme nombre de frontieres et les memes noms sur tous les procs
  check_frontiere(mes_faces_bord, "(Bord)");
  check_frontiere(mes_faces_raccord, "(Raccord)");
  check_frontiere(mes_faces_int, "(Face_Interne)");

  return s ;
}

Entree& Zone::Lire_Bords_a_imprimer(Entree& is)
{
  Nom nom_lu;
  Motcle motlu, accolade_fermee="}", accolade_ouverte="{";
  is >> motlu;
  Cerr << "Reading boundaries to be print:"  << finl;
  if(motlu!=accolade_ouverte)
    {
      Cerr << "We expected a list { ... } of boundaries to be print" << finl;
      Cerr << "and not : " << motlu << finl;
      exit();
    }
  is >> nom_lu;
  while (nom_lu != "}")
    {
      Cerr << nom_lu << " ";
      Bords_a_imprimer_.add(Nom(nom_lu));
      is >> nom_lu;
    }
  Cerr << finl;
  return is ;
}

Entree& Zone::Lire_Bords_a_imprimer_sum(Entree& is)
{
  Nom nom_lu;
  Motcle motlu, accolade_fermee="}", accolade_ouverte="{";
  is >> motlu;
  Cerr << "Reading boundaries to be print:"  << finl;
  if(motlu!=accolade_ouverte)
    {
      Cerr << "We expected a list { ... } of boundaries to be print" << finl;
      Cerr << "and not : " << motlu << finl;
      exit();
    }
  is >> nom_lu;
  while (nom_lu != "}")
    {
      Cerr << nom_lu << " ";
      Bords_a_imprimer_sum_.add(Nom(nom_lu));
      is >> nom_lu;
    }
  Cerr << finl;
  return is ;
}

/////////////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Zone
//
/////////////////////////////////////////////////////////////////////


// Description:
//    Renvoie le domaine dont la zone fait partie.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Domaine&
//    Signification: le domaine dont la zone fait partie
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Domaine& Zone::domaine() const
{
  return le_domaine.valeur();
}

// Description:
//    Renvoie le domaine dont la zone fait partie.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Domaine&
//    Signification: le domaine dont la zone fait partie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Domaine& Zone::domaine()
{
  return le_domaine.valeur();
}

// Description:
//    che les numeros (indices) des elements contenants les sommets
//    specifies par le parametre "sommets".
//    Utilise:
//     ArrOfInt& Zone::chercher_elements(const DoubleTab&,ArrOfInt&) const
// Precondition:
// Parametre: IntTab& sommets
//    Signification: le tableau des numeros des sommets dont
//                   on cherche les elements correspondants
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: ArrOfInt& elem_
//    Signification: le tableau contenant les numeros des elements
//                   contenant les sommets specifies
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: ArrOfInt&
//    Signification: le tableau des numeros des sommets dont
//                   on cherche les elements correspondants
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
ArrOfInt& Zone::indice_elements(const IntTab& sommets,
                                ArrOfInt& elem_,
                                int reel) const
{
  int i,j,k;
  const DoubleTab& les_coord = domaine().coord_sommets();
  int sz_sommets = sommets.dimension(0);
  DoubleTab xg(sz_sommets,Objet_U::dimension);
  for(i=0; i<sz_sommets; i++)
    for(j=0; j<nb_som_elem(); j++)
      for(k=0; k<Objet_U::dimension; k++)
        xg(i,k)+=les_coord(sommets(i,j),k);
  xg/=nb_som_elem();
  return chercher_elements(xg,elem_,reel);
}

// Description:
//    Recherche des elements contenant les points dont les
//    coordonnees sont specifiees.
// Precondition:
// Parametre: DoubleTab& positions
//    Signification: les coordonnees des points dont on veut
//                   connaitre l'element correspondant
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: ArrOfInt& elements
//    Signification: le tableau des numeros des elements contenant
//                   les points specifies
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: ArrOfInt&
//    Signification: le tableau des numeros des elements contenant
//                   les points specifies
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
ArrOfInt& Zone::chercher_elements(const DoubleTab& positions,
                                  ArrOfInt& elements,
                                  int reel) const
{
  const OctreeRoot& octree = construit_octree(reel);
  int i, sz=positions.dimension(0);
  const int dim = positions.dimension(1);
  // resize_tab est virtuelle, si c'est un Vect ou un Tab elle appelle le
  // resize de la classe derivee:
  elements.resize_tab(sz, ArrOfInt::NOCOPY_NOINIT);
  double x, y=0, z=0;
  for (i=0; i<sz; i++)
    {
      x = positions(i,0);
      if (dim > 1) y = positions(i,1);
      if (dim > 2) z = positions(i,2);
      elements[i] = octree.rang_elem(x, y, z);
    }
  return elements;
}

// Description:
//    Recherche des elements contenant les points dont les
//    coordonnees sont specifiees.
// Precondition:
// Parametre: DoubleVect& positions
//    Signification: les coordonnees du point dont on veut
//                   connaitre l'element correspondant
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: ArrOfInt& elements
//    Signification: le tableau des numeros des elements contenant
//                   les points specifies
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: ArrOfInt&
//    Signification: le tableau des numeros des elements contenant
//                   les points specifies
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
ArrOfInt& Zone::chercher_elements(const DoubleVect& positions,
                                  ArrOfInt& elements,
                                  int reel) const
{
  //  Cerr<<"Dans Zone::chercher_elements(const DoubleVect&"<<finl;
  //  Cerr<<"codage lourd....."<<finl;
  int n=positions.size();
  if (n!=dimension)
    {
      Cerr<<"Zone::chercher_elements(const DoubleVect& positions, ArrOfInt& elements, int reel) const -> Coding is made to copy a doublevect(dimesnion) in a DoubleTab(1,dimension)"<<finl;
      Cerr<<"But, it comes with a DoubleVect of size "<<n << " instead of "<<dimension<<finl;
      assert(0);
      exit();
    }
  DoubleTab positions2(1,n);
  for (int ii=0; ii<n; ii++) positions2(0,ii)=positions(ii);
  return chercher_elements( positions2, elements, reel) ;
}
// Description:
//    Specifie le domaine dont la zone fait partie.
// Precondition:
// Parametre: Domaine& un_domaine
//    Signification: le domaine dont la zone fait partie
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la zone est considere comme faisant partie
//                du domaine specifie.
void Zone::associer_domaine(const Domaine& un_domaine)
{
  le_domaine=un_domaine;
}

// Description:
//    Renvoie le nombre de faces qui sont des bords.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de faces qui sont des bords
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Zone::nb_faces_bord() const
{
  return mes_faces_bord.nb_faces();
}

// Description:
//    Renvoie le nombre de joints de la zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de joints de la zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Zone::nb_faces_joint() const
{
  return mes_faces_joint.nb_faces();
}

// Description:
//    Renvoie le nombre de raccords de la zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de raccords de la zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Zone::nb_faces_raccord() const
{
  return mes_faces_raccord.nb_faces();
}

// Description:
//    Renvoie le nombre de face internes de la zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de face internes de la zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Zone::nb_faces_int() const
{
  return mes_faces_int.nb_faces();
}



// Description:
//    Renvoie le nombre de sommets de la zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de sommets de la zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int  Zone::nb_som() const
{
  // MONOZONE pour le moment!
  return domaine().nb_som();
}

// Description:
//    Renvoie le nombre total de sommets de la zone.
//    i.e. le nombre de sommets reels et virtuels
//    sur le processeur courant.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre total de sommets de la zone
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int  Zone::nb_som_tot() const
{
  // MONOZONE pour le moment!
  return domaine().nb_som_tot();
}

// Description:
//    Renvoie le nombre de faces du i-ieme bord.
// Precondition:
// Parametre: int i
//    Signification: le numero du bord dont on veut
//                   connaitre le nombre de faces
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de faces du i-ieme bord
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Zone::nb_faces_bord(int i) const
{
  return mes_faces_bord(i).nb_faces();
}

// Description:
//    Renvoie le nombre de faces du i-ieme joint.
// Precondition:
// Parametre: int i
//    Signification: le numero du joint dont on veut
//                   connaitre le nombre de faces
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: nombre de faces du i-ieme joint
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Zone::nb_faces_joint(int i) const
{
  return mes_faces_joint(i).nb_faces();
}

// Description:
//    Renvoie le nombre de faces du i-ieme raccord.
// Precondition:
// Parametre: int i
//    Signification: le numero du raccord dont on veut
//                   connaitre le nombre de faces
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de faces du i-ieme raccord
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Zone::nb_faces_raccord(int i) const
{
  return mes_faces_raccord(i)->nb_faces();
}
// Description:
//    Renvoie le nombre de faces de la i-ieme
//    liste de faces internes
// Precondition:
// Parametre: int i
//    Signification: le numero de la liste de faces internes dont on veut
//                   connaitre le nombre de faces
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int i
//    Signification: le nombre de faces de la i-ieme liste
//                   de faces internes
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Zone::nb_faces_int(int i) const
{
  return mes_faces_int(i).nb_faces();
}

// Description:
//    Renumerotation des noeuds:
//      Le noeud de numero k devient le noeud de numero Les_Nums[k]
// Precondition:
// Parametre: IntVect& Les_Nums
//    Signification: le vecteur contenant la nouvelle numerotation
//                   Nouveau_numero_noeud_i = Les_Nums[Ancien_numero_noeud_i]
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: les noeuds ont une nouvelle numerotation
void Zone::renum(const IntVect& Les_Nums)
{
  int dim0 = mes_elems.dimension(0);
  int dim1 = mes_elems.dimension(1);
  for(int i=0; i<dim0; i++)
    for(int j=0; j<dim1; j++)
      mes_elems(i,j)=Les_Nums[mes_elems(i,j)];
  for(int i=0; i<nb_bords(); i++)
    mes_faces_bord(i).renum(Les_Nums);
  for(int i=0; i<nb_joints(); i++)
    mes_faces_joint(i).renum(Les_Nums);
  for(int i=0; i<nb_raccords(); i++)
    mes_faces_raccord(i)->renum(Les_Nums);
  for(int i=0; i<nb_frontieres_internes(); i++)
    mes_faces_int(i).renum(Les_Nums);
}

// Description:
//    Renvoie -1 si face n'est pas une face interne
//    Renvoie le numero de la face dupliquee sinon.
// Precondition:
// Parametre: int face
//    Signification: le numero de la face interne a chercher
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: int
//    Signification: -1 si la face specifiee n'est pas une face interne
//                   le numero de la face dupliquee sinon
//    Contraintes:
// Exception: erreur TRUST (face non trouvee)
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Zone::face_interne_conjuguee(int face) const
{
  if( (face)>=nb_faces_frontiere())
    return -1;
  int compteur=nb_faces_frontiere()-nb_faces_int();
  if( (face)<compteur)
    return -1;

  CONST_LIST_CURSEUR(Faces_Interne) curseur(mes_faces_int);;;
  while(curseur)
    {
      const Faces& les_faces=curseur->faces();
      int nbf=les_faces.nb_faces();
      if(face < nbf + compteur)
        {
          nbf/=2;
          if( (face-compteur)<nbf) return face+nbf;
          else return face-nbf;
        }
      compteur+=(2*nbf);
      ++curseur;
    }
  Cerr << "TRUST error " << finl;
  assert(0);
  exit();
  return -1;
}

// Description:
//    Concatene les bords de meme nom et ceci pour:
//    les bords, les bords periodiques et les faces internes.
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
// Postcondition: les bords qui portent le meme nom sont
//                regroupes dans un seul
int Zone::comprimer()
{
  {
    // Les Bords
    LIST_CURSEUR(Bord) curseur(mes_faces_bord);;;
    while(curseur)
      {
        Frontiere& front=curseur.valeur();
        // Au cas ou la zone de la frontiere n'est pas la bonne zone
        front.associer_zone(*this);
        LIST_CURSEUR(Bord) curseur2(curseur.list());;;
        Journal() << "Zone::comprimer() bord : " << front.le_nom() << finl;
        while(!curseur2.list().est_dernier())
          {
            Frontiere& front2=curseur2.list().suivant().valeur();
            if(front.le_nom() == front2.le_nom())
              {
                Journal() << "On agglomere le bord : " << front.le_nom() << finl;
                front.add(front2);
                curseur2.list().supprimer();
              }
            else
              ++curseur2;
          }
        Journal() << front.le_nom() << " est associee a : " << front.zone().le_nom() << finl;
        ++curseur;
      }
  }

  {
    // Les Faces Internes :
    LIST_CURSEUR(Faces_Interne) curseur(mes_faces_int);;;
    while(curseur)
      {
        Frontiere& front=curseur.valeur();
        LIST_CURSEUR(Faces_Interne) curseur2(curseur.list());;;
        while(!curseur2.list().est_dernier())
          {
            Frontiere& front2=curseur2.list().suivant().valeur();
            if(front.le_nom() == front2.le_nom())
              {
                front.add(front2);
                curseur2.list().supprimer();
              }
            else
              ++curseur2;
          }
        ++curseur;
      }
  }
  {
    // Les Raccords
    LIST_CURSEUR(Raccord) curseur(mes_faces_raccord);;;
    while(curseur)
      {
        Frontiere& front=curseur->valeur();
        LIST_CURSEUR(Raccord) curseur2(curseur.list());;;
        Journal() << "Raccord : " << front.le_nom() << finl;
        while(!curseur2.list().est_dernier())
          {
            Frontiere& front2=curseur2.list().suivant()->valeur();
            if(front.le_nom() == front2.le_nom())
              {
                front.add(front2);
                curseur2.list().supprimer();
              }
            else
              ++curseur2;
          }
        ++curseur;
      }
  }
  return 1;
}

// Description:
//    Ecriture des noms des bords sur un flot de sortie
//    Ecrit les noms des: bords, bords periodiques, raccords
//                        et faces internes.
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
void Zone::ecrire_noms_bords(Sortie& os) const
{
  {
    // Les Bords
    CONST_LIST_CURSEUR(Bord) curseur(mes_faces_bord);
    while(curseur)
      {
        os << curseur->le_nom() << finl;
        ++curseur;
      }
  }

  {
    // Les Faces Internes :
    CONST_LIST_CURSEUR(Raccord) curseur(mes_faces_raccord);
    while(curseur)
      {
        os << curseur.valeur()->le_nom() << finl;
        ++curseur;
      }
  }

  {
    // Les Faces Internes :
    CONST_LIST_CURSEUR(Faces_Interne) curseur(mes_faces_int);
    while(curseur)
      {
        os << curseur->le_nom() << finl;
        ++curseur;
      }
  }
}

double Zone::epsilon() const
{
  return domaine().epsilon();
}


// Description:
//    Renvoie le nombre de faces du type specifie.
// Precondition:
// Parametre: Type_Face type
//    Signification: un type de face
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de faces du type specifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Zone::nb_faces_bord(Type_Face type) const
{
  return mes_faces_bord.nb_faces(type);
}

// Description:
//    Renvoie le nombre de joints du type specifie
// Precondition:
// Parametre: Type_Face type
//    Signification: un type de face
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de joints du type specifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Zone::nb_faces_joint(Type_Face type) const
{
  return mes_faces_joint.nb_faces(type);
}

// Description:
//    Renvoie le nombre de raccords du type specifie
// Precondition:
// Parametre: Type_Face type
//    Signification: un type de face
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de raccords du type specifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Zone::nb_faces_raccord(Type_Face type) const
{
  return mes_faces_raccord.nb_faces(type);
}

// Description:
//    Renvoie le nombre de faces interieures du type specifie
// Precondition:
// Parametre: Type_Face type
//    Signification: un type de face
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de faces interieures du type specifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Zone::nb_faces_int(Type_Face type) const
{
  return mes_faces_int.nb_faces(type);
}

// Description:
//    Renvoie le rang de l'element contenant
//    le point dont les coordonnees sont specifiees.
// Precondition:
// Parametre: double x
//    Signification: coordonnee en X
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: double y
//    Signification: coordonnee en Y
//    Valeurs par defaut: 0
//    Contraintes:
//    Acces:
// Parametre: double z
//    Signification: coordonnee en Z
//    Valeurs par defaut: 0
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le rang de l'element contenant
//                   le point dont les coordonnees sont specifiees.
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Zone::chercher_elements(double x, double y, double z,int reel) const
{
  const OctreeRoot& octree = construit_octree(reel);
  return octree.rang_elem(x, y, z);
}

// Description:
// Precondition:
// Parametre: DoubleTab& pos
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: ArrOfInt& som
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: ArrOfInt&
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
ArrOfInt& Zone::chercher_sommets(const DoubleTab& pos,
                                 ArrOfInt& som,
                                 int reel) const
{
  const OctreeRoot& octree = construit_octree(reel);
  octree.rang_sommet(pos, som);
  return som;
}

// Description:
// Precondition:
// Parametre: DoubleTab& pos
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: IntTab& aretes_som
//    Signification: la definition des aretes par leurs sommets
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: ArrOfInt& aretes
//    Signification: Liste des aretes trouvees
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
ArrOfInt& Zone::chercher_aretes(const DoubleTab& pos,
                                ArrOfInt& aretes,
                                int reel) const
{
  const OctreeRoot& octree = construit_octree(reel);
  octree.rang_arete(pos, aretes);
  return aretes;
}

// Description:
// Precondition:
// Parametre: double x
//    Signification: coordonnee en X
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: double y
//    Signification: coordonnee en Y
//    Valeurs par defaut: 0
//    Contraintes:
//    Acces:
// Parametre: double z
//    Signification: coordonnee en Z
//    Valeurs par defaut: 0
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Zone::chercher_sommets(double x, double y, double z, int reel) const
{
  const OctreeRoot& octree = construit_octree(reel);
  return octree.rang_sommet(x, y, z);
}

int Zone::rang_frontiere(const Nom& un_nom) const
{
  int i=0;
  {
    CONST_LIST_CURSEUR(Bord) curseur(mes_faces_bord);
    while(curseur)
      {
        if(curseur->le_nom() == un_nom)
          return i;
        ++curseur;
        ++i;
      }
  }
  {
    CONST_LIST_CURSEUR(Raccord) curseur(mes_faces_raccord);
    while(curseur)
      {
        if(curseur.valeur()->le_nom() == un_nom)
          return i;
        ++curseur;
        ++i;
      }
  }

  {
    // Les Faces Internes :
    CONST_LIST_CURSEUR(Faces_Interne) curseur(mes_faces_int);
    while(curseur)
      {
        if(curseur->le_nom() == un_nom)
          return i;
        ++curseur;
        ++i;
      }
  }
  Cerr << "Zone::rang_frontiere(): We not found boundary with name " << un_nom << finl;
  exit();
  return -1;
}

const Frontiere& Zone::frontiere(const Nom& un_nom) const
{
  int i = rang_frontiere(un_nom);
  return frontiere(i);
}

Frontiere& Zone::frontiere(const Nom& un_nom)
{
  int i = rang_frontiere(un_nom);
  return frontiere(i);
}

void Zone::fixer_premieres_faces_frontiere()
{
  Journal() << "Zone::fixer_premieres_faces_frontiere()" << finl;
  int compteur=0;
  {
    LIST_CURSEUR(Bord) curseur(mes_faces_bord);
    while(curseur)
      {
        curseur->fixer_num_premiere_face(compteur);
        compteur+=curseur->nb_faces();
        Journal() << "Le bord " << curseur->le_nom() << " commence a la face : "
                  << curseur->num_premiere_face() << finl;
        ++curseur;
      }
  }
  {
    LIST_CURSEUR(Raccord) curseur(mes_faces_raccord);
    while(curseur)
      {
        curseur.valeur()->fixer_num_premiere_face(compteur);
        compteur+=curseur.valeur()->nb_faces();
        Journal() << "Le raccord " << curseur->le_nom() << " commence a la face : "
                  << curseur.valeur()->num_premiere_face() << finl;
        ++curseur;
      }
  }
  {
    LIST_CURSEUR(Joint) curseur(mes_faces_joint);
    while(curseur)
      {
        curseur->fixer_num_premiere_face(compteur);
        compteur+=curseur->nb_faces();
        Journal() << "Le joint " << curseur->le_nom() << " commence a la face : "
                  << curseur->num_premiere_face() << finl;
        ++curseur;
      }
  }
  // GF recalcul de nb_faces_Cl (inutile dans la 1.5.8)
  // nb_faces_Cl = nb_faces_bord() + nb_faces_raccord() + nb_faces_int();
}

// Construction du tableau elem_virt_pe_num_ a partir du tableau mes_elems
// (on se sert des espaces distants et virtuels de mes_elems).
// Algorithme non optimal en memoire : on duplique mes_elems alors qu'on a
// besoin que d'un tableau a deux colonnes.
// Voir Zone.h : elem_virt_pe_num_
void Zone::construire_elem_virt_pe_num()
{
  IntTab tableau_echange(mes_elems);
  assert(tableau_echange.dimension(1) >= 2);
  int i;
  const int n = nb_elem();
  const int n_virt = nb_elem_tot() - n;
  const int moi = me();
  for (i = 0; i < n; i++)
    {
      tableau_echange(i, 0) = moi;
      tableau_echange(i, 1) = i;
    }
  tableau_echange.echange_espace_virtuel();

  elem_virt_pe_num_.resize(n_virt, 2);
  for (i = 0; i < n_virt; i++)
    {
      elem_virt_pe_num_(i, 0) = tableau_echange(n + i, 0);
      elem_virt_pe_num_(i, 1) = tableau_echange(n + i, 1);
    }
}

const IntTab& Zone::elem_virt_pe_num() const
{
  return elem_virt_pe_num_;
}
// Description:
// Calcule le centre de gravite de la zone
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
void Zone::calculer_mon_centre_de_gravite(ArrOfDouble& c)
{
  c=0;
  // Volumes computed cause stored in Zone_VF and so not available in Zone...
  DoubleVect volumes;
  DoubleVect inverse_volumes;
  calculer_volumes(volumes,inverse_volumes);
  DoubleTab xp;
  calculer_centres_gravite(xp);
  double volume=0;
  for (int i=0; i<nb_elem(); i++)
    for (int j=0; j<dimension; j++)
      {
        c(j)+=xp(i,j)*volumes(i);
        volume+=volumes(i);
      }
  // Cas de Zone vide:
  if (volume>0)
    c/=volume;
  cg_moments_=c;
  volume_total_ = mp_somme_vect(volumes);
}

// Description:
//    Calcule les volumes des elements de la zone.
// Precondition:
// Parametre: DoubleVect& volumes
//    Signification: le tableau contenant les volumes
//                   des elements de la zone
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Zone::calculer_volumes(DoubleVect& volumes, DoubleVect& inverse_volumes) const
{
  if (!volumes.get_md_vector().non_nul())
    creer_tableau_elements(volumes, Array_base::NOCOPY_NOINIT);
  elem.calculer_volumes(volumes); // Dimensionne et calcule le DoubleVect volumes
  // Check and fill inverse_volumes
  if (!inverse_volumes.get_md_vector().non_nul())
    creer_tableau_elements(inverse_volumes, Array_base::NOCOPY_NOINIT);
  int size = volumes.size_totale();
  for (int i=0; i<size; i++)
    {
      double v = volumes(i);
      if (v<=0.)
        {
          Cerr << "Volume[" << i << "]=" << v << finl;
          Cerr << "Several volumes of the mesh are not positive." << finl;
          Cerr << "Something is wrong in the mesh..." << finl;
          exit();
        }
      inverse_volumes(i)=1./v;
    }
}

// Description:
//    Calcule les centres de gravites des aretes de la zone.
// Precondition:
// Parametre: DoubleTab& xa
//    Signification: le tableau contenant les centres de
//                   gravites des aretes de la zone
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Zone::calculer_centres_gravite_aretes(DoubleTab& xa) const
{
  const DoubleTab& coord = domaine().coord_sommets();
  // Calcule les centres de gravite des aretes reelles seulement
  xa.resize(nb_aretes(),dimension);
  for (int i=0; i<nb_aretes(); i++)
    for (int j=0; j<dimension; j++)
      xa(i,j) = 0.5*(coord(Aretes_som(i,0),j)+coord(Aretes_som(i,1),j));
}

int Zone::rang_elem_depuis(const DoubleTab& coord,
                           const ArrOfInt& elems,
                           ArrOfInt& prems) const
{
  Cerr << "Zone::rang_elem_depuis: function not any more implemented" << finl;
  exit();
  return -1;
}

void Zone::rang_elems_sommet(ArrOfInt& elems,
                             double x, double y, double z) const
{
  const OctreeRoot octree = construit_octree();
  octree.rang_elems_sommet(elems,x,y,z);
}

void Zone::invalide_octree()
{
  if (deriv_octree_.non_nul())
    deriv_octree_.detach();
}

const OctreeRoot& Zone::construit_octree() const
{
  if (!deriv_octree_.non_nul())
    deriv_octree_.typer("OctreeRoot");
  OctreeRoot& octree = deriv_octree_.valeur();
  if (!octree.construit())
    {
      octree.associer_Zone(*this);
      octree.construire();
    }
  return octree;
}

// Description: construction de l'octree si pas deja fait
const OctreeRoot& Zone::construit_octree(int& reel) const
{
  if (!deriv_octree_.non_nul())
    deriv_octree_.typer("OctreeRoot");
  OctreeRoot& octree = deriv_octree_.valeur();
  if (!octree.construit() || (reel != octree.reel()))
    {
      octree.associer_Zone(*this);
      octree.construire(reel);
    }
  return octree;
}

// Description: creation d'un tableau parallele de valeurs aux elements.
//  Voir MD_Vector_tools::creer_tableau_distribue()
void Zone::creer_tableau_elements(Array_base& x, Array_base::Resize_Options opt) const
{
  const MD_Vector& md = md_vector_elements();
  MD_Vector_tools::creer_tableau_distribue(md, x, opt);
}

// Description: renvoie le descripteur parallele des tableaux aux elements de la zone
const MD_Vector& Zone::md_vector_elements() const
{
  const MD_Vector& md = mes_elems.get_md_vector();
  if (!md.non_nul())
    {
      Cerr << "Internal error in Zone::md_vector_elements(): descriptor for elements not initialized\n"
           << " You might use a buggy Domain constructor that does not build descriptors,\n"
           << " Use the following syntax to finish the domain construction\n"
           << "  Scatter ; " << domaine().le_nom() << finl;
      exit();
    }
  // Pour l'instant je prends le descripteur dans le tableau mes_elems, mais on
  // pourrait en stocker une copie dans la zone si ca a un interet...
  return md;
}

double Zone::volume_total() const
{
  assert(volume_total_ >= 0.); // Pas calcule ???
  return volume_total_;
}

// Selection d'un item unique (sommet, face ...) parmi une liste (item_possible)
// afin d'assurer le parallelisme de certains algorithmes
// La selection est faite en testant la distance entre les coordonnees (coord_possible)
// localisant ces items par rapport aux coordonnes (coord_ref) d'un point de reference.
// L'item retenu est celui qui presente la distance minimum par rapport au point de reference.
// S'il reste plusieurs items se trouvant a la meme distance du point de reference
// alors on repete le test en translatant le point de reference

int Zone::identifie_item_unique(IntList& item_possible,
                                DoubleTab& coord_possible,
                                const DoubleVect& coord_ref)
{
  int it_selection = -1;
  DoubleTab decentre_face(4,Objet_U::dimension);
  decentre_face = 0.;
  for (int t=1; t<4; t++)
    for (int dir=0; dir<Objet_U::dimension; dir++)
      if (dir==(t-1))
        decentre_face(t,dir) = 1.;
  // decentre_face(0,0:dim)={0,0,0}
  // decentre_face(1,0:dim)={1,0,0}
  // decentre_face(2,0:dim)={0,1,0}
  // decentre_face(3,0:dim)={0,0,1}

  //Au premier passage (t=0) pas de translation effectuee
  DoubleVect dist;
  assert (item_possible.size()!=0);
  int t = 0;
  while ((item_possible.size()!=1) && (t<4))
    {
      double distmin = DMAXFLOAT;
      int size_initiale = item_possible.size();
      dist.resize(size_initiale);
      dist = 0.;

      for (int ind_it=0; ind_it<size_initiale; ind_it++)
        {
          for (int dir=0; dir<Objet_U::dimension; dir++)
            dist[ind_it] += (coord_possible(ind_it,dir)-(coord_ref(dir)+decentre_face(t,dir)))*(coord_possible(ind_it,dir)-(coord_ref(dir)+decentre_face(t,dir)));
          if (dist[ind_it]<=distmin)
            distmin = dist[ind_it];
        }

      int ind_it = 0;
      int nb_it_suppr = 0;
      while (ind_it<size_initiale)
        {
          if (!est_egal(dist[ind_it],distmin))
            {
              int ind_it_suppr = ind_it-nb_it_suppr;
              int it_suppr = item_possible[ind_it_suppr];
              item_possible.suppr(it_suppr);

              int size_actuelle = item_possible.size();
              for (int ind=ind_it_suppr; ind<size_actuelle; ind++)
                for (int dir=0; dir<dimension; dir++)
                  coord_possible(ind,dir) = coord_possible(ind+1,dir);
              coord_possible.resize(size_actuelle,dimension,Array_base::COPY_NOINIT);
              nb_it_suppr++;
            }
          ind_it++;
        }
      t++;
    }
  if (item_possible.size()==1)
    it_selection = item_possible[0];
  else
    {
      Cerr<<"Zone::identifie_item_unique()"<<finl;
      Cerr<<"An item has not been found among the list."<<finl;
      Cerr<<"Please contact TRUST support."<<finl;
      Process::exit();
    }
  return it_selection;
}

