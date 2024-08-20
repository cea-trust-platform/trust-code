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

#include <Modele_turbulence_scal_base.h>
#include <Echange_externe_impose.h>
#include <Op_Dift_Stab_VEF_Face.h>
#include <Scalaire_impose_paroi.h>
#include <Neumann_sortie_libre.h>
#include <Check_espace_virtuel.h>
#include <Dirichlet_homogene.h>
#include <Neumann_homogene.h>
#include <Porosites_champ.h>
#include <Neumann_paroi.h>
#include <Periodique.h>
#include <Champ_P1NC.h>
#include <TRUSTTrav.h>
#include <Dirichlet.h>
#include <Symetrie.h>
#include <Debog.h>

Implemente_instanciable(Op_Dift_Stab_VEF_Face, "Op_Dift_VEF_P1NC_stab", Op_Dift_VEF_base);
// XD diffusion_stab diffusion_deriv stab 1 keyword allowing consistent and stable calculations even in case of obtuse angle meshes.
// XD attr standard entier standard 1 to recover the same results as calculations made by standard laminar diffusion operator. However, no stabilization technique is used and calculations may be unstable when working with obtuse angle meshes (by default 0)
// XD attr info entier info 1 developer option to get the stabilizing ratio (by default 0)
// XD attr new_jacobian entier new_jacobian 1 when implicit time schemes are used, this option defines a new jacobian that may be more suitable to get stationary solutions (by default 0)
// XD attr nu entier nu 1 (respectively nut 1) takes the molecular viscosity (resp. eddy viscosity) into account in the velocity gradient part of the diffusion expression (by default nu=1 and nut=1)
// XD attr nut entier nut 1 not_set
// XD attr nu_transp entier nu_transp 1 (respectively nut_transp 1) takes the molecular viscosity (resp. eddy viscosity) into account in the transposed velocity gradient part of the diffusion expression (by default nu_transp=0 and nut_transp=1)
// XD attr nut_transp entier nut_transp 1 not_set

double my_minimum(double a, double b, double c)
{
  if (a <= b) return (a <= c) ? a : c;
  else if (b <= c) return b;
  else return c;
}

double my_maximum(double a, double b, double c)
{
  if (a >= b) return (a >= c) ? a : c;
  else if (b >= c) return b;
  else return c;
}

double my_minimum(double a, double b)
{
  return (a <= b) ? a : b;
}

double my_maximum(double a, double b)
{
  return (a >= b) ? a : b;
}

Sortie& Op_Dift_Stab_VEF_Face::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Op_Dift_Stab_VEF_Face::readOn(Entree& is)
{
  Motcle accouverte = "{", accfermee = "}", motlu;
  Motcles les_mots(6);
  {
    les_mots[0] = "nu";
    les_mots[1] = "nut";
    les_mots[2] = "nu_transp";
    les_mots[3] = "nut_transp";
    les_mots[4] = "standard";
    les_mots[5] = "new_jacobian";
  }

  is >> motlu;
  if (motlu != accouverte)
    Process::exit("Error in Op_Dift_Stab_VEF_Face::readOn(). Option keywords must be preceded by an open brace { !");

  is >> motlu;
  while (motlu != accfermee)
    {
      int rang = les_mots.search(motlu);
      switch(rang)
        {
        case 0:
          is >> nu_lu_;
          break;
        case 1:
          is >> nut_lu_;
          break;
        case 2:
          is >> nu_transp_lu_;
          break;
        case 3:
          is >> nut_transp_lu_;
          break;
        case 4:
          is >> standard_;
          break;
        case 5:
          is >> new_jacobian_;
          break;
        default:
          Cerr << "Error in Op_Dift_Stab_VEF_Face::readOn(). Word " << motlu << " is unknown !" << finl;
          Cerr << "Known keywords are : " << les_mots << finl;
          Process::exit();
          break;
        }
      is >> motlu;
    }

  is >> motlu;
  if (motlu != accfermee)
    {
      Cerr << "Error in Op_Dift_Stab_VEF_Face::readOn(). Final known keyword is the closing brace sign } !" << finl;
      Process::exit();
    }

  return is;
}

