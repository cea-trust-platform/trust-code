/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Echange_externe_radiatif.h>
#include <Champ_front_calc_interne.h>
#include <Echange_interne_impose.h>
#include <T_paroi_Champ_P0_VDF.h>
#include <Echange_impose_base.h>
#include <Dirichlet_homogene.h>
#include <Iterateur_VDF_Elem.h>
#include <Neumann_homogene.h>
#include <Op_Diff_VDF_base.h>
#include <Equation_base.h>
#include <Neumann_paroi.h>
#include <Eval_Diff_VDF.h>
#include <Champ_P0_VDF.h>
#include <Periodique.h>
#include <Operateur.h>
#include <Dirichlet.h>
#include <Navier.h>

Implemente_instanciable(T_paroi_Champ_P0_VDF, "T_paroi_Champ_P0_VDF", Champ_Fonc_P0_VDF);

Sortie& T_paroi_Champ_P0_VDF::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& T_paroi_Champ_P0_VDF::readOn(Entree& s) { return s; }

void T_paroi_Champ_P0_VDF::mettre_a_jour(double tps)
{
  me_calculer(tps);
  changer_temps(tps);
  Champ_Fonc_base::mettre_a_jour(tps);
}

void T_paroi_Champ_P0_VDF::associer_champ(const Champ_P0_VDF& un_champ)
{
  mon_champ_ = un_champ;
}

// TODO FIXME factorize
inline double newton_T_paroi_Champ_P0_VDF(double eps, double T_ext, double lambda, double e, double T0, double sgn)
{
  double Tb = T0;
  const double it_max = 100, tolerance = 1e-5;
  for (int i = 0; i < it_max; i++)
    {
      const double f = sgn * (5.67e-8 * eps * (T_ext * T_ext * T_ext * T_ext - Tb * Tb * Tb * Tb) - lambda / e * (Tb - T0));
      const double f_p = sgn * (-4 * 5.67e-8 * eps * Tb * Tb * Tb - lambda / e);
      const double Tb_new = Tb - f / f_p;
      if (std::abs(Tb_new - Tb) < tolerance)
        return Tb_new;
      Tb = Tb_new;
    }
  Process::exit("T_paroi_Champ_P0_VDF : newton did not converge !");
  return Tb;
}

