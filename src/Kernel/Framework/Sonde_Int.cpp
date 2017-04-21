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
// File:        Sonde_Int.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////


#include <Sonde_Int.h>
#include <Postraitement.h>
#include <communications.h>

Implemente_instanciable_sans_constructeur_ni_destructeur(Sonde_Int,"Sonde_Int",Objet_U);
Sonde_Int::Sonde_Int():le_fichier(0) { }


// Description:
//    Imprime le type de l'objet sur un flot de sortie.
// Precondition:
// Parametre: Sortie& s
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
Sortie& Sonde_Int::printOn(Sortie& s ) const
{
  return s << que_suis_je();
}


// Description:
//    Lit les specifications d'une sonde a partir d'un flot d'entree.
//    Format:
//    Sonde_Ints
//     {
//       [nom_sonde nom_champ Periode dts Points n x1 y1 [z1] ... xn yn [zn]
//       [nom_sonde nom_champ Periode dts Segment ns x1 y1 [z1] x2 y2 [z2]
//       ...
//     }
// Precondition: le postraitement doit avoir ete associe
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: donnees de la sonde non definies
// Exception: erreur de format, mot clef inconnus
// Exception: donnees de la sonde pas definies correctement
// Effets de bord:
// Postcondition:
Entree& Sonde_Int::readOn(Entree& is )
{
  assert(mon_post.non_nul());

  Probleme_base& pb = mon_post->probleme();
  Motcle motlu;
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  int nbre_points;

  // initialisation de periode par defaut

  periode = 1.e10;

  // Recherche du tableau sonde
  // Remplissage de la reference au tableau

  is >> motlu;
  Noms liste_noms;
  pb.get_noms_champs_postraitables(liste_noms);
  ////if (pb.comprend_mot(motlu))
  //if (liste_noms.contient(motlu))
  for (int i=0; i<liste_noms.size(); i++)
    {
      if (liste_noms[i]==motlu)
        {
          REF(IntVect) ref_tab;
          ////pb.a_pour_IntVect(motlu,ref_tab);
          if (pb.a_pour_IntVect(motlu,ref_tab))
            mon_tableau = ref_tab;
          else
            {
              Cerr << "The problem does not have the array " << motlu << finl;
              exit();
            }
        }
    }
  /*
      else
      if (!ref_tab.non_nul())
      {
      Cerr << "The array name " << motlu << " is not understood by the problem" << finl;
      exit();
      }
  */

  // Lecture des caracteristiques de la sonde

  IntVect fait(2);

  Motcles les_motcles(6);
  {
    les_motcles[0] = "periode";
    les_motcles[1] = "point";
    les_motcles[2] = "points";
    les_motcles[3] = "segment";
    les_motcles[4] = "plan";
    les_motcles[5] = "volume";
  }

  while ((fait(0) != 1) || (fait(1) != 1))
    {
      is >> motlu;
      if (motlu == accolade_fermee)
        {
          Cerr << "Error while reading the probe " << nom_ <<finl;
          Cerr << "The data of the probe were not defined" << finl;
          exit();
        }
      int rang=les_motcles.search(motlu);
      if (rang == -1)
        {
          Cerr << "Error while reading the probe " << nom_ <<finl;
          Cerr << motlu << " is not understood; the keywords understood are : " << finl;
          Cerr << les_motcles;
          exit();
        }

      switch(rang)
        {
        case 0:
          {
            is >> periode;
            break;
          }
        case 1:
        case 2:
          {
            rang = 1;
            dim = 0;
            is >> nbre_points;
            les_positions_.resize(nbre_points,dimension);

            for (int i=0; i<nbre_points; i++)
              for (int j=0; j<dimension; j++)
                is >> les_positions_(i,j);

            break;
          }
        case 3:
          {
            rang = 1;
            dim = 1;
            DoubleVect origine(dimension);
            DoubleVect extremite(dimension);
            DoubleVect dx(dimension);
            int i=0,j=0;
            is >> nbre_points;
            les_positions_.resize(nbre_points,dimension);

            for (; i<dimension; i++)
              is >> origine(i);
            for (i=0; i<dimension; i++)
              is >> extremite(i);
            for (i=0; i<dimension; i++)
              dx(i)=(extremite(i)-origine(i))/(nbre_points-1);
            for (i=0; i<nbre_points; i++)
              for (j=0; j<dimension; j++)
                les_positions_(i,j)=origine(j)+i*dx(j);
            break;
          }
        case 4:
          {
            rang = 1;
            dim = 2;
            DoubleVect origine(dimension);
            DoubleVect extremite1(dimension);
            DoubleVect extremite2(dimension);
            DoubleVect dx1(dimension);
            DoubleVect dx2(dimension);
            int i=0,j=0,k=0;
            is >> nbre_points1;
            is >> nbre_points2;
            nbre_points=nbre_points1*nbre_points2;
            les_positions_.resize(nbre_points,dimension);

            for (; i<dimension; i++)
              is >> origine(i);
            for (i=0; i<dimension; i++)
              is >> extremite1(i);
            for (i=0; i<dimension; i++)
              is >> extremite2(i);
            for (i=0; i<dimension; i++)
              dx1(i)=(extremite1(i)-origine(i))/(nbre_points1-1);
            for (i=0; i<dimension; i++)
              dx2(i)=(extremite2(i)-origine(i))/(nbre_points2-1);
            for (i=0; i<nbre_points1; i++)
              for (j=0; j<nbre_points2; j++)
                for (k=0; k<dimension; k++)
                  les_positions_(i*nbre_points2+j,k)=origine(k)+i*dx1(k)+j*dx2(k);
            break;
          }
        case 5:
          {
            rang = 1;
            dim = 3;
            DoubleVect origine(dimension);
            DoubleVect extremite1(dimension);
            DoubleVect extremite2(dimension);
            DoubleVect extremite3(dimension);
            DoubleVect dx1(dimension);
            DoubleVect dx2(dimension);
            DoubleVect dx3(dimension);
            int i=0,j=0,k=0;
            is >> nbre_points1;
            is >> nbre_points2;
            is >> nbre_points3;
            nbre_points=nbre_points1*nbre_points2*nbre_points3;
            les_positions_.resize(nbre_points,dimension);

            for (; i<dimension; i++)
              is >> origine(i);
            for (i=0; i<dimension; i++)
              is >> extremite1(i);
            for (i=0; i<dimension; i++)
              is >> extremite2(i);
            for (i=0; i<dimension; i++)
              is >> extremite3(i);
            for (i=0; i<dimension; i++)
              dx1(i)=(extremite1(i)-origine(i))/(nbre_points1-1);
            for (i=0; i<dimension; i++)
              dx2(i)=(extremite2(i)-origine(i))/(nbre_points2-1);
            for (i=0; i<dimension; i++)
              dx3(i)=(extremite3(i)-origine(i))/(nbre_points3-1);
            for (i=0; i<nbre_points1; i++)
              for (j=0; j<nbre_points2; j++)
                for (int m=0; m<nbre_points3; m++)
                  for (k=0; k<dimension; k++)
                    les_positions_(i*nbre_points2*nbre_points1+j*nbre_points1+m,k)=origine(k)+i*dx1(k)+j*dx2(k)+m*dx3(k);
            break;
          }
        default:
          {
            Cerr << motlu <<"is not yet understood!" << finl;
            exit();
          }
        }
      fait(rang) = 1;
    }

  if ( (fait[0] == 0) || (fait[1] == 0) )
    {
      Cerr << "Error while reading the probe " << nom_ << finl;
      Cerr << "The data of the probe have not been properly defined" << finl;
      exit();
    }

  const Zone& la_zone =pb.domaine().zone(0);
  initialiser(la_zone);
  return is;
}


