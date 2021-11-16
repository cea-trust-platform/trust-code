/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Champ_front_fonc_pois_ipsn.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_front_fonc_pois_ipsn.h>
#include <Domaine.h>
#include <Frontiere_dis_base.h>

Implemente_instanciable(Champ_front_fonc_pois_ipsn,"Champ_front_fonc_pois_ipsn",Champ_front_fonc);

//// printOn
//

Sortie& Champ_front_fonc_pois_ipsn::printOn(Sortie& s ) const
{

  s << que_suis_je() << " " << le_nom() << finl ;

  const DoubleTab& tab=valeurs();
  s << tab.size() << " ";
  for(int i=0; i<tab.size(); i++)
    s << tab(0,i);

  return  s ;
}

//// readOn
//

Entree& Champ_front_fonc_pois_ipsn::readOn(Entree& is )
{

  is >> R_tube ;

  int nbcomp , i;
  nbcomp=lire_dimension(is,que_suis_je());

  fixer_nb_comp(nbcomp);
  U_moy.resize(nbcomp) ;
  R_loc.resize(nbcomp) ;

  for (i = 0; i < nbcomp; i++)
    {
      is >> U_moy(i) ;
    }

  for (i = 0; i < nbcomp; i++)
    {
      is >> R_loc(i) ;
    }


  //Cerr << "Champ_front_fonc_pois_ipsn lu U_moy = "<< U_moy << finl ;
  return is ;
}

int Champ_front_fonc_pois_ipsn::initialiser(double temps, const Champ_Inc_base& inco)
{
  if (!Champ_front_fonc::initialiser(temps,inco))
    return 0;

  int dim = nb_comp();

  const Frontiere& front=la_frontiere_dis->frontiere();
  const Zone& zone=front.zone();
  const Domaine& domaine=zone.domaine();
  const Faces& faces=front.faces();

  int nb_faces=front.nb_faces();
  int nbsf=faces.nb_som_faces();

  DoubleTab& tab=valeurs();

  DoubleVect loc_local(dim) ;

  double teta = 40./360.*2.*3.14159  ;
  double dummy ;

  DoubleTab coord_trans(3,3) ;

  coord_trans(0,0) = cos(teta) ;
  coord_trans(0,1) = 0.        ;
  coord_trans(0,2) = sin(teta) ;

  coord_trans(1,0) = 0. ;
  coord_trans(1,1) = 1. ;
  coord_trans(1,2) = 0. ;

  coord_trans(2,0) = -sin(teta) ;
  coord_trans(2,1) = 1.        ;
  coord_trans(2,2) = cos(teta) ;

  DoubleVect trans(3) ;

  int k, i ,j ;

  for( int fac=0; fac<nb_faces; fac++)
    {
      loc_local = 0.;

      for( k=0; k<nbsf; k++)
        {
          for ( i=0; i<dim; i++)
            loc_local(i)+=domaine.coord(faces.sommet(fac,k),i);
        }
      loc_local /= double(nbsf);

      trans = 0. ;
      for (i=0; i<dimension; i++)
        for (j=0; j<dimension; j++)
          trans(i) += (loc_local(j) - R_loc(j)) * coord_trans(i,j) ;

      loc_local = trans ;

      double ray2 = R_tube*R_tube ;

      dummy = loc_local(0)*loc_local(1)*ray2 ;
      tab(fac,2) = fabs(dummy) ;
      dummy = loc_local(0)*loc_local(2)*ray2 ;
      tab(fac,1) = fabs(dummy) ;
      dummy = loc_local(1)*loc_local(2)*ray2 ;
      tab(fac,0) = fabs(dummy) ;

      dummy = loc_local(0)*loc_local(0)*loc_local(0)*loc_local(1) ;
      tab(fac,2) -= fabs(dummy) ;
      dummy = loc_local(1)*loc_local(1)*loc_local(1)*loc_local(0) ;
      tab(fac,2) -= fabs(dummy) ;

      dummy = loc_local(0)*loc_local(0)*loc_local(0)*loc_local(2) ;
      tab(fac,1) -= fabs(dummy) ;
      dummy = loc_local(2)*loc_local(2)*loc_local(2)*loc_local(0) ;
      tab(fac,1) -= fabs(dummy) ;

      dummy = loc_local(2)*loc_local(2)*loc_local(2)*loc_local(1) ;
      tab(fac,0) -= fabs(dummy) ;
      dummy = loc_local(1)*loc_local(1)*loc_local(1)*loc_local(2) ;
      tab(fac,0) -= fabs(dummy) ;

      for ( i=0; i<dim; i++)
        tab(fac,i) *= U_moy(i) ;
    }
  tab.echange_espace_virtuel();
  return 1;
}

