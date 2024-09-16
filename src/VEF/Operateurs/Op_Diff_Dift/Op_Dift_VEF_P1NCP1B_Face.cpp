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

#include <Op_Dift_VEF_P1NCP1B_Face.h>
#include <Neumann_sortie_libre.h>
#include <Domaine_Cl_VEF.h>
#include <Champ_Uniforme.h>
#include <Domaine_VEF.h>
#include <Champ_P1NC.h>
#include <Periodique.h>
#include <TRUSTLists.h>
#include <Champ_Don.h>
#include <Solv_GCP.h>
#include <Symetrie.h>
#include <Domaine.h>
#include <SSOR.h>

Implemente_instanciable(Op_Dift_VEF_P1NCP1B_Face, "Op_Dift_VEF_P1NCP1B_P1NC", Op_Dift_VEF_base);
// XD diffusion_p1ncp1b diffusion_deriv p1ncp1b 1 not_set

// XD difusion_p1b diffusion_deriv p1b 0 not_set

Sortie& Op_Dift_VEF_P1NCP1B_Face::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Op_Dift_VEF_P1NCP1B_Face::readOn(Entree& s) { return s; }

void Op_Dift_VEF_P1NCP1B_Face::associer(const Domaine_dis_base& domaine_dis, const Domaine_Cl_dis& domaine_cl_dis, const Champ_Inc& ch_transporte)
{
  le_dom_vef = ref_cast(Domaine_VEF, domaine_dis);
  la_zcl_vef = ref_cast(Domaine_Cl_VEF, domaine_cl_dis.valeur());
  inconnue_ = ref_cast(Champ_P1NC, ch_transporte.valeur());
  solveur.typer("Solv_GCP");
  OWN_PTR(Precond_base) p;
  p.typer("SSOR");
  ref_cast(Solv_GCP,solveur.valeur()).set_precond(p);
  solveur.nommer("diffusion_solver");
}

DoubleTab& Op_Dift_VEF_P1NCP1B_Face::calculer_gradient_elem(const DoubleTab& vit, DoubleTab& grad) const
{
  const Domaine_VEF& domaine_VEF = domaine_vef();
  const DoubleTab& face_normales = domaine_VEF.face_normales();
  const IntTab& elem_faces = domaine_VEF.elem_faces();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  const int nfe = domaine_VEF.domaine().nb_faces_elem(), nb_elem_tot = domaine_VEF.domaine().nb_elem_tot();
  int elem, indice, face, compi, compj;

  for (elem = 0; elem < nb_elem_tot; elem++)
    for (indice = 0; indice < nfe; indice++)
      {
        face = elem_faces(elem, indice);
        int signe = 1;
        if (elem != face_voisins(face, 0)) signe = -1;
        for (compi = 0; compi < dimension; compi++)
          for (compj = 0; compj < dimension; compj++)
            grad(elem, compi, compj) += signe * vit(face, compi) * face_normales(face, compj);
      }
  return grad;
}

