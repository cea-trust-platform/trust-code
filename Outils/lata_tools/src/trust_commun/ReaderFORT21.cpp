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
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LoSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

#include <ReaderFORT21.h>

#include <algorithm>
#include <cassert>
#include <float.h>
#include <iostream>
#include <set>
#include <numeric>
#include <stdio.h>

#include <exception>
#include <iomanip>  // used for setprecision()
#include <iostream>
#include <string.h>
#include <sstream>

#define MAX_NAME_SIZE 16
#define MAX_NUM_VARS 10000

#ifdef WNT
#pragma warning(disable : 4996)
#else
#include <fenv.h>
#include <math.h>
#include <string.h>
#endif

/*

Description of FORT21

Heder:
 TYPE$$$$ FICHIER$ DOUBLE$$
 VERSION$ LABEL$$$ NONE$$$$
 IDENT$$$ 26/03/24  c3 v3.0.0 (git: f46e225) Date: 11-12-2023                                        PWR 6 INCH COLD LEG BREAK LOCA                                                  08:02:39

Description (constant part)
 DESCR$$$
list de STACK (see below)

List of times
 VARIA$$$
 VARIA$$$ 0.000000D+00
list de STACK (see below)
 VARIA$$$
 VARIA$$$ 1.000000D+00
....


Read STACK
 DESSTACK
 name_of_stack type   nb_ptr nb_int nb_float nb_string
 DIMEVAL$$          (nb_ptr+nb_int+nb_float+nb_string) integers size of the vars
for each type
 var_name value(s)

exemple
 DESSTACK
 WALL1            WALVORIF                1        3        9        1
 DIMVAL$$        1        1        1        2        1        1        2        1        1        1        2        2        2        1
 ITYPER                540
 GLOBAL                  1
 SCALAR                  1
 IWHYD                   1        1
 GLOBAL           0.100000D+03
 SCALAR           0.110000D+03
 ZSW              0.100000D+01 0.200000D+01
 WHSURTOT         0.453616D+02
 ZPMIN            0.200000D-03
 ZPMAX            0.288800D+01
 RAYON1           0.250000D+01 0.250000D+01
 RAYON2           0.255000D+01 0.255000D+01
 RAYON3           0.275200D+01 0.275200D+01
 NAMPER           VOLDOWN


 */
struct tbuff
{
  int Lo;
  int Hi;
  tbuff()
    : Lo(0)
    , Hi(0)
  {
  }
};

//=============================================================================
// Function : int InverseInt
// Purpose  :
//=============================================================================
inline static int InverseInt(const int theValue)
{
  return (0 | ((theValue & 0x000000ff) << 24) | ((theValue & 0x0000ff00) << 8) | ((theValue & 0x00ff0000) >> 8) | ((theValue >> 24) & 0x000000ff));
}

//================================================================
// Function : inverse
// Purpose  :
//================================================================
inline static tbuff inverse(const tbuff theBuff)
{
  tbuff aRes;
  aRes.Hi = InverseInt(theBuff.Lo);
  aRes.Lo = InverseInt(theBuff.Hi);
  return aRes;
}

//=============================================================================
// Function : getSizeOfRecordLength
// Purpose  : Fets amount of bytes used for storing record length in FORT21 file
//=============================================================================
inline static int getSizeOfRecordLength(Platform thePlatform)
{
  return thePlatform == Linux_64gcc ? 8 : 4;
}

//=============================================================================
// Function : int getSizeOfLong
// Purpose  : gets amount of bytes used for storing integer variable in FORT21 file
//=============================================================================
inline static int getSizeOfLong(Platform thePlatform)
{
  return thePlatform == Linux_64f90 || thePlatform == Tru64_64f90 || thePlatform == Unix_64f77 ? 8 : 4;
}

//=============================================================================
// Function : IsBigEndian
// Purpose  : Verifies whether platform uses Big-endian byte order or Little-endian
//=============================================================================
inline static bool IsBigEndian(Platform thePlatform)
{
  return thePlatform == SGI_32;
}

//=============================================================================
// Function : getSizeOfReal
// Purpose  : gets amount of bytes used for storing float variable in FORT21 file
//=============================================================================
inline static int getSizeOfReal(Platform thePlatform)
{
  return thePlatform == Unix_64f77 || thePlatform == Linux_Tech ? 8 : 4;
}

//=============================================================================
// Function : int ToInt
// Purpose  :
//=============================================================================
inline static int ToInt(const tbuff theBuff, const int theSize, const bool theIsBigEndian)
{
  int aRes;

  // inverse buffer if necessary
  if (theIsBigEndian)
    {
      if (theSize == 8)
        {
          aRes = inverse(theBuff).Lo;
        }
      else
        aRes = InverseInt(theBuff.Lo);
    }
  else
    aRes = theBuff.Lo;

  return aRes;
}

//=============================================================================
// Function : inverseReal
// Purpose  :
//=============================================================================
inline double MyInverseReal(const double& theValue)
{
  double aResult;
  int* i = (int*)&theValue;
  int* j = (int*)&aResult;
  j[1] = InverseInt(i[0]);
  j[0] = InverseInt(i[1]);
  return aResult;
}

//================================================================
// Function : ToReal
// Purpose  : Convert real variable (stored in double theBuff) to
//            double in accordance with parameters of given platform
//================================================================
inline static double ToReal(const tbuff theBuff, const int theSize, const bool theIsBigEndian)
{
#ifdef WNT
  // disable all floating point exceptions
  unsigned int oldx87mask = _control87(MCW_EM, MCW_EM);
#elif defined(__APPLE__)
  feclearexcept(FE_ALL_EXCEPT);
#else
  fedisableexcept(FE_ALL_EXCEPT);
#endif

  double aRes = DBL_MAX;
  // inverse buffer if necessary
  if (theIsBigEndian)
    {
      if (theSize == 8)
        {
          const double* val = (const double*)(&theBuff);
          aRes = MyInverseReal(*val);
          // aRes = MyInverseReal( (const double&)(theBuff) );
        }
      else if (theSize == 4)
        {
          // Function can crash during conversation from Nan to float,
          // so make conversion right here before check result value
          // float aResult = InverseShortReal( *(float*) &theBuff );
          int* j = (int*)&theBuff;
          float aResult;
          int* i = (int*)&aResult;
          *i = InverseInt(*j);
#ifndef WNT
#if defined(__APPLE__)
          if (!isnan(aResult) && !isinf(aResult))
#else
          // Verification performed for Linux platform only because isnan method works
          // with float and double on this platform. On WNT _isnan method works with double
          // argument only. In this case assignement NAN float calue to double leads exception.
          // To avoid this GUITHARE mask floating point exceptions.
          if (!__isnanf(aResult) && !__isinf(aResult))
#endif
#endif
            aRes = aResult;
        }
    }
  else
    {
      if (theSize == 8)
        {
          const double* val = (const double*)&theBuff;
          aRes = *(val);
        }
      else if (theSize == 4)
        {
          const float* val = (const float*)&theBuff;
          aRes = *(val);
        }
    }

#ifdef WNT
  _control87(oldx87mask, MCW_EM);
#endif
  return aRes;
}

//=============================================================================
// Function : float ToFloat
// Purpose  :
//=============================================================================
/* inline static float ToFloat(const tbuff theBuff, const int theSize, const bool theIsBigEndian)
{
  double aVal = ToReal(theBuff, theSize, theIsBigEndian);

  if (aVal > FLT_MAX)
    return FLT_MAX;
  else if (aVal < -FLT_MAX)
    return FLT_MIN;
  else
    return (float)aVal;
} */

//=============================================================================
// Function : ReaderFORT21
// Purpose  :
//=============================================================================
ReaderFORT21::ReaderFORT21(int v)
{
  myStatus = NotDone;
  myRealSize = 4;
  myMaxSizeInt = 100000000;
  myIsC3 = false;
  myIsSimpleReal = true;
  verbosity_ = v;
}

//=============================================================================
// Function : ~ReaderFORT21
// Purpose  :
//=============================================================================
ReaderFORT21::~ReaderFORT21()
{
}

//=============================================================================
// Function : setFile
// Purpose  :
//=============================================================================
void ReaderFORT21::setFile(const std::string& theName)
{
  myInFile = theName;
  // reset previous read
  Times_glob_.clear();
  offset_a_time_.clear();
  elements_list_.clear();

}

