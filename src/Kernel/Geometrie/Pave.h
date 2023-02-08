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

#ifndef Pave_included
#define Pave_included
#include <Domaine.h>


/*! @brief Classe Pave Un domaine particulierement facile a mailler!
 *
 *     La structure du jeu de donnee pour specifier un Pave est:
 *     Pave nom_pave
 *      {
 *      Origine OX OY (OZ)
 *      Longueurs LX LY (LZ)
 *      Nombre_de_noeuds NX NY (NZ)
 *      Facteurs Fx Fy (Fz)
 *      (Symx)
 *      (Symy)
 *      (Symz)
 *      }
 *      {
 *      (Bord)  nom X = X0 Y0 <= Y <= Y1 Z0 <= Z <= Z1
 *      ...
 *      (Raccord)  local homogene nom X = X0 Y0 <= Y <= Y1 Z0 <= Z <= Z1
 *      ...
 *      (Internes)  nom X = X0 Y0 <= Y <= Y1 Z0 <= Z <= Z1
 *      ...
 *      (Joint)  nom X = X0 Y0 <= Y <= Y1 Z0 <= Z <= Z1 PE_voisin
 *      ...
 *      }
 *
 * @sa Domaine Mailler, Actuellemnt c'est le seul type d'objet reconnu par Trio-U pour mailler, un domaine
 */
class Pave : public Domaine
{
  Declare_instanciable(Pave);
public :

protected:
  DoubleVect Origine, Longueurs, Facteurs, Pas;
  IntVect Nb_Noeuds, Les_Nums, Symetrique;
  DoubleTab Les_Noeuds;

  int Nx = -1, Ny = -1, Nz = -1, Mx = -1, My = -1, Mz = -1;
  double a_tanh= 10.;  // a pour le maillage en tanh dans la diry!!
  int tanh_dilatation=0;
  double xa_tanh= 10.;  // xa pour le maillage en tanh dans la dirx!!
  int xtanh_dilatation=0;
  double za_tanh= 10.;  // za pour le maillage en tanh dans la dirz!!
  int ztanh_dilatation=0;
  int rep_VEF=0;
  int tour_complet = 0;

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

  void maille1D();
  void maille2D();
  void maille3D();

  void typer_();

  void lire_longueurs(Entree& is);
  void lire_noeuds(Entree& is);
  void lire_front(Entree& , Frontiere& );
};

/*! @brief Renvoie le numero de la i-ieme maille (suivant X)
 *
 * @param (int i) le rang de la maille suivant X dont on veut le numero
 * @return (int) le numero de la maille cherchee
 */
inline int Pave::numero_maille(int i)
{
  assert(dimension == 1);
  assert(i < Nx);
  return i;
}

/*! @brief Renvoie le numero de la (i,j)-ieme maille (suivant (X,Y)).
 *
 * @param (int i) le rang de la maille suivant X
 * @param (int j) le rang de la maille suivant Y
 * @return (int) le numero de la maille cherchee
 */
inline int Pave::numero_maille(int i, int j)
{
  assert(dimension == 2);
  assert(i < Nx);
  assert(j < Ny);
  return j*Nx+i;
}

/*! @brief Renvoie le numero de la (i,j,k)-ieme maille (suivant (X,Y,Z)).
 *
 * @param (int i) le rang de la maille suivant X
 * @param (int j) le rang de la maille suivant Y
 * @param (int k) le rang de la maille suivant Z
 * @return (int) le numero de la maille cherchee
 */
inline int Pave::numero_maille(int i, int j, int k)
{
  assert(dimension == 3);
  assert(i < Nx);
  assert(j < Ny);
  assert(k < Nz);
  return k*Ny*Nx+j*Nx+i;
}

/*! @brief Renvoie le numero du i-ieme sommet (suivant X)
 *
 * @param (int i) le rang du sommet suivant X dont on veut le numero
 * @return (int) le numero du sommet cherche
 */
inline int Pave::numero_sommet(int i)
{
  assert(dimension == 1);
  assert(i < Mx);
  return i;
}

/*! @brief Renvoie le numero du (i,j)-ieme sommet (suivant (X,Y)).
 *
 * @param (int i) le rang du sommet suivant X
 * @param (int j) le rang du sommet suivant Y
 * @return (int) le numero du sommet cherche
 */
inline int Pave::numero_sommet(int i, int j)
{
  assert(dimension == 2);
  assert(i < Mx);
  assert(tour_complet!=-123);
  if((tour_complet) && (j==My))
    j=0;
  return j*Mx+i;
}

/*! @brief Renvoie le numero du (i,j,k)-ieme sommet (suivant (X,Y,Z)).
 *
 * @param (int i) le rang du sommet suivant X
 * @param (int j) le rang du sommet suivant Y
 * @param (int k) le rang du sommet suivant Z
 * @return (int) le numero du sommet cherche
 */
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

/*! @brief Renvoie une reference sur le numero du l-ieme sommet de la i-ieme maille (suivant X) du pave.
 *
 * @param (int i) le rang de la maille suivant X
 * @param (int l) le rang du sommet cherche
 * @return (int&) reference sur le numero du sommet cherche
 */
inline int& Pave::maille_sommet(int i, int l)
{
  assert(dimension == 1);
  return mes_elems_(numero_maille(i),l);
}

/*! @brief Renvoie une reference sur le numero du l-ieme sommet de la (i,j)-ieme maille (suivant (X,Y)) du pave.
 *
 * @param (int i) le rang de la maille suivant X
 * @param (int j) le rang de la maille suivant Y
 * @param (int l) le rang du sommet cherche
 * @return (int&) reference sur le numero du sommet cherche
 */
inline int& Pave::maille_sommet(int i, int j, int l)
{
  assert(dimension == 2);
  return mes_elems_(numero_maille(i, j),l);
}

/*! @brief Renvoie une reference sur le numero du l-ieme sommet de la (i,j,k)-ieme maille (suivant (X,Y,Z)) du pave.
 *
 * @param (int i) le rang de la maille suivant X
 * @param (int j) le rang de la maille suivant Y
 * @param (int j) le rang de la maille suivant Z
 * @param (int l) le rang du sommet cherche
 * @return (int&) reference sur le numero du sommet cherche
 */
inline int& Pave::maille_sommet(int i, int j, int k, int l)
{
  assert(dimension == 3);
  return mes_elems_(numero_maille(i, j, k), l);
}

/*! @brief Renvoie une reference sur les coordonnees du i-ieme noeud.
 *
 * @param (int i) le rang du noeud suivant X
 * @return (double&) reference sur les coordonnees du noeud cherche
 */
inline double& Pave::coord_noeud(int i)
{
  assert(dimension == 1);
  return sommets_(numero_sommet(i));
}

/*! @brief Renvoie une reference sur les coordonnees du (i,j)-ieme noeud.
 *
 * @param (int i) le rang du noeud suivant X
 * @param (int j) le rang du noeud suivant Y
 * @return (double&) reference sur les coordonnees du noeud cherche
 */
inline double& Pave::coord_noeud(int i, int j, int l)
{
  assert(dimension == 2);
  return sommets_(numero_sommet(i, j),l);
}

/*! @brief Renvoie une reference sur les coordonnees du (i,j,k)-ieme noeud.
 *
 * @param (int i) le rang du noeud suivant X
 * @param (int j) le rang du noeud suivant Y
 * @param (int k) le rang du noeud suivant Z
 * @return (double&) reference sur les coordonnees du noeud cherche
 */
inline double& Pave::coord_noeud(int i, int j, int k, int l)
{
  assert(dimension == 3);
  return sommets_(numero_sommet(i, j, k),l);
}

#endif
