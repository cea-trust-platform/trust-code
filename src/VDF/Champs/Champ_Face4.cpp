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

#include <Champ_Face.h>
#include <Zone_VDF.h>

// Calcul du produit scalaire du tenseur des vitesses de deformation
// en coordonnees cartesiennes : calcul 2D puis 3D.


// Traitement des elements internes

// Cas 2D
void calcul_dscald_interne2D(int num_elem,
                             int elx0,int elx1,
                             int ely0,int ely1,
                             const Zone_VDF& zone_VDF,
                             const DoubleTab& val,DoubleTab& dscald)
{
  const IntTab& elem_faces = zone_VDF.elem_faces();
  double delta_x_0,delta_x_1,delta_y_0,delta_y_1;
  double delta_x,delta_y;
  double delta_xbis,delta_ybis;
  double deriv_ux,deriv_uy,deriv_vx,deriv_vy;

  deriv_ux = 0;
  deriv_uy = 0;
  deriv_vx = 0;
  deriv_vy = 0;

  delta_x_0 = zone_VDF.dist_elem(num_elem,elx0,0);
  delta_x_1 = zone_VDF.dist_elem(elx1,num_elem,0);
  delta_y_0 = zone_VDF.dist_elem(num_elem,ely0,1);
  delta_y_1 = zone_VDF.dist_elem(ely1,num_elem,1);

  delta_x = (delta_x_1-delta_x_0)*(delta_x_1+delta_x_0)/(delta_x_1*delta_x_0);
  delta_y = (delta_y_1-delta_y_0)*(delta_y_1+delta_y_0)/(delta_y_1*delta_y_0);

  delta_xbis = zone_VDF.dim_elem(num_elem,0);
  delta_ybis = zone_VDF.dim_elem(num_elem,1);

  deriv_vx = (delta_x_0/delta_x_1*val(elem_faces(elx1,1))
              + delta_x*val(elem_faces(num_elem,1))
              - delta_x_1/delta_x_0*val(elem_faces(elx0,1)));
  deriv_vx += (delta_x_0/delta_x_1*val(elem_faces(elx1,3))
               + delta_x*val(elem_faces(num_elem,3))
               - delta_x_1/delta_x_0*val(elem_faces(elx0,3)));
  deriv_vx *= 0.5/(delta_x_0 + delta_x_1);

  deriv_uy = (delta_y_0/delta_y_1*val(elem_faces(ely1,0))
              + delta_y*val(elem_faces(num_elem,0))
              - delta_y_1/delta_y_0*val(elem_faces(ely0,0)));
  deriv_uy += (delta_y_0/delta_y_1*val(elem_faces(ely1,2))
               + delta_y*val(elem_faces(num_elem,2))
               - delta_y_1/delta_y_0*val(elem_faces(ely0,2)));
  deriv_uy *= 0.5/(delta_y_0 + delta_y_1);

  deriv_ux = (1./delta_xbis)*(val(elem_faces(num_elem,0))-val(elem_faces(num_elem,2)));
  deriv_vy = (1./delta_ybis)*(val(elem_faces(num_elem,1))-val(elem_faces(num_elem,3)));

  dscald[num_elem] = 4.* (deriv_ux*deriv_ux + deriv_vy*deriv_vy)
                     + 2.*((deriv_uy+deriv_vx)*(deriv_uy+deriv_vx));
}

