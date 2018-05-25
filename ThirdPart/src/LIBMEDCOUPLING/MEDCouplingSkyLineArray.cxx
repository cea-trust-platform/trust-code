// Copyright (C) 2007-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "MEDCouplingSkyLineArray.hxx"

#include <sstream>
#include <deque>
#include <set>

using namespace MEDCoupling;

MEDCouplingSkyLineArray::MEDCouplingSkyLineArray():
  _super_index( DataArrayInt::New() ), _index( DataArrayInt::New() ), _values( DataArrayInt::New() )
{
}

MEDCouplingSkyLineArray::~MEDCouplingSkyLineArray()
{
}

MEDCouplingSkyLineArray* MEDCouplingSkyLineArray::New()
{
  return new MEDCouplingSkyLineArray();
}

MEDCouplingSkyLineArray* MEDCouplingSkyLineArray::New( const std::vector<int>& index,
                                                       const std::vector<int>& value )
{
  MEDCouplingSkyLineArray * ret = new MEDCouplingSkyLineArray();
  ret->_index->reserve( index.size() );
  ret->_index->insertAtTheEnd( index.begin(), index.end() );
  ret->_values->reserve( value.size() );
  ret->_values->insertAtTheEnd( value.begin(), value.end() );
  return ret;
}

MEDCouplingSkyLineArray* MEDCouplingSkyLineArray::New( DataArrayInt* index, DataArrayInt* value )
{
  MEDCouplingSkyLineArray* ret = new MEDCouplingSkyLineArray();
  ret->set(index, value);
  return ret;
}

MEDCouplingSkyLineArray* MEDCouplingSkyLineArray::New( const MEDCouplingSkyLineArray & other )
{
  MEDCouplingSkyLineArray* ret = new MEDCouplingSkyLineArray();
  ret->_super_index = other._super_index;
  ret->_index = other._index;
  ret->_values = other._values;
  return ret;
}

/**! Build a three level SkyLine array from the dynamic connectivity of a dynamic mesh (i.e. containing only
 * polyhedrons or polygons).
 * The input arrays are deep copied, contrary to the other ctors.
 */
MEDCouplingSkyLineArray * MEDCouplingSkyLineArray::BuildFromPolyhedronConn( const DataArrayInt* c, const DataArrayInt* cI )
{
  using namespace std;

  MEDCouplingSkyLineArray* ret = new MEDCouplingSkyLineArray();

  const int * cP(c->begin()), * cIP(cI->begin());
  int prev = -1;
  if ((int)c->getNbOfElems() != *(cI->end()-1))
    throw INTERP_KERNEL::Exception("MEDCouplingSkyLineArray::BuildFromDynamicConn: misformatted connectivity (wrong nb of tuples)!");
  for (std::size_t i=0; i < cI->getNbOfElems(); i++)
    {
      int j = cIP[i];
      if (cIP[i] < prev)
        throw INTERP_KERNEL::Exception("MEDCouplingSkyLineArray::BuildFromDynamicConn: misformatted connectivity (indices not monotonic ascending)!");
      prev = cIP[i];
      if (i!=cI->getNbOfElems()-1)
        if (cP[j] != INTERP_KERNEL::NORM_POLYHED)
          throw INTERP_KERNEL::Exception("MEDCouplingSkyLineArray::BuildFromDynamicConn: connectivity containing other types than POLYHED!");
    }

  vector<int> superIdx, idx, vals;
  int cnt = 0, cnt2 = 0;
  superIdx.reserve(cI->getNbOfElems());
  superIdx.push_back(0);
  idx.push_back(0);
  vals.resize(c->getNbOfElems()); // too much because of the type and the -1, but still better than push_back().
  for (std::size_t i=0; i < cI->getNbOfElems()-1; i++)
    {
      int start = cIP[i]+1, end = cIP[i+1];
      int * work = vals.data() + cnt;
      const int * w = cP+start;
      const int * w2 = find(w, cP+end, -1);
      while (w2 != cP+end)
        {
          copy(w, w2, work);
          int d = distance(w, w2);
          cnt += d; work +=d;
          idx.push_back(cnt); cnt2++;
          w = w2+1;  // skip the -1
          w2 = find(w, cP+end, -1);
        }
      copy(w, cP+end, work);
      cnt += distance(w, cP+end);
      idx.push_back(cnt); cnt2++;
      superIdx.push_back(cnt2);
    }
  ret->_super_index->alloc(superIdx.size(),1);
  copy(superIdx.begin(), superIdx.end(), ret->_super_index->getPointer());
  ret->_index->alloc(idx.size(),1);
  copy(idx.begin(), idx.end(), ret->_index->getPointer());
  ret->_values->alloc(cnt,1);
  copy(vals.begin(), vals.begin()+cnt, ret->_values->getPointer());

  return ret;
}

