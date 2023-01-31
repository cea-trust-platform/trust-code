/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <distances_VDF.h>

void moy_2D_vit(const DoubleVect& vit, int elem, int iori, const Zone_VDF& zone, double& u)
{
  int num1, num2;
  if (iori == 0)
    {
      num1 = zone.elem_faces(elem, 1);
      num2 = zone.elem_faces(elem, 3);
    }
  else if (iori == 1)
    {
      num1 = zone.elem_faces(elem, 0);
      num2 = zone.elem_faces(elem, 2);
    }
  else
    {
      Cerr << "valeur de iori " << iori << " impossible en 2D" << finl;
      Process::exit();
      num1 = num2 = -1;
    }
  u = 0.5 * (vit(num1) + vit(num2));
}

double norm_2D_vit(const DoubleVect& vit, int elem, int iori, const Zone_VDF& zone, double& u)
{
  double v;
  moy_2D_vit(vit, elem, iori, zone, v);
  v = std::fabs(v);
  if (v == 0)
    u = 0;
  else
    u = 1;
  return v;
}

double norm_2D_vit(const DoubleVect& vit, int elem, int iori, const Zone_VDF& zone, double u_paroi, double v_paroi, double& u)
{
  double vit_paroi;
  if (iori == 0)
    vit_paroi = v_paroi;
  else if (iori == 1)
    vit_paroi = u_paroi;
  else
    {
      Cerr << "valeur de iori " << iori << " impossible en 2D" << finl;
      Process::exit();
      vit_paroi = 0;
    }
  double v;
  double n_v;
  moy_2D_vit(vit, elem, iori, zone, v);

  //YB:30/01/04:
  //Les valeurs du cisaillement parietal sont maintenant signees.
  //En considerant les valeurs signees de la projection de la vitesse sur le plan parietal
  v = v - vit_paroi;
  n_v = std::fabs(v);

  //Fin modif YB

  if (v == 0)
    u = 0;
  else if (v > 0)
    u = 1;
  else
    u = -1;
  return n_v;
}

void moy_3D_vit(const DoubleVect& vit, int elem, int iori, const Zone_VDF& zone, double& val1, double& val2)
{
  int num1, num2, num3, num4;
  if (iori == 0)
    {
      num1 = zone.elem_faces(elem, 1);
      num2 = zone.elem_faces(elem, 4);
      num3 = zone.elem_faces(elem, 2);
      num4 = zone.elem_faces(elem, 5);
    }
  else if (iori == 1)
    {
      num1 = zone.elem_faces(elem, 0);
      num2 = zone.elem_faces(elem, 3);
      num3 = zone.elem_faces(elem, 2);
      num4 = zone.elem_faces(elem, 5);
    }
  else if (iori == 2)
    {
      num1 = zone.elem_faces(elem, 0);
      num2 = zone.elem_faces(elem, 3);
      num3 = zone.elem_faces(elem, 1);
      num4 = zone.elem_faces(elem, 4);
    }
  else
    {
      Cerr << "valeur de iori " << iori << " impossible en 3D" << finl;
      Process::exit();
      num1 = num2 = num3 = num4 = -1;
    }
  val1 = 0.5 * (vit(num1) + vit(num2));
  val2 = 0.5 * (vit(num3) + vit(num4));
}

double norm_3D_vit(const DoubleVect& vit, int elem, int iori, const Zone_VDF& zone, double& val1, double& val2)
{
  moy_3D_vit(vit, elem, iori, zone, val1, val2);
  double v1 = std::fabs(val1);
  double v2 = std::fabs(val2);
  double norm_vit = sqrt(v1 * v1 + v2 * v2);
  val1 = v1 / (norm_vit + DMINFLOAT);
  val2 = v2 / (norm_vit + DMINFLOAT);
  return norm_vit;
}

double norm_3D_vit(const DoubleVect& vit, int elem, int iori, const Zone_VDF& zone, double u_paroi, double v_paroi, double w_paroi, double& val1, double& val2)
{
  double v1, v2, norm_vit;
  moy_3D_vit(vit, elem, iori, zone, val1, val2);
  if (iori == 0)
    {
      v1 = val1 - u_paroi;
      v2 = val2 - w_paroi;
    }
  else if (iori == 1)
    {
      v1 = val1 - u_paroi;
      v2 = val2 - w_paroi;
    }
  else if (iori == 2)
    {
      v1 = val1 - u_paroi;
      v2 = val2 - v_paroi;
    }
  //Fin modif YB

  else
    {
      Cerr << "valeur de iori " << iori << " impossible en 3D" << finl;
      Process::exit();
      v1 = v2 = 0;
    }
  norm_vit = sqrt(v1 * v1 + v2 * v2);
  val1 = v1 / (norm_vit + DMINFLOAT);
  val2 = v2 / (norm_vit + DMINFLOAT);
  return norm_vit;
}

double norm_vit(const DoubleVect& vit, int elem, int ori, const Zone_VDF& zone, const ArrOfDouble& vit_paroi, ArrOfDouble& val)
{
  if (Objet_U::dimension == 3)
    return norm_3D_vit(vit, elem, ori, zone, vit_paroi[0], vit_paroi[1], vit_paroi[2], val[0], val[1]);
  else
    return norm_2D_vit(vit, elem, ori, zone, vit_paroi[0], vit_paroi[1], val[0]);
}


void calcul_interne2D(int num_elem, int elx0, int elx1, int ely0, int ely1, const Zone_VDF& zone_VDF, const DoubleTab& val, DoubleTab& rot)
{
  const IntTab& elem_faces = zone_VDF.elem_faces();
  double delta_x_0, delta_x_1, delta_y_0, delta_y_1;
  double delta_x, delta_y;
  double deriv_vx, deriv_uy;

  deriv_vx = 0;
  deriv_uy = 0;

  delta_x_0 = zone_VDF.dist_elem_period(num_elem, elx0, 0);
  delta_x_1 = zone_VDF.dist_elem_period(elx1, num_elem, 0);
  delta_y_0 = zone_VDF.dist_elem_period(num_elem, ely0, 1);
  delta_y_1 = zone_VDF.dist_elem_period(ely1, num_elem, 1);

  delta_x = (delta_x_1 - delta_x_0) * (delta_x_1 + delta_x_0) / (delta_x_1 * delta_x_0);
  delta_y = (delta_y_1 - delta_y_0) * (delta_y_1 + delta_y_0) / (delta_y_1 * delta_y_0);

  deriv_vx = (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 1)) + delta_x * val(elem_faces(num_elem, 1)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 1)));
  deriv_vx += (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 3)) + delta_x * val(elem_faces(num_elem, 3)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 3)));
  deriv_vx *= 0.5 / (delta_x_0 + delta_x_1);

  deriv_uy = (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 0)) + delta_y * val(elem_faces(num_elem, 0)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 0)));
  deriv_uy += (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 2)) + delta_y * val(elem_faces(num_elem, 2)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 2)));
  deriv_uy *= 0.5 / (delta_y_0 + delta_y_1);

  rot[num_elem] = deriv_vx - deriv_uy;

}

