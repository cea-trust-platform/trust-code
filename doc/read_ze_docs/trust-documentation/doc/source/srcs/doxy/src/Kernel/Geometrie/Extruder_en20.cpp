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

#include <Connectivite_som_elem.h>
#include <Static_Int_Lists.h>
#include <Extruder_en20.h>
#include <Faces_builder.h>
#include <Domaine.h>
#include <Scatter.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Extruder_en20, "Extruder_en20", Interprete_geometrique_base);

Extruder_en20::Extruder_en20() { direction.resize(3, Array_base::NOCOPY_NOINIT); }

Sortie& Extruder_en20::printOn(Sortie& os) const { return Interprete::printOn(os); }

Entree& Extruder_en20::readOn(Entree& is) { return Interprete::readOn(is); }

/*! @brief Fonction principale de l'interprete Extruder_en20 Triangule tout le domaine
 *
 *     specifie par la directive.
 *     On triangule le domaine grace a la methode:
 *       void Extruder_en20::extruder(Domaine& domaine) const
 *     Extruder signifie ici transformer en triangle des
 *     elements geometrique d'un domaine.
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree
 * @throws l'objet a mailler n'est pas du type Domaine
 */
Entree& Extruder_en20::interpreter_(Entree& is)
{
  Nom nom_dom;
  Param param(que_suis_je());
  param.ajouter("domaine",&nom_dom,Param::REQUIRED);
  param.ajouter("nb_tranches",&NZ,Param::REQUIRED);
  param.ajouter_arr_size_predefinie("direction",&direction,Param::REQUIRED);
  param.lire_avec_accolades_depuis(is);
  associer_domaine(nom_dom);
  Scatter::uninit_sequential_domain(domaine());
  extruder(domaine());
  Scatter::init_sequential_domain(domaine());
  return is;
}

/*! @brief Triangule tous les element d'un domaine: transforme les elements goemetriques du domaine en triangles.
 *
 *     Pour l'instant on ne sait raffiner que des Rectangles
 *     (on les coupe en 4).
 *
 * @param (Domaine& domaine) le domaine dont on veut raffiner les elements
 */
