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

#include <Champ_Q1_EF.h>
#include <Domaine_EF.h>
#include <Domaine.h>
#include <Equation_base.h>
#include <distances_EF.h>
#include <Dirichlet_paroi_fixe.h>
#include <Dirichlet_paroi_defilante.h>
#include <Equation_base.h>
#include <Fluide_base.h>
#include <Champ_Uniforme.h>
#include <Modele_turbulence_hyd_base.h>
#include <Debog.h>

Implemente_instanciable(Champ_Q1_EF,"Champ_Q1_EF",Champ_Inc_Q1_base);

Sortie& Champ_Q1_EF::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Champ_Q1_EF::readOn(Entree& s)
{
  lire_donnees(s) ;
  return s ;
}

const Domaine_EF& Champ_Q1_EF::domaine_EF() const
{
  return ref_cast(Domaine_EF, le_dom_VF.valeur());
}

int Champ_Q1_EF::imprime(Sortie& os, int ncomp) const
{
  const Domaine_dis_base& domaine_dis = domaine_dis_base();
  const Domaine& domaine = domaine_dis.domaine();
  const DoubleTab& coord = domaine.coord_sommets();
  const int nb_som = domaine.nb_som();
  const DoubleTab& val = valeurs();
  int som;
  os << nb_som << finl;
  for (som = 0; som < nb_som; som++)
    {
      if (dimension == 3)
        os << coord(som, 0) << " " << coord(som, 1) << " " << coord(som, 2) << " ";
      if (dimension == 2)
        os << coord(som, 0) << " " << coord(som, 1) << " ";
      if (nb_compo_ == 1)
        os << val(som) << finl;
      else
        os << val(som, ncomp) << finl;
    }
  os << finl;
  Cout << "Champ_Q1_EF::imprime FIN >>>>>>>>>> " << finl;
  return 1;
}

void Champ_Q1_EF::gradient(DoubleTab& gradient_elem)
{
  // Calcul du gradient de la vitesse pour le calcul de la vorticite
  // Gradient ordre 1 (valeur moyenne dans un element)
  // Order 1 gradient (mean value within an element)
  const Domaine_EF& domaine_EF_ = domaine_EF();
  const DoubleTab& vitesse = equation().inconnue()->valeurs();
  const IntTab& elems = domaine_EF_.domaine().les_elems();
  int nb_som_elem = domaine_EF_.domaine().nb_som_elem();
  int nb_elems = domaine_EF_.domaine().nb_elem_tot();
  const DoubleVect& volume_thilde = domaine_EF_.volumes_thilde();
  const DoubleTab& Bij_thilde = domaine_EF_.Bij_thilde();

  assert(gradient_elem.dimension_tot(0) == nb_elems);
  assert(gradient_elem.dimension(1) == dimension); // line
  assert(gradient_elem.dimension(2) == dimension); // column

  operator_egal(gradient_elem, 0.); // Espace reel uniquement

  for (int num_elem = 0; num_elem < nb_elems; num_elem++)
    {
      for (int a = 0; a < dimension; a++) // composante numero 1, component number 1
        {
          for (int b = 0; b < dimension; b++) // composante numero 2, component number 2
            {
              for (int j = 0; j < nb_som_elem; j++)
                {
                  int s = elems(num_elem, j);
                  gradient_elem(num_elem, a, b) += vitesse(s, a) * Bij_thilde(num_elem, j, b);
                }
              gradient_elem(num_elem, a, b) /= volume_thilde(num_elem);
            }
        }
    }
}

void Champ_Q1_EF::cal_rot_ordre1(DoubleTab& vorticite)
{
  const Domaine_EF& domaine_EF_ = domaine_EF();
  int nb_elems = domaine_EF_.domaine().nb_elem_tot();

  DoubleTab gradient_elem(0, dimension, dimension);
  // le tableau est initialise dans la methode gradient():
  domaine_EF_.domaine().creer_tableau_elements(gradient_elem, RESIZE_OPTIONS::NOCOPY_NOINIT);
  gradient(gradient_elem);
  Debog::verifier("apres calcul gradient", gradient_elem);
  int num_elem;
  switch(dimension)
    {
    case 2:
      {
        for (num_elem = 0; num_elem < nb_elems; num_elem++)
          {
            vorticite(num_elem) = gradient_elem(num_elem, 1, 0) - gradient_elem(num_elem, 0, 1);
          }
      }
      break;
    case 3:
      {
        for (num_elem = 0; num_elem < nb_elems; num_elem++)
          {
            vorticite(num_elem, 0) = gradient_elem(num_elem, 2, 1) - gradient_elem(num_elem, 1, 2);
            vorticite(num_elem, 1) = gradient_elem(num_elem, 0, 2) - gradient_elem(num_elem, 2, 0);
            vorticite(num_elem, 2) = gradient_elem(num_elem, 1, 0) - gradient_elem(num_elem, 0, 1);
          }
      }
    }
  Debog::verifier("apres calcul vort", vorticite);
  return;
}

