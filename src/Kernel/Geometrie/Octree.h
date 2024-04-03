/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef Octree_included
#define Octree_included

#include <TRUSTTabs_forward.h>
#include <TRUST_Ref.h>
#include <TRUSTTab.h>

class Domaine;

// TODO should be a class enum
#define GaucheArriereBas 0
#define DroitArriereBas 1
#define GaucheAvantBas 2
#define DroitAvantBas 3
#define GaucheArriereHaut 4
#define DroitArriereHaut 5
#define GaucheAvantHaut 6
#define DroitAvantHaut 7

/*! @brief Struct OctreeLoc
 *
 * @sa Octree
 */
struct OctreeLoc
{
  double xmin, xmax, ymin, ymax, zmin, zmax;
  void construire(const OctreeLoc& loc, int  i);
  void construire(const OctreeLoc& loc, int i, double xmil, double ymil, double zmil);
  int direction(double x, double y, double z) const;
};


/*! @brief Classe Octree
 *
 * @sa OctreeLoc OctreeRoot OctreeFloor
 */
template <typename _SIZE_>
class Octree_32_64
{
public :
  using int_t = _SIZE_;
  using ArrOfInt_t = AOInt_T<_SIZE_>;
  using SmallArrOfTID_t = SmallAOTID_T<_SIZE_>;
  using IntTab_t = ITab_T<_SIZE_>;
  using DoubleTab_t = DTab_T<_SIZE_>;
  using Vect_IntTab_t = TRUST_Vector<IntTab_t>;

  using Domaine_t = Domaine_32_64<_SIZE_>;
  using Octree_t = Octree_32_64<_SIZE_>;

  Octree_32_64() : les_octrees(nullptr), pere(nullptr) {}
  virtual ~Octree_32_64() { detruire(); }
  Octree_32_64(int n, Octree_32_64* mon_pere, const ArrOfInt_t& val, const OctreeLoc& loc) : les_octrees(nullptr), pere(nullptr)  { construire(n, val, loc, mon_pere); }
  virtual int_t rang_elem_loc(const OctreeLoc&, double x, double y=0, double z=0) const;
  virtual int_t rang_elem_depuis_loc(const OctreeLoc&, int_t prems, double x, double y=0, double z=0) const;

  virtual const Domaine_t& domaine() const  {  return pere->domaine(); }
  void construire(int, const ArrOfInt_t&, const OctreeLoc&, Octree_t* p=0);
  int niveau() const;
  virtual int_t taille() const;  // taille memoire
  static int nombre_d_octrees()  { return 1 << Objet_U::dimension; /* = 2**O_U::dimension */ }

  virtual Sortie& printOn(Sortie& is) const;
  virtual Entree& readOn(Entree& is) {  return is; }

protected :
  void detruire();
  void ranger_elem_1D(ArrOfInt& oks, int_t elem, int_t idx, int nb_som_elem, const DoubleTab_t& coords_som, const IntTab_t& les_elems,
                      SmallArrOfTID_t& compteur, Vect_IntTab_t& sous_tab, double xmil);
  void ranger_elem_2D(ArrOfInt& oks, int_t elem, int_t idx, int nb_som_elem, const DoubleTab_t& coords_som, const IntTab_t& les_elems,
                      SmallArrOfTID_t& compteur, Vect_IntTab_t& sous_tab, double xmil, double ymil);
  void ranger_elem_3D(ArrOfInt& oks, int_t elem, int_t idx, int nb_som_elem, const DoubleTab_t& coords_som, const IntTab_t& les_elems,
                      SmallArrOfTID_t& compteur, Vect_IntTab_t& sous_tab, double xmil, double ymil, double zmil);
  double get_epsilon() const { return domaine().epsilon(); }

  Octree_32_64** les_octrees;
  Octree_32_64* pere;
};


/*! @brief Classe OctreeRoot
 *
 * @sa OctreeLoc Octree OctreeFloor
 */