// Description:
//    Associer le postraitement a la sonde.
// Precondition:
// Parametre: Postraitement& le_post
//    Signification: le postraitement a associer
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la sonde a un postraitement associe
void Sonde_Int::associer_post(const Postraitement& le_post)
{
  mon_post=le_post;
  if (mon_post->noms_fichiers_sondes().contient(nom_)==0)
    mon_post->noms_fichiers_sondes().add(nom_);
  else
    {
      Cerr << "The filename " << nom_ << " is used several times for probes." << finl;
      Cerr << "A probe must have a unique filename to avoid writing conflicts." << finl;
      exit();
    }

}


// Description:
//    Initialise la sonde. Dimensionne les tableaux,
//    de valeurs, verifie si les points specifies sont
//    bien dans la zone de calcul.
// Precondition:
// Parametre: Zone& zone_geom
//    Signification: la zone de calcul qui sera sondee
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception: point de sondage en dehors de la zone de calcul
// Effets de bord:
// Postcondition: la sonde est initialisee
void Sonde_Int::initialiser(const Zone& zone_geom)
{
  nb_bip = 0.;
  int nbre_points = les_positions_.dimension(0);
  elem_.resize(nbre_points);
  zone_geom.chercher_elements(les_positions_,elem_);
  for (int i=0; i<nbre_points; i++)
    if (mp_max(elem_[i])==-1 && je_suis_maitre())
      {
        Cerr << "The point number " << i+1 << " of probe " << nom_ << finl;
        Cerr << "is outside of the computational domain" << finl;
      }
  if (sub_type(IntTab,mon_tableau.valeur()))
    {
      const IntTab& tab_val = ref_cast(IntTab,mon_tableau.valeur());
      valeurs.resize(nbre_points,tab_val.dimension(1));
    }
  else
    valeurs.resize(nbre_points);

}


