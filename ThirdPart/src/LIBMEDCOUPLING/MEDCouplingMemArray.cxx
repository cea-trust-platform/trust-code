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
// Author : Anthony Geay (EDF R&D)

#include "MEDCouplingMemArray.txx"

#include "BBTree.txx"
#include "GenMathFormulae.hxx"
#include "InterpKernelAutoPtr.hxx"
#include "InterpKernelExprParser.hxx"

#include "InterpKernelAutoPtr.hxx"
#include "InterpKernelGeo2DEdgeArcCircle.hxx"
#include "InterpKernelAutoPtr.hxx"
#include "InterpKernelGeo2DNode.hxx"
#include "InterpKernelGeo2DEdgeLin.hxx"

#include <set>
#include <cmath>
#include <limits>
#include <numeric>
#include <algorithm>
#include <functional>

typedef double (*MYFUNCPTR)(double);

using namespace MEDCoupling;

template class MEDCoupling::MemArray<int>;
template class MEDCoupling::MemArray<double>;
template class MEDCoupling::DataArrayTemplate<int>;
template class MEDCoupling::DataArrayTemplate<double>;
template class MEDCoupling::DataArrayTemplateClassic<int>;
template class MEDCoupling::DataArrayTemplateClassic<double>;
template class MEDCoupling::DataArrayTemplateFP<double>;
template class MEDCoupling::DataArrayIterator<double>;
template class MEDCoupling::DataArrayIterator<int>;
template class MEDCoupling::DataArrayDiscrete<Int32>;
template class MEDCoupling::DataArrayDiscreteSigned<Int32>;

template Traits<Int32>::ArrayType* MEDCoupling::DataArrayTuple<Int32>::buildDA(int, int) const;
template Traits<double>::ArrayType* MEDCoupling::DataArrayTuple<double>::buildDA(int, int) const;
template Traits<float>::ArrayType* MEDCoupling::DataArrayTuple<float>::buildDA(int, int) const;

template<int SPACEDIM>
void DataArrayDouble::findCommonTuplesAlg(const double *bbox, int nbNodes, int limitNodeId, double prec, DataArrayInt *c, DataArrayInt *cI) const
{
  const double *coordsPtr=getConstPointer();
  BBTreePts<SPACEDIM,int> myTree(bbox,0,0,nbNodes,prec);
  std::vector<bool> isDone(nbNodes);
  for(int i=0;i<nbNodes;i++)
    {
      if(!isDone[i])
        {
          std::vector<int> intersectingElems;
          myTree.getElementsAroundPoint(coordsPtr+i*SPACEDIM,intersectingElems);
          if(intersectingElems.size()>1)
            {
              std::vector<int> commonNodes;
              for(std::vector<int>::const_iterator it=intersectingElems.begin();it!=intersectingElems.end();it++)
                if(*it!=i)
                  if(*it>=limitNodeId)
                    {
                      commonNodes.push_back(*it);
                      isDone[*it]=true;
                    }
              if(!commonNodes.empty())
                {
                  cI->pushBackSilent(cI->back()+(int)commonNodes.size()+1);
                  c->pushBackSilent(i);
                  c->insertAtTheEnd(commonNodes.begin(),commonNodes.end());
                }
            }
        }
    }
}

template<int SPACEDIM>
void DataArrayDouble::FindTupleIdsNearTuplesAlg(const BBTreePts<SPACEDIM,int>& myTree, const double *pos, int nbOfTuples, double eps,
                                                DataArrayInt *c, DataArrayInt *cI)
{
  for(int i=0;i<nbOfTuples;i++)
    {
      std::vector<int> intersectingElems;
      myTree.getElementsAroundPoint(pos+i*SPACEDIM,intersectingElems);
      std::vector<int> commonNodes;
      for(std::vector<int>::const_iterator it=intersectingElems.begin();it!=intersectingElems.end();it++)
        commonNodes.push_back(*it);
      cI->pushBackSilent(cI->back()+(int)commonNodes.size());
      c->insertAtTheEnd(commonNodes.begin(),commonNodes.end());
    }
}

template<int SPACEDIM>
void DataArrayDouble::FindClosestTupleIdAlg(const BBTreePts<SPACEDIM,int>& myTree, double dist, const double *pos, int nbOfTuples, const double *thisPt, int thisNbOfTuples, int *res)
{
  // PL si dist nul (cas de nbOfTuples=1), alors boucle infinie plus bas
  if (dist==0) dist=1.e-12;
  double distOpt(dist);
  const double *p(pos);
  int *r(res);
  for(int i=0;i<nbOfTuples;i++,p+=SPACEDIM,r++)
    {
      while(true)
        {
          int elem=-1;
          double ret=myTree.getElementsAroundPoint2(p,distOpt,elem);
          if(ret!=std::numeric_limits<double>::max())
            {
              distOpt=std::max(ret,1e-4);
              *r=elem;
              break;
            }
          else
            { distOpt=2*distOpt; continue; }
        }
    }
}

int DataArray::EffectiveCircPerm(int nbOfShift, int nbOfTuples)
{
  if(nbOfTuples<=0)
    throw INTERP_KERNEL::Exception("DataArray::EffectiveCircPerm : number of tuples is expected to be > 0 !");
  if(nbOfShift>=0)
    {
      return nbOfShift%nbOfTuples;
    }
  else
    {
      int tmp(-nbOfShift);
      tmp=tmp%nbOfTuples;
      return nbOfTuples-tmp;
    }
}

std::size_t DataArray::getHeapMemorySizeWithoutChildren() const
{
  std::size_t sz1=_name.capacity();
  std::size_t sz2=_info_on_compo.capacity();
  std::size_t sz3=0;
  for(std::vector<std::string>::const_iterator it=_info_on_compo.begin();it!=_info_on_compo.end();it++)
    sz3+=(*it).capacity();
  return sz1+sz2+sz3;
}

std::vector<const BigMemoryObject *> DataArray::getDirectChildrenWithNull() const
{
  return std::vector<const BigMemoryObject *>();
}

/*!
 * Sets the attribute \a _name of \a this array.
 * See \ref MEDCouplingArrayBasicsName "DataArrays infos" for more information.
 *  \param [in] name - new array name
 */
void DataArray::setName(const std::string& name)
{
  _name=name;
}

/*!
 * Copies textual data from an \a other DataArray. The copied data are
 * - the name attribute,
 * - the information of components.
 *
 * For more information on these data see \ref MEDCouplingArrayBasicsName "DataArrays infos".
 *
 *  \param [in] other - another instance of DataArray to copy the textual data from.
 *  \throw If number of components of \a this array differs from that of the \a other.
 */
void DataArray::copyStringInfoFrom(const DataArray& other)
{
  if(_info_on_compo.size()!=other._info_on_compo.size())
    throw INTERP_KERNEL::Exception("Size of arrays mismatches on copyStringInfoFrom !");
  _name=other._name;
  _info_on_compo=other._info_on_compo;
}

void DataArray::copyPartOfStringInfoFrom(const DataArray& other, const std::vector<int>& compoIds)
{
  int nbOfCompoOth=other.getNumberOfComponents();
  std::size_t newNbOfCompo=compoIds.size();
  for(std::size_t i=0;i<newNbOfCompo;i++)
    if(compoIds[i]>=nbOfCompoOth || compoIds[i]<0)
      {
        std::ostringstream oss; oss << "Specified component id is out of range (" << compoIds[i] << ") compared with nb of actual components (" << nbOfCompoOth << ")";
        throw INTERP_KERNEL::Exception(oss.str().c_str());
      }
  for(std::size_t i=0;i<newNbOfCompo;i++)
    setInfoOnComponent((int)i,other.getInfoOnComponent(compoIds[i]));
}

void DataArray::copyPartOfStringInfoFrom2(const std::vector<int>& compoIds, const DataArray& other)
{
  std::size_t nbOfCompo(getNumberOfComponents());
  std::size_t partOfCompoToSet=compoIds.size();
  if(partOfCompoToSet!=other.getNumberOfComponents())
    throw INTERP_KERNEL::Exception("Given compoIds has not the same size as number of components of given array !");
  for(std::size_t i=0;i<partOfCompoToSet;i++)
    if(compoIds[i]>=(int)nbOfCompo || compoIds[i]<0)
      {
        std::ostringstream oss; oss << "Specified component id is out of range (" << compoIds[i] << ") compared with nb of actual components (" << nbOfCompo << ")";
        throw INTERP_KERNEL::Exception(oss.str().c_str());
      }
  for(std::size_t i=0;i<partOfCompoToSet;i++)
    setInfoOnComponent(compoIds[i],other.getInfoOnComponent((int)i));
}

bool DataArray::areInfoEqualsIfNotWhy(const DataArray& other, std::string& reason) const
{
  std::ostringstream oss;
  if(_name!=other._name)
    {
      oss << "Names DataArray mismatch : this name=\"" << _name << " other name=\"" << other._name << "\" !";
      reason=oss.str();
      return false;
    }
  if(_info_on_compo!=other._info_on_compo)
    {
      oss << "Components DataArray mismatch : \nThis components=";
      for(std::vector<std::string>::const_iterator it=_info_on_compo.begin();it!=_info_on_compo.end();it++)
        oss << "\"" << *it << "\",";
      oss << "\nOther components=";
      for(std::vector<std::string>::const_iterator it=other._info_on_compo.begin();it!=other._info_on_compo.end();it++)
        oss << "\"" << *it << "\",";
      reason=oss.str();
      return false;
    }
  return true;
}

/*!
 * Compares textual information of \a this DataArray with that of an \a other one.
 * The compared data are
 * - the name attribute,
 * - the information of components.
 *
 * For more information on these data see \ref MEDCouplingArrayBasicsName "DataArrays infos".
 *  \param [in] other - another instance of DataArray to compare the textual data of.
 *  \return bool - \a true if the textual information is same, \a false else.
 */
bool DataArray::areInfoEquals(const DataArray& other) const
{
  std::string tmp;
  return areInfoEqualsIfNotWhy(other,tmp);
}

void DataArray::reprWithoutNameStream(std::ostream& stream) const
{
  stream << "Number of components : "<< getNumberOfComponents() << "\n";
  stream << "Info of these components : ";
  for(std::vector<std::string>::const_iterator iter=_info_on_compo.begin();iter!=_info_on_compo.end();iter++)
    stream << "\"" << *iter << "\"   ";
  stream << "\n";
}

std::string DataArray::cppRepr(const std::string& varName) const
{
  std::ostringstream ret;
  reprCppStream(varName,ret);
  return ret.str();
}

/*!
 * Sets information on all components. To know more on format of this information
 * see \ref MEDCouplingArrayBasicsCompoName "DataArrays infos".
 *  \param [in] info - a vector of strings.
 *  \throw If size of \a info differs from the number of components of \a this.
 */
