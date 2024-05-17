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

#include <Champ_som_lu_VDF.h>

Implemente_instanciable(Champ_som_lu_VDF,"Champ_som_lu_VDF",Champ_som_lu);
// XD champ_som_lu_vdf champ_don_base champ_som_lu_vdf 0 Keyword to read in a file values located at the nodes of a mesh in VDF discretization.
// XD attr domain_name ref_domaine domain_name 0 Name of the domain.
// XD attr dim entier dim 0 Value of the dimension of the field.
// XD attr tolerance floattant tolerance 0 Value of the tolerance to check the coordinates of the nodes.
// XD attr file chaine file 0 name of the file NL2 This file has the following format: NL2 Xi Yi Zi -> Coordinates of the node NL2 Ui Vi Wi -> Value of the field on this node NL2 Xi+1 Yi+1 Zi+1 -> Next point NL2 Ui+1 Vi+1 Zi+1 -> Next value ...

Sortie& Champ_som_lu_VDF::printOn(Sortie& os) const { return Champ_som_lu::printOn(os); }
Entree& Champ_som_lu_VDF::readOn(Entree& is) { return Champ_som_lu::readOn(is); }
