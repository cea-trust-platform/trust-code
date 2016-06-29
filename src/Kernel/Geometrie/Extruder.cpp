/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Extruder.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/26
//
//////////////////////////////////////////////////////////////////////////////

#include <Extruder.h>
#include <Domaine.h>
#include <Static_Int_Lists.h>
#include <Connectivite_som_elem.h>
#include <Faces_builder.h>
#include <Scatter.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Extruder,"Extruder",Interprete_geometrique_base);
Extruder::Extruder()
{
  direction.resize(3,Array_base::NOCOPY_NOINIT);
}

// Description:
//    Simple appel a: Interprete::printOn(Sortie&)
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Extruder::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}


// Description:
//    Simple appel a: Interprete::readOn(Entree&)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Extruder::readOn(Entree& is)
{
  return Interprete::readOn(is);
}


// Description:
//    Fonction principale de l'interprete Extruder
//    Triangule 1 a 1 toutes les zones du domaine
//    specifie par la directive.
//    On triangule la zone grace a la methode:
//      void Extruder::extruder(Zone& zone) const
//    Extruder signifie ici transformer en triangle des
//    elements geometrique d'une zone.
// Precondition: on doit etre en 2D (dimension d'espace=2)
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree
//    Contraintes:
// Exception: l'objet a mailler n'est pas du type Domaine
// Effets de bord:
// Postcondition:
Entree& Extruder::interpreter_(Entree& is)
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

