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

#include <Dirichlet_homogene.h>
#include <Masse_ajoutee_base.h>
#include <Champ_Face_base.h>
#include <Masse_VDF_Face.h>
#include <Pb_Multiphase.h>
#include <Equation_base.h>
#include <Milieu_base.h>
#include <Zone_Cl_VDF.h>
#include <TRUSTTrav.h>
#include <Dirichlet.h>
#include <Zone_VDF.h>
#include <Symetrie.h>

Implemente_instanciable(Masse_VDF_Face,"Masse_VDF_Face",Masse_VDF_base);

Sortie& Masse_VDF_Face::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Masse_VDF_Face::readOn(Entree& s) { return s ; }

void Masse_VDF_Face::completer()
{
  Solveur_Masse_Face_proto::associer_masse_proto(*this,la_zone_VDF.valeur());
}

DoubleTab& Masse_VDF_Face::appliquer_impl(DoubleTab& sm) const
{
  if (sub_type(Pb_Multiphase, equation().probleme())) return Solveur_Masse_Face_proto::appliquer_impl_proto(sm);
  else
    {

      assert(la_zone_VDF.non_nul());
      assert(la_zone_Cl_VDF.non_nul());
      const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
      const DoubleVect& porosite_face = equation().milieu().porosite_face();
      const DoubleVect& volumes_entrelaces = zone_VDF.volumes_entrelaces();
      const int nb_faces = zone_VDF.nb_faces(), N = sm.line_size();

      if (sm.dimension(0) != nb_faces)
        {
          Cerr << "Masse_VDF_Face::appliquer :  erreur dans la taille de sm" << finl;
          Process::exit();
        }

      // Boucle sur les faces joint

      // Boucle sur les bords
      // Sur les faces qui portent des conditions aux limites de Dirichlet ou de Symetrie
      // la vitesse normale reste egale a sa valeur initiale.
      // Donc sur ces faces vpoint doit rester a 0.

      for (int n_bord = 0; n_bord < zone_VDF.nb_front_Cl(); n_bord++)
        {

          // pour chaque Condition Limite on regarde son type
          const Cond_lim& la_cl = la_zone_Cl_VDF->les_conditions_limites(n_bord);
          const Front_VF& la_front_dis = ref_cast(Front_VF, la_cl.frontiere_dis());
          const int ndeb = la_front_dis.num_premiere_face();
          const int nfin = ndeb + la_front_dis.nb_faces();

          if ( sub_type(Dirichlet,la_cl.valeur()) || sub_type(Dirichlet_homogene, la_cl.valeur()))
            // Pour les faces de Dirichlet on met sm a 0
            for (int f = ndeb; f < nfin; f++)
              for (int n = 0; n < N; n++)
                sm(f, n) = 0;
          else if (sub_type(Symetrie, la_cl.valeur()))
            // Pour les faces de Symetrie on met vpoint a 0
            for (int f = ndeb; f < nfin; f++)
              for (int n = 0; n < N; n++)
                sm(f, n) = 0;
          else
            for (int f = ndeb; f < nfin; f++)
              for (int n = 0; n < N; n++)
                sm(f, n) /= (volumes_entrelaces(f) * porosite_face(f));

        }

      // Boucle sur les faces internes
      const int ndeb = zone_VDF.premiere_face_int();
      for (int f = ndeb; f < nb_faces; f++)
        for (int n = 0; n < N; n++)
          sm(f, n) /= (volumes_entrelaces(f) * porosite_face(f));
      //sm.echange_espace_virtuel();
      //Debog::verifier("Masse_VDF_Face::appliquer sm",sm);
      return sm;
    }
}

void Masse_VDF_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  if (sub_type(Pb_Multiphase, equation().probleme()))
    {
      IntTrav sten(0, 2);
      sten.set_smart_resize(1);
      Solveur_Masse_Face_proto::dimensionner_blocs_proto(matrices, semi_impl, true /* allocate too */, sten);
    }
  else
    Masse_VDF_base::dimensionner_blocs(matrices, semi_impl);
}

