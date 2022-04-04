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
// File:        Loi_horaire.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#include <Loi_horaire.h>
#include <Param.h>
#include <Ref_Loi_horaire.h>
#include <EChaine.h>
#include <Schema_Temps_base.h>
#include <Probleme_base.h>
#include <SFichier.h>
#include <sys/stat.h>

Implemente_instanciable(Loi_horaire,"Loi_horaire",Objet_U);
Implemente_ref(Loi_horaire);

// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& Loi_horaire::printOn(Sortie& os) const
{
  return os;
}

// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Loi_horaire::readOn(Entree& is)
{
  // Initialisations
  verification_derivee_=1;
  impr_=1;

  EChaine x(dimension==3?"3 0. 0. 0.":"2 0. 0.");
  x >> position_;
  EChaine v(dimension==3?"3 0. 0. 0.":"2 0. 0.");
  v >> vitesse_;
  EChaine r(dimension==3?"9 1. 0. 0. 0. 1. 0. 0. 0. 1.":"4 1. 0. 0. 1.");
  r >> rotation_;
  EChaine drdt(dimension==3?"9 0. 0. 0. 0. 0. 0. 0. 0. 0.":"4 0. 0. 0. 0.");
  drdt >> derivee_rotation_;

  // Lecture de jeu de donnees
  Param param(que_suis_je());
  param.ajouter("position",&position_);                                // Vecteur position
  param.ajouter("vitesse",&vitesse_);                // Vecteur vitesse
  param.ajouter("rotation",&rotation_);                                        // Matrice de passage
  param.ajouter("derivee_rotation",&derivee_rotation_);                        // Derivee matrice de passage
  param.ajouter("verification_derivee",&verification_derivee_);
  param.ajouter("impr",&impr_);
  param.lire_avec_accolades_depuis(is);

  // Verification de ce qui a ete lu: ( pourquoi le_nom() retourne neant ???
  if (position_.valeurs().size_array()!=dimension)
    {
      Cerr << "The position vector of the schedule law " << le_nom() << " must have " << dimension << " components." << finl;
      Process::exit();
    }
  if (vitesse_.valeurs().size_array()!=dimension)
    {
      Cerr << "The velocity vector of the schedule law " << le_nom() << " must have " << dimension << " components." << finl;
      Process::exit();
    }
  if (rotation_.valeurs().size_array()!=dimension*dimension)
    {
      Cerr << "The matrix rotation of the schedule law " << le_nom() << " must be read" << finl;
      Cerr << "as a vector of " << dimension*dimension << " components." << finl;
      Process::exit();
    }
  if (derivee_rotation_.valeurs().size_array()!=dimension*dimension)
    {
      Cerr << "The matrix derivee_rotation of the schedule law " << le_nom() << " must be read" << finl;
      Cerr << "as a vector of " << dimension*dimension << " components." << finl;
      Process::exit();
    }
  return is;
}

inline void check(const DoubleTab& mat)
{
  // Champ_Fonc_t DoubleTab(1,nb_comp)
  if (mat.dimension(0)!=1)
    {
      Cerr << "Problem in Loi_horaire::check" << finl;
      Cerr << "Format of the array of values of Champ_Fonc_t" << finl;
      Cerr << "Contact TRUST support." << finl;
      Process::exit();
    }
}

// Produit Matrice*Vecteur stockes dans des tableaux
inline void multiplie(const DoubleTab& matrice, ArrOfDouble& vecteur)
{
  check(matrice);
  ArrOfDouble x(vecteur);
  vecteur=0;
  int dimension = vecteur.size_array();
  for (int i=0; i<dimension; i++)
    for (int j=0; j<dimension; j++)
      vecteur[i] += matrice(0,i*dimension+j) * x[j];
}

