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

#ifndef NbCasesParNoeuds
#define NbCasesParNoeuds 512
#endif

#include <TRUSTVects.h>
#include <Domaine.h>
#include <Octree.h>

Implemente_instanciable_sans_constructeur_32_64(OctreeRoot_32_64,"OctreeRoot",Objet_U);

template <typename _SIZE_>
Sortie& Octree_32_64<_SIZE_>::printOn(Sortie& is) const
{
  int nb_octrees=Octree_32_64::nombre_d_octrees();
  int i;
  is << finl << "--------------------------------" << finl;
  is << "niveau : " << niveau() << " taille : " << taille() << finl;
  for(i=0; i<nb_octrees; i++)
    {
      if(les_octrees[i]!=0)
        les_octrees[i]->printOn(is);
      else
        is << "vide" << " ";
    }
  return is << finl;
}

template <typename _SIZE_>
Sortie& OctreeRoot_32_64<_SIZE_>::printOn(Sortie& os) const
{
  return Octree_32_64<_SIZE_>::printOn(os);
}

template <typename _SIZE_>
Entree& OctreeRoot_32_64<_SIZE_>::readOn(Entree& is)
{
  return Octree_32_64<_SIZE_>::readOn(is);
}


/*! @brief
 *
 * @param (OctreeLoc& loc)
 * @param (int  i)
 */
void OctreeLoc::construire(const OctreeLoc& loc, int  i)
{
  double xmil=(loc.xmin+loc.xmax)/2;
  double ymil=(loc.ymin+loc.ymax)/2;
  double zmil=(loc.zmin+loc.zmax)/2;
  construire(loc, i, xmil, ymil, zmil);
}


/*! @brief
 *
 * @param (OctreeLoc& loc)
 * @param (int  i)
 * @param (double xmil)
 * @param (double ymil)
 * @param (double zmil)
 */
void OctreeLoc::construire(const OctreeLoc& loc, int  i, double xmil, double ymil, double zmil)
{
  switch(i)
    {
    case GaucheArriereBas:
      xmin=loc.xmin;
      xmax=xmil;
      ymin=loc.ymin;
      ymax=ymil;
      zmin=loc.zmin;
      zmax=zmil;
      break;
    case GaucheArriereHaut:
      xmin=loc.xmin;
      xmax=xmil;
      ymin=loc.ymin;
      ymax=ymil;
      zmin=zmil;
      zmax=loc.zmax;
      break;
    case GaucheAvantBas:
      xmin=loc.xmin;
      xmax=xmil;
      ymin=ymil;
      ymax=loc.ymax;
      zmin=loc.zmin;
      zmax=zmil;
      break;
    case GaucheAvantHaut:
      xmin=loc.xmin;
      xmax=xmil;
      ymin=ymil;
      ymax=loc.ymax;
      zmin=zmil;
      zmax=loc.zmax;
      break;
    case DroitArriereBas:
      xmin=xmil;
      xmax=loc.xmax;
      ymin=loc.ymin;
      ymax=ymil;
      zmin=loc.zmin;
      zmax=zmil;
      break;
    case DroitArriereHaut:
      xmin=xmil;
      xmax=loc.xmax;
      ymin=loc.ymin;
      ymax=ymil;
      zmin=zmil;
      zmax=loc.zmax;
      break;
    case DroitAvantBas:
      xmin=xmil;
      xmax=loc.xmax;
      ymin=ymil;
      ymax=loc.ymax;
      zmin=loc.zmin;
      zmax=zmil;
      break;
    case DroitAvantHaut:
      xmin=xmil;
      xmax=loc.xmax;
      ymin=ymil;
      ymax=loc.ymax;
      zmin=zmil;
      zmax=loc.zmax;
      break;
    }
}


/*! @brief
 *
 * @param (double x)
 * @param (double y)
 * @param (double z)
 * @return (int)
 */
int OctreeLoc::direction(double x, double y, double z) const
{
  double xmil=(xmin+xmax)/2;
  double ymil=(ymin+ymax)/2;
  double zmil=(zmin+zmax)/2;

  if( (x<xmil) && (y<ymil) && (z<zmil))
    return GaucheArriereBas;
  if( (x<xmil) && (y<ymil) && !(z<zmil))
    return GaucheArriereHaut;
  if( (x<xmil) && !(y<ymil) && (z<zmil))
    return GaucheAvantBas;
  if( (x<xmil) && !(y<ymil) && !(z<zmil))
    return GaucheAvantHaut;
  if( !(x<xmil) && (y<ymil) && (z<zmil))
    return DroitArriereBas;
  if( !(x<xmil) && (y<ymil) && !(z<zmil))
    return DroitArriereHaut;
  if( !(x<xmil) && !(y<ymil) && (z<zmil))
    return DroitAvantBas;
  if( !(x<xmil) && !(y<ymil) && !(z<zmil))
    return DroitAvantHaut;

  Cerr << "Error in OctreeLoc::direction" << finl;
  Process::exit();
  return -1;
}


