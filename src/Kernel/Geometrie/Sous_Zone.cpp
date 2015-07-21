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
// File:        Sous_Zone.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/41
//
//////////////////////////////////////////////////////////////////////////////

#include <Sous_Zone.h>
#include <Domaine.h>
#include <Polynomes.h>
#include <LecFicDistribue.h>
#include <communications.h>
#include <Parser_U.h>
#include <Interprete.h>

Implemente_instanciable(Sous_Zone,"Sous_Zone",Objet_U);


// Description:
//    Ecrit la liste des polyedres de la sous-zone
//    sur un flot de sortie.
//    Format:
//    Liste n1 n2 .. Ni
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
Sortie& Sous_Zone::printOn(Sortie& os) const
{
  os << "{" << finl;
  os << "Liste" << finl;
  os << les_polys_;
  os << "}" << finl;
  return os;
}


// Description:
//    Lit les specifications d'une sous-zone dans le jeu
//    de donnee a partir d'un flot d'entree.
//    Format:
//     { Rectangle Origine x0 y0 Cotes lx ly } en dimension 2
//     { Boite Origine x0 y0 z0 Cotes lx ly lz} en dimension 3
//      ou
//     { Liste n n1  ni   nn }
//      ou
//     { Intervalle n1 n2 }
//      ou
//     { Polynomes {bloc_lecture_poly1 et bloc_lecture_poly_i
//                  et bloc_lecture_poly_n}
//     }
// Precondition: un domaine doit avoir ete associe a la zone
//               dotn on definit une sous-zone
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Exception: mot clef non reconnu
// Exception: mot clef "et" ou "}" attendu
// Exception: En dimension 2, il faut utiliser Rectangle
// Exception: mot clef "Origine" attendu
// Exception: mot clef "Cotes" attendu
// Exception: En dimension 3, il faut utiliser Boite
// Exception: Erreur TRUST (mot clef reconnu non prevu)
// Exception: accolade fermante attendue
// Effets de bord:
// Postcondition:
Entree& Sous_Zone::readOn(Entree& is)
{
  Motcles les_mots(12);
  {
    les_mots[0] = "Liste";
    les_mots[1] = "Polynomes";
    les_mots[2] = "Boite";
    les_mots[3] = "Rectangle";
    les_mots[4] = "Intervalle";
    les_mots[5] = "Fichier";
    les_mots[6] = "Plaque";
    les_mots[7] = "Segment";
    les_mots[8] = "Couronne";
    les_mots[9] = "Tube";
    les_mots[10]= "Tube_Hexagonal";
    les_mots[11]= "fonction_sous_zone";
  }

  if (!la_zone_.non_nul())
    {
      Cerr << "You have not associated one of the objects of type Sous_Zone " << finl;
      Cerr << "to the object of type Domain " << finl;
      exit();
    }

  const Zone& lazone=la_zone_.valeur();
  const Domaine& dom=lazone.domaine();
  ArrOfInt les_polys_possibles_;

  // GF de prendre nb_elem_tot au lieu de nb_elem permet de ne plus avoir besoin de decouper les sous zones..
  int nb_pol_possible=lazone.nb_elem_tot();
  les_polys_possibles_.resize_array(nb_pol_possible);
  for (int i=0; i<nb_pol_possible; i++)
    les_polys_possibles_(i)=i;

  Motcle motlu;
  is >>  motlu;
  if(motlu != Motcle("{"))
    {
      Cerr << "We expected a { to the reading of the subarea" << finl;
      Cerr << "instead of " << motlu << finl;
      exit();
    }
  is >> motlu;

  if (motlu=="restriction")
    {
      Nom nom_ss_zone;
      is >> nom_ss_zone;
      const Sous_Zone& ssz=ref_cast(Sous_Zone,interprete().objet(nom_ss_zone));
      nb_pol_possible=ssz.nb_elem_tot();
      les_polys_possibles_.resize_array(nb_pol_possible);

      for (int i=0; i<nb_pol_possible; i++)
        les_polys_possibles_(i)=ssz(i);
      is>>motlu;
    }
  int rang = les_mots.search(motlu);
  if (rang==-1)
    {
      int ok=lire_motcle_non_standard(motlu,is);
      if (!ok)
	{
	  Cerr << motlu << " is not understood " << finl;
	  Cerr << "The understood keywords are " << les_mots;
	  exit();
	}
    }
  if ((rang == 0) && (Process::nproc()>1))
    {
      Cerr << "When the subareas are defined as lists of elements" << finl;
      Cerr << "it is necessary to split them for parallel computing." << finl;
      exit();
    }
  switch(rang)
    {
    case 0 :
      is >> les_polys_;
      break;
    case 1 :
      {
        Cerr<<"Sous_Zone::readOn : Reading of the polynomials"<<finl;
        LIST(Polynome) les_polynomes;
        is >> motlu;
        if(motlu!=Motcle("{"))
          {
            Cerr << "We expected a { " << finl;
            exit();
          }
        while (1)
          {
            Polynome un_poly;
            is >> un_poly;
            les_polynomes.add(un_poly);
            is >> motlu;
            Motcle Et("et");
            if(motlu==Motcle("}")) break;
            if(motlu!=Motcle("et"))
              {
                Cerr << "We expected " << Et << " or } " << finl;
                exit();
              }
          }
        {
          const Zone& la_zone=la_zone_.valeur();
          int le_poly, nbsom=la_zone.nb_som_elem();
          DoubleVect x(dimension);
          int compteur=0;

          les_polys_.resize(nb_pol_possible);
          for (int n_pol=0; n_pol<nb_pol_possible; n_pol++)
            {
              le_poly=les_polys_possibles_[n_pol];
              x=0;
              for(int le_som=0; le_som<nbsom; le_som++)
                {
                  for(int k=0; k<dimension; k++)
                    x(k)+=dom.coord(lazone.sommet_elem(le_poly,le_som),k);
                }
              x/=((double)(nbsom));
              LIST_CURSEUR(Polynome) curseur=les_polynomes;
              int test = 1;
              while(curseur)
                {
                  if (dimension==2)
                    {
                      if (curseur.valeur()(x(0), x(1)) <0) test = -1;
                    }

                  if (dimension==3)
                    {
                      if (curseur.valeur()(x(0), x(1),x(2)) <0) test = -1;
                    }
                  ++curseur;
                }

              if(test > 0)
                {
                  les_polys_(compteur)=le_poly;
                  compteur++;
                }
            }
          les_polys_.resize(compteur);
          Cerr << "Construction of the subarea OK" << finl;
        }
        break;
      }
    case 2 :
      {
        // Boite
        if(dimension!=3)
          {
            // Une Boite est un volume
            Cerr << "In 2 dimensions, you must use \"Rectangle\" " << finl;
            exit();
          }
        is >> motlu;
        if (motlu != Motcle("Origine"))
          {
            Cerr << "We expected the keyword \"Origine\"" << finl;
            exit();
          }
        double deux_pi=M_PI*2.0 ;
        double ox, oy, oz;
        is >> ox >> oy >> oz ;
        is >> motlu;
        if (motlu != Motcle("Cotes"))
          {
            Cerr << "We expected the keyword \"Cotes\"" << finl;
            exit();
          }
        double lx, ly, lz;
        is >> lx >> ly >>lz;
        lx+= ox;
        ly+=oy;
        lz+=oz;
        if(axi)
          {
            oy*=deux_pi;
            ly*=deux_pi;
          }
        {
          const Zone& la_zone=la_zone_.valeur();
          int le_poly, nbsom=la_zone.nb_som_elem();
          double x, y, z;
          int compteur=0;
          // int nb_poly=la_zone.nb_elem();
          Cerr << "Construction of the subarea " << le_nom()<< finl;
          les_polys_.resize(nb_pol_possible);
          for (int n_pol=0; n_pol<nb_pol_possible; n_pol++)
            {
              le_poly=les_polys_possibles_[n_pol];
              x=y=z=0;
              for(int le_som=0; le_som<nbsom; le_som++)
                {
                  x+=dom.coord(lazone.sommet_elem(le_poly,le_som),0);
                  y+=dom.coord(lazone.sommet_elem(le_poly,le_som),1);
                  z+=dom.coord(lazone.sommet_elem(le_poly,le_som),2);
                }
              x/=((double)(nbsom));
              y/=((double)(nbsom));
              z/=((double)(nbsom));
              if ( sup_strict(x,ox)
                   && sup_strict(lx,x)
                   && sup_strict(y,oy)
                   && sup_strict(ly,y)
                   && sup_strict(z,oz)
                   && sup_strict(lz,z) )
                {
                  les_polys_(compteur)=le_poly;
                  compteur++;
                }
            }
          les_polys_.resize(compteur);
          Cerr << "Construction of the subarea OK" << finl;
        }
        break;
      }
    case 3 :
      {
        // Rectangle
        if(dimension!=2)
          {
            // Un Rectangle est une surface
            Cerr << "In 3 dimensions, you must use \"Boite\" " << finl;
            exit();
          }
        is >> motlu;
        if (motlu != Motcle("Origine"))
          {
            Cerr << "We expected the keyword \"Origine\"" << finl;
            exit();
          }
        double ox, oy;
        is >> ox >> oy ;
        is >> motlu;
        if (motlu != Motcle("Cotes"))
          {
            Cerr << "We expected the keyword \"Cotes\"" << finl;
            exit();
          }
        double lx, ly;
        is >> lx >> ly ;
        lx+= ox;
        ly+=oy;
        {
          const Zone& la_zone=la_zone_.valeur();
          int le_poly, nbsom=la_zone.nb_som_elem();
          double x, y;
          int compteur=0;

          Cerr << "Construction of the subarea " << le_nom() << finl;
          les_polys_.resize(nb_pol_possible);
          for (int n_pol=0; n_pol<nb_pol_possible; n_pol++)
            {
              le_poly=les_polys_possibles_[n_pol];
              x=y=0;
              for(int le_som=0; le_som<nbsom; le_som++)
                {
                  x+=dom.coord(lazone.sommet_elem(le_poly,le_som),0);
                  y+=dom.coord(lazone.sommet_elem(le_poly,le_som),1);
                }
              x/=((double)(nbsom));
              y/=((double)(nbsom));
              if ( sup_strict(x,ox)
                   && sup_strict(lx,x)
                   && sup_strict(y,oy)
                   && sup_strict(ly,y) )
                {
                  les_polys_(compteur)=le_poly;
                  compteur++;
                }
            }
          les_polys_.resize(compteur);
          Cerr << "Construction of the subarea OK" << finl;
        }
        break;
      }
    case 4 :
      {
        // Intervalle (only be used in sequential calculation)
        if (Process::nproc()>1)
          {
            Cerr << "You can't use Intervalle option for parallel calculation." << finl;
            exit();
          }
        int prems, nombre;
        is >> prems >> nombre;
        les_polys_.resize(nombre);
        for(int i=0; i<nombre; i++)
          les_polys_[i]=prems+i;
        break;
      }
    case 5 :
      {
        // Lecture de la sous-zone dans un fichier ascii.
        // Le fichier contient, pour chaque processeur dans l'ordre croissant,
        // un IntVect contenant les indices dans la zone(0) des elements reels
        // qui constituent la sous-zone.
        Nom nomfic;
        is >> nomfic;
        if(je_suis_maitre())
          {
            IntVect tab;
            EFichier fic;
            Cerr << "Reading of a subarea in the file " << nomfic << finl;
            if (!fic.ouvrir(nomfic))
              {
                Cerr << " Error while opening file." << finl;
                exit();
              }
            fic >> les_polys_;
            for(int p=1; p<nproc(); p++)
              {
                fic >> tab;
                envoyer(tab,0,p,p);
              }
          }
        else
          {
            recevoir(les_polys_,0,me(),me());
          }

        // Ajout a la liste "les_polys_" des indices des elements virtuels
        // de la sous-zone.
        // On cree un tableau distribue de marqueurs des elements de la sous-zone
        const int nb_elem = lazone.nb_elem();
        IntVect marqueurs;
        dom.creer_tableau_elements(marqueurs);
        const int nb_polys_reels = les_polys_.size();
        for (int i = 0; i < nb_polys_reels; i++)
          {
            const int elem = les_polys_[i];
            marqueurs[elem] = 1;
          }
        marqueurs.echange_espace_virtuel();
        // Compter les elements virtuels dans la sous-zone:
        const int zone_nb_elem_tot = lazone.nb_elem_tot();
        int nb_polys = nb_polys_reels;
        for (int i = nb_elem; i < zone_nb_elem_tot; i++)
          nb_polys += marqueurs[i];
        // Ajouter les indices des elements virtuels a "les_polys_"
        les_polys_.resize(nb_polys);
        nb_polys = nb_polys_reels;
        for (int i = nb_elem; i < zone_nb_elem_tot; i++)
          if (marqueurs[i])
            les_polys_[nb_polys++] = i;

        break;
      }
    case 6 :
      {
        // Plaque
        if(dimension!=3)
          {
            Cerr << "In 2 dimensions, you must use something else like a segment."<< finl;
            exit();
          }
        is >> motlu;
        if (motlu != Motcle("Origine"))
          {
            Cerr << "We expected the keyword \"Origine\"" << finl;
            exit();
          }
        double deux_pi=M_PI*2.0 ;
        double ox, oy, oz;
        is >> ox >> oy >> oz ;
        is >> motlu;
        if (motlu != Motcle("Cotes"))
          {
            Cerr << "We expected the keyword \"Cotes\"" << finl;
            exit();
          }
        double lx, ly, lz;
        is >> lx >> ly >>lz;
        if ((lx!=0)&&(ly!=0)&&(lz!=0))
          {
            Cerr << "We expected at least one quotation to zero" << finl;
            exit();
          }
        lx+=ox;
        ly+=oy;
        lz+=oz;
        if(axi)
          {
            oy*=deux_pi;
            ly*=deux_pi;
          }
        {
          const Zone& la_zone=la_zone_.valeur();
          int le_poly, nbsom=la_zone.nb_som_elem();
          double x, y, z;
          int compteur=0;
          Cerr << "Construction of the subarea " << le_nom() << finl;
          les_polys_.resize(nb_pol_possible);
          for (int n_pol=0; n_pol<nb_pol_possible; n_pol++)
            {
              le_poly=les_polys_possibles_[n_pol];
              int le_som;
              x=y=z=0;
              for(le_som=0; le_som<nbsom; le_som++)
                {
                  x+=dom.coord(lazone.sommet_elem(le_poly,le_som),0);
                  y+=dom.coord(lazone.sommet_elem(le_poly,le_som),1);
                  z+=dom.coord(lazone.sommet_elem(le_poly,le_som),2);
                }
              le_som=0;
              double xmin,xmax,ymin,ymax,zmin,zmax;
              xmin=dom.coord(lazone.sommet_elem(le_poly,le_som),0);
              ymin=dom.coord(lazone.sommet_elem(le_poly,le_som),1);
              zmin=dom.coord(lazone.sommet_elem(le_poly,le_som),2);
              xmax=xmin;
              ymax=ymin;
              zmax=zmin;
              for(le_som=1; le_som<nbsom; le_som++)
                {
                  xmin=min(xmin,dom.coord(lazone.sommet_elem(le_poly,le_som),0));
                  ymin=min(ymin,dom.coord(lazone.sommet_elem(le_poly,le_som),1));
                  zmin=min(zmin,dom.coord(lazone.sommet_elem(le_poly,le_som),2));
                  xmax=max(xmax,dom.coord(lazone.sommet_elem(le_poly,le_som),0));
                  ymax=max(ymax,dom.coord(lazone.sommet_elem(le_poly,le_som),1));
                  zmax=max(zmax,dom.coord(lazone.sommet_elem(le_poly,le_som),2));
                }
              x/=((double)(nbsom));
              y/=((double)(nbsom));
              z/=((double)(nbsom));
              if (
                (    sup_strict(x,ox)    && sup_strict(lx,x)
                     && sup_strict(y,oy)    && sup_strict(ly,y)
                     && sup_strict(oz,zmin) && inf_ou_egal(oz,zmax) )
                || ( sup_strict(x,ox)    && sup_strict(lx,x)
                     && sup_strict(z,oz)    && sup_strict(lz,z)
                     && sup_strict(oy,ymin) && inf_ou_egal(oy,ymax) )
                || ( sup_strict(y,oy)    && sup_strict(ly,y)
                     && sup_strict(z,oz)    && sup_strict(lz,z)
                     && sup_strict(ox,xmin) && inf_ou_egal(ox,xmax) )
              )
                {
                  les_polys_(compteur)=le_poly;
                  compteur++;
                }
            }
          les_polys_.resize(compteur);
          Cerr << "Construction of the subarea OK" << finl;
        }
        break;
      }
    case 7:
      {
        // Segment
        if(dimension!=2)
          {
            Cerr << "In 3 dimensions, you must use something else like a plaque."<< finl;
            exit();
          }
        is >> motlu;
        if (motlu != Motcle("Origine"))
          {
            Cerr << "We expected the keyword \"Origine\"" << finl;
            exit();
          }
        //double deux_pi=M_PI*2.0 ;
        double ox, oy;
        is >> ox >> oy  ;
        is >> motlu;
        if (motlu != Motcle("Cotes"))
          {
            Cerr << "We expected the keyword \"Cotes\"" << finl;
            exit();
          }
        double lx, ly;
        is >> lx >> ly ;
        if ((lx!=0)&&(ly!=0))
          {
            Cerr << "We expected at least one quotation to zero" << finl;
            exit();
          }
        lx+= ox;
        ly+=oy;

        {
          const Zone& la_zone=la_zone_.valeur();
          int le_poly, nbsom=la_zone.nb_som_elem();
          double x, y;
          int compteur=0;
          Cerr << "Construction of the subarea " << le_nom() << finl;
          les_polys_.resize(nb_pol_possible);
          for (int n_pol=0; n_pol<nb_pol_possible; n_pol++)
            {
              le_poly=les_polys_possibles_[n_pol];
              int le_som;
              x=y=0;
              for(le_som=0; le_som<nbsom; le_som++)
                {
                  x+=dom.coord(lazone.sommet_elem(le_poly,le_som),0);
                  y+=dom.coord(lazone.sommet_elem(le_poly,le_som),1);
                }
              le_som=0;
              double xmin,xmax,ymin,ymax;
              xmin=dom.coord(lazone.sommet_elem(le_poly,le_som),0);
              ymin=dom.coord(lazone.sommet_elem(le_poly,le_som),1);
              xmax=xmin;
              ymax=ymin;//zmax=zmin;
              for(le_som=1; le_som<nbsom; le_som++)
                {
                  xmin=min(xmin,dom.coord(lazone.sommet_elem(le_poly,le_som),0));
                  ymin=min(ymin,dom.coord(lazone.sommet_elem(le_poly,le_som),1));
                  xmax=max(xmax,dom.coord(lazone.sommet_elem(le_poly,le_som),0));
                  ymax=max(ymax,dom.coord(lazone.sommet_elem(le_poly,le_som),1));
                }
              x/=((double)(nbsom));
              y/=((double)(nbsom));
              if (
                ( sup_strict(x,ox)    && sup_strict(lx,x)
                  && sup_strict(oy,ymin) && inf_ou_egal(oy,ymax) )
                || ( sup_strict(y,oy)    && sup_strict(ly,y)
                     && sup_strict(ox,xmin) && inf_ou_egal(ox,xmax) )
              )
                {
                  les_polys_(compteur)=le_poly;
                  compteur++;
                }
            }
          les_polys_.resize(compteur);
          Cerr << "Construction of the subarea OK" << finl;
        }
        break;
      }
    case 8 :
      {
        // Couronne
        if(dimension!=2)
          {
            // Une Couronne en 2D seulement !
            Cerr << "A crown is only in 2D : in 3 dimensions it is a tube \"Tube\" "<< finl;
            exit();
          }
        is >> motlu;
        if(motlu!=Motcle("Origine"))
          {
            Cerr << "We expected the keyword \"ORIGINE\" " << finl;
            exit();
          }
        double xo,yo;
        is >> xo >> yo ;
        is >> motlu;
        if(motlu!=Motcle("RI"))
          {
            Cerr << "We expected the internal radius \"RI\" " << finl;
            exit();
          }
        double ri,re;
        is >> ri;
        is >> motlu;
        if(motlu!=Motcle("RE"))
          {
            Cerr << "We expected the external radius \"RE\" " << finl;
            exit();
          }
        is >> re;
        {
          double ri2=ri*ri; // Internal radius^2
          double re2=re*re; // External radius^2
          const Zone& la_zone=la_zone_.valeur();
          int nbsom=la_zone.nb_som_elem();
          DoubleVect x(dimension);
          int compteur=0;
          Cerr << "Construction of the subarea " << le_nom() << finl;
          les_polys_.resize(nb_pol_possible);
          for (int n_pol=0; n_pol<nb_pol_possible; n_pol++)
            {
              int le_poly=les_polys_possibles_[n_pol];
              x=0;
              for(int som=0; som<nbsom; som++)
                {
                  for(int k=0; k<dimension; k++)
                    x(k)+=dom.coord(lazone.sommet_elem(le_poly,som),k);
                }
              x/=((double)(nbsom)); // Center of gravity of the cell
              if ( sup_strict((x(0)-xo)*(x(0)-xo)+(x(1)-yo)*(x(1)-yo),ri2) && sup_strict(re2,(x(0)-xo)*(x(0)-xo)+ (x(1)-yo)*(x(1)-yo)) )
                {
                  les_polys_(compteur)=le_poly;
                  compteur++;
                }
            }
          les_polys_.resize(compteur);
          Cerr << "Construction of the subarea OK" << finl;
        }
        break;
      }
    case 9 :
      {
        // Tube
        if(dimension!=3)
          {
            // Un tube en 3D seulement !
            Cerr << "A tube is only in 3D : in 2 dimensions it is a crown \"Couronne\" "<< finl;
            exit();
          }
        is >> motlu;
        if(motlu!=Motcle("Origine"))
          {
            Cerr << "We expected the keyword \"ORIGINE\" " << finl;
            exit();
          }
        double xo,yo,zo;
        is >> xo >> yo >> zo;
        is >> motlu;
        if(motlu!=Motcle("DIR"))
          {
            Cerr << "We expected the tube direction, keyword \"DIR\" " << finl;
            exit();
          }
        Motcles coords(3);
        {
          coords[0] = "X";
          coords[1] = "Y";
          coords[2] = "Z";
        }
        Nom coord;
        is >> coord;
        motlu=coord;
        int idir = coords.search(motlu);
        int dir[3];
        dir[0]=dir[1]=dir[2]=0;
        double h0=0;
        switch(idir)
          {
          case 0:
            dir[0]=0;
            dir[1]=dir[2]=1;
            h0=xo;
            break;

          case 1:
            dir[1]=0;
            dir[0]=dir[2]=1;
            h0=yo;
            break;

          case 2:
            dir[2]=0;
            dir[1]=dir[0]=1;
            h0=zo;
            break;

          default:
            h0=-1;
            Cerr << "DIR is equal to X for a tube parallel to OX ; Y for a tube parallel to OY and Z for a tube parallel to OZ" << endl;
            Cerr << "Currently, DIR is equal to " << coord << endl;
            exit();
          }

        is >> motlu;
        if(motlu!=Motcle("RI"))
          {
            Cerr << "We expected the internal radius, keyword \"RI\" " << finl;
            exit();
          }
        double ri,re,h;
        is >> ri;
        is >> motlu;
        if(motlu!=Motcle("RE"))
          {
            Cerr << "We expected the external radius, keyword \"RE\" " << finl;
            exit();
          }
        is >> re;
        is >> motlu;
        if(motlu!=Motcle("Hauteur"))
          {
            Cerr << "We expected the height of tube, keyword \"Hauteur\" " << finl;
            exit();
          }
        is >> h;
        h+=h0;
        {
          double ri2 = ri*ri, re2=re*re;
          const Zone& la_zone=la_zone_.valeur();
          int le_poly, nbsom=la_zone.nb_som_elem();
          DoubleVect x(dimension);
          int compteur=0;
          Cerr << "Construction of the subarea " << le_nom() << finl;
          les_polys_.resize(nb_pol_possible);
          for (int n_pol=0; n_pol<nb_pol_possible; n_pol++)
            {
              le_poly=les_polys_possibles_[n_pol];
              x=0;
              for(int le_som=0; le_som<nbsom; le_som++)
                {
                  for(int k=0; k<dimension; k++)
                    x(k)+=dom.coord(lazone.sommet_elem(le_poly,le_som),k);
                }
              x/=((double)(nbsom));
              double tmp = dir[0]*(x(0)-xo)*(x(0)-xo)+ dir[1]*(x(1)-yo)*(x(1)-yo) + dir[2]*(x(2)-zo)*(x(2)-zo);
              if ( sup_ou_egal(tmp,ri2) && inf_ou_egal(tmp,re2) && inf_ou_egal(x(idir),h) && sup_ou_egal(x(idir),h0) )
                {
                  les_polys_(compteur)=le_poly;
                  compteur++;
                }
            }
          les_polys_.resize(compteur);
          Cerr << "Construction of the subarea OK" << finl;
        }
        break;
      }
    case 10 :
      {
        if(dimension!=3)
          {
            // Un tube en 3D seulement !
            Cerr << "An hexagonal tube \"Tube_hexagonal\" is only in 3D "<< finl;
            exit();
          }

        // PQ : 17/09/08 : on suppose le tube hexagonal centre sur l'origine, porte par l'axe z
        // et pour lequel l'entreplat est entre y=-ep/2 et y=+ep/2

        double ep;
        bool in=1;

        is >> motlu;
        if(motlu!=Motcle("ENTREPLAT"))
          {
            Cerr << "We expected the entreplat of the tube, keyword \"ENTREPLAT\"" << finl;
            exit();
          }
        is >> ep;
        is >> motlu;
        if(motlu==Motcle("IN"))       in=1;
        else if(motlu==Motcle("OUT")) in=0;
        else
          {
            Cerr << "We expected the keyword \"IN\" or \"OUT\" " << finl;
            exit();
          }

        {
          const Zone& la_zone=la_zone_.valeur();
          int le_poly, nbsom=la_zone.nb_som_elem();
          DoubleVect x(dimension);
          int compteur=0;
          Cerr << "Construction of the subarea " << le_nom()<< finl;

          les_polys_.resize(nb_pol_possible);
          for (int n_pol=0; n_pol<nb_pol_possible; n_pol++)
            {
              le_poly=les_polys_possibles_[n_pol];
              x=0;
              for(int le_som=0; le_som<nbsom; le_som++)
                {
                  for(int k=0; k<dimension; k++)
                    x(k)+=dom.coord(lazone.sommet_elem(le_poly,le_som),k);
                }
              x/=((double)(nbsom));

              if ( sup_ou_egal(x(1),-ep/2.) && sup_ou_egal(x(1),-ep-x(0)*sqrt(3.)) && sup_ou_egal(x(1),-ep+x(0)*sqrt(3.))
                   &&  inf_ou_egal(x(1), ep/2.) && inf_ou_egal(x(1), ep-x(0)*sqrt(3.)) && inf_ou_egal(x(1), ep+x(0)*sqrt(3.)) )
                {
                  if(in==1)
                    {
                      les_polys_(compteur)=le_poly;
                      compteur++;
                    }
                }
              else
                {
                  if(in==0)
                    {
                      les_polys_(compteur)=le_poly;
                      compteur++;
                    }
                }
            }
          les_polys_.resize(compteur);
          Cerr << "Construction of the subarea OK" << finl;
        }
        break;
      }
    case 11 :
      {

        Parser_U F;
        F.setNbVar(dimension);
        Nom fct;
        is>> fct;
        F.setString(fct);
        F.addVar("x");
        F.addVar("y");
        if (dimension==3)
          F.addVar("z");
        F.parseString();

        {
          const Zone& la_zone=la_zone_.valeur();
          int le_poly, nbsom=la_zone.nb_som_elem();
          DoubleVect x(dimension);
          int compteur=0;
          Cerr << "Construction of the subarea " << le_nom()<< finl;

          les_polys_.resize(nb_pol_possible);
          for (int n_pol=0; n_pol<nb_pol_possible; n_pol++)
            {
              le_poly=les_polys_possibles_[n_pol];
              x=0;
              for(int le_som=0; le_som<nbsom; le_som++)
                {
                  for(int k=0; k<dimension; k++)
                    x(k)+=dom.coord(lazone.sommet_elem(le_poly,le_som),k);
                }
              x/=((double)(nbsom));
              F.setVar("x",x[0]);
              F.setVar("y",x[1]);
              if (dimension==3)
                F.setVar("z",x[2]);
              double test=F.eval();
              // attention le test absolu est voulu
              // si on fait une fonction qui vaut 0 ou 1 ....
              if (test>0)
                {
                  les_polys_(compteur)=le_poly;
                  compteur++;
                }
            }
          les_polys_.resize(compteur);
          Cerr << "Construction of the subarea OK" << finl;
        }
        break;
      }
     case -1:
     // traite avant
      break;    
    default :
      {
        Cerr << "TRUST error" << finl;
        exit();
      }
      break;
    }
  is >> motlu;
  if (motlu=="union")
    {
      Nom nom_ss_zone;
      is >> nom_ss_zone;
      const Sous_Zone& ssz=ref_cast(Sous_Zone,interprete().objet(nom_ss_zone));
      nb_pol_possible=ssz.nb_elem_tot();

      les_polys_possibles_.resize_array(nb_pol_possible);
      int compt=0;
      // on cherche dans ces polys ceux que l'on n a pas
      for (int i=0; i<nb_pol_possible; i++)
        {
          int poly_i=ssz(i);
          int trouve=0;
          for (int j=0; j<nb_elem_tot(); j++)
            if ((les_polys_(j))==poly_i)
              {
                trouve=1;
                break;
              }
          if (!trouve)
            les_polys_possibles_(compt++)=poly_i;
        }
      // on rajoute ces polys a la fin
      int old_size=les_polys_.size();
      les_polys_.resize(old_size+compt);
      Journal()<<les_polys_<<finl;

      for (int i=0; i<compt; i++)
        les_polys_(old_size+i)=les_polys_possibles_(i);


      is >>motlu;
    }

  if(motlu != Motcle("}"))
    {
      Cerr << "We expected a } to the reading of the subarea" << finl;
      Cerr << "instead of " << motlu << finl;
      exit();
    }
  return is;
}

