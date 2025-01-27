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
template <typename _SIZE_>
class Pave_32_64 : public Domaine_32_64<_SIZE_>
{
  Declare_instanciable_32_64(Pave_32_64);
public :
  using int_t = _SIZE_;
  using IntVect_t = IntVect_T<_SIZE_>;
  using IntTab_t = IntTab_T<_SIZE_>;
  using DoubleVect_t = DoubleVect_T<_SIZE_>;
  using DoubleTab_t = DoubleTab_T<_SIZE_>;
  using Domaine_t = Domaine_32_64<_SIZE_>;
  using Frontiere_t = Frontiere_32_64<_SIZE_>;

protected:
  inline int_t numero_maille(int i);
  inline int_t numero_maille(int i, int j);
  inline int_t numero_maille(int i, int j, int k);

  inline int_t numero_sommet(int i);
  inline int_t numero_sommet(int i, int j);
  inline int_t numero_sommet(int i, int j, int k);

  inline int_t& maille_sommet(int i,int l);
  inline int_t& maille_sommet(int i, int j, int l);
  inline int_t& maille_sommet(int i, int j, int k, int l);

  inline double& coord_noeud(int i);
  inline double& coord_noeud(int i, int j, int l);
  inline double& coord_noeud(int i, int j, int k, int l);

  void maille2D();
  void maille3D();

  void typer_();

  void lire_longueurs(Entree& is);
  void lire_noeuds(Entree& is);
  void lire_front(Entree& , Frontiere_t& );


  DoubleVect origine_, longueurs_, facteurs_, pas_;
  IntVect symetrique_;  // which directions (x,y,z) are symetrical?
  IntVect nb_noeuds_;

  int Nx = -1, Ny = -1, Nz = -1, Mx = -1, My = -1, Mz = -1;
  double a_tanh= 10.;  // a pour le maillage en tanh dans la diry!!
  int tanh_dilatation=0;  // can be -1,0 or 1
  double xa_tanh= 10.;  // xa pour le maillage en tanh dans la dirx!!
  int xtanh_dilatation=0; // can be -1,0 or 1
  double za_tanh= 10.;  // za pour le maillage en tanh dans la dirz!!
  int ztanh_dilatation=0; // can be -1,0 or 1
  bool rep_VEF=false;
  bool tour_complet = false;
};

/*! @brief Renvoie le numero de la i-ieme maille (suivant X)
 *
 * @param (int i) le rang de la maille suivant X dont on veut le numero
 * @return (int) le numero de la maille cherchee
 */
template <typename _SIZE_>
inline typename Pave_32_64<_SIZE_>::int_t Pave_32_64<_SIZE_>::numero_maille(int i)
{
  assert(this->dimension == 1);
  assert(i < Nx);
  return i;
}

/*! @brief Renvoie le numero de la (i,j)-ieme maille (suivant (X,Y)).
 *
 * @param (int i) le rang de la maille suivant X
 * @param (int j) le rang de la maille suivant Y
 * @return (int) le numero de la maille cherchee
 */
template <typename _SIZE_>
inline typename Pave_32_64<_SIZE_>::int_t Pave_32_64<_SIZE_>::numero_maille(int i, int j)
{
  assert(this->dimension == 2);
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
template <typename _SIZE_>
inline typename Pave_32_64<_SIZE_>::int_t Pave_32_64<_SIZE_>::numero_maille(int i, int j, int k)
{
  assert(this->dimension == 3);
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
template <typename _SIZE_>
inline typename Pave_32_64<_SIZE_>::int_t Pave_32_64<_SIZE_>::numero_sommet(int i)
{
  assert(this->dimension == 1 && i < Mx);
  return i;
}

/*! @brief Renvoie le numero du (i,j)-ieme sommet (suivant (X,Y)).
 *
 * @param (int i) le rang du sommet suivant X
 * @param (int j) le rang du sommet suivant Y
 * @return (int) le numero du sommet cherche
 */
template <typename _SIZE_>
inline typename Pave_32_64<_SIZE_>::int_t Pave_32_64<_SIZE_>::numero_sommet(int i, int j)
{
  assert(this->dimension == 2);
  assert(i < Mx);
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
template <typename _SIZE_>
inline typename Pave_32_64<_SIZE_>::int_t Pave_32_64<_SIZE_>::numero_sommet(int i, int j, int k  )
{
  assert(this->dimension == 3);
  assert(i < Mx);
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
template <typename _SIZE_>
inline typename Pave_32_64<_SIZE_>::int_t& Pave_32_64<_SIZE_>::maille_sommet(int i, int l)
{
  assert(this->dimension == 1);
  return this->mes_elems_(numero_maille(i),l);
}

/*! @brief Renvoie une reference sur le numero du l-ieme sommet de la (i,j)-ieme maille (suivant (X,Y)) du pave.
 *
 * @param (int i) le rang de la maille suivant X
 * @param (int j) le rang de la maille suivant Y
 * @param (int l) le rang du sommet cherche
 * @return (int&) reference sur le numero du sommet cherche
 */
template <typename _SIZE_>
inline typename Pave_32_64<_SIZE_>::int_t& Pave_32_64<_SIZE_>::maille_sommet(int i, int j, int l)
{
  assert(this->dimension == 2);
  return this->mes_elems_(numero_maille(i, j),l);
}

/*! @brief Renvoie une reference sur le numero du l-ieme sommet de la (i,j,k)-ieme maille (suivant (X,Y,Z)) du pave.
 *
 * @param (int i) le rang de la maille suivant X
 * @param (int j) le rang de la maille suivant Y
 * @param (int j) le rang de la maille suivant Z
 * @param (int l) le rang du sommet cherche
 * @return (int&) reference sur le numero du sommet cherche
 */
template <typename _SIZE_>
inline typename Pave_32_64<_SIZE_>::int_t& Pave_32_64<_SIZE_>::maille_sommet(int i, int j, int k, int l)
{
  assert(this->dimension == 3);
  return this->mes_elems_(numero_maille(i, j, k), l);
}

/*! @brief Renvoie une reference sur les coordonnees du i-ieme noeud.
 *
 * @param (int i) le rang du noeud suivant X
 * @return (double&) reference sur les coordonnees du noeud cherche
 */
template <typename _SIZE_>
inline double& Pave_32_64<_SIZE_>::coord_noeud(int i)
{
  assert(this->dimension == 1);
  return this->sommets_(numero_sommet(i));
}

/*! @brief Renvoie une reference sur les coordonnees du (i,j)-ieme noeud.
 *
 * @param (int i) le rang du noeud suivant X
 * @param (int j) le rang du noeud suivant Y
 * @return (double&) reference sur les coordonnees du noeud cherche
 */
template <typename _SIZE_>
inline double& Pave_32_64<_SIZE_>::coord_noeud(int i, int j, int l)
{
  assert(this->dimension == 2);
  return this->sommets_(numero_sommet(i, j),l);
}

/*! @brief Renvoie une reference sur les coordonnees du (i,j,k)-ieme noeud.
 *
 * @param (int i) le rang du noeud suivant X
 * @param (int j) le rang du noeud suivant Y
 * @param (int k) le rang du noeud suivant Z
 * @return (double&) reference sur les coordonnees du noeud cherche
 */
template <typename _SIZE_>
inline double& Pave_32_64<_SIZE_>::coord_noeud(int i, int j, int k, int l)
{
  assert(this->dimension == 3);
  return this->sommets_(numero_sommet(i, j, k),l);
}

using Pave = Pave_32_64<int>;
using Pave_64 = Pave_32_64<trustIdType>;

#endif