// Description:
//    Ouvre le fichier associe a la sonde. (*.son)
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
// Postcondition: le fichier associe est ouvert
void Sonde_Int::ouvrir_fichier()
{
  if(je_suis_maitre())
    {
      if( le_fichier)
        delete le_fichier;
      Nom nom_fich(nom_du_cas());
      nom_fich += "_";
      nom_fich += nom_;
      nom_fich += ".son";
      le_fichier=new SFichier(nom_fich);
      SFichier& s = *le_fichier;
      s.setf(ios::scientific);
      s.precision(8);

      // Ecriture de l'en tete des fichiers sondes :
      if (dim==0 || dim==1)
        {
          const DoubleTab& p=les_positions();
          int nbre_points = les_positions_.dimension(0);
          s << "# " << nom_fich << finl;
          s << "# Temps";
          for(int i=0; i<nbre_points; i++)
            {
              s << " x=" << p(i,0) << " y=" << p(i,1) ;
              if (dimension==3) fichier() << " z=" << p(i,2) ;
            }
          s << finl;
        }
      if (dim==2)
        {
          s << "TRUST   Version1  01/09/96" << finl;
          s << nom_du_cas() << finl;
          s << "TRUST" << finl;
          s << "GRILLE" << finl;
          Nom nom_grille("Grille");
          Nom nom_topologie("Topologie");
          nom_grille += "_";
          nom_grille += nom_;
          nom_topologie += "_";
          nom_topologie += nom_;
          int k,kn;
          int nbre_points = les_positions_.dimension(0);
          const DoubleTab& p=les_positions();
          double xn,yn,zn,norme;
          if (dimension==3)
            {
              xn=(p(1,2)-p(0,2))*(p(nbre_points1,1)
                                  -p(0,1))-(p(1,1)-p(0,1))*(p(nbre_points1,2)-p(0,2));
              yn=(p(1,0)-p(0,0))*(p(nbre_points1,2)-p(0,2))
                 -(p(1,2)-p(0,2))*(p(nbre_points1,0)-p(0,0));
            }
          else// if (dimension==2)
            {
              assert(dimension==2);
              xn=0.;
              yn=0.;
            }
          zn=(p(1,1)-p(0,1))*(p(nbre_points1,0)-p(0,0))
             -(p(1,0)-p(0,0))*(p(nbre_points1,1)-p(0,1));
          norme=dabs(xn)+dabs(yn)+dabs(zn);
          xn/=norme;
          yn/=norme;
          zn/=norme;
          s << nom_grille << " 3 " << 2*nbre_points << finl;
          int i;
          for(i=0; i<nbre_points; i++)
            {
              s << p(i,0) << " " << p(i,1) ;
              if (dimension==3) fichier() << " " << p(i,2) << finl;
              else if (dimension==2) s << " 0." << finl;
            }
          for(i=0; i<nbre_points; i++)
            {
              s << p(i,0)+xn << " " << p(i,1)+yn ;
              if (dimension==3) s << " " << p(i,2)+zn << finl;
              else if (dimension==2) s << " " << zn << finl;
            }
          s << "TOPOLOGIE" << finl;
          s << nom_topologie << " " << nom_grille << finl;
          s << "MAILLE" << finl;
          s << (nbre_points1-1)*(nbre_points2-1) << finl;
          for(int j=0; j<nbre_points2-1; j++)
            for(i=0; i<nbre_points1-1; i++)
              {
                k=j*nbre_points1+i+1;
                kn=k+nbre_points;
                s << "VOXEL8 " << k << " " << k+1 << " ";
                s << k+nbre_points1 << " " << k+nbre_points1+1;
                s << " " << kn << " " << kn+1 << " " << kn+nbre_points1;
                s << " " << kn+nbre_points1+1 << finl;
              }
          s << "FACE" << finl;
          s << "0" << finl;
        }
      if (dim==3)
        {
          s << "TRUST   Version1  01/09/96" << finl;
          s << nom_du_cas() << finl;
          s << "TRUST" << finl;
          s << "GRILLE" << finl;
          Nom nom_grille("Grille");
          Nom nom_topologie("Topologie");
          nom_grille += "_";
          nom_grille += nom_;
          nom_topologie += "_";
          nom_topologie += nom_;
          int k,kn;
          int nbre_points = les_positions_.dimension(0);
          const DoubleTab& p=les_positions();
          s << nom_grille << " 3 " << nbre_points << finl;
          int i;
          for(i=0; i<nbre_points; i++)
            {
              s << p(i,0) << " " << p(i,1) ;
              s << " " << p(i,2) << finl;
            }
          s << "TOPOLOGIE" << finl;
          s << nom_topologie << " " << nom_grille << finl;
          s << "MAILLE" << finl;
          s << (nbre_points1-1)*(nbre_points2-1)*(nbre_points3-1) << finl;
          for(int m=0; m<nbre_points3-1; m++)
            for(int j=0; j<nbre_points2-1; j++)
              for(i=0; i<nbre_points1-1; i++)
                {
                  k=m*nbre_points2*nbre_points1+j*nbre_points1+i+1;
                  kn=k+nbre_points2*nbre_points1;
                  s << "VOXEL8 " << k << " " << k+1 << " ";
                  s << k+nbre_points1 << " " << k+nbre_points1+1;
                  s << " " << kn << " " << kn+1 << " " << kn+nbre_points1;
                  s << " " << kn+nbre_points1+1 << finl;
                }
          s << "FACE" << finl;
          s << "0" << finl;
        }
    }
}