int Sous_Zone::lire_motcle_non_standard(const Motcle& motlu , Entree& is)
{
  return 0;
}
  
  



// Description:
//    Ajoute un polyedre a la sous-zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode modifie l'objet
int Sous_Zone::add_poly(const int& poly)
{
  assert(0<=poly);
  int nb_poly=les_polys_.size();
  int OK=1;
  for(int i=0; i<nb_poly; i++)
    if(les_polys_(i)==poly)
      OK=0;
  if(OK==1)
    {
      les_polys_.resize(nb_poly+1);
      les_polys_(nb_poly)=poly;
    }
  return 1;
}


// Description:
//    Enleve un polyedre a la sous-zone.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode modifie l'objet
int Sous_Zone::remove_poly(const int& poly)
{
  assert(0<=poly);
  assert(poly<=la_zone_.valeur().nb_elem());
  int nb_poly=les_polys_.size();
  IntVect les_polys_tmp;
  les_polys_tmp.resize(la_zone_.valeur().nb_elem());
  int compteur=0;
  for(int i=0; i<nb_poly; i++)
    if(les_polys_(i)!=poly)
      {
        les_polys_tmp(compteur)=les_polys_(i);
        compteur++;
      }
  les_polys_tmp.resize(compteur);
  les_polys_.resize(compteur);
  les_polys_=les_polys_tmp;
  return 1;
}


