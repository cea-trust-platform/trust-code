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

#ifndef Octree_included
#define Octree_included

#include <TRUSTTabs_forward.h>
#include <TRUST_Ref.h>
#include <TRUSTTab.h>

class Domaine;

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
class OctreeRoot;


/*! @brief Classe Octree
 *
 * @sa OctreeLoc OctreeRoot OctreeFloor
 */
class Octree
{
public :
  Octree():les_octrees(0), pere(0) {}
  virtual ~Octree();
  Octree(const Octree&):les_octrees(0), pere(0) {}
  Octree(int n, Octree* mon_pere, const ArrOfInt& val,
         const OctreeLoc& loc)
  {
    construire(n, val, loc, mon_pere);
  }
  virtual int rang_elem_loc(const OctreeLoc&, double x, double y=0, double z=0) const;
  virtual int rang_elem_depuis_loc(const OctreeLoc&, int prems, double x, double y=0, double z=0) const;

  virtual const Domaine& domaine() const;
  void construire(int, const ArrOfInt&, const OctreeLoc&, Octree* p=0);
  int niveau() const;
  virtual int taille() const;
  static int nombre_d_octrees();
  virtual Sortie& printOn(Sortie& is) const;
  virtual Entree& readOn(Entree& is);
protected :
  void detruire();
  void ranger_elem_1D(ArrOfInt&, int, int, int, const DoubleTab&, const IntTab&, ArrOfInt&, IntVects&, double);
  void ranger_elem_2D(ArrOfInt&, int, int, int, const DoubleTab&, const IntTab&, ArrOfInt&, IntVects&, double, double);
  void ranger_elem_3D(ArrOfInt&, int, int, int, const DoubleTab&, const IntTab&, ArrOfInt&, IntVects&, double, double, double);
  double get_epsilon() const;
  Octree** les_octrees;
  Octree* pere;

private :
};


/*! @brief Classe OctreeRoot
 *
 * @sa OctreeLoc Octree OctreeFloor
 */
class OctreeRoot :  public Objet_U, public Octree
{
  Declare_instanciable_sans_constructeur(OctreeRoot);

public :
  OctreeRoot():valid_(0) {}
  OctreeRoot(const Domaine& z)
  {
    associer_Domaine(z);
    construire();
  }
  OctreeRoot(const OctreeRoot& oc): Objet_U(oc),Octree(oc)
  {
    if(oc.construit())
      {
        associer_Domaine(oc.domaine());
        construire();
      }
    else
      valid_=0;
  }
  int rang_sommet(double x, double y=0, double z=0) const;
  int rang_arete(double x, double y=0, double z=0) const;
  int rang_elem(double x, double y=0, double z=0) const;
  int rang_elem_depuis(int prems, double x, double y=0, double z=0) const;
  int rang_sommet(const DoubleTab&, ArrOfInt&) const;
  int rang_arete(const DoubleTab&, ArrOfInt&) const;
  int rang_elem(const DoubleTab&, ArrOfInt&) const;
  int rang_elem_depuis(const DoubleTab&, const ArrOfInt&, ArrOfInt&) const;
  void rang_elems_sommet(ArrOfInt&, double x, double y=0, double z=0) const;

  const Domaine& domaine() const override;
  void associer_Domaine(const Domaine&);
  void construire(int reel=0);
  int construit() const;
  void invalide();
  inline int reel() const { return reel_; }

protected :
  REF(Domaine) le_dom;
  OctreeLoc loc;
  int valid_, reel_ = -1;
};

/*! @brief Classe OctreeFloor
 *
 * @sa OctreeLoc Octree OctreeRoot
 */
class OctreeFloor : public Octree
{

public :

  OctreeFloor(Octree *mon_pere, const ArrOfInt& val, const OctreeLoc& loc)
  {
    pos.resize_array(Objet_U::dimension);
    construire(mon_pere, val, loc);
  }
  void construire(Octree* , const ArrOfInt&, const OctreeLoc&);
  int rang_elem_loc(const OctreeLoc&, double x, double y=0, double z=0) const override;
  int rang_elem_depuis_loc(const OctreeLoc&, int prems, double x, double y=0, double z=0) const override;

  Sortie& printOn(Sortie& is) const override;
  Entree& readOn(Entree& is) override;
  int taille() const override;
protected :
  ArrOfInt num_elem;
  mutable ArrOfDouble pos; // Tableau de travail
};

#endif//OCTREE_INCLUS