void Extruder_en20::extruder(Domaine& dom)
{
  if (dom.type_elem()->que_suis_je() == "Rectangle" || dom.type_elem()->que_suis_je() ==  "Quadrangle" )
    {
      Cerr << " The extrusion of quadrangle is made by Extruder and not by Extruder_en20 " << finl;
      exit();
    }
  else if( dom.type_elem()->que_suis_je() == "Triangle")
    {
      int oldnbsom = dom.nb_som();
      IntTab& les_elems=dom.les_elems();
      int oldsz=les_elems.dimension(0);
      double dx = direction[0]/NZ;
      double dy = direction[1]/NZ;
      double dz = direction[2]/NZ;

      Faces les_faces;
      //domaine.creer_faces(les_faces);
      {
        // bloc a factoriser avec Domaine_VF.cpp :
        Type_Face type_face = dom.type_elem().type_face(0);
        les_faces.typer(type_face);
        les_faces.associer_domaine(dom);

        Static_Int_Lists connectivite_som_elem;
        const int     nb_sommets_tot = dom.nb_som_tot();
        const IntTab&    elements       = dom.les_elems();

        construire_connectivite_som_elem(nb_sommets_tot,
                                         elements,
                                         connectivite_som_elem,
                                         1 /* include virtual elements */);

        Faces_builder faces_builder;
        IntTab elem_faces; // Tableau dont on aura pas besoin
        faces_builder.creer_faces_reeles(dom,
                                         connectivite_som_elem,
                                         les_faces,
                                         elem_faces);
      }
      const int nbfaces2D = les_faces.nb_faces();


      int newnbsom = oldnbsom*(NZ+1)+NZ*oldsz+nbfaces2D*NZ+oldnbsom*NZ;
      DoubleTab new_soms(newnbsom, 3);
      DoubleTab& coord_sommets=dom.les_sommets();
      Objet_U::dimension=3;


      // les sommets du maillage 2D sont translates en premier
      for (int i=0; i<oldnbsom; i++)
        {
          double x = coord_sommets(i,0);
          double y = coord_sommets(i,1);
          double z=0;
          if (coord_sommets.dimension(1)>2)
            z=coord_sommets(i,2);
          for (int k=0; k<=NZ; k++)
            {
              new_soms(k*oldnbsom+i,0)=x;
              new_soms(k*oldnbsom+i,1)=y;
              new_soms(k*oldnbsom+i,2)=z;

              x += dx;
              y += dy;
              z += dz;
            }
        }


      // creation des centres de gravite des elements 2D puis translation de ces points
      for (int i=0; i<oldsz; i++)
        {
          int i0=les_elems(i,0);
          int i1=les_elems(i,1);
          int i2=les_elems(i,2);

          double xg = 1./3.*(coord_sommets(i0,0)+coord_sommets(i1,0)+coord_sommets(i2,0))+0.5*dx;
          double yg = 1./3.*(coord_sommets(i0,1)+coord_sommets(i1,1)+coord_sommets(i2,1))+0.5*dy;
          double z = 0.5*dz;
          if (coord_sommets.dimension(1)>2)
            z = 1./3.*(coord_sommets(i0,2)+coord_sommets(i1,2)+coord_sommets(i2,2))+0.5*dz;
          for (int k=0; k<NZ; k++)
            {

              new_soms(oldnbsom*(NZ+1)+k*oldsz+i,0)=xg;
              new_soms(oldnbsom*(NZ+1)+k*oldsz+i,1)=yg;
              new_soms(oldnbsom*(NZ+1)+k*oldsz+i,2)=z;

              xg += dx;
              yg += dy;
              z  += dz;
            }
        }


      // creation des centres des faces du maillage 2D puis translation de ces points
      for (int i=0; i<nbfaces2D; i++)
        {
          int i0=les_faces.sommet(i,0);
          int i1=les_faces.sommet(i,1);

          double x01 = 0.5*(coord_sommets(i0,0)+coord_sommets(i1,0))+0.5*dx;
          double y01 = 0.5*(coord_sommets(i0,1)+coord_sommets(i1,1))+0.5*dy;
          double z = 0.5*dz;
          if (coord_sommets.dimension(1)>2)
            z = 0.5*(coord_sommets(i0,2)+coord_sommets(i1,2))+0.5*dz;
          for (int k=0; k<NZ; k++)
            {
              new_soms(oldnbsom*(NZ+1)+NZ*oldsz+k*nbfaces2D+i,0)=x01;
              new_soms(oldnbsom*(NZ+1)+NZ*oldsz+k*nbfaces2D+i,1)=y01;
              new_soms(oldnbsom*(NZ+1)+NZ*oldsz+k*nbfaces2D+i,2)=z;
              x01 += dx;
              y01 += dy;
              z   += dz;
            }
        }

      // creation des centres des aretes du maillage 2D puis translation de ces points
      for (int i=0; i<oldnbsom; i++)
        {
          double x = coord_sommets(i,0)+0.5*dx;
          double y = coord_sommets(i,1)+ 0.5*dy;
          double z = 0.5*dz;
          if (coord_sommets.dimension(1)>2)
            z=coord_sommets(i,2)+0.5*dz;
          for (int k=0; k<NZ; k++)
            {
              new_soms(oldnbsom*(NZ+1)+NZ*oldsz+NZ*nbfaces2D+k*oldnbsom+i,0)=x;
              new_soms(oldnbsom*(NZ+1)+NZ*oldsz+NZ*nbfaces2D+k*oldnbsom+i,1)=y;
              new_soms(oldnbsom*(NZ+1)+NZ*oldsz+NZ*nbfaces2D+k*oldnbsom+i,2)=z;
              x += dx;
              y += dy;
              z += dz;
            }
        }


      coord_sommets.reset();
      dom.ajouter(new_soms);

      int newnbelem = 20*NZ*oldsz;
      IntTab new_elems(newnbelem, 4); // les nouveaux elements
      int cpt=0;


      // en premier, on stocke les tetra du haut et du bas : NZ*nb_triangle*2 tetra
      for (int i=0; i<oldsz; i++)
        {
          int i0=les_elems(i,0);
          int i1=les_elems(i,1);
          int i2=les_elems(i,2);

          int ig=oldnbsom*(NZ+1)+i;

          for (int k=0; k<NZ; k++)
            {
              new_elems(2*k*oldsz+2*i,0) = i0;
              new_elems(2*k*oldsz+2*i,1) = i1;
              new_elems(2*k*oldsz+2*i,2) = i2;
              new_elems(2*k*oldsz+2*i,3) = ig;
              cpt++;

              new_elems(2*k*oldsz+2*i+1,0) = i0+oldnbsom;
              new_elems(2*k*oldsz+2*i+1,1) = i1+oldnbsom;
              new_elems(2*k*oldsz+2*i+1,2) = i2+oldnbsom;
              new_elems(2*k*oldsz+2*i+1,3) = ig;
              cpt++;

              mettre_a_jour_sous_domaine(dom,i,2*k*oldsz+2*i,2);

              i0+=oldnbsom;
              i1+=oldnbsom;
              i2+=oldnbsom;
              ig+=oldsz;
            }
        }



      // puis les autres tetras
      for (int i=0; i<nbfaces2D; i++)
        {
          for (int ivois=0; ivois<2; ivois++)
            {
              int elem = les_faces.voisin(i,ivois);

              if (elem>=0)
                {
                  int i0=les_faces.sommet(i,0);
                  int i1=les_faces.sommet(i,1);
                  int i01=oldnbsom*(NZ+1)+NZ*oldsz+i;

                  for (int k=0; k<NZ; k++)
                    {
                      int ig=oldnbsom*(NZ+1)+k*oldsz+elem;

                      int i00=oldnbsom*(NZ+1)+NZ*oldsz+NZ*nbfaces2D+i0;
                      int i11=oldnbsom*(NZ+1)+NZ*oldsz+NZ*nbfaces2D+i1;
                      //                                int i00=oldnbsom*(NZ+1)+NZ*oldsz+NZ*nbfaces2D+k*oldnbsom+i0;
                      //                                int i11=oldnbsom*(NZ+1)+NZ*oldsz+NZ*nbfaces2D+k*oldnbsom+i1;

                      new_elems(cpt,0) = i0;
                      new_elems(cpt,1) = i1;
                      new_elems(cpt,2) = i01;
                      new_elems(cpt++,3) = ig;

                      new_elems(cpt,0) = i0+oldnbsom;
                      new_elems(cpt,1) = i1+oldnbsom;
                      new_elems(cpt,2) = i01;
                      new_elems(cpt++,3) = ig;

                      new_elems(cpt,0) = i1;
                      new_elems(cpt,1) = i11;
                      new_elems(cpt,2) = i01;
                      new_elems(cpt++,3) = ig;

                      new_elems(cpt,0) = i11;
                      new_elems(cpt,1) = i1+oldnbsom;
                      new_elems(cpt,2) = i01;
                      new_elems(cpt++,3) = ig;

                      new_elems(cpt,0) = i0;
                      new_elems(cpt,1) = i00;
                      new_elems(cpt,2) = i01;
                      new_elems(cpt++,3) = ig;

                      new_elems(cpt,0) = i00;
                      new_elems(cpt,1) = i0+oldnbsom;
                      new_elems(cpt,2) = i01;
                      new_elems(cpt++,3) = ig;


                      i0+=oldnbsom;
                      i1+=oldnbsom;
                      i01+=nbfaces2D;
                      ig+=oldsz;
                    }
                }
            }
        }

      les_elems.ref(new_elems);

      // Reconstruction de l'octree
      dom.invalide_octree();
      dom.typer("Tetraedre");

      extruder_dvt(dom, les_faces,oldnbsom, oldsz);

    }
  else
    {
      Cerr << "It is not known yet how to extrude "
           << dom.type_elem()->que_suis_je() <<"s"<<finl;
      exit();
    }
}



