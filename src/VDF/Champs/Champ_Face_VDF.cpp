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

#include <Dirichlet_paroi_defilante.h>
#include <Dirichlet_entree_fluide_leaves.h>
#include <Champ_Uniforme_Morceaux.h>
#include <Dirichlet_paroi_fixe.h>
#include <Mod_turb_hyd_base.h>
#include <Champ_Uniforme.h>
#include <Champ_Face_VDF.h>
#include <distances_VDF.h>
#include <Equation_base.h>
#include <Champ_Don_lu.h>
#include <Fluide_base.h>
#include <Zone_Cl_VDF.h>
#include <Periodique.h>
#include <Symetrie.h>

/* ***************************************************** */
/*           METHODES UTILES MAIS HORS CLASSE            */
/* ***************************************************** */

double Champ_Face_get_val_imp_face_bord_sym(const DoubleTab& tab_valeurs, const double temp, int face, int comp, const Zone_Cl_VDF& zclo)
{
  const Zone_VDF& zone_vdf = zclo.zone_VDF();
  int face_globale, face_locale;

  face_globale = face + zone_vdf.premiere_face_bord(); // Maintenant numero dans le tableau global des faces.
  const Zone_Cl_dis_base& zcl = zclo; //equation().zone_Cl_dis().valeur();
  // On recupere la CL associee a la face et le numero local de la face dans la frontiere.
  //assert(equation().zone_Cl_dis().valeur()==zclo);

  const Cond_lim_base& cl = (face < zone_vdf.nb_faces()) ? zcl.condition_limite_de_la_face_reelle(face_globale, face_locale) : zcl.condition_limite_de_la_face_virtuelle(face_globale, face_locale);

  const IntTab& face_voisins = zone_vdf.face_voisins();
  const IntTab& elem_faces = zone_vdf.elem_faces();
  const DoubleVect& porosite = zclo.equation().milieu().porosite_face();
  int ori = zone_vdf.orientation()(face_globale);

  const DoubleTab& vals = cl.champ_front()->valeurs_au_temps(temp);

  int face_de_vals = vals.dimension(0) == 1 ? 0 : face_locale;

  if (sub_type(Symetrie, cl))
    {
      if (comp == ori)
        return 0;
      else
        {
          int elem = 0;
          if (face_voisins(face_globale, 0) != -1)
            elem = face_voisins(face_globale, 0);
          else
            elem = face_voisins(face_globale, 1);
          int comp2 = comp + Objet_U::dimension;
          return (tab_valeurs(elem_faces(elem, comp)) * porosite[elem_faces(elem, comp)] + tab_valeurs(elem_faces(elem, comp2)) * porosite[elem_faces(elem, comp2)])
                 / (porosite[elem_faces(elem, comp)] + porosite[elem_faces(elem, comp2)]);
        }
    }
  else if (sub_type(Dirichlet_entree_fluide, cl))
    return vals(face_de_vals, comp);
  else if (sub_type(Dirichlet_paroi_fixe, cl))
    return 0;
  else if (sub_type(Dirichlet_paroi_defilante, cl))
    return vals(face_de_vals, comp);

  return 0; // All other cases
}

double Champ_Face_get_val_imp_face_bord(const double temp, int face, int comp, const Zone_Cl_VDF& zclo)
{
  const Zone_VDF& zone_vdf = zclo.zone_VDF();
  int face_globale, face_locale;

  face_globale = face + zone_vdf.premiere_face_bord(); // Maintenant numero dans le tableau global des faces.
  const Zone_Cl_dis_base& zcl = zclo; //equation().zone_Cl_dis().valeur();
  // On recupere la CL associee a la face et le numero local de la face dans la frontiere.
  //assert(equation().zone_Cl_dis().valeur()==zclo);

  const Cond_lim_base& cl = (face < zone_vdf.nb_faces()) ? zcl.condition_limite_de_la_face_reelle(face_globale, face_locale) : zcl.condition_limite_de_la_face_virtuelle(face_globale, face_locale);
  int ori = zone_vdf.orientation()(face_globale);
  const DoubleTab& vals = cl.champ_front()->valeurs_au_temps(temp);
  int face_de_vals = vals.dimension(0) == 1 ? 0 : face_locale;

  if (sub_type(Symetrie, cl))
    {
      if (comp == ori)
        return 0;
      else
        {
          Cerr << "You should call Champ_Face_get_val_imp_face_bord_sym and not  Champ_Face_get_val_imp_face_bord" << finl;
          Process::exit();
          return 1e9;
        }
    }
  else if (sub_type(Dirichlet_entree_fluide, cl))
    return vals(face_de_vals, comp);
  else if (sub_type(Dirichlet_paroi_fixe, cl))
    return 0;
  else if (sub_type(Dirichlet_paroi_defilante, cl))
    return vals(face_de_vals, comp);

  return 0; // All other cases
}