/*! @brief
 *
 * @param (OctreeLoc& loc)
 * @param (double x)
 * @param (double y)
 * @param (double z)
 * @return (int)
 */
template <typename _SIZE_>
typename Octree_32_64<_SIZE_>::int_t Octree_32_64<_SIZE_>::rang_elem_loc(const OctreeLoc& loc, double x, double y, double z) const
{
  int_t element=-1;
  int j=1;
  while(j<nombre_d_octrees())
    {
      if(les_octrees[j]!=0)
        break;
      else
        j++;
    }
  if(j<nombre_d_octrees())
    {
      int i=loc.direction(x, y, z);
      OctreeLoc loci;
      loci.construire(loc, i);
      if(les_octrees[i]==0)
        return -1;
      element = les_octrees[i]->rang_elem_loc(loci, x, y, z);
    }
  else
    element = les_octrees[0]->rang_elem_loc(loc, x, y, z);
  return element;
}


/*! @brief
 *
 * @param (OctreeLoc& loc)
 * @param (int prems)
 * @param (double x)
 * @param (double y)
 * @param (double z)
 * @return (int)
 */
template <typename _SIZE_>
typename Octree_32_64<_SIZE_>::int_t Octree_32_64<_SIZE_>::rang_elem_depuis_loc(const OctreeLoc& loc, int_t prems, double x, double y, double z) const
{
  int_t element=-1;
  int j=1;
  while(j<nombre_d_octrees())
    {
      if(les_octrees[j]!=0)
        break;
      else
        j++;
    }
  if(j<nombre_d_octrees())
    {
      int i=loc.direction(x, y, z);
      OctreeLoc loci;
      loci.construire(loc, i);
      if(les_octrees[i]==0)
        return -1;
      element = les_octrees[i]->rang_elem_depuis_loc(loci, prems, x, y, z);
    }
  else
    element = les_octrees[0]->rang_elem_depuis_loc(loc, prems, x, y, z);
  return element;
}


template <typename _SIZE_>
void Octree_32_64<_SIZE_>::ranger_elem_1D(ArrOfInt& ok, int_t elem, int_t i, int nb_som_elem, const DoubleTab_t& coord, const IntTab_t& les_elems,
                                          SmallArrOfTID_t& compteur, Vect_IntTab_t& SousTab, double xmil)
{
  for(int som=0; som<nb_som_elem; som++)
    {
      int_t sommet=les_elems(i,som);
      assert(sommet>=0);
      if(coord(sommet,0)<xmil)
        {
          if(!ok[GaucheArriereBas])
            {
              SousTab[GaucheArriereBas][compteur[GaucheArriereBas]]=i;
              compteur[GaucheArriereBas]++;
              ok[GaucheArriereBas]=1;
            }
        }
      else
        {
          if(!ok[DroitArriereBas])
            {
              SousTab[DroitArriereBas][compteur[DroitArriereBas]]=i;
              compteur[DroitArriereBas]++;
              ok[DroitArriereBas]=1;
            }
        }
    }
}

