/****************************************************************************
* Copyright (c) 2015, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Terme_Source_Coriolis_VDF_Face.cpp
// Directory:   $TRUST_ROOT/src/VDF/Sources
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Source_Coriolis_VDF_Face.h>
#include <math.h>
#include <Zone_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Les_Cl.h>
#include <Pb_qdm_fluide.h>
#include <Navier_Stokes_std.h>
#include <Fluide_Quasi_Compressible.h>

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
  return Terme_Source_Coriolis::lire_donnees(s);
}




Implemente_instanciable(Terme_Source_Coriolis_VDF_Face,"Coriolis_VDF_Face",Source_base);

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
  return Terme_Source_Coriolis::lire_donnees(s);
}

void Terme_Source_Coriolis_VDF_Face::associer_pb(const Probleme_base& pb)
{
  if (sub_type(Pb_qdm_fluide,pb))
    {
      const Navier_Stokes_std& eqn_th = ref_cast(Navier_Stokes_std,pb.equation(0));
      Terme_Source_Coriolis::associer_eqn(eqn_th);
    }
  else
    {
      Cerr << "Error for the method Terme_Source_Coriolis_VDF_Face::associer_pb" << finl;
      Cerr << "The source term "<<que_suis_je()<<" cannot be activated for the problem "<<pb.que_suis_je()<< finl;
      exit();
    }

  la_source.resize(0,dimension);
  la_zone_VDF.valeur().zone().creer_tableau_elements(la_source);
}


void Terme_Source_Coriolis_VDF_Face::associer_zones(const Zone_dis& zone_dis,
                                                    const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VDF = ref_cast(Zone_VDF, zone_dis.valeur());
  la_zone_Cl_VDF = ref_cast(Zone_Cl_VDF, zone_Cl_dis.valeur());

}


DoubleTab& Terme_Source_Coriolis_VDF_Face::ajouter(DoubleTab& resu) const
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const Zone_Cl_VDF& zone_Cl_VDF = la_zone_Cl_VDF.valeur();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntVect& orientation = zone_VDF.orientation();
  const DoubleVect& porosite_surf = zone_VDF.porosite_face();
  const DoubleVect& volumes_entrelaces = zone_VDF.volumes_entrelaces();


  int ndeb,nfin,ncomp,num_face,num_elem;
  double vol;

  calculer_force_de_Coriolis();

  // Boucle sur les conditions limites pour traiter les faces de bord

  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {

      // pour chaque Condition Limite on regarde son type
      // Si face de Dirichlet ou de Symetrie on ne fait rien
      // Si face de Neumann on calcule la contribution au terme source

      const Cond_lim& la_cl = zone_Cl_VDF.les_conditions_limites(n_bord);

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

              resu(num_face)+= la_source(num_elem,ncomp)*vol;
            }
        }

      else if (sub_type(Symetrie,la_cl.valeur()))
        ;
      else if ( (sub_type(Dirichlet,la_cl.valeur()))
                ||
                (sub_type(Dirichlet_homogene,la_cl.valeur()))
              )
        ;
      else if (sub_type(Periodique,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          for (num_face=ndeb; num_face<nfin; num_face++)
            {
              vol = volumes_entrelaces(num_face)*porosite_surf(num_face);
              ncomp = orientation(num_face);

              resu(num_face)+= 0.5*(la_source(face_voisins(num_face,0),ncomp)
                                    +la_source(face_voisins(num_face,1),ncomp))*vol;
            }
        }
    }

  // Boucle sur les faces internes

  ndeb = zone_VDF.premiere_face_int();
  for (num_face =zone_VDF.premiere_face_int(); num_face<zone_VDF.nb_faces(); num_face++)
    {
      vol = volumes_entrelaces(num_face)*porosite_surf(num_face);
      ncomp = orientation(num_face);
      resu(num_face)+= 0.5*(la_source(face_voisins(num_face,0),ncomp)+
                            la_source(face_voisins(num_face,1),ncomp))*vol;
    }
  return resu;
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
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const DoubleTab& vitesse = eq_hydraulique().inconnue().valeurs();
  //  int nb_faces = zone_VDF.nb_faces();
  const int nb_elems =  eq_hydraulique().zone_dis().zone().nb_elem();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  //  const IntTab& face_voisins = zone_VDF.face_voisins();
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



  // Si l'on est en Quasi Compressible, le terme source doit etre
  // multiplie par la masse volumique puisque c'est sous cette forme
  // qu'est ecrite l'equation de NS.
  const Probleme_base& pb = eq_hydraulique().probleme();
  if(pb.is_QC())
    {
      const Fluide_Quasi_Compressible& le_fluide = ref_cast(Fluide_Quasi_Compressible,eq_hydraulique().milieu());
      const DoubleTab& tab_rho_elem = le_fluide.masse_volumique();
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