inline void check_boundary_name(const Nom& name)
{
// On verifie que le bord ne se nomme pas deja devant ou derriere
  if (name=="devant" || name=="derriere")
    {
      Cerr << "Problem : you must change the name of the boundary  : " <<name<<finl;
      Cerr << "because the extrusion keyword is going to create new boundaries named derriere and devant" << finl;
      Cerr << "which will create a conflict." << finl;
      Process::exit();
    }
}
// Description:
//    Triangule tous les element d'une zone: transforme
//    les elements goemetriques de la zone en triangles.
//    Pour l'instant on ne sait raffiner que des Rectangles
//    (on les coupe en 4).
// Precondition:
// Parametre: Zone& zone
//    Signification: la zone dont on veut raffiner les elements
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Extruder::extruder(Domaine& dom)
{
  Zone& zone = dom.zone(0);

  if(Objet_U::dimension == 2 && (zone.type_elem()->que_suis_je() == "Rectangle" || zone.type_elem()->que_suis_je() ==  "Quadrangle" ))
    {
      extruder_hexa(dom);
    }
  else if(Objet_U::dimension == 2 && zone.type_elem()->que_suis_je() == "Triangle")
    {
      int oldnbsom = zone.nb_som();
      IntTab& les_elems=zone.les_elems();
      int oldsz=les_elems.dimension(0);
      double dx = direction(0)/NZ;
      double dy = direction(1)/NZ;
      double dz = direction(2)/NZ;

      Faces les_faces;
      //zone.creer_faces(les_faces);
      {
        // bloc a factoriser avec Zone_VF.cpp :
        Type_Face type_face = zone.type_elem().type_face(0);
        les_faces.typer(type_face);
        les_faces.associer_zone(zone);

        Static_Int_Lists connectivite_som_elem;
        const int     nb_sommets_tot = zone.domaine().nb_som_tot();
        const IntTab&    elements       = zone.les_elems();

        construire_connectivite_som_elem(nb_sommets_tot,
                                         elements,
                                         connectivite_som_elem,
                                         1 /* include virtual elements */);

        Faces_builder faces_builder;
        IntTab elem_faces; // Tableau dont on aura pas besoin
        faces_builder.creer_faces_reeles(zone,
                                         connectivite_som_elem,
                                         les_faces,
                                         elem_faces);
      }
      const int nbfaces2D = les_faces.nb_faces();


      int newnbsom = oldnbsom*(NZ+1)+NZ*oldsz+nbfaces2D*NZ;
      DoubleTab new_soms(newnbsom, 3);
      DoubleTab& coord_sommets=dom.les_sommets();
      Objet_U::dimension=3;


      // les sommets du maillage 2D sont translates en premier
      for (int i=0; i<oldnbsom; i++)
        {
          double x = coord_sommets(i,0);
          double y = coord_sommets(i,1);
          double z=0.;
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


      // puis on cree les centres de gravite des elements 2D puis translation de ces points
      for (int i=0; i<oldsz; i++)
        {
          int i0=les_elems(i,0);
          int i1=les_elems(i,1);
          int i2=les_elems(i,2);

          double xg = 1./3.*(coord_sommets(i0,0)+coord_sommets(i1,0)+coord_sommets(i2,0))+0.5*dx;
          double yg = 1./3.*(coord_sommets(i0,1)+coord_sommets(i1,1)+coord_sommets(i2,1))+0.5*dy;
          double z = 0.5*dz;

          for (int k=0; k<NZ; k++)
            {

              new_soms(oldnbsom*(NZ+1)+k*oldsz+i,0)=xg;
              new_soms(oldnbsom*(NZ+1)+k*oldsz+i,1)=yg;
              new_soms(oldnbsom*(NZ+1)+k*oldsz+i,2)=z;

              xg += dx;
              yg += dy;
              z += dz;
            }
        }


      // enfin, on cree les centres des faces du maillage 2D puis translation de ces points
      for (int i=0; i<nbfaces2D; i++)
        {
          int i0=les_faces.sommet(i,0);
          int i1=les_faces.sommet(i,1);

          double x01 = 0.5*(coord_sommets(i0,0)+coord_sommets(i1,0))+0.5*dx;
          double y01 = 0.5*(coord_sommets(i0,1)+coord_sommets(i1,1))+0.5*dy;
          double z = 0.5*dz;

          for (int k=0; k<NZ; k++)
            {
              new_soms(oldnbsom*(NZ+1)+NZ*oldsz+k*nbfaces2D+i,0)=x01;
              new_soms(oldnbsom*(NZ+1)+NZ*oldsz+k*nbfaces2D+i,1)=y01;
              new_soms(oldnbsom*(NZ+1)+NZ*oldsz+k*nbfaces2D+i,2)=z;
              x01 += dx;
              y01 += dy;
              z += dz;
            }
        }


      coord_sommets.resize(0);
      dom.ajouter(new_soms);

      int newnbelem = 14*NZ*oldsz;
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

              mettre_a_jour_sous_zone(zone,i,2*k*oldsz+2*i,2);

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

                      new_elems(cpt,0) = i0;
                      new_elems(cpt,1) = i1;
                      new_elems(cpt,2) = i01;
                      new_elems(cpt++,3) = ig;

                      new_elems(cpt,0) = i0+oldnbsom;
                      new_elems(cpt,1) = i1+oldnbsom;
                      new_elems(cpt,2) = i01;
                      new_elems(cpt++,3) = ig;

                      new_elems(cpt,0) = i1;
                      new_elems(cpt,1) = i1+oldnbsom;
                      new_elems(cpt,2) = i01;
                      new_elems(cpt++,3) = ig;

                      new_elems(cpt,0) = i0;
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
      zone.invalide_octree();
      zone.typer("Tetraedre");

      extruder_dvt(dom, les_faces,oldnbsom, oldsz);

    }
  else
    {
      Cerr << "It is not known yet how to extrude "
           << zone.type_elem()->que_suis_je() <<"s"<<finl;
      exit();
    }
}



void Extruder::traiter_faces_dvt(Faces& les_faces_bord, Faces& les_faces, int oldnbsom, int oldsz, int nbfaces2D )
{
  int size_2D = les_faces_bord.nb_faces();

  IntTab les_sommets(4*size_2D*NZ, 3);

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
          //int i01 = zone.chercher_sommets(x01, y01, z);
          int j01 = oldnbsom*(NZ+1)+NZ*oldsz+k*nbfaces2D+jface;

          les_sommets(k*4*size_2D+4*i,0) = i0;
          les_sommets(k*4*size_2D+4*i,1) = i1;
          les_sommets(k*4*size_2D+4*i,2) = j01;

          les_sommets(k*4*size_2D+4*i+1,0) = j01;
          les_sommets(k*4*size_2D+4*i+1,1) = i1;
          les_sommets(k*4*size_2D+4*i+1,2) = i1+oldnbsom;


          les_sommets(k*4*size_2D+4*i+2,0) = j01;
          les_sommets(k*4*size_2D+4*i+2,1) = i0+oldnbsom;
          les_sommets(k*4*size_2D+4*i+2,2) = i1+oldnbsom;


          les_sommets(k*4*size_2D+4*i+3,0) = j01;
          les_sommets(k*4*size_2D+4*i+3,1) = i0+oldnbsom;
          les_sommets(k*4*size_2D+4*i+3,2) = i0;

          i0+=oldnbsom;
          i1+=oldnbsom;
        }
    }

  les_faces_bord.typer(triangle_3D);
  les_faces_bord.les_sommets().ref(les_sommets);
  les_faces_bord.voisins().resize(4*size_2D*NZ, 2);
  les_faces_bord.voisins()=-1;
}



