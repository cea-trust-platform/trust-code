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

#include <Raffiner_anisotrope.h>
#include <Scatter.h>

Implemente_instanciable(Raffiner_anisotrope,"Raffiner_anisotrope",Interprete_geometrique_base);
// XD raffiner_anisotrope interprete raffiner_anisotrope -1 Only for VEF discretizations, allows to cut triangle elements in 3, or tetrahedra in 4 parts, by defining a new summit located at the center of the element: \includepng{{raffineranisotri.pdf}}{{4}} \includepng{{raffineranisotetra.jpeg}}{{6}} NL2 Note that such a cut creates flat elements (anisotropic).
// XD attr domain_name ref_domaine domain_name 0 Name of domain.


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
  raffiner_(domaine());
  Scatter::init_sequential_domain(domaine());
  Cerr << "Refinement... OK" << finl;
  return is;
}

void Raffiner_anisotrope::raffiner_(Domaine& domaine)
{
  if  ((domaine.type_elem()->que_suis_je() == "Triangle")
       ||(domaine.type_elem()->que_suis_je() == "Tetraedre"))
    {
      Domaine& dom=domaine;
      IntTab& les_elems=domaine.les_elems();
      int oldsz=les_elems.dimension(0);
      DoubleTab& sommets=dom.les_sommets();
      int cpt=sommets.dimension(0);
      {
        DoubleTab sommets_ajoutes(oldsz, dimension);
        domaine.type_elem()->calculer_centres_gravite(sommets_ajoutes);
        sommets.resize(cpt+oldsz, dimension);
        for(int i=0; i<oldsz; i++)
          for(int j=0; j<dimension; j++)
            sommets(cpt+i,j)=sommets_ajoutes(i,j);
      }

      domaine.typer(domaine.type_elem()->que_suis_je());
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

              mettre_a_jour_sous_domaine(domaine,i,i+oldsz,1);

              new_elems(i+2*oldsz, 0) = i+cpt;
              new_elems(i+2*oldsz, 1) = les_elems(i,1);
              new_elems(i+2*oldsz, 2) = les_elems(i,2);

              mettre_a_jour_sous_domaine(domaine,i,i+2*oldsz,1);
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

              mettre_a_jour_sous_domaine(domaine,i,i+oldsz,1);

              new_elems(i+2*oldsz, 0) = i+cpt;
              new_elems(i+2*oldsz, 1) = les_elems(i,0);
              new_elems(i+2*oldsz, 2) = les_elems(i,2);
              new_elems(i+2*oldsz, 3) = les_elems(i,3);

              mettre_a_jour_sous_domaine(domaine,i,i+2*oldsz,1);

              new_elems(i+3*oldsz, 0) = i+cpt;
              new_elems(i+3*oldsz, 1) = les_elems(i,1);
              new_elems(i+3*oldsz, 2) = les_elems(i,2);
              new_elems(i+3*oldsz, 3) = les_elems(i,3);

              mettre_a_jour_sous_domaine(domaine,i,i+3*oldsz,1);
            }
        }

      les_elems.ref(new_elems);

      // Reconstruction de l'octree
      if (dimension == 2)
        Cerr << "We have split the triangles ..." << finl;
      else
        Cerr << "We have split the tetrahedra ..." << finl;
      domaine.invalide_octree();

      Cerr<<"END of Raffiner_anisotrope..."<<finl;
      Cerr<<"  1 NbElem="<<domaine.les_elems().dimension(0)<<"  NbNod="<<domaine.nb_som()<<finl;
    }
  else
    {
      Cerr << "We do not yet know how to Raffiner_anisotrope the "
           << domaine.type_elem()->que_suis_je() <<"s"<<finl;
      exit();
    }
}


