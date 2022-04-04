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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Hexaedre_VEF.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/25
//
//////////////////////////////////////////////////////////////////////////////

#include <Hexaedre_VEF.h>
#include <Domaine.h>

Implemente_instanciable(Hexaedre_VEF,"Hexaedre_VEF",Elem_geom_base);


// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Hexaedre_VEF::printOn(Sortie& s ) const
{
  return s;
}


// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification: le flot d'entree
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Hexaedre_VEF::readOn(Entree& s )
{
  return s;
}




// Description:
//    Renvoie le nom LML d'un Hexaedre_VEF = "HEXA8".
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: toujours egal a "HEXA8"
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Nom& Hexaedre_VEF::nom_lml() const
{
  static Nom nom="VOXEL8";
  return nom;
}

// Description:
//    teste si une position pos est un tetraedre de sommets som(i)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: toujours egal a "HEXA8"
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
static int entre_faces(const Zone& zone,const Domaine& dom,const ArrOfDouble& pos, int Asom0_, int Asom1_, int Asom2_, int Bsom0_, int Bsom1_, int Bsom2_)
{
  double prodA,prodB;
  const DoubleTab& coord=dom.les_sommets();

  // pour A
  double vecA1[3], vecA2[3], AP[3];
  double normaleA[3];
  for (int i=0; i<3; i++)
    {
      double ref=coord(Asom0_,i);
      vecA1[i]=coord(Asom1_,i)-ref;
      vecA2[i]=coord(Asom2_,i)-ref;
      AP[i]=pos[i]-ref;
    }
  normaleA[0]=(vecA1[1]*vecA2[2]-vecA1[2]*vecA2[1]);
  normaleA[1]=(vecA1[2]*vecA2[0]-vecA1[0]*vecA2[2]);
  normaleA[2]=(vecA1[0]*vecA2[1]-vecA1[1]*vecA2[0]);
  prodA=0;
  for (int i=0; i<3; i++)
    prodA+=normaleA[i]*AP[i];

  // pour B
  double vecB1[3], vecB2[3], BP[3];
  double normaleB[3];
  for (int i=0; i<3; i++)
    {
      double ref=coord(Bsom0_,i);
      vecB1[i]=coord(Bsom1_,i)-ref;
      vecB2[i]=coord(Bsom2_,i)-ref;
      BP[i]=pos[i]-ref;
    }
  normaleB[0]=(vecB1[1]*vecB2[2]-vecB1[2]*vecB2[1]);
  normaleB[1]=(vecB1[2]*vecB2[0]-vecB1[0]*vecB2[2]);
  normaleB[2]=(vecB1[0]*vecB2[1]-vecB1[1]*vecB2[0]);
  prodB=0;
  for (int i=0; i<3; i++)
    prodB+=normaleB[i]*BP[i];

  if (prodA*prodB<=0)
    return 1;
  else
    {
      // on regarde si on n'est pas limite
      double rap=prodA/prodB;
      if (rap>1) rap=prodB/prodA;
      if (rap< Objet_U::precision_geom) return 1;
    }
  return 0;
}
// Description:
//    teste si une position pos est un tetraedre de sommets som(i)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: toujours egal a "HEXA8"
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
static int contient_Tetra(const Zone& zone,const Domaine& dom,const ArrOfDouble& pos, int som0_, int som1_, int som2_, int som3_, int aff)
{
  double prod1,prod2;
  int som0, som1, som2, som3;
  som0 = som0_;
  som1 = som1_;
  som2 = som2_;
  som3 = som3_;
  double vec0[3], vec1[3], vec2[3];
  double vecx0[3], vecx1[3], vecx2[3];

  if (aff)
    {
      Cerr<<"Test contient_Tetra nodes="<<som0_<<" "<<som1_<<" "<<som2_<<" "<<som3_<<finl;
      Cerr<<"  position="<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<finl;
    }

  if( ( est_egal(dom.coord(som0,0),pos[0]) && est_egal(dom.coord(som0,1),pos[1]) && est_egal(dom.coord(som0,2),pos[2]) )
      || (est_egal(dom.coord(som1,0),pos[0]) && est_egal(dom.coord(som1,1),pos[1]) && est_egal(dom.coord(som1,2),pos[2]))
      || (est_egal(dom.coord(som2,0),pos[0]) && est_egal(dom.coord(som2,1),pos[1]) && est_egal(dom.coord(som2,2),pos[2]))
      || (est_egal(dom.coord(som3,0),pos[0]) && est_egal(dom.coord(som3,1),pos[1]) && est_egal(dom.coord(som3,2),pos[2])) )
    return 1;

  for (int j=0; j<4; j++)
    {
      switch(j)
        {
        case 0 :
          som0 = som0_;
          som1 = som1_;
          som2 = som2_;
          som3 = som3_;
          break;
        case 1 :
          som0 = som1_;
          som1 = som2_;
          som2 = som3_;
          som3 = som0_;
          break;
        case 2 :
          som0 = som2_;
          som1 = som3_;
          som2 = som0_;
          som3 = som1_;
          break;
        case 3 :
          som0 = som3_;
          som1 = som0_;
          som2 = som1_;
          som3 = som2_;
          break;
        }

      //algo : les produits mixtes (som0, som1, som2,som3) et (pos, som1, som2,som3) doivent a voir le meme signe
      //construction des vecteurs
      vec0[0] = dom.coord(som1,0) - dom.coord(som0,0);
      vec0[1] = dom.coord(som1,1) - dom.coord(som0,1);
      vec0[2] = dom.coord(som1,2) - dom.coord(som0,2);
      vec1[0] = dom.coord(som2,0) - dom.coord(som0,0);
      vec1[1] = dom.coord(som2,1) - dom.coord(som0,1);
      vec1[2] = dom.coord(som2,2) - dom.coord(som0,2);
      vec2[0] = dom.coord(som3,0) - dom.coord(som0,0);
      vec2[1] = dom.coord(som3,1) - dom.coord(som0,1);
      vec2[2] = dom.coord(som3,2) - dom.coord(som0,2);
      vecx0[0] = dom.coord(som1,0) - pos[0];
      vecx0[1] = dom.coord(som1,1) - pos[1];
      vecx0[2] = dom.coord(som1,2) - pos[2];
      vecx1[0] = dom.coord(som2,0) - pos[0];
      vecx1[1] = dom.coord(som2,1) - pos[1];
      vecx1[2] = dom.coord(som2,2) - pos[2];
      vecx2[0] = dom.coord(som3,0) - pos[0];
      vecx2[1] = dom.coord(som3,1) - pos[1];
      vecx2[2] = dom.coord(som3,2) - pos[2];
      prod1 = vec0[0]*vec1[1]*vec2[2] + vec0[1]*vec1[2]*vec2[0] + vec0[2]*vec1[0]*vec2[1]
              - vec0[0]*vec1[2]*vec2[1] - vec0[1]*vec1[0]*vec2[2] - vec0[2]*vec1[1]*vec2[0];
      prod2 = vecx0[0]*vecx1[1]*vecx2[2] + vecx0[1]*vecx1[2]*vecx2[0] + vecx0[2]*vecx1[0]*vecx2[1]
              - vecx0[0]*vecx1[2]*vecx2[1] - vecx0[1]*vecx1[0]*vecx2[2] - vecx0[2]*vecx1[1]*vecx2[0];

      if (aff)
        {
          Cerr<<"  test "<<som0<<" "<<som1<<" "<<som2<<" "<<som3<<finl;
          Cerr<<"  node0="<<som0<<" : "<<dom.coord(som0,0)<<"  "<<dom.coord(som0,1)<<"  "<<dom.coord(som0,2)<<finl;
          Cerr<<"  node1="<<som1<<" : "<<dom.coord(som1,0)<<"  "<<dom.coord(som1,1)<<"  "<<dom.coord(som1,2)<<finl;
          Cerr<<"  node2="<<som2<<" : "<<dom.coord(som2,0)<<"  "<<dom.coord(som2,1)<<"  "<<dom.coord(som2,2)<<finl;
          Cerr<<"  node3="<<som3<<" : "<<dom.coord(som3,0)<<"  "<<dom.coord(som3,1)<<"  "<<dom.coord(som3,2)<<finl;
          Cerr<<"  position : "<<pos[0]<<"  "<<pos[1]<<"  "<<pos[2]<<finl;
          Cerr<<"      prod1="<<prod1<<" prod2="<<prod2<<finl;
        }

      if (std::fabs(prod1)<Objet_U::precision_geom)
        {
          double norm_v0=0,norm_v1=0,norm_v2=0;
          for (int ii=0; ii<3; ii++)
            {
              norm_v0+=vec0[ii]*vec0[ii];
              norm_v1+=vec1[ii]*vec1[ii];
              norm_v2+=vec2[ii]*vec2[ii];
            }
          if (std::fabs(prod1)<Objet_U::precision_geom*sqrt(norm_v0*norm_v1*norm_v2))
            {
              Cerr<<"Test contient_Tetra ERROR : coplanar nodes"<<finl;
              Cerr<<"  node0="<<som0<<" : "<<dom.coord(som0,0)<<"  "<<dom.coord(som0,1)<<"  "<<dom.coord(som0,2)<<finl;
              Cerr<<"  node1="<<som1<<" : "<<dom.coord(som1,0)<<"  "<<dom.coord(som1,1)<<"  "<<dom.coord(som1,2)<<finl;
              Cerr<<"  node2="<<som2<<" : "<<dom.coord(som2,0)<<"  "<<dom.coord(som2,1)<<"  "<<dom.coord(som2,2)<<finl;
              Cerr<<"  node3="<<som3<<" : "<<dom.coord(som3,0)<<"  "<<dom.coord(som3,1)<<"  "<<dom.coord(som3,2)<<finl;
              Cerr<<"  position : "<<pos[0]<<"  "<<pos[1]<<"  "<<pos[2]<<finl;
              Process::exit();
            }
        }

      if (prod1*prod2<0 && std::fabs(prod2)>std::fabs(prod1)*Objet_U::precision_geom)
        {
          if (aff)
            {
              Cerr<<"  CONTAINS=0"<<finl;
            }
          return 0;
        }
    }
  if (aff)
    {
      Cerr<<"  CONTAINS=1"<<finl;
    }
  return 1;
}

