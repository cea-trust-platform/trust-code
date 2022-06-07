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

#include <Raffiner_anisotrope.h>
#include <Scatter.h>

Implemente_instanciable(Raffiner_anisotrope,"Raffiner_anisotrope",Interprete_geometrique_base);

Sortie& Raffiner_anisotrope::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

Entree& Raffiner_anisotrope::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

Entree& Raffiner_anisotrope::interpreter_(Entree& is)
{
  if (dimension == 2)
    {
      Cerr << "Refinement of each element into 3 new triangles... " <<finl;
    }
  else if (dimension == 3)
    {
      Cerr << "Refinement of each element into 4 new tetrahedra... " <<finl;
    }
  else
    {
      Cerr << "we can not refine in dimension " << dimension <<finl;
      exit();
    }
  associer_domaine(is);
  Scatter::uninit_sequential_domain(domaine());
  for(int i=0; i<domaine().nb_zones() ; i++)

    {
      Raffiner_anisotrope_(domaine().zone(i));
    }
  Scatter::init_sequential_domain(domaine());
  Cerr << "Refinement... OK" << finl;
  return is;
}

void Raffiner_anisotrope::Raffiner_anisotrope_(Zone& zone)
{
  if  ((zone.type_elem()->que_suis_je() == "Triangle")
       ||(zone.type_elem()->que_suis_je() == "Tetraedre"))
    {
      Domaine& dom=zone.domaine();
      IntTab& les_elems=zone.les_elems();
      int oldsz=les_elems.dimension(0);
      DoubleTab& sommets=dom.les_sommets();
      int cpt=sommets.dimension(0);
      {
        DoubleTab sommets_ajoutes(oldsz, dimension);
        zone.type_elem()->calculer_centres_gravite(sommets_ajoutes);
        sommets.resize(cpt+oldsz, dimension);
        for(int i=0; i<oldsz; i++)
          for(int j=0; j<dimension; j++)
            sommets(cpt+i,j)=sommets_ajoutes(i,j);
      }

      zone.typer(zone.type_elem()->que_suis_je());
      IntTab new_elems((dimension+1)*oldsz, dimension+1);

      if(dimension==2)
        {
          for(int i=0; i< oldsz; i++)
            {
              new_elems(i  , 0) = i+cpt;
              new_elems(i  , 1) = les_elems(i,0);
              new_elems(i  , 2) = les_elems(i,1);

              new_elems(i+oldsz, 0) = i+cpt;
              new_elems(i+oldsz, 1) = les_elems(i,0);
              new_elems(i+oldsz, 2) = les_elems(i,2);

              mettre_a_jour_sous_zone(zone,i,i+oldsz,1);

              new_elems(i+2*oldsz, 0) = i+cpt;
              new_elems(i+2*oldsz, 1) = les_elems(i,1);
              new_elems(i+2*oldsz, 2) = les_elems(i,2);

              mettre_a_jour_sous_zone(zone,i,i+2*oldsz,1);
            }
        }
      else
        {
          for(int i=0; i< oldsz; i++)
            {
              new_elems(i  , 0) = i+cpt;
              new_elems(i  , 1) = les_elems(i,0);
              new_elems(i  , 2) = les_elems(i,1);
              new_elems(i  , 3) = les_elems(i,2);

              new_elems(i+oldsz, 0) = i+cpt;
              new_elems(i+oldsz, 1) = les_elems(i,0);
              new_elems(i+oldsz, 2) = les_elems(i,1);
              new_elems(i+oldsz, 3) = les_elems(i,3);

              mettre_a_jour_sous_zone(zone,i,i+oldsz,1);

              new_elems(i+2*oldsz, 0) = i+cpt;
              new_elems(i+2*oldsz, 1) = les_elems(i,0);
              new_elems(i+2*oldsz, 2) = les_elems(i,2);
              new_elems(i+2*oldsz, 3) = les_elems(i,3);

              mettre_a_jour_sous_zone(zone,i,i+2*oldsz,1);

              new_elems(i+3*oldsz, 0) = i+cpt;
              new_elems(i+3*oldsz, 1) = les_elems(i,1);
              new_elems(i+3*oldsz, 2) = les_elems(i,2);
              new_elems(i+3*oldsz, 3) = les_elems(i,3);

              mettre_a_jour_sous_zone(zone,i,i+3*oldsz,1);
            }
        }

      les_elems.ref(new_elems);

      // Reconstruction de l'octree
      if (dimension == 2)
        Cerr << "We have split the triangles ..." << finl;
      else
        Cerr << "We have split the tetrahedra ..." << finl;
      zone.invalide_octree();

      Cerr<<"END of Raffiner_anisotrope..."<<finl;
      Cerr<<"  1 NbElem="<<zone.les_elems().dimension(0)<<"  NbNod="<<zone.nb_som()<<finl;
    }
  else
    {
      Cerr << "We do not yet know how to Raffiner_anisotrope the "
           << zone.type_elem()->que_suis_je() <<"s"<<finl;
      exit();
    }
}


