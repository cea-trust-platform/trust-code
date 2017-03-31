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
// Author : Anthony Geay (CEA/DEN)

#ifndef __MEDCOUPLING_MEDCOUPLINGMEMARRAY_HXX__
#define __MEDCOUPLING_MEDCOUPLINGMEMARRAY_HXX__

#include "MEDCoupling.hxx"
#include "MCAuto.hxx"
#include "MEDCouplingTimeLabel.hxx"
#include "MEDCouplingRefCountObject.hxx"
#include "InterpKernelException.hxx"
#include "MEDCouplingTraits.hxx"
#include "BBTreePts.txx"

#include <string>
#include <vector>
#include <iterator>

namespace MEDCoupling
{
  typedef enum
    {
      AX_CART = 3,
      AX_CYL = 4,
      AX_SPHER = 5
    } MEDCouplingAxisType;
  // -- WARNING this enum must be synchronized with MEDCouplingCommon.i file ! --

  class PartDefinition;
  
  template<class T>
  class MEDCouplingPointer
  {
  public:
    MEDCouplingPointer():_internal(0),_external(0) { }
    void null() { _internal=0; _external=0; }
    bool isNull() const { return _internal==0 && _external==0; }
    void setInternal(T *pointer);
    void setExternal(const T *pointer);
    const T *getConstPointer() const { if(_internal) return _internal; else return _external; }
    const T *getConstPointerLoc(std::size_t offset) const { if(_internal) return _internal+offset; else return _external+offset; }
    T *getPointer() { if(_internal) return _internal; if(_external) throw INTERP_KERNEL::Exception("Trying to write on an external pointer."); else return 0; }
  private:
    T *_internal;
    const T *_external;
  };

  template<class T>
  class MemArray
  {
  public:
    typedef void (*Deallocator)(void *,void *);
  public:
    MemArray():_nb_of_elem(0),_nb_of_elem_alloc(0),_ownership(false),_dealloc(0),_param_for_deallocator(0) { }
    MemArray(const MemArray<T>& other);
    bool isNull() const { return _pointer.isNull(); }
    const T *getConstPointerLoc(std::size_t offset) const { return _pointer.getConstPointerLoc(offset); }
    const T *getConstPointer() const { return _pointer.getConstPointer(); }
    std::size_t getNbOfElem() const { return _nb_of_elem; }
    std::size_t getNbOfElemAllocated() const { return _nb_of_elem_alloc; }
    T *getPointer() { return _pointer.getPointer(); }
    MemArray<T> &operator=(const MemArray<T>& other);
    T operator[](std::size_t id) const { return _pointer.getConstPointer()[id]; }
    T& operator[](std::size_t id) { return _pointer.getPointer()[id]; }
    bool isEqual(const MemArray<T>& other, T prec, std::string& reason) const;
    void repr(int sl, std::ostream& stream) const;
    bool reprHeader(int sl, std::ostream& stream) const;
    void reprZip(int sl, std::ostream& stream) const;
    void reprNotTooLong(int sl, std::ostream& stream) const;
    void fillWithValue(const T& val);
    T *fromNoInterlace(int nbOfComp) const;
    T *toNoInterlace(int nbOfComp) const;
    void sort(bool asc);
    void reverse(int nbOfComp);
    void alloc(std::size_t nbOfElements);
    void reserve(std::size_t newNbOfElements);
    void reAlloc(std::size_t newNbOfElements);
    void useArray(const T *array, bool ownership, DeallocType type, std::size_t nbOfElem);
    void useExternalArrayWithRWAccess(const T *array, std::size_t nbOfElem);
    void writeOnPlace(std::size_t id, T element0, const T *others, std::size_t sizeOfOthers);
    template<class InputIterator>
    void insertAtTheEnd(InputIterator first, InputIterator last);
    void pushBack(T elem);
    T popBack();
    void pack() const;
    bool isDeallocatorCalled() const { return _ownership; }
    Deallocator getDeallocator() const { return _dealloc; }
    void setSpecificDeallocator(Deallocator dealloc) { _dealloc=dealloc; }
    void setParameterForDeallocator(void *param) { _param_for_deallocator=param; }
    void *getParameterForDeallocator() const { return _param_for_deallocator; }
    void destroy();
    ~MemArray() { destroy(); }
  public:
    static void CPPDeallocator(void *pt, void *param);
    static void CDeallocator(void *pt, void *param);
  private:
    static void DestroyPointer(T *pt, Deallocator dealloc, void *param);
    static Deallocator BuildFromType(DeallocType type);
  private:
    std::size_t _nb_of_elem;
    std::size_t _nb_of_elem_alloc;
    bool _ownership;
    MEDCouplingPointer<T> _pointer;
    Deallocator _dealloc;
    void *_param_for_deallocator;
  };

  class DataArrayInt;
  class DataArrayByte;

