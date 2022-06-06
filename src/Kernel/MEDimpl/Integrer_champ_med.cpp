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

#include <Integrer_champ_med.h>
#include <Param.h>
#include <Champ_Fonc_MED.h>
#include <SFichier.h>

Implemente_instanciable(Integrer_champ_med,"Integrer_champ_med",Interprete);

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
Sortie& Integrer_champ_med::printOn(Sortie& os) const
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
Entree& Integrer_champ_med::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

void calcul_normal_norme(const ArrOfDouble& org,const ArrOfDouble& point1, const ArrOfDouble& point2,ArrOfDouble& normal);
double calcul_surface_triangle(const ArrOfDouble& origine,const ArrOfDouble& point1, const ArrOfDouble& point2)
{
  double normal0=(point1[1]-origine[1])*(point2[2]-origine[2])-((point2[1]-origine[1])*(point1[2]-origine[2]));

  double normal1=(point1[2]-origine[2])*(point2[0]-origine[0])-((point2[2]-origine[2])*(point1[0]-origine[0]));
  double normal2=(point1[0]-origine[0])*(point2[1]-origine[1])-((point2[0]-origine[0])*(point1[1]-origine[1]));
  double res=normal0*normal0+ normal1*normal1 + normal2*normal2 ;

  return sqrt(res)/2.;
}

double  portion_surface_elem(const ArrOfDouble& pointA,const ArrOfDouble& pointB, const ArrOfDouble& pointC,const double z)
{
  if (z<=pointA[2]) return 0;
  if (z>pointC[2]) return calcul_surface_triangle(pointA,pointB,pointC);
  ArrOfDouble pointD(3);
  // le point D est sur AC a hauteur de B.
  pointD[2]=pointB[2];
  double lambda=(pointB[2]-pointA[2])/(pointC[2]-pointA[2]);
  for (int i=0; i<2; i++)
    pointD[i]=pointA[i]+lambda*(pointC[i]-pointA[i]);
  if (z<=pointB[2])
    {
      // on calcul la surface du triangle inferieur
      // et on prend la proportion...
      double surf_inf=calcul_surface_triangle(pointA,pointB,pointD);
      double rap=1;
      if (pointB[2]!=pointA[2])
        rap=(z-pointA[2])/(pointB[2]-pointA[2]);
      return surf_inf*rap*rap;
    }
  else
    {
      // on calcul la surface du triangle inferieur
      // et on prend la proportion...
      // et on retire a la surface totale
      double surf_sup=calcul_surface_triangle(pointB,pointD,pointC);
      double rap=(z-pointC[2])/(pointB[2]-pointC[2]);
      double surf_tot=calcul_surface_triangle(pointA,pointB,pointC);
      return surf_tot-surf_sup*rap*rap;
    }

}

double portion_surface(const ArrOfDouble& point0,const ArrOfDouble& point1, const ArrOfDouble& point2,const double zmin,const double zmax)
{
  return portion_surface_elem(point0,point1,point2,zmax)- portion_surface_elem(point0,point1,point2,zmin);
}