/*
 * METHODES GENERIQUES
 */

inline double aij_extradiag__(const Op_Dift_Stab_VEF_Face& z_class, const int elem, const int facei, const int facej, const int dim, const int dim2, const double nu_elem)
{
  const Domaine_VEF& domaine_VEF = z_class.domaine_vef();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  const DoubleTab& face_normales = domaine_VEF.face_normales();
  const DoubleVect& volumes = domaine_VEF.volumes();

  double volume = 0., signei = 1., signej = 1., aij = 0.;

  assert(dim < z_class.equation().inconnue()->valeurs().dimension(1));
  assert(dim2 < z_class.equation().inconnue()->valeurs().dimension(1));
  assert(dim < dim2);

  volume = 1. / volumes(elem);
  volume *= nu_elem;

  if (face_voisins(facei, 0) != elem) signei = -1.;
  if (face_voisins(facej, 0) != elem) signej = -1.;

  aij = face_normales(facei, dim2) * face_normales(facej, dim);
  aij *= signei * signej;
  aij *= volume;

  return aij;
}

template <Type_Champ _TYPE_>
void ajouter_operateur_centre__(const Op_Dift_Stab_VEF_Face& z_class, const DoubleTab& Aij_diag, const DoubleTab& nu, const DoubleTab& inconnue, DoubleTab& resu)
{
  constexpr bool is_VECT = (_TYPE_ == Type_Champ::VECTORIEL);

  const Domaine_VEF& domaine_VEF = z_class.domaine_vef();
  const IntTab& elem_faces = domaine_VEF.elem_faces();
  const int nb_elem_tot = domaine_VEF.nb_elem_tot(), nb_faces_elem = domaine_VEF.domaine().nb_faces_elem(), nb_comp = resu.line_size();
  double nu_elem = 0., aij = 0.;

  for (int elem = 0; elem < nb_elem_tot; elem++)
    {
      if (is_VECT) nu_elem = nu(elem);

      for (int facei_loc = 0; facei_loc < nb_faces_elem; facei_loc++)
        {
          const int facei = elem_faces(elem, facei_loc);

          // Ajout de la partie diagonale : on tient compte de la symetrie de la matrice Aij_diag
          for (int facej_loc = facei_loc + 1; facej_loc < nb_faces_elem; facej_loc++)
            {
              const int facej = elem_faces(elem, facej_loc);

              if (!is_VECT) aij = Aij_diag(elem, facei_loc, facej_loc);

              for (int nc = 0; nc < nb_comp; nc++)
                {
                  if (is_VECT) aij = Aij_diag(elem, facei_loc, facej_loc, nc);

                  const double delta_ij = aij * (inconnue(facej, nc) - inconnue(facei, nc));
                  resu(facei, nc) += delta_ij;
                  resu(facej, nc) -= delta_ij;
                }
            }

          //Ajout de la partie extra-diagonale : on tient compte de la symetrie de la partie extradiagonale de la matrice du laplacien
          if (is_VECT)
            for (int facej_loc = 0; facej_loc < nb_faces_elem; facej_loc++)
              if (facej_loc != facei_loc)
                {
                  const int facej = elem_faces(elem, facej_loc);

                  for (int nc = 0; nc < nb_comp; nc++)
                    for (int nc2 = nc + 1; nc2 < nb_comp; nc2++)
                      {
                        aij = aij_extradiag__(z_class, elem, facei, facej, nc, nc2, nu_elem);

                        double delta_ij = aij * (inconnue(facej, nc2) - inconnue(facei, nc2));
                        resu(facei, nc) += delta_ij;

                        delta_ij = aij * (inconnue(facej, nc) - inconnue(facei, nc));
                        resu(facej, nc2) -= delta_ij;
                      }
                }
        }
    }
}

