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
// File:        Iterateur_VDF_base.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Iterateurs
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <Iterateur_VDF_base.h>

Implemente_deriv(Iterateur_VDF_base);
Implemente_base(Iterateur_VDF_base,"Iterateur_VDF_base",Objet_U);
Implemente_instanciable(Iterateur_VDF,"Iterateur_VDF",DERIV(Iterateur_VDF_base));

/* *************************
 * Classe Iterateur_VDF_base
 * *************************/

Entree& Iterateur_VDF_base::readOn(Entree& s ) { return s ; }
Entree& Iterateur_VDF::readOn(Entree& s ) { return s ; }

void Iterateur_VDF_base::associer(const Zone_dis& z, const Zone_Cl_dis& zcl, const Operateur_base& op)
{
  const Zone_VDF& zone_vdf=ref_cast(Zone_VDF, z.valeur());
  const Zone_Cl_VDF& zone_cl_vdf=ref_cast(Zone_Cl_VDF, zcl.valeur());
  associer(zone_vdf, zone_cl_vdf,op);
}

void Iterateur_VDF_base::associer_zone_cl_dis(const Zone_Cl_dis_base& zcl)
{
  const Zone_Cl_VDF& zone_cl_vdf=ref_cast(Zone_Cl_VDF, zcl);
  la_zcl=zone_cl_vdf;
}

void Iterateur_VDF_base::associer(const Zone_VDF& zone_vdf, const Zone_Cl_VDF& zone_cl_vdf, const Operateur_base& op)
{
  la_zone=zone_vdf;
  la_zcl=zone_cl_vdf;
  op_base=op;
  // completer_();
}

void Iterateur_VDF_base::ajouter_contribution_autre_pb(const DoubleTab& inco, Matrice_Morse& matrice, const Cond_lim& la_cl, std::map<int, std::pair<int, int>>& f2e) const
{ /* ne fait rien en general */ }

/* ********************
 * Classe Iterateur_VDF
 * ********************/

Sortie& Iterateur_VDF::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Sortie& Iterateur_VDF_base::printOn(Sortie& s ) const { return s << que_suis_je() ; }

void Iterateur_VDF_base::calculer_flux_bord(const DoubleTab& inco) const
{
  Cerr << que_suis_je() << " must implement calculer_flux_bord" << finl;
  assert(0);
  Process::exit();
}