  class DataArray : public RefCountObject, public TimeLabel
  {
  public:
    MEDCOUPLING_EXPORT std::size_t getHeapMemorySizeWithoutChildren() const;
    MEDCOUPLING_EXPORT std::vector<const BigMemoryObject *> getDirectChildrenWithNull() const;
    MEDCOUPLING_EXPORT void setName(const std::string& name);
    MEDCOUPLING_EXPORT void copyStringInfoFrom(const DataArray& other);
    MEDCOUPLING_EXPORT void copyPartOfStringInfoFrom(const DataArray& other, const std::vector<int>& compoIds);
    MEDCOUPLING_EXPORT void copyPartOfStringInfoFrom2(const std::vector<int>& compoIds, const DataArray& other);
    MEDCOUPLING_EXPORT bool areInfoEqualsIfNotWhy(const DataArray& other, std::string& reason) const;
    MEDCOUPLING_EXPORT bool areInfoEquals(const DataArray& other) const;
    MEDCOUPLING_EXPORT std::string cppRepr(const std::string& varName) const;
    MEDCOUPLING_EXPORT std::string getName() const { return _name; }
    MEDCOUPLING_EXPORT const std::vector<std::string> &getInfoOnComponents() const { return _info_on_compo; }
    MEDCOUPLING_EXPORT std::vector<std::string> &getInfoOnComponents() { return _info_on_compo; }
    MEDCOUPLING_EXPORT void setInfoOnComponents(const std::vector<std::string>& info);
    MEDCOUPLING_EXPORT void setInfoAndChangeNbOfCompo(const std::vector<std::string>& info);
    MEDCOUPLING_EXPORT std::vector<std::string> getVarsOnComponent() const;
    MEDCOUPLING_EXPORT std::vector<std::string> getUnitsOnComponent() const;
    MEDCOUPLING_EXPORT std::string getInfoOnComponent(int i) const;
    MEDCOUPLING_EXPORT std::string getVarOnComponent(int i) const;
    MEDCOUPLING_EXPORT std::string getUnitOnComponent(int i) const;
    MEDCOUPLING_EXPORT void setInfoOnComponent(int i, const std::string& info);
    MEDCOUPLING_EXPORT std::size_t getNumberOfComponents() const { return _info_on_compo.size(); }
    MEDCOUPLING_EXPORT void setPartOfValuesBase3(const DataArray *aBase, const int *bgTuples, const int *endTuples, int bgComp, int endComp, int stepComp, bool strictCompoCompare=true);
    MEDCOUPLING_EXPORT virtual void *getVoidStarPointer() = 0;
    MEDCOUPLING_EXPORT virtual DataArray *deepCopy() const = 0;
    MEDCOUPLING_EXPORT virtual DataArray *buildNewEmptyInstance() const = 0;
    MEDCOUPLING_EXPORT virtual bool isAllocated() const = 0;
    MEDCOUPLING_EXPORT virtual void checkAllocated() const = 0;
    MEDCOUPLING_EXPORT virtual void desallocate() = 0;
    MEDCOUPLING_EXPORT virtual int getNumberOfTuples() const = 0;
    MEDCOUPLING_EXPORT virtual std::size_t getNbOfElems() const = 0;
    MEDCOUPLING_EXPORT virtual std::size_t getNbOfElemAllocated() const = 0;
    MEDCOUPLING_EXPORT virtual void alloc(std::size_t nbOfTuple, std::size_t nbOfCompo=1) = 0;
    MEDCOUPLING_EXPORT virtual void reAlloc(std::size_t newNbOfTuple) = 0;
    MEDCOUPLING_EXPORT virtual void renumberInPlace(const int *old2New) = 0;
    MEDCOUPLING_EXPORT virtual void renumberInPlaceR(const int *new2Old) = 0;
    MEDCOUPLING_EXPORT virtual void setContigPartOfSelectedValues(int tupleIdStart, const DataArray *aBase, const DataArrayInt *tuplesSelec) = 0;
    MEDCOUPLING_EXPORT virtual void setContigPartOfSelectedValuesSlice(int tupleIdStart, const DataArray *aBase, int bg, int end2, int step) = 0;
    MEDCOUPLING_EXPORT virtual DataArray *selectByTupleRanges(const std::vector<std::pair<int,int> >& ranges) const = 0;
    MEDCOUPLING_EXPORT virtual DataArray *keepSelectedComponents(const std::vector<int>& compoIds) const = 0;
    MEDCOUPLING_EXPORT virtual DataArray *selectByTupleId(const int *new2OldBg, const int *new2OldEnd) const = 0;
    MEDCOUPLING_EXPORT virtual DataArray *selectByTupleIdSafe(const int *new2OldBg, const int *new2OldEnd) const = 0;
    MEDCOUPLING_EXPORT virtual DataArray *selectByTupleIdSafeSlice(int bg, int end2, int step) const = 0;
    MEDCOUPLING_EXPORT virtual void rearrange(int newNbOfCompo) = 0;
    MEDCOUPLING_EXPORT virtual void circularPermutation(int nbOfShift=1) = 0;
    MEDCOUPLING_EXPORT virtual void circularPermutationPerTuple(int nbOfShift=1) = 0;
    MEDCOUPLING_EXPORT virtual void reversePerTuple() = 0;
    MEDCOUPLING_EXPORT void checkNbOfTuples(int nbOfTuples, const std::string& msg) const;
    MEDCOUPLING_EXPORT void checkNbOfComps(int nbOfCompo, const std::string& msg) const;
    MEDCOUPLING_EXPORT void checkNbOfTuplesAndComp(const DataArray& other, const std::string& msg) const;
    MEDCOUPLING_EXPORT void checkNbOfTuplesAndComp(int nbOfTuples, int nbOfCompo, const std::string& msg) const;
    MEDCOUPLING_EXPORT void checkNbOfElems(std::size_t nbOfElems, const std::string& msg) const;
    MEDCOUPLING_EXPORT static void GetSlice(int start, int stop, int step, int sliceId, int nbOfSlices, int& startSlice, int& stopSlice);
    MEDCOUPLING_EXPORT static int GetNumberOfItemGivenBES(int begin, int end, int step, const std::string& msg);
    MEDCOUPLING_EXPORT static int GetNumberOfItemGivenBESRelative(int begin, int end, int step, const std::string& msg);
    MEDCOUPLING_EXPORT static int GetPosOfItemGivenBESRelativeNoThrow(int value, int begin, int end, int step);
    MEDCOUPLING_EXPORT static std::string GetVarNameFromInfo(const std::string& info);
    MEDCOUPLING_EXPORT static std::string GetUnitFromInfo(const std::string& info);
    MEDCOUPLING_EXPORT static std::string BuildInfoFromVarAndUnit(const std::string& var, const std::string& unit);
    MEDCOUPLING_EXPORT static std::string GetAxisTypeRepr(MEDCouplingAxisType at);
    MEDCOUPLING_EXPORT static DataArray *Aggregate(const std::vector<const DataArray *>& arrs);
    MEDCOUPLING_EXPORT virtual void reprStream(std::ostream& stream) const = 0;
    MEDCOUPLING_EXPORT virtual void reprZipStream(std::ostream& stream) const = 0;
    MEDCOUPLING_EXPORT virtual void reprWithoutNameStream(std::ostream& stream) const;
    MEDCOUPLING_EXPORT virtual void reprZipWithoutNameStream(std::ostream& stream) const = 0;
    MEDCOUPLING_EXPORT virtual void reprCppStream(const std::string& varName, std::ostream& stream) const = 0;
    MEDCOUPLING_EXPORT virtual void reprQuickOverview(std::ostream& stream) const = 0;
    MEDCOUPLING_EXPORT virtual void reprQuickOverviewData(std::ostream& stream, std::size_t maxNbOfByteInRepr) const = 0;
  protected:
    DataArray() { }
    ~DataArray() { }
  protected:
    MEDCOUPLING_EXPORT static void CheckValueInRange(int ref, int value, const std::string& msg);
    MEDCOUPLING_EXPORT static void CheckValueInRangeEx(int value, int start, int end, const std::string& msg);
    MEDCOUPLING_EXPORT static void CheckClosingParInRange(int ref, int value, const std::string& msg);
    MEDCOUPLING_EXPORT static int EffectiveCircPerm(int nbOfShift, int nbOfTuples);
  protected:
    std::string _name;
    std::vector<std::string> _info_on_compo;
  };
}

namespace MEDCoupling
{
  class DataArrayInt;

  template<class T>
  class DataArrayTemplate : public DataArray
  {
  public:
    MEDCOUPLING_EXPORT static MCAuto< typename Traits<T>::ArrayTypeCh > NewFromStdVector(const typename std::vector<T>& v);
    MEDCOUPLING_EXPORT std::vector< MCAuto< typename Traits<T>::ArrayTypeCh > > explodeComponents() const;
    //
    std::size_t getHeapMemorySizeWithoutChildren() const;
    //
    MEDCOUPLING_EXPORT int getNumberOfTuples() const { return _info_on_compo.empty()?0:_mem.getNbOfElem()/getNumberOfComponents(); }
    MEDCOUPLING_EXPORT std::size_t getNbOfElems() const { return _mem.getNbOfElem(); }
    bool empty() const;
    MEDCOUPLING_EXPORT void *getVoidStarPointer() { return getPointer(); }
    MEDCOUPLING_EXPORT const T *getConstPointer() const { return _mem.getConstPointer(); }
    MEDCOUPLING_EXPORT const T *begin() const { return getConstPointer(); }
    MEDCOUPLING_EXPORT const T *end() const { return getConstPointer()+getNbOfElems(); }
    void alloc(std::size_t nbOfTuple, std::size_t nbOfCompo=1);
    void useArray(const T *array, bool ownership, DeallocType type, int nbOfTuple, int nbOfCompo);
    void useExternalArrayWithRWAccess(const T *array, int nbOfTuple, int nbOfCompo);
    T getIJSafe(int tupleId, int compoId) const;
    MEDCOUPLING_EXPORT T getIJ(int tupleId, int compoId) const { return _mem[tupleId*_info_on_compo.size()+compoId]; }
    MEDCOUPLING_EXPORT void setIJ(int tupleId, int compoId, T newVal) { _mem[tupleId*_info_on_compo.size()+compoId]=newVal; declareAsNew(); }
    MEDCOUPLING_EXPORT void setIJSilent(int tupleId, int compoId, T newVal) { _mem[tupleId*_info_on_compo.size()+compoId]=newVal; }
    MEDCOUPLING_EXPORT T *getPointer() { return _mem.getPointer(); declareAsNew(); }
    void pack() const;
    bool isAllocated() const;
    void checkAllocated() const;
    void desallocate();
    void reserve(std::size_t nbOfElems);
    void rearrange(int newNbOfCompo);
    void transpose();
    void pushBackSilent(T val);
    void pushBackValsSilent(const T *valsBg, const T *valsEnd);
    T popBackSilent();
    T front() const;
    T back() const;
    std::size_t getNbOfElemAllocated() const { return _mem.getNbOfElemAllocated(); }
    void allocIfNecessary(int nbOfTuple, int nbOfCompo);
    void deepCopyFrom(const DataArrayTemplate<T>& other);
    void reverse();
    void fillWithValue(T val);
    void reAlloc(std::size_t newNbOfTuple);
    void renumberInPlace(const int *old2New);
    void renumberInPlaceR(const int *new2Old);
    void sort(bool asc=true);
    typename Traits<T>::ArrayType *renumber(const int *old2New) const;
    typename Traits<T>::ArrayType *renumberR(const int *new2Old) const;
    typename Traits<T>::ArrayType *renumberAndReduce(const int *old2New, int newNbOfTuple) const;
    typename Traits<T>::ArrayType *changeNbOfComponents(int newNbOfComp, T dftValue) const;
    typename Traits<T>::ArrayType *subArray(int tupleIdBg, int tupleIdEnd=-1) const;
    MCAuto<typename Traits<T>::ArrayTypeCh> selectPartDef(const PartDefinition* pd) const;
    void circularPermutation(int nbOfShift=1);
    void circularPermutationPerTuple(int nbOfShift=1);
    void reversePerTuple();
    void setPartOfValues1(const typename Traits<T>::ArrayType *a, int bgTuples, int endTuples, int stepTuples, int bgComp, int endComp, int stepComp, bool strictCompoCompare=true);
    void setPartOfValuesSimple1(T a, int bgTuples, int endTuples, int stepTuples, int bgComp, int endComp, int stepComp);
    void setPartOfValues2(const typename Traits<T>::ArrayType *a, const int *bgTuples, const int *endTuples, const int *bgComp, const int *endComp, bool strictCompoCompare=true);
    void setPartOfValuesSimple2(T a, const int *bgTuples, const int *endTuples, const int *bgComp, const int *endComp);
    void setPartOfValues3(const typename Traits<T>::ArrayType *a, const int *bgTuples, const int *endTuples, int bgComp, int endComp, int stepComp, bool strictCompoCompare=true);
    void setPartOfValuesSimple3(T a, const int *bgTuples, const int *endTuples, int bgComp, int endComp, int stepComp);
    void setPartOfValues4(const typename Traits<T>::ArrayType *a, int bgTuples, int endTuples, int stepTuples, const int *bgComp, const int *endComp, bool strictCompoCompare=true);
    void setPartOfValuesSimple4(T a, int bgTuples, int endTuples, int stepTuples, const int *bgComp, const int *endComp);
    void setPartOfValuesAdv(const typename Traits<T>::ArrayType *a, const DataArrayInt *tuplesSelec);
    void setContigPartOfSelectedValues(int tupleIdStart, const DataArray *aBase, const DataArrayInt *tuplesSelec);
    void setContigPartOfSelectedValuesSlice(int tupleIdStart, const DataArray *aBase, int bg, int end2, int step);
    T getMaxValue(int& tupleId) const;
    T getMaxValueInArray() const;
    T getMinValue(int& tupleId) const;
    T getMinValueInArray() const;
  protected:
    typename Traits<T>::ArrayType *mySelectByTupleId(const int *new2OldBg, const int *new2OldEnd) const;
    typename Traits<T>::ArrayType *mySelectByTupleId(const DataArrayInt& di) const;
    typename Traits<T>::ArrayType *mySelectByTupleIdSafe(const int *new2OldBg, const int *new2OldEnd) const;
    typename Traits<T>::ArrayType *myKeepSelectedComponents(const std::vector<int>& compoIds) const;
    typename Traits<T>::ArrayType *mySelectByTupleIdSafeSlice(int bg, int end2, int step) const;
    typename Traits<T>::ArrayType *mySelectByTupleRanges(const std::vector<std::pair<int,int> >& ranges) const;
  protected:
    MemArray<T> _mem;
  };
}

