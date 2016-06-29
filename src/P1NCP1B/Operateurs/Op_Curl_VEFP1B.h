/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Op_Curl_VEFP1B.h
// Directory:   $TRUST_ROOT/src/P1NCP1B/Operateurs
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_Curl_VEFP1B_included
#define Op_Curl_VEFP1B_included

#include <Operateur_base.h>
#include <Ref_Zone_VEF.h>
#include <Zone_VEF.h>
#include <Ref_Zone_Cl_VEF.h>
#include <Domaine.h>
#include <IntLists.h>

class Zone_VEF_PreP1b;

class Op_Curl_VEFP1B : public Operateur_base
{

  Declare_instanciable(Op_Curl_VEFP1B);


public:

  ///////////////////////////////////////////////////
  // 3 methodes a surcharger car heritees de Op_base.
  ///////////////////////////////////////////////////
  DoubleTab& ajouter( const DoubleTab&, DoubleTab&) const;
  DoubleTab& calculer( const DoubleTab&, DoubleTab&) const;
  void associer( const Zone_dis&, const Zone_Cl_dis&, const Champ_Inc&);

  //////////////////////////////////////////////////////
  // Autres methodes
  /////////////////////////////////////////////////////

  DoubleTab vecteur_normal(const int& face, const int& elem) const;
  int elements_pour_sommet();
  int elements_pour_sommet(const int& sommet,const int& indice) const;
  int elem_som_size(const int& sommet) const;

  const Zone_VEF_PreP1b& zone_Vef() const;

protected:
  REF(Zone_VEF) la_zone_vef;
  REF(Zone_Cl_VEF) la_zcl_vef;
  IntLists elements_pour_sommet_;
  /*   DoubleTab flux_tangent_bords_; */
};

#endif
