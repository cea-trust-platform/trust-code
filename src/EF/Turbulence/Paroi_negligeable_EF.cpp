/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Paroi_negligeable_EF.h>
#include <Domaine_Cl_dis.h>

Implemente_instanciable(Paroi_negligeable_EF, "negligeable_EF", Paroi_hyd_base_EF);

Sortie& Paroi_negligeable_EF::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }
Entree& Paroi_negligeable_EF::readOn(Entree& s) { return s; }
int Paroi_negligeable_EF::init_lois_paroi() { return 1; }
int Paroi_negligeable_EF::calculer_hyd_BiK(DoubleTab& tab_k, DoubleTab& tab_eps) { return 1; }
int Paroi_negligeable_EF::calculer_hyd(DoubleTab& tab_k_eps) { return 1; }
int Paroi_negligeable_EF::calculer_hyd(DoubleTab& tab_nu_t, DoubleTab& tab_k) { return 1; }
int Paroi_negligeable_EF::calculer_scal(Champ_Fonc_base&) { return 1; }
bool Paroi_negligeable_EF::use_shear() const { return false; }