//=============================================================================
// Function : getFileName
// Purpose  :
//=============================================================================
const std::string ReaderFORT21::getFileName()
{
  return myInFile;
}

//=============================================================================
// Function : TruncTailWS
// Purpose  : Truncates the trailing white-space in the string of given size
//=============================================================================
inline void TruncTailWS(char* theStr, int theSize)
{
  int i = theSize - 1;
  while (i > -1 && (theStr[i] == ' ' || theStr[i] == 0))
    i--;
  theStr[i + 1] = 0;
}

//=============================================================================
// Function : getStatus
// Purpose  :
//=============================================================================
ReaderFORT21::Status ReaderFORT21::getStatus() const
{
  return myStatus;
}

//=============================================================================
// Function : setPlatform
// Purpose  :
//=============================================================================
void ReaderFORT21::setPlatform(const Platform thePlatform)
{
  myField.setPlatform(thePlatform);
}

//=======================================================================
// function : parse
// purpose  : parses the file
//=======================================================================
void ReaderFORT21::parse()
{
  //#define _TIMER
#ifdef _TIMER
  OSD_Timer aTimer;
  aTimer.Start();
#endif

  myStatus = Fail;

  // open the file
  FILE* aFile = open_file(myInFile.c_str(), "rb");
  if (aFile == 0)
    {
      std::cerr << " error while opening " << myInFile << std::endl;
      // exit(-1);
      return;
    }

  myField.setFile(aFile);
  myField.setMaxFileInt(myMaxSizeInt);

  // read records
  if (!myField.read(24))
    return;

  myStatus = Done;
  while (myField.size() != 0)
    {
      if (!readRecord())
        {
          std::cerr << " error while reading " << myInFile << std::endl;
          // exit(-1);
          return;
        }
    }

#ifdef _TIMER
  aTimer.Stop();
  std::ofstream stream("D:/a.txt", std::ofstream::out);
  aTimer.Show(stream);
#endif
}
void ReaderFORT21::fix_bad_times()
{
  double told=-1;
  bool repair=false;
  unsigned int i=0;
  for (; i<getTimes().size(); i++)
    {
      double t = getTimes()[i];
      if (t==told)
        {
          //std::cout <<"t == told == " << t<<std::endl;
          repair=true;
          break;
        }
      told=t;
    }
  if (repair)
    {
      // on doit fusionner i et i-1
      // on garde le premier enregistrement du champ
      if (verbosity_ > 3)
        std::cout<< " removing duplicate times "<<told<< " nb times before "<< Times_glob_.size()<<std::endl;
      for (auto& a : elements_list_)
        {
          auto& myvector=      a.second.index_times_;
          auto     it = find (myvector.begin(), myvector.end(), i);
          if (it != myvector.end())
            {
              auto     itold = find (myvector.begin(), myvector.end(), i-1);
              if (itold==myvector.end())
                {
                  // on n'avait pas le temps d'avant
                  if (verbosity_ > 3)
                    std::cout <<" removing duplicate times " << told <<" not for "<< a.first  << std::endl;
                  // on change l'offset pour prendre le seul enregistrement dispo
                  a.second.offset_rel_time_[it-myvector.begin()]+= (offset_a_time_[i]-offset_a_time_[i-1] );
                }
              else
                {
                  // on garde le dernier enregistrement pour cela on change l'offset
                  a.second.offset_rel_time_[itold-myvector.begin()] = (offset_a_time_[i]-offset_a_time_[i-1] )   + a.second.offset_rel_time_[it-myvector.begin()];
                  a.second.offset_rel_time_.erase(a.second.offset_rel_time_.begin()+(it-myvector.begin()));
                  myvector.erase(it);
                }

            }
        }
      for (auto& a : elements_list_)
        {
          std::vector<int>& myvector=      a.second.index_times_;

          for (auto& c : myvector)
            {
              if (c>=int(i)) c--;
            }
        }
      Times_glob_.erase(Times_glob_.begin()+i);
      //auto save_offset = offset_a_time_[i];
      offset_a_time_.erase(offset_a_time_.begin()+i);
      //offset_a_time_[i-1]=save_offset;
      fix_bad_times();
      return;
    }
  for (unsigned int e=0; e<getTimes().size(); e++)
    {
      bool found =false;
      for (auto& a : elements_list_)
        {
          auto& myvector=      a.second.index_times_;
          auto     it = find (myvector.begin(), myvector.end(), e);
          if (it != myvector.end())
            {
              if (verbosity_>5) std::cout << "time "<<e <<" found in "<<a.first<<std::endl;
              found = true;
              break;
            }
          if (found) break;

        }
      if (!found)
        std::cout<< "time "<<e <<" no found " <<std::endl;
    }
  for (auto& a : elements_list_)
    {
      auto& stack=      a.second;
      size_t n=stack.index_times_.size();
      for (size_t c=1; c<n; c++)
        {
          if (stack.index_times_[c]== stack.index_times_[c-1])
            {
              if (verbosity_ > 4)
                std::cout << "remove time index  "<<c << " in "<< a.first<<" "<< getTimes()[stack.index_times_[c]] << std::endl;
              stack.index_times_.erase(stack.index_times_.begin()+c);
              stack.offset_rel_time_.erase(stack.offset_rel_time_.begin()+c);
              n-=1;
            }

        }
    }
}

int ReaderFORT21::getNbCells(const std::string& name_stack) const
{
  return getMeshStack(name_stack).nb_elems_;
}
const ReaderFORT21::ElementInfo& ReaderFORT21::getElementInfo(const std::string& name_stack) const
{
  for (auto search = elements_list_.find(name_stack); search != elements_list_.end(); search++)
    {
      return search->second;
    }
  throw std::invalid_argument(name_stack+" not found");
}
std::vector<double> ReaderFORT21::getTimesStack(const std::string& name_stack) const
{
  std::vector<double> times;
  const ElementInfo& eleminfo = getElementInfo(name_stack);
  for (int i : eleminfo.index_times_)
    {
      times.push_back(Times_glob_[i]);
    }
  return times;
}
std::vector<float> ReaderFORT21::getTimesStack_float(const std::string& name_stack, const std::string& name_field) const
{
  const ElementInfo& eleminfo = getElementInfo(name_stack);
  if (eleminfo.stack_var_)
    {
      const auto& fieldinfo = getVarFieldInfo(eleminfo, name_field);
      return fieldinfo.times_;
    }
  std::vector<float> times;
  for (int i : eleminfo.index_times_)
    {
      times.push_back(float(Times_glob_[i]));
    }
  return times;
}
/*
std::vector<int> ReaderFORT21::getTimesIndexStack(const std::string& name_stack) const
{

  const ElementInfo& eleminfo = getElementInfo(name_stack);
  return  eleminfo.index_times_;
}
 */