/**
 * Convert a three-level SkyLineArray into a polyhedral connectivity.
 * The super-packs are interpreted as cell description, and the packs represent the face connectivity.
 */
void MEDCouplingSkyLineArray::convertToPolyhedronConn( MCAuto<DataArrayInt>& c,  MCAuto<DataArrayInt>& cI) const
{
  // TODO: in this case an iterator would be nice
  using namespace std;

  checkSuperIndex("convertToPolyhedronConn");

  const int * siP(_super_index->begin()), * iP(_index->begin()), *vP(_values->begin());
  int cnt = 0;
  cI->alloc(_super_index->getNbOfElems(),1);  // same number of super packs as number of cells
  int * cIVecP(cI->getPointer());
  MCAuto <DataArrayInt> dsi = _index->deltaShiftIndex();
  int sz = dsi->accumulate(0) + dsi->getNbOfElems();  // think about it: one slot for the type, -1 at the end of each face of the cell
  c->alloc(sz, 1);
  int * cVecP(c->getPointer());

  for ( std::size_t i=0; i < _super_index->getNbOfElems()-1; i++)
     {
       cIVecP[i]= cnt;
       int endId = siP[i+1];
       cVecP[cnt++] = INTERP_KERNEL::NORM_POLYHED;
       for (int j=siP[i]; j < endId; j++)
         {
           int startId2 = iP[j], endId2 = iP[j+1];
           copy(vP+startId2, vP+endId2, cVecP+cnt);
           cnt += endId2-startId2;
           if(j != endId-1)
             cVecP[cnt++] = -1;
         }
     }
  cIVecP[_super_index->getNbOfElems()-1] = cnt;
}

std::size_t MEDCouplingSkyLineArray::getHeapMemorySizeWithoutChildren() const
{
  return _index->getHeapMemorySizeWithoutChildren()+_values->getHeapMemorySizeWithoutChildren()+_super_index->getHeapMemorySizeWithoutChildren();
}

std::vector<const BigMemoryObject *> MEDCouplingSkyLineArray::getDirectChildrenWithNull() const
{
  std::vector<const BigMemoryObject *> ret;
  ret.push_back(_super_index);
  ret.push_back(_index);
  ret.push_back(_values);
  return ret;
}


void MEDCouplingSkyLineArray::set( DataArrayInt* index, DataArrayInt* value )
{
  _index=index;
  _values=value;
  if ( (DataArrayInt*)_index ) _index->incrRef();
  else                         _index = DataArrayInt::New();
  if ( (DataArrayInt*)_values ) _values->incrRef();
  else                         _values = DataArrayInt::New();
}

void MEDCouplingSkyLineArray::set3( DataArrayInt* superIndex, DataArrayInt* index, DataArrayInt* value )
{
  _super_index=superIndex;
  if ( (DataArrayInt*)_super_index ) _super_index->incrRef();
  else                         _super_index = DataArrayInt::New();
  set(index, value);
}

DataArrayInt* MEDCouplingSkyLineArray::getSuperIndexArray() const
{
  return const_cast<MEDCouplingSkyLineArray*>(this)->_super_index;
}


DataArrayInt* MEDCouplingSkyLineArray::getIndexArray() const
{
  return const_cast<MEDCouplingSkyLineArray*>(this)->_index;
}

DataArrayInt* MEDCouplingSkyLineArray::getValuesArray() const
{
  return const_cast<MEDCouplingSkyLineArray*>(this)->_values;
}

