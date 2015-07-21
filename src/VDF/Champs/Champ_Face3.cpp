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
// File:        Champ_Face3.cpp
// Directory:   $TRUST_ROOT/src/VDF/Champs
// Version:     /main/28
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Face.h>
#include <Champ_Uniforme.h>
#include <Zone_Cl_VDF.h>
#include <Les_Cl.h>
#include <Fluide_Incompressible.h>
#include <Equation_base.h>
#include <Mod_turb_hyd_base.h>
#include <distances_VDF.h>

void calrotord2centelemdim2(DoubleTab& rot, const DoubleTab& val,
                            const Zone_VDF& zone_VDF,
                            int nb_elem, const IntTab& face_voisins,
                            const IntTab& elem_faces);

// Traitement des elements internes
void calcul_interne3D(int num_elem,
                      int elx0,int elx1,
                      int ely0,int ely1,
                      int elz0,int elz1,
                      const Zone_VDF& zone_VDF,
                      const DoubleTab& val,DoubleTab& rot)
{
  const IntTab& elem_faces = zone_VDF.elem_faces();
  double delta_x_0,delta_x_1,delta_y_0,delta_y_1,delta_z_0,delta_z_1;
  double delta_x,delta_y,delta_z;
  double deriv_wy,deriv_vz,deriv_uz,deriv_wx,deriv_vx,deriv_uy;

  deriv_wy = 0;
  deriv_vz = 0;
  deriv_uz = 0;
  deriv_wx = 0;
  deriv_vx = 0;
  deriv_uy = 0;

  delta_x_0 = zone_VDF.dist_elem_period(num_elem,elx0,0);
  delta_x_1 = zone_VDF.dist_elem_period(elx1,num_elem,0);
  delta_y_0 = zone_VDF.dist_elem_period(num_elem,ely0,1);
  delta_y_1 = zone_VDF.dist_elem_period(ely1,num_elem,1);
  delta_z_0 = zone_VDF.dist_elem_period(num_elem,elz0,2);
  delta_z_1 = zone_VDF.dist_elem_period(elz1,num_elem,2);

  delta_x = (delta_x_1-delta_x_0)*(delta_x_1+delta_x_0)/(delta_x_1*delta_x_0);
  delta_y = (delta_y_1-delta_y_0)*(delta_y_1+delta_y_0)/(delta_y_1*delta_y_0);
  delta_z = (delta_z_1-delta_z_0)*(delta_z_1+delta_z_0)/(delta_z_1*delta_z_0);

  deriv_vz = (delta_z_0/delta_z_1*val(elem_faces(elz1,1))
              + delta_z*val(elem_faces(num_elem,1))
              - delta_z_1/delta_z_0*val(elem_faces(elz0,1)));
  deriv_vz += (delta_z_0/delta_z_1*val(elem_faces(elz1,4))
               + delta_z*val(elem_faces(num_elem,4))
               - delta_z_1/delta_z_0*val(elem_faces(elz0,4)));
  deriv_vz *= 0.5/(delta_z_0 + delta_z_1);

  deriv_wy = (delta_y_0/delta_y_1*val(elem_faces(ely1,2))
              + delta_y*val(elem_faces(num_elem,2))
              - delta_y_1/delta_y_0*val(elem_faces(ely0,2)));
  deriv_wy += (delta_y_0/delta_y_1*val(elem_faces(ely1,5))
               + delta_y*val(elem_faces(num_elem,5))
               - delta_y_1/delta_y_0*val(elem_faces(ely0,5)));
  deriv_wy *= 0.5/(delta_y_0 + delta_y_1);


  deriv_uz = (delta_z_0/delta_z_1*val(elem_faces(elz1,0))
              + delta_z*val(elem_faces(num_elem,0))
              - delta_z_1/delta_z_0*val(elem_faces(elz0,0)));
  deriv_uz += (delta_z_0/delta_z_1*val(elem_faces(elz1,3))
               + delta_z*val(elem_faces(num_elem,3))
               - delta_z_1/delta_z_0*val(elem_faces(elz0,3)));
  deriv_uz *= 0.5/(delta_z_0 + delta_z_1);

  deriv_wx = (delta_x_0/delta_x_1*val(elem_faces(elx1,2))
              + delta_x*val(elem_faces(num_elem,2))
              - delta_x_1/delta_x_0*val(elem_faces(elx0,2)));
  deriv_wx += (delta_x_0/delta_x_1*val(elem_faces(elx1,5))
               + delta_x*val(elem_faces(num_elem,5))
               - delta_x_1/delta_x_0*val(elem_faces(elx0,5)));
  deriv_wx *= 0.5/(delta_x_0 + delta_x_1);

  deriv_vx = (delta_x_0/delta_x_1*val(elem_faces(elx1,1))
              + delta_x*val(elem_faces(num_elem,1))
              - delta_x_1/delta_x_0*val(elem_faces(elx0,1)));
  deriv_vx += (delta_x_0/delta_x_1*val(elem_faces(elx1,4))
               + delta_x*val(elem_faces(num_elem,4))
               - delta_x_1/delta_x_0*val(elem_faces(elx0,4)));
  deriv_vx *= 0.5/(delta_x_0 + delta_x_1);

  deriv_uy = (delta_y_0/delta_y_1*val(elem_faces(ely1,0))
              + delta_y*val(elem_faces(num_elem,0))
              - delta_y_1/delta_y_0*val(elem_faces(ely0,0)));
  deriv_uy += (delta_y_0/delta_y_1*val(elem_faces(ely1,3))
               + delta_y*val(elem_faces(num_elem,3))
               - delta_y_1/delta_y_0*val(elem_faces(ely0,3)));
  deriv_uy *= 0.5/(delta_y_0 + delta_y_1);

  rot(num_elem,0) = deriv_wy - deriv_vz;
  rot(num_elem,1) = deriv_uz - deriv_wx;
  rot(num_elem,2) = deriv_vx - deriv_uy;

}

