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
// File:        Champ_Don_lu.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/24
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Don_lu.h>
#include <Domaine.h>
#include <LecFicDiffuse.h>
#include <Interprete.h>

Implemente_instanciable(Champ_Don_lu,"Champ_Don_lu",Champ_Don_base);

// Description:
//    Imprime les valeurs du champ sur un flot de sortie
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
Sortie& Champ_Don_lu::printOn(Sortie& os) const
{
  const DoubleTab& tab=valeurs();
  os << tab.size() << " ";
  for(int i=0; i<tab.size(); i++)
    os << tab(0,i);
  return os;
}


// Description:
//    Lit le champ sur un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: impossible d'ouvrir le fichier
// Exception: pas de valeur pour l'element lu
// Effets de bord:
// Postcondition: le champ est initialise avec les donnees lues
Entree& Champ_Don_lu::readOn(Entree& is)
{
  int dim;
  Motcle motlu;
  Nom nom;
  is >> nom;
  mon_domaine = ref_cast(Domaine, Interprete::objet(nom));
  Domaine& domaine=mon_domaine.valeur();
  const Zone& ma_zone = domaine.zone(0);
  int nb_elems = ma_zone.nb_elem();
  dim=lire_dimension(is,que_suis_je());

  dimensionner(nb_elems,dim);
  domaine.creer_tableau_elements(valeurs());

  is >> nom;
  LecFicDiffuse fic(nom);
  Cerr << "Open and read the file : " << nom << finl;

  int nb_val_lues=0;
  Nom chaine;
  fic >> chaine;
  int star_ccm=0;
  // Est ce un fichier .csv de STAR-CCM  avec en tete:
  // "champ1 unite" "champ2 unite" ....  "X" "Y" "Z"
  if (chaine.debute_par("\""))
    {
      star_ccm=1;
      Cerr << "STAR-CCM csv file detected..." << finl;
      Cerr << "Find component 1 of the field: " << chaine << finl;
      for (int i=0; i<dim; i++)
        {
          fic >> chaine; // Unite du champ
          fic >> chaine;
          if (i<dim-1) Cerr << "Find component " << i+2 << " of the field: " << chaine << finl;
        }
      if (chaine!="\"X\"")
        {
          Cerr << "Error in the file. There must be one field with " << dim  << " components." << finl;
          exit();
        }
      fic >> chaine; // "Y"
      fic >> chaine; // "Z"
      // Determination du nombre de lignes du fichier STAR-CCM
      LecFicDiffuse tmp(nom);
      while (!tmp.eof())
        {
          nb_val_lues++;
          tmp >> chaine;
        }
      nb_val_lues-=(dim*2+dimension+1);
      nb_val_lues/=(dim+dimension);
    }
  else
    {
      // Sinon nombre de lignes a lire
      nb_val_lues=atoi(chaine);
      Cerr << "The format of this file containing values of a field should be:" << finl;
      Cerr << "N" << finl;
      int nb_lines=4;
      for (int line=1; line<=nb_lines; line++)
        {
          if (line==nb_lines-1) Cerr << "..." << finl;
          else
            {
              Nom l;
              if (line==nb_lines)
                l="N";
              else
                l=(Nom)line;
              Cerr << "x["<<l<<"] y["<<l<<"] ";
              if (dimension==3) Cerr << "z["<<l<<"] ";
              for (int k=0; k<dim; k++)
                Cerr << "value["<<l<<","<<k<<"] ";
              Cerr << finl;
            }
        }
    }
  Cerr << finl << "Read number of values (N)=" << nb_val_lues << finl;
  // Lecture des valeurs dans le fichier fic
  DoubleTab& mes_val = valeurs();
  IntTab compteur(nb_elems);
  compteur = 0;
  DoubleVect point(dimension);
  DoubleVect val_lu(dim);
  int elem2;
  double x,y,z=0;
  int pourcent=0,tmp;
  for (int i=0; i<nb_val_lues; i++)
    {
      tmp=(i+1)*10/nb_val_lues;
      if (tmp>pourcent)
        {
          pourcent=tmp;
          Cerr<<"\rProcess "<<Process::me()<<" has read "<<pourcent*10<<"% of values"<<flush;
        }

      // Lecture des coordonnees du centre de l'element et des valeurs du champs
      if (star_ccm)
        {
          for (int k=0; k<dim; k++) fic >> val_lu[k];
          for (int k=0; k<dimension; k++) fic >> point[k];
        }
      else
        {
          for (int k=0; k<dimension; k++) fic >> point[k];
          for (int k=0; k<dim; k++) fic >> val_lu[k];
        }
      x=point[0];
      y=point[1];
      if (Objet_U::dimension==3) z=point[2];

      // Recherche de l'element
      elem2=ma_zone.chercher_elements(x,y,z);
      if ((elem2!=-1) && (elem2<nb_elems))
        {
          for (int k=0; k<dim; k++)
            mes_val(elem2,k) = val_lu[k];
          compteur[elem2] += 1;
        }
    }
  Cerr << finl;
  // Verification que tous les elements ont ete trouves
  for (int num_elem=0; num_elem<nb_elems; num_elem++)
    if (compteur[num_elem] == 0)
      {
        Cerr << "Error when using Champ_Don_lu keyword:" << finl;
        Cerr << "There is no value found for the cell number " << num_elem << finl;
        Cerr << "Check your mesh and/or the file " << nom << finl;
        exit();
      }

  mes_val.echange_espace_virtuel();

  return is;
}


