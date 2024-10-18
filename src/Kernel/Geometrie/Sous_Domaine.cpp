/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <LecFicDistribue.h>
#include <communications.h>
#include <Synonyme_info.h>
#include <Sous_Domaine.h>
#include <Interprete.h>
#include <Polynome.h>
#include <Parser_U.h>
#include <Domaine.h>

Implemente_instanciable_32_64(Sous_Domaine_32_64,"Sous_Domaine",Objet_U);
Add_synonym(Sous_Domaine, "Sous_Zone");
// XD sous_zone objet_u sous_zone 1 It is an object type describing a domain sub-set. NL2 A Sous_Zone (Sub-area) type object must be associated with a Domaine type object. The Read (Lire) interpretor is used to define the items comprising the sub-area. NL2 Caution: The Domain type object nom_domaine must have been meshed (and triangulated or tetrahedralised in VEF) prior to carrying out the Associate (Associer) nom_sous_zone nom_domaine instruction; this instruction must always be preceded by the read instruction.
// XD  attr restriction ref_sous_zone restriction 1 The elements of the sub-area nom_sous_zone must be included into the other sub-area named nom_sous_zone2. This keyword should be used first in the Read keyword.
// XD  attr rectangle bloc_origine_cotes rectangle 1 The sub-area will include all the domain elements whose centre of gravity is within the Rectangle (in dimension 2).
// XD  attr segment bloc_origine_cotes segment 1 not_set
// XD  attr boite bloc_origine_cotes box 1 The sub-area will include all the domain elements whose centre of gravity is within the Box (in dimension 3).
// XD  attr liste listentier liste 1 The sub-area will include n domain items, numbers No. 1 No. i No. n.
// XD  attr fichier chaine filename 1 The sub-area is read into the file filename.
// XD  attr intervalle deuxentiers intervalle 1 The sub-area will include domain items whose number is between n1 and n2 (where n1<=n2).
// XD  attr polynomes bloc_lecture polynomes 1 A REPRENDRE
// XD  attr couronne bloc_couronne couronne 1 In 2D case, to create a couronne.
// XD  attr tube bloc_tube tube 1 In 3D case, to create a tube.
// XD  attr fonction_sous_zone chaine fonction_sous_domaine 1 Keyword to build a sub-area with the the elements included into the area defined by fonction>0.
// XD  attr union ref_sous_zone union_with 1 The elements of the sub-area nom_sous_zone3 will be added to the sub-area nom_sous_zone. This keyword should be used last in the Read keyword.
// XD ref domaine domaine

// XD bloc_origine_cotes objet_lecture nul 0 Class to create a rectangle (or a box).
// XD  attr name chaine(into=["Origine"]) name 0 Keyword to define the origin of the rectangle (or the box).
// XD  attr origin listf origine 0 Coordinates of the origin of the rectangle (or the box).
// XD  attr name2 chaine(into=["Cotes"]) name2 0 Keyword to define the length along the axes.
// XD  attr cotes listf cotes 0 Length along the axes.

// XD bloc_couronne objet_lecture nul 0 Class to create a couronne (2D).
// XD  attr name chaine(into=["Origine"]) name 0 Keyword to define the center of the circle.
// XD  attr origin listf origine 0 Center of the circle.
// XD  attr name3 chaine(into=["ri"]) name3 0 Keyword to define the interior radius.
// XD  attr ri floattant ri 0 Interior radius.
// XD  attr name4 chaine(into=["re"]) name4 0 Keyword to define the exterior radius.
// XD  attr re floattant re 0 Exterior radius.