template <Type_Champ _TYPE_>
void calculer_coefficients__(const Op_Dift_Stab_VEF_Face& z_class, const DoubleTab& nu,  const DoubleTab& nu2 /* transpose */ , DoubleTab& Aij)
{
  constexpr bool is_VECT = (_TYPE_ == Type_Champ::VECTORIEL);

  const Domaine_VEF& domaine_VEF = z_class.domaine_vef();
  const IntTab& elem_faces = domaine_VEF.elem_faces(), &face_voisins = domaine_VEF.face_voisins();
  const DoubleTab& face_normales = domaine_VEF.face_normales();
  const DoubleVect& volumes = domaine_VEF.volumes();
  const int nb_elem_tot = domaine_VEF.nb_elem_tot(), nb_faces_elem = domaine_VEF.domaine().nb_faces_elem(),
            nb_comp = z_class.equation().inconnue()->valeurs().line_size();

  assert(Aij.dimension(0) == nb_elem_tot);
  assert(Aij.dimension(1) == nb_faces_elem);
  assert(Aij.dimension(2) == nb_faces_elem);

  if (is_VECT)
    {
      assert(nb_comp == Objet_U::dimension);
      assert(Aij.nb_dim() == 4);
      assert(Aij.dimension(3) == nb_comp);
    }
  else
    assert(Aij.nb_dim() == 3);

  double nu2_elem = 0.;

  for (int elem = 0; elem < nb_elem_tot; elem++)
    {
      double volume = 1. / volumes(elem);

      double nu_elem = nu(elem);
      nu_elem *= volume;

      if (is_VECT)
        {
          nu2_elem = nu2(elem); // transpose
          nu2_elem *= volume;
        }

      for (int facei_loc = 0; facei_loc < nb_faces_elem; facei_loc++)
        {
          const int facei = elem_faces(elem, facei_loc);

          int signei = 1;
          if (face_voisins(facei, 0) != elem) signei = -1;

          for (int facej_loc = facei_loc + 1; facej_loc < nb_faces_elem; facej_loc++)
            {
              const int facej = elem_faces(elem, facej_loc);

              int signej = 1;
              if (face_voisins(facej, 0) != elem) signej = -1;

              //Partie standard
              double psc = 0.;
              for (int dim = 0; dim < Objet_U::dimension; dim++)
                psc += face_normales(facei, dim) * face_normales(facej, dim);

              psc *= signei * signej;
              psc *= nu_elem;

              if (!is_VECT) // scalaire
                {
                  Aij(elem, facei_loc, facej_loc) = psc;
                  Aij(elem, facej_loc, facei_loc) = psc;
                }
              else
                {
                  for (int dim = 0; dim < nb_comp; dim++)
                    {
                      Aij(elem, facei_loc, facej_loc, dim) = psc;
                      Aij(elem, facej_loc, facei_loc, dim) = psc;
                    }

                  //Partie transposee
                  for (int dim = 0; dim < nb_comp; dim++)
                    {
                      psc = face_normales(facei, dim) * face_normales(facej, dim);
                      psc *= signei * signej;
                      psc *= nu2_elem;

                      Aij(elem, facei_loc, facej_loc, dim) += psc;
                      Aij(elem, facej_loc, facei_loc, dim) += psc;
                    }
                }
            }
        }
    }
}

/*
 * FIN METHODES GENERIQUES
 */

