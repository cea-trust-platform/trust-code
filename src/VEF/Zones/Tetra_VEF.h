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
// File:        Tetra_VEF.h
// Directory:   $TRUST_ROOT/src/VEF/Zones
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Tetra_VEF_included
#define Tetra_VEF_included

#include <Elem_VEF_base.h>


class Tetra_VEF : public Elem_VEF_base
{

  Declare_instanciable_sans_constructeur(Tetra_VEF);

public:
  Tetra_VEF();
  inline int nb_facette() const
  {
    return 6;
  };
  void creer_facette_normales(const Zone&  ,DoubleTab&,
                              const IntVect& ) const;
  void creer_normales_facettes_Cl(DoubleTab&, int ,int ,
                                  const DoubleTab& ,const DoubleVect& , const Zone&) const ;
  void modif_volumes_entrelaces(int ,int ,const Zone_VEF& ,DoubleVect& ,int ) const ;
  void modif_volumes_entrelaces_faces_joints(int ,int ,const Zone_VEF& ,DoubleVect& ,int ) const ;
  void modif_normales_facettes_Cl(DoubleTab& ,int ,int ,int ,int, int ,int ) const ;
  void calcul_vc(const ArrOfInt& ,ArrOfDouble& ,const ArrOfDouble& ,
                 const DoubleTab& ,const Champ_Inc_base& ,int, const DoubleVect& ) const ;
  void calcul_xg(DoubleVect& ,const DoubleTab& ,const int ,int& ,
                 int& ,int& ,int& ) const ;
  void normale(int ,DoubleTab&, const IntTab& ,const IntTab&,
               const IntTab& ,const Zone& )  const ;
};

#endif