void calcul_bord2D(int num_elem, int elx0, int elx1, int ely0, int ely1, const Zone_VDF& zone_VDF, const DoubleTab& val, DoubleTab& rot)
{
  const IntTab& elem_faces = zone_VDF.elem_faces();
  double delta_x_0, delta_x_1, delta_y_0, delta_y_1;
  double delta_x, delta_y;
  double deriv_vx, deriv_uy;

  deriv_vx = 0;
  deriv_uy = 0;

  // Traitement des elements bord
  if ((elx0 == -1) || (elx1 == -1))
    {
      if (elx0 == -1)
        {
          if (elx1 != -1)
            {
              delta_x_1 = zone_VDF.dist_elem_period(elx1, num_elem, 0);
              deriv_vx = (val(elem_faces(elx1, 1)) - val(elem_faces(num_elem, 1)) + val(elem_faces(elx1, 3)) - val(elem_faces(num_elem, 3)));
              deriv_vx *= 0.5 / delta_x_1;

            }
          else
            deriv_vx = 0;
        }
      else // elx1 = -1 et elx0 != -1
        {
          delta_x_0 = zone_VDF.dist_elem_period(num_elem, elx0, 0);
          deriv_vx = (val(elem_faces(num_elem, 1)) - val(elem_faces(elx0, 1)) + val(elem_faces(num_elem, 3)) - val(elem_faces(elx0, 3)));
          deriv_vx *= 0.5 / delta_x_0;

        }
    }
  else // elx0 != -1 et elx1 != -1
    {
      delta_x_0 = zone_VDF.dist_elem_period(num_elem, elx0, 0);
      delta_x_1 = zone_VDF.dist_elem_period(elx1, num_elem, 0);
      delta_x = (delta_x_1 - delta_x_0) * (delta_x_1 + delta_x_0) / (delta_x_1 * delta_x_0);
      deriv_vx = (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 1)) + delta_x * val(elem_faces(num_elem, 1)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 1)));
      deriv_vx += (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 3)) + delta_x * val(elem_faces(num_elem, 3)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 3)));
      deriv_vx *= 0.5 / (delta_x_0 + delta_x_1);

    }

  if ((ely0 == -1) || (ely1 == -1))
    {
      if (ely0 == -1)
        {
          if (ely1 != -1)
            {
              delta_y_1 = zone_VDF.dist_elem_period(ely1, num_elem, 1);
              deriv_uy = (val(elem_faces(ely1, 0)) - val(elem_faces(num_elem, 0)) + val(elem_faces(ely1, 2)) - val(elem_faces(num_elem, 2)));
              deriv_uy *= 0.5 / delta_y_1;

            }
          else
            deriv_uy = 0;
        }
      else // ely1 = -1 et ely0 != -1
        {
          delta_y_0 = zone_VDF.dist_elem_period(num_elem, ely0, 1);
          deriv_uy = (val(elem_faces(num_elem, 0)) - val(elem_faces(ely0, 0)) + val(elem_faces(num_elem, 2)) - val(elem_faces(ely0, 2)));
          deriv_uy *= 0.5 / delta_y_0;

        }
    }
  else  // ely0 != -1 et ely1 != -1
    {
      delta_y_0 = zone_VDF.dist_elem_period(num_elem, ely0, 1);
      delta_y_1 = zone_VDF.dist_elem_period(ely1, num_elem, 1);
      delta_y = (delta_y_1 - delta_y_0) * (delta_y_1 + delta_y_0) / (delta_y_1 * delta_y_0);

      deriv_uy = (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 0)) + delta_y * val(elem_faces(num_elem, 0)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 0)));
      deriv_uy += (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 2)) + delta_y * val(elem_faces(num_elem, 2)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 2)));
      deriv_uy *= 0.5 / (delta_y_0 + delta_y_1);

    }

  rot[num_elem] = deriv_vx - deriv_uy;
}

void calrotord2centelemdim2(DoubleTab& rot, const DoubleTab& val, const Zone_VDF& zone_VDF, int nb_elem, const IntTab& face_voisins, const IntTab& elem_faces)
{
  if (rot.dimension(0) != nb_elem)
    rot.resize(nb_elem);
  int elx0, elx1, ely0, ely1;

  for (int num_elem = 0; num_elem < nb_elem; num_elem++)
    {
      elx0 = face_voisins(elem_faces(num_elem, 0), 0);
      elx1 = face_voisins(elem_faces(num_elem, 2), 1);
      ely0 = face_voisins(elem_faces(num_elem, 1), 0);
      ely1 = face_voisins(elem_faces(num_elem, 3), 1);

      if ((elx0 != -1) && (elx1 != -1) && (ely0 != -1) && (ely1 != -1))
        // Cas d'un element interne

        calcul_interne2D(num_elem, elx0, elx1, ely0, ely1, zone_VDF, val, rot);
      else
        calcul_bord2D(num_elem, elx0, elx1, ely0, ely1, zone_VDF, val, rot);
    }
}

// Traitement des elements internes
void calcul_interne3D(int num_elem, int elx0, int elx1, int ely0, int ely1, int elz0, int elz1, const Zone_VDF& zone_VDF, const DoubleTab& val, DoubleTab& rot)
{
  const IntTab& elem_faces = zone_VDF.elem_faces();
  double delta_x_0, delta_x_1, delta_y_0, delta_y_1, delta_z_0, delta_z_1;
  double delta_x, delta_y, delta_z;
  double deriv_wy, deriv_vz, deriv_uz, deriv_wx, deriv_vx, deriv_uy;

  deriv_wy = 0;
  deriv_vz = 0;
  deriv_uz = 0;
  deriv_wx = 0;
  deriv_vx = 0;
  deriv_uy = 0;

  delta_x_0 = zone_VDF.dist_elem_period(num_elem, elx0, 0);
  delta_x_1 = zone_VDF.dist_elem_period(elx1, num_elem, 0);
  delta_y_0 = zone_VDF.dist_elem_period(num_elem, ely0, 1);
  delta_y_1 = zone_VDF.dist_elem_period(ely1, num_elem, 1);
  delta_z_0 = zone_VDF.dist_elem_period(num_elem, elz0, 2);
  delta_z_1 = zone_VDF.dist_elem_period(elz1, num_elem, 2);

  delta_x = (delta_x_1 - delta_x_0) * (delta_x_1 + delta_x_0) / (delta_x_1 * delta_x_0);
  delta_y = (delta_y_1 - delta_y_0) * (delta_y_1 + delta_y_0) / (delta_y_1 * delta_y_0);
  delta_z = (delta_z_1 - delta_z_0) * (delta_z_1 + delta_z_0) / (delta_z_1 * delta_z_0);

  deriv_vz = (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 1)) + delta_z * val(elem_faces(num_elem, 1)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 1)));
  deriv_vz += (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 4)) + delta_z * val(elem_faces(num_elem, 4)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 4)));
  deriv_vz *= 0.5 / (delta_z_0 + delta_z_1);

  deriv_wy = (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 2)) + delta_y * val(elem_faces(num_elem, 2)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 2)));
  deriv_wy += (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 5)) + delta_y * val(elem_faces(num_elem, 5)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 5)));
  deriv_wy *= 0.5 / (delta_y_0 + delta_y_1);

  deriv_uz = (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 0)) + delta_z * val(elem_faces(num_elem, 0)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 0)));
  deriv_uz += (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 3)) + delta_z * val(elem_faces(num_elem, 3)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 3)));
  deriv_uz *= 0.5 / (delta_z_0 + delta_z_1);

  deriv_wx = (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 2)) + delta_x * val(elem_faces(num_elem, 2)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 2)));
  deriv_wx += (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 5)) + delta_x * val(elem_faces(num_elem, 5)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 5)));
  deriv_wx *= 0.5 / (delta_x_0 + delta_x_1);

  deriv_vx = (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 1)) + delta_x * val(elem_faces(num_elem, 1)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 1)));
  deriv_vx += (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 4)) + delta_x * val(elem_faces(num_elem, 4)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 4)));
  deriv_vx *= 0.5 / (delta_x_0 + delta_x_1);

  deriv_uy = (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 0)) + delta_y * val(elem_faces(num_elem, 0)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 0)));
  deriv_uy += (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 3)) + delta_y * val(elem_faces(num_elem, 3)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 3)));
  deriv_uy *= 0.5 / (delta_y_0 + delta_y_1);

  rot(num_elem, 0) = deriv_wy - deriv_vz;
  rot(num_elem, 1) = deriv_uz - deriv_wx;
  rot(num_elem, 2) = deriv_vx - deriv_uy;

}