namespace // Anonymous namespace
{

template <typename _SIZE_>
inline void range2D(double x, double y, double xmil, double ymil,
                    ArrOfInt& ok, TRUST_Vector<ITab_T<_SIZE_>>& SousTab, SmallAOTID_T<_SIZE_>& compteur, _SIZE_ i)
{
  if(x<xmil)
    {
      if(y<ymil)
        {
          if(!ok[GaucheArriereBas])
            {
              SousTab[GaucheArriereBas][compteur[GaucheArriereBas]]=i;
              compteur[GaucheArriereBas]++;
              ok[GaucheArriereBas]=1;
            }
        }
      else
        {
          if(!ok[GaucheAvantBas])
            {
              SousTab[GaucheAvantBas][compteur[GaucheAvantBas]]=i;
              compteur[GaucheAvantBas]++;
              ok[GaucheAvantBas]=1;
            }
        }
    }
  else
    {
      if(y<ymil)
        {
          if(!ok[DroitArriereBas])
            {
              SousTab[DroitArriereBas][compteur[DroitArriereBas]]=i;
              compteur[DroitArriereBas]++;
              ok[DroitArriereBas]=1;
            }
        }
      else
        {
          if(!ok[DroitAvantBas])
            {
              SousTab[DroitAvantBas][compteur[DroitAvantBas]]=i;
              compteur[DroitAvantBas]++;
              ok[DroitAvantBas]=1;
            }
        }
    }
}

template <typename _SIZE_>
inline void range3D(double x, double y, double z,
                    double xmil, double ymil, double zmil,
                    ArrOfInt& ok, TRUST_Vector<ITab_T<_SIZE_>>& SousTab, SmallAOTID_T<_SIZE_>& compteur, _SIZE_ i)
{
  if(x<xmil)
    {
      if(y<ymil)
        {
          if(z<zmil)
            {
              if(!ok[GaucheArriereBas])
                {
                  SousTab[GaucheArriereBas][compteur[GaucheArriereBas]]=i;
                  compteur[GaucheArriereBas]++;
                  ok[GaucheArriereBas]=1;
                }
            }
          else
            {
              if(!ok[GaucheArriereHaut])
                {
                  SousTab[GaucheArriereHaut][compteur[GaucheArriereHaut]]=i;
                  compteur[GaucheArriereHaut]++;
                  ok[GaucheArriereHaut]=1;
                }
            }
        }
      else
        {
          if(z<zmil)
            {
              if(!ok[GaucheAvantBas])
                {
                  SousTab[GaucheAvantBas][compteur[GaucheAvantBas]]=i;
                  compteur[GaucheAvantBas]++;
                  ok[GaucheAvantBas]=1;
                }
            }
          else
            {
              if(!ok[GaucheAvantHaut])
                {
                  SousTab[GaucheAvantHaut][compteur[GaucheAvantHaut]]=i;
                  compteur[GaucheAvantHaut]++;
                  ok[GaucheAvantHaut]=1;
                }
            }
        }
    }
  else
    {
      if(y<ymil)
        {
          if(z<zmil)
            {
              if(!ok[DroitArriereBas])
                {
                  SousTab[DroitArriereBas][compteur[DroitArriereBas]]=i;
                  compteur[DroitArriereBas]++;
                  ok[DroitArriereBas]=1;
                }
            }
          else
            {
              if(!ok[DroitArriereHaut])
                {
                  SousTab[DroitArriereHaut][compteur[DroitArriereHaut]]=i;
                  compteur[DroitArriereHaut]++;
                  ok[DroitArriereHaut]=1;
                }
            }
        }
      else
        {
          if(z<zmil)
            {
              if(!ok[DroitAvantBas])
                {
                  SousTab[DroitAvantBas][compteur[DroitAvantBas]]=i;
                  compteur[DroitAvantBas]++;
                  ok[DroitAvantBas]=1;
                }
            }
          else
            {
              if(!ok[DroitAvantHaut])
                {
                  SousTab[DroitAvantHaut][compteur[DroitAvantHaut]]=i;
                  compteur[DroitAvantHaut]++;
                  ok[DroitAvantHaut]=1;
                }
            }
        }
    }
}

}// end anonymous namespace


template <typename _SIZE_>
void Octree_32_64<_SIZE_>::ranger_elem_2D(ArrOfInt& ok, int_t elem, int_t idx, int nb_som_elem, const DoubleTab_t& coord, const IntTab_t& elems,
                                          SmallArrOfTID_t& compteur, Vect_IntTab_t& SousTab, double xmil, double ymil)
{
  double xmin=DMAXFLOAT, xmax=-DMAXFLOAT;
  double ymin=xmin, ymax=xmax;
  for(int som=0; som<nb_som_elem; som++)
    {
      int_t sommet=elems(idx,som);
//      assert(sommet>=0);
// GF dans le cas dde polyedre le tableau elem est surdimensionne et peut contenir des -1
      if (sommet>=0)
        {
          xmin=std::min(xmin, coord(sommet,0));
          xmax=std::max(xmax, coord(sommet,0));
          ymin=std::min(ymin, coord(sommet,1));
          ymax=std::max(ymax, coord(sommet,1));
        }
    }
  range2D(xmin, ymin, xmil, ymil, ok, SousTab, compteur, idx);
  range2D(xmin, ymax, xmil, ymil, ok, SousTab, compteur, idx);
  range2D(xmax, ymin, xmil, ymil, ok, SousTab, compteur, idx);
  range2D(xmax, ymax, xmil, ymil, ok, SousTab, compteur, idx);
}

