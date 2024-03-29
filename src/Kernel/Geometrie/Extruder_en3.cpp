/****************************************************************************
* Copyright (c) 2024, CEA
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
#include <Faces_builder.h>
#include <Extruder_en3.h>
#include <Domaine.h>
#include <Scatter.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Extruder_en3,"Extruder_en3",Interprete_geometrique_base);
// XD extruder_en3 extruder extruder_en3 1 Class to create a 3D tetrahedral/hexahedral mesh (a prism is cut in 3) from a 2D triangular/quadrangular mesh. The names of the boundaries (by default, devant (front) and derriere (back)) may be edited by the keyword nom_cl_devant and nom_cl_derriere. If 'null' is written for nom_cl, then no boundary condition is generated at this place. NL2 Recommendation : to ensure conformity between meshes (in case of fluid/solid coupling) it is recommended to extrude all the domains at the same time.

Extruder_en3::Extruder_en3():
  NZ_(-1),
  nom_dvt_("devant"),
  nom_derriere_("derriere")
{
  direction_.resize(3,RESIZE_OPTIONS::NOCOPY_NOINIT);
}

Sortie& Extruder_en3::printOn(Sortie& os) const { return Interprete::printOn(os); }

Entree& Extruder_en3::readOn(Entree& is) { return Interprete::readOn(is); }

/*! @brief Fonction principale de l'interprete Extruder_en3
 *
 * Triangule 1 a 1 toutes les domaines du domaine specifie par la directive.
 *     On triangule le domaine grace a la methode:
 *       void Extruder_en3::extruder(Domaine& domaine) const
 *     Extruder_en3 signifie ici transformer en triangle des
 *     elements geometrique d'un domaine.
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree
 * @throws l'objet a mailler n'est pas de type Domaine
 */
Entree& Extruder_en3::interpreter_(Entree& is)
{
  if (Objet_U::dimension!=2)
    {
      Cerr << " We only can extruder_en3 the 2D meshes ! " << finl;
      exit();
    }

  int nb_dom=0;
  Noms noms_dom;
  Param param(que_suis_je());
  param.ajouter("domaine",&noms_dom,Param::REQUIRED);  // XD attr domaine listchaine domain_name 0 List of the domains
  param.ajouter("nb_tranches",&NZ_,Param::REQUIRED);
  param.ajouter_arr_size_predefinie("direction",&direction_,Param::REQUIRED);
  param.ajouter("nom_cl_devant",&nom_dvt_);         // XD attr nom_cl_devant chaine nom_cl_devant 1 New name of the first boundary.
  param.ajouter("nom_cl_derriere",&nom_derriere_);  // XD attr nom_cl_derriere chaine nom_cl_derriere 1 New name of the second boundary.
  param.lire_avec_accolades_depuis(is);
  nb_dom=noms_dom.size();

  // creation du tableau de correspondance des indices
  // pour assurer le decoupage conforme entre domaines
  //////////////////////////////////////////////////////

  // PQ : 07/09/08 : le decoupage des prismes apres extrusion est base sur une numerotation
  // globale des indices qui permet d'assurer la conformite des maillages. Ces indices references
  // dans le tableau nums sont issus du regroupement des domaines en un seul
  // On procede par concatenation des domaines dans "dom_tot" de maniere a ne pas interferer
  // avec les domaines d'origines.
  Domaine dom_tot;   // just to get correct renumbering
  for(int i=0; i<nb_dom; i++)
    {
      associer_domaine(noms_dom[nb_dom-1-i]);
      Domaine& domi=domaine(i);
      IntVect num;
      dom_tot.ajouter(domi.coord_sommets(), num);

      /////////////////////////
      // extrusion des domaines
      /////////////////////////
      Scatter::uninit_sequential_domain(domi);
      extruder(domi,num);
      Scatter::init_sequential_domain(domi);
    }

  return is;
}

/*! @brief Extrusion d'un domaine surfacique
 *
 */
void Extruder_en3::extruder(Domaine& dom, const IntVect& num)
{
  if(dom.type_elem()->que_suis_je() == "Triangle")
    {
      int oldnbsom = dom.nb_som();
      IntTab& les_elems=dom.les_elems();
      int oldsz=les_elems.dimension(0);
      double dx = direction_[0]/NZ_;
      double dy = direction_[1]/NZ_;
      double dz = direction_[2]/NZ_;

      Faces les_faces;
      {
        // bloc a factoriser avec Domaine_VF.cpp :
        Type_Face type_face = dom.type_elem()->type_face(0);
        les_faces.typer(type_face);
        les_faces.associer_domaine(dom);

        Static_Int_Lists connectivite_som_elem;
        const IntTab& elements = dom.les_elems();
        const int nb_sommets_tot = dom.nb_som_tot();

        construire_connectivite_som_elem(nb_sommets_tot,
                                         elements,
                                         connectivite_som_elem,
                                         1 /* include virtual elements */);

        Faces_builder faces_builder;
        IntTab dnu; // Tableau dont on aura pas besoin
        faces_builder.creer_faces_reeles(dom,
                                         connectivite_som_elem,
                                         les_faces,
                                         dnu);
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

              mettre_a_jour_sous_domaine(dom,i,j,3);
              i1+=oldnbsom;
              i2+=oldnbsom;
              i3+=oldnbsom;
              i4+=oldnbsom;
              i5+=oldnbsom;
              i6+=oldnbsom;
            }
        }
      // Reconstruction de l'octree
      dom.invalide_octree();
      dom.typer("Tetraedre");

      les_elems.ref(new_elems);
      construire_bords(dom, les_faces,oldnbsom, oldsz, num);
    }
  else
    Cerr << "TRUST doesn't know how to extrude " << dom.type_elem()->que_suis_je() <<"s"<<finl;
}

/*! @brief Creation des bords du domaine extrude
 *
 */
void Extruder_en3::construire_bords(Domaine& dom, Faces& les_faces, int oldnbsom, int oldsz, const IntVect& num)
{
  IntTab& les_elems = dom.les_elems();
  // Les bords:
  for (auto &itr : dom.faces_bord())
    {
      Faces& les_faces_du_bord = itr.faces();
      construire_bord_lateral(les_faces_du_bord, les_faces, oldnbsom, num);
    }

  // Les raccords:
  for (auto &itr : dom.faces_raccord())
    {
      Faces& les_faces_du_bord = itr->faces();
      construire_bord_lateral(les_faces_du_bord, les_faces, oldnbsom, num);
    }

  // Devant
  if(nom_dvt_=="NULL")
    {
      Cerr << "We don't associate any boundary to the front of the domain " << dom.le_nom() << finl;
    }
  else
    {
      Bord& devant = dom.faces_bord().add(Bord());
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
      Bord& derriere = dom.faces_bord().add(Bord());
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

/*! @brief Creation d'un bord lateral
 *
 */
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

