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

#include <Terme_Source_Coriolis_VDF_Face.h>
#include <Domaine_VDF.h>
#include <Domaine_Cl_VDF.h>
#include <Neumann_sortie_libre.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Periodique.h>
#include <Pb_Fluide_base.h>
#include <Navier_Stokes_std.h>
#include <Fluide_Dilatable_base.h>

// Ajoute pour compatibilite avec Quasi-Compressible
Implemente_instanciable(Terme_Source_Coriolis_QC_VDF_Face,"Coriolis_QC_VDF_Face",Terme_Source_Coriolis_VDF_Face);
//// printOn
//
Sortie& Terme_Source_Coriolis_QC_VDF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}
//// readOn
//
Entree& Terme_Source_Coriolis_QC_VDF_Face::readOn(Entree& s )
{
  return Terme_Source_Coriolis_base::readOn(s);
}




Implemente_instanciable(Terme_Source_Coriolis_VDF_Face,"Coriolis_VDF_Face",Terme_Source_Coriolis_base);

//// printOn
//

Sortie& Terme_Source_Coriolis_VDF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Terme_Source_Coriolis_VDF_Face::readOn(Entree& s )
{
  return Terme_Source_Coriolis_base::readOn(s);
}

void Terme_Source_Coriolis_VDF_Face::associer_pb(const Probleme_base& pb)
{
  if (sub_type(Pb_Fluide_base,pb))
    {
      const Navier_Stokes_std& eqn_th = ref_cast(Navier_Stokes_std,pb.equation(0));
      Terme_Source_Coriolis_base::associer_eqn(eqn_th);
    }
  else
    {
      Cerr << "Error for the method Terme_Source_Coriolis_VDF_Face::associer_pb" << finl;
      Cerr << "The source term "<<que_suis_je()<<" cannot be activated for the problem "<<pb.que_suis_je()<< finl;
      exit();
    }

  la_source.resize(0,dimension);
  le_dom_VDF.valeur().domaine().creer_tableau_elements(la_source);
}


void Terme_Source_Coriolis_VDF_Face::associer_domaines(const Domaine_dis& domaine_dis,
                                                       const Domaine_Cl_dis& domaine_Cl_dis)
{
  le_dom_VDF = ref_cast(Domaine_VDF, domaine_dis.valeur());
  le_dom_Cl_VDF = ref_cast(Domaine_Cl_VDF, domaine_Cl_dis.valeur());

}


void Terme_Source_Coriolis_VDF_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_VDF& domaine_VDF = le_dom_VDF.valeur();
  const Domaine_Cl_VDF& domaine_Cl_VDF = le_dom_Cl_VDF.valeur();
  const IntTab& face_voisins = domaine_VDF.face_voisins();
  const IntVect& orientation = domaine_VDF.orientation();
  const DoubleVect& porosite_surf = equation().milieu().porosite_face();
  const DoubleVect& volumes_entrelaces = domaine_VDF.volumes_entrelaces();


  int ndeb,nfin,ncomp,num_face,num_elem;
  double vol;

  calculer_force_de_Coriolis();

  // Boucle sur les conditions limites pour traiter les faces de bord

  for (int n_bord=0; n_bord<domaine_VDF.nb_front_Cl(); n_bord++)
    {

      // pour chaque Condition Limite on regarde son type
      // Si face de Dirichlet ou de Symetrie on ne fait rien
      // Si face de Neumann on calcule la contribution au terme source

      const Cond_lim& la_cl = domaine_Cl_VDF.les_conditions_limites(n_bord);

      if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {

          //          const Neumann_sortie_libre& la_cl_neumann = ref_cast(Neumann_sortie_libre,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          for (num_face=ndeb; num_face<nfin; num_face++)
            {
              vol = volumes_entrelaces(num_face)*porosite_surf(num_face);
              ncomp = orientation(num_face);

              num_elem = face_voisins(num_face,0);
              if (num_elem == -1)
                num_elem = face_voisins(num_face,1);

              secmem(num_face)+= la_source(num_elem,ncomp)*vol;
            }
        }

      else if (sub_type(Symetrie,la_cl.valeur()))
        { /* Do nothing */}
      else if ( (sub_type(Dirichlet,la_cl.valeur()))
                ||
                (sub_type(Dirichlet_homogene,la_cl.valeur()))
              )
        { /* Do nothing */}
      else if (sub_type(Periodique,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          for (num_face=ndeb; num_face<nfin; num_face++)
            {
              vol = volumes_entrelaces(num_face)*porosite_surf(num_face);
              ncomp = orientation(num_face);

              secmem(num_face)+= 0.5*(la_source(face_voisins(num_face,0),ncomp)
                                      +la_source(face_voisins(num_face,1),ncomp))*vol;
            }
        }
    }

  // Boucle sur les faces internes

  ndeb = domaine_VDF.premiere_face_int();
  for (num_face =domaine_VDF.premiere_face_int(); num_face<domaine_VDF.nb_faces(); num_face++)
    {
      vol = volumes_entrelaces(num_face)*porosite_surf(num_face);
      ncomp = orientation(num_face);
      secmem(num_face)+= 0.5*(la_source(face_voisins(num_face,0),ncomp)+
                              la_source(face_voisins(num_face,1),ncomp))*vol;
    }
}