double Champ_Face_get_val_imp_face_bord(const double temp, int face, int comp, int comp2, const Zone_Cl_VDF& zclo)
{
  Cerr << "Champ_Face_VDF::val_imp_face_bord(,,) exit" << finl;
  Process::exit();
  return 0; // For compilers
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

/* ***************************************************** */
/*           FIN METHODES UTILES HORS CLASSE             */
/* ***************************************************** */

// XXX : Elie Saikali : je garde Champ_Face aussi sinon faut changer typer et reprise dans bcp des endroits ...
Implemente_instanciable(Champ_Face_VDF,"Champ_Face|Champ_Face_VDF",Champ_Face_base);

Sortie& Champ_Face_VDF::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Champ_Face_VDF::readOn(Entree& s)
{
  lire_donnees (s) ;
  return s ;
}

int Champ_Face_VDF::fixer_nb_valeurs_nodales(int nb_noeuds)
{
  assert(nb_noeuds == zone_vdf().nb_faces());
  const MD_Vector& md = zone_vdf().md_vector_faces();
  // Probleme: nb_comp vaut 2 mais on ne veut qu'une dimension !!!
  // HACK :
  int old_nb_compo = nb_compo_;
  if(nb_compo_ >= dimension) nb_compo_ /= dimension;
  creer_tableau_distribue(md);
  nb_compo_ = old_nb_compo;
  return nb_noeuds;
}

void Champ_Face_VDF::dimensionner_tenseur_Grad()
{
  tau_diag_.resize(zone_vdf().nb_elem(), dimension);
  tau_croises_.resize(zone_vdf().nb_aretes(), 2);
}

Champ_base& Champ_Face_VDF::affecter_(const Champ_base& ch)
{
  DoubleTab& val = valeurs();
  const Zone_VDF& zone_VDF = zone_vdf();
  const IntVect& orientation = zone_VDF.orientation();
  const int N = val.dimension(1), unif = sub_type(Champ_Uniforme, ch), D = dimension;

  if ((sub_type(Champ_Uniforme_Morceaux, ch)) || (sub_type(Champ_Don_lu, ch)))
    {
      const DoubleTab& v = ch.valeurs();
      int ndeb_int = zone_VDF.premiere_face_int();
      const IntTab& f_e = zone_VDF.face_voisins();

      for (int f = 0; f < ndeb_int; f++)
        {
          const int ori = orientation(f);
          const int e = f_e(f, f_e(f, 0) == -1);
          for (int n = 0; n < N; n++)
            val(f, n) = v(e, N * ori + n);
        }

      for (int f = ndeb_int; f < zone_VDF.nb_faces(); f++)
        {
          const int ori = orientation(f);
          for (int n = 0; n < N; n++)
            val(f, n) = 0.5 * (v(f_e(f, 0), N * ori + n) + v(f_e(f, 1), N * ori + n));
        }
    }
  else
    {

      DoubleTab eval;
      if (unif) eval = ch.valeurs();
      else eval.resize(val.dimension(0), N * D), ch.valeur_aux(zone_VDF.xv(), eval);

      for (int f = 0; f < zone_VDF.nb_faces(); f++)
        for (int n = 0; n < N; n++)
          val(f, n) = eval(unif ? 0 : f, N * orientation(f) + n);
    }
  val.echange_espace_virtuel();
  return *this;
}

const Champ_Proto& Champ_Face_VDF::affecter(const double x1, const double x2)
{
  const IntVect& orientation = zone_vdf().orientation();
  DoubleTab& val = valeurs();
  for (int num_face = 0; num_face < val.size(); num_face++)
    {
      int ori = orientation(num_face);
      switch(ori)
        {
        case 0:
          val(num_face) = x1;
          break;
        case 1:
          val(num_face) = x2;
          break;
        }
    }
  return *this;
}

const Champ_Proto& Champ_Face_VDF::affecter(const double x1, const double x2, const double x3)
{
  const IntVect& orientation = zone_vdf().orientation();
  DoubleTab& val = valeurs();
  for (int num_face = 0; num_face < val.size(); num_face++)
    {
      int ori = orientation(num_face);
      switch(ori)
        {
        case 0:
          val(num_face) = x1;
          break;
        case 1:
          val(num_face) = x2;
          break;
        case 2:
          val(num_face) = x3;
          break;
        }
    }
  return *this;
}

const Champ_Proto& Champ_Face_VDF::affecter(const DoubleTab& v)
{
  const IntVect& orientation = zone_vdf().orientation();
  DoubleTab& val = valeurs();

  if (v.nb_dim() == 2)
    {
      if (v.dimension(1) == dimension)
        {
          if (v.dimension(0) == val.size())
            for (int num_face = 0; num_face < val.size(); num_face++)
              val(num_face) = v(num_face, orientation(num_face));
          else
            {
              Cerr << "Erreur TRUST dans Champ_Face_VDF::affecter(const DoubleTab& )" << finl;
              Cerr << "les dimensions du DoubleTab passe en parametre sont incompatibles " << finl;
              Cerr << "avec celles du Champ_Face_VDF " << finl;
              Process::exit();
            }
        }
      else
        {
          Cerr << "Erreur TRUST dans Champ_Face_VDF::affecter(const DoubleTab& )" << finl;
          Cerr << "les dimensions du DoubleTab passe en parametre sont incompatibles " << finl;
          Cerr << "avec celles du Champ_Face_VDF " << finl;
          Process::exit();
        }
    }
  return *this;
}

//-Cas CL periodique : assure que les valeurs sur des faces periodiques
// en vis a vis sont identiques. Pour cela on prend la demi somme des deux valeurs.
void Champ_Face_VDF::verifie_valeurs_cl()
{
  const Zone_Cl_dis_base& zcl = zone_Cl_dis().valeur();
  int nb_cl = zcl.nb_cond_lim();
  DoubleTab& ch_tab = valeurs();
  int ndeb, nfin, num_face;

  for (int i = 0; i < nb_cl; i++)
    {
      const Cond_lim_base& la_cl = zcl.les_conditions_limites(i).valeur();
      if (sub_type(Periodique, la_cl))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl);
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();
          int voisine;
          double moy;

          for (num_face = ndeb; num_face < nfin; num_face++)
            {
              voisine = la_cl_perio.face_associee(num_face - ndeb) + ndeb;
              if (ch_tab[num_face] != ch_tab[voisine])
                {
                  moy = 0.5 * (ch_tab[num_face] + ch_tab[voisine]);
                  ch_tab[num_face] = moy;
                  ch_tab[voisine] = moy;
                }
            }
        }
    }
  ch_tab.echange_espace_virtuel();
}

/*! @brief Renvoie la valeur que devrait avoir le champ sur une face de bord, si on en croit les conditions aux limites.
 *
 * Le numero est compte
 *  dans la liste des faces de bord reelles. Le temps considere est le
 *  present du Champ_Face
 *  L'implementation a change : ces valeurs ne sont plus stockees dans le champ.
 *
 */
double Champ_Face_VDF::val_imp_face_bord_private(int face, int comp) const
{
  const Zone_Cl_VDF& zclo = ref_cast(Zone_Cl_VDF, equation().zone_Cl_dis().valeur());
  return Champ_Face_get_val_imp_face_bord_sym(valeurs(), temps(), face, comp, zclo);
}

// WEC : jamais appele !!
double Champ_Face_VDF::val_imp_face_bord_private(int face, int comp1, int comp2) const
{
  Cerr << "Champ_Face_VDF::val_imp_face_bord(,,) exit" << finl;
  Process::exit();
  return 0; // For compilers
}

// Cette fonction retourne :
//   1 si le fluide est sortant sur la face num_face
//   0 si la face correspond a une reentree de fluide
int Champ_Face_VDF::compo_normale_sortante(int num_face) const
{
  int signe = 1;
  double vit_norm;
  // signe vaut -1 si face_voisins(num_face,0) est a l'exterieur
  // signe vaut  1 si face_voisins(num_face,1) est a l'exterieur
  if (zone_vdf().face_voisins(num_face, 0) == -1)
    signe = -1;
  vit_norm = (*this)(num_face) * signe;
  return (vit_norm > 0);
}

DoubleTab& Champ_Face_VDF::trace(const Frontiere_dis_base& fr, DoubleTab& x, double tps, int distant) const
{
  return Champ_Face_VDF_implementation::trace(fr, valeurs(tps), x, distant);
}

void Champ_Face_VDF::mettre_a_jour(double un_temps)
{
  Champ_Inc_base::mettre_a_jour(un_temps);
}

void Champ_Face_VDF::calculer_rotationnel_ordre2_centre_element(DoubleTab& rot) const
{
  const DoubleTab& val = valeurs();
  const Zone_VDF& zone_VDF = zone_vdf();
  int nb_elem = zone_VDF.nb_elem();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();

  if (dimension == 2)
    calrotord2centelemdim2(rot, val, zone_VDF, nb_elem, face_voisins, elem_faces);
  else if (dimension == 3)
    calrotord2centelemdim3(rot, val, zone_VDF, nb_elem, face_voisins, elem_faces);
}