void Op_Dift_Stab_VEF_Face::ajouter_diffusion(const DoubleTab& Aij, const DoubleTab& inconnueTab, DoubleTab& resuTab, const bool is_VECT) const
{
  const Domaine_VEF& domaine_VEF = domaine_vef();
  const IntTab& elem_faces = domaine_VEF.elem_faces();
  const int nb_elem_tot = domaine_VEF.nb_elem_tot(), nb_faces_elem = domaine_VEF.domaine().nb_faces_elem(), nb_comp = resuTab.line_size();
  const DoubleVect& inconnue = inconnueTab;
  DoubleVect& resu = resuTab;
  int elem = 0, facei_loc = 0, facei = 0, facej_loc = 0, facej = 0, dim = 0;
  double inc_i = 0., inc_j = 0., delta_ij = 0., dij = 0.;

  for (elem = 0; elem < nb_elem_tot; elem++)
    for (facei_loc = 0; facei_loc < nb_faces_elem; facei_loc++)
      {
        facei = elem_faces(elem, facei_loc);
        for (facej_loc = facei_loc + 1; facej_loc < nb_faces_elem; facej_loc++)
          {
            facej = elem_faces(elem, facej_loc);
            if (is_VECT) // eq QDM
              for (dim = 0; dim < nb_comp; dim++)
                {
                  const double aij = Aij(elem, facei_loc, facej_loc, dim);
                  if (aij > 0.)
                    {
                      dij = -aij;

                      inc_i = inconnue[facei * nb_comp + dim];
                      inc_j = inconnue[facej * nb_comp + dim];
                      delta_ij = dij * (inc_j - inc_i);

                      resu[facei * nb_comp + dim] += delta_ij;
                      resu[facej * nb_comp + dim] -= delta_ij;
                    }
                }
            else
              {
                const double aij = Aij(elem, facei_loc, facej_loc);
                if (aij > 0.)
                  {
                    dij = -aij;
                    for (dim = 0; dim < nb_comp; dim++)
                      {
                        inc_i = inconnue[facei * nb_comp + dim];
                        inc_j = inconnue[facej * nb_comp + dim];
                        delta_ij = dij * (inc_j - inc_i);

                        resu[facei * nb_comp + dim] += delta_ij;
                        resu[facej * nb_comp + dim] -= delta_ij;
                      }
                  }
              }
          }
      }
}

void Op_Dift_Stab_VEF_Face::ajouter_antidiffusion(const DoubleTab& Aij, const DoubleTab& inconnueTab, DoubleTab& resuTab, const bool is_VECT) const
{
  const Domaine_VEF& domaine_VEF = domaine_vef();
  const DoubleTab& xv = domaine_VEF.xv();
  const IntTab& elem_faces = domaine_VEF.elem_faces();
  const int nb_elem_tot = domaine_VEF.nb_elem_tot(), nb_faces_tot = domaine_VEF.nb_faces_tot(),
            nb_faces_elem = domaine_VEF.domaine().nb_faces_elem(),  nb_comp = resuTab.line_size();

  const DoubleVect& inconnue = inconnueTab;
  DoubleVect& resu = resuTab;

  DoubleTab rij(Objet_U::dimension), rji(rij);

  int elem = 0, facei_loc = 0, facei = 0, facej_loc = 0, facej = 0, dim = 0, dim2 = 0;
  double inc_i = 0., inc_j = 0., delta_ij = 0., delta_imax = 0., delta_imin = 0.;
  double delta_jmax = 0., delta_jmin = 0., sij = 0., muij = 0., muji = 0.;

  bool ok_facei = false, ok_facej = false;

  DoubleTab Minima(nb_faces_tot), Maxima(nb_faces_tot);

  for (dim = 0; dim < nb_comp; dim++)
    {
      calculer_min_max(inconnueTab, dim, Minima, false /* is_max */);
      calculer_min_max(inconnueTab, dim, Maxima, true /* is_max */);

      for (elem = 0; elem < nb_elem_tot; elem++)
        for (facei_loc = 0; facei_loc < nb_faces_elem; facei_loc++)
          {
            facei = elem_faces(elem, facei_loc);
            ok_facei = is_dirichlet_faces_(facei);
            inc_i = inconnue[facei * nb_comp + dim];

            delta_imin = Minima(facei) - inc_i;
            delta_imax = Maxima(facei) - inc_i;
            assert(delta_imax >= 0.);
            assert(delta_imin <= 0.);

            for (facej_loc = facei_loc + 1; facej_loc < nb_faces_elem; facej_loc++)
              {
                const double aij = is_VECT ? Aij(elem, facei_loc, facej_loc, dim) : Aij(elem, facei_loc, facej_loc);

                if (aij > 0.)
                  {
                    facej = elem_faces(elem, facej_loc);
                    ok_facej = is_dirichlet_faces_(facej);
                    inc_j = inconnue[facej * nb_comp + dim];

                    for (dim2 = 0; dim2 < Objet_U::dimension; dim2++)
                      rij(dim2) = xv(facej, dim2) - xv(facei, dim2);

                    rji = rij;
                    rji *= -1.;

                    delta_ij = inc_i - inc_j;
                    delta_jmin = Minima(facej) - inc_j;
                    delta_jmax = Maxima(facej) - inc_j;
                    assert(delta_jmax >= 0.);
                    assert(delta_jmin <= 0.);

                    muij = calculer_gradients(facei, rij);
                    muji = calculer_gradients(facej, rji);

                    sij = 0.; //reste a 0 si que des faces de Dirichlet
                    if (delta_ij > 0.)
                      {
                        muij *= delta_imax;
                        muji *= -delta_jmin;

                        if (!ok_facei && !ok_facej) //pas de face de Dirichlet
                          sij = my_minimum(muij, delta_ij, muji);
                        if (!ok_facei && ok_facej) //facej Dirichlet et pas facei
                          sij = my_minimum(muij, delta_ij);
                        if (ok_facei && !ok_facej) //facei Dirichlet et pas facej
                          sij = my_minimum(delta_ij, muji);
                      }
                    else if (delta_ij < 0.)
                      {
                        muij *= delta_imin;
                        muji *= -delta_jmax;

                        if (!ok_facei && !ok_facej) //pas de face de Dirichlet
                          sij = my_maximum(muij, delta_ij, muji);
                        if (!ok_facei && ok_facej) //facej Dirichlet et pas facei
                          sij = my_maximum(muij, delta_ij);
                        if (ok_facei && !ok_facej) //facei Dirichlet et pas facej
                          sij = my_maximum(delta_ij, muji);
                      }

                    resu[facei * nb_comp + dim] -= aij * sij;
                    resu[facej * nb_comp + dim] += aij * sij;

                  }
              }
          }
    }
}

