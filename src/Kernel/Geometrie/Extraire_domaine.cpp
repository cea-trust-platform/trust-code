/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Extraire_domaine.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#include <Extraire_domaine.h>
#include <Domaine.h>
#include <Param.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Zone_VF.h>
#include <NettoieNoeuds.h>
#include <Parser_U.h>
#include <Scatter.h>
#include <Sous_Zone.h>

Implemente_instanciable(Extraire_domaine,"Extraire_Domaine",Interprete_geometrique_base);

Sortie& Extraire_domaine::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}
Entree& Extraire_domaine::readOn(Entree& is)
{
  return Interprete::readOn(is);
}







Entree& Extraire_domaine::interpreter_(Entree& is)
{
  Nom nom_pb;
  Nom nom_dom;
  Nom nom_sous_zone;
  Parser_U condition_elements;
  Nom expr_elements("1");
  condition_elements.setNbVar(dimension);
  condition_elements.addVar("x");
  condition_elements.addVar("y");
  if (dimension==3)
    condition_elements.addVar("z");

  ArrOfDouble origine,point1,point2,point3;
  // int avec_les_bords;
  Param param(que_suis_je());
  param.ajouter("domaine",&nom_dom,Param::REQUIRED);
  param.ajouter("probleme",&nom_pb,Param::REQUIRED);
  param.ajouter("condition_elements",&expr_elements);
  param.ajouter("sous_zone",&nom_sous_zone);
  //param.ajouter("condition_faces",&expr_faces);
  // param.ajouter_flag("avec_les_bords",&avec_les_bords);
  param.lire_avec_accolades_depuis(is);

  condition_elements.setString(expr_elements);
  condition_elements.parseString();

  associer_domaine(nom_dom);
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
  //const DoubleTab& coord=dom.les_sommets();
  Zone zone__;
  dom.add(zone__);
  Zone& zone=dom.zone(0);
  zone.nommer(dom.le_nom());
  zone.typer(zone_vf.zone().type_elem().valeur().que_suis_je());
  zone.associer_domaine(dom);
  // IntTab& les_elems=zone.les_elems();

  //

  const DoubleTab& xp =zone_vf.xp();

  int nb_elem=zone_vf.nb_elem();
  IntTab marq_elem;

  // on marque les elts qui nous interessent
  zone_vf.zone().creer_tableau_elements(marq_elem);
  int nb_elem_m=0;
  if (nom_sous_zone== Nom())
    {
      for (int elem=0; elem<nb_elem; elem++)
        {
          condition_elements.setVar("x",xp(elem,0));
          condition_elements.setVar("y",xp(elem,1));
          if (dimension==3)
            condition_elements.setVar("z",xp(elem,2));
          double res=condition_elements.eval();
          if (std::fabs(res)>1e-5)
            {
              marq_elem(elem)=1;
              nb_elem_m++;
            }
          else
            marq_elem(elem)=0;

        }
    }
  else
    {
      const Sous_Zone& ssz= ref_cast(Sous_Zone,objet(nom_sous_zone));
      int npol=ssz.nb_elem_tot();
      for (int i=0; i<npol; i++)
        {
          if (ssz(i)<nb_elem)
            {
              marq_elem(ssz(i))=1;
              nb_elem_m++;
            }
        }
    }
  // Attention grosse ruse on echange pas les espaces virtuels pour que le joint devienne un bord
  //marq_elem.echange_espace_virtuel();
  // const DoubleTab& xv =zone_vf.xv();

  int nb_faces=zone_vf.nb_faces();
  const IntTab& face_voisin=zone_vf.face_voisins();

  ArrOfInt marq(nb_faces);
  // on marque les joints
  //const Joints& joints=zone_vf.face_joints();
  // int nbjoints=zone_vf.nb_joints();

  //  for(int njoint=0; njoint<nbjoints; njoint++)
  //    {
  //      const Joint& joint_temp = zone_vf.joint(njoint);
  //      int pe_voisin=joint_temp.PEvoisin();
  //      if (pe_voisin<me())
  //        {
  //          const IntTab & indices_faces_joint = joint_temp.joint_item(Joint::FACE).renum_items_communs();
  //          const int nb_faces = indices_faces_joint.dimension(0);
  //          for (int j = 0; j < nb_faces; j++) {
  //            int face_de_joint = indices_faces_joint(j, 1);
  //            marq(face_de_joint) = -1;
  //          }
  //        }
  //    }

  // on cherche les faces au bord du domaine (joint compris)
  int nb_t=0;

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
          if ((val0==1)||(val1==1))
            {
              if (marq(fac)!=-1) //pas un joint
                {
                  marq(fac)=1;
                  nb_t++;
                }
            }
        }
    }

  Cerr<<me()<<" nb_elem_m "<<nb_elem_m<<finl;
  Cerr<<me()<<" nb_t "<<nb_t<<finl;
  IntTab& les_elems=zone.les_elems();
  const IntTab& les_elems_old=zone_vf.zone().les_elems();
  int nb_som_elem=les_elems_old.dimension(1);
  les_elems.resize(nb_elem_m,nb_som_elem);
  const IntTab& face_sommets=zone_vf.face_sommets();
  int nb=0;
  for (int ele=0; ele<nb_elem; ele++)
    {
      if (marq_elem(ele)==1)
        {
          for (int k=0; k<nb_som_elem; k++)
            les_elems(nb,k)=les_elems_old(ele,k);
          nb++;
        }
    }
  assert(nb==nb_elem_m);
  Bord faces;
  faces.nommer("Bord");
  faces.typer_faces(zone_vf.zone().type_elem().type_face());
  Faces& les_faces=faces.faces();
  // const IntTab& faces_sommets=zone_vf.faces_sommets();
  int nb_som_face=face_sommets.dimension(1);
  IntTab& indfaces=les_faces.les_sommets();
  indfaces.resize(nb_t,nb_som_face);
  IntTab& facesv=les_faces.voisins();
  facesv.resize(nb_t,2);
  facesv=-1;


  nb=0;
  for (int fac=0; fac<nb_faces; fac++)
    if (marq(fac)==1)
      {
        for (int s=0; s<nb_som_face; s++)
          indfaces(nb,s)=face_sommets(fac,s);
        // on calcule la normale
        nb++;
      }
  Cerr<<finl;;
  zone.faces_bord().add(faces);
  zone.faces_bord().associer_zone(zone);
  zone.type_elem().associer_zone(zone);
  zone.fixer_premieres_faces_frontiere();



  assert(nb==nb_t);
  NettoieNoeuds::nettoie(dom);

  return is;

}