// Produit Inverse(Matrice)*Vecteur dans des tableaux
inline void inverse(const DoubleTab& mat, ArrOfDouble& x)
{
  check(mat);
  int dimension = x.size_array();
  if (dimension==2)
    {
      const double a=mat(0,0);
      const double b=mat(0,1);
      const double c=mat(0,2);
      const double d=mat(0,3);
      double det=a*d-b*c;
      if (det==0)
        {
          Cerr << "Matrix of order 2 not invertible in Loi_horaire::inverse" << finl;
          Cerr << mat << finl;
          Process::exit();
        }
      double y0=(d*x[0]-b*x[1])/det;
      double y1=(-c*x[0]+a*x[1])/det;
      x[0]=y0;
      x[1]=y1;
    }
  else if (dimension==3)
    {
      const double a=mat(0,0);
      const double b=mat(0,1);
      const double c=mat(0,2);
      const double d=mat(0,3);
      const double e=mat(0,4);
      const double f=mat(0,5);
      const double g=mat(0,6);
      const double h=mat(0,7);
      const double i=mat(0,8);

      double det=a*(e*i-f*h)
                 -b*(d*i-f*g)
                 +c*(d*h-e*g);
      if (det==0)
        {
          Cerr << "Matrix of order 3 not invertible in Loi_horaire::inverse" << finl;
          Cerr << mat << finl;
          Process::exit();
        }
      double y0=((e*i-f*h)*x[0]
                 +(c*h-b*i)*x[1]
                 +(b*f-c*e)*x[2])/det;
      double y1=((f*g-d*i)*x[0]
                 +(a*i-c*g)*x[1]
                 +(c*d-a*f)*x[2])/det;
      double y2=((d*h-e*g)*x[0]
                 +(b*g-a*h)*x[1]
                 +(a*e-b*d)*x[2])/det;
      x[0]=y0;
      x[1]=y1;
      x[2]=y2;
    }
  else
    {
      Cerr << "Case not provided in Loi_horaire::inverse()" << finl;
      Process::exit();
    }
}

/* Renvoie un tableau contenant la position xM(t) d'un point M au temps t
   en fonction du temps t0 et de sa position xM(t0) au temps t0.
   Pour cela, on utilisera les relations suivantes:
   xM(t)=xG(t)+R(t)(xM(0)-xG(0))
   xM(t0)=xG(t0)+R(t0)(xM(0)-xG(0))
   On a:
   xM(0)-xG(0)=R-1(t0)(xM(t0)-xG(t0))
   Donc:
   xM(t) = xG(t)+R(t)R-1(t0)(xM(t0)-xG(t0))
*/
ArrOfDouble Loi_horaire::position(const double& t, const double& t0, const ArrOfDouble& xMt0)
{
  ArrOfDouble xMt(xMt0);                // xM(t0)
  position_.me_calculer(t0);                // xG(t0)
  xMt -= position_.valeurs();                // xM(t0)-xG(t0)
  rotation_.me_calculer(t0);                // R(t0)
  inverse(rotation_.valeurs(),xMt);        // R-1(t0)(xM(t0)-xG(t0))
  rotation_.me_calculer(t);                // R(t)
  multiplie(rotation_.valeurs(),xMt);        // R(t)R-1(t0)(xM(t0)-xG(t0))
  position_.me_calculer(t);                // xG(t)
  xMt += position_.valeurs();                // xM(t) = xG(t)+R(t)R-1(t0)(xM(t0)-xG(t0))
  return xMt;
}

/* Renvoie un tableau contenant la vitesse vM(t) d'un point M au temps t
   en fonction de sa position xM(t) au temps t. Pour connaitre cette vitesse du point M,
   on utilise les relations suivantes:
   xM(t)=xG(t)+R(t)(xM(0)-xG(0))
   vM(t)=vG(t)+R'(t)(xM(0)-xG(0))
   On a:
   xM(0)-xG(0)=R-1(t)(xM(t)-xG(t))
   Donc:
   vM(t) = vG(t)+R'(t)R-1(t)(xM(t)-xG(t))
*/
ArrOfDouble Loi_horaire::vitesse(const double& t, const ArrOfDouble& xMt)
{
  ArrOfDouble vMt(xMt);                        // xM(t)
  position_.me_calculer(t);                        // xG(t)
  vMt -= position_.valeurs();                        // xM(t)-xG(t)
  rotation_.me_calculer(t);                        // R(t)
  inverse(rotation_.valeurs(),vMt);                // R-1(t)(xM(t)-xG(t))
  derivee_rotation_.me_calculer(t);                // R'(t)
  multiplie(derivee_rotation_.valeurs(),vMt);        // R'(t)R-1(t)(xM(t)-xG(t))
  vitesse_.me_calculer(t);                        // vG(t)
  vMt += vitesse_.valeurs();                        // vM(t) = vG(t)+R'(t)R-1(t)(xM(t)-xG(t))
  return vMt;
}