// Description:
//    Associe une zone a la sous-zone.
//    La sous-zone sera une sous-zone de la Zone specifiee.
// Precondition:
// Parametre: Zone& zone
//    Signification: la zone a associe a la sous-zone
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Sous_Zone::associer_zone(const Zone& une_zone)
{
  la_zone_=une_zone;
}


// Description:
//    Associe un Objet_U a la sous-zone.
//    On controle le type de l'objet a associer
//    dynamiquement.
// Precondition:
// Parametre: Objet_U& ob
//    Signification: objet a associer a la sous-zone.
//    Valeurs par defaut:
//    Contraintes: doit etre un sous-type de Domaine
//    Acces:
// Retour: int
//    Signification: renvoie 1 si l'association a reussi
//                   0 sinon.
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Sous_Zone::associer_(Objet_U& ob)
{
  if( sub_type(Domaine, ob))
    {
      // MONOZONE pour le moment
      if(la_zone_.non_nul()) return 1;
      associer_zone(ref_cast(Domaine, ob).zone(0));
      ob.associer_(*this);
      return 1;
    }
  return 0;
}



// Description:
//    Donne un nom a la sous-zone.
// Precondition:
// Parametre: Nom& nom
//    Signification: le nom a donner a la sous-zone
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Sous_Zone::nommer(const Nom& nom)
{
  nom_=nom;
}