template <typename _SIZE_>
class OctreeRoot_32_64 :  public Objet_U, public Octree_32_64<_SIZE_>
{
  Declare_instanciable_sans_constructeur_32_64(OctreeRoot_32_64);

public :
  using int_t = _SIZE_;
  using ArrOfInt_t = AOInt_T<_SIZE_>;
  using IntTab_t = ITab_T<_SIZE_>;
  using SmallArrOfTID_t = SmallAOTID_T<_SIZE_>;
  using DoubleTab_t = DTab_T<_SIZE_>;
  using Domaine_t = Domaine_32_64<_SIZE_>;
  using Octree_t = Octree_32_64<_SIZE_>;

  OctreeRoot_32_64():valid_(0) {}
  OctreeRoot_32_64(const Domaine_t& z)
  {
    associer_Domaine(z);
    construire();
  }

  OctreeRoot_32_64(const OctreeRoot_32_64& oc): Objet_U(oc),Octree_32_64<_SIZE_>(oc)
  {
    if(oc.construit())
      {
        associer_Domaine(oc.domaine());
        construire();
      }
    else
      valid_=0;
  }

  int_t rang_sommet(double x, double y=0, double z=0) const;
  int_t rang_arete(double x, double y=0, double z=0) const;
  int_t rang_elem(double x, double y=0, double z=0) const;
  int_t rang_elem_depuis(int_t prems, double x, double y=0, double z=0) const;
  int_t rang_elem_depuis(const DoubleTab& positions, const SmallArrOfTID_t& prems, SmallArrOfTID_t& elems) const;
  int_t rang_sommet(const DoubleTab& positions, SmallArrOfTID_t& sommets) const;
  int_t rang_arete(const DoubleTab& positions, SmallArrOfTID_t& aretes) const;
  int_t rang_elem(const DoubleTab& positions, SmallArrOfTID_t& elem) const;
  void rang_elems_sommet(SmallArrOfTID_t&, double x, double y=0, double z=0) const;

  const Domaine_t& domaine() const override { return le_dom.valeur(); }
  void associer_Domaine(const Domaine_t& d) { le_dom=d; }
  void construire(int reel=0);
  int construit() const;
  void invalide();
  inline int reel() const { return reel_; }

protected :
  using Octree_t::les_octrees;
  using Octree_t::pere;

  REF(Domaine_t) le_dom;
  OctreeLoc loc;
  int valid_, reel_ = -1;
};

/*! @brief Classe OctreeFloor
 *
 * @sa OctreeLoc Octree  OctreeRoot
 */
template <typename _SIZE_>
class OctreeFloor_32_64 : public Octree_32_64<_SIZE_>
{
public :
  using int_t = _SIZE_;
  using Octree_t = Octree_32_64<_SIZE_>;
  using ArrOfInt_t = AOInt_T<_SIZE_>;

  OctreeFloor_32_64(Octree_t *mon_pere, const ArrOfInt_t& val, const OctreeLoc& loc)
  {
    pos.resize_array(Objet_U::dimension);
    construire(mon_pere, val, loc);
  }

  void construire(Octree_t* , const ArrOfInt_t&, const OctreeLoc&);
  int_t rang_elem_loc(const OctreeLoc&, double x, double y=0, double z=0) const override;
  int_t rang_elem_depuis_loc(const OctreeLoc&, int_t prems, double x, double y=0, double z=0) const override;

  Sortie& printOn(Sortie& is) const override { return Octree_t::printOn(is); }
  Entree& readOn(Entree& is) override { return Octree_t::readOn(is); }
  int_t taille() const override;

protected :
  using Octree_t::les_octrees;
  using Octree_t::pere;

  ArrOfInt_t num_elem;
  mutable ArrOfDouble pos; // Tableau de travail - size 3
};


using Octree = Octree_32_64<int>;
using OctreeRoot = OctreeRoot_32_64<int>;

#endif     // Octree_included

