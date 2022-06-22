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

#ifndef Tetra_VEF_included
#define Tetra_VEF_included

#include <Elem_VEF_base.h>

class Tetra_VEF : public Elem_VEF_base
{

  Declare_instanciable_sans_constructeur(Tetra_VEF);

public:
  Tetra_VEF();
  inline int nb_facette() const override
  {
    return 6;
  };
  void creer_facette_normales(const Zone&  ,DoubleTab&,
                              const IntVect& ) const override;
  void creer_normales_facettes_Cl(DoubleTab&, int ,int ,
                                  const DoubleTab& ,const DoubleVect& , const Zone&) const override ;
  void modif_volumes_entrelaces(int ,int ,const Zone_VEF& ,DoubleVect& ,int ) const override ;
  void modif_volumes_entrelaces_faces_joints(int ,int ,const Zone_VEF& ,DoubleVect& ,int ) const override ;
  void modif_normales_facettes_Cl(DoubleTab& ,int ,int ,int ,int, int ,int ) const override ;
  void calcul_vc(const ArrOfInt& ,ArrOfDouble& ,const ArrOfDouble& ,
                 const DoubleTab& ,const Champ_Inc_base& ,int, const DoubleVect& ) const override ;
  void calcul_xg(DoubleVect& ,const DoubleTab& ,const int ,int& ,
                 int& ,int& ,int& ) const override ;
  void normale(int ,DoubleTab&, const IntTab& ,const IntTab&,
               const IntTab& ,const Zone& )  const override ;
};

