/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Ch_front_Vortex.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/21
//
//////////////////////////////////////////////////////////////////////////////

#include <Ch_front_Vortex.h>
#include <Domaine.h>
#include <LecFicDiffuse.h>
#include <Interprete.h>
#include <Zone_VF.h>
#include <time.h>
#include <SFichier.h>

Implemente_instanciable_sans_destructeur(Ch_front_Vortex,"Champ_front_Vortex",Champ_front_var_instationnaire);


Ch_front_Vortex::~Ch_front_Vortex()
{
  sauvegarder_vortex();
}

// Description:
//    Imprime le champ sur flot de sortie.
//    Imprime la taille du champ et la valeur (constante) sur
//    la frontiere.
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
Sortie& Ch_front_Vortex::printOn(Sortie& os) const
{
  const DoubleTab& tab=valeurs();
  os << tab.size() << " ";
  for(int i=0; i<tab.size(); i++)
    os << tab(0,i);
  return os;
}

// Description:
//    Lit le champ a partir d'un flot d'entree.
//    Format:
//      Champ_front_Vortex nb_compo vrel_1 ... [vrel_i]
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree& is
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: le champ analytique a la valeur lue
Entree& Ch_front_Vortex::readOn(Entree& is)
{

  if(dimension!=3)
    {
      Cerr << " Vortex Boundary condition requests 3D problem " << finl;
      exit();
    }

  fixer_nb_comp(dimension);

  init = 1;
  first_rand = 1;

  Nom nom;
  is >> nom >> geom >> nu >> utau;

  mon_domaine = ref_cast(Domaine, Interprete::objet(nom));

  return is;
}

// Description:
//    Renvoie l'objet upcaste en Champ_front_base&
// Precondition:
// Parametre: Champ_front_base& ch
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: NON ACCEDE
// Retour: Champ_front_base&
//    Signification: (*this) upcaste en Champ_front_base&
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_front_base& Ch_front_Vortex::affecter_(const Champ_front_base& ch)
{
  return *this;
}


int Ch_front_Vortex::my_rand( )
{
  if (first_rand == 1)
    {
      srand(time (NULL));
      first_rand = 0;
    }
  return (rand ());
}

void Ch_front_Vortex::sauvegarder_vortex()
{
  Nom fichier = "vortex.sauv";
  if (nproc()>1) fichier=fichier.nom_me(me());
  if(la_frontiere_dis.non_nul())
    {
      Cerr << "Saving vortices in " << fichier << finl;
      SFichier fic(fichier);
      fic.setf(ios::scientific);

      int nbvortex =  xvort.size();

      fic << temps << finl;
      fic << nbvortex << finl;

      for (int i=0; i<nbvortex; i++)
        {
          fic << xvort(i) << " " << yvort(i) << " " << zvort(i) << " " << tvort(i) << " " ;
          fic << svort(i) << " " << fvort(i) << " " << gamma(i) << " " << sigma(i) << finl;
        }
    }
}

void Ch_front_Vortex::reprendre_vortex()
{
  Cerr << " Ch_front_Vortex::reprendre_vortex" << finl;

  Nom fichier = "vortex.sauv";
  if (nproc()>1) fichier=fichier.nom_me(me());
  if(la_frontiere_dis.non_nul())
    {
      Cerr << "Resumption of vortices in " << fichier << finl;
      EFichier fic(fichier);
      fic.setf(ios::scientific);

      if(fic.fail())
        {
          Cerr << "Unable to open file " << fichier << finl;
          exit();
        }

      double tps;
      int nbvortex;

      fic >> tps;
      fic >> nbvortex;

      if(!(dabs(temps-tps)/(dabs(tps)+1.e-24)<1.e-6))
        {
          Cerr << " time : " << tps << " in " << fichier << " does not match the time of resumed " << temps << finl;
          exit();
        }

      Cerr << " resumed " << nbvortex << " vortex at time " << temps << finl;

      for (int i=0; i<nbvortex; i++)
        {
          fic >> xvort(i) >> yvort(i) >> zvort(i) >> tvort(i);
          fic >> svort(i) >> fvort(i) >> gamma(i) >> sigma(i);
        }

      Cerr << " resumed made " << finl;
    }
}


