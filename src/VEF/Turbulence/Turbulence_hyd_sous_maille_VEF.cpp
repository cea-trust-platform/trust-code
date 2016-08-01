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
// File:        Turbulence_hyd_sous_maille_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/28
//
//////////////////////////////////////////////////////////////////////////////

#include <Turbulence_hyd_sous_maille_VEF.h>
#include <Periodique.h>
#include <Debog.h>
#include <Schema_Temps_base.h>
#include <Equation_base.h>
#include <Zone_VEF.h>
#include <Zone_Cl_VEF.h>

Implemente_instanciable(Turbulence_hyd_sous_maille_VEF,"Modele_turbulence_hyd_sous_maille_VEF",Mod_turb_hyd_ss_maille_VEF);

//// printOn
//

Sortie& Turbulence_hyd_sous_maille_VEF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}


//// readOn
//

Entree& Turbulence_hyd_sous_maille_VEF::readOn(Entree& s )
{
  return Mod_turb_hyd_ss_maille_VEF::readOn(s) ;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Implementation de fonctions de la classe Turbulence_hyd_sous_maille_VEF
//
//////////////////////////////////////////////////////////////////////////////


Champ_Fonc& Turbulence_hyd_sous_maille_VEF::calculer_viscosite_turbulente()
{
  static const double Csm1 = CSM1;
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zone_Cl_VEF.valeur();
  double temps = mon_equation->inconnue().temps();
  DoubleTab& visco_turb = la_viscosite_turbulente.valeurs();
  const int nb_face = zone_VEF.nb_faces();
  const int nb_face_tot = zone_VEF.nb_faces_tot();
  const int nb_elem = zone_VEF.nb_elem();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  int elem1,elem2,fac=0,i;
  double temp;

  F2.resize(nb_face_tot);
  r.resize(nb_face_tot);

  calculer_fonction_structure();

  if (visco_turb.size() != nb_elem)
    {
      Cerr << "erreur dans la taille du DoubleTab valeurs de la viscosite" << finl;
      exit();
    }
  visco_turb=0.;

  // Traitement des bords

  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();

  for (i=0; i<les_cl.size(); i++)
    {
      const Cond_lim& la_cl = les_cl[i];
      const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = la_front_dis.num_premiere_face();
      int nfin = ndeb + la_front_dis.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        for (fac=ndeb; fac<nfin ; fac++)
          {
            elem1=face_voisins(fac,0);
            elem2=face_voisins(fac,1);
            temp=Csm1*r(fac)*sqrt(F2(fac));
            visco_turb(elem1)+=0.5*temp;
            visco_turb(elem2)+=0.5*temp;
          }
      else
        for (fac=ndeb; fac<nfin ; fac++)
          {
            elem1=face_voisins(fac,0);
            temp=Csm1*r(fac)*sqrt(F2(fac));
            visco_turb(elem1)+=temp;
          }
    }

  // Traitement des faces internes
  for (; fac<nb_face ; fac++)
    {
      elem1=face_voisins(fac,0);
      elem2=face_voisins(fac,1);
      temp=Csm1*r(fac)*sqrt(F2(fac));
      visco_turb(elem1)+=temp;
      visco_turb(elem2)+=temp;
    }

  visco_turb/=(zone_VEF.zone().nb_faces_elem());

  la_viscosite_turbulente.changer_temps(temps);
  return la_viscosite_turbulente;
}

void Turbulence_hyd_sous_maille_VEF::calculer_fonction_structure()
{
  const DoubleTab& la_vitesse = mon_equation->inconnue().valeurs();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zone_Cl_VEF.valeur();
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();

  const int nb_face = zone_VEF.nb_faces();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const DoubleTab& xv = zone_VEF.xv();

  DoubleVect d2(dimension+1),dv(dimension);
  DoubleTab  dd2(dimension+1,2);

  int fac=0,i,j,z;
  IntVect num(2);


  // On calcule tout d'abord F2 pour les faces se trouvant sur
  // les bords du domaine

  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();

  for (int num_cl=0; num_cl<les_cl.size(); num_cl++)
    {
      const Cond_lim& la_cl = les_cl[num_cl];
      const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = la_front_dis.num_premiere_face();
      int nfin = ndeb + la_front_dis.nb_faces();
      IntVect faces_elem_loc(dimension+1);
      int face_appartient_elem =0;

      if (sub_type(Periodique,la_cl.valeur()) )
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());

          for (fac=ndeb ; fac<nfin ; fac++)
            {

              double m;
              double petit=1.e30;

              for (j=0 ; j<2 ; j++)
                {
                  num(j) = face_voisins(fac,j);

                  face_appartient_elem = 0;

                  for (i=0 ; i<dimension+1 ; i++)
                    {
                      faces_elem_loc[i] = elem_faces(num(j),i);
                      if (faces_elem_loc[i] == fac)
                        face_appartient_elem = 1;
                    }

                  if (face_appartient_elem == 1)

                    for (i=0 ; i<dimension+1 ; i++)
                      {

                        m=0;
                        for (z=0 ; z<dimension ; z++)
                          {
                            m+=carre(xv(faces_elem_loc[i],z)-xv(fac,z));
                          }
                        double temp=dd2(i,j)=sqrt(m);
                        if(temp>1.e-24)
                          petit = min(petit,temp);
                      }

                  else

                    for (i=0 ; i<dimension+1 ; i++)
                      {

                        m=0;
                        for (z=0 ; z<dimension ; z++)
                          {
                            if (z == la_cl_perio.direction_periodicite())
                              {
                                m+=carre(dabs(xv(faces_elem_loc[i],z)-xv(fac,z)) - la_cl_perio.distance());
                              }
                            else
                              m+=carre(xv(faces_elem_loc[i],z)-xv(fac,z));
                          }
                        double temp=dd2(i,j)=sqrt(m);
                        if(temp>1.e-24)
                          petit = min(petit,temp);
                      }
                }
              r(fac) = petit ;

              if(petit<0.)
                {
                  Cerr <<"pb avec r(fac) negatif dans Turbulence_hyd_sous_maille_VEF" << finl;
                  exit();
                }

              // On calcule la vitesse aux 2*dimension points qui entourent le point 0
              //  puis la fonction de structure d'abord en chaque point de calcul, puis sur la
              //  face comportant le point 0 .

              double F1=0;
              double F1b=0;


              for (i=0 ; i<2; i++ )
                {
                  num(i)=face_voisins(fac,i);
                  for (z=0 ; z<dimension+1 ; z++)
                    {
                      if(dd2(z,i)>1e-24)
                        {
                          for (j=0 ; j<dimension ; j++)
                            {
                              dv(j)=(la_vitesse(elem_faces(num(i),z),j)-la_vitesse(fac,j))
                                    *r(fac)/dd2(z,i);
                              F1+=dv(j)*dv(j);
                            }

                          F1b+=F1;
                          F1=0;
                        }
                      else
                        {
                          F1b+=F1;
                          F1=0;
                        }
                    }
                }
              F2(fac)=F1b/(2*dimension);
            }
        }
      else
        {
          for (fac=ndeb ; fac<nfin ; fac++)
            {
              double m;
              double petit=1e30;
              int num1;
              num1 = face_voisins(fac,0);
              for (i=0 ; i<dimension+1 ; i++)
                {
                  m=0;
                  for (z=0 ; z<dimension ; z++)
                    {
                      m+=carre(xv(elem_faces(num1,i),z)-xv(fac,z));
                    }
                  double temp=d2(i)=sqrt(m);
                  if(temp>1.e-24)
                    petit = min(petit,temp);
                }
              r(fac) = petit ;
              if(petit<0.)
                {
                  Cerr <<"pb avec r(fac) negatif dans Turbulence_hyd_sous_maille_VEF" << finl;
                  exit();
                }

              double F1=0;
              double F1b=0;


              for (z=0 ; z<dimension+1 ; z++)
                {
                  if(d2(z)>1.e-24)
                    {
                      for (j=0 ; j<dimension ; j++)
                        {
                          dv(j)=(la_vitesse(elem_faces(num1,z),j)-la_vitesse(fac,j))
                                *r(fac)/d2(z);
                          F1+=dv(j)*dv(j);
                        }
                      F1b+=F1;
                      F1=0;
                    }
                  else
                    {
                      F1b+=F1;
                      F1=0;
                    }
                }
              F2(fac)=F1b/(dimension);


            }
        }
    } // fin traitement des bords


  //
  // On traite maintenant les faces restantes
  //

  for (; fac<nb_face ; fac++)
    {

      // On commence par determiner la distance entre le point ou on calcule
      // et les points qui l'entourent


      double m;
      double petit=1.e30;

      for (j=0 ; j<2 ; j++)
        {
          num(j) = face_voisins(fac,j);
          for (i=0 ; i<dimension+1 ; i++)
            {
              m=0;
              for (z=0 ; z<dimension ; z++)
                {
                  m+=carre(xv(elem_faces(num(j),i),z)-xv(fac,z));
                }
              double temp=dd2(i,j)=sqrt(m);
              if(temp>1.e-24)
                petit = min(petit,temp);
            }
        }

      r(fac) = petit ;
      if(petit<0.)
        {
          Cerr <<"pb avec r(fac) negatif dans Turbulence_hyd_sous_maille_VEF" << finl;
          exit();
        }


      // On calcule la vitesse aux 2*dimension points qui entourent le point 0
      //  puis la fonction de structure d'abord en chaque point de calcul, puis sur la
      //  face comportant le point 0 .

      double F1=0;
      double F1b=0;


      for (i=0 ; i<2; i++ )
        {
          num(i)=face_voisins(fac,i);
          for (z=0 ; z<dimension+1 ; z++)
            {
              if(dd2(z,i)>1e-24)
                {
                  for (j=0 ; j<dimension ; j++)
                    {
                      dv(j)=(la_vitesse(elem_faces(num(i),z),j)-la_vitesse(fac,j))
                            *r(fac)/dd2(z,i);
                      F1+=dv(j)*dv(j);
                    }
                  F1b+=F1;
                  F1=0;
                }
              else
                {
                  F1b+=F1;
                  F1=0;
                }
            }

        }
      F2(fac)=F1b/(2*dimension);

    }

}