#pragma omp declare target
inline void calcul_vc_tetra(const int* Face, double *vc, const double * vs, const double * vsom,
                            const double* vitesse,int type_cl, const double* porosite_face)
{
  int comp;
  switch(type_cl)
    {

    case 0: // le tetraedre n'a pas de Face de Dirichlet
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = 0.25*vs[comp];
        break;
      }

    case 1: // le tetraedre a une Face de Dirichlet : KEL3
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = vitesse[Face[3]*3+comp]*porosite_face[Face[3]];
        break;
      }

    case 2: // le tetraedre a une Face de Dirichlet : KEL2
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = vitesse[Face[2]*3+comp]*porosite_face[Face[2]];
        break;
      }

    case 4: // le tetraedre a une Face de Dirichlet : KEL1
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = vitesse[Face[1]*3+comp]*porosite_face[Face[1]];
        break;
      }

    case 8: // le tetraedre a une Face de Dirichlet : KEL0
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = vitesse[Face[0]*3+comp]*porosite_face[Face[0]];
        break;
      }

    case 3: // le tetraedre a deux faces de Dirichlet : KEL3 et KEL2
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = 0.5* (vsom[comp] + vsom[3+comp]);
        break;
      }

    case 5: // le tetraedre a deux faces de Dirichlet : KEL3 et KEL1
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = 0.5* (vsom[comp] + vsom[6+comp]);
        break;
      }

    case 6: // le tetraedre a deux faces de Dirichlet : KEL1 et KEL2
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = 0.5* (vsom[comp] + vsom[9+comp]);
        break;
      }

    case 9: // le tetraedre a deux faces de Dirichlet : KEL0 et KEL3
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = 0.5* (vsom[3+comp] + vsom[6+comp]);
        break;
      }

    case 10: // le tetraedre a deux faces de Dirichlet : KEL0 et KEL2
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = 0.5* (vsom[3+comp] + vsom[9+comp]);
        break;
      }

    case 12: // le tetraedre a deux faces de Dirichlet : KEL0 et KEL1
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = 0.5*(vsom[6+comp] + vsom[9+comp]);
        break;
      }

    case 7: // le tetraedre a trois faces de Dirichlet : KEL1, KEL2 et KEL3
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = vsom[comp];
        break;
      }

    case 11: // le tetraedre a trois faces de Dirichlet : KEL0,KEL2 et KEL3
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = vsom[3+comp];
        break;
      }

    case 13: // le tetraedre a trois faces de Dirichlet : KEL0, KEL1 et KEL3
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = vsom[6+comp];
        break;
      }

    case 14: // le tetraedre a trois faces de Dirichlet : KEL0, KEL1 et KEL2
      {
        for (comp=0; comp<3; comp++)
          vc[comp] = vsom[9+comp];
        break;
      }

    } // fin du switch

}
#pragma omp end declare target
// Description:
// calcule les coord xg du centre d'un element non standard
// calcule aussi idirichlet=nb de faces de Dirichlet de l'element
#pragma omp declare target
inline void calcul_xg_tetra(double * xg, const double *x, const int type_elem_Cl, int& idirichlet,int& n1,int& n2,int& n3)
{
  int dim = 3;
  switch(type_elem_Cl)
    {
    case 0:  // le tetraedre n'a pas de Face de Dirichlet. Il a 6 Facettes
      {
        for (int j=0; j<dim; j++)
          xg[j]=0.25*(x[j]+x[dim+j]+x[2*dim+j]+x[3*dim+j]);

        idirichlet=0;
        break;
      }

    case 1:  // le tetraedre a une Face de Dirichlet.  Le 'centre'
      // du tetraedre est au milieu de la Face 3 qui a pour sommets
      // 0, 1, 2
      // Il a 3 Facettes reelles : 0   aux noeuds 2 3 xg
      //                           1   aux noeuds 1 3 xg
      //                           3   aux noeuds 3 0 xg
      // les 3 autres Facettes sont sur la Face 3

      {
        for (int j=0; j<dim; j++)
          xg[j]=(x[j]+x[dim+j]+x[2*dim+j])/3.;

        idirichlet=1;
        break;

      }

    case 2:  // le tetraedre a une Face de Dirichlet.  Le 'centre'
      // du tetraedre est au milieu de la Face 2 qui a pour sommets
      // 0, 1, 3
      // Il a 3 Facettes reelles : 0   aux noeuds 2 3 xg
      //                           2   aux noeuds 1 2 xg
      //                           4   aux noeuds 2 0 xg

      {
        for (int j=0; j<dim; j++)
          xg[j]=(x[j]+x[dim+j]+x[3*dim+j])/3.;

        idirichlet=1;
        break;
      }

    case 4:  // le tetraedre a une Face de Dirichlet.  Le 'centre'
      // du tetraedre est au milieu de la Face 1 qui a pour sommets
      // 0, 2, 3
      // Il a 3 Facettes reelles : 1   aux noeuds 1 3 xg
      //                           2   aux noeuds 1 2 xg
      //                           5   aux noeuds 1 0 xg

      {
        for (int j=0; j<dim; j++)
          xg[j]=(x[j]+x[2*dim+j]+x[3*dim+j])/3.;

        idirichlet=1;
        break;
      }

    case 8:  // le tetraedre a une Face de Dirichlet.  Le 'centre'
      // du tetraedre est au milieu de la Face 0 qui a pour sommets
      // 1, 2, 3
      // Il a 3 Facettes reelles : 3   aux noeuds 3 0 xg
      //                           4   aux noeuds 2 0 xg
      //                           5   aux noeuds 1 0 xg

      {
        for (int j=0; j<dim; j++)
          xg[j]=(x[dim+j]+x[2*dim+j]+x[3*dim+j])/3.;

        idirichlet=1;
        break;
      }

    case 3:  // le tetraedre a deux faces de Dirichlet 2 et 3. Le 'centre'
      // du tetraedre est au milieu de l'arete qui a pour extremites 0, 1.
      // Il a 1 Facette nulle: 5

      {
        for (int j=0; j<dim; j++)
          xg[j]= 0.5*(x[j]+x[dim+j]);

        n1=5;
        idirichlet=2;
        break;
      }


    case 5:  // le tetraedre a deux faces de Dirichlet 3 et 1. Le 'centre'
      // du tetraedre est au milieu de l'arete qui a pour extremites 0, 2.
      // Il a 1 Facette  nulle  : 4

      {
        for (int j=0; j<dim; j++)
          xg[j]= 0.5*(x[j]+x[2*dim+j]);

        n1=4;
        idirichlet=2;
        break;
      }

    case 6:  // le tetraedre a deux faces de Dirichlet 1 et 2. Le 'centre'
      // du tetraedre est au milieu de l'arete qui a pour extremites 0, 3.
      // Il a 1 Facette  nulle: 3

      {
        for (int j=0; j<dim; j++)
          xg[j]= 0.5*(x[j]+x[3*dim+j]);

        n1=3;
        idirichlet=2;
        break;
      }

    case 9:  // le tetraedre a deux faces de Dirichlet 0 et 3. Le 'centre'
      // du tetraedre est au milieu de l'arete qui a pour extremites 1, 2
      // Il a 1 Facette  nulle: 2

      {
        for (int j=0; j<dim; j++)
          xg[j]= 0.5*(x[dim+j]+x[2*dim+j]);

        n1=2;
        idirichlet=2;
        break;
      }

    case 10:  // le tetraedre a deux faces de Dirichlet 0 et 2. Le 'centre'
      // du tetraedre est au milieu de l'arete qui a pour extremites 1, 3.
      // Il a 1 Facette  nulle: 1

      {
        for (int j=0; j<dim; j++)
          xg[j]= 0.5*(x[dim+j]+x[3*dim+j]);

        n1=1;
        idirichlet=2;
        break;
      }


    case 12:  // le tetraedre a deux faces de Dirichlet 0 et 1. Le 'centre'
      // du tetraedre est au milieu de l'arete qui a pour sommets 2, 3.
      // Il a 1 Facette  nulle

      {
        for (int j=0; j<dim; j++)
          xg[j]= 0.5*(x[2*dim+j]+x[3*dim+j]);

        n1=0;
        idirichlet=2;
        break;
      }

    case 7:  // trois faces de Dirichlet : 1,2,3. Le centre est au sommet 0
      // il y 3 Facettes nulles: 3,4,5

      {
        for (int j=0; j<dim; j++)
          xg[j]= x[j];

        n1=3;
        n2=4;
        n3=5;
        idirichlet=3;
        break;

      }

    case 11:  // trois faces de Dirichlet : 0,2,3. Le centre est au sommet 1
      // il y 3 Facettes nulles: 1, 2, 5

      {
        for (int j=0; j<dim; j++)
          xg[j]= x[dim+j];

        n1=1;
        n2=2;
        n3=5;
        idirichlet=3;
        break;

      }

    case 13:  // trois faces de Dirichlet : 0,1,3. Le centre est au sommet 2
      // il y 3 Facettes nulles: 0, 2, 4

      {
        for (int j=0; j<dim; j++)
          xg[j]= x[2*dim+j];

        n1=0;
        n2=2;
        n3=4;
        idirichlet=3;
        break;

      }
    case 14:  // trois faces de Dirichlet : 0,1,2. Le centre est au sommet 3
      // il y 3 Facettes nulles: 0, 1, 3

      {
        for (int j=0; j<dim; j++)
          xg[j]= x[3*dim+j];

        n1=0;
        n2=1;
        n3=3;
        idirichlet=3;
        break;

      }
    }
}
#pragma omp end declare target
#endif