int Ch_front_Vortex::initialiser(double un_temps, const Champ_Inc_base& inco)
{
  if (!Champ_front_var_instationnaire::initialiser(un_temps,inco))
    return 0;

  Domaine& domaine=mon_domaine.valeur();
  const Frontiere_dis_base& fr_dis=frontiere_dis();
  const Frontiere& frontiere=fr_dis.frontiere();
  const int nb_faces=frontiere.nb_faces();
  const int ndeb=frontiere.num_premiere_face();
  const int nfin=ndeb+nb_faces;
  const Faces& faces=frontiere.faces();
  int nbsf=faces.nb_som_faces();
  const Zone_VF& zvf = ref_cast(Zone_VF, zone_dis());
  const DoubleTab& xv=zvf.xv();

  double x,y,z;
  double dx,dy,dz;
  double norme;

  double  C_mu = 0.09;


  u.resize(nb_faces);
  v.resize(nb_faces);
  w.resize(nb_faces);
  u_moy.resize(nb_faces);
  dudy.resize(nb_faces);
  k.resize(nb_faces);
  eps.resize(nb_faces);
  Wk.resize(nb_faces);

  for(int face=ndeb; face<nfin; face++)
    {
      Wk(face) = 2.*drand48()-1.;
    }


  /////////////////////////////////////////////////////////
  // caracteristiques geometriques associees a la frontiere
  /////////////////////////////////////////////////////////

  if(geom=="circle")
    {

      double x_xmin=1.e6;
      double x_xmax=-1.e6;
      double y_xmin=1.e6;
      double y_xmax=-1.e6;
      double z_xmin=1.e6;
      double z_xmax=-1.e6;
      double x_ymin=1.e6;
      double x_ymax=-1.e6;
      double y_ymin=1.e6;
      double y_ymax=-1.e6;
      double z_ymin=1.e6;
      double z_ymax=-1.e6;


      for(int face=ndeb; face<nfin; face++)
        {
          for(int kk=0; kk<nbsf; kk++)
            {
              x=domaine.coord(faces.sommet(face,kk),0);
              y=domaine.coord(faces.sommet(face,kk),1);
              z=domaine.coord(faces.sommet(face,kk),2);

              if(y<y_ymin)
                {
                  y_ymin=y;
                  x_ymin=x;
                  z_ymin=z;
                }
              if(y>y_ymax)
                {
                  y_ymax=y;
                  x_ymax=x;
                  z_ymax=z;
                }

              if(x<x_xmin)
                {
                  x_xmin=x;
                  y_xmin=y;
                  z_xmin=z;
                }
              if(x>x_xmax)
                {
                  x_xmax=x;
                  y_xmax=y;
                  z_xmax=z;
                }
            }
        }

      if(!est_egal(y_ymin,y_ymax))
        {
          dx=x_ymax-x_ymin;
          dy=y_ymax-y_ymin;
          dz=z_ymax-z_ymin;

          Ox=0.5*(x_ymax+x_ymin);
          Oy=0.5*(y_ymax+y_ymin);
          Oz=0.5*(z_ymax+z_ymin);
        }
      else
        {
          dx=x_xmax-x_xmin;
          dy=y_xmax-y_xmin;
          dz=z_xmax-z_xmin;

          Ox=0.5*(x_xmax+x_xmin);
          Oy=0.5*(y_xmax+y_xmin);
          Oz=0.5*(z_xmax+z_xmin);
        }

      R=0.5*sqrt(dx*dx+dy*dy+dz*dz);

      surf = M_PI*R*R;

      Cerr << " Coordinates associated with the boundary : " << Ox << " " << Oy << " " << Oz << finl;
      Cerr << " Radius computed : " << R << finl;

    }
  else if(geom=="channel")
    {
      //
      // le canal est suppose etre oriente suivant x,
      // periodique suivant z (et x)
      // la frontiere d'entree centree sur l'origine
      // de hauteur et largeur: 2
      // soit (x,y,z) : [0;0]x[-1;+1]x[-1;+1]

      R=1.;

      Ox=0.;
      Oy=0.;
      Oz=0.;

      surf = 4.;
    }

  ///////////////////////////////////////////////////////////////
  // repere local (t1;t2) associe a la frontiere (supposee plane)
  ///////////////////////////////////////////////////////////////
  if (nb_faces==0)
    {
      nb_vortex=0;
    }
  else
    {
      nx=-zvf.face_normales(ndeb,0);  // normales orientees vers l'interieur du domaine
      ny=-zvf.face_normales(ndeb,1);
      nz=-zvf.face_normales(ndeb,2);

      norme = sqrt( nx*nx + ny*ny + nz*nz );

      nx/=norme;
      ny/=norme;
      nz/=norme;

      t1x = -ny;
      t1y =  nx;
      t1z =  0.;

      if( (est_egal(nx,0.)) && (est_egal(ny,0.)) )
        {
          t1x =  0.;
          t1y =  nz;
          t1z = -ny;
        }

      norme = sqrt( t1x*t1x + t1y*t1y + t1z*t1z );

      t1x/=norme;
      t1y/=norme;
      t1z/=norme;

      t2x =  ny*t1z - nz*t1y;
      t2y =  nz*t1x - nx*t1z;
      t2z =  nx*t1y - ny*t1x;
      /*
           Cerr << " t1x " << t1x << finl;
           Cerr << " t1y " << t1y << finl;
           Cerr << " t1z " << t1z << finl;
           Cerr << " t2x " << t2x << finl;
           Cerr << " t2y " << t2y << finl;
           Cerr << " t2z " << t2z << finl;
      */
      ///////////////////////////////////////////////////////////////////
      // initialisation des champ de : vitesse - k - epsilon (analytique)
      ///////////////////////////////////////////////////////////////////

      double d,dplus,Reytau;
      double kappa = 0.41;
      double beta = 5.1;
      double tmp=15*15*15*15;
      for(int face=ndeb; face<nfin; face++)
        {
          x=xv(face,0);
          y=xv(face,1);
          z=xv(face,2);

          dx=x-Ox;
          dy=y-Oy;
          dz=z-Oz;

          if(geom=="circle")
            {
              d = R - sqrt(dx*dx+dy*dy+dz*dz) ;  // distance a la paroi
            }
          else
            {
              d = R - dabs(y) ;  // distance a la paroi
            }

          if(d<=0.)
            {
              Cerr << " the computation of a zero or negative distance is prejudicial for the rest of the calculation" << finl;
              Cerr << " Problem certainly related to the calculation of the size of domain or origin related to the bondary" << finl;
              exit();
            }

          dplus   = d*utau/nu;
          Reytau  = R*utau/nu;

          if(dplus<11.)
            {
              u_moy(face) = utau*dplus ;
            }
          else
            {
              u_moy(face) = utau*((1./kappa)*log(dplus)+beta) ;
            }

          if(dplus<11.)
            {
              dudy(face) = utau*utau/nu ;  // -> gradient dudy discontinu a dplus = 11
            }
          else
            {
              dudy(face) = utau/(kappa*d) ;  // -> genere une discontinuite sur u' en cette position
            }

          k(face)   = utau*utau * ( 0.07*dplus*dplus*exp(-dplus/8)+4.5*(1.-exp(-dplus/20.))/(1.+4.*dplus/Reytau) ) ;
          eps(face) = (utau*utau*utau*utau/nu) * (1./(0.41 *(pow(dplus*dplus*dplus*dplus+tmp,0.25)))) ;
        }

      u = u_moy;

      //////////////////////////////////
      // evaluation du nombre de vortex
      /////////////////////////////////

      double sigma_moy = 0.;

      for(int face=ndeb; face<nfin; face++)
        {
          double sx=zvf.face_normales(face,0);
          double sy=zvf.face_normales(face,1);
          double sz=zvf.face_normales(face,2);

          sigma_moy += sqrt(sx*sx+sy*sy+sz*sz) * pow(k(face),1.5) / eps(face);

        }
      sigma_moy *= pow(C_mu,0.75);
      sigma_moy /= surf;
      // On ne peut pas utiliser modf (voir Schema_Temps_base::limpr) car
      // nb_vortex sert a dimensionner des tableaux !
      nb_vortex = int ( surf / (M_PI * sigma_moy *sigma_moy) );
    }
  Cerr << "nb_vortex " <<nb_vortex << finl;

  //nb_vortex=1;
  //nb_vortex=nb_faces;

  xvort.resize(nb_vortex);
  yvort.resize(nb_vortex);
  zvort.resize(nb_vortex);
  tvort.resize(nb_vortex);
  svort.resize(nb_vortex);
  fvort.resize_array(nb_vortex);
  gamma.resize(nb_vortex);
  sigma.resize(nb_vortex);

  //////////////////////////////////////////////////////
  // Calcul du temps de duree de vie minimum des vortex
  //////////////////////////////////////////////////////

  double dt_vortex,dt_min=1.e6;

  for (int i=0; i<nb_faces; i++) // estimation du dt_min requis
    {
      dt_vortex=5.*C_mu*pow(k(i),3./2.)/(eps(i)*u_moy(i));
      if(dt_vortex<dt_min) dt_min=dt_vortex;
    }

  Cerr << " DT_MIN VORTEX " << dt_min << finl;

  /////////////////////////////////////////////////
  // initialisation des vortex par tirage aleatoire
  /////////////////////////////////////////////////

  /*
           for (int i=0;i<nb_vortex;i++)  // random different a chaque nouveau tirage
           {
           int face = my_rand() % nb_faces;
           double rand1 = (my_rand() % 100)/100.;
           double rand2 = (my_rand() % 100)/100.;
           xvort(i)=xv(face,0);
           yvort(i)=xv(face,1);
           zvort(i)=xv(face,2);
           tvort(i)=rand1*5.*C_mu*pow(k(face),3./2.)/(eps(face)*u_moy(face));
           if (rand2<0.5) svort(i)=-1.;
           else svort(i)=1.;

           Cerr << " Vortex " << i << finl;
           Cerr << " Face " << face << " " << xvort(i) << " " <<yvort(i) << " " <<zvort(i) << finl;
           Cerr <<" " << tvort(i) << " " <<svort(i) << finl;
           Cerr << " " << finl;

           }
  */

  for (int i=0; i<nb_vortex; i++) // random identique a chaque nouveau tirage
    {
      int face = int(nb_faces*drand48());
      xvort(i)=xv(face,0);
      yvort(i)=xv(face,1);
      zvort(i)=xv(face,2);
      tvort(i)=drand48()*5.*C_mu*pow(k(face),3./2.)/(eps(face)*u_moy(face));
      if (drand48()<0.5) svort(i)=-1.;
      else svort(i)=1.;
      /*
             Cerr << " Vortex " << i << finl;
             Cerr << " Face " << face << " " << xvort(i) << " " <<yvort(i) << " " <<zvort(i) << finl;
             Cerr <<" " << tvort(i) << " " <<svort(i) << finl;
             Cerr << " " << finl;
      */
    }

  mettre_a_jour(un_temps);

  return 1;
}