void Extruder_en20::traiter_faces_dvt(Faces& les_faces_bord, Faces& les_faces, int oldnbsom, int oldsz, int nbfaces2D )
{
  int size_2D = les_faces_bord.nb_faces();

  IntTab les_sommets(6*size_2D*NZ, 3);

  for (int i=0; i<size_2D; i++)
    {
      int i0=les_faces_bord.sommet(i,0);
      int i1=les_faces_bord.sommet(i,1);

      //double x01 = 0.5*(coord_sommets(i0,0)+coord_sommets(i1,0));
      //double y01 = 0.5*(coord_sommets(i0,1)+coord_sommets(i1,1));

      // on recherche le numero de cette face de bord: pas top!
      int jface=-1;
      for (int iface=0; iface<nbfaces2D; iface++)
        {
          int j0=les_faces.sommet(iface,0);
          int j1=les_faces.sommet(iface,1);

          if (((i0==j0) &&(i1==j1)) || ((i0==j1) &&(i1==j0)))
            {
              jface=iface;
              break;
            }
        }
      assert(jface>=0);

      for (int k=0; k<NZ; k++)
        {
          //double z = (k+0.5)*dz;
          //int i01 = domaine.chercher_sommets(x01, y01, z);
          int j01 = oldnbsom*(NZ+1)+NZ*oldsz+k*nbfaces2D+jface;

          int i00=oldnbsom*(NZ+1)+NZ*oldsz+NZ*nbfaces2D+i0;
          int i11=oldnbsom*(NZ+1)+NZ*oldsz+NZ*nbfaces2D+i1;
          //                        int i00=oldnbsom*(NZ+1)+NZ*oldsz+NZ*nbfaces2D+k*oldnbsom+i0;
          //                        int i11=oldnbsom*(NZ+1)+NZ*oldsz+NZ*nbfaces2D+k*oldnbsom+i1;

          les_sommets(k*6*size_2D+6*i,0) = i0;
          les_sommets(k*6*size_2D+6*i,1) = i1;
          les_sommets(k*6*size_2D+6*i,2) = j01;

          les_sommets(k*6*size_2D+6*i+1,0) = j01;
          les_sommets(k*6*size_2D+6*i+1,1) = i1;
          les_sommets(k*6*size_2D+6*i+1,2) = i11;

          les_sommets(k*6*size_2D+6*i+2,0) = j01;
          les_sommets(k*6*size_2D+6*i+2,1) = i11;
          les_sommets(k*6*size_2D+6*i+2,2) = i1+oldnbsom;

          les_sommets(k*6*size_2D+6*i+3,0) = j01;
          les_sommets(k*6*size_2D+6*i+3,1) = i0+oldnbsom;
          les_sommets(k*6*size_2D+6*i+3,2) = i1+oldnbsom;

          les_sommets(k*6*size_2D+6*i+4,0) = j01;
          les_sommets(k*6*size_2D+6*i+4,1) = i0+oldnbsom;
          les_sommets(k*6*size_2D+6*i+4,2) = i00;

          les_sommets(k*6*size_2D+6*i+5,0) = j01;
          les_sommets(k*6*size_2D+6*i+5,1) = i00;
          les_sommets(k*6*size_2D+6*i+5,2) = i0;

          i0+=oldnbsom;
          i1+=oldnbsom;
        }
    }

  les_faces_bord.typer(Faces::triangle_3D);
  les_faces_bord.les_sommets().ref(les_sommets);
  les_faces_bord.voisins().resize(6*size_2D*NZ, 2);
  les_faces_bord.voisins()=-1;
}