template <typename _SIZE_>
void Octree_32_64<_SIZE_>::ranger_elem_3D(ArrOfInt& ok, int_t elem, int_t idx, int nb_som_elem, const DoubleTab_t& coord, const IntTab_t& elems,
                                          SmallArrOfTID_t& compteur, Vect_IntTab_t& SousTab, double xmil, double ymil, double zmil)
{
  double epsilon=get_epsilon();
  double xmin=DMAXFLOAT, xmax=-DMAXFLOAT;
  double ymin=xmin, ymax=xmax;
  double zmin=xmin, zmax=xmax;
  for(int som=0; som<nb_som_elem; som++)
    {
      int_t sommet=elems(idx,som);
//      assert(sommet>=0);
// GF dans le cas dde polyedre le tableau elem est surdimensionne et peut contenir des -1
      if (sommet>=0)
        {
          xmin=std::min(xmin, coord(sommet,0))-epsilon;
          xmax=std::max(xmax, coord(sommet,0))+epsilon;
          ymin=std::min(ymin, coord(sommet,1))-epsilon;
          ymax=std::max(ymax, coord(sommet,1))+epsilon;
          zmin=std::min(zmin, coord(sommet,2))-epsilon;
          zmax=std::max(zmax, coord(sommet,2))+epsilon;
        }
    }
  range3D(xmin, ymin, zmin, xmil, ymil, zmil, ok, SousTab, compteur, idx);
  range3D(xmin, ymax, zmin, xmil, ymil, zmil, ok, SousTab, compteur, idx);
  range3D(xmax, ymin, zmin, xmil, ymil, zmil, ok, SousTab, compteur, idx);
  range3D(xmax, ymax, zmin, xmil, ymil, zmil, ok, SousTab, compteur, idx);
  range3D(xmin, ymin, zmax, xmil, ymil, zmil, ok, SousTab, compteur, idx);
  range3D(xmin, ymax, zmax, xmil, ymil, zmil, ok, SousTab, compteur, idx);
  range3D(xmax, ymin, zmax, xmil, ymil, zmil, ok, SousTab, compteur, idx);
  range3D(xmax, ymax, zmax, xmil, ymil, zmil, ok, SousTab, compteur, idx);
}

/*! @brief
 *
 * @param (int nb_octrees)
 * @param (ArrOfInt& Tab)
 * @param (OctreeLoc& loc)
 * @param (Octree* pe)
 */
template <typename _SIZE_>
void Octree_32_64<_SIZE_>::construire(int nb_octrees, const ArrOfInt_t& Tab, const OctreeLoc& loc, Octree_t* pe)
{
  static int level=0;
  int_t nb_elem=Tab.size_array();
  pere=pe;
  les_octrees=new Octree_32_64*[nb_octrees];

  if(niveau()>level)
    {
      level=niveau();
      Process::Journal() << "Octree: level=" << level
                         << " nb_elem=" << nb_elem << finl;
      if(level > 100) Process::exit();
    }

  if(nb_elem<=NbCasesParNoeuds)
    {
      for(int i=0; i<nb_octrees; i++)
        les_octrees[i]=0;
      les_octrees[0]=new OctreeFloor_32_64<_SIZE_>(this, Tab, loc);
    }
  else
    {
      //else
      const Domaine_t& dom=domaine();
      Vect_IntTab_t SousTab(nb_octrees);
      SmallArrOfTID_t compteur(nb_octrees);
      int nb_som_elem=dom.nb_som_elem();
      const IntTab_t& les_elems=dom.les_elems();
      const DoubleTab_t& coord_sommets=dom.coord_sommets();
      for(int i=0; i<nb_octrees; i++)
        SousTab[i].resize(nb_elem);
      double xmil=(loc.xmin+loc.xmax)/2;
      double ymil=(loc.ymin+loc.ymax)/2;
      double zmil=(loc.zmin+loc.zmax)/2;
      ArrOfInt ok(nb_octrees);
      for(int_t elem=0; elem<nb_elem; elem++)
        {
          int_t idx=Tab[elem];
          ok=0;
          switch(Objet_U::dimension)
            {
            case 1:
              // ArrOfInt oks, int_t elem, int_t idx, int nb_som_elem, DoubleTab_t coords_som, IntTab_t les_elems,
              // SmallArrOfTID_t compteur, ArrOfInt_t sous_tab, double xmil
              ranger_elem_1D(ok, elem, idx , nb_som_elem, coord_sommets,
                             les_elems, compteur, SousTab, xmil);
              break;
            case 2:
              ranger_elem_2D(ok, elem, idx , nb_som_elem, coord_sommets,
                             les_elems, compteur, SousTab, xmil, ymil);
              break;
            case 3:
              ranger_elem_3D(ok, elem, idx ,nb_som_elem, coord_sommets,
                             les_elems, compteur, SousTab, xmil, ymil, zmil);
              break;
            default:
              Cerr << "Space dimension " << Objet_U::dimension << " incorrect" << finl;
              Process::exit();
            }
        }

      for(int i=0; i<nb_octrees; i++)
        {
          les_octrees[i]=0;
          OctreeLoc loci;
          loci.construire(loc, i, xmil, ymil, zmil);
          SousTab[i].resize(compteur[i]);
          if(compteur[i]==nb_elem)
            les_octrees[i]=new OctreeFloor_32_64<_SIZE_>(this, SousTab[i], loci);
          else if(compteur[i]>NbCasesParNoeuds)
            les_octrees[i]=new Octree_32_64<_SIZE_>(nb_octrees, this, SousTab[i], loci);
          else if(compteur[i]!=0)
            les_octrees[i]=new OctreeFloor_32_64<_SIZE_>(this, SousTab[i], loci);
        }
    }
}