void DataArray::setInfoOnComponents(const std::vector<std::string>& info)
{
  if(getNumberOfComponents()!=info.size())
    {
      std::ostringstream oss; oss << "DataArray::setInfoOnComponents : input is of size " << info.size() << " whereas number of components is equal to " << getNumberOfComponents() << " !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  _info_on_compo=info;
}

/*!
 * This method is only a dispatcher towards DataArrayDouble::setPartOfValues3, DataArrayInt::setPartOfValues3, DataArrayChar::setPartOfValues3 depending on the true
 * type of \a this and \a aBase.
 *
 * \throw If \a aBase and \a this do not have the same type.
 *
 * \sa DataArrayDouble::setPartOfValues3, DataArrayInt::setPartOfValues3, DataArrayChar::setPartOfValues3.
 */
void DataArray::setPartOfValuesBase3(const DataArray *aBase, const int *bgTuples, const int *endTuples, int bgComp, int endComp, int stepComp, bool strictCompoCompare)
{
  if(!aBase)
    throw INTERP_KERNEL::Exception("DataArray::setPartOfValuesBase3 : input aBase object is NULL !");
  DataArrayDouble *this1(dynamic_cast<DataArrayDouble *>(this));
  DataArrayInt *this2(dynamic_cast<DataArrayInt *>(this));
  DataArrayChar *this3(dynamic_cast<DataArrayChar *>(this));
  const DataArrayDouble *a1(dynamic_cast<const DataArrayDouble *>(aBase));
  const DataArrayInt *a2(dynamic_cast<const DataArrayInt *>(aBase));
  const DataArrayChar *a3(dynamic_cast<const DataArrayChar *>(aBase));
  if(this1 && a1)
    {
      this1->setPartOfValues3(a1,bgTuples,endTuples,bgComp,endComp,stepComp,strictCompoCompare);
      return ;
    }
  if(this2 && a2)
    {
      this2->setPartOfValues3(a2,bgTuples,endTuples,bgComp,endComp,stepComp,strictCompoCompare);
      return ;
    }
  if(this3 && a3)
    {
      this3->setPartOfValues3(a3,bgTuples,endTuples,bgComp,endComp,stepComp,strictCompoCompare);
      return ;
    }
  throw INTERP_KERNEL::Exception("DataArray::setPartOfValuesBase3 : input aBase object and this do not have the same type !");
}

std::vector<std::string> DataArray::getVarsOnComponent() const
{
  int nbOfCompo=(int)_info_on_compo.size();
  std::vector<std::string> ret(nbOfCompo);
  for(int i=0;i<nbOfCompo;i++)
    ret[i]=getVarOnComponent(i);
  return ret;
}

std::vector<std::string> DataArray::getUnitsOnComponent() const
{
  int nbOfCompo=(int)_info_on_compo.size();
  std::vector<std::string> ret(nbOfCompo);
  for(int i=0;i<nbOfCompo;i++)
    ret[i]=getUnitOnComponent(i);
  return ret;
}

/*!
 * Returns information on a component specified by an index.
 * To know more on format of this information
 * see \ref MEDCouplingArrayBasicsCompoName "DataArrays infos".
 *  \param [in] i - the index (zero based) of the component of interest.
 *  \return std::string - a string containing the information on \a i-th component.
 *  \throw If \a i is not a valid component index.
 */
std::string DataArray::getInfoOnComponent(int i) const
{
  if(i<(int)_info_on_compo.size() && i>=0)
    return _info_on_compo[i];
  else
    {
      std::ostringstream oss; oss << "DataArray::getInfoOnComponent : Specified component id is out of range (" << i << ") compared with nb of actual components (" << (int) _info_on_compo.size();
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
}

/*!
 * Returns the var part of the full information of the \a i-th component.
 * For example, if \c getInfoOnComponent(0) returns "SIGXY [N/m^2]", then
 * \c getVarOnComponent(0) returns "SIGXY".
 * If a unit part of information is not detected by presence of
 * two square brackets, then the full information is returned.
 * To read more about the component information format, see
 * \ref MEDCouplingArrayBasicsCompoName "DataArrays infos".
 *  \param [in] i - the index (zero based) of the component of interest.
 *  \return std::string - a string containing the var information, or the full info.
 *  \throw If \a i is not a valid component index.
 */
std::string DataArray::getVarOnComponent(int i) const
{
  if(i<(int)_info_on_compo.size() && i>=0)
    {
      return GetVarNameFromInfo(_info_on_compo[i]);
    }
  else
    {
      std::ostringstream oss; oss << "DataArray::getVarOnComponent : Specified component id is out of range  (" << i << ") compared with nb of actual components (" << (int) _info_on_compo.size();
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
}

/*!
 * Returns the unit part of the full information of the \a i-th component.
 * For example, if \c getInfoOnComponent(0) returns "SIGXY [ N/m^2]", then
 * \c getUnitOnComponent(0) returns " N/m^2".
 * If a unit part of information is not detected by presence of
 * two square brackets, then an empty string is returned.
 * To read more about the component information format, see
 * \ref MEDCouplingArrayBasicsCompoName "DataArrays infos".
 *  \param [in] i - the index (zero based) of the component of interest.
 *  \return std::string - a string containing the unit information, if any, or "".
 *  \throw If \a i is not a valid component index.
 */
std::string DataArray::getUnitOnComponent(int i) const
{
  if(i<(int)_info_on_compo.size() && i>=0)
    {
      return GetUnitFromInfo(_info_on_compo[i]);
    }
  else
    {
      std::ostringstream oss; oss << "DataArray::getUnitOnComponent : Specified component id is out of range  (" << i << ") compared with nb of actual components (" << (int) _info_on_compo.size();
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
}

/*!
 * Returns the var part of the full component information.
 * For example, if \a info == "SIGXY [N/m^2]", then this method returns "SIGXY".
 * If a unit part of information is not detected by presence of
 * two square brackets, then the whole \a info is returned.
 * To read more about the component information format, see
 * \ref MEDCouplingArrayBasicsCompoName "DataArrays infos".
 *  \param [in] info - the full component information.
 *  \return std::string - a string containing only var information, or the \a info.
 */
std::string DataArray::GetVarNameFromInfo(const std::string& info)
{
  std::size_t p1=info.find_last_of('[');
  std::size_t p2=info.find_last_of(']');
  if(p1==std::string::npos || p2==std::string::npos)
    return info;
  if(p1>p2)
    return info;
  if(p1==0)
    return std::string();
  std::size_t p3=info.find_last_not_of(' ',p1-1);
  return info.substr(0,p3+1);
}

/*!
 * Returns the unit part of the full component information.
 * For example, if \a info == "SIGXY [ N/m^2]", then this method returns " N/m^2".
 * If a unit part of information is not detected by presence of
 * two square brackets, then an empty string is returned.
 * To read more about the component information format, see
 * \ref MEDCouplingArrayBasicsCompoName "DataArrays infos".
 *  \param [in] info - the full component information.
 *  \return std::string - a string containing only unit information, if any, or "".
 */
std::string DataArray::GetUnitFromInfo(const std::string& info)
{
  std::size_t p1=info.find_last_of('[');
  std::size_t p2=info.find_last_of(']');
  if(p1==std::string::npos || p2==std::string::npos)
    return std::string();
  if(p1>p2)
    return std::string();
  return info.substr(p1+1,p2-p1-1);
}

/*!
 * This method put in info format the result of the merge of \a var and \a unit.
 * The standard format for that is "var [unit]".
 * Inversely you can retrieve the var part or the unit part of info string using resp. GetVarNameFromInfo and GetUnitFromInfo.
 */
std::string DataArray::BuildInfoFromVarAndUnit(const std::string& var, const std::string& unit)
{
  std::ostringstream oss;
  oss << var << " [" << unit << "]";
  return oss.str();
}

std::string DataArray::GetAxisTypeRepr(MEDCouplingAxisType at)
{
  switch(at)
    {
    case AX_CART:
      return std::string("AX_CART");
    case AX_CYL:
      return std::string("AX_CYL");
    case AX_SPHER:
      return std::string("AX_SPHER");
    default:
      throw INTERP_KERNEL::Exception("DataArray::GetAxisTypeRepr : unrecognized axis type enum !");
    }
}

/*!
 * Returns a new DataArray by concatenating all given arrays, so that (1) the number
 * of tuples in the result array is a sum of the number of tuples of given arrays and (2)
 * the number of component in the result array is same as that of each of given arrays.
 * Info on components is copied from the first of the given arrays. Number of components
 * in the given arrays must be  the same.
 *  \param [in] arrs - a sequence of arrays to include in the result array. All arrays must have the same type.
 *  \return DataArray * - the new instance of DataArray (that can be either DataArrayInt, DataArrayDouble, DataArrayChar).
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If all arrays within \a arrs are NULL.
 *  \throw If all not null arrays in \a arrs have not the same type.
 *  \throw If getNumberOfComponents() of arrays within \a arrs.
 */
DataArray *DataArray::Aggregate(const std::vector<const DataArray *>& arrs)
{
  std::vector<const DataArray *> arr2;
  for(std::vector<const DataArray *>::const_iterator it=arrs.begin();it!=arrs.end();it++)
    if(*it)
      arr2.push_back(*it);
  if(arr2.empty())
    throw INTERP_KERNEL::Exception("DataArray::Aggregate : only null instance in input vector !");
  std::vector<const DataArrayDouble *> arrd;
  std::vector<const DataArrayInt *> arri;
  std::vector<const DataArrayChar *> arrc;
  for(std::vector<const DataArray *>::const_iterator it=arr2.begin();it!=arr2.end();it++)
    {
      const DataArrayDouble *a=dynamic_cast<const DataArrayDouble *>(*it);
      if(a)
        { arrd.push_back(a); continue; }
      const DataArrayInt *b=dynamic_cast<const DataArrayInt *>(*it);
      if(b)
        { arri.push_back(b); continue; }
      const DataArrayChar *c=dynamic_cast<const DataArrayChar *>(*it);
      if(c)
        { arrc.push_back(c); continue; }
      throw INTERP_KERNEL::Exception("DataArray::Aggregate : presence of not null instance in inuput that is not in [DataArrayDouble, DataArrayInt, DataArrayChar] !");
    }
  if(arr2.size()==arrd.size())
    return DataArrayDouble::Aggregate(arrd);
  if(arr2.size()==arri.size())
    return DataArrayInt::Aggregate(arri);
  if(arr2.size()==arrc.size())
    return DataArrayChar::Aggregate(arrc);
  throw INTERP_KERNEL::Exception("DataArray::Aggregate : all input arrays must have the same type !");
}

/*!
 * Sets information on a component specified by an index.
 * To know more on format of this information
 * see \ref MEDCouplingArrayBasicsCompoName "DataArrays infos".
 *  \warning Don't pass NULL as \a info!
 *  \param [in] i - the index (zero based) of the component of interest.
 *  \param [in] info - the string containing the information.
 *  \throw If \a i is not a valid component index.
 */
void DataArray::setInfoOnComponent(int i, const std::string& info)
{
  if(i<(int)_info_on_compo.size() && i>=0)
    _info_on_compo[i]=info;
  else
    {
      std::ostringstream oss; oss << "DataArray::setInfoOnComponent : Specified component id is out of range  (" << i << ") compared with nb of actual components (" << (int) _info_on_compo.size();
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
}

/*!
 * Sets information on all components. This method can change number of components
 * at certain conditions; if the conditions are not respected, an exception is thrown.
 * The number of components can be changed in \a this only if \a this is not allocated.
 * The condition of number of components must not be changed.
 *
 * To know more on format of the component information see
 * \ref MEDCouplingArrayBasicsCompoName "DataArrays infos".
 *  \param [in] info - a vector of component infos.
 *  \throw If \a this->getNumberOfComponents() != \a info.size() && \a this->isAllocated()
 */
void DataArray::setInfoAndChangeNbOfCompo(const std::vector<std::string>& info)
{
  if(getNumberOfComponents()!=info.size())
    {
      if(!isAllocated())
        _info_on_compo=info;
      else
        {
          std::ostringstream oss; oss << "DataArray::setInfoAndChangeNbOfCompo : input is of size " << info.size() << " whereas number of components is equal to " << getNumberOfComponents() << "  and this is already allocated !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  else
    _info_on_compo=info;
}

void DataArray::checkNbOfTuples(int nbOfTuples, const std::string& msg) const
{
  if((int)getNumberOfTuples()!=nbOfTuples)
    {
      std::ostringstream oss; oss << msg << " : mismatch number of tuples : expected " <<  nbOfTuples << " having " << getNumberOfTuples() << " !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
}

void DataArray::checkNbOfComps(int nbOfCompo, const std::string& msg) const
{
  if((int)getNumberOfComponents()!=nbOfCompo)
    {
      std::ostringstream oss; oss << msg << " : mismatch number of components : expected " << nbOfCompo << " having " << getNumberOfComponents() << " !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
}

void DataArray::checkNbOfElems(std::size_t nbOfElems, const std::string& msg) const
{
  if(getNbOfElems()!=nbOfElems)
    {
      std::ostringstream oss; oss << msg << " : mismatch number of elems : Expected " << nbOfElems << " having " << getNbOfElems() << " !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
}

void DataArray::checkNbOfTuplesAndComp(const DataArray& other, const std::string& msg) const
{
  if(getNumberOfTuples()!=other.getNumberOfTuples())
    {
      std::ostringstream oss; oss << msg << " : mismatch number of tuples : expected " <<  other.getNumberOfTuples() << " having " << getNumberOfTuples() << " !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  if(getNumberOfComponents()!=other.getNumberOfComponents())
    {
      std::ostringstream oss; oss << msg << " : mismatch number of components : expected " << other.getNumberOfComponents() << " having " << getNumberOfComponents() << " !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
}

void DataArray::checkNbOfTuplesAndComp(int nbOfTuples, int nbOfCompo, const std::string& msg) const
{
  checkNbOfTuples(nbOfTuples,msg);
  checkNbOfComps(nbOfCompo,msg);
}

/*!
 * Simply this method checks that \b value is in [0,\b ref).
 */
void DataArray::CheckValueInRange(int ref, int value, const std::string& msg)
{
  if(value<0 || value>=ref)
    {
      std::ostringstream oss; oss << "DataArray::CheckValueInRange : " << msg  << " ! Expected in range [0," << ref << "[ having " << value << " !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
}

/*!
 * This method checks that [\b start, \b end) is compliant with ref length \b value.
 * typically start in [0,\b value) and end in [0,\b value). If value==start and start==end, it is supported.
 */
void DataArray::CheckValueInRangeEx(int value, int start, int end, const std::string& msg)
{
  if(start<0 || start>=value)
    {
      if(value!=start || end!=start)
        {
          std::ostringstream oss; oss << "DataArray::CheckValueInRangeEx : " << msg  << " ! Expected start " << start << " of input range, in [0," << value << "[ !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  if(end<0 || end>value)
    {
      std::ostringstream oss; oss << "DataArray::CheckValueInRangeEx : " << msg  << " ! Expected end " << end << " of input range, in [0," << value << "] !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
}

void DataArray::CheckClosingParInRange(int ref, int value, const std::string& msg)
{
  if(value<0 || value>ref)
    {
      std::ostringstream oss; oss << "DataArray::CheckClosingParInRange : " << msg  << " ! Expected input range in [0," << ref << "] having closing open parenthesis " << value << " !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
}

/*!
 * This method is useful to slice work among a pool of threads or processes. \a begin, \a end \a step is the input whole slice of work to perform, 
 * typically it is a whole slice of tuples of DataArray or cells, nodes of a mesh...
 *
 * The input \a sliceId should be an id in [0, \a nbOfSlices) that specifies the slice of work.
 *
 * \param [in] start - the start of the input slice of the whole work to perform split into slices.
 * \param [in] stop - the stop of the input slice of the whole work to perform split into slices.
 * \param [in] step - the step (that can be <0) of the input slice of the whole work to perform split into slices.
 * \param [in] sliceId - the slice id considered
 * \param [in] nbOfSlices - the number of slices (typically the number of cores on which the work is expected to be sliced)
 * \param [out] startSlice - the start of the slice considered
 * \param [out] stopSlice - the stop of the slice consided
 * 
 * \throw If \a step == 0
 * \throw If \a nbOfSlices not > 0
 * \throw If \a sliceId not in [0,nbOfSlices)
 */
void DataArray::GetSlice(int start, int stop, int step, int sliceId, int nbOfSlices, int& startSlice, int& stopSlice)
{
  if(nbOfSlices<=0)
    {
      std::ostringstream oss; oss << "DataArray::GetSlice : nbOfSlices (" << nbOfSlices << ") must be > 0 !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  if(sliceId<0 || sliceId>=nbOfSlices)
    {
      std::ostringstream oss; oss << "DataArray::GetSlice : sliceId (" << nbOfSlices << ") must be in [0 , nbOfSlices (" << nbOfSlices << ") ) !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  int nbElems=GetNumberOfItemGivenBESRelative(start,stop,step,"DataArray::GetSlice");
  int minNbOfElemsPerSlice=nbElems/nbOfSlices;
  startSlice=start+minNbOfElemsPerSlice*step*sliceId;
  if(sliceId<nbOfSlices-1)
    stopSlice=start+minNbOfElemsPerSlice*step*(sliceId+1);
  else
    stopSlice=stop;
}

int DataArray::GetNumberOfItemGivenBES(int begin, int end, int step, const std::string& msg)
{
  if(end<begin)
    {
      std::ostringstream oss; oss << msg << " : end before begin !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  if(end==begin)
    return 0;
  if(step<=0)
    {
      std::ostringstream oss; oss << msg << " : invalid step should be > 0 !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  return (end-1-begin)/step+1;
}

int DataArray::GetNumberOfItemGivenBESRelative(int begin, int end, int step, const std::string& msg)
{
  if(step==0)
    throw INTERP_KERNEL::Exception("DataArray::GetNumberOfItemGivenBES : step=0 is not allowed !");
  if(end<begin && step>0)
    {
      std::ostringstream oss; oss << msg << " : end before begin whereas step is positive !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  if(begin<end && step<0)
    {
      std::ostringstream oss; oss << msg << " : invalid step should be > 0 !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  if(begin!=end)
    return (std::max(begin,end)-1-std::min(begin,end))/std::abs(step)+1;
  else
    return 0;
}

int DataArray::GetPosOfItemGivenBESRelativeNoThrow(int value, int begin, int end, int step)
{
  if(step!=0)
    {
      if(step>0)
        {
          if(begin<=value && value<end)
            {
              if((value-begin)%step==0)
                return (value-begin)/step;
              else
                return -1;
            }
          else
            return -1;
        }
      else
        {
          if(begin>=value && value>end)
            {
              if((begin-value)%(-step)==0)
                return (begin-value)/(-step);
              else
                return -1;
            }
          else
            return -1;
        }
    }
  else
    return -1;
}

/*!
 * Returns a new instance of DataArrayDouble. The caller is to delete this array
 * using decrRef() as it is no more needed. 
 */
DataArrayDouble *DataArrayDouble::New()
{
  return new DataArrayDouble;
}

/*!
 * Returns the only one value in \a this, if and only if number of elements
 * (nb of tuples * nb of components) is equal to 1, and that \a this is allocated.
 *  \return double - the sole value stored in \a this array.
 *  \throw If at least one of conditions stated above is not fulfilled.
 */
double DataArrayDouble::doubleValue() const
{
  if(isAllocated())
    {
      if(getNbOfElems()==1)
        {
          return *getConstPointer();
        }
      else
        throw INTERP_KERNEL::Exception("DataArrayDouble::doubleValue : DataArrayDouble instance is allocated but number of elements is not equal to 1 !");
    }
  else
    throw INTERP_KERNEL::Exception("DataArrayDouble::doubleValue : DataArrayDouble instance is not allocated !");
}

/*!
 * Returns a full copy of \a this. For more info on copying data arrays see
 * \ref MEDCouplingArrayBasicsCopyDeep.
 *  \return DataArrayDouble * - a new instance of DataArrayDouble. The caller is to
 *          delete this array using decrRef() as it is no more needed. 
 */
DataArrayDouble *DataArrayDouble::deepCopy() const
{
  return new DataArrayDouble(*this);
}

/*!
 * Checks that \a this array is consistently **increasing** or **decreasing** in value,
 * with at least absolute difference value of |\a eps| at each step.
 * If not an exception is thrown.
 *  \param [in] increasing - if \a true, the array values should be increasing.
 *  \param [in] eps - minimal absolute difference between the neighbor values at which 
 *                    the values are considered different.
 *  \throw If sequence of values is not strictly monotonic in agreement with \a
 *         increasing arg.
 *  \throw If \a this->getNumberOfComponents() != 1.
 *  \throw If \a this is not allocated.
 */
void DataArrayDouble::checkMonotonic(bool increasing, double eps) const
{
  if(!isMonotonic(increasing,eps))
    {
      if (increasing)
        throw INTERP_KERNEL::Exception("DataArrayDouble::checkMonotonic : 'this' is not INCREASING monotonic !");
      else
        throw INTERP_KERNEL::Exception("DataArrayDouble::checkMonotonic : 'this' is not DECREASING monotonic !");
    }
}

/*!
 * Checks that \a this array is consistently **increasing** or **decreasing** in value,
 * with at least absolute difference value of |\a eps| at each step.
 *  \param [in] increasing - if \a true, array values should be increasing.
 *  \param [in] eps - minimal absolute difference between the neighbor values at which 
 *                    the values are considered different.
 *  \return bool - \a true if values change in accordance with \a increasing arg.
 *  \throw If \a this->getNumberOfComponents() != 1.
 *  \throw If \a this is not allocated.
 */
bool DataArrayDouble::isMonotonic(bool increasing, double eps) const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayDouble::isMonotonic : only supported with 'this' array with ONE component !");
  int nbOfElements=getNumberOfTuples();
  const double *ptr=getConstPointer();
  if(nbOfElements==0)
    return true;
  double ref=ptr[0];
  double absEps=fabs(eps);
  if(increasing)
    {
      for(int i=1;i<nbOfElements;i++)
        {
          if(ptr[i]<(ref+absEps))
            return false;
          ref=ptr[i];
        }
      return true;
    }
  else
    {
      for(int i=1;i<nbOfElements;i++)
        {
          if(ptr[i]>(ref-absEps))
            return false;
          ref=ptr[i];
        }
      return true;
    }
}

/*!
 * Returns a textual and human readable representation of \a this instance of
 * DataArrayDouble. This text is shown when a DataArrayDouble is printed in Python.
 * \return std::string - text describing \a this DataArrayDouble.
 *
 * \sa reprNotTooLong, reprZip
 */
std::string DataArrayDouble::repr() const
{
  std::ostringstream ret;
  reprStream(ret);
  return ret.str();
}

std::string DataArrayDouble::reprZip() const
{
  std::ostringstream ret;
  reprZipStream(ret);
  return ret.str();
}

void DataArrayDouble::writeVTK(std::ostream& ofs, int indent, const std::string& nameInFile, DataArrayByte *byteArr) const
{
  static const char SPACE[4]={' ',' ',' ',' '};
  checkAllocated();
  std::string idt(indent,' ');
  ofs.precision(17);
  ofs << idt << "<DataArray type=\"Float32\" Name=\"" << nameInFile << "\" NumberOfComponents=\"" << getNumberOfComponents() << "\"";
  //
  bool areAllEmpty(true);
  for(std::vector<std::string>::const_iterator it=_info_on_compo.begin();it!=_info_on_compo.end();it++)
    if(!(*it).empty())
      areAllEmpty=false;
  if(!areAllEmpty)
    for(std::size_t i=0;i<_info_on_compo.size();i++)
      ofs << " ComponentName" << i << "=\"" << _info_on_compo[i] << "\"";
  //
  if(byteArr)
    {
      ofs << " format=\"appended\" offset=\"" << byteArr->getNumberOfTuples() << "\">";
      INTERP_KERNEL::AutoPtr<float> tmp(new float[getNbOfElems()]);
      float *pt(tmp);
      // to make Visual C++ happy : instead of std::copy(begin(),end(),(float *)tmp);
      for(const double *src=begin();src!=end();src++,pt++)
        *pt=float(*src);
      const char *data(reinterpret_cast<const char *>((float *)tmp));
      std::size_t sz(getNbOfElems()*sizeof(float));
      byteArr->insertAtTheEnd(data,data+sz);
      byteArr->insertAtTheEnd(SPACE,SPACE+4);
    }
  else
    {
      ofs << " RangeMin=\"" << getMinValueInArray() << "\" RangeMax=\"" << getMaxValueInArray() << "\" format=\"ascii\">\n" << idt;
      std::copy(begin(),end(),std::ostream_iterator<double>(ofs," "));
    }
  ofs << std::endl << idt << "</DataArray>\n";
}

void DataArrayDouble::reprCppStream(const std::string& varName, std::ostream& stream) const
{
  int nbTuples(getNumberOfTuples()),nbComp(getNumberOfComponents());
  const double *data(getConstPointer());
  stream.precision(17);
  stream << "DataArrayDouble *" << varName << "=DataArrayDouble::New();" << std::endl;
  if(nbTuples*nbComp>=1)
    {
      stream << "const double " << varName << "Data[" << nbTuples*nbComp << "]={";
      std::copy(data,data+nbTuples*nbComp-1,std::ostream_iterator<double>(stream,","));
      stream << data[nbTuples*nbComp-1] << "};" << std::endl;
      stream << varName << "->useArray(" << varName << "Data,false,CPP_DEALLOC," << nbTuples << "," << nbComp << ");" << std::endl;
    }
  else
    stream << varName << "->alloc(" << nbTuples << "," << nbComp << ");" << std::endl;
  stream << varName << "->setName(\"" << getName() << "\");" << std::endl;
}

/*!
 * Method that gives a quick overvien of \a this for python.
 */
void DataArrayDouble::reprQuickOverview(std::ostream& stream) const
{
  static const std::size_t MAX_NB_OF_BYTE_IN_REPR=300;
  stream << "DataArrayDouble C++ instance at " << this << ". ";
  if(isAllocated())
    {
      int nbOfCompo=(int)_info_on_compo.size();
      if(nbOfCompo>=1)
        {
          int nbOfTuples=getNumberOfTuples();
          stream << "Number of tuples : " << nbOfTuples << ". Number of components : " << nbOfCompo << "." << std::endl;
          reprQuickOverviewData(stream,MAX_NB_OF_BYTE_IN_REPR);
        }
      else
        stream << "Number of components : 0.";
    }
  else
    stream << "*** No data allocated ****";
}

void DataArrayDouble::reprQuickOverviewData(std::ostream& stream, std::size_t maxNbOfByteInRepr) const
{
  const double *data=begin();
  int nbOfTuples=getNumberOfTuples();
  int nbOfCompo=(int)_info_on_compo.size();
  std::ostringstream oss2; oss2 << "[";
  oss2.precision(17);
  std::string oss2Str(oss2.str());
  bool isFinished=true;
  for(int i=0;i<nbOfTuples && isFinished;i++)
    {
      if(nbOfCompo>1)
        {
          oss2 << "(";
          for(int j=0;j<nbOfCompo;j++,data++)
            {
              oss2 << *data;
              if(j!=nbOfCompo-1) oss2 << ", ";
            }
          oss2 << ")";
        }
      else
        oss2 << *data++;
      if(i!=nbOfTuples-1) oss2 << ", ";
      std::string oss3Str(oss2.str());
      if(oss3Str.length()<maxNbOfByteInRepr)
        oss2Str=oss3Str;
      else
        isFinished=false;
    }
  stream << oss2Str;
  if(!isFinished)
    stream << "... ";
  stream << "]";
}

/*!
 * Equivalent to DataArrayDouble::isEqual except that if false the reason of
 * mismatch is given.
 * 
 * \param [in] other the instance to be compared with \a this
 * \param [in] prec the precision to compare numeric data of the arrays.
 * \param [out] reason In case of inequality returns the reason.
 * \sa DataArrayDouble::isEqual
 */
bool DataArrayDouble::isEqualIfNotWhy(const DataArrayDouble& other, double prec, std::string& reason) const
{
  if(!areInfoEqualsIfNotWhy(other,reason))
    return false;
  return _mem.isEqual(other._mem,prec,reason);
}

/*!
 * Checks if \a this and another DataArrayDouble are fully equal. For more info see
 * \ref MEDCouplingArrayBasicsCompare.
 *  \param [in] other - an instance of DataArrayDouble to compare with \a this one.
 *  \param [in] prec - precision value to compare numeric data of the arrays.
 *  \return bool - \a true if the two arrays are equal, \a false else.
 */
bool DataArrayDouble::isEqual(const DataArrayDouble& other, double prec) const
{
  std::string tmp;
  return isEqualIfNotWhy(other,prec,tmp);
}

/*!
 * Checks if values of \a this and another DataArrayDouble are equal. For more info see
 * \ref MEDCouplingArrayBasicsCompare.
 *  \param [in] other - an instance of DataArrayDouble to compare with \a this one.
 *  \param [in] prec - precision value to compare numeric data of the arrays.
 *  \return bool - \a true if the values of two arrays are equal, \a false else.
 */
bool DataArrayDouble::isEqualWithoutConsideringStr(const DataArrayDouble& other, double prec) const
{
  std::string tmp;
  return _mem.isEqual(other._mem,prec,tmp);
}

/*!
 * This method checks that all tuples in \a other are in \a this.
 * If true, the output param \a tupleIds contains the tuples ids of \a this that correspond to tupes in \a this.
 * For each i in [ 0 , other->getNumberOfTuples() ) tuple #i in \a other is equal ( regarding input precision \a prec ) to tuple tupleIds[i] in \a this.
 *
 * \param [in] other - the array having the same number of components than \a this.
 * \param [out] tupleIds - the tuple ids containing the same number of tuples than \a other has.
 * \sa DataArrayDouble::findCommonTuples
 */
bool DataArrayDouble::areIncludedInMe(const DataArrayDouble *other, double prec, DataArrayInt *&tupleIds) const
{
  if(!other)
    throw INTERP_KERNEL::Exception("DataArrayDouble::areIncludedInMe : input array is NULL !");
  checkAllocated(); other->checkAllocated();
  if(getNumberOfComponents()!=other->getNumberOfComponents())
    throw INTERP_KERNEL::Exception("DataArrayDouble::areIncludedInMe : the number of components does not match !");
  MCAuto<DataArrayDouble> a=DataArrayDouble::Aggregate(this,other);
  DataArrayInt *c=0,*ci=0;
  a->findCommonTuples(prec,getNumberOfTuples(),c,ci);
  MCAuto<DataArrayInt> cSafe(c),ciSafe(ci);
  int newNbOfTuples=-1;
  MCAuto<DataArrayInt> ids=DataArrayInt::ConvertIndexArrayToO2N(a->getNumberOfTuples(),c->begin(),ci->begin(),ci->end(),newNbOfTuples);
  MCAuto<DataArrayInt> ret1=ids->selectByTupleIdSafeSlice(getNumberOfTuples(),a->getNumberOfTuples(),1);
  tupleIds=ret1.retn();
  return newNbOfTuples==getNumberOfTuples();
}

/*!
 * Searches for tuples coincident within \a prec tolerance. Each tuple is considered
 * as coordinates of a point in getNumberOfComponents()-dimensional space. The
 * distance separating two points is computed with the infinite norm.
 *
 * Indices of coincident tuples are stored in output arrays.
 * A pair of arrays (\a comm, \a commIndex) is called "Surjective Format 2".
 *
 * This method is typically used by MEDCouplingPointSet::findCommonNodes() and
 * MEDCouplingUMesh::mergeNodes().
 *  \param [in] prec - minimal absolute distance between two tuples (infinite norm) at which they are
 *              considered not coincident.
 *  \param [in] limitTupleId - limit tuple id. If all tuples within a group of coincident
 *              tuples have id strictly lower than \a limitTupleId then they are not returned.
 *  \param [out] comm - the array holding ids (== indices) of coincident tuples. 
 *               \a comm->getNumberOfComponents() == 1. 
 *               \a comm->getNumberOfTuples() == \a commIndex->back().
 *  \param [out] commIndex - the array dividing all indices stored in \a comm into
 *               groups of (indices of) coincident tuples. Its every value is a tuple
 *               index where a next group of tuples begins. For example the second
 *               group of tuples in \a comm is described by following range of indices:
 *               [ \a commIndex[1], \a commIndex[2] ). \a commIndex->getNumberOfTuples()-1
 *               gives the number of groups of coincident tuples.
 *  \throw If \a this is not allocated.
 *  \throw If the number of components is not in [1,2,3,4].
 *
 *  \if ENABLE_EXAMPLES
 *  \ref cpp_mcdataarraydouble_findcommontuples "Here is a C++ example".
 *
 *  \ref py_mcdataarraydouble_findcommontuples  "Here is a Python example".
 *  \endif
 *  \sa DataArrayInt::ConvertIndexArrayToO2N(), DataArrayDouble::areIncludedInMe
 */
void DataArrayDouble::findCommonTuples(double prec, int limitTupleId, DataArrayInt *&comm, DataArrayInt *&commIndex) const
{
  checkAllocated();
  int nbOfCompo=getNumberOfComponents();
  if ((nbOfCompo<1) || (nbOfCompo>4)) //test before work
    throw INTERP_KERNEL::Exception("DataArrayDouble::findCommonTuples : Unexpected spacedim of coords. Must be 1, 2, 3 or 4.");

  int nbOfTuples=getNumberOfTuples();
  //
  MCAuto<DataArrayInt> c(DataArrayInt::New()),cI(DataArrayInt::New()); c->alloc(0,1); cI->pushBackSilent(0);
  switch(nbOfCompo)
  {
    case 4:
      findCommonTuplesAlg<4>(begin(),nbOfTuples,limitTupleId,prec,c,cI);
      break;
    case 3:
      findCommonTuplesAlg<3>(begin(),nbOfTuples,limitTupleId,prec,c,cI);
      break;
    case 2:
      findCommonTuplesAlg<2>(begin(),nbOfTuples,limitTupleId,prec,c,cI);
      break;
    case 1:
      findCommonTuplesAlg<1>(begin(),nbOfTuples,limitTupleId,prec,c,cI);
      break;
    default:
      throw INTERP_KERNEL::Exception("DataArrayDouble::findCommonTuples : nb of components managed are 1,2,3 and 4 ! not implemented for other number of components !");
  }
  comm=c.retn();
  commIndex=cI.retn();
}

/*!
 * This methods returns the minimal distance between the two set of points \a this and \a other.
 * So \a this and \a other have to have the same number of components. If not an INTERP_KERNEL::Exception will be thrown.
 * This method works only if number of components of \a this (equal to those of \a other) is in 1, 2 or 3.
 *
 * \param [out] thisTupleId the tuple id in \a this corresponding to the returned minimal distance
 * \param [out] otherTupleId the tuple id in \a other corresponding to the returned minimal distance
 * \return the minimal distance between the two set of points \a this and \a other.
 * \sa DataArrayDouble::findClosestTupleId
 */
double DataArrayDouble::minimalDistanceTo(const DataArrayDouble *other, int& thisTupleId, int& otherTupleId) const
{
  MCAuto<DataArrayInt> part1=findClosestTupleId(other);
  int nbOfCompo(getNumberOfComponents());
  int otherNbTuples(other->getNumberOfTuples());
  const double *thisPt(begin()),*otherPt(other->begin());
  const int *part1Pt(part1->begin());
  double ret=std::numeric_limits<double>::max();
  for(int i=0;i<otherNbTuples;i++,part1Pt++,otherPt+=nbOfCompo)
    {
      double tmp(0.);
      for(int j=0;j<nbOfCompo;j++)
        tmp+=(otherPt[j]-thisPt[nbOfCompo*(*part1Pt)+j])*(otherPt[j]-thisPt[nbOfCompo*(*part1Pt)+j]);
      if(tmp<ret)
        { ret=tmp; thisTupleId=*part1Pt; otherTupleId=i; }
    }
  return sqrt(ret);
}

/*!
 * This methods returns for each tuple in \a other which tuple in \a this is the closest.
 * So \a this and \a other have to have the same number of components. If not an INTERP_KERNEL::Exception will be thrown.
 * This method works only if number of components of \a this (equal to those of \a other) is in 1, 2 or 3.
 *
 * \return a newly allocated (new object to be dealt by the caller) DataArrayInt having \c other->getNumberOfTuples() tuples and one components.
 * \sa DataArrayDouble::minimalDistanceTo
 */
DataArrayInt *DataArrayDouble::findClosestTupleId(const DataArrayDouble *other) const
{
  if(!other)
    throw INTERP_KERNEL::Exception("DataArrayDouble::findClosestTupleId : other instance is NULL !");
  checkAllocated(); other->checkAllocated();
  std::size_t nbOfCompo(getNumberOfComponents());
  if(nbOfCompo!=other->getNumberOfComponents())
    {
      std::ostringstream oss; oss << "DataArrayDouble::findClosestTupleId : number of components in this is " << nbOfCompo;
      oss << ", whereas number of components in other is " << other->getNumberOfComponents() << "! Should be equal !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  int nbOfTuples=other->getNumberOfTuples();
  int thisNbOfTuples=getNumberOfTuples();
  MCAuto<DataArrayInt> ret=DataArrayInt::New(); ret->alloc(nbOfTuples,1);
  double bounds[6];
  getMinMaxPerComponent(bounds);
  switch(nbOfCompo)
  {
    case 3:
      {
        double xDelta(fabs(bounds[1]-bounds[0])),yDelta(fabs(bounds[3]-bounds[2])),zDelta(fabs(bounds[5]-bounds[4]));
        double delta=std::max(xDelta,yDelta); delta=std::max(delta,zDelta);
        double characSize=pow((delta*delta*delta)/((double)thisNbOfTuples),1./3.);
        BBTreePts<3,int> myTree(begin(),0,0,getNumberOfTuples(),characSize*1e-12);
        FindClosestTupleIdAlg<3>(myTree,3.*characSize*characSize,other->begin(),nbOfTuples,begin(),thisNbOfTuples,ret->getPointer());
        break;
      }
    case 2:
      {
        double xDelta(fabs(bounds[1]-bounds[0])),yDelta(fabs(bounds[3]-bounds[2]));
        double delta=std::max(xDelta,yDelta);
        double characSize=sqrt(delta/(double)thisNbOfTuples);
        BBTreePts<2,int> myTree(begin(),0,0,getNumberOfTuples(),characSize*1e-12);
        FindClosestTupleIdAlg<2>(myTree,2.*characSize*characSize,other->begin(),nbOfTuples,begin(),thisNbOfTuples,ret->getPointer());
        break;
      }
    case 1:
      {
        double characSize=fabs(bounds[1]-bounds[0])/thisNbOfTuples;
        BBTreePts<1,int> myTree(begin(),0,0,getNumberOfTuples(),characSize*1e-12);
        FindClosestTupleIdAlg<1>(myTree,1.*characSize*characSize,other->begin(),nbOfTuples,begin(),thisNbOfTuples,ret->getPointer());
        break;
      }
    default:
      throw INTERP_KERNEL::Exception("Unexpected spacedim of coords for findClosestTupleId. Must be 1, 2 or 3.");
  }
  return ret.retn();
}

/*!
 * This method expects that \a this and \a otherBBoxFrmt arrays are bounding box arrays ( as the output of MEDCouplingPointSet::getBoundingBoxForBBTree method ).
 * This method will return a DataArrayInt array having the same number of tuples than \a this. This returned array tells for each cell in \a this
 * how many bounding boxes in \a otherBBoxFrmt.
 * So, this method expects that \a this and \a otherBBoxFrmt have the same number of components.
 *
 * \param [in] otherBBoxFrmt - It is an array .
 * \param [in] eps - the absolute precision of the detection. when eps < 0 the bboxes are enlarged so more interactions are detected. Inversely when > 0 the bboxes are stretched.
 * \sa MEDCouplingPointSet::getBoundingBoxForBBTree
 * \throw If \a this and \a otherBBoxFrmt have not the same number of components.
 * \throw If \a this and \a otherBBoxFrmt number of components is not even (BBox format).
 */
DataArrayInt *DataArrayDouble::computeNbOfInteractionsWith(const DataArrayDouble *otherBBoxFrmt, double eps) const
{
  if(!otherBBoxFrmt)
    throw INTERP_KERNEL::Exception("DataArrayDouble::computeNbOfInteractionsWith : input array is NULL !");
  if(!isAllocated() || !otherBBoxFrmt->isAllocated())
    throw INTERP_KERNEL::Exception("DataArrayDouble::computeNbOfInteractionsWith : this and input array must be allocated !");
  std::size_t nbOfComp(getNumberOfComponents()),nbOfTuples(getNumberOfTuples());
  if(nbOfComp!=otherBBoxFrmt->getNumberOfComponents())
    {
      std::ostringstream oss; oss << "DataArrayDouble::computeNbOfInteractionsWith : this number of components (" << nbOfComp << ") must be equal to the number of components of input array (" << otherBBoxFrmt->getNumberOfComponents() << ") !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  if(nbOfComp%2!=0)
    {
      std::ostringstream oss; oss << "DataArrayDouble::computeNbOfInteractionsWith : Number of components (" << nbOfComp << ") is not even ! It should be to be compatible with bbox format !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  MCAuto<DataArrayInt> ret(DataArrayInt::New()); ret->alloc(nbOfTuples,1);
  const double *thisBBPtr(begin());
  int *retPtr(ret->getPointer());
  switch(nbOfComp/2)
  {
    case 3:
      {
        BBTree<3,int> bbt(otherBBoxFrmt->begin(),0,0,otherBBoxFrmt->getNumberOfTuples(),eps);
        for(std::size_t i=0;i<nbOfTuples;i++,retPtr++,thisBBPtr+=nbOfComp)
          *retPtr=bbt.getNbOfIntersectingElems(thisBBPtr);
        break;
      }
    case 2:
      {
        BBTree<2,int> bbt(otherBBoxFrmt->begin(),0,0,otherBBoxFrmt->getNumberOfTuples(),eps);
        for(std::size_t i=0;i<nbOfTuples;i++,retPtr++,thisBBPtr+=nbOfComp)
          *retPtr=bbt.getNbOfIntersectingElems(thisBBPtr);
        break;
      }
    case 1:
      {
        BBTree<1,int> bbt(otherBBoxFrmt->begin(),0,0,otherBBoxFrmt->getNumberOfTuples(),eps);
        for(std::size_t i=0;i<nbOfTuples;i++,retPtr++,thisBBPtr+=nbOfComp)
          *retPtr=bbt.getNbOfIntersectingElems(thisBBPtr);
        break;
      }
    default:
      throw INTERP_KERNEL::Exception("DataArrayDouble::computeNbOfInteractionsWith : space dimension supported are [1,2,3] !");
  }

  return ret.retn();
}

/*!
 * Returns a copy of \a this array by excluding coincident tuples. Each tuple is
 * considered as coordinates of a point in getNumberOfComponents()-dimensional
 * space. The distance between tuples is computed using norm2. If several tuples are
 * not far each from other than \a prec, only one of them remains in the result
 * array. The order of tuples in the result array is same as in \a this one except
 * that coincident tuples are excluded.
 *  \param [in] prec - minimal absolute distance between two tuples at which they are
 *              considered not coincident.
 *  \param [in] limitTupleId - limit tuple id. If all tuples within a group of coincident
 *              tuples have id strictly lower than \a limitTupleId then they are not excluded.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble that the caller
 *          is to delete using decrRef() as it is no more needed.
 *  \throw If \a this is not allocated.
 *  \throw If the number of components is not in [1,2,3,4].
 *
 *  \if ENABLE_EXAMPLES
 *  \ref py_mcdataarraydouble_getdifferentvalues "Here is a Python example".
 *  \endif
 */
DataArrayDouble *DataArrayDouble::getDifferentValues(double prec, int limitTupleId) const
{
  checkAllocated();
  DataArrayInt *c0=0,*cI0=0;
  findCommonTuples(prec,limitTupleId,c0,cI0);
  MCAuto<DataArrayInt> c(c0),cI(cI0);
  int newNbOfTuples=-1;
  MCAuto<DataArrayInt> o2n=DataArrayInt::ConvertIndexArrayToO2N(getNumberOfTuples(),c0->begin(),cI0->begin(),cI0->end(),newNbOfTuples);
  return renumberAndReduce(o2n->getConstPointer(),newNbOfTuples);
}

/*!
 * Copy all components in a specified order from another DataArrayDouble.
 * Both numerical and textual data is copied. The number of tuples in \a this and
 * the other array can be different.
 *  \param [in] a - the array to copy data from.
 *  \param [in] compoIds - sequence of zero based indices of components, data of which is
 *              to be copied.
 *  \throw If \a a is NULL.
 *  \throw If \a compoIds.size() != \a a->getNumberOfComponents().
 *  \throw If \a compoIds[i] < 0 or \a compoIds[i] > \a this->getNumberOfComponents().
 *
 *  \if ENABLE_EXAMPLES
 *  \ref py_mcdataarraydouble_setselectedcomponents "Here is a Python example".
 *  \endif
 */
void DataArrayDouble::setSelectedComponents(const DataArrayDouble *a, const std::vector<int>& compoIds)
{
  if(!a)
    throw INTERP_KERNEL::Exception("DataArrayDouble::setSelectedComponents : input DataArrayDouble is NULL !");
  checkAllocated();
  copyPartOfStringInfoFrom2(compoIds,*a);
  std::size_t partOfCompoSz=compoIds.size();
  int nbOfCompo=getNumberOfComponents();
  int nbOfTuples=std::min(getNumberOfTuples(),a->getNumberOfTuples());
  const double *ac=a->getConstPointer();
  double *nc=getPointer();
  for(int i=0;i<nbOfTuples;i++)
    for(std::size_t j=0;j<partOfCompoSz;j++,ac++)
      nc[nbOfCompo*i+compoIds[j]]=*ac;
}

/*!
 * Checks if 0.0 value is present in \a this array. If it is the case, an exception
 * is thrown.
 * \throw If zero is found in \a this array.
 */
void DataArrayDouble::checkNoNullValues() const
{
  const double *tmp=getConstPointer();
  std::size_t nbOfElems=getNbOfElems();
  const double *where=std::find(tmp,tmp+nbOfElems,0.);
  if(where!=tmp+nbOfElems)
    throw INTERP_KERNEL::Exception("A value 0.0 have been detected !");
}

/*!
 * Computes minimal and maximal value in each component. An output array is filled
 * with \c 2 * \a this->getNumberOfComponents() values, so the caller is to allocate
 * enough memory before calling this method.
 *  \param [out] bounds - array of size at least 2 *\a this->getNumberOfComponents().
 *               It is filled as follows:<br>
 *               \a bounds[0] = \c min_of_component_0 <br>
 *               \a bounds[1] = \c max_of_component_0 <br>
 *               \a bounds[2] = \c min_of_component_1 <br>
 *               \a bounds[3] = \c max_of_component_1 <br>
 *               ...
 */
void DataArrayDouble::getMinMaxPerComponent(double *bounds) const
{
  checkAllocated();
  int dim=getNumberOfComponents();
  for (int idim=0; idim<dim; idim++)
    {
      bounds[idim*2]=std::numeric_limits<double>::max();
      bounds[idim*2+1]=-std::numeric_limits<double>::max();
    } 
  const double *ptr=getConstPointer();
  int nbOfTuples=getNumberOfTuples();
  for(int i=0;i<nbOfTuples;i++)
    {
      for(int idim=0;idim<dim;idim++)
        {
          if(bounds[idim*2]>ptr[i*dim+idim])
            {
              bounds[idim*2]=ptr[i*dim+idim];
            }
          if(bounds[idim*2+1]<ptr[i*dim+idim])
            {
              bounds[idim*2+1]=ptr[i*dim+idim];
            }
        }
    }
}

/*!
 * This method retrieves a newly allocated DataArrayDouble instance having same number of tuples than \a this and twice number of components than \a this
 * to store both the min and max per component of each tuples. 
 * \param [in] epsilon the width of the bbox (identical in each direction) - 0.0 by default
 *
 * \return a newly created DataArrayDouble instance having \c this->getNumberOfTuples() tuples and 2 * \c this->getNumberOfComponent() components
 *
 * \throw If \a this is not allocated yet.
 */
DataArrayDouble *DataArrayDouble::computeBBoxPerTuple(double epsilon) const
{
  checkAllocated();
  const double *dataPtr=getConstPointer();
  int nbOfCompo=getNumberOfComponents();
  int nbTuples=getNumberOfTuples();
  MCAuto<DataArrayDouble> bbox=DataArrayDouble::New();
  bbox->alloc(nbTuples,2*nbOfCompo);
  double *bboxPtr=bbox->getPointer();
  for(int i=0;i<nbTuples;i++)
    {
      for(int j=0;j<nbOfCompo;j++)
        {
          bboxPtr[2*nbOfCompo*i+2*j]=dataPtr[nbOfCompo*i+j]-epsilon;
          bboxPtr[2*nbOfCompo*i+2*j+1]=dataPtr[nbOfCompo*i+j]+epsilon;
        }
    }
  return bbox.retn();
}

/*!
 * For each tuples **t** in \a other, this method retrieves tuples in \a this that are equal to **t**.
 * Two tuples are considered equal if the euclidian distance between the two tuples is lower than \a eps.
 * 
 * \param [in] other a DataArrayDouble having same number of components than \a this.
 * \param [in] eps absolute precision representing distance (using infinite norm) between 2 tuples behind which 2 tuples are considered equal.
 * \param [out] c will contain the set of tuple ids in \a this that are equal to to the tuple ids in \a other contiguously.
 *             \a cI allows to extract information in \a c.
 * \param [out] cI is an indirection array that allows to extract the data contained in \a c.
 *
 * \throw In case of:
 *  - \a this is not allocated
 *  - \a other is not allocated or null
 *  - \a this and \a other do not have the same number of components
 *  - if number of components of \a this is not in [1,2,3]
 *
 * \sa MEDCouplingPointSet::getNodeIdsNearPoints, DataArrayDouble::getDifferentValues
 */
void DataArrayDouble::computeTupleIdsNearTuples(const DataArrayDouble *other, double eps, DataArrayInt *& c, DataArrayInt *& cI) const
{
  if(!other)
    throw INTERP_KERNEL::Exception("DataArrayDouble::computeTupleIdsNearTuples : input pointer other is null !");
  checkAllocated();
  other->checkAllocated();
  int nbOfCompo=getNumberOfComponents();
  int otherNbOfCompo=other->getNumberOfComponents();
  if(nbOfCompo!=otherNbOfCompo)
    throw INTERP_KERNEL::Exception("DataArrayDouble::computeTupleIdsNearTuples : number of components should be equal between this and other !");
  int nbOfTuplesOther=other->getNumberOfTuples();
  MCAuto<DataArrayInt> cArr(DataArrayInt::New()),cIArr(DataArrayInt::New()); cArr->alloc(0,1); cIArr->pushBackSilent(0);
  switch(nbOfCompo)
  {
    case 3:
      {
        BBTreePts<3,int> myTree(begin(),0,0,getNumberOfTuples(),eps);
        FindTupleIdsNearTuplesAlg<3>(myTree,other->getConstPointer(),nbOfTuplesOther,eps,cArr,cIArr);
        break;
      }
    case 2:
      {
        BBTreePts<2,int> myTree(begin(),0,0,getNumberOfTuples(),eps);
        FindTupleIdsNearTuplesAlg<2>(myTree,other->getConstPointer(),nbOfTuplesOther,eps,cArr,cIArr);
        break;
      }
    case 1:
      {
        BBTreePts<1,int> myTree(begin(),0,0,getNumberOfTuples(),eps);
        FindTupleIdsNearTuplesAlg<1>(myTree,other->getConstPointer(),nbOfTuplesOther,eps,cArr,cIArr);
        break;
      }
    default:
      throw INTERP_KERNEL::Exception("Unexpected spacedim of coords for computeTupleIdsNearTuples. Must be 1, 2 or 3.");
  }
  c=cArr.retn(); cI=cIArr.retn();
}

/*!
 * This method recenter tuples in \b this in order to be centered at the origin to benefit about the advantages of maximal precision to be around the box
 * around origin of 'radius' 1.
 * 
 * \param [in] eps absolute epsilon. under that value of delta between max and min no scale is performed.
 */
void DataArrayDouble::recenterForMaxPrecision(double eps)
{
  checkAllocated();
  int dim=getNumberOfComponents();
  std::vector<double> bounds(2*dim);
  getMinMaxPerComponent(&bounds[0]);
  for(int i=0;i<dim;i++)
    {
      double delta=bounds[2*i+1]-bounds[2*i];
      double offset=(bounds[2*i]+bounds[2*i+1])/2.;
      if(delta>eps)
        applyLin(1./delta,-offset/delta,i);
      else
        applyLin(1.,-offset,i);
    }
}

/*!
 * Returns the maximal value and all its locations within \a this one-dimensional array.
 *  \param [out] tupleIds - a new instance of DataArrayInt containing indices of
 *               tuples holding the maximal value. The caller is to delete it using
 *               decrRef() as it is no more needed.
 *  \return double - the maximal value among all values of \a this array.
 *  \throw If \a this->getNumberOfComponents() != 1
 *  \throw If \a this->getNumberOfTuples() < 1
 */
double DataArrayDouble::getMaxValue2(DataArrayInt*& tupleIds) const
{
  int tmp;
  tupleIds=0;
  double ret=getMaxValue(tmp);
  tupleIds=findIdsInRange(ret,ret);
  return ret;
}

/*!
 * Returns the minimal value and all its locations within \a this one-dimensional array.
 *  \param [out] tupleIds - a new instance of DataArrayInt containing indices of
 *               tuples holding the minimal value. The caller is to delete it using
 *               decrRef() as it is no more needed.
 *  \return double - the minimal value among all values of \a this array.
 *  \throw If \a this->getNumberOfComponents() != 1
 *  \throw If \a this->getNumberOfTuples() < 1
 */
double DataArrayDouble::getMinValue2(DataArrayInt*& tupleIds) const
{
  int tmp;
  tupleIds=0;
  double ret=getMinValue(tmp);
  tupleIds=findIdsInRange(ret,ret);
  return ret;
}

/*!
 * This method returns the number of values in \a this that are equals ( within an absolute precision of \a eps ) to input parameter \a value.
 * This method only works for single component array.
 *
 * \return a value in [ 0, \c this->getNumberOfTuples() )
 *
 * \throw If \a this is not allocated
 *
 */
int DataArrayDouble::count(double value, double eps) const
{
  int ret=0;
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayDouble::count : must be applied on DataArrayDouble with only one component, you can call 'rearrange' method before !");
  const double *vals=begin();
  int nbOfTuples=getNumberOfTuples();
  for(int i=0;i<nbOfTuples;i++,vals++)
    if(fabs(*vals-value)<=eps)
      ret++;
  return ret;
}

/*!
 * Returns the average value of \a this one-dimensional array.
 *  \return double - the average value over all values of \a this array.
 *  \throw If \a this->getNumberOfComponents() != 1
 *  \throw If \a this->getNumberOfTuples() < 1
 */
double DataArrayDouble::getAverageValue() const
{
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayDouble::getAverageValue : must be applied on DataArrayDouble with only one component, you can call 'rearrange' method before !");
  int nbOfTuples=getNumberOfTuples();
  if(nbOfTuples<=0)
    throw INTERP_KERNEL::Exception("DataArrayDouble::getAverageValue : array exists but number of tuples must be > 0 !");
  const double *vals=getConstPointer();
  double ret=std::accumulate(vals,vals+nbOfTuples,0.);
  return ret/nbOfTuples;
}

/*!
 * Returns the Euclidean norm of the vector defined by \a this array.
 *  \return double - the value of the Euclidean norm, i.e.
 *          the square root of the inner product of vector.
 *  \throw If \a this is not allocated.
 */
double DataArrayDouble::norm2() const
{
  checkAllocated();
  double ret=0.;
  std::size_t nbOfElems=getNbOfElems();
  const double *pt=getConstPointer();
  for(std::size_t i=0;i<nbOfElems;i++,pt++)
    ret+=(*pt)*(*pt);
  return sqrt(ret);
}

/*!
 * Returns the maximum norm of the vector defined by \a this array.
 * This method works even if the number of components is different from one.
 * If the number of elements in \a this is 0, -1. is returned.
 *  \return double - the value of the maximum norm, i.e.
 *          the maximal absolute value among values of \a this array (whatever its number of components).
 *  \throw If \a this is not allocated.
 */
double DataArrayDouble::normMax() const
{
  checkAllocated();
  double ret(-1.);
  std::size_t nbOfElems(getNbOfElems());
  const double *pt(getConstPointer());
  for(std::size_t i=0;i<nbOfElems;i++,pt++)
    {
      double val(std::abs(*pt));
      if(val>ret)
        ret=val;
    }
  return ret;
}

/*!
 * Returns the minimum norm (absolute value) of the vector defined by \a this array.
 * This method works even if the number of components is different from one.
 * If the number of elements in \a this is 0, std::numeric_limits<double>::max() is returned.
 *  \return double - the value of the minimum norm, i.e.
 *          the minimal absolute value among values of \a this array (whatever its number of components).
 *  \throw If \a this is not allocated.
 */
double DataArrayDouble::normMin() const
{
  checkAllocated();
  double ret(std::numeric_limits<double>::max());
  std::size_t nbOfElems(getNbOfElems());
  const double *pt(getConstPointer());
  for(std::size_t i=0;i<nbOfElems;i++,pt++)
    {
      double val(std::abs(*pt));
      if(val<ret)
        ret=val;
    }
  return ret;
}

/*!
 * Accumulates values of each component of \a this array.
 *  \param [out] res - an array of length \a this->getNumberOfComponents(), allocated 
 *         by the caller, that is filled by this method with sum value for each
 *         component.
 *  \throw If \a this is not allocated.
 */
void DataArrayDouble::accumulate(double *res) const
{
  checkAllocated();
  const double *ptr=getConstPointer();
  int nbTuple=getNumberOfTuples();
  int nbComps=getNumberOfComponents();
  std::fill(res,res+nbComps,0.);
  for(int i=0;i<nbTuple;i++)
    std::transform(ptr+i*nbComps,ptr+(i+1)*nbComps,res,res,std::plus<double>());
}

/*!
 * This method returns the min distance from an external tuple defined by [ \a tupleBg , \a tupleEnd ) to \a this and
 * the first tuple in \a this that matches the returned distance. If there is no tuples in \a this an exception will be thrown.
 *
 *
 * \a this is expected to be allocated and expected to have a number of components equal to the distance from \a tupleBg to
 * \a tupleEnd. If not an exception will be thrown.
 *
 * \param [in] tupleBg start pointer (included) of input external tuple
 * \param [in] tupleEnd end pointer (not included) of input external tuple
 * \param [out] tupleId the tuple id in \a this that matches the min of distance between \a this and input external tuple
 * \return the min distance.
 * \sa MEDCouplingUMesh::distanceToPoint
 */
double DataArrayDouble::distanceToTuple(const double *tupleBg, const double *tupleEnd, int& tupleId) const
{
  checkAllocated();
  int nbTuple=getNumberOfTuples();
  int nbComps=getNumberOfComponents();
  if(nbComps!=(int)std::distance(tupleBg,tupleEnd))
    { std::ostringstream oss; oss << "DataArrayDouble::distanceToTuple : size of input tuple is " << std::distance(tupleBg,tupleEnd) << " should be equal to the number of components in this : " << nbComps << " !"; throw INTERP_KERNEL::Exception(oss.str().c_str()); }
  if(nbTuple==0)
    throw INTERP_KERNEL::Exception("DataArrayDouble::distanceToTuple : no tuple in this ! No distance to compute !");
  double ret0=std::numeric_limits<double>::max();
  tupleId=-1;
  const double *work=getConstPointer();
  for(int i=0;i<nbTuple;i++)
    {
      double val=0.;
      for(int j=0;j<nbComps;j++,work++) 
        val+=(*work-tupleBg[j])*((*work-tupleBg[j]));
      if(val>=ret0)
        continue;
      else
        { ret0=val; tupleId=i; }
    }
  return sqrt(ret0);
}

/*!
 * Accumulate values of the given component of \a this array.
 *  \param [in] compId - the index of the component of interest.
 *  \return double - a sum value of \a compId-th component.
 *  \throw If \a this is not allocated.
 *  \throw If \a the condition ( 0 <= \a compId < \a this->getNumberOfComponents() ) is
 *         not respected.
 */
double DataArrayDouble::accumulate(int compId) const
{
  checkAllocated();
  const double *ptr=getConstPointer();
  int nbTuple=getNumberOfTuples();
  int nbComps=getNumberOfComponents();
  if(compId<0 || compId>=nbComps)
    throw INTERP_KERNEL::Exception("DataArrayDouble::accumulate : Invalid compId specified : No such nb of components !");
  double ret=0.;
  for(int i=0;i<nbTuple;i++)
    ret+=ptr[i*nbComps+compId];
  return ret;
}

/*!
 * This method accumulate using addition tuples in \a this using input index array [ \a bgOfIndex, \a endOfIndex ).
 * The returned array will have same number of components than \a this and number of tuples equal to
 * \c std::distance(bgOfIndex,endOfIndex) \b minus \b one.
 *
 * The input index array is expected to be ascendingly sorted in which the all referenced ids should be in [0, \c this->getNumberOfTuples).
 * This method is quite useful for users that need to put a field on cells to field on nodes on the same mesh without a need of conservation.
 *
 * \param [in] bgOfIndex - begin (included) of the input index array.
 * \param [in] endOfIndex - end (excluded) of the input index array.
 * \return DataArrayDouble * - the new instance having the same number of components than \a this.
 * 
 * \throw If bgOfIndex or end is NULL.
 * \throw If input index array is not ascendingly sorted.
 * \throw If there is an id in [ \a bgOfIndex, \a endOfIndex ) not in [0, \c this->getNumberOfTuples).
 * \throw If std::distance(bgOfIndex,endOfIndex)==0.
 */
DataArrayDouble *DataArrayDouble::accumulatePerChunck(const int *bgOfIndex, const int *endOfIndex) const
{
  if(!bgOfIndex || !endOfIndex)
    throw INTERP_KERNEL::Exception("DataArrayDouble::accumulatePerChunck : input pointer NULL !");
  checkAllocated();
  int nbCompo=getNumberOfComponents();
  int nbOfTuples=getNumberOfTuples();
  int sz=(int)std::distance(bgOfIndex,endOfIndex);
  if(sz<1)
    throw INTERP_KERNEL::Exception("DataArrayDouble::accumulatePerChunck : invalid size of input index array !");
  sz--;
  MCAuto<DataArrayDouble> ret=DataArrayDouble::New(); ret->alloc(sz,nbCompo);
  const int *w=bgOfIndex;
  if(*w<0 || *w>=nbOfTuples)
    throw INTERP_KERNEL::Exception("DataArrayDouble::accumulatePerChunck : The first element of the input index not in [0,nbOfTuples) !");
  const double *srcPt=begin()+(*w)*nbCompo;
  double *tmp=ret->getPointer();
  for(int i=0;i<sz;i++,tmp+=nbCompo,w++)
    {
      std::fill(tmp,tmp+nbCompo,0.);
      if(w[1]>=w[0])
        {
          for(int j=w[0];j<w[1];j++,srcPt+=nbCompo)
            {
              if(j>=0 && j<nbOfTuples)
                std::transform(srcPt,srcPt+nbCompo,tmp,tmp,std::plus<double>());
              else
                {
                  std::ostringstream oss; oss << "DataArrayDouble::accumulatePerChunck : At rank #" << i << " the input index array points to id " << j << " should be in [0," << nbOfTuples << ") !";
                  throw INTERP_KERNEL::Exception(oss.str().c_str());
                }
            }
        }
      else
        {
          std::ostringstream oss; oss << "DataArrayDouble::accumulatePerChunck : At rank #" << i << " the input index array is not in ascendingly sorted.";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  ret->copyStringInfoFrom(*this);
  return ret.retn();
}

/*!
 * This method is close to numpy cumSum except that number of element is equal to \a this->getNumberOfTuples()+1. First element of DataArray returned is equal to 0.
 * This method expects that \a this as only one component. The returned array will have \a this->getNumberOfTuples()+1 tuple with also one component.
 * The ith element of returned array is equal to the sum of elements in \a this with rank strictly lower than i.
 *
 * \return DataArrayDouble - A newly built array containing cum sum of \a this.
 */
MCAuto<DataArrayDouble> DataArrayDouble::cumSum() const
{
  checkAllocated();
  checkNbOfComps(1,"DataArrayDouble::cumSum : this is expected to be single component");
  int nbOfTuple(getNumberOfTuples());
  MCAuto<DataArrayDouble> ret(DataArrayDouble::New()); ret->alloc(nbOfTuple+1,1);
  double *ptr(ret->getPointer());
  ptr[0]=0.;
  const double *thisPtr(begin());
  for(int i=0;i<nbOfTuple;i++)
    ptr[i+1]=ptr[i]+thisPtr[i];
  return ret;
}

/*!
 * Converts each 2D point defined by the tuple of \a this array from the Polar to the
 * Cartesian coordinate system. The two components of the tuple of \a this array are 
 * considered to contain (1) radius and (2) angle of the point in the Polar CS.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble, whose each tuple
 *          contains X and Y coordinates of the point in the Cartesian CS. The caller
 *          is to delete this array using decrRef() as it is no more needed. The array
 *          does not contain any textual info on components.
 *  \throw If \a this->getNumberOfComponents() != 2.
 * \sa fromCartToPolar
 */
DataArrayDouble *DataArrayDouble::fromPolarToCart() const
{
  checkAllocated();
  int nbOfComp(getNumberOfComponents());
  if(nbOfComp!=2)
    throw INTERP_KERNEL::Exception("DataArrayDouble::fromPolarToCart : must be an array with exactly 2 components !");
  int nbOfTuple(getNumberOfTuples());
  DataArrayDouble *ret(DataArrayDouble::New());
  ret->alloc(nbOfTuple,2);
  double *w(ret->getPointer());
  const double *wIn(getConstPointer());
  for(int i=0;i<nbOfTuple;i++,w+=2,wIn+=2)
    {
      w[0]=wIn[0]*cos(wIn[1]);
      w[1]=wIn[0]*sin(wIn[1]);
    }
  return ret;
}

/*!
 * Converts each 3D point defined by the tuple of \a this array from the Cylindrical to
 * the Cartesian coordinate system. The three components of the tuple of \a this array 
 * are considered to contain (1) radius, (2) azimuth and (3) altitude of the point in
 * the Cylindrical CS.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble, whose each tuple
 *          contains X, Y and Z coordinates of the point in the Cartesian CS. The info
 *          on the third component is copied from \a this array. The caller
 *          is to delete this array using decrRef() as it is no more needed. 
 *  \throw If \a this->getNumberOfComponents() != 3.
 * \sa fromCartToCyl
 */
DataArrayDouble *DataArrayDouble::fromCylToCart() const
{
  checkAllocated();
  int nbOfComp(getNumberOfComponents());
  if(nbOfComp!=3)
    throw INTERP_KERNEL::Exception("DataArrayDouble::fromCylToCart : must be an array with exactly 3 components !");
  int nbOfTuple(getNumberOfTuples());
  DataArrayDouble *ret(DataArrayDouble::New());
  ret->alloc(getNumberOfTuples(),3);
  double *w(ret->getPointer());
  const double *wIn(getConstPointer());
  for(int i=0;i<nbOfTuple;i++,w+=3,wIn+=3)
    {
      w[0]=wIn[0]*cos(wIn[1]);
      w[1]=wIn[0]*sin(wIn[1]);
      w[2]=wIn[2];
    }
  ret->setInfoOnComponent(2,getInfoOnComponent(2));
  return ret;
}

/*!
 * Converts each 3D point defined by the tuple of \a this array from the Spherical to
 * the Cartesian coordinate system. The three components of the tuple of \a this array 
 * are considered to contain (1) radius, (2) polar angle and (3) azimuthal angle of the
 * point in the Cylindrical CS.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble, whose each tuple
 *          contains X, Y and Z coordinates of the point in the Cartesian CS. The info
 *          on the third component is copied from \a this array. The caller
 *          is to delete this array using decrRef() as it is no more needed.
 *  \throw If \a this->getNumberOfComponents() != 3.
 * \sa fromCartToSpher
 */
DataArrayDouble *DataArrayDouble::fromSpherToCart() const
{
  checkAllocated();
  int nbOfComp(getNumberOfComponents());
  if(nbOfComp!=3)
    throw INTERP_KERNEL::Exception("DataArrayDouble::fromSpherToCart : must be an array with exactly 3 components !");
  int nbOfTuple(getNumberOfTuples());
  DataArrayDouble *ret(DataArrayDouble::New());
  ret->alloc(getNumberOfTuples(),3);
  double *w(ret->getPointer());
  const double *wIn(getConstPointer());
  for(int i=0;i<nbOfTuple;i++,w+=3,wIn+=3)
    {
      w[0]=wIn[0]*cos(wIn[2])*sin(wIn[1]);
      w[1]=wIn[0]*sin(wIn[2])*sin(wIn[1]);
      w[2]=wIn[0]*cos(wIn[1]);
    }
  return ret;
}

/*!
 * This method returns a new array containing the same number of tuples than \a this. To do this, this method needs \a at parameter to specify the convention of \a this.
 * All the tuples of the returned array will be in cartesian sense. So if \a at equals to AX_CART the returned array is basically a deep copy of \a this.
 * If \a at equals to AX_CYL the returned array will be the result of operation cylindric to cartesian of \a this...
 *
 * \param [in] atOfThis - The axis type of \a this.
 * \return DataArrayDouble * - the new instance of DataArrayDouble (that must be dealed by caller) containing the result of the cartesianizification of \a this.
 */
DataArrayDouble *DataArrayDouble::cartesianize(MEDCouplingAxisType atOfThis) const
{
  checkAllocated();
  int nbOfComp(getNumberOfComponents());
  MCAuto<DataArrayDouble> ret;
  switch(atOfThis)
    {
    case AX_CART:
      ret=deepCopy();
    case AX_CYL:
      if(nbOfComp==3)
        {
          ret=fromCylToCart();
          break;
        }
      if(nbOfComp==2)
        {
          ret=fromPolarToCart();
          break;
        }
      else
        throw INTERP_KERNEL::Exception("DataArrayDouble::cartesianize : For AX_CYL, number of components must be in [2,3] !");
    case AX_SPHER:
      if(nbOfComp==3)
        {
          ret=fromSpherToCart();
          break;
        }
      if(nbOfComp==2)
        {
          ret=fromPolarToCart();
          break;
        }
      else
        throw INTERP_KERNEL::Exception("DataArrayDouble::cartesianize : For AX_CYL, number of components must be in [2,3] !");
    default:
      throw INTERP_KERNEL::Exception("DataArrayDouble::cartesianize : not recognized axis type ! Only AX_CART, AX_CYL and AX_SPHER supported !");
    }
  ret->copyStringInfoFrom(*this);
  return ret.retn();
}

/*!
 * This method returns a newly created array to be deallocated that contains the result of conversion from cartesian to polar.
 * This method expects that \a this has exactly 2 components.
 * \sa fromPolarToCart
 */
DataArrayDouble *DataArrayDouble::fromCartToPolar() const
{
  MCAuto<DataArrayDouble> ret(DataArrayDouble::New());
  checkAllocated();
  int nbOfComp(getNumberOfComponents()),nbTuples(getNumberOfTuples());
  if(nbOfComp!=2)
    throw INTERP_KERNEL::Exception("DataArrayDouble::fromCartToPolar : must be an array with exactly 2 components !");
  ret->alloc(nbTuples,2);
  double *retPtr(ret->getPointer());
  const double *ptr(begin());
  for(int i=0;i<nbTuples;i++,ptr+=2,retPtr+=2)
    {
      retPtr[0]=sqrt(ptr[0]*ptr[0]+ptr[1]*ptr[1]);
      retPtr[1]=atan2(ptr[1],ptr[0]);
    }
  return ret.retn();
}

/*!
 * This method returns a newly created array to be deallocated that contains the result of conversion from cartesian to cylindrical.
 * This method expects that \a this has exactly 3 components.
 * \sa fromCylToCart
 */
DataArrayDouble *DataArrayDouble::fromCartToCyl() const
{
  MCAuto<DataArrayDouble> ret(DataArrayDouble::New());
  checkAllocated();
  int nbOfComp(getNumberOfComponents()),nbTuples(getNumberOfTuples());
  if(nbOfComp!=3)
    throw INTERP_KERNEL::Exception("DataArrayDouble::fromCartToCyl : must be an array with exactly 3 components !");
  ret->alloc(nbTuples,3);
  double *retPtr(ret->getPointer());
  const double *ptr(begin());
  for(int i=0;i<nbTuples;i++,ptr+=3,retPtr+=3)
    {
      retPtr[0]=sqrt(ptr[0]*ptr[0]+ptr[1]*ptr[1]);
      retPtr[1]=atan2(ptr[1],ptr[0]);
      retPtr[2]=ptr[2];
    }
  return ret.retn();
}

/*!
 * This method returns a newly created array to be deallocated that contains the result of conversion from cartesian to spherical coordinates.
 * \sa fromSpherToCart
 */
DataArrayDouble *DataArrayDouble::fromCartToSpher() const
{
  MCAuto<DataArrayDouble> ret(DataArrayDouble::New());
  checkAllocated();
  int nbOfComp(getNumberOfComponents()),nbTuples(getNumberOfTuples());
  if(nbOfComp!=3)
    throw INTERP_KERNEL::Exception("DataArrayDouble::fromCartToSpher : must be an array with exactly 3 components !");
  ret->alloc(nbTuples,3);
  double *retPtr(ret->getPointer());
  const double *ptr(begin());
  for(int i=0;i<nbTuples;i++,ptr+=3,retPtr+=3)
    {
      retPtr[0]=sqrt(ptr[0]*ptr[0]+ptr[1]*ptr[1]+ptr[2]*ptr[2]);
      retPtr[1]=acos(ptr[2]/retPtr[0]);
      retPtr[2]=atan2(ptr[1],ptr[0]);
    }
  return ret.retn();
}

/*!
 * This method returns a newly created array to be deallocated that contains the result of conversion from cartesian to cylindrical relative to the given \a center and a \a vector.
 * This method expects that \a this has exactly 3 components.
 * \sa MEDCouplingFieldDouble::computeVectorFieldCyl
 */
DataArrayDouble *DataArrayDouble::fromCartToCylGiven(const DataArrayDouble *coords, const double center[3], const double vect[3]) const
{
  if(!coords)
    throw INTERP_KERNEL::Exception("DataArrayDouble::fromCartToCylGiven : input coords are NULL !");
  MCAuto<DataArrayDouble> ret(DataArrayDouble::New());
  checkAllocated(); coords->checkAllocated();
  std::size_t nbOfComp(getNumberOfComponents()),nbTuples(getNumberOfTuples());
  if(nbOfComp!=3)
    throw INTERP_KERNEL::Exception("DataArrayDouble::fromCartToCylGiven : must be an array with exactly 3 components !");
  if(coords->getNumberOfComponents()!=3)
    throw INTERP_KERNEL::Exception("DataArrayDouble::fromCartToCylGiven : coords array must have exactly 3 components !");
  if(coords->getNumberOfTuples()!=nbTuples)
    throw INTERP_KERNEL::Exception("DataArrayDouble::fromCartToCylGiven : coords array must have the same number of tuples !");
  ret->alloc(nbTuples,nbOfComp);
  double magOfVect(sqrt(vect[0]*vect[0]+vect[1]*vect[1]+vect[2]*vect[2]));
  if(magOfVect<1e-12)
    throw INTERP_KERNEL::Exception("DataArrayDouble::fromCartToCylGiven : magnitude of vect is too low !");
  double Ur[3],Uteta[3],Uz[3],*retPtr(ret->getPointer());
  const double *coo(coords->begin()),*vectField(begin());
  std::transform(vect,vect+3,Uz,std::bind2nd(std::multiplies<double>(),1./magOfVect));
  for(int i=0;i<nbTuples;i++,vectField+=3,retPtr+=3,coo+=3)
    {
      std::transform(coo,coo+3,center,Ur,std::minus<double>());
      Uteta[0]=Uz[1]*Ur[2]-Uz[2]*Ur[1]; Uteta[1]=Uz[2]*Ur[0]-Uz[0]*Ur[2]; Uteta[2]=Uz[0]*Ur[1]-Uz[1]*Ur[0];
      double magOfTeta(sqrt(Uteta[0]*Uteta[0]+Uteta[1]*Uteta[1]+Uteta[2]*Uteta[2]));
      std::transform(Uteta,Uteta+3,Uteta,std::bind2nd(std::multiplies<double>(),1./magOfTeta));
      Ur[0]=Uteta[1]*Uz[2]-Uteta[2]*Uz[1]; Ur[1]=Uteta[2]*Uz[0]-Uteta[0]*Uz[2]; Ur[2]=Uteta[0]*Uz[1]-Uteta[1]*Uz[0];
      retPtr[0]=Ur[0]*vectField[0]+Ur[1]*vectField[1]+Ur[2]*vectField[2];
      retPtr[1]=Uteta[0]*vectField[0]+Uteta[1]*vectField[1]+Uteta[2]*vectField[2];
      retPtr[2]=Uz[0]*vectField[0]+Uz[1]*vectField[1]+Uz[2]*vectField[2];
    }
  ret->copyStringInfoFrom(*this);
  return ret.retn();
}

/*!
 * Computes the doubly contracted product of every tensor defined by the tuple of \a this
 * array contating 6 components.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble, whose each tuple
 *          is calculated from the tuple <em>(t)</em> of \a this array as follows:
 *          \f$ t[0]^2+t[1]^2+t[2]^2+2*t[3]^2+2*t[4]^2+2*t[5]^2\f$.
 *         The caller is to delete this result array using decrRef() as it is no more needed. 
 *  \throw If \a this->getNumberOfComponents() != 6.
 */
DataArrayDouble *DataArrayDouble::doublyContractedProduct() const
{
  checkAllocated();
  int nbOfComp(getNumberOfComponents());
  if(nbOfComp!=6)
    throw INTERP_KERNEL::Exception("DataArrayDouble::doublyContractedProduct : must be an array with exactly 6 components !");
  DataArrayDouble *ret=DataArrayDouble::New();
  int nbOfTuple=getNumberOfTuples();
  ret->alloc(nbOfTuple,1);
  const double *src=getConstPointer();
  double *dest=ret->getPointer();
  for(int i=0;i<nbOfTuple;i++,dest++,src+=6)
    *dest=src[0]*src[0]+src[1]*src[1]+src[2]*src[2]+2.*src[3]*src[3]+2.*src[4]*src[4]+2.*src[5]*src[5];
  return ret;
}

/*!
 * Computes the determinant of every square matrix defined by the tuple of \a this
 * array, which contains either 4, 6 or 9 components. The case of 6 components
 * corresponds to that of the upper triangular matrix.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble, whose each tuple
 *          is the determinant of matrix of the corresponding tuple of \a this array.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed. 
 *  \throw If \a this->getNumberOfComponents() is not in [4,6,9].
 */
DataArrayDouble *DataArrayDouble::determinant() const
{
  checkAllocated();
  DataArrayDouble *ret=DataArrayDouble::New();
  int nbOfTuple=getNumberOfTuples();
  ret->alloc(nbOfTuple,1);
  const double *src=getConstPointer();
  double *dest=ret->getPointer();
  switch(getNumberOfComponents())
  {
    case 6:
      for(int i=0;i<nbOfTuple;i++,dest++,src+=6)
        *dest=src[0]*src[1]*src[2]+2.*src[4]*src[5]*src[3]-src[0]*src[4]*src[4]-src[2]*src[3]*src[3]-src[1]*src[5]*src[5];
      return ret;
    case 4:
      for(int i=0;i<nbOfTuple;i++,dest++,src+=4)
        *dest=src[0]*src[3]-src[1]*src[2];
      return ret;
    case 9:
      for(int i=0;i<nbOfTuple;i++,dest++,src+=9)
        *dest=src[0]*src[4]*src[8]+src[1]*src[5]*src[6]+src[2]*src[3]*src[7]-src[0]*src[5]*src[7]-src[1]*src[3]*src[8]-src[2]*src[4]*src[6];
      return ret;
    default:
      ret->decrRef();
      throw INTERP_KERNEL::Exception("DataArrayDouble::determinant : Invalid number of components ! must be in 4,6,9 !");
  }
}

/*!
 * Computes 3 eigenvalues of every upper triangular matrix defined by the tuple of
 * \a this array, which contains 6 components.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble containing 3
 *          components, whose each tuple contains the eigenvalues of the matrix of
 *          corresponding tuple of \a this array. 
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed. 
 *  \throw If \a this->getNumberOfComponents() != 6.
 */
DataArrayDouble *DataArrayDouble::eigenValues() const
{
  checkAllocated();
  int nbOfComp=getNumberOfComponents();
  if(nbOfComp!=6)
    throw INTERP_KERNEL::Exception("DataArrayDouble::eigenValues : must be an array with exactly 6 components !");
  DataArrayDouble *ret=DataArrayDouble::New();
  int nbOfTuple=getNumberOfTuples();
  ret->alloc(nbOfTuple,3);
  const double *src=getConstPointer();
  double *dest=ret->getPointer();
  for(int i=0;i<nbOfTuple;i++,dest+=3,src+=6)
    INTERP_KERNEL::computeEigenValues6(src,dest);
  return ret;
}

/*!
 * Computes 3 eigenvectors of every upper triangular matrix defined by the tuple of
 * \a this array, which contains 6 components.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble containing 9
 *          components, whose each tuple contains 3 eigenvectors of the matrix of
 *          corresponding tuple of \a this array.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If \a this->getNumberOfComponents() != 6.
 */
DataArrayDouble *DataArrayDouble::eigenVectors() const
{
  checkAllocated();
  int nbOfComp=getNumberOfComponents();
  if(nbOfComp!=6)
    throw INTERP_KERNEL::Exception("DataArrayDouble::eigenVectors : must be an array with exactly 6 components !");
  DataArrayDouble *ret=DataArrayDouble::New();
  int nbOfTuple=getNumberOfTuples();
  ret->alloc(nbOfTuple,9);
  const double *src=getConstPointer();
  double *dest=ret->getPointer();
  for(int i=0;i<nbOfTuple;i++,src+=6)
    {
      double tmp[3];
      INTERP_KERNEL::computeEigenValues6(src,tmp);
      for(int j=0;j<3;j++,dest+=3)
        INTERP_KERNEL::computeEigenVectorForEigenValue6(src,tmp[j],1e-12,dest);
    }
  return ret;
}

/*!
 * Computes the inverse matrix of every matrix defined by the tuple of \a this
 * array, which contains either 4, 6 or 9 components. The case of 6 components
 * corresponds to that of the upper triangular matrix.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble containing the
 *          same number of components as \a this one, whose each tuple is the inverse
 *          matrix of the matrix of corresponding tuple of \a this array. 
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed. 
 *  \throw If \a this->getNumberOfComponents() is not in [4,6,9].
 */
DataArrayDouble *DataArrayDouble::inverse() const
{
  checkAllocated();
  int nbOfComp=getNumberOfComponents();
  if(nbOfComp!=6 && nbOfComp!=9 && nbOfComp!=4)
    throw INTERP_KERNEL::Exception("DataArrayDouble::inversion : must be an array with 4,6 or 9 components !");
  DataArrayDouble *ret=DataArrayDouble::New();
  int nbOfTuple=getNumberOfTuples();
  ret->alloc(nbOfTuple,nbOfComp);
  const double *src=getConstPointer();
  double *dest=ret->getPointer();
  if(nbOfComp==6)
    for(int i=0;i<nbOfTuple;i++,dest+=6,src+=6)
      {
        double det=src[0]*src[1]*src[2]+2.*src[4]*src[5]*src[3]-src[0]*src[4]*src[4]-src[2]*src[3]*src[3]-src[1]*src[5]*src[5];
        dest[0]=(src[1]*src[2]-src[4]*src[4])/det;
        dest[1]=(src[0]*src[2]-src[5]*src[5])/det;
        dest[2]=(src[0]*src[1]-src[3]*src[3])/det;
        dest[3]=(src[5]*src[4]-src[3]*src[2])/det;
        dest[4]=(src[5]*src[3]-src[0]*src[4])/det;
        dest[5]=(src[3]*src[4]-src[1]*src[5])/det;
      }
  else if(nbOfComp==4)
    for(int i=0;i<nbOfTuple;i++,dest+=4,src+=4)
      {
        double det=src[0]*src[3]-src[1]*src[2];
        dest[0]=src[3]/det;
        dest[1]=-src[1]/det;
        dest[2]=-src[2]/det;
        dest[3]=src[0]/det;
      }
  else
    for(int i=0;i<nbOfTuple;i++,dest+=9,src+=9)
      {
        double det=src[0]*src[4]*src[8]+src[1]*src[5]*src[6]+src[2]*src[3]*src[7]-src[0]*src[5]*src[7]-src[1]*src[3]*src[8]-src[2]*src[4]*src[6];
        dest[0]=(src[4]*src[8]-src[7]*src[5])/det;
        dest[1]=(src[7]*src[2]-src[1]*src[8])/det;
        dest[2]=(src[1]*src[5]-src[4]*src[2])/det;
        dest[3]=(src[6]*src[5]-src[3]*src[8])/det;
        dest[4]=(src[0]*src[8]-src[6]*src[2])/det;
        dest[5]=(src[2]*src[3]-src[0]*src[5])/det;
        dest[6]=(src[3]*src[7]-src[6]*src[4])/det;
        dest[7]=(src[6]*src[1]-src[0]*src[7])/det;
        dest[8]=(src[0]*src[4]-src[1]*src[3])/det;
      }
  return ret;
}

/*!
 * Computes the trace of every matrix defined by the tuple of \a this
 * array, which contains either 4, 6 or 9 components. The case of 6 components
 * corresponds to that of the upper triangular matrix.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble containing 
 *          1 component, whose each tuple is the trace of
 *          the matrix of corresponding tuple of \a this array. 
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed. 
 *  \throw If \a this->getNumberOfComponents() is not in [4,6,9].
 */
DataArrayDouble *DataArrayDouble::trace() const
{
  checkAllocated();
  int nbOfComp=getNumberOfComponents();
  if(nbOfComp!=6 && nbOfComp!=9 && nbOfComp!=4)
    throw INTERP_KERNEL::Exception("DataArrayDouble::trace : must be an array with 4,6 or 9 components !");
  DataArrayDouble *ret=DataArrayDouble::New();
  int nbOfTuple=getNumberOfTuples();
  ret->alloc(nbOfTuple,1);
  const double *src=getConstPointer();
  double *dest=ret->getPointer();
  if(nbOfComp==6)
    for(int i=0;i<nbOfTuple;i++,dest++,src+=6)
      *dest=src[0]+src[1]+src[2];
  else if(nbOfComp==4)
    for(int i=0;i<nbOfTuple;i++,dest++,src+=4)
      *dest=src[0]+src[3];
  else
    for(int i=0;i<nbOfTuple;i++,dest++,src+=9)
      *dest=src[0]+src[4]+src[8];
  return ret;
}

/*!
 * Computes the stress deviator tensor of every stress tensor defined by the tuple of
 * \a this array, which contains 6 components.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble containing the
 *          same number of components and tuples as \a this array.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If \a this->getNumberOfComponents() != 6.
 */
DataArrayDouble *DataArrayDouble::deviator() const
{
  checkAllocated();
  int nbOfComp=getNumberOfComponents();
  if(nbOfComp!=6)
    throw INTERP_KERNEL::Exception("DataArrayDouble::deviator : must be an array with exactly 6 components !");
  DataArrayDouble *ret=DataArrayDouble::New();
  int nbOfTuple=getNumberOfTuples();
  ret->alloc(nbOfTuple,6);
  const double *src=getConstPointer();
  double *dest=ret->getPointer();
  for(int i=0;i<nbOfTuple;i++,dest+=6,src+=6)
    {
      double tr=(src[0]+src[1]+src[2])/3.;
      dest[0]=src[0]-tr;
      dest[1]=src[1]-tr;
      dest[2]=src[2]-tr;
      dest[3]=src[3];
      dest[4]=src[4];
      dest[5]=src[5];
    }
  return ret;
}

/*!
 * Computes the magnitude of every vector defined by the tuple of
 * \a this array.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble containing the
 *          same number of tuples as \a this array and one component.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If \a this is not allocated.
 */
DataArrayDouble *DataArrayDouble::magnitude() const
{
  checkAllocated();
  int nbOfComp=getNumberOfComponents();
  DataArrayDouble *ret=DataArrayDouble::New();
  int nbOfTuple=getNumberOfTuples();
  ret->alloc(nbOfTuple,1);
  const double *src=getConstPointer();
  double *dest=ret->getPointer();
  for(int i=0;i<nbOfTuple;i++,dest++)
    {
      double sum=0.;
      for(int j=0;j<nbOfComp;j++,src++)
        sum+=(*src)*(*src);
      *dest=sqrt(sum);
    }
  return ret;
}

/*!
 * Computes the maximal value within every tuple of \a this array.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble containing the
 *          same number of tuples as \a this array and one component.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If \a this is not allocated.
 *  \sa DataArrayDouble::maxPerTupleWithCompoId
 */
DataArrayDouble *DataArrayDouble::maxPerTuple() const
{
  checkAllocated();
  int nbOfComp=getNumberOfComponents();
  MCAuto<DataArrayDouble> ret=DataArrayDouble::New();
  int nbOfTuple=getNumberOfTuples();
  ret->alloc(nbOfTuple,1);
  const double *src=getConstPointer();
  double *dest=ret->getPointer();
  for(int i=0;i<nbOfTuple;i++,dest++,src+=nbOfComp)
    *dest=*std::max_element(src,src+nbOfComp);
  return ret.retn();
}

/*!
 * Computes the maximal value within every tuple of \a this array and it returns the first component
 * id for each tuple that corresponds to the maximal value within the tuple.
 * 
 *  \param [out] compoIdOfMaxPerTuple - the new new instance of DataArrayInt containing the
 *          same number of tuples and only one component.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble containing the
 *          same number of tuples as \a this array and one component.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If \a this is not allocated.
 *  \sa DataArrayDouble::maxPerTuple
 */
DataArrayDouble *DataArrayDouble::maxPerTupleWithCompoId(DataArrayInt* &compoIdOfMaxPerTuple) const
{
  checkAllocated();
  int nbOfComp=getNumberOfComponents();
  MCAuto<DataArrayDouble> ret0=DataArrayDouble::New();
  MCAuto<DataArrayInt> ret1=DataArrayInt::New();
  int nbOfTuple=getNumberOfTuples();
  ret0->alloc(nbOfTuple,1); ret1->alloc(nbOfTuple,1);
  const double *src=getConstPointer();
  double *dest=ret0->getPointer(); int *dest1=ret1->getPointer();
  for(int i=0;i<nbOfTuple;i++,dest++,dest1++,src+=nbOfComp)
    {
      const double *loc=std::max_element(src,src+nbOfComp);
      *dest=*loc;
      *dest1=(int)std::distance(src,loc);
    }
  compoIdOfMaxPerTuple=ret1.retn();
  return ret0.retn();
}

/*!
 * This method returns a newly allocated DataArrayDouble instance having one component and \c this->getNumberOfTuples() * \c this->getNumberOfTuples() tuples.
 * \n This returned array contains the euclidian distance for each tuple in \a this. 
 * \n So the returned array can be seen as a dense symmetrical matrix whose diagonal elements are equal to 0.
 * \n The returned array has only one component (and **not** \c this->getNumberOfTuples() components to avoid the useless memory consumption due to components info in returned DataArrayDouble)
 *
 * \warning use this method with care because it can leads to big amount of consumed memory !
 * 
 * \return A newly allocated (huge) MEDCoupling::DataArrayDouble instance that the caller should deal with.
 *
 * \throw If \a this is not allocated.
 *
 * \sa DataArrayDouble::buildEuclidianDistanceDenseMatrixWith
 */
DataArrayDouble *DataArrayDouble::buildEuclidianDistanceDenseMatrix() const
{
  checkAllocated();
  int nbOfComp=getNumberOfComponents();
  int nbOfTuples=getNumberOfTuples();
  const double *inData=getConstPointer();
  MCAuto<DataArrayDouble> ret=DataArrayDouble::New();
  ret->alloc(nbOfTuples*nbOfTuples,1);
  double *outData=ret->getPointer();
  for(int i=0;i<nbOfTuples;i++)
    {
      outData[i*nbOfTuples+i]=0.;
      for(int j=i+1;j<nbOfTuples;j++)
        {
          double dist=0.;
          for(int k=0;k<nbOfComp;k++)
            { double delta=inData[i*nbOfComp+k]-inData[j*nbOfComp+k]; dist+=delta*delta; }
          dist=sqrt(dist);
          outData[i*nbOfTuples+j]=dist;
          outData[j*nbOfTuples+i]=dist;
        }
    }
  return ret.retn();
}

/*!
 * This method returns a newly allocated DataArrayDouble instance having one component and \c this->getNumberOfTuples() * \c other->getNumberOfTuples() tuples.
 * \n This returned array contains the euclidian distance for each tuple in \a other with each tuple in \a this. 
 * \n So the returned array can be seen as a dense rectangular matrix with \c other->getNumberOfTuples() rows and \c this->getNumberOfTuples() columns.
 * \n Output rectangular matrix is sorted along rows.
 * \n The returned array has only one component (and **not** \c this->getNumberOfTuples() components to avoid the useless memory consumption due to components info in returned DataArrayDouble)
 *
 * \warning use this method with care because it can leads to big amount of consumed memory !
 * 
 * \param [in] other DataArrayDouble instance having same number of components than \a this.
 * \return A newly allocated (huge) MEDCoupling::DataArrayDouble instance that the caller should deal with.
 *
 * \throw If \a this is not allocated, or if \a other is null or if \a other is not allocated, or if number of components of \a other and \a this differs.
 *
 * \sa DataArrayDouble::buildEuclidianDistanceDenseMatrix
 */
DataArrayDouble *DataArrayDouble::buildEuclidianDistanceDenseMatrixWith(const DataArrayDouble *other) const
{
  if(!other)
    throw INTERP_KERNEL::Exception("DataArrayDouble::buildEuclidianDistanceDenseMatrixWith : input parameter is null !");
  checkAllocated();
  other->checkAllocated();
  int nbOfComp=getNumberOfComponents();
  int otherNbOfComp=other->getNumberOfComponents();
  if(nbOfComp!=otherNbOfComp)
    {
      std::ostringstream oss; oss << "DataArrayDouble::buildEuclidianDistanceDenseMatrixWith : this nb of compo=" << nbOfComp << " and other nb of compo=" << otherNbOfComp << ". It should match !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  int nbOfTuples=getNumberOfTuples();
  int otherNbOfTuples=other->getNumberOfTuples();
  const double *inData=getConstPointer();
  const double *inDataOther=other->getConstPointer();
  MCAuto<DataArrayDouble> ret=DataArrayDouble::New();
  ret->alloc(otherNbOfTuples*nbOfTuples,1);
  double *outData=ret->getPointer();
  for(int i=0;i<otherNbOfTuples;i++,inDataOther+=nbOfComp)
    {
      for(int j=0;j<nbOfTuples;j++)
        {
          double dist=0.;
          for(int k=0;k<nbOfComp;k++)
            { double delta=inDataOther[k]-inData[j*nbOfComp+k]; dist+=delta*delta; }
          dist=sqrt(dist);
          outData[i*nbOfTuples+j]=dist;
        }
    }
  return ret.retn();
}

/*!
 * This method expects that \a this stores 3 tuples containing 2 components each.
 * Each of this tuples represent a point into 2D space.
 * This method tries to find an arc of circle starting from first point (tuple) to 2nd and middle point (tuple) along 3nd and last point (tuple).
 * If such arc of circle exists, the corresponding center, radius of circle is returned. And additionnaly the length of arc expressed with an \a ang output variable in ]0,2*pi[.
 *  
 *  \throw If \a this is not allocated.
 *  \throw If \a this has not 3 tuples of 2 components
 *  \throw If tuples/points in \a this are aligned
 */
void DataArrayDouble::asArcOfCircle(double center[2], double& radius, double& ang) const
{
  checkAllocated();
  INTERP_KERNEL::QuadraticPlanarArcDetectionPrecision arcPrec(1e-14);
  if(getNumberOfTuples()!=3 && getNumberOfComponents()!=2)
    throw INTERP_KERNEL::Exception("DataArrayDouble::asArcCircle : this method expects");
  const double *pt(begin());
  MCAuto<INTERP_KERNEL::Node> n0(new INTERP_KERNEL::Node(pt[0],pt[1])),n1(new INTERP_KERNEL::Node(pt[2],pt[3])),n2(new INTERP_KERNEL::Node(pt[4],pt[5]));
  {
    INTERP_KERNEL::AutoCppPtr<INTERP_KERNEL::EdgeLin> e1(new INTERP_KERNEL::EdgeLin(n0,n2)),e2(new INTERP_KERNEL::EdgeLin(n2,n1));
    INTERP_KERNEL::SegSegIntersector inters(*e1,*e2);
    bool colinearity(inters.areColinears());
    if(colinearity)
      throw INTERP_KERNEL::Exception("DataArrayDouble::asArcOfCircle : 3 points in this have been detected as colinear !");
  }
  INTERP_KERNEL::AutoCppPtr<INTERP_KERNEL::EdgeArcCircle> ret(new INTERP_KERNEL::EdgeArcCircle(n0,n2,n1));
  const double *c(ret->getCenter());
  center[0]=c[0]; center[1]=c[1];
  radius=ret->getRadius();
  ang=ret->getAngle();
}

/*!
 * Sorts value within every tuple of \a this array.
 *  \param [in] asc - if \a true, the values are sorted in ascending order, else,
 *              in descending order.
 *  \throw If \a this is not allocated.
 */
void DataArrayDouble::sortPerTuple(bool asc)
{
  checkAllocated();
  double *pt=getPointer();
  int nbOfTuple=getNumberOfTuples();
  int nbOfComp=getNumberOfComponents();
  if(asc)
    for(int i=0;i<nbOfTuple;i++,pt+=nbOfComp)
      std::sort(pt,pt+nbOfComp);
  else
    for(int i=0;i<nbOfTuple;i++,pt+=nbOfComp)
      std::sort(pt,pt+nbOfComp,std::greater<double>());
  declareAsNew();
}

/*!
 * Modify all elements of \a this array, so that
 * an element _x_ becomes \f$ numerator / x \f$.
 *  \warning If an exception is thrown because of presence of 0.0 element in \a this 
 *           array, all elements processed before detection of the zero element remain
 *           modified.
 *  \param [in] numerator - the numerator used to modify array elements.
 *  \throw If \a this is not allocated.
 *  \throw If there is an element equal to 0.0 in \a this array.
 */
void DataArrayDouble::applyInv(double numerator)
{
  checkAllocated();
  double *ptr=getPointer();
  std::size_t nbOfElems=getNbOfElems();
  for(std::size_t i=0;i<nbOfElems;i++,ptr++)
    {
      if(std::abs(*ptr)>std::numeric_limits<double>::min())
        {
          *ptr=numerator/(*ptr);
        }
      else
        {
          std::ostringstream oss; oss << "DataArrayDouble::applyInv : presence of null value in tuple #" << i/getNumberOfComponents() << " component #" << i%getNumberOfComponents();
          oss << " !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  declareAsNew();
}

/*!
 * Modify all elements of \a this array, so that
 * an element _x_ becomes <em> val ^ x </em>. Contrary to DataArrayInt::applyPow
 * all values in \a this have to be >= 0 if val is \b not integer.
 *  \param [in] val - the value used to apply pow on all array elements.
 *  \throw If \a this is not allocated.
 *  \warning If an exception is thrown because of presence of 0 element in \a this 
 *           array and \a val is \b not integer, all elements processed before detection of the zero element remain
 *           modified.
 */
void DataArrayDouble::applyPow(double val)
{
  checkAllocated();
  double *ptr=getPointer();
  std::size_t nbOfElems=getNbOfElems();
  int val2=(int)val;
  bool isInt=((double)val2)==val;
  if(!isInt)
    {
      for(std::size_t i=0;i<nbOfElems;i++,ptr++)
        {
          if(*ptr>=0)
            *ptr=pow(*ptr,val);
          else
            {
              std::ostringstream oss; oss << "DataArrayDouble::applyPow (double) : At elem # " << i << " value is " << *ptr << " ! must be >=0. !";
              throw INTERP_KERNEL::Exception(oss.str().c_str());
            }
        }
    }
  else
    {
      for(std::size_t i=0;i<nbOfElems;i++,ptr++)
        *ptr=pow(*ptr,val2);
    }
  declareAsNew();
}

/*!
 * Modify all elements of \a this array, so that
 * an element _x_ becomes \f$ val ^ x \f$.
 *  \param [in] val - the value used to apply pow on all array elements.
 *  \throw If \a this is not allocated.
 *  \throw If \a val < 0.
 *  \warning If an exception is thrown because of presence of 0 element in \a this 
 *           array, all elements processed before detection of the zero element remain
 *           modified.
 */
void DataArrayDouble::applyRPow(double val)
{
  checkAllocated();
  if(val<0.)
    throw INTERP_KERNEL::Exception("DataArrayDouble::applyRPow : the input value has to be >= 0 !");
  double *ptr=getPointer();
  std::size_t nbOfElems=getNbOfElems();
  for(std::size_t i=0;i<nbOfElems;i++,ptr++)
    *ptr=pow(val,*ptr);
  declareAsNew();
}

/*!
 * Returns a new DataArrayDouble created from \a this one by applying \a
 * FunctionToEvaluate to every tuple of \a this array. Textual data is not copied.
 * For more info see \ref MEDCouplingArrayApplyFunc
 *  \param [in] nbOfComp - number of components in the result array.
 *  \param [in] func - the \a FunctionToEvaluate declared as 
 *              \c bool (*\a func)(\c const \c double *\a pos, \c double *\a res), 
 *              where \a pos points to the first component of a tuple of \a this array
 *              and \a res points to the first component of a tuple of the result array.
 *              Note that length (number of components) of \a pos can differ from
 *              that of \a res.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble containing the
 *          same number of tuples as \a this array.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If \a this is not allocated.
 *  \throw If \a func returns \a false.
 */
DataArrayDouble *DataArrayDouble::applyFunc(int nbOfComp, FunctionToEvaluate func) const
{
  checkAllocated();
  DataArrayDouble *newArr=DataArrayDouble::New();
  int nbOfTuples=getNumberOfTuples();
  int oldNbOfComp=getNumberOfComponents();
  newArr->alloc(nbOfTuples,nbOfComp);
  const double *ptr=getConstPointer();
  double *ptrToFill=newArr->getPointer();
  for(int i=0;i<nbOfTuples;i++)
    {
      if(!func(ptr+i*oldNbOfComp,ptrToFill+i*nbOfComp))
        {
          std::ostringstream oss; oss << "For tuple # " << i << " with value (";
          std::copy(ptr+oldNbOfComp*i,ptr+oldNbOfComp*(i+1),std::ostream_iterator<double>(oss,", "));
          oss << ") : Evaluation of function failed !";
          newArr->decrRef();
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  return newArr;
}

/*!
 * Returns a new DataArrayDouble created from \a this one by applying a function to every
 * tuple of \a this array. Textual data is not copied.
 * For more info see \ref MEDCouplingArrayApplyFunc1.
 *  \param [in] nbOfComp - number of components in the result array.
 *  \param [in] func - the expression defining how to transform a tuple of \a this array.
 *              Supported expressions are described \ref MEDCouplingArrayApplyFuncExpr "here".
 *  \param [in] isSafe - By default true. If true invalid operation (division by 0. acos of value > 1. ...) leads to a throw of an exception.
 *              If false the computation is carried on without any notification. When false the evaluation is a little faster.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble containing the
 *          same number of tuples as \a this array and \a nbOfComp components.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If \a this is not allocated.
 *  \throw If computing \a func fails.
 */
DataArrayDouble *DataArrayDouble::applyFunc(int nbOfComp, const std::string& func, bool isSafe) const
{
  INTERP_KERNEL::ExprParser expr(func);
  expr.parse();
  std::set<std::string> vars;
  expr.getTrueSetOfVars(vars);
  std::vector<std::string> varsV(vars.begin(),vars.end());
  return applyFuncNamedCompo(nbOfComp,varsV,func,isSafe);
}

/*!
 * Returns a new DataArrayDouble created from \a this one by applying a function to every
 * tuple of \a this array. Textual data is not copied. This method works by tuples (whatever its size).
 * If \a this is a one component array, call applyFuncOnThis instead that performs the same work faster.
 *
 * For more info see \ref MEDCouplingArrayApplyFunc0.
 *  \param [in] func - the expression defining how to transform a tuple of \a this array.
 *              Supported expressions are described \ref MEDCouplingArrayApplyFuncExpr "here".
 *  \param [in] isSafe - By default true. If true invalid operation (division by 0. acos of value > 1. ...) leads to a throw of an exception.
 *                       If false the computation is carried on without any notification. When false the evaluation is a little faster.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble containing the
 *          same number of tuples and components as \a this array.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \sa applyFuncOnThis
 *  \throw If \a this is not allocated.
 *  \throw If computing \a func fails.
 */
DataArrayDouble *DataArrayDouble::applyFunc(const std::string& func, bool isSafe) const
{
  int nbOfComp(getNumberOfComponents());
  if(nbOfComp<=0)
    throw INTERP_KERNEL::Exception("DataArrayDouble::applyFunc : output number of component must be > 0 !");
  checkAllocated();
  int nbOfTuples(getNumberOfTuples());
  MCAuto<DataArrayDouble> newArr(DataArrayDouble::New());
  newArr->alloc(nbOfTuples,nbOfComp);
  INTERP_KERNEL::ExprParser expr(func);
  expr.parse();
  std::set<std::string> vars;
  expr.getTrueSetOfVars(vars);
  if((int)vars.size()>1)
    {
      std::ostringstream oss; oss << "DataArrayDouble::applyFunc : this method works only with at most one var func expression ! If you need to map comps on variables please use applyFuncCompo or applyFuncNamedCompo instead ! Vars in expr are : ";
      std::copy(vars.begin(),vars.end(),std::ostream_iterator<std::string>(oss," "));
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  if(vars.empty())
    {
      expr.prepareFastEvaluator();
      newArr->rearrange(1);
      newArr->fillWithValue(expr.evaluateDouble());
      newArr->rearrange(nbOfComp);
      return newArr.retn();
    }
  std::vector<std::string> vars2(vars.begin(),vars.end());
  double buff,*ptrToFill(newArr->getPointer());
  const double *ptr(begin());
  std::vector<double> stck;
  expr.prepareExprEvaluationDouble(vars2,1,1,0,&buff,&buff+1);
  expr.prepareFastEvaluator();
  if(!isSafe)
    {
      for(int i=0;i<nbOfTuples;i++)
        {
          for(int iComp=0;iComp<nbOfComp;iComp++,ptr++,ptrToFill++)
            {
              buff=*ptr;
              expr.evaluateDoubleInternal(stck);
              *ptrToFill=stck.back();
              stck.pop_back();
            }
        }
    }
  else
    {
      for(int i=0;i<nbOfTuples;i++)
        {
          for(int iComp=0;iComp<nbOfComp;iComp++,ptr++,ptrToFill++)
            {
              buff=*ptr;
              try
              {
                  expr.evaluateDoubleInternalSafe(stck);
              }
              catch(INTERP_KERNEL::Exception& e)
              {
                  std::ostringstream oss; oss << "For tuple # " << i << " component # " << iComp << " with value (";
                  oss << buff;
                  oss << ") : Evaluation of function failed !" << e.what();
                  throw INTERP_KERNEL::Exception(oss.str().c_str());
              }
              *ptrToFill=stck.back();
              stck.pop_back();
            }
        }
    }
  return newArr.retn();
}

/*!
 * This method is a non const method that modify the array in \a this.
 * This method only works on one component array. It means that function \a func must
 * contain at most one variable.
 * This method is a specialization of applyFunc method with one parameter on one component array.
 *
 *  \param [in] func - the expression defining how to transform a tuple of \a this array.
 *              Supported expressions are described \ref MEDCouplingArrayApplyFuncExpr "here".
 *  \param [in] isSafe - By default true. If true invalid operation (division by 0. acos of value > 1. ...) leads to a throw of an exception.
 *              If false the computation is carried on without any notification. When false the evaluation is a little faster.
 *
 * \sa applyFunc
 */
void DataArrayDouble::applyFuncOnThis(const std::string& func, bool isSafe)
{
  int nbOfComp(getNumberOfComponents());
  if(nbOfComp<=0)
    throw INTERP_KERNEL::Exception("DataArrayDouble::applyFuncOnThis : output number of component must be > 0 !");
  checkAllocated();
  int nbOfTuples(getNumberOfTuples());
  INTERP_KERNEL::ExprParser expr(func);
  expr.parse();
  std::set<std::string> vars;
  expr.getTrueSetOfVars(vars);
  if((int)vars.size()>1)
    {
      std::ostringstream oss; oss << "DataArrayDouble::applyFuncOnThis : this method works only with at most one var func expression ! If you need to map comps on variables please use applyFuncCompo or applyFuncNamedCompo instead ! Vars in expr are : ";
      std::copy(vars.begin(),vars.end(),std::ostream_iterator<std::string>(oss," "));
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  if(vars.empty())
    {
      expr.prepareFastEvaluator();
      std::vector<std::string> compInfo(getInfoOnComponents());
      rearrange(1);
      fillWithValue(expr.evaluateDouble());
      rearrange(nbOfComp);
      setInfoOnComponents(compInfo);
      return ;
    }
  std::vector<std::string> vars2(vars.begin(),vars.end());
  double buff,*ptrToFill(getPointer());
  const double *ptr(begin());
  std::vector<double> stck;
  expr.prepareExprEvaluationDouble(vars2,1,1,0,&buff,&buff+1);
  expr.prepareFastEvaluator();
  if(!isSafe)
    {
      for(int i=0;i<nbOfTuples;i++)
        {
          for(int iComp=0;iComp<nbOfComp;iComp++,ptr++,ptrToFill++)
            {
              buff=*ptr;
              expr.evaluateDoubleInternal(stck);
              *ptrToFill=stck.back();
              stck.pop_back();
            }
        }
    }
  else
    {
      for(int i=0;i<nbOfTuples;i++)
        {
          for(int iComp=0;iComp<nbOfComp;iComp++,ptr++,ptrToFill++)
            {
              buff=*ptr;
              try
              {
                  expr.evaluateDoubleInternalSafe(stck);
              }
              catch(INTERP_KERNEL::Exception& e)
              {
                  std::ostringstream oss; oss << "For tuple # " << i << " component # " << iComp << " with value (";
                  oss << buff;
                  oss << ") : Evaluation of function failed !" << e.what();
                  throw INTERP_KERNEL::Exception(oss.str().c_str());
              }
              *ptrToFill=stck.back();
              stck.pop_back();
            }
        }
    }
}

/*!
 * Returns a new DataArrayDouble created from \a this one by applying a function to every
 * tuple of \a this array. Textual data is not copied.
 * For more info see \ref MEDCouplingArrayApplyFunc2.
 *  \param [in] nbOfComp - number of components in the result array.
 *  \param [in] func - the expression defining how to transform a tuple of \a this array.
 *              Supported expressions are described \ref MEDCouplingArrayApplyFuncExpr "here".
 *  \param [in] isSafe - By default true. If true invalid operation (division by 0. acos of value > 1. ...) leads to a throw of an exception.
 *              If false the computation is carried on without any notification. When false the evaluation is a little faster.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble containing the
 *          same number of tuples as \a this array.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If \a this is not allocated.
 *  \throw If \a func contains vars that are not in \a this->getInfoOnComponent().
 *  \throw If computing \a func fails.
 */
DataArrayDouble *DataArrayDouble::applyFuncCompo(int nbOfComp, const std::string& func, bool isSafe) const
{
  return applyFuncNamedCompo(nbOfComp,getVarsOnComponent(),func,isSafe);
}

/*!
 * Returns a new DataArrayDouble created from \a this one by applying a function to every
 * tuple of \a this array. Textual data is not copied.
 * For more info see \ref MEDCouplingArrayApplyFunc3.
 *  \param [in] nbOfComp - number of components in the result array.
 *  \param [in] varsOrder - sequence of vars defining their order.
 *  \param [in] func - the expression defining how to transform a tuple of \a this array.
 *              Supported expressions are described \ref MEDCouplingArrayApplyFuncExpr "here".
 *  \param [in] isSafe - By default true. If true invalid operation (division by 0. acos of value > 1. ...) leads to a throw of an exception.
 *              If false the computation is carried on without any notification. When false the evaluation is a little faster.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble containing the
 *          same number of tuples as \a this array.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If \a this is not allocated.
 *  \throw If \a func contains vars not in \a varsOrder.
 *  \throw If computing \a func fails.
 */
DataArrayDouble *DataArrayDouble::applyFuncNamedCompo(int nbOfComp, const std::vector<std::string>& varsOrder, const std::string& func, bool isSafe) const
{
  if(nbOfComp<=0)
    throw INTERP_KERNEL::Exception("DataArrayDouble::applyFuncNamedCompo : output number of component must be > 0 !");
  std::vector<std::string> varsOrder2(varsOrder);
  int oldNbOfComp(getNumberOfComponents());
  for(int i=(int)varsOrder.size();i<oldNbOfComp;i++)
    varsOrder2.push_back(std::string());
  checkAllocated();
  int nbOfTuples(getNumberOfTuples());
  INTERP_KERNEL::ExprParser expr(func);
  expr.parse();
  std::set<std::string> vars;
  expr.getTrueSetOfVars(vars);
  if((int)vars.size()>oldNbOfComp)
    {
      std::ostringstream oss; oss << "The field has " << oldNbOfComp << " components and there are ";
      oss << vars.size() << " variables : ";
      std::copy(vars.begin(),vars.end(),std::ostream_iterator<std::string>(oss," "));
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  MCAuto<DataArrayDouble> newArr(DataArrayDouble::New());
  newArr->alloc(nbOfTuples,nbOfComp);
  INTERP_KERNEL::AutoPtr<double> buff(new double[oldNbOfComp]);
  double *buffPtr(buff),*ptrToFill;
  std::vector<double> stck;
  for(int iComp=0;iComp<nbOfComp;iComp++)
    {
      expr.prepareExprEvaluationDouble(varsOrder2,oldNbOfComp,nbOfComp,iComp,buffPtr,buffPtr+oldNbOfComp);
      expr.prepareFastEvaluator();
      const double *ptr(getConstPointer());
      ptrToFill=newArr->getPointer()+iComp;
      if(!isSafe)
        {
          for(int i=0;i<nbOfTuples;i++,ptrToFill+=nbOfComp,ptr+=oldNbOfComp)
            {
              std::copy(ptr,ptr+oldNbOfComp,buffPtr);
              expr.evaluateDoubleInternal(stck);
              *ptrToFill=stck.back();
              stck.pop_back();
            }
        }
      else
        {
          for(int i=0;i<nbOfTuples;i++,ptrToFill+=nbOfComp,ptr+=oldNbOfComp)
            {
              std::copy(ptr,ptr+oldNbOfComp,buffPtr);
              try
              {
                  expr.evaluateDoubleInternalSafe(stck);
                  *ptrToFill=stck.back();
                  stck.pop_back();
              }
              catch(INTERP_KERNEL::Exception& e)
              {
                  std::ostringstream oss; oss << "For tuple # " << i << " with value (";
                  std::copy(ptr+oldNbOfComp*i,ptr+oldNbOfComp*(i+1),std::ostream_iterator<double>(oss,", "));
                  oss << ") : Evaluation of function failed !" << e.what();
                  throw INTERP_KERNEL::Exception(oss.str().c_str());
              }
            }
        }
    }
  return newArr.retn();
}

void DataArrayDouble::applyFuncFast32(const std::string& func)
{
  checkAllocated();
  INTERP_KERNEL::ExprParser expr(func);
  expr.parse();
  char *funcStr=expr.compileX86();
  MYFUNCPTR funcPtr;
  *((void **)&funcPtr)=funcStr;//he he...
  //
  double *ptr=getPointer();
  int nbOfComp=getNumberOfComponents();
  int nbOfTuples=getNumberOfTuples();
  int nbOfElems=nbOfTuples*nbOfComp;
  for(int i=0;i<nbOfElems;i++,ptr++)
    *ptr=funcPtr(*ptr);
  declareAsNew();
}

void DataArrayDouble::applyFuncFast64(const std::string& func)
{
  checkAllocated();
  INTERP_KERNEL::ExprParser expr(func);
  expr.parse();
  char *funcStr=expr.compileX86_64();
  MYFUNCPTR funcPtr;
  *((void **)&funcPtr)=funcStr;//he he...
  //
  double *ptr=getPointer();
  int nbOfComp=getNumberOfComponents();
  int nbOfTuples=getNumberOfTuples();
  int nbOfElems=nbOfTuples*nbOfComp;
  for(int i=0;i<nbOfElems;i++,ptr++)
    *ptr=funcPtr(*ptr);
  declareAsNew();
}

/*!
 * \return a new object that is the result of the symmetry along 3D plane defined by its normal vector \a normalVector and a point \a point.
 */
MCAuto<DataArrayDouble> DataArrayDouble::symmetry3DPlane(const double point[3], const double normalVector[3]) const
{
  checkAllocated();
  if(getNumberOfComponents()!=3)
    throw INTERP_KERNEL::Exception("DataArrayDouble::symmetry3DPlane : this is excepted to have 3 components !");
  int nbTuples(getNumberOfTuples());
  MCAuto<DataArrayDouble> ret(DataArrayDouble::New());
  ret->alloc(nbTuples,3);
  Symmetry3DPlane(point,normalVector,nbTuples,begin(),ret->getPointer());
  return ret;
}

DataArrayDoubleIterator *DataArrayDouble::iterator()
{
  return new DataArrayDoubleIterator(this);
}

/*!
 * Returns a new DataArrayInt containing indices of tuples of \a this one-dimensional
 * array whose values are within a given range. Textual data is not copied.
 *  \param [in] vmin - a lowest acceptable value (included).
 *  \param [in] vmax - a greatest acceptable value (included).
 *  \return DataArrayInt * - the new instance of DataArrayInt.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If \a this->getNumberOfComponents() != 1.
 *
 *  \sa DataArrayDouble::findIdsNotInRange
 *
 *  \if ENABLE_EXAMPLES
 *  \ref cpp_mcdataarraydouble_getidsinrange "Here is a C++ example".<br>
 *  \ref py_mcdataarraydouble_getidsinrange "Here is a Python example".
 *  \endif
 */
DataArrayInt *DataArrayDouble::findIdsInRange(double vmin, double vmax) const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayDouble::findIdsInRange : this must have exactly one component !");
  const double *cptr(begin());
  MCAuto<DataArrayInt> ret(DataArrayInt::New()); ret->alloc(0,1);
  int nbOfTuples(getNumberOfTuples());
  for(int i=0;i<nbOfTuples;i++,cptr++)
    if(*cptr>=vmin && *cptr<=vmax)
      ret->pushBackSilent(i);
  return ret.retn();
}

/*!
 * Returns a new DataArrayInt containing indices of tuples of \a this one-dimensional
 * array whose values are not within a given range. Textual data is not copied.
 *  \param [in] vmin - a lowest not acceptable value (excluded).
 *  \param [in] vmax - a greatest not acceptable value (excluded).
 *  \return DataArrayInt * - the new instance of DataArrayInt.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If \a this->getNumberOfComponents() != 1.
 *
 *  \sa DataArrayDouble::findIdsInRange
 */
DataArrayInt *DataArrayDouble::findIdsNotInRange(double vmin, double vmax) const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayDouble::findIdsNotInRange : this must have exactly one component !");
  const double *cptr(begin());
  MCAuto<DataArrayInt> ret(DataArrayInt::New()); ret->alloc(0,1);
  int nbOfTuples(getNumberOfTuples());
  for(int i=0;i<nbOfTuples;i++,cptr++)
    if(*cptr<vmin || *cptr>vmax)
      ret->pushBackSilent(i);
  return ret.retn();
}

/*!
 * Returns a new DataArrayDouble by concatenating two given arrays, so that (1) the number
 * of tuples in the result array is a sum of the number of tuples of given arrays and (2)
 * the number of component in the result array is same as that of each of given arrays.
 * Info on components is copied from the first of the given arrays. Number of components
 * in the given arrays must be  the same.
 *  \param [in] a1 - an array to include in the result array.
 *  \param [in] a2 - another array to include in the result array.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If both \a a1 and \a a2 are NULL.
 *  \throw If \a a1->getNumberOfComponents() != \a a2->getNumberOfComponents().
 */
DataArrayDouble *DataArrayDouble::Aggregate(const DataArrayDouble *a1, const DataArrayDouble *a2)
{
  std::vector<const DataArrayDouble *> tmp(2);
  tmp[0]=a1; tmp[1]=a2;
  return Aggregate(tmp);
}

/*!
 * Returns a new DataArrayDouble by concatenating all given arrays, so that (1) the number
 * of tuples in the result array is a sum of the number of tuples of given arrays and (2)
 * the number of component in the result array is same as that of each of given arrays.
 * Info on components is copied from the first of the given arrays. Number of components
 * in the given arrays must be  the same.
 * If the number of non null of elements in \a arr is equal to one the returned object is a copy of it
 * not the object itself.
 *  \param [in] arr - a sequence of arrays to include in the result array.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If all arrays within \a arr are NULL.
 *  \throw If getNumberOfComponents() of arrays within \a arr.
 */
DataArrayDouble *DataArrayDouble::Aggregate(const std::vector<const DataArrayDouble *>& arr)
{
  std::vector<const DataArrayDouble *> a;
  for(std::vector<const DataArrayDouble *>::const_iterator it4=arr.begin();it4!=arr.end();it4++)
    if(*it4)
      a.push_back(*it4);
  if(a.empty())
    throw INTERP_KERNEL::Exception("DataArrayDouble::Aggregate : input list must contain at least one NON EMPTY DataArrayDouble !");
  std::vector<const DataArrayDouble *>::const_iterator it=a.begin();
  std::size_t nbOfComp((*it)->getNumberOfComponents());
  int nbt=(*it++)->getNumberOfTuples();
  for(int i=1;it!=a.end();it++,i++)
    {
      if((*it)->getNumberOfComponents()!=nbOfComp)
        throw INTERP_KERNEL::Exception("DataArrayDouble::Aggregate : Nb of components mismatch for array aggregation !");
      nbt+=(*it)->getNumberOfTuples();
    }
  MCAuto<DataArrayDouble> ret=DataArrayDouble::New();
  ret->alloc(nbt,nbOfComp);
  double *pt=ret->getPointer();
  for(it=a.begin();it!=a.end();it++)
    pt=std::copy((*it)->getConstPointer(),(*it)->getConstPointer()+(*it)->getNbOfElems(),pt);
  ret->copyStringInfoFrom(*(a[0]));
  return ret.retn();
}

/*!
 * Returns a new DataArrayDouble containing a dot product of two given arrays, so that
 * the i-th tuple of the result array is a sum of products of j-th components of i-th
 * tuples of given arrays (\f$ a_i = \sum_{j=1}^n a1_j * a2_j \f$).
 * Info on components and name is copied from the first of the given arrays.
 * Number of tuples and components in the given arrays must be the same.
 *  \param [in] a1 - a given array.
 *  \param [in] a2 - another given array.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If either \a a1 or \a a2 is NULL.
 *  \throw If any given array is not allocated.
 *  \throw If \a a1->getNumberOfTuples() != \a a2->getNumberOfTuples()
 *  \throw If \a a1->getNumberOfComponents() != \a a2->getNumberOfComponents()
 */
DataArrayDouble *DataArrayDouble::Dot(const DataArrayDouble *a1, const DataArrayDouble *a2)
{
  if(!a1 || !a2)
    throw INTERP_KERNEL::Exception("DataArrayDouble::Dot : input DataArrayDouble instance is NULL !");
  a1->checkAllocated();
  a2->checkAllocated();
  std::size_t nbOfComp(a1->getNumberOfComponents());
  if(nbOfComp!=a2->getNumberOfComponents())
    throw INTERP_KERNEL::Exception("Nb of components mismatch for array Dot !");
  std::size_t nbOfTuple(a1->getNumberOfTuples());
  if(nbOfTuple!=a2->getNumberOfTuples())
    throw INTERP_KERNEL::Exception("Nb of tuples mismatch for array Dot !");
  DataArrayDouble *ret=DataArrayDouble::New();
  ret->alloc(nbOfTuple,1);
  double *retPtr=ret->getPointer();
  const double *a1Ptr=a1->begin(),*a2Ptr(a2->begin());
  for(std::size_t i=0;i<nbOfTuple;i++)
    {
      double sum=0.;
      for(std::size_t j=0;j<nbOfComp;j++)
        sum+=a1Ptr[i*nbOfComp+j]*a2Ptr[i*nbOfComp+j];
      retPtr[i]=sum;
    }
  ret->setInfoOnComponent(0,a1->getInfoOnComponent(0));
  ret->setName(a1->getName());
  return ret;
}

/*!
 * Returns a new DataArrayDouble containing a cross product of two given arrays, so that
 * the i-th tuple of the result array contains 3 components of a vector which is a cross
 * product of two vectors defined by the i-th tuples of given arrays.
 * Info on components is copied from the first of the given arrays.
 * Number of tuples in the given arrays must be the same.
 * Number of components in the given arrays must be 3.
 *  \param [in] a1 - a given array.
 *  \param [in] a2 - another given array.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If either \a a1 or \a a2 is NULL.
 *  \throw If \a a1->getNumberOfTuples() != \a a2->getNumberOfTuples()
 *  \throw If \a a1->getNumberOfComponents() != 3
 *  \throw If \a a2->getNumberOfComponents() != 3
 */
DataArrayDouble *DataArrayDouble::CrossProduct(const DataArrayDouble *a1, const DataArrayDouble *a2)
{
  if(!a1 || !a2)
    throw INTERP_KERNEL::Exception("DataArrayDouble::CrossProduct : input DataArrayDouble instance is NULL !");
  std::size_t nbOfComp(a1->getNumberOfComponents());
  if(nbOfComp!=a2->getNumberOfComponents())
    throw INTERP_KERNEL::Exception("Nb of components mismatch for array crossProduct !");
  if(nbOfComp!=3)
    throw INTERP_KERNEL::Exception("Nb of components must be equal to 3 for array crossProduct !");
  std::size_t nbOfTuple(a1->getNumberOfTuples());
  if(nbOfTuple!=a2->getNumberOfTuples())
    throw INTERP_KERNEL::Exception("Nb of tuples mismatch for array crossProduct !");
  DataArrayDouble *ret=DataArrayDouble::New();
  ret->alloc(nbOfTuple,3);
  double *retPtr=ret->getPointer();
  const double *a1Ptr(a1->begin()),*a2Ptr(a2->begin());
  for(std::size_t i=0;i<nbOfTuple;i++)
    {
      retPtr[3*i]=a1Ptr[3*i+1]*a2Ptr[3*i+2]-a1Ptr[3*i+2]*a2Ptr[3*i+1];
      retPtr[3*i+1]=a1Ptr[3*i+2]*a2Ptr[3*i]-a1Ptr[3*i]*a2Ptr[3*i+2];
      retPtr[3*i+2]=a1Ptr[3*i]*a2Ptr[3*i+1]-a1Ptr[3*i+1]*a2Ptr[3*i];
    }
  ret->copyStringInfoFrom(*a1);
  return ret;
}

/*!
 * Returns a new DataArrayDouble containing maximal values of two given arrays.
 * Info on components is copied from the first of the given arrays.
 * Number of tuples and components in the given arrays must be the same.
 *  \param [in] a1 - an array to compare values with another one.
 *  \param [in] a2 - another array to compare values with the first one.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If either \a a1 or \a a2 is NULL.
 *  \throw If \a a1->getNumberOfTuples() != \a a2->getNumberOfTuples()
 *  \throw If \a a1->getNumberOfComponents() != \a a2->getNumberOfComponents()
 */
DataArrayDouble *DataArrayDouble::Max(const DataArrayDouble *a1, const DataArrayDouble *a2)
{
  if(!a1 || !a2)
    throw INTERP_KERNEL::Exception("DataArrayDouble::Max : input DataArrayDouble instance is NULL !");
  std::size_t nbOfComp(a1->getNumberOfComponents());
  if(nbOfComp!=a2->getNumberOfComponents())
    throw INTERP_KERNEL::Exception("Nb of components mismatch for array Max !");
  std::size_t nbOfTuple(a1->getNumberOfTuples());
  if(nbOfTuple!=a2->getNumberOfTuples())
    throw INTERP_KERNEL::Exception("Nb of tuples mismatch for array Max !");
  MCAuto<DataArrayDouble> ret(DataArrayDouble::New());
  ret->alloc(nbOfTuple,nbOfComp);
  double *retPtr(ret->getPointer());
  const double *a1Ptr(a1->begin()),*a2Ptr(a2->begin());
  std::size_t nbElem(nbOfTuple*nbOfComp);
  for(std::size_t i=0;i<nbElem;i++)
    retPtr[i]=std::max(a1Ptr[i],a2Ptr[i]);
  ret->copyStringInfoFrom(*a1);
  return ret.retn();
}

/*!
 * Returns a new DataArrayDouble containing minimal values of two given arrays.
 * Info on components is copied from the first of the given arrays.
 * Number of tuples and components in the given arrays must be the same.
 *  \param [in] a1 - an array to compare values with another one.
 *  \param [in] a2 - another array to compare values with the first one.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If either \a a1 or \a a2 is NULL.
 *  \throw If \a a1->getNumberOfTuples() != \a a2->getNumberOfTuples()
 *  \throw If \a a1->getNumberOfComponents() != \a a2->getNumberOfComponents()
 */
DataArrayDouble *DataArrayDouble::Min(const DataArrayDouble *a1, const DataArrayDouble *a2)
{
  if(!a1 || !a2)
    throw INTERP_KERNEL::Exception("DataArrayDouble::Min : input DataArrayDouble instance is NULL !");
  std::size_t nbOfComp(a1->getNumberOfComponents());
  if(nbOfComp!=a2->getNumberOfComponents())
    throw INTERP_KERNEL::Exception("Nb of components mismatch for array min !");
  std::size_t nbOfTuple(a1->getNumberOfTuples());
  if(nbOfTuple!=a2->getNumberOfTuples())
    throw INTERP_KERNEL::Exception("Nb of tuples mismatch for array min !");
  MCAuto<DataArrayDouble> ret(DataArrayDouble::New());
  ret->alloc(nbOfTuple,nbOfComp);
  double *retPtr(ret->getPointer());
  const double *a1Ptr(a1->begin()),*a2Ptr(a2->begin());
  std::size_t nbElem(nbOfTuple*nbOfComp);
  for(std::size_t i=0;i<nbElem;i++)
    retPtr[i]=std::min(a1Ptr[i],a2Ptr[i]);
  ret->copyStringInfoFrom(*a1);
  return ret.retn();
}

/*!
 * Returns a new DataArrayDouble that is the result of pow of two given arrays. There are 3
 * valid cases.
 *
 *  \param [in] a1 - an array to pow up.
 *  \param [in] a2 - another array to sum up.
 *  \return DataArrayDouble * - the new instance of DataArrayDouble.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If either \a a1 or \a a2 is NULL.
 *  \throw If \a a1->getNumberOfTuples() != \a a2->getNumberOfTuples()
 *  \throw If \a a1->getNumberOfComponents() != 1 or \a a2->getNumberOfComponents() != 1.
 *  \throw If there is a negative value in \a a1.
 */
DataArrayDouble *DataArrayDouble::Pow(const DataArrayDouble *a1, const DataArrayDouble *a2)
{
  if(!a1 || !a2)
    throw INTERP_KERNEL::Exception("DataArrayDouble::Pow : at least one of input instances is null !");
  int nbOfTuple=a1->getNumberOfTuples();
  int nbOfTuple2=a2->getNumberOfTuples();
  int nbOfComp=a1->getNumberOfComponents();
  int nbOfComp2=a2->getNumberOfComponents();
  if(nbOfTuple!=nbOfTuple2)
    throw INTERP_KERNEL::Exception("DataArrayDouble::Pow : number of tuples mismatches !");
  if(nbOfComp!=1 || nbOfComp2!=1)
    throw INTERP_KERNEL::Exception("DataArrayDouble::Pow : number of components of both arrays must be equal to 1 !");
  MCAuto<DataArrayDouble> ret=DataArrayDouble::New(); ret->alloc(nbOfTuple,1);
  const double *ptr1(a1->begin()),*ptr2(a2->begin());
  double *ptr=ret->getPointer();
  for(int i=0;i<nbOfTuple;i++,ptr1++,ptr2++,ptr++)
    {
      if(*ptr1>=0)
        {
          *ptr=pow(*ptr1,*ptr2);
        }
      else
        {
          std::ostringstream oss; oss << "DataArrayDouble::Pow : on tuple #" << i << " of a1 value is < 0 (" << *ptr1 << ") !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  return ret.retn();
}

/*!
 * Apply pow on values of another DataArrayDouble to values of \a this one.
 *
 *  \param [in] other - an array to pow to \a this one.
 *  \throw If \a other is NULL.
 *  \throw If \a this->getNumberOfTuples() != \a other->getNumberOfTuples()
 *  \throw If \a this->getNumberOfComponents() != 1 or \a other->getNumberOfComponents() != 1
 *  \throw If there is a negative value in \a this.
 */
void DataArrayDouble::powEqual(const DataArrayDouble *other)
{
  if(!other)
    throw INTERP_KERNEL::Exception("DataArrayDouble::powEqual : input instance is null !");
  int nbOfTuple=getNumberOfTuples();
  int nbOfTuple2=other->getNumberOfTuples();
  int nbOfComp=getNumberOfComponents();
  int nbOfComp2=other->getNumberOfComponents();
  if(nbOfTuple!=nbOfTuple2)
    throw INTERP_KERNEL::Exception("DataArrayDouble::powEqual : number of tuples mismatches !");
  if(nbOfComp!=1 || nbOfComp2!=1)
    throw INTERP_KERNEL::Exception("DataArrayDouble::powEqual : number of components of both arrays must be equal to 1 !");
  double *ptr=getPointer();
  const double *ptrc=other->begin();
  for(int i=0;i<nbOfTuple;i++,ptrc++,ptr++)
    {
      if(*ptr>=0)
        *ptr=pow(*ptr,*ptrc);
      else
        {
          std::ostringstream oss; oss << "DataArrayDouble::powEqual : on tuple #" << i << " of this value is < 0 (" << *ptr << ") !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  declareAsNew();
}

/*!
 * This method is \b NOT wrapped into python because it can be useful only for performance reasons in C++ context.
 * All values in \a this must be 0. or 1. within eps error. 0 means false, 1 means true.
 * If an another value than 0 or 1 appear (within eps precision) an INTERP_KERNEL::Exception will be thrown.
 *
 * \throw if \a this is not allocated.
 * \throw if \a this has not exactly one component.
 */
std::vector<bool> DataArrayDouble::toVectorOfBool(double eps) const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayDouble::toVectorOfBool : must be applied on single component array !");
  int nbt(getNumberOfTuples());
  std::vector<bool> ret(nbt);
  const double *pt(begin());
  for(int i=0;i<nbt;i++)
    {
      if(fabs(pt[i])<eps)
        ret[i]=false;
      else if(fabs(pt[i]-1.)<eps)
        ret[i]=true;
      else
        {
          std::ostringstream oss; oss << "DataArrayDouble::toVectorOfBool : the tuple #" << i << " has value " << pt[i] << " is invalid ! must be 0. or 1. !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  return ret;
}

/*!
 * Useless method for end user. Only for MPI/Corba/File serialsation for multi arrays class.
 * Server side.
 */
void DataArrayDouble::getTinySerializationIntInformation(std::vector<int>& tinyInfo) const
{
  tinyInfo.resize(2);
  if(isAllocated())
    {
      tinyInfo[0]=getNumberOfTuples();
      tinyInfo[1]=getNumberOfComponents();
    }
  else
    {
      tinyInfo[0]=-1;
      tinyInfo[1]=-1;
    }
}

/*!
 * Useless method for end user. Only for MPI/Corba/File serialsation for multi arrays class.
 * Server side.
 */
void DataArrayDouble::getTinySerializationStrInformation(std::vector<std::string>& tinyInfo) const
{
  if(isAllocated())
    {
      int nbOfCompo=getNumberOfComponents();
      tinyInfo.resize(nbOfCompo+1);
      tinyInfo[0]=getName();
      for(int i=0;i<nbOfCompo;i++)
        tinyInfo[i+1]=getInfoOnComponent(i);
    }
  else
    {
      tinyInfo.resize(1);
      tinyInfo[0]=getName();
    }
}

/*!
 * Useless method for end user. Only for MPI/Corba/File serialsation for multi arrays class.
 * This method returns if a feeding is needed.
 */
bool DataArrayDouble::resizeForUnserialization(const std::vector<int>& tinyInfoI)
{
  int nbOfTuple=tinyInfoI[0];
  int nbOfComp=tinyInfoI[1];
  if(nbOfTuple!=-1 || nbOfComp!=-1)
    {
      alloc(nbOfTuple,nbOfComp);
      return true;
    }
  return false;
}

/*!
 * Useless method for end user. Only for MPI/Corba/File serialsation for multi arrays class.
 */
void DataArrayDouble::finishUnserialization(const std::vector<int>& tinyInfoI, const std::vector<std::string>& tinyInfoS)
{
  setName(tinyInfoS[0]);
  if(isAllocated())
    {
      int nbOfCompo=getNumberOfComponents();
      for(int i=0;i<nbOfCompo;i++)
        setInfoOnComponent(i,tinyInfoS[i+1]);
    }
}

/*!
 * Low static method that operates 3D rotation of 'nbNodes' 3D nodes whose coordinates are arranged in \a coordsIn
 * around an axe ( \a center, \a vect) and with angle \a angle.
 */
void DataArrayDouble::Rotate3DAlg(const double *center, const double *vect, double angle, int nbNodes, const double *coordsIn, double *coordsOut)
{
  if(!center || !vect)
    throw INTERP_KERNEL::Exception("DataArrayDouble::Rotate3DAlg : null vector in input !");
  double sina(sin(angle));
  double cosa(cos(angle));
  double vectorNorm[3];
  double matrix[9];
  double matrixTmp[9];
  double norm(sqrt(vect[0]*vect[0]+vect[1]*vect[1]+vect[2]*vect[2]));
  if(norm<std::numeric_limits<double>::min())
    throw INTERP_KERNEL::Exception("DataArrayDouble::Rotate3DAlg : magnitude of input vector is too close of 0. !");
  std::transform(vect,vect+3,vectorNorm,std::bind2nd(std::multiplies<double>(),1/norm));
  //rotation matrix computation
  matrix[0]=cosa; matrix[1]=0.; matrix[2]=0.; matrix[3]=0.; matrix[4]=cosa; matrix[5]=0.; matrix[6]=0.; matrix[7]=0.; matrix[8]=cosa;
  matrixTmp[0]=vectorNorm[0]*vectorNorm[0]; matrixTmp[1]=vectorNorm[0]*vectorNorm[1]; matrixTmp[2]=vectorNorm[0]*vectorNorm[2];
  matrixTmp[3]=vectorNorm[1]*vectorNorm[0]; matrixTmp[4]=vectorNorm[1]*vectorNorm[1]; matrixTmp[5]=vectorNorm[1]*vectorNorm[2];
  matrixTmp[6]=vectorNorm[2]*vectorNorm[0]; matrixTmp[7]=vectorNorm[2]*vectorNorm[1]; matrixTmp[8]=vectorNorm[2]*vectorNorm[2];
  std::transform(matrixTmp,matrixTmp+9,matrixTmp,std::bind2nd(std::multiplies<double>(),1-cosa));
  std::transform(matrix,matrix+9,matrixTmp,matrix,std::plus<double>());
  matrixTmp[0]=0.; matrixTmp[1]=-vectorNorm[2]; matrixTmp[2]=vectorNorm[1];
  matrixTmp[3]=vectorNorm[2]; matrixTmp[4]=0.; matrixTmp[5]=-vectorNorm[0];
  matrixTmp[6]=-vectorNorm[1]; matrixTmp[7]=vectorNorm[0]; matrixTmp[8]=0.;
  std::transform(matrixTmp,matrixTmp+9,matrixTmp,std::bind2nd(std::multiplies<double>(),sina));
  std::transform(matrix,matrix+9,matrixTmp,matrix,std::plus<double>());
  //rotation matrix computed.
  double tmp[3];
  for(int i=0; i<nbNodes; i++)
    {
      std::transform(coordsIn+i*3,coordsIn+(i+1)*3,center,tmp,std::minus<double>());
      coordsOut[i*3]=matrix[0]*tmp[0]+matrix[1]*tmp[1]+matrix[2]*tmp[2]+center[0];
      coordsOut[i*3+1]=matrix[3]*tmp[0]+matrix[4]*tmp[1]+matrix[5]*tmp[2]+center[1];
      coordsOut[i*3+2]=matrix[6]*tmp[0]+matrix[7]*tmp[1]+matrix[8]*tmp[2]+center[2];
    }
}

void DataArrayDouble::Symmetry3DPlane(const double point[3], const double normalVector[3], int nbNodes, const double *coordsIn, double *coordsOut)
{
  double matrix[9],matrix2[9],matrix3[9];
  double vect[3],crossVect[3];
  INTERP_KERNEL::orthogonalVect3(normalVector,vect);
  crossVect[0]=normalVector[1]*vect[2]-normalVector[2]*vect[1];
  crossVect[1]=normalVector[2]*vect[0]-normalVector[0]*vect[2];
  crossVect[2]=normalVector[0]*vect[1]-normalVector[1]*vect[0];
  double nv(INTERP_KERNEL::norm<3>(vect)),ni(INTERP_KERNEL::norm<3>(normalVector)),nc(INTERP_KERNEL::norm<3>(crossVect));
  matrix[0]=vect[0]/nv; matrix[1]=crossVect[0]/nc; matrix[2]=-normalVector[0]/ni;
  matrix[3]=vect[1]/nv; matrix[4]=crossVect[1]/nc; matrix[5]=-normalVector[1]/ni;
  matrix[6]=vect[2]/nv; matrix[7]=crossVect[2]/nc; matrix[8]=-normalVector[2]/ni;
  matrix2[0]=vect[0]/nv; matrix2[1]=vect[1]/nv; matrix2[2]=vect[2]/nv;
  matrix2[3]=crossVect[0]/nc; matrix2[4]=crossVect[1]/nc; matrix2[5]=crossVect[2]/nc;
  matrix2[6]=normalVector[0]/ni; matrix2[7]=normalVector[1]/ni; matrix2[8]=normalVector[2]/ni;
  for(int i=0;i<3;i++)
    for(int j=0;j<3;j++)
      {
        double val(0.);
        for(int k=0;k<3;k++)
          val+=matrix[3*i+k]*matrix2[3*k+j];
        matrix3[3*i+j]=val;
      }
  //rotation matrix computed.
  double tmp[3];
  for(int i=0; i<nbNodes; i++)
    {
      std::transform(coordsIn+i*3,coordsIn+(i+1)*3,point,tmp,std::minus<double>());
      coordsOut[i*3]=matrix3[0]*tmp[0]+matrix3[1]*tmp[1]+matrix3[2]*tmp[2]+point[0];
      coordsOut[i*3+1]=matrix3[3]*tmp[0]+matrix3[4]*tmp[1]+matrix3[5]*tmp[2]+point[1];
      coordsOut[i*3+2]=matrix3[6]*tmp[0]+matrix3[7]*tmp[1]+matrix3[8]*tmp[2]+point[2];
    }
}

void DataArrayDouble::GiveBaseForPlane(const double normalVector[3], double baseOfPlane[9])
{
  double vect[3],crossVect[3];
  INTERP_KERNEL::orthogonalVect3(normalVector,vect);
  crossVect[0]=normalVector[1]*vect[2]-normalVector[2]*vect[1];
  crossVect[1]=normalVector[2]*vect[0]-normalVector[0]*vect[2];
  crossVect[2]=normalVector[0]*vect[1]-normalVector[1]*vect[0];
  double nv(INTERP_KERNEL::norm<3>(vect)),ni(INTERP_KERNEL::norm<3>(normalVector)),nc(INTERP_KERNEL::norm<3>(crossVect));
  baseOfPlane[0]=vect[0]/nv; baseOfPlane[1]=vect[1]/nv; baseOfPlane[2]=vect[2]/nv;
  baseOfPlane[3]=crossVect[0]/nc; baseOfPlane[4]=crossVect[1]/nc; baseOfPlane[5]=crossVect[2]/nc;
  baseOfPlane[6]=normalVector[0]/ni; baseOfPlane[7]=normalVector[1]/ni; baseOfPlane[8]=normalVector[2]/ni;
}

/*!
 * Low static method that operates 3D rotation of \a nbNodes 3D nodes whose coordinates are arranged in \a coords
 * around the center point \a center and with angle \a angle.
 */
void DataArrayDouble::Rotate2DAlg(const double *center, double angle, int nbNodes, const double *coordsIn, double *coordsOut)
{
  double cosa=cos(angle);
  double sina=sin(angle);
  double matrix[4];
  matrix[0]=cosa; matrix[1]=-sina; matrix[2]=sina; matrix[3]=cosa;
  double tmp[2];
  for(int i=0; i<nbNodes; i++)
    {
      std::transform(coordsIn+i*2,coordsIn+(i+1)*2,center,tmp,std::minus<double>());
      coordsOut[i*2]=matrix[0]*tmp[0]+matrix[1]*tmp[1]+center[0];
      coordsOut[i*2+1]=matrix[2]*tmp[0]+matrix[3]*tmp[1]+center[1];
    }
}

DataArrayDoubleIterator::DataArrayDoubleIterator(DataArrayDouble *da):DataArrayIterator<double>(da)
{
}

DataArrayDoubleTuple::DataArrayDoubleTuple(double *pt, int nbOfComp):DataArrayTuple<double>(pt,nbOfComp)
{
}


std::string DataArrayDoubleTuple::repr() const
{
  std::ostringstream oss; oss.precision(17); oss << "(";
  for(int i=0;i<_nb_of_compo-1;i++)
    oss << _pt[i] << ", ";
  oss << _pt[_nb_of_compo-1] << ")";
  return oss.str();
}

double DataArrayDoubleTuple::doubleValue() const
{
  return this->zeValue();
}

/*!
 * This method returns a newly allocated instance the caller should dealed with by a MEDCoupling::DataArrayDouble::decrRef.
 * This method performs \b no copy of data. The content is only referenced using MEDCoupling::DataArrayDouble::useArray with ownership set to \b false.
 * This method throws an INTERP_KERNEL::Exception is it is impossible to match sizes of \b this that is too say \b nbOfCompo=this->_nb_of_elem and \bnbOfTuples==1 or
 * \b nbOfCompo=1 and \bnbOfTuples==this->_nb_of_elem.
 */
DataArrayDouble *DataArrayDoubleTuple::buildDADouble(int nbOfTuples, int nbOfCompo) const
{
  return this->buildDA(nbOfTuples,nbOfCompo);
}

/*!
 * Returns a new instance of DataArrayInt. The caller is to delete this array
 * using decrRef() as it is no more needed. 
 */
DataArrayInt *DataArrayInt::New()
{
  return new DataArrayInt;
}

/*!
 * Returns the only one value in \a this, if and only if number of elements
 * (nb of tuples * nb of components) is equal to 1, and that \a this is allocated.
 *  \return double - the sole value stored in \a this array.
 *  \throw If at least one of conditions stated above is not fulfilled.
 */
int DataArrayInt::intValue() const
{
  if(isAllocated())
    {
      if(getNbOfElems()==1)
        {
          return *getConstPointer();
        }
      else
        throw INTERP_KERNEL::Exception("DataArrayInt::intValue : DataArrayInt instance is allocated but number of elements is not equal to 1 !");
    }
  else
    throw INTERP_KERNEL::Exception("DataArrayInt::intValue : DataArrayInt instance is not allocated !");
}

/*!
 * Returns an integer value characterizing \a this array, which is useful for a quick
 * comparison of many instances of DataArrayInt.
 *  \return int - the hash value.
 *  \throw If \a this is not allocated.
 */
int DataArrayInt::getHashCode() const
{
  checkAllocated();
  std::size_t nbOfElems=getNbOfElems();
  int ret=nbOfElems*65536;
  int delta=3;
  if(nbOfElems>48)
    delta=nbOfElems/8;
  int ret0=0;
  const int *pt=begin();
  for(std::size_t i=0;i<nbOfElems;i+=delta)
    ret0+=pt[i] & 0x1FFF;
  return ret+ret0;
}

/*!
 * Returns a full copy of \a this. For more info on copying data arrays see
 * \ref MEDCouplingArrayBasicsCopyDeep.
 *  \return DataArrayInt * - a new instance of DataArrayInt.
 */
DataArrayInt32 *DataArrayInt32::deepCopy() const
{
  return new DataArrayInt32(*this);
}

/*!
 * Returns a textual and human readable representation of \a this instance of
 * DataArrayInt. This text is shown when a DataArrayInt is printed in Python.
 * \return std::string - text describing \a this DataArrayInt.
 * 
 * \sa reprNotTooLong, reprZip
 */
std::string DataArrayInt::repr() const
{
  std::ostringstream ret;
  reprStream(ret);
  return ret.str();
}

std::string DataArrayInt::reprZip() const
{
  std::ostringstream ret;
  reprZipStream(ret);
  return ret.str();
}

void DataArrayInt::writeVTK(std::ostream& ofs, int indent, const std::string& type, const std::string& nameInFile, DataArrayByte *byteArr) const
{
  static const char SPACE[4]={' ',' ',' ',' '};
  checkAllocated();
  std::string idt(indent,' ');
  ofs << idt << "<DataArray type=\"" << type << "\" Name=\"" << nameInFile << "\" NumberOfComponents=\"" << getNumberOfComponents() << "\"";
  if(byteArr)
    {
      ofs << " format=\"appended\" offset=\"" << byteArr->getNumberOfTuples() << "\">";
      if(std::string(type)=="Int32")
        {
          const char *data(reinterpret_cast<const char *>(begin()));
          std::size_t sz(getNbOfElems()*sizeof(int));
          byteArr->insertAtTheEnd(data,data+sz);
          byteArr->insertAtTheEnd(SPACE,SPACE+4);
        }
      else if(std::string(type)=="Int8")
        {
          INTERP_KERNEL::AutoPtr<char> tmp(new char[getNbOfElems()]);
          std::copy(begin(),end(),(char *)tmp);
          byteArr->insertAtTheEnd((char *)tmp,(char *)tmp+getNbOfElems());
          byteArr->insertAtTheEnd(SPACE,SPACE+4);
        }
      else if(std::string(type)=="UInt8")
        {
          INTERP_KERNEL::AutoPtr<unsigned char> tmp(new unsigned char[getNbOfElems()]);
          std::copy(begin(),end(),(unsigned char *)tmp);
          byteArr->insertAtTheEnd((unsigned char *)tmp,(unsigned char *)tmp+getNbOfElems());
          byteArr->insertAtTheEnd(SPACE,SPACE+4);
        }
      else
        throw INTERP_KERNEL::Exception("DataArrayInt::writeVTK : Only Int32, Int8 and UInt8 supported !");
    }
  else
    {
      ofs << " RangeMin=\"" << getMinValueInArray() << "\" RangeMax=\"" << getMaxValueInArray() << "\" format=\"ascii\">\n" << idt;
      std::copy(begin(),end(),std::ostream_iterator<int>(ofs," "));
    }
  ofs << std::endl << idt << "</DataArray>\n";
}

void DataArrayInt::reprCppStream(const std::string& varName, std::ostream& stream) const
{
  int nbTuples=getNumberOfTuples(),nbComp=getNumberOfComponents();
  const int *data=getConstPointer();
  stream << "DataArrayInt *" << varName << "=DataArrayInt::New();" << std::endl;
  if(nbTuples*nbComp>=1)
    {
      stream << "const int " << varName << "Data[" << nbTuples*nbComp << "]={";
      std::copy(data,data+nbTuples*nbComp-1,std::ostream_iterator<int>(stream,","));
      stream << data[nbTuples*nbComp-1] << "};" << std::endl;
      stream << varName << "->useArray(" << varName << "Data,false,CPP_DEALLOC," << nbTuples << "," << nbComp << ");" << std::endl;
    }
  else
    stream << varName << "->alloc(" << nbTuples << "," << nbComp << ");" << std::endl;
  stream << varName << "->setName(\"" << getName() << "\");" << std::endl;
}

/*!
 * Method that gives a quick overvien of \a this for python.
 */
void DataArrayInt::reprQuickOverview(std::ostream& stream) const
{
  static const std::size_t MAX_NB_OF_BYTE_IN_REPR=300;
  stream << "DataArrayInt C++ instance at " << this << ". ";
  if(isAllocated())
    {
      int nbOfCompo=(int)_info_on_compo.size();
      if(nbOfCompo>=1)
        {
          int nbOfTuples=getNumberOfTuples();
          stream << "Number of tuples : " << nbOfTuples << ". Number of components : " << nbOfCompo << "." << std::endl;
          reprQuickOverviewData(stream,MAX_NB_OF_BYTE_IN_REPR);
        }
      else
        stream << "Number of components : 0.";
    }
  else
    stream << "*** No data allocated ****";
}

void DataArrayInt::reprQuickOverviewData(std::ostream& stream, std::size_t maxNbOfByteInRepr) const
{
  const int *data=begin();
  int nbOfTuples=getNumberOfTuples();
  int nbOfCompo=(int)_info_on_compo.size();
  std::ostringstream oss2; oss2 << "[";
  std::string oss2Str(oss2.str());
  bool isFinished=true;
  for(int i=0;i<nbOfTuples && isFinished;i++)
    {
      if(nbOfCompo>1)
        {
          oss2 << "(";
          for(int j=0;j<nbOfCompo;j++,data++)
            {
              oss2 << *data;
              if(j!=nbOfCompo-1) oss2 << ", ";
            }
          oss2 << ")";
        }
      else
        oss2 << *data++;
      if(i!=nbOfTuples-1) oss2 << ", ";
      std::string oss3Str(oss2.str());
      if(oss3Str.length()<maxNbOfByteInRepr)
        oss2Str=oss3Str;
      else
        isFinished=false;
    }
  stream << oss2Str;
  if(!isFinished)
    stream << "... ";
  stream << "]";
}

/*!
 * Computes distribution of values of \a this one-dimensional array between given value
 * ranges (casts). This method is typically useful for entity number spliting by types,
 * for example. 
 *  \warning The values contained in \a arrBg should be sorted ascendently. No
 *           check of this is be done. If not, the result is not warranted. 
 *  \param [in] arrBg - the array of ascending values defining the value ranges. The i-th
 *         value of \a arrBg (\a arrBg[ i ]) gives the lowest value of the i-th range,
 *         and the greatest value of the i-th range equals to \a arrBg[ i+1 ] - 1. \a
 *         arrBg containing \a n values defines \a n-1 ranges. The last value of \a arrBg
 *         should be more than every value in \a this array.
 *  \param [in] arrEnd - specifies the end of the array \a arrBg, so that
 *              the last value of \a arrBg is \a arrEnd[ -1 ].
 *  \param [out] castArr - a new instance of DataArrayInt, of same size as \a this array
 *         (same number of tuples and components), the caller is to delete 
 *         using decrRef() as it is no more needed.
 *         This array contains indices of ranges for every value of \a this array. I.e.
 *         the i-th value of \a castArr gives the index of range the i-th value of \a this
 *         belongs to. Or, in other words, this parameter contains for each tuple in \a
 *         this in which cast it holds.
 *  \param [out] rankInsideCast - a new instance of DataArrayInt, of same size as \a this
 *         array, the caller is to delete using decrRef() as it is no more needed.
 *         This array contains ranks of values of \a this array within ranges
 *         they belongs to. I.e. the i-th value of \a rankInsideCast gives the rank of
 *         the i-th value of \a this array within the \a castArr[ i ]-th range, to which
 *         the i-th value of \a this belongs to. Or, in other words, this param contains 
 *         for each tuple its rank inside its cast. The rank is computed as difference
 *         between the value and the lowest value of range.
 *  \param [out] castsPresent - a new instance of DataArrayInt, containing indices of
 *         ranges (casts) to which at least one value of \a this array belongs.
 *         Or, in other words, this param contains the casts that \a this contains.
 *         The caller is to delete this array using decrRef() as it is no more needed.
 *
 * \b Example: If \a this contains [6,5,0,3,2,7,8,1,4] and \a arrBg contains [0,4,9] then
 *            the output of this method will be : 
 * - \a castArr       : [1,1,0,0,0,1,1,0,1]
 * - \a rankInsideCast: [2,1,0,3,2,3,4,1,0]
 * - \a castsPresent  : [0,1]
 *
 * I.e. values of \a this array belong to 2 ranges: #0 and #1. Value 6 belongs to the
 * range #1 and its rank within this range is 2; etc.
 *
 *  \throw If \a this->getNumberOfComponents() != 1.
 *  \throw If \a arrEnd - arrBg < 2.
 *  \throw If any value of \a this is not less than \a arrEnd[-1].
 */
void DataArrayInt::splitByValueRange(const int *arrBg, const int *arrEnd,
                                     DataArrayInt *& castArr, DataArrayInt *& rankInsideCast, DataArrayInt *& castsPresent) const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("Call splitByValueRange  method on DataArrayInt with only one component, you can call 'rearrange' method before !");
  int nbOfTuples=getNumberOfTuples();
  std::size_t nbOfCast=std::distance(arrBg,arrEnd);
  if(nbOfCast<2)
    throw INTERP_KERNEL::Exception("DataArrayInt::splitByValueRange : The input array giving the cast range values should be of size >=2 !");
  nbOfCast--;
  const int *work=getConstPointer();
  typedef std::reverse_iterator<const int *> rintstart;
  rintstart bg(arrEnd);//OK no problem because size of 'arr' is greater or equal 2
  rintstart end2(arrBg);
  MCAuto<DataArrayInt> ret1=DataArrayInt::New();
  MCAuto<DataArrayInt> ret2=DataArrayInt::New();
  MCAuto<DataArrayInt> ret3=DataArrayInt::New();
  ret1->alloc(nbOfTuples,1);
  ret2->alloc(nbOfTuples,1);
  int *ret1Ptr=ret1->getPointer();
  int *ret2Ptr=ret2->getPointer();
  std::set<std::size_t> castsDetected;
  for(int i=0;i<nbOfTuples;i++)
    {
      rintstart res=std::find_if(bg,end2,std::bind2nd(std::less_equal<int>(), work[i]));
      std::size_t pos=std::distance(bg,res);
      std::size_t pos2=nbOfCast-pos;
      if(pos2<nbOfCast)
        {
          ret1Ptr[i]=(int)pos2;
          ret2Ptr[i]=work[i]-arrBg[pos2];
          castsDetected.insert(pos2);
        }
      else
        {
          std::ostringstream oss; oss << "DataArrayInt::splitByValueRange : At rank #" << i << " the value is " << work[i] << " should be in [0," << *bg << ") !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  ret3->alloc((int)castsDetected.size(),1);
  std::copy(castsDetected.begin(),castsDetected.end(),ret3->getPointer());
  castArr=ret1.retn();
  rankInsideCast=ret2.retn();
  castsPresent=ret3.retn();
}

/*!
 * This method look at \a this if it can be considered as a range defined by the 3-tuple ( \a strt , \a sttoopp , \a stteepp ).
 * If false is returned the tuple must be ignored. If true is returned \a this can be considered by a range( \a strt , \a sttoopp , \a stteepp ).
 * This method works only if \a this is allocated and single component. If not an exception will be thrown.
 *
 * \param [out] strt - the start of the range (included) if true is returned.
 * \param [out] sttoopp - the end of the range (not included) if true is returned.
 * \param [out] stteepp - the step of the range if true is returned.
 * \return the verdict of the check.
 *
 * \sa DataArray::GetNumberOfItemGivenBES
 */
bool DataArrayInt::isRange(int& strt, int& sttoopp, int& stteepp) const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::isRange : this must be single component array !");
  int nbTuples(getNumberOfTuples());
  if(nbTuples==0)
    { strt=0; sttoopp=0; stteepp=1; return true; }
  const int *pt(begin());
  strt=*pt; 
  if(nbTuples==1)
    { sttoopp=strt+1; stteepp=1; return true; }
  strt=*pt; sttoopp=pt[nbTuples-1];
  if(strt==sttoopp)
    return false;
  if(sttoopp>strt)
    {
      sttoopp++;
      int a(sttoopp-1-strt),tmp(strt);
      if(a%(nbTuples-1)!=0)
        return false;
      stteepp=a/(nbTuples-1);
      for(int i=0;i<nbTuples;i++,tmp+=stteepp)
        if(pt[i]!=tmp)
          return false;
      return true;
    }
  else
    {
      sttoopp--;
      int a(strt-sttoopp-1),tmp(strt);
      if(a%(nbTuples-1)!=0)
        return false;
      stteepp=-(a/(nbTuples-1));
      for(int i=0;i<nbTuples;i++,tmp+=stteepp)
        if(pt[i]!=tmp)
          return false;
      return true;
    }
}


/*!
 * Modifies in place \a this one-dimensional array so that each value \a v = \a indArrBg[ \a v ],
 * i.e. a current value is used as in index to get a new value from \a indArrBg.
 *  \param [in] indArrBg - pointer to the first element of array of new values to assign
 *         to \a this array.
 *  \param [in] indArrEnd - specifies the end of the array \a indArrBg, so that
 *              the last value of \a indArrBg is \a indArrEnd[ -1 ].
 *  \throw If \a this->getNumberOfComponents() != 1
 *  \throw If any value of \a this can't be used as a valid index for 
 *         [\a indArrBg, \a indArrEnd).
 *
 *  \sa changeValue, findIdForEach
 */
void DataArrayInt::transformWithIndArr(const int *indArrBg, const int *indArrEnd)
{
  this->checkAllocated();
  if(this->getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("Call transformWithIndArr method on DataArrayInt with only one component, you can call 'rearrange' method before !");
  int nbElemsIn((int)std::distance(indArrBg,indArrEnd)),nbOfTuples(getNumberOfTuples()),*pt(getPointer());
  for(int i=0;i<nbOfTuples;i++,pt++)
    {
      if(*pt>=0 && *pt<nbElemsIn)
        *pt=indArrBg[*pt];
      else
        {
          std::ostringstream oss; oss << "DataArrayInt::transformWithIndArr : error on tuple #" << i << " of this value is " << *pt << ", should be in [0," << nbElemsIn << ") !";
          throw INTERP_KERNEL::Exception(oss.str());
        }
    }
  this->declareAsNew();
}

void DataArrayInt::transformWithIndArr(const MapKeyVal<int>& m)
{
  this->checkAllocated();
  if(this->getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("Call transformWithIndArr method on DataArrayInt with only one component, you can call 'rearrange' method before !");
  const std::map<int,int>& dat(m.data());
  int nbOfTuples(getNumberOfTuples()),*pt(getPointer());
  for(int i=0;i<nbOfTuples;i++,pt++)
    {
      std::map<int,int>::const_iterator it(dat.find(*pt));
      if(it!=dat.end())
        *pt=(*it).second;
      else
        {
          std::ostringstream oss; oss << "DataArrayInt::transformWithIndArr : error on tuple #" << i << " of this value is " << *pt << " not in map !";
          throw INTERP_KERNEL::Exception(oss.str());
        }
    }
  this->declareAsNew();
}

/*!
 * Creates a one-dimensional DataArrayInt (\a res) whose contents are computed from 
 * values of \a this (\a a) and the given (\a indArr) arrays as follows:
 * \a res[ \a indArr[ \a a[ i ]]] = i. I.e. for each value in place i \a v = \a a[ i ],
 * new value in place \a indArr[ \a v ] is i.
 *  \param [in] indArrBg - the array holding indices within the result array to assign
 *         indices of values of \a this array pointing to values of \a indArrBg.
 *  \param [in] indArrEnd - specifies the end of the array \a indArrBg, so that
 *              the last value of \a indArrBg is \a indArrEnd[ -1 ].
 *  \return DataArrayInt * - the new instance of DataArrayInt.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If \a this->getNumberOfComponents() != 1.
 *  \throw If any value of \a this array is not a valid index for \a indArrBg array.
 *  \throw If any value of \a indArrBg is not a valid index for \a this array.
 */
DataArrayInt *DataArrayInt::transformWithIndArrR(const int *indArrBg, const int *indArrEnd) const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("Call transformWithIndArrR method on DataArrayInt with only one component, you can call 'rearrange' method before !");
  int nbElemsIn=(int)std::distance(indArrBg,indArrEnd);
  int nbOfTuples=getNumberOfTuples();
  const int *pt=getConstPointer();
  MCAuto<DataArrayInt> ret=DataArrayInt::New();
  ret->alloc(nbOfTuples,1);
  ret->fillWithValue(-1);
  int *tmp=ret->getPointer();
  for(int i=0;i<nbOfTuples;i++,pt++)
    {
      if(*pt>=0 && *pt<nbElemsIn)
        {
          int pos=indArrBg[*pt];
          if(pos>=0 && pos<nbOfTuples)
            tmp[pos]=i;
          else
            {
              std::ostringstream oss; oss << "DataArrayInt::transformWithIndArrR : error on tuple #" << i << " value of new pos is " << pos << " ( indArrBg[" << *pt << "]) ! Should be in [0," << nbOfTuples << ") !";
              throw INTERP_KERNEL::Exception(oss.str().c_str());
            }
        }
      else
        {
          std::ostringstream oss; oss << "DataArrayInt::transformWithIndArrR : error on tuple #" << i << " value is " << *pt << " and indirectionnal array as a size equal to " << nbElemsIn << " !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  return ret.retn();
}

/*!
 * Creates a one-dimensional DataArrayInt of given length, whose contents are computed
 * from values of \a this array, which is supposed to contain a renumbering map in 
 * "Old to New" mode. The result array contains a renumbering map in "New to Old" mode.
 * To know how to use the renumbering maps see \ref numbering.
 *  \param [in] newNbOfElem - the number of tuples in the result array.
 *  \return DataArrayInt * - the new instance of DataArrayInt.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 * 
 *  \if ENABLE_EXAMPLES
 *  \ref cpp_mcdataarrayint_invertarrayo2n2n2o "Here is a C++ example".<br>
 *  \ref py_mcdataarrayint_invertarrayo2n2n2o  "Here is a Python example".
 *  \endif
 */
DataArrayInt *DataArrayInt::invertArrayO2N2N2O(int newNbOfElem) const
{
  MCAuto<DataArrayInt> ret(DataArrayInt::New());
  ret->alloc(newNbOfElem,1);
  int nbOfOldNodes(this->getNumberOfTuples());
  const int *old2New(begin());
  int *pt(ret->getPointer());
  for(int i=0;i!=nbOfOldNodes;i++)
    {
      int newp(old2New[i]);
      if(newp!=-1)
        {
          if(newp>=0 && newp<newNbOfElem)
            pt[newp]=i;
          else
            {
              std::ostringstream oss; oss << "DataArrayInt::invertArrayO2N2N2O : At place #" << i << " the newplace is " << newp << " must be in [0," << newNbOfElem << ") !";
              throw INTERP_KERNEL::Exception(oss.str().c_str());
            }
        }
    }
  return ret.retn();
}

/*!
 * This method is similar to DataArrayInt::invertArrayO2N2N2O except that 
 * Example : If \a this contains [0,1,2,0,3,4,5,4,6,4] this method will return [0,1,2,4,5,6,8] whereas DataArrayInt::invertArrayO2N2N2O returns [3,1,2,4,9,6,8]
 */
DataArrayInt *DataArrayInt::invertArrayO2N2N2OBis(int newNbOfElem) const
{
  MCAuto<DataArrayInt> ret=DataArrayInt::New();
  ret->alloc(newNbOfElem,1);
  int nbOfOldNodes=getNumberOfTuples();
  const int *old2New=getConstPointer();
  int *pt=ret->getPointer();
  for(int i=nbOfOldNodes-1;i>=0;i--)
    {
      int newp(old2New[i]);
      if(newp!=-1)
        {
          if(newp>=0 && newp<newNbOfElem)
            pt[newp]=i;
          else
            {
              std::ostringstream oss; oss << "DataArrayInt::invertArrayO2N2N2OBis : At place #" << i << " the newplace is " << newp << " must be in [0," << newNbOfElem << ") !";
              throw INTERP_KERNEL::Exception(oss.str().c_str());
            }
        }
    }
  return ret.retn();
}

/*!
 * Creates a one-dimensional DataArrayInt of given length, whose contents are computed
 * from values of \a this array, which is supposed to contain a renumbering map in 
 * "New to Old" mode. The result array contains a renumbering map in "Old to New" mode.
 * To know how to use the renumbering maps see \ref numbering.
 *  \param [in] newNbOfElem - the number of tuples in the result array.
 *  \return DataArrayInt * - the new instance of DataArrayInt.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 * 
 *  \if ENABLE_EXAMPLES
 *  \ref cpp_mcdataarrayint_invertarrayn2o2o2n "Here is a C++ example".
 *
 *  \ref py_mcdataarrayint_invertarrayn2o2o2n "Here is a Python example".
 *  \sa invertArrayN2O2O2NOptimized
 *  \endif
 */
DataArrayInt *DataArrayInt::invertArrayN2O2O2N(int oldNbOfElem) const
{
  checkAllocated();
  MCAuto<DataArrayInt> ret=DataArrayInt::New();
  ret->alloc(oldNbOfElem,1);
  const int *new2Old=getConstPointer();
  int *pt=ret->getPointer();
  std::fill(pt,pt+oldNbOfElem,-1);
  int nbOfNewElems=getNumberOfTuples();
  for(int i=0;i<nbOfNewElems;i++)
    {
      int v(new2Old[i]);
      if(v>=0 && v<oldNbOfElem)
        pt[v]=i;
      else
        {
          std::ostringstream oss; oss << "DataArrayInt::invertArrayN2O2O2N : in new id #" << i << " old value is " << v << " expected to be in [0," << oldNbOfElem << ") !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  return ret.retn();
}

/*!
 * Creates a map, whose contents are computed
 * from values of \a this array, which is supposed to contain a renumbering map in 
 * "New to Old" mode. The result array contains a renumbering map in "Old to New" mode.
 * To know how to use the renumbering maps see \ref numbering.
 *  \param [in] newNbOfElem - the number of tuples in the result array.
 *  \return MapII  - the new instance of Map.
 * 
 *  \if ENABLE_EXAMPLES
 *  \ref cpp_mcdataarrayint_invertarrayn2o2o2n "Here is a C++ example".
 *
 *  \ref py_mcdataarrayint_invertarrayn2o2o2n "Here is a Python example".
 *  \sa invertArrayN2O2O2N
 *  \endif
 */
MCAuto< MapKeyVal<int> > DataArrayInt32::invertArrayN2O2O2NOptimized() const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt32::invertArrayN2O2O2NOptimized : single component expected !");
  MCAuto< MapKeyVal<int> > ret(MapKeyVal<int>::New());
  std::map<int,int>& m(ret->data());
  const int *new2Old(begin());
  std::size_t nbOfNewElems(this->getNumberOfTuples());
  for(std::size_t i=0;i<nbOfNewElems;i++)
    {
      int v(new2Old[i]);
      m[v]=i;
    }
  return ret;
}

/*!
 * Returns a new DataArrayInt containing a renumbering map in "Old to New" mode.
 * This map, if applied to \a this array, would make it sorted. For example, if
 * \a this array contents are [9,10,0,6,4,11,3,7] then the contents of the result array
 * are [5,6,0,3,2,7,1,4]; if this result array (\a res) is used as an argument in call
 * \a this->renumber(\a res) then the returned array contains [0,3,4,6,7,9,10,11].
 * This method is useful for renumbering (in MED file for example). For more info
 * on renumbering see \ref numbering.
 *  \return DataArrayInt * - a new instance of DataArrayInt. The caller is to delete this
 *          array using decrRef() as it is no more needed.
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1.
 *  \throw If there are equal values in \a this array.
 */
DataArrayInt *DataArrayInt::checkAndPreparePermutation() const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::checkAndPreparePermutation : number of components must == 1 !");
  int nbTuples=getNumberOfTuples();
  const int *pt=getConstPointer();
  int *pt2=CheckAndPreparePermutation(pt,pt+nbTuples);
  DataArrayInt *ret=DataArrayInt::New();
  ret->useArray(pt2,true,C_DEALLOC,nbTuples,1);
  return ret;
}

/*!
 * This method tries to find the permutation to apply to the first input \a ids1 to obtain the same array (without considering strings information) the second
 * input array \a ids2.
 * \a ids1 and \a ids2 are expected to be both a list of ids (both with number of components equal to one) not sorted and with values that can be negative.
 * This method will throw an exception is no such permutation array can be obtained. It is typically the case if there is some ids in \a ids1 not in \a ids2 or
 * inversely.
 * In case of success both assertion will be true (no throw) :
 * \c ids1->renumber(ret)->isEqual(ids2) where \a ret is the return of this method.
 * \c ret->transformWithIndArr(ids2)->isEqual(ids1)
 *
 * \b Example:
 * - \a ids1 : [3,1,103,4,6,10,-7,205]
 * - \a ids2 : [-7,1,205,10,6,3,103,4]
 * - \a return is : [5,1,6,7,4,3,0,2] because ids2[5]==ids1[0], ids2[1]==ids1[1], ids2[6]==ids1[2]...
 *
 * \return DataArrayInt * - a new instance of DataArrayInt. The caller is to delete this
 *          array using decrRef() as it is no more needed.
 * \throw If either ids1 or ids2 is null not allocated or not with one components.
 * 
 * \sa DataArrayInt32::findIdForEach
 */
DataArrayInt *DataArrayInt::FindPermutationFromFirstToSecond(const DataArrayInt *ids1, const DataArrayInt *ids2)
{
  if(!ids1 || !ids2)
    throw INTERP_KERNEL::Exception("DataArrayInt::FindPermutationFromFirstToSecond : the two input arrays must be not null !");
  if(!ids1->isAllocated() || !ids2->isAllocated())
    throw INTERP_KERNEL::Exception("DataArrayInt::FindPermutationFromFirstToSecond : the two input arrays must be allocated !");
  if(ids1->getNumberOfComponents()!=1 || ids2->getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::FindPermutationFromFirstToSecond : the two input arrays have exactly one component !");
  if(ids1->getNumberOfTuples()!=ids2->getNumberOfTuples())
    {
      std::ostringstream oss; oss << "DataArrayInt::FindPermutationFromFirstToSecond : first array has " << ids1->getNumberOfTuples() << " tuples and the second one " << ids2->getNumberOfTuples() << " tuples ! No chance to find a permutation between the 2 arrays !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  MCAuto<DataArrayInt> p1(ids1->deepCopy());
  MCAuto<DataArrayInt> p2(ids2->deepCopy());
  p1->sort(true); p2->sort(true);
  if(!p1->isEqualWithoutConsideringStr(*p2))
    throw INTERP_KERNEL::Exception("DataArrayInt::FindPermutationFromFirstToSecond : the two arrays are not lying on same ids ! Impossible to find a permutation between the 2 arrays !");
  p1=ids1->checkAndPreparePermutation();
  p2=ids2->checkAndPreparePermutation();
  p2=p2->invertArrayO2N2N2O(p2->getNumberOfTuples());
  p2=p2->selectByTupleIdSafe(p1->begin(),p1->end());
  return p2.retn();
}

/*!
 * Returns two arrays describing a surjective mapping from \a this set of values (\a A)
 * onto a set of values of size \a targetNb (\a B). The surjective function is 
 * \a B[ \a A[ i ]] = i. That is to say that for each \a id in [0,\a targetNb), where \a
 * targetNb < \a this->getNumberOfTuples(), there exists at least one tupleId (\a tid) so
 * that <em> this->getIJ( tid, 0 ) == id</em>. <br>
 * The first of out arrays returns indices of elements of \a this array, grouped by their
 * place in the set \a B. The second out array is the index of the first one; it shows how
 * many elements of \a A are mapped into each element of \a B. <br>
 * For more info on
 * mapping and its usage in renumbering see \ref numbering. <br>
 * \b Example:
 * - \a this: [0,3,2,3,2,2,1,2]
 * - \a targetNb: 4
 * - \a arr:  [0,  6,  2,4,5,7,  1,3]
 * - \a arrI: [0,1,2,6,8]
 *
 * This result means: <br>
 * the element of \a B 0 encounters within \a A once (\a arrI[ 0+1 ] - \a arrI[ 0 ]) and
 * its index within \a A is 0 ( \a arr[ 0:1 ] == \a arr[ \a arrI[ 0 ] : \a arrI[ 0+1 ]]);<br>
 * the element of \a B 2 encounters within \a A 4 times (\a arrI[ 2+1 ] - \a arrI[ 2 ]) and
 * its indices within \a A are [2,4,5,7] ( \a arr[ 2:6 ] == \a arr[ \a arrI[ 2 ] : 
 * \a arrI[ 2+1 ]]); <br> etc.
 *  \param [in] targetNb - the size of the set \a B. \a targetNb must be equal or more
 *         than the maximal value of \a A.
 *  \param [out] arr - a new instance of DataArrayInt returning indices of
 *         elements of \a this, grouped by their place in the set \a B. The caller is to delete
 *         this array using decrRef() as it is no more needed.
 *  \param [out] arrI - a new instance of DataArrayInt returning size of groups of equal
 *         elements of \a this. The caller is to delete this array using decrRef() as it
 *         is no more needed.
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1.
 *  \throw If any value in \a this is more or equal to \a targetNb.
 */
void DataArrayInt::changeSurjectiveFormat(int targetNb, DataArrayInt *&arr, DataArrayInt *&arrI) const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::changeSurjectiveFormat : number of components must == 1 !");
  int nbOfTuples=getNumberOfTuples();
  MCAuto<DataArrayInt> ret(DataArrayInt::New());
  MCAuto<DataArrayInt> retI(DataArrayInt::New());
  retI->alloc(targetNb+1,1);
  const int *input=getConstPointer();
  std::vector< std::vector<int> > tmp(targetNb);
  for(int i=0;i<nbOfTuples;i++)
    {
      int tmp2=input[i];
      if(tmp2>=0 && tmp2<targetNb)
        tmp[tmp2].push_back(i);
      else
        {
          std::ostringstream oss; oss << "DataArrayInt::changeSurjectiveFormat : At pos " << i << " presence of element " << tmp2 << " ! should be in [0," << targetNb << ") !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  int *retIPtr=retI->getPointer();
  *retIPtr=0;
  for(std::vector< std::vector<int> >::const_iterator it1=tmp.begin();it1!=tmp.end();it1++,retIPtr++)
    retIPtr[1]=retIPtr[0]+(int)((*it1).size());
  if(nbOfTuples!=retI->getIJ(targetNb,0))
    throw INTERP_KERNEL::Exception("DataArrayInt::changeSurjectiveFormat : big problem should never happen !");
  ret->alloc(nbOfTuples,1);
  int *retPtr=ret->getPointer();
  for(std::vector< std::vector<int> >::const_iterator it1=tmp.begin();it1!=tmp.end();it1++)
    retPtr=std::copy((*it1).begin(),(*it1).end(),retPtr);
  arr=ret.retn();
  arrI=retI.retn();
}


/*!
 * Returns a new DataArrayInt containing a renumbering map in "Old to New" mode computed
 * from a zip representation of a surjective format (returned e.g. by
 * \ref MEDCoupling::DataArrayDouble::findCommonTuples() "DataArrayDouble::findCommonTuples()"
 * for example). The result array minimizes the permutation. <br>
 * For more info on renumbering see \ref numbering. <br>
 * \b Example: <br>
 * - \a nbOfOldTuples: 10 
 * - \a arr          : [0,3, 5,7,9]
 * - \a arrIBg       : [0,2,5]
 * - \a newNbOfTuples: 7
 * - result array    : [0,1,2,0,3,4,5,4,6,4]
 *
 *  \param [in] nbOfOldTuples - number of tuples in the initial array \a arr.
 *  \param [in] arr - the array of tuple indices grouped by \a arrIBg array.
 *  \param [in] arrIBg - the array dividing all indices stored in \a arr into groups of
 *         (indices of) equal values. Its every element (except the last one) points to
 *         the first element of a group of equal values.
 *  \param [in] arrIEnd - specifies the end of \a arrIBg, so that the last element of \a
 *          arrIBg is \a arrIEnd[ -1 ].
 *  \param [out] newNbOfTuples - number of tuples after surjection application.
 *  \return DataArrayInt * - a new instance of DataArrayInt. The caller is to delete this
 *          array using decrRef() as it is no more needed.
 *  \throw If any value of \a arr breaks condition ( 0 <= \a arr[ i ] < \a nbOfOldTuples ).
 */
DataArrayInt *DataArrayInt::ConvertIndexArrayToO2N(int nbOfOldTuples, const int *arr, const int *arrIBg, const int *arrIEnd, int &newNbOfTuples)
{
  MCAuto<DataArrayInt> ret=DataArrayInt::New();
  ret->alloc(nbOfOldTuples,1);
  int *pt=ret->getPointer();
  std::fill(pt,pt+nbOfOldTuples,-1);
  int nbOfGrps=((int)std::distance(arrIBg,arrIEnd))-1;
  const int *cIPtr=arrIBg;
  for(int i=0;i<nbOfGrps;i++)
    pt[arr[cIPtr[i]]]=-(i+2);
  int newNb=0;
  for(int iNode=0;iNode<nbOfOldTuples;iNode++)
    {
      if(pt[iNode]<0)
        {
          if(pt[iNode]==-1)
            pt[iNode]=newNb++;
          else
            {
              int grpId=-(pt[iNode]+2);
              for(int j=cIPtr[grpId];j<cIPtr[grpId+1];j++)
                {
                  if(arr[j]>=0 && arr[j]<nbOfOldTuples)
                    pt[arr[j]]=newNb;
                  else
                    {
                      std::ostringstream oss; oss << "DataArrayInt::ConvertIndexArrayToO2N : With element #" << j << " value is " << arr[j] << " should be in [0," << nbOfOldTuples << ") !";
                      throw INTERP_KERNEL::Exception(oss.str().c_str());
                    }
                }
              newNb++;
            }
        }
    }
  newNbOfTuples=newNb;
  return ret.retn();
}

/*!
 * Returns a new DataArrayInt containing a renumbering map in "New to Old" mode,
 * which if applied to \a this array would make it sorted ascendingly.
 * For more info on renumbering see \ref numbering. <br>
 * \b Example: <br>
 * - \a this: [2,0,1,1,0,1,2,0,1,1,0,0]
 * - result: [10,0,5,6,1,7,11,2,8,9,3,4]
 * - after applying result to \a this: [0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2] 
 *
 *  \return DataArrayInt * - a new instance of DataArrayInt. The caller is to delete this
 *          array using decrRef() as it is no more needed.
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1.
 */
DataArrayInt *DataArrayInt::buildPermArrPerLevel() const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::buildPermArrPerLevel : number of components must == 1 !");
  int nbOfTuples=getNumberOfTuples();
  const int *pt=getConstPointer();
  std::map<int,int> m;
  MCAuto<DataArrayInt> ret=DataArrayInt::New();
  ret->alloc(nbOfTuples,1);
  int *opt=ret->getPointer();
  for(int i=0;i<nbOfTuples;i++,pt++,opt++)
    {
      int val=*pt;
      std::map<int,int>::iterator it=m.find(val);
      if(it!=m.end())
        {
          *opt=(*it).second;
          (*it).second++;
        }
      else
        {
          *opt=0;
          m.insert(std::pair<int,int>(val,1));
        }
    }
  int sum=0;
  for(std::map<int,int>::iterator it=m.begin();it!=m.end();it++)
    {
      int vt=(*it).second;
      (*it).second=sum;
      sum+=vt;
    }
  pt=getConstPointer();
  opt=ret->getPointer();
  for(int i=0;i<nbOfTuples;i++,pt++,opt++)
    *opt+=m[*pt];
  //
  return ret.retn();
}

/*!
 * Checks if \a this array has the given size, and if its contents is equal to an array filled with
 * iota(). This method is particularly useful for DataArrayInt instances that represent
 * a renumbering array, to check if there is a real need in renumbering.
 * This method checks than \a this can be considered as an identity mapping
 * of a set having \a sizeExpected elements into itself.
 *
 *  \param [in] sizeExpected - The number of elements expected.
 *  \return bool - \a true if \a this array contents == \a range( \a this->getNumberOfTuples())
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1.
 */
bool DataArrayInt::isIota(int sizeExpected) const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    return false;
  int nbOfTuples(getNumberOfTuples());
  if(nbOfTuples!=sizeExpected)
    return false;
  const int *pt=getConstPointer();
  for(int i=0;i<nbOfTuples;i++,pt++)
    if(*pt!=i)
      return false;
  return true;
}

/*!
 * Checks if all values in \a this array are equal to \a val.
 *  \param [in] val - value to check equality of array values to.
 *  \return bool - \a true if all values are \a val.
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1
 *  \sa DataArrayInt::checkUniformAndGuess
 */
bool DataArrayInt::isUniform(int val) const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::isUniform : must be applied on DataArrayInt with only one component, you can call 'rearrange' method before !");
  const int *w(begin()),*end2(end());
  for(;w!=end2;w++)
    if(*w!=val)
      return false;
  return true;
}

/*!
 * This method checks that \a this is uniform. If not and exception will be thrown.
 * In case of uniformity the corresponding value is returned.
 *
 * \return int - the unique value contained in this
 * \throw If \a this is not allocated.
 * \throw If \a this->getNumberOfComponents() != 1
 * \throw If \a this is not uniform.
 * \sa DataArrayInt::isUniform
 */
int DataArrayInt::checkUniformAndGuess() const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::checkUniformAndGuess : must be applied on DataArrayInt with only one component, you can call 'rearrange' method before !");
  if(empty())
    throw INTERP_KERNEL::Exception("DataArrayInt::checkUniformAndGuess : this is empty !");
  const int *w(begin()),*end2(end());
  int ret(*w);
  for(;w!=end2;w++)
    if(*w!=ret)
      throw INTERP_KERNEL::Exception("DataArrayInt::checkUniformAndGuess : this is not uniform !");
  return ret;
}

/*!
 * Checks if all values in \a this array are unique.
 *  \return bool - \a true if condition above is true
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1
 */
bool DataArrayInt::hasUniqueValues() const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::hasOnlyUniqueValues: must be applied on DataArrayInt with only one component, you can call 'rearrange' method before !");
  std::size_t nbOfTuples(getNumberOfTuples());
  std::set<int> s(begin(),end());  // in C++11, should use unordered_set (O(1) complexity)
  if (s.size() != nbOfTuples)
    return false;
  return true;
}

/*!
 * Copy all components in a specified order from another DataArrayInt.
 * The specified components become the first ones in \a this array.
 * Both numerical and textual data is copied. The number of tuples in \a this and
 * the other array can be different.
 *  \param [in] a - the array to copy data from.
 *  \param [in] compoIds - sequence of zero based indices of components, data of which is
 *              to be copied.
 *  \throw If \a a is NULL.
 *  \throw If \a compoIds.size() != \a a->getNumberOfComponents().
 *  \throw If \a compoIds[i] < 0 or \a compoIds[i] > \a this->getNumberOfComponents().
 *
 *  \if ENABLE_EXAMPLES
 *  \ref py_mcdataarrayint_setselectedcomponents "Here is a Python example".
 *  \endif
 */
void DataArrayInt::setSelectedComponents(const DataArrayInt *a, const std::vector<int>& compoIds)
{
  if(!a)
    throw INTERP_KERNEL::Exception("DataArrayInt::setSelectedComponents : input DataArrayInt is NULL !");
  checkAllocated();
  a->checkAllocated();
  copyPartOfStringInfoFrom2(compoIds,*a);
  std::size_t partOfCompoSz=compoIds.size();
  int nbOfCompo=getNumberOfComponents();
  int nbOfTuples=std::min(getNumberOfTuples(),a->getNumberOfTuples());
  const int *ac=a->getConstPointer();
  int *nc=getPointer();
  for(int i=0;i<nbOfTuples;i++)
    for(std::size_t j=0;j<partOfCompoSz;j++,ac++)
      nc[nbOfCompo*i+compoIds[j]]=*ac;
}

DataArrayIntIterator *DataArrayInt::iterator()
{
  return new DataArrayIntIterator(this);
}

/*!
 * Creates a new DataArrayInt containing IDs (indices) of tuples holding value equal to a
 * given one. The ids are sorted in the ascending order.
 *  \param [in] val - the value to find within \a this.
 *  \return DataArrayInt * - a new instance of DataArrayInt. The caller is to delete this
 *          array using decrRef() as it is no more needed.
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1.
 *  \sa DataArrayInt::findIdsEqualTuple
 */
DataArrayInt *DataArrayInt::findIdsEqual(int val) const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::findIdsEqual : the array must have only one component, you can call 'rearrange' method before !");
  const int *cptr(getConstPointer());
  MCAuto<DataArrayInt> ret(DataArrayInt::New()); ret->alloc(0,1);
  int nbOfTuples=getNumberOfTuples();
  for(int i=0;i<nbOfTuples;i++,cptr++)
    if(*cptr==val)
      ret->pushBackSilent(i);
  return ret.retn();
}

/*!
 * Creates a new DataArrayInt containing IDs (indices) of tuples holding value \b not
 * equal to a given one. 
 *  \param [in] val - the value to ignore within \a this.
 *  \return DataArrayInt * - a new instance of DataArrayInt. The caller is to delete this
 *          array using decrRef() as it is no more needed.
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1.
 */
DataArrayInt *DataArrayInt::findIdsNotEqual(int val) const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::findIdsNotEqual : the array must have only one component, you can call 'rearrange' method before !");
  const int *cptr(getConstPointer());
  MCAuto<DataArrayInt> ret(DataArrayInt::New()); ret->alloc(0,1);
  int nbOfTuples=getNumberOfTuples();
  for(int i=0;i<nbOfTuples;i++,cptr++)
    if(*cptr!=val)
      ret->pushBackSilent(i);
  return ret.retn();
}

/*!
 * Creates a new DataArrayInt containing IDs (indices) of tuples holding tuple equal to those defined by [ \a tupleBg , \a tupleEnd )
 * This method is an extension of  DataArrayInt::findIdsEqual method.
 *
 *  \param [in] tupleBg - the begin (included) of the input tuple to find within \a this.
 *  \param [in] tupleEnd - the end (excluded) of the input tuple to find within \a this.
 *  \return DataArrayInt * - a new instance of DataArrayInt. The caller is to delete this
 *          array using decrRef() as it is no more needed.
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != std::distance(tupleBg,tupleEnd).
 * \throw If \a this->getNumberOfComponents() is equal to 0.
 * \sa DataArrayInt::findIdsEqual
 */
DataArrayInt *DataArrayInt::findIdsEqualTuple(const int *tupleBg, const int *tupleEnd) const
{
  std::size_t nbOfCompoExp(std::distance(tupleBg,tupleEnd));
  checkAllocated();
  if(getNumberOfComponents()!=nbOfCompoExp)
    {
      std::ostringstream oss; oss << "DataArrayInt::findIdsEqualTuple : mismatch of number of components. Input tuple has " << nbOfCompoExp << " whereas this array has " << getNumberOfComponents() << " components !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  if(nbOfCompoExp==0)
    throw INTERP_KERNEL::Exception("DataArrayInt::findIdsEqualTuple : number of components should be > 0 !");
  MCAuto<DataArrayInt> ret(DataArrayInt::New()); ret->alloc(0,1);
  const int *bg(begin()),*end2(end()),*work(begin());
  while(work!=end2)
    {
      work=std::search(work,end2,tupleBg,tupleEnd);
      if(work!=end2)
        {
          std::size_t pos(std::distance(bg,work));
          if(pos%nbOfCompoExp==0)
            ret->pushBackSilent(pos/nbOfCompoExp);
          work++;
        }
    }
  return ret.retn();
}

/*!
 * This method finds for each element \a ELT in [valsBg,valsEnd) elements in \a this equal to it. Associated to ELT
 * this method will return the tuple id of last element found. If there is no element in \a this equal to ELT
 * an exception will be thrown.
 *
 * In case of success this[ret]==vals. Samely ret->transformWithIndArr(this->begin(),this->end())==vals.
 * Where \a vals is the [valsBg,valsEnd) array and \a ret the array returned by this method.
 * This method can be seen as an extension of FindPermutationFromFirstToSecond.
 * <br>
 * \b Example: <br>
 * - \a this: [17,27,2,10,-4,3,12,27,16]
 * - \a val : [3,16,-4,27,17]
 * - result: [5,8,4,7,0]
 *
 * \return - An array of size std::distance(valsBg,valsEnd)
 *
 * \sa DataArrayInt32::FindPermutationFromFirstToSecond
 */
MCAuto<DataArrayInt32> DataArrayInt32::findIdForEach(const int *valsBg, const int *valsEnd) const
{
  MCAuto<DataArrayInt32> ret(DataArrayInt32::New());
  std::size_t nbOfTuplesOut(std::distance(valsBg,valsEnd));
  ret->alloc(nbOfTuplesOut,1);
  MCAuto< MapKeyVal<int> > zeMap(invertArrayN2O2O2NOptimized());
  const std::map<int,int>& dat(zeMap->data());
  int *ptToFeed(ret->getPointer());
  for(const int *pt=valsBg;pt!=valsEnd;pt++)
    {
      std::map<int,int>::const_iterator it(dat.find(*pt));
      if(it!=dat.end())
        *ptToFeed++=(*it).second;
      else
        {
          std::ostringstream oss; oss << "DataArrayInt32::findIdForEach : error for element at place " << std::distance(valsBg,pt);
          oss << " of input array value is " << *pt << " which is not in this !";
          throw INTERP_KERNEL::Exception(oss.str());
        }
    }
  return ret;
}

/*!
 * Assigns \a newValue to all elements holding \a oldValue within \a this
 * one-dimensional array.
 *  \param [in] oldValue - the value to replace.
 *  \param [in] newValue - the value to assign.
 *  \return int - number of replacements performed.
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1.
 */
int DataArrayInt::changeValue(int oldValue, int newValue)
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::changeValue : the array must have only one component, you can call 'rearrange' method before !");
  if(oldValue==newValue)
    return 0;
  int *start(getPointer()),*end2(start+getNbOfElems());
  int ret(0);
  for(int *val=start;val!=end2;val++)
    {
      if(*val==oldValue)
        {
          *val=newValue;
          ret++;
        }
    }
  if(ret>0)
    declareAsNew();
  return ret;
}

/*!
 * Creates a new DataArrayInt containing IDs (indices) of tuples holding value equal to
 * one of given values.
 *  \param [in] valsBg - an array of values to find within \a this array.
 *  \param [in] valsEnd - specifies the end of the array \a valsBg, so that
 *              the last value of \a valsBg is \a valsEnd[ -1 ].
 *  \return DataArrayInt * - a new instance of DataArrayInt. The caller is to delete this
 *          array using decrRef() as it is no more needed.
 *  \throw If \a this->getNumberOfComponents() != 1.
 */
DataArrayInt *DataArrayInt::findIdsEqualList(const int *valsBg, const int *valsEnd) const
{
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::findIdsEqualList : the array must have only one component, you can call 'rearrange' method before !");
  std::set<int> vals2(valsBg,valsEnd);
  const int *cptr(getConstPointer());
  std::vector<int> res;
  int nbOfTuples(getNumberOfTuples());
  MCAuto<DataArrayInt> ret(DataArrayInt::New()); ret->alloc(0,1);
  for(int i=0;i<nbOfTuples;i++,cptr++)
    if(vals2.find(*cptr)!=vals2.end())
      ret->pushBackSilent(i);
  return ret.retn();
}

/*!
 * Creates a new DataArrayInt containing IDs (indices) of tuples holding values \b not
 * equal to any of given values.
 *  \param [in] valsBg - an array of values to ignore within \a this array.
 *  \param [in] valsEnd - specifies the end of the array \a valsBg, so that
 *              the last value of \a valsBg is \a valsEnd[ -1 ].
 *  \return DataArrayInt * - a new instance of DataArrayInt. The caller is to delete this
 *          array using decrRef() as it is no more needed.
 *  \throw If \a this->getNumberOfComponents() != 1.
 */
DataArrayInt *DataArrayInt::findIdsNotEqualList(const int *valsBg, const int *valsEnd) const
{
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::findIdsNotEqualList : the array must have only one component, you can call 'rearrange' method before !");
  std::set<int> vals2(valsBg,valsEnd);
  const int *cptr=getConstPointer();
  std::vector<int> res;
  int nbOfTuples=getNumberOfTuples();
  MCAuto<DataArrayInt> ret(DataArrayInt::New()); ret->alloc(0,1);
  for(int i=0;i<nbOfTuples;i++,cptr++)
    if(vals2.find(*cptr)==vals2.end())
      ret->pushBackSilent(i);
  return ret.retn();
}

/*!
 * This method is an extension of DataArrayInt::findIdFirstEqual method because this method works for DataArrayInt with
 * any number of components excepted 0 (an INTERP_KERNEL::Exception is thrown in this case).
 * This method searches in \b this is there is a tuple that matched the input parameter \b tupl.
 * If any the tuple id is returned. If not -1 is returned.
 * 
 * This method throws an INTERP_KERNEL::Exception if the number of components in \b this mismatches with the size of
 * the input vector. An INTERP_KERNEL::Exception is thrown too if \b this is not allocated.
 *
 * \return tuple id where \b tupl is. -1 if no such tuple exists in \b this.
 * \sa DataArrayInt::findIdSequence, DataArrayInt::presenceOfTuple.
 */
int DataArrayInt::findIdFirstEqualTuple(const std::vector<int>& tupl) const
{
  checkAllocated();
  int nbOfCompo=getNumberOfComponents();
  if(nbOfCompo==0)
    throw INTERP_KERNEL::Exception("DataArrayInt::findIdFirstEqualTuple : 0 components in 'this' !");
  if(nbOfCompo!=(int)tupl.size())
    {
      std::ostringstream oss; oss << "DataArrayInt::findIdFirstEqualTuple : 'this' contains " << nbOfCompo << " components and searching for a tuple of length " << tupl.size() << " !";
      throw INTERP_KERNEL::Exception(oss.str().c_str());
    }
  const int *cptr=getConstPointer();
  std::size_t nbOfVals=getNbOfElems();
  for(const int *work=cptr;work!=cptr+nbOfVals;)
    {
      work=std::search(work,cptr+nbOfVals,tupl.begin(),tupl.end());
      if(work!=cptr+nbOfVals)
        {
          if(std::distance(cptr,work)%nbOfCompo!=0)
            work++;
          else
            return std::distance(cptr,work)/nbOfCompo;
        }
    }
  return -1;
}

/*!
 * This method searches the sequence specified in input parameter \b vals in \b this.
 * This works only for DataArrayInt having number of components equal to one (if not an INTERP_KERNEL::Exception will be thrown).
 * This method differs from DataArrayInt::findIdFirstEqualTuple in that the position is internal raw data is not considered here contrary to DataArrayInt::findIdFirstEqualTuple.
 * \sa DataArrayInt::findIdFirstEqualTuple
 */
int DataArrayInt::findIdSequence(const std::vector<int>& vals) const
{
  checkAllocated();
  int nbOfCompo=getNumberOfComponents();
  if(nbOfCompo!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::findIdSequence : works only for DataArrayInt instance with one component !");
  const int *cptr=getConstPointer();
  std::size_t nbOfVals=getNbOfElems();
  const int *loc=std::search(cptr,cptr+nbOfVals,vals.begin(),vals.end());
  if(loc!=cptr+nbOfVals)
    return std::distance(cptr,loc);
  return -1;
}

/*!
 * This method expects to be called when number of components of this is equal to one.
 * This method returns the tuple id, if it exists, of the first tuple equal to \b value.
 * If not any tuple contains \b value -1 is returned.
 * \sa DataArrayInt::presenceOfValue
 */
int DataArrayInt::findIdFirstEqual(int value) const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::presenceOfValue : the array must have only one component, you can call 'rearrange' method before !");
  const int *cptr=getConstPointer();
  int nbOfTuples=getNumberOfTuples();
  const int *ret=std::find(cptr,cptr+nbOfTuples,value);
  if(ret!=cptr+nbOfTuples)
    return std::distance(cptr,ret);
  return -1;
}

/*!
 * This method expects to be called when number of components of this is equal to one.
 * This method returns the tuple id, if it exists, of the first tuple so that the value is contained in \b vals.
 * If not any tuple contains one of the values contained in 'vals' -1 is returned.
 * \sa DataArrayInt::presenceOfValue
 */
int DataArrayInt::findIdFirstEqual(const std::vector<int>& vals) const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::presenceOfValue : the array must have only one component, you can call 'rearrange' method before !");
  std::set<int> vals2(vals.begin(),vals.end());
  const int *cptr=getConstPointer();
  int nbOfTuples=getNumberOfTuples();
  for(const int *w=cptr;w!=cptr+nbOfTuples;w++)
    if(vals2.find(*w)!=vals2.end())
      return std::distance(cptr,w);
  return -1;
}

/*!
 * This method returns the number of values in \a this that are equals to input parameter \a value.
 * This method only works for single component array.
 *
 * \return a value in [ 0, \c this->getNumberOfTuples() )
 *
 * \throw If \a this is not allocated
 *
 */
int DataArrayInt::count(int value) const
{
  int ret=0;
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::count : must be applied on DataArrayInt with only one component, you can call 'rearrange' method before !");
  const int *vals=begin();
  int nbOfTuples=getNumberOfTuples();
  for(int i=0;i<nbOfTuples;i++,vals++)
    if(*vals==value)
      ret++;
  return ret;
}

/*!
 * This method is an extension of DataArrayInt::presenceOfValue method because this method works for DataArrayInt with
 * any number of components excepted 0 (an INTERP_KERNEL::Exception is thrown in this case).
 * This method searches in \b this is there is a tuple that matched the input parameter \b tupl.
 * This method throws an INTERP_KERNEL::Exception if the number of components in \b this mismatches with the size of
 * the input vector. An INTERP_KERNEL::Exception is thrown too if \b this is not allocated.
 * \sa DataArrayInt::findIdFirstEqualTuple
 */
bool DataArrayInt::presenceOfTuple(const std::vector<int>& tupl) const
{
  return findIdFirstEqualTuple(tupl)!=-1;
}


/*!
 * Returns \a true if a given value is present within \a this one-dimensional array.
 *  \param [in] value - the value to find within \a this array.
 *  \return bool - \a true in case if \a value is present within \a this array.
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1.
 *  \sa findIdFirstEqual()
 */
bool DataArrayInt::presenceOfValue(int value) const
{
  return findIdFirstEqual(value)!=-1;
}

/*!
 * This method expects to be called when number of components of this is equal to one.
 * This method returns true if it exists a tuple so that the value is contained in \b vals.
 * If not any tuple contains one of the values contained in 'vals' false is returned.
 * \sa DataArrayInt::findIdFirstEqual
 */
bool DataArrayInt::presenceOfValue(const std::vector<int>& vals) const
{
  return findIdFirstEqual(vals)!=-1;
}

/*!
 * Accumulates values of each component of \a this array.
 *  \param [out] res - an array of length \a this->getNumberOfComponents(), allocated 
 *         by the caller, that is filled by this method with sum value for each
 *         component.
 *  \throw If \a this is not allocated.
 */
void DataArrayInt::accumulate(int *res) const
{
  checkAllocated();
  const int *ptr=getConstPointer();
  int nbTuple=getNumberOfTuples();
  int nbComps=getNumberOfComponents();
  std::fill(res,res+nbComps,0);
  for(int i=0;i<nbTuple;i++)
    std::transform(ptr+i*nbComps,ptr+(i+1)*nbComps,res,res,std::plus<int>());
}

int DataArrayInt::accumulate(int compId) const
{
  checkAllocated();
  const int *ptr=getConstPointer();
  int nbTuple=getNumberOfTuples();
  int nbComps=getNumberOfComponents();
  if(compId<0 || compId>=nbComps)
    throw INTERP_KERNEL::Exception("DataArrayInt::accumulate : Invalid compId specified : No such nb of components !");
  int ret=0;
  for(int i=0;i<nbTuple;i++)
    ret+=ptr[i*nbComps+compId];
  return ret;
}

/*!
 * This method accumulate using addition tuples in \a this using input index array [ \a bgOfIndex, \a endOfIndex ).
 * The returned array will have same number of components than \a this and number of tuples equal to
 * \c std::distance(bgOfIndex,endOfIndex) \b minus \b one.
 *
 * The input index array is expected to be ascendingly sorted in which the all referenced ids should be in [0, \c this->getNumberOfTuples).
 *
 * \param [in] bgOfIndex - begin (included) of the input index array.
 * \param [in] endOfIndex - end (excluded) of the input index array.
 * \return DataArrayInt * - the new instance having the same number of components than \a this.
 * 
 * \throw If bgOfIndex or end is NULL.
 * \throw If input index array is not ascendingly sorted.
 * \throw If there is an id in [ \a bgOfIndex, \a endOfIndex ) not in [0, \c this->getNumberOfTuples).
 * \throw If std::distance(bgOfIndex,endOfIndex)==0.
 */
DataArrayInt *DataArrayInt::accumulatePerChunck(const int *bgOfIndex, const int *endOfIndex) const
{
  if(!bgOfIndex || !endOfIndex)
    throw INTERP_KERNEL::Exception("DataArrayInt::accumulatePerChunck : input pointer NULL !");
  checkAllocated();
  int nbCompo=getNumberOfComponents();
  int nbOfTuples=getNumberOfTuples();
  int sz=(int)std::distance(bgOfIndex,endOfIndex);
  if(sz<1)
    throw INTERP_KERNEL::Exception("DataArrayInt::accumulatePerChunck : invalid size of input index array !");
  sz--;
  MCAuto<DataArrayInt> ret=DataArrayInt::New(); ret->alloc(sz,nbCompo);
  const int *w=bgOfIndex;
  if(*w<0 || *w>=nbOfTuples)
    throw INTERP_KERNEL::Exception("DataArrayInt::accumulatePerChunck : The first element of the input index not in [0,nbOfTuples) !");
  const int *srcPt=begin()+(*w)*nbCompo;
  int *tmp=ret->getPointer();
  for(int i=0;i<sz;i++,tmp+=nbCompo,w++)
    {
      std::fill(tmp,tmp+nbCompo,0);
      if(w[1]>=w[0])
        {
          for(int j=w[0];j<w[1];j++,srcPt+=nbCompo)
            {
              if(j>=0 && j<nbOfTuples)
                std::transform(srcPt,srcPt+nbCompo,tmp,tmp,std::plus<int>());
              else
                {
                  std::ostringstream oss; oss << "DataArrayInt::accumulatePerChunck : At rank #" << i << " the input index array points to id " << j << " should be in [0," << nbOfTuples << ") !";
                  throw INTERP_KERNEL::Exception(oss.str().c_str());
                }
            }
        }
      else
        {
          std::ostringstream oss; oss << "DataArrayInt::accumulatePerChunck : At rank #" << i << " the input index array is not in ascendingly sorted.";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  ret->copyStringInfoFrom(*this);
  return ret.retn();
}

/*!
 * Returns a new DataArrayInt by concatenating two given arrays, so that (1) the number
 * of tuples in the result array is <em> a1->getNumberOfTuples() + a2->getNumberOfTuples() -
 * offsetA2</em> and (2)
 * the number of component in the result array is same as that of each of given arrays.
 * First \a offsetA2 tuples of \a a2 are skipped and thus are missing from the result array.
 * Info on components is copied from the first of the given arrays. Number of components
 * in the given arrays must be the same.
 *  \param [in] a1 - an array to include in the result array.
 *  \param [in] a2 - another array to include in the result array.
 *  \param [in] offsetA2 - number of tuples of \a a2 to skip.
 *  \return DataArrayInt * - the new instance of DataArrayInt.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If either \a a1 or \a a2 is NULL.
 *  \throw If \a a1->getNumberOfComponents() != \a a2->getNumberOfComponents().
 */
DataArrayInt *DataArrayInt::Aggregate(const DataArrayInt *a1, const DataArrayInt *a2, int offsetA2)
{
  if(!a1 || !a2)
    throw INTERP_KERNEL::Exception("DataArrayInt::Aggregate : input DataArrayInt instance is NULL !");
  std::size_t nbOfComp(a1->getNumberOfComponents());
  if(nbOfComp!=a2->getNumberOfComponents())
    throw INTERP_KERNEL::Exception("Nb of components mismatch for array Aggregation !");
  std::size_t nbOfTuple1(a1->getNumberOfTuples()),nbOfTuple2(a2->getNumberOfTuples());
  MCAuto<DataArrayInt> ret(DataArrayInt::New());
  ret->alloc(nbOfTuple1+nbOfTuple2-offsetA2,nbOfComp);
  int *pt=std::copy(a1->begin(),a1->end(),ret->getPointer());
  std::copy(a2->getConstPointer()+offsetA2*nbOfComp,a2->getConstPointer()+nbOfTuple2*nbOfComp,pt);
  ret->copyStringInfoFrom(*a1);
  return ret.retn();
}

/*!
 * Returns a new DataArrayInt by concatenating all given arrays, so that (1) the number
 * of tuples in the result array is a sum of the number of tuples of given arrays and (2)
 * the number of component in the result array is same as that of each of given arrays.
 * Info on components is copied from the first of the given arrays. Number of components
 * in the given arrays must be  the same.
 * If the number of non null of elements in \a arr is equal to one the returned object is a copy of it
 * not the object itself.
 *  \param [in] arr - a sequence of arrays to include in the result array.
 *  \return DataArrayInt * - the new instance of DataArrayInt.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If all arrays within \a arr are NULL.
 *  \throw If getNumberOfComponents() of arrays within \a arr.
 */
DataArrayInt *DataArrayInt::Aggregate(const std::vector<const DataArrayInt *>& arr)
{
  std::vector<const DataArrayInt *> a;
  for(std::vector<const DataArrayInt *>::const_iterator it4=arr.begin();it4!=arr.end();it4++)
    if(*it4)
      a.push_back(*it4);
  if(a.empty())
    throw INTERP_KERNEL::Exception("DataArrayInt::Aggregate : input list must be NON EMPTY !");
  std::vector<const DataArrayInt *>::const_iterator it=a.begin();
  std::size_t nbOfComp((*it)->getNumberOfComponents()),nbt((*it++)->getNumberOfTuples());
  for(int i=1;it!=a.end();it++,i++)
    {
      if((*it)->getNumberOfComponents()!=nbOfComp)
        throw INTERP_KERNEL::Exception("DataArrayInt::Aggregate : Nb of components mismatch for array aggregation !");
      nbt+=(*it)->getNumberOfTuples();
    }
  MCAuto<DataArrayInt> ret=DataArrayInt::New();
  ret->alloc(nbt,nbOfComp);
  int *pt=ret->getPointer();
  for(it=a.begin();it!=a.end();it++)
    pt=std::copy((*it)->getConstPointer(),(*it)->getConstPointer()+(*it)->getNbOfElems(),pt);
  ret->copyStringInfoFrom(*(a[0]));
  return ret.retn();
}

/*!
 * This method takes as input a list of DataArrayInt instances \a arrs that represent each a packed index arrays.
 * A packed index array is an allocated array with one component, and at least one tuple. The first element
 * of each array in \a arrs must be 0. Each array in \a arrs is expected to be increasingly monotonic.
 * This method is useful for users that want to aggregate a pair of DataArrayInt representing an indexed data (typically nodal connectivity index in unstructured meshes.
 * 
 * \return DataArrayInt * - a new object to be managed by the caller.
 */
DataArrayInt *DataArrayInt::AggregateIndexes(const std::vector<const DataArrayInt *>& arrs)
{
  int retSz=1;
  for(std::vector<const DataArrayInt *>::const_iterator it4=arrs.begin();it4!=arrs.end();it4++)
    {
      if(*it4)
        {
          (*it4)->checkAllocated();
          if((*it4)->getNumberOfComponents()!=1)
            {
              std::ostringstream oss; oss << "DataArrayInt::AggregateIndexes : presence of a DataArrayInt instance with nb of compo != 1 at pos " << std::distance(arrs.begin(),it4) << " !";
              throw INTERP_KERNEL::Exception(oss.str().c_str());
            }
          int nbTupl=(*it4)->getNumberOfTuples();
          if(nbTupl<1)
            {
              std::ostringstream oss; oss << "DataArrayInt::AggregateIndexes : presence of a DataArrayInt instance with nb of tuples < 1 at pos " << std::distance(arrs.begin(),it4) << " !";
              throw INTERP_KERNEL::Exception(oss.str().c_str());
            }
          if((*it4)->front()!=0)
            {
              std::ostringstream oss; oss << "DataArrayInt::AggregateIndexes : presence of a DataArrayInt instance with front value != 0 at pos " << std::distance(arrs.begin(),it4) << " !";
              throw INTERP_KERNEL::Exception(oss.str().c_str());
            }
          retSz+=nbTupl-1;
        }
      else
        {
          std::ostringstream oss; oss << "DataArrayInt::AggregateIndexes : presence of a null instance at pos " << std::distance(arrs.begin(),it4) << " !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  if(arrs.empty())
    throw INTERP_KERNEL::Exception("DataArrayInt::AggregateIndexes : input list must be NON EMPTY !");
  MCAuto<DataArrayInt> ret=DataArrayInt::New();
  ret->alloc(retSz,1);
  int *pt=ret->getPointer(); *pt++=0;
  for(std::vector<const DataArrayInt *>::const_iterator it=arrs.begin();it!=arrs.end();it++)
    pt=std::transform((*it)->begin()+1,(*it)->end(),pt,std::bind2nd(std::plus<int>(),pt[-1]));
  ret->copyStringInfoFrom(*(arrs[0]));
  return ret.retn();
}

/*!
 * Returns in a single walk in \a this the min value and the max value in \a this.
 * \a this is expected to be single component array.
 *
 * \param [out] minValue - the min value in \a this.
 * \param [out] maxValue - the max value in \a this.
 *
 * \sa getMinValueInArray, getMinValue, getMaxValueInArray, getMaxValue
 */
void DataArrayInt::getMinMaxValues(int& minValue, int& maxValue) const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::getMinMaxValues : must be applied on DataArrayInt with only one component !");
  int nbTuples(getNumberOfTuples());
  const int *pt(begin());
  minValue=std::numeric_limits<int>::max(); maxValue=-std::numeric_limits<int>::max();
  for(int i=0;i<nbTuples;i++,pt++)
    {
      if(*pt<minValue)
        minValue=*pt;
      if(*pt>maxValue)
        maxValue=*pt;
    }
}

/*!
 * Modify all elements of \a this array, so that
 * an element _x_ becomes \f$ numerator / x \f$.
 *  \warning If an exception is thrown because of presence of 0 element in \a this 
 *           array, all elements processed before detection of the zero element remain
 *           modified.
 *  \param [in] numerator - the numerator used to modify array elements.
 *  \throw If \a this is not allocated.
 *  \throw If there is an element equal to 0 in \a this array.
 */
void DataArrayInt::applyInv(int numerator)
{
  checkAllocated();
  int *ptr=getPointer();
  std::size_t nbOfElems=getNbOfElems();
  for(std::size_t i=0;i<nbOfElems;i++,ptr++)
    {
      if(*ptr!=0)
        {
          *ptr=numerator/(*ptr);
        }
      else
        {
          std::ostringstream oss; oss << "DataArrayInt::applyInv : presence of null value in tuple #" << i/getNumberOfComponents() << " component #" << i%getNumberOfComponents();
          oss << " !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  declareAsNew();
}

/*!
 * Modify all elements of \a this array, so that
 * an element _x_ becomes \f$ x / val \f$.
 *  \param [in] val - the denominator used to modify array elements.
 *  \throw If \a this is not allocated.
 *  \throw If \a val == 0.
 */
void DataArrayInt::applyDivideBy(int val)
{
  if(val==0)
    throw INTERP_KERNEL::Exception("DataArrayInt::applyDivideBy : Trying to divide by 0 !");
  checkAllocated();
  int *ptr=getPointer();
  std::size_t nbOfElems=getNbOfElems();
  std::transform(ptr,ptr+nbOfElems,ptr,std::bind2nd(std::divides<int>(),val));
  declareAsNew();
}

/*!
 * Modify all elements of \a this array, so that
 * an element _x_ becomes  <em> x % val </em>.
 *  \param [in] val - the divisor used to modify array elements.
 *  \throw If \a this is not allocated.
 *  \throw If \a val <= 0.
 */
void DataArrayInt::applyModulus(int val)
{
  if(val<=0)
    throw INTERP_KERNEL::Exception("DataArrayInt::applyDivideBy : Trying to operate modulus on value <= 0 !");
  checkAllocated();
  int *ptr=getPointer();
  std::size_t nbOfElems=getNbOfElems();
  std::transform(ptr,ptr+nbOfElems,ptr,std::bind2nd(std::modulus<int>(),val));
  declareAsNew();
}

/*!
 * This method works only on data array with one component.
 * This method returns a newly allocated array storing stored ascendantly tuple ids in \b this so that
 * this[*id] in [\b vmin,\b vmax)
 * 
 * \param [in] vmin begin of range. This value is included in range (included).
 * \param [in] vmax end of range. This value is \b not included in range (excluded).
 * \return a newly allocated data array that the caller should deal with.
 *
 * \sa DataArrayInt::findIdsNotInRange , DataArrayInt::findIdsStricltyNegative
 */
DataArrayInt *DataArrayInt::findIdsInRange(int vmin, int vmax) const
{
  InRange<int> ir(vmin,vmax);
  MCAuto<DataArrayInt> ret(findIdsAdv(ir));
  return ret.retn();
}

/*!
 * This method works only on data array with one component.
 * This method returns a newly allocated array storing stored ascendantly tuple ids in \b this so that
 * this[*id] \b not in [\b vmin,\b vmax)
 * 
 * \param [in] vmin begin of range. This value is \b not included in range (excluded).
 * \param [in] vmax end of range. This value is included in range (included).
 * \return a newly allocated data array that the caller should deal with.
 * 
 * \sa DataArrayInt::findIdsInRange , DataArrayInt::findIdsStricltyNegative
 */
DataArrayInt *DataArrayInt::findIdsNotInRange(int vmin, int vmax) const
{
  NotInRange<int> nir(vmin,vmax);
  MCAuto<DataArrayInt> ret(findIdsAdv(nir));
  return ret.retn();
}

/*!
 * This method works only on data array with one component.
 * This method checks that all ids in \b this are in [ \b vmin, \b vmax ). If there is at least one element in \a this not in [ \b vmin, \b vmax ) an exception will be thrown.
 * 
 * \param [in] vmin begin of range. This value is included in range (included).
 * \param [in] vmax end of range. This value is \b not included in range (excluded).
 * \return if all ids in \a this are so that (*this)[i]==i for all i in [ 0, \c this->getNumberOfTuples() ). */
bool DataArrayInt::checkAllIdsInRange(int vmin, int vmax) const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::checkAllIdsInRange : this must have exactly one component !");
  int nbOfTuples=getNumberOfTuples();
  bool ret=true;
  const int *cptr=getConstPointer();
  for(int i=0;i<nbOfTuples;i++,cptr++)
    {
      if(*cptr>=vmin && *cptr<vmax)
        { ret=ret && *cptr==i; }
      else
        {
          std::ostringstream oss; oss << "DataArrayInt::checkAllIdsInRange : tuple #" << i << " has value " << *cptr << " should be in [" << vmin << "," << vmax << ") !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  return ret;
}

/*!
 * Modify all elements of \a this array, so that
 * an element _x_ becomes <em> val % x </em>.
 *  \warning If an exception is thrown because of presence of an element <= 0 in \a this 
 *           array, all elements processed before detection of the zero element remain
 *           modified.
 *  \param [in] val - the divident used to modify array elements.
 *  \throw If \a this is not allocated.
 *  \throw If there is an element equal to or less than 0 in \a this array.
 */
void DataArrayInt::applyRModulus(int val)
{
  checkAllocated();
  int *ptr=getPointer();
  std::size_t nbOfElems=getNbOfElems();
  for(std::size_t i=0;i<nbOfElems;i++,ptr++)
    {
      if(*ptr>0)
        {
          *ptr=val%(*ptr);
        }
      else
        {
          std::ostringstream oss; oss << "DataArrayInt::applyRModulus : presence of value <=0 in tuple #" << i/getNumberOfComponents() << " component #" << i%getNumberOfComponents();
          oss << " !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  declareAsNew();
}

/*!
 * Modify all elements of \a this array, so that
 * an element _x_ becomes <em> val ^ x </em>.
 *  \param [in] val - the value used to apply pow on all array elements.
 *  \throw If \a this is not allocated.
 *  \throw If \a val < 0.
 */
void DataArrayInt::applyPow(int val)
{
  checkAllocated();
  if(val<0)
    throw INTERP_KERNEL::Exception("DataArrayInt::applyPow : input pow in < 0 !");
  int *ptr=getPointer();
  std::size_t nbOfElems=getNbOfElems();
  if(val==0)
    {
      std::fill(ptr,ptr+nbOfElems,1);
      return ;
    }
  for(std::size_t i=0;i<nbOfElems;i++,ptr++)
    {
      int tmp=1;
      for(int j=0;j<val;j++)
        tmp*=*ptr;
      *ptr=tmp;
    }
  declareAsNew();
}

/*!
 * Modify all elements of \a this array, so that
 * an element _x_ becomes \f$ val ^ x \f$.
 *  \param [in] val - the value used to apply pow on all array elements.
 *  \throw If \a this is not allocated.
 *  \throw If there is an element < 0 in \a this array.
 *  \warning If an exception is thrown because of presence of 0 element in \a this 
 *           array, all elements processed before detection of the zero element remain
 *           modified.
 */
void DataArrayInt::applyRPow(int val)
{
  checkAllocated();
  int *ptr=getPointer();
  std::size_t nbOfElems=getNbOfElems();
  for(std::size_t i=0;i<nbOfElems;i++,ptr++)
    {
      if(*ptr>=0)
        {
          int tmp=1;
          for(int j=0;j<*ptr;j++)
            tmp*=val;
          *ptr=tmp;
        }
      else
        {
          std::ostringstream oss; oss << "DataArrayInt::applyRPow : presence of negative value in tuple #" << i/getNumberOfComponents() << " component #" << i%getNumberOfComponents();
          oss << " !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  declareAsNew();
}

/*!
 * Returns a new DataArrayInt which is a minimal partition of elements of \a groups.
 * The i-th item of the result array is an ID of a set of elements belonging to a
 * unique set of groups, which the i-th element is a part of. This set of elements
 * belonging to a unique set of groups is called \a family, so the result array contains
 * IDs of families each element belongs to.
 *
 * \b Example: if we have two groups of elements: \a group1 [0,4] and \a group2 [ 0,1,2 ],
 * then there are 3 families:
 * - \a family1 (with ID 1) contains element [0] belonging to ( \a group1 + \a group2 ),
 * - \a family2 (with ID 2) contains elements [4] belonging to ( \a group1 ),
 * - \a family3 (with ID 3) contains element [1,2] belonging to ( \a group2 ), <br>
 * and the result array contains IDs of families [ 1,3,3,0,2 ]. <br> Note a family ID 0 which
 * stands for the element #3 which is in none of groups.
 *
 *  \param [in] groups - sequence of groups of element IDs.
 *  \param [in] newNb - total number of elements; it must be more than max ID of element
 *         in \a groups.
 *  \param [out] fidsOfGroups - IDs of families the elements of each group belong to.
 *  \return DataArrayInt * - a new instance of DataArrayInt containing IDs of families
 *         each element with ID from range [0, \a newNb ) belongs to. The caller is to
 *         delete this array using decrRef() as it is no more needed.
 *  \throw If any element ID in \a groups violates condition ( 0 <= ID < \a newNb ).
 */
DataArrayInt *DataArrayInt::MakePartition(const std::vector<const DataArrayInt *>& groups, int newNb, std::vector< std::vector<int> >& fidsOfGroups)
{
  std::vector<const DataArrayInt *> groups2;
  for(std::vector<const DataArrayInt *>::const_iterator it4=groups.begin();it4!=groups.end();it4++)
    if(*it4)
      groups2.push_back(*it4);
  MCAuto<DataArrayInt> ret=DataArrayInt::New();
  ret->alloc(newNb,1);
  int *retPtr=ret->getPointer();
  std::fill(retPtr,retPtr+newNb,0);
  int fid=1;
  for(std::vector<const DataArrayInt *>::const_iterator iter=groups2.begin();iter!=groups2.end();iter++)
    {
      const int *ptr=(*iter)->getConstPointer();
      std::size_t nbOfElem=(*iter)->getNbOfElems();
      int sfid=fid;
      for(int j=0;j<sfid;j++)
        {
          bool found=false;
          for(std::size_t i=0;i<nbOfElem;i++)
            {
              if(ptr[i]>=0 && ptr[i]<newNb)
                {
                  if(retPtr[ptr[i]]==j)
                    {
                      retPtr[ptr[i]]=fid;
                      found=true;
                    }
                }
              else
                {
                  std::ostringstream oss; oss << "DataArrayInt::MakePartition : In group \"" << (*iter)->getName() << "\" in tuple #" << i << " value = " << ptr[i] << " ! Should be in [0," << newNb;
                  oss << ") !";
                  throw INTERP_KERNEL::Exception(oss.str().c_str());
                }
            }
          if(found)
            fid++;
        }
    }
  fidsOfGroups.clear();
  fidsOfGroups.resize(groups2.size());
  int grId=0;
  for(std::vector<const DataArrayInt *>::const_iterator iter=groups2.begin();iter!=groups2.end();iter++,grId++)
    {
      std::set<int> tmp;
      const int *ptr=(*iter)->getConstPointer();
      std::size_t nbOfElem=(*iter)->getNbOfElems();
      for(const int *p=ptr;p!=ptr+nbOfElem;p++)
        tmp.insert(retPtr[*p]);
      fidsOfGroups[grId].insert(fidsOfGroups[grId].end(),tmp.begin(),tmp.end());
    }
  return ret.retn();
}

/*!
 * Returns a new DataArrayInt which contains all elements of given one-dimensional
 * arrays. The result array does not contain any duplicates and its values
 * are sorted in ascending order.
 *  \param [in] arr - sequence of DataArrayInt's to unite.
 *  \return DataArrayInt * - a new instance of DataArrayInt. The caller is to delete this
 *         array using decrRef() as it is no more needed.
 *  \throw If any \a arr[i] is not allocated.
 *  \throw If \a arr[i]->getNumberOfComponents() != 1.
 */
DataArrayInt *DataArrayInt::BuildUnion(const std::vector<const DataArrayInt *>& arr)
{
  std::vector<const DataArrayInt *> a;
  for(std::vector<const DataArrayInt *>::const_iterator it4=arr.begin();it4!=arr.end();it4++)
    if(*it4)
      a.push_back(*it4);
  for(std::vector<const DataArrayInt *>::const_iterator it=a.begin();it!=a.end();it++)
    {
      (*it)->checkAllocated();
      if((*it)->getNumberOfComponents()!=1)
        throw INTERP_KERNEL::Exception("DataArrayInt::BuildUnion : only single component allowed !");
    }
  //
  std::set<int> r;
  for(std::vector<const DataArrayInt *>::const_iterator it=a.begin();it!=a.end();it++)
    {
      const int *pt=(*it)->getConstPointer();
      int nbOfTuples=(*it)->getNumberOfTuples();
      r.insert(pt,pt+nbOfTuples);
    }
  DataArrayInt *ret=DataArrayInt::New();
  ret->alloc((int)r.size(),1);
  std::copy(r.begin(),r.end(),ret->getPointer());
  return ret;
}

/*!
 * Returns a new DataArrayInt which contains elements present in each of given one-dimensional
 * arrays. The result array does not contain any duplicates and its values
 * are sorted in ascending order.
 *  \param [in] arr - sequence of DataArrayInt's to intersect.
 *  \return DataArrayInt * - a new instance of DataArrayInt. The caller is to delete this
 *         array using decrRef() as it is no more needed.
 *  \throw If any \a arr[i] is not allocated.
 *  \throw If \a arr[i]->getNumberOfComponents() != 1.
 */
DataArrayInt *DataArrayInt::BuildIntersection(const std::vector<const DataArrayInt *>& arr)
{
  std::vector<const DataArrayInt *> a;
  for(std::vector<const DataArrayInt *>::const_iterator it4=arr.begin();it4!=arr.end();it4++)
    if(*it4)
      a.push_back(*it4);
  for(std::vector<const DataArrayInt *>::const_iterator it=a.begin();it!=a.end();it++)
    {
      (*it)->checkAllocated();
      if((*it)->getNumberOfComponents()!=1)
        throw INTERP_KERNEL::Exception("DataArrayInt::BuildIntersection : only single component allowed !");
    }
  //
  std::set<int> r;
  for(std::vector<const DataArrayInt *>::const_iterator it=a.begin();it!=a.end();it++)
    {
      const int *pt=(*it)->getConstPointer();
      int nbOfTuples=(*it)->getNumberOfTuples();
      std::set<int> s1(pt,pt+nbOfTuples);
      if(it!=a.begin())
        {
          std::set<int> r2;
          std::set_intersection(r.begin(),r.end(),s1.begin(),s1.end(),inserter(r2,r2.end()));
          r=r2;
        }
      else
        r=s1;
    }
  DataArrayInt *ret(DataArrayInt::New());
  ret->alloc((int)r.size(),1);
  std::copy(r.begin(),r.end(),ret->getPointer());
  return ret;
}

/// @cond INTERNAL
namespace MEDCouplingImpl
{
  class OpSwitchedOn
  {
  public:
    OpSwitchedOn(int *pt):_pt(pt),_cnt(0) { }
    void operator()(const bool& b) { if(b) *_pt++=_cnt; _cnt++; }
  private:
    int *_pt;
    int _cnt;
  };

  class OpSwitchedOff
  {
  public:
    OpSwitchedOff(int *pt):_pt(pt),_cnt(0) { }
    void operator()(const bool& b) { if(!b) *_pt++=_cnt; _cnt++; }
  private:
    int *_pt;
    int _cnt;
  };
}
/// @endcond

/*!
 * This method returns the list of ids in ascending mode so that v[id]==true.
 */
DataArrayInt *DataArrayInt::BuildListOfSwitchedOn(const std::vector<bool>& v)
{
  int sz((int)std::count(v.begin(),v.end(),true));
  MCAuto<DataArrayInt> ret(DataArrayInt::New()); ret->alloc(sz,1);
  std::for_each(v.begin(),v.end(),MEDCouplingImpl::OpSwitchedOn(ret->getPointer()));
  return ret.retn();
}

/*!
 * This method returns the list of ids in ascending mode so that v[id]==false.
 */
DataArrayInt *DataArrayInt::BuildListOfSwitchedOff(const std::vector<bool>& v)
{
  int sz((int)std::count(v.begin(),v.end(),false));
  MCAuto<DataArrayInt> ret(DataArrayInt::New()); ret->alloc(sz,1);
  std::for_each(v.begin(),v.end(),MEDCouplingImpl::OpSwitchedOff(ret->getPointer()));
  return ret.retn();
}

/*!
 * This method allows to put a vector of vector of integer into a more compact data structure (skyline). 
 * This method is not available into python because no available optimized data structure available to map std::vector< std::vector<int> >.
 *
 * \param [in] v the input data structure to be translate into skyline format.
 * \param [out] data the first element of the skyline format. The user is expected to deal with newly allocated array.
 * \param [out] dataIndex the second element of the skyline format.
 */
void DataArrayInt::PutIntoToSkylineFrmt(const std::vector< std::vector<int> >& v, DataArrayInt *& data, DataArrayInt *& dataIndex)
{
  int sz((int)v.size());
  MCAuto<DataArrayInt> ret0(DataArrayInt::New()),ret1(DataArrayInt::New());
  ret1->alloc(sz+1,1);
  int *pt(ret1->getPointer()); *pt=0;
  for(int i=0;i<sz;i++,pt++)
    pt[1]=pt[0]+(int)v[i].size();
  ret0->alloc(ret1->back(),1);
  pt=ret0->getPointer();
  for(int i=0;i<sz;i++)
    pt=std::copy(v[i].begin(),v[i].end(),pt);
  data=ret0.retn(); dataIndex=ret1.retn();
}

/*!
 * Returns a new DataArrayInt which contains a complement of elements of \a this
 * one-dimensional array. I.e. the result array contains all elements from the range [0,
 * \a nbOfElement) not present in \a this array.
 *  \param [in] nbOfElement - maximal size of the result array.
 *  \return DataArrayInt * - a new instance of DataArrayInt. The caller is to delete this
 *         array using decrRef() as it is no more needed.
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1.
 *  \throw If any element \a x of \a this array violates condition ( 0 <= \a x < \a
 *         nbOfElement ).
 */
DataArrayInt *DataArrayInt::buildComplement(int nbOfElement) const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::buildComplement : only single component allowed !");
  std::vector<bool> tmp(nbOfElement);
  const int *pt=getConstPointer();
  int nbOfTuples=getNumberOfTuples();
  for(const int *w=pt;w!=pt+nbOfTuples;w++)
    if(*w>=0 && *w<nbOfElement)
      tmp[*w]=true;
    else
      throw INTERP_KERNEL::Exception("DataArrayInt::buildComplement : an element is not in valid range : [0,nbOfElement) !");
  int nbOfRetVal=(int)std::count(tmp.begin(),tmp.end(),false);
  DataArrayInt *ret=DataArrayInt::New();
  ret->alloc(nbOfRetVal,1);
  int j=0;
  int *retPtr=ret->getPointer();
  for(int i=0;i<nbOfElement;i++)
    if(!tmp[i])
      retPtr[j++]=i;
  return ret;
}

/*!
 * Returns a new DataArrayInt containing elements of \a this one-dimensional missing
 * from an \a other one-dimensional array.
 *  \param [in] other - a DataArrayInt containing elements not to include in the result array.
 *  \return DataArrayInt * - a new instance of DataArrayInt with one component. The
 *         caller is to delete this array using decrRef() as it is no more needed.
 *  \throw If \a other is NULL.
 *  \throw If \a other is not allocated.
 *  \throw If \a other->getNumberOfComponents() != 1.
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1.
 *  \sa DataArrayInt::buildSubstractionOptimized()
 */
DataArrayInt *DataArrayInt::buildSubstraction(const DataArrayInt *other) const
{
  if(!other)
    throw INTERP_KERNEL::Exception("DataArrayInt::buildSubstraction : DataArrayInt pointer in input is NULL !");
  checkAllocated();
  other->checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::buildSubstraction : only single component allowed !");
  if(other->getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::buildSubstraction : only single component allowed for other type !");
  const int *pt=getConstPointer();
  int nbOfTuples=getNumberOfTuples();
  std::set<int> s1(pt,pt+nbOfTuples);
  pt=other->getConstPointer();
  nbOfTuples=other->getNumberOfTuples();
  std::set<int> s2(pt,pt+nbOfTuples);
  std::vector<int> r;
  std::set_difference(s1.begin(),s1.end(),s2.begin(),s2.end(),std::back_insert_iterator< std::vector<int> >(r));
  DataArrayInt *ret=DataArrayInt::New();
  ret->alloc((int)r.size(),1);
  std::copy(r.begin(),r.end(),ret->getPointer());
  return ret;
}

/*!
 * \a this is expected to have one component and to be sorted ascendingly (as for \a other).
 * \a other is expected to be a part of \a this. If not DataArrayInt::buildSubstraction should be called instead.
 * 
 * \param [in] other an array with one component and expected to be sorted ascendingly.
 * \ret list of ids in \a this but not in \a other.
 * \sa DataArrayInt::buildSubstraction
 */
DataArrayInt *DataArrayInt::buildSubstractionOptimized(const DataArrayInt *other) const
{
  static const char *MSG="DataArrayInt::buildSubstractionOptimized : only single component allowed !";
  if(!other) throw INTERP_KERNEL::Exception("DataArrayInt::buildSubstractionOptimized : NULL input array !");
  checkAllocated(); other->checkAllocated();
  if(getNumberOfComponents()!=1) throw INTERP_KERNEL::Exception(MSG);
  if(other->getNumberOfComponents()!=1) throw INTERP_KERNEL::Exception(MSG);
  const int *pt1Bg(begin()),*pt1End(end()),*pt2Bg(other->begin()),*pt2End(other->end());
  const int *work1(pt1Bg),*work2(pt2Bg);
  MCAuto<DataArrayInt> ret(DataArrayInt::New()); ret->alloc(0,1);
  for(;work1!=pt1End;work1++)
    {
      if(work2!=pt2End && *work1==*work2)
        work2++;
      else
        ret->pushBackSilent(*work1);
    }
  return ret.retn();
}


/*!
 * Returns a new DataArrayInt which contains all elements of \a this and a given
 * one-dimensional arrays. The result array does not contain any duplicates
 * and its values are sorted in ascending order.
 *  \param [in] other - an array to unite with \a this one.
 *  \return DataArrayInt * - a new instance of DataArrayInt. The caller is to delete this
 *         array using decrRef() as it is no more needed.
 *  \throw If \a this or \a other is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1.
 *  \throw If \a other->getNumberOfComponents() != 1.
 */
DataArrayInt *DataArrayInt::buildUnion(const DataArrayInt *other) const
{
  std::vector<const DataArrayInt *>arrs(2);
  arrs[0]=this; arrs[1]=other;
  return BuildUnion(arrs);
}


/*!
 * Returns a new DataArrayInt which contains elements present in both \a this and a given
 * one-dimensional arrays. The result array does not contain any duplicates
 * and its values are sorted in ascending order.
 *  \param [in] other - an array to intersect with \a this one.
 *  \return DataArrayInt * - a new instance of DataArrayInt. The caller is to delete this
 *         array using decrRef() as it is no more needed.
 *  \throw If \a this or \a other is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1.
 *  \throw If \a other->getNumberOfComponents() != 1.
 */
DataArrayInt *DataArrayInt::buildIntersection(const DataArrayInt *other) const
{
  std::vector<const DataArrayInt *>arrs(2);
  arrs[0]=this; arrs[1]=other;
  return BuildIntersection(arrs);
}

/*!
 * This method can be applied on allocated with one component DataArrayInt instance.
 * This method is typically relevant for sorted arrays. All consecutive duplicated items in \a this will appear only once in returned DataArrayInt instance.
 * Example : if \a this contains [1,2,2,3,3,3,3,4,5,5,7,7,7,19] the returned array will contain [1,2,3,4,5,7,19]
 * 
 * \return a newly allocated array that contain the result of the unique operation applied on \a this.
 * \throw if \a this is not allocated or if \a this has not exactly one component.
 * \sa DataArrayInt::buildUniqueNotSorted
 */
DataArrayInt *DataArrayInt::buildUnique() const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::buildUnique : only single component allowed !");
  int nbOfTuples=getNumberOfTuples();
  MCAuto<DataArrayInt> tmp=deepCopy();
  int *data=tmp->getPointer();
  int *last=std::unique(data,data+nbOfTuples);
  MCAuto<DataArrayInt> ret=DataArrayInt::New();
  ret->alloc(std::distance(data,last),1);
  std::copy(data,last,ret->getPointer());
  return ret.retn();
}

/*!
 * This method can be applied on allocated with one component DataArrayInt instance.
 * This method keep elements only once by keeping the same order in \a this that is not expected to be sorted.
 *
 * \return a newly allocated array that contain the result of the unique operation applied on \a this.
 *
 * \throw if \a this is not allocated or if \a this has not exactly one component.
 *
 * \sa DataArrayInt::buildUnique
 */
DataArrayInt *DataArrayInt::buildUniqueNotSorted() const
{
  checkAllocated();
    if(getNumberOfComponents()!=1)
      throw INTERP_KERNEL::Exception("DataArrayInt::buildUniqueNotSorted : only single component allowed !");
  int minVal,maxVal;
  getMinMaxValues(minVal,maxVal);
  std::vector<bool> b(maxVal-minVal+1,false);
  const int *ptBg(begin()),*endBg(end());
  MCAuto<DataArrayInt> ret(DataArrayInt::New()); ret->alloc(0,1);
  for(const int *pt=ptBg;pt!=endBg;pt++)
    {
      if(!b[*pt-minVal])
        {
          ret->pushBackSilent(*pt);
          b[*pt-minVal]=true;
        }
    }
  ret->copyStringInfoFrom(*this);
  return ret.retn();
}

/*!
 * Returns a new DataArrayInt which contains size of every of groups described by \a this
 * "index" array. Such "index" array is returned for example by 
 * \ref MEDCoupling::MEDCouplingUMesh::buildDescendingConnectivity
 * "MEDCouplingUMesh::buildDescendingConnectivity" and
 * \ref MEDCoupling::MEDCouplingUMesh::getNodalConnectivityIndex
 * "MEDCouplingUMesh::getNodalConnectivityIndex" etc.
 * This method performs the reverse operation of DataArrayInt::computeOffsetsFull.
 *  \return DataArrayInt * - a new instance of DataArrayInt, whose number of tuples
 *          equals to \a this->getNumberOfComponents() - 1, and number of components is 1.
 *          The caller is to delete this array using decrRef() as it is no more needed. 
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1.
 *  \throw If \a this->getNumberOfTuples() < 2.
 *
 *  \b Example: <br> 
 *         - this contains [1,3,6,7,7,9,15]
 *         - result array contains [2,3,1,0,2,6],
 *          where 2 = 3 - 1, 3 = 6 - 3, 1 = 7 - 6 etc.
 *
 * \sa DataArrayInt::computeOffsetsFull
 */
DataArrayInt *DataArrayInt::deltaShiftIndex() const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::deltaShiftIndex : only single component allowed !");
  int nbOfTuples=getNumberOfTuples();
  if(nbOfTuples<2)
    throw INTERP_KERNEL::Exception("DataArrayInt::deltaShiftIndex : 1 tuple at least must be present in 'this' !");
  const int *ptr=getConstPointer();
  DataArrayInt *ret=DataArrayInt::New();
  ret->alloc(nbOfTuples-1,1);
  int *out=ret->getPointer();
  std::transform(ptr+1,ptr+nbOfTuples,ptr,out,std::minus<int>());
  return ret;
}

/*!
 * Modifies \a this one-dimensional array so that value of each element \a x
 * of \a this array (\a a) is computed as \f$ x_i = \sum_{j=0}^{i-1} a[ j ] \f$.
 * Or: for each i>0 new[i]=new[i-1]+old[i-1] for i==0 new[i]=0. Number of tuples
 * and components remains the same.<br>
 * This method is useful for allToAllV in MPI with contiguous policy. This method
 * differs from computeOffsetsFull() in that the number of tuples is \b not changed by
 * this one.
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1.
 *
 *  \b Example: <br>
 *          - Before \a this contains [3,5,1,2,0,8]
 *          - After \a this contains  [0,3,8,9,11,11]<br>
 *          Note that the last element 19 = 11 + 8 is missing because size of \a this
 *          array is retained and thus there is no space to store the last element.
 */
void DataArrayInt::computeOffsets()
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::computeOffsets : only single component allowed !");
  int nbOfTuples=getNumberOfTuples();
  if(nbOfTuples==0)
    return ;
  int *work=getPointer();
  int tmp=work[0];
  work[0]=0;
  for(int i=1;i<nbOfTuples;i++)
    {
      int tmp2=work[i];
      work[i]=work[i-1]+tmp;
      tmp=tmp2;
    }
  declareAsNew();
}


/*!
 * Modifies \a this one-dimensional array so that value of each element \a x
 * of \a this array (\a a) is computed as \f$ x_i = \sum_{j=0}^{i-1} a[ j ] \f$.
 * Or: for each i>0 new[i]=new[i-1]+old[i-1] for i==0 new[i]=0. Number
 * components remains the same and number of tuples is inceamented by one.<br>
 * This method is useful for allToAllV in MPI with contiguous policy. This method
 * differs from computeOffsets() in that the number of tuples is changed by this one.
 * This method performs the reverse operation of DataArrayInt::deltaShiftIndex.
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1.
 *
 *  \b Example: <br>
 *          - Before \a this contains [3,5,1,2,0,8]
 *          - After \a this contains  [0,3,8,9,11,11,19]<br>
 * \sa DataArrayInt::deltaShiftIndex
 */
void DataArrayInt::computeOffsetsFull()
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::computeOffsetsFull : only single component allowed !");
  int nbOfTuples=getNumberOfTuples();
  int *ret=(int *)malloc((nbOfTuples+1)*sizeof(int));
  const int *work=getConstPointer();
  ret[0]=0;
  for(int i=0;i<nbOfTuples;i++)
    ret[i+1]=work[i]+ret[i];
  useArray(ret,true,C_DEALLOC,nbOfTuples+1,1);
  declareAsNew();
}

/*!
 * Returns two new DataArrayInt instances whose contents is computed from that of \a this and \a listOfIds arrays as follows.
 * \a this is expected to be an offset format ( as returned by DataArrayInt::computeOffsetsFull ) that is to say with one component
 * and ** sorted strictly increasingly **. \a listOfIds is expected to be sorted ascendingly (not strictly needed for \a listOfIds).
 * This methods searches in \a this, considered as a set of contiguous \c this->getNumberOfComponents() ranges, all ids in \a listOfIds
 * filling completely one of the ranges in \a this.
 *
 * \param [in] listOfIds a list of ids that has to be sorted ascendingly.
 * \param [out] rangeIdsFetched the range ids fetched
 * \param [out] idsInInputListThatFetch contains the list of ids in \a listOfIds that are \b fully included in a range in \a this. So
 *              \a idsInInputListThatFetch is a part of input \a listOfIds.
 *
 * \sa DataArrayInt::computeOffsetsFull
 *
 *  \b Example: <br>
 *          - \a this : [0,3,7,9,15,18]
 *          - \a listOfIds contains  [0,1,2,3,7,8,15,16,17]
 *          - \a rangeIdsFetched result array: [0,2,4]
 *          - \a idsInInputListThatFetch result array: [0,1,2,7,8,15,16,17]
 * In this example id 3 in input \a listOfIds is alone so it do not appear in output \a idsInInputListThatFetch.
 * <br>
 */
void DataArrayInt::findIdsRangesInListOfIds(const DataArrayInt *listOfIds, DataArrayInt *& rangeIdsFetched, DataArrayInt *& idsInInputListThatFetch) const
{
  if(!listOfIds)
    throw INTERP_KERNEL::Exception("DataArrayInt::findIdsRangesInListOfIds : input list of ids is null !");
  listOfIds->checkAllocated(); checkAllocated();
  if(listOfIds->getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::findIdsRangesInListOfIds : input list of ids must have exactly one component !");
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::findIdsRangesInListOfIds : this must have exactly one component !");
  MCAuto<DataArrayInt> ret0=DataArrayInt::New(); ret0->alloc(0,1);
  MCAuto<DataArrayInt> ret1=DataArrayInt::New(); ret1->alloc(0,1);
  const int *tupEnd(listOfIds->end()),*offBg(begin()),*offEnd(end()-1);
  const int *tupPtr(listOfIds->begin()),*offPtr(offBg);
  while(tupPtr!=tupEnd && offPtr!=offEnd)
    {
      if(*tupPtr==*offPtr)
        {
          int i=offPtr[0];
          while(i<offPtr[1] && *tupPtr==i && tupPtr!=tupEnd) { i++; tupPtr++; }
          if(i==offPtr[1])
            {
              ret0->pushBackSilent((int)std::distance(offBg,offPtr));
              ret1->pushBackValsSilent(tupPtr-(offPtr[1]-offPtr[0]),tupPtr);
              offPtr++;
            }
        }
      else
        { if(*tupPtr<*offPtr) tupPtr++; else offPtr++; }
    }
  rangeIdsFetched=ret0.retn();
  idsInInputListThatFetch=ret1.retn();
}

/*!
 * Returns a new DataArrayInt whose contents is computed from that of \a this and \a
 * offsets arrays as follows. \a offsets is a one-dimensional array considered as an
 * "index" array of a "iota" array, thus, whose each element gives an index of a group
 * beginning within the "iota" array. And \a this is a one-dimensional array
 * considered as a selector of groups described by \a offsets to include into the result array.
 *  \throw If \a offsets is NULL.
 *  \throw If \a offsets is not allocated.
 *  \throw If \a offsets->getNumberOfComponents() != 1.
 *  \throw If \a offsets is not monotonically increasing.
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1.
 *  \throw If any element of \a this is not a valid index for \a offsets array.
 *
 *  \b Example: <br>
 *          - \a this: [0,2,3]
 *          - \a offsets: [0,3,6,10,14,20]
 *          - result array: [0,1,2,6,7,8,9,10,11,12,13] == <br>
 *            \c range(0,3) + \c range(6,10) + \c range(10,14) ==<br>
 *            \c range( \a offsets[ \a this[0] ], offsets[ \a this[0]+1 ]) + 
 *            \c range( \a offsets[ \a this[1] ], offsets[ \a this[1]+1 ]) + 
 *            \c range( \a offsets[ \a this[2] ], offsets[ \a this[2]+1 ])
 */
DataArrayInt *DataArrayInt::buildExplicitArrByRanges(const DataArrayInt *offsets) const
{
  if(!offsets)
    throw INTERP_KERNEL::Exception("DataArrayInt::buildExplicitArrByRanges : DataArrayInt pointer in input is NULL !");
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::buildExplicitArrByRanges : only single component allowed !");
  offsets->checkAllocated();
  if(offsets->getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::buildExplicitArrByRanges : input array should have only single component !");
  int othNbTuples=offsets->getNumberOfTuples()-1;
  int nbOfTuples=getNumberOfTuples();
  int retNbOftuples=0;
  const int *work=getConstPointer();
  const int *offPtr=offsets->getConstPointer();
  for(int i=0;i<nbOfTuples;i++)
    {
      int val=work[i];
      if(val>=0 && val<othNbTuples)
        {
          int delta=offPtr[val+1]-offPtr[val];
          if(delta>=0)
            retNbOftuples+=delta;
          else
            {
              std::ostringstream oss; oss << "DataArrayInt::buildExplicitArrByRanges : Tuple #" << val << " of offset array has a delta < 0 !";
              throw INTERP_KERNEL::Exception(oss.str().c_str());
            }
        }
      else
        {
          std::ostringstream oss; oss << "DataArrayInt::buildExplicitArrByRanges : Tuple #" << i << " in this contains " << val;
          oss << " whereas offsets array is of size " << othNbTuples+1 << " !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  MCAuto<DataArrayInt> ret=DataArrayInt::New();
  ret->alloc(retNbOftuples,1);
  int *retPtr=ret->getPointer();
  for(int i=0;i<nbOfTuples;i++)
    {
      int val=work[i];
      int start=offPtr[val];
      int off=offPtr[val+1]-start;
      for(int j=0;j<off;j++,retPtr++)
        *retPtr=start+j;
    }
  return ret.retn();
}

/*!
 * Returns a new DataArrayInt whose contents is computed using \a this that must be a 
 * scaled array (monotonically increasing).
from that of \a this and \a
 * offsets arrays as follows. \a offsets is a one-dimensional array considered as an
 * "index" array of a "iota" array, thus, whose each element gives an index of a group
 * beginning within the "iota" array. And \a this is a one-dimensional array
 * considered as a selector of groups described by \a offsets to include into the result array.
 *  \throw If \a  is NULL.
 *  \throw If \a this is not allocated.
 *  \throw If \a this->getNumberOfComponents() != 1.
 *  \throw If \a this->getNumberOfTuples() == 0.
 *  \throw If \a this is not monotonically increasing.
 *  \throw If any element of ids in ( \a bg \a stop \a step ) points outside the scale in \a this.
 *
 *  \b Example: <br>
 *          - \a bg , \a stop and \a step : (0,5,2)
 *          - \a this: [0,3,6,10,14,20]
 *          - result array: [0,0,0, 2,2,2,2, 4,4,4,4,4,4] == <br>
 */
DataArrayInt *DataArrayInt::buildExplicitArrOfSliceOnScaledArr(int bg, int stop, int step) const
{
  if(!isAllocated())
    throw INTERP_KERNEL::Exception("DataArrayInt::buildExplicitArrOfSliceOnScaledArr : not allocated array !");
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::buildExplicitArrOfSliceOnScaledArr : number of components is expected to be equal to one !");
  int nbOfTuples(getNumberOfTuples());
  if(nbOfTuples==0)
    throw INTERP_KERNEL::Exception("DataArrayInt::buildExplicitArrOfSliceOnScaledArr : number of tuples must be != 0 !");
  const int *ids(begin());
  int nbOfEltsInSlc(GetNumberOfItemGivenBESRelative(bg,stop,step,"DataArrayInt::buildExplicitArrOfSliceOnScaledArr")),sz(0),pos(bg);
  for(int i=0;i<nbOfEltsInSlc;i++,pos+=step)
    {
      if(pos>=0 && pos<nbOfTuples-1)
        {
          int delta(ids[pos+1]-ids[pos]);
          sz+=delta;
          if(delta<0)
            {
              std::ostringstream oss; oss << "DataArrayInt::buildExplicitArrOfSliceOnScaledArr : At pos #" << i << " of input slice, value is " << pos << " and at this pos this is not monotonically increasing !";
              throw INTERP_KERNEL::Exception(oss.str().c_str());
            }          
        }
      else
        {
          std::ostringstream oss; oss << "DataArrayInt::buildExplicitArrOfSliceOnScaledArr : At pos #" << i << " of input slice, value is " << pos << " should be in [0," << nbOfTuples-1 << ") !";  
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  MCAuto<DataArrayInt> ret(DataArrayInt::New()); ret->alloc(sz,1);
  int *retPtr(ret->getPointer());
  pos=bg;
  for(int i=0;i<nbOfEltsInSlc;i++,pos+=step)
    {
      int delta(ids[pos+1]-ids[pos]);
      for(int j=0;j<delta;j++,retPtr++)
        *retPtr=pos;
    }
  return ret.retn();
}

/*!
 * Given in input ranges \a ranges, it returns a newly allocated DataArrayInt instance having one component and the same number of tuples than \a this.
 * For each tuple at place **i** in \a this it tells which is the first range in \a ranges that contains value \c this->getIJ(i,0) and put the result
 * in tuple **i** of returned DataArrayInt.
 * If ranges overlapped (in theory it should not) this method do not detect it and always returns the first range.
 *
 * For example if \a this contains : [1,24,7,8,10,17] and \a ranges contains [(0,3),(3,8),(8,15),(15,22),(22,30)]
 * The return DataArrayInt will contain : **[0,4,1,2,2,3]**
 * 
 * \param [in] ranges typically come from output of MEDCouplingUMesh::ComputeRangesFromTypeDistribution. Each range is specified like this : 1st component is
 *             for lower value included and 2nd component is the upper value of corresponding range **excluded**.
 * \throw If offsets is a null pointer or does not have 2 components or if \a this is not allocated or \a this do not have exactly one component. To finish an exception
 *        is thrown if no ranges in \a ranges contains value in \a this.
 * 
 * \sa DataArrayInt::findIdInRangeForEachTuple
 */
DataArrayInt *DataArrayInt::findRangeIdForEachTuple(const DataArrayInt *ranges) const
{
  if(!ranges)
    throw INTERP_KERNEL::Exception("DataArrayInt::findRangeIdForEachTuple : null input pointer !");
  if(ranges->getNumberOfComponents()!=2)
    throw INTERP_KERNEL::Exception("DataArrayInt::findRangeIdForEachTuple : input DataArrayInt instance should have 2 components !");
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::findRangeIdForEachTuple : this should have only one component !");
  int nbTuples=getNumberOfTuples();
  MCAuto<DataArrayInt> ret=DataArrayInt::New(); ret->alloc(nbTuples,1);
  int nbOfRanges=ranges->getNumberOfTuples();
  const int *rangesPtr=ranges->getConstPointer();
  int *retPtr=ret->getPointer();
  const int *inPtr=getConstPointer();
  for(int i=0;i<nbTuples;i++,retPtr++)
    {
      int val=inPtr[i];
      bool found=false;
      for(int j=0;j<nbOfRanges && !found;j++)
        if(val>=rangesPtr[2*j] && val<rangesPtr[2*j+1])
          { *retPtr=j; found=true; }
      if(found)
        continue;
      else
        {
          std::ostringstream oss; oss << "DataArrayInt::findRangeIdForEachTuple : tuple #" << i << " not found by any ranges !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  return ret.retn();
}

/*!
 * Given in input ranges \a ranges, it returns a newly allocated DataArrayInt instance having one component and the same number of tuples than \a this.
 * For each tuple at place **i** in \a this it tells which is the sub position of the first range in \a ranges that contains value \c this->getIJ(i,0) and put the result
 * in tuple **i** of returned DataArrayInt.
 * If ranges overlapped (in theory it should not) this method do not detect it and always returns the sub position of the first range.
 *
 * For example if \a this contains : [1,24,7,8,10,17] and \a ranges contains [(0,3),(3,8),(8,15),(15,22),(22,30)]
 * The return DataArrayInt will contain : **[1,2,4,0,2,2]**
 * This method is often called in pair with DataArrayInt::findRangeIdForEachTuple method.
 * 
 * \param [in] ranges typically come from output of MEDCouplingUMesh::ComputeRangesFromTypeDistribution. Each range is specified like this : 1st component is
 *             for lower value included and 2nd component is the upper value of corresponding range **excluded**.
 * \throw If offsets is a null pointer or does not have 2 components or if \a this is not allocated or \a this do not have exactly one component. To finish an exception
 *        is thrown if no ranges in \a ranges contains value in \a this.
 * \sa DataArrayInt::findRangeIdForEachTuple
 */
DataArrayInt *DataArrayInt::findIdInRangeForEachTuple(const DataArrayInt *ranges) const
{
  if(!ranges)
    throw INTERP_KERNEL::Exception("DataArrayInt::findIdInRangeForEachTuple : null input pointer !");
  if(ranges->getNumberOfComponents()!=2)
    throw INTERP_KERNEL::Exception("DataArrayInt::findIdInRangeForEachTuple : input DataArrayInt instance should have 2 components !");
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::findIdInRangeForEachTuple : this should have only one component !");
  int nbTuples=getNumberOfTuples();
  MCAuto<DataArrayInt> ret=DataArrayInt::New(); ret->alloc(nbTuples,1);
  int nbOfRanges=ranges->getNumberOfTuples();
  const int *rangesPtr=ranges->getConstPointer();
  int *retPtr=ret->getPointer();
  const int *inPtr=getConstPointer();
  for(int i=0;i<nbTuples;i++,retPtr++)
    {
      int val=inPtr[i];
      bool found=false;
      for(int j=0;j<nbOfRanges && !found;j++)
        if(val>=rangesPtr[2*j] && val<rangesPtr[2*j+1])
          { *retPtr=val-rangesPtr[2*j]; found=true; }
      if(found)
        continue;
      else
        {
          std::ostringstream oss; oss << "DataArrayInt::findIdInRangeForEachTuple : tuple #" << i << " not found by any ranges !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  return ret.retn();
}

/*!
 * \b WARNING this method is a \b non \a const \b method. This method works tuple by tuple. Each tuple is expected to be pairs (number of components must be equal to 2).
 * This method rearrange each pair in \a this so that, tuple with id \b tid will be after the call \c this->getIJ(tid,0)==this->getIJ(tid-1,1) and \c this->getIJ(tid,1)==this->getIJ(tid+1,0).
 * If it is impossible to reach such condition an exception will be thrown ! \b WARNING In case of throw \a this can be partially modified !
 * If this method has correctly worked, \a this will be able to be considered as a linked list.
 * This method does nothing if number of tuples is lower of equal to 1.
 *
 * This method is useful for users having an unstructured mesh having only SEG2 to rearrange internaly the connectibity without any coordinates consideration.
 *
 * \sa MEDCouplingUMesh::orderConsecutiveCells1D, DataArrayInt::fromLinkedListOfPairToList
 */
void DataArrayInt::sortEachPairToMakeALinkedList()
{
  checkAllocated();
  if(getNumberOfComponents()!=2)
    throw INTERP_KERNEL::Exception("DataArrayInt::sortEachPairToMakeALinkedList : Only works on DataArrayInt instance with nb of components equal to 2 !");
  int nbOfTuples(getNumberOfTuples());
  if(nbOfTuples<=1)
    return ;
  int *conn(getPointer());
  for(int i=1;i<nbOfTuples;i++,conn+=2)
    {
      if(i>1)
        {
          if(conn[2]==conn[3])
            {
              std::ostringstream oss; oss << "DataArrayInt::sortEachPairToMakeALinkedList : In the tuple #" << i << " presence of a pair filled with same ids !";
              throw INTERP_KERNEL::Exception(oss.str().c_str());
            }
          if(conn[2]!=conn[1] && conn[3]==conn[1] && conn[2]!=conn[0])
            std::swap(conn[2],conn[3]);
          //not(conn[2]==conn[1] && conn[3]!=conn[1] && conn[3]!=conn[0])
          if(conn[2]!=conn[1] || conn[3]==conn[1] || conn[3]==conn[0])
            {
              std::ostringstream oss; oss << "DataArrayInt::sortEachPairToMakeALinkedList : In the tuple #" << i << " something is invalid !";
              throw INTERP_KERNEL::Exception(oss.str().c_str());
            }
        }
      else
        {
          if(conn[0]==conn[1] || conn[2]==conn[3])
            throw INTERP_KERNEL::Exception("DataArrayInt::sortEachPairToMakeALinkedList : In the 2 first tuples presence of a pair filled with same ids !");
          int tmp[4];
          std::set<int> s;
          s.insert(conn,conn+4);
          if(s.size()!=3)
            throw INTERP_KERNEL::Exception("DataArrayInt::sortEachPairToMakeALinkedList : This can't be considered as a linked list regarding 2 first tuples !");
          if(std::count(conn,conn+4,conn[0])==2)
            {
              tmp[0]=conn[1];
              tmp[1]=conn[0];
              tmp[2]=conn[0];
              if(conn[2]==conn[0])
                { tmp[3]=conn[3]; }
              else
                { tmp[3]=conn[2];}
              std::copy(tmp,tmp+4,conn);
            }
          else
            {//here we are sure to have (std::count(conn,conn+4,conn[1])==2)
              if(conn[1]==conn[3])
                std::swap(conn[2],conn[3]);
            }
        }
    }
}

/*!
 * \a this is expected to be a correctly linked list of pairs.
 * 
 * \sa DataArrayInt::sortEachPairToMakeALinkedList
 */
MCAuto<DataArrayInt> DataArrayInt::fromLinkedListOfPairToList() const
{
  checkAllocated();
  checkNbOfComps(2,"DataArrayInt::fromLinkedListOfPairToList : this is expected to have 2 components");
  int nbTuples(getNumberOfTuples());
  if(nbTuples<1)
    throw INTERP_KERNEL::Exception("DataArrayInt::fromLinkedListOfPairToList : no tuples in this ! Not a linked list !");
  MCAuto<DataArrayInt> ret(DataArrayInt::New()); ret->alloc(nbTuples+1,1);
  const int *thisPtr(begin());
  int *retPtr(ret->getPointer());
  retPtr[0]=thisPtr[0];
  for(int i=0;i<nbTuples;i++)
    {
      retPtr[i+1]=thisPtr[2*i+1];
      if(i<nbTuples-1)
        if(thisPtr[2*i+1]!=thisPtr[2*(i+1)+0])
          {
            std::ostringstream oss; oss << "DataArrayInt::fromLinkedListOfPairToList : this is not a proper linked list of pair. The link is broken between tuple #" << i << " and tuple #" << i+1 << " ! Call sortEachPairToMakeALinkedList ?";
            throw INTERP_KERNEL::Exception(oss.str());
          }
    }
  return ret;
}

/*!
 * This method returns all different values found in \a this. This method throws if \a this has not been allocated.
 * But the number of components can be different from one.
 * \return a newly allocated array (that should be dealt by the caller) containing different values in \a this.
 */
DataArrayInt *DataArrayInt::getDifferentValues() const
{
  checkAllocated();
  std::set<int> ret;
  ret.insert(begin(),end());
  MCAuto<DataArrayInt> ret2=DataArrayInt::New(); ret2->alloc((int)ret.size(),1);
  std::copy(ret.begin(),ret.end(),ret2->getPointer());
  return ret2.retn();
}

/*!
 * This method is a refinement of DataArrayInt::getDifferentValues because it returns not only different values in \a this but also, for each of
 * them it tells which tuple id have this id.
 * This method works only on arrays with one component (if it is not the case call DataArrayInt::rearrange(1) ).
 * This method returns two arrays having same size.
 * The instances of DataArrayInt in the returned vector have be specially allocated and computed by this method. Each of them should be dealt by the caller of this method.
 * Example : if this is equal to [1,0,1,2,0,2,2,-3,2] -> differentIds=[-3,0,1,2] and returned array will be equal to [[7],[1,4],[0,2],[3,5,6,8]]
 */
std::vector<DataArrayInt *> DataArrayInt::partitionByDifferentValues(std::vector<int>& differentIds) const
{
  checkAllocated();
  if(getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::partitionByDifferentValues : this should have only one component !");
  int id=0;
  std::map<int,int> m,m2,m3;
  for(const int *w=begin();w!=end();w++)
    m[*w]++;
  differentIds.resize(m.size());
  std::vector<DataArrayInt *> ret(m.size());
  std::vector<int *> retPtr(m.size());
  for(std::map<int,int>::const_iterator it=m.begin();it!=m.end();it++,id++)
    {
      m2[(*it).first]=id;
      ret[id]=DataArrayInt::New();
      ret[id]->alloc((*it).second,1);
      retPtr[id]=ret[id]->getPointer();
      differentIds[id]=(*it).first;
    }
  id=0;
  for(const int *w=begin();w!=end();w++,id++)
    {
      retPtr[m2[*w]][m3[*w]++]=id;
    }
  return ret;
}

/*!
 * This method split ids in [0, \c this->getNumberOfTuples() ) using \a this array as a field of weight (>=0 each).
 * The aim of this method is to return a set of \a nbOfSlices chunk of contiguous ids as balanced as possible.
 *
 * \param [in] nbOfSlices - number of slices expected.
 * \return - a vector having a size equal to \a nbOfSlices giving the start (included) and the stop (excluded) of each chunks.
 * 
 * \sa DataArray::GetSlice
 * \throw If \a this is not allocated or not with exactly one component.
 * \throw If an element in \a this if < 0.
 */
std::vector< std::pair<int,int> > DataArrayInt::splitInBalancedSlices(int nbOfSlices) const
{
  if(!isAllocated() || getNumberOfComponents()!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::splitInBalancedSlices : this array should have number of components equal to one and must be allocated !");
  if(nbOfSlices<=0)
    throw INTERP_KERNEL::Exception("DataArrayInt::splitInBalancedSlices : number of slices must be >= 1 !");
  int sum(accumulate(0)),nbOfTuples(getNumberOfTuples());
  int sumPerSlc(sum/nbOfSlices),pos(0);
  const int *w(begin());
  std::vector< std::pair<int,int> > ret(nbOfSlices);
  for(int i=0;i<nbOfSlices;i++)
    {
      std::pair<int,int> p(pos,-1);
      int locSum(0);
      while(locSum<sumPerSlc && pos<nbOfTuples) { pos++; locSum+=*w++; }
      if(i!=nbOfSlices-1)
        p.second=pos;
      else
        p.second=nbOfTuples;
      ret[i]=p;
    }
  return ret;
}

/*!
 * Returns a new DataArrayInt that is a modulus of two given arrays. There are 3
 * valid cases.
 * 1.  The arrays have same number of tuples and components. Then each value of
 *   the result array (_a_) is a division of the corresponding values of \a a1 and
 *   \a a2, i.e.: _a_ [ i, j ] = _a1_ [ i, j ] % _a2_ [ i, j ].
 * 2.  The arrays have same number of tuples and one array, say _a2_, has one
 *   component. Then
 *   _a_ [ i, j ] = _a1_ [ i, j ] % _a2_ [ i, 0 ].
 * 3.  The arrays have same number of components and one array, say _a2_, has one
 *   tuple. Then
 *   _a_ [ i, j ] = _a1_ [ i, j ] % _a2_ [ 0, j ].
 *
 * Info on components is copied either from the first array (in the first case) or from
 * the array with maximal number of elements (getNbOfElems()).
 *  \warning No check of division by zero is performed!
 *  \param [in] a1 - a dividend array.
 *  \param [in] a2 - a divisor array.
 *  \return DataArrayInt * - the new instance of DataArrayInt.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If either \a a1 or \a a2 is NULL.
 *  \throw If \a a1->getNumberOfTuples() != \a a2->getNumberOfTuples() and
 *         \a a1->getNumberOfComponents() != \a a2->getNumberOfComponents() and
 *         none of them has number of tuples or components equal to 1.
 */
DataArrayInt *DataArrayInt::Modulus(const DataArrayInt *a1, const DataArrayInt *a2)
{
  if(!a1 || !a2)
    throw INTERP_KERNEL::Exception("DataArrayInt::Modulus : input DataArrayInt instance is NULL !");
  int nbOfTuple1=a1->getNumberOfTuples();
  int nbOfTuple2=a2->getNumberOfTuples();
  int nbOfComp1=a1->getNumberOfComponents();
  int nbOfComp2=a2->getNumberOfComponents();
  if(nbOfTuple2==nbOfTuple1)
    {
      if(nbOfComp1==nbOfComp2)
        {
          MCAuto<DataArrayInt> ret=DataArrayInt::New();
          ret->alloc(nbOfTuple2,nbOfComp1);
          std::transform(a1->begin(),a1->end(),a2->begin(),ret->getPointer(),std::modulus<int>());
          ret->copyStringInfoFrom(*a1);
          return ret.retn();
        }
      else if(nbOfComp2==1)
        {
          MCAuto<DataArrayInt> ret=DataArrayInt::New();
          ret->alloc(nbOfTuple1,nbOfComp1);
          const int *a2Ptr=a2->getConstPointer();
          const int *a1Ptr=a1->getConstPointer();
          int *res=ret->getPointer();
          for(int i=0;i<nbOfTuple1;i++)
            res=std::transform(a1Ptr+i*nbOfComp1,a1Ptr+(i+1)*nbOfComp1,res,std::bind2nd(std::modulus<int>(),a2Ptr[i]));
          ret->copyStringInfoFrom(*a1);
          return ret.retn();
        }
      else
        {
          a1->checkNbOfComps(nbOfComp2,"Nb of components mismatch for array Modulus !");
          return 0;
        }
    }
  else if(nbOfTuple2==1)
    {
      a1->checkNbOfComps(nbOfComp2,"Nb of components mismatch for array Modulus !");
      MCAuto<DataArrayInt> ret=DataArrayInt::New();
      ret->alloc(nbOfTuple1,nbOfComp1);
      const int *a1ptr=a1->getConstPointer(),*a2ptr=a2->getConstPointer();
      int *pt=ret->getPointer();
      for(int i=0;i<nbOfTuple1;i++)
        pt=std::transform(a1ptr+i*nbOfComp1,a1ptr+(i+1)*nbOfComp1,a2ptr,pt,std::modulus<int>());
      ret->copyStringInfoFrom(*a1);
      return ret.retn();
    }
  else
    {
      a1->checkNbOfTuples(nbOfTuple2,"Nb of tuples mismatch for array Modulus !");//will always throw an exception
      return 0;
    }
}

/*!
 * Modify \a this array so that each value becomes a modulus of division of this value by
 * a value of another DataArrayInt. There are 3 valid cases.
 * 1.  The arrays have same number of tuples and components. Then each value of
 *    \a this array is divided by the corresponding value of \a other one, i.e.:
 *   _a_ [ i, j ] %= _other_ [ i, j ].
 * 2.  The arrays have same number of tuples and \a other array has one component. Then
 *   _a_ [ i, j ] %= _other_ [ i, 0 ].
 * 3.  The arrays have same number of components and \a other array has one tuple. Then
 *   _a_ [ i, j ] %= _a2_ [ 0, j ].
 *
 *  \warning No check of division by zero is performed!
 *  \param [in] other - a divisor array.
 *  \throw If \a other is NULL.
 *  \throw If \a this->getNumberOfTuples() != \a other->getNumberOfTuples() and
 *         \a this->getNumberOfComponents() != \a other->getNumberOfComponents() and
 *         \a other has number of both tuples and components not equal to 1.
 */
void DataArrayInt::modulusEqual(const DataArrayInt *other)
{
  if(!other)
    throw INTERP_KERNEL::Exception("DataArrayInt::modulusEqual : input DataArrayInt instance is NULL !");
  const char *msg="Nb of tuples mismatch for DataArrayInt::modulusEqual !";
  checkAllocated(); other->checkAllocated();
  int nbOfTuple=getNumberOfTuples();
  int nbOfTuple2=other->getNumberOfTuples();
  int nbOfComp=getNumberOfComponents();
  int nbOfComp2=other->getNumberOfComponents();
  if(nbOfTuple==nbOfTuple2)
    {
      if(nbOfComp==nbOfComp2)
        {
          std::transform(begin(),end(),other->begin(),getPointer(),std::modulus<int>());
        }
      else if(nbOfComp2==1)
        {
          if(nbOfComp2==nbOfComp)
            {
              int *ptr=getPointer();
              const int *ptrc=other->getConstPointer();
              for(int i=0;i<nbOfTuple;i++)
                std::transform(ptr+i*nbOfComp,ptr+(i+1)*nbOfComp,ptr+i*nbOfComp,std::bind2nd(std::modulus<int>(),*ptrc++));
            }
          else
            throw INTERP_KERNEL::Exception(msg);
        }
      else
        throw INTERP_KERNEL::Exception(msg);
    }
  else if(nbOfTuple2==1)
    {
      int *ptr=getPointer();
      const int *ptrc=other->getConstPointer();
      for(int i=0;i<nbOfTuple;i++)
        std::transform(ptr+i*nbOfComp,ptr+(i+1)*nbOfComp,ptrc,ptr+i*nbOfComp,std::modulus<int>());
    }
  else
    throw INTERP_KERNEL::Exception(msg);
  declareAsNew();
}

/*!
 * Returns a new DataArrayInt that is the result of pow of two given arrays. There are 3
 * valid cases.
 *
 *  \param [in] a1 - an array to pow up.
 *  \param [in] a2 - another array to sum up.
 *  \return DataArrayInt * - the new instance of DataArrayInt.
 *          The caller is to delete this result array using decrRef() as it is no more
 *          needed.
 *  \throw If either \a a1 or \a a2 is NULL.
 *  \throw If \a a1->getNumberOfTuples() != \a a2->getNumberOfTuples()
 *  \throw If \a a1->getNumberOfComponents() != 1 or \a a2->getNumberOfComponents() != 1.
 *  \throw If there is a negative value in \a a2.
 */
DataArrayInt *DataArrayInt::Pow(const DataArrayInt *a1, const DataArrayInt *a2)
{
  if(!a1 || !a2)
    throw INTERP_KERNEL::Exception("DataArrayInt::Pow : at least one of input instances is null !");
  int nbOfTuple=a1->getNumberOfTuples();
  int nbOfTuple2=a2->getNumberOfTuples();
  int nbOfComp=a1->getNumberOfComponents();
  int nbOfComp2=a2->getNumberOfComponents();
  if(nbOfTuple!=nbOfTuple2)
    throw INTERP_KERNEL::Exception("DataArrayInt::Pow : number of tuples mismatches !");
  if(nbOfComp!=1 || nbOfComp2!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::Pow : number of components of both arrays must be equal to 1 !");
  MCAuto<DataArrayInt> ret=DataArrayInt::New(); ret->alloc(nbOfTuple,1);
  const int *ptr1(a1->begin()),*ptr2(a2->begin());
  int *ptr=ret->getPointer();
  for(int i=0;i<nbOfTuple;i++,ptr1++,ptr2++,ptr++)
    {
      if(*ptr2>=0)
        {
          int tmp=1;
          for(int j=0;j<*ptr2;j++)
            tmp*=*ptr1;
          *ptr=tmp;
        }
      else
        {
          std::ostringstream oss; oss << "DataArrayInt::Pow : on tuple #" << i << " of a2 value is < 0 (" << *ptr2 << ") !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  return ret.retn();
}

/*!
 * Apply pow on values of another DataArrayInt to values of \a this one.
 *
 *  \param [in] other - an array to pow to \a this one.
 *  \throw If \a other is NULL.
 *  \throw If \a this->getNumberOfTuples() != \a other->getNumberOfTuples()
 *  \throw If \a this->getNumberOfComponents() != 1 or \a other->getNumberOfComponents() != 1
 *  \throw If there is a negative value in \a other.
 */
void DataArrayInt::powEqual(const DataArrayInt *other)
{
  if(!other)
    throw INTERP_KERNEL::Exception("DataArrayInt::powEqual : input instance is null !");
  int nbOfTuple=getNumberOfTuples();
  int nbOfTuple2=other->getNumberOfTuples();
  int nbOfComp=getNumberOfComponents();
  int nbOfComp2=other->getNumberOfComponents();
  if(nbOfTuple!=nbOfTuple2)
    throw INTERP_KERNEL::Exception("DataArrayInt::powEqual : number of tuples mismatches !");
  if(nbOfComp!=1 || nbOfComp2!=1)
    throw INTERP_KERNEL::Exception("DataArrayInt::powEqual : number of components of both arrays must be equal to 1 !");
  int *ptr=getPointer();
  const int *ptrc=other->begin();
  for(int i=0;i<nbOfTuple;i++,ptrc++,ptr++)
    {
      if(*ptrc>=0)
        {
          int tmp=1;
          for(int j=0;j<*ptrc;j++)
            tmp*=*ptr;
          *ptr=tmp;
        }
      else
        {
          std::ostringstream oss; oss << "DataArrayInt::powEqual : on tuple #" << i << " of other value is < 0 (" << *ptrc << ") !";
          throw INTERP_KERNEL::Exception(oss.str().c_str());
        }
    }
  declareAsNew();
}

/*!
 * Returns a C array which is a renumbering map in "Old to New" mode for the input array.
 * This map, if applied to \a start array, would make it sorted. For example, if
 * \a start array contents are [9,10,0,6,4,11,3,7] then the contents of the result array is
 * [5,6,0,3,2,7,1,4].
 *  \param [in] start - pointer to the first element of the array for which the
 *         permutation map is computed.
 *  \param [in] end - pointer specifying the end of the array \a start, so that
 *         the last value of \a start is \a end[ -1 ].
 *  \return int * - the result permutation array that the caller is to delete as it is no
 *         more needed.
 *  \throw If there are equal values in the input array.
 */
int *DataArrayInt::CheckAndPreparePermutation(const int *start, const int *end)
{
  std::size_t sz=std::distance(start,end);
  int *ret=(int *)malloc(sz*sizeof(int));
  int *work=new int[sz];
  std::copy(start,end,work);
  std::sort(work,work+sz);
  if(std::unique(work,work+sz)!=work+sz)
    {
      delete [] work;
      free(ret);
      throw INTERP_KERNEL::Exception("Some elements are equals in the specified array !");
    }
  std::map<int,int> m;
  for(int *workPt=work;workPt!=work+sz;workPt++)
    m[*workPt]=(int)std::distance(work,workPt);
  int *iter2=ret;
  for(const int *iter=start;iter!=end;iter++,iter2++)
    *iter2=m[*iter];
  delete [] work;
  return ret;
}

/*!
 * Returns a new DataArrayInt containing an arithmetic progression
 * that is equal to the sequence returned by Python \c range(\a begin,\a  end,\a  step )
 * function.
 *  \param [in] begin - the start value of the result sequence.
 *  \param [in] end - limiting value, so that every value of the result array is less than
 *              \a end.
 *  \param [in] step - specifies the increment or decrement.
 *  \return DataArrayInt * - a new instance of DataArrayInt. The caller is to delete this
 *          array using decrRef() as it is no more needed.
 *  \throw If \a step == 0.
 *  \throw If \a end < \a begin && \a step > 0.
 *  \throw If \a end > \a begin && \a step < 0.
 */
DataArrayInt *DataArrayInt::Range(int begin, int end, int step)
{
  int nbOfTuples=GetNumberOfItemGivenBESRelative(begin,end,step,"DataArrayInt::Range");
  MCAuto<DataArrayInt> ret=DataArrayInt::New();
  ret->alloc(nbOfTuples,1);
  int *ptr=ret->getPointer();
  if(step>0)
    {
      for(int i=begin;i<end;i+=step,ptr++)
        *ptr=i;
    }
  else
    {
      for(int i=begin;i>end;i+=step,ptr++)
        *ptr=i;
    }
  return ret.retn();
}

/*!
 * Useless method for end user. Only for MPI/Corba/File serialsation for multi arrays class.
 * Server side.
 */
void DataArrayInt::getTinySerializationIntInformation(std::vector<int>& tinyInfo) const
{
  tinyInfo.resize(2);
  if(isAllocated())
    {
      tinyInfo[0]=getNumberOfTuples();
      tinyInfo[1]=getNumberOfComponents();
    }
  else
    {
      tinyInfo[0]=-1;
      tinyInfo[1]=-1;
    }
}

/*!
 * Useless method for end user. Only for MPI/Corba/File serialsation for multi arrays class.
 * Server side.
 */
void DataArrayInt::getTinySerializationStrInformation(std::vector<std::string>& tinyInfo) const
{
  if(isAllocated())
    {
      int nbOfCompo=getNumberOfComponents();
      tinyInfo.resize(nbOfCompo+1);
      tinyInfo[0]=getName();
      for(int i=0;i<nbOfCompo;i++)
        tinyInfo[i+1]=getInfoOnComponent(i);
    }
  else
    {
      tinyInfo.resize(1);
      tinyInfo[0]=getName();
    }
}

/*!
 * Useless method for end user. Only for MPI/Corba/File serialsation for multi arrays class.
 * This method returns if a feeding is needed.
 */
bool DataArrayInt::resizeForUnserialization(const std::vector<int>& tinyInfoI)
{
  int nbOfTuple=tinyInfoI[0];
  int nbOfComp=tinyInfoI[1];
  if(nbOfTuple!=-1 || nbOfComp!=-1)
    {
      alloc(nbOfTuple,nbOfComp);
      return true;
    }
  return false;
}

/*!
 * Useless method for end user. Only for MPI/Corba/File serialsation for multi arrays class.
 * This method returns if a feeding is needed.
 */
void DataArrayInt::finishUnserialization(const std::vector<int>& tinyInfoI, const std::vector<std::string>& tinyInfoS)
{
  setName(tinyInfoS[0]);
  if(isAllocated())
    {
      int nbOfCompo=tinyInfoI[1];
      for(int i=0;i<nbOfCompo;i++)
        setInfoOnComponent(i,tinyInfoS[i+1]);
    }
}

DataArrayIntIterator::DataArrayIntIterator(DataArrayInt *da):DataArrayIterator<int>(da)
{
}

DataArrayInt32Tuple::DataArrayInt32Tuple(int *pt, int nbOfComp):DataArrayTuple<int>(pt,nbOfComp)
{
}

std::string DataArrayIntTuple::repr() const
{
  std::ostringstream oss; oss << "(";
  for(int i=0;i<_nb_of_compo-1;i++)
    oss << _pt[i] << ", ";
  oss << _pt[_nb_of_compo-1] << ")";
  return oss.str();
}

int DataArrayIntTuple::intValue() const
{
  return this->zeValue();
}

/*!
 * This method returns a newly allocated instance the caller should dealed with by a MEDCoupling::DataArrayInt::decrRef.
 * This method performs \b no copy of data. The content is only referenced using MEDCoupling::DataArrayInt::useArray with ownership set to \b false.
 * This method throws an INTERP_KERNEL::Exception is it is impossible to match sizes of \b this that is too say \b nbOfCompo=this->_nb_of_elem and \bnbOfTuples==1 or
 * \b nbOfCompo=1 and \bnbOfTuples==this->_nb_of_elem.
 */
DataArrayInt *DataArrayIntTuple::buildDAInt(int nbOfTuples, int nbOfCompo) const
{
  return this->buildDA(nbOfTuples,nbOfCompo);
}
