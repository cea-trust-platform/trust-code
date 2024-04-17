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

#include <Modele_turbulence_hyd_LES_Wale_VDF.h>
#include <Schema_Temps_base.h>
#include <Champ_Face_VDF.h>
#include <Domaine_Cl_VDF.h>
#include <Equation_base.h>
#include <Domaine_VDF.h>
#include <TRUSTTrav.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Modele_turbulence_hyd_LES_Wale_VDF, "Modele_turbulence_hyd_sous_maille_Wale_VDF", Modele_turbulence_hyd_LES_VDF_base);

Modele_turbulence_hyd_LES_Wale_VDF::Modele_turbulence_hyd_LES_Wale_VDF()
{
  declare_support_masse_volumique(1);
}

Sortie& Modele_turbulence_hyd_LES_Wale_VDF::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Modele_turbulence_hyd_LES_Wale_VDF::readOn(Entree& is) { return Modele_turbulence_hyd_LES_VDF_base::readOn(is); }

void Modele_turbulence_hyd_LES_Wale_VDF::set_param(Param& param)
{
  Modele_turbulence_hyd_LES_VDF_base::set_param(param);
  param.ajouter("cw", &cw_);
  param.ajouter_condition("value_of_cw_ge_0", "sous_maille_Wale_VDF model constant must be positive.");
}

Champ_Fonc& Modele_turbulence_hyd_LES_Wale_VDF::calculer_viscosite_turbulente()
{
  const Domaine_VDF& domaine_VDF = ref_cast(Domaine_VDF, le_dom_VF_.valeur());
  const double temps = mon_equation_->inconnue().temps();
  DoubleTab& visco_turb = la_viscosite_turbulente_.valeurs();

  if (est_egal(cw_, 0., 1.e-15))
    visco_turb = 0.;
  else
    {
      const int nb_elem = domaine_VDF.domaine().nb_elem(), nb_elem_tot = domaine_VDF.nb_elem_tot();

      OP1_.resize(nb_elem_tot);  // OP1 est le premier operateur spatial du modele WALE.
      OP2_.resize(nb_elem_tot);  // OP2 est le deuxieme operateur spatial du modele WALE.

      calculer_OP1_OP2();

      if (visco_turb.size() != nb_elem)
        {
          Cerr << "Size error for the array containing the values of the turbulent viscosity." << finl;
          exit();
        }

      for (int elem = 0; elem < nb_elem; elem++)
        {
          if (OP1_[elem] != 0.) // donc sd2 (et OP2 par voie de consequence) sont differents de zero
            visco_turb[elem] = cw_ * cw_ * l_(elem) * l_(elem) * OP1_[elem] / OP2_[elem];
          else
            visco_turb[elem] = 0;
        }
    }

  la_viscosite_turbulente_.changer_temps(temps);

  return la_viscosite_turbulente_;
}

void Modele_turbulence_hyd_LES_Wale_VDF::calculer_OP1_OP2()
{
  Champ_Face_VDF& vit = ref_cast(Champ_Face_VDF, mon_equation_->inconnue().valeur());
  const DoubleTab& vitesse = mon_equation_->inconnue().valeurs();
  const Domaine_VDF& domaine_VDF = ref_cast(Domaine_VDF, le_dom_VF_.valeur());
  const Domaine_Cl_VDF& domaine_Cl_VDF = ref_cast(Domaine_Cl_VDF, le_dom_Cl_.valeur());
  const int nb_elem = domaine_VDF.domaine().nb_elem_tot(), nb_elem_tot = domaine_VDF.nb_elem_tot();

  const IntTab& face_voisins = domaine_VDF.face_voisins(), &elem_faces = domaine_VDF.elem_faces();

  DoubleTrav gij2(dimension, dimension), sd(dimension, dimension);

  double gkk2, sd2, Sij, Sij2;

  assert(vitesse.line_size() == 1);
  DoubleTab duidxj(nb_elem_tot, dimension, dimension, vitesse.line_size());

  vit.calcul_duidxj(vitesse, duidxj, domaine_Cl_VDF);

  for (int elem = 0; elem < nb_elem; elem++)
    {
      //Calcul du terme gij2
      for (int i = 0; i < dimension; i++)
        for (int j = 0; j < dimension; j++)
          {
            gij2(i, j) = 0;

            for (int k = 0; k < dimension; k++)
              gij2(i, j) += duidxj(elem, i, k, 0) * duidxj(elem, k, j, 0);
          }

      // Calcul du terme gkk2
      gkk2 = 0;
      for (int k = 0; k < dimension; k++)
        gkk2 += gij2(k, k);

      // Calcul de sd
      for (int i = 0; i < dimension; i++)
        for (int j = 0; j < dimension; j++)
          {
            sd(i, j) = 0.5 * (gij2(i, j) + gij2(j, i));
            if (i == j)
              sd(i, j) -= gkk2 / 3.; // Terme derriere le tenseur de Kronecker
          }

      // Calcul de sd2 et Sij2
      sd2 = 0.;
      Sij2 = 0.;

      int face1 = 0, face2 = 0;
      int elem1, elem2;

      for (int i = 0; i < dimension; i++)
        for (int j = 0; j < dimension; j++)
          {
            sd2 += sd(i, j) * sd(i, j);
            //Deplacement du calcul de sij
            Sij = 0.5 * (duidxj(elem, i, j, 0) + duidxj(elem, j, i, 0));

            // PQ : 24/01/07 : le stencil de Sij est par contruction de :
            //                   -  1 maille pour les termes diagonaux Sii
            //                   - ~2 mailles pour les termes croises Sij
            //
            // Wale s'appuyant a la fois sur sd2 (porte par Sij) et sur Sij2 (porte principalement par Sii)
            // est sensible a cette difference de stencil.
            // En portant le stencil a 3 maille spour le calcul de Sii, on retrouve en THI
            // le bon taux de dissipation ainsi que des spectres possedant la bonne allure en k^-5/3.
            //
            // A traiter : Quid sur canal plan ???

            if (i == j)  // augmentation du stencil de Sii
              {
                face1 = elem_faces(elem, i);
                face2 = elem_faces(elem, i + dimension);

                elem1 = face_voisins(face1, 0);
                elem2 = face_voisins(face2, 1);

                //if(elem1==elem) elem1=face_voisins(face1,1);  // par construction il n'y a pas besoin
                //if(elem2==elem) elem2=face_voisins(face2,0);  // par construction il n'y a pas besoin

                // si pas de bord a proximite on passe au stencil de 3 mailles
                // sinon on reste au stencil a 1 maille

                if (elem1 >= 0 && elem2 >= 0)
                  Sij = ((duidxj(elem1, i, i, 0) + duidxj(elem, i, i, 0) + duidxj(elem2, i, i, 0))) / 3.;
              }

            Sij2 += Sij * Sij;
          }

      // Calcul de OP1 et OP2
      OP1_(elem) = pow(sd2, 1.5);
      OP2_(elem) = pow(Sij2, 2.5) + pow(sd2, 1.25);

    }                // fin de la boucle sur les elements
}