// Traitement des elements bord
void calcul_bord3D(int num_elem,
                   int elx0,int elx1,
                   int ely0,int ely1,
                   int elz0,int elz1,
                   const Zone_VDF& zone_VDF,
                   const DoubleTab& val,DoubleTab& rot)
{
  const IntTab& elem_faces = zone_VDF.elem_faces();
  double delta_x_0,delta_x_1,delta_y_0,delta_y_1,delta_z_0,delta_z_1;
  double delta_x,delta_y,delta_z;
  double deriv_wy,deriv_vz,deriv_uz,deriv_wx,deriv_vx,deriv_uy;

  deriv_wy = 0;
  deriv_vz = 0;
  deriv_uz = 0;
  deriv_wx = 0;
  deriv_vx = 0;
  deriv_uy = 0;

  // Traitement des elements bord

  if ( (elx0 == -1) || (elx1 == -1) )
    {
      if (elx0 == -1)
        {
          if (elx1 != -1)
            {
              delta_x_1 = zone_VDF.dist_elem_period(elx1,num_elem,0);
              deriv_vx = (val(elem_faces(elx1,1))-val(elem_faces(num_elem,1))
                          + val(elem_faces(elx1,4))-val(elem_faces(num_elem,4)));
              deriv_vx *= 0.5/delta_x_1;
              deriv_wx = (val(elem_faces(elx1,2))-val(elem_faces(num_elem,2))
                          + val(elem_faces(elx1,5))-val(elem_faces(num_elem,5)));
              deriv_wx *= 0.5/delta_x_1;
            }
          else
            {
              deriv_vx = 0;
              deriv_wx = 0;
            }
        }
      else // elx1 = -1 et elx0 != -1
        {
          delta_x_0 = zone_VDF.dist_elem_period(num_elem,elx0,0);
          deriv_vx = (val(elem_faces(num_elem,1))-val(elem_faces(elx0,1))
                      + val(elem_faces(num_elem,4))-val(elem_faces(elx0,4)));
          deriv_vx *= 0.5/delta_x_0;
          deriv_wx = (val(elem_faces(num_elem,2))-val(elem_faces(elx0,2))
                      + val(elem_faces(num_elem,5))-val(elem_faces(elx0,5)));
          deriv_wx *= 0.5/delta_x_0;
        }

      if ( (ely0 == -1) || (ely1 == -1) )
        {
          if (ely0 == -1)
            {
              if (ely1 != -1)
                {
                  delta_y_1 = zone_VDF.dist_elem_period(ely1,num_elem,1);
                  deriv_uy = (val(elem_faces(ely1,0))-val(elem_faces(num_elem,0))
                              + val(elem_faces(ely1,3))-val(elem_faces(num_elem,3)));
                  deriv_uy *= 0.5/delta_y_1;
                  deriv_wy = (val(elem_faces(ely1,2))-val(elem_faces(num_elem,2))
                              + val(elem_faces(ely1,5))-val(elem_faces(num_elem,5)));
                  deriv_wy *= 0.5/delta_y_1;
                }
              else
                {
                  deriv_uy = 0;
                  deriv_wy = 0;
                }
            }
          else // ely1 = -1 et ely0 != -1
            {
              delta_y_0 = zone_VDF.dist_elem_period(num_elem,ely0,1);
              deriv_uy = (val(elem_faces(num_elem,0))-val(elem_faces(ely0,0))
                          + val(elem_faces(num_elem,3))-val(elem_faces(ely0,3)));
              deriv_uy *= 0.5/delta_y_0;
              deriv_wy = (val(elem_faces(num_elem,2))-val(elem_faces(ely0,2))
                          + val(elem_faces(num_elem,5))-val(elem_faces(ely0,5)));
              deriv_wy *= 0.5/delta_y_0;
            }
        }
      else
        {
          delta_y_0 = zone_VDF.dist_elem_period(num_elem,ely0,1);
          delta_y_1 = zone_VDF.dist_elem_period(ely1,num_elem,1);
          delta_y = (delta_y_1-delta_y_0)*(delta_y_1+delta_y_0)/(delta_y_1*delta_y_0);

          deriv_uy = (delta_y_0/delta_y_1*val(elem_faces(ely1,0))
                      + delta_y*val(elem_faces(num_elem,0))
                      - delta_y_1/delta_y_0*val(elem_faces(ely0,0)));
          deriv_uy += (delta_y_0/delta_y_1*val(elem_faces(ely1,3))
                       + delta_y*val(elem_faces(num_elem,3))
                       - delta_y_1/delta_y_0*val(elem_faces(ely0,3)));
          deriv_uy *= 0.5/(delta_y_0 + delta_y_1);

          deriv_wy = (delta_y_0/delta_y_1*val(elem_faces(ely1,2))
                      + delta_y*val(elem_faces(num_elem,2))
                      - delta_y_1/delta_y_0*val(elem_faces(ely0,2)));
          deriv_wy += (delta_y_0/delta_y_1*val(elem_faces(ely1,5))
                       + delta_y*val(elem_faces(num_elem,5))
                       - delta_y_1/delta_y_0*val(elem_faces(ely0,5)));
          deriv_wy *= 0.5/(delta_y_0 + delta_y_1);
        }

      if ( (elz0 == -1) || (elz1 == -1) )
        {
          if (elz0 == -1)
            {
              if (elz1 != -1)
                {
                  delta_z_1 = zone_VDF.dist_elem_period(elz1,num_elem,2);
                  deriv_uz = (val(elem_faces(elz1,0))-val(elem_faces(num_elem,0))
                              + val(elem_faces(elz1,3))-val(elem_faces(num_elem,3)));
                  deriv_uz *= 0.5/delta_z_1;
                  deriv_vz = (val(elem_faces(elz1,1))-val(elem_faces(num_elem,1))
                              + val(elem_faces(elz1,4))-val(elem_faces(num_elem,4)));
                  deriv_vz *= 0.5/delta_z_1;
                }
              else
                {
                  deriv_uz = 0;
                  deriv_vz = 0;
                }
            }
          else // elz1 = -1 et elz0 != -1
            {
              delta_z_0 = zone_VDF.dist_elem_period(num_elem,elz0,2);
              deriv_uz = (val(elem_faces(num_elem,0))-val(elem_faces(elz0,0))
                          + val(elem_faces(num_elem,3))-val(elem_faces(elz0,3)));
              deriv_uz *= 0.5/delta_z_0;
              deriv_vz =(val(elem_faces(num_elem,1))-val(elem_faces(elz0,1))
                         + val(elem_faces(num_elem,4))-val(elem_faces(elz0,4)));
              deriv_vz *= 0.5/delta_z_0;
            }
        }
      else
        {
          delta_z_0 = zone_VDF.dist_elem_period(num_elem,elz0,2);
          delta_z_1 = zone_VDF.dist_elem_period(elz1,num_elem,2);
          delta_z = (delta_z_1-delta_z_0)*(delta_z_1+delta_z_0)/(delta_z_1*delta_z_0);

          deriv_uz = (delta_z_0/delta_z_1*val(elem_faces(elz1,0))
                      + delta_z*val(elem_faces(num_elem,0))
                      - delta_z_1/delta_z_0*val(elem_faces(elz0,0)));
          deriv_uz += (delta_z_0/delta_z_1*val(elem_faces(elz1,3))
                       + delta_z*val(elem_faces(num_elem,3))
                       - delta_z_1/delta_z_0*val(elem_faces(elz0,3)));
          deriv_uz *= 0.5/(delta_z_0 + delta_z_1);

          deriv_vz = (delta_z_0/delta_z_1*val(elem_faces(elz1,1))
                      + delta_z*val(elem_faces(num_elem,1))
                      - delta_z_1/delta_z_0*val(elem_faces(elz0,1)));
          deriv_vz += (delta_z_0/delta_z_1*val(elem_faces(elz1,4))
                       + delta_z*val(elem_faces(num_elem,4))
                       - delta_z_1/delta_z_0*val(elem_faces(elz0,4)));
          deriv_vz *= 0.5/(delta_z_0 + delta_z_1);
        }

    }
  else if ( (ely0 == -1) || (ely1 == -1) )
    {

      if (ely0 == -1)
        {
          if (ely1 != -1)
            {
              delta_y_1 = zone_VDF.dist_elem_period(ely1,num_elem,1);
              deriv_uy = (val(elem_faces(ely1,0))-val(elem_faces(num_elem,0))
                          + val(elem_faces(ely1,3))-val(elem_faces(num_elem,3)));
              deriv_uy *= 0.5/delta_y_1;
              deriv_wy = (val(elem_faces(ely1,2))-val(elem_faces(num_elem,2))
                          + val(elem_faces(ely1,5))-val(elem_faces(num_elem,5)));
              deriv_wy *= 0.5/delta_y_1;
            }
          else
            {
              deriv_uy = 0;
              deriv_wy = 0;
            }
        }
      else  // ely1 = -1 et ely0 != -1
        {
          delta_y_0 = zone_VDF.dist_elem_period(num_elem,ely0,1);
          deriv_uy = (val(elem_faces(num_elem,0))-val(elem_faces(ely0,0))
                      + val(elem_faces(num_elem,3))-val(elem_faces(ely0,3)));
          deriv_uy *= 0.5/delta_y_0;
          deriv_wy = (val(elem_faces(num_elem,2))-val(elem_faces(ely0,2))
                      + val(elem_faces(num_elem,5))-val(elem_faces(ely0,5)));
          deriv_wy *= 0.5/delta_y_0;
        }

      delta_x_0 = zone_VDF.dist_elem_period(num_elem,elx0,0);
      delta_x_1 = zone_VDF.dist_elem_period(elx1,num_elem,0);
      delta_x = (delta_x_1-delta_x_0)*(delta_x_1+delta_x_0)/(delta_x_1*delta_x_0);

      deriv_wx = (delta_x_0/delta_x_1*val(elem_faces(elx1,2))
                  + delta_x*val(elem_faces(num_elem,2))
                  - delta_x_1/delta_x_0*val(elem_faces(elx0,2)));
      deriv_wx += (delta_x_0/delta_x_1*val(elem_faces(elx1,5))
                   + delta_x*val(elem_faces(num_elem,5))
                   - delta_x_1/delta_x_0*val(elem_faces(elx0,5)));
      deriv_wx *= 0.5/(delta_x_0 + delta_x_1);

      deriv_vx = (delta_x_0/delta_x_1*val(elem_faces(elx1,1))
                  + delta_x*val(elem_faces(num_elem,1))
                  - delta_x_1/delta_x_0*val(elem_faces(elx0,1)));
      deriv_vx += (delta_x_0/delta_x_1*val(elem_faces(elx1,4))
                   + delta_x*val(elem_faces(num_elem,4))
                   - delta_x_1/delta_x_0*val(elem_faces(elx0,4)));
      deriv_vx *= 0.5/(delta_x_0 + delta_x_1);


      if ( (elz0 == -1) || (elz1 == -1) )
        {
          if (elz0 == -1)
            {
              if (elz1 != -1)
                {
                  delta_z_1 = zone_VDF.dist_elem_period(elz1,num_elem,2);
                  deriv_uz = (val(elem_faces(elz1,0))-val(elem_faces(num_elem,0))
                              + val(elem_faces(elz1,3))-val(elem_faces(num_elem,3)));
                  deriv_uz *= 0.5/delta_z_1;
                  deriv_vz = (val(elem_faces(elz1,1))-val(elem_faces(num_elem,1))
                              + val(elem_faces(elz1,4))-val(elem_faces(num_elem,4)));
                  deriv_vz *= 0.5/delta_z_1;
                }
              else
                {
                  deriv_uz = 0;
                  deriv_vz = 0;
                }
            }
          else
            {
              delta_z_0 = zone_VDF.dist_elem_period(num_elem,elz0,2);
              deriv_uz = (val(elem_faces(num_elem,0))-val(elem_faces(elz0,0))
                          + val(elem_faces(num_elem,3))-val(elem_faces(elz0,3)));
              deriv_uz *= 0.5/delta_z_0;
              deriv_vz =(val(elem_faces(num_elem,1))-val(elem_faces(elz0,1))
                         + val(elem_faces(num_elem,4))-val(elem_faces(elz0,4)));
              deriv_vz *= 0.5/delta_z_0;
            }
        }
      else
        {
          delta_z_0 = zone_VDF.dist_elem_period(num_elem,elz0,2);
          delta_z_1 = zone_VDF.dist_elem_period(elz1,num_elem,2);
          delta_z = (delta_z_1-delta_z_0)*(delta_z_1+delta_z_0)/(delta_z_1*delta_z_0);

          deriv_uz = (delta_z_0/delta_z_1*val(elem_faces(elz1,0))
                      + delta_z*val(elem_faces(num_elem,0))
                      - delta_z_1/delta_z_0*val(elem_faces(elz0,0)));
          deriv_uz += (delta_z_0/delta_z_1*val(elem_faces(elz1,3))
                       + delta_z*val(elem_faces(num_elem,3))
                       - delta_z_1/delta_z_0*val(elem_faces(elz0,3)));
          deriv_uz *= 0.5/(delta_z_0 + delta_z_1);

          deriv_vz = (delta_z_0/delta_z_1*val(elem_faces(elz1,1))
                      + delta_z*val(elem_faces(num_elem,1))
                      - delta_z_1/delta_z_0*val(elem_faces(elz0,1)));
          deriv_vz += (delta_z_0/delta_z_1*val(elem_faces(elz1,4))
                       + delta_z*val(elem_faces(num_elem,4))
                       - delta_z_1/delta_z_0*val(elem_faces(elz0,4)));
          deriv_vz *= 0.5/(delta_z_0 + delta_z_1);
        }
    }
  else if ( (elz0 == -1) || (elz1 == -1) )
    {
      if (elz0 == -1)
        {
          if (elz1 != -1)
            {
              delta_z_1 = zone_VDF.dist_elem_period(elz1,num_elem,2);
              deriv_uz = (val(elem_faces(elz1,0))-val(elem_faces(num_elem,0))
                          + val(elem_faces(elz1,3))-val(elem_faces(num_elem,3)));
              deriv_uz *= 0.5/delta_z_1;
              deriv_vz = (val(elem_faces(elz1,1))-val(elem_faces(num_elem,1))
                          + val(elem_faces(elz1,4))-val(elem_faces(num_elem,4)));
              deriv_vz *= 0.5/delta_z_1;
            }
          else
            {
              deriv_uz = 0;
              deriv_vz = 0;
            }
        }
      else // elz1 = -1 et elz0 != -1
        {
          delta_z_0 = zone_VDF.dist_elem_period(num_elem,elz0,2);
          deriv_uz = (val(elem_faces(num_elem,0))-val(elem_faces(elz0,0))
                      + val(elem_faces(num_elem,3))-val(elem_faces(elz0,3)));
          deriv_uz *= 0.5/delta_z_0;
          deriv_vz =(val(elem_faces(num_elem,1))-val(elem_faces(elz0,1))
                     + val(elem_faces(num_elem,4))-val(elem_faces(elz0,4)));
          deriv_vz *= 0.5/delta_z_0;
        }

      delta_x_0 = zone_VDF.dist_elem_period(num_elem,elx0,0);
      delta_x_1 = zone_VDF.dist_elem_period(elx1,num_elem,0);
      delta_x = (delta_x_1-delta_x_0)*(delta_x_1+delta_x_0)/(delta_x_1*delta_x_0);

      deriv_wx = (delta_x_0/delta_x_1*val(elem_faces(elx1,2))
                  + delta_x*val(elem_faces(num_elem,2))
                  - delta_x_1/delta_x_0*val(elem_faces(elx0,2)));
      deriv_wx += (delta_x_0/delta_x_1*val(elem_faces(elx1,5))
                   + delta_x*val(elem_faces(num_elem,5))
                   - delta_x_1/delta_x_0*val(elem_faces(elx0,5)));
      deriv_wx *= 0.5/(delta_x_0 + delta_x_1);

      deriv_vx = (delta_x_0/delta_x_1*val(elem_faces(elx1,1))
                  + delta_x*val(elem_faces(num_elem,1))
                  - delta_x_1/delta_x_0*val(elem_faces(elx0,1)));
      deriv_vx += (delta_x_0/delta_x_1*val(elem_faces(elx1,4))
                   + delta_x*val(elem_faces(num_elem,4))
                   - delta_x_1/delta_x_0*val(elem_faces(elx0,4)));
      deriv_vx *= 0.5/(delta_x_0 + delta_x_1);

      delta_y_0 = zone_VDF.dist_elem_period(num_elem,ely0,1);
      delta_y_1 = zone_VDF.dist_elem_period(ely1,num_elem,1);
      delta_y = (delta_y_1-delta_y_0)*(delta_y_1+delta_y_0)/(delta_y_1*delta_y_0);

      deriv_wy = (delta_y_0/delta_y_1*val(elem_faces(ely1,2))
                  + delta_y*val(elem_faces(num_elem,2))
                  - delta_y_1/delta_y_0*val(elem_faces(ely0,2)));
      deriv_wy += (delta_y_0/delta_y_1*val(elem_faces(ely1,5))
                   + delta_y*val(elem_faces(num_elem,5))
                   - delta_y_1/delta_y_0*val(elem_faces(ely0,5)));
      deriv_wy *= 0.5/(delta_y_0 + delta_y_1);

      deriv_uy = (delta_y_0/delta_y_1*val(elem_faces(ely1,0))
                  + delta_y*val(elem_faces(num_elem,0))
                  - delta_y_1/delta_y_0*val(elem_faces(ely0,0)));
      deriv_uy += (delta_y_0/delta_y_1*val(elem_faces(ely1,3))
                   + delta_y*val(elem_faces(num_elem,3))
                   - delta_y_1/delta_y_0*val(elem_faces(ely0,3)));
      deriv_uy *= 0.5/(delta_y_0 + delta_y_1);
    }

  rot(num_elem,0) = deriv_wy - deriv_vz;
  rot(num_elem,1) = deriv_uz - deriv_wx;
  rot(num_elem,2) = deriv_vx - deriv_uy;
}