std::vector<std::string> ReaderFORT21::getElementNames() const
{
  std::vector<std::string> res;
  for (auto& e : elements_list_)
    res.push_back(e.first);

  return res;
}
std::string ReaderFORT21::getElementType(const std::string& name_stack) const
{
  const ElementInfo& eleminfo = getElementInfo(name_stack);
  return eleminfo.type_;
}
const ReaderFORT21::FieldInfo& ReaderFORT21::getVarFieldInfo(const std::string& name_stack, const std::string& name_field) const
{
  return getVarFieldInfo(getElementInfo(name_stack), name_field);
}
const ReaderFORT21::FieldInfo& ReaderFORT21::getConstFieldInfo(const std::string& name_stack, const std::string& name_field) const
{
  return getConstFieldInfo(getElementInfo(name_stack), name_field);
}
const ReaderFORT21::FieldInfo& ReaderFORT21::getVarFieldInfo(const ElementInfo& eleminfo, const std::string& name_field) const
{
  for (auto f = eleminfo.VarFields_.find(name_field); f != eleminfo.VarFields_.end(); f++)
    {
      return f->second;
    }
  throw std::invalid_argument(name_field+ " No such var");
}
const ReaderFORT21::FieldInfo& ReaderFORT21::getConstFieldInfo(const ElementInfo& eleminfo, const std::string& name_field) const
{
  for (auto f = eleminfo.ConstFields_.find(name_field); f != eleminfo.ConstFields_.end(); f++)
    {
      return f->second;
    }
  throw std::invalid_argument(name_field + " No such const");
}
std::vector<std::string> ReaderFORT21::getVarFieldNames(const std::string& name_stack) const
{
  const ElementInfo& eleminfo = getElementInfo(name_stack);
  std::vector<std::string> res;
  for (auto& e : eleminfo.VarFields_)
    res.push_back(e.first);
  return res;
}
std::vector<std::string> ReaderFORT21::getConstFieldNames(const std::string& name_stack) const
{
  const ElementInfo& eleminfo = getElementInfo(name_stack);
  std::vector<std::string> res;
  for (auto& e : eleminfo.ConstFields_)
    res.push_back(e.first);
  return res;
}
file_pos_t ReaderFORT21::getOffsetVarField(const std::string& name_stack, const std::string& name_field, const int& id_time_field) const
{

  const ElementInfo& eleminfo = getElementInfo(name_stack);
  const FieldInfo& fieldinfo = getVarFieldInfo(eleminfo, name_field);
  if (eleminfo.stack_var_)
    return fieldinfo.offsets_[id_time_field];
  int id_time = eleminfo.index_times_.at(id_time_field);
  file_pos_t fp = offset_a_time_.at(id_time) + eleminfo.offset_rel_time_.at(id_time_field) + fieldinfo.offset_rel_stack_;
  return fp;
}

template <typename _TYPE_> void ReaderFORT21::getValuesVarFieldOnIndex(const std::string& name_stack, const std::string& name_field, std::vector<_TYPE_>& data, const int& id_index) const
{
  size_t ntimes = getTimesStack_float(name_stack,name_field).size();

  //std::vector<float> values(ntimes);
  data.resize(ntimes);

  // int i=0;
  for (unsigned int local_index_time=0; local_index_time< ntimes; local_index_time++)
    {
      std::vector<_TYPE_> datap;
      getValuesVarField(name_stack, name_field, datap, local_index_time);
      if ((id_index<0)||(id_index>=int(datap.size())))
        {
          std::stringstream ss;
          ss << "index too big for "+ name_stack <<  " " << name_field  << " " << id_index << "/" << datap.size() << std::endl;
          throw std::invalid_argument(ss.str());
        }
      //std::cerr<< "Celliiiii "<< cellId-1 << " / " << data.size()<<std::endl;
      data[local_index_time] = datap[id_index];
    }
}

template <typename _TYPE_> void ReaderFORT21::getValuesVarField(const std::string& name_stack, const std::string& name_field, std::vector<_TYPE_>& data, const int& id_time_field) const
{
  const ElementInfo& eleminfo = getElementInfo(name_stack);
  const FieldInfo& fieldinfo = getVarFieldInfo(eleminfo, name_field);
  data.resize(fieldinfo.size_*fieldinfo.nb_parts());
  /* int aSize = (myIsSimpleReal && aSizeOfReal == 4) ? 4 : 8;
   int typi = fieldinfo.type;
     ...

  if (aSize!=4) {
      throw std::invalid_argument("not implemented");
  }
   */
  if (fieldinfo.type_of_field()== T_INT)
    {
      if(!std::is_same<_TYPE_, int>::value)
        {
          std::vector<int> data2 ;
          getValuesVarField(name_stack,name_field, data2,id_time_field);
          size_t n= data2.size();
          data.resize(n);
          if(std::is_same<_TYPE_, float>::value)
            {
              float* datab = (float*)(&(data[0]));
              for (unsigned int c= 0; c<n; c++)
                {
                  datab[c]=float(data2[c]);
                }
              if (verbosity_>3)
                std::cerr<<"field int, data no conversion to float " << name_field<< " "<< id_time_field<<std::endl;
              return;
            }
          else
            throw std::invalid_argument("field int , data no");
        }
    }
  else if (fieldinfo.type_of_field()== T_REAL)
    {
      if(!std::is_same<_TYPE_, float>::value)
        {
          throw std::invalid_argument("field float , data no");
        }
    }
  else
    throw std::invalid_argument("field type not implemented");

  size_t aSizeOfRecordLength = fieldinfo.size() * sizeof(_TYPE_);
  file_pos_t fp = getOffsetVarField(name_stack,name_field,id_time_field) ;
  set_file_pos(myField.getFile(), fp, SEEK_SET);
  if (fread(data.data(), aSizeOfRecordLength, 1, myField.getFile()) != 1)
    throw std::invalid_argument("error reading");
  for (int p=2; p<=fieldinfo.nb_parts(); p++)
    {
      std::string nn = name_field+"_p"+std::to_string(p);
      file_pos_t fp2 = getOffsetVarField(name_stack,nn,id_time_field) ;
      set_file_pos(myField.getFile(), fp2, SEEK_SET);
      //std::cout<<p << " "<<(p-1)*fieldinfo.size()<< "iuuu  "<<data.data()[(p-1)*data.size()]<< std::endl;
      if (fread(data.data()+(p-1)*fieldinfo.size(), aSizeOfRecordLength, 1, myField.getFile()) != 1)
        throw std::invalid_argument("error reading");
    }
}

int ReaderFORT21::getIndexFromTime(const std::string& name_stack, const double& t, const std::string& method) const
{
  std::vector<double> times = getTimesStack(name_stack);
  int ntimes=int(times.size());
  if (method=="asC3")
    {
      int i ;
      for (i=0; i<ntimes; i++)
        if (times[i]>=t)
          break;
      if (i>=ntimes)
        {
          std::stringstream ss;
          ss << "Expected time between " << times.front() << " and " << times.back() << ".\n";
          std::cout<< ss.str();
          std::cout<<"We take the last time"<< std::endl;
          return ntimes-1;
        }
      assert(times[i]>=t);
      if (i>0)
        assert( times[i-1]<t);
      //std::cout<<t<< " real time "<<times[i]<<std::endl;
      return i;
    }
  if (t == -1)
    return ntimes-1;

  int i ;
  for (i=1; i<ntimes; i++)
    if (times[i]>t)
      break;
  i-=1;
  assert(t>=times[i]);
  if (i<ntimes-1)
    assert( t<times[i+1]);
  //std::cout<<t<< " real time "<<times[i]<<std::endl;
  return i;
}

int index_from_zs(const float& ZCURV, std::vector<float>& ZAX, const ReaderFORT21::LocalisationField& loc, const int& verbosity_ )
{

  int nf = int(ZAX.size());
  {
    if (ZCURV <= ZAX[0])
      {
        return 0;
      }
    else if (ZCURV >= ZAX[nf-1])
      {
        return  nf-1;
      }
    else
      {
        for (int I = 1; I < nf; I++)
          {
            if (ZCURV <= ZAX[I])
              {
                if ((ZCURV - ZAX[I - 1]) >= (ZAX[I] - ZCURV))
                  {
                    return  I;
                  }
                else
                  {
                    return   I - 1;
                  }
              }
          }
      }
  }

  throw std::invalid_argument("not implemented kkk");

}
int index_from_zv(const double& pos, std::vector<float>& zv, const ReaderFORT21::LocalisationField& loc, const int& verbosity_ )
{
  size_t nf = zv.size();
  if ((loc == ReaderFORT21::LocalisationField::L_ELEM)
      || (loc == ReaderFORT21::LocalisationField::L_ALTERNATE))
    {
      for (unsigned int i=0; i< nf-1; i++)
        if( (pos<zv[i+1])&& (pos>=zv[i]))
          {
            if (verbosity_>4)
              std::cerr<<"elem  found "<< pos << " between  " <<zv[i] <<" "<<zv[i+1]<< " index " << i<<std::endl;

            // proche face gche ?
            if (i>0)
              if ((pos-zv[i])< (zv[i+1]-zv[i])*1e-2)
                {
                  if ((zv[i]-zv[i-1]) < (zv[i+1]-zv[i]))
                    {
                      std::cerr <<pos <<" near left face , left cell smaller we take it" <<std::endl;
                      i=i-1;
                    }
                }
            if (i<nf-2)
              if ((zv[i+1]-pos)< (zv[i+1]-zv[i])*1e-2)
                {
                  if ((zv[i+2]-zv[i+1]) < (zv[i+1]-zv[i]))
                    {
                      std::cerr <<pos <<" near right face , rright cell smaller we take it" <<std::endl;
                      i=i+1;
                    }
                }


            return i;
          }
      std::cerr<<" warning z too big ???? " << pos << " "<<zv[nf-1]<<std::endl;
      return int(nf-2);
    }
  else
    {

      float xfn =(zv[0]+zv[1])*0.5f;
      if (pos<=xfn)
        return 0;
      for (unsigned int i=1; i< nf-1; i++)
        {
          float xfp =xfn;
          xfn =(zv[i]+zv[i+1])*0.5f;
          if( (pos<=xfn)&& (pos>xfp) )
            {
              if (verbosity_>4)
                std::cerr<<" found "<< pos << " between  " <<zv[i] <<" "<<zv[i+1]<< " index " << i<<std::endl;
              return i;
            }
        }
      if (pos<= zv[nf -1 ])
        {
          int i=int(nf-1);
          if (verbosity_>4)
            std::cerr<<" found last face "<< pos <<  " between  " <<xfn <<" "<<zv[i]<< " index " << i<<std::endl;
          return i;
        }
      std::cerr<<" warning z too big ???? " << pos << " "<<zv[nf-1]<<std::endl;
      return int(nf-1);
      //throw std::invalid_argument("not implemented kkk");
    }
}