void MEDCouplingSkyLineArray::checkSuperIndex(const std::string& func) const
{
  if (!_super_index->getNbOfElems())
    {
      std::ostringstream oss;
      oss << "MEDCouplingSkyLineArray::"<< func << ": not a three level SkyLineArray! Method is not available for two-level SkyLineArray.";
      throw INTERP_KERNEL::Exception(oss.str());
    }
}

void MEDCouplingSkyLineArray::validSuperIndex(const std::string& func, int superIndex) const
{
  if(superIndex < 0 || superIndex >= (int)_super_index->getNbOfElems())
    {
      std::ostringstream oss;
      oss << "MEDCouplingSkyLineArray::" << func <<  ": invalid super index!";
      throw INTERP_KERNEL::Exception(oss.str());
    }
}

void MEDCouplingSkyLineArray::validIndex(const std::string& func, int idx) const
{
  if(idx < 0 || idx >= (int)_index->getNbOfElems())
    {
      std::ostringstream oss;
      oss << "MEDCouplingSkyLineArray::" << func <<  ": invalid index!";
      throw INTERP_KERNEL::Exception(oss.str());
    }
}

void MEDCouplingSkyLineArray::validSuperIndexAndIndex(const std::string& func, int superIndex, int index) const
{
  validSuperIndex(func, superIndex);
  int idx = _super_index->begin()[superIndex] + index;
  if(idx < 0 || idx >= (int)_index->getNbOfElems())
    {
      std::ostringstream oss;
      oss << "MEDCouplingSkyLineArray::" << func <<  ": invalid index!";
      throw INTERP_KERNEL::Exception(oss.str());
    }
}

std::string MEDCouplingSkyLineArray::simpleRepr() const
{
  std::ostringstream oss;
  oss << "MEDCouplingSkyLineArray (" << this << ")" << std::endl;
  MCAuto<DataArrayInt> super_index = _super_index->deepCopy();
  if (_super_index->getNbOfElems())
    oss << "   Nb of super-packs: " << getSuperNumberOf() << std::endl;
  else
    {
      super_index->alloc(2,1);
      super_index->setIJSilent(0,0,0);
      super_index->setIJSilent(1,0,_index->getNbOfElems()-1);
    }
  oss << "   Nb of packs: " << getNumberOf() << std::endl;
  oss << "   Nb of values: " << getLength() << std::endl;

  if (_super_index->getNbOfElems())
    {
      oss << "   Super-indices:" << std::endl;
      oss << "   ";
      const int * i = _super_index->begin();
      for ( ; i != _super_index->end(); ++i )
        oss << *i << " ";
      oss << std::endl;
    }

  oss << "   Indices:" << std::endl;
  oss << "   ";
  const int * i = _index->begin();
  for ( ; i != _index->end(); ++i )
    oss << *i << " ";
  oss << std::endl;
  oss << "   Values:" << std::endl;
  oss << "     ";
  const int * v = _values->begin();
  int cnt = 0, cntI = 0;
  i = _index->begin();
  for ( const int * si = super_index->begin()+1; v != _values->end(); ++v, ++cnt )
    {
      if ( cnt == *i )
        {
          if ( cntI == *si && cnt != 0)
            {
              oss << std::endl << "     ";
              ++si;
            }

          oss << "| ";
          ++i; ++cntI;
        }
      oss << *v << " ";
    }
  oss << std::endl;

  return oss.str();
}

/**
 * For a 2- or 3-level SkyLine array, return a copy of the absolute pack with given identifier.
 */
void MEDCouplingSkyLineArray::getSimplePackSafe(const int absolutePackId, std::vector<int> & pack) const
{
  if(absolutePackId < 0 || absolutePackId >= (int)_index->getNbOfElems())
    throw INTERP_KERNEL::Exception("MEDCouplingSkyLineArray::getPackSafe: invalid index!");
  const int * iP(_index->begin()), *vP(_values->begin());
  int sz = iP[absolutePackId+1]-iP[absolutePackId];
  pack.resize(sz);
  std::copy(vP+iP[absolutePackId], vP+iP[absolutePackId+1],pack.begin());
}