// Traitement des elements bord
void calcul_bord3D(int num_elem, int elx0, int elx1, int ely0, int ely1, int elz0, int elz1, const Zone_VDF& zone_VDF, const DoubleTab& val, DoubleTab& rot)
{
  const IntTab& elem_faces = zone_VDF.elem_faces();
  double delta_x_0, delta_x_1, delta_y_0, delta_y_1, delta_z_0, delta_z_1;
  double delta_x, delta_y, delta_z;
  double deriv_wy, deriv_vz, deriv_uz, deriv_wx, deriv_vx, deriv_uy;

  deriv_wy = 0;
  deriv_vz = 0;
  deriv_uz = 0;
  deriv_wx = 0;
  deriv_vx = 0;
  deriv_uy = 0;

  // Traitement des elements bord

  if ((elx0 == -1) || (elx1 == -1))
    {
      if (elx0 == -1)
        {
          if (elx1 != -1)
            {
              delta_x_1 = zone_VDF.dist_elem_period(elx1, num_elem, 0);
              deriv_vx = (val(elem_faces(elx1, 1)) - val(elem_faces(num_elem, 1)) + val(elem_faces(elx1, 4)) - val(elem_faces(num_elem, 4)));
              deriv_vx *= 0.5 / delta_x_1;
              deriv_wx = (val(elem_faces(elx1, 2)) - val(elem_faces(num_elem, 2)) + val(elem_faces(elx1, 5)) - val(elem_faces(num_elem, 5)));
              deriv_wx *= 0.5 / delta_x_1;
            }
          else
            {
              deriv_vx = 0;
              deriv_wx = 0;
            }
        }
      else // elx1 = -1 et elx0 != -1
        {
          delta_x_0 = zone_VDF.dist_elem_period(num_elem, elx0, 0);
          deriv_vx = (val(elem_faces(num_elem, 1)) - val(elem_faces(elx0, 1)) + val(elem_faces(num_elem, 4)) - val(elem_faces(elx0, 4)));
          deriv_vx *= 0.5 / delta_x_0;
          deriv_wx = (val(elem_faces(num_elem, 2)) - val(elem_faces(elx0, 2)) + val(elem_faces(num_elem, 5)) - val(elem_faces(elx0, 5)));
          deriv_wx *= 0.5 / delta_x_0;
        }

      if ((ely0 == -1) || (ely1 == -1))
        {
          if (ely0 == -1)
            {
              if (ely1 != -1)
                {
                  delta_y_1 = zone_VDF.dist_elem_period(ely1, num_elem, 1);
                  deriv_uy = (val(elem_faces(ely1, 0)) - val(elem_faces(num_elem, 0)) + val(elem_faces(ely1, 3)) - val(elem_faces(num_elem, 3)));
                  deriv_uy *= 0.5 / delta_y_1;
                  deriv_wy = (val(elem_faces(ely1, 2)) - val(elem_faces(num_elem, 2)) + val(elem_faces(ely1, 5)) - val(elem_faces(num_elem, 5)));
                  deriv_wy *= 0.5 / delta_y_1;
                }
              else
                {
                  deriv_uy = 0;
                  deriv_wy = 0;
                }
            }
          else // ely1 = -1 et ely0 != -1
            {
              delta_y_0 = zone_VDF.dist_elem_period(num_elem, ely0, 1);
              deriv_uy = (val(elem_faces(num_elem, 0)) - val(elem_faces(ely0, 0)) + val(elem_faces(num_elem, 3)) - val(elem_faces(ely0, 3)));
              deriv_uy *= 0.5 / delta_y_0;
              deriv_wy = (val(elem_faces(num_elem, 2)) - val(elem_faces(ely0, 2)) + val(elem_faces(num_elem, 5)) - val(elem_faces(ely0, 5)));
              deriv_wy *= 0.5 / delta_y_0;
            }
        }
      else
        {
          delta_y_0 = zone_VDF.dist_elem_period(num_elem, ely0, 1);
          delta_y_1 = zone_VDF.dist_elem_period(ely1, num_elem, 1);
          delta_y = (delta_y_1 - delta_y_0) * (delta_y_1 + delta_y_0) / (delta_y_1 * delta_y_0);

          deriv_uy = (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 0)) + delta_y * val(elem_faces(num_elem, 0)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 0)));
          deriv_uy += (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 3)) + delta_y * val(elem_faces(num_elem, 3)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 3)));
          deriv_uy *= 0.5 / (delta_y_0 + delta_y_1);

          deriv_wy = (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 2)) + delta_y * val(elem_faces(num_elem, 2)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 2)));
          deriv_wy += (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 5)) + delta_y * val(elem_faces(num_elem, 5)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 5)));
          deriv_wy *= 0.5 / (delta_y_0 + delta_y_1);
        }

      if ((elz0 == -1) || (elz1 == -1))
        {
          if (elz0 == -1)
            {
              if (elz1 != -1)
                {
                  delta_z_1 = zone_VDF.dist_elem_period(elz1, num_elem, 2);
                  deriv_uz = (val(elem_faces(elz1, 0)) - val(elem_faces(num_elem, 0)) + val(elem_faces(elz1, 3)) - val(elem_faces(num_elem, 3)));
                  deriv_uz *= 0.5 / delta_z_1;
                  deriv_vz = (val(elem_faces(elz1, 1)) - val(elem_faces(num_elem, 1)) + val(elem_faces(elz1, 4)) - val(elem_faces(num_elem, 4)));
                  deriv_vz *= 0.5 / delta_z_1;
                }
              else
                {
                  deriv_uz = 0;
                  deriv_vz = 0;
                }
            }
          else // elz1 = -1 et elz0 != -1
            {
              delta_z_0 = zone_VDF.dist_elem_period(num_elem, elz0, 2);
              deriv_uz = (val(elem_faces(num_elem, 0)) - val(elem_faces(elz0, 0)) + val(elem_faces(num_elem, 3)) - val(elem_faces(elz0, 3)));
              deriv_uz *= 0.5 / delta_z_0;
              deriv_vz = (val(elem_faces(num_elem, 1)) - val(elem_faces(elz0, 1)) + val(elem_faces(num_elem, 4)) - val(elem_faces(elz0, 4)));
              deriv_vz *= 0.5 / delta_z_0;
            }
        }
      else
        {
          delta_z_0 = zone_VDF.dist_elem_period(num_elem, elz0, 2);
          delta_z_1 = zone_VDF.dist_elem_period(elz1, num_elem, 2);
          delta_z = (delta_z_1 - delta_z_0) * (delta_z_1 + delta_z_0) / (delta_z_1 * delta_z_0);

          deriv_uz = (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 0)) + delta_z * val(elem_faces(num_elem, 0)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 0)));
          deriv_uz += (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 3)) + delta_z * val(elem_faces(num_elem, 3)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 3)));
          deriv_uz *= 0.5 / (delta_z_0 + delta_z_1);

          deriv_vz = (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 1)) + delta_z * val(elem_faces(num_elem, 1)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 1)));
          deriv_vz += (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 4)) + delta_z * val(elem_faces(num_elem, 4)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 4)));
          deriv_vz *= 0.5 / (delta_z_0 + delta_z_1);
        }

    }
  else if ((ely0 == -1) || (ely1 == -1))
    {

      if (ely0 == -1)
        {
          if (ely1 != -1)
            {
              delta_y_1 = zone_VDF.dist_elem_period(ely1, num_elem, 1);
              deriv_uy = (val(elem_faces(ely1, 0)) - val(elem_faces(num_elem, 0)) + val(elem_faces(ely1, 3)) - val(elem_faces(num_elem, 3)));
              deriv_uy *= 0.5 / delta_y_1;
              deriv_wy = (val(elem_faces(ely1, 2)) - val(elem_faces(num_elem, 2)) + val(elem_faces(ely1, 5)) - val(elem_faces(num_elem, 5)));
              deriv_wy *= 0.5 / delta_y_1;
            }
          else
            {
              deriv_uy = 0;
              deriv_wy = 0;
            }
        }
      else  // ely1 = -1 et ely0 != -1
        {
          delta_y_0 = zone_VDF.dist_elem_period(num_elem, ely0, 1);
          deriv_uy = (val(elem_faces(num_elem, 0)) - val(elem_faces(ely0, 0)) + val(elem_faces(num_elem, 3)) - val(elem_faces(ely0, 3)));
          deriv_uy *= 0.5 / delta_y_0;
          deriv_wy = (val(elem_faces(num_elem, 2)) - val(elem_faces(ely0, 2)) + val(elem_faces(num_elem, 5)) - val(elem_faces(ely0, 5)));
          deriv_wy *= 0.5 / delta_y_0;
        }

      delta_x_0 = zone_VDF.dist_elem_period(num_elem, elx0, 0);
      delta_x_1 = zone_VDF.dist_elem_period(elx1, num_elem, 0);
      delta_x = (delta_x_1 - delta_x_0) * (delta_x_1 + delta_x_0) / (delta_x_1 * delta_x_0);

      deriv_wx = (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 2)) + delta_x * val(elem_faces(num_elem, 2)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 2)));
      deriv_wx += (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 5)) + delta_x * val(elem_faces(num_elem, 5)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 5)));
      deriv_wx *= 0.5 / (delta_x_0 + delta_x_1);

      deriv_vx = (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 1)) + delta_x * val(elem_faces(num_elem, 1)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 1)));
      deriv_vx += (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 4)) + delta_x * val(elem_faces(num_elem, 4)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 4)));
      deriv_vx *= 0.5 / (delta_x_0 + delta_x_1);

      if ((elz0 == -1) || (elz1 == -1))
        {
          if (elz0 == -1)
            {
              if (elz1 != -1)
                {
                  delta_z_1 = zone_VDF.dist_elem_period(elz1, num_elem, 2);
                  deriv_uz = (val(elem_faces(elz1, 0)) - val(elem_faces(num_elem, 0)) + val(elem_faces(elz1, 3)) - val(elem_faces(num_elem, 3)));
                  deriv_uz *= 0.5 / delta_z_1;
                  deriv_vz = (val(elem_faces(elz1, 1)) - val(elem_faces(num_elem, 1)) + val(elem_faces(elz1, 4)) - val(elem_faces(num_elem, 4)));
                  deriv_vz *= 0.5 / delta_z_1;
                }
              else
                {
                  deriv_uz = 0;
                  deriv_vz = 0;
                }
            }
          else
            {
              delta_z_0 = zone_VDF.dist_elem_period(num_elem, elz0, 2);
              deriv_uz = (val(elem_faces(num_elem, 0)) - val(elem_faces(elz0, 0)) + val(elem_faces(num_elem, 3)) - val(elem_faces(elz0, 3)));
              deriv_uz *= 0.5 / delta_z_0;
              deriv_vz = (val(elem_faces(num_elem, 1)) - val(elem_faces(elz0, 1)) + val(elem_faces(num_elem, 4)) - val(elem_faces(elz0, 4)));
              deriv_vz *= 0.5 / delta_z_0;
            }
        }
      else
        {
          delta_z_0 = zone_VDF.dist_elem_period(num_elem, elz0, 2);
          delta_z_1 = zone_VDF.dist_elem_period(elz1, num_elem, 2);
          delta_z = (delta_z_1 - delta_z_0) * (delta_z_1 + delta_z_0) / (delta_z_1 * delta_z_0);

          deriv_uz = (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 0)) + delta_z * val(elem_faces(num_elem, 0)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 0)));
          deriv_uz += (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 3)) + delta_z * val(elem_faces(num_elem, 3)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 3)));
          deriv_uz *= 0.5 / (delta_z_0 + delta_z_1);

          deriv_vz = (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 1)) + delta_z * val(elem_faces(num_elem, 1)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 1)));
          deriv_vz += (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 4)) + delta_z * val(elem_faces(num_elem, 4)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 4)));
          deriv_vz *= 0.5 / (delta_z_0 + delta_z_1);
        }
    }
  else if ((elz0 == -1) || (elz1 == -1))
    {
      if (elz0 == -1)
        {
          if (elz1 != -1)
            {
              delta_z_1 = zone_VDF.dist_elem_period(elz1, num_elem, 2);
              deriv_uz = (val(elem_faces(elz1, 0)) - val(elem_faces(num_elem, 0)) + val(elem_faces(elz1, 3)) - val(elem_faces(num_elem, 3)));
              deriv_uz *= 0.5 / delta_z_1;
              deriv_vz = (val(elem_faces(elz1, 1)) - val(elem_faces(num_elem, 1)) + val(elem_faces(elz1, 4)) - val(elem_faces(num_elem, 4)));
              deriv_vz *= 0.5 / delta_z_1;
            }
          else
            {
              deriv_uz = 0;
              deriv_vz = 0;
            }
        }
      else // elz1 = -1 et elz0 != -1
        {
          delta_z_0 = zone_VDF.dist_elem_period(num_elem, elz0, 2);
          deriv_uz = (val(elem_faces(num_elem, 0)) - val(elem_faces(elz0, 0)) + val(elem_faces(num_elem, 3)) - val(elem_faces(elz0, 3)));
          deriv_uz *= 0.5 / delta_z_0;
          deriv_vz = (val(elem_faces(num_elem, 1)) - val(elem_faces(elz0, 1)) + val(elem_faces(num_elem, 4)) - val(elem_faces(elz0, 4)));
          deriv_vz *= 0.5 / delta_z_0;
        }

      delta_x_0 = zone_VDF.dist_elem_period(num_elem, elx0, 0);
      delta_x_1 = zone_VDF.dist_elem_period(elx1, num_elem, 0);
      delta_x = (delta_x_1 - delta_x_0) * (delta_x_1 + delta_x_0) / (delta_x_1 * delta_x_0);

      deriv_wx = (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 2)) + delta_x * val(elem_faces(num_elem, 2)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 2)));
      deriv_wx += (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 5)) + delta_x * val(elem_faces(num_elem, 5)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 5)));
      deriv_wx *= 0.5 / (delta_x_0 + delta_x_1);

      deriv_vx = (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 1)) + delta_x * val(elem_faces(num_elem, 1)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 1)));
      deriv_vx += (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 4)) + delta_x * val(elem_faces(num_elem, 4)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 4)));
      deriv_vx *= 0.5 / (delta_x_0 + delta_x_1);

      delta_y_0 = zone_VDF.dist_elem_period(num_elem, ely0, 1);
      delta_y_1 = zone_VDF.dist_elem_period(ely1, num_elem, 1);
      delta_y = (delta_y_1 - delta_y_0) * (delta_y_1 + delta_y_0) / (delta_y_1 * delta_y_0);

      deriv_wy = (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 2)) + delta_y * val(elem_faces(num_elem, 2)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 2)));
      deriv_wy += (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 5)) + delta_y * val(elem_faces(num_elem, 5)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 5)));
      deriv_wy *= 0.5 / (delta_y_0 + delta_y_1);

      deriv_uy = (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 0)) + delta_y * val(elem_faces(num_elem, 0)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 0)));
      deriv_uy += (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 3)) + delta_y * val(elem_faces(num_elem, 3)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 3)));
      deriv_uy *= 0.5 / (delta_y_0 + delta_y_1);
    }

  rot(num_elem, 0) = deriv_wy - deriv_vz;
  rot(num_elem, 1) = deriv_uz - deriv_wx;
  rot(num_elem, 2) = deriv_vx - deriv_uy;
}

