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

#include <DP_Impose_VEF_Face.h>
#include <Champ_Don_base.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Synonyme_info.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <Milieu_base.h>
#include <Domaine_VEF.h>
#include <cfloat>

Implemente_instanciable(DP_Impose_VEF_Face, "DP_Impose_VEF_P1NC", Perte_Charge_VEF_Face);

Sortie& DP_Impose_VEF_Face::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& DP_Impose_VEF_Face::readOn(Entree& s)
{
  DP_Impose::lire_donnees(s);
  remplir_num_faces(s);
  if (!mp_max(sgn.size()))
    {
      Cerr << "DP_Impose_PolyMAC_Face : champ d'orientation non renseigne!" << finl;
      Process::exit();
    }
  //fichier de sortie
  set_fichier(Nom("DP_") + identifiant_);
  set_description(Nom("DP impose sur la surface ") + identifiant_);
  Noms col_names;
  if (regul_)
    {
      col_names.add("DP");
      col_names.add("Flow_rate");
      col_names.add("Target_Flow_rate");
    }
  else
    {
      col_names.add("DP");
      col_names.add("dDP/dQ");
      col_names.add("Q");
      col_names.add("Q0");
    }
  set_col_names(col_names);
  return s;
}

void DP_Impose_VEF_Face::completer()
{
  Perte_Charge_VEF_Face::completer();
  bilan().resize(3 + !regul_);
}

void DP_Impose_VEF_Face::remplir_num_faces(Entree& s)
{
  const Domaine& le_domaine = equation().probleme().domaine();
  const Domaine_VEF& dom = ref_cast(Domaine_VEF, equation().domaine_dis().valeur());
  int taille_bloc = dom.nb_elem();
  num_faces.resize(taille_bloc);
  lire_surfaces(s,le_domaine,dom,num_faces, sgn);
  // int nfac_tot = mp_sum(num_faces.size());
  int nfac_max = (int)mp_max(num_faces.size()); // not to count several (number of processes) times the same face

  if(je_suis_maitre() && nfac_max == 0)
    {
      Cerr << "Error when defining the surface plane for the singular porosity :" << finl;
      Cerr << "No mesh faces has been found for the surface plane." << finl;
      Cerr << "Check the coordinate of the surface plane which should match mesh coordinates." << finl;
      Process::exit();
    }

  DoubleTrav S;
  dom.creer_tableau_faces(S);
  for (int i = 0; i < num_faces.size(); i++) S(num_faces(i)) = dom.face_surfaces(num_faces(i));
  surf = mp_somme_vect(S);
}

void DP_Impose_VEF_Face::mettre_a_jour(double temps)
{
  DP_Impose::mettre_a_jour(temps);
  update_dp_regul(equation(), calculate_Q(equation(), num_faces, sgn), bilan());
  if (regul_) return;

  bilan()(2) = calculate_Q(equation(), num_faces, sgn) * (Process::me() ? 0 : 1); //pour eviter le sommage en sortie
}

void DP_Impose_VEF_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const Domaine_VEF& dom = ref_cast(Domaine_VEF, equation().domaine_dis().valeur());

  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  Matrice_Morse& mat = *matrices.at(nom_inco), mat2;

  IntTrav sten(0, 2);
  sten.set_smart_resize(1);

  int i, f, d, db, D = dimension, n, N = equation().inconnue().valeurs().line_size() / D, nf_tot = dom.nb_faces_tot();
  for (i = 0; i < num_faces.size(); i++)
    if ((f = num_faces(i)) < dom.nb_faces())
      for (d = 0; d < D; d++)
        for (db = 0; db < D; db++)
          for (n = 0; n < N; n++)
            sten.append_line(N * (D * f + d) + n, N * (D * f + db) + n);

  tableau_trier_retirer_doublons(sten);
  Matrix_tools::allocate_morse_matrix(N * D * nf_tot, N * D * nf_tot, sten, mat2);
  mat.nb_colonnes() ? mat += mat2 : mat = mat2;
}

void DP_Impose_VEF_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_VEF& dom = ref_cast(Domaine_VEF, equation().domaine_dis().valeur());
  const DoubleVect& pf = equation().milieu().porosite_face(), &fs = dom.face_surfaces();
  const DoubleTab& vit = equation().inconnue().valeurs(), &nf = dom.face_normales();
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  Matrice_Morse *mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : nullptr;
  const int D = dimension, N = vit.line_size() / D;

  double rho = equation().milieu().masse_volumique()(0, 0), fac_rho = equation().probleme().is_dilatable() ? 1.0 : 1.0 / rho;

  if (regul_)
    {
      for (int i = 0, f; i < num_faces.size(); i++)
        if ((f = num_faces(i)) < dom.nb_faces())
          for (int d = 0; d < D; d++)
            secmem(f, d) += nf(f, d) * pf(f) * sgn(i) * dp_regul_ * fac_rho;
    }
  else
    {
      DoubleTrav xvf(num_faces.size(), dimension), DP(num_faces.size(), 3), vn(N);
      for (int i = 0; i < num_faces.size(); i++)
        for (int j = 0; j < dimension; j++)
          xvf(i, j) = dom.xv()(num_faces(i), j);
      DP_.valeur().valeur_aux(xvf, DP);

      for (int i = 0, f; i < num_faces.size(); i++)
        if ((f = num_faces(i)) < dom.nb_faces())
          {
            vn = 0;
            for (int d = 0; d < D; d++)
              for (int n = 0; n < N; n++)
                vn(n) += nf(f, d) * vit(f, N * d + n) / fs(f);

            for (int d = 0; d < D; d++)
              for (int n = 0; n < N; n++)
                secmem(f, N * d + n) += nf(f, d) * pf(f) * sgn(i) * (DP(i, 0) + DP(i, 1) * (surf * sgn(i) * vn(n) - DP(i, 2))) * fac_rho;
            if (mat)
              for (int d = 0; d < D; d++)
                for (int db = 0; db < D; db++)
                  for (int n = 0; n < N; n++)
                    (*mat)(N * (D * f + d) + n, N * (D * f + db) + n) -= nf(f, d) * nf(f, db) / fs(f) * pf(f) * DP(i, 1) * surf * fac_rho;
          }

      bilan()(0) = Process::mp_max(num_faces.size() ? DP(0, 0) : -DBL_MAX);
      bilan()(1) = Process::mp_max(num_faces.size() ? DP(0, 1) / rho : -DBL_MAX);
      bilan()(3) = Process::mp_max(num_faces.size() ? DP(0, 2) * rho : -DBL_MAX);
      if (Process::me())
        bilan() = 0; //pour eviter un sommage en sortie
    }
}