// XD bloc_tube objet_lecture nul 0 Class to create a tube (3D).
// XD  attr name chaine(into=["Origine"]) name 0 Keyword to define the center of the tube.
// XD  attr origin listf origine 0 Center of the tube.
// XD  attr name2 chaine(into=["dir"]) name2 0 Keyword to define the direction of the main axis.
// XD  attr direction chaine(into=["X","Y","Z"]) direction 0 direction of the main axis X, Y or Z
// XD  attr name3 chaine(into=["ri"]) name3 0 Keyword to define the interior radius.
// XD  attr ri floattant ri 0 Interior radius.
// XD  attr name4 chaine(into=["re"]) name4 0 Keyword to define the exterior radius.
// XD  attr re floattant re 0 Exterior radius.
// XD  attr name5 chaine(into=["hauteur"]) name5 0 Keyword to define the heigth of the tube.
// XD  attr h floattant h 0 Heigth of the tube.


/*! @brief Ecrit la liste des polyedres de la sous-domaine sur un flot de sortie.
 *
 *     Format:
 *     Liste n1 n2 .. Ni
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
template <typename _SIZE_>
Sortie& Sous_Domaine_32_64<_SIZE_>::printOn(Sortie& os) const
{
  os << "{" << finl;
  os << "Liste" << finl;
  os << les_elems_;
  os << "}" << finl;
  return os;
}


/*! @brief Lit les specifications d'un sous-domaine dans le jeu de donnee a partir d'un flot d'entree.
 *
 *     Format:
 *      { Rectangle Origine x0 y0 Cotes lx ly } en dimension 2
 *      { Boite Origine x0 y0 z0 Cotes lx ly lz} en dimension 3
 *       ou
 *      { Liste n n1  ni   nn }
 *       ou
 *      { Intervalle n1 n2 }
 *       ou
 *      { Polynomes {bloc_lecture_poly1 et bloc_lecture_poly_i
 *                   et bloc_lecture_poly_n}
 *      }
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 * @throws accolade ouvrante attendue
 * @throws mot clef non reconnu
 * @throws mot clef "et" ou "}" attendu
 * @throws En dimension 2, il faut utiliser Rectangle
 * @throws mot clef "Origine" attendu
 * @throws mot clef "Cotes" attendu
 * @throws En dimension 3, il faut utiliser Boite
 * @throws Erreur TRUST (mot clef reconnu non prevu)
 * @throws accolade fermante attendue
 */
