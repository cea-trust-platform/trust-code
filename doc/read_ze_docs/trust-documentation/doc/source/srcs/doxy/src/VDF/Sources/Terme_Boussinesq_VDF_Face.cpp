/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Terme_Boussinesq_VDF_Face.h>
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>
#include <Neumann_sortie_libre.h>
#include <Dirichlet.h>
#include <Domaine_VDF.h>
#include <Domaine_Cl_VDF.h>
#include <Convection_Diffusion_Concentration.h>
#include <Synonyme_info.h>

Implemente_instanciable(Terme_Boussinesq_VDF_Face,"Boussinesq_VDF_Face",Terme_Boussinesq_base);
Add_synonym(Terme_Boussinesq_VDF_Face,"Boussinesq_temperature_VDF_Face");
Add_synonym(Terme_Boussinesq_VDF_Face,"Boussinesq_concentration_VDF_Face");

//// printOn
Sortie& Terme_Boussinesq_VDF_Face::printOn(Sortie& s ) const
{
  return Terme_Boussinesq_base::printOn(s);
}

//// readOn
Entree& Terme_Boussinesq_VDF_Face::readOn(Entree& s )
{
  return Terme_Boussinesq_base::readOn(s);
}

void Terme_Boussinesq_VDF_Face::associer_domaines(const Domaine_dis& domaine_dis,
                                                  const Domaine_Cl_dis& domaine_Cl_dis)
{
  le_dom_VDF = ref_cast(Domaine_VDF, domaine_dis.valeur());
  le_dom_Cl_VDF = ref_cast(Domaine_Cl_VDF, domaine_Cl_dis.valeur());
}

