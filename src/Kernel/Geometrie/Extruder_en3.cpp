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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Extruder_en3.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////

#include <Extruder_en3.h>
#include <Domaine.h>
#include <Static_Int_Lists.h>
#include <Connectivite_som_elem.h>
#include <Faces_builder.h>
#include <Scatter.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Extruder_en3,"Extruder_en3",Interprete_geometrique_base);

Extruder_en3::Extruder_en3()
{
  nom_dvt_ = "devant";
  nom_derriere_ = "derriere";
  direction_.resize(3,Array_base::NOCOPY_NOINIT);
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
Sortie& Extruder_en3::printOn(Sortie& os) const
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
Entree& Extruder_en3::readOn(Entree& is)
{
  return Interprete::readOn(is);
}


// Description:
//    Fonction principale de l'interprete Extruder_en3
//    Triangule 1 a 1 toutes les zones du domaine
//    specifie par la directive.
//    On triangule la zone grace a la methode:
//      void Extruder_en3::extruder(Zone& zone) const
//    Extruder_en3 signifie ici transformer en triangle des
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
Entree& Extruder_en3::interpreter_(Entree& is)
{
  if (Objet_U::dimension!=2)
    {
      Cerr << " We only can extruder_en3 the 2D meshes ! " << finl;
      exit();
    }

  int nb_dom=0;
  Noms nom_dom;
  Cerr << "The format has changed after the 1.5.3. A list of domains must now be readen as follows :" << finl;
  Cerr << "domaine N dom1 dom2 domN" << finl;
  Param param(que_suis_je());
  param.ajouter("domaine",&nom_dom,Param::REQUIRED);
  param.ajouter("nb_tranches",&NZ_,Param::REQUIRED);
  param.ajouter_arr_size_predefinie("direction",&direction_,Param::REQUIRED);
  param.ajouter("nom_cl_devant",&nom_dvt_);
  param.ajouter("nom_cl_derriere",&nom_derriere_);
  param.lire_avec_accolades_depuis(is);
  nb_dom=nom_dom.size();

  // creation du tableau de correspondance des indices
  // pour assurer le decoupage conforme entre domaines
  //////////////////////////////////////////////////////

  // PQ : 07/09/08 : le decoupage des prismes apres extrusion est base sur une numerotation
  // globale des indices qui permet d'assurer la conformite des maillages. Ces indices references
  // dans le tableau nums sont issus du regroupement des domaines en un seul suivant la methode
  // se trouvant dans Mailler.cpp.
  // On procede par concatenation des domaines dans "dom_tot" de maniere a ne pas interferer
  // avec les domaines et les zones d'origines.

  Domaine dom_tot;
  for(int i=0; i<nb_dom; i++)
    {
      // on parcourt les domaines a l'envers pour retrouver l'ancien comportement
      associer_domaine(nom_dom[nb_dom-1-i]);
      Domaine& domi=domaine(i);
      Zone& zone_tot=dom_tot.add(domi.zone(0));
      zone_tot.associer_domaine(dom_tot);
      int nb_som=domi.coord_sommets().dimension(0);
      IntVect num(nb_som);
      dom_tot.ajouter(domi.coord_sommets(), num);
      zone_tot.renum(num);
      zone_tot.associer_domaine(dom_tot);
      Scatter::uninit_sequential_domain(dom_tot);
      dom_tot.comprimer();
      dom_tot.zone(0).fixer_premieres_faces_frontiere();
      dom_tot.zone(0).type_elem().associer_zone(dom_tot.zone(0));

      /////////////////////////
      // extrusion des domaines
      /////////////////////////
      Scatter::uninit_sequential_domain(domi);
      extruder(domi,num);
      Scatter::init_sequential_domain(domi);
    }

  return is;
}

// Description:
// Extrusion d'un domaine surfacique
void Extruder_en3::extruder(Domaine& dom, const IntVect& num)
{
  Zone& zone = dom.zone(0);
  if(zone.type_elem()->que_suis_je() == "Triangle")
    {
      int oldnbsom = zone.nb_som();
      IntTab& les_elems=zone.les_elems();
      int oldsz=les_elems.dimension(0);
      double dx = direction_(0)/NZ_;
      double dy = direction_(1)/NZ_;
      double dz = direction_(2)/NZ_;

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

      int newnbsom = oldnbsom*(NZ_+1);
      DoubleTab new_soms(newnbsom, 3);
      DoubleTab& coord_sommets=dom.les_sommets();
      Objet_U::dimension=3;

      // les sommets du maillage 2D sont translates en premier
      for (int i=0; i<oldnbsom; i++)
        {
          double x = coord_sommets(i,0);
          double y = coord_sommets(i,1);
          double z=0.;
          if (coord_sommets.dimension(1)>2)
            z=coord_sommets(i,2);
          for (int k=0; k<=NZ_; k++)
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

      int newnbelem = 3*NZ_*oldsz;
      IntTab new_elems(newnbelem, 4); // les nouveaux elements
      for (int i=0; i<oldsz; i++)
        {
          int i1=les_elems(i,0);
          int i2=les_elems(i,1);
          int i3=les_elems(i,2);

          int ii1 = num[i1];
          int ii2 = num[i2];
          int ii3 = num[i3];
          int i4=i1+oldnbsom;
          int i5=i2+oldnbsom;
          int i6=i3+oldnbsom;

          for (int k=0; k<NZ_; k++)
            {
              int j=3*k*oldsz+3*i;
              if (ii1>ii2 && ii1>ii3)
                {
                  new_elems(j,0) = i1;
                  new_elems(j,1) = i2;
                  new_elems(j,2) = i3;
                  new_elems(j,3) = i4;
                  if (ii3>ii2)
                    {
                      new_elems(j+1,0) = i2;
                      new_elems(j+1,1) = i3;
                      new_elems(j+1,2) = i4;
                      new_elems(j+1,3) = i6;

                      new_elems(j+2,0) = i2;
                      new_elems(j+2,1) = i4;
                      new_elems(j+2,2) = i5;
                      new_elems(j+2,3) = i6;
                    }
                  else
                    {
                      new_elems(j+1,0) = i2;
                      new_elems(j+1,1) = i3;
                      new_elems(j+1,2) = i4;
                      new_elems(j+1,3) = i5;

                      new_elems(j+2,0) = i3;
                      new_elems(j+2,1) = i4;
                      new_elems(j+2,2) = i5;
                      new_elems(j+2,3) = i6;
                    }
                }

              if (ii2>ii1 && ii2>ii3)
                {
                  new_elems(j,0) = i1;
                  new_elems(j,1) = i2;
                  new_elems(j,2) = i3;
                  new_elems(j,3) = i5;
                  if (ii1>ii3)
                    {
                      new_elems(j+1,0) = i1;
                      new_elems(j+1,1) = i3;
                      new_elems(j+1,2) = i4;
                      new_elems(j+1,3) = i5;

                      new_elems(j+2,0) = i3;
                      new_elems(j+2,1) = i4;
                      new_elems(j+2,2) = i5;
                      new_elems(j+2,3) = i6;
                    }
                  else
                    {
                      new_elems(j+1,0) = i1;
                      new_elems(j+1,1) = i3;
                      new_elems(j+1,2) = i5;
                      new_elems(j+1,3) = i6;

                      new_elems(j+2,0) = i1;
                      new_elems(j+2,1) = i4;
                      new_elems(j+2,2) = i5;
                      new_elems(j+2,3) = i6;
                    }
                }

              if (ii3>ii1 && ii3>ii2)
                {
                  new_elems(j,0) = i1;
                  new_elems(j,1) = i2;
                  new_elems(j,2) = i3;
                  new_elems(j,3) = i6;
                  if (ii2>ii1)
                    {
                      new_elems(j+1,0) = i1;
                      new_elems(j+1,1) = i2;
                      new_elems(j+1,2) = i5;
                      new_elems(j+1,3) = i6;

                      new_elems(j+2,0) = i1;
                      new_elems(j+2,1) = i4;
                      new_elems(j+2,2) = i5;
                      new_elems(j+2,3) = i6;
                    }
                  else
                    {
                      new_elems(j+1,0) = i1;
                      new_elems(j+1,1) = i2;
                      new_elems(j+1,2) = i4;
                      new_elems(j+1,3) = i6;

                      new_elems(j+2,0) = i2;
                      new_elems(j+2,1) = i4;
                      new_elems(j+2,2) = i5;
                      new_elems(j+2,3) = i6;
                    }
                }

              mettre_a_jour_sous_zone(zone,i,j,3);
              i1+=oldnbsom;
              i2+=oldnbsom;
              i3+=oldnbsom;
              i4+=oldnbsom;
              i5+=oldnbsom;
              i6+=oldnbsom;
            }
        }
      // Reconstruction de l'octree
      zone.invalide_octree();
      zone.typer("Tetraedre");

      les_elems.ref(new_elems);
      construire_bords(dom, les_faces,oldnbsom, oldsz, num);
    }
  else
    {
      Cerr << "It is not known yet how to extrude "
           << zone.type_elem()->que_suis_je() <<"s"<<finl;
    }
}

// Description:
// Creation des bords du domaine extrude
void Extruder_en3::construire_bords(Domaine& dom, Faces& les_faces, int oldnbsom, int oldsz, const IntVect& num)
{
  Zone& zone = dom.zone(0);
  IntTab& les_elems=zone.les_elems();
  // Les bords:
  {
    LIST_CURSEUR(Bord) curseur(zone.faces_bord());
    while(curseur)
      {
        Faces& les_faces_du_bord=curseur->faces();
        construire_bord_lateral(les_faces_du_bord, les_faces, oldnbsom, num);
        ++curseur;
      }
  }
  // Les raccords:
  {
    LIST_CURSEUR(Raccord) curseur(zone.faces_raccord());
    while(curseur)
      {
        Faces& les_faces_du_bord=curseur.valeur()->faces();
        construire_bord_lateral(les_faces_du_bord, les_faces, oldnbsom, num);
        ++curseur;
      }
  }

  // Devant
  if(nom_dvt_=="NULL")
    {
      Cerr << "We don't associate any boundary to the front of the domain " << dom.le_nom() << finl;
    }
  else
    {
      Bord& devant = zone.faces_bord().add(Bord());
      devant.nommer(nom_dvt_);
      Faces& les_faces_dvt=devant.faces();
      les_faces_dvt.typer(Faces::triangle_3D);

      IntTab som_dvt(oldsz, 3);
      les_faces_dvt.voisins().resize(oldsz, 2);
      les_faces_dvt.voisins()=-1;

      for (int i=0; i<oldsz; i++)
        {
          int i0=les_elems(3*i,0);
          int i1=les_elems(3*i,1);
          int i2=les_elems(3*i,2);

          som_dvt(i,0) = i0;
          som_dvt(i,1) = i1;
          som_dvt(i,2) = i2;
        }
      les_faces_dvt.les_sommets().ref(som_dvt);
    }

  // Derriere
  if(nom_derriere_=="NULL")
    {
      Cerr << "We don't associate any boundary to the back of the domain " << dom.le_nom() << finl;
    }
  else
    {
      Bord& derriere = zone.faces_bord().add(Bord());
      derriere.nommer(nom_derriere_);
      Faces& les_faces_der=derriere.faces();
      les_faces_der.typer(Faces::triangle_3D);

      IntTab som_der(oldsz, 3);
      les_faces_der.voisins().resize(oldsz, 2);
      les_faces_der.voisins()=-1;

      for (int i=0; i<oldsz; i++)
        {
          int i0=les_elems(3*i,0);
          int i1=les_elems(3*i,1);
          int i2=les_elems(3*i,2);

          som_der(i,0) = i0+oldnbsom*NZ_;
          som_der(i,1) = i1+oldnbsom*NZ_;
          som_der(i,2) = i2+oldnbsom*NZ_;
        }
      les_faces_der.les_sommets().ref(som_der);
    }
}

// Description:
// Creation d'un bord lateral
void Extruder_en3::construire_bord_lateral(Faces& les_faces_du_bord, Faces& les_faces, int oldnbsom, const IntVect& num)
{
  // oldnbsom = number of nodes of the 2D mesh
  int nb_faces = les_faces_du_bord.nb_faces();
  IntTab les_sommets(2*nb_faces*NZ_,3);
  for (int i=0; i<nb_faces; i++)
    {
      int i0=les_faces_du_bord.sommet(i,0);
      int i1=les_faces_du_bord.sommet(i,1);
      int ii0= num[i0];
      int ii1= num[i1];
      for (int k=0; k<NZ_; k++)
        {
          int j=2*nb_faces*k+2*i;

          if (ii0>ii1)
            {
              les_sommets(j,0) = i0;
              les_sommets(j,1) = i1;
              les_sommets(j,2) = i0+oldnbsom;

              les_sommets(j+1,0) = i1;
              les_sommets(j+1,1) = i0+oldnbsom;
              les_sommets(j+1,2) = i1+oldnbsom;
            }
          else
            {
              les_sommets(j,0) = i0;
              les_sommets(j,1) = i1;
              les_sommets(j,2) = i1+oldnbsom;

              les_sommets(j+1,0) = i0;
              les_sommets(j+1,1) = i0+oldnbsom;
              les_sommets(j+1,2) = i1+oldnbsom;
            }
          i0+=oldnbsom;
          i1+=oldnbsom;
        }
    }

  les_faces_du_bord.typer(Faces::triangle_3D);
  les_faces_du_bord.les_sommets().ref(les_sommets);
  les_faces_du_bord.voisins().resize(2*nb_faces*NZ_, 2);
  les_faces_du_bord.voisins()=-1;
}