void T_paroi_Champ_P0_VDF::me_calculer(double tps)
{
  const Domaine_VDF& dvdf = ref_cast(Domaine_VDF, le_dom_VF.valeur());
  const IntTab& face_voisins = dvdf.face_voisins();
  const DoubleVect& vol = dvdf.volumes();

  const Op_Diff_VDF_base& op_diff_vdf = ref_cast(Op_Diff_VDF_base, mon_champ_->equation().operateur(0).l_op_base());
  const Eval_Diff_VDF& eval = dynamic_cast<const Eval_Diff_VDF&>( op_diff_vdf.get_iter()->evaluateur());

  const DoubleTab& temp = mon_champ_->valeurs();
  DoubleTab& val = valeurs(tps);
  val = -1; // interne ou cl pas traitees

  for (int n_bord = 0; n_bord < dvdf.nb_front_Cl(); n_bord++)
    for (int k = 0; k < temp.line_size(); k++) // pour multiphase
      {
        const Cond_lim& la_cl = le_dom_Cl_VDF->les_conditions_limites(n_bord);
        const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
        const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();

        if (sub_type(Periodique, la_cl.valeur()))
          {
            for (int num_face = ndeb; num_face < nfin; num_face++)
              {
                const int elem1 = face_voisins(num_face, 0), elem2 = face_voisins(num_face, 1);

                val(elem1, k) = val(elem2, k) = (vol(elem1) * temp(elem1, k) + vol(elem2) * temp(elem2, k)) / (vol(elem1) + vol(elem2)); // moyenne volumique
              }
          }
        else if (sub_type(Dirichlet, la_cl.valeur()))
          {
            for (int num_face = ndeb, num_face_cl = 0; num_face < nfin; num_face++, num_face_cl++)
              {
                int elem = face_voisins(num_face, 0);
                if (elem < 0)
                  elem = face_voisins(num_face, 1);

                val(elem, k) = ref_cast(Dirichlet, la_cl.valeur()).val_imp(num_face_cl, k);
              }
          }
        else if (sub_type(Dirichlet_homogene, la_cl.valeur()))
          {
            for (int num_face = ndeb; num_face < nfin; num_face++)
              {
                int elem = face_voisins(num_face, 0);
                if (elem < 0)
                  elem = face_voisins(num_face, 1);

                val(elem, k) = 0.;
              }
          }
        else if (sub_type(Neumann_homogene, la_cl.valeur()) || sub_type(Navier, la_cl.valeur())) // grad nulle
          {
            for (int num_face = ndeb; num_face < nfin; num_face++)
              {
                int elem = face_voisins(num_face, 0);
                if (elem < 0)
                  elem = face_voisins(num_face, 1);

                val(elem, k) = temp(elem, k);
              }
          }
        else if (sub_type(Echange_externe_radiatif, la_cl.valeur()))
          {
            const Echange_externe_radiatif& la_cl_rad = ref_cast(Echange_externe_radiatif, la_cl.valeur());
            for (int num_face = ndeb, num_face_cl = 0; num_face < nfin; num_face++, num_face_cl++)
              {
                const int elem1 = face_voisins(num_face, 0), elem2 = face_voisins(num_face, 1);

                const double signe = elem1 > -1 ? 1.0 : -1.0;

                const int elem = elem1 > -1 ? elem1 : elem2;

                const double e = Objet_U::axi ? dvdf.dist_norm_bord_axi(num_face) : dvdf.dist_norm_bord(num_face);
//                const double e = eval.get_equivalent_distance(n_bord,num_face);

                const double eps = la_cl_rad.emissivite(num_face_cl, k), T_ext = la_cl_rad.T_ext(num_face_cl, k);

                const double nu = eval.nu_2_impl(elem, k), t_elem = temp(elem, k);

                const double Tb = newton_T_paroi_Champ_P0_VDF(eps, T_ext, nu, e, t_elem, signe);

                val(elem, k) = Tb;
              }
          }
        else if (sub_type(Echange_externe_impose, la_cl.valeur()))
          {
            const Echange_externe_impose& la_cl_ext = ref_cast(Echange_externe_impose, la_cl.valeur());
            for (int num_face = ndeb, num_face_cl = 0; num_face < nfin; num_face++, num_face_cl++)
              {
                const int elem1 = face_voisins(num_face, 0), elem2 = face_voisins(num_face, 1);
                int elem_opp = -1; // si Echange_interne_impose

                if (sub_type(Echange_interne_impose, la_cl_ext))
                  {
                    const Echange_interne_impose& la_cl_int = ref_cast(Echange_interne_impose, la_cl_ext);
                    const Champ_front_calc_interne& Text_int = ref_cast(Champ_front_calc_interne, la_cl_int.T_ext());
                    const IntTab& fmap = Text_int.face_map();
                    int opp_face = fmap[num_face_cl]+ num_face - num_face_cl ;  // num1 is the index of the first face
                    int e1 = face_voisins(opp_face, 0);
                    elem_opp = (e1 != -1) ? e1 : face_voisins(opp_face, 1);
                  }

                const double signe = elem1 > -1 ? 1.0 : -1.0;

                const int elem = elem1 > -1 ? elem1 : elem2;

                const double e = Objet_U::axi ? dvdf.dist_norm_bord_axi(num_face) : dvdf.dist_norm_bord(num_face);

                const double h_imp = la_cl_ext.h_imp(num_face_cl, k) , T_ext = (elem_opp == -1) ? la_cl_ext.T_ext(num_face_cl, k) : temp(elem_opp, k);

                const double nu = eval.nu_2_impl(elem, k), t_elem = temp(elem, k);

                const double heq = (nu == 0.) ? 0. : 1.0 / (1.0 / h_imp + e / nu);

                const double phi = heq * (T_ext - t_elem);

                val(elem, k) = signe * e * phi / nu + t_elem;
              }
          }
        else if (sub_type(Echange_global_impose, la_cl.valeur()))
          {
            const Echange_global_impose& la_cl_glob = ref_cast(Echange_global_impose, la_cl.valeur());
            for (int num_face = ndeb, num_face_cl = 0; num_face < nfin; num_face++, num_face_cl++)
              {
                const int elem1 = face_voisins(num_face, 0), elem2 = face_voisins(num_face, 1);

                const int elem = elem1 > -1 ? elem1 : elem2;

                const double e = Objet_U::axi ? dvdf.dist_norm_bord_axi(num_face) : dvdf.dist_norm_bord(num_face);

                const double h_imp = la_cl_glob.h_imp(num_face_cl, k) , T_ext = la_cl_glob.T_ext(num_face_cl, k);

                const double nu = eval.nu_2_impl(elem, k), t_elem = temp(elem, k);

                const double phi_ext = la_cl_glob.has_phi_ext() ? la_cl_glob.flux_exterieur_impose(num_face_cl,k) : 0;

                const double phi = phi_ext + h_imp * (T_ext - t_elem);

                val(elem, k) = e * phi / nu + t_elem;
              }
          }
        else if (sub_type(Neumann_paroi, la_cl.valeur()))
          for (int num_face = ndeb, num_face_cl = 0; num_face < nfin; num_face++, num_face_cl++)
            {
              const int elem1 = face_voisins(num_face, 0), elem2 = face_voisins(num_face, 1);

              const double signe = elem1 > -1 ? -1.0 : 1.0;

              const int elem = elem1 > -1 ? elem1 : elem2;

              const double e = Objet_U::axi ? dvdf.dist_norm_bord_axi(num_face) : dvdf.dist_norm_bord(num_face);

              const double nu = eval.nu_2_impl(elem, k), t_elem = temp(elem, k);

              val(elem, k) = signe * e * ref_cast(Neumann_paroi, la_cl.valeur()).flux_impose(num_face_cl, k) / nu + t_elem;
            }
      }
}