void Masse_VDF_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, double dt, const tabs_t& semi_impl, int resoudre_en_increments) const
{
  if (sub_type(Pb_Multiphase, equation().probleme()))
    {
      const DoubleTab& inco = equation().inconnue().valeurs(), &passe = equation().inconnue().passe();
      Matrice_Morse *mat = matrices[equation().inconnue().le_nom().getString()]; //facultatif
      const Zone_VF& zone = ref_cast(Zone_VF, equation().zone_dis().valeur());
      const Conds_lim& cls = ref_cast(Zone_Cl_dis_base, equation().zone_Cl_dis().valeur()).les_conditions_limites();
      const IntTab& f_e = zone.face_voisins(), &fcl = ref_cast(Champ_Face_base, equation().inconnue().valeur()).fcl();
      const DoubleVect& pf = equation().milieu().porosite_face(), &pe = equation().milieu().porosite_elem(), &vf = zone.volumes_entrelaces(), &ve = zone.volumes(), &fs = zone.face_surfaces();
      const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : NULL;
      const DoubleTab& rho = equation().milieu().masse_volumique().passe(),
                       *alpha = pbm ? &pbm->eq_masse.inconnue().passe() : NULL, *a_r = pbm ? &pbm->eq_masse.champ_conserve().passe() : NULL, &vfd = zone.volumes_entrelaces_dir();
      const Masse_ajoutee_base *corr = pbm && pbm->has_correlation("masse_ajoutee") ? &ref_cast(Masse_ajoutee_base, pbm->get_correlation("masse_ajoutee").valeur()) : NULL;
      int i, e, f, nf_tot = zone.nb_faces_tot(), m, n, N = inco.line_size(), d, D = dimension, cR = rho.dimension_tot(0) == 1;

      /* faces : si CLs, pas de produit par alpha * rho en multiphase */
      DoubleTrav masse(N, N), masse_e(N, N); //masse alpha * rho, contribution
      for (f = 0; f < zone.nb_faces(); f++) //faces reelles
        {
          if (!pbm || fcl(f, 0) >= 2)
            for (masse = 0, n = 0; n < N; n++) masse(n, n) = 1; //pas Pb_Multiphase ou CL -> pas de alpha * rho
          else for (masse = 0, i = 0; i < 2; i++)
              if ((e = f_e(f, i)) >= 0)
                {
                  for (masse_e = 0, n = 0; n < N; n++) masse_e(n, n) = (*a_r)(e, n); //partie diagonale
                  if (corr) corr->ajouter(&(*alpha)(e, 0), &rho(!cR * e, 0), masse_e); //partie masse ajoutee
                  for (n = 0; n < N; n++)
                    for (m = 0; m < N; m++) masse(n, m) += vfd(f, i) / vf(f) * masse_e(n, m); //contribution au alpha * rho de la face
                }
          for (n = 0; n < N; n++)
            {
              double fac = pf(f) * vf(f) / dt;
              for (m = 0; m < N; m++) secmem(f, n) -= fac * resoudre_en_increments * masse(n, m) * inco(f, m);
              if (fcl(f, 0) < 2)
                for (m = 0; m < N; m++) secmem(f, n) += fac * masse(n, m) * passe(f, m);
              else if (fcl(f, 0) == 3)
                for (d = 0; d < D; d++)
                  secmem(f, n) += fac * masse(n, n) * ref_cast(Dirichlet, cls[fcl(f, 1)].valeur()).val_imp(fcl(f, 2), N *  + n) * zone.face_normales(f, d) / fs(f);
              if (mat)
                for (m = 0; m < N; m++)
                  if (masse(n, m)) (*mat)(N * f + n, N * f + m) += fac * masse(n, m);
            }
        }
      i++;
    }
  else
    Masse_VDF_base::ajouter_blocs(matrices, secmem, dt, semi_impl, resoudre_en_increments);
}