template <typename _SIZE_>
Entree& Sous_Domaine_32_64<_SIZE_>::readOn(Entree& is)
{
  Motcles les_mots(13);
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
    les_mots[12]= "fonction_sous_domaine";
  }

  if (!le_dom_.non_nul())
    {
      Cerr << "You have not associated one of the objects of type Sous_Domaine " << finl;
      Cerr << "to the object of type Domain " << finl;
      Process::exit();
    }

  const Domaine_t& ledomaine=le_dom_.valeur();
  const Domaine_t& dom=ledomaine;
  ArrOfInt_t les_polys_possibles_;

  // GF de prendre nb_elem_tot au lieu de nb_elem permet de ne plus avoir besoin de decouper les sous domaines..
  int_t nb_pol_possible=ledomaine.nb_elem_tot();
  les_polys_possibles_.resize_array(nb_pol_possible);
  for (int_t i=0; i<nb_pol_possible; i++)
    les_polys_possibles_[i]=i;

  Motcle motlu;
  is >>  motlu;
  if(motlu != Motcle("{"))
    {
      Cerr << "We expected a { to the reading of the subarea" << finl;
      Cerr << "instead of " << motlu << finl;
      Process::exit();
    }
  is >> motlu;

  if (motlu=="restriction")
    {
      Nom nom_ss_domaine;
      is >> nom_ss_domaine;
      const Sous_Domaine_32_64& ssz=ref_cast(Sous_Domaine_32_64,interprete().objet(nom_ss_domaine));
      nb_pol_possible=ssz.nb_elem_tot();
      les_polys_possibles_.resize_array(nb_pol_possible);

      for (int_t i=0; i<nb_pol_possible; i++)
        les_polys_possibles_[i]=ssz(i);
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
          Process::exit();
        }
    }
  if ((rang == 0) && (Process::is_parallel()))
    {
      Cerr << "When the subareas are defined as lists of elements" << finl;
      Cerr << "it is necessary to split them for parallel computing." << finl;
      Process::exit();
    }
  switch(rang)
    {
    case 0 :
      is >> les_elems_;
      break;
    case 1 :
      {
        Cerr<<"Sous_Domaine_32_64<_SIZE_>::readOn : Reading of the polynomials"<<finl;
        LIST(Polynome) les_polynomes;
        is >> motlu;
        if(motlu!=Motcle("{"))
          {
            Cerr << "We expected a { " << finl;
            Process::exit();
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
                Process::exit();
              }
          }
        {
          const Domaine_t& le_dom=le_dom_.valeur();
          int nbsom=le_dom.nb_som_elem();
          int_t le_poly;
          DoubleVect x(dimension);
          int_t compteur=0;

          les_elems_.resize(nb_pol_possible);
          for (int_t n_pol=0; n_pol<nb_pol_possible; n_pol++)
            {
              le_poly=les_polys_possibles_[n_pol];
              x=0;
              for(int le_som=0; le_som<nbsom; le_som++)
                {
                  for(int k=0; k<dimension; k++)
                    x(k)+=dom.coord(ledomaine.sommet_elem(le_poly,le_som),k);
                }
              x/=((double)(nbsom));
              int test = 1;
              for (auto& itr : les_polynomes)
                {
                  if (dimension==2)
                    {
                      if (itr(x(0), x(1)) <0) test = -1;
                    }

                  if (dimension==3)
                    {
                      if (itr(x(0), x(1),x(2)) <0) test = -1;
                    }
                }

              if(test > 0)
                {
                  les_elems_(compteur)=le_poly;
                  compteur++;
                }
            }
          les_elems_.resize(compteur);
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
            Process::exit();
          }
        is >> motlu;
        if (motlu != Motcle("Origine"))
          {
            Cerr << "We expected the keyword \"Origine\"" << finl;
            Process::exit();
          }
        double deux_pi=M_PI*2.0 ;
        double ox, oy, oz;
        is >> ox >> oy >> oz ;
        is >> motlu;
        if (motlu != Motcle("Cotes"))
          {
            Cerr << "We expected the keyword \"Cotes\"" << finl;
            Process::exit();
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
          const Domaine_t& le_dom=le_dom_.valeur();
          int nbsom=le_dom.nb_som_elem();
          int_t le_poly;
          double x, y, z;
          int_t compteur=0;
          // int nb_poly=le_dom.nb_elem();
          Cerr << "Construction of the subarea " << le_nom()<< finl;
          les_elems_.resize(nb_pol_possible);
          for (int_t n_pol=0; n_pol<nb_pol_possible; n_pol++)
            {
              le_poly=les_polys_possibles_[n_pol];
              x=y=z=0;
              int nb_som_poly = 0;
              int_t s;
              for(int le_som = 0; le_som < nbsom && ((s = ledomaine.sommet_elem(le_poly,le_som)) >= 0); le_som++)
                {
                  x+=dom.coord(s, 0);
                  y+=dom.coord(s, 1);
                  z+=dom.coord(s, 2);
                  nb_som_poly++;
                }
              x/=((double)(nb_som_poly));
              y/=((double)(nb_som_poly));
              z/=((double)(nb_som_poly));
              if ( sup_strict(x,ox)
                   && sup_strict(lx,x)
                   && sup_strict(y,oy)
                   && sup_strict(ly,y)
                   && sup_strict(z,oz)
                   && sup_strict(lz,z) )
                {
                  les_elems_(compteur)=le_poly;
                  compteur++;
                }
            }
          les_elems_.resize(compteur);
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
            Process::exit();
          }
        is >> motlu;
        if (motlu != Motcle("Origine"))
          {
            Cerr << "We expected the keyword \"Origine\"" << finl;
            Process::exit();
          }
        double ox, oy;
        is >> ox >> oy ;
        is >> motlu;
        if (motlu != Motcle("Cotes"))
          {
            Cerr << "We expected the keyword \"Cotes\"" << finl;
            Process::exit();
          }
        double lx, ly;
        is >> lx >> ly ;
        lx+= ox;
        ly+=oy;
        {
          const Domaine_t& le_dom=le_dom_.valeur();
          int nbsom=le_dom.nb_som_elem();
          int_t le_poly;
          double x, y;
          int_t compteur=0;

          Cerr << "Construction of the subarea " << le_nom() << finl;
          les_elems_.resize(nb_pol_possible);
          for (int_t n_pol=0; n_pol<nb_pol_possible; n_pol++)
            {
              le_poly=les_polys_possibles_[n_pol];
              x=y=0;
              for(int le_som=0; le_som<nbsom; le_som++)
                {
                  x+=dom.coord(ledomaine.sommet_elem(le_poly,le_som),0);
                  y+=dom.coord(ledomaine.sommet_elem(le_poly,le_som),1);
                }
              x/=((double)(nbsom));
              y/=((double)(nbsom));
              if ( sup_strict(x,ox)
                   && sup_strict(lx,x)
                   && sup_strict(y,oy)
                   && sup_strict(ly,y) )
                {
                  les_elems_(compteur)=le_poly;
                  compteur++;
                }
            }
          les_elems_.resize(compteur);
          Cerr << "Construction of the subarea OK" << finl;
        }
        break;
      }
    case 4 :
      {
        // Intervalle (only be used in sequential calculation)
        if (Process::is_parallel())
          {
            Cerr << "You can't use Intervalle option for parallel calculation." << finl;
            Process::exit();
          }
        int_t prems, nombre;
        is >> prems >> nombre;
        les_elems_.resize(nombre);
        for(int_t i=0; i<nombre; i++)
          les_elems_[i]=prems+i;
        break;
      }
    case 5 :
      {
        // Lecture de la sous-domaine dans un fichier ascii.
        // Le fichier contient, pour chaque processeur dans l'ordre croissant,
        // un IntVect contenant les indices dans le domaine(0) des elements reels
        // qui constituent la sous-domaine.
        Nom nomfic;
        is >> nomfic;
        if(je_suis_maitre())
          {
            IntVect_t tab;
            EFichier fic;
            Cerr << "Reading of a subarea in the file " << nomfic << finl;
            if (!fic.ouvrir(nomfic))
              {
                Cerr << " Error while opening file." << finl;
                Process::exit();
              }
            fic >> les_elems_;
            for(int p=1; p<nproc(); p++)
              {
                fic >> tab;
                envoyer(tab,0,p,p);
              }
          }
        else
          {
            recevoir(les_elems_,0,me(),me());
          }

        // Ajout a la liste "les_polys_" des indices des elements virtuels
        // de la sous-domaine.
        // On cree un tableau distribue de marqueurs des elements de la sous-domaine
        const int_t nb_elem = ledomaine.nb_elem();
        IntVect_t marqueurs;
        dom.creer_tableau_elements(marqueurs);
        const int_t nb_polys_reels = les_elems_.size();
        for (int_t i = 0; i < nb_polys_reels; i++)
          {
            const int_t elem = les_elems_[i];
            marqueurs[elem] = 1;
          }
        marqueurs.echange_espace_virtuel();
        // Compter les elements virtuels dans la sous-domaine:
        const int_t domaine_nb_elem_tot = ledomaine.nb_elem_tot();
        int_t nb_polys = nb_polys_reels;
        for (int_t i = nb_elem; i < domaine_nb_elem_tot; i++)
          nb_polys += marqueurs[i];
        // Ajouter les indices des elements virtuels a "les_polys_"
        les_elems_.resize(nb_polys);
        nb_polys = nb_polys_reels;
        for (int_t i = nb_elem; i < domaine_nb_elem_tot; i++)
          if (marqueurs[i])
            les_elems_[nb_polys++] = i;

        break;
      }
    case 6 :
      {
        // Plaque
        if(dimension!=3)
          {
            Cerr << "In 2 dimensions, you must use something else like a segment."<< finl;
            Process::exit();
          }
        is >> motlu;
        if (motlu != Motcle("Origine"))
          {
            Cerr << "We expected the keyword \"Origine\"" << finl;
            Process::exit();
          }
        double deux_pi=M_PI*2.0 ;
        double ox, oy, oz;
        is >> ox >> oy >> oz ;
        is >> motlu;
        if (motlu != Motcle("Cotes"))
          {
            Cerr << "We expected the keyword \"Cotes\"" << finl;
            Process::exit();
          }
        double lx, ly, lz;
        is >> lx >> ly >>lz;
        if ((lx!=0)&&(ly!=0)&&(lz!=0))
          {
            Cerr << "We expected at least one quotation to zero" << finl;
            Process::exit();
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
          const Domaine_t& le_dom=le_dom_.valeur();
          int nbsom=le_dom.nb_som_elem();
          int_t le_poly;
          double x, y, z;
          int_t compteur=0;
          Cerr << "Construction of the subarea " << le_nom() << finl;
          les_elems_.resize(nb_pol_possible);
          for (int_t n_pol=0; n_pol<nb_pol_possible; n_pol++)
            {
              le_poly=les_polys_possibles_[n_pol];
              int le_som, nb_som_poly = 0;
              int_t s;
              x=y=z=0;
              for(le_som=0; le_som<nbsom && ((s = ledomaine.sommet_elem(le_poly,le_som)) >= 0); le_som++)
                {
                  x+=dom.coord(s, 0);
                  y+=dom.coord(s, 1);
                  z+=dom.coord(s, 2);
                  nb_som_poly++;
                }
              le_som=0;
              double xmin,xmax,ymin,ymax,zmin,zmax;
              xmin=dom.coord(ledomaine.sommet_elem(le_poly,le_som),0);
              ymin=dom.coord(ledomaine.sommet_elem(le_poly,le_som),1);
              zmin=dom.coord(ledomaine.sommet_elem(le_poly,le_som),2);
              xmax=xmin;
              ymax=ymin;
              zmax=zmin;
              for(le_som=1; le_som<nbsom && ((s = ledomaine.sommet_elem(le_poly,le_som)) >= 0); le_som++)
                {
                  xmin=std::min(xmin,dom.coord(s, 0));
                  ymin=std::min(ymin,dom.coord(s, 1));
                  zmin=std::min(zmin,dom.coord(s, 2));
                  xmax=std::max(xmax,dom.coord(s, 0));
                  ymax=std::max(ymax,dom.coord(s, 1));
                  zmax=std::max(zmax,dom.coord(s, 2));
                }
              x/=((double)(nb_som_poly));
              y/=((double)(nb_som_poly));
              z/=((double)(nb_som_poly));
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
                  les_elems_(compteur)=le_poly;
                  compteur++;
                }
            }
          les_elems_.resize(compteur);
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
            Process::exit();
          }
        is >> motlu;
        if (motlu != Motcle("Origine"))
          {
            Cerr << "We expected the keyword \"Origine\"" << finl;
            Process::exit();
          }
        //double deux_pi=M_PI*2.0 ;
        double ox, oy;
        is >> ox >> oy  ;
        is >> motlu;
        if (motlu != Motcle("Cotes"))
          {
            Cerr << "We expected the keyword \"Cotes\"" << finl;
            Process::exit();
          }
        double lx, ly;
        is >> lx >> ly ;
        if ((lx!=0)&&(ly!=0))
          {
            Cerr << "We expected at least one quotation to zero" << finl;
            Process::exit();
          }
        lx+= ox;
        ly+=oy;

        {
          const Domaine_t& le_dom=le_dom_.valeur();
          int nbsom=le_dom.nb_som_elem();
          int_t le_poly;
          double x, y;
          int_t compteur=0;
          Cerr << "Construction of the subarea " << le_nom() << finl;
          les_elems_.resize(nb_pol_possible);
          for (int_t n_pol=0; n_pol<nb_pol_possible; n_pol++)
            {
              le_poly=les_polys_possibles_[n_pol];
              int le_som;
              x=y=0;
              for(le_som=0; le_som<nbsom; le_som++)
                {
                  x+=dom.coord(ledomaine.sommet_elem(le_poly,le_som),0);
                  y+=dom.coord(ledomaine.sommet_elem(le_poly,le_som),1);
                }
              le_som=0;
              double xmin,xmax,ymin,ymax;
              xmin=dom.coord(ledomaine.sommet_elem(le_poly,le_som),0);
              ymin=dom.coord(ledomaine.sommet_elem(le_poly,le_som),1);
              xmax=xmin;
              ymax=ymin;//zmax=zmin;
              for(le_som=1; le_som<nbsom; le_som++)
                {
                  xmin=std::min(xmin,dom.coord(ledomaine.sommet_elem(le_poly,le_som),0));
                  ymin=std::min(ymin,dom.coord(ledomaine.sommet_elem(le_poly,le_som),1));
                  xmax=std::max(xmax,dom.coord(ledomaine.sommet_elem(le_poly,le_som),0));
                  ymax=std::max(ymax,dom.coord(ledomaine.sommet_elem(le_poly,le_som),1));
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
                  les_elems_(compteur)=le_poly;
                  compteur++;
                }
            }
          les_elems_.resize(compteur);
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
            Process::exit();
          }
        is >> motlu;
        if(motlu!=Motcle("Origine"))
          {
            Cerr << "We expected the keyword \"ORIGINE\" " << finl;
            Process::exit();
          }
        double xo,yo;
        is >> xo >> yo ;
        is >> motlu;
        if(motlu!=Motcle("RI"))
          {
            Cerr << "We expected the internal radius \"RI\" " << finl;
            Process::exit();
          }
        double ri,re;
        is >> ri;
        is >> motlu;
        if(motlu!=Motcle("RE"))
          {
            Cerr << "We expected the external radius \"RE\" " << finl;
            Process::exit();
          }
        is >> re;
        {
          double ri2=ri*ri; // Internal radius^2
          double re2=re*re; // External radius^2
          const Domaine_t& le_dom=le_dom_.valeur();
          int nbsom=le_dom.nb_som_elem();
          DoubleVect x(dimension);
          int_t compteur=0;
          Cerr << "Construction of the subarea " << le_nom() << finl;
          les_elems_.resize(nb_pol_possible);
          for (int_t n_pol=0; n_pol<nb_pol_possible; n_pol++)
            {
              int_t le_poly=les_polys_possibles_[n_pol];
              x=0;
              for(int som=0; som<nbsom; som++)
                {
                  for(int k=0; k<dimension; k++)
                    x(k)+=dom.coord(ledomaine.sommet_elem(le_poly,som),k);
                }
              x/=((double)(nbsom)); // Center of gravity of the cell
              if ( sup_strict((x(0)-xo)*(x(0)-xo)+(x(1)-yo)*(x(1)-yo),ri2) && sup_strict(re2,(x(0)-xo)*(x(0)-xo)+ (x(1)-yo)*(x(1)-yo)) )
                {
                  les_elems_(compteur)=le_poly;
                  compteur++;
                }
            }
          les_elems_.resize(compteur);
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
            Process::exit();
          }
        is >> motlu;
        if(motlu!=Motcle("Origine"))
          {
            Cerr << "We expected the keyword \"ORIGINE\" " << finl;
            Process::exit();
          }
        double xo,yo,zo;
        is >> xo >> yo >> zo;
        is >> motlu;
        if(motlu!=Motcle("DIR"))
          {
            Cerr << "We expected the tube direction, keyword \"DIR\" " << finl;
            Process::exit();
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
            Cerr << "DIR is equal to X for a tube parallel to OX ; Y for a tube parallel to OY and Z for a tube parallel to OZ" << finl;
            Cerr << "Currently, DIR is equal to " << coord << finl;
            Process::exit();
          }

        is >> motlu;
        if(motlu!=Motcle("RI"))
          {
            Cerr << "We expected the internal radius, keyword \"RI\" " << finl;
            Process::exit();
          }
        double ri,re,h;
        is >> ri;
        is >> motlu;
        if(motlu!=Motcle("RE"))
          {
            Cerr << "We expected the external radius, keyword \"RE\" " << finl;
            Process::exit();
          }
        is >> re;
        is >> motlu;
        if(motlu!=Motcle("Hauteur"))
          {
            Cerr << "We expected the height of tube, keyword \"Hauteur\" " << finl;
            Process::exit();
          }
        is >> h;
        h+=h0;
        {
          double ri2 = ri*ri, re2=re*re;
          const Domaine_t& le_dom=le_dom_.valeur();
          int nbsom=le_dom.nb_som_elem();
          int_t le_poly;
          DoubleVect x(dimension);
          int_t compteur=0;
          Cerr << "Construction of the subarea " << le_nom() << finl;
          les_elems_.resize(nb_pol_possible);
          for (int_t n_pol=0; n_pol<nb_pol_possible; n_pol++)
            {
              le_poly=les_polys_possibles_[n_pol];
              x=0;
              for(int le_som=0; le_som<nbsom; le_som++)
                {
                  for(int k=0; k<dimension; k++)
                    x(k)+=dom.coord(ledomaine.sommet_elem(le_poly,le_som),k);
                }
              x/=((double)(nbsom));
              double tmp = dir[0]*(x(0)-xo)*(x(0)-xo)+ dir[1]*(x(1)-yo)*(x(1)-yo) + dir[2]*(x(2)-zo)*(x(2)-zo);
              if ( sup_ou_egal(tmp,ri2) && inf_ou_egal(tmp,re2) && inf_ou_egal(x(idir),h) && sup_ou_egal(x(idir),h0) )
                {
                  les_elems_(compteur)=le_poly;
                  compteur++;
                }
            }
          les_elems_.resize(compteur);
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
            Process::exit();
          }

        // PQ : 17/09/08 : on suppose le tube hexagonal centre sur l'origine, porte par l'axe z
        // et pour lequel l'entreplat est entre y=-ep/2 et y=+ep/2

        double ep;
        bool in=true;

        is >> motlu;
        if(motlu!=Motcle("ENTREPLAT"))
          {
            Cerr << "We expected the entreplat of the tube, keyword \"ENTREPLAT\"" << finl;
            Process::exit();
          }
        is >> ep;
        is >> motlu;
        if(motlu==Motcle("IN"))       in=1;
        else if(motlu==Motcle("OUT")) in=0;
        else
          {
            Cerr << "We expected the keyword \"IN\" or \"OUT\" " << finl;
            Process::exit();
          }

        {
          const Domaine_t& le_dom=le_dom_.valeur();
          int nbsom=le_dom.nb_som_elem();
          int_t le_poly;
          DoubleVect x(dimension);
          int_t compteur=0;
          Cerr << "Construction of the subarea " << le_nom()<< finl;

          les_elems_.resize(nb_pol_possible);
          for (int_t n_pol=0; n_pol<nb_pol_possible; n_pol++)
            {
              le_poly=les_polys_possibles_[n_pol];
              x=0;
              for(int le_som=0; le_som<nbsom; le_som++)
                {
                  for(int k=0; k<dimension; k++)
                    x(k)+=dom.coord(ledomaine.sommet_elem(le_poly,le_som),k);
                }
              x/=((double)(nbsom));

              if ( sup_ou_egal(x(1),-ep/2.) && sup_ou_egal(x(1),-ep-x(0)*sqrt(3.)) && sup_ou_egal(x(1),-ep+x(0)*sqrt(3.))
                   &&  inf_ou_egal(x(1), ep/2.) && inf_ou_egal(x(1), ep-x(0)*sqrt(3.)) && inf_ou_egal(x(1), ep+x(0)*sqrt(3.)) )
                {
                  if(in==1)
                    {
                      les_elems_(compteur)=le_poly;
                      compteur++;
                    }
                }
              else
                {
                  if(in==0)
                    {
                      les_elems_(compteur)=le_poly;
                      compteur++;
                    }
                }
            }
          les_elems_.resize(compteur);
          Cerr << "Construction of the subarea OK" << finl;
        }
        break;
      }
    case 11:
    case 12:
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
          const Domaine_t& le_dom=le_dom_.valeur();
          int nbsom=le_dom.nb_som_elem();
          int_t le_poly;
          DoubleVect x(dimension);
          int_t compteur=0;
          Cerr << "Construction of the subarea " << le_nom()<< finl;
          les_elems_.resize(nb_pol_possible);
          for (int_t n_pol=0; n_pol<nb_pol_possible; n_pol++)
            {
              le_poly=les_polys_possibles_[n_pol];
              x=0;
              int nbsom_loc = 0;
              for(int le_som=0; le_som<nbsom; le_som++)
                if (ledomaine.sommet_elem(le_poly,le_som) >= 0)
                  {
                    for(int k=0; k<dimension; k++)
                      x(k)+=dom.coord(ledomaine.sommet_elem(le_poly,le_som),k);
                    nbsom_loc++;
                  }
              x *= 1. / nbsom_loc;
              F.setVar(0,x[0]);
              F.setVar(1,x[1]);
              if (dimension==3)
                F.setVar(2,x[2]);
              double test=F.eval();
              // attention le test absolu est voulu
              // si on fait une fonction qui vaut 0 ou 1 ....
              if (test>0)
                {
                  les_elems_(compteur)=le_poly;
                  compteur++;
                }
            }
          les_elems_.resize(compteur);
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
        Process::exit();
      }
      break;
    }
  is >> motlu;
  while (motlu=="union")
    {
      Nom nom_ss_domaine;
      is >> nom_ss_domaine;
      const Sous_Domaine_32_64& ssz=ref_cast(Sous_Domaine_32_64, interprete().objet(nom_ss_domaine));
      std::set<int> poly_set(les_elems_.begin(), les_elems_.end()); //to detect already present elements
      for (int i = 0; i < ssz.nb_elem_tot(); i++)
        if (!poly_set.count(ssz(i))) //only add new elements
          les_elems_.append_array(ssz(i));

      is >>motlu;
    }

  if(motlu != Motcle("}"))
    {
      Cerr << "We expected a } to the reading of the subarea" << finl;
      Cerr << "instead of " << motlu << finl;
      Process::exit();
    }
  return is;
}