int Champ_Face_VDF::imprime(Sortie& os, int ncomp) const
{
  imprime_Face(os, ncomp);
  return 1;
}

void Champ_Face_VDF::calcul_critere_Q(DoubleTab& Q, const Zone_Cl_VDF& zone_Cl_VDF)
{
  // Q=0.5*(\Omega_{ij}*\Omega_{ij}-S_{ij}*S_{ij})=-0.25*du_i/dx_j*du_j/dx_i

  const Zone_VDF& zone_VDF = zone_vdf();
  const int nb_elem = zone_VDF.nb_elem();
  const int nb_elem_tot = zone_VDF.nb_elem_tot();
  int num_elem, i, j;
  double crit, deriv1, deriv2;

  DoubleTab gradient_elem(nb_elem_tot, dimension, dimension);
  gradient_elem = 0.;

  Champ_Face_VDF& vit = *this;
  const DoubleTab& vitesse = valeurs();

  vit.calcul_duidxj(vitesse, gradient_elem, zone_Cl_VDF);

  for (num_elem = 0; num_elem < nb_elem; num_elem++)
    {
      crit = 0.;
      for (i = 0; i < dimension; i++)
        for (j = 0; j < dimension; j++)
          {
            deriv1 = gradient_elem(num_elem, i, j);
            deriv2 = gradient_elem(num_elem, j, i);

            crit += -0.25 * deriv1 * deriv2;
          }
      Q[num_elem] = crit;
    }
}