double Op_Dift_Stab_VEF_Face::calculer_gradients(int facei, const DoubleTab& rij) const
{
  const Domaine_VEF& domaine_VEF = domaine_vef();
  const IntTab& elem_faces = domaine_VEF.elem_faces(), &face_voisins = domaine_VEF.face_voisins(), &get_num_fac_loc = domaine_VEF.get_num_fac_loc();
  const DoubleTab& face_normales = domaine_VEF.face_normales();
  const DoubleVect& volumes = domaine_VEF.volumes();
  const int nb_faces_elem = domaine_VEF.domaine().nb_faces_elem();
  double volume = 0., signek = 0., psc = 0., mu_ij = 0.;
  int elem = 0, elem_loc = 0, facei_loc = 0, facek_loc = 0, facek = 0, dim = 0;

  for (elem_loc = 0; elem_loc < 2; elem_loc++)
    {
      facei_loc = get_num_fac_loc(facei, elem_loc);
      elem = face_voisins(facei, elem_loc);

      if (elem != -1)
        {
          volume += volumes(elem);

          for (facek_loc = 0; facek_loc < nb_faces_elem; facek_loc++)
            if (facek_loc != facei_loc)
              {
                facek = elem_faces(elem, facek_loc);

                signek = 1.;
                if (face_voisins(facek, 0) != elem)
                  signek = -1.;

                psc = 0.;
                for (dim = 0; dim < Objet_U::dimension; dim++)
                  psc += face_normales(facek, dim) * rij(dim);
                psc *= signek;
                if (psc < 0.)
                  psc *= -1.;

                mu_ij += psc;
              }
        }
    }

  mu_ij /= volume;
  mu_ij *= 2.;
  return mu_ij;
}

