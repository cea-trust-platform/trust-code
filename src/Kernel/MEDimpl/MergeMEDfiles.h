/****************************************************************************
* Copyright (c) 2021, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        MergeMEDfiles.h
// Directory:   $TRUST_ROOT/src/Kernel/MEDimpl
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MergeMEDfiles_included
#define MergeMEDfiles_included




///////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
// Class MergeMEDfiles
//    Merge several MED files written by several procs into a single field object
//    with a proper underlying mesh (no duplicate nodes)
//    Usage in data file
//    Merge_MED [NOM_DU_CAS||med_files_base_name] [all_times||last_time]
// .SECTION voir aussi
//
//
///////////////////////////////////////////////////////////////////////////
#include <Interprete.h>
#include <vector>
#include <string>

class MergeMEDfiles : public Interprete
{
  Declare_instanciable(MergeMEDfiles);
public :
  Entree& interpreter(Entree&);

private:
  void mergeFields(const std::vector< std::string >& field_names,
                   const std::vector< std::string >& meshes_names,
                   const std::vector<std::string>& listmed,
                   const std::vector< std::pair< std::pair< int,int>,double >>& lst_dt,
                   const int mesh_numb,
                   const int iter_numb,
                   Nom out_file,
                   bool& first_time, const bool isCell) const;
};

#endif /* MergeMEDfiles_included */