// Description:
//    Renvoie 1 si l'element ielem de la zone associee a
//              l'element geometrique contient le point
//              de coordonnees specifiees par le parametre "pos".
//    Renvoie 0 sinon.
// Precondition:
// Parametre: DoubleVect& pos
//    Signification: coordonnees du point que l'on
//                   cherche a localiser
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: int ielem
//    Signification: le numero de l'element de la zone
//                   dans lequel on cherche le point.
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si le point de coordonnees specifiees
//                   appartient a l'element ielem
//                   0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Hexaedre_VEF::contient(const ArrOfDouble& pos, int element ) const
{
  assert(pos.size_array()==3);
  const Zone& zone=ma_zone.valeur();
  const Domaine& dom=zone.domaine();
  int som0 = zone.sommet_elem(element,0);
  int som1 = zone.sommet_elem(element,1);
  int som2 = zone.sommet_elem(element,2);
  int som3 = zone.sommet_elem(element,3);
  int som4 = zone.sommet_elem(element,4);
  int som5 = zone.sommet_elem(element,5);
  int som6 = zone.sommet_elem(element,6);
  int som7 = zone.sommet_elem(element,7);

  // GF on teste si le point est boen entre les 2 faces
  if (entre_faces(zone,dom,pos,som0,som1,som2,som4,som5,som6))
    if (entre_faces(zone,dom,pos,som0,som1,som4,som2,som3,som6))

      if (entre_faces(zone,dom,pos,som0,som2,som4,som1,som3,som5))
        return 1;
  return 0;
  // PQ : 04/03 : optimisation
  int aff = 0;
  if (element==-701)
    {
      aff = 1;
    }
  if (contient_Tetra(zone, dom, pos, som0,som1,som2,som4,aff) ||
      contient_Tetra(zone, dom, pos, som1,som2,som3,som6,aff) ||
      contient_Tetra(zone, dom, pos, som1,som2,som4,som6,aff) ||
      contient_Tetra(zone, dom, pos, som3,som5,som6,som7,aff) ||
      contient_Tetra(zone, dom, pos, som1,som4,som5,som6,aff) ||
      contient_Tetra(zone, dom, pos, som1,som3,som5,som6,aff))
    {
      return 1;
    }

  return 0;

}