/**
 * Same as getPackSafe, but directly returns a pointer to the internal data with the size of the pack.
 */
const int * MEDCouplingSkyLineArray::getSimplePackSafePtr(const int absolutePackId, int & packSize) const
{
  if(absolutePackId < 0 || absolutePackId >= (int)_index->getNbOfElems())
    throw INTERP_KERNEL::Exception("MEDCouplingSkyLineArray::getPackSafe: invalid index!");
  const int * iP(_index->begin()), *vP(_values->begin());
  packSize = iP[absolutePackId+1]-iP[absolutePackId];
  return vP+iP[absolutePackId];
}


/**!
 * For each given super-pack ID, provide the sub-index of the first matching pack. If no matching pack is found for the
 * given super-pack -1 is returned.
 * \param[in] superPackIndices the list of super-packs that should be inspected
 * \param[in] packBg the pack that the function is looking for in each of the provided super-pack
 * \param[in] packEnd the pack that the function is looking for in each of the provided super-pack
 * \param[out] a vector of int, having the same size as superPackIndices and containing for each inspected super-pack
 * the index of the first matching pack, or -1 if none found.
 */
void MEDCouplingSkyLineArray::findPackIds(const std::vector<int> & superPackIndices,
                                          const int *packBg, const int *packEnd,
                                          std::vector<int>& out) const
{
  using namespace std;

  checkSuperIndex("findPackIds");

  int packSz = std::distance(packBg, packEnd);
  if (!packSz)
    throw INTERP_KERNEL::Exception("MEDCouplingSkyLineArray::findPackIds: void pack!");

  out.resize(superPackIndices.size());
  int i = 0;
  const int * siP(_super_index->begin()), * iP(_index->begin()), *vP(_values->begin());
  for(vector<int>::const_iterator it=superPackIndices.begin(); it!=superPackIndices.end(); ++it, i++)
    {
      out[i] = -1;
      const int sPackIdx = *it;
      // for each pack
      for (int idx=siP[sPackIdx], j=0; idx < siP[sPackIdx+1]; idx++, j++)
        {
          if (packSz == (iP[idx+1] - iP[idx]))
            if (equal(&vP[iP[idx]], &vP[iP[idx+1]], packBg))
              {
                out[i] = j;
                break;
              }
        }
    }
}

/**!
 * Delete pack number 'idx' in super-pack number 'superIdx'.
 * \param[in] superIdx is the super-pack number
 * \param[in] idx is the pack index inside the super-pack 'superIdx'.
 */
void MEDCouplingSkyLineArray::deletePack(const int superIdx, const int idx)
{
  checkSuperIndex("deletePack");
  validSuperIndexAndIndex("deletePack", superIdx, idx);

  int * vP = _values->getPointer();
  int * siP(_super_index->getPointer()), *iP(_index->getPointer());
  const int start = iP[siP[superIdx]+idx], end = iP[siP[superIdx]+idx+1];
  // _values
  std::copy(vP+end, vP+_values->getNbOfElems(), vP+start);
  _values->reAlloc(_values->getNbOfElems() - (end-start));

  // _index
  int nt = _index->getNbOfElems();
  std::copy(iP+siP[superIdx]+idx+1, iP+nt, iP+siP[superIdx]+idx);
  _index->reAlloc(nt-1); iP = _index->getPointer();  // better not forget this ...
  for(int ii = siP[superIdx]+idx; ii < nt-1; ii++)
    iP[ii] -= (end-start);

  // _super_index
  for(int ii = superIdx+1; ii < (int)_super_index->getNbOfElems(); ii++)
    (siP[ii])--;
}

