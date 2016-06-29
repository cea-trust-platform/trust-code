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
// File:        Assembleur_P_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Solveurs
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Assembleur_P_VDF_included
#define Assembleur_P_VDF_included

#include <Assembleur_base.h>
#include <Ref_Zone_VDF.h>
#include <Ref_Zone_Cl_VDF.h>

//////////////////////////////////////////////////////////////////////////////
//
// CLASS:Assembleur_Pression_VDF
//
//////////////////////////////////////////////////////////////////////////////

class Neumann_sortie_libre;
class Entree_fluide_vitesse_imposee;
class Front_VF;

class Assembleur_P_VDF : public Assembleur_base
{
  Declare_instanciable(Assembleur_P_VDF);

public:
  void associer_zone_dis_base(const Zone_dis_base& )                ;
  void associer_zone_cl_dis_base(const Zone_Cl_dis_base& )          ;
  const Zone_dis_base& zone_dis_base() const                        ;
  const Zone_Cl_dis_base& zone_Cl_dis_base() const                  ;
  int assembler(Matrice&)                                        ;
  int assembler_mat(Matrice&,const DoubleVect&,int incr_pression,int resoudre_en_u) ;
  int assembler_rho_variable(Matrice&, const Champ_Don_base& rho);
  int assembler_QC(const DoubleTab&, Matrice&)                   ;
  int modifier_secmem(DoubleTab&)                                ;
  int modifier_solution(DoubleTab&)                              ;
  void completer(const Equation_base& )                             ;

protected:
  int construire(Matrice& la_matrice);
  int remplir   (Matrice& la_matrice, const DoubleVect& volumes_entrelaces,const Champ_Don_base * rho_ptr);
  void modifier_secmem_pression_imposee(const Neumann_sortie_libre& cond_lim,
                                        const Front_VF& frontiere_vf,
                                        DoubleTab& secmem);
  void modifier_secmem_vitesse_imposee(const Entree_fluide_vitesse_imposee& cond_lim,
                                       const Front_VF& frontiere_vf,
                                       DoubleTab& secmem);
  int liste_faces_periodiques(ArrOfInt& faces);

  REF(Zone_VDF) la_zone_VDF;
  REF(Zone_Cl_VDF) la_zone_Cl_VDF;
  ArrOfDouble les_coeff_pression;

  // Drapeau, indique si la pression est imposee quelque part sur une C.L.
  int has_P_ref;
};

#endif