/*! @brief Detruit l'octree.
 *
 * Methode appelee par le destructeur
 *
 */
template <typename _SIZE_>
void Octree_32_64<_SIZE_>::detruire()
{
  if(les_octrees==0)
    return;
  int nb_octrees=Octree_32_64<_SIZE_>::nombre_d_octrees();
  for(int i=0; i<nb_octrees; i++)
    if(les_octrees[i])
      delete les_octrees[i];
  delete[] les_octrees;
  les_octrees=0;
}


template <typename _SIZE_>
int Octree_32_64<_SIZE_>::niveau() const
{
  int i=0;
  if (pere==0) return i;
  return (pere->niveau()+1);
}


/*! @brief Renvoie la taille de l'octree.
 *
 * @return (unsigned) la taille de l'octree
 */
template <typename _SIZE_>
typename Octree_32_64<_SIZE_>::int_t Octree_32_64<_SIZE_>::taille() const
{
  int nb_octrees=Octree_32_64<_SIZE_>::nombre_d_octrees();
  int_t la_taille = (int_t) sizeof(Octree);
  for(int i=0; i<nb_octrees; i++)
    if(les_octrees[i]!=0)
      la_taille+= les_octrees[i]->taille();
  return la_taille;
}


template <typename _SIZE_>
void OctreeRoot_32_64<_SIZE_>::construire(int reel_prec)
{
  this->detruire();
  valid_=1;
  reel_=reel_prec;
  pere=0;
  const Domaine_t& dom = domaine();

  {
    // Calcul du min et du max des coordonnees dans chaque direction
    double min[3] = { 1e30, 1e30, 1e30 };
    double max[3] = {-1e30,-1e30,-1e30 };
    const DoubleTab_t& tab = dom.coord_sommets();
    //  tot sinon bouding box trop petite
    //  peut etre teste reel_ ...
    const int_t n = tab.dimension_tot(0);
    const int dim = (int)tab.dimension(1);
    assert(dim <= 3);
    for (int_t i = 0; i < n; i++)
      {
        for (int j = 0; j < 3; j++)
          {
            if (j>=dim) break;
            double x = tab(i,j);
            if (x < min[j])
              min[j] = x;
            if (x > max[j])
              max[j] = x;
          }
      }
    loc.xmin = min[0];
    loc.xmax = max[0];
    if (axi)
      {
        loc.ymin = 0.;
        loc.ymax = 2. * M_PI;
      }
    else
      {
        loc.ymin = min[1];
        loc.ymax = max[1];
      }
    if (dim > 2)
      {
        loc.zmin = min[2];
        loc.zmax = max[2];
      }
    else
      {
        loc.zmin = 0.;
        loc.zmax = 1.;
      }
  }
  int_t nb_elem=dom.nb_elem_tot();
  if(reel_prec)
    nb_elem=dom.nb_elem();

  ArrOfInt_t SousTab(nb_elem);
  for(int_t i=0; i<nb_elem; i++)
    SousTab[i]=i;
  Octree_32_64<_SIZE_>::construire(Octree_32_64<_SIZE_>::nombre_d_octrees(), SousTab, loc);
  Cerr << "Construction of the OctreeRoot OK " << finl;
}


/*! @brief
 *
 * @param (double x)
 * @param (double y)
 * @param (double z)
 * @return (int)
 * @throws Erreur dans OctreeRoot_32_64<_SIZE_>::rang_sommet
 * @throws On a pas trouve de sommet a ces coordonnees
 */
template <typename _SIZE_>
typename OctreeRoot_32_64<_SIZE_>::int_t OctreeRoot_32_64<_SIZE_>::rang_sommet(double x, double y, double z) const
{
  int_t elem=rang_elem(x, y, z);
  if (elem != -1)
    {
      const Domaine_t& dom=domaine();
      int nb_som_elem=dom.nb_som_elem();
      double epsilon=this->get_epsilon();
      for(int i=0; i<nb_som_elem; i++)
        {
          //for i
          int_t sommet=dom.sommet_elem(elem,i);
          if(sommet<0)
            {
              Cerr << "This is not a mesh !" << finl;
              Cerr << dom << finl;
              Process::exit();
            }
          switch(Objet_U::dimension)
            {
              //switch
            case 1:
              if(std::fabs(dom.coord(sommet,0)-x)<epsilon)
                return sommet;
              break;
            case 2:
              if((std::fabs(dom.coord(sommet,0)-x)<epsilon) &&
                  (std::fabs(dom.coord(sommet,1)-y)<epsilon))
                return sommet;
              break;
            case 3:
              if((std::fabs(dom.coord(sommet,0)-x)<epsilon) &&
                  (std::fabs(dom.coord(sommet,1)-y)<epsilon) &&
                  (std::fabs(dom.coord(sommet,2)-z)<epsilon))
                return sommet;
              break;
            default:
              Cerr << "Error in OctreeRoot_32_64<_SIZE_>::rang_sommet" << finl;
              Process::exit();
              return -1;
            }
        }
      return -1;
    }
  else
    return -1;
}

