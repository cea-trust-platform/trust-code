/****************************************************************************
* Copyright (c) 2019, CEA
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
/////////////////////////////////////////////////////////////////////////////
//
// File      : Zone_EF_axi.cpp
// Directory : $TRUST_FIXES     # NAME OF THE PROJECT    (REQUIRED)_ROOT/src
//
/////////////////////////////////////////////////////////////////////////////

#include <Zone_EF_axi.h>
#include <DomaineAxi1d.h>

Implemente_instanciable( Zone_EF_axi, "Zone_EF_axi", Zone_EF ) ;

Sortie& Zone_EF_axi::printOn( Sortie& os ) const
{
  Zone_EF::printOn( os );
  return os;
}

Entree& Zone_EF_axi::readOn( Entree& is )
{
  Zone_EF::readOn( is );
  return is;
}

void Zone_EF_axi::calculer_Bij_gen(DoubleTab& bij)
{
  const IntTab& les_elems=zone().les_elems() ;
  int nbsom_face=nb_som_face();
  int nbelem=nb_elem_tot();
  int nbsom_elem=zone().nb_som_elem();
  const IntTab& elemfaces=elem_faces_;
  int nbface_elem=zone().nb_faces_elem();

  const DoubleTab& les_coords = domaine_axi().coord_sommets();

  const DoubleVect& surf = face_surfaces();

  for (int elem=0; elem<nbelem; elem++)
    {
      double x0 = origine_repere_(elem,0);
      double y0 = origine_repere_(elem,1);

      for (int i=0; i<nbsom_elem; i++)
        {
          // on cherche les faces contribuantes ,ce n'est pas optimal
          for (int f=0; f<nbface_elem; f++)
            {
              int face=elemfaces(elem,f);
              for (int s=0; s<nbsom_face; s++)
                {
                  // on cherche les faces contribuantes ,ce n'est pas optimal
                  if (face_sommets_(face,s)==les_elems(elem,i))
                    {
                      double x1 = les_coords(les_elems(elem,0),0);
                      double x2 = les_coords(les_elems(elem,1),0);
                      double y1 = les_coords(les_elems(elem,0),1);
                      double y2 = les_coords(les_elems(elem,1),1);

                      double r1 = sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
                      double r2 = sqrt((x2-x0)*(x2-x0)+(y2-y0)*(y2-y0));

                      for (int j=0; j<dimension; j++)
                        {
                          if (surf(face)>0)
                            bij(elem,i,j)+=face_normales(face,j)*oriente_normale(face,elem)*(r1+r2)*M_PI/surf(face);
                          else
                            bij(elem,i,j)+=face_normales(face,j)*oriente_normale(face,elem)*(r1+r2)*M_PI;
                        }
                    }
                }
            }
        }
    }
}

void Zone_EF_axi::calculer_IPhi()
{
  int nbelem=zone().nb_elem();
  int nb_som_elem=zone().nb_som_elem();
  assert(nb_som_elem==2);

  IPhi_.resize(nbelem,nb_som_elem);
  domaine_axi().creer_tableau_elements(IPhi_);
  //  Scatter::creer_tableau_distribue(zone().domaine(), Joint::ELEMENT, IPhi_);
  IPhi_thilde_=IPhi_;

  const IntTab& les_elems=zone().les_elems() ;

  const DoubleTab& les_coords = domaine_axi().coord_sommets();

  for (int elem=0; elem<nbelem; elem++)
    {
      double x1 = les_coords(les_elems(elem,0),0);
      double x2 = les_coords(les_elems(elem,1),0);
      double y1 = les_coords(les_elems(elem,0),1);
      double y2 = les_coords(les_elems(elem,1),1);

      double x0 = origine_repere_(elem,0);
      double y0 = origine_repere_(elem,1);

      double r1 = sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
      double r2 = sqrt((x2-x0)*(x2-x0)+(y2-y0)*(y2-y0));
      IPhi_(elem,0) = (M_PI/3.)*(r2*r2+r1*r2-2.*r1*r1);
      IPhi_(elem,1) = (M_PI/3.)*(2.*r2*r2-r1*r2-r1*r1);

      for (int s=0; s<nb_som_elem; s++)
        IPhi_thilde_(elem,s)=IPhi_(elem,s)*porosite_elem_(elem);
    }

  IPhi_.echange_espace_virtuel();
  IPhi_thilde_.echange_espace_virtuel();
}

void Zone_EF_axi::remplir_tableau_origine()
{
  DomaineAxi1d& domax = domaine_axi();
  const Champ& orig = domax.champ_origine();

  origine_repere_.resize(0,Objet_U::dimension-1);
  domax.creer_tableau_elements(origine_repere_);

  if (orig.non_nul())
    {
      const DoubleTab& positions = xp();

      IntVect les_polys(nb_elem());
      for(int e = 0; e < nb_elem(); e++)
        les_polys(e) = e;

      orig->valeur_aux_elems(positions, les_polys, origine_repere_);
    }
  else
    origine_repere_=0.;

  domax.associer_origine_repere(origine_repere_);
}

void Zone_EF_axi::verifie_compatibilite_domaine()
{
  if (!zone().domaine().Axi1d())
    {
      Cerr << "*****************************************************************************" << finl;
      Cerr << " Error in " << que_suis_je() << " : the type of domain " << zone().domaine().que_suis_je();
      Cerr << " is not compatible" << finl;
      Cerr << " with the discretisation EF_axi. " << finl;
      Cerr << " Please use the discretization EF or define a domain of type DomaineAxi1d." << finl;
      Cerr << "*****************************************************************************" << finl;
      Process::exit();
    }
}

void Zone_EF_axi::discretiser()
{

  verifie_compatibilite_domaine();
  typer_elem(zone());
  remplir_tableau_origine();
  Zone_EF::discretiser();
}

const DomaineAxi1d& Zone_EF_axi::domaine_axi() const
{
  return ref_cast(DomaineAxi1d,zone().domaine());
}

DomaineAxi1d& Zone_EF_axi::domaine_axi()
{
  return ref_cast(DomaineAxi1d,zone().domaine());
}

