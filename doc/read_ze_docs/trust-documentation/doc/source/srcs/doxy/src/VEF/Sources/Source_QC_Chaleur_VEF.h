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

#ifndef Source_QC_Chaleur_VEF_included
#define Source_QC_Chaleur_VEF_included

#include <Source_Fluide_Dilatable_VEF_Proto.h>
#include <Source_QC_Chaleur.h>

/*! @brief class  Source_QC_Chaleur_VEF
 *
 *  Cette classe represente un terme source supplementaire a prendre en compte
 *  dans les equations de la chaleur dans le cas ou le fluide est quasi compressible
 *  et pour une discretisation VEF
 *
 *
 * @sa Source_base Fluide_Quasi_Compressible Source_QC_Chaleur
 */

class Source_QC_Chaleur_VEF : public Source_QC_Chaleur, public Source_Fluide_Dilatable_VEF_Proto
{
  Declare_instanciable(Source_QC_Chaleur_VEF);
protected:
  void associer_domaines(const Domaine_dis& ,const Domaine_Cl_dis& ) override;
};

#endif /* Source_QC_Chaleur_VEF_included */
