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
// File:        Pave.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Pave_included
#define Pave_included
#include <Zone.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Pave
//    Une zone particulierement facile a mailler!
//    La structure du jeu de donnee pour specifier un Pave est:
//    Pave nom_pave
//     {
//     Origine OX OY (OZ)
//     Longueurs LX LY (LZ)
//     Nombre_de_noeuds NX NY (NZ)
//     Facteurs Fx Fy (Fz)
//     (Symx)
//     (Symy)
//     (Symz)
//     }
//     {
//     (Bord)  nom X = X0 Y0 <= Y <= Y1 Z0 <= Z <= Z1
//     ...
//     (Raccord)  local homogene nom X = X0 Y0 <= Y <= Y1 Z0 <= Z <= Z1
//     ...
//     (Internes)  nom X = X0 Y0 <= Y <= Y1 Z0 <= Z <= Z1
//     ...
//     (Joint)  nom X = X0 Y0 <= Y <= Y1 Z0 <= Z <= Z1 PE_voisin
//     ...
//     }
// .SECTION voir aussi
//    Zone Mailler
//    Actuellemnt c'est le seul type d'objet reconnu par Trio-U pour mailler
//    un domaine
//////////////////////////////////////////////////////////////////////////////
class Pave : public Zone
{
  Declare_instanciable(Pave);
public :
  void associer_domaine(const Domaine& );
  inline IntVect& Nb_Noeud();
  DoubleVect Origine;
  DoubleVect Longueurs;
  IntVect Nb_Noeuds;
  DoubleVect Facteurs;
  DoubleVect Pas;
  DoubleTab Les_Noeuds;
  IntVect Les_Nums;
  IntVect Symetrique;
  double a_tanh;  // a pour le maillage en tanh valable que dans la diry!!
  int tanh_dilatation;
  int rep_VEF;
  inline int numero_maille(int );
  inline int numero_maille(int, int );
  inline int numero_maille(int , int, int);
  inline int numero_sommet(int );
  inline int numero_sommet(int, int );
  inline int numero_sommet(int, int, int );
  inline int& maille_sommet(int ,int);
  inline int& maille_sommet(int, int, int);
  inline int& maille_sommet(int, int, int, int);
  inline double& coord_noeud(int);
  inline double& coord_noeud(int, int, int);
  inline double& coord_noeud(int, int, int, int);

  int Nx, Ny, Nz, Mx, My, Mz;

  void maille1D();
  void maille2D();
  void maille3D();
  void typer_();
  void lire_Longueurs(Entree& is);
  void lire_Noeuds(Entree& is);
  void lire_front(Entree& , Frontiere& );

  int tour_complet;
};

// Description:
//    Renvoie le nombre de noeuds du pave dans
//    chacune des dimensions d'espace.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: IntVect&
//    Signification: vecteur d'entier contenant
//                   le nombre de noeud du pave suivant
//                   X, Y et Z
//    Contraintes: taille du vecteur = dimension de l'espace
// Exception:
// Effets de bord:
// Postcondition:
inline IntVect& Pave::Nb_Noeud()
{
  return Nb_Noeuds;
}


// Description:
//    Renvoie le numero de la i-ieme maille
//    (suivant X)
// Precondition: utilisable en dimension 1
// Parametre: int i
//    Signification: le rang de la maille suivant X
//                   dont on veut le numero
//    Valeurs par defaut:
//    Contraintes: i < nombre de maille suivant X
//    Acces:
// Retour: int
//    Signification: le numero de la maille cherchee
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int Pave::numero_maille(int i)
{
  assert(dimension == 1);
  assert(i < Nx);
  return i;
}

// Description:
//    Renvoie le numero de la (i,j)-ieme maille
//    (suivant (X,Y)).
// Precondition: utilisable en dimension 2
// Parametre: int i
//    Signification: le rang de la maille suivant X
//    Valeurs par defaut:
//    Contraintes: i < nombre de maille suivant X
//    Acces:
// Parametre: int j
//    Signification: le rang de la maille suivant Y
//    Valeurs par defaut:
//    Contraintes: j < nombre de maille suivant Y
//    Acces:
// Retour: int
//    Signification: le numero de la maille cherchee
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int Pave::numero_maille(int i, int j)
{
  assert(dimension == 2);
  assert(i < Nx);
  assert(j < Ny);
  return j*Nx+i;
}

// Description:
//    Renvoie le numero de la (i,j,k)-ieme maille
//    (suivant (X,Y,Z)).
// Precondition: utilisable en dimension 3
// Parametre: int i
//    Signification: le rang de la maille suivant X
//    Valeurs par defaut:
//    Contraintes: i < nombre de maille suivant X
//    Acces:
// Parametre: int j
//    Signification: le rang de la maille suivant Y
//    Valeurs par defaut:
//    Contraintes: j < nombre de maille suivant Y
//    Acces:
// Parametre: int k
//    Signification: le rang de la maille suivant Z
//    Valeurs par defaut:
//    Contraintes: k < nombre de maille suivant Z
//    Acces:
// Retour: int
//    Signification: le numero de la maille cherchee
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int Pave::numero_maille(int i, int j, int k)
{
  assert(dimension == 3);
  assert(i < Nx);
  assert(j < Ny);
  assert(k < Nz);
  return k*Ny*Nx+j*Nx+i;
}

