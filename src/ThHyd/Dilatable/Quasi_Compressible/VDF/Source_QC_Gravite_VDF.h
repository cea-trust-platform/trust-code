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
// File:        Source_QC_Gravite_VDF.h
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Quasi_Compressible/VDF
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Source_QC_Gravite_VDF_included
#define Source_QC_Gravite_VDF_included

#include <Source_Gravite_Fluide_Dilatable_base.h>
#include <Source_Fluide_Dilatable_VDF_Proto.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION class  Source_QC_Gravite_VDF
//
//  Cette classe represente un terme source supplementaire a prendre en compte
//  dans les equations de quantite de mouvement dans le cas ou le fluide est
//  quasi compressible, en cas de gravite, et pour une discretisation VDF.
//
// .SECTION voir aussi
// Source_base Fluide_Quasi_Compressible Source_Gravite_Fluide_Dilatable_base
//
//////////////////////////////////////////////////////////////////////////////

class Source_QC_Gravite_VDF : public Source_Gravite_Fluide_Dilatable_base,
  public Source_Fluide_Dilatable_VDF_Proto
{
  Declare_instanciable(Source_QC_Gravite_VDF);

public:
  DoubleTab& ajouter(DoubleTab& ) const;

protected :
  void associer_zones(const Zone_dis& zone,const Zone_Cl_dis& );
};

#endif /* Source_QC_Gravite_VDF_included */