void calrotord2centelemdim3(DoubleTab& rot, const DoubleTab& val, const Zone_VDF& zone_VDF, int nb_elem, const IntTab& face_voisins, const IntTab& elem_faces)
{
  if (rot.dimension(0) != nb_elem)
    rot.resize(nb_elem, 3);
  int elx0, elx1, ely0, ely1, elz0, elz1;

  for (int num_elem = 0; num_elem < nb_elem; num_elem++)
    {

      elx0 = face_voisins(elem_faces(num_elem, 0), 0);
      elx1 = face_voisins(elem_faces(num_elem, 3), 1);
      ely0 = face_voisins(elem_faces(num_elem, 1), 0);
      ely1 = face_voisins(elem_faces(num_elem, 4), 1);
      elz0 = face_voisins(elem_faces(num_elem, 2), 0);
      elz1 = face_voisins(elem_faces(num_elem, 5), 1);

      if ((elx0 != -1) && (elx1 != -1) && (ely0 != -1) && (ely1 != -1) && (elz0 != -1) && (elz1 != -1))
        // Cas d'un element interne

        calcul_interne3D(num_elem, elx0, elx1, ely0, ely1, elz0, elz1, zone_VDF, val, rot);
      else
        calcul_bord3D(num_elem, elx0, elx1, ely0, ely1, elz0, elz1, zone_VDF, val, rot);
    }
}

// Calcul du produit scalaire du tenseur des vitesses de deformation
// en coordonnees cartesiennes : calcul 2D puis 3D.
// Traitement des elements internes

