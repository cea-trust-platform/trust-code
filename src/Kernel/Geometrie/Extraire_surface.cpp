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

#include <Extraire_surface.h>
#include <Domaine.h>
#include <Param.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Zone_VF.h>
#include <NettoieNoeuds.h>
#include <Parser_U.h>

Implemente_instanciable(Extraire_surface,"Extraire_surface",Interprete_geometrique_base);

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
Sortie& Extraire_surface::printOn(Sortie& os) const
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
Entree& Extraire_surface::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

// Description:
//    Fonction principale de l'interprete.
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Extraire_surface::interpreter_(Entree& is)
{
  Nom nom_pb;
  Nom nom_domaine_surfacique;
  Nom expr_elements("1"),expr_faces("1");
  int avec_les_bords;
  Noms noms_des_bords;
  Param param(que_suis_je());
  param.ajouter("domaine",&nom_domaine_surfacique,Param::REQUIRED);
  param.ajouter("probleme",&nom_pb,Param::REQUIRED);
  param.ajouter("condition_elements",&expr_elements);
  param.ajouter("condition_faces",&expr_faces);
  param.ajouter_flag("avec_les_bords",&avec_les_bords);
  param.ajouter("avec_certains_bords",&noms_des_bords);
  param.lire_avec_accolades_depuis(is);

  associer_domaine(nom_domaine_surfacique);
  Domaine& domaine_surfacique=domaine();

  if (domaine_surfacique.nb_som_tot()!=0)
    {
      Cerr << "Error!" << finl;
      Cerr <<"The domain " << domaine_surfacique.le_nom() << " can't be used by the Extraire_surface keyword." <<finl;
      Cerr <<"The domain for Extraire_surface keyword should be empty and created just before." << finl;
      exit();
    }

  // on recupere le pb
  if(! sub_type(Probleme_base, objet(nom_pb)))
    {
      Cerr << nom_pb << " is of type " << objet(nom_pb).que_suis_je() << finl;
      Cerr << "and not of type Probleme_base" << finl;
      exit();
    }
  Probleme_base& pb=ref_cast(Probleme_base, objet(nom_pb));
  const Zone_VF& zone_vf=ref_cast(Zone_VF,pb.equation(0).zone_dis().valeur());
  const Domaine& domaine_volumique = zone_vf.zone().domaine();

  extraire_surface(domaine_surfacique,domaine_volumique,nom_domaine_surfacique,zone_vf,expr_elements,expr_faces,avec_les_bords,noms_des_bords);

  return is;

}

void calcul_normal(const ArrOfDouble& origine,const ArrOfDouble& point1, const ArrOfDouble& point2,ArrOfDouble& normal);

