/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Paroi_loi_WW_scal_VEF.h
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Paroi_loi_WW_scal_VEF_included
#define Paroi_loi_WW_scal_VEF_included

#include <Paroi_std_scal_hyd_VEF.h>
#include <Table.h>
#include <Zone_VEF.h>

//.DESCRIPTION classe Paroi_loi_WW_scal_VEF
//
//
//

//.SECTION  voir aussi
//  Paroi_loi_WW_scal_VEF

class Paroi_loi_WW_scal_VEF : public Paroi_std_scal_hyd_VEF
{

  Declare_instanciable_sans_constructeur(Paroi_loi_WW_scal_VEF);

public:
  void associer(const Zone_dis& ,const Zone_Cl_dis& );
  int calculer_scal(Champ_Fonc_base& );

private:

};

#endif