// Cas 2D
void calcul_dscald_interne2D(int num_elem, int elx0, int elx1, int ely0, int ely1, const Zone_VDF& zone_VDF, const DoubleTab& val, DoubleTab& dscald)
{
  const IntTab& elem_faces = zone_VDF.elem_faces();
  double delta_x_0, delta_x_1, delta_y_0, delta_y_1;
  double delta_x, delta_y;
  double delta_xbis, delta_ybis;
  double deriv_ux, deriv_uy, deriv_vx, deriv_vy;

  deriv_ux = 0;
  deriv_uy = 0;
  deriv_vx = 0;
  deriv_vy = 0;

  delta_x_0 = zone_VDF.dist_elem(num_elem, elx0, 0);
  delta_x_1 = zone_VDF.dist_elem(elx1, num_elem, 0);
  delta_y_0 = zone_VDF.dist_elem(num_elem, ely0, 1);
  delta_y_1 = zone_VDF.dist_elem(ely1, num_elem, 1);

  delta_x = (delta_x_1 - delta_x_0) * (delta_x_1 + delta_x_0) / (delta_x_1 * delta_x_0);
  delta_y = (delta_y_1 - delta_y_0) * (delta_y_1 + delta_y_0) / (delta_y_1 * delta_y_0);

  delta_xbis = zone_VDF.dim_elem(num_elem, 0);
  delta_ybis = zone_VDF.dim_elem(num_elem, 1);

  deriv_vx = (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 1)) + delta_x * val(elem_faces(num_elem, 1)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 1)));
  deriv_vx += (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 3)) + delta_x * val(elem_faces(num_elem, 3)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 3)));
  deriv_vx *= 0.5 / (delta_x_0 + delta_x_1);

  deriv_uy = (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 0)) + delta_y * val(elem_faces(num_elem, 0)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 0)));
  deriv_uy += (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 2)) + delta_y * val(elem_faces(num_elem, 2)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 2)));
  deriv_uy *= 0.5 / (delta_y_0 + delta_y_1);

  deriv_ux = (1. / delta_xbis) * (val(elem_faces(num_elem, 0)) - val(elem_faces(num_elem, 2)));
  deriv_vy = (1. / delta_ybis) * (val(elem_faces(num_elem, 1)) - val(elem_faces(num_elem, 3)));

  dscald[num_elem] = 4. * (deriv_ux * deriv_ux + deriv_vy * deriv_vy) + 2. * ((deriv_uy + deriv_vx) * (deriv_uy + deriv_vx));
}

// Cas 3D
void calcul_dscald_interne3D(int num_elem, int elx0, int elx1, int ely0, int ely1, int elz0, int elz1, const Zone_VDF& zone_VDF, const DoubleTab& val, DoubleTab& dscald)
{
  const IntTab& elem_faces = zone_VDF.elem_faces();
  double delta_x_0, delta_x_1, delta_y_0, delta_y_1, delta_z_0, delta_z_1;
  double delta_x, delta_y, delta_z;
  double delta_xbis, delta_ybis, delta_zbis;
  double deriv_ux, deriv_uy, deriv_uz, deriv_vx, deriv_vy, deriv_vz;
  double deriv_wx, deriv_wy, deriv_wz;

  deriv_ux = 0;
  deriv_uy = 0;
  deriv_uz = 0;
  deriv_vx = 0;
  deriv_vy = 0;
  deriv_vz = 0;
  deriv_wx = 0;
  deriv_wy = 0;
  deriv_wz = 0;

  delta_x_0 = zone_VDF.dist_elem(num_elem, elx0, 0);
  delta_x_1 = zone_VDF.dist_elem(elx1, num_elem, 0);
  delta_y_0 = zone_VDF.dist_elem(num_elem, ely0, 1);
  delta_y_1 = zone_VDF.dist_elem(ely1, num_elem, 1);
  delta_z_0 = zone_VDF.dist_elem(num_elem, elz0, 2);
  delta_z_1 = zone_VDF.dist_elem(elz1, num_elem, 2);

  delta_x = (delta_x_1 - delta_x_0) * (delta_x_1 + delta_x_0) / (delta_x_1 * delta_x_0);
  delta_y = (delta_y_1 - delta_y_0) * (delta_y_1 + delta_y_0) / (delta_y_1 * delta_y_0);
  delta_z = (delta_z_1 - delta_z_0) * (delta_z_1 + delta_z_0) / (delta_z_1 * delta_z_0);

  delta_xbis = zone_VDF.dim_elem(num_elem, 0);
  delta_ybis = zone_VDF.dim_elem(num_elem, 1);
  delta_zbis = zone_VDF.dim_elem(num_elem, 2);

  deriv_vz = (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 1)) + delta_z * val(elem_faces(num_elem, 1)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 1)));
  deriv_vz += (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 4)) + delta_z * val(elem_faces(num_elem, 4)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 4)));
  deriv_vz *= 0.5 / (delta_z_0 + delta_z_1);

  deriv_wy = (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 2)) + delta_y * val(elem_faces(num_elem, 2)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 2)));
  deriv_wy += (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 5)) + delta_y * val(elem_faces(num_elem, 5)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 5)));
  deriv_wy *= 0.5 / (delta_y_0 + delta_y_1);

  deriv_uz = (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 0)) + delta_z * val(elem_faces(num_elem, 0)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 0)));
  deriv_uz += (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 3)) + delta_z * val(elem_faces(num_elem, 3)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 3)));
  deriv_uz *= 0.5 / (delta_z_0 + delta_z_1);

  deriv_wx = (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 2)) + delta_x * val(elem_faces(num_elem, 2)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 2)));
  deriv_wx += (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 5)) + delta_x * val(elem_faces(num_elem, 5)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 5)));
  deriv_wx *= 0.5 / (delta_x_0 + delta_x_1);

  deriv_vx = (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 1)) + delta_x * val(elem_faces(num_elem, 1)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 1)));
  deriv_vx += (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 4)) + delta_x * val(elem_faces(num_elem, 4)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 4)));
  deriv_vx *= 0.5 / (delta_x_0 + delta_x_1);

  deriv_uy = (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 0)) + delta_y * val(elem_faces(num_elem, 0)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 0)));
  deriv_uy += (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 3)) + delta_y * val(elem_faces(num_elem, 3)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 3)));
  deriv_uy *= 0.5 / (delta_y_0 + delta_y_1);

  deriv_ux = (1. / delta_xbis) * val(elem_faces(num_elem, 3)) - val(elem_faces(num_elem, 0));
  deriv_vy = (1. / delta_ybis) * val(elem_faces(num_elem, 4)) - val(elem_faces(num_elem, 1));
  deriv_wz = (1. / delta_zbis) * val(elem_faces(num_elem, 5)) - val(elem_faces(num_elem, 2));

  dscald[num_elem] = 4. * (deriv_ux * deriv_ux + deriv_vy * deriv_vy + deriv_wz * deriv_wz)
                     + 2. * ((deriv_uy + deriv_vx) * (deriv_uy + deriv_vx) + (deriv_wx + deriv_uz) * (deriv_wx + deriv_uz) + (deriv_wy + deriv_vz) * (deriv_wy + deriv_vz));
}

// Traitement des elements bord