/*! @brief
 *
 * @param (double x)
 * @param (double y)
 * @param (double z)
 * @return (int)
 * @throws Erreur dans OctreeRoot_32_64<_SIZE_>::rang_arete
 * @throws On a pas trouve d'arete a ces coordonnees
 */
template <typename _SIZE_>
typename OctreeRoot_32_64<_SIZE_>::int_t OctreeRoot_32_64<_SIZE_>::rang_arete(double x, double y, double z) const
{
  // Recherche l'element contenant le point x,y,z:
  int_t elem=rang_elem(x, y, z);
  const IntTab_t& elem_aretes=domaine().elem_aretes();
  if (elem>=elem_aretes.dimension(0)) return -2;
  if (elem != -1 && elem<elem_aretes.dimension(0))
    {
      // Boucle sur les aretes de l'element
      double epsilon=this->get_epsilon();
      const IntTab_t& aretes_som=domaine().aretes_som();
      const DoubleTab_t& coord=domaine().coord_sommets();
      int nb_aretes_elem = (int)elem_aretes.dimension(1);
      for(int i=0; i<nb_aretes_elem; i++)
        {
          // On boucle sur les aretes de l'element pour trouver celle
          // dont le centre de gravite coincide avec le point x,y,z
          int_t arete=elem_aretes(elem,i);
          int_t s0=aretes_som(arete,0);
          int_t s1=aretes_som(arete,1);
          switch(Objet_U::dimension)
            {
            case 3:
              if((std::fabs(0.5*(coord(s0,0)+coord(s1,0))-x)<epsilon) &&
                  (std::fabs(0.5*(coord(s0,1)+coord(s1,1))-y)<epsilon) &&
                  (std::fabs(0.5*(coord(s0,2)+coord(s1,2))-z)<epsilon))
                return arete;
              break;
            default:
              Cerr << "Error in OctreeRoot_32_64<_SIZE_>::rang_elem" << finl;
              Process::exit();
              return -1;
            }
        }
      return -1;
    }
  else
    return -1;
}

/*! @brief
 *
 * @param (double x)
 * @param (double y)
 * @param (double z)
 * @return (int)
 */
template <typename _SIZE_>
typename OctreeRoot_32_64<_SIZE_>::int_t OctreeRoot_32_64<_SIZE_>::rang_elem(double x, double y, double z) const
{
  double epsilon = this->get_epsilon();
  if(       (x<loc.xmin-epsilon) || (y<loc.ymin-epsilon) || (z<loc.zmin-epsilon)
            ||   (x>loc.xmax+epsilon) || (y>loc.ymax+epsilon) || (z>loc.zmax+epsilon)   )
    return -1;
  return Octree_32_64<_SIZE_>::rang_elem_loc(loc, x, y, z);
}


/*! @brief
 *
 * @param (int prems)
 * @param (double x)
 * @param (double y)
 * @param (double z)
 */
template <typename _SIZE_>
typename OctreeRoot_32_64<_SIZE_>::int_t OctreeRoot_32_64<_SIZE_>::rang_elem_depuis(int_t prems, double x, double y, double z) const
{
  double epsilon = this->get_epsilon();
  if(       (x<loc.xmin-epsilon) || (y<loc.ymin-epsilon) || (z<loc.zmin-epsilon)
            ||   (x>loc.xmax+epsilon) || (y>loc.ymax+epsilon) || (z>loc.zmax+epsilon)   )
    return -1;
  return Octree_32_64<_SIZE_>::rang_elem_depuis_loc(loc, prems, x, y, z);
}


/*! @brief
 *
 * @param (DoubleTab& positions)
 * @param (ArrOfInt& sommets)
 * @return (int)
 * @throws dimension d'espace non prevue
 */