int ReaderFORT21::getIndexFromPos(const std::string& name_stack, const std::string& name_field, const float& x, const float& y, const float& z) const
{
  const ElementInfo& eleminfo = getElementInfo(name_stack);
  auto fieldinfo =getVarFieldInfo(name_stack, name_field);
  auto loc= fieldinfo.localisation_of_field();
  if (loc == ReaderFORT21::LocalisationField::L_GLOBAL)
    return 0;
  std::vector<float> xv,yv,zv;
  if  ((eleminfo.type_ == "TROIDRAV")|| (eleminfo.type_ == "WAL3DRAV"))
    {

      if  ((eleminfo.type_ == "TROIDRAV"))
        {
          getValuesConstField(name_stack,  "ZV",  zv) ;
          getValuesConstField(name_stack,  "YV", yv);
          getValuesConstField(name_stack,  "XV",  xv) ;
        }
      else
        {
          try
            {
              getValuesConstField(name_stack,  "ZVW",  zv) ;
              getValuesConstField(name_stack,  "YVW", yv);
              getValuesConstField(name_stack,  "XVW",  xv) ;
            }
          catch(...)
            {
              throw std::invalid_argument("error in getIndexFromPos for wall");
            }
        }
      int nx=int(xv.size())-1;
      int ny=int(yv.size())-1;
      int nz=int(zv.size())-1;
      int k=   index_from_zv( z,  zv,  loc, verbosity_ );
      int j=   index_from_zv( y,  yv,  loc, verbosity_ );
      int i=   index_from_zv( x,  xv,  loc, verbosity_ );


      //std::cout<<i << " "<<j<<" "<<k<<std::endl;
      //int res= i+nx*(j+ny*k);
      int res= k+nz*(j+ny*i);
      if (res >=nx*ny*nz)
        throw std::invalid_argument("error in getIndexFromPos");
      return res;
    }
  std::cout<< "unkonwn conversion from pos to Index "<<eleminfo.type_<<std::endl;
  return 0;
}
int ReaderFORT21::getIndexFromPos(const std::string& name_stack, const std::string& name_field, const float& pos) const
{
  const ElementInfo& eleminfo = getElementInfo(name_stack);
  auto loc =getVarFieldInfo(name_stack, name_field).localisation_of_field();
  if (loc == ReaderFORT21::LocalisationField::L_GLOBAL)
    return 0;
  if  ((eleminfo.type_ == "AXIALRAV")|| (eleminfo.type_ == "WALAXRAV"))
    {
      // const FieldInfo& fieldinfo = getConstFieldInfo(eleminfo, "ZV");
      std::vector<float> zs;
      getInterpolatedValuesVarPos(name_stack,name_field,zs);

      return index_from_zs( pos,  zs,  loc, verbosity_ );
      std::vector<float> zv;
      if (eleminfo.type_ == "AXIALRAV")
        getValuesConstField(name_stack,  "ZV",  zv) ;
      else
        getValuesConstField(name_stack,  "ZVW",  zv) ;


      return  index_from_zv( pos,  zv,  loc, verbosity_ );

    }
  std::cout<< "unkonwn conversion from pos to Index "<<eleminfo.type_<<std::endl;
  return 0;
}
void ReaderFORT21::getInterpolatedValuesVarPos(const std::string& name_stack, const std::string& name_field, std::vector<float>& data) const
{

  const ElementInfo& eleminfo = getElementInfo(name_stack);
  auto loc =getVarFieldInfo(name_stack, name_field).localisation_of_field();
  if  ((eleminfo.type_ == "AXIALRAV")|| (eleminfo.type_ == "WALAXRAV"))
    {
      if ((loc == ReaderFORT21::LocalisationField::L_GLOBAL)|| (loc == ReaderFORT21::LocalisationField::L_ELEM))
        {
          if (eleminfo.type_ == "AXIALRAV")
            getValuesConstField(name_stack,  "ZS",  data) ;
          else
            getValuesConstField(name_stack,  "ZSW",  data) ;
        }
      else
        {
          if (loc == ReaderFORT21::LocalisationField::L_ALTERNATE)
            {
              getValuesConstField(name_stack,  "EXCH_ZSW",  data) ;

            }
          else
            {
              getValuesConstField(name_stack,  "ZV",  data) ;
            }
        }
    }
  else
    {

      int nbcell=1;
      if ((loc == ReaderFORT21::LocalisationField::L_GLOBAL)|| (loc == ReaderFORT21::LocalisationField::L_ELEM))
        {
          data.resize(nbcell);
        }
      else
        data.resize(nbcell+1);
      std::iota(data.begin(),data.end(),0.0f);
    }
}

template <typename _TYPE_>
void ReaderFORT21::getInterpolatedValuesVarField(const std::string& name_stack, const std::string& name_field, std::vector<_TYPE_>& data, const int& global_id_time) const
{

  std::vector<double> stak_times = getTimesStack(name_stack);
  if (global_id_time >= int(Times_glob_.size()))
    throw std::invalid_argument("invalid index time in getInterpolatedValuesVarField "+ name_field);
  double global_time = Times_glob_[global_id_time];
  std::vector<double>::iterator it = std::find_if(stak_times.begin(),stak_times.end(),[global_time](double b)
  {
    return global_time<=b;
  });
  int index = int( std::distance(stak_times.begin(), it));
  getValuesVarField(name_stack, name_field,data, index);
}

template <typename _TYPE_> void ReaderFORT21::getValuesConstField(const std::string& name_stack, const std::string& name_field, std::vector<_TYPE_>& data) const
{
  const ElementInfo& eleminfo = getElementInfo(name_stack);
  const FieldInfo& fieldinfo = getConstFieldInfo(eleminfo, name_field);
  if (fieldinfo.nb_parts()!=1)
    throw std::invalid_argument("nb_parts!=1 not implemented for const field "+ name_field);
  data.resize(fieldinfo.size_);
  file_pos_t fp = fieldinfo.offsets_.back();
  /* int aSize = (myIsSimpleReal && aSizeOfReal == 4) ? 4 : 8;

  if (aSize!=4) {
      throw std::invalid_argument("not implemented");
  }
   */
  if (fieldinfo.type_of_field()== T_INT)
    {
      if(!std::is_same<_TYPE_, int>::value)
        {
          throw std::invalid_argument("field int , data no");
        }
    }
  else if (fieldinfo.type_of_field()== T_REAL)
    {
      if(!std::is_same<_TYPE_, float>::value)
        {
          throw std::invalid_argument("field float , data no");
        }
    }
  else
    throw std::invalid_argument("field type not implemented");

  set_file_pos(myField.getFile(), fp, SEEK_SET);
  size_t aSizeOfRecordLength = data.size() * sizeof(_TYPE_);
  if (fread(data.data(), aSizeOfRecordLength, 1, myField.getFile()) != 1)
    throw std::invalid_argument("error reading");
}

//=======================================================================
// function : readRecord
// purpose  :
//=======================================================================