void Op_Dift_Stab_VEF_Face::calculer_min_max(const DoubleTab& inconnueTab, int& dim, DoubleTab& minima_ou_maxima, const bool is_max) const
{
  const Domaine_VEF& domaine_VEF = domaine_vef();
  const int nb_elem_tot = domaine_VEF.nb_elem_tot(), nb_faces_elem = domaine_VEF.domaine().nb_faces_elem(),
            nb_faces_tot = domaine_VEF.nb_faces_tot(), nb_comp = inconnueTab.line_size();
  const IntTab& elem_faces = domaine_VEF.elem_faces();
  int elem = 0, facei_loc = 0, facei = 0, facej_loc = 0, facej = 0;
  double inc_i = 0., inc_j = 0.;
  const DoubleVect& inconnue = inconnueTab;

  assert(minima_ou_maxima.nb_dim() == 1);
  assert(minima_ou_maxima.dimension(0) == nb_faces_tot);
  assert(dim < nb_comp);

  for (facei = 0; facei < nb_faces_tot; facei++)
    minima_ou_maxima(facei) = inconnue[facei * nb_comp + dim];

  for (elem = 0; elem < nb_elem_tot; elem++)
    for (facei_loc = 0; facei_loc < nb_faces_elem; facei_loc++)
      {
        facei = elem_faces(elem, facei_loc);

        inc_i = inconnue[facei * nb_comp + dim];
        double& min_ou_maxi = minima_ou_maxima(facei);

        for (facej_loc = facei_loc + 1; facej_loc < nb_faces_elem; facej_loc++)
          {
            facej = elem_faces(elem, facej_loc);

            inc_j = inconnue[facej * nb_comp + dim];
            double& min_ou_maxj = minima_ou_maxima(facej);

            if (is_max)
              {
                if (inc_j > min_ou_maxi)
                  min_ou_maxi = inc_j;
                if (inc_i > min_ou_maxj)
                  min_ou_maxj = inc_i;
              }
            else
              {
                if (inc_j < min_ou_maxi)
                  min_ou_maxi = inc_j;
                if (inc_i < min_ou_maxj)
                  min_ou_maxj = inc_i;
              }
          }
      }

  const Domaine_Cl_VEF& domaine_Cl_VEF = domaine_cl_vef();
  const Conds_lim& les_cl = domaine_Cl_VEF.les_conditions_limites();
  const int nb_bords = les_cl.size();
  int num1 = 0, num2 = 0, n_bord = 0, ind_face = 0;

  for (n_bord = 0; n_bord < nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());

      num1 = 0;
      num2 = le_bord.nb_faces_tot();

      if (sub_type(Periodique, la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());

          for (ind_face = num1; ind_face < num2; ind_face++)
            {
              facei = le_bord.num_face(ind_face);
              facej = le_bord.num_face(la_cl_perio.face_associee(ind_face));

              if (facei < facej)
                {
                  double& min_ou_maxi = minima_ou_maxima(facei);
                  double& min_ou_maxj = minima_ou_maxima(facej);
                  if (is_max)
                    {
                      if (min_ou_maxi > min_ou_maxj)
                        min_ou_maxj = min_ou_maxi;
                      if (min_ou_maxj > min_ou_maxi)
                        min_ou_maxi = min_ou_maxj;
                    }
                  else
                    {
                      if (min_ou_maxi < min_ou_maxj)
                        min_ou_maxj = min_ou_maxi;
                      if (min_ou_maxj < min_ou_maxi)
                        min_ou_maxi = min_ou_maxj;
                    }
                }
            }
        }

    }
}

void Op_Dift_Stab_VEF_Face::completer()
{
  Op_Dift_VEF_base::completer();

  const Domaine_VEF& domaine_VEF = domaine_vef();
  const Domaine_Cl_VEF& domaine_Cl_VEF = domaine_cl_vef();
  const Conds_lim& les_cl = domaine_Cl_VEF.les_conditions_limites();
  const int nb_bord = les_cl.size(), nb_faces_tot = domaine_VEF.nb_faces_tot();
  int ind_face = -1;

  is_dirichlet_faces_.resize(nb_faces_tot);
  is_dirichlet_faces_ = 0;

  for (int n_bord = 0; n_bord < nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
      int nb_faces_bord_tot = le_bord.nb_faces_tot(), face = -1;

      if ((sub_type(Dirichlet, la_cl.valeur())) || (sub_type(Dirichlet_homogene, la_cl.valeur())))
        for (ind_face = 0; ind_face < nb_faces_bord_tot; ind_face++)
          {
            face = le_bord.num_face(ind_face);
            is_dirichlet_faces_(face) = 1;
          }
    }
}

