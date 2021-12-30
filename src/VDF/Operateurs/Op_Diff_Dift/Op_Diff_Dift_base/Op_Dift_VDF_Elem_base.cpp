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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Op_Dift_VDF_Elem_base.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Op_Diff_Dift/Op_Diff_Dift_base
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Dift_VDF_Elem_base.h>

Implemente_base(Op_Dift_VDF_Elem_base,"Op_Dift_VDF_Elem_base",Op_Dift_VDF_base);
Sortie& Op_Dift_VDF_Elem_base::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Dift_VDF_Elem_base::readOn(Entree& s ) { return s ; }

/*
 * Calcul du pas de temps de stabilite : Pour const/var Elem, const/var Multi-inco
 * La diffusivite laminaire est constante ou variable, La diffusivite turbulente est variable
 *    dt_stab = Min (1/(2*(diff_lam(i)+diff_turb(i))*coeff(elem))
 *     ou
 *    dt_stab = Min (1/(2*(Max(diffu1(i),diffu2(i),....)+diff_turb(i))*coeff(elem))
 *  avec coeff =  1/(dx*dx) + 1/(dy*dy) + 1/(dz*dz) et i decrivant l'ensemble des elements du maillage
 *  diffu1(i),diffu2(i) ...: diffusivites des differents constituants sur l'element
 */
double Op_Dift_VDF_Elem_base::calculer_dt_stab_elem() const
{
  double dt_stab, coef = -1.e10;
  const Zone_VDF& zone_VDF = iter->zone();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const DoubleVect& alpha_t = diffusivite_turbulente()->valeurs();
  bool is_concentration = (equation().que_suis_je().debute_par("Convection_Diffusion_Concentration") || equation().que_suis_je().debute_par("Convection_Diffusion_Espece"));

  ArrOfInt numfa(2*dimension);
  for (int elem = 0; elem < zone_VDF.nb_elem(); elem++)
    {
      // choix du facteur
      double rcp = 1.;
      if (!is_concentration)
        {
          const int Ccp = sub_type(Champ_Uniforme, mon_equation->milieu().capacite_calorifique().valeur());
          const int Cr = sub_type(Champ_Uniforme, mon_equation->milieu().masse_volumique());
          const DoubleTab& tab_Cp = mon_equation->milieu().capacite_calorifique().valeurs(), tab_r = mon_equation->milieu().masse_volumique().valeurs();
          rcp = tab_r(Cr ? 0 : elem, 0) * tab_Cp(Ccp ? 0 : elem, 0);
        }

      double moy = 0.;
      for (int i = 0; i < 2 * dimension; i++) numfa[i] = elem_faces(elem, i);

      // XXX : E Saikali j'ai corrige pour multi inco parce que c'etait 1/dx et pas 1/dx^2 ... donc attention si ecart !
      // c'etait comme ca : for (int d = 0; d < dimension; d++) moy += 1. / (zone_VDF.dist_face(numfa[d], numfa[dimension + d], d));
      for (int d = 0; d < dimension; d++)
        {
          const double hd = zone_VDF.dist_face(numfa[d], numfa[dimension + d], d);
          moy += 1. / (hd * hd);
        }
      const double alpha_local = (alpha_(elem) + alpha_t(elem)) / rcp * moy;
      coef = max(coef, alpha_local);
    }

  coef = Process::mp_max(coef);
  dt_stab = 1. / (2. * (coef + DMINFLOAT));
  return dt_stab;
}

/*
 * Calcul du pas de temps de stabilite : Pour const/var Axi, const/var Multi-inco Axi
 * La diffusivite laminaire est constante ou variable, La diffusivite turbulente est variable
 *    dt_stab = Min (1/(2*(diff_lam(i)+diff_turb(i))*coeff(elem))
 *     ou
 *    dt_stab = Min (1/(2*(Max(diffu1(i),diffu2(i),....)+diff_turb(i))*coeff(elem))
 *  avec coeff =  1/(dx*dx) + 1/(dy*dy) + 1/(dz*dz) et i decrivant l'ensemble des elements du maillage
 *  diffu1(i),diffu2(i) ...: diffusivites des differents constituants sur l'element
 */
double Op_Dift_VDF_Elem_base::calculer_dt_stab_elem_axi() const
{
  double dt_stab, coef = -1.e10;
  const Zone_VDF& zone_VDF = iter->zone();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const DoubleVect& alpha_t = diffusivite_turbulente()->valeurs();
  double alpha_local,h_x,h_y,h_z;

  if (dimension == 2)
    {
      int numfa[4];
      for (int elem=0; elem<zone_VDF.nb_elem(); elem++)
        {
          for (int i=0; i<4; i++) numfa[i] = elem_faces(elem,i);
          h_x = zone_VDF.dist_face_axi(numfa[0],numfa[2],0);
          h_y = zone_VDF.dist_face_axi(numfa[1],numfa[3],1);
          alpha_local = (alpha_(elem)+alpha_t(elem)) *(1/(h_x*h_x) + 1/(h_y*h_y));
          coef = max(coef,alpha_local);
        }
    }
  else if (dimension == 3)
    {
      int numfa[6];
      for (int elem=0; elem<zone_VDF.nb_elem(); elem++)
        {
          for (int i=0; i<6; i++) numfa[i] = elem_faces(elem,i);
          h_x = zone_VDF.dist_face_axi(numfa[0],numfa[3],0);
          h_y = zone_VDF.dist_face_axi(numfa[1],numfa[4],1);
          h_z = zone_VDF.dist_face_axi(numfa[2],numfa[5],2);
          alpha_local = (alpha_(elem)+alpha_t(elem)) *(1/(h_x*h_x) + 1/(h_y*h_y) + 1/(h_z*h_z));
          coef = max(coef,alpha_local);
        }
    }

  dt_stab = 1/(2*(coef+DMINFLOAT));
  return dt_stab;
}

double Op_Dift_VDF_Elem_base::calculer_dt_stab_elem_var_axi() const
{
  double dt_stab, coef = -1.e10;
  const Zone_VDF& zone_VDF = iter->zone();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const DoubleVect& alpha_t = diffusivite_turbulente()->valeurs();
  const int D = dimension;

  DoubleVect numfa(2 * D), h(D);

  for (int e = 0; e < zone_VDF.nb_elem(); e++)
    {
      for (int i = 0; i < 2 * D; i++) numfa(i) = elem_faces(e, i);
      for (int d = 0; d < D; d++) h(d) = zone_VDF.dist_face_axi(numfa(d), numfa(d + D), d);
      double invh = 0.;
      for (int d = 0; d < D; d++) invh += 1. / (h(d) * h(d));
      const double alpha_local = (alpha_(e) + alpha_t(e)) * invh;
      coef = max(coef, alpha_local);
    }

  dt_stab = 1. / (2. * (coef + DMINFLOAT));
  return dt_stab;
}