// Description:
//    Renvoie le numero du i-ieme sommet
//    (suivant X)
// Precondition: utilisable en dimension 1
// Parametre: int i
//    Signification: le rang du sommet suivant X
//                   dont on veut le numero
//    Valeurs par defaut:
//    Contraintes: i < nombre de sommets suivant X
//    Acces:
// Retour: int
//    Signification: le numero du sommet cherche
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int Pave::numero_sommet(int i)
{
  assert(dimension == 1);
  assert(i < Mx);
  return i;
}

// Description:
//    Renvoie le numero du (i,j)-ieme sommet
//    (suivant (X,Y)).
// Precondition: utilisable en dimension 2
// Parametre: int i
//    Signification: le rang du sommet suivant X
//    Valeurs par defaut:
//    Contraintes: i < nombre de sommets suivant X
//    Acces:
// Parametre: int j
//    Signification: le rang du sommet suivant Y
//    Valeurs par defaut:
//    Contraintes: j < nombre de sommets suivant Y
//    Acces:
// Retour: int
//    Signification: le numero du sommet cherche
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int Pave::numero_sommet(int i, int j)
{
  assert(dimension == 2);
  assert(i < Mx);
  assert(tour_complet!=-123);
  if((tour_complet) && (j==My))
    j=0;
  return j*Mx+i;
}

// Description:
//    Renvoie le numero du (i,j,k)-ieme sommet
//    (suivant (X,Y,Z)).
// Precondition: utilisable en dimension 2
// Parametre: int i
//    Signification: le rang du sommet suivant X
//    Valeurs par defaut:
//    Contraintes: i < nombre de sommets suivant X
//    Acces:
// Parametre: int j
//    Signification: le rang du sommet suivant Y
//    Valeurs par defaut:
//    Contraintes: j < nombre de sommets suivant Y
//    Acces:
// Parametre: int k
//    Signification: le rang du sommet suivant Z
//    Valeurs par defaut:
//    Contraintes: k < nombre de sommets suivant Z
//    Acces:
// Retour: int
//    Signification: le numero du sommet cherche
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int Pave::numero_sommet(int i, int j, int k  )
{
  assert(dimension == 3);
  assert(i < Mx);
  assert(tour_complet!=-123);
  if((tour_complet) && (j==My))
    j=0;
  assert(j < My);
  assert(k < Mz);
  return k*My*Mx+j*Mx+i;
}

// Description:
//    Renvoie une reference sur le numero du l-ieme sommet
//    de la i-ieme maille (suivant X) du pave.
// Precondition: utilisable en dimension 1
// Parametre: int i
//    Signification: le rang de la maille suivant X
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int l
//    Signification: le rang du sommet cherche
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int&
//    Signification: reference sur le numero du sommet cherche
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int& Pave::maille_sommet(int i, int l)
{
  assert(dimension == 1);
  return mes_elems(numero_maille(i),l);
}

// Description:
//    Renvoie une reference sur le numero du l-ieme sommet
//    de la (i,j)-ieme maille (suivant (X,Y)) du pave.
// Precondition: utilisable en dimension 2
// Parametre: int i
//    Signification: le rang de la maille suivant X
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int j
//    Signification: le rang de la maille suivant Y
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int l
//    Signification: le rang du sommet cherche
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int&
//    Signification: reference sur le numero du sommet cherche
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int& Pave::maille_sommet(int i, int j, int l)
{
  assert(dimension == 2);
  return mes_elems(numero_maille(i, j),l);
}

// Description:
//    Renvoie une reference sur le numero du l-ieme sommet
//    de la (i,j,k)-ieme maille (suivant (X,Y,Z)) du pave.
// Precondition: utilisable en dimension 3
// Parametre: int i
//    Signification: le rang de la maille suivant X
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int j
//    Signification: le rang de la maille suivant Y
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int j
//    Signification: le rang de la maille suivant Z
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int l
//    Signification: le rang du sommet cherche
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int&
//    Signification: reference sur le numero du sommet cherche
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int& Pave::maille_sommet(int i, int j, int k, int l)
{
  assert(dimension == 3);
  return mes_elems(numero_maille(i, j, k), l);
}

// Description:
//    Renvoie une reference sur les coordonnees
//    du i-ieme noeud.
// Precondition: utilisable en dimension 1
// Parametre: int i
//    Signification: le rang du noeud suivant X
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double&
//    Signification: reference sur les coordonnees du noeud cherche
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline double& Pave::coord_noeud(int i)
{
  assert(dimension == 1);
  return Les_Noeuds(numero_sommet(i));
}

// Description:
//    Renvoie une reference sur les coordonnees
//    du (i,j)-ieme noeud.
// Precondition: utilisable en dimension 2
// Parametre: int i
//    Signification: le rang du noeud suivant X
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int j
//    Signification: le rang du noeud suivant Y
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double&
//    Signification: reference sur les coordonnees du noeud cherche
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline double& Pave::coord_noeud(int i, int j, int l)
{
  assert(dimension == 2);
  return Les_Noeuds(numero_sommet(i, j),l);
}

// Description:
//    Renvoie une reference sur les coordonnees
//    du (i,j,k)-ieme noeud.
// Precondition: utilisable en dimension 3
// Parametre: int i
//    Signification: le rang du noeud suivant X
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int j
//    Signification: le rang du noeud suivant Y
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int k
//    Signification: le rang du noeud suivant Z
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double&
//    Signification: reference sur les coordonnees du noeud cherche
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline double& Pave::coord_noeud(int i, int j, int k, int l)
{
  assert(dimension == 3);
  return Les_Noeuds(numero_sommet(i, j, k),l);
}

#endif