// Cas 2D
void calcul_dscald_bord2D(int num_elem, int elx0, int elx1, int ely0, int ely1, const Zone_VDF& zone_VDF, const DoubleTab& val, DoubleTab& dscald)
{
  const IntTab& elem_faces = zone_VDF.elem_faces();
  double delta_x_0, delta_x_1, delta_y_0, delta_y_1;
  double delta_x, delta_y;
  double delta_xbis, delta_ybis;
  double deriv_ux, deriv_vx, deriv_uy, deriv_vy;

  deriv_ux = 0;
  deriv_vx = 0;
  deriv_uy = 0;
  deriv_vy = 0;

  // Traitement des elements bord
  if ((elx0 == -1) || (elx1 == -1))
    {
      if (elx0 == -1)
        {
          if (elx1 != -1)
            {
              delta_x_1 = zone_VDF.dist_elem(elx1, num_elem, 0);
              delta_xbis = zone_VDF.dim_elem(num_elem, 0);

              deriv_vx = (val(elem_faces(elx1, 1)) - val(elem_faces(num_elem, 1)) + val(elem_faces(elx1, 3)) - val(elem_faces(num_elem, 3)));
              deriv_vx *= 0.5 / delta_x_1;
              deriv_ux = (val(elem_faces(num_elem, 2)) - val(elem_faces(num_elem, 0)));
              deriv_ux *= (1. / delta_xbis);

            }
          else
            {
              deriv_ux = 0;
              deriv_vx = 0;
            }
        }
      else // elx1 = -1 et elx0 != -1
        {
          delta_x_0 = zone_VDF.dist_elem(num_elem, elx0, 0);
          delta_xbis = zone_VDF.dim_elem(num_elem, 0);

          deriv_vx = (val(elem_faces(num_elem, 1)) - val(elem_faces(elx0, 1)) + val(elem_faces(num_elem, 3)) - val(elem_faces(elx0, 3)));
          deriv_vx *= 0.5 / delta_x_0;
          deriv_ux = (val(elem_faces(num_elem, 2)) - val(elem_faces(num_elem, 0)));
          deriv_ux *= (1. / delta_xbis);
        }
    }
  else // elx0 != -1 et elx1 != -1
    {
      delta_x_0 = zone_VDF.dist_elem(num_elem, elx0, 0);
      delta_x_1 = zone_VDF.dist_elem(elx1, num_elem, 0);
      delta_x = (delta_x_1 - delta_x_0) * (delta_x_1 + delta_x_0) / (delta_x_1 * delta_x_0);
      delta_xbis = zone_VDF.dim_elem(num_elem, 0);

      deriv_vx = (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 1)) + delta_x * val(elem_faces(num_elem, 1)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 1)));
      deriv_vx += (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 3)) + delta_x * val(elem_faces(num_elem, 3)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 3)));
      deriv_vx *= 0.5 / (delta_x_0 + delta_x_1);
      deriv_ux = (val(elem_faces(num_elem, 2)) - val(elem_faces(num_elem, 0)));
      deriv_ux *= (1. / delta_xbis);

    }

  if ((ely0 == -1) || (ely1 == -1))
    {
      if (ely0 == -1)
        {
          if (ely1 != -1)
            {
              delta_y_1 = zone_VDF.dist_elem(ely1, num_elem, 1);
              delta_ybis = zone_VDF.dim_elem(num_elem, 1);

              deriv_uy = (val(elem_faces(ely1, 0)) - val(elem_faces(num_elem, 0)) + val(elem_faces(ely1, 2)) - val(elem_faces(num_elem, 2)));
              deriv_uy *= 0.5 / delta_y_1;
              deriv_vy = (val(elem_faces(num_elem, 3)) - val(elem_faces(num_elem, 1)));
              deriv_vy *= (1. / delta_ybis);

            }
          else
            {
              deriv_uy = 0;
              deriv_vy = 0;
            }
        }
      else // ely1 = -1 et ely0 != -1
        {
          delta_y_0 = zone_VDF.dist_elem(num_elem, ely0, 1);
          delta_ybis = zone_VDF.dim_elem(num_elem, 1);

          deriv_uy = (val(elem_faces(num_elem, 0)) - val(elem_faces(ely0, 0)) + val(elem_faces(num_elem, 2)) - val(elem_faces(ely0, 2)));
          deriv_uy *= 0.5 / delta_y_0;
          deriv_vy = (val(elem_faces(num_elem, 3)) - val(elem_faces(num_elem, 1)));
          deriv_vy *= (1. / delta_ybis);
        }
    }
  else  // ely0 != -1 et ely1 != -1
    {
      delta_y_0 = zone_VDF.dist_elem(num_elem, ely0, 1);
      delta_y_1 = zone_VDF.dist_elem(ely1, num_elem, 1);
      delta_y = (delta_y_1 - delta_y_0) * (delta_y_1 + delta_y_0) / (delta_y_1 * delta_y_0);
      delta_ybis = zone_VDF.dim_elem(num_elem, 1);

      deriv_uy = (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 0)) + delta_y * val(elem_faces(num_elem, 0)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 0)));
      deriv_uy += (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 2)) + delta_y * val(elem_faces(num_elem, 2)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 2)));
      deriv_uy *= 0.5 / (delta_y_0 + delta_y_1);
      deriv_vy = (val(elem_faces(num_elem, 3)) - val(elem_faces(num_elem, 1)));
      deriv_vy *= (1. / delta_ybis);

    }

  dscald[num_elem] = 4. * (deriv_ux * deriv_ux + deriv_vy * deriv_vy) + 2. * ((deriv_uy + deriv_vx) * (deriv_uy + deriv_vx));
}