DoubleTab& Op_Dift_VEF_P1NCP1B_Face::calculer_gradient_som(const DoubleTab& vit, DoubleTab& grad) const
{
  const Domaine_VEF& domaine_VEF = domaine_vef();
  const DoubleTab& face_normales = domaine_VEF.face_normales();
  const IntTab& som_elem = domaine_VEF.domaine().les_elems(), &elem_faces = domaine_VEF.elem_faces(), &face_voisins = domaine_VEF.face_voisins();

  double mijK, miiK, miK = 1. / (dimension + 1) / (dimension + 1);
  const int nfe = domaine_VEF.domaine().nb_faces_elem(), nb_elem_tot = domaine_VEF.domaine().nb_elem_tot();
  const int nps = domaine_VEF.numero_premier_sommet(), nb_som_elem = domaine_VEF.domaine().nb_som_elem();
  const int nb_som = grad.dimension(0) - nb_elem_tot;
  const IntTab& elem_som = domaine_VEF.domaine().les_elems();
  DoubleTab secmem(nb_som, dimension, dimension);

  if (Objet_U::dimension == 2)
    {
      mijK = 1. / 12.;
      miiK = 1. / 6.;
    }
  else
    {
      mijK = 1. / 18.;
      miiK = 1. / 12.;
    }

  if (masse.nb_lignes() < 2)
    {
      Cerr << "On remplit la matrice de masse" << finl;
      (ref_cast_non_const(DoubleTab, savgrad)).resize(nb_som, dimension, dimension);
      Matrice_Morse_Sym& mat = ref_cast_non_const(Matrice_Morse_Sym, masse);
      int rang;
      int nnz = 0;
      IntLists voisins(nb_som);
      DoubleLists coeffs(nb_som);
      DoubleVect diag(nb_som);
      int elem;
      for (elem = 0; elem < nb_elem_tot; elem++)
        {
          double vol = domaine_VEF.volumes(elem);
          double coeff_ij = mijK * vol - miK * vol;
          double coeff_ii = miiK * vol - miK * vol;
          for (int isom = 0; isom < nb_som_elem; isom++)
            {
              int som = elem_som(elem, isom);
              int ii = domaine_VEF.domaine().get_renum_som_perio(som);
              diag[ii] += coeff_ii;
              for (int jsom = isom + 1; jsom < nb_som_elem; jsom++)
                {
                  int sombis = elem_som(elem, jsom);
                  int j = domaine_VEF.domaine().get_renum_som_perio(sombis);
                  int i = ii;
                  if (i > j)
                    {
                      int kl = j;
                      j = i;
                      i = kl;
                    }
                  rang = voisins[i].rang(j);
                  if (rang == -1)
                    {
                      voisins[i].add(j);
                      coeffs[i].add(coeff_ij);
                      nnz++;
                    }
                  else
                    {
                      coeffs[i][rang] += coeff_ij;
                    }
                }
            }
        }

      {
        for (int i = 0; i < nb_som; i++)
          if (diag(i) == 0)
            diag(i) = 1.;
      }

      mat.dimensionner(nb_som, nnz + nb_som);
      mat.remplir(voisins, coeffs, diag);
      mat.compacte();
      mat.set_est_definie(1);
    }
  int elem, indice, face, compi, compj, som;
  ArrOfDouble sigma(dimension);
  static double coeff_som = 1. / (dimension) / (dimension + 1);
  double signe;

  for (elem = 0; elem < nb_elem_tot; elem++)
    {
      sigma = 0;
      for (indice = 0; indice < nfe; indice++)
        {
          face = elem_faces(elem, indice);
          for (int comp = 0; comp < dimension; comp++)
            sigma[comp] += vit(face, comp);
        }
      for (indice = 0; indice < nfe; indice++)
        {
          som = domaine_VEF.domaine().get_renum_som_perio(som_elem(elem, indice));
          face = elem_faces(elem, indice);
          signe = 1;
          if (elem != face_voisins(face, 0))
            signe = -1;
          for (compi = 0; compi < dimension; compi++)
            for (compj = 0; compj < dimension; compj++)
              secmem(som, compi, compj) += coeff_som * signe * sigma[compi] * face_normales(face, compj);
        }
    }
  //Cerr << "Avant CL secmem = " << secmem << finl;
  const Domaine_Cl_VEF& domaine_Cl_VEF = domaine_cl_vef();
  const Conds_lim& les_cl = domaine_Cl_VEF.les_conditions_limites();
  const IntTab& face_sommets = domaine_VEF.face_sommets();
  int nb_bords = les_cl.size();
  for (int n_bord = 0; n_bord < nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      {
        if (!sub_type(Periodique, la_cl.valeur()))
          {
            const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
            int num1 = le_bord.num_premiere_face();
            int num2 = num1 + le_bord.nb_faces();
            for (face = num1; face < num2; face++)
              {
                for (indice = 0; indice < (nfe - 1); indice++)
                  {
                    som = domaine_VEF.domaine().get_renum_som_perio(face_sommets(face, indice));
                    for (compi = 0; compi < dimension; compi++)
                      for (compj = 0; compj < dimension; compj++)
                        secmem(som, compi, compj) += 1. / dimension * vit(face, compi) * face_normales(face, compj);
                  }
              }
          }
      }
    }
  //Cerr << "Apres CL secmem = " << secmem << finl;
  double coeff = 1. / (dimension + 1.);
  {
    DoubleVect secmemij;
    DoubleVect gradij;
    domaine_VEF.domaine().creer_tableau_sommets(secmemij);
    domaine_VEF.domaine().creer_tableau_sommets(gradij);
    DoubleTab& sgrad = ref_cast_non_const(DoubleTab, savgrad);

    for (compi = 0; compi < dimension; compi++)
      for (compj = 0; compj < dimension; compj++)
        {
          int i;
          for (i = 0; i < nb_som; i++)
            {
              int soml = domaine_VEF.domaine().get_renum_som_perio(i);
              secmemij(soml) = secmem(soml, compi, compj);
              gradij(soml) = sgrad(soml, compi, compj);
            }
          for (elem = 0; elem < nb_elem_tot; elem++)
            for (indice = 0; indice < nfe; indice++)
              {
                som = domaine_VEF.domaine().get_renum_som_perio(som_elem(elem, indice));
                secmemij(som) -= coeff * grad(elem, compi, compj);
              }
          (ref_cast_non_const(SolveurSys, solveur)).resoudre_systeme(masse, secmemij, gradij);
          for (i = 0; i < nb_som; i++)
            {
              int soml = domaine_VEF.domaine().get_renum_som_perio(i);
              double x = gradij(soml);
              grad(nps + soml, compi, compj) = x;
              sgrad(soml, compi, compj) = x;
            }
        }
  }
  for (elem = 0; elem < nb_elem_tot; elem++)
    {
      double vol = domaine_VEF.volumes(elem);
      for (compi = 0; compi < dimension; compi++)
        for (compj = 0; compj < dimension; compj++)
          grad(elem, compi, compj) /= vol;
      for (indice = 0; indice < nfe; indice++)
        {
          som = nps + domaine_VEF.domaine().get_renum_som_perio(som_elem(elem, indice));
          for (compi = 0; compi < dimension; compi++)
            for (compj = 0; compj < dimension; compj++)
              grad(elem, compi, compj) -= coeff * grad(som, compi, compj);
        }
    }

  DoubleTab tmp(grad);
  int ntot = tmp.dimension(0);
  for (int ind = 0; ind < ntot; ind++)
    for (compi = 0; compi < dimension; compi++)
      for (compj = 0; compj < dimension; compj++)
        tmp(ind, compi, compj) = 0.5 * (grad(ind, compi, compj) + grad(ind, compj, compi));
  grad = tmp;
  return grad;
}

