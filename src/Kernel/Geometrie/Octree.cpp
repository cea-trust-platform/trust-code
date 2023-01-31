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

#ifndef NbCasesParNoeuds
#define NbCasesParNoeuds 512
#endif

#include <TRUSTVects.h>
#include <RTabInt.h>
#include <Domaine.h>
#include <Octree.h>

Implemente_instanciable_sans_constructeur(OctreeRoot,"OctreeRoot",Objet_U);

Sortie& Octree::printOn(Sortie& is) const
{
  int nb_octrees=Octree::nombre_d_octrees();
  int i;
  is << finl << "--------------------------------" << finl;
  is << "niveau : " << niveau() << " taille : " << taille() << finl;
  for(i=0; i<nb_octrees; i++)
    {
      if(les_octrees[i]!=0)
        {
          les_octrees[i]->printOn(is);
        }
      else
        is << "vide" << " ";
    }
  return is << finl;
}

Entree& Octree::readOn(Entree& is)
{
  return is;
}

Sortie& OctreeRoot::printOn(Sortie& os) const
{
  return Octree::printOn(os);
}

Entree& OctreeRoot::readOn(Entree& is)
{
  return Octree::readOn(is);
}

Sortie& OctreeFloor::printOn(Sortie& os) const
{
  Octree::printOn(os);
  return os << num_elem << finl;
}

Entree& OctreeFloor::readOn(Entree& is)
{
  return Octree::readOn(is);
}


/*! @brief Destructeur: appelle Octree::detruire().
 *
 */