void Champ_Q1_EF::calcul_y_plus(const Domaine_Cl_EF& domaine_Cl_EF, DoubleTab& y_plus)
{
  // On initialise le champ y_plus avec une valeur negative,
  // comme ca lorsqu'on veut visualiser le champ pres de la paroi,
  // on n'a qu'a supprimer les valeurs negatives et n'apparaissent
  // que les valeurs aux parois.

  int ndeb, nfin, elem, l_unif;
  double norm_tau, u_etoile, norm_v = 0, dist, d_visco = 0, visco = 1.;
  y_plus = -1.;

  const Domaine_EF& domaine_EF = ref_cast(Domaine_EF, le_dom_VF.valeur());
  const IntTab& face_voisins = domaine_EF.face_voisins();
  int nsom = domaine_EF.nb_som_face();
  int nsom_elem = domaine_EF.domaine().nb_som_elem();
  int nb_nodes_free = nsom_elem - nsom;
  const IntTab& elems=domaine_EF.domaine().les_elems() ;
  const Equation_base& eqn_hydr = equation();
  const DoubleTab& vitesse = eqn_hydr.inconnue()->valeurs();
  const Fluide_base& le_fluide = ref_cast(Fluide_base, eqn_hydr.milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
  const DoubleTab& tab_visco = ch_visco_cin->valeurs();

  if (sub_type(Champ_Uniforme, ch_visco_cin.valeur()))
    {
      visco = tab_visco(0, 0);
      l_unif = 1;
    }
  else
    l_unif = 0;

  DoubleTab yplus_faces(1, 1); // will contain yplus values if available
  int yplus_already_computed = 0; // flag

  const RefObjU& modele_turbulence = eqn_hydr.get_modele(TURBULENCE);
  if (modele_turbulence.non_nul() && sub_type(Modele_turbulence_hyd_base, modele_turbulence.valeur()))
    {
      const Modele_turbulence_hyd_base& mod_turb = ref_cast(Modele_turbulence_hyd_base, modele_turbulence.valeur());
      const Turbulence_paroi_base& loipar = mod_turb.loi_paroi();
      if (loipar.use_shear())
        {
          yplus_faces.resize(domaine_EF.nb_faces_tot());
          yplus_faces.ref(loipar.tab_d_plus());
          yplus_already_computed = 1;
        }
    }

  ArrOfDouble vit(dimension);
  ArrOfDouble val(dimension);
  ArrOfDouble vit_face(dimension);
  ArrOfInt nodes_face(nsom);

  for (int n_bord = 0; n_bord < domaine_EF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_EF.les_conditions_limites(n_bord);

      if (sub_type(Dirichlet_paroi_fixe, la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
          // Loop on real faces
          ndeb = 0;
          nfin = le_bord.nb_faces_tot();

          for (int ind_face = ndeb; ind_face < nfin; ind_face++)
            {

              int num_face=le_bord.num_face(ind_face);
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
                  vit_face=0.;
                  nodes_face=0;
                  for(int jsom=0; jsom<nsom; jsom++)
                    {
                      int num_som = domaine_EF.face_sommets(num_face, jsom);
                      nodes_face[jsom] = num_som;
                      for(int comp=0; comp<dimension; comp++) vit_face[comp]+=vitesse(num_som,comp)/nsom;
                    }
                  vit=0.;
                  // Loop on nodes : vitesse moyenne des noeuds n'appartenant pas a la face CL
                  for (int i=0; i<nsom_elem; i++)
                    {
                      int node=elems(elem,i);
                      int IOK = 1;
                      for(int jsom=0; jsom<nsom; jsom++)
                        if (nodes_face[jsom] == node) IOK=0;
                      // Le noeud contribue
                      if (IOK)
                        for (int j=0; j<dimension; j++)
                          vit[j]+=(vitesse(node,j)-vit_face[j])/nb_nodes_free; // permet de soustraire la vitesse de glissement eventuelle
                    }
                  norm_v = norm_vit_lp(vit,num_face,domaine_EF,val);
                  dist = distance_face_elem(num_face,elem,domaine_EF);
                  dist *= 2.;
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