void Op_Dift_Stab_VEF_Face::ajouter_cas_scalaire(const DoubleTab& inconnue, const DoubleTab& nu, const DoubleTab& nu_turb_m, DoubleTab& resu2) const
{
  const Domaine_VEF& domaine_VEF = domaine_vef();
  const int nb_elem_tot = domaine_VEF.nb_elem_tot(), nb_faces_elem = domaine_VEF.domaine().nb_faces_elem();
  DoubleTab Aij(nb_elem_tot, nb_faces_elem, nb_faces_elem), nu_total(nu);
  nu_total += nu_turb_m;

  calculer_coefficients__<Type_Champ::SCALAIRE>(*this, nu_total, nu_total /* poubelle */, Aij);
  ajouter_operateur_centre__<Type_Champ::SCALAIRE>(*this, Aij, nu, inconnue, resu2);

  if (!standard_)
    {
      ajouter_diffusion(Aij, inconnue, resu2, false /* is_VECT */);
      ajouter_antidiffusion(Aij, inconnue, resu2, false /* is_VECT */);
    }
}

void Op_Dift_Stab_VEF_Face::ajouter_cas_vectoriel(const DoubleTab& inconnue, const DoubleTab& nu, const DoubleTab& nu_turb_m, DoubleTab& resu2) const
{
  const Domaine_VEF& domaine_VEF = domaine_vef();
  const int nb_elem_tot = domaine_VEF.nb_elem_tot(), nb_faces_elem = domaine_VEF.domaine().nb_faces_elem(), nb_comp = resu2.line_size();

  DoubleTab Aij_diag(nb_elem_tot, nb_faces_elem, nb_faces_elem, nb_comp);

  DoubleTab nu_total(nu);
  if (nu_lu_ == 0) nu_total = 0.;
  if (nut_lu_) nu_total += nu_turb_m;

  DoubleTab nu_transp_total(nu);
  if (nu_transp_lu_ == 0) nu_transp_total = 0.;
  if (nut_transp_lu_) nu_transp_total += nu_turb_m;

  calculer_coefficients__<Type_Champ::VECTORIEL>(*this, nu_total, nu_transp_total, Aij_diag);
  ajouter_operateur_centre__<Type_Champ::VECTORIEL>(*this, Aij_diag, nu_transp_total, inconnue, resu2);

  if (!standard_)
    {
      ajouter_diffusion(Aij_diag, inconnue, resu2, true /* is_VECT */);
      ajouter_antidiffusion(Aij_diag, inconnue, resu2, true /* is_VECT */);
    }
}