namespace MEDCoupling
{
  class DataArrayDoubleIterator;
  class DataArrayDouble : public DataArrayTemplate<double>
  {
  public:
    MEDCOUPLING_EXPORT static DataArrayDouble *New();
    MEDCOUPLING_EXPORT double doubleValue() const;
    MEDCOUPLING_EXPORT DataArrayDouble *deepCopy() const;
    MEDCOUPLING_EXPORT DataArrayDouble *buildNewEmptyInstance() const { return DataArrayDouble::New(); }
    MEDCOUPLING_EXPORT DataArrayDouble *performCopyOrIncrRef(bool deepCopy) const;
    MEDCOUPLING_EXPORT void fillWithZero();
    MEDCOUPLING_EXPORT void iota(double init=0.);
    MEDCOUPLING_EXPORT bool isUniform(double val, double eps) const;
    MEDCOUPLING_EXPORT void checkMonotonic(bool increasing, double eps) const;
    MEDCOUPLING_EXPORT bool isMonotonic(bool increasing, double eps) const;
    MEDCOUPLING_EXPORT std::string repr() const;
    MEDCOUPLING_EXPORT std::string reprZip() const;
    MEDCOUPLING_EXPORT std::string reprNotTooLong() const;
    MEDCOUPLING_EXPORT void writeVTK(std::ostream& ofs, int indent, const std::string& nameInFile, DataArrayByte *byteArr) const;
    MEDCOUPLING_EXPORT void reprStream(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprZipStream(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprNotTooLongStream(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprWithoutNameStream(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprZipWithoutNameStream(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprNotTooLongWithoutNameStream(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprCppStream(const std::string& varName, std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprQuickOverview(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprQuickOverviewData(std::ostream& stream, std::size_t maxNbOfByteInRepr) const;
    MEDCOUPLING_EXPORT bool isEqual(const DataArrayDouble& other, double prec) const;
    MEDCOUPLING_EXPORT bool isEqualIfNotWhy(const DataArrayDouble& other, double prec, std::string& reason) const;
    MEDCOUPLING_EXPORT bool isEqualWithoutConsideringStr(const DataArrayDouble& other, double prec) const;
    MEDCOUPLING_EXPORT DataArrayInt *convertToIntArr() const;
    MEDCOUPLING_EXPORT DataArrayDouble *fromNoInterlace() const;
    MEDCOUPLING_EXPORT DataArrayDouble *toNoInterlace() const;
    MEDCOUPLING_EXPORT DataArrayDouble *selectByTupleId(const int *new2OldBg, const int *new2OldEnd) const { return DataArrayTemplate<double>::mySelectByTupleId(new2OldBg,new2OldEnd); }
    MEDCOUPLING_EXPORT DataArrayDouble *selectByTupleId(const DataArrayInt& di) const { return DataArrayTemplate<double>::mySelectByTupleId(di); }
    MEDCOUPLING_EXPORT DataArrayDouble *selectByTupleIdSafe(const int *new2OldBg, const int *new2OldEnd) const { return DataArrayTemplate<double>::mySelectByTupleIdSafe(new2OldBg,new2OldEnd); }
    MEDCOUPLING_EXPORT DataArrayDouble *keepSelectedComponents(const std::vector<int>& compoIds) const { return DataArrayTemplate<double>::myKeepSelectedComponents(compoIds); }
    MEDCOUPLING_EXPORT DataArrayDouble *selectByTupleIdSafeSlice(int bg, int end2, int step) const { return DataArrayTemplate<double>::mySelectByTupleIdSafeSlice(bg,end2,step); }
    MEDCOUPLING_EXPORT DataArrayDouble *selectByTupleRanges(const std::vector<std::pair<int,int> >& ranges) const { return DataArrayTemplate<double>::mySelectByTupleRanges(ranges); }
    MEDCOUPLING_EXPORT void meldWith(const DataArrayDouble *other);
    MEDCOUPLING_EXPORT bool areIncludedInMe(const DataArrayDouble *other, double prec, DataArrayInt *&tupleIds) const;
    MEDCOUPLING_EXPORT void findCommonTuples(double prec, int limitTupleId, DataArrayInt *&comm, DataArrayInt *&commIndex) const;
    MEDCOUPLING_EXPORT double minimalDistanceTo(const DataArrayDouble *other, int& thisTupleId, int& otherTupleId) const;
    MEDCOUPLING_EXPORT DataArrayDouble *duplicateEachTupleNTimes(int nbTimes) const;
    MEDCOUPLING_EXPORT DataArrayDouble *getDifferentValues(double prec, int limitTupleId=-1) const;
    MEDCOUPLING_EXPORT DataArrayInt *findClosestTupleId(const DataArrayDouble *other) const;
    MEDCOUPLING_EXPORT DataArrayInt *computeNbOfInteractionsWith(const DataArrayDouble *otherBBoxFrmt, double eps) const;
    MEDCOUPLING_EXPORT void setSelectedComponents(const DataArrayDouble *a, const std::vector<int>& compoIds);
    MEDCOUPLING_EXPORT void getTuple(int tupleId, double *res) const { std::copy(_mem.getConstPointerLoc(tupleId*_info_on_compo.size()),_mem.getConstPointerLoc((tupleId+1)*_info_on_compo.size()),res); }
    MEDCOUPLING_EXPORT static void SetArrayIn(DataArrayDouble *newArray, DataArrayDouble* &arrayToSet);
    MEDCOUPLING_EXPORT DataArrayDoubleIterator *iterator();
    template<class InputIterator>
    void insertAtTheEnd(InputIterator first, InputIterator last);
    MEDCOUPLING_EXPORT void aggregate(const DataArrayDouble *other);
    MEDCOUPLING_EXPORT void writeOnPlace(std::size_t id, double element0, const double *others, int sizeOfOthers) { _mem.writeOnPlace(id,element0,others,sizeOfOthers); }
    MEDCOUPLING_EXPORT void checkNoNullValues() const;
    MEDCOUPLING_EXPORT void getMinMaxPerComponent(double *bounds) const;
    MEDCOUPLING_EXPORT DataArrayDouble *computeBBoxPerTuple(double epsilon=0.0) const;
    MEDCOUPLING_EXPORT void computeTupleIdsNearTuples(const DataArrayDouble *other, double eps, DataArrayInt *& c, DataArrayInt *& cI) const;
    MEDCOUPLING_EXPORT void recenterForMaxPrecision(double eps);
    MEDCOUPLING_EXPORT double getMaxValue2(DataArrayInt*& tupleIds) const;
    MEDCOUPLING_EXPORT double getMinValue2(DataArrayInt*& tupleIds) const;
    MEDCOUPLING_EXPORT int count(double value, double eps) const;
    MEDCOUPLING_EXPORT double getAverageValue() const;
    MEDCOUPLING_EXPORT double norm2() const;
    MEDCOUPLING_EXPORT double normMax() const;
    MEDCOUPLING_EXPORT double normMin() const;
    MEDCOUPLING_EXPORT void accumulate(double *res) const;
    MEDCOUPLING_EXPORT double accumulate(int compId) const;
    MEDCOUPLING_EXPORT DataArrayDouble *accumulatePerChunck(const int *bgOfIndex, const int *endOfIndex) const;
    MEDCOUPLING_EXPORT double distanceToTuple(const double *tupleBg, const double *tupleEnd, int& tupleId) const;
    MEDCOUPLING_EXPORT DataArrayDouble *fromPolarToCart() const;
    MEDCOUPLING_EXPORT DataArrayDouble *fromCylToCart() const;
    MEDCOUPLING_EXPORT DataArrayDouble *fromSpherToCart() const;
    MEDCOUPLING_EXPORT DataArrayDouble *cartesianize(MEDCouplingAxisType atOfThis) const;
    MEDCOUPLING_EXPORT DataArrayDouble *fromCartToPolar() const;
    MEDCOUPLING_EXPORT DataArrayDouble *fromCartToCyl() const;
    MEDCOUPLING_EXPORT DataArrayDouble *fromCartToSpher() const;
    MEDCOUPLING_EXPORT DataArrayDouble *fromCartToCylGiven(const DataArrayDouble *coords, const double center[3], const double vect[3]) const;
    MEDCOUPLING_EXPORT DataArrayDouble *doublyContractedProduct() const;
    MEDCOUPLING_EXPORT DataArrayDouble *determinant() const;
    MEDCOUPLING_EXPORT DataArrayDouble *eigenValues() const;
    MEDCOUPLING_EXPORT DataArrayDouble *eigenVectors() const;
    MEDCOUPLING_EXPORT DataArrayDouble *inverse() const;
    MEDCOUPLING_EXPORT DataArrayDouble *trace() const;
    MEDCOUPLING_EXPORT DataArrayDouble *deviator() const;
    MEDCOUPLING_EXPORT DataArrayDouble *magnitude() const;
    MEDCOUPLING_EXPORT DataArrayDouble *sumPerTuple() const;
    MEDCOUPLING_EXPORT DataArrayDouble *maxPerTuple() const;
    MEDCOUPLING_EXPORT DataArrayDouble *maxPerTupleWithCompoId(DataArrayInt* &compoIdOfMaxPerTuple) const;
    MEDCOUPLING_EXPORT DataArrayDouble *buildEuclidianDistanceDenseMatrix() const;
    MEDCOUPLING_EXPORT DataArrayDouble *buildEuclidianDistanceDenseMatrixWith(const DataArrayDouble *other) const;
    MEDCOUPLING_EXPORT void sortPerTuple(bool asc);
    MEDCOUPLING_EXPORT void abs();
    MEDCOUPLING_EXPORT DataArrayDouble *computeAbs() const;
    MEDCOUPLING_EXPORT void applyLin(double a, double b, int compoId);
    MEDCOUPLING_EXPORT void applyLin(double a, double b);
    MEDCOUPLING_EXPORT void applyInv(double numerator);
    MEDCOUPLING_EXPORT void applyPow(double val);
    MEDCOUPLING_EXPORT void applyRPow(double val);
    MEDCOUPLING_EXPORT DataArrayDouble *negate() const;
    MEDCOUPLING_EXPORT DataArrayDouble *applyFunc(int nbOfComp, FunctionToEvaluate func) const;
    MEDCOUPLING_EXPORT DataArrayDouble *applyFunc(int nbOfComp, const std::string& func, bool isSafe=true) const;
    MEDCOUPLING_EXPORT DataArrayDouble *applyFunc(const std::string& func, bool isSafe=true) const;
    MEDCOUPLING_EXPORT void applyFuncOnThis(const std::string& func, bool isSafe=true);
    MEDCOUPLING_EXPORT DataArrayDouble *applyFuncCompo(int nbOfComp, const std::string& func, bool isSafe=true) const;
    MEDCOUPLING_EXPORT DataArrayDouble *applyFuncNamedCompo(int nbOfComp, const std::vector<std::string>& varsOrder, const std::string& func, bool isSafe=true) const;
    MEDCOUPLING_EXPORT void applyFuncFast32(const std::string& func);
    MEDCOUPLING_EXPORT void applyFuncFast64(const std::string& func);
    MEDCOUPLING_EXPORT MCAuto<DataArrayDouble> symmetry3DPlane(const double point[3], const double normalVector[3]) const;
    MEDCOUPLING_EXPORT DataArrayInt *findIdsInRange(double vmin, double vmax) const;
    MEDCOUPLING_EXPORT DataArrayInt *findIdsNotInRange(double vmin, double vmax) const;
    MEDCOUPLING_EXPORT static DataArrayDouble *Aggregate(const DataArrayDouble *a1, const DataArrayDouble *a2);
    MEDCOUPLING_EXPORT static DataArrayDouble *Aggregate(const std::vector<const DataArrayDouble *>& arr);
    MEDCOUPLING_EXPORT static DataArrayDouble *Meld(const DataArrayDouble *a1, const DataArrayDouble *a2);
    MEDCOUPLING_EXPORT static DataArrayDouble *Meld(const std::vector<const DataArrayDouble *>& arr);
    MEDCOUPLING_EXPORT static DataArrayDouble *Dot(const DataArrayDouble *a1, const DataArrayDouble *a2);
    MEDCOUPLING_EXPORT static DataArrayDouble *CrossProduct(const DataArrayDouble *a1, const DataArrayDouble *a2);
    MEDCOUPLING_EXPORT static DataArrayDouble *Max(const DataArrayDouble *a1, const DataArrayDouble *a2);
    MEDCOUPLING_EXPORT static DataArrayDouble *Min(const DataArrayDouble *a1, const DataArrayDouble *a2);
    MEDCOUPLING_EXPORT static DataArrayDouble *Add(const DataArrayDouble *a1, const DataArrayDouble *a2);
    MEDCOUPLING_EXPORT void addEqual(const DataArrayDouble *other);
    MEDCOUPLING_EXPORT static DataArrayDouble *Substract(const DataArrayDouble *a1, const DataArrayDouble *a2);
    MEDCOUPLING_EXPORT void substractEqual(const DataArrayDouble *other);
    MEDCOUPLING_EXPORT static DataArrayDouble *Multiply(const DataArrayDouble *a1, const DataArrayDouble *a2);
    MEDCOUPLING_EXPORT void multiplyEqual(const DataArrayDouble *other);
    MEDCOUPLING_EXPORT static DataArrayDouble *Divide(const DataArrayDouble *a1, const DataArrayDouble *a2);
    MEDCOUPLING_EXPORT void divideEqual(const DataArrayDouble *other);
    MEDCOUPLING_EXPORT static DataArrayDouble *Pow(const DataArrayDouble *a1, const DataArrayDouble *a2);
    MEDCOUPLING_EXPORT void powEqual(const DataArrayDouble *other);
    MEDCOUPLING_EXPORT void updateTime() const { }
    MEDCOUPLING_EXPORT MemArray<double>& accessToMemArray() { return _mem; }
    MEDCOUPLING_EXPORT const MemArray<double>& accessToMemArray() const { return _mem; }
    MEDCOUPLING_EXPORT std::vector<bool> toVectorOfBool(double eps) const;
    MEDCOUPLING_EXPORT static void Rotate2DAlg(const double *center, double angle, int nbNodes, const double *coordsIn, double *coordsOut);
    MEDCOUPLING_EXPORT static void Rotate3DAlg(const double *center, const double *vect, double angle, int nbNodes, const double *coordsIn, double *coordsOut);
    MEDCOUPLING_EXPORT static void Symmetry3DPlane(const double point[3], const double normalVector[3], int nbNodes, const double *coordsIn, double *coordsOut);
    MEDCOUPLING_EXPORT static void GiveBaseForPlane(const double normalVector[3], double baseOfPlane[9]);
  public:
    MEDCOUPLING_EXPORT void getTinySerializationIntInformation(std::vector<int>& tinyInfo) const;
    MEDCOUPLING_EXPORT void getTinySerializationStrInformation(std::vector<std::string>& tinyInfo) const;
    MEDCOUPLING_EXPORT bool resizeForUnserialization(const std::vector<int>& tinyInfoI);
    MEDCOUPLING_EXPORT void finishUnserialization(const std::vector<int>& tinyInfoI, const std::vector<std::string>& tinyInfoS);
  public:
    template<int SPACEDIM>
    void findCommonTuplesAlg(const double *bbox, int nbNodes, int limitNodeId, double prec, DataArrayInt *c, DataArrayInt *cI) const;
    template<int SPACEDIM>
    static void FindClosestTupleIdAlg(const BBTreePts<SPACEDIM,int>& myTree, double dist, const double *pos, int nbOfTuples, const double *thisPt, int thisNbOfTuples, int *res);
    template<int SPACEDIM>
    static void FindTupleIdsNearTuplesAlg(const BBTreePts<SPACEDIM,int>& myTree, const double *pos, int nbOfTuples, double eps,
                                          DataArrayInt *c, DataArrayInt *cI);
  private:
    ~DataArrayDouble() { }
    DataArrayDouble() { }
  };

  class DataArrayDoubleTuple;

  class DataArrayDoubleIterator
  {
  public:
    MEDCOUPLING_EXPORT DataArrayDoubleIterator(DataArrayDouble *da);
    MEDCOUPLING_EXPORT ~DataArrayDoubleIterator();
    MEDCOUPLING_EXPORT DataArrayDoubleTuple *nextt();
  private:
    DataArrayDouble *_da;
    double *_pt;
    int _tuple_id;
    int _nb_comp;
    int _nb_tuple;
  };

  class DataArrayDoubleTuple
  {
  public:
    MEDCOUPLING_EXPORT DataArrayDoubleTuple(double *pt, int nbOfComp);
    MEDCOUPLING_EXPORT std::string repr() const;
    MEDCOUPLING_EXPORT int getNumberOfCompo() const { return _nb_of_compo; }
    MEDCOUPLING_EXPORT const double *getConstPointer() const { return  _pt; }
    MEDCOUPLING_EXPORT double *getPointer() { return _pt; }
    MEDCOUPLING_EXPORT double doubleValue() const;
    MEDCOUPLING_EXPORT DataArrayDouble *buildDADouble(int nbOfTuples, int nbOfCompo) const;
  private:
    double *_pt;
    int _nb_of_compo;
  };

  class DataArrayIntIterator;

  class DataArrayInt : public DataArrayTemplate<int>
  {
  public:
    MEDCOUPLING_EXPORT static DataArrayInt *New();
    MEDCOUPLING_EXPORT int intValue() const;
    MEDCOUPLING_EXPORT int getHashCode() const;
    MEDCOUPLING_EXPORT DataArrayInt *deepCopy() const;
    MEDCOUPLING_EXPORT DataArrayInt *performCopyOrIncrRef(bool deepCopy) const;
    MEDCOUPLING_EXPORT DataArrayInt *buildNewEmptyInstance() const { return DataArrayInt::New(); }
    MEDCOUPLING_EXPORT bool isEqual(const DataArrayInt& other) const;
    MEDCOUPLING_EXPORT bool isEqualIfNotWhy(const DataArrayInt& other, std::string& reason) const;
    MEDCOUPLING_EXPORT bool isEqualWithoutConsideringStr(const DataArrayInt& other) const;
    MEDCOUPLING_EXPORT bool isEqualWithoutConsideringStrAndOrder(const DataArrayInt& other) const;
    MEDCOUPLING_EXPORT bool isFittingWith(const std::vector<bool>& v) const;
    MEDCOUPLING_EXPORT void switchOnTupleEqualTo(int val, std::vector<bool>& vec) const;
    MEDCOUPLING_EXPORT void switchOnTupleNotEqualTo(int val, std::vector<bool>& vec) const;
    MEDCOUPLING_EXPORT DataArrayInt *buildPermutationArr(const DataArrayInt& other) const;
    MEDCOUPLING_EXPORT DataArrayInt *indicesOfSubPart(const DataArrayInt& partOfThis) const;
    MEDCOUPLING_EXPORT DataArrayInt *sumPerTuple() const;
    MEDCOUPLING_EXPORT void checkMonotonic(bool increasing) const;
    MEDCOUPLING_EXPORT bool isMonotonic(bool increasing) const;
    MEDCOUPLING_EXPORT void checkStrictlyMonotonic(bool increasing) const;
    MEDCOUPLING_EXPORT bool isStrictlyMonotonic(bool increasing) const;
    MEDCOUPLING_EXPORT void fillWithZero();
    MEDCOUPLING_EXPORT void iota(int init=0);
    MEDCOUPLING_EXPORT std::string repr() const;
    MEDCOUPLING_EXPORT std::string reprZip() const;
    MEDCOUPLING_EXPORT std::string reprNotTooLong() const;
    MEDCOUPLING_EXPORT void writeVTK(std::ostream& ofs, int indent, const std::string& type, const std::string& nameInFile, DataArrayByte *byteArr) const;
    MEDCOUPLING_EXPORT void reprStream(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprZipStream(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprNotTooLongStream(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprWithoutNameStream(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprZipWithoutNameStream(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprNotTooLongWithoutNameStream(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprCppStream(const std::string& varName, std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprQuickOverview(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprQuickOverviewData(std::ostream& stream, std::size_t maxNbOfByteInRepr) const;
    MEDCOUPLING_EXPORT void transformWithIndArr(const int *indArrBg, const int *indArrEnd);
    MEDCOUPLING_EXPORT DataArrayInt *transformWithIndArrR(const int *indArrBg, const int *indArrEnd) const;
    MEDCOUPLING_EXPORT void splitByValueRange(const int *arrBg, const int *arrEnd,
                                              DataArrayInt *& castArr, DataArrayInt *& rankInsideCast, DataArrayInt *& castsPresent) const;
    MEDCOUPLING_EXPORT bool isRange(int& strt, int& sttoopp, int& stteepp) const;
    MEDCOUPLING_EXPORT DataArrayInt *invertArrayO2N2N2O(int newNbOfElem) const;
    MEDCOUPLING_EXPORT DataArrayInt *invertArrayN2O2O2N(int oldNbOfElem) const;
    MEDCOUPLING_EXPORT DataArrayInt *invertArrayO2N2N2OBis(int newNbOfElem) const;
    MEDCOUPLING_EXPORT DataArrayDouble *convertToDblArr() const;
    MEDCOUPLING_EXPORT DataArrayInt *fromNoInterlace() const;
    MEDCOUPLING_EXPORT DataArrayInt *toNoInterlace() const;
    MEDCOUPLING_EXPORT DataArrayInt *selectByTupleId(const int *new2OldBg, const int *new2OldEnd) const { return DataArrayTemplate<int>::mySelectByTupleId(new2OldBg,new2OldEnd); }
    MEDCOUPLING_EXPORT DataArrayInt *selectByTupleId(const DataArrayInt& di) const { return DataArrayTemplate<int>::mySelectByTupleId(di); }
    MEDCOUPLING_EXPORT DataArrayInt *selectByTupleIdSafe(const int *new2OldBg, const int *new2OldEnd) const { return DataArrayTemplate<int>::mySelectByTupleIdSafe(new2OldBg,new2OldEnd); }
    MEDCOUPLING_EXPORT DataArrayInt *keepSelectedComponents(const std::vector<int>& compoIds) const { return DataArrayTemplate<int>::myKeepSelectedComponents(compoIds); }
    MEDCOUPLING_EXPORT DataArrayInt *selectByTupleIdSafeSlice(int bg, int aend, int step) const { return DataArrayTemplate<int>::mySelectByTupleIdSafeSlice(bg,aend,step); }
    MEDCOUPLING_EXPORT DataArrayInt *selectByTupleRanges(const std::vector<std::pair<int,int> >& ranges) const { return DataArrayTemplate<int>::mySelectByTupleRanges(ranges); }
    MEDCOUPLING_EXPORT DataArrayInt *checkAndPreparePermutation() const;
    MEDCOUPLING_EXPORT static DataArrayInt *FindPermutationFromFirstToSecond(const DataArrayInt *ids1, const DataArrayInt *ids2);
    MEDCOUPLING_EXPORT void changeSurjectiveFormat(int targetNb, DataArrayInt *&arr, DataArrayInt *&arrI) const;
    MEDCOUPLING_EXPORT static DataArrayInt *ConvertIndexArrayToO2N(int nbOfOldTuples, const int *arr, const int *arrIBg, const int *arrIEnd, int &newNbOfTuples);
    MEDCOUPLING_EXPORT DataArrayInt *buildPermArrPerLevel() const;
    MEDCOUPLING_EXPORT bool isIota(int sizeExpected) const;
    MEDCOUPLING_EXPORT bool isUniform(int val) const;
    MEDCOUPLING_EXPORT bool hasUniqueValues() const;
    MEDCOUPLING_EXPORT void meldWith(const DataArrayInt *other);
    MEDCOUPLING_EXPORT void setSelectedComponents(const DataArrayInt *a, const std::vector<int>& compoIds);
    MEDCOUPLING_EXPORT void getTuple(int tupleId, int *res) const { std::copy(_mem.getConstPointerLoc(tupleId*_info_on_compo.size()),_mem.getConstPointerLoc((tupleId+1)*_info_on_compo.size()),res); }
    MEDCOUPLING_EXPORT static void SetArrayIn(DataArrayInt *newArray, DataArrayInt* &arrayToSet);
    MEDCOUPLING_EXPORT DataArrayIntIterator *iterator();
    MEDCOUPLING_EXPORT DataArrayInt *findIdsEqual(int val) const;
    MEDCOUPLING_EXPORT DataArrayInt *findIdsNotEqual(int val) const;
    MEDCOUPLING_EXPORT DataArrayInt *findIdsEqualList(const int *valsBg, const int *valsEnd) const;
    MEDCOUPLING_EXPORT DataArrayInt *findIdsNotEqualList(const int *valsBg, const int *valsEnd) const;
    MEDCOUPLING_EXPORT DataArrayInt *findIdsEqualTuple(const int *tupleBg, const int *tupleEnd) const;
    MEDCOUPLING_EXPORT int changeValue(int oldValue, int newValue);
    MEDCOUPLING_EXPORT int findIdFirstEqualTuple(const std::vector<int>& tupl) const;
    MEDCOUPLING_EXPORT int findIdFirstEqual(int value) const;
    MEDCOUPLING_EXPORT int findIdFirstEqual(const std::vector<int>& vals) const;
    MEDCOUPLING_EXPORT int findIdSequence(const std::vector<int>& vals) const;
    MEDCOUPLING_EXPORT bool presenceOfTuple(const std::vector<int>& tupl) const;
    MEDCOUPLING_EXPORT bool presenceOfValue(int value) const;
    MEDCOUPLING_EXPORT bool presenceOfValue(const std::vector<int>& vals) const;
    MEDCOUPLING_EXPORT int count(int value) const;
    MEDCOUPLING_EXPORT void accumulate(int *res) const;
    MEDCOUPLING_EXPORT int accumulate(int compId) const;
    MEDCOUPLING_EXPORT DataArrayInt *accumulatePerChunck(const int *bgOfIndex, const int *endOfIndex) const;
    MEDCOUPLING_EXPORT void getMinMaxValues(int& minValue, int& maxValue) const;
    MEDCOUPLING_EXPORT void abs();
    MEDCOUPLING_EXPORT DataArrayInt *computeAbs() const;
    MEDCOUPLING_EXPORT void applyLin(int a, int b, int compoId);
    MEDCOUPLING_EXPORT void applyLin(int a, int b);
    MEDCOUPLING_EXPORT void applyInv(int numerator);
    MEDCOUPLING_EXPORT DataArrayInt *negate() const;
    MEDCOUPLING_EXPORT void applyDivideBy(int val);
    MEDCOUPLING_EXPORT void applyModulus(int val);
    MEDCOUPLING_EXPORT void applyRModulus(int val);
    MEDCOUPLING_EXPORT void applyPow(int val);
    MEDCOUPLING_EXPORT void applyRPow(int val);
    MEDCOUPLING_EXPORT DataArrayInt *findIdsInRange(int vmin, int vmax) const;
    MEDCOUPLING_EXPORT DataArrayInt *findIdsNotInRange(int vmin, int vmax) const;
    MEDCOUPLING_EXPORT DataArrayInt *findIdsStricltyNegative() const;
    MEDCOUPLING_EXPORT bool checkAllIdsInRange(int vmin, int vmax) const;
    MEDCOUPLING_EXPORT static DataArrayInt *Aggregate(const DataArrayInt *a1, const DataArrayInt *a2, int offsetA2);
    MEDCOUPLING_EXPORT static DataArrayInt *Aggregate(const std::vector<const DataArrayInt *>& arr);
    MEDCOUPLING_EXPORT static DataArrayInt *AggregateIndexes(const std::vector<const DataArrayInt *>& arrs);
    MEDCOUPLING_EXPORT static DataArrayInt *Meld(const DataArrayInt *a1, const DataArrayInt *a2);
    MEDCOUPLING_EXPORT static DataArrayInt *Meld(const std::vector<const DataArrayInt *>& arr);
    MEDCOUPLING_EXPORT static DataArrayInt *MakePartition(const std::vector<const DataArrayInt *>& groups, int newNb, std::vector< std::vector<int> >& fidsOfGroups);
    MEDCOUPLING_EXPORT static DataArrayInt *BuildUnion(const std::vector<const DataArrayInt *>& arr);
    MEDCOUPLING_EXPORT static DataArrayInt *BuildIntersection(const std::vector<const DataArrayInt *>& arr);
    MEDCOUPLING_EXPORT static DataArrayInt *BuildListOfSwitchedOn(const std::vector<bool>& v);
    MEDCOUPLING_EXPORT static DataArrayInt *BuildListOfSwitchedOff(const std::vector<bool>& v);
    MEDCOUPLING_EXPORT static void PutIntoToSkylineFrmt(const std::vector< std::vector<int> >& v, DataArrayInt *& data, DataArrayInt *& dataIndex);
    MEDCOUPLING_EXPORT DataArrayInt *buildComplement(int nbOfElement) const;
    MEDCOUPLING_EXPORT DataArrayInt *buildSubstraction(const DataArrayInt *other) const;
    MEDCOUPLING_EXPORT DataArrayInt *buildSubstractionOptimized(const DataArrayInt *other) const;
    MEDCOUPLING_EXPORT DataArrayInt *buildUnion(const DataArrayInt *other) const;
    MEDCOUPLING_EXPORT DataArrayInt *buildIntersection(const DataArrayInt *other) const;
    MEDCOUPLING_EXPORT DataArrayInt *buildUnique() const;
    MEDCOUPLING_EXPORT DataArrayInt *buildUniqueNotSorted() const;
    MEDCOUPLING_EXPORT DataArrayInt *deltaShiftIndex() const;
    MEDCOUPLING_EXPORT void computeOffsets();
    MEDCOUPLING_EXPORT void computeOffsetsFull();
    MEDCOUPLING_EXPORT void findIdsRangesInListOfIds(const DataArrayInt *listOfIds, DataArrayInt *& rangeIdsFetched, DataArrayInt *& idsInInputListThatFetch) const;
    MEDCOUPLING_EXPORT DataArrayInt *buildExplicitArrByRanges(const DataArrayInt *offsets) const;
    MEDCOUPLING_EXPORT DataArrayInt *buildExplicitArrOfSliceOnScaledArr(int begin, int stop, int step) const;
    MEDCOUPLING_EXPORT DataArrayInt *findRangeIdForEachTuple(const DataArrayInt *ranges) const;
    MEDCOUPLING_EXPORT DataArrayInt *findIdInRangeForEachTuple(const DataArrayInt *ranges) const;
    MEDCOUPLING_EXPORT void sortEachPairToMakeALinkedList();
    MEDCOUPLING_EXPORT DataArrayInt *duplicateEachTupleNTimes(int nbTimes) const;
    MEDCOUPLING_EXPORT DataArrayInt *getDifferentValues() const;
    MEDCOUPLING_EXPORT std::vector<DataArrayInt *> partitionByDifferentValues(std::vector<int>& differentIds) const;
    MEDCOUPLING_EXPORT std::vector< std::pair<int,int> > splitInBalancedSlices(int nbOfSlices) const;
    template<class InputIterator>
    void insertAtTheEnd(InputIterator first, InputIterator last);
    MEDCOUPLING_EXPORT void aggregate(const DataArrayInt *other);
    MEDCOUPLING_EXPORT void writeOnPlace(std::size_t id, int element0, const int *others, int sizeOfOthers) { _mem.writeOnPlace(id,element0,others,sizeOfOthers); }
    MEDCOUPLING_EXPORT static DataArrayInt *Add(const DataArrayInt *a1, const DataArrayInt *a2);
    MEDCOUPLING_EXPORT void addEqual(const DataArrayInt *other);
    MEDCOUPLING_EXPORT static DataArrayInt *Substract(const DataArrayInt *a1, const DataArrayInt *a2);
    MEDCOUPLING_EXPORT void substractEqual(const DataArrayInt *other);
    MEDCOUPLING_EXPORT static DataArrayInt *Multiply(const DataArrayInt *a1, const DataArrayInt *a2);
    MEDCOUPLING_EXPORT void multiplyEqual(const DataArrayInt *other);
    MEDCOUPLING_EXPORT static DataArrayInt *Divide(const DataArrayInt *a1, const DataArrayInt *a2);
    MEDCOUPLING_EXPORT void divideEqual(const DataArrayInt *other);
    MEDCOUPLING_EXPORT static DataArrayInt *Modulus(const DataArrayInt *a1, const DataArrayInt *a2);
    MEDCOUPLING_EXPORT void modulusEqual(const DataArrayInt *other);
    MEDCOUPLING_EXPORT static DataArrayInt *Pow(const DataArrayInt *a1, const DataArrayInt *a2);
    MEDCOUPLING_EXPORT void powEqual(const DataArrayInt *other);
    MEDCOUPLING_EXPORT void updateTime() const { }
    MEDCOUPLING_EXPORT MemArray<int>& accessToMemArray() { return _mem; }
    MEDCOUPLING_EXPORT const MemArray<int>& accessToMemArray() const { return _mem; }
  public:
    MEDCOUPLING_EXPORT static int *CheckAndPreparePermutation(const int *start, const int *end);
    MEDCOUPLING_EXPORT static DataArrayInt *Range(int begin, int end, int step);
  public:
    MEDCOUPLING_EXPORT void getTinySerializationIntInformation(std::vector<int>& tinyInfo) const;
    MEDCOUPLING_EXPORT void getTinySerializationStrInformation(std::vector<std::string>& tinyInfo) const;
    MEDCOUPLING_EXPORT bool resizeForUnserialization(const std::vector<int>& tinyInfoI);
    MEDCOUPLING_EXPORT void finishUnserialization(const std::vector<int>& tinyInfoI, const std::vector<std::string>& tinyInfoS);
  private:
    ~DataArrayInt() { }
    DataArrayInt() { }
  };

  class DataArrayIntTuple;

  class DataArrayIntIterator
  {
  public:
    MEDCOUPLING_EXPORT DataArrayIntIterator(DataArrayInt *da);
    MEDCOUPLING_EXPORT ~DataArrayIntIterator();
    MEDCOUPLING_EXPORT DataArrayIntTuple *nextt();
  private:
    DataArrayInt *_da;
    int *_pt;
    int _tuple_id;
    int _nb_comp;
    int _nb_tuple;
  };

  class DataArrayIntTuple
  {
  public:
    MEDCOUPLING_EXPORT DataArrayIntTuple(int *pt, int nbOfComp);
    MEDCOUPLING_EXPORT std::string repr() const;
    MEDCOUPLING_EXPORT int getNumberOfCompo() const { return _nb_of_compo; }
    MEDCOUPLING_EXPORT const int *getConstPointer() const { return  _pt; }
    MEDCOUPLING_EXPORT int *getPointer() { return _pt; }
    MEDCOUPLING_EXPORT int intValue() const;
    MEDCOUPLING_EXPORT DataArrayInt *buildDAInt(int nbOfTuples, int nbOfCompo) const;
  private:
    int *_pt;
    int _nb_of_compo;
  };

  class DataArrayChar : public DataArrayTemplate<char>
  {
  public:
    MEDCOUPLING_EXPORT virtual DataArrayChar *buildEmptySpecializedDAChar() const = 0;
    MEDCOUPLING_EXPORT int getHashCode() const;
    MEDCOUPLING_EXPORT bool isEqual(const DataArrayChar& other) const;
    MEDCOUPLING_EXPORT virtual bool isEqualIfNotWhy(const DataArrayChar& other, std::string& reason) const;
    MEDCOUPLING_EXPORT bool isEqualWithoutConsideringStr(const DataArrayChar& other) const;
    MEDCOUPLING_EXPORT void fillWithZero();
    MEDCOUPLING_EXPORT std::string repr() const;
    MEDCOUPLING_EXPORT std::string reprZip() const;
    MEDCOUPLING_EXPORT DataArrayInt *convertToIntArr() const;
    MEDCOUPLING_EXPORT DataArrayChar *selectByTupleId(const int *new2OldBg, const int *new2OldEnd) const { return DataArrayTemplate<char>::mySelectByTupleId(new2OldBg,new2OldEnd); }
    MEDCOUPLING_EXPORT DataArrayChar *selectByTupleId(const DataArrayInt& di) const { return DataArrayTemplate<char>::mySelectByTupleId(di); }
    MEDCOUPLING_EXPORT DataArrayChar *selectByTupleIdSafe(const int *new2OldBg, const int *new2OldEnd) const { return DataArrayTemplate<char>::mySelectByTupleIdSafe(new2OldBg,new2OldEnd); }
    MEDCOUPLING_EXPORT DataArrayChar *keepSelectedComponents(const std::vector<int>& compoIds) const { return DataArrayTemplate<char>::myKeepSelectedComponents(compoIds); }
    MEDCOUPLING_EXPORT DataArrayChar *selectByTupleIdSafeSlice(int bg, int aend, int step) const { return DataArrayTemplate<char>::mySelectByTupleIdSafeSlice(bg,aend,step); }
    MEDCOUPLING_EXPORT bool isUniform(char val) const;
    MEDCOUPLING_EXPORT void meldWith(const DataArrayChar *other);
    MEDCOUPLING_EXPORT DataArray *selectByTupleRanges(const std::vector<std::pair<int,int> >& ranges) const { return DataArrayTemplate<char>::mySelectByTupleRanges(ranges); }
    MEDCOUPLING_EXPORT void getTuple(int tupleId, char *res) const { std::copy(_mem.getConstPointerLoc(tupleId*_info_on_compo.size()),_mem.getConstPointerLoc((tupleId+1)*_info_on_compo.size()),res); }
    MEDCOUPLING_EXPORT DataArrayInt *findIdsEqual(char val) const;
    MEDCOUPLING_EXPORT DataArrayInt *findIdsNotEqual(char val) const;
    MEDCOUPLING_EXPORT int findIdSequence(const std::vector<char>& vals) const;
    MEDCOUPLING_EXPORT int findIdFirstEqualTuple(const std::vector<char>& tupl) const;
    MEDCOUPLING_EXPORT int findIdFirstEqual(char value) const;
    MEDCOUPLING_EXPORT int findIdFirstEqual(const std::vector<char>& vals) const;
    MEDCOUPLING_EXPORT bool presenceOfTuple(const std::vector<char>& tupl) const;
    MEDCOUPLING_EXPORT bool presenceOfValue(char value) const;
    MEDCOUPLING_EXPORT bool presenceOfValue(const std::vector<char>& vals) const;
    MEDCOUPLING_EXPORT DataArrayInt *findIdsInRange(char vmin, char vmax) const;
    MEDCOUPLING_EXPORT static DataArrayChar *Aggregate(const DataArrayChar *a1, const DataArrayChar *a2);
    MEDCOUPLING_EXPORT static DataArrayChar *Aggregate(const std::vector<const DataArrayChar *>& arr);
    MEDCOUPLING_EXPORT static DataArrayChar *Meld(const DataArrayChar *a1, const DataArrayChar *a2);
    MEDCOUPLING_EXPORT static DataArrayChar *Meld(const std::vector<const DataArrayChar *>& arr);
    template<class InputIterator>
    void insertAtTheEnd(InputIterator first, InputIterator last);
    MEDCOUPLING_EXPORT void updateTime() const { }
    MEDCOUPLING_EXPORT MemArray<char>& accessToMemArray() { return _mem; }
    MEDCOUPLING_EXPORT const MemArray<char>& accessToMemArray() const { return _mem; }
  public:
    //MEDCOUPLING_EXPORT void getTinySerializationIntInformation(std::vector<int>& tinyInfo) const;
    //MEDCOUPLING_EXPORT void getTinySerializationStrInformation(std::vector<std::string>& tinyInfo) const;
    //MEDCOUPLING_EXPORT bool resizeForUnserialization(const std::vector<int>& tinyInfoI);
    //MEDCOUPLING_EXPORT void finishUnserialization(const std::vector<int>& tinyInfoI, const std::vector<std::string>& tinyInfoS);
  protected:
    DataArrayChar() { }
  };

  class DataArrayByteIterator;

  class DataArrayByte : public DataArrayChar
  {
  public:
    MEDCOUPLING_EXPORT static DataArrayByte *New();
    MEDCOUPLING_EXPORT DataArrayChar *buildEmptySpecializedDAChar() const;
    MEDCOUPLING_EXPORT DataArrayByteIterator *iterator();
    MEDCOUPLING_EXPORT DataArrayByte *deepCopy() const;
    MEDCOUPLING_EXPORT DataArrayByte *performCopyOrIncrRef(bool deepCopy) const;
    MEDCOUPLING_EXPORT DataArrayByte *buildNewEmptyInstance() const { return DataArrayByte::New(); }
    MEDCOUPLING_EXPORT char byteValue() const;
    MEDCOUPLING_EXPORT void reprStream(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprZipStream(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprWithoutNameStream(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprZipWithoutNameStream(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprCppStream(const std::string& varName, std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprQuickOverview(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprQuickOverviewData(std::ostream& stream, std::size_t maxNbOfByteInRepr) const;
    MEDCOUPLING_EXPORT bool isEqualIfNotWhy(const DataArrayChar& other, std::string& reason) const;
    MEDCOUPLING_EXPORT std::vector<bool> toVectorOfBool() const;
  private:
    ~DataArrayByte() { }
    DataArrayByte() { }
  };

  class DataArrayByteTuple;

  class DataArrayByteIterator
  {
  public:
    MEDCOUPLING_EXPORT DataArrayByteIterator(DataArrayByte *da);
    MEDCOUPLING_EXPORT ~DataArrayByteIterator();
    MEDCOUPLING_EXPORT DataArrayByteTuple *nextt();
  private:
    DataArrayByte *_da;
    char *_pt;
    int _tuple_id;
    int _nb_comp;
    int _nb_tuple;
  };

  class DataArrayByteTuple
  {
  public:
    MEDCOUPLING_EXPORT DataArrayByteTuple(char *pt, int nbOfComp);
    MEDCOUPLING_EXPORT std::string repr() const;
    MEDCOUPLING_EXPORT int getNumberOfCompo() const { return _nb_of_compo; }
    MEDCOUPLING_EXPORT const char *getConstPointer() const { return  _pt; }
    MEDCOUPLING_EXPORT char *getPointer() { return _pt; }
    MEDCOUPLING_EXPORT char byteValue() const;
    MEDCOUPLING_EXPORT DataArrayByte *buildDAByte(int nbOfTuples, int nbOfCompo) const;
  private:
    char *_pt;
    int _nb_of_compo;
  };

  class DataArrayAsciiCharIterator;

  class DataArrayAsciiChar : public DataArrayChar
  {
  public:
    MEDCOUPLING_EXPORT static DataArrayAsciiChar *New();
    MEDCOUPLING_EXPORT static DataArrayAsciiChar *New(const std::string& st);
    MEDCOUPLING_EXPORT static DataArrayAsciiChar *New(const std::vector<std::string>& vst, char defaultChar);
    MEDCOUPLING_EXPORT DataArrayChar *buildEmptySpecializedDAChar() const;
    MEDCOUPLING_EXPORT DataArrayAsciiCharIterator *iterator();
    MEDCOUPLING_EXPORT DataArrayAsciiChar *deepCopy() const;
    MEDCOUPLING_EXPORT DataArrayAsciiChar *performCopyOrIncrRef(bool deepCopy) const;
    MEDCOUPLING_EXPORT DataArrayAsciiChar *buildNewEmptyInstance() const { return DataArrayAsciiChar::New(); }
    MEDCOUPLING_EXPORT char asciiCharValue() const;
    MEDCOUPLING_EXPORT void reprStream(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprZipStream(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprWithoutNameStream(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprZipWithoutNameStream(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprCppStream(const std::string& varName, std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprQuickOverview(std::ostream& stream) const;
    MEDCOUPLING_EXPORT void reprQuickOverviewData(std::ostream& stream, std::size_t maxNbOfByteInRepr) const;
    MEDCOUPLING_EXPORT bool isEqualIfNotWhy(const DataArrayChar& other, std::string& reason) const;
  private:
    ~DataArrayAsciiChar() { }
    DataArrayAsciiChar() { }
    DataArrayAsciiChar(const std::string& st);
    DataArrayAsciiChar(const std::vector<std::string>& vst, char defaultChar);
  };

  class DataArrayAsciiCharTuple;

  class DataArrayAsciiCharIterator
  {
  public:
    MEDCOUPLING_EXPORT DataArrayAsciiCharIterator(DataArrayAsciiChar *da);
    MEDCOUPLING_EXPORT ~DataArrayAsciiCharIterator();
    MEDCOUPLING_EXPORT DataArrayAsciiCharTuple *nextt();
  private:
    DataArrayAsciiChar *_da;
    char *_pt;
    int _tuple_id;
    int _nb_comp;
    int _nb_tuple;
  };

  class DataArrayAsciiCharTuple
  {
  public:
    MEDCOUPLING_EXPORT DataArrayAsciiCharTuple(char *pt, int nbOfComp);
    MEDCOUPLING_EXPORT std::string repr() const;
    MEDCOUPLING_EXPORT int getNumberOfCompo() const { return _nb_of_compo; }
    MEDCOUPLING_EXPORT const char *getConstPointer() const { return  _pt; }
    MEDCOUPLING_EXPORT char *getPointer() { return _pt; }
    MEDCOUPLING_EXPORT char asciiCharValue() const;
    MEDCOUPLING_EXPORT DataArrayAsciiChar *buildDAAsciiChar(int nbOfTuples, int nbOfCompo) const;
  private:
    char *_pt;
    int _nb_of_compo;
  };
}

namespace MEDCoupling
{
  template<class T>
  template<class InputIterator>
  void MemArray<T>::insertAtTheEnd(InputIterator first, InputIterator last)
  {
    T *pointer=_pointer.getPointer();
    while(first!=last)
      {
        if(_nb_of_elem>=_nb_of_elem_alloc)
          {
            reserve(_nb_of_elem_alloc>0?2*_nb_of_elem_alloc:1);
            pointer=_pointer.getPointer();
          }
        pointer[_nb_of_elem++]=*first++;
      }
  }
  
  template<class InputIterator>
  void DataArrayDouble::insertAtTheEnd(InputIterator first, InputIterator last)
  {
    int nbCompo(getNumberOfComponents());
    if(nbCompo==1)
      _mem.insertAtTheEnd(first,last);
    else if(nbCompo==0)
      {
        _info_on_compo.resize(1);
        _mem.insertAtTheEnd(first,last);
      }
    else
      throw INTERP_KERNEL::Exception("DataArrayDouble::insertAtTheEnd : not available for DataArrayDouble with number of components different than 1 !");
  }

  template<class InputIterator>
  void DataArrayInt::insertAtTheEnd(InputIterator first, InputIterator last)
  {
    int nbCompo(getNumberOfComponents());
    if(nbCompo==1)
      _mem.insertAtTheEnd(first,last);
    else if(nbCompo==0)
      {
        _info_on_compo.resize(1);
        _mem.insertAtTheEnd(first,last);
      }
    else
      throw INTERP_KERNEL::Exception("DataArrayInt::insertAtTheEnd : not available for DataArrayInt with number of components different than 1 !");
  }

  template<class InputIterator>
  void DataArrayChar::insertAtTheEnd(InputIterator first, InputIterator last)
  {
    int nbCompo(getNumberOfComponents());
    if(nbCompo==1)
      _mem.insertAtTheEnd(first,last);
    else if(nbCompo==0)
      {
        _info_on_compo.resize(1);
        _mem.insertAtTheEnd(first,last);
      }
    else
      throw INTERP_KERNEL::Exception("DataArrayChar::insertAtTheEnd : not available for DataArrayChar with number of components different than 1 !");
  }
}

#endif