template <typename _SIZE_>
typename OctreeRoot_32_64<_SIZE_>::int_t OctreeRoot_32_64<_SIZE_>::rang_sommet(const DoubleTab& positions, SmallArrOfTID_t& sommets) const
{
  int sz=positions.dimension(0);
  assert(positions.dimension(1)==Objet_U::dimension);
  sommets.resize_array(sz);
  for(int i=0; i<sz; i++)
    {
      double x=0, y=0, z=0;
      switch(Objet_U::dimension)
        {
        case 1:
          x=positions(i,0);
          break;
        case 2:
          x=positions(i,0);
          y=positions(i,1);
          break;
        case 3:
          x=positions(i,0);
          y=positions(i,1);
          z=positions(i,2);
          break;
        default:
          Cerr << "Error in OctreeRoot_32_64<_SIZE_>::rang_sommet" << finl;
          Process::exit();
        }
      sommets[i]=rang_sommet(x,y,z);
    }
  return 1;
}

/*! @brief
 *
 * @param (IntTab& elem_aretes) Definition des aretes par leur sommet
 * @param (DoubleTab& positions)
 * @param (ArrOfInt& aretes) Les aretes trouvees
 * @return (int)
 * @throws dimension d'espace non prevue
 */
template <typename _SIZE_>
typename OctreeRoot_32_64<_SIZE_>::int_t OctreeRoot_32_64<_SIZE_>::rang_arete(const DoubleTab& positions, SmallArrOfTID_t& aretes) const
{
  int sz=positions.dimension(0);
  assert(positions.dimension(1)==Objet_U::dimension);
  aretes.resize_array(sz);
  for(int i=0; i<sz; i++)
    {
      double x=0, y=0, z=0;
      switch(Objet_U::dimension)
        {
        case 1:
          x=positions(i,0);
          break;
        case 2:
          x=positions(i,0);
          y=positions(i,1);
          break;
        case 3:
          x=positions(i,0);
          y=positions(i,1);
          z=positions(i,2);
          break;
        default:
          Cerr << "Error in OctreeRoot_32_64<_SIZE_>::rang_arete" << finl;
          Process::exit();
        }
      aretes[i]=rang_arete(x,y,z);
    }
  return 1;
}

/*! @brief
 *
 * @param (DoubleTab& positions)
 * @param (ArrOfInt& elems)
 * @return (int)
 * @throws dimension d'espace non prevue
 */
template <typename _SIZE_>
typename OctreeRoot_32_64<_SIZE_>::int_t OctreeRoot_32_64<_SIZE_>::rang_elem(const DoubleTab& positions, SmallArrOfTID_t& elems) const
{
  int sz=positions.dimension(0);
  assert(positions.dimension(1)==Objet_U::dimension);
  elems.resize_array(sz);
  for(int i=0; i<sz; i++)
    {
      double x=0, y=0, z=0;
      switch(Objet_U::dimension)
        {
        case 1:
          x=positions(i,0);
          break;
        case 2:
          x=positions(i,0);
          y=positions(i,1);
          break;
        case 3:
          x=positions(i,0);
          y=positions(i,1);
          z=positions(i,2);
          break;
        default:
          Cerr << "Error in OctreeRoot_32_64<_SIZE_>::rang_elem" << finl;
          Process::exit();
        }
      elems[i]=rang_elem(x,y,z);
    }
  return 1;
}


/*! @brief
 *
 * @param (DoubleTab& positions)
 * @param (ArrOfInt& prems)
 * @param (ArrOfInt& elems)
 * @throws dimension d'espace non prevue
 */
template <typename _SIZE_>
typename OctreeRoot_32_64<_SIZE_>::int_t OctreeRoot_32_64<_SIZE_>::rang_elem_depuis(const DoubleTab& positions, const SmallArrOfTID_t& prems, SmallArrOfTID_t& elems) const
{
  int sz=positions.dimension(0);
  assert(positions.dimension(1)==Objet_U::dimension);
  elems.resize_array(sz);
  for(int i=0; i<sz; i++)
    {
      double x=0, y=0, z=0;
      switch(Objet_U::dimension)
        {
        case 1:
          x=positions(i,0);
          break;
        case 2:
          x=positions(i,0);
          y=positions(i,1);
          break;
        case 3:
          x=positions(i,0);
          y=positions(i,1);
          z=positions(i,2);
          break;
        default:
          Cerr << "Error in OctreeRoot_32_64<_SIZE_>::rang_elem_depuis" << finl;
          Process::exit();
        }
      elems[i]=rang_elem_depuis(prems[i],x,y,z);
    }
  return 1;
}


/*! @brief
 *
 * @param (ArrOfInt& elements)  we suppose we look for a small number of elements!
 * @param (double x)
 * @param (double y)
 * @param (double z)
 */
