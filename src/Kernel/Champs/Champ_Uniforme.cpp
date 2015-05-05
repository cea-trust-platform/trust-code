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
// File:        Champ_Uniforme.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/21
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Uniforme.h>
#include <Synonyme_info.h>

Implemente_instanciable(Champ_Uniforme,"Champ_Uniforme",Champ_Don_base);
Add_synonym(Champ_Uniforme,"Uniform_field");

// Description:
//    Imprime le champ sur un flot de sortie.
//    Imprime la taille du tableau et les valeurs.
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
Sortie& Champ_Uniforme::printOn(Sortie& os) const
{
  const DoubleTab& tab=valeurs();
  os << tab.size() << " ";
  for(int i=0; i<tab.size(); i++)
    os << tab(0,i);
  return os;
}


// Description:
//    Lit un champ a partir d'un flot d'entree.
//    On lit le nombre de composante du champ (nb_comp)
//    et les valeurs de ces composantes.
//    Format:
//      Champ_Uniforme nb_comp vrel_1 ... [vrel_i]
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
// Postcondition: le champ contient les valeurs lues
Entree& Champ_Uniforme::readOn(Entree& is)
{
  int dim;
  DoubleTab& tab=valeurs();
  //  is >> dim;
  dim=lire_dimension(is,que_suis_je());
  dimensionner(1,dim);
  for(int i=0; i<dim; i++)
    is >> tab(0,i);
  return is;
}



// Description:
//    Renvoie l'objet upcaste en Champ_base.
// Precondition:
// Parametre: Champ_base& ch
//    Signification: un champ (parametre inutilise)
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Champ_base&
//    Signification: renvoie (*this) upcaste en Champ_base
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_base& Champ_Uniforme::affecter_(const Champ_base& ch)
{
  if(sub_type(Champ_Uniforme, ch))
    {
      valeurs()=ch.valeurs();
    }
  else
    {
      Cerr << "We do not know how to make Champ_Uniforme::affecter_("<<ch.que_suis_je() << finl;
      exit();
    }
  return *this;
}


// Description:
//    Renvoie la valeur du champ au point specifie
//    par ses coordonnees.
// Precondition:
// Parametre: DoubleVect&
//    Signification: les coordonnees du point de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleVect&
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
DoubleVect& Champ_Uniforme::valeur_a(const DoubleVect& ,
                                     DoubleVect& tab_valeurs) const
{
  for(int j=0; j<nb_comp(); j++)
    tab_valeurs(j)=valeurs_(0,j);
  return tab_valeurs;
}


// Description:
//    Renvoie la valeur de la composante du champ au point specifie
//    par ses coordonnees.
// Precondition:
// Parametre: DoubleVect&
//    Signification: les coordonnees du point de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleVect&
//    Signification: les coordonnees du  point specifie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Parametre: int ncomp
//    Signification: l'index de la composante du champ a calculer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: la valeur de la composante du champ au point specifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
double Champ_Uniforme::valeur_a_compo(const DoubleVect& position,
                                      int ncomp) const
{
  return valeurs_(0,ncomp);
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
// Parametre: DoubleVect&
//    Signification: la valeur du champ au point specifie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Parametre: int
//    Signification: l'element dans lequel est situe le point de calcul
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleVect&
//    Signification: la valeur du champ au point specifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleVect& Champ_Uniforme::valeur_a_elem(const DoubleVect& ,
                                          DoubleVect& tab_valeurs,
                                          int ) const
{
  for(int j=0; j<nb_comp(); j++)
    tab_valeurs(j)=valeurs_(0,j);
  return tab_valeurs;
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
// Parametre: int
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
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
double Champ_Uniforme::valeur_a_elem_compo(const DoubleVect& ,
                                           int ,int ncomp) const
{
  return valeurs_(0,ncomp);
}


// Description:
//    Renvoie les valeurs du champ aux points specifies
//    par leurs coordonnees.
// Precondition:
// Parametre: DoubleTab&
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
DoubleTab& Champ_Uniforme::valeur_aux(const DoubleTab& ,
                                      DoubleTab& tab_valeurs) const
{
  // GF dimension_tot pour que la ligne soit valide pour les champs P1B
  for(int i=0; i<tab_valeurs.dimension_tot(0); i++)
    if (tab_valeurs.nb_dim()==1)
      tab_valeurs(i)=valeurs_(0,0);
    else
      for(int j=0; j<tab_valeurs.dimension(1); j++)
        tab_valeurs(i,j)=valeurs_(0,j);
  return tab_valeurs;
}


// Description:
//    Renvoie les valeurs d'une composante du champ aux points specifies
//    par leurs coordonnees.
// Precondition:
// Parametre: DoubleTab&
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
DoubleVect& Champ_Uniforme::valeur_aux_compo(const DoubleTab& ,
                                             DoubleVect& tab_valeurs, int ncomp) const
{
  return tab_valeurs=valeurs_(0,ncomp);
}


// Description:
//    Renvoie les valeurs du champ aux points specifies
//    par leurs coordonnees, en indiquant que les points de
//    calculs sont situes dans les elements indiques.
// Precondition:
// Parametre: DoubleTab&
//    Signification: le tableau des coordonnees des points de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: IntVect&
//    Signification: le tableau des elements dans lesquels sont
//                   situes les points de calcul
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
DoubleTab& Champ_Uniforme::valeur_aux_elems(const DoubleTab& ,
                                            const IntVect& ,
                                            DoubleTab& tab_valeurs) const
{
  for(int i=0; i<tab_valeurs.dimension(0); i++)
    if (tab_valeurs.nb_dim()==1)
      tab_valeurs(i)=valeurs_(0,0);
    else
      for(int j=0; j<tab_valeurs.dimension(1); j++)
        tab_valeurs(i,j)=valeurs_(0,j);
  return tab_valeurs;
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
// Parametre: IntVect&
//    Signification: le tableau des elements dans lesquels sont
//                   situes les points de calcul
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
DoubleVect& Champ_Uniforme::valeur_aux_elems_compo(const DoubleTab& ,
                                                   const IntVect& ,
                                                   DoubleVect& tab_valeurs,
                                                   int ncomp) const
{
  return tab_valeurs=valeurs_(0,ncomp);
}