// Description:
//    Renvoie 1 si les sommets specifies par le parametre "pos"
//    sont les sommets de l'element "element" de la zone associee a
//    l'element geometrique.
// Precondition:
// Parametre: IntVect& pos
//    Signification: les numeros des sommets a comparer
//                   avec ceux de l'elements "element"
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: int element
//    Signification: le numero de l'element de la zone
//                   dont on veut comparer les sommets
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si les sommets passes en parametre
//                   sont ceux de l'element specifie, 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Hexaedre_VEF::contient(const ArrOfInt& som, int element ) const
{
  const Zone& zone=ma_zone.valeur();
  if((zone.sommet_elem(element,0)==som[0])&&
      (zone.sommet_elem(element,1)==som[1])&&
      (zone.sommet_elem(element,2)==som[2])&&
      (zone.sommet_elem(element,3)==som[3])&&
      (zone.sommet_elem(element,4)==som[4])&&
      (zone.sommet_elem(element,5)==som[5])&&
      (zone.sommet_elem(element,6)==som[6])&&
      (zone.sommet_elem(element,7)==som[7]))
    return 1;
  else
    return 0;
}

// Description:
//    Calcule les volumes des elements de la zone associee.
// Precondition:
// Parametre: DoubleVect& volumes
//    Signification: le vecteur contenant les valeurs  des
//                   des volumes des elements de la zone
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Hexaedre_VEF::calculer_volumes(DoubleVect& volumes) const
{
  const Zone& zone=ma_zone.valeur();
  const Domaine& dom=zone.domaine();
  IntTab face_sommet_global;
  face_sommet_global.resize(6,4);
  double x0,y0,z0;
  double x1,y1,z1;
  double x2,y2,z2;
  double x3,y3,z3;
  double volume;
  double volume_global;
  int s1,s2,s3,s4;
  int som0,som1,som2,som3,som4,som5,som6,som7;

  volume_global =0;

  int size_tot = zone.nb_elem_tot();
  assert(volumes.size_totale()==size_tot);
  for (int num_poly=0; num_poly<size_tot; num_poly++)
    {
      // on initialise le volume
      // volumes[num_poly] =0;
      // pour chaque elem on le decoupe en tetra.
      // On calcul le volume par somme des volumes des tetras.

      // on definit les sommets de l'elem
      // on recupere le centre de l'element.

      som0 = zone.sommet_elem(num_poly,0);
      som1 = zone.sommet_elem(num_poly,1);
      som2 = zone.sommet_elem(num_poly,2);
      som3 = zone.sommet_elem(num_poly,3);
      som4 = zone.sommet_elem(num_poly,4);
      som5 = zone.sommet_elem(num_poly,5);
      som6 = zone.sommet_elem(num_poly,6);
      som7 = zone.sommet_elem(num_poly,7);

      //  Cerr << " som0 " << som0 << " som1 " << som1 << " som2 " << som2 << " som3 " << som3 <<  finl;
      //  Cerr << " som4 " << som4 << " som5 " << som5 << " som6 " << som6 << " som7 " << som7 <<   finl;


      // on construit le tableau de travail face_sommet_global
      // les faces seront ordonnee suivant Xmin, Xmax, Ymin, Ymax, Zmin,Zmax
      // toujours en gardant la numerotation de TRUST
      face_sommet_global(0,0) =som0;
      face_sommet_global(0,1) =som1;
      face_sommet_global(0,2) =som2;
      face_sommet_global(0,3) =som3;

      face_sommet_global(1,0) =som4;
      face_sommet_global(1,1) =som5;
      face_sommet_global(1,2) =som6;
      face_sommet_global(1,3) =som7;

      face_sommet_global(2,0) =som0;
      face_sommet_global(2,1) =som4;
      face_sommet_global(2,2) =som2;
      face_sommet_global(2,3) =som6;

      face_sommet_global(3,0) =som1;
      face_sommet_global(3,1) =som5;
      face_sommet_global(3,2) =som3;
      face_sommet_global(3,3) =som7;

      face_sommet_global(4,0) =som0;
      face_sommet_global(4,1) =som4;
      face_sommet_global(4,2) =som1;
      face_sommet_global(4,3) =som5;

      face_sommet_global(5,0) =som2;
      face_sommet_global(5,1) =som6;
      face_sommet_global(5,2) =som3;
      face_sommet_global(5,3) =som7;

      // le sommet 0 sera toujours le centre de gravite
      x0 = ( dom.coord(som0,0) + dom.coord(som1,0) + dom.coord(som2,0) + dom.coord(som3,0) + dom.coord(som4,0) + dom.coord(som5,0) + dom.coord(som6,0) + dom.coord(som7,0) )*0.125;
      y0 = ( dom.coord(som0,1) + dom.coord(som1,1) + dom.coord(som2,1) + dom.coord(som3,1) + dom.coord(som4,1) + dom.coord(som5,1) + dom.coord(som6,1) + dom.coord(som7,1) )*0.125;
      z0 = ( dom.coord(som0,2) + dom.coord(som1,2) + dom.coord(som2,2) + dom.coord(som3,2) + dom.coord(som4,2) + dom.coord(som5,2) + dom.coord(som6,2) + dom.coord(som7,2) )*0.125;

      //  Cerr << "le num poly traite " << num_poly << finl;
      //  Cerr << "le centre de gravite " << x0 << " " << y0 << " " << z0 << finl;

      volume =0;


      // On decoupe en tetra.
      // pour chaque face de l'hexa ( 0 a 7 )
      for ( int k=0 ; k<nb_faces(); k++)
        {
          // on recupere les sommets de la face consideree
          s1 = face_sommet_global(k,0);
          s2 = face_sommet_global(k,1);
          s3 = face_sommet_global(k,2);
          s4 = face_sommet_global(k,3);

          // Cerr << " les sommets de la face " << k << " = " << s1 << " " << s2 << " " << s3 << " " << s4 << finl;
          //         Cerr << "coord de s1 " << dom.coord(s1,0) << " " << dom.coord(s1,1) << " " << dom.coord(s1,2) << finl;
          //         Cerr << "coord de s2 " << dom.coord(s2,0) << " " << dom.coord(s2,1) << " " << dom.coord(s2,2) << finl;
          //         Cerr << "coord de s3 " << dom.coord(s3,0) << " " << dom.coord(s3,1) << " " << dom.coord(s3,2) << finl;
          //         Cerr << "coord de s4 " << dom.coord(s4,0) << " " << dom.coord(s4,1) << " " << dom.coord(s4,2) << finl;

          x1 = dom.coord(s1,0);
          y1 = dom.coord(s1,1);
          z1 = dom.coord(s1,2);

          x2 = dom.coord(s2,0);
          y2 = dom.coord(s2,1);
          z2 = dom.coord(s2,2);

          x3 = dom.coord(s4,0);
          y3 = dom.coord(s4,1);
          z3 = dom.coord(s4,2);

          //volume_tetra1 =  std::fabs((x1-x0)*((y2-y0)*(z3-z0)-(y3-y0)*(z2-z0))-
          //                   (x2-x0)*((y1-y0)*(z3-z0)-(y3-y0)*(z1-z0))+
          //                    (x3-x0)*((y1-y0)*(z2-z0)-(y2-y0)*(z1-z0)))/6;

          volume += std::fabs((x1-x0)*((y2-y0)*(z3-z0)-(y3-y0)*(z2-z0))-
                              (x2-x0)*((y1-y0)*(z3-z0)-(y3-y0)*(z1-z0))+
                              (x3-x0)*((y1-y0)*(z2-z0)-(y2-y0)*(z1-z0)))/6;

          x1 = dom.coord(s1,0);
          y1 = dom.coord(s1,1);
          z1 = dom.coord(s1,2);

          x2 = dom.coord(s4,0);
          y2 = dom.coord(s4,1);
          z2 = dom.coord(s4,2);

          x3 = dom.coord(s3,0);
          y3 = dom.coord(s3,1);
          z3 = dom.coord(s3,2);

          // volume_tetra2 = std::fabs((x1-x0)*((y2-y0)*(z3-z0)-(y3-y0)*(z2-z0))-
          //                   (x2-x0)*((y1-y0)*(z3-z0)-(y3-y0)*(z1-z0))+
          //                   (x3-x0)*((y1-y0)*(z2-z0)-(y2-y0)*(z1-z0)))/6;

          volume += std::fabs((x1-x0)*((y2-y0)*(z3-z0)-(y3-y0)*(z2-z0))-
                              (x2-x0)*((y1-y0)*(z3-z0)-(y3-y0)*(z1-z0))+
                              (x3-x0)*((y1-y0)*(z2-z0)-(y2-y0)*(z1-z0)))/6;
        }

      volumes[num_poly] = volume;
      volume_global += volumes[num_poly];
    }

  // Cerr << "volumes " << volumes  << finl;
  // Cerr << " le volume global est = " << volume_global << finl;

}

