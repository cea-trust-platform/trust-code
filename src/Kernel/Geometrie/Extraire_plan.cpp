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

#include <Extraire_plan.h>
#include <Domaine.h>
#include <Param.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Zone_VF.h>
#include <NettoieNoeuds.h>
#include <EChaine.h>
#include <SChaine.h>
#include <Extraire_surface.h>

Implemente_instanciable(Extraire_plan,"Extraire_plan",Interprete_geometrique_base);

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
Sortie& Extraire_plan::printOn(Sortie& os) const
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
Entree& Extraire_plan::readOn(Entree& is)
{
  return Interprete::readOn(is);
}


void calcul_normal(const ArrOfDouble& origine,const ArrOfDouble& point1, const ArrOfDouble& point2,ArrOfDouble& normal)
{
  normal[0]=(point1[1]-origine[1])*(point2[2]-origine[2])-((point2[1]-origine[1])*(point1[2]-origine[2]));

  normal[1]=(point1[2]-origine[2])*(point2[0]-origine[0])-((point2[2]-origine[2])*(point1[0]-origine[0]));
  normal[2]=(point1[0]-origine[0])*(point2[1]-origine[1])-((point2[0]-origine[0])*(point1[1]-origine[1]));
}

void calcul_normal_norme(const ArrOfDouble& org,const ArrOfDouble& point1, const ArrOfDouble& point2,ArrOfDouble& normal)
{
  calcul_normal(org, point1,  point2,normal);
  normal/=norme_array(normal);
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
Entree& Extraire_plan::interpreter_(Entree& is)
{
  Nom nom_pb;
  Nom nom_dom;
  ArrOfDouble origine,point1,point2,point3;
  int triangle;
  double epaisseur;
  Param param(que_suis_je());
  param.ajouter("domaine",&nom_dom,Param::REQUIRED);
  param.ajouter("probleme",&nom_pb,Param::REQUIRED);
  param.ajouter("origine",&origine,Param::REQUIRED);
  param.ajouter("point1",&point1,Param::REQUIRED);
  param.ajouter("point2",&point2,Param::REQUIRED);
  param.ajouter("point3",&point3);
  param.ajouter_flag("triangle",&triangle);
  param.ajouter("epaisseur",&epaisseur,Param::REQUIRED);

  int via_extraire_surface=0, inverse_condition_element;
  param.ajouter_flag("via_extraire_surface",&via_extraire_surface);
  param.ajouter_flag("inverse_condition_element",&inverse_condition_element);

  Noms bords;

  param.ajouter("avec_certains_bords_pour_extraire_surface",&bords);

  param.lire_avec_accolades_depuis(is);

  associer_domaine(nom_dom);

  if (point3.size_array()==0)
    {
      point3.resize_array(3);
      for (int dir=0; dir<3; dir++)
        if (triangle)
          point3[dir]=(point1[dir]+point2[dir])/2.;
        else
          point3[dir]=point1[dir]+point2[dir]-origine[dir];
    }


  if (via_extraire_surface)
    {

      // determination de la normale
      ArrOfDouble normal(3);
      calcul_normal_norme(origine,point1,point2,normal);

      double dx=0;
      for (int dir=0; dir<3; dir++)
        dx+=normal[dir]*origine[dir];
      Noms axes(3);
      axes[0]="x";
      axes[1]="y";
      axes[2]="z";

      SChaine out;
      out << " { domaine "<<nom_dom <<" probleme "<<nom_pb <<finl;
      out<<" condition_elements (" <<-dx ;
      for (int dir=0; dir<3; dir++)
        if (normal[dir])
          out<<"+"<<axes[dir]<<"*("<<normal[dir]<<")";
      if (inverse_condition_element)
        out<<")_ge_0"<<finl;
      else
        out<<")_le_0"<<finl;
      out<<" condition_faces (";
      for (int ori=0; ori<2; ori++)

        {
          ArrOfDouble& A=(ori==0?origine:point3);
          ArrOfDouble& D=(ori!=0?origine:point3);
          ArrOfDouble normal2(3),prov(3),zero(3);
          for (int te=0; te<2; te++)
            {
              ArrOfDouble& B=(te==0?point1:point2);
              //ArrOfDouble& C=(ori!=0?point1:point2);
              prov=normal;
              prov+=A;
              calcul_normal_norme(A,B,prov,normal2);
              //prov=REF;
              //prov-=A;
              double dxbis=0;
              double ref=0;
              out<<"((";
              for (int dir=0; dir<3; dir++)
                {
                  dxbis+=normal2[dir]*D[dir];
                  ref+=normal2[dir]*A[dir];
                  if (normal2[dir])
                    out << axes[dir]<<"*("<< normal2[dir]<<")+";
                }
              out<<"("<<-ref<<"))";
              //Cout<<"INFO " << dx-ref<<finl;
              if ((dxbis-ref)>0)
                out <<"_ge_0)*";
              else
                out <<"_le_0)*";
            }


        }
      out<<"1)"<<finl;
      //out<<" avec_les_bords "<<finl;
      if (bords.size())
        out<<"avec_certains_bords " <<bords<<finl;
      out<<"}" << finl;
      Cout<<" Creation of the plan through Extraire_surface "<< out.get_str()<<finl;;
      EChaine in(out.get_str());
      Extraire_surface extraction;
      extraction.interpreter(in);
      // abort();
      return is;
    }

  Domaine& dom=domaine();
  // on recupere le pb
  if(! sub_type(Probleme_base, objet(nom_pb)))
    {
      Cerr << nom_pb << " is of type " << objet(nom_pb).que_suis_je() << finl;
      Cerr << "and not of type Probleme_base" << finl;
      exit();
    }
  Probleme_base& pb=ref_cast(Probleme_base, objet(nom_pb));
  const Zone_VF& zone_vf=ref_cast(Zone_VF,pb.equation(0).zone_dis().valeur());
  dom.les_sommets()=zone_vf.zone().domaine().les_sommets();
  const DoubleTab& coord=dom.les_sommets();
  Zone zone__;
  dom.add(zone__);
  Zone& zone=dom.zone(0);
  zone.nommer(dom.le_nom());
  const Nom& type_elem=zone_vf.zone().type_elem().valeur().que_suis_je();
  if (type_elem==Motcle("Tetraedre"))
    zone.typer("Triangle");
  else
    {
      Cerr<<que_suis_je()<<"  not coded for this type of elements "<<finl;
      exit();
    }
  zone.associer_domaine(dom);
  // IntTab& les_elems=zone.les_elems();

  // creation d'une zone pipo pour pouvoir chercher les faces
  Domaine test;
  ArrOfDouble normal(3);
  {
    Zone prov;
    test.add(prov);
    Zone& zone_test=test.zone(0);
    zone.nommer("NO_FACE");
    if (triangle)
      zone_test.typer("Prisme");
    else
      zone_test.typer("Hexaedre_vef");
    zone_test.associer_domaine(test);

    DoubleTab& somm_hexa=test.les_sommets();
    int nb_som=4;
    if (triangle) nb_som=3;
    somm_hexa.resize(nb_som*2,3);

    for (int dir=0; dir<3; dir++)
      {
        somm_hexa(0,dir)=origine[dir];
        somm_hexa(1,dir)=point1[dir];
        somm_hexa(2,dir)=point2[dir];
        somm_hexa(3,dir)=point3[dir];
        for (int t=0; t<nb_som; t++)
          somm_hexa(t+nb_som,dir)=somm_hexa(t,dir);
      }
    // il faut maintenant epaissir l'hexa dans la direction normale

    calcul_normal_norme(origine,point1,point2,normal);
    double d_epaisseur=epaisseur/2.;
    for (int t=0; t<nb_som; t++)
      for (int dir=0; dir<3; dir++)
        {
          somm_hexa(t,dir)=somm_hexa(t,dir)-d_epaisseur*normal[dir];
          somm_hexa(nb_som+t,dir)=somm_hexa(nb_som+t,dir)+d_epaisseur*normal[dir];
        }
    IntTab& elem_test=zone_test.les_elems();
    elem_test.resize(1,nb_som*2);
    for (int s=0; s<nb_som*2; s++) elem_test(0,s)=s;
    /*
      SFichier es("hex.geom");
      es<<test;
    */
  }
  Zone& zone_test=test.zone(0);
  const DoubleTab& xv =zone_vf.xv();

  int nbfaces=zone_vf.nb_faces();


  ArrOfInt marq(nbfaces);
  // on marque les joints
  //const Joints& joints=zone_vf.face_joints();
  int nbjoints=zone_vf.nb_joints();

  for(int njoint=0; njoint<nbjoints; njoint++)
    {
      const Joint& joint_temp = zone_vf.joint(njoint);
      int pe_voisin=joint_temp.PEvoisin();
      if (pe_voisin<me())
        {
          const IntTab& indices_faces_joint = joint_temp.joint_item(Joint::FACE).renum_items_communs();
          const int nb_faces_j = indices_faces_joint.dimension(0);
          for (int j = 0; j < nb_faces_j; j++)
            {
              int face_de_joint = indices_faces_joint(j, 1);
              marq[face_de_joint] = -1;
            }
        }
    }
  int nb_t=0;

  for (int fac=0; fac<nbfaces; fac++)
    {
      if (zone_test.chercher_elements(xv(fac,0),xv(fac,1),xv(fac,2))==0)
        if (marq[fac]!=-1)
          {
            // tester si item_commun....
            marq[fac]=1;
            nb_t++;
          }
    }
  ArrOfDouble point0b(3),point1b(3),point2b(3);
  Cerr<<"Number of elements of the new domain "<<nb_t<<finl;
  IntTab& les_elems=zone.les_elems();
  les_elems.resize(nb_t,3);
  const IntTab& face_sommets=zone_vf.face_sommets();
  int nb=0;
  for (int fac=0; fac<nbfaces; fac++)
    if (marq[fac]==1)
      {
        Cerr<<fac <<" ";
        for (int s=0; s<3; s++)
          les_elems(nb,s)=face_sommets(fac,s);
        // on calcule la normale
        ArrOfDouble normal_b(3);
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
        nb++;
      }
  Cerr<<finl;;
  assert(nb==nb_t);
  NettoieNoeuds::nettoie(dom);

  return is;

}