// Verification de la coherence des derivees vitesse et derivee_rotation
// par rapport a position et rotation. On utilise un developpement limite d'ordre 2
// |f(t+dt)-f(t)-dt*f'(t)|=|0.5*f''(t)*dt*dt+O(dt*dt*dt)|>|10*f''(t)*dt*dt|
// Une erreur sur des expressions de derivee seconde nulle ne sera donc pas detectee...
void Loi_horaire::verifier_derivee(const double& t)
{
  if (verification_derivee_)
    {
      double dt=0.001*t;
      DoubleVect err_t(position_.valeurs());
      position_.me_calculer(t+dt);                        // xG(t+dt)
      err_t=position_.valeurs();                        // xG(t+dt)
      position_.me_calculer(t);                                // xG(t)
      err_t-=position_.valeurs();                        // xG(t+dt)-xG(t)
      vitesse_.me_calculer(t);                                // vG(t)
      err_t.ajoute(-dt, vitesse_.valeurs());                        // xG(t+dt)-xG(t)-dt*vG(t)
      // Evaluation d'un seuil avec la derivee seconde xG'' cad vG'
      ArrOfDouble seuil_t(position_.valeurs());
      vitesse_.me_calculer(t+dt);                        // vG(t+dt)
      seuil_t=vitesse_.valeurs();
      vitesse_.me_calculer(t);                                // vG(t)
      seuil_t-=vitesse_.valeurs();
      seuil_t*=100*dt;                                        // ~vG'(t)*dt*dt
      int n=err_t.size_array();
      for (int i=0; i<n; i++)
        {
          double seuil=std::fabs(seuil_t[i]);
          if (!est_egal(seuil,0) && std::fabs(err_t(i))>seuil)
            {
              if (Process::je_suis_maitre())
                {
                  Cerr << "At time " << t << ", inconsistency in the schedule law " << le_nom() << finl;
                  Cerr << "The expression of the component " << i << " of velocity does not appear" << finl;
                  Cerr << "to be the time derivative of the position expression." << finl;
                  Cerr << "Verify the expressions of this schedule law in your data set." << finl;
                  Cerr << "If the expressions are correct after all, add the option 'verification_derivee 0'" << finl;
                  Cerr << "in the law to not to make this verification." << finl;
                }
              Process::exit();
            }
        }
      DoubleTab err_r(rotation_.valeurs());
      rotation_.me_calculer(t+dt);                        // R(t+dt)
      err_r=rotation_.valeurs();                        // R(t+dt)
      rotation_.me_calculer(t);                                // R(t)
      err_r-=rotation_.valeurs();                        // R(t+dt)-R(t)
      derivee_rotation_.me_calculer(t);                        // R'(t)
      err_r.ajoute(-dt, derivee_rotation_.valeurs());                // R(t+dt)-R(t)-dt*R'(t)
      // Evaluation d'un seuil avec la derivee seconde de R cad R''
      DoubleTab seuil_r(rotation_.valeurs());
      derivee_rotation_.me_calculer(t+dt);                // R'(t+dt)
      seuil_r=derivee_rotation_.valeurs();
      derivee_rotation_.me_calculer(t);                        // R'(t)
      seuil_r-=derivee_rotation_.valeurs();                // R'(t+dt)-R'(t)
      seuil_r*=100*dt;                                        // ~R''(t)*dt*dt
      int ni=err_r.dimension(0);
      int nj=err_r.dimension(1);
      for (int i=0; i<ni; i++)
        for (int j=0; j<nj; j++)
          {
            double seuil=std::fabs(seuil_r(i,j));
            if (!est_egal(seuil,0) && std::fabs(err_r(i,j))>seuil)
              {
                if (Process::je_suis_maitre())
                  {
                    Cerr << "At time " << t << ", inconsistency in the schedule law " << le_nom() << finl;
                    Cerr << "The expression of the component " << i*ni+j << " of derivee_rotation does not appear" << finl;
                    Cerr << "to be the time derivative of the rotation expression." << finl;
                    Cerr << "Verify the expressions of this schedule law in your data set" << finl;
                    Cerr << "or add the option 'verification_derivee 0' in this law." << finl;
                  }
                Process::exit();
              }
          }
    }
}