void MEDCouplingSkyLineArray::deleteSimplePack(const int idx)
{
  validIndex("deleteSimplePack", idx);
  
  int* iP(_index->getPointer());
  const int start(iP[idx]), end(iP[idx+1]);

  // _values
  int initValSz( _values->getNbOfElems() );
  int deltaSz( start-end );  // should be negative
  int *vP(_values->getPointer());
  if (deltaSz < 0)
    {
      std::copy(vP+end, vP+initValSz, vP+start);
      _values->reAlloc(initValSz+deltaSz);
    }
  else
    throw INTERP_KERNEL::Exception("MEDCouplingSkyLineArray::deleteSimplePack");
  // _index
  int nt(_index->getNbOfElems());
  std::copy(iP+idx+1, iP+nt, iP+idx);
  for(int ii = idx; ii < nt-1; ii++)
    iP[ii] += deltaSz;
  _index->reAlloc(nt-1);
}

void MEDCouplingSkyLineArray::replaceSimplePacks(const DataArrayInt* idx, const std::vector<const DataArrayInt*>& packs)
{    
  if (idx->empty())
    return;
    
  for (const int * id = idx->begin(); id != idx->end(); id++)
    validIndex("deleteSimplePacks", *id);
    
  if (idx->getNbOfElems() != packs.size())
    throw INTERP_KERNEL::Exception("MEDCouplingSkyLineArray::deleteSimplePacks: size of list of pack is incorrect");
    
  // copy _index, _values into a deque<set<int>>
  std::deque< std::set<int> > valuesByIdx;
  int* vP(_values->getPointer());
  int* iP(_index->getPointer());
  std::size_t nt ( _index->getNbOfElems() );
  for (int ii = 0; ii < nt-1; ii++)
    valuesByIdx.push_back(std::set<int>(vP+iP[ii], vP+iP[ii+1]));
    
  // modify the deque<set<int>> according to idx and packs
  int ii(0);
  for (const int *id = idx->begin(); id != idx->end(); id++)
    {
      valuesByIdx[*id] = std::set<int>(packs[ii]->begin(), packs[ii]->end());
      ii++;
    }
  // copy back the deque<set<int>> into _index, _values
  int valSz(0);
  *iP = 0;
  for (std::deque< std::set<int> >::const_iterator values=valuesByIdx.begin();values!=valuesByIdx.end();values++)
    {
      valSz += (*values).size();
      *(++iP) = valSz;
    }
  _values->reAlloc(valSz);
  iP = _index->getPointer();
  vP = _values->getPointer();
  for (std::deque< std::set<int> >::const_iterator values=valuesByIdx.begin(); values!=valuesByIdx.end(); ++values)
    {
      std::copy((*values).begin(), (*values).end(), vP+(*iP));
      iP++;
    }
}

void MEDCouplingSkyLineArray::deleteSimplePacks(const DataArrayInt* idx)
{    
  for (const int *id = idx->begin(); id != idx->end(); id++)
    validIndex("deleteSimplePacks", *id);
  
  std::set<int> packsToDelete(idx->begin(), idx->end());
    
  // _values
  int* iP(_index->getPointer());
  int initValSz = _values->getNbOfElems();
  int *vP(_values->getPointer());
  int end_prec(0),start_prec(0);
  for(std::set<int>::const_iterator ii=packsToDelete.begin();ii!=packsToDelete.end();ii++)
    {
      int start = iP[*ii];
      if (end_prec != 0)
        std::copy(vP+end_prec, vP+start, vP+start_prec);
      start_prec += start-end_prec;
      end_prec = iP[*ii+1];
    }
  if (end_prec != 0)
    std::copy(vP+end_prec, vP+initValSz, vP+start_prec);
  _values->reAlloc(initValSz-(end_prec-start_prec));
    
  // _index
  int nt = _index->getNbOfElems();
  int offset = 0;
  end_prec = 0;
  start_prec = 0;
  int deleted = 0;
  for(std::set<int>::const_iterator ii=packsToDelete.begin();ii!=packsToDelete.end();ii++)
    {
      if (end_prec != 0)
        {
          std::copy(iP+end_prec, iP+*ii, iP+start_prec);
          for (int i=start_prec; i<*ii; i++)
            iP[i] -= offset;
        }
      offset += iP[*ii+1] - iP[*ii];
      start_prec = *ii-deleted;
      end_prec = *ii+1;
      deleted += 1;
    }
  if (end_prec != 0)
    {
      std::copy(iP+end_prec, iP+nt, iP+start_prec);
      for (int i=start_prec; i<nt; i++)
        iP[i] -= offset;
    }
  _index->reAlloc(nt-deleted);
}