DoubleTab& Op_Dift_VEF_P1NCP1B_Face::corriger_div_pour_Cl(DoubleTab& div) const
{
  const Domaine_VEF& domaine_VEF = domaine_vef();
  const DoubleTab& face_normales = domaine_VEF.face_normales();
  const Domaine_Cl_VEF& domaine_Cl_VEF = domaine_cl_vef();
  const Conds_lim& les_cl = domaine_Cl_VEF.les_conditions_limites();
  const int nb_bords = les_cl.size();
  for (int n_bord = 0; n_bord < nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      {
        const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
        int num1 = le_bord.num_premiere_face();
        int num2 = num1 + le_bord.nb_faces();
        if (sub_type(Periodique, la_cl.valeur()))
          {
            //periodicite
            const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
            ArrOfInt fait(le_bord.nb_faces());
            for (int face = num1; face < num2; face++)
              {
                if (!fait[face - num1])
                  {
                    int face_associee = num1 + la_cl_perio.face_associee(face - num1);
                    fait[face - num1] = (fait[face_associee - num1] = 1);
                    for (int comp = 0; comp < dimension; comp++)
                      {
                        div(face, comp) += div(face_associee, comp);
                        div(face_associee, comp) = div(face, comp);
                      }
                  }
              }
          }
        else
          {
            if (sub_type(Symetrie, la_cl.valeur()))
              {
                for (int face = num1; face < num2; face++)
                  {
                    double psc = 0;
                    double norme = 0;
                    int comp;
                    for (comp = 0; comp < dimension; comp++)
                      psc += div(face, comp) * face_normales(face, comp);
                    for (comp = 0; comp < dimension; comp++)
                      norme += face_normales(face, comp) * face_normales(face, comp);

                    // psc/=norme; // Fixed bug: Arithmetic exception
                    if (std::fabs(norme) >= DMINFLOAT)
                      psc /= norme;
                    for (comp = 0; comp < dimension; comp++)
                      div(face, comp) -= psc * face_normales(face, comp);
                  }
              }
          }
      }
    }
  return div;
}

