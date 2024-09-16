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

#include <Loi_paroi_log.h>
#include <Navier_Stokes_std.h>
#include <Correlation_base.h>
#include <QDM_Multiphase.h>
#include <TRUSTTab_parts.h>
#include <Cond_lim_base.h>
#include <Pb_Multiphase.h>
#include <Domaine_VF.h>
#include <TRUSTTrav.h>
#include <Motcle.h>
#include <Param.h>
#include <math.h>
#include <Nom.h>
#include <Champ_Face_base.h>

Implemente_instanciable(Loi_paroi_log, "Loi_paroi_log", Loi_paroi_base);

Sortie& Loi_paroi_log::printOn(Sortie& os) const
{
  return os;
}

Entree& Loi_paroi_log::readOn(Entree& is)
{
  return Loi_paroi_base::readOn(is);
}

void Loi_paroi_log::calc_y_plus(const DoubleTab& vit, const DoubleTab& nu_visc)
{
  Domaine_VF& domaine = ref_cast(Domaine_VF, pb_->domaine_dis());
  DoubleTab& u_t = valeurs_loi_paroi_["u_tau"], &y_p = valeurs_loi_paroi_["y_plus"];
  const DoubleTab& n_f = domaine.face_normales();
  const DoubleVect& fs = domaine.face_surfaces();
  const IntTab& f_e = domaine.face_voisins();

  int nf_tot = domaine.nb_faces_tot(), D = dimension, N = vit.line_size();

  DoubleTab pvit_elem(0, N * dimension);
  if (nf_tot == vit.dimension_tot(0))
    {
      const Champ_Face_base& ch = ref_cast(Champ_Face_base, pb_->equation(0).inconnue().valeur());
      domaine.domaine().creer_tableau_elements(pvit_elem);
      ch.get_elem_vector_field(pvit_elem, true);
    }

  int n=0; // pour l'instant, turbulence dans seulement une phase

  for (int f = 0 ; f < nf_tot ; f ++)
    if (Faces_a_calculer_(f,0)==1)
      {
        int c = (f_e(f,0)>=0) ? 0 : 1 ;
        if (f_e(f, (c==0) ? 1 : 0 ) >= 0) Process::exit("Error in the definition of the boundary conditions for wall laws");
        int e = f_e(f,c);

        double u_orth = 0 ;
        DoubleTrav u_parallel(D);
        if (nf_tot == vit.dimension_tot(0)) // VDF case
          {
            for (int d = 0; d <D ; d++) u_orth -= pvit_elem(e, N*d+n)*n_f(f,d)/fs(f); // ! n_f pointe vers la face 1 donc vers l'exterieur de l'element, d'ou le -
            for (int d = 0 ; d < D ; d++) u_parallel(d) = pvit_elem(e, N*d+n) - u_orth*(-n_f(f,d))/fs(f) ; // ! n_f pointe vers la face 1 donc vers l'exterieur de l'element, d'ou le -
          }
        else // PolyMAC case
          {
            for (int d = 0; d <D ; d++) u_orth -= vit(nf_tot + e * D+d, n)*n_f(f,d)/fs(f); // ! n_f pointe vers la face 1 donc vers l'exterieur de l'element, d'ou le -
            for (int d = 0 ; d < D ; d++) u_parallel(d) = vit(nf_tot + e * D + d, n) - u_orth*(-n_f(f,d))/fs(f) ; // ! n_f pointe vers la face 1 donc vers l'exterieur de l'element, d'ou le -
          }

        double residu = 0 ;
        for (int d = 0; d <D ; d++) residu += u_parallel(d)*n_f(f,d)/fs(f);
        if (residu > 1e-8) Process::exit("Loi_paroi_adaptative : Error in the calculation of the parallel velocity for wall laws");
        double norm_u_parallel = std::sqrt(domaine.dot(&u_parallel(0), &u_parallel(0)));

        double y_loc = (c==0) ? domaine.dist_face_elem0(f,e) : domaine.dist_face_elem1(f,e) ;
        y_p(f, n) = std::max(y_p_min_, calc_y_plus_loc(norm_u_parallel, nu_visc(e, n), y_loc, y_p(f, n)));
        u_t(f, n) = y_p(f, n)*nu_visc(e, n)/y_loc;
      }
}

double Loi_paroi_log::calc_y_plus_loc(double u_par, double nu, double y, double y_p_0)
{
  if (u_par*y/nu < limiteur_y_p) return limiteur_y_p;

  double eps = eps_y_p_;
  int step = 1, iter_max = 30;

  double y_p = y_p_0 ;
  double u_tau = nu*y_p/y;

  do
    {
      y_p = std::max(limiteur_y_p, y_p - (u_plus_de_y_plus(y_p) - u_par/u_tau)/(deriv_u_plus_de_y_plus(y_p) + u_par/(u_tau*y_p) ));
      step = step+1;
      u_tau = nu*y_p/y;
    }
  while( (std::fabs(u_plus_de_y_plus(y_p) - u_par/u_tau) > eps) and (step < iter_max));

  assert ( (std::fabs(u_par/u_tau - u_plus_de_y_plus(y_p)) < eps_y_p_*10) and (step < iter_max));

  return y_p;
}

double Loi_paroi_log::u_plus_de_y_plus(double y_p)  // Blended Reichardt model
{
  return std::log(y_p+limiteur_y_p)/von_karman_ + 5.1;
}

double Loi_paroi_log::deriv_u_plus_de_y_plus(double y_p)
{
  return 1./((y_p+limiteur_y_p)*von_karman_);
}