void Terme_Boussinesq_VDF_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_VDF& domaine_VDF = le_dom_VDF.valeur();
  const Domaine_Cl_VDF& domaine_Cl_VDF_hyd = le_dom_Cl_VDF.valeur();
  const Domaine_Cl_dis& domaine_Cl_scal = equation_scalaire().domaine_Cl_dis();
  const Domaine_Cl_VDF& domaine_Cl_VDF_scal = ref_cast(Domaine_Cl_VDF,domaine_Cl_scal.valeur());
  const DoubleTab& param = equation_scalaire().inconnue().valeurs();
  const DoubleTab& beta_valeurs = beta().valeur().valeurs();
  const DoubleVect& grav = gravite().valeurs();
  const IntTab& face_voisins = domaine_VDF.face_voisins();
  const IntVect& orientation = domaine_VDF.orientation();
  const DoubleTab& xv = domaine_VDF.xv();
  const DoubleVect& porosite_surf = equation().milieu().porosite_face();
  const DoubleVect& volumes_entrelaces = domaine_VDF.volumes_entrelaces();
  const DoubleTab& vitesse = equation().inconnue().valeurs();

  DoubleVect g(dimension);
  g = grav;

  int nb_dim = param.line_size();

  // Verifie la validite de T0:
  check();

  // Boucle sur les conditions limites pour traiter les faces de bord
  for (int n_bord=0; n_bord<domaine_VDF.nb_front_Cl(); n_bord++)
    {
      // pour chaque Condition Limite on regarde son type
      const Cond_lim& la_cl = domaine_Cl_VDF_hyd.les_conditions_limites(n_bord);
      const Cond_lim& la_cl_scal = domaine_Cl_VDF_scal.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();

      if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          if (sub_type(Neumann_sortie_libre,la_cl_scal.valeur()))
            {
              const Neumann_sortie_libre& la_cl_neumann_scal = ref_cast(Neumann_sortie_libre, la_cl_scal.valeur());
              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  int outlet;
                  int elem = face_voisins(num_face,0);
                  if (elem==-1)
                    {
                      outlet = (vitesse(num_face)<0?1:0);
                      elem = face_voisins(num_face,1);
                    }
                  else
                    outlet = (vitesse(num_face)>0?1:0);
                  double coef=0;
                  for (int dim=0; dim<nb_dim; dim++)
                    {
                      double param_face = (outlet ? valeur(param,elem,dim) : 0.5*(valeur(param,elem,dim)+la_cl_neumann_scal.val_ext(num_face-ndeb,dim)));
                      coef += valeur(beta_valeurs,elem,elem,dim)*(Scalaire0(dim)-param_face);
                    }

                  if (axi)
                    {
                      double cos_teta = cos(xv(num_face,1));
                      double sin_teta = sin(xv(num_face,1));
                      g(0) = grav(0)*cos_teta + grav(1)*sin_teta;
                      g(1) = grav(1)*cos_teta - grav(0)*sin_teta;
                    }
                  double vol = volumes_entrelaces(num_face)*porosite_surf(num_face);
                  int ncomp = orientation(num_face);
                  secmem(num_face)+= coef*g(ncomp)*vol;
                }
            }
          else if (sub_type(Dirichlet,la_cl_scal.valeur()))
            {
              const Dirichlet& la_cl_diri_scal = ref_cast(Dirichlet,la_cl_scal.valeur());
              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  int outlet;
                  int elem = face_voisins(num_face,0);
                  if (elem==-1)
                    {
                      outlet = (vitesse(num_face)<0?1:0);
                      elem = face_voisins(num_face,1);
                    }
                  else
                    outlet = (vitesse(num_face)>0?1:0);

                  double coef=0;
                  for (int dim=0; dim<nb_dim; dim++)
                    {
                      double param_face = (outlet ? valeur(param,elem,dim) : 0.5*(valeur(param,elem,dim)+la_cl_diri_scal.val_imp(num_face-ndeb,dim)));
                      coef += valeur(beta_valeurs,elem,elem,dim)*(Scalaire0(dim)-param_face);
                    }
                  if (axi)
                    {
                      double cos_teta = cos(xv(num_face,1));
                      double sin_teta = sin(xv(num_face,1));
                      g(0) = grav(0)*cos_teta + grav(1)*sin_teta;
                      g(1) = grav(1)*cos_teta - grav(0)*sin_teta;
                    }
                  double vol = volumes_entrelaces(num_face)*porosite_surf(num_face);
                  int ncomp = orientation(num_face);
                  secmem(num_face)+= coef*g(ncomp)*vol;
                }
            }
        }
      else
        {
          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              int elem = face_voisins(num_face,0);
              if (elem == -1) elem = face_voisins(num_face,1);
              double coef = 0;
              for (int dim=0; dim<nb_dim; dim++)
                {
                  double param_face = valeur(param,elem,dim);
                  coef += valeur(beta_valeurs,elem,elem,dim)*(Scalaire0(dim)-param_face);
                }
              if (axi)
                {
                  double cos_teta = cos(xv(num_face,1));
                  double sin_teta = sin(xv(num_face,1));
                  g(0) = grav(0)*cos_teta + grav(1)*sin_teta;
                  g(1) = grav(1)*cos_teta - grav(0)*sin_teta;
                }
              int ncomp = orientation(num_face);
              double vol = volumes_entrelaces(num_face)*porosite_surf(num_face);
              secmem(num_face) += coef*g(ncomp)*vol;
            }
        }
    }

  // Boucle sur les faces internes
  int ndeb = domaine_VDF.premiere_face_int();
  int nb_faces = domaine_VDF.nb_faces();
  for (int num_face=ndeb; num_face<nb_faces; num_face++)
    {
      int elem1 = face_voisins(num_face,0);
      int elem2 = face_voisins(num_face,1);
      double coef = 0;
      for (int dim=0; dim<nb_dim; dim++)
        {
          double param_face = 0.5*(valeur(param,elem1,dim)+valeur(param,elem2,dim));
          coef += valeur(beta_valeurs,elem1,elem2,dim)*(Scalaire0(dim)-param_face);
        }
      if (axi)
        {
          double cos_teta = cos(xv(num_face,1));
          double sin_teta = sin(xv(num_face,1));
          g(0) = grav(0)*cos_teta + grav(1)*sin_teta;
          g(1) = grav(1)*cos_teta - grav(0)*sin_teta;
        }
      int ncomp = orientation(num_face);
      double vol = volumes_entrelaces(num_face)*porosite_surf(num_face);
      secmem(num_face) += coef*g(ncomp)*vol;
    }

}