void Champ_Face_VDF::calcul_y_plus(DoubleTab& y_plus, const Zone_Cl_VDF& zone_Cl_VDF)
{
  // On initialise le champ y_plus avec une valeur negative,
  // comme ca lorsqu'on veut visualiser le champ pres de la paroi,
  // on n'a qu'a supprimer les valeurs negatives et n'apparaissent
  // que les valeurs aux parois.

  int ndeb, nfin, elem, ori, l_unif;
  double norm_tau, u_etoile, norm_v = 0, dist, val0, val1, val2, d_visco = 0, visco = 1.;
  y_plus = -1.;

  const Champ_Face_VDF& vit = *this;
  const Zone_VDF& zone_VDF = zone_vdf();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntVect& orientation = zone_VDF.orientation();
  const Equation_base& eqn_hydr = equation();
  const Fluide_base& le_fluide = ref_cast(Fluide_base, eqn_hydr.milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
  const DoubleTab& tab_visco = ch_visco_cin->valeurs();
  //DoubleTab& tab_visco = ch_visco_cin.valeur().valeurs();

  if (sub_type(Champ_Uniforme, ch_visco_cin.valeur()))
    {
      visco = tab_visco(0, 0);
      l_unif = 1;
    }
  else
    l_unif = 0;

  // Changer uniquement les valeurs < DMINFLOAT (l'ancien code n'est pas parallele)
  /* GF on a pas a change tab_visco ici !
   if (!l_unif)
   {
   const int n = tab_visco.size_array();
   ArrOfDouble& v = tab_visco;
   for (int i = 0; i < n; i++)
   if (v[i] < DMINFLOAT)
   v[i] = DMINFLOAT;
   }
   */

  DoubleTab yplus_faces(1, 1); // will contain yplus values if available
  int yplus_already_computed = 0; // flag

  const RefObjU& modele_turbulence = eqn_hydr.get_modele(TURBULENCE);
  if (modele_turbulence.non_nul() && sub_type(Mod_turb_hyd_base, modele_turbulence.valeur()))
    {
      const Mod_turb_hyd_base& mod_turb = ref_cast(Mod_turb_hyd_base, modele_turbulence.valeur());
      const Turbulence_paroi_base& loipar = mod_turb.loi_paroi();
      if (loipar.use_shear())
        {
          yplus_faces.resize(zone_vdf().nb_faces_tot());
          yplus_faces.ref(loipar.tab_d_plus());
          yplus_already_computed = 1;
        }
    }

  for (int n_bord = 0; n_bord < zone_VDF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VDF.les_conditions_limites(n_bord);

      if (sub_type(Dirichlet_paroi_fixe, la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          for (int num_face = ndeb; num_face < nfin; num_face++)
            {

              if (face_voisins(num_face, 0) != -1)
                elem = face_voisins(num_face, 0);
              else
                elem = face_voisins(num_face, 1);

              if (yplus_already_computed)
                {
                  // y+ is only defined on faces so we take the face value to put in the element
                  y_plus(elem) = yplus_faces(num_face);
                }
              else
                {

                  if (dimension == 2)
                    {
                      ori = orientation(num_face);
                      norm_v = norm_2D_vit(vit.valeurs(), elem, ori, zone_VDF, val0);
                    }
                  else if (dimension == 3)
                    {
                      ori = orientation(num_face);
                      norm_v = norm_3D_vit(vit.valeurs(), elem, ori, zone_VDF, val1, val2);
                    } // dim 3

                  if (axi)
                    dist = zone_VDF.dist_norm_bord_axi(num_face);
                  else
                    dist = zone_VDF.dist_norm_bord(num_face);
                  if (l_unif)
                    d_visco = visco;
                  else
                    d_visco = tab_visco[elem];

                  // PQ : 01/10/03 : corrections par rapport a la version premiere
                  norm_tau = d_visco * norm_v / dist;

                  u_etoile = sqrt(norm_tau);
                  y_plus(elem) = dist * u_etoile / d_visco;

                } // else yplus already computed

            } // loop on faces

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

DoubleTab& Champ_Face_VDF::calcul_duidxj(const DoubleTab& vitesse, DoubleTab& gij, const Zone_Cl_VDF& zone_Cl_VDF) const
{
  const Champ_Face_VDF& vit = ref_cast(Champ_Face_VDF, mon_equation->inconnue().valeur());
  const Zone_VDF& zone_VDF = zone_vdf();
  int nb_elem = zone_VDF.zone().nb_elem_tot();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const IntTab& Qdm = zone_VDF.Qdm();
  const IntVect& orientation = zone_VDF.orientation();

  int premiere_arete_mixte = zone_VDF.premiere_arete_mixte();
  int premiere_arete_interne = zone_VDF.premiere_arete_interne();
  int derniere_arete_mixte = premiere_arete_mixte + zone_VDF.nb_aretes_mixtes();
  int derniere_arete_interne = premiere_arete_interne + zone_VDF.nb_aretes_internes();
  int elem;
  int num_arete;
  IntVect element(4);

  int num0, num1, num2, num3, signe;
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

  for (num_arete = ndeb; num_arete < nfin; num_arete++)
    {
      n_type = zone_Cl_VDF.type_arete_bord(num_arete - ndeb);

      if (n_type == 4) // arete de type periodicite
        {
          num0 = Qdm(num_arete, 0);
          num1 = Qdm(num_arete, 1);
          num2 = Qdm(num_arete, 2);
          num3 = Qdm(num_arete, 3);
          int i = 0;
          int j = 0;
          i = orientation(num0);
          j = orientation(num2);

          double temp1 = (vitesse[num1] - vitesse[num0]) / zone_VDF.dist_face_period(num0, num1, j);            // dui/dxj
          double temp2 = (vitesse[num3] - vitesse[num2]) / zone_VDF.dist_face_period(num2, num3, i);            // duj/dxi
          element(0) = face_voisins(num0, 0);
          element(1) = face_voisins(num0, 1);
          element(2) = face_voisins(num1, 0);
          element(3) = face_voisins(num1, 1);
          for (int k = 0; k < 4; k++)
            {
              gij(element(k), i, j) += temp1 * 0.5 * 0.25;
              gij(element(k), j, i) += temp2 * 0.5 * 0.25;

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
          num0 = Qdm(num_arete, 0);
          num1 = Qdm(num_arete, 1);
          num2 = Qdm(num_arete, 2);
          signe = Qdm(num_arete, 3);
          int j = 0;
          j = orientation(num2);

          double temp2;
          double temp1 = (vitesse[num1] - vitesse[num0]) / zone_VDF.dist_face_period(num0, num1, j);        // dui/dxj
          double vit_imp = 0.5 * (vit.val_imp_face_bord_private(num0, j) + vit.val_imp_face_bord_private(num1, j));                // vitesse tangentielle
          //Dans cette partie, on conserve le codage de Hyd_SGE_Wale_VDF (num1 et non num2)
          //pour calculer la distance entre le centre de la maille et le bord.

          temp2 = -signe * (vitesse[num2] - vit_imp) / zone_VDF.dist_norm_bord(num1);

          element(0) = face_voisins(num2, 0);
          element(1) = face_voisins(num2, 1);

          int i = orientation(num0);

          for (int k = 0; k < 2; k++)
            {
              gij(element(k), i, j) += temp1 * 0.25;
              gij(element(k), j, i) += temp2 * 0.25;

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

  for (num_arete = ndeb; num_arete < nfin; num_arete++)
    {
      n_type = zone_Cl_VDF.type_arete_coin(num_arete - ndeb);
      //***************************************
      // Traitement des aretes coin perio-perio
      //***************************************

      if (n_type == 0) // arete de type periodicite-periodicite
        {
          num0 = Qdm(num_arete, 0);
          num1 = Qdm(num_arete, 1);
          num2 = Qdm(num_arete, 2);
          num3 = Qdm(num_arete, 3);
          int i = 0;
          int j = 0;
          i = orientation(num0);
          j = orientation(num2);

          double temp1 = (vitesse[num1] - vitesse[num0]) / zone_VDF.dist_face_period(num0, num1, j);        // dui/dxj
          double temp2 = (vitesse[num3] - vitesse[num2]) / zone_VDF.dist_face_period(num2, num3, i);        // duj/dxi
          element(0) = face_voisins(num0, 0);
          element(1) = face_voisins(num0, 1);
          element(2) = face_voisins(num1, 0);
          element(3) = face_voisins(num1, 1);
          for (int k = 0; k < 4; k++)
            {
              gij(element(k), i, j) += temp1 * 0.5 * 0.5 * 0.25;
              gij(element(k), j, i) += temp2 * 0.5 * 0.5 * 0.25;

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
          num0 = Qdm(num_arete, 0);
          num1 = Qdm(num_arete, 1);
          num2 = Qdm(num_arete, 2);
          signe = Qdm(num_arete, 3);
          int j = 0;
          j = orientation(num2);

          double temp2;
          double temp1 = (vitesse[num1] - vitesse[num0]) / zone_VDF.dist_face_period(num0, num1, j);                // dui/dxj
          double vit_imp = 0.5 * (vit.val_imp_face_bord_private(num0, j) + vit.val_imp_face_bord_private(num1, j));        // vitesse tangentielle

          temp2 = -signe * (vitesse[num2] - vit_imp) / zone_VDF.dist_norm_bord(num1);

          element(0) = face_voisins(num2, 0);
          element(1) = face_voisins(num2, 1);

          int i = orientation(num1);

          for (int k = 0; k < 2; k++)
            {
              gij(element(k), i, j) += temp1 * 0.5 * 0.25;
              gij(element(k), j, i) += temp2 * 0.5 * 0.25;

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

  for (num_arete = premiere_arete_mixte; num_arete < derniere_arete_mixte; num_arete++)
    {
      num0 = Qdm(num_arete, 0);
      num1 = Qdm(num_arete, 1);
      num2 = Qdm(num_arete, 2);
      num3 = Qdm(num_arete, 3);
      int i = 0;
      int j = 0;
      i = orientation(num0);
      j = orientation(num2);

      double temp1 = (vitesse[num1] - vitesse[num0]) / zone_VDF.dist_face_period(num0, num1, j);        // dui/dxj
      double temp2 = (vitesse[num3] - vitesse[num2]) / zone_VDF.dist_face_period(num2, num3, i);        // duj/dxi

      element(0) = face_voisins(num0, 0);
      element(1) = face_voisins(num0, 1);
      element(2) = face_voisins(num1, 0);
      element(3) = face_voisins(num1, 1);

      for (int k = 0; k < 4; k++)
        {
          if (element(k) != -1)
            {
              gij(element(k), i, j) += temp1 * 0.25;
              gij(element(k), j, i) += temp2 * 0.25;

              // Justification des coeff derriere :
              // 1) 0.25 : on distribue le gradient de vitesse
              //            sur les 3 elements qui l'entourent.
              //            C'est pour cela que l'on regarde si element(k)!=-1,
              //            car dans ce cas la, c'est qu'il s'agit de "la case qui manque" !
            } // fin de else if
        } // fin du for k
    } // fin de la boucle sur les aretes mixtes

  for (num_arete = premiere_arete_interne; num_arete < derniere_arete_interne; num_arete++)
    {
      num0 = Qdm(num_arete, 0);
      num1 = Qdm(num_arete, 1);
      num2 = Qdm(num_arete, 2);
      num3 = Qdm(num_arete, 3);
      int i = 0;
      int j = 0;
      i = orientation(num0);
      j = orientation(num2);

      double temp1 = (vitesse[num1] - vitesse[num0]) / zone_VDF.dist_face_period(num0, num1, j);        // dui/dxj
      assert(est_egal(zone_VDF.dist_face_period(num0, num1, j), zone_VDF.dist_face(num0, num1, j)));
      double temp2 = (vitesse[num3] - vitesse[num2]) / zone_VDF.dist_face_period(num2, num3, i);        // duj/dxi
      assert(est_egal(zone_VDF.dist_face_period(num2, num3, j), zone_VDF.dist_face(num2, num3, j)));
      element(0) = face_voisins(num0, 0);
      element(1) = face_voisins(num0, 1);
      element(2) = face_voisins(num1, 0);
      element(3) = face_voisins(num1, 1);
      for (int k = 0; k < 4; k++)
        {
          gij(element(k), i, j) += temp1 * 0.25;
          gij(element(k), j, i) += temp2 * 0.25;

          // Justification des coeff :
          // 1) 0.25 : on distribue le gradient de vitesse
          //            sur les 4 elements qui l'entourent.
        } // fin du for k
    } // fin de la boucle sur les aretes internes

  // 2eme partie: boucle sur les elements et remplissage de Sij pour les
  //  derivees non croisees (dui/dxi).
  // En fait dans ces cas la, on calcul directement le gradient dans l'element
  //  et on ne redistribue pas.

  for (elem = 0; elem < nb_elem; elem++)
    {
      for (int i = 0; i < dimension; i++)
        {
          double temp1 = (vitesse[elem_faces(elem, i)] - vitesse[elem_faces(elem, i + dimension)]) / zone_VDF.dim_elem(elem, orientation(elem_faces(elem, i)));

          gij(elem, i, i) = -temp1;

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

DoubleTab& Champ_Face_VDF::calcul_duidxj(const DoubleTab& in_vel, DoubleTab& gij) const
{

  const Zone_VDF& zone_VDF = zone_vdf();
  int nb_elem_tot = zone_VDF.zone().nb_elem_tot();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();

  int element_number;
  int num0, num1, num2, num3, num4, num5;
  int f0, f1, f2, f3, f4, f5;

  //
  // Calculate the Sij tensor
  //
  if (dimension == 2)
    {
      for (element_number = 0; element_number < nb_elem_tot; element_number++)
        {
          f0 = elem_faces(element_number, 0);
          num0 = face_voisins(f0, 0);
          if (num0 == -1)
            num0 = element_number;
          f1 = elem_faces(element_number, 1);
          num1 = face_voisins(f1, 0);
          if (num1 == -1)
            num1 = element_number;
          f2 = elem_faces(element_number, 2);
          num2 = face_voisins(f2, 1);
          if (num2 == -1)
            num2 = element_number;
          f3 = elem_faces(element_number, 3);
          num3 = face_voisins(f3, 1);
          if (num3 == -1)
            num3 = element_number;

          gij(element_number, 0, 0) = 0.5 * ((in_vel(num2, 0) - in_vel(num0, 0)) / zone_VDF.dim_elem(element_number, 0));
          gij(element_number, 0, 1) = 0.5 * ((in_vel(num3, 0) - in_vel(num1, 0)) / zone_VDF.dim_elem(element_number, 1));
          gij(element_number, 1, 0) = 0.5 * ((in_vel(num2, 1) - in_vel(num0, 1)) / zone_VDF.dim_elem(element_number, 0));
          gij(element_number, 1, 1) = 0.5 * ((in_vel(num3, 1) - in_vel(num1, 1)) / zone_VDF.dim_elem(element_number, 1));
        }
    }
  else
    {
      for (element_number = 0; element_number < nb_elem_tot; element_number++)
        {
          f0 = elem_faces(element_number, 0);
          num0 = face_voisins(f0, 0);
          if (num0 == -1)
            num0 = element_number;
          f1 = elem_faces(element_number, 1);
          num1 = face_voisins(f1, 0);
          if (num1 == -1)
            num1 = element_number;
          f2 = elem_faces(element_number, 2);
          num2 = face_voisins(f2, 0);
          if (num2 == -1)
            num2 = element_number;
          f3 = elem_faces(element_number, 3);
          num3 = face_voisins(f3, 1);
          if (num3 == -1)
            num3 = element_number;
          f4 = elem_faces(element_number, 4);
          num4 = face_voisins(f4, 1);
          if (num4 == -1)
            num4 = element_number;
          f5 = elem_faces(element_number, 5);
          num5 = face_voisins(f5, 1);
          if (num5 == -1)
            num5 = element_number;

          gij(element_number, 0, 0) = 0.5 * ((in_vel(num3, 0) - in_vel(num0, 0)) / zone_VDF.dim_elem(element_number, 0));

          gij(element_number, 0, 1) = 0.5 * ((in_vel(num4, 0) - in_vel(num1, 0)) / zone_VDF.dim_elem(element_number, 1));
          gij(element_number, 1, 0) = 0.5 * ((in_vel(num3, 1) - in_vel(num0, 1)) / zone_VDF.dim_elem(element_number, 0));

          gij(element_number, 0, 2) = 0.5 * ((in_vel(num5, 0) - in_vel(num2, 0)) / zone_VDF.dim_elem(element_number, 2));

          gij(element_number, 2, 0) = 0.5 * ((in_vel(num3, 2) - in_vel(num0, 2)) / zone_VDF.dim_elem(element_number, 0));

          gij(element_number, 1, 1) = 0.5 * ((in_vel(num4, 1) - in_vel(num1, 1)) / zone_VDF.dim_elem(element_number, 1));

          gij(element_number, 1, 2) = 0.5 * ((in_vel(num5, 1) - in_vel(num2, 1)) / zone_VDF.dim_elem(element_number, 2));
          gij(element_number, 2, 1) = 0.5 * ((in_vel(num4, 2) - in_vel(num1, 2)) / zone_VDF.dim_elem(element_number, 1));

          gij(element_number, 2, 2) = 0.5 * ((in_vel(num5, 2) - in_vel(num2, 2)) / zone_VDF.dim_elem(element_number, 2));

        }
    }

  return gij;

}

//Methode qui renvoie SMA_barre aux elements a partir de la vitesse aux faces
//SMA_barre = Sij*Sij (sommation sur les indices i et j)
DoubleVect& Champ_Face_VDF::calcul_S_barre_sans_contrib_paroi(const DoubleTab& vitesse, DoubleVect& SMA_barre, const Zone_Cl_VDF& zone_Cl_VDF) const
{
  // On calcule directement S_barre(num_elem)!!!!!!!!!!
  // Le parametre contribution_paroi (ici fixe a 0) permet de ne pas prendre en compte
  // la contribution de la paroi au produit SMA_barre = Sij*Sij

  int contribution_paroi;
  contribution_paroi = 0;

  const Champ_Face_VDF& vit = ref_cast(Champ_Face_VDF, mon_equation->inconnue().valeur());
  const Zone_VDF& zone_VDF = zone_vdf();

  int nb_elem = zone_VDF.zone().nb_elem();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const IntTab& Qdm = zone_VDF.Qdm();
  const IntVect& orientation = zone_VDF.orientation();

  int premiere_arete_mixte = zone_VDF.premiere_arete_mixte();
  int premiere_arete_interne = zone_VDF.premiere_arete_interne();
  int derniere_arete_mixte = premiere_arete_mixte + zone_VDF.nb_aretes_mixtes();
  int derniere_arete_interne = premiere_arete_interne + zone_VDF.nb_aretes_internes();
  int elem;
  int num_arete;
  ArrOfInt element(4);

  int num0, num1, num2, num3, signe;
  int ndeb = zone_VDF.premiere_arete_bord();
  int nfin = ndeb + zone_VDF.nb_aretes_bord();
  int n_type;

  for (num_arete = ndeb; num_arete < nfin; num_arete++)
    {
      n_type = zone_Cl_VDF.type_arete_bord(num_arete - ndeb);

      if (n_type == 4) // arete de type periodicite
        {
          num0 = Qdm(num_arete, 0);
          num1 = Qdm(num_arete, 1);
          num2 = Qdm(num_arete, 2);
          num3 = Qdm(num_arete, 3);
          int i = 0;
          int j = 0;
          i = orientation(num0);
          j = orientation(num2);

          double temp1 = (vitesse[num1] - vitesse[num0]) / zone_VDF.dist_face_period(num0, num1, j);            // dv/dx
          double temp2 = (vitesse[num3] - vitesse[num2]) / zone_VDF.dist_face_period(num2, num3, i);            // du/dy
          element[0] = face_voisins(num0, 0);
          element[1] = face_voisins(num0, 1);
          element[2] = face_voisins(num1, 0);
          element[3] = face_voisins(num1, 1);
          for (int k = 0; k < 4; k++)
            {
              SMA_barre[element[k]] += 0.5 * (temp1 + temp2) * (temp1 + temp2) * 0.25;
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
          num0 = Qdm(num_arete, 0);
          num1 = Qdm(num_arete, 1);
          num2 = Qdm(num_arete, 2);
          signe = Qdm(num_arete, 3);
          int j = 0;
          j = orientation(num2);

          double temp2;
          double temp1 = (vitesse[num1] - vitesse[num0]) / zone_VDF.dist_face_period(num0, num1, j);        // dv/dx
          double vit_imp = 0.5 * (vit.val_imp_face_bord_private(num0, j) + vit.val_imp_face_bord_private(num1, j));                // vitesse tangentielle

          if (n_type == 0 && contribution_paroi == 0)
            temp2 = 0;
          else
            temp2 = -signe * (vitesse[num2] - vit_imp) / zone_VDF.dist_norm_bord(num1);

          element[0] = face_voisins(num2, 0);
          element[1] = face_voisins(num2, 1);

          for (int k = 0; k < 2; k++)
            {
              SMA_barre[element[k]] += (temp1 + temp2) * (temp1 + temp2) * 0.25;
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

  for (num_arete = ndeb; num_arete < nfin; num_arete++)
    {
      n_type = zone_Cl_VDF.type_arete_coin(num_arete - ndeb);
      //////////////////////////////////////////
      // Traitement des aretes coin perio-perio
      //////////////////////////////////////////

      if (n_type == 0) // arete de type periodicite-periodicite
        {
          num0 = Qdm(num_arete, 0);
          num1 = Qdm(num_arete, 1);
          num2 = Qdm(num_arete, 2);
          num3 = Qdm(num_arete, 3);
          int i = 0;
          int j = 0;
          i = orientation(num0);
          j = orientation(num2);

          double temp1 = (vitesse[num1] - vitesse[num0]) / zone_VDF.dist_face_period(num0, num1, j);            // dv/dx
          double temp2 = (vitesse[num3] - vitesse[num2]) / zone_VDF.dist_face_period(num2, num3, i);            // du/dy
          element[0] = face_voisins(num0, 0);
          element[1] = face_voisins(num0, 1);
          element[2] = face_voisins(num1, 0);
          element[3] = face_voisins(num1, 1);
          for (int k = 0; k < 4; k++)
            {
              SMA_barre[element[k]] += 0.5 * 0.5 * (temp1 + temp2) * (temp1 + temp2) * 0.25;
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
          num0 = Qdm(num_arete, 0);
          num1 = Qdm(num_arete, 1);
          num2 = Qdm(num_arete, 2);
          signe = Qdm(num_arete, 3);
          int j = 0;
          j = orientation(num2);

          double temp2;
          double temp1 = (vitesse[num1] - vitesse[num0]) / zone_VDF.dist_face_period(num0, num1, j);        // dv/dx
          double vit_imp = 0.5 * (vit.val_imp_face_bord_private(num0, j) + vit.val_imp_face_bord_private(num1, j));             // vitesse tangentielle

          if (contribution_paroi == 0)
            temp2 = 0;
          else
            temp2 = -signe * (vitesse[num2] - vit_imp) / zone_VDF.dist_norm_bord(num1);

          element[0] = face_voisins(num2, 0);
          element[1] = face_voisins(num2, 1);

          for (int k = 0; k < 2; k++)
            {
              SMA_barre[element[k]] += 0.5 * (temp1 + temp2) * (temp1 + temp2) * 0.25;
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

  for (num_arete = premiere_arete_mixte; num_arete < derniere_arete_mixte; num_arete++)
    {
      num0 = Qdm(num_arete, 0);
      num1 = Qdm(num_arete, 1);
      num2 = Qdm(num_arete, 2);
      num3 = Qdm(num_arete, 3);
      int i = 0;
      int j = 0;
      i = orientation(num0);
      j = orientation(num2);

      double temp1 = (vitesse[num1] - vitesse[num0]) / zone_VDF.dist_face_period(num0, num1, j);          // dv/dx
      double temp2 = (vitesse[num3] - vitesse[num2]) / zone_VDF.dist_face_period(num2, num3, i);          // du/dy

      element[0] = face_voisins(num0, 0);
      element[1] = face_voisins(num0, 1);
      element[2] = face_voisins(num1, 0);
      element[3] = face_voisins(num1, 1);

      for (int k = 0; k < 4; k++)
        {
          if (element[k] != -1)
            {
              SMA_barre[element[k]] += (temp1 + temp2) * (temp1 + temp2) * 0.25;
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

  for (num_arete = premiere_arete_interne; num_arete < derniere_arete_interne; num_arete++)
    {
      num0 = Qdm(num_arete, 0);
      num1 = Qdm(num_arete, 1);
      num2 = Qdm(num_arete, 2);
      num3 = Qdm(num_arete, 3);
      int i = 0;
      int j = 0;
      i = orientation(num0);
      j = orientation(num2);

      double temp1 = (vitesse[num1] - vitesse[num0]) / zone_VDF.dist_face_period(num0, num1, j);            // dv/dx
      double temp2 = (vitesse[num3] - vitesse[num2]) / zone_VDF.dist_face_period(num2, num3, i);            // du/dy
      element[0] = face_voisins(num0, 0);
      element[1] = face_voisins(num0, 1);
      element[2] = face_voisins(num1, 0);
      element[3] = face_voisins(num1, 1);
      for (int k = 0; k < 4; k++)
        {
          SMA_barre[element[k]] += (temp1 + temp2) * (temp1 + temp2) * 0.25;
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

  for (elem = 0; elem < nb_elem; elem++)
    {
      for (int i = 0; i < dimension; i++)
        {
          double temp1 = (vitesse[elem_faces(elem, i)] - vitesse[elem_faces(elem, i + dimension)]) / zone_VDF.dim_elem(elem, orientation(elem_faces(elem, i)));
          SMA_barre(elem) += 2.0 * temp1 * temp1;
        }
    }

  // On prend la racine carre!!!!!  ATTENTION SMA_barre=invariant au carre!!!
  //  racine_carree(SMA_barre)

  return SMA_barre;
}

DoubleVect& Champ_Face_VDF::calcul_S_barre(const DoubleTab& vitesse, DoubleVect& SMA_barre, const Zone_Cl_VDF& zone_Cl_VDF) const
{
  const Zone_VDF& zone_VDF = zone_vdf();
  const int nb_elem_tot = zone_VDF.nb_elem_tot();
  const int nb_elem = zone_VDF.nb_elem();

  int i, j;
  int elem;
  double Sij, temp;

  DoubleTab duidxj(nb_elem_tot, dimension, dimension);

  calcul_duidxj(vitesse, duidxj, zone_Cl_VDF);

  for (elem = 0; elem < nb_elem; elem++)
    {
      temp = 0.;
      for (i = 0; i < dimension; i++)
        for (j = 0; j < dimension; j++)
          {
            Sij = 0.5 * (duidxj(elem, i, j) + duidxj(elem, j, i));
            temp += Sij * Sij;
          }
      SMA_barre(elem) = 2. * temp;
    }

  return SMA_barre;

}

void Champ_Face_VDF::calcul_grad_u(const DoubleTab& vitesse, DoubleTab& grad_u, const Zone_Cl_VDF& zone_Cl_VDF)
{
  const Zone_VDF& zone_VDF = zone_vdf();
  const int nb_elem = zone_VDF.nb_elem();
  const int nb_elem_tot = zone_VDF.nb_elem_tot();

  DoubleTab gradient_elem(nb_elem_tot, dimension, dimension);
  gradient_elem = 0.;

  calcul_duidxj(vitesse, gradient_elem, zone_Cl_VDF);

  for (int elem = 0; elem < nb_elem; elem++)
    {
      int comp = 0;
      for (int i = 0; i < dimension; i++)
        for (int j = 0; j < dimension; j++)
          {
            grad_u(elem, comp) = gradient_elem(elem, i, j);
            comp++;
          }
    }
}

void Champ_Face_VDF::calculer_dscald_centre_element(DoubleTab& dscald) const
{
  const DoubleTab& val = valeurs();
  const Zone_VDF& zone_VDF = zone_vdf();
  int nb_elem = zone_VDF.nb_elem();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();

  if (dimension == 2)
    caldscaldcentelemdim2(dscald, val, zone_VDF, nb_elem, face_voisins, elem_faces);
  else if (dimension == 3)
    caldscaldcentelemdim3(dscald, val, zone_VDF, nb_elem, face_voisins, elem_faces);
}

// Fonctions de calcul des composantes du tenseur GradU (derivees covariantes de la vitesse)
// dans le repere des coordonnees cylindriques
void Champ_Face_VDF::calculer_dercov_axi(const Zone_Cl_VDF& zone_Cl_VDF)
{
  const Zone_VDF& zone_VDF = zone_vdf();
  const DoubleTab& inco = valeurs();
  const IntVect& orientation = zone_VDF.orientation();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const IntTab& Qdm = zone_VDF.Qdm();
  const DoubleTab& xv = zone_VDF.xv();
  const DoubleTab& xp = zone_VDF.xp();
  const IntVect& type_arete_bord = zone_Cl_VDF.type_arete_bord();

  double d_teta, R;
  double deux_pi = M_PI * 2.0;

  // Remplissage de tau_diag_ : termes diagonaux du tenseur GradU

  int fx0, fx1, fy0, fy1;
  int num_elem;
  for (num_elem = 0; num_elem < zone_VDF.nb_elem(); num_elem++)
    {
      fx0 = elem_faces(num_elem, 0);
      fx1 = elem_faces(num_elem, dimension);
      fy0 = elem_faces(num_elem, 1);
      fy1 = elem_faces(num_elem, 1 + dimension);

      // Calcul de tau11
      tau_diag_(num_elem, 0) = (inco[fx1] - inco[fx0]) / (xv(fx1, 0) - xv(fx0, 0));

      // Calcul de tau22
      R = xp(num_elem, 0);
      d_teta = xv(fy1, 1) - xv(fy0, 1);
      if (d_teta < 0)
        d_teta += deux_pi;
      tau_diag_(num_elem, 1) = (inco[fy1] - inco[fy0]) / (R * d_teta) + 0.5 * (inco[fx0] + inco[fx1]) / R;
    }

  if (dimension == 3)
    {
      int fz0, fz1;
      for (num_elem = 0; num_elem < zone_VDF.nb_elem(); num_elem++)
        {
          fz0 = elem_faces(num_elem, 2);
          fz1 = elem_faces(num_elem, 2 + dimension);

          // Calcul de tau33
          tau_diag_(num_elem, 2) = (inco[fz1] - inco[fz0]) / (xv(fz1, 2) - xv(fz0, 2));

        }
    }

  // Remplissage de tau_croises_ : termes extradiagonaux du tenseur GradU
  // Les derivees croisees de la vitesse (termes extradiagonaux du tenseur
  // GradU) sont calculees sur les aretes.
  // Il y a deux derivees par arete:
  //    Pour une arete XY : tau12 et tau21
  //    Pour une arete YZ : tau23 et tau32
  //    Pour une arete XZ : tau13 et tau31

  // Boucle sur les aretes bord

  int n_arete;
  int ndeb = zone_VDF.premiere_arete_bord();
  int nfin = ndeb + zone_VDF.nb_aretes_bord();
  int ori1, ori3;
  int fac1, fac2, fac3, fac4, signe;
  double dist3;

  int n_type;

  for (n_arete = ndeb; n_arete < nfin; n_arete++)
    {
      n_type = type_arete_bord(n_arete - ndeb);

      switch(n_type)
        {
        case TypeAreteBordVDF::PAROI_PAROI:
          // paroi-paroi
        case TypeAreteBordVDF::FLUIDE_FLUIDE:
          // fluide-fluide
        case TypeAreteBordVDF::PAROI_FLUIDE:
          // paroi-fluide
          {
            fac1 = Qdm(n_arete, 0);
            fac2 = Qdm(n_arete, 1);
            fac3 = Qdm(n_arete, 2);
            signe = Qdm(n_arete, 3);
            ori1 = orientation(fac1);
            ori3 = orientation(fac3);
            int rang1 = fac1 - zone_VDF.premiere_face_bord();
            int rang2 = fac2 - zone_VDF.premiere_face_bord();
            double vit_imp;

            if (n_type == TypeAreteBordVDF::PAROI_FLUIDE)
              // arete paroi_fluide :il faut determiner qui est la face fluide
              {
                if (est_egal(inco[fac1], 0))
                  vit_imp = val_imp_face_bord_private(rang2, ori3);
                else
                  vit_imp = val_imp_face_bord_private(rang1, ori3);
              }
            else
              vit_imp = 0.5 * (val_imp_face_bord_private(rang1, ori3) + val_imp_face_bord_private(rang2, ori3));

            if (ori1 == 0) // bord d'equation R = cte
              {
                dist3 = xv(fac3, 0) - xv(fac1, 0);
                if (n_type != TypeAreteBordVDF::PAROI_PAROI)
                  dist3 *= 2;

                if (ori3 == 1)
                  {
                    // calcul de tau12
                    tau_croises_(n_arete, 0) = signe * (vit_imp - inco[fac3]) / dist3;

                    // calcul de tau21
                    R = xv(fac1, 0);
                    d_teta = xv(fac2, 1) - xv(fac1, 1);
                    if (d_teta < 0)
                      d_teta += deux_pi;
                    tau_croises_(n_arete, 1) = (inco[fac2] - inco[fac1]) / (R * d_teta);
                  }
                else if (ori3 == 2)
                  {
                    // calcul de tau13
                    tau_croises_(n_arete, 0) = signe * (vit_imp - inco[fac3]) / dist3;
                    // calcul de tau31
                    tau_croises_(n_arete, 1) = (inco[fac2] - inco[fac1]) / (xv(fac2, 2) - xv(fac1, 2));
                  }
              }
            else if (ori1 == 1) // bord d'equation teta = cte
              {
                R = xv(fac3, 0);
                d_teta = xv(fac3, 1) - xv(fac1, 1);
                if (d_teta < 0)
                  d_teta += deux_pi;
                dist3 = R * d_teta;
                if (n_type != TypeAreteBordVDF::PAROI_PAROI)
                  dist3 *= 2;

                if (ori3 == 0)
                  {
                    // calcul de tau21
                    tau_croises_(n_arete, 0) = signe * (vit_imp - inco[fac3]) / dist3 - 0.5 * (inco[fac1] + inco[fac2]) / R;
                    // calcul de tau12
                    tau_croises_(n_arete, 1) = (inco[fac2] - inco[fac1]) / (xv(fac2, 0) - xv(fac1, 0));
                  }
                else if (ori3 == 2)
                  {
                    // calcul de tau23
                    tau_croises_(n_arete, 0) = signe * (vit_imp - inco[fac3]) / dist3;
                    // calcul de tau32
                    tau_croises_(n_arete, 1) = (inco[fac2] - inco[fac1]) / (xv(fac2, 2) - xv(fac1, 2));
                  }
              }
            else // (ori1 == 2) bord d'equation Z = cte
              {
                dist3 = xv(fac3, 2) - xv(fac1, 2);
                if (n_type != TypeAreteBordVDF::PAROI_PAROI)
                  dist3 *= 2;

                if (ori3 == TypeAreteBordVDF::PAROI_PAROI)
                  {
                    // calcul de tau31
                    tau_croises_(n_arete, 0) = signe * (vit_imp - inco[fac3]) / dist3;
                    // calcul de tau13
                    tau_croises_(n_arete, 1) = (inco[fac2] - inco[fac1]) / (xv(fac2, 0) - xv(fac1, 0));
                  }
                else if (ori3 == 1)
                  {
                    // calcul de tau32
                    tau_croises_(n_arete, 0) = signe * (vit_imp - inco[fac3]) / dist3;

                    // calcul de tau23
                    R = xv(fac1, 0);
                    d_teta = xv(fac2, 1) - xv(fac1, 1);
                    if (d_teta < 0)
                      d_teta += deux_pi;
                    tau_croises_(n_arete, 1) = (inco[fac2] - inco[fac1]) / (R * d_teta);
                  }
              }
            break;
          }
        case 3:
          {
            // symetrie-symetrie
            // pas de flux diffusif calcule
            break;
          }
        default:
          {
            Cerr << "On a rencontre un type d'arete non prevu\n";
            Cerr << "num arete : " << n_arete;
            Cerr << " type : " << n_type;
            exit();
            break;
          }
        }
    }

  // Boucle sur les aretes mixtes et internes
  ndeb = zone_VDF.premiere_arete_mixte();
  nfin = zone_VDF.nb_aretes();
  for (n_arete = ndeb; n_arete < nfin; n_arete++)
    {
      fac1 = Qdm(n_arete, 0);
      fac2 = Qdm(n_arete, 1);
      fac3 = Qdm(n_arete, 2);
      fac4 = Qdm(n_arete, 3);
      ori1 = orientation(fac1);
      ori3 = orientation(fac3);
      if (ori1 == 1)  // (seule possibilite : ori3 =0)  Arete XY
        {
          // Calcul de tau21
          R = xv(fac3, 0);
          d_teta = xv(fac4, 1) - xv(fac3, 1);
          if (d_teta < 0)
            d_teta += deux_pi;
          tau_croises_(n_arete, 1) = (inco(fac4) - inco(fac3)) / (R * d_teta) - 0.5 * (inco[fac1] + inco[fac2]) / R;
          // Calcul de tau12
          tau_croises_(n_arete, 0) = (inco(fac2) - inco(fac1)) / (xv(fac2, 0) - xv(fac1, 0));
        }
      else if (ori3 == 1) // (seule possibilite ori1 = 2) arete YZ
        {
          // Calcul de tau32
          tau_croises_(n_arete, 1) = (inco(fac4) - inco(fac3)) / (xv(fac4, 2) - xv(fac3, 2));
          // Calcul de tau23
          R = xv(fac1, 0);
          d_teta = xv(fac2, 1) - xv(fac1, 1);
          if (d_teta < 0)
            d_teta += deux_pi;
          tau_croises_(n_arete, 0) = (inco(fac2) - inco(fac1)) / (R * d_teta);
        }
      else // seule possibilite ori1 = 2 et ori3 = 0:  arete XZ
        {
          // Calcul de tau31
          tau_croises_(n_arete, 1) = (inco(fac4) - inco(fac3)) / (xv(fac4, 2) - xv(fac3, 2));
          // Calcul de tau13
          tau_croises_(n_arete, 0) = (inco(fac2) - inco(fac1)) / (xv(fac2, 0) - xv(fac1, 0));
        }
    }
}