bool ReaderFORT21::readRecord()
{
  if (!readHeader())
    return false;

  if (!readDescr())
    return false;
  //  unsigned int nbVarias = 0;
  while (readVaria());
  //  nbVarias++;

  return myStatus == Done;
}

//=======================================================================
// function : readHeader
// purpose  :
//=======================================================================

bool ReaderFORT21::readHeader()
{
  // read first field - TYPE
  if (myField.size() == 0)
    {
      if (!myField.read(24))
        {
          myStatus = Fail;
          return false;
        }
    }

  char* aData = myField.getData();
  if (strncmp(aData, "TYPE$$$$", 8) != 0)
    {
      // std::cout<<aData<<std::endl;
      myStatus = Fail;
      return false;
    }

  // get real variable type (SIMPLE or DOUBLE)
  aData += 16;
  aData[6] = 0;
  if (strcmp(aData, "SIMPLE") == 0)
    {
      myIsSimpleReal = true;
      myRealSize = 4;
    }
  else if (strcmp(aData, "DOUBLE") == 0)
    {
      myIsSimpleReal = false;
      myRealSize = 8;
    }
  else
    {
      myStatus = Fail;
      return false;
    }

  // skip a next field
  if (!myField.read(24))
    {
      myStatus = Fail;
      return false;
    }

  // read IDENT field
  if (!myField.read(184))
    {
      myStatus = Fail;
      return false;
    }

  // get date of calculation
  aData = myField.getData() + 8;
  char aDate[18];
  strncpy(aDate, aData, 8);

  // get CATHARE version
  char aBuf[100];
  strncpy(aBuf, aData + 8, 80);
  TruncTailWS(aBuf, 80);

  // IsC3()
  std::string aStr(aBuf);
  myIsC3 = find(aStr, "c3") >= 0;

  // get title of calculation
  strncpy(aBuf, aData + 88, 80);
  TruncTailWS(aBuf, 80);

  // get time of calculation
  strncpy(aDate + 9, aData + 168, 8);
  aDate[8] = ' ';
  aDate[17] = 0;
  // change spaces to '0'
  char* p = aDate;
  while (*p != 0)
    {
      if (*p == ' ')
        *p = '0';
      p++;
    }
  aDate[8] = ' ';

  myField.clear();
  return true;
}

//=======================================================================
// function : readDescr
// purpose  :
//=======================================================================

bool ReaderFORT21::readDescr()
{
  // read DESCR field
  if (myField.size() == 0)
    {
      if (!myField.read(8))
        {
          myStatus = Fail;
          return false;
        }
    }
  char* aData = myField.getData();
  if (strncmp(aData, "DESCR$$$", 8) != 0)
    {
      myStatus = Fail;
      return false;
    }
#ifdef DEB_PRINT
  cout << "Objects description:" << endl;
#endif
  myField.clear();

  // skip DESSTACK blocks
  // while(readDesStack(true))
  while (readDesStack(false))
    {
      // to do: exit from loop
    }

  return myStatus == Done;
}

//=======================================================================
// function : readVaria
// purpose  :
//=======================================================================

bool ReaderFORT21::readVaria()
{
  // read VARIA field
  if (myField.size() == 0)
    {
      if (!myField.read())
        return false;
    }

  char* aData = myField.getData();
  if (strncmp(aData, "VARIA$$$", 8) != 0)
    return false;

  // read timestamp
  if (!myField.read(16))
    {
      myStatus = Fail;
      return false;
    }
  // double aTime = *(double*) (myField.getData() + 8);

  const bool isBigEndian = IsBigEndian(myField.getPlatform()) ? true : false;

  tbuff aBuff;
  memcpy(&aBuff, myField.getData() + 8, 8);
  double aTime = ToReal(aBuff, 8, isBigEndian);
  // int nb_fields_read_sa=nb_fields_read;

  // try first DESSTACK block to check if timestamp is not empty
  if (!myField.read(8))
    return true;

  aData = myField.getData();
  if (strncmp(aData, "DESSTACK", 8) != 0)
    return true;

  // save timestamp
  Times_glob_.push_back(aTime);
  file_pos_t aPos0 = get_file_pos(myField.getFile());
  offset_a_time_.push_back(aPos0);

  // read all DESSTACK blocks for the current timestamp
  while (readDesStack(false))
    {
    }

  return true;
}