void Loi_horaire::imprimer(const Schema_Temps_base& sch, const ArrOfDouble& coord_barycentre)
{
  if (Process::je_suis_maitre())
    {
      // Ouverture du fichier
      Nom nom_fichier(nom_du_cas());
      nom_fichier+="_loi_horaire_";
      nom_fichier+=le_nom();
      nom_fichier+=".out";
      SFichier fic; // * os=NULL;
      struct stat f;
      // On cree le fichier a la premiere impression avec l'en tete ou si le fichier n'existe pas
      if (stat(nom_fichier,&f) || (sch.nb_impr()==1 && !sch.pb_base().reprise_effectuee()))
        {
          tester(sch);
          fic.ouvrir(nom_fichier);
          // Ecriture en tete
          fic << "# (xG,yG,...):Position du centre de gravite dont le mouvement est defini par la loi horaire " << le_nom() << finl;
          fic << "# (moy(xi),moy(yi),...):Position du barycentre des noeuds du maillage de l'interface." << finl;
          fic << "# Les trajectoires de ces 2 points doivent etre proches." << finl;
          fic << "# Pour visualiser dans gnuplot ces trajectoires, faire: " << finl;
          if (dimension==3)
            {
              fic << "# set param;splot '" << nom_fichier << "' using 2:3:4 with lines,'" << nom_fichier << "' using 5:6:7 with points" << finl;
              fic << "# Temps     xG(t)     yG(t)     zG(t)     moy(xi)   moy(yi)   moy(zi)" << finl;
            }
          else
            {
              fic << "# set param;plot '" << nom_fichier << "' using 2:3 with lines,'" << nom_fichier << "' using 4:5 with points" << finl;
              fic << "# Temps     xG(t)     yG(t)     moy(xi)   moy(yi)" << finl;
            }
        }
      else
        fic.ouvrir(nom_fichier,ios::app);

      fic.precision(sch.precision_impr());
      fic.setf(ios::scientific);
      // Ecriture de t,xG(t),coord_barycentre
      double t=sch.temps_courant();
      fic << t;
      position_.me_calculer(t);
      for (int i=0; i<dimension; i++)
        fic << " " << position_.valeurs()(0,i);        // xG(t)
      for (int i=0; i<dimension; i++)
        fic << " " << coord_barycentre[i];                 // Barycentre des noeuds de l'interface
      fic << finl;
      // Fermeture du fichier
      fic.close();
    }
}

// Tests divers de la classe
void Loi_horaire::tester(const Schema_Temps_base& sch)
{
  // Test de produit et d'inversion de matrice
  for (int dim=2; dim<=3; dim++)
    {
      DoubleTab mat(1,dim*dim);
      for (int i=0; i<dim; i++)
        for (int j=0; j<dim; j++)
          mat(0,i*dim+j)=(i>j?1+i+j:-2*j);
      ArrOfDouble vect(dim);
      for (int i=0; i<dim; i++)
        vect[i]=i;
      ArrOfDouble tmp(vect);
      multiplie(mat,vect);
      inverse(mat,vect);
      for (int i=0; i<dim; i++)
        if (!est_egal(tmp[i],vect[i]))
          {
            Cerr << "Loi_horaire::inverse is not validated for dim=" << dim << finl;
            Cerr << "A=" << mat << finl;
            Cerr << "x=" << tmp << finl;
            Cerr << "Inv(A)Ax=" << vect << finl;
            Cerr << "Contact TRUST support." << finl;
            Process::exit();
          }
    }
  // Verification entre tinit et tmax
  double tinit=sch.temps_init();
  double tmax=sch.temps_max();
  // Test des derivees:
  int n=100;
  for (int i=1; i<n; i++)
    {
      double temps=tinit+(tmax-tinit)*(i+1)/n;
      //Cerr << "Verification a t=" << temps << finl;
      verifier_derivee(temps);
    }
  // Test de position et vitesse:
  double t=0.5*(tinit+tmax);
  double eps=1e-8;
  double dt=eps*t;
  ArrOfDouble pos(dimension);
  pos[0]=dimension*2;
  pos[1]=dimension*3;
  if (dimension==3) pos[2]=-dimension;
  // Verification que position(t,t,pos)=pos
  ArrOfDouble tmp(position(t,t,pos));
  for (int i=0; i<dimension; i++)
    if (!est_egal(tmp[i],pos[i]))
      {
        Cerr << "Loi_horaire::position is not validated for pos=position(t,t,pos):" << finl;
        Cerr << pos << finl;
        Cerr << tmp << finl;
        Cerr << "Contact TRUST support." << finl;
        Process::exit();
      }
  // Verification que vitesse(t,pos)~(position(t+dt,t,pos)-pos)/dt;
  tmp=position(t+dt,t,pos);
  tmp-=pos;
  tmp/=dt;
  ArrOfDouble vit(vitesse(t,pos));
  ArrOfDouble relative_error(vit);
  relative_error-=tmp;
  relative_error/=(norme_array(tmp)!=0?norme_array(tmp):1);
  for (int i=0; i<dimension; i++)
    if (!est_egal(relative_error[i],0,0.001))
      {
        Cerr << "Loi_horaire::position is not validated for velocity(t,pos):" << finl;
        Cerr << "t=" << t << finl;
        Cerr << "dt=" << dt << finl;
        Cerr << "vit=" << vit << finl;
        Cerr << "tmp=" << tmp << finl;
        Cerr << "relative_error=" << relative_error << finl;
        Cerr << "Contact TRUST support." << finl;
        Process::exit();
      }
  Cerr << "Loi_horaire::tester() OK" << finl;
}

