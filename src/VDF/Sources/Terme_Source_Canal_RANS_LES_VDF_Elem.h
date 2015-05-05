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
// File:        Terme_Source_Canal_RANS_LES_VDF_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Sources
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Terme_Source_Canal_RANS_LES_VDF_Elem_included
#define Terme_Source_Canal_RANS_LES_VDF_Elem_included



//
// .DESCRIPTION class Terme_Source_Canal_RANS_LES_VDF_Elem
//  Cette classe concerne un terme source calcule en partie grace
//  a un calcul RANS preliminaire et applique au calcul LES en cours
//
#include <Source_base.h>
#include <Ref_Zone_VDF.h>
#include <Ref_Zone_Cl_VDF.h>
#include <Champ_Don.h>

class Probleme_base;
class Navier_Stokes_std;
class DoubleTab;


class Terme_Source_Canal_RANS_LES_VDF_Elem : public Source_base
{
  Declare_instanciable_sans_destructeur(Terme_Source_Canal_RANS_LES_VDF_Elem);

public :
  ~Terme_Source_Canal_RANS_LES_VDF_Elem();
  void associer_pb(const Probleme_base& );
  DoubleTab& ajouter(DoubleTab& ) const;
  DoubleTab& calculer(DoubleTab& ) const;
  void init();
  void mettre_a_jour(double );

protected :
  void associer_zones(const Zone_dis& ,const Zone_Cl_dis& );
  REF(Zone_VDF) la_zone_VDF;
  REF(Zone_Cl_VDF) la_zone_Cl_VDF;

private :
  int moyenne; //type de moyenne
  int cpt, compteur_reprise;
  double alpha_tau, Ly;
  double f_start, t_av;

  DoubleTab tau;
  DoubleTab U_RANS;
  Nom nom_pb_rans;

  DoubleVect utemp_gliss, utemp, utemp_sum, umoy;

  //DoubleVect U, U_RANS;
  //DoubleVect force;


};
#endif
