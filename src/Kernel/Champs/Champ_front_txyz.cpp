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
// File:        Champ_front_txyz.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////
//   Nouveau champ frontiere

#include <Champ_front_txyz.h>
#include <Domaine.h>
#include <Frontiere_dis_base.h>
#include <DoubleTrav.h>

Implemente_instanciable(Champ_front_txyz,"Champ_front_fonc_txyz",Ch_front_var_instationnaire_indep);


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
Sortie& Champ_front_txyz::printOn(Sortie& os) const
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
//      Champ_front_txyz nb_compo vrel_1 ... [vrel_i]
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
Entree& Champ_front_txyz::readOn(Entree& is)
{
  //Cout << "Dans  Champ_front_txyz.readOn !!!!!!!!!" << finl;
  int dim;
  dim=lire_dimension(is,que_suis_je());
  fixer_nb_comp(dim);

  fxyz.dimensionner(dim);


  //        Cout << "dim = " << dim << finl;
  for (int i = 0; i<dim; i++)
    {
      Nom tmp;
      //Cout << "i = " << i << finl;
      is >> tmp;
      //Cout << "fonc = " << tmp << finl;
      Cerr << "Reading and interpretation of the function " << tmp << finl;
      fxyz[i].setNbVar(4);
      fxyz[i].setString(tmp);
      fxyz[i].addVar("t");
      fxyz[i].addVar("x");
      fxyz[i].addVar("y");
      fxyz[i].addVar("z");
      fxyz[i].parseString();
      Cerr << "Interpretation of function " << tmp << " Ok" << finl;
      //        Cout << "end = " << tmp << finl;
    }

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
Champ_front_base& Champ_front_txyz::affecter_(const Champ_front_base& ch)
{
  return *this;
}


void Champ_front_txyz::mettre_a_jour(double temps)
{
  int dim=nb_comp();
  const Frontiere_dis_base& fr_dis=frontiere_dis();
  const Frontiere& frontiere=fr_dis.frontiere();
  const Zone& zone=frontiere.zone();
  const Domaine& domaine=zone.domaine();
  const Faces& faces=frontiere.faces();
  int nb_faces=frontiere.nb_faces();
  double x,y,z;
  int nbsf=faces.nb_som_faces();
  int i,k;
  DoubleTab& tab=valeurs_au_temps(temps);
  for( i=0; i<nb_faces; i++)
    {
      x=y=z=0;
      for( k=0; k<nbsf; k++)
        {
          x+=domaine.coord(faces.sommet(i,k),0);
          if(dimension>1)
            y+=domaine.coord(faces.sommet(i,k),1);
          if(dimension>2)
            z+=domaine.coord(faces.sommet(i,k),2);
        }
      x/=nbsf;
      y/=nbsf;
      z/=nbsf;
      for( k=0; k<dim; k++)
        {
          fxyz[k].setVar("t",temps);
          fxyz[k].setVar("x",x);
          fxyz[k].setVar("y",y);
          fxyz[k].setVar("z",z);
          tab(i,k)=fxyz[k].eval();
        }
    }
  tab.echange_espace_virtuel();
}

double Champ_front_txyz::valeur_au_temps_et_au_point(double temps,int som,double x,double y, double z, int k) const
{
  Parser_U& fxyzk=fxyz[k];
  fxyzk.setVar("t",temps);
  fxyzk.setVar("x",x);
  fxyzk.setVar("y",y);
  fxyzk.setVar("z",z);
  return fxyzk.eval();
}