// Description:
//    Provoque une erreur: cela n'a pas de sens
//    de vouloir effectuer une affectation avec un Champ_Don_lu.
// Precondition:
// Parametre: Champ_base&
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: NON ACCEDE
// Retour: Champ_base&
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_base& Champ_Don_lu::affecter(const Champ_base& )
{
  Champ_base::affecter_erreur();
  return *this;
}


// Description:
//    Renvoie la valeur du champ au point specifie
//    par ses coordonnees.
// Precondition:
// Parametre: DoubleVect& positions
//    Signification: les coordonnees du point de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleVect& valeurs
//    Signification: la valeur du champ au point specifie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: DoubleVect&
//    Signification: la valeur du champ au point specifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleVect& Champ_Don_lu::valeur_a(const DoubleVect& positions,
                                   DoubleVect& tab_valeurs) const
{
  const Zone& la_zone = mon_domaine->zone(0);
  IntVect le_poly(1);
  la_zone.chercher_elements(positions,le_poly);
  return valeur_a_elem(positions,tab_valeurs,le_poly(0));
}


// Description:
//    Renvoie la valeur du champ au point specifie
//    par ses coordonnees, en indiquant que ce point est
//    situe dans un element specifie.
// Precondition:
// Parametre: DoubleVect&
//    Signification: les coordonnees du point de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleVect& val
//    Signification: la valeur du champ au point specifie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Parametre: int le_poly
//    Signification: l'element dans lequel est situe le point de calcul
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleVect&
//    Signification: la valeur du champ au point specifie
//    Contraintes:
// Exception: taille du tableau des valeurs incorrecte
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleVect& Champ_Don_lu::valeur_a_elem(const DoubleVect&,
                                        DoubleVect& val,
                                        int le_poly) const
{
  if (val.size() != nb_compo_)
    {
      Cerr << "Error TRUST in Champ_Don_lu::valeur_a()" << finl;
      Cerr << "The DoubleVect val doesn't have the correct size" << finl;
      exit();
    }

  const DoubleTab& ch = valeurs();

  if (nb_compo_ == 1)
    val(0) = ch(le_poly,0);
  else
    for (int k=0; k<nb_compo_; k++)
      val(k) = ch(le_poly,k);

  return val;
}


// Description:
//    Renvoie la valeur d'une composante du champ au point specifie
//    par ses coordonnees, en indiquant que ce point est
//    situe dans un element specifie.
// Precondition:
// Parametre: DoubleVect&
//    Signification: les coordonnees du point de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: int le_poly
//    Signification: l'element dans lequel est situe le point de calcul
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int ncomp
//    Signification: l'index de la composante du champ a calculer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: la valeur de la composante du champ specifiee au
//                   point specifie
//    Contraintes:
// Exception: l'index de la composante du champ specifie est plus grand
//            que le nombre de composante du champ
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
double Champ_Don_lu::valeur_a_elem_compo(const DoubleVect& ,
                                         int le_poly,int ncomp) const
{
  double val;
  if (ncomp > nb_compo_)
    {
      Cerr << "Error TRUST in Champ_Don_lu::valeur_a()" << finl;
      Cerr << "the integer ncomp is upper than the number of field components" << finl;
      exit();
    }

  const DoubleTab& ch = valeurs();

  if (nb_compo_ == 1)
    val = ch(le_poly,0);
  else
    val = ch(le_poly,ncomp);

  return val;
}


// Description:
//    Renvoie les valeurs du champ aux points specifies
//    par leurs coordonnees.
// Precondition:
// Parametre: DoubleTab& positions
//    Signification: le tableau des coordonnees des points de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleTab& valeurs
//    Signification: le tableau des valeurs du champ aux points specifies
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: DoubleTab&
//    Signification: le tableau des valeurs du champ aux points specifies
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Champ_Don_lu::valeur_aux(const DoubleTab& positions,
                                    DoubleTab& tab_valeurs) const
{
  const Zone& la_zone = mon_domaine->zone(0);
  IntVect les_polys(la_zone.nb_elem());
  la_zone.chercher_elements(positions,les_polys);
  return valeur_aux_elems(positions,les_polys,tab_valeurs);
}