// Description:
//    Effectue une mise a jour en temps de la sonde
//    effectue le postraitement.
// Precondition:
// Parametre: double temps
//    Signification: le temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: double tinit
//    Signification: le temps initial de la sonde
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Sonde_Int::mettre_a_jour(double un_temps, double tinit)
{
  double nb;
  modf((un_temps-tinit)/periode, &nb);
  if (nb>nb_bip)
    {
      nb_bip=nb;
      postraiter(un_temps);
    }
}


// Description:
//    Effectue un postraitement.
//    Imprime les valeurs du tableau aux positions demandees
//    sur le fichier associe.
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
void Sonde_Int::postraiter(double un_temps)
{
  if (sub_type(IntTab,mon_tableau.valeur()))
    {
      const IntTab& val_tab= ref_cast(IntTab,mon_tableau.valeur());
      int nval = val_tab.dimension(1);
      //int i,j;
      // int nb_compo=valeurs.nb_dim();
      int nbre_points = les_positions_.dimension(0);

      for (int i=0; i<nbre_points; i++)
        for (int j=0; j<nval; j++)
          valeurs(i,j) = val_tab(elem_(i),j);

      if(je_suis_maitre())
        {
          int p;
          int nbproc = Process::nproc();
          IntTab valeurs_pe;
          int val_max;
          for(p=1; p<nbproc; p++)
            {
              recevoir(valeurs_pe,p,0,p);
              int n1=valeurs.dimension(0);
              int n2=valeurs.dimension(1);
              int k;
              for(int i=0; i<n1; i++)
                for(k=0; k<n2; k++)
                  {
#ifdef INT_is_64_
                    val_max = max(labs(valeurs(i,k)),labs(valeurs_pe(i,k)));
                    if(val_max==(labs(valeurs_pe(i,k))))
                      valeurs(i,k)=valeurs_pe(i,k);
#else
                    val_max = max(abs(valeurs(i,k)),abs(valeurs_pe(i,k)));
                    if(val_max==(abs(valeurs_pe(i,k))))
                      valeurs(i,k)=valeurs_pe(i,k);
#endif
                  }
            }

          if (dim==0 || dim==1)
            {
              fichier() << un_temps;
              for(int i=0; i<valeurs.dimension(0); i++)
                for(int k=0; k<valeurs.dimension(1); k++)
                  fichier() << " " << valeurs(i,k);
              fichier() << finl;
            }

          // Pour les sondes type plan, impression au format lml :
          // num_sommet comp1 [comp2] [comp3]
          // et dans la troisieme direction :
          else if (dim==2 || dim==3)
            {
              Nom unite;
              Nom nom_topologie("Topologie");
              nom_topologie += "_";
              nom_topologie += nom_;
              fichier() << "TEMPS " << un_temps << "\n";
              fichier() << "CHAMPOINT " << nom_ << " " << nom_topologie
                        << " " << un_temps << "\n";
              fichier() << nom_ << " " << nval << " " << unite << "\n";
              int nbp=nbre_points;
              if (dim==2) nbp*=2;
              fichier() << "type1 " << nbp << "\n";
              int i;
              for(i=0; i<nbre_points; i++)
                {
                  fichier() << i+1;
                  for(int j=0; j<valeurs.dimension(1); j++)
                    fichier() << " " << valeurs(i,j);
                  // Pour ne pas flusher :
                  fichier() << "\n";
                }
              // Pour le 2D, on rajoute une direction
              if (dim==2)
                {
                  for(i=0; i<nbre_points; i++)
                    {
                      fichier() << nbre_points+i+1;
                      for(int j=0; j<valeurs.dimension(1); j++)
                        fichier() << " " << valeurs(i,j);
                      // Pour ne pas flusher :
                      fichier() << "\n";
                    }
                }
            }
          fichier().flush();
        }
      else
        envoyer(valeurs,Process::me(),0,Process::me());
    }
  else
    {
      const IntVect& val_tab= ref_cast(IntVect,mon_tableau.valeur());
      int nval =1;


      //int nb_compo=valeurs.nb_dim();
      int nbre_points = les_positions_.dimension(0);

      for (int i=0; i<nbre_points; i++)
        valeurs(i) = val_tab(elem_(i));

      if(je_suis_maitre())
        {
          int p;
          int nbproc = Process::nproc();
          IntTab valeurs_pe;
          int val_max;
          for(p=1; p<nbproc; p++)
            {
              recevoir(valeurs_pe,p,0,p);

              for(int i=0; i<valeurs.dimension(0); i++)
                {
#ifdef INT_is_64_
                  val_max = max(labs(valeurs(i)),labs(valeurs_pe(i)));
                  if(val_max==(labs(valeurs_pe(i))))
                    valeurs(i)=valeurs_pe(i);
#else
                  val_max = max(abs(valeurs(i)),abs(valeurs_pe(i)));
                  if(val_max==(abs(valeurs_pe(i))))
                    valeurs(i)=valeurs_pe(i);
#endif
                }
            }
          if (dim==0 || dim==1)
            {
              fichier() << un_temps;
              for(int i=0; i<valeurs.dimension(0); i++)
                fichier() << " " << valeurs(i);
              fichier() << finl;
            }

          // Pour les sondes type plan, impression au format lml :
          // num_sommet comp1 [comp2] [comp3]
          // et dans la troisieme direction :
          else if (dim==2 || dim==3)
            {
              Nom unite;
              Nom nom_topologie("Topologie");
              nom_topologie += "_";
              nom_topologie += nom_;
              fichier() << "TEMPS " << un_temps << "\n";
              fichier() << "CHAMPOINT " << nom_ << " " << nom_topologie
                        << " " << un_temps << "\n";
              fichier() << nom_ << " " << nval << " " << unite << "\n";
              int nbp=nbre_points;
              if (dim==2) nbp*=2;
              fichier() << "type0 " << nbp << "\n";
              //int i;
              for(int i=0; i<nbre_points; i++)
                {
                  fichier() << i+1;
                  fichier() << " " << valeurs(i);
                  // Pour ne pas flusher :
                  fichier() << "\n";
                }
              // Pour le 2D, on rajoute une direction
              if (dim==2)
                {
                  for(int i=0; i<nbre_points; i++)
                    {
                      fichier() << nbre_points+i+1;
                      fichier() << " " << valeurs(i);
                      // Pour ne pas flusher :
                      fichier() << "\n";
                    }
                }
            }
          fichier().flush();
        }
      else
        envoyer(valeurs,Process::me(),0,Process::me());
    }

}