void Extruder::extruder_dvt(Domaine& dom, Faces& les_faces, int oldnbsom, int oldsz)
{
  Zone& zone = dom.zone(0);
  const int nbfaces2D = les_faces.nb_faces();
  IntTab& les_elems=zone.les_elems();
  {
    LIST_CURSEUR(Bord) curseur(zone.faces_bord());
    while(curseur)
      {
        check_boundary_name(curseur->le_nom());
        Faces& les_faces_bord=curseur->faces();
        traiter_faces_dvt(les_faces_bord, les_faces, oldnbsom, oldsz, nbfaces2D );
        ++curseur;
      }
  }
  {
    LIST_CURSEUR(Raccord) curseur(zone.faces_raccord());
    while(curseur)
      {
        check_boundary_name(curseur->le_nom());
        Faces& les_faces_bord=curseur.valeur()->faces();
        traiter_faces_dvt(les_faces_bord, les_faces, oldnbsom, oldsz, nbfaces2D );
        ++curseur;
      }
  }

  Bord& devant = zone.faces_bord().add(Bord());
  devant.nommer("devant");
  Faces& les_faces_dvt=devant.faces();
  les_faces_dvt.typer(triangle_3D);

  IntTab som_dvt(oldsz, 3);
  les_faces_dvt.voisins().resize(oldsz, 2);
  les_faces_dvt.voisins()=-1;

  Bord& derriere = zone.faces_bord().add(Bord());
  derriere.nommer("derriere");
  Faces& les_faces_der=derriere.faces();
  les_faces_der.typer(triangle_3D);

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

void Extruder::extruder_hexa(Domaine& dom)
{
  Zone& zone = dom.zone(0);
  int oldnbsom = zone.nb_som();
  IntTab& les_elems=zone.les_elems();
  int oldsz=les_elems.dimension(0);
  double dx = direction(0)/NZ;
  double dy = direction(1)/NZ;
  double dz = direction(2)/NZ;

  Faces les_faces;
  {
    // bloc a factoriser avec Zone_VF.cpp :
    Type_Face type_face = zone.type_elem().type_face(0);
    les_faces.typer(type_face);
    les_faces.associer_zone(zone);

    Static_Int_Lists connectivite_som_elem;
    const int     nb_sommets_tot = zone.domaine().nb_som_tot();
    const IntTab&    elements       = zone.les_elems();

    construire_connectivite_som_elem(nb_sommets_tot,
                                     elements,
                                     connectivite_som_elem,
                                     1 /* include virtual elements */);

    Faces_builder faces_builder;
    IntTab elem_faces; // Tableau dont on aura pas besoin
    faces_builder.creer_faces_reeles(zone,
                                     connectivite_som_elem,
                                     les_faces,
                                     elem_faces);
  }

  int newnbsom = oldnbsom*(NZ+1);
  DoubleTab new_soms(newnbsom, 3);
  DoubleTab& coord_sommets=dom.les_sommets();
  Objet_U::dimension=3;

  int i;
  // les sommets du maillage 2D sont translates
  for (i=0; i<oldnbsom; i++)
    {
      double x = coord_sommets(i,0);
      double y = coord_sommets(i,1);
      double z=0.;
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


  coord_sommets.resize(0);
  dom.ajouter(new_soms);

  int newnbelem = NZ*oldsz;
  IntTab new_elems(newnbelem, 8); // les nouveaux elements


  // definition des nouveaux hexas
  for (i=0; i<oldsz; i++)
    {
      int i0=les_elems(i,0);
      int i1=les_elems(i,1);
      int i2=les_elems(i,2);
      int i3=les_elems(i,3);


      for (int k=0; k<NZ; k++)
        {
          new_elems(k*oldsz+i,0) = i0;
          new_elems(k*oldsz+i,1) = i1;
          new_elems(k*oldsz+i,2) = i2;
          new_elems(k*oldsz+i,3) = i3;
          new_elems(k*oldsz+i,4) = i0+oldnbsom;
          new_elems(k*oldsz+i,5) = i1+oldnbsom;
          new_elems(k*oldsz+i,6) = i2+oldnbsom;
          new_elems(k*oldsz+i,7) = i3+oldnbsom;


          i0+=oldnbsom;
          i1+=oldnbsom;
          i2+=oldnbsom;
          i3+=oldnbsom;
        }
    }

  les_elems.ref(new_elems);

  // Reconstruction de l'octree
  zone.invalide_octree();
  if ((zone.type_elem()->que_suis_je()) ==  "Quadrangle")
    zone.typer("Hexaedre_VEF");
  else
    zone.typer("Hexaedre");

  extruder_dvt_hexa(dom, les_faces,oldnbsom, oldsz);
}

void Extruder::traiter_faces_dvt_hexa(Faces& les_faces_bord, int oldnbsom)
{
  int size_2D = les_faces_bord.nb_faces();

  IntTab les_sommets(size_2D*NZ, 4);

  for (int i=0; i<size_2D; i++)
    {
      int i0=les_faces_bord.sommet(i,0);
      int i1=les_faces_bord.sommet(i,1);


      for (int k=0; k<NZ; k++)
        {
          les_sommets(k*size_2D+i,0) = i0;
          les_sommets(k*size_2D+i,1) = i1;
          les_sommets(k*size_2D+i,2) = i0+oldnbsom;
          les_sommets(k*size_2D+i,3) = i1+oldnbsom;

          i0+=oldnbsom;
          i1+=oldnbsom;
        }
    }

  les_faces_bord.typer(quadrangle_3D);
  les_faces_bord.les_sommets().ref(les_sommets);
  les_faces_bord.voisins().resize(size_2D*NZ, 2);
  les_faces_bord.voisins()=-1;
}

void Extruder::extruder_dvt_hexa(Domaine& dom, Faces& les_faces, int oldnbsom, int oldsz)
{
  Zone& zone = dom.zone(0);
  IntTab& les_elems=zone.les_elems();
  {
    LIST_CURSEUR(Bord) curseur(zone.faces_bord());
    while(curseur)
      {
        check_boundary_name(curseur->le_nom());
        Faces& les_faces_bord=curseur->faces();
        traiter_faces_dvt_hexa(les_faces_bord, oldnbsom);
        ++curseur;
      }
  }
  {
    LIST_CURSEUR(Raccord) curseur(zone.faces_raccord());
    while(curseur)
      {
        check_boundary_name(curseur->le_nom());
        Faces& les_faces_bord=curseur.valeur()->faces();
        traiter_faces_dvt_hexa(les_faces_bord, oldnbsom);
        ++curseur;
      }
  }

  Bord& devant = zone.faces_bord().add(Bord());
  devant.nommer("devant");
  Faces& les_faces_dvt=devant.faces();
  les_faces_dvt.typer(quadrangle_3D);

  IntTab som_dvt(oldsz, 4);
  les_faces_dvt.voisins().resize(oldsz, 2);
  les_faces_dvt.voisins()=-1;

  Bord& derriere = zone.faces_bord().add(Bord());
  derriere.nommer("derriere");
  Faces& les_faces_der=derriere.faces();
  les_faces_der.typer(quadrangle_3D);

  IntTab som_der(oldsz, 4);
  les_faces_der.voisins().resize(oldsz, 2);
  les_faces_der.voisins()=-1;

  for (int i=0; i<oldsz; i++)
    {
      int i0=les_elems(i,0);
      int i1=les_elems(i,1);
      int i2=les_elems(i,2);
      int i3=les_elems(i,3);

      som_dvt(i,0) = i0;
      som_dvt(i,1) = i1;
      som_dvt(i,2) = i2;
      som_dvt(i,3) = i3;

      som_der(i,0) = i0+oldnbsom*NZ;
      som_der(i,1) = i1+oldnbsom*NZ;
      som_der(i,2) = i2+oldnbsom*NZ;
      som_der(i,3) = i3+oldnbsom*NZ;

    }


  les_faces_dvt.les_sommets().ref(som_dvt);
  les_faces_der.les_sommets().ref(som_der);

}