//=======================================================================
// function : readDesStack
// purpose  :
//=======================================================================
bool ReaderFORT21::readDesStack(bool theSkip)
{
  int aNameSize = myIsC3 ? 16 : 8;

  // read DESSTACK field
  if (myField.size() == 0)
    {
      if (!myField.read())
        return false;
    }

  int aSizeOfRecordLength = getSizeOfRecordLength(myField.getPlatform());
  const int aSizeOfLong = getSizeOfLong(myField.getPlatform());
  const int aSizeOfReal = getSizeOfReal(myField.getPlatform());

  const bool isBigEndian = IsBigEndian(myField.getPlatform()) ? true : false;

  if (!myIsSimpleReal && aSizeOfReal > 4)  // not supported
    return false;

  char* aData = myField.getData();
  if (strncmp(aData, "DESSTACK", 8) != 0)
    {
      return false;
    }

  file_pos_t aPos0 = get_file_pos(myField.getFile());
  // read name and type of object
  // Record length = Name(8 or 16) + Type(8 or 16) + 4*LongValue
  if (!myField.read(aNameSize * 2 + aSizeOfLong * 4))
    {
      myStatus = Fail;
      return false;
    }
  file_pos_t decpos = get_file_pos(myField.getFile()) - aPos0;

  aData = myField.getData();
  char aName[MAX_NAME_SIZE + 1];
  char aTypeC[MAX_NAME_SIZE + 1];

  strncpy(aName, aData, aNameSize);
  TruncTailWS(aName, aNameSize);
  std::string anAsciiName(aName);
  // anAsciiName.uppercase();
  std::transform(anAsciiName.begin(), anAsciiName.end(), anAsciiName.begin(), ::toupper);
  bool constPart = true;

  /*    if (elements_list_.count(anAsciiName)==0)
      {
          elements_list_[anAsciiName]=ElementInfo();
      }
   */
  bool stack_var=false;
  if ((anAsciiName== "XUSER")|| (anAsciiName== "GLOBAL"))
    {
      stack_var=true;
    }
  ElementInfo& eleminfo = elements_list_[anAsciiName];
  int current_index_time = int(Times_glob_.size()) - 1;
  file_pos_t offsetelem = -1;
  if (current_index_time >= 0)
    {
      constPart = false;
      // stack info already read ?
      eleminfo.index_times_.push_back(current_index_time);
      offsetelem = aPos0;
      eleminfo.offset_rel_time_.push_back(aPos0 - offset_a_time_[current_index_time]);
      /*
      for (auto& search=element_sizeblock_.find(anAsciiName); search != element_sizeblock_.end();search++)
      {
      set_file_pos(myField.getFile() , search->second-(decpos),SEEK_CUR);
        //file_pos_t aPos1 = get_file_pos( myField.getFile() );
       */
      if (eleminfo.sizeblock_ != -1)
        {
          set_file_pos(myField.getFile(), eleminfo.sizeblock_ - (decpos), SEEK_CUR);
          myField.clear();
          return true;
        }
    }

  strncpy(aTypeC, aData + aNameSize, aNameSize);
  TruncTailWS(aTypeC, aNameSize);
  std::string aType(aTypeC);
  eleminfo.type_ = aType;
  eleminfo.stack_var_=stack_var;
  /*
    int aTypeId = 0;
    hid_t aGrpId = getObjGrp( anAsciiName, aType );
    if ( aGrpId <= 0 )
      return false;
   */
  // numeration from one
  // int aTimeStep =  myTimes.size() > 0 ? myTimes.size() : 1;

  // open or craete group

  // get numbers of variables of each type among
  // pointers, integers, reals and characters
  int aNbVar[4], nbVars;
  int i;
  for (i = 0; i < 4; i++)
    {
      tbuff aBuff;
      memcpy(&aBuff, aData + aNameSize * 2 + i * aSizeOfLong, 8);
      aNbVar[i] = ToInt(aBuff, aSizeOfLong, isBigEndian);
    }

  nbVars = aNbVar[0] + aNbVar[1] + aNbVar[2] + aNbVar[3];

  // read dimensions of variables
  if (!myField.read(8 + nbVars * aSizeOfLong))
    {
      myStatus = Fail;
      return false;
    }

  aData = myField.getData();

  int aDimVar[MAX_NUM_VARS];
  for (i = 0; i < nbVars; i++)
    {
      tbuff aBuff;
      memcpy(&aBuff, aData + 8 + i * aSizeOfLong, 8);
      aDimVar[i + 1] = ToInt(aBuff, aSizeOfLong, isBigEndian);
    }

  char aQNameC[MAX_NAME_SIZE + 1];
  // read variables
  int ct;
  long posdata = -1;
  for (i = 1, ct = 0; ct < 4; ct++)
    {
      // std::string loc ( "UNKONWN");
      LocalisationField locfield = LocalisationField::L_UNDEF;
      // std::string add("");
      CathareType catType = (CathareType)ct;

      for (int n = aNbVar[catType]; n > 0; n--, i++)
        {
          FieldInfo fieldinfo;
          posdata = ftell(myField.getFile()) + aNameSize + aSizeOfRecordLength;
          //      std::cout<<"aPos0 "<< ftell( myField.getFile())+aNameSize+aSizeOfRecordLength<< std::endl;
          if (!myField.read())
            {
              myStatus = Fail;
              return false;
            }

          if (theSkip)
            continue;
          /*
          // provisoire pour guithare slt
          if ( catType != T_INT && catType != T_REAL)
          continue;
           */


          aData = myField.getData();
          strncpy(aQNameC, aData, aNameSize);
          TruncTailWS(aQNameC, aNameSize);
          std::string aQName(aQNameC);
          fieldinfo.type_of_field_ = catType;
          if (catType==T_PTR)
            fieldinfo.type_of_field_ = T_INT;

          int nbPnt = aDimVar[i];
          if (nbPnt==1)
            {
              if (aQName=="GLOBAL")
                {
                  // loc=aQName;
                  locfield=LocalisationField::L_GLOBAL;
                  continue;
                }
              if ((aQName=="SCALAR_ALTERNATE"))
                {
                  //  loc=aQName;
                  locfield=LocalisationField::L_ALTERNATE;
                  continue;
                }
              if ((aQName=="SCALAR")||(aQName=="SUBVOL")||(aQName=="ZS")||(aQName=="ZSW"))
                {
                  //  loc=aQName;
                  locfield=LocalisationField::L_ELEM;
                  continue;
                }
              // cas particulier ZV marqueur et variable !!!!
              if ((aQName=="ZV"))
                {
                  //loc=aQName;
                  locfield=LocalisationField::L_DUAL;
                  continue;
                }
              if ((aQName=="VECTX")||(aQName=="VECTY")||(aQName=="VECTZ"))
                {
                  //loc=aQName;
                  locfield=LocalisationField::L_DUAL;
                  continue;
                }
              if (aQName=="SUBVOLINF")
                {
                  //add="_INF";
                  // loc=aQName;
                  locfield=LocalisationField::L_ELEM;
                  continue;
                }
              if (aQName=="SUBVOLSUP")
                {
                  //add="_SUP";
                  //  loc=aQName;
                  locfield=LocalisationField::L_ELEM;
                  continue;
                }
            }
          /*
          if (fieldinfo.type_of_field_==T_INT)
          {
          aQName+="_INT";
          }
           */
          //aQName+=add;
          /*
          if (nbPnt == 0)
          throw std::invalid_argument(aQName+" is size null ?");
           */
          fieldinfo.size_ = nbPnt;
          fieldinfo.nb_parts_=1;
          fieldinfo.localisation_of_field_= locfield;
          if (constPart)
            {
              fieldinfo.offsets_.push_back(posdata) ;
              if (eleminfo.ConstFields_.count(aQName)>0)
                {
                  std::cout << aQName<< " already a const field of "<<anAsciiName << std::endl;
                  //throw;
                }
              eleminfo.ConstFields_[aQName] = fieldinfo;
            }
          else
            {
              if (stack_var)
                {
                  if (eleminfo.VarFields_.count(aQName)>0)
                    {
                      auto& fieldinfom = eleminfo.VarFields_[aQName];
                      fieldinfom.offsets_.push_back(posdata);
                      fieldinfom.times_.push_back(float(Times_glob_.back()));
                      //std::cout<<aQName <<" iiii "<< Times_glob_.back()<<" "<<fieldinfom.times_.size()<<std::endl;
                    }
                  else
                    {
                      fieldinfo.offsets_.push_back(posdata);
                      fieldinfo.times_.push_back(float(Times_glob_.back()));
                      eleminfo.VarFields_[aQName] = fieldinfo;
                    }
                }
              else
                {
                  fieldinfo.offset_rel_stack_ = posdata - offsetelem;
                  if (eleminfo.VarFields_.count(aQName)>0)
                    {

                      int nbpartsold=eleminfo.VarFields_[aQName].nb_parts_;
                      if (nbpartsold==1)
                        {
                          // on ajoute  p1
                          std::string aQNamep1= aQName + std::string("_p1");
                          eleminfo.VarFields_[aQNamep1]=eleminfo.VarFields_[aQName];

                        }
                      eleminfo.VarFields_[aQName].nb_parts_++;

                      aQName+= std::string("_p")+std::to_string(nbpartsold+1);
                      if (verbosity_ > 4)
                        std::cout<< aQName<< " already a var field of "<<anAsciiName << std::endl;
                    }
                  eleminfo.VarFields_[aQName] = fieldinfo;
                }
            }
        }
    }
  myField.clear();
  file_pos_t aPos1 = get_file_pos(myField.getFile());
  if ((!constPart) && (!stack_var))
    {

      eleminfo.sizeblock_ = aPos1 - aPos0;
      if (verbosity_ > 5)
        std::cout << anAsciiName << " taille bloc " << eleminfo.sizeblock_ << std::endl;
    }

  return true;
}

//=======================================================================
// function : Field::Read
// purpose  :
//=======================================================================

bool ReaderFORT21::Field::read(int theSize)
{
  if (myFile == 0)
    return false;
  mySize = 0;
  // read size
  tbuff aBuff;

  int aSizeOfRecordLength = getSizeOfRecordLength(myPlatform);
  const bool isBigEndian = IsBigEndian(myPlatform) ? true : false;

  if (fread(&aBuff, aSizeOfRecordLength, 1, myFile) != 1)
    return false;

  myFPos += aSizeOfRecordLength;

  int aSize = ToInt(aBuff, aSizeOfRecordLength, isBigEndian);
  // size_t aSize = ToInt( aBuff, mySizeOfRecordLength, myIsBigEndian );
  // additional safety control against garbage data,
  // if the size is known beforehand
  if ((theSize > 0 && aSize != theSize) || aSize < 0 || aSize > myMaxSizeInt)
    {
      std::cout << " KO " << theSize << std::endl;
      return false;
    }
  if (aSize > myCapacity)
    {
      // resize myData
      if (myData != 0)
        delete[] myData;
      myCapacity = 0;
      myData = new char[aSize + 4];
      if (myData == 0)
        return false;
      myCapacity = aSize;
      myData[myCapacity] = 0;
    }
  if (int(fread(myData, 1, aSize, myFile)) != aSize)
    return false;

  myFPos += aSize;

  if (fread(&aBuff, aSizeOfRecordLength, 1, myFile) != 1)
    return false;

  myFPos += aSizeOfRecordLength;

  mySize = ToInt(aBuff, aSizeOfRecordLength, isBigEndian);

  // fread(&mySize, mySizeOfLength, 1, myFile);
  // control check (hz may be should be removed)
  if (aSize != mySize)
    {
      std::cout << " KO2 " << theSize << std::endl;
      mySize = 0;
      return false;
    }
  // std::cout<<" OK "<<theSize<<std::endl;
  return true;
}

ReaderFORT21::BasicMesh::BasicMesh()
  : type_mesh_(MESH_Unknown)
  , mesh_dim_(0)
  , space_dim_(0)
  , nbnodes_(0)
  , nodes_per_elem_(0)
  , nb_elems_(0)
  , connectivity_(0)
  , coords_(0)
{
}