Octree::~Octree()
{
  detruire();
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
void OctreeLoc::construire(const OctreeLoc& loc, int  i,
                           double xmil, double ymil, double zmil)
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


/*! @brief Renvoie le nombre d'octree.
 *
 * @return (int) le nombre d'octree
 */
int Octree::nombre_d_octrees()
{
  int nb_octrees;
  switch(Objet_U::dimension)
    {
    case 1:
      nb_octrees=2;
      break;
    case 2:
      nb_octrees=4;
      break;
    case 3:
      nb_octrees=8;
      break;
    default :
      nb_octrees=0;
      break;
    }
  return nb_octrees;
}


/*! @brief
 *
 * @param (OctreeLoc& loc)
 * @param (double x)
 * @param (double y)
 * @param (double z)
 * @return (int)
 */
int Octree::rang_elem_loc(const OctreeLoc& loc, double x, double y, double z) const
{
  int element=-1;

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
        {
          return -1;
        }
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
int Octree::rang_elem_depuis_loc(const OctreeLoc& loc, int prems, double x, double y, double z) const
{
  int element=-1;

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


/*! @brief Renvoie une reference sur la domaine associe a l'octree.
 *
 * @return (Domaine&) reference sur la domaine associe a l'octree
 */
const Domaine& Octree::domaine() const
{
  return pere->domaine();
}
double Octree::get_epsilon() const
{
  return domaine().epsilon();
}
void Octree::ranger_elem_1D(ArrOfInt& ok, int elem, int i, int nb_som_elem, const DoubleTab& coord,
                            const IntTab& elems, ArrOfInt& compteur,
                            IntVects& SousTab, double xmil)
{
  for(int som=0; som<nb_som_elem; som++)
    {
      int sommet=elems(i,som);
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
inline void range2D(double x, double y, double xmil, double ymil,
                    ArrOfInt& ok, IntVects& SousTab, ArrOfInt& compteur, int i)
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
inline void range3D(double x, double y, double z,
                    double xmil, double ymil, double zmil,
                    ArrOfInt& ok, IntVects& SousTab, ArrOfInt& compteur, int i)
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

void Octree::ranger_elem_2D(ArrOfInt& ok, int elem, int i, int nb_som_elem, const DoubleTab& coord,
                            const IntTab& elems, ArrOfInt& compteur,
                            IntVects& SousTab, double xmil, double ymil)
{
  double xmin=DMAXFLOAT, xmax=-DMAXFLOAT;
  double ymin=xmin, ymax=xmax;
  for(int som=0; som<nb_som_elem; som++)
    {
      int sommet=elems(i,som);
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
  range2D(xmin, ymin, xmil, ymil, ok, SousTab, compteur, i);
  range2D(xmin, ymax, xmil, ymil, ok, SousTab, compteur, i);
  range2D(xmax, ymin, xmil, ymil, ok, SousTab, compteur, i);
  range2D(xmax, ymax, xmil, ymil, ok, SousTab, compteur, i);
}

void Octree::ranger_elem_3D(ArrOfInt& ok, int elem, int i, int nb_som_elem, const DoubleTab& coord,
                            const IntTab& elems, ArrOfInt& compteur,
                            IntVects& SousTab, double xmil, double ymil, double zmil)
{
  double epsilon=get_epsilon();
  double xmin=DMAXFLOAT, xmax=-DMAXFLOAT;
  double ymin=xmin, ymax=xmax;
  double zmin=xmin, zmax=xmax;
  for(int som=0; som<nb_som_elem; som++)
    {
      int sommet=elems(i,som);
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
  range3D(xmin, ymin, zmin, xmil, ymil, zmil, ok, SousTab, compteur, i);
  range3D(xmin, ymax, zmin, xmil, ymil, zmil, ok, SousTab, compteur, i);
  range3D(xmax, ymin, zmin, xmil, ymil, zmil, ok, SousTab, compteur, i);
  range3D(xmax, ymax, zmin, xmil, ymil, zmil, ok, SousTab, compteur, i);
  range3D(xmin, ymin, zmax, xmil, ymil, zmil, ok, SousTab, compteur, i);
  range3D(xmin, ymax, zmax, xmil, ymil, zmil, ok, SousTab, compteur, i);
  range3D(xmax, ymin, zmax, xmil, ymil, zmil, ok, SousTab, compteur, i);
  range3D(xmax, ymax, zmax, xmil, ymil, zmil, ok, SousTab, compteur, i);
}

/*! @brief
 *
 * @param (int nb_octrees)
 * @param (ArrOfInt& Tab)
 * @param (OctreeLoc& loc)
 * @param (Octree* pe)
 */
void Octree::construire(int nb_octrees, const ArrOfInt& Tab,
                        const OctreeLoc& loc, Octree* pe)
{
  //static int nb_octree=0;
  static int level=0;
  //nb_octree++;
  int nb_elem=Tab.size_array();
  pere=pe;
  les_octrees=new Octree*[nb_octrees];

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
        {
          les_octrees[i]=0;
        }
      les_octrees[0]=new OctreeFloor(this, Tab, loc);
    }
  else
    {
      //else
      const Domaine& z=domaine();
      const Domaine& dom=z;
      IntVects SousTab(nb_octrees);
      ArrOfInt compteur(nb_octrees);
      int nb_som_elem=z.nb_som_elem();
      int i;
      const IntTab& les_elems=z.les_elems();
      const DoubleTab& coord_sommets=dom.coord_sommets();
      for(i=0; i<nb_octrees; i++)
        SousTab[i].resize(nb_elem);
      double xmil=(loc.xmin+loc.xmax)/2;
      double ymil=(loc.ymin+loc.ymax)/2;
      double zmil=(loc.zmin+loc.zmax)/2;
      ArrOfInt ok(nb_octrees);
      for(int elem=0; elem<nb_elem; elem++)
        {
          i=Tab[elem];
          ok=0;
          switch(Objet_U::dimension)
            {
            case 1:
              ranger_elem_1D(ok, elem, i , nb_som_elem, coord_sommets,
                             les_elems, compteur, SousTab, xmil);
              break;
            case 2:
              ranger_elem_2D(ok, elem, i , nb_som_elem, coord_sommets,
                             les_elems, compteur, SousTab, xmil, ymil);
              break;
            case 3:
              ranger_elem_3D(ok, elem, i ,nb_som_elem, coord_sommets,
                             les_elems, compteur, SousTab, xmil, ymil, zmil);
              break;
            default:
              Cerr << "Space dimension " << Objet_U::dimension << " incorrect" << finl;
              Process::exit();
            }
        }

      for(i=0; i<nb_octrees; i++)
        {
          les_octrees[i]=0;
          OctreeLoc loci;
          loci.construire(loc, i, xmil, ymil, zmil);
          SousTab[i].resize(compteur[i]);
          if(compteur[i]==nb_elem)
            {
              les_octrees[i]=new OctreeFloor(this, SousTab[i], loci);
            }
          else if(compteur[i]>NbCasesParNoeuds)
            {
              les_octrees[i]=new Octree(nb_octrees, this, SousTab[i], loci);
            }
          else if(compteur[i]!=0)
            {
              les_octrees[i]=new OctreeFloor(this, SousTab[i], loci);
            }
        }
      //Cerr << "On continue dans cette direction" << finl;
    }
}


/*! @brief Detruit l'octree.
 *
 * Methode appelee par le destructeur
 *
 */
void Octree::detruire()
{
  if(les_octrees==0)
    return;
  int nb_octrees=Octree::nombre_d_octrees();
  for(int i=0; i<nb_octrees; i++)
    if(les_octrees[i])
      delete les_octrees[i];
  delete[] les_octrees;
  les_octrees=0;
}


int Octree::niveau() const
{
  int i=0;
  if (pere==0) return i;
  return (pere->niveau()+1);
}


/*! @brief Renvoie la taille de l'octree.
 *
 * @return (unsigned) la taille de l'octree
 */
int Octree::taille() const
{
  int nb_octrees=Octree::nombre_d_octrees();
  unsigned la_taille = sizeof(Octree);
  for(int i=0; i<nb_octrees; i++)
    if(les_octrees[i]!=0)
      la_taille+= les_octrees[i]->taille();
  return la_taille;
}


/*! @brief Renvoie la domaine associe a l'octree.
 *
 * @return (Domaine&) la domaine associe a l'octree
 */
const Domaine& OctreeRoot::domaine() const
{
  return le_dom.valeur();
}


/*! @brief Associe une domaine a l'octree
 *
 * @param (Domaine& z) la domaine a associer a l'octree
 */
void OctreeRoot::associer_Domaine(const Domaine& z)
{
  le_dom=z;
}

void OctreeRoot::construire(int reel_prec)
{
  detruire();
  valid_=1;
  reel_=reel_prec;
  pere=0;
  const Domaine& z=domaine();
  const Domaine& dom=z;

  {
    // Calcul du min et du max des coordonnees dans chaque direction
    double min[3] = { 1e30, 1e30, 1e30 };
    double max[3] = {-1e30,-1e30,-1e30 };
    int i;
    const DoubleTab& tab = dom.coord_sommets();
// 	tot sinon bouding box trop petite
// 	peut etre teste reel_ ...
    const int n = tab.dimension_tot(0);
    const int dim = tab.dimension(1);
    assert(dim <= 3);
    for (i = 0; i < n; i++)
      {
        int j;
        for (j = 0; j < 3; j++)
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
  int nb_elem=z.nb_elem_tot();
  if(reel_prec)
    nb_elem=z.nb_elem();

  ArrOfInt SousTab(nb_elem);
  for(int i=0; i<nb_elem; i++)
    SousTab[i]=i;
  Octree::construire(Octree::nombre_d_octrees(), SousTab, loc);
  Cerr << "Construction of the OctreeRoot OK " << finl;
}


/*! @brief
 *
 * @param (double x)
 * @param (double y)
 * @param (double z)
 * @return (int)
 * @throws Erreur dans OctreeRoot::rang_sommet
 * @throws On a pas trouve de sommet a ces coordonnees
 */
int OctreeRoot::rang_sommet(double x, double y, double z) const
{
  int elem=rang_elem(x, y, z);
  if (elem != -1)
    {
      const Domaine& zo=domaine();
      const Domaine& dom=zo;
      int nb_som_elem=zo.nb_som_elem();
      double epsilon=get_epsilon();
      for(int i=0; i<nb_som_elem; i++)
        {
          //for i
          int sommet=zo.sommet_elem(elem,i);
          if(sommet<0)
            {
              Cerr << "This is not a mesh !" << finl;
              Cerr << zo << finl;
              exit();
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
              Cerr << "Error in OctreeRoot::rang_sommet" << finl;
              assert(0);
              exit();
              return -1;
            }
        }
      return -1;
    }
  else
    //Cerr << "On a pas trouve de sommet a ces coordonnees" << finl;
    return -1;
}

/*! @brief
 *
 * @param (double x)
 * @param (double y)
 * @param (double z)
 * @return (int)
 * @throws Erreur dans OctreeRoot::rang_arete
 * @throws On a pas trouve d'arete a ces coordonnees
 */
int OctreeRoot::rang_arete(double x, double y, double z) const
{
  // Recherche l'element contenant le point x,y,z:
  int elem=rang_elem(x, y, z);
  const IntTab& elem_aretes=domaine().elem_aretes();
  if (elem>=elem_aretes.dimension(0)) return -2;
  if (elem != -1 && elem<elem_aretes.dimension(0))
    {
      // Boucle sur les aretes de l'element
      double epsilon=get_epsilon();
      const IntTab& aretes_som=domaine().aretes_som();
      const DoubleTab& coord=domaine().coord_sommets();
      int nb_aretes_elem=elem_aretes.dimension(1);
      for(int i=0; i<nb_aretes_elem; i++)
        {
          // On boucle sur les aretes de l'element pour trouver celle
          // dont le centre de gravite coincide avec le point x,y,z
          int arete=elem_aretes(elem,i);
          int s0=aretes_som(arete,0);
          int s1=aretes_som(arete,1);
          switch(Objet_U::dimension)
            {
            case 3:
              if((std::fabs(0.5*(coord(s0,0)+coord(s1,0))-x)<epsilon) &&
                  (std::fabs(0.5*(coord(s0,1)+coord(s1,1))-y)<epsilon) &&
                  (std::fabs(0.5*(coord(s0,2)+coord(s1,2))-z)<epsilon))
                return arete;
              break;
            default:
              Cerr << "Error in OctreeRoot::rang_elem" << finl;
              exit();
              return -1;
            }
        }
      return -1;
    }
  else
    //Cerr << "On a pas trouve d'arete a ces coordonnees" << finl;
    return -1;
}

/*! @brief
 *
 * @param (double x)
 * @param (double y)
 * @param (double z)
 * @return (int)
 */
int OctreeRoot::rang_elem(double x, double y, double z) const
{
  double epsilon=get_epsilon();
  if(       (x<loc.xmin-epsilon) || (y<loc.ymin-epsilon) || (z<loc.zmin-epsilon)
            ||   (x>loc.xmax+epsilon) || (y>loc.ymax+epsilon) || (z>loc.zmax+epsilon)   )
    return -1;
  return Octree::rang_elem_loc(loc, x, y, z);
}


/*! @brief
 *
 * @param (int prems)
 * @param (double x)
 * @param (double y)
 * @param (double z)
 */
int OctreeRoot::rang_elem_depuis(int prems, double x, double y, double z) const
{
  double epsilon=get_epsilon();
  if(       (x<loc.xmin-epsilon) || (y<loc.ymin-epsilon) || (z<loc.zmin-epsilon)
            ||   (x>loc.xmax+epsilon) || (y>loc.ymax+epsilon) || (z>loc.zmax+epsilon)   )
    return -1;
  return Octree::rang_elem_depuis_loc(loc, prems, x, y, z);
}


/*! @brief
 *
 * @param (DoubleTab& positions)
 * @param (ArrOfInt& sommets)
 * @return (int)
 * @throws dimension d'espace non prevue
 */
int OctreeRoot::rang_sommet(const DoubleTab& positions, ArrOfInt& sommets) const
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
          Cerr << "Error in OctreeRoot::rang_sommet" << finl;
          assert(0);
          exit();
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
int OctreeRoot::rang_arete(const DoubleTab& positions, ArrOfInt& aretes) const
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
          Cerr << "Error in OctreeRoot::rang_arete" << finl;
          exit();
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
int OctreeRoot::rang_elem(const DoubleTab& positions, ArrOfInt& elems) const
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
          Cerr << "Error in OctreeRoot::rang_elem" << finl;
          assert(0);
          exit();
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
int OctreeRoot::rang_elem_depuis(const DoubleTab& positions, const ArrOfInt& prems, ArrOfInt& elems) const
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
          Cerr << "Error in OctreeRoot::rang_elem_depuis" << finl;
          assert(0);
          exit();
        }
      elems[i]=rang_elem_depuis(prems[i],x,y,z);
    }
  return 1;
}


/*! @brief
 *
 * @param (ArrOfInt& elements)
 * @param (double x)
 * @param (double y)
 * @param (double z)
 */
void OctreeRoot::rang_elems_sommet(ArrOfInt& elements, double x, double y, double z) const
{
  RTabInt els;
  int fini=0;
  int i=0;
  int el=0;
  while(fini==0)
    {
      el = Octree::rang_elem_depuis_loc(loc, el, x, y, z);
      if(el==-1)
        fini=1;
      else
        {
          els.add(el);
          i++;
          el++;
        }
    }
  elements.resize_array(i);
  for(int j=0; j<i; j++)
    elements[j] = els[j];
}


/*! @brief Renvoie vrai si la domaine associee a l'octree est non nulle.
 *
 * @return (int) code de retour propage
 */
int OctreeRoot::construit() const
{
  if((le_dom.non_nul()==0)||(valid_!=1))
    // L'Octree n'est pas construit ou la domaine est nulle
    return 0;
  else
    return 1;
}

void OctreeRoot::invalide()
{
  if(valid_!=0)
    {
      Cerr << "Invalidation of the Octree" << finl;
      valid_ = 0;
      detruire();
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
int OctreeFloor::rang_elem_loc(const OctreeLoc& loc, double x, double y, double z) const
{
  int sz=num_elem.size_array();
  int element=-1;
  const Elem_geom& elemgeom=domaine().type_elem();
  pos[0]=x;
  if(Objet_U::dimension>1) pos[1]=y;
  if(Objet_U::dimension>2) pos[2]=z;
  for (int ielem = 0; ielem < sz; ielem++)
    {

      if(elemgeom.contient(pos,num_elem[ielem]))
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
int OctreeFloor::rang_elem_depuis_loc(const OctreeLoc& loc, int prems, double x, double y, double z) const
{
  int sz=num_elem.size_array();
  int element=-1;
  const Elem_geom& elemgeom=domaine().type_elem();
  pos[0]=x;
  if(Objet_U::dimension>1) pos[1]=y;
  if(Objet_U::dimension>2) pos[2]=z;
  int trouve=0;
  int ielem=0;
  while((ielem < sz) && (trouve==0))
    {
      if(elemgeom.contient(pos,num_elem[ielem]))
        {
          element=num_elem[ielem];
          if(element >= prems)
            trouve=1;
          else
            ielem++;
        }
      else
        ielem++;
    }
  if(ielem>=sz)
    {
      element=-1;
    }
  return element;
}


/*! @brief
 *
 * @param (Octree* pe)
 * @param (ArrOfInt& Tab)
 * @param (OctreeLoc& loc)
 */
void OctreeFloor::construire(Octree* pe, const ArrOfInt& Tab, const OctreeLoc& loc)
{
  //static int nb_octreefloor=0;
  //nb_octreefloor++;
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
int OctreeFloor::taille() const
{
  return (int)sizeof(OctreeFloor)+num_elem.size_array()*(int)sizeof(int);
}