DoubleTab& Op_Dift_VEF_P1NCP1B_Face::calculer_divergence_elem(double nu, const DoubleTab& nu_turb, const DoubleTab& grad, DoubleTab& div) const
{
  const Domaine_VEF& domaine_VEF = domaine_vef();
  const DoubleTab& face_normales = domaine_VEF.face_normales();
  const IntTab& elem_faces = domaine_VEF.elem_faces(), &face_voisins = domaine_VEF.face_voisins();
  const int nfe = domaine_VEF.domaine().nb_faces_elem(), nb_elem_tot = domaine_VEF.domaine().nb_elem_tot();

  int elem, indice, face, compi, compj;
  //ArrOfDouble sigma(dimension);
  for (elem = 0; elem < nb_elem_tot; elem++)
    {
      double nu_tot = nu + nu_turb(elem);
      for (indice = 0; indice < nfe; indice++)
        {
          face = elem_faces(elem, indice);
          double signe = 1;
          if (elem != face_voisins(face, 0))
            signe = -1;
          for (compi = 0; compi < dimension; compi++)
            for (compj = 0; compj < dimension; compj++)
              div(face, compi) -= nu_tot * grad(elem, compi, compj) * signe * face_normales(face, compj);
        }
    }
  return div;
}
DoubleTab& Op_Dift_VEF_P1NCP1B_Face::calculer_divergence_som(double nu, const DoubleTab& nu_turb, const DoubleTab& grad, DoubleTab& div) const
{
  const Domaine_VEF& domaine_VEF = domaine_vef();
  const DoubleTab& face_normales = domaine_VEF.face_normales();
  const IntTab& som_elem = domaine_VEF.domaine().les_elems();
  const IntTab& elem_faces = domaine_VEF.elem_faces();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  int nfe = domaine_VEF.domaine().nb_faces_elem();
  int nb_elem_tot = domaine_VEF.domaine().nb_elem_tot();
  int nps = domaine_VEF.numero_premier_sommet();
  int elem, indice, indice2, face, compi, compj, som;
  ArrOfDouble sigma(dimension);
  static double coeff_som = 1. / (dimension) / (dimension + 1);
  for (elem = 0; elem < nb_elem_tot; elem++)
    {
      double nu_tot = nu + nu_turb(elem);
      for (indice = 0; indice < nfe; indice++)
        {
          som = nps + domaine_VEF.domaine().get_renum_som_perio(som_elem(elem, indice));
          face = elem_faces(elem, indice);
          double signe = 1;
          if (elem != face_voisins(face, 0))
            signe = -1;
          for (int comp = 0; comp < dimension; comp++)
            {
              sigma[comp] = signe * face_normales(face, comp);
            }
          for (indice2 = 0; indice2 < nfe; indice2++)
            for (compi = 0; compi < dimension; compi++)
              for (compj = 0; compj < dimension; compj++)
                div(elem_faces(elem, indice2), compi) -= coeff_som * nu_tot * grad(som, compi, compj) * sigma[compj];
        }
    }
  const Domaine_Cl_VEF& domaine_Cl_VEF = domaine_cl_vef();
  const Conds_lim& les_cl = domaine_Cl_VEF.les_conditions_limites();
  const IntTab& face_sommets = domaine_VEF.face_sommets();
  int nb_bords = les_cl.size();
  for (int n_bord = 0; n_bord < nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      {
        const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
        int num1 = le_bord.num_premiere_face();
        int num2 = num1 + le_bord.nb_faces();
        if (sub_type(Neumann_sortie_libre, la_cl.valeur()))
          for (int face2 = num1; face2 < num2; face2++)
            {
              double nu_tot = nu + nu_turb(face_voisins(face2, 0));
              for (indice = 0; indice < (nfe - 1); indice++)
                {
                  som = nps + domaine_VEF.domaine().get_renum_som_perio(face_sommets(face2, indice));
                  for (compi = 0; compi < dimension; compi++)
                    for (compj = 0; compj < dimension; compj++)
                      div(face2, compi) -= nu_tot / dimension * face_normales(face2, compj) * grad(som, compi, compj);
                }
            }
      }
    }
  return div;
}

DoubleTab& Op_Dift_VEF_P1NCP1B_Face::ajouter(const DoubleTab& inconnue, DoubleTab& resu) const
{
  const Domaine_VEF& domaine_VEF = domaine_vef();
  const DoubleTab& nu_turb = diffusivite_turbulente()->valeurs();
  const double nu = diffusivite(0);
  const int nb_elem_tot = domaine_VEF.domaine().nb_elem_tot(), nb_som_tot = domaine_VEF.domaine().nb_som_tot();

  DoubleTab gradient(nb_elem_tot + nb_som_tot, dimension, dimension);
  gradient = 0.;

  calculer_gradient_elem(inconnue, gradient);
  calculer_gradient_som(inconnue, gradient);
  calculer_divergence_elem(nu, nu_turb, gradient, resu);
  calculer_divergence_som(nu, nu_turb, gradient, resu);
  corriger_div_pour_Cl(resu);

  resu.echange_espace_virtuel();

  modifier_flux(*this);
  return resu;
}