ReaderFORT21::BasicMesh::~BasicMesh()
{
  clear();
}

void ReaderFORT21::BasicMesh::clear()
{
  connectivity_.resize(0);
  coords_.resize(0);
}

void ReaderFORT21::BasicMesh::save(std::ostream& os) const
{
  //os << std::setprecision(12);
  os << (int)type_mesh_ << std::endl;
  os << mesh_dim_ << std::endl;
  os << space_dim_ << std::endl;
  os << nbnodes_ << std::endl;
  os << nodes_per_elem_ << std::endl;
  os << nb_elems_ << std::endl;

  for (int i = 0; i < nb_elems_; i++)
    {
      for (int j = 0; j < nodes_per_elem_; j++)
        os << " " << connectivity_[i * nodes_per_elem_ + j];
      os << std::endl;
    }

  for (int i = 0; i < nbnodes_; i++)
    {
      for (int j = 0; j < space_dim_; j++)
        os << " " << coords_[i * space_dim_ + j];
      os << std::endl;
    }
}

void ReaderFORT21::BasicMesh::restore(std::istream& in)
{
  std::string name;
  in >> name;
  int int_type_mesh;
  in >> int_type_mesh;
  type_mesh_=(MeshType)int_type_mesh;
  in >> mesh_dim_;
  in >> space_dim_;
  in >> nbnodes_;
  in >> nodes_per_elem_;
  in >> nb_elems_;

  connectivity_.resize(nodes_per_elem_ * nb_elems_);
  for (int i = 0; i < nb_elems_; i++)
    {
      for (int j = 0; j < nodes_per_elem_; j++)
        in >> connectivity_[i * nodes_per_elem_ + j];
    }
  coords_.resize(nbnodes_ * space_dim_);
  for (int i = 0; i < nbnodes_; i++)
    {
      for (int j = 0; j < space_dim_; j++)
        in >> coords_[i * space_dim_ + j];
    }
}

void ReaderFORT21::BasicMesh::dummy_geom()
{
  type_mesh_ = MESH_Unknown;
  mesh_dim_ = 2;
  space_dim_ = 2;
  nbnodes_ = 4;
  nodes_per_elem_ = 4;
  nb_elems_ = 1;
  connectivity_.resize(nb_elems_ * nodes_per_elem_);
  connectivity_[0] = 0;
  connectivity_[1] = 1;
  connectivity_[2] = 2;
  connectivity_[3] = 3;
  coords_.resize(nbnodes_ * space_dim_);
  coords_[0] = 0;
  coords_[1] = 0;
  coords_[2] = 1;
  coords_[3] = 0;
  coords_[4] = 1;
  coords_[5] = 1;
  coords_[6] = 0;
  coords_[7] = 1;
}