// Cas 3D
void calcul_dscald_bord3D(int num_elem, int elx0, int elx1, int ely0, int ely1, int elz0, int elz1, const Zone_VDF& zone_VDF, const DoubleTab& val, DoubleTab& dscald)
{
  const IntTab& elem_faces = zone_VDF.elem_faces();
  double delta_x_0, delta_x_1, delta_y_0, delta_y_1, delta_z_0, delta_z_1;
  double delta_x, delta_y, delta_z;
  double delta_xbis, delta_ybis, delta_zbis;
  double deriv_ux, deriv_uy, deriv_uz, deriv_vx, deriv_vy, deriv_vz;
  double deriv_wx, deriv_wy, deriv_wz;

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

  if ((elx0 == -1) || (elx1 == -1))
    {
      if (elx0 == -1)
        {
          if (elx1 != -1)
            {
              delta_x_1 = zone_VDF.dist_elem(elx1, num_elem, 0);
              delta_xbis = zone_VDF.dim_elem(num_elem, 0);
              // A ecrire et a voir sur papier : calcul de deriv_ux pres de la paroi
              deriv_vx = (val(elem_faces(elx1, 1)) - val(elem_faces(num_elem, 1)) + val(elem_faces(elx1, 4)) - val(elem_faces(num_elem, 4)));
              deriv_vx *= 0.5 / delta_x_1;
              deriv_wx = (val(elem_faces(elx1, 2)) - val(elem_faces(num_elem, 2)) + val(elem_faces(elx1, 5)) - val(elem_faces(num_elem, 5)));
              deriv_wx *= 0.5 / delta_x_1;
              deriv_ux = (val(elem_faces(num_elem, 3)) - val(elem_faces(num_elem, 0)));
              deriv_ux *= (1. / delta_xbis);
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
          delta_x_0 = zone_VDF.dist_elem(num_elem, elx0, 0);
          delta_xbis = zone_VDF.dim_elem(num_elem, 0);

          deriv_vx = (val(elem_faces(num_elem, 1)) - val(elem_faces(elx0, 1)) + val(elem_faces(num_elem, 4)) - val(elem_faces(elx0, 4)));
          deriv_vx *= 0.5 / delta_x_0;
          deriv_wx = (val(elem_faces(num_elem, 2)) - val(elem_faces(elx0, 2)) + val(elem_faces(num_elem, 5)) - val(elem_faces(elx0, 5)));
          deriv_wx *= 0.5 / delta_x_0;
          deriv_ux = (val(elem_faces(num_elem, 3)) - val(elem_faces(num_elem, 0)));
          deriv_ux *= (1. / delta_xbis);
        }

      if ((ely0 == -1) || (ely1 == -1))
        {
          if (ely0 == -1)
            {
              if (ely1 != -1)
                {
                  delta_y_1 = zone_VDF.dist_elem(ely1, num_elem, 1);
                  delta_ybis = zone_VDF.dim_elem(num_elem, 1);

                  deriv_uy = (val(elem_faces(ely1, 0)) - val(elem_faces(num_elem, 0)) + val(elem_faces(ely1, 3)) - val(elem_faces(num_elem, 3)));
                  deriv_uy *= 0.5 / delta_y_1;
                  deriv_wy = (val(elem_faces(ely1, 2)) - val(elem_faces(num_elem, 2)) + val(elem_faces(ely1, 5)) - val(elem_faces(num_elem, 5)));
                  deriv_wy *= 0.5 / delta_y_1;
                  deriv_vy = (val(elem_faces(num_elem, 4)) - val(elem_faces(num_elem, 1)));
                  deriv_vy *= (1. / delta_ybis);
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
              delta_y_0 = zone_VDF.dist_elem(num_elem, ely0, 1);
              delta_ybis = zone_VDF.dim_elem(num_elem, 1);

              deriv_uy = (val(elem_faces(num_elem, 0)) - val(elem_faces(ely0, 0)) + val(elem_faces(num_elem, 3)) - val(elem_faces(ely0, 3)));
              deriv_uy *= 0.5 / delta_y_0;
              deriv_wy = (val(elem_faces(num_elem, 2)) - val(elem_faces(ely0, 2)) + val(elem_faces(num_elem, 5)) - val(elem_faces(ely0, 5)));
              deriv_wy *= 0.5 / delta_y_0;
              deriv_vy = (val(elem_faces(num_elem, 4)) - val(elem_faces(num_elem, 1)));
              deriv_vy *= (1. / delta_ybis);
            }
        }
      else
        {
          delta_y_0 = zone_VDF.dist_elem(num_elem, ely0, 1);
          delta_y_1 = zone_VDF.dist_elem(ely1, num_elem, 1);
          delta_y = (delta_y_1 - delta_y_0) * (delta_y_1 + delta_y_0) / (delta_y_1 * delta_y_0);
          delta_ybis = zone_VDF.dim_elem(num_elem, 1);

          deriv_uy = (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 0)) + delta_y * val(elem_faces(num_elem, 0)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 0)));
          deriv_uy += (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 3)) + delta_y * val(elem_faces(num_elem, 3)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 3)));
          deriv_uy *= 0.5 / (delta_y_0 + delta_y_1);

          deriv_wy = (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 2)) + delta_y * val(elem_faces(num_elem, 2)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 2)));
          deriv_wy += (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 5)) + delta_y * val(elem_faces(num_elem, 5)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 5)));
          deriv_wy *= 0.5 / (delta_y_0 + delta_y_1);
          deriv_vy = (val(elem_faces(num_elem, 4)) - val(elem_faces(num_elem, 1)));
          deriv_vy *= (1. / delta_ybis);
        }

      if ((elz0 == -1) || (elz1 == -1))
        {
          if (elz0 == -1)
            {
              if (elz1 != -1)
                {
                  delta_z_1 = zone_VDF.dist_elem(elz1, num_elem, 2);
                  delta_zbis = zone_VDF.dim_elem(num_elem, 2);

                  deriv_uz = (val(elem_faces(elz1, 0)) - val(elem_faces(num_elem, 0)) + val(elem_faces(elz1, 3)) - val(elem_faces(num_elem, 3)));
                  deriv_uz *= 0.5 / delta_z_1;
                  deriv_vz = (val(elem_faces(elz1, 1)) - val(elem_faces(num_elem, 1)) + val(elem_faces(elz1, 4)) - val(elem_faces(num_elem, 4)));
                  deriv_vz *= 0.5 / delta_z_1;
                  deriv_wz = (val(elem_faces(num_elem, 5)) - val(elem_faces(num_elem, 2)));
                  deriv_wz *= (1. / delta_zbis);
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
              delta_z_0 = zone_VDF.dist_elem(num_elem, elz0, 2);
              delta_zbis = zone_VDF.dim_elem(num_elem, 2);

              deriv_uz = (val(elem_faces(num_elem, 0)) - val(elem_faces(elz0, 0)) + val(elem_faces(num_elem, 3)) - val(elem_faces(elz0, 3)));
              deriv_uz *= 0.5 / delta_z_0;
              deriv_vz = (val(elem_faces(num_elem, 1)) - val(elem_faces(elz0, 1)) + val(elem_faces(num_elem, 4)) - val(elem_faces(elz0, 4)));
              deriv_vz *= 0.5 / delta_z_0;
              deriv_wz = (val(elem_faces(num_elem, 5)) - val(elem_faces(num_elem, 2)));
              deriv_wz *= (1. / delta_zbis);
            }
        }
      else
        {
          delta_z_0 = zone_VDF.dist_elem(num_elem, elz0, 2);
          delta_z_1 = zone_VDF.dist_elem(elz1, num_elem, 2);
          delta_z = (delta_z_1 - delta_z_0) * (delta_z_1 + delta_z_0) / (delta_z_1 * delta_z_0);
          delta_zbis = zone_VDF.dim_elem(num_elem, 2);

          deriv_uz = (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 0)) + delta_z * val(elem_faces(num_elem, 0)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 0)));
          deriv_uz += (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 3)) + delta_z * val(elem_faces(num_elem, 3)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 3)));
          deriv_uz *= 0.5 / (delta_z_0 + delta_z_1);

          deriv_vz = (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 1)) + delta_z * val(elem_faces(num_elem, 1)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 1)));
          deriv_vz += (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 4)) + delta_z * val(elem_faces(num_elem, 4)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 4)));
          deriv_vz *= 0.5 / (delta_z_0 + delta_z_1);
          deriv_wz = (val(elem_faces(num_elem, 5)) - val(elem_faces(num_elem, 2)));
          deriv_wz *= (1. / delta_zbis);
        }

    }
  else if ((ely0 == -1) || (ely1 == -1))
    {

      if (ely0 == -1)
        {
          if (ely1 != -1)
            {
              delta_y_1 = zone_VDF.dist_elem(ely1, num_elem, 1);
              delta_ybis = zone_VDF.dim_elem(num_elem, 1);

              deriv_uy = (val(elem_faces(ely1, 0)) - val(elem_faces(num_elem, 0)) + val(elem_faces(ely1, 3)) - val(elem_faces(num_elem, 3)));
              deriv_uy *= 0.5 / delta_y_1;
              deriv_wy = (val(elem_faces(ely1, 2)) - val(elem_faces(num_elem, 2)) + val(elem_faces(ely1, 5)) - val(elem_faces(num_elem, 5)));
              deriv_wy *= 0.5 / delta_y_1;
              deriv_vy = (val(elem_faces(num_elem, 3)) - val(elem_faces(num_elem, 1)));
              deriv_vy *= (1. / delta_ybis);
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
          delta_y_0 = zone_VDF.dist_elem(num_elem, ely0, 1);
          delta_ybis = zone_VDF.dim_elem(num_elem, 1);

          deriv_uy = (val(elem_faces(num_elem, 0)) - val(elem_faces(ely0, 0)) + val(elem_faces(num_elem, 3)) - val(elem_faces(ely0, 3)));
          deriv_uy *= 0.5 / delta_y_0;
          deriv_wy = (val(elem_faces(num_elem, 2)) - val(elem_faces(ely0, 2)) + val(elem_faces(num_elem, 5)) - val(elem_faces(ely0, 5)));
          deriv_wy *= 0.5 / delta_y_0;
          deriv_vy = (val(elem_faces(num_elem, 3)) - val(elem_faces(num_elem, 1)));
          deriv_vy *= (1. / delta_ybis);
        }

      delta_x_0 = zone_VDF.dist_elem(num_elem, elx0, 0);
      delta_x_1 = zone_VDF.dist_elem(elx1, num_elem, 0);
      delta_x = (delta_x_1 - delta_x_0) * (delta_x_1 + delta_x_0) / (delta_x_1 * delta_x_0);
      delta_xbis = zone_VDF.dim_elem(num_elem, 0);

      deriv_wx = (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 2)) + delta_x * val(elem_faces(num_elem, 2)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 2)));
      deriv_wx += (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 5)) + delta_x * val(elem_faces(num_elem, 5)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 5)));
      deriv_wx *= 0.5 / (delta_x_0 + delta_x_1);

      deriv_vx = (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 1)) + delta_x * val(elem_faces(num_elem, 1)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 1)));
      deriv_vx += (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 4)) + delta_x * val(elem_faces(num_elem, 4)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 4)));
      deriv_vx *= 0.5 / (delta_x_0 + delta_x_1);
      deriv_ux = (val(elem_faces(num_elem, 3)) - val(elem_faces(num_elem, 0)));
      deriv_ux *= (1. / delta_xbis);

      if ((elz0 == -1) || (elz1 == -1))
        {
          if (elz0 == -1)
            {
              if (elz1 != -1)
                {
                  delta_z_1 = zone_VDF.dist_elem(elz1, num_elem, 2);
                  delta_zbis = zone_VDF.dim_elem(num_elem, 2);

                  deriv_uz = (val(elem_faces(elz1, 0)) - val(elem_faces(num_elem, 0)) + val(elem_faces(elz1, 3)) - val(elem_faces(num_elem, 3)));
                  deriv_uz *= 0.5 / delta_z_1;
                  deriv_vz = (val(elem_faces(elz1, 1)) - val(elem_faces(num_elem, 1)) + val(elem_faces(elz1, 4)) - val(elem_faces(num_elem, 4)));
                  deriv_vz *= 0.5 / delta_z_1;
                  deriv_wz = (val(elem_faces(num_elem, 5)) - val(elem_faces(num_elem, 2)));
                  deriv_wz *= (1. / delta_zbis);
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
              delta_z_0 = zone_VDF.dist_elem(num_elem, elz0, 2);
              delta_zbis = zone_VDF.dim_elem(num_elem, 2);

              deriv_uz = (val(elem_faces(num_elem, 0)) - val(elem_faces(elz0, 0)) + val(elem_faces(num_elem, 3)) - val(elem_faces(elz0, 3)));
              deriv_uz *= 0.5 / delta_z_0;
              deriv_vz = (val(elem_faces(num_elem, 1)) - val(elem_faces(elz0, 1)) + val(elem_faces(num_elem, 4)) - val(elem_faces(elz0, 4)));
              deriv_vz *= 0.5 / delta_z_0;
              deriv_wz = (val(elem_faces(num_elem, 5)) - val(elem_faces(num_elem, 2)));
              deriv_wz *= (1. / delta_zbis);
            }
        }
      else
        {
          delta_z_0 = zone_VDF.dist_elem(num_elem, elz0, 2);
          delta_z_1 = zone_VDF.dist_elem(elz1, num_elem, 2);
          delta_z = (delta_z_1 - delta_z_0) * (delta_z_1 + delta_z_0) / (delta_z_1 * delta_z_0);
          delta_zbis = zone_VDF.dim_elem(num_elem, 2);

          deriv_uz = (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 0)) + delta_z * val(elem_faces(num_elem, 0)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 0)));
          deriv_uz += (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 3)) + delta_z * val(elem_faces(num_elem, 3)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 3)));
          deriv_uz *= 0.5 / (delta_z_0 + delta_z_1);

          deriv_vz = (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 1)) + delta_z * val(elem_faces(num_elem, 1)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 1)));
          deriv_vz += (delta_z_0 / delta_z_1 * val(elem_faces(elz1, 4)) + delta_z * val(elem_faces(num_elem, 4)) - delta_z_1 / delta_z_0 * val(elem_faces(elz0, 4)));
          deriv_vz *= 0.5 / (delta_z_0 + delta_z_1);
          deriv_wz = (val(elem_faces(num_elem, 5)) - val(elem_faces(num_elem, 2)));
          deriv_wz *= (1. / delta_zbis);
        }
    }
  else if ((elz0 == -1) || (elz1 == -1))
    {
      if (elz0 == -1)
        {
          if (elz1 != -1)
            {
              delta_z_1 = zone_VDF.dist_elem(elz1, num_elem, 2);
              delta_zbis = zone_VDF.dim_elem(num_elem, 2);

              deriv_uz = (val(elem_faces(elz1, 0)) - val(elem_faces(num_elem, 0)) + val(elem_faces(elz1, 3)) - val(elem_faces(num_elem, 3)));
              deriv_uz *= 0.5 / delta_z_1;
              deriv_vz = (val(elem_faces(elz1, 1)) - val(elem_faces(num_elem, 1)) + val(elem_faces(elz1, 4)) - val(elem_faces(num_elem, 4)));
              deriv_vz *= 0.5 / delta_z_1;
              deriv_wz = (val(elem_faces(num_elem, 5)) - val(elem_faces(num_elem, 2)));
              deriv_wz *= (1. / delta_zbis);
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
          delta_z_0 = zone_VDF.dist_elem(num_elem, elz0, 2);
          delta_zbis = zone_VDF.dim_elem(num_elem, 2);

          deriv_uz = (val(elem_faces(num_elem, 0)) - val(elem_faces(elz0, 0)) + val(elem_faces(num_elem, 3)) - val(elem_faces(elz0, 3)));
          deriv_uz *= 0.5 / delta_z_0;
          deriv_vz = (val(elem_faces(num_elem, 1)) - val(elem_faces(elz0, 1)) + val(elem_faces(num_elem, 4)) - val(elem_faces(elz0, 4)));
          deriv_vz *= 0.5 / delta_z_0;
          deriv_wz = (val(elem_faces(num_elem, 5)) - val(elem_faces(num_elem, 2)));
          deriv_wz *= (1. / delta_zbis);
        }

      delta_x_0 = zone_VDF.dist_elem(num_elem, elx0, 0);
      delta_x_1 = zone_VDF.dist_elem(elx1, num_elem, 0);
      delta_x = (delta_x_1 - delta_x_0) * (delta_x_1 + delta_x_0) / (delta_x_1 * delta_x_0);
      delta_xbis = zone_VDF.dim_elem(num_elem, 0);

      deriv_wx = (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 2)) + delta_x * val(elem_faces(num_elem, 2)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 2)));
      deriv_wx += (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 5)) + delta_x * val(elem_faces(num_elem, 5)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 5)));
      deriv_wx *= 0.5 / (delta_x_0 + delta_x_1);

      deriv_vx = (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 1)) + delta_x * val(elem_faces(num_elem, 1)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 1)));
      deriv_vx += (delta_x_0 / delta_x_1 * val(elem_faces(elx1, 4)) + delta_x * val(elem_faces(num_elem, 4)) - delta_x_1 / delta_x_0 * val(elem_faces(elx0, 4)));
      deriv_vx *= 0.5 / (delta_x_0 + delta_x_1);
      deriv_ux = (val(elem_faces(num_elem, 3)) - val(elem_faces(num_elem, 0)));
      deriv_ux *= (1. / delta_xbis);

      delta_y_0 = zone_VDF.dist_elem(num_elem, ely0, 1);
      delta_y_1 = zone_VDF.dist_elem(ely1, num_elem, 1);
      delta_y = (delta_y_1 - delta_y_0) * (delta_y_1 + delta_y_0) / (delta_y_1 * delta_y_0);
      delta_ybis = zone_VDF.dim_elem(num_elem, 1);

      deriv_wy = (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 2)) + delta_y * val(elem_faces(num_elem, 2)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 2)));
      deriv_wy += (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 5)) + delta_y * val(elem_faces(num_elem, 5)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 5)));
      deriv_wy *= 0.5 / (delta_y_0 + delta_y_1);

      deriv_uy = (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 0)) + delta_y * val(elem_faces(num_elem, 0)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 0)));
      deriv_uy += (delta_y_0 / delta_y_1 * val(elem_faces(ely1, 3)) + delta_y * val(elem_faces(num_elem, 3)) - delta_y_1 / delta_y_0 * val(elem_faces(ely0, 3)));
      deriv_uy *= 0.5 / (delta_y_0 + delta_y_1);
      deriv_vy = (val(elem_faces(num_elem, 4)) - val(elem_faces(num_elem, 1)));
      deriv_ux *= (1. / delta_ybis);
    }

  dscald[num_elem] = 4. * (deriv_ux * deriv_ux + deriv_vy * deriv_vy + deriv_wz * deriv_wz)
                     + 2. * ((deriv_uy + deriv_vx) * (deriv_uy + deriv_vx) + (deriv_wx + deriv_uz) * (deriv_wx + deriv_uz) + (deriv_wy + deriv_vz) * (deriv_wy + deriv_vz));
}