/**!
 * Insert a new pack in super-pack at index 'superIdx'. The pack is inserted at the end of the pack list of the chosen super-pack.
 */
void MEDCouplingSkyLineArray::pushBackPack(const int superIdx, const int * packBg, const int * packEnd)
{
  using namespace std;

  checkSuperIndex("pushBackPack");
  validSuperIndex("pushBackPack", superIdx);

  int *siP(_super_index->getPointer()), *iP(_index->getPointer());
  const int sz(distance(packBg, packEnd));

  // _values
  _values->reAlloc(_values->getNbOfElems()+sz);
  int * vPE(_values->getPointer()+_values->getNbOfElems());
  int *vP(_values->getPointer());
  copy(vP+iP[siP[superIdx+1]], vPE-sz, vP+iP[siP[superIdx+1]]+sz);
  // insert pack
  copy(packBg, packEnd, vP+iP[siP[superIdx+1]]);

  // _index
  int nt = _index->getNbOfElems();
  _index->reAlloc(nt+1); iP = _index->getPointer();
  copy(iP+siP[superIdx+1]+1, iP+nt, iP+siP[superIdx+1]+2);
  iP[siP[superIdx+1]+1] = iP[siP[superIdx+1]] + sz;
  for(int ii = siP[superIdx+1]+2; ii < nt+1; ii++)
    iP[ii] += sz;

  // _super_index
  for(int ii = superIdx+1; ii < (int)_super_index->getNbOfElems(); ii++)
    (siP[ii])++;
}

/**
 * Replace pack with absolute index 'idx' with the provided new pack. Function can be used either
 * for 2-level SkyLine or 3-level SkyLine.
 */
void MEDCouplingSkyLineArray::replaceSimplePack(const int idx, const int * packBg, const int * packEnd)
{
  validIndex("replaceSimplePack", idx);

  int * iP(_index->getPointer());
  int newSz = std::distance(packBg, packEnd);
  const int start = iP[idx], end = iP[idx+1];

  // _values
  int initValSz = _values->getNbOfElems();
  int deltaSz = newSz-(end-start);  // can be negative
  if (deltaSz)
    {
      if (deltaSz > 0)
        _values->reAlloc(initValSz+deltaSz);
      int *vP(_values->getPointer());
      std::copy(vP+end, vP+initValSz, vP+end+deltaSz);
      if (deltaSz < 0)
        _values->reAlloc(initValSz+deltaSz);
    }

  // copy new pack
  std::copy(packBg, packEnd, _values->getPointer()+start);

  // _index
  for(int ii = idx+1; ii < (int)_index->getNbOfElems(); ii++)
    iP[ii] += deltaSz;
}

/**
 * Replace pack with super index 'superIdx' and index 'idx' with the provided new pack.
 * Function can be used only for 3-level SkyLine.
 */
void MEDCouplingSkyLineArray::replacePack(const int superIdx, const int idx, const int *packBg, const int *packEnd)
{
  checkSuperIndex("replacePack");
  validSuperIndexAndIndex("replacePack", superIdx, idx);

  int * siP(_super_index->getPointer()), *iP(_index->getPointer());
  int newSz = std::distance(packBg, packEnd);
  const int start = iP[siP[superIdx]+idx], end = iP[siP[superIdx]+idx+1];

  // _values
  int initValSz = _values->getNbOfElems();
  int deltaSz = newSz-(end-start);  // can be negative
  if (deltaSz)
    {
      if (deltaSz > 0)
        _values->reAlloc(initValSz+deltaSz);
      int *vP(_values->getPointer());
      std::copy(vP+end, vP+initValSz, vP+end+deltaSz);
      if (deltaSz < 0)
        _values->reAlloc(initValSz+deltaSz);
    }

  // copy new pack
  std::copy(packBg, packEnd, _values->getPointer()+start);

  // _index
  for(int ii = siP[superIdx]+idx+1; ii < (int)_index->getNbOfElems(); ii++)
    iP[ii] += deltaSz;
}