void Extruder_en20::extruder_dvt(Domaine& dom, Faces& les_faces, int oldnbsom, int oldsz)
{
  const int nbfaces2D = les_faces.nb_faces();
  IntTab& les_elems = dom.les_elems();

  for (auto &itr : dom.faces_bord())
    {
      Faces& les_faces_bord = itr.faces();
      traiter_faces_dvt(les_faces_bord, les_faces, oldnbsom, oldsz, nbfaces2D);
    }

  for (auto &itr : dom.faces_raccord())
    {
      Faces& les_faces_bord = itr->faces();
      traiter_faces_dvt(les_faces_bord, les_faces, oldnbsom, oldsz, nbfaces2D);
    }

  Bord& devant = dom.faces_bord().add(Bord());
  devant.nommer("devant");
  Faces& les_faces_dvt=devant.faces();
  les_faces_dvt.typer(Faces::triangle_3D);

  IntTab som_dvt(oldsz, 3);
  les_faces_dvt.voisins().resize(oldsz, 2);
  les_faces_dvt.voisins()=-1;

  Bord& derriere = dom.faces_bord().add(Bord());
  derriere.nommer("derriere");
  Faces& les_faces_der=derriere.faces();
  les_faces_der.typer(Faces::triangle_3D);

  IntTab som_der(oldsz, 3);
  les_faces_der.voisins().resize(oldsz, 2);
  les_faces_der.voisins()=-1;

  for (int i=0; i<oldsz; i++)
    {
      int i0=les_elems(2*i,0);
      int i1=les_elems(2*i,1);
      int i2=les_elems(2*i,2);

      som_dvt(i,0) = i0;
      som_dvt(i,1) = i1;
      som_dvt(i,2) = i2;

      som_der(i,0) = i0+oldnbsom*NZ;
      som_der(i,1) = i1+oldnbsom*NZ;
      som_der(i,2) = i2+oldnbsom*NZ;

    }


  les_faces_dvt.les_sommets().ref(som_dvt);
  les_faces_der.les_sommets().ref(som_der);

}

