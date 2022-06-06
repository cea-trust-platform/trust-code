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

#include <Champ_front_fonc_pois_tube.h>
#include <Domaine.h>
#include <Frontiere_dis_base.h>

Implemente_instanciable(Champ_front_fonc_pois_tube,"Champ_front_fonc_pois_tube",Champ_front_fonc);

//// printOn
//

Sortie& Champ_front_fonc_pois_tube::printOn(Sortie& s ) const
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

Entree& Champ_front_fonc_pois_tube::readOn(Entree& is )
{

  is >> R_tube ;

  int nbcomp , i;
  nbcomp=lire_dimension(is,que_suis_je());

  fixer_nb_comp(nbcomp);
  U_moy.resize(nbcomp) ;
  R_loc.resize(nbcomp) ;
  R_loc_mult.resize(nbcomp) ;
  for (i = 0; i < nbcomp; i++)
    {
      is >> U_moy(i) ;
    }

  for (i = 0; i < nbcomp; i++)
    {
      is >> R_loc(i) ;
    }

  for (i = 0; i < nbcomp; i++)
    {
      is >> R_loc_mult(i) ;
    }

  //Cerr << "Champ_front_fonc_pois_tube lu U_moy = "<< U_moy << finl ;
  return is ;
}

int Champ_front_fonc_pois_tube::initialiser(double temps, const Champ_Inc_base& inco)
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

  ArrOfDouble loc_local(dim) ;

  int k, i ,j ;

  for( int fac=0; fac<nb_faces; fac++)
    {
      loc_local = 0.;

      for( k=0; k<nbsf; k++)
        {
          for ( i=0; i<dim; i++)
            loc_local[i]+=domaine.coord(faces.sommet(fac,k),i);
        }
      loc_local /= double(nbsf);

      loc_local -= R_loc ;

      double ray = 0. ;
      for ( i=0; i<dim; i++) ray += (loc_local[i]*loc_local[i]) ;

      ray = sqrt(ray) ;

      for ( i=0; i<dim; i++)
        tab(fac,i)= 2.*U_moy(i) ;

      for ( i=0; i<dim; i++)
        tab(fac,i) *=  ( 1. - ( (ray/R_tube) * (ray/R_tube) ) ) ;

      for ( i=0; i<dim; i++)
        for ( j=0; j<dim; j++)
          {
            double dummy =  ( loc_local[j] * double(R_loc_mult(j)) / R_tube );
            tab(fac,i) *=  ( 1. + 1.25 * dummy ) ;
          }
    }
  tab.echange_espace_virtuel();
  return 1;
}