// Cas 3D
void calcul_dscald_interne3D(int num_elem,
                             int elx0,int elx1,
                             int ely0,int ely1,
                             int elz0,int elz1,
                             const Zone_VDF& zone_VDF,
                             const DoubleTab& val,DoubleTab& dscald)
{
  const IntTab& elem_faces = zone_VDF.elem_faces();
  double delta_x_0,delta_x_1,delta_y_0,delta_y_1,delta_z_0,delta_z_1;
  double delta_x,delta_y,delta_z;
  double delta_xbis,delta_ybis,delta_zbis;
  double deriv_ux,deriv_uy,deriv_uz,deriv_vx,deriv_vy,deriv_vz;
  double deriv_wx,deriv_wy,deriv_wz;

  deriv_ux = 0;
  deriv_uy = 0;
  deriv_uz = 0;
  deriv_vx = 0;
  deriv_vy = 0;
  deriv_vz = 0;
  deriv_wx = 0;
  deriv_wy = 0;
  deriv_wz = 0;


  delta_x_0 = zone_VDF.dist_elem(num_elem,elx0,0);
  delta_x_1 = zone_VDF.dist_elem(elx1,num_elem,0);
  delta_y_0 = zone_VDF.dist_elem(num_elem,ely0,1);
  delta_y_1 = zone_VDF.dist_elem(ely1,num_elem,1);
  delta_z_0 = zone_VDF.dist_elem(num_elem,elz0,2);
  delta_z_1 = zone_VDF.dist_elem(elz1,num_elem,2);

  delta_x = (delta_x_1-delta_x_0)*(delta_x_1+delta_x_0)/(delta_x_1*delta_x_0);
  delta_y = (delta_y_1-delta_y_0)*(delta_y_1+delta_y_0)/(delta_y_1*delta_y_0);
  delta_z = (delta_z_1-delta_z_0)*(delta_z_1+delta_z_0)/(delta_z_1*delta_z_0);


  delta_xbis = zone_VDF.dim_elem(num_elem,0);
  delta_ybis = zone_VDF.dim_elem(num_elem,1);
  delta_zbis = zone_VDF.dim_elem(num_elem,2);


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

  deriv_ux = (1./delta_xbis)*val(elem_faces(num_elem,3))-val(elem_faces(num_elem,0));
  deriv_vy = (1./delta_ybis)*val(elem_faces(num_elem,4))-val(elem_faces(num_elem,1));
  deriv_wz = (1./delta_zbis)*val(elem_faces(num_elem,5))-val(elem_faces(num_elem,2));

  dscald[num_elem] = 4.* (deriv_ux*deriv_ux + deriv_vy*deriv_vy + deriv_wz*deriv_wz)
                     + 2.*((deriv_uy+deriv_vx)*(deriv_uy+deriv_vx)
                           +(deriv_wx+deriv_uz)*(deriv_wx+deriv_uz)
                           +(deriv_wy+deriv_vz)*(deriv_wy+deriv_vz));
}

// Traitement des elements bord