// Description:
//    Renvoie les valeurs d'une composante du champ aux points specifies
//    par leurs coordonnees.
// Precondition:
// Parametre: DoubleVect& positions
//    Signification: le tableau des coordonnees des points de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleVect& valeurs
//    Signification: le tableau des valeurs de la composante du champ
//                   aux points specifies
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Parametre: int ncomp
//    Signification: l'index de la composante du champ a calculer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleVect&
//    Signification: le tableau des valeurs de la composante du champ
//                   aux points specifies
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleVect& Champ_Don_lu::valeur_aux_compo(const DoubleTab& positions,
                                           DoubleVect& tab_valeurs,
                                           int ncomp) const
{
  const Zone& la_zone = mon_domaine->zone(0);
  IntVect les_polys(la_zone.nb_elem());
  la_zone.chercher_elements(positions,les_polys);
  return valeur_aux_elems_compo(positions,les_polys,tab_valeurs,ncomp);
}


// Description:
//    Renvoie les valeurs du champ aux points specifies
//    par leurs coordonnees, en indiquant que les points de
//    calculs sont situes dans les elements indiques.
// Precondition: le nombre de valeurs demandes doit etre
//               egal aux nombre d'elements specifies
// Precondition: la dernier dimension du tableau des valeurs
//               doit etre egal au nombre de composante du champ
// Parametre: DoubleTab&
//    Signification: le tableau des coordonnees des points de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: IntVect& les_polys
//    Signification: le tableau des elements dans lesquels sont
//                   situes les points de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleTab& val
//    Signification: le tableau des valeurs du champ aux points specifies
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: DoubleTab&
//    Signification: le tableau des valeurs du champ aux points specifies
//    Contraintes:
// Exception: le tableau des valeurs a plus de 2 entrees (rang du tableau > 2)
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Champ_Don_lu::valeur_aux_elems(const DoubleTab& ,
                                          const IntVect& les_polys,
                                          DoubleTab& val) const
{
  if (val.nb_dim() == 1)
    {
      assert((val.dimension(0) == les_polys.size())||(val.dimension_tot(0) == les_polys.size()));
      assert(nb_compo_ == 1);
    }
  else if (val.nb_dim() == 2)
    {
      assert((val.dimension(0) == les_polys.size())||(val.dimension_tot(0) == les_polys.size()));
      assert(val.dimension(1) == nb_compo_);
    }
  else
    {
      Cerr << "Error TRUST in Champ_Don_lu::valeur_aux_elems()" << finl;
      Cerr << "The DoubleTab val has more than 2 entries" << finl;
      exit();
    }

  int le_poly;

  if (nb_compo_ == 1)
    {
      const DoubleTab& ch = valeurs();

      for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
        {
          le_poly=les_polys(rang_poly);
          if (le_poly == -1)
            val(rang_poly) = 0;
          else if (val.nb_dim()==1)
            val(rang_poly) = ch(le_poly,0);
          else
            val(rang_poly,0) = ch(le_poly,0);
        }
    }
  else // nb_compo_ > 1
    {
      const DoubleTab& ch = valeurs();

      for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
        {
          le_poly=les_polys(rang_poly);
          if (le_poly == -1)
            for(int ncomp=0; ncomp<nb_compo_; ncomp++)
              val(rang_poly, ncomp) = 0;
          else
            for(int ncomp=0; ncomp<nb_compo_; ncomp++)
              val(rang_poly, ncomp) = ch(le_poly,ncomp);
        }

    }
  return val;
}


// Description:
//    Renvoie les valeurs d'une composante du champ aux points specifies
//    par leurs coordonnees, en indiquant que les points de
//    calculs sont situes dans les elements indiques.
// Precondition:
// Parametre: DoubleTab&
//    Signification: le tableau des coordonnees des points de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: IntVect& les_polys
//    Signification: le tableau des elements dans lesquels sont
//                   situes les points de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleVect& val
//    Signification: le tableau des valeurs de la composante du champ
//                   aux points specifies
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Parametre: int ncomp
//    Signification: l'index de la composante du champ a calculer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleVect&
//    Signification: le tableau des valeurs de la composante du champ
//                   aux points specifies
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleVect& Champ_Don_lu::valeur_aux_elems_compo(const DoubleTab& ,
                                                 const IntVect& les_polys,
                                                 DoubleVect& val,
                                                 int ncomp) const
{
  assert(val.size() == les_polys.size());
  int le_poly;

  const DoubleTab& ch = valeurs();

  for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
    {
      le_poly=les_polys(rang_poly);
      if (le_poly == -1)
        val(rang_poly) = 0;
      else
        val(rang_poly) = ch(le_poly,ncomp);
    }
  return val;
}