static True_int fonction_tri_data(const void *ptr1,
                                  const void *ptr2)
{
  const double * tab1 = (const double *) ptr1;
  const double * tab2 = (const double *) ptr2;
  double delta=(tab1[0] - tab2[0]);
  if (delta>0)
    return 1;
  else if (delta<0)
    return -1;
  return 0;
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
Entree& Integrer_champ_med::interpreter(Entree& is)
{
  Motcle nom_methode;
  Nom nom_fichier("integrale");
  Nom nom_champ_fonc_med;

  double zmin=0,zmax=0;
  int nb_tranche=1;
  Param param(que_suis_je());
  param.ajouter("champ_med",&nom_champ_fonc_med,Param::REQUIRED);
  param.ajouter("methode",&nom_methode,Param::REQUIRED);
  param.ajouter("zmin",&zmin);
  param.ajouter("zmax",&zmax);
  param.ajouter("nb_tranche",&nb_tranche);
  param.ajouter("fichier_sortie",&nom_fichier);
  param.lire_avec_accolades_depuis(is);
  if ((nom_methode!="integrale_en_z")&&(nom_methode!="debit_total"))
    {
      Cerr<<"method "<<nom_methode <<" not coded yet "<<finl;
    }
  // on recupere le domaine
  if(! sub_type(Champ_Fonc_MED, objet(nom_champ_fonc_med)))
    {
      Cerr << nom_champ_fonc_med << " type is " << objet(nom_champ_fonc_med).que_suis_je() << finl;
      Cerr << "only the objects of type Champ_Fonc_MED can be meshed" << finl;
      exit();
    }
  Champ_Fonc_MED& champ=ref_cast(Champ_Fonc_MED, objet(nom_champ_fonc_med));
  const Zone& zone=champ.zone_dis_base().zone();
  const DoubleTab& coord=zone.domaine().les_sommets();
  // on fait une coie pour les modifier
  IntTab les_elems_mod=zone.les_elems();
  const IntTab&  les_elems=zone.les_elems();

  ArrOfDouble normal(3);


  const DoubleTab& valeurs= champ.valeurs();
  assert(valeurs.dimension(0)==zone.nb_elem());
  assert(valeurs.dimension(1)==3);
  double dz=(zmax-zmin)/nb_tranche;
  ArrOfDouble res(nb_tranche),pos(nb_tranche),surface(nb_tranche);
  int nb_elem=les_elems.dimension(0);
  Cerr<<" Field integration using the method "<<nom_methode<<" on the domain "<<zone.domaine().le_nom() <<finl;
  if (nom_methode=="debit_total")
    {
      nb_tranche=1;
      zmax=DMAXFLOAT/2.;
      zmin=-zmax;
      dz=(zmax-zmin);
      pos[0]=0;

    }
  //if (nom_methode=="integrale_en_z")
  {
    // Etape 1 on reordonne les triangles pour classeren z les sommets

    for (int elem=0; elem<nb_elem; elem++)
      {
        DoubleTab zz(3,2);
        for (int i=0; i<3; i++)
          {
            int s=les_elems(elem,i);
            zz(i,1)=s;
            zz(i,0)=coord(s,2);
          }

        qsort(zz.addr(),3,sizeof(double) * 2 , fonction_tri_data);
        for (int i=0; i<3; i++)
          {
            int ss=(int)zz(i,1);
            //          if (ss!=les_elems(elem,i)) Cerr<<" coucou" <<elem<<finl;
            les_elems_mod(elem,i)=ss;
          }

      }
    //
    ArrOfDouble point0(3),point1(3),point2(3);

    for (int elem=0; elem<nb_elem; elem++)
      {

        for (int i=0; i<3; i++)
          {
            point0[i]=coord(les_elems(elem,0),i);
            point1[i]=coord(les_elems(elem,1),i);
            point2[i]=coord(les_elems(elem,2),i);
          }
        calcul_normal_norme(point0,point1,  point2, normal);
        for (int i=0; i<3; i++)
          {
            point0[i]=coord(les_elems_mod(elem,0),i);
            point1[i]=coord(les_elems_mod(elem,1),i);
            point2[i]=coord(les_elems_mod(elem,2),i);
          }
        for (int tranche=0; tranche<nb_tranche; tranche++)
          {

            double zminl=(zmin)+(zmax-zmin)/nb_tranche*tranche;
            double zmaxl=zminl+dz;
            pos[tranche]=(zminl+zmaxl)/2.;
            double z0=coord(les_elems_mod(elem,0),2);
            if (z0<=zmaxl)
              {

                double z2=coord(les_elems_mod(elem,2),2);
                if (z2>=zminl)
                  {


                    double portion=portion_surface(point0,point1,point2,zminl,zmaxl);
                    double prod_scal=0;

                    for (int i=0; i<3; i++)
                      prod_scal+=valeurs(elem,i)*normal[i];
                    res[tranche]+=portion*prod_scal;
                    surface[tranche]+=portion;
                  }
              }

          }
      }
  }

  double trace=0;
  SFichier so(nom_fichier);
  for (int nb=0; nb<nb_tranche; nb++)
    {
      trace+=res[nb];
      if (surface[nb]!=0)
        res[nb]/=surface[nb];
      else assert(res[nb]==0);
    }
  Cout<<"# overall balance "<<trace<<finl;
  so<<"# bilan global "<<trace<<finl;
  so<<"# position, valeur moyenne, surface, flux"<<finl;
  for (int nb=0; nb<nb_tranche; nb++)
    so << pos[nb]<<" "<<res[nb]<<" "<<surface[nb]<<" "<<res[nb]*surface[nb]<<finl;

  return is;

}