// Cas 2D
void calcul_dscald_bord2D(int num_elem,
                          int elx0,int elx1,
                          int ely0,int ely1,
                          const Zone_VDF& zone_VDF,
                          const DoubleTab& val,DoubleTab& dscald)
{
  const IntTab& elem_faces = zone_VDF.elem_faces();
  double delta_x_0,delta_x_1,delta_y_0,delta_y_1;
  double delta_x,delta_y;
  double delta_xbis,delta_ybis;
  double deriv_ux,deriv_vx,deriv_uy,deriv_vy;

  deriv_ux = 0;
  deriv_vx = 0;
  deriv_uy = 0;
  deriv_vy = 0;

  // Traitement des elements bord
  if ( (elx0 == -1) || (elx1 == -1) )
    {
      if (elx0 == -1)
        {
          if (elx1 != -1)
            {
              delta_x_1 = zone_VDF.dist_elem(elx1,num_elem,0);
              delta_xbis = zone_VDF.dim_elem(num_elem,0);

              deriv_vx = (val(elem_faces(elx1,1))-val(elem_faces(num_elem,1))
                          + val(elem_faces(elx1,3))-val(elem_faces(num_elem,3)));
              deriv_vx *= 0.5/delta_x_1;
              deriv_ux = (val(elem_faces(num_elem,2))-val(elem_faces(num_elem,0)));
              deriv_ux *= (1./delta_xbis);


            }
          else
            {
              deriv_ux = 0;
              deriv_vx = 0;
            }
        }
      else // elx1 = -1 et elx0 != -1
        {
          delta_x_0 = zone_VDF.dist_elem(num_elem,elx0,0);
          delta_xbis = zone_VDF.dim_elem(num_elem,0);

          deriv_vx = (val(elem_faces(num_elem,1))-val(elem_faces(elx0,1))
                      + val(elem_faces(num_elem,3))-val(elem_faces(elx0,3)));
          deriv_vx *= 0.5/delta_x_0;
          deriv_ux = (val(elem_faces(num_elem,2))-val(elem_faces(num_elem,0)));
          deriv_ux *= (1./delta_xbis);
        }
    }
  else // elx0 != -1 et elx1 != -1
    {
      delta_x_0 = zone_VDF.dist_elem(num_elem,elx0,0);
      delta_x_1 = zone_VDF.dist_elem(elx1,num_elem,0);
      delta_x = (delta_x_1-delta_x_0)*(delta_x_1+delta_x_0)/(delta_x_1*delta_x_0);
      delta_xbis = zone_VDF.dim_elem(num_elem,0);

      deriv_vx = (delta_x_0/delta_x_1*val(elem_faces(elx1,1))
                  + delta_x*val(elem_faces(num_elem,1))
                  - delta_x_1/delta_x_0*val(elem_faces(elx0,1)));
      deriv_vx += (delta_x_0/delta_x_1*val(elem_faces(elx1,3))
                   + delta_x*val(elem_faces(num_elem,3))
                   - delta_x_1/delta_x_0*val(elem_faces(elx0,3)));
      deriv_vx *= 0.5/(delta_x_0 + delta_x_1);
      deriv_ux = (val(elem_faces(num_elem,2))-val(elem_faces(num_elem,0)));
      deriv_ux *= (1./delta_xbis);

    }

  if ( (ely0 == -1) || (ely1 == -1) )
    {
      if (ely0 == -1)
        {
          if (ely1 != -1)
            {
              delta_y_1 = zone_VDF.dist_elem(ely1,num_elem,1);
              delta_ybis = zone_VDF.dim_elem(num_elem,1);

              deriv_uy = (val(elem_faces(ely1,0))-val(elem_faces(num_elem,0))
                          + val(elem_faces(ely1,2))-val(elem_faces(num_elem,2)));
              deriv_uy *= 0.5/delta_y_1;
              deriv_vy = (val(elem_faces(num_elem,3))-val(elem_faces(num_elem,1)));
              deriv_vy *= (1./delta_ybis);

            }
          else
            {
              deriv_uy = 0;
              deriv_vy = 0;
            }
        }
      else // ely1 = -1 et ely0 != -1
        {
          delta_y_0 = zone_VDF.dist_elem(num_elem,ely0,1);
          delta_ybis = zone_VDF.dim_elem(num_elem,1);

          deriv_uy = (val(elem_faces(num_elem,0))-val(elem_faces(ely0,0))
                      + val(elem_faces(num_elem,2))-val(elem_faces(ely0,2)));
          deriv_uy *= 0.5/delta_y_0;
          deriv_vy = (val(elem_faces(num_elem,3))-val(elem_faces(num_elem,1)));
          deriv_vy *= (1./delta_ybis);
        }
    }
  else  // ely0 != -1 et ely1 != -1
    {
      delta_y_0 = zone_VDF.dist_elem(num_elem,ely0,1);
      delta_y_1 = zone_VDF.dist_elem(ely1,num_elem,1);
      delta_y = (delta_y_1-delta_y_0)*(delta_y_1+delta_y_0)/(delta_y_1*delta_y_0);
      delta_ybis = zone_VDF.dim_elem(num_elem,1);

      deriv_uy = (delta_y_0/delta_y_1*val(elem_faces(ely1,0))
                  + delta_y*val(elem_faces(num_elem,0))
                  - delta_y_1/delta_y_0*val(elem_faces(ely0,0)));
      deriv_uy += (delta_y_0/delta_y_1*val(elem_faces(ely1,2))
                   + delta_y*val(elem_faces(num_elem,2))
                   - delta_y_1/delta_y_0*val(elem_faces(ely0,2)));
      deriv_uy *= 0.5/(delta_y_0 + delta_y_1);
      deriv_vy = (val(elem_faces(num_elem,3))-val(elem_faces(num_elem,1)));
      deriv_vy *= (1./delta_ybis);

    }

  dscald[num_elem] = 4.* (deriv_ux*deriv_ux + deriv_vy*deriv_vy)
                     + 2.*((deriv_uy+deriv_vx)*(deriv_uy+deriv_vx));
}