/*! @brief Ajoute un polyedre au sous-domaine.
 *
 * @return (int)
 */
template <typename _SIZE_>
void Sous_Domaine_32_64<_SIZE_>::add_elem(const int_t poly)
{
  assert(0<=poly);
  int_t nb_poly=les_elems_.size();
  int OK=1;
  for(int_t i=0; i<nb_poly; i++)
    if(les_elems_(i)==poly)
      OK=0;
  if(OK==1)
    {
      les_elems_.resize(nb_poly+1);
      les_elems_(nb_poly)=poly;
    }
}


/*! @brief Associe un Objet_U au sous-domaine.
 *
 * On controle le type de l'objet a associer
 *     dynamiquement.
 *
 * @param (Objet_U& ob) objet a associer au sous-domaine.
 * @return (int) renvoie 1 si l'association a reussi 0 sinon.
 */
template <typename _SIZE_>
int Sous_Domaine_32_64<_SIZE_>::associer_(Objet_U& ob)
{
  if( sub_type(Domaine_t, ob))
    {
      if(le_dom_.non_nul()) return 1;
      associer_domaine(ref_cast(Domaine_t, ob));
      ob.associer_(*this);
      return 1;
    }
  return 0;
}

template class Sous_Domaine_32_64<int>;
#if INT_is_64_ == 2
template class Sous_Domaine_32_64<trustIdType>;
#endif