void calrotord2centelemdim3(DoubleTab& rot, const DoubleTab& val, const Zone_VDF& zone_VDF,
                            int nb_elem, const IntTab& face_voisins, const IntTab& elem_faces)
{
  if(rot.dimension(0)!=nb_elem)
    rot.resize(nb_elem, 3);
  int elx0,elx1,ely0,ely1,elz0,elz1;

  for (int num_elem=0; num_elem<nb_elem; num_elem++)
    {

      elx0 = face_voisins(elem_faces(num_elem,0),0);
      elx1 = face_voisins(elem_faces(num_elem,3),1);
      ely0 = face_voisins(elem_faces(num_elem,1),0);
      ely1 = face_voisins(elem_faces(num_elem,4),1);
      elz0 = face_voisins(elem_faces(num_elem,2),0);
      elz1 = face_voisins(elem_faces(num_elem,5),1);

      if ( (elx0 != -1) && (elx1 != -1) && (ely0 != -1)
           && (ely1 != -1) && (elz0 != -1) && (elz1 != -1) )
        // Cas d'un element interne

        calcul_interne3D(num_elem,elx0,elx1,ely0,ely1,elz0,elz1,zone_VDF,val,rot);
      else
        calcul_bord3D(num_elem,elx0,elx1,ely0,ely1,elz0,elz1,zone_VDF,val,rot);
    }
}



