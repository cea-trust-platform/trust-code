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
// File:        EDO_Pression_th_VEF.h
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible/VEF
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#ifndef EDO_Pression_th_VEF_included
#define EDO_Pression_th_VEF_included

#include <EDO_Pression_th_base.h>
#include <Ref_Zone_VEF.h>
#include <Ref_Zone_Cl_dis.h>
class Fluide_Quasi_Compressible;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe EDO_Pression_th_VEF
//     Cette classe represente l'EDO sur la pression associee au schema de
//     de calcul pour les fluides faiblement compressibles, et relatif a
//     discretisation de type VEF.
// .SECTION voir aussi
//     Fluide_Quasi_Compressible EDO_Pression_th_base
//////////////////////////////////////////////////////////////////////////////

class EDO_Pression_th_VEF: public EDO_Pression_th_base
{
  Declare_base(EDO_Pression_th_VEF);
public :
  const Fluide_Quasi_Compressible& le_fluide() const
  {
    return le_fluide_.valeur();
  };

  void associer_zones(const Zone_dis&,const Zone_Cl_dis&);
  void completer();
  void calculer_grad(const DoubleTab&,DoubleTab&);
  void divu_discvit(const DoubleTab& , DoubleTab& );
  double masse_totale(double P,const DoubleTab& T);
  void secmembre_divU_Z(DoubleTab& ) const;
  void secmembre_divU_Z_VEFP1B(DoubleTab& ) const;
  double moyenne_vol(const DoubleTab&) const;
  void mettre_a_jour_CL(double P);
  double getM0()
  {
    return M0; // renvoie la masse totale initiale.
  }
  const DoubleTab& rho_discvit() const;
  const DoubleTab& rho_face_n() const;
  const DoubleTab& rho_face_np1() const;
  // ne fait  rien rho est deja au face
  inline void calculer_rho_face_np1(const DoubleTab& rho)
  {
    ;
  };

protected :
  REF(Zone_VEF) la_zone;
  REF(Zone_Cl_dis) la_zone_Cl;

  double M0;// la masse totale initiale

};



#endif