DoubleTab& Op_Dift_Stab_VEF_Face::ajouter(const DoubleTab& inconnue_org, DoubleTab& resu) const
{
  const DoubleTab& nu_turb = diffusivite_turbulente()->valeurs();
  const DoubleVect& porosite_face = equation().milieu().porosite_face(), &porosite_elem = equation().milieu().porosite_elem();
  const int nb_comp = resu.line_size();

  // On dimensionne et initialise le tableau des bilans de flux:
  flux_bords_.resize(le_dom_vef->nb_faces_bord(), nb_comp);
  flux_bords_ = 0.;

  DoubleTab resu2(resu);
  resu2 = 0.;

  // soit on a div(phi nu grad inco) OU div(nu grad phi inco) : cela depend si on diffuse phi_psi ou psi
  DoubleTab nu, nu_turb_m, tab_inconnue_;

  const int marq = phi_psi_diffuse(equation());

  remplir_nu(nu_);
  modif_par_porosite_si_flag(nu_, nu, !marq, porosite_elem);
  modif_par_porosite_si_flag(nu_turb, nu_turb_m, !marq, porosite_elem);
  const DoubleTab& inconnue = modif_par_porosite_si_flag(inconnue_org, tab_inconnue_, marq, porosite_face);

  assert_espace_virtuel_vect(nu);
  assert_espace_virtuel_vect(inconnue);
  assert_espace_virtuel_vect(nu_turb_m);
  Debog::verifier("Op_Dift_Stab_VEF_Face::ajouter nu", nu);
  Debog::verifier("Op_Dift_Stab_VEF_Face::ajouter nu_turb", nu_turb_m);
  Debog::verifier("Op_Dift_Stab_VEF_Face::ajouter inconnue_org", inconnue_org);
  Debog::verifier("Op_Dift_Stab_VEF_Face::ajouter inconnue", inconnue);

  if (equation().inconnue()->nature_du_champ() != vectoriel)
    ajouter_cas_scalaire(inconnue, nu, nu_turb_m, resu2);
  else
    ajouter_cas_vectoriel(inconnue, nu, nu_turb_m, resu2);

  modifie_pour_cl_gen<true /* _IS_STAB_ */>(inconnue, resu2, flux_bords_);

  resu -= resu2;  // -= car le laplacien est place en terme source dans l'equation
  modifier_flux(*this);
  return resu;
}

void Op_Dift_Stab_VEF_Face::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  modifier_matrice_pour_periodique_avant_contribuer(matrice, equation());
  remplir_nu(nu_); // On remplit le tableau nu car l'assemblage d'une matrice avec ajouter_contribution peut se faire avant le premier pas de temps

  const DoubleTab& nu_turb_ = diffusivite_turbulente()->valeurs();
  DoubleTab nu, nu_turb;

  int marq = phi_psi_diffuse(equation());
  const DoubleVect& porosite_elem = equation().milieu().porosite_elem();

  // soit on a div(phi nu grad inco) OU on a div(nu grad phi inco) : cela depend si on diffuse phi_psi ou psi
  modif_par_porosite_si_flag(nu_, nu, !marq, porosite_elem);
  modif_par_porosite_si_flag(nu_turb_, nu_turb, !marq, porosite_elem);

  DoubleVect porosite_eventuelle(equation().milieu().porosite_face());
  if (!marq) porosite_eventuelle = 1;

  if (equation().inconnue()->nature_du_champ() == vectoriel)
    {
      if (!new_jacobian_)
        {
          ajouter_contribution_bord_gen<Type_Champ::VECTORIEL>(inco, matrice, nu, nu_turb, porosite_eventuelle);
          ajouter_contribution_interne_gen<Type_Champ::VECTORIEL>(inco, matrice, nu, nu_turb, porosite_eventuelle);
        }
      else /* _IS_STAB_ = true */
        {
          ajouter_contribution_bord_gen<Type_Champ::VECTORIEL, true /* _IS_STAB_ */>(inco, matrice, nu, nu_turb, porosite_eventuelle);
          ajouter_contribution_interne_gen<Type_Champ::VECTORIEL, true /* _IS_STAB_ */>(inco, matrice, nu, nu_turb, porosite_eventuelle);
        }
    }
  else
    {
      if (!new_jacobian_)
        {
          ajouter_contribution_bord_gen<Type_Champ::SCALAIRE>(inco, matrice, nu, nu_turb, porosite_eventuelle);
          ajouter_contribution_interne_gen<Type_Champ::SCALAIRE>(inco, matrice, nu, nu_turb, porosite_eventuelle);
        }
      else /* _IS_STAB_ = true */
        {
          ajouter_contribution_bord_gen<Type_Champ::SCALAIRE, true /* _IS_STAB_ */>(inco, matrice, nu, nu_turb, porosite_eventuelle);
          ajouter_contribution_interne_gen<Type_Champ::SCALAIRE, true /* _IS_STAB_ */>(inco, matrice, nu, nu_turb, porosite_eventuelle);
        }
    }

  modifier_matrice_pour_periodique_apres_contribuer(matrice, equation());
}
