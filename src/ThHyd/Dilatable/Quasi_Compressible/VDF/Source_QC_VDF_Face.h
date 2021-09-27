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
// File:        Source_QC_VDF_Face.h
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Quasi_Compressible/VDF
// Version:     /main/integration_fauchet_165/1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Source_QC_VDF_Face_included
#define Source_QC_VDF_Face_included

#include <Source_Darcy_VDF_Face.h>
#include <Terme_Source_Qdm_VDF_Face.h>
#include <Source_Forchheimer_VDF_Face.h>
#include <Source_Fluide_Dilatable_Face.h>
#include <Perte_Charge_Reguliere_VDF_Face.h>
#include <Perte_Charge_Singuliere_VDF_Face.h>
#include <Terme_Source_Acceleration_VDF_Face.h>

class Source_QC_VDF_Face
{ };

class Acceleration_QC_VDF_Face : public Source_Fluide_Dilatable_Face<Terme_Source_Acceleration_VDF_Face>
{
  Declare_instanciable(Acceleration_QC_VDF_Face);
};

class Source_qdm_QC_VDF_Face : public Source_Fluide_Dilatable_Face<Terme_Source_Qdm_VDF_Face>
{
  Declare_instanciable(Source_qdm_QC_VDF_Face);
};

class Perte_Charge_Reguliere_QC_VDF_Face : public Source_Fluide_Dilatable_Face<Perte_Charge_Reguliere_VDF_Face>
{
  Declare_instanciable(Perte_Charge_Reguliere_QC_VDF_Face);
};

class Perte_Charge_Singuliere_QC_VDF_Face : public Source_Fluide_Dilatable_Face<Perte_Charge_Singuliere_VDF_Face>
{
  Declare_instanciable(Perte_Charge_Singuliere_QC_VDF_Face);
};

class Darcy_QC_VDF_Face : public Source_Fluide_Dilatable_Face<Source_Darcy_VDF_Face>
{
  Declare_instanciable(Darcy_QC_VDF_Face);
};

class Forchheimer_QC_VDF_Face : public Source_Fluide_Dilatable_Face<Source_Forchheimer_VDF_Face>
{
  Declare_instanciable(Forchheimer_QC_VDF_Face);
};

#endif /* Source_QC_VDF_Face_included */