// Extraction d'une ou plusieurs frontieres du domaine volumique selon certaines conditions
void Extraire_surface::extraire_surface(Domaine& domaine_surfacique,const Domaine& domaine_volumique, const Nom& nom_domaine_surfacique, const Zone_VF& zone_vf, const Nom& expr_elements,const Nom& expr_faces, int avec_les_bords, const Noms& noms_des_bords)
{
  domaine_surfacique.nommer(nom_domaine_surfacique);
  Parser_U condition_elements,condition_faces;
  condition_elements.setNbVar(dimension);
  condition_elements.addVar("x");
  condition_elements.addVar("y");
  if (dimension==3)
    condition_elements.addVar("z");
  condition_faces.setNbVar(dimension);
  condition_faces.addVar("x");
  condition_faces.addVar("y");
  if (dimension==3)
    condition_faces.addVar("z");

  condition_faces.setString(expr_faces);
  condition_faces.parseString();
  condition_elements.setString(expr_elements);
  condition_elements.parseString();

  // Copie des sommets
  domaine_surfacique.les_sommets()=domaine_volumique.les_sommets();
  const DoubleTab& coord=domaine_surfacique.les_sommets();
  Zone zone__;
  domaine_surfacique.add(zone__);
  Zone& zone=domaine_surfacique.zone(0);
  zone.nommer("NO_FACE");
  const Nom& type_elem=zone_vf.zone().type_elem().valeur().que_suis_je();

  if (dimension==3)
    {
      if (type_elem==Motcle("Tetraedre"))
        {
          zone.typer("Triangle");
        }
      else
        {
          if (type_elem==Motcle("Segment"))
            {
              zone.typer("Point");
            }
          else
            {
              if ((type_elem==Motcle("Hexaedre"))|| (type_elem==Motcle("Hexaedre_VEF")))
                {
                  zone.typer("Quadrangle");
                }
              else
                {
                  if (type_elem==Motcle("Polyedre"))
                    {
                      zone.typer("Polygone");
                    }
                  else
                    {
                      Cerr<<"WARNING "<<type_elem<< " not coded, use Quadrangle" <<finl;
                      zone.typer("Quadrangle");
                      //      exit();
                    }
                }
            }
        }
    }
  else
    {
      zone.typer("segment");
    }
  zone.associer_domaine(domaine_surfacique);

  const DoubleTab& xp =zone_vf.xp();

  int nb_elem=zone_vf.nb_elem();
  IntTab marq_elem;

  zone_vf.zone().creer_tableau_elements(marq_elem);
  std::string X("X"), Y("Y"), Z("Z");
  for (int elem=0; elem<nb_elem; elem++)
    {
      condition_elements.setVar(X,xp(elem,0));
      condition_elements.setVar(Y,xp(elem,1));
      if (dimension==3)
        condition_elements.setVar(Z,xp(elem,2));
      double res=condition_elements.eval();
      if (std::fabs(res)>1e-5)
        marq_elem(elem)=1;
      else
        marq_elem(elem)=0;

    }
  marq_elem.echange_espace_virtuel();
  const DoubleTab& xv =zone_vf.xv();

  int nb_faces=zone_vf.nb_faces();
  const IntTab& face_voisin=zone_vf.face_voisins();

  ArrOfInt marq(nb_faces);
  // on marque les joints
  int nbjoints=zone_vf.nb_joints();

  for(int njoint=0; njoint<nbjoints; njoint++)
    {
      const Joint& joint_temp = zone_vf.joint(njoint);
      int pe_voisin=joint_temp.PEvoisin();
      if (pe_voisin<me())
        {
          const IntTab& indices_faces_joint = joint_temp.joint_item(Joint::FACE).renum_items_communs();
          const int nbfaces = indices_faces_joint.dimension(0);
          for (int j = 0; j < nbfaces; j++)
            {
              int face_de_joint = indices_faces_joint(j, 1);
              marq[face_de_joint] = -1;
            }
        }
    }
  int nb_t=0;

  // on flage les face sde bords qui nous interesse
  ArrOfInt face_bord_int(nb_faces);
  if (avec_les_bords==1)
    face_bord_int=1;
  if (noms_des_bords.size()!=0)
    {
      if (avec_les_bords!=0)
        {
          Cerr<<" the option avec_les_bords is incompatible with the option avec_certains_bords"<<finl;
          exit();
        }
      for (int b=0; b<noms_des_bords.size(); b++)
        {
          const Frontiere& fr=zone_vf.frontiere_dis(zone_vf.rang_frontiere(noms_des_bords[b])).frontiere();
          int deb=fr.num_premiere_face();
          int fin=deb+fr.nb_faces();
          for (int f=deb; f<fin; f++)
            face_bord_int[f]=1;
        }

    }

  // on marque toutes les faces que l'on veut mettre dans le domaine
  for (int fac=0; fac<nb_faces; fac++)
    {
      int elem0=face_voisin(fac,0);
      int elem1=face_voisin(fac,1);
      int val0=-1;
      if (elem0!=-1) val0=marq_elem(elem0);
      int val1=-1;
      if (elem1!=-1) val1=marq_elem(elem1);

      if (val0!=val1)
        {
          if (((val0*val1==0)&&(val0+val1==1))||((val0==1)&&(face_bord_int[fac]==1))||((val1==1)&&(face_bord_int[fac]==1)))
            //if (zone_test.chercher_elements(xv(fac,0),xv(fac,1),xv(fac,2))==0)
            {
              condition_faces.setVar(X,xv(fac,0));
              condition_faces.setVar(Y,xv(fac,1));
              if (dimension==3)
                condition_faces.setVar(Z,xv(fac,2));
              double res=condition_faces.eval();
              if (std::fabs(res)>1e-5)
                if (marq[fac]!=-1)  // pas un joint, ou on est le proprietaire
                  {
                    marq[fac]=1;
                    nb_t++;
                  }
            }
        }
    }

  ArrOfDouble point0b(3),point1b(3),point2b(3);
  Cerr<<"Number of elements of the new domain : "<<nb_t<<finl;
  IntTab& les_elems=zone.les_elems();

  const IntTab& face_sommets=zone_vf.face_sommets();
  int nb_sommet_face=face_sommets.dimension(1);
  les_elems.resize(nb_t,nb_sommet_face);
  int nb=0;
  ArrOfDouble normal(dimension);
  ArrOfDouble normal_b(3);
  for (int fac=0; fac<nb_faces; fac++)
    if (marq[fac]==1)
      {
        int el1=face_voisin(fac,0);
        if (el1==-1)  el1=face_voisin(fac,1);
        if (marq_elem(el1)!=1)
          el1=face_voisin(fac,1);
        for (int i=0; i<dimension; i++)
          normal[i]=xv(fac,i)-xp(el1,i);
        for (int s=0; s<nb_sommet_face; s++)
          les_elems(nb,s)=face_sommets(fac,s);
        // on calcule la normale
        if (nb_sommet_face>1)
          {
            if (dimension==3)
              {
                for (int i=0; i<3; i++)
                  {
                    point0b[i]=coord(les_elems(nb,0),i);
                    point1b[i]=coord(les_elems(nb,1),i);
                    point2b[i]=coord(les_elems(nb,2),i);
                  }
                calcul_normal(point0b,point1b,point2b,normal_b);
                if (dotproduct_array(normal,normal_b)<0)
                  {
                    // si normal a l'envers on inverse les deux sommets
                    les_elems(nb,1)=face_sommets(fac,2);
                    les_elems(nb,2)=face_sommets(fac,1);
                  }
              }
            else
              {
                for (int i=0; i<nb_sommet_face; i++)
                  {

                    point0b[i]=coord(les_elems(nb,1),i)-coord(les_elems(nb,0),i);
                  }
                double produit=normal[0]*point0b[1]-normal[1]*point0b[0];
                if (produit<0)
                  {
                    // si normal a l'envers on inverse les deux sommets
                    les_elems(nb,0)=face_sommets(fac,1);
                    les_elems(nb,1)=face_sommets(fac,0);
                  }
              }
          }
        nb++;
      }

  assert(nb==nb_t);
  NettoieNoeuds::nettoie(domaine_surfacique);
}











