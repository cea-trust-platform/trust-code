/****************************************************************************
* Copyright (c) 2022, CEA
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

#ifndef LataLoader_H
#define LataLoader_H

#include <medcoupling++.h>
#include <vector>
#include "LataFilter.h"
namespace MEDCoupling {
class MEDCouplingMesh;
class DataArray;
class  MEDCouplingFieldDouble;
}

class LataLoader
{
public:
  LataLoader(const char*) throw (LataDBError);
  ~LataLoader();

  int GetNTimesteps(void);

  const char* GetType(void) { return "lata"; }

  void FreeUpResources(void);

  MEDCoupling::MEDCouplingMesh* GetMesh(const char *varname, int timestate, int block = -1);
  MEDCoupling::DataArray* GetVectorVar(int, int, const char*);
  void GetTimes(std::vector<double> &times);
  inline std::vector<double> getTimes()
  {
    std::vector<double> a;
    GetTimes(a);
    return a;
  }

  MEDCoupling::MEDCouplingFieldDouble* GetFieldDouble(const char *varname, int timestate, int block = -1);
  std::vector<std::string> GetMeshNames();
  std::vector<std::string> GetFieldNames();
  std::vector<std::string> GetFieldNamesOnMesh(const std::string&);

protected:
  void PopulateDatabaseMetaData(int);
  void register_fieldname(const char *visit_name, const Field_UName&, int component);
  void register_meshname(const char *visit_name, const char *latafilter_name);
  void get_field_info_from_visitname(const char *varname, Field_UName&, int &component) const;

  LataDB lata_db_; // Source database
  LataFilter filter_; // Data processor and cache
  Noms field_username_;
  Field_UNames field_uname_;

  Noms mesh_username_, mesh_latafilter_name_;
  Nom filename;
  // For each name, which component is it in the source field:
  LataVector<int> field_component_;
};

#endif