DoubleTab& Terme_Source_Coriolis_VDF_Face::calculer(DoubleTab& resu) const
{
  resu = 0;
  return ajouter(resu);
}

void Terme_Source_Coriolis_VDF_Face::calculer_force_de_Coriolis() const
{
  // On calcule la force de Coriolis par element (maillage entrelace!)
  // On se ramenera aux faces dans le ajouter
  const Domaine_VDF& domaine_VDF = le_dom_VDF.valeur();
  const DoubleTab& vitesse = eq_hydraulique().inconnue().valeurs();
  //  int nb_faces = domaine_VDF.nb_faces();
  const int nb_elems =  eq_hydraulique().domaine_dis().domaine().nb_elem();
  const IntTab& elem_faces = domaine_VDF.elem_faces();
  //  const IntTab& face_voisins = domaine_VDF.face_voisins();
  DoubleVect om = omega();

  int num_elem,i;
  DoubleVect vit(dimension);

  DoubleTab& la_source_ = ref_cast_non_const(DoubleTab, la_source);

  switch(dimension)
    {
    case 2:
      {
        for (num_elem=0; num_elem <nb_elems; num_elem++)
          {
            for (i=0; i<dimension; i++)
              vit[i] = 0.5*(vitesse(elem_faces(num_elem,i))+vitesse(elem_faces(num_elem,i+dimension)));

            la_source_(num_elem,0)= 2.*om(0)*vit[1];
            la_source_(num_elem,1)=-2.*om(0)*vit[0];
          }
        break;
      }
    case 3:
      {
        for (num_elem=0; num_elem <nb_elems; num_elem++)
          {
            for (i=0; i<dimension; i++)
              vit[i] = 0.5*(vitesse(elem_faces(num_elem,i))+vitesse(elem_faces(num_elem,i+dimension)));

            la_source_(num_elem,0)=-2.*(om(1)*vit[2]-om(2)*vit[1]);
            la_source_(num_elem,1)=-2.*(om(2)*vit[0]-om(0)*vit[2]);
            la_source_(num_elem,2)=-2.*(om(0)*vit[1]-om(1)*vit[0]);
          }
        break;
      }
    default:
      {
        Cerr << "Pour pouvoir utiliser la force de Coriolis, il faut etre en 2D ou 3D" << finl;
        exit();
      }
    }

  if (eq_hydraulique().milieu().que_suis_je()=="Fluide_Weakly_Compressible")
    {
      Cerr << "Terme_Source_Coriolis_VDF_Face is not yet tested for a Weakly Compressible fluid !" << finl;
      Cerr << "Contact the TRUST support if you want to use this source term." << finl;
      Process::exit();
    }

  // Si l'on est en Quasi Compressible, le terme source doit etre
  // multiplie par la masse volumique puisque c'est sous cette forme
  // qu'est ecrite l'equation de NS.
  const Probleme_base& pb = eq_hydraulique().probleme();
  if(pb.is_dilatable())
    {
      const Fluide_Dilatable_base& le_fluide = ref_cast(Fluide_Dilatable_base,eq_hydraulique().milieu());
      const DoubleTab& tab_rho_elem = le_fluide.masse_volumique().valeurs();
      double rhoelem;
      for (num_elem=0; num_elem <nb_elems; num_elem++)
        {
          rhoelem=tab_rho_elem[num_elem];
          for (i=0; i<dimension; i++)
            la_source_(num_elem,i)*=rhoelem;
        }
    }

  la_source_.echange_espace_virtuel();
}