// Description:
//    Reordonne
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int reordonne(int i0,int i1,int i2,int i3,DoubleTab& coord,IntTab& elem,int& num_poly,int test, DoubleTab& v, ArrOfDouble& prod_,ArrOfDouble& prod_v, ArrOfDouble& dist, DoubleTab& prod_v2)
{
  int s[4];
  s[0]=elem(num_poly,i0);
  s[1]=elem(num_poly,i1);
  s[2]=elem(num_poly,i2);
  s[3]=elem(num_poly,i3);
  //  DoubleTab v(3,3);
  for (int i=1; i<4; i++)
    for (int dir=0; dir<3; dir++)
      v(i-1,dir)=coord(s[i],dir)-coord(s[0],dir);
  //ArrOfDouble prod_(3);
  int opp=-1;
  // avant tout on teste si on a bien un plan
  {

    // ArrOfDouble prod_v(3);
    int op=0,i=1;
    // prod_v=O1^O2
    prod_v[0]=v(op,1)*v(i,2)-v(op,2)*v(i,1);
    prod_v[1]=v(op,2)*v(i,0)-v(op,0)*v(i,2);
    prod_v[2]=v(op,0)*v(i,1)-v(op,1)*v(i,0);
    // ArrOfDouble dist(8);
    dist=0;
    for (int j=0; j<8; j++)
      {
        for (int dir=0; dir<3; dir++)
          dist[j]+=prod_v[dir]*(coord(elem(num_poly,j),dir)-coord(s[0],dir));
      }
    //Cerr<<"ici "<<i0 <<" i1 " <<i1<<" i2 "<<i2 << " i3 "<<i3 <<" "<<dist<<finl;
    // sommet le plus proche du plan
    int s_plan=0;
    double min_dist=1e25;
    for (int j=0; j<8; j++)
      {
        if ((j!=i0)&&(j!=i1)&&(j!=i2))
          if (std::fabs(dist[j])<min_dist)
            {
              s_plan=j;
              min_dist=std::fabs(dist[j]);
            }
      }
    if (s_plan!=i3)
      {
        // pas un plan
        if (test)
          {
            //Cerr<<"ici pb"<<finl;
            return 1;
          }
        else
          {
            Cerr<<"we do not have a plan "<<finl;
            Process::exit();
          }
      }
    // doit on tester  si les 4 autres points sont du meme cote ???

  }
  for (int op=0; op<3; op++)
    {
      //DoubleTab prod_v2(3,3);
      for (int i=0; i<3; i++)
        {
          prod_v2(i,0)=v(op,1)*v(i,2)-v(op,2)*v(i,1);
          prod_v2(i,1)=v(op,2)*v(i,0)-v(op,0)*v(i,2);
          prod_v2(i,2)=v(op,0)*v(i,1)-v(op,1)*v(i,0);
        }
      double prod=0;
      int i1bis=-1,j2bis=-1;
      if (op==0)
        {
          i1bis=1;
          j2bis=2;
        }
      else if (op==1)
        {
          i1bis=0;
          j2bis=2;
        }
      else if (op==2)
        {
          i1bis=0;
          j2bis=1;
        }
      for (int dir=0; dir<3; dir++)
        prod+=(prod_v2(i1bis,dir)*prod_v2(j2bis,dir));
      prod_[op]=prod;
      if (prod<0)
        {
          if (opp!=-1)
            {
              op=3;
              if (test) return 1;
              // on a deux produits negatifs on ne fait rien
            }
          opp=op;

        }
    }
  if ((test==0)&& ((opp==3)||(opp==-1)))
    {
      Cerr<<" this does not seem to be a plan "<<finl;
      Process::exit();
    }
  if ((opp!=2)&&(opp!=-2))
    {
      int i2b=i2;
      if (opp==0) i2b=i1;
      if (test==0)
        {
          int tmp=elem(num_poly,i2b);
          elem(num_poly,i2b)=elem(num_poly,i3);
          elem(num_poly,i3)=tmp;


          Cerr << "Permutation of local nodes "<<i2b<<" and "<<i3<<" on the element " <<num_poly<<" prod "<<prod_<<finl;

        }
      return 1;
    }
  return 0;
}
void Hexaedre_VEF::reordonner()
{
  /*
    22/12/04 : Generalisation de la methode (mais c'est plus lent!)
    Numerotation d'un hexa VEF:
    6--------7
    | \       |\
    4   2-----3  5
    \ /       \ |
    0---------1
    On boucle sur chacune des faces et on fait le produit scalaire 03.12/|03||12|
    et on le compare a 02.13/|02||13|
    S'il est plus grand on intervertit les sommets 2 et 3.
    L'algorithme fonctionne si 0123 et 4567 sont deja sur une meme face
  */
  Zone& zone=ma_zone.valeur();
  IntTab& elem=zone.les_elems();
  int nb_elem=zone.nb_elem();
  DoubleTab& coord=zone.domaine().les_sommets();
  // tableau contenant les permutations valides d'un quadrangle
  IntTab perm(8,4);
  perm( 0 , 0 )= 0 ;
  perm( 0 , 1 )= 1 ;
  perm( 0 , 2 )= 2 ;
  perm( 0 , 3 )= 3 ;
  perm( 1 , 0 )= 0 ;
  perm( 1 , 1 )= 2 ;
  perm( 1 , 2 )= 1 ;
  perm( 1 , 3 )= 3 ;
  perm( 2 , 0 )= 1 ;
  perm( 2 , 1 )= 0 ;
  perm( 2 , 2 )= 3 ;
  perm( 2 , 3 )= 2 ;
  perm( 3 , 0 )= 1 ;
  perm( 3 , 1 )= 3 ;
  perm( 3 , 2 )= 0 ;
  perm( 3 , 3 )= 2 ;
  perm( 4 , 0 )= 2 ;
  perm( 4 , 1 )= 0 ;
  perm( 4 , 2 )= 3 ;
  perm( 4 , 3 )= 1 ;
  perm( 5 , 0 )= 2 ;
  perm( 5 , 1 )= 3 ;
  perm( 5 , 2 )= 0 ;
  perm( 5 , 3 )= 1 ;
  perm( 6 , 0 )= 3 ;
  perm( 6 , 1 )= 1 ;
  perm( 6 , 2 )= 2 ;
  perm( 6 , 3 )= 0 ;
  perm( 7 , 0 )= 3 ;
  perm( 7 , 1 )= 2 ;
  perm( 7 , 2 )= 1 ;
  perm( 7 , 3 )= 0 ;

  DoubleTab v(3,3);
  ArrOfDouble prod_(3);
  ArrOfDouble prod_v(3);
  ArrOfDouble dist(8);
  DoubleTab prod_v2(3,3);

  ArrOfInt sa(8);
  for (int num_poly=0; num_poly<nb_elem; num_poly++)
    {
      int permutations=-1;
      //     int niter=0;
      // on commence par remettre correctement les 2 plans opposes
      permutations+=reordonne(0,1,2,3,coord,elem,num_poly,0, v,prod_, prod_v, dist, prod_v2);
      permutations+=reordonne(4,5,6,7,coord,elem,num_poly,0, v,prod_, prod_v, dist, prod_v2);
      for (int i=0; i<8; i++)
        sa[i]=elem(num_poly,i);
      // on teste les 8 possibilites
      int j;
      int n=0;
      int perm_valid=-1;
      for ( j=0; j<8; j++)
        {
          // Cerr<<" debut test "<<j<<finl;
          int test=1;
          for (int i=4; i<8; i++)
            elem(num_poly,i)=sa[perm(j,i-4)+4];
          // on regarde si les 6 plans sont valides
          permutations=0;
          permutations+=reordonne(2,0,3,1,coord,elem,num_poly,test, v,prod_, prod_v, dist, prod_v2);
          permutations+=reordonne(3,1,7,5,coord,elem,num_poly,test, v,prod_, prod_v, dist, prod_v2);
          permutations+=reordonne(7,5,6,4,coord,elem,num_poly,test, v,prod_, prod_v, dist, prod_v2);
          permutations+=reordonne(6,4,2,0,coord,elem,num_poly,test, v,prod_, prod_v, dist, prod_v2);
          permutations+=reordonne(2,3,6,7,coord,elem,num_poly,test, v,prod_, prod_v, dist, prod_v2);
          permutations+=reordonne(4,5,0,1,coord,elem,num_poly,test, v,prod_, prod_v, dist, prod_v2);
          if (permutations==0)
            {
              if (n>0)

                Cerr<<" former permutation "<< perm_valid<<" new permutation "<<j<<finl;
              perm_valid=j;
              n++;
            }

          //        Cerr << permutations << " permutations sur l'element " << num_poly << finl;
          //        Cerr << elem(num_poly,0) << " " << elem(num_poly,1) << " " << elem(num_poly,2) << " " << elem(num_poly,3);
          //        Cerr << " " << elem(num_poly,4) << " " << elem(num_poly,5) << " " << elem(num_poly,6) << " " << elem(num_poly,7) << finl;
          //        Cerr<<" fin test "<<j<<finl;
        }
      if (n==0)
        {
          Cerr << "Failure of the algorithm in Hexaedre_VEF::reordonner" << finl;
          Cerr << "Here is the connectivity element-node of the cell " << num_poly << " that raises problem:" << finl;
          for (int i=0; i<8; i++)
            Cerr << sa[i] << " ";
          Cerr << finl;
          exit();
        }
      else if (n>1)
        {
          Cerr<<"Failure of the algorithm in Hexaedre_VEF::reordonner"<<finl;
          Cerr<<"many permutations possibles"<<finl;
          Cerr << "Here is the connectivity element-node of the cell " << num_poly << " that raises problem:" << finl;
          for (int i=0; i<8; i++)
            Cerr << sa[i] << " ";
          Cerr << finl;
          Cerr << "Here is the new connectivity element-node of the cell " << num_poly << " that raises problem:" << finl;
          for (int i=0; i<8; i++)
            Cerr << elem(num_poly,i) << " ";
          Cerr << finl;
          // exit();
        }
      {
        for (int i=4; i<8; i++)
          elem(num_poly,i)=sa[perm(perm_valid,i-4)+4];

        if(perm_valid!=0)
          Cerr<<" we carried out the permutation "<<perm_valid<<" on the element "<<num_poly<<finl;
      }
    }
}
static int faces_sommets_hexa_vef[6][4] =
{
  { 0, 2, 4, 6 },
  { 0, 1, 4, 5 },
  { 0, 1, 2, 3 },
  { 1, 3, 5, 7 },
  { 2, 3, 6, 7 },
  { 4, 5, 6, 7 }
};

// Description: voir ElemGeomBase::get_tab_faces_sommets_locaux
int Hexaedre_VEF::get_tab_faces_sommets_locaux(IntTab& faces_som_local) const
{
  faces_som_local.resize(6,4);
  for (int i=0; i<6; i++)
    for (int j=0; j<4; j++)
      faces_som_local(i,j) = faces_sommets_hexa_vef[i][j];
  return 1;
}
