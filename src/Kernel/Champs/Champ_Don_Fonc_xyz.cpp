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
// File:        Champ_Don_Fonc_xyz.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/27
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Don_Fonc_xyz.h>
#include <Domaine.h>
#include <Interprete.h>
#include <Champ_Uniforme.h>

Implemente_instanciable(Champ_Don_Fonc_xyz,"Champ_Fonc_xyz",Champ_Don_base);


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
Sortie& Champ_Don_Fonc_xyz::printOn(Sortie& os) const
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
Entree& Champ_Don_Fonc_xyz::readOn(Entree& is)
{
  int dim;
  Motcle motlu;
  Nom nom;
  is >> nom;
  mon_domaine = ref_cast(Domaine, Interprete::objet(nom));
  Domaine& domaine=mon_domaine.valeur();
  const Zone& ma_zone = domaine.zone(0);
  const IntTab& les_Polys = ma_zone.les_elems();
  int nb_elems = ma_zone.nb_elem(),num_som;
  dim=lire_dimension(is,que_suis_je());
  dimensionner(nb_elems,dim);
  domaine.creer_tableau_elements(valeurs());

  DoubleTab& mes_val = valeurs();
  IntTab compteur(nb_elems);
  compteur = 0;
  int num_elem;
  int dernier_elem=0;
  double x,y,z;

  VECT(Parser_U)& fxyz = fonction();
  fxyz.dimensionner(dim);

  for (int i=0; i<dim; i++)
    {
      Nom tmp;
      is >> tmp;
      Cerr << "Reading and interpretation of the function " << tmp << finl;
      fxyz[i].setNbVar(3);
      fxyz[i].setString(tmp);
      fxyz[i].addVar("x");
      fxyz[i].addVar("y");
      if (dimension>2)
        fxyz[i].addVar("z");
      fxyz[i].parseString();
      Cerr << "Interpretation of function " << tmp << " Ok" << finl;
    }

  DoubleTab positions(nb_elems-dernier_elem,dimension);
  for (num_elem=dernier_elem; num_elem<nb_elems; num_elem++)
    {
      x=y=z=0;
      int s, n;
      for(s = n = 0; s < les_Polys.dimension(1); s++) if ((num_som = les_Polys(num_elem,s)) >= 0)
          {
            x += domaine.coord(num_som,0);
            y += domaine.coord(num_som,1);
            if (dimension>2) z += domaine.coord(num_som,2);
            n++;
          }
      positions(num_elem,0) = x/n;
      positions(num_elem,1) = y/n;
      if (dimension>2)
        positions(num_elem,2) = z/n;
    }

  eval_fct(positions,mes_val);
  mes_val.echange_espace_virtuel();

  return is;
}


// Description:
//    Provoque une erreur: cela n'a pas de sens
//    de vouloir effectuer une affectation avec un Champ_Don_Fonc_xyz.
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
Champ_base& Champ_Don_Fonc_xyz::affecter(const Champ_base& )
{
  Champ_base::affecter_erreur();
  return *this;
}

Champ_base& Champ_Don_Fonc_xyz::affecter_(const Champ_base& ch)
{
  if(sub_type(Champ_Uniforme, ch))
    valeurs()=ch.valeurs()(0,0);
  else
    valeurs()=ch.valeurs();
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
DoubleVect& Champ_Don_Fonc_xyz::valeur_a(const DoubleVect& x,
                                         DoubleVect& val) const
{
  if (val.size() != nb_compo_)
    {
      Cerr << "Error TRUST in "<<que_suis_je()<<"::valeur_a()" << finl;
      Cerr << "The DoubleVect val doesn't have the correct size" << finl;
      exit();
    }
  eval_fct(x,val);
  return val;
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
double Champ_Don_Fonc_xyz::valeur_a_compo(const DoubleVect& x,
                                          int ncomp) const
{
  double val;
  if (ncomp > nb_compo_)
    {
      Cerr << "Error TRUST in "<<que_suis_je()<<"::valeur_a_compo()" << finl;
      Cerr << "the integer ncomp is upper than the number of field components" << finl;
      exit();
    }

  DoubleTab positions(1,dimension);
  DoubleTab val_fct(1);
  positions(0,0) = x(0);
  positions(0,1) = x(1);
  if (dimension>2)
    positions(0,2) = x(2);

  eval_fct(positions,val_fct,ncomp);
  val = val_fct(0);

  return val;
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
DoubleVect& Champ_Don_Fonc_xyz::valeur_a_elem(const DoubleVect& x,
                                              DoubleVect& val,
                                              int le_poly) const
{

  return valeur_a(x,val);
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
double Champ_Don_Fonc_xyz::valeur_a_elem_compo(const DoubleVect& x,
                                               int le_poly,
                                               int ncomp) const
{
  if (ncomp > nb_compo_)
    {
      Cerr << "Error TRUST in "<<que_suis_je()<<"::valeur_a()" << finl;
      Cerr << "the integer ncomp is upper than the number of field components" << finl;
      exit();
    }

  return valeur_a_compo(x,ncomp);
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
DoubleTab& Champ_Don_Fonc_xyz::valeur_aux(const DoubleTab& x,
                                          DoubleTab& val) const
{
  if (val.nb_dim() > 2)
    {
      Cerr << "Error TRUST in "<<que_suis_je()<<"::valeur_aux_elems()" << finl;
      Cerr << "The DoubleTab val has more than 2 entries" << finl;
      exit();
    }
  eval_fct(x,val);
  return val;
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
DoubleVect& Champ_Don_Fonc_xyz::valeur_aux_compo(const DoubleTab& x,
                                                 DoubleVect& val,
                                                 int ncomp) const
{
  eval_fct(x,val,ncomp);
  return val;
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
DoubleTab& Champ_Don_Fonc_xyz::valeur_aux_elems(const DoubleTab& x,
                                                const IntVect& les_polys,
                                                DoubleTab& val) const
{
  return valeur_aux(x,val);
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
DoubleVect& Champ_Don_Fonc_xyz::valeur_aux_elems_compo(const DoubleTab& x,
                                                       const IntVect& les_polys,
                                                       DoubleVect& val,
                                                       int ncomp) const
{
  return valeur_aux_compo(x,val,ncomp);
}