void Champ_Face::calculer_rotationnel_ordre2_centre_element(DoubleTab& rot) const
{
  const DoubleTab& val = valeurs();
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  int nb_elem = zone_VDF.nb_elem();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();

  if (dimension == 2 )
    calrotord2centelemdim2(rot, val, zone_VDF, nb_elem, face_voisins, elem_faces);
  else if (dimension == 3)
    calrotord2centelemdim3(rot, val, zone_VDF, nb_elem, face_voisins, elem_faces);
}

int Champ_Face::imprime(Sortie& os, int ncomp) const
{
  imprime_Face(os,ncomp);
  return 1;
}


void Champ_Face::calcul_critere_Q(DoubleTab& Q, const Zone_Cl_VDF& zone_Cl_VDF)
{
  // Q=0.5*(\Omega_{ij}*\Omega_{ij}-S_{ij}*S_{ij})=-0.25*du_i/dx_j*du_j/dx_i

  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const int nb_elem = zone_VDF.nb_elem();
  const int nb_elem_tot = zone_VDF.nb_elem_tot();
  int num_elem,i,j;
  double crit,deriv1,deriv2;

  DoubleTab gradient_elem(nb_elem_tot,dimension,dimension);
  gradient_elem=0.;

  Champ_Face& vit = *this;
  const DoubleTab& vitesse = valeurs();

  vit.calcul_duidxj(vitesse,gradient_elem,zone_Cl_VDF);

  for (num_elem=0; num_elem<nb_elem; num_elem++)
    {
      crit = 0.;
      for (i=0; i<dimension; i++)
        for(j=0; j<dimension; j++)
          {
            deriv1 = gradient_elem(num_elem,i,j);
            deriv2 = gradient_elem(num_elem,j,i);

            crit += -0.25*deriv1*deriv2;
          }
      Q[num_elem] = crit;
    }

}



void Champ_Face::calcul_y_plus(DoubleTab& y_plus, const Zone_Cl_VDF& zone_Cl_VDF)
{
  // On initialise le champ y_plus avec une valeur negative,
  // comme ca lorsqu'on veut visualiser le champ pres de la paroi,
  // on n'a qu'a supprimer les valeurs negatives et n'apparaissent
  // que les valeurs aux parois.

  int ndeb,nfin,elem,ori,l_unif;
  double norm_tau,u_etoile,norm_v, dist, val0, val1, val2, d_visco, visco=1.;
  y_plus=-1.;

  const Champ_Face& vit = *this;
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntVect& orientation = zone_VDF.orientation();
  const Equation_base& eqn_hydr = equation();
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible, eqn_hydr.milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
  DoubleTab& tab_visco = ref_cast_non_const(DoubleTab,ch_visco_cin->valeurs());
  //DoubleTab& tab_visco = ch_visco_cin.valeur().valeurs();

  if (sub_type(Champ_Uniforme, ch_visco_cin.valeur()))
    {
      visco = tab_visco(0,0);
      l_unif = 1;
    }
  else
    l_unif = 0;

  // Changer uniquement les valeurs < DMINFLOAT (l'ancien code n'est pas parallele)
  if (!l_unif)
    {
      const int n = tab_visco.size_array();
      ArrOfDouble& v = tab_visco;
      for (int i = 0; i < n; i++)
        if (v[i] < DMINFLOAT)
          v[i] = DMINFLOAT;
    }

  DoubleTab cisaillement(1,1);
  int lp=0;

  const RefObjU& modele_turbulence = eqn_hydr.get_modele(TURBULENCE);
  if (modele_turbulence.non_nul() && sub_type(Mod_turb_hyd_base,modele_turbulence.valeur()))
    {
      const Mod_turb_hyd_base& mod_turb = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur());
      const Turbulence_paroi_base& loipar = mod_turb.loi_paroi();
      if( loipar.use_shear() )
        {
          cisaillement.resize(zone_VDF.nb_faces_bord());
          cisaillement.ref(loipar.Cisaillement_paroi());
          lp=1;
        }
      else lp=0;
    }

  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VDF.les_conditions_limites(n_bord);

      if ( sub_type(Dirichlet_paroi_fixe,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          if (dimension == 2 )
            for (int num_face=ndeb; num_face<nfin; num_face++)
              {
                ori = orientation(num_face);
                if ( (elem =face_voisins(num_face,0)) != -1)
                  norm_v=norm_2D_vit(vit,elem,ori,zone_VDF,val0);
                else
                  {
                    elem = face_voisins(num_face,1);
                    norm_v=norm_2D_vit(vit,elem,ori,zone_VDF,val0);
                  }
                if (axi)
                  dist=zone_VDF.dist_norm_bord_axi(num_face);
                else
                  dist=zone_VDF.dist_norm_bord(num_face);
                if (l_unif)
                  d_visco = visco;
                else
                  d_visco = tab_visco[elem];

                // PQ : 01/10/03 : corrections par rapport a la version premiere

                if(lp)
                  {
                    norm_tau = sqrt(cisaillement(num_face,0)*cisaillement(num_face,0)
                                    +cisaillement(num_face,1)*cisaillement(num_face,1));
                  }
                else  norm_tau = d_visco*norm_v/dist;

                u_etoile = sqrt(norm_tau);
                y_plus(elem) = dist*u_etoile/d_visco;

              }

          else if (dimension == 3)
            for (int num_face=ndeb; num_face<nfin; num_face++)
              {
                ori = orientation(num_face);
                if ( (elem = face_voisins(num_face,0)) != -1)
                  norm_v=norm_3D_vit(vit,elem,ori,zone_VDF,val1,val2);
                else
                  {
                    elem = face_voisins(num_face,1);
                    norm_v=norm_3D_vit(vit,elem,ori,zone_VDF,val1,val2);
                  }
                if (axi)
                  dist=zone_VDF.dist_norm_bord_axi(num_face);
                else
                  dist=zone_VDF.dist_norm_bord(num_face);
                if (l_unif)
                  d_visco = visco;
                else
                  d_visco = tab_visco[elem];

                // PQ : 01/10/03 : corrections par rapport a la version premiere

                if(lp)
                  {
                    norm_tau = sqrt(cisaillement(num_face,0)*cisaillement(num_face,0)
                                    +cisaillement(num_face,1)*cisaillement(num_face,1)
                                    +cisaillement(num_face,2)*cisaillement(num_face,2));
                  }
                else norm_tau = d_visco*norm_v/dist;

                u_etoile = sqrt(norm_tau);
                y_plus(elem) = dist*u_etoile/d_visco;

              }
        } // Fin paroi fixe

    } // Fin boucle sur les bords
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//Methode qui renvoie gij aux elements a partir de la vitesse aux faces
//(gij represente la derivee partielle dui/dxj)
//
//A partir de gij, on peut calculer Sij = 0.5(gij(i,j)+gij(j,i))
//
////////////////////////////////////////////////////////////////////////////////////////////////////