template <typename _SIZE_>
void OctreeRoot_32_64<_SIZE_>::rang_elems_sommet(SmallArrOfTID_t& elements, double x, double y, double z) const
{
  std::vector<int_t> els;
  bool fini=false;
  int i = 0;
  int_t el=0;
  while(!fini)
    {
      el = Octree_32_64<_SIZE_>::rang_elem_depuis_loc(loc, el, x, y, z);
      if(el==-1)
        fini=1;
      else
        {
          els.push_back(el);
          i++;
          el++;
        }
    }
  elements.resize_array(i);
  for(int j=0; j<i; j++)
    elements[j] = els[j];
}


/*! @brief Renvoie vrai si le domaine associe a l'octree est non nulle.
 *
 * @return (int) code de retour propage
 */
template <typename _SIZE_>
int OctreeRoot_32_64<_SIZE_>::construit() const
{
  if((le_dom.non_nul()==0)||(valid_!=1))
    // L'Octree n'est pas construit ou le domaine est nulle
    return 0;
  else
    return 1;
}

template <typename _SIZE_>
void OctreeRoot_32_64<_SIZE_>::invalide()
{
  if(valid_!=0)
    {
      Cerr << "Invalidation of the Octree" << finl;
      valid_ = 0;
      this->detruire();
    }
}


/*! @brief
 *
 * @param (OctreeLoc& loc)
 * @param (double x)
 * @param (double y)
 * @param (double z)
 * @return (int)
 */
template <typename _SIZE_>
typename OctreeFloor_32_64<_SIZE_>::int_t OctreeFloor_32_64<_SIZE_>::rang_elem_loc(const OctreeLoc& loc, double x, double y, double z) const
{
  int_t sz=num_elem.size_array();
  int_t element=-1;
  const Elem_geom_32_64<_SIZE_>& elemgeom=this->domaine().type_elem();
  pos[0]=x;
  pos[1]=y;
  if(Objet_U::dimension>2) pos[2]=z;
  // ToDo Kokkos: exemple de virtual function facile a implementer:
  for (int_t ielem = 0; ielem < sz; ielem++)
    {
      if(elemgeom->contient(pos,num_elem[ielem]))
        {
          element=num_elem[ielem];
          break;
        }
      else
        {
          //         Process::Journal() << "PAS TROUVE !!: " <<  finl;
        }
    }
  return element;
}

/*! @brief
 *
 * @param (OctreeLoc& loc)
 * @param (int prems)
 * @param (double x)
 * @param (double y)
 * @param (double z)
 * @return (int)
 */
template <typename _SIZE_>
typename OctreeFloor_32_64<_SIZE_>::int_t OctreeFloor_32_64<_SIZE_>::rang_elem_depuis_loc(const OctreeLoc& loc, int_t prems, double x, double y, double z) const
{
  int_t sz=num_elem.size_array();
  int_t element=-1;
  const Elem_geom_32_64<_SIZE_>& elemgeom = this->domaine().type_elem();
  pos[0]=x;
  pos[1]=y;
  if(Objet_U::dimension>2) pos[2]=z;
  bool trouve=false;
  int_t ielem=0;
  while((ielem < sz) && !trouve)
    {
      if(elemgeom->contient(pos,num_elem[ielem]))
        {
          element=num_elem[ielem];
          if(element >= prems)
            trouve=true;
          else
            ielem++;
        }
      else
        ielem++;
    }
  if(ielem>=sz)
    element=-1;
  return element;
}


/*! @brief
 *
 * @param (Octree* pe)
 * @param (ArrOfInt& Tab)
 * @param (OctreeLoc& loc)
 */
template <typename _SIZE_>
void OctreeFloor_32_64<_SIZE_>::construire(Octree_t* pe, const ArrOfInt_t& Tab, const OctreeLoc& loc)
{
  assert(pe!=0);
  pere=pe;
  les_octrees=0;
  num_elem=Tab;
  // Ajout B.M: tri des elements dans l'ordre croissant de l'indice local.
  // Ainsi, lors des recherches de sommets ou faces de joint, on tombe
  //  sur l'element reel, pas l'element virtuel
  num_elem.ordonne_array();
}


/*! @brief Renvoie la taille de l'octree.
 *
 * @return (unsigned) la taille de l'octree
 */
template <typename _SIZE_>
typename OctreeFloor_32_64<_SIZE_>::int_t OctreeFloor_32_64<_SIZE_>::taille() const
{
  return (int_t)sizeof(OctreeFloor_32_64)+num_elem.size_array()*(int_t)sizeof(int);
}


template class Octree_32_64<int>;
template class OctreeRoot_32_64<int>;
template class OctreeFloor_32_64<int>;
#if INT_is_64_ == 2
template class Octree_32_64<trustIdType>;
template class OctreeRoot_32_64<trustIdType>;
template class OctreeFloor_32_64<trustIdType>;
#endif