void  Ch_front_Vortex::deplacement_vortex(double dx,double dy,double dz,double asigma,double agamma,
                                          double& vv,double& ww)
{
  double norme2 = dx*dx + dy*dy + dz*dz ;

  if(norme2<(4.*asigma) && norme2>1.e-8)
    {
      double e1 = exp(-norme2/(2.*asigma*asigma));
      double f  = (1.-e1) * agamma * e1 / (2.* M_PI * norme2 );

      double dt1 = dx*t1x + dy*t1y + dz*t1z ;
      double dt2 = dx*t2x + dy*t2y + dz*t2z ;

      vv += dt2*f;
      ww -= dt1*f;
    }
}

void Ch_front_Vortex::mettre_a_jour(double tps)
{
  const Frontiere_dis_base& fr_dis=frontiere_dis();
  const Frontiere& frontiere=fr_dis.frontiere();
  const int nb_faces=frontiere.nb_faces();
  const int ndeb=frontiere.num_premiere_face();
  const int nfin=ndeb+nb_faces;
  DoubleTab& les_val=valeurs();

  const Zone_VF& zvf = ref_cast(Zone_VF, zone_dis());
  const DoubleTab& xv=zvf.xv();

  double alpha=0;
  double dist,dist2;
  double x,yter,z;
  double xp,yp,zp;
  double xvort_mir,yvort_mir,zvort_mir;
  double xvort_per,yvort_per,zvort_per;
  double dx,dy,dz;

  double  C_mu = 0.09;

  if(init)
    {
      init=0;
      temps=tps;

      if(temps!=0.) // il s'agit d'une reprise
        reprendre_vortex();
    }

  //////////////////////////////////////////////////
  // caracteristiques des vortex (position-vorticite)
  //////////////////////////////////////////////////
  if (nb_vortex!=0)
    alpha = 4.*sqrt(M_PI*surf/(3.*nb_vortex*(2.*log(3.)-3.*log(2.))));

  for (int i=0; i<nb_vortex; i++)
    {
      x=xvort(i);
      yter=yvort(i);
      z=zvort(i);

      dist=1.e6;
      int face=-1;

      for(int j=ndeb; j<nfin; j++)
        {
          dist2=(xv(j,0)-x)*(xv(j,0)-x)+(xv(j,1)-yter)*(xv(j,1)-yter)+(xv(j,2)-z)*(xv(j,2)-z);

          if(dist2<dist)
            {
              dist=dist2;
              face=j;
            }
        }
      gamma(i)     = alpha * sqrt(k(face)) * svort(i);
      sigma(i)     = pow(C_mu,0.75) * pow(k(face),1.5) / eps(face);
      fvort(i)     = face ;
    }

  ///////////////////////////////////////////////////////////////////////////////////
  // calcul de la vitesse induite par les vortex en chacune des faces de la frontiere
  ///////////////////////////////////////////////////////////////////////////////////

  for(int face=ndeb; face<nfin; face++)
    {
      x=xv(face,0);
      yter=xv(face,1);
      z=xv(face,2);

      x -= Ox ;  // ATTENTION : Pour des raisons pratiques,
      yter -= Oy ;  // les coordonnees des points sont exprimees
      z -= Oz ;  // en fonction de l'origine Ox,Oy,Oz de la frontiere

      v(face) = 0.;  // vitesse induite par les vortex a la face suivant t1
      w(face) = 0.;  // vitesse induite par les vortex a la face suivant t2

      for (int i=0; i<nb_vortex; i++)
        {
          dx = (xvort(i)-Ox) - x;
          dy = (yvort(i)-Oy) - yter;
          dz = (zvort(i)-Oz) - z;

          deplacement_vortex(dx,dy,dz,sigma(i),gamma(i),v(face),w(face));


          ////////////////////////////////////
          // traitement des conditions limites
          ////////////////////////////////////

          //***************************************
          // Parois : traitement des vortex miroirs
          //***************************************

          if(geom=="circle")
            {
              xp = x * R / (sqrt(x*x+yter*yter+z*z)) ;
              yp = yter * R / (sqrt(x*x+yter*yter+z*z)) ;
              zp = z * R / (sqrt(x*x+yter*yter+z*z)) ;
            }
          else //channel
            {
              xp = x;
              yp = R;
              if(yter<0) yp = -R;
              zp = z;
            }

          xvort_mir = 2. * xp - (xvort(i)-Ox);
          yvort_mir = 2. * yp - (yvort(i)-Oy);
          zvort_mir = 2. * zp - (zvort(i)-Oz);

          dx = xvort_mir - x;
          dy = yvort_mir - yter;
          dz = zvort_mir - z;

          deplacement_vortex(dx,dy,dz,sigma(i),gamma(i),v(face),w(face));


          //***************************************
          // Periodicite : canal plan uniquement
          //***************************************

          if(geom=="channel")
            {

              xvort_per = (xvort(i)-Ox);
              yvort_per = (yvort(i)-Oy);
              if( (zvort(i)-Oz) >0.) zvort_per = -2.*R + (zvort(i)-Oz) ;
              else                            zvort_per =  2.*R + (zvort(i)-Oz);

              dx = xvort_per - x;
              dy = yvort_per - yter;
              dz = zvort_per - z;

              deplacement_vortex(dx,dy,dz,sigma(i),gamma(i),v(face),w(face));

            }

        }//boucle sur vortex


      ////////////////////////////////////////////////////////////////////////////
      // calcul de la fluctuation de vitesse logitudinale par equation de Langevin
      ////////////////////////////////////////////////////////////////////////////


      //     double up = u(face) - u_moy(face);
      double up= u(face);
      if(tps==0.) up=0.;
      double vp;

      if(geom=="channel") vp = w(face);
      else //(geom=="circle")
        {
          double norme = sqrt(x*x+yter*yter+z*z);
          double costheta = ( x*t1x + yter*t1y + z*t1z ) / norme;
          double sintheta = ( x*t2x + yter*t2y + z*t2z ) / norme;
          vp = costheta*v(face) + sintheta*w(face) ;
        }

      ///// processus de Wiener : tirage aleatoire "gaussien" verifiant <dW>=0 et <dW^2>=1 /////////
      /////
      ///// http://www.taygeta.com/random/gaussian.html

      double x1,x2,ybis,Wkp1,dW;

      do
        {
          x1 = 2. * drand48() - 1. ;
          x2 = 2. * drand48() - 1. ;
          ybis = x1 * x1 + x2 * x2;
        }
      while ( ybis >= 1. );

      Wkp1     =  x1 * sqrt( (-1.0 * log( ybis ) ) / ybis );  // On travaille sur dW -> pour assurer <dW^2>=1, on doit modifier le coef
      dW       =  Wkp1 - Wk(face) ;                       // par rapport a la formulation proposee dans 'taygeta'
      Wk(face) =  Wkp1 ;

      /*
        double Wkp1,dW;

        Wkp1     =  2.*drand48()-1.;
        dW       = (Wkp1 - Wk(face))*sqrt(3./2.) ;
        Wk(face) =  Wkp1 ;


        double W,Wp1,dW;
        W     =  2.*drand48()-1.;
        Wp1   =  2.*drand48()-1.;
        dW    = (Wp1 - W)*sqrt(3./2.) ;
      */


      //////////////////////////////////

      double C0,C1,C2;

      C0=14./15.;
      C1=1.8;
      C2=0.6;

      double T=k(face)/eps(face);

      double dt = tps - temps;

      if(dt!=0.) // codage conforme a these Sergent + papier eDF (au coef 2/3 pres vis-a-vis de C2)
        {
          up += dt*( -(C1/(2.*T))*up  + (C2-1.)*dudy(face)*vp + sqrt(C0*eps(face))*dW/dt );

          //        up += dt*( -(C1/(2.*T))*up  + ((2./3.)*C2-1.)*dudy(face)*vp + sqrt(C0*eps(face))*dW/dt );
        }
      /*
             if(dt!=0.) // codage conforme a ce qui est code dans Saturne
             {
             up += dt*(-(1.-(2./3.)*C2)*dudy(face)*vp)  + 2.*sqrt((2./3.)*(C1-1.)*eps(face)*dt)*dW;
             up /= (1.+5.*C1*dt/T);
             }
      */

      //     u(face) = u_moy(face) + up;
      u(face) = up;


      les_val(face,0) = u(face)*nx + v(face)*t1x + w(face)*t2x ;
      les_val(face,1) = u(face)*ny + v(face)*t1y + w(face)*t2y ;
      les_val(face,2) = u(face)*nz + v(face)*t1z + w(face)*t2z ;

      /*
        les_val(face,0) =  gamma(face) ;
        les_val(face,1) =  sigma(face) ;
        les_val(face,2) =  fvort(face) ;

        les_val(face,0) =  k(face)  ;
        les_val(face,1) =  eps(face)  ;
        les_val(face,2) =  u_moy(face)  ;

      */
    }//boucle sur face


  //////////////////////////
  // deplacement des vortex
  //////////////////////////


  double dt = tps - temps;
  temps=tps;


  for (int i=0; i<nb_vortex; i++)
    {
      int face = fvort(i);

      x = xvort(i) + dt * ( v(face)*t1x + w(face)*t2x ); // Les deplacements des vortex ne s'operent
      yter = yvort(i) + dt * ( v(face)*t1y + w(face)*t2y ); // que dans le plan d'entree. D'ou les composantes
      z = zvort(i) + dt * ( v(face)*t1z + w(face)*t2z ); // u.n proscrites

      x -= Ox ;  // ATTENTION : Pour des raisons pratiques,
      yter -= Oy ;  // les coordonnees des points sont exprimees
      z -= Oz ;  // en fonction de l'origine Ox,Oy,Oz de la frontiere


      if( (geom=="circle") && ((x*x+yter*yter+z*z)>=(R*R)) ) //paroi
        {
          // paroi : On ne fait rien - on laisse le vortex a sa position
        }
      else if( (geom=="channel") && (dabs(yter)>R) ) //paroi
        {
          // paroi : On ne fait rien - on laisse le vortex a sa position
        }
      else if( (geom=="channel") && (z>R) ) //periodicite
        {
          xvort(i) = x + Ox;
          yvort(i) = yter + Oy;
          zvort(i) = -2.*R + z + Oz;
        }
      else if( (geom=="channel") && (z<-R) ) //periodicite
        {
          xvort(i) = x + Ox;
          yvort(i) = yter + Oy;
          zvort(i) = 2.*R - z + Oz;
        }
      else
        {
          xvort(i) = x + Ox;
          yvort(i) = yter + Oy;
          zvort(i) = z + Oz;
        }
    }

  ///////////////////////////////////////////////////////
  // fin de vie des vortex - generation de nouveau vortex
  ///////////////////////////////////////////////////////

  for (int i=0; i<nb_vortex; i++)
    {
      tvort(i) -= dt ;

      if(tvort(i)<=0.)// fin de vie du vortex
        {
          /*
            int face = my_rand() % nb_faces;            // random different a chaque nouveau tirage
            //       double rand1 = (my_rand() % 100)/100.;
            double rand2 = (my_rand() % 100)/100.;
            xvort(i)=xv(face,0);
            yvort(i)=xv(face,1);
            zvort(i)=xv(face,2);
            tvort(i)=5.*C_mu*pow(k(face),3./2.)/(eps(face)*u_moy(face));
            if (rand2<0.5) svort(i)=-1.;
            else svort(i)=1.;
          */
          int face = int(nb_faces*drand48());       // random identique a chaque execution
          xvort(i)=xv(face,0);
          yvort(i)=xv(face,1);
          zvort(i)=xv(face,2);
          tvort(i)=5.*C_mu*pow(k(face),3./2.)/(eps(face)*u_moy(face));
          if (drand48()<0.5) svort(i)=-1.;
          else svort(i)=1.;

          Cerr << " Time " << temps << " Vortex " << i << finl;
          Cerr << " Face " << face << " " << xvort(i) << " " <<yvort(i) << " " <<zvort(i) << finl;
          Cerr <<" " << tvort(i) << " " <<svort(i) << finl;
          Cerr << " " << finl;
        }
    }
}