// Cas 3D
void calcul_dscald_bord3D(int num_elem,
                          int elx0,int elx1,
                          int ely0,int ely1,
                          int elz0,int elz1,
                          const Zone_VDF& zone_VDF,
                          const DoubleTab& val,DoubleTab& dscald)
{
  const IntTab& elem_faces = zone_VDF.elem_faces();
  double delta_x_0,delta_x_1,delta_y_0,delta_y_1,delta_z_0,delta_z_1;
  double delta_x,delta_y,delta_z;
  double delta_xbis,delta_ybis,delta_zbis;
  double deriv_ux,deriv_uy,deriv_uz,deriv_vx,deriv_vy,deriv_vz;
  double deriv_wx,deriv_wy,deriv_wz;

  deriv_ux = 0;
  deriv_uy = 0;
  deriv_uz = 0;
  deriv_vx = 0;
  deriv_vy = 0;
  deriv_vz = 0;
  deriv_wx = 0;
  deriv_wy = 0;
  deriv_wz = 0;

  // Traitement des elements bord

  if ( (elx0 == -1) || (elx1 == -1) )
    {
      if (elx0 == -1)
        {
          if (elx1 != -1)
            {
              delta_x_1 = zone_VDF.dist_elem(elx1,num_elem,0);
              delta_xbis = zone_VDF.dim_elem(num_elem,0);
              // A ecrire et a voir sur papier : calcul de deriv_ux pres de la paroi
              deriv_vx = (val(elem_faces(elx1,1))-val(elem_faces(num_elem,1))
                          + val(elem_faces(elx1,4))-val(elem_faces(num_elem,4)));
              deriv_vx *= 0.5/delta_x_1;
              deriv_wx = (val(elem_faces(elx1,2))-val(elem_faces(num_elem,2))
                          + val(elem_faces(elx1,5))-val(elem_faces(num_elem,5)));
              deriv_wx *= 0.5/delta_x_1;
              deriv_ux = (val(elem_faces(num_elem,3))-val(elem_faces(num_elem,0)));
              deriv_ux *= (1./delta_xbis);
            }
          else
            {
              deriv_ux = 0;
              deriv_vx = 0;
              deriv_wx = 0;
            }
        }
      else // elx1 = -1 et elx0 != -1
        {
          delta_x_0 = zone_VDF.dist_elem(num_elem,elx0,0);
          delta_xbis = zone_VDF.dim_elem(num_elem,0);

          deriv_vx = (val(elem_faces(num_elem,1))-val(elem_faces(elx0,1))
                      + val(elem_faces(num_elem,4))-val(elem_faces(elx0,4)));
          deriv_vx *= 0.5/delta_x_0;
          deriv_wx = (val(elem_faces(num_elem,2))-val(elem_faces(elx0,2))
                      + val(elem_faces(num_elem,5))-val(elem_faces(elx0,5)));
          deriv_wx *= 0.5/delta_x_0;
          deriv_ux = (val(elem_faces(num_elem,3))-val(elem_faces(num_elem,0)));
          deriv_ux *= (1./delta_xbis);
        }

      if ( (ely0 == -1) || (ely1 == -1) )
        {
          if (ely0 == -1)
            {
              if (ely1 != -1)
                {
                  delta_y_1 = zone_VDF.dist_elem(ely1,num_elem,1);
                  delta_ybis = zone_VDF.dim_elem(num_elem,1);

                  deriv_uy = (val(elem_faces(ely1,0))-val(elem_faces(num_elem,0))
                              + val(elem_faces(ely1,3))-val(elem_faces(num_elem,3)));
                  deriv_uy *= 0.5/delta_y_1;
                  deriv_wy = (val(elem_faces(ely1,2))-val(elem_faces(num_elem,2))
                              + val(elem_faces(ely1,5))-val(elem_faces(num_elem,5)));
                  deriv_wy *= 0.5/delta_y_1;
                  deriv_vy = (val(elem_faces(num_elem,4))-val(elem_faces(num_elem,1)));
                  deriv_vy *= (1./delta_ybis);
                }
              else
                {
                  deriv_uy = 0;
                  deriv_vy = 0;
                  deriv_wy = 0;
                }
            }
          else // ely1 = -1 et ely0 != -1
            {
              delta_y_0 = zone_VDF.dist_elem(num_elem,ely0,1);
              delta_ybis = zone_VDF.dim_elem(num_elem,1);

              deriv_uy = (val(elem_faces(num_elem,0))-val(elem_faces(ely0,0))
                          + val(elem_faces(num_elem,3))-val(elem_faces(ely0,3)));
              deriv_uy *= 0.5/delta_y_0;
              deriv_wy = (val(elem_faces(num_elem,2))-val(elem_faces(ely0,2))
                          + val(elem_faces(num_elem,5))-val(elem_faces(ely0,5)));
              deriv_wy *= 0.5/delta_y_0;
              deriv_vy = (val(elem_faces(num_elem,4))-val(elem_faces(num_elem,1)));
              deriv_vy *= (1./delta_ybis);
            }
        }
      else
        {
          delta_y_0 = zone_VDF.dist_elem(num_elem,ely0,1);
          delta_y_1 = zone_VDF.dist_elem(ely1,num_elem,1);
          delta_y = (delta_y_1-delta_y_0)*(delta_y_1+delta_y_0)/(delta_y_1*delta_y_0);
          delta_ybis = zone_VDF.dim_elem(num_elem,1);

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
          deriv_vy = (val(elem_faces(num_elem,4))-val(elem_faces(num_elem,1)));
          deriv_vy *= (1./delta_ybis);
        }

      if ( (elz0 == -1) || (elz1 == -1) )
        {
          if (elz0 == -1)
            {
              if (elz1 != -1)
                {
                  delta_z_1 = zone_VDF.dist_elem(elz1,num_elem,2);
                  delta_zbis = zone_VDF.dim_elem(num_elem,2);

                  deriv_uz = (val(elem_faces(elz1,0))-val(elem_faces(num_elem,0))
                              + val(elem_faces(elz1,3))-val(elem_faces(num_elem,3)));
                  deriv_uz *= 0.5/delta_z_1;
                  deriv_vz = (val(elem_faces(elz1,1))-val(elem_faces(num_elem,1))
                              + val(elem_faces(elz1,4))-val(elem_faces(num_elem,4)));
                  deriv_vz *= 0.5/delta_z_1;
                  deriv_wz = (val(elem_faces(num_elem,5))-val(elem_faces(num_elem,2)));
                  deriv_wz *= (1./delta_zbis);
                }
              else
                {
                  deriv_uz = 0;
                  deriv_vz = 0;
                  deriv_wz = 0;
                }
            }
          else // elz1 = -1 et elz0 != -1
            {
              delta_z_0 = zone_VDF.dist_elem(num_elem,elz0,2);
              delta_zbis = zone_VDF.dim_elem(num_elem,2);

              deriv_uz = (val(elem_faces(num_elem,0))-val(elem_faces(elz0,0))
                          + val(elem_faces(num_elem,3))-val(elem_faces(elz0,3)));
              deriv_uz *= 0.5/delta_z_0;
              deriv_vz =(val(elem_faces(num_elem,1))-val(elem_faces(elz0,1))
                         + val(elem_faces(num_elem,4))-val(elem_faces(elz0,4)));
              deriv_vz *= 0.5/delta_z_0;
              deriv_wz = (val(elem_faces(num_elem,5))-val(elem_faces(num_elem,2)));
              deriv_wz *= (1./delta_zbis);
            }
        }
      else
        {
          delta_z_0 = zone_VDF.dist_elem(num_elem,elz0,2);
          delta_z_1 = zone_VDF.dist_elem(elz1,num_elem,2);
          delta_z = (delta_z_1-delta_z_0)*(delta_z_1+delta_z_0)/(delta_z_1*delta_z_0);
          delta_zbis = zone_VDF.dim_elem(num_elem,2);

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
          deriv_wz = (val(elem_faces(num_elem,5))-val(elem_faces(num_elem,2)));
          deriv_wz *= (1./delta_zbis);
        }

    }
  else if ( (ely0 == -1) || (ely1 == -1) )
    {

      if (ely0 == -1)
        {
          if (ely1 != -1)
            {
              delta_y_1 = zone_VDF.dist_elem(ely1,num_elem,1);
              delta_ybis = zone_VDF.dim_elem(num_elem,1);

              deriv_uy = (val(elem_faces(ely1,0))-val(elem_faces(num_elem,0))
                          + val(elem_faces(ely1,3))-val(elem_faces(num_elem,3)));
              deriv_uy *= 0.5/delta_y_1;
              deriv_wy = (val(elem_faces(ely1,2))-val(elem_faces(num_elem,2))
                          + val(elem_faces(ely1,5))-val(elem_faces(num_elem,5)));
              deriv_wy *= 0.5/delta_y_1;
              deriv_vy  = (val(elem_faces(num_elem,3))-val(elem_faces(num_elem,1)));
              deriv_vy *= (1./delta_ybis);
            }
          else
            {
              deriv_uy = 0;
              deriv_vy = 0;
              deriv_wy = 0;
            }
        }
      else  // ely1 = -1 et ely0 != -1
        {
          delta_y_0 = zone_VDF.dist_elem(num_elem,ely0,1);
          delta_ybis = zone_VDF.dim_elem(num_elem,1);

          deriv_uy = (val(elem_faces(num_elem,0))-val(elem_faces(ely0,0))
                      + val(elem_faces(num_elem,3))-val(elem_faces(ely0,3)));
          deriv_uy *= 0.5/delta_y_0;
          deriv_wy = (val(elem_faces(num_elem,2))-val(elem_faces(ely0,2))
                      + val(elem_faces(num_elem,5))-val(elem_faces(ely0,5)));
          deriv_wy *= 0.5/delta_y_0;
          deriv_vy  = (val(elem_faces(num_elem,3))-val(elem_faces(num_elem,1)));
          deriv_vy *= (1./delta_ybis);
        }

      delta_x_0 = zone_VDF.dist_elem(num_elem,elx0,0);
      delta_x_1 = zone_VDF.dist_elem(elx1,num_elem,0);
      delta_x = (delta_x_1-delta_x_0)*(delta_x_1+delta_x_0)/(delta_x_1*delta_x_0);
      delta_xbis = zone_VDF.dim_elem(num_elem,0);

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
      deriv_ux  = (val(elem_faces(num_elem,3))-val(elem_faces(num_elem,0)));
      deriv_ux *= (1./delta_xbis);


      if ( (elz0 == -1) || (elz1 == -1) )
        {
          if (elz0 == -1)
            {
              if (elz1 != -1)
                {
                  delta_z_1 = zone_VDF.dist_elem(elz1,num_elem,2);
                  delta_zbis = zone_VDF.dim_elem(num_elem,2);

                  deriv_uz = (val(elem_faces(elz1,0))-val(elem_faces(num_elem,0))
                              + val(elem_faces(elz1,3))-val(elem_faces(num_elem,3)));
                  deriv_uz *= 0.5/delta_z_1;
                  deriv_vz = (val(elem_faces(elz1,1))-val(elem_faces(num_elem,1))
                              + val(elem_faces(elz1,4))-val(elem_faces(num_elem,4)));
                  deriv_vz *= 0.5/delta_z_1;
                  deriv_wz  = (val(elem_faces(num_elem,5))-val(elem_faces(num_elem,2)));
                  deriv_wz *= (1./delta_zbis);
                }
              else
                {
                  deriv_uz = 0;
                  deriv_vz = 0;
                  deriv_wz = 0;
                }
            }
          else
            {
              delta_z_0 = zone_VDF.dist_elem(num_elem,elz0,2);
              delta_zbis = zone_VDF.dim_elem(num_elem,2);

              deriv_uz = (val(elem_faces(num_elem,0))-val(elem_faces(elz0,0))
                          + val(elem_faces(num_elem,3))-val(elem_faces(elz0,3)));
              deriv_uz *= 0.5/delta_z_0;
              deriv_vz =(val(elem_faces(num_elem,1))-val(elem_faces(elz0,1))
                         + val(elem_faces(num_elem,4))-val(elem_faces(elz0,4)));
              deriv_vz *= 0.5/delta_z_0;
              deriv_wz  = (val(elem_faces(num_elem,5))-val(elem_faces(num_elem,2)));
              deriv_wz *= (1./delta_zbis);
            }
        }
      else
        {
          delta_z_0 = zone_VDF.dist_elem(num_elem,elz0,2);
          delta_z_1 = zone_VDF.dist_elem(elz1,num_elem,2);
          delta_z = (delta_z_1-delta_z_0)*(delta_z_1+delta_z_0)/(delta_z_1*delta_z_0);
          delta_zbis = zone_VDF.dim_elem(num_elem,2);

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
          deriv_wz  = (val(elem_faces(num_elem,5))-val(elem_faces(num_elem,2)));
          deriv_wz *= (1./delta_zbis);
        }
    }
  else if ( (elz0 == -1) || (elz1 == -1) )
    {
      if (elz0 == -1)
        {
          if (elz1 != -1)
            {
              delta_z_1 = zone_VDF.dist_elem(elz1,num_elem,2);
              delta_zbis = zone_VDF.dim_elem(num_elem,2);

              deriv_uz = (val(elem_faces(elz1,0))-val(elem_faces(num_elem,0))
                          + val(elem_faces(elz1,3))-val(elem_faces(num_elem,3)));
              deriv_uz *= 0.5/delta_z_1;
              deriv_vz = (val(elem_faces(elz1,1))-val(elem_faces(num_elem,1))
                          + val(elem_faces(elz1,4))-val(elem_faces(num_elem,4)));
              deriv_vz *= 0.5/delta_z_1;
              deriv_wz  = (val(elem_faces(num_elem,5))-val(elem_faces(num_elem,2)));
              deriv_wz *= (1./delta_zbis);
            }
          else
            {
              deriv_uz = 0;
              deriv_vz = 0;
              deriv_wz = 0;
            }
        }
      else // elz1 = -1 et elz0 != -1
        {
          delta_z_0 = zone_VDF.dist_elem(num_elem,elz0,2);
          delta_zbis = zone_VDF.dim_elem(num_elem,2);

          deriv_uz = (val(elem_faces(num_elem,0))-val(elem_faces(elz0,0))
                      + val(elem_faces(num_elem,3))-val(elem_faces(elz0,3)));
          deriv_uz *= 0.5/delta_z_0;
          deriv_vz =(val(elem_faces(num_elem,1))-val(elem_faces(elz0,1))
                     + val(elem_faces(num_elem,4))-val(elem_faces(elz0,4)));
          deriv_vz *= 0.5/delta_z_0;
          deriv_wz  = (val(elem_faces(num_elem,5))-val(elem_faces(num_elem,2)));
          deriv_wz *= (1./delta_zbis);
        }

      delta_x_0 = zone_VDF.dist_elem(num_elem,elx0,0);
      delta_x_1 = zone_VDF.dist_elem(elx1,num_elem,0);
      delta_x = (delta_x_1-delta_x_0)*(delta_x_1+delta_x_0)/(delta_x_1*delta_x_0);
      delta_xbis = zone_VDF.dim_elem(num_elem,0);

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
      deriv_ux  = (val(elem_faces(num_elem,3))-val(elem_faces(num_elem,0)));
      deriv_ux *= (1./delta_xbis);

      delta_y_0 = zone_VDF.dist_elem(num_elem,ely0,1);
      delta_y_1 = zone_VDF.dist_elem(ely1,num_elem,1);
      delta_y = (delta_y_1-delta_y_0)*(delta_y_1+delta_y_0)/(delta_y_1*delta_y_0);
      delta_ybis = zone_VDF.dim_elem(num_elem,1);

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
      deriv_vy  = (val(elem_faces(num_elem,4))-val(elem_faces(num_elem,1)));
      deriv_ux *= (1./delta_ybis);
    }

  dscald[num_elem] = 4.* (deriv_ux*deriv_ux + deriv_vy*deriv_vy + deriv_wz*deriv_wz)
                     + 2.*((deriv_uy+deriv_vx)*(deriv_uy+deriv_vx)
                           +(deriv_wx+deriv_uz)*(deriv_wx+deriv_uz)
                           +(deriv_wy+deriv_vz)*(deriv_wy+deriv_vz));
}