// Cas 2D
void caldscaldcentelemdim2(DoubleTab& dscald, const DoubleTab& val, const Zone_VDF& zone_VDF, int nb_elem, const IntTab& face_voisins, const IntTab& elem_faces)
{
  if (dscald.dimension(0) != nb_elem)
    dscald.resize(nb_elem);
  int elx0, elx1, ely0, ely1;

  for (int num_elem = 0; num_elem < nb_elem; num_elem++)
    {

      elx0 = face_voisins(elem_faces(num_elem, 0), 0);
      elx1 = face_voisins(elem_faces(num_elem, 2), 1);
      ely0 = face_voisins(elem_faces(num_elem, 1), 0);
      ely1 = face_voisins(elem_faces(num_elem, 3), 1);

      if ((elx0 != -1) && (elx1 != -1) && (ely0 != -1) && (ely1 != -1))
        // Cas d'un element interne

        calcul_dscald_interne2D(num_elem, elx0, elx1, ely0, ely1, zone_VDF, val, dscald);
      else
        calcul_dscald_bord2D(num_elem, elx0, elx1, ely0, ely1, zone_VDF, val, dscald);
    }
}

// Cas 3D
void caldscaldcentelemdim3(DoubleTab& dscald, const DoubleTab& val, const Zone_VDF& zone_VDF, int nb_elem, const IntTab& face_voisins, const IntTab& elem_faces)
{
  if (dscald.dimension(0) != nb_elem)
    dscald.resize(nb_elem);
  int elx0, elx1, ely0, ely1, elz0, elz1;

  for (int num_elem = 0; num_elem < nb_elem; num_elem++)
    {

      elx0 = face_voisins(elem_faces(num_elem, 0), 0);
      elx1 = face_voisins(elem_faces(num_elem, 3), 1);
      ely0 = face_voisins(elem_faces(num_elem, 1), 0);
      ely1 = face_voisins(elem_faces(num_elem, 4), 1);
      elz0 = face_voisins(elem_faces(num_elem, 2), 0);
      elz1 = face_voisins(elem_faces(num_elem, 5), 1);

      if ((elx0 != -1) && (elx1 != -1) && (ely0 != -1) && (ely1 != -1) && (elz0 != -1) && (elz1 != -1))
        // Cas d'un element interne

        calcul_dscald_interne3D(num_elem, elx0, elx1, ely0, ely1, elz0, elz1, zone_VDF, val, dscald);
      else
        calcul_dscald_bord3D(num_elem, elx0, elx1, ely0, ely1, elz0, elz1, zone_VDF, val, dscald);
    }
}
