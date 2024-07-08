/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef Tri_VEF_included
#define Tri_VEF_included

#include <Elem_VEF_base.h>

class Tri_VEF : public Elem_VEF_base
{

  Declare_instanciable_sans_constructeur(Tri_VEF);

public:
  Tri_VEF();
  inline int nb_facette() const override
  {
    return 3;
  };
  void creer_facette_normales(const Domaine&  ,DoubleTab&,
                              const IntVect& ) const override;
  void creer_normales_facettes_Cl(DoubleTab& ,int ,int ,
                                  const DoubleTab& ,const DoubleVect& ,const Domaine&) const override;
  void modif_volumes_entrelaces(int ,int ,const Domaine_VEF& ,DoubleVect& ,int ) const override;
  void modif_volumes_entrelaces_faces_joints(int ,int ,const Domaine_VEF& ,DoubleVect& ,int ) const override;
  void modif_normales_facettes_Cl(DoubleTab&,int ,int
                                  ,int ,int ,int ,int ) const override;
  void calcul_vc(const ArrOfInt& ,ArrOfDouble& ,const ArrOfDouble& ,
                 const DoubleTab& ,const Champ_Inc_base& ,int, const DoubleVect& ) const override;
  void calcul_xg(DoubleVect& ,const DoubleTab& ,const int ,int& ,
                 int& ,int&  ,int& ) const override;
  void normale(int ,DoubleTab&, const IntTab& ,const IntTab&,
               const IntTab& ,const Domaine& )  const override ;
};

KOKKOS_INLINE_FUNCTION void calcul_vc_tri_views(const int* Face, double *vc, const double * vs, const double * vsom,
                                                CDoubleTabView vitesse,True_int type_cl, CDoubleArrView porosite_face)
{
  True_int comp;
  switch(type_cl)
    {
    case 0: // le triangle n'a pas de Face de Dirichlet
      {
        for (comp=0; comp<2; comp++)
          vc[comp] = vs[comp]/3;
        break;
      }

    case 1: // le triangle a une Face de Dirichlet :la Face 2
      {
        for (comp=0; comp<2; comp++)
          vc[comp] = vitesse(Face[2], comp) * porosite_face(Face[2]);
        break;
      }

    case 2: // le triangle a une Face de Dirichlet :la Face 1
      {
        for (comp=0; comp<2; comp++)
          vc[comp] = vitesse(Face[1], comp) * porosite_face(Face[1]);
        break;
      }

    case 4: // le triangle a une Face de Dirichlet :la Face 0
      {
        for (comp=0; comp<2; comp++)
          vc[comp] = vitesse(Face[0], comp) * porosite_face(Face[0]);
        break;
      }

    case 3: // le triangle a deux faces de Dirichlet :les faces 1 et 2
      {
        for (comp=0; comp<2; comp++)
          vc[comp] = vsom[comp];
        break;
      }

    case 5: // le triangle a deux faces de Dirichlet :les faces 0 et 2
      {
        for (comp=0; comp<2; comp++)
          vc[comp] = vsom[2+comp];
        break;
      }

    case 6: // le triangle a deux faces de Dirichlet :les faces 0 et 1
      {
        for (comp=0; comp<2; comp++)
          vc[comp] = vsom[4+comp];
        break;
      }

    } // fin du switch
}
#endif