DoubleTab& Champ_Face::calcul_duidxj(const DoubleTab& vitesse, DoubleTab& gij, const Zone_Cl_VDF& zone_Cl_VDF)
{

  Champ_Face& vit = ref_cast(Champ_Face, mon_equation->inconnue().valeur());
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  int nb_elem = zone_VDF.zone().nb_elem_tot();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const IntTab& Qdm = zone_VDF.Qdm();
  const IntVect& orientation = zone_VDF.orientation();

  int premiere_arete_mixte = zone_VDF.premiere_arete_mixte();
  int premiere_arete_interne =  zone_VDF.premiere_arete_interne();
  int derniere_arete_mixte = premiere_arete_mixte+zone_VDF.nb_aretes_mixtes();
  int derniere_arete_interne = premiere_arete_interne+zone_VDF.nb_aretes_internes();
  int elem;
  int num_arete;
  IntVect element(4);

  int num0,num1,num2,num3,signe;
  int ndeb = zone_VDF.premiere_arete_bord();
  int nfin = ndeb + zone_VDF.nb_aretes_bord();
  int n_type;

  gij = 0.;


  ///////////////////////////////////////////////
  // On parcourt toutes les aretes qui permettent
  //  de calculer les termes croises dui/dxj.
  //
  // (les termes non-croises sont calcules
  //  en bouclant sur les elements)
  ///////////////////////////////////////////////

  for (num_arete=ndeb; num_arete<nfin; num_arete++)
    {
      n_type=zone_Cl_VDF.type_arete_bord(num_arete-ndeb);

      if (n_type == 4) // arete de type periodicite
        {
          num0 = Qdm(num_arete,0);
          num1 = Qdm(num_arete,1);
          num2 = Qdm(num_arete,2);
          num3 = Qdm(num_arete,3);
          int i=0;
          int j=0;
          i=orientation(num0);
          j=orientation(num2);

          double temp1 = (vitesse[num1]-vitesse[num0])
                         /zone_VDF.dist_face_period(num0,num1,j);            // dui/dxj
          double temp2 = (vitesse[num3]-vitesse[num2])
                         /zone_VDF.dist_face_period(num2,num3,i);            // duj/dxi
          element(0) = face_voisins(num0,0);
          element(1) = face_voisins(num0,1);
          element(2) = face_voisins(num1,0);
          element(3) = face_voisins(num1,1);
          for (int k=0 ; k<4 ; k++)
            {
              gij(element(k),i,j)+=temp1*0.5*0.25;
              gij(element(k),j,i)+=temp2*0.5*0.25;

              // Justification des coeff derriere :
              // 1) 0.5 : pour la periodicite,
              //           car on distribuera deux fois sur
              //           les elements qui "touchent" cette arete
              //           puisqu'elle existe en double.
              // 2) 0.25 : on distribue le gradient de vitesse
              //            sur les 4 elements qui l'entourent.
            }        // fin du for k
        }
      else
        {
          num0 = Qdm(num_arete,0);
          num1 = Qdm(num_arete,1);
          num2 = Qdm(num_arete,2);
          signe = Qdm(num_arete,3);
          int j=0;
          j=orientation(num2);

          double temp2;
          double temp1 = (vitesse[num1]-vitesse[num0])
                         /zone_VDF.dist_face_period(num0,num1,j);        // dui/dxj
          double vit_imp = 0.5*(vit.val_imp_face_bord_private(num0,j)+
                                vit.val_imp_face_bord_private(num1,j));                // vitesse tangentielle
          //Dans cette partie, on conserve le codage de Hyd_SGE_Wale_VDF (num1 et non num2)
          //pour calculer la distance entre le centre de la maille et le bord.

          temp2 = -signe*(vitesse[num2]-vit_imp)/zone_VDF.dist_norm_bord(num1);

          element(0) = face_voisins(num2,0);
          element(1) = face_voisins(num2,1);

          int i=orientation(num0);

          for (int k=0 ; k<2 ; k++)
            {
              gij(element(k),i,j)+=temp1*0.25;
              gij(element(k),j,i)+=temp2*0.25;

              // Justification des coeff derriere :
              // 1) 0.25 : on distribue le gradient de vitesse
              //            sur les 4 elements qui l'entourent.
            } // fin du for k
        }
    }


  //*******************************
  //On parcourt les aretes coins
  //*******************************

  ndeb = zone_VDF.premiere_arete_coin();
  nfin = ndeb + zone_VDF.nb_aretes_coin();

  for (num_arete=ndeb; num_arete<nfin; num_arete++)
    {
      n_type=zone_Cl_VDF.type_arete_coin(num_arete-ndeb);
      //***************************************
      // Traitement des aretes coin perio-perio
      //***************************************

      if (n_type == 0) // arete de type periodicite-periodicite
        {
          num0 = Qdm(num_arete,0);
          num1 = Qdm(num_arete,1);
          num2 = Qdm(num_arete,2);
          num3 = Qdm(num_arete,3);
          int i=0;
          int j=0;
          i=orientation(num0);
          j=orientation(num2);

          double temp1 = (vitesse[num1]-vitesse[num0])
                         /zone_VDF.dist_face_period(num0,num1,j);        // dui/dxj
          double temp2 = (vitesse[num3]-vitesse[num2])
                         /zone_VDF.dist_face_period(num2,num3,i);        // duj/dxi
          element(0) = face_voisins(num0,0);
          element(1) = face_voisins(num0,1);
          element(2) = face_voisins(num1,0);
          element(3) = face_voisins(num1,1);
          for (int k=0 ; k<4 ; k++)
            {
              gij(element(k),i,j)+=temp1*0.5*0.5*0.25;
              gij(element(k),j,i)+=temp2*0.5*0.5*0.25;

              // Justification des coeff derriere :
              // 1) 0.5 : pour la periodicite,
              //           car on distribuera deux fois sur
              //           les elements qui "touchent" cette arete
              //           puisqu'elle existe en double.
              // 2) 0.5 : idem ci-dessus, car cette fois-ci on a
              //           un coin perio-perio.
              // 3) 0.25 : on distribue le gradient de vitesse
              //            sur les 4 elements qui l'entourent.
            } // fin du for k

        }

      //***************************************
      // Traitement des aretes coin perio-paroi
      //***************************************
      if (n_type == 1) // arete de type periodicite-paroi
        {
          num0 = Qdm(num_arete,0);
          num1 = Qdm(num_arete,1);
          num2 = Qdm(num_arete,2);
          signe = Qdm(num_arete,3);
          int j=0;
          j=orientation(num2);

          double temp2;
          double temp1 = (vitesse[num1]-vitesse[num0])
                         /zone_VDF.dist_face_period(num0,num1,j);                // dui/dxj
          double vit_imp = 0.5*(vit.val_imp_face_bord_private(num0,j)+
                                vit.val_imp_face_bord_private(num1,j));        // vitesse tangentielle

          temp2  = -signe*(vitesse[num2]-vit_imp)/zone_VDF.dist_norm_bord(num1);

          element(0) = face_voisins(num2,0);
          element(1) = face_voisins(num2,1);

          int i=orientation(num1);

          for (int k=0 ; k<2 ; k++)
            {
              gij(element(k),i,j)+=temp1*0.5*0.25;
              gij(element(k),j,i)+=temp2*0.5*0.25;

              // Justification des coeff derriere :
              // 1) 0.5 : pour la periodicite,
              //           car on distribuera deux fois sur
              //           les elements qui "touchent" cette arete
              //           puisqu'elle existe en double.
              // 2) 0.25 : on distribue le gradient de vitesse
              //            sur les 4 elements qui l'entourent.
            } // fin du for k
        }
    }


  // 1ere partie:boucles sur les aretes et remplissage de Sij pour la partie
  // derivees croisees (dui/dxj)
  // vitesse[face] renvoie la vitesse NORMALE a la face
  //   for (num_arete = premiere_arete_bord ; num_arete<derniere_arete_bord ; num_arete ++)
  //     {
  //       const IntVect& type_arete_bord = zone_Cl_VDF.type_arete_bord();


  for(num_arete=premiere_arete_mixte ; num_arete<derniere_arete_mixte ; num_arete ++)
    {
      num0 = Qdm(num_arete,0);
      num1 = Qdm(num_arete,1);
      num2 = Qdm(num_arete,2);
      num3 = Qdm(num_arete,3);
      int i=0;
      int j=0;
      i=orientation(num0);
      j=orientation(num2);

      double temp1 = (vitesse[num1]-vitesse[num0])
                     /zone_VDF.dist_face_period(num0,num1,j);        // dui/dxj
      double temp2 = (vitesse[num3]-vitesse[num2])
                     /zone_VDF.dist_face_period(num2,num3,i);        // duj/dxi

      element(0) = face_voisins(num0,0);
      element(1) = face_voisins(num0,1);
      element(2) = face_voisins(num1,0);
      element(3) = face_voisins(num1,1);

      for (int k=0 ; k<4 ; k++)
        {
          if (element(k)!=-1)
            {
              gij(element(k),i,j)+=temp1*0.25;
              gij(element(k),j,i)+=temp2*0.25;

              // Justification des coeff derriere :
              // 1) 0.25 : on distribue le gradient de vitesse
              //            sur les 3 elements qui l'entourent.
              //            C'est pour cela que l'on regarde si element(k)!=-1,
              //            car dans ce cas la, c'est qu'il s'agit de "la case qui manque" !
            } // fin de else if
        } // fin du for k
    } // fin de la boucle sur les aretes mixtes


  for (num_arete=premiere_arete_interne ; num_arete<derniere_arete_interne ; num_arete ++)
    {
      num0 = Qdm(num_arete,0);
      num1 = Qdm(num_arete,1);
      num2 = Qdm(num_arete,2);
      num3 = Qdm(num_arete,3);
      int i=0;
      int j=0;
      i=orientation(num0);
      j=orientation(num2);

      double temp1 = (vitesse[num1]-vitesse[num0])
                     /zone_VDF.dist_face_period(num0,num1,j);        // dui/dxj
      assert(est_egal(zone_VDF.dist_face_period(num0,num1,j),zone_VDF.dist_face(num0,num1,j)));
      double temp2 = (vitesse[num3]-vitesse[num2])
                     /zone_VDF.dist_face_period(num2,num3,i);        // duj/dxi
      assert(est_egal(zone_VDF.dist_face_period(num2,num3,j),zone_VDF.dist_face(num2,num3,j)));
      element(0) = face_voisins(num0,0);
      element(1) = face_voisins(num0,1);
      element(2) = face_voisins(num1,0);
      element(3) = face_voisins(num1,1);
      for (int k=0 ; k<4 ; k++)
        {
          gij(element(k),i,j)+=temp1*0.25;
          gij(element(k),j,i)+=temp2*0.25;

          // Justification des coeff :
          // 1) 0.25 : on distribue le gradient de vitesse
          //            sur les 4 elements qui l'entourent.
        } // fin du for k
    } // fin de la boucle sur les aretes internes



  // 2eme partie: boucle sur les elements et remplissage de Sij pour les
  //  derivees non croisees (dui/dxi).
  // En fait dans ces cas la, on calcul directement le gradient dans l'element
  //  et on ne redistribue pas.

  for(elem=0 ; elem<nb_elem ; elem ++)
    {
      for (int i = 0 ; i<dimension ; i++)
        {
          double temp1 = (vitesse[elem_faces(elem,i)]
                          -vitesse[elem_faces(elem,i+dimension)])
                         /zone_VDF.dim_elem(elem,orientation(elem_faces(elem,i)));

          gij(elem,i,i)=-temp1;

        }
    }

  return gij;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//Methode qui renvoie gij aux elements a partir de la vitesse aux elements
//(gij represente la derivee partielle dui/dxj)
//
//A partir de gij, on peut calculer Sij = 0.5(gij(i,j)+gij(j,i))
//
////////////////////////////////////////////////////////////////////////////////////////////////////


DoubleTab& Champ_Face::calcul_duidxj(const DoubleTab& in_vel, DoubleTab& gij)
{

  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  int nb_elem_tot = zone_VDF.zone().nb_elem_tot();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();

  int element_number;
  int num0,num1,num2,num3,num4,num5;
  int f0,f1,f2,f3,f4,f5;


  //
  // Calculate the Sij tensor
  //
  if (dimension ==2)
    {
      for (element_number=0 ; element_number<nb_elem_tot ; element_number ++)
        {
          f0 = elem_faces(element_number,0);
          num0 = face_voisins(f0,0);
          if (num0 == -1)
            num0 = element_number;
          f1 = elem_faces(element_number,1);
          num1 = face_voisins(f1,0);
          if (num1 == -1)
            num1 = element_number;
          f2 = elem_faces(element_number,2);
          num2 = face_voisins(f2,1);
          if (num2 == -1)
            num2 = element_number;
          f3 = elem_faces(element_number,3);
          num3 = face_voisins(f3,1);
          if (num3 == -1)
            num3 = element_number;

          gij(element_number,0,0)=0.5*((in_vel(num2,0)-in_vel(num0,0))/
                                       zone_VDF.dim_elem(element_number,0));
          gij(element_number,0,1) = 0.5*((in_vel(num3,0)-in_vel(num1,0))/zone_VDF.dim_elem(element_number,1));
          gij(element_number,1,0) = 0.5*((in_vel(num2,1)-in_vel(num0,1))/zone_VDF.dim_elem(element_number,0));
          gij(element_number,1,1)=0.5*((in_vel(num3,1)-in_vel(num1,1))/
                                       zone_VDF.dim_elem(element_number,1));
        }
    }
  else
    {
      for (element_number=0 ; element_number<nb_elem_tot ; element_number ++)
        {
          f0 = elem_faces(element_number,0);
          num0 = face_voisins(f0,0);
          if (num0 == -1)
            num0 = element_number;
          f1 = elem_faces(element_number,1);
          num1 = face_voisins(f1,0);
          if (num1 == -1)
            num1 = element_number;
          f2 = elem_faces(element_number,2);
          num2 = face_voisins(f2,0);
          if (num2 == -1)
            num2 = element_number;
          f3 = elem_faces(element_number,3);
          num3 = face_voisins(f3,1);
          if (num3 == -1)
            num3 = element_number;
          f4 = elem_faces(element_number,4);
          num4 = face_voisins(f4,1);
          if (num4 == -1)
            num4 = element_number;
          f5 = elem_faces(element_number,5);
          num5 = face_voisins(f5,1);
          if (num5 == -1)
            num5 = element_number;

          gij(element_number,0,0)=0.5*((in_vel(num3,0)-in_vel(num0,0))/
                                       zone_VDF.dim_elem(element_number,0));

          gij(element_number,0,1) = 0.5*((in_vel(num4,0)-in_vel(num1,0))/
                                         zone_VDF.dim_elem(element_number,1));
          gij(element_number,1,0) = 0.5*((in_vel(num3,1)-in_vel(num0,1))/
                                         zone_VDF.dim_elem(element_number,0));

          gij(element_number,0,2) =  0.5*((in_vel(num5,0)-in_vel(num2,0))/
                                          zone_VDF.dim_elem(element_number,2));

          gij(element_number,2,0) = 0.5*((in_vel(num3,2)-in_vel(num0,2))/
                                         zone_VDF.dim_elem(element_number,0));

          gij(element_number,1,1)=0.5*((in_vel(num4,1)-in_vel(num1,1))/
                                       zone_VDF.dim_elem(element_number,1));

          gij(element_number,1,2) = 0.5*((in_vel(num5,1)-in_vel(num2,1))/
                                         zone_VDF.dim_elem(element_number,2));
          gij(element_number,2,1) = 0.5*((in_vel(num4,2)-in_vel(num1,2))/
                                         zone_VDF.dim_elem(element_number,1));

          gij(element_number,2,2)=0.5*((in_vel(num5,2)-in_vel(num2,2))/
                                       zone_VDF.dim_elem(element_number,2));

        }
    }

  return gij;

}
/*

DoubleTab& Champ_Face::calcul_Sij(const DoubleTab& duidxj, DoubleTab& Sij, const Zone_Cl_VDF& zone_Cl_VDF)
{

const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
const int nb_elem_tot = zone_VDF.nb_elem_tot();
int i,j,elem;

for(elem=0 ; elem<nb_elem_tot ; elem ++)
{

for(i=0 ; i<dimension ; i ++)
for(j=0 ; j<dimension ; j ++)
{
Sij(elem,i,j)=0.5*(duidxj(elem,i,j) + duidxj(elem,j,i));
}
}

return Sij;
}
*/


/////////////////////////////////////////////////////////////////////////////////////////////////////
//Methode qui renvoie SMA_barre aux elements a partir de la vitesse aux faces
//SMA_barre = Sij*Sij (sommation sur les indices i et j)
////////////////////////////////////////////////////////////////////////////////////////////////////

DoubleVect& Champ_Face::calcul_S_barre_sans_contrib_paroi(const DoubleTab& vitesse, DoubleVect& SMA_barre, const Zone_Cl_VDF& zone_Cl_VDF)
{
  // On calcule directement S_barre(num_elem)!!!!!!!!!!
  // Le parametre contribution_paroi (ici fixe a 0) permet de ne pas prendre en compte
  // la contribution de la paroi au produit SMA_barre = Sij*Sij

  int contribution_paroi;
  contribution_paroi=0;

  Champ_Face& vit = ref_cast(Champ_Face, mon_equation->inconnue().valeur());
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();

  int nb_elem = zone_VDF.zone().nb_elem();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const IntTab& Qdm = zone_VDF.Qdm();
  const IntVect& orientation = zone_VDF.orientation();

  int premiere_arete_mixte = zone_VDF.premiere_arete_mixte();
  int premiere_arete_interne =  zone_VDF.premiere_arete_interne();
  int derniere_arete_mixte = premiere_arete_mixte+zone_VDF.nb_aretes_mixtes();
  int derniere_arete_interne = premiere_arete_interne+zone_VDF.nb_aretes_internes();
  int elem;
  int num_arete;
  ArrOfInt element(4);

  int num0,num1,num2,num3,signe;
  int ndeb = zone_VDF.premiere_arete_bord();
  int nfin = ndeb + zone_VDF.nb_aretes_bord();
  int n_type;

  for (num_arete=ndeb; num_arete<nfin; num_arete++)
    {
      n_type=zone_Cl_VDF.type_arete_bord(num_arete-ndeb);

      if (n_type == 4) // arete de type periodicite
        {
          num0 = Qdm(num_arete,0);
          num1 = Qdm(num_arete,1);
          num2 = Qdm(num_arete,2);
          num3 = Qdm(num_arete,3);
          int i=0;
          int j=0;
          i=orientation(num0);
          j=orientation(num2);

          double temp1 = (vitesse[num1]-vitesse[num0])
                         /zone_VDF.dist_face_period(num0,num1,j);            // dv/dx
          double temp2 = (vitesse[num3]-vitesse[num2])
                         /zone_VDF.dist_face_period(num2,num3,i);            // du/dy
          element(0) = face_voisins(num0,0);
          element(1) = face_voisins(num0,1);
          element(2) = face_voisins(num1,0);
          element(3) = face_voisins(num1,1);
          for (int k=0 ; k<4 ; k++)
            {
              SMA_barre(element(k))+=0.5*(temp1+temp2)*(temp1+temp2)*0.25;
              // Justification du coeff :
              // on calcule la somme des termes croises :
              // 2*( (0.5*Sij)^2+(0.5*Sji)^2)
              // Comme on est sur les aretes et qu on distribue sur l'element
              // il faut multiplier par 0.25
              // d ou : 0.25*(2*(2*0.5^2))=0.25*4*0.25=0.25!!!!!!
              // le 0.5 devant vient du fait que nous parcourons les faces de periodicite
              // comme les aretes periodiques sont les "memes", on ajoute deux fois ce qu il faut
              // aux elements -> 0.5!!!
            }        // fin du for k
        }
      else
        {
          num0 = Qdm(num_arete,0);
          num1 = Qdm(num_arete,1);
          num2 = Qdm(num_arete,2);
          signe = Qdm(num_arete,3);
          int j=0;
          j=orientation(num2);

          double temp2;
          double temp1 = (vitesse[num1]-vitesse[num0])
                         /zone_VDF.dist_face_period(num0,num1,j);        // dv/dx
          double vit_imp = 0.5*(vit.val_imp_face_bord_private(num0,j)+
                                vit.val_imp_face_bord_private(num1,j));                // vitesse tangentielle

          if ( n_type == 0 && contribution_paroi == 0 )
            temp2 = 0;
          else
            temp2  = -signe*(vitesse[num2]-vit_imp)/zone_VDF.dist_norm_bord(num1);

          element(0) = face_voisins(num2,0);
          element(1) = face_voisins(num2,1);

          for (int k=0 ; k<2 ; k++)
            {
              SMA_barre(element(k))+=(temp1+temp2)*(temp1+temp2)*0.25;
              // Justification du coeff :
              // on calcule la somme des termes croises :
              // 2*( (0.5*Sij)^2+(0.5*Sji)^2)
              // Comme on est sur les aretes et qu on distribue sur l'element
              // il faut multiplier par 0.25
              // d ou : 0.25*(2*(2*0.5^2))=0.25*4*0.25=0.25!!!!!!
              // Prise en compte des 2 termes symetriques : SijSij+SjiSji
            }                // fin du for k
        }
    }


  /////////////////////////////////
  //On parcourt les aretes coins
  ////////////////////////////////

  ndeb = zone_VDF.premiere_arete_coin();
  nfin = ndeb + zone_VDF.nb_aretes_coin();

  for (num_arete=ndeb; num_arete<nfin; num_arete++)
    {
      n_type=zone_Cl_VDF.type_arete_coin(num_arete-ndeb);
      //////////////////////////////////////////
      // Traitement des aretes coin perio-perio
      //////////////////////////////////////////

      if (n_type == 0) // arete de type periodicite-periodicite
        {
          num0 = Qdm(num_arete,0);
          num1 = Qdm(num_arete,1);
          num2 = Qdm(num_arete,2);
          num3 = Qdm(num_arete,3);
          int i=0;
          int j=0;
          i=orientation(num0);
          j=orientation(num2);

          double temp1 = (vitesse[num1]-vitesse[num0])
                         /zone_VDF.dist_face_period(num0,num1,j);            // dv/dx
          double temp2 = (vitesse[num3]-vitesse[num2])
                         /zone_VDF.dist_face_period(num2,num3,i);            // du/dy
          element(0) = face_voisins(num0,0);
          element(1) = face_voisins(num0,1);
          element(2) = face_voisins(num1,0);
          element(3) = face_voisins(num1,1);
          for (int k=0 ; k<4 ; k++)
            {
              SMA_barre(element(k))+=0.5*0.5*(temp1+temp2)*(temp1+temp2)*0.25;
              // Justification du coeff :
              // on calcule la somme des termes croises :
              // 2*( (0.5*Sij)^2+(0.5*Sji)^2)
              // Comme on est sur les aretes et qu on distribue sur l'element
              // il faut multiplier par 0.25
              // d ou : 0.25*(2*(2*0.5^2))=0.25*4*0.25=0.25!!!!!!
              // le 0.5 devant vient du fait que nous parcourons les faces de periodicite
              // comme les aretes periodiques sont les "memes", on ajoute deux fois ce qu il faut
              // aux elements -> 0.5!!!
              // encore un *0.5 car ce sont des aretes perio perio donc que l on parcourt 4 fois!!!!
            }        // fin du for k

        }

      //////////////////////////////////////////
      // Traitement des aretes coin perio-paroi
      //////////////////////////////////////////
      if (n_type == 1) // arete de type periodicite-paroi
        {
          num0 = Qdm(num_arete,0);
          num1 = Qdm(num_arete,1);
          num2 = Qdm(num_arete,2);
          signe = Qdm(num_arete,3);
          int j=0;
          j=orientation(num2);

          double temp2;
          double temp1 = (vitesse[num1]-vitesse[num0])
                         /zone_VDF.dist_face_period(num0,num1,j);        // dv/dx
          double vit_imp = 0.5*(vit.val_imp_face_bord_private(num0,j)+
                                vit.val_imp_face_bord_private(num1,j));             // vitesse tangentielle

          if ( contribution_paroi == 0 )
            temp2 = 0;
          else
            temp2  = -signe*(vitesse[num2]-vit_imp)/zone_VDF.dist_norm_bord(num1);

          element(0) = face_voisins(num2,0);
          element(1) = face_voisins(num2,1);

          for (int k=0 ; k<2 ; k++)
            {
              SMA_barre(element(k))+=0.5*(temp1+temp2)*(temp1+temp2)*0.25;
              // Justification du coeff :
              // on calcule la somme des termes croises :
              // 2*( (0.5*Sij)^2+(0.5*Sji)^2)
              // Comme on est sur les aretes et qu on distribue sur l'element
              // il faut multiplier par 0.25
              // d ou : 0.25*(2*(2*0.5^2))=0.25*4*0.25=0.25!!!!!!
              // Prise en compte des 2 termes symetriques : SijSij+SjiSji
              // encore un *0.5 car ce sont des aretes perio perio donc que l on parcourt 2 fois!!!!

            }                // fin du for k
        }
    }


  // 1ere partie:boucles sur les aretes et remplissage de Sij pour la partie
  // derivees croisees (dv/dx+du/dy)
  // vitesse[face] renvoie la vitesse NORMALE a la face
  //   for (num_arete = premiere_arete_bord ; num_arete<derniere_arete_bord ; num_arete ++)
  //     {
  //       const IntVect& type_arete_bord = zone_Cl_VDF.type_arete_bord();


  for(num_arete=premiere_arete_mixte ; num_arete<derniere_arete_mixte ; num_arete ++)
    {
      num0 = Qdm(num_arete,0);
      num1 = Qdm(num_arete,1);
      num2 = Qdm(num_arete,2);
      num3 = Qdm(num_arete,3);
      int i=0;
      int j=0;
      i=orientation(num0);
      j=orientation(num2);

      double temp1 = (vitesse[num1]-vitesse[num0])
                     /zone_VDF.dist_face_period(num0,num1,j);          // dv/dx
      double temp2 = (vitesse[num3]-vitesse[num2])
                     /zone_VDF.dist_face_period(num2,num3,i);          // du/dy

      element(0) = face_voisins(num0,0);
      element(1) = face_voisins(num0,1);
      element(2) = face_voisins(num1,0);
      element(3) = face_voisins(num1,1);

      for (int k=0 ; k<4 ; k++)
        {
          if (element(k)!=-1)
            {
              SMA_barre(element(k))+=(temp1+temp2)*(temp1+temp2)*0.25;
              // Justification du coeff :
              // on calcule la somme des termes croises :
              // 2*( (0.5*Sij)^2+(0.5*Sji)^2)
              // Comme on est sur les aretes et qu on distribue sur l'element
              // il faut multiplier par 0.25
              // d ou : 0.25*(2*(2*0.5^2))=0.25*4*0.25=0.25!!!!!!
              // Prise en compte des 2 termes symetriques : SijSij+SjiSji
            }        // fin de else if
        }                // fin du for k
    }                // fin de la boucle sur les aretes mixtes


  for (num_arete=premiere_arete_interne ; num_arete<derniere_arete_interne ; num_arete ++)
    {
      num0 = Qdm(num_arete,0);
      num1 = Qdm(num_arete,1);
      num2 = Qdm(num_arete,2);
      num3 = Qdm(num_arete,3);
      int i=0;
      int j=0;
      i=orientation(num0);
      j=orientation(num2);

      double temp1 = (vitesse[num1]-vitesse[num0])
                     /zone_VDF.dist_face_period(num0,num1,j);            // dv/dx
      double temp2 = (vitesse[num3]-vitesse[num2])
                     /zone_VDF.dist_face_period(num2,num3,i);            // du/dy
      element(0) = face_voisins(num0,0);
      element(1) = face_voisins(num0,1);
      element(2) = face_voisins(num1,0);
      element(3) = face_voisins(num1,1);
      for (int k=0 ; k<4 ; k++)
        {
          SMA_barre(element(k))+=(temp1+temp2)*(temp1+temp2)*0.25;
          // Justification du coeff :
          // on calcule la somme des termes croises :
          // 2*( (0.5*Sij)^2+(0.5*Sji)^2)
          // Comme on est sur les aretes et qu on distribue sur l'element
          // il faut multiplier par 0.25
          // d ou : 0.25*(2*(2*0.5^2))=0.25*4*0.25=0.25!!!!!!
          // Prise en compte des 2 termes symetriques : SijSij+SjiSji
        }        // fin du for k
    }        // fin de la boucle sur les aretes internes


  // 2eme partie: boucle sur les elements et remplissage de Sij pour les
  // derivees non croisees (du/dx et dv/dy)

  for(elem=0 ; elem<nb_elem ; elem ++)
    {
      for (int i = 0 ; i<dimension ; i++)
        {
          double temp1 = (vitesse[elem_faces(elem,i)]
                          -vitesse[elem_faces(elem,i+dimension)])
                         /zone_VDF.dim_elem(elem,orientation(elem_faces(elem,i)));
          SMA_barre(elem)+=2.0*temp1*temp1;
        }
    }

  // On prend la racine carre!!!!!  ATTENTION SMA_barre=invariant au carre!!!
  //  racine_carree(SMA_barre)


  return SMA_barre;
}

DoubleVect& Champ_Face::calcul_S_barre(const DoubleTab& vitesse,DoubleVect& SMA_barre,const Zone_Cl_VDF& zone_Cl_VDF)
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const int nb_elem_tot = zone_VDF.nb_elem_tot();
  const int nb_elem = zone_VDF.nb_elem();

  int i,j;
  int elem;
  double Sij,temp;

  DoubleTab duidxj(nb_elem_tot,dimension,dimension);

  calcul_duidxj(vitesse,duidxj,zone_Cl_VDF);

  for (elem=0 ; elem<nb_elem; elem++)
    {
      temp = 0.;
      for ( i=0 ; i<dimension ; i++)
        for ( j=0 ; j<dimension ; j++)
          {
            Sij=0.5*(duidxj(elem,i,j) + duidxj(elem,j,i));
            temp+=Sij*Sij;
          }
      SMA_barre(elem)=2.*temp;
    }

  return SMA_barre;

}


