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

#include <Champ_Generique_Tparoi_VEF.h>
#include <Probleme_base.h>
#include <Nom.h>
#include <TRUSTTabs.h>
#include <Zone_VEF.h>
#include <Zone_Cl_VEF.h>
#include <Equation_base.h>
#include <Modele_turbulence_scal_base.h>

#include <Neumann_paroi.h>
#include <Neumann_homogene.h>
#include <Discretisation_base.h>
#include <Synonyme_info.h>

Implemente_instanciable(Champ_Generique_Tparoi_VEF,"Tparoi_VEF",Champ_Gen_de_Champs_Gen);
Add_synonym(Champ_Generique_Tparoi_VEF,"Champ_Post_Tparoi_VEF");

Sortie& Champ_Generique_Tparoi_VEF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

//cf Champ_Gen_de_Champs_Gen::readOn
Entree& Champ_Generique_Tparoi_VEF::readOn(Entree& s )
{
  Champ_Gen_de_Champs_Gen::readOn(s);
  return s ;
}

const Champ_base& Champ_Generique_Tparoi_VEF::get_champ_without_evaluation(Champ& espace_stockage) const
{

  Champ source_espace_stockage;
  const Champ_base& source = get_source(0).get_champ_without_evaluation(source_espace_stockage);

  const Zone_dis_base& zone_dis = get_ref_zone_dis_base();
  Nature_du_champ nature_source = source.nature_du_champ();
  Noms noms;
  Noms unites;
  noms.add("bidon");
  unites.add("bidon");
  int nb_comp = 1;
  double temps;
  temps=0.;

  Champ_Fonc espace_stockage_fonc;
  const Discretisation_base&  discr = get_discretisation();
  Motcle directive = get_directive_pour_discr();
  discr.discretiser_champ(directive,zone_dis,nature_source,noms,unites,nb_comp,temps,espace_stockage_fonc);
  espace_stockage = espace_stockage_fonc;


  return espace_stockage.valeur();
}
const Champ_base& Champ_Generique_Tparoi_VEF::get_champ(Champ& espace_stockage) const
{

  Champ source_espace_stockage;
  const Champ_base& source = get_source(0).get_champ(source_espace_stockage);

  const Zone_dis_base& zone_dis = get_ref_zone_dis_base();
  Nature_du_champ nature_source = source.nature_du_champ();
  Noms noms;
  Noms unites;
  noms.add("bidon");
  unites.add("bidon");
  int nb_comp = 1;
  double temps;
  temps=0.;

  Champ_Fonc espace_stockage_fonc;
  const Discretisation_base&  discr = get_discretisation();
  Motcle directive = get_directive_pour_discr();
  discr.discretiser_champ(directive,zone_dis,nature_source,noms,unites,nb_comp,temps,espace_stockage_fonc);
  espace_stockage = espace_stockage_fonc;

  DoubleTab& valeurs_espace = espace_stockage->valeurs();
  const DoubleTab& source_so_val = source.valeurs();
  const DoubleTab& inconnue = source_so_val;
  valeurs_espace = source_so_val;

  // Sur toutes les faces ou le flux est impose si le flux turbulent pas calcule on recalcule la temperature
  // on recuprere l'eqn de temp
  const Equation_base& my_eqn=ref_cast(Champ_Inc_base,source).equation();
  const RefObjU& modele_turbulence = my_eqn.get_modele(TURBULENCE);
  if ( modele_turbulence.non_nul() && sub_type(Modele_turbulence_scal_base,modele_turbulence.valeur()))
    {
      const Modele_turbulence_scal_base& mod_turb_scal = ref_cast(Modele_turbulence_scal_base,modele_turbulence.valeur());
      const Turbulence_paroi_scal& loiparth = mod_turb_scal.loi_paroi();

      if( loiparth.valeur().use_equivalent_distance() )
        {
          // const Paroi_scal_hyd_base_VEF& paroi_scal_vef = ref_cast(Paroi_scal_hyd_base_VEF,loiparth.valeur());

          const Zone_VEF& zone_VEF=ref_cast(Zone_VEF,my_eqn.zone_dis().valeur());
          const IntTab& elem_faces = zone_VEF.elem_faces();
          const DoubleVect& vol = zone_VEF.volumes();
          const IntTab& face_voisins = zone_VEF.face_voisins();
          const DoubleTab& face_normale = zone_VEF.face_normales();
          const Zone_Cl_VEF& zone_Cl_VEF=ref_cast(Zone_Cl_VEF,my_eqn.zone_Cl_dis().valeur());
          int nb_dim_pb=Objet_U::dimension;
          DoubleVect le_mauvais_gradient(nb_dim_pb);
          int nb_front=zone_VEF.nb_front_Cl();
          for (int n_bord=0; n_bord<nb_front; n_bord++)
            {
              const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
              const Cond_lim_base& cl_base=la_cl.valeur();
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

              // Les lois de parois ne s'appliquent qu'aux cas ou la CL
              // est de type temperature imposee, car dans les autres cas
              // (flux impose et adiabatique) le flux a la paroi est connu et fixe.
              int ldp_appli=0;
              if ((sub_type(Neumann_paroi,cl_base))||(sub_type(Neumann_homogene,cl_base)))
                {
                  ldp_appli=1;
                  // if (paroi_scal_vef.get_flag_calcul_ldp_en_flux_impose() )
                  if (loiparth.valeur().get_flag_calcul_ldp_en_flux_impose() )
                    ldp_appli=0;
                }

              if (ldp_appli)
                {
                  // const DoubleVect& d_equiv = paroi_scal_vef.equivalent_distance(n_bord);
                  const DoubleVect& d_equiv = loiparth.valeur().equivalent_distance(n_bord);
                  // d_equiv contient la distance equivalente pour le bord
                  // Dans d_equiv, pour les faces qui ne sont pas paroi_fixe (eg periodique, symetrie, etc...)
                  // il y a la distance geometrique grace a l'initialisation du tableau dans la loi de paroi.

                  int num1 = 0;
                  int num2 = le_bord.nb_faces_tot();
                  for (int ind_face=num1; ind_face<num2; ind_face++)
                    {
                      // Tf est la temperature fluide moyenne dans le premier element
                      // sans tenir compte de la temperature de paroi.
                      double Tf=0.;
                      // double bon_gradient=0.; // c'est la norme du gradient de temperature normal a la paroi calculee a l'aide de la loi de paroi.
                      le_mauvais_gradient=0.;
                      int num_face = le_bord.num_face(ind_face);
                      int elem1 = face_voisins(num_face,0);
                      if (elem1==-1) elem1 = face_voisins(num_face,1);
                      double surface_face = zone_VEF.face_surfaces(num_face);

                      int nb_faces_elem = zone_VEF.zone().nb_faces_elem();
                      for (int i=0; i<nb_faces_elem; i++)
                        {
                          int j = elem_faces(elem1,i);
                          if ( j != num_face )
                            {
                              double surface_pond = 0.;
                              for (int kk=0; kk<nb_dim_pb; kk++)
                                surface_pond -= (face_normale(j,kk)*zone_VEF.oriente_normale(j,elem1)*face_normale(num_face,kk)*
                                                 zone_VEF.oriente_normale(num_face,elem1))/(surface_face*surface_face);
                              Tf+=inconnue(j)*surface_pond;
                            }

                          for(int kk=0; kk<nb_dim_pb; kk++)
                            le_mauvais_gradient(kk)+=inconnue(j)*face_normale(j,kk)*zone_VEF.oriente_normale(j,elem1);
                        }
                      le_mauvais_gradient/=vol(elem1);
                      // mauvais_gradient = le_mauvais_gradient.n
                      double mauvais_gradient=0;
                      for(int kk=0; kk<nb_dim_pb; kk++)
                        mauvais_gradient+=le_mauvais_gradient(kk)*face_normale(num_face,kk)/surface_face;

                      // valeurs_espace(num_face) est la temperature de paroi : Tw.
                      // On se fiche du signe de bon gradient car c'est la norme du gradient
                      // de temperature dans l'element.
                      // Ensuite ce sera multiplie par le vecteur normal a la face de paroi
                      // qui lui a les bons signes.
                      //bon_gradient=(Tf-inconnue(num_face))/d_equiv(ind_face)*(-zone_VEF.oriente_normale(num_face,elem1));
                      // on doit faire bon=mauvais...
                      // on fait le calcul de facon a avoir bon_grad =mauva
                      valeurs_espace(num_face)=Tf-mauvais_gradient*d_equiv(ind_face)*(-zone_VEF.oriente_normale(num_face,elem1));
                    }
                }
            }
        }
    }
  DoubleTab& espace_valeurs = espace_stockage->valeurs();
  espace_valeurs.echange_espace_virtuel();

  return espace_stockage.valeur();
}

const Noms Champ_Generique_Tparoi_VEF::get_property(const Motcle& query) const
{

  Motcles motcles(1);
  motcles[0] = "composantes";
  int rang = motcles.search(query);
  switch(rang)
    {

    case 0:
      {
        Noms source_compos = get_source(0).get_property("composantes");
        int nb_comp = source_compos.size();
        Noms compo(nb_comp);

        for (int i=0; i<nb_comp; i++)
          {
            Nom nume(i);
            compo[i] = nom_post_+nume;
          }

        return compo;
        break;
      }

    }
  return Champ_Gen_de_Champs_Gen::get_property(query);
}

//Nomme le champ en tant que source par defaut
//"Combinaison_"+nom_champ_source
void Champ_Generique_Tparoi_VEF::nommer_source()
{
  if (nom_post_=="??")
    {
      Nom nom_post_source, nom_champ_source;
      const Noms nom = get_source(0).get_property("nom");
      nom_champ_source = nom[0];
      nom_post_source =  "Tparoi_";
      nom_post_source +=  nom_champ_source;
      nommer(nom_post_source);
    }
}
