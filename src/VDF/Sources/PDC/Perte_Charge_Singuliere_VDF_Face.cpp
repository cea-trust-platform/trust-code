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

#include <Perte_Charge_Singuliere_VDF_Face.h>
#include <Champ_Face_VDF.h>
#include <Equation_base.h>
#include <Pb_Multiphase.h>
#include <Matrice_Morse.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <Domaine_VDF.h>
#include <Motcle.h>

Implemente_instanciable(Perte_Charge_Singuliere_VDF_Face,"Perte_Charge_Singuliere_VDF_Face",Perte_Charge_VDF_Face);

Sortie& Perte_Charge_Singuliere_VDF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

Entree& Perte_Charge_Singuliere_VDF_Face::readOn(Entree& s)
{
  Perte_Charge_Singuliere::lire_donnees(s);
  remplir_num_faces(s);
  if (regul_) //fichier de sortie si regulation
    {
      bilan().resize(3); //K deb cible
      set_fichier(Nom("K_") + identifiant_);
      set_description(Nom("Regulation du Ksing de la surface ") + identifiant_ + "\nt K deb cible");
    }
  return s;
}

void Perte_Charge_Singuliere_VDF_Face::completer()
{
  Perte_Charge_VDF_Face::completer();
  // eq_masse besoin de champ_conserve !
  if (sub_type(Pb_Multiphase, mon_equation->probleme()))
    ref_cast(Pb_Multiphase, mon_equation->probleme()).equation_masse().init_champ_conserve();
}

void Perte_Charge_Singuliere_VDF_Face::remplir_num_faces(Entree& s)
{
  const Domaine& le_domaine = equation().probleme().domaine();
  const Domaine_VDF& domaine_VDF = ref_cast(Domaine_VDF,equation().domaine_dis());
  int taille_bloc = domaine_VDF.nb_elem();
  num_faces.resize(taille_bloc);
  lire_surfaces(s,le_domaine,domaine_VDF,num_faces, sgn);
  int nfac = num_faces.size();
  int nfac_tot = static_cast<int>(mp_sum(num_faces.size()));
  if (nfac_tot==0)
    {
      Cerr << "Erreur a la lecture des donnees de la perte de charge singuliere :" << finl;
      Cerr << " la surface sur laquelle vous avez defini une perte de charge" << finl;
      Cerr << " ne contient entierement aucune des faces de la geometrie " <<  finl;
      exit();
    }

  if (nfac != 0)
    {
      int ori = domaine_VDF.orientation(num_faces[0]);
      if (ori != direction_perte_charge())
        {
          Cerr << "Erreur a la lecture des donnees de la perte de charge singuliere : " << finl;
          Cerr << " l'orientation de la surface est differente de la direction" << finl;
          Cerr << " de la perte de charge" << finl;
          exit();
        }
    }
}


void Perte_Charge_Singuliere_VDF_Face::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  Matrice_Morse *mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : nullptr, mat2;

  const Domaine_VDF& domaine_VDF = le_dom_VDF.valeur();
  IntTab stencil(0, 2);

  for (int f = 0; f < domaine_VDF.nb_faces(); f++)
    stencil.append_line(f, f);
  tableau_trier_retirer_doublons(stencil);
  if (mat && axi)
    {
      Matrix_tools::allocate_morse_matrix(domaine_VDF.nb_faces_tot(), domaine_VDF.nb_faces_tot(), stencil, mat2);
      mat->nb_colonnes() ? *mat += mat2 : *mat = mat2;
    }

}

void Perte_Charge_Singuliere_VDF_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  const DoubleTab& inco = semi_impl.count(nom_inco) ? semi_impl.at(nom_inco) : equation().inconnue().valeurs();
  Matrice_Morse *mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : nullptr;
  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : nullptr;

  const Domaine_VDF& domaine_VDF = le_dom_VDF.valeur();
  const DoubleVect& volumes_entrelaces = domaine_VDF.volumes_entrelaces();
  const DoubleVect& porosite_surf = equation().milieu().porosite_face();
  const DoubleTab& vit = la_vitesse->valeurs(),&vfd = domaine_VDF.volumes_entrelaces_dir(),
                   *alpha = pbm ? &pbm->equation_masse().inconnue().passe() : nullptr,
                    *a_r = pbm ? &pbm->equation_masse().champ_conserve().passe() : nullptr;;
  int ndeb_faces_int = domaine_VDF.premiere_face_int();

  const int nb_faces = num_faces.size(),  N = equation().inconnue().valeurs().line_size();
  double Ck;
  const IntTab& f_e = domaine_VDF.face_voisins();
  DoubleTrav aar_f(N); //alpha * alpha * rho a chaque face

  if (axi)
    for (int i=0; i<nb_faces; i++)
      {
        const int numfa = num_faces[i];

        if (pbm)
          {
            aar_f = 0.;
            for (int j = 0; j < 2; j++)
              {
                const int e = f_e(numfa, j);

                if (e < 0 ) continue;

                for (int n = 0; n < N; n++)
                  aar_f(n) +=  vfd(numfa, j) / volumes_entrelaces(numfa) * (*a_r)(e, n) * (*alpha)(e, n);
              }
          }
        else aar_f = 1.;


        if (numfa < ndeb_faces_int)
          Ck = -0.5*K()/le_dom_VDF->dist_norm_bord_axi(numfa);
        else
          Ck = -0.5*K()/le_dom_VDF->dist_norm_axi(numfa);
        for (int n = 0; n < N; n++)
          {
            const double U = vit(numfa, n);
            secmem(numfa, n) += aar_f(n) * Ck*U*std::fabs(U)*volumes_entrelaces[numfa]*porosite_surf[numfa];
          }
      }
  else
    for (int i=0; i<nb_faces; i++)
      {
        const int numfa = num_faces[i];
        if (pbm)
          {
            aar_f = 0.;
            for (int j = 0; j < 2; j++)
              {
                const int e = f_e(numfa, j);

                if (e < 0 ) continue;

                for (int n = 0; n < N; n++)
                  aar_f(n) +=  vfd(numfa, j) / volumes_entrelaces(numfa) * (*a_r)(e, n) * (*alpha)(e, n);
              }
          }
        else aar_f = 1.;

        if (numfa < ndeb_faces_int)
          Ck = -0.5 * K() / le_dom_VDF->dist_norm_bord(numfa);
        else
          Ck = -0.5 * K() / le_dom_VDF->dist_norm(numfa);

        for (int n = 0; n < N; n++)
          {
            const double Ud = vit(numfa, n) * porosite_surf[numfa]; // vitesse debitante
            const double U = inco(numfa, n) * porosite_surf[numfa];

            secmem(numfa,n) += aar_f(n) * Ck * U * std::fabs(Ud) * volumes_entrelaces[numfa] * porosite_surf[numfa];
            if (mat)
              (*mat)(N * numfa + n, N * numfa + n) -= aar_f(n) * Ck * porosite_surf[numfa] * std::fabs(Ud) * volumes_entrelaces[numfa] * porosite_surf[numfa];
          }
      }
}

DoubleTab& Perte_Charge_Singuliere_VDF_Face::ajouter_(const DoubleTab& inco, DoubleTab& resu) const
{
  const std::string& nom_inco = equation().inconnue().le_nom().getString();
  ajouter_blocs({}, resu, {{nom_inco, inco}});
  return resu;
}

void Perte_Charge_Singuliere_VDF_Face::mettre_a_jour(double temps)
{
  Perte_Charge_VDF_Face::mettre_a_jour(temps);
  update_K(equation(), calculate_Q(equation(), num_faces, sgn), bilan());
}