// Cas 2D
void caldscaldcentelemdim2(DoubleTab& dscald, const DoubleTab& val,
                           const Zone_VDF& zone_VDF, int nb_elem,
                           const IntTab& face_voisins, const IntTab& elem_faces)
{
  if(dscald.dimension(0)!=nb_elem)
    dscald.resize(nb_elem);
  int elx0,elx1,ely0,ely1;

  for (int num_elem=0; num_elem<nb_elem; num_elem++)
    {

      elx0 = face_voisins(elem_faces(num_elem,0),0);
      elx1 = face_voisins(elem_faces(num_elem,2),1);
      ely0 = face_voisins(elem_faces(num_elem,1),0);
      ely1 = face_voisins(elem_faces(num_elem,3),1);

      if ( (elx0 != -1) && (elx1 != -1) && (ely0 != -1)
           && (ely1 != -1))
        // Cas d'un element interne

        calcul_dscald_interne2D(num_elem,elx0,elx1,ely0,ely1,zone_VDF,val,dscald);
      else
        calcul_dscald_bord2D(num_elem,elx0,elx1,ely0,ely1,zone_VDF,val,dscald);
    }
}


// Cas 3D
void caldscaldcentelemdim3(DoubleTab& dscald, const DoubleTab& val, const Zone_VDF& zone_VDF,
                           int nb_elem, const IntTab& face_voisins, const IntTab& elem_faces)
{
  if(dscald.dimension(0)!=nb_elem)
    dscald.resize(nb_elem);
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

        calcul_dscald_interne3D(num_elem,elx0,elx1,ely0,ely1,elz0,elz1,zone_VDF,val,dscald);
      else
        calcul_dscald_bord3D(num_elem,elx0,elx1,ely0,ely1,elz0,elz1,zone_VDF,val,dscald);
    }
}



void Champ_Face::calculer_dscald_centre_element(DoubleTab& dscald) const
{
  const DoubleTab& val = valeurs();
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  int nb_elem = zone_VDF.nb_elem();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();

  if (dimension == 2 )
    caldscaldcentelemdim2(dscald, val, zone_VDF, nb_elem, face_voisins, elem_faces);
  else if (dimension == 3)
    caldscaldcentelemdim3(dscald, val, zone_VDF, nb_elem, face_voisins, elem_faces);
}