/// @brief Return the image of a point at (x0 + dx, y0 + dy) due to a rotation expressed as \p cos and \p sin around the
/// center at (x0, y0)
void rotate(const double& dx, const double& dy, const double& x0, const double& y0,
            const double& cos, const double& sin, double& xn, double& yn)
{
  xn = x0 + cos * dx + sin * dy;
  yn = y0 - sin * dx + cos * dy;
}
ReaderFORT21::BasicMesh ReaderFORT21::getMeshStack(const std::string& name_stack) const
{
  const ElementInfo& eleminfo = getElementInfo(name_stack);
  ReaderFORT21::BasicMesh mesh;
#define nodes_(i,j) mesh.coords_[(i)*mesh.space_dim_+j]
#define elements_(i,j) mesh.connectivity_[(i)*mesh.nodes_per_elem_ + j]
  if (eleminfo.type_ == "AXIALRAV")
    {
      const FieldInfo& fieldinfo = getConstFieldInfo(eleminfo, "ZV");
      mesh.nb_elems_ = fieldinfo.size_ - 1;
      std::vector<float> data;
      getValuesConstField(name_stack,  "ZV",  data) ;
      // for (float x : data)
      // std::cout<<"ZV " << x<<std::endl;
      std::vector<float>           gs,spf,zv;
      float gz;
      getValuesConstField(name_stack,  "GSREF",gs);
      getValuesConstField(name_stack,  "SPF",spf);
      getValuesConstField(name_stack,  "ZV",zv);
      if (eleminfo.ConstFields_.count("GZ")>0)
        {
          std::vector<float> tmp;
          getValuesConstField(name_stack,  "GZ",tmp);
          gz=std::abs(tmp[0]);
        }
      else
        gz=9.81f;


      int NT=int (zv.size());
      int nbelem=NT-1;
      mesh.nb_elems_=nbelem;
      mesh.space_dim_ = 2;
      mesh.mesh_dim_=2;
      mesh.nbnodes_=4*nbelem;
      mesh.coords_.resize(4*nbelem*2);
      mesh.connectivity_.resize(nbelem*4);
      mesh.space_dim_ =2;
      mesh.nodes_per_elem_ = 4;
      mesh.type_mesh_=MESH_Polygone;
      double xold = 0;
      double yold = zv[0];  // has to be 0.0
      int n0=0;
      for (int ele = 0; ele < nbelem; ele++)
        {
          const double dx0 = std::sqrt(spf[ele] / 3.1415926f);
          const double dx1 = std::sqrt(spf[ele + 1] / 3.1415926f);
          // std::cout << ele << " ZV " << ZV(ele) << " " << COSIN(ele) << " " << dx0 << " " << dx1 << std::endl;
          const double cos = std::max(std::min(double(gs[ele]/gz),1.),-1.);
          const double sin = std::sqrt(1. - cos * cos);
          //
          // for each cell:
          //         ^ z
          //  (2)    |(B)
          //    +----+----+ (1)
          //    |    .    |        1, 2, 3, 4: cell's contour
          //    |    |    |        A: "old" which is the center of rotation
          //   |     .     |       B: "old2" created after rotation, middle between (1) and (2), will become "old"
          //   |     |(A)  |
          //   +-----+-----+ (0)
          //  (3)    |
          //

          rotate(dx0, 0., xold, yold, cos, sin,nodes_(n0+0,0),nodes_(n0+0,1));
          rotate(dx1, zv[ele+1] - zv[ele], xold, yold, cos, sin,nodes_(n0+1,0),nodes_(n0+1,1));

          rotate(-dx1, zv[ele+1] - zv[ele], xold, yold, cos, sin,nodes_(n0+2,0),nodes_(n0+2,1));
          rotate(-dx0, 0., xold, yold, cos, sin,nodes_(n0+3,0),nodes_(n0+3,1));
          // le nouveau old est entre 1 et 2
          const double xold2 = (nodes_(n0+1,0)+nodes_(n0+2,0)) * 0.5f;
          const double yold2 = (nodes_(n0+1,1)+nodes_(n0+2,1)) * 0.5f;

          {
            // control lenght of the cell (curvilinear axis): after rotation vs Cathare original data
            const double ds_x = xold2 - xold;
            const double ds_y = yold2 - yold;
            if (std::abs(ds_x * ds_x + ds_y * ds_y - (zv[ele+1] - zv[ele]) * (zv[ele+1] - zv[ele])) > 1e-5)
              {
                std::cout << "Soucis coherence..." << ds_x * ds_x + ds_y * ds_y << " " << (zv[ele+1] - zv[ele]) * (zv[ele+1] - zv[ele])
                          << " "<<std::abs(ds_x * ds_x + ds_y * ds_y - (zv[ele+1] - zv[ele]) * (zv[ele+1] - zv[ele]))
                          << std::endl;
                //abort();
              }
          }

          xold = xold2;
          yold = yold2;


          elements_(ele,0)=n0;
          elements_(ele,1)=n0+1;
          elements_(ele,2)=n0+2;
          elements_(ele,3)=n0+3;
          n0+=4;
        }
    }
  else if (eleminfo.type_ == "VOLUMRAV")
    {
      std::vector<float> cotes,diam;
      getValuesConstField(name_stack,  "COTE",cotes);
      getValuesConstField(name_stack,  "DIAMETER",diam);
      int NT = int(diam.size());
      mesh.nb_elems_=1;
      mesh.space_dim_ = 2;
      mesh.mesh_dim_=2;
      mesh.nbnodes_=2*NT;
      mesh.coords_.resize(2*NT*2,-1000.);
      mesh.connectivity_.resize(1*2*NT,-1);
      mesh.space_dim_ =2;
      mesh.nodes_per_elem_ = 2*NT;
      mesh.type_mesh_=MESH_Polygone;
      for (int c = 0; c < NT; c++)
        {
          nodes_(c,0)= diam[c] * 0.5f;
          nodes_(c,1)= cotes[c];
          nodes_(2*NT- c - 1,0) = -nodes_(c,0);
          nodes_(2*NT- c - 1,1) = nodes_(c,1);
        }
      for (int c = 0; c < 2*NT; c++)
        elements_(0,c)=c;
    }
  else if  ((eleminfo.type_ == "WALVORAV")|| (eleminfo.type_ == "WALAXRAV"))
    {

      std::vector<float>  R;
      std::vector<std::vector<float>> ri;
      int nr=1;
      while(1)
        {
          std::string ris("RAYON");
          ris+=std::to_string(nr);
          try
            {
              getValuesConstField(name_stack,  ris,R);
              ri.push_back(R);
              nr++;

            }
          catch(...)
            {
              break;
            }
        }
      nr--;
      std::vector<int> iwhyd;
      getValuesConstField(name_stack,  "IWHYD",iwhyd);
      int ncellhyd=int(iwhyd.size());
      std::vector<float> zvw;
      if  (eleminfo.type_ == "WALVORAV")
        {
          std::vector<float> zpmin,zpmax;
          getValuesConstField(name_stack,  "ZPMIN",zpmin);
          getValuesConstField(name_stack,  "ZPMAX",zpmax);
          zvw.resize(ncellhyd+1);
          for (int i=0; i<ncellhyd+1; i++) zvw[i]=zpmin[0]+float(i)*(zpmax[0]-zpmin[0])/float(ncellhyd+1);
        }
      else
        {
          getValuesConstField(name_stack,  "ZVW",zvw);
        }
      /*

      int NT=nr-1;
      mesh.nb_elems_=NT;
      mesh.space_dim_ = 2;
      mesh.mesh_dim_=2;
      mesh.nbnodes_=4*NT;
      mesh.coords_.resize(4*NT*2,-1000.);
      mesh.connectivity_.resize(4*NT,-1);
      mesh.nodes_per_elem_ = 4;
      mesh.type_mesh_=MESH_Polygone;
      for (int c = 0; c < NT; c++)
      {
        nodes_(4*c,0)= ri[c+1][0];
        nodes_(4*c,1)= zpmin[0];
        nodes_(4*c+1,0)= ri[c+1][0];
        nodes_(4*c+1,1)= zpmax[0];
        nodes_(4*c+2,0)= ri[c][0];
        nodes_(4*c+2,1)= zpmax[0];
        nodes_(4*c+3,0)= ri[c][0];
        nodes_(4*c+3,1)= zpmin[0];
        for (int i=0; i<4; i++)
          elements_(c,i)=4*c+i;
      }
      }
      else
      {
       */
      {
        int NT=(nr-1)*ncellhyd;
        mesh.nb_elems_=NT;
        mesh.space_dim_ = 2;
        mesh.mesh_dim_=2;
        mesh.nbnodes_=4*NT;
        mesh.coords_.resize(4*NT*2,-1000.);
        mesh.connectivity_.resize(4*NT,-1);
        mesh.nodes_per_elem_ = 4;
        mesh.type_mesh_=MESH_Polygone;
        for (int m = 0; m < ncellhyd; m++)
          for (int r = 0; r < nr-1; r++)
            {
              int m2=iwhyd[m]-1;
              m2=m;
              double sens=1;
              //if (m>1) if (iwhyd[m]==iwhyd[m-1]) sens=-1;
              int c=(r*ncellhyd+m);
              nodes_(4*c,0)= sens*ri[r+1][m];
              nodes_(4*c,1)= zvw[m2];
              nodes_(4*c+1,0)= nodes_(4*c,0);
              nodes_(4*c+1,1)= zvw[m2+1];
              nodes_(4*c+2,0)= sens*ri[r][m];
              nodes_(4*c+2,1)= nodes_(4*c+1,1);
              nodes_(4*c+3,0)= nodes_(4*c+2,0);
              nodes_(4*c+3,1)= nodes_(4*c,1);
              for (int i=0; i<4; i++)
                elements_(c,i)=4*c+i;
            }
      }

    }
  else if ((eleminfo.type_ == "TROIDRAV")
           || (eleminfo.type_ == "WAL3DRAV"))
    {
      std::vector<float> tabx,taby,tabz;
      if  (eleminfo.type_ == "WAL3DRAV")
        {
          getValuesConstField(name_stack,  "XS",tabx);
          getValuesConstField(name_stack,  "YS",taby);
          getValuesConstField(name_stack,  "ZS",tabz);
          int nx=int(tabx.size());
          int ny=int(taby.size());
          int nz=int(tabz.size());
          tabx.resize(nx+1);
          tabx[nx]=tabx[nx-1]+1;
          taby.resize(ny+1);
          taby[ny]=taby[ny-1]+1;
          tabz.resize(nz+1);
          tabz[nz]=tabz[nz-1]+1;
        }
      else
        {
          getValuesConstField(name_stack,  "XV",tabx);
          getValuesConstField(name_stack,  "YV",taby);
          getValuesConstField(name_stack,  "ZV",tabz);
        }
      int nx=int(tabx.size())-1;
      int ny=int(taby.size())-1;
      int nz=int(tabz.size())-1;
      unsigned int NT = (nx*ny*nz);
      mesh.nb_elems_=NT;
      mesh.space_dim_ = 3;
      mesh.mesh_dim_=3;
      mesh.nbnodes_=((nx+1)*(ny+1)*(nz+1));

      mesh.coords_.resize(mesh.nbnodes_*3,-1000.);
      mesh.connectivity_.resize(8*NT,-1);
      mesh.nodes_per_elem_ = 8;
      mesh.type_mesh_=MESH_Hexa;
      for (int cx=0; cx<nx+1; cx++)
        for (int cy=0; cy<ny+1; cy++)
          for (int cz=0; cz<nz+1; cz++)
            {
              //	    int elem=cx+cy*nx+ nx*ny*cz;
              int som=cz+(nz+1)*(cy+(ny+1)*cx);

              nodes_(som,0)= tabx[cx];
              nodes_(som,1)= taby[cy];
              nodes_(som,2)= tabz[cz];
            }
      for (int cx=0; cx<nx; cx++)
        for (int cy=0; cy<ny; cy++)
          for (int cz=0; cz<nz; cz++)
            {
              int elem=cz+nz*(cy+ny*cx);

              elements_(elem,0)=cz+(nz+1)*(cy+(ny+1)*cx);
              elements_(elem,1)=cz+(nz+1)*(cy+(ny+1)*(cx+1));
              elements_(elem,2)=cz+(nz+1)*((cy+1)+(ny+1)*(cx+1));
              elements_(elem,3)=cz+(nz+1)*((cy+1)+(ny+1)*(cx));

              for (int c=0; c<4; c++)
                elements_(elem,c+4)=elements_(elem,c)+1;
            }
    }
  else
    {
      if (verbosity_ > 5)
        std::cout<< eleminfo.type_ << " not implemented"<<std::endl;
      mesh.dummy_geom();
    }
  return mesh;
}

template void ReaderFORT21::getInterpolatedValuesVarField(const std::string& name_stack, const std::string& name_field, std::vector<float>& data, const int& id_time_field) const;
template void ReaderFORT21::getInterpolatedValuesVarField(const std::string& name_stack, const std::string& name_field, std::vector<int>& data, const int& id_time_field) const;
template void ReaderFORT21::getValuesVarField(const std::string& name_stack, const std::string& name_field, std::vector<float>& data, const int& id_time_field) const;
template void ReaderFORT21::getValuesVarField(const std::string& name_stack, const std::string& name_field, std::vector<int>& data, const int& id_time_field) const;
template void ReaderFORT21::getValuesVarFieldOnIndex(const std::string& name_stack, const std::string& name_field, std::vector<float>& data, const int& id_index) const;
template void ReaderFORT21::getValuesVarFieldOnIndex(const std::string& name_stack, const std::string& name_field, std::vector<int>& data, const int& id_index) const;
