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
// File:        Transformer.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Transformer.h>
#include <Vect_Parser_U.h>
#include <Rectangle.h>
#include <Hexaedre.h>
#include <Scatter.h>

Implemente_instanciable(Transformer,"Transformer",Interprete_geometrique_base);


// Description:
//    Simple appel a: Interprete::printOn(Sortie&)
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
Sortie& Transformer::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}


// Description:
//    Simple appel a: Interprete::readOn(Entree&)
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
// Postcondition:
Entree& Transformer::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

Entree& Transformer::interpreter_(Entree& is)
{
  associer_domaine(is);
  verifie_type_elem();

  Noms les_fcts;

  les_fcts.dimensionner(dimension);

  for (int i=0; i<dimension; i++)
    {
      is >> les_fcts[i];
    }

  transformation_complete(les_fcts);
  return is;
}


void Transformer::transformer(Domaine& dom, Noms& les_fcts)
{
  VECT(Parser_U) fxyz(dimension);

  for (int i = 0; i<dimension; i++)
    {
      Cerr << "Reading and interpretation of the function " << les_fcts[i] << finl;
      fxyz[i].setNbVar(3);
      fxyz[i].setString(les_fcts[i]);
      fxyz[i].addVar("x");
      fxyz[i].addVar("y");
      fxyz[i].addVar("z");
      fxyz[i].parseString();
      Cerr << "Interpretation of the function " << les_fcts[i] << " Ok" << finl;
    }
  DoubleTab& sommets=dom.les_sommets();
  DoubleTab new_sommets(sommets);
  int sz=sommets.dimension(0);
  double x=0;
  double y=0;
  double z=0;
  for (int i = 0; i<sz; i++)
    {
      for (int j = 0; j<dimension; j++)
        {
          x=sommets(i,0);
          y=sommets(i,1);
          z=0;

          if (dimension >2 )
            z = sommets(i,2);
        }
      for(int  j=0; j<dimension; j++)
        {
          fxyz[j].setVar("x",x);
          fxyz[j].setVar("y",y);
          fxyz[j].setVar("z",z);
          new_sommets(i,j)=fxyz[j].eval();
        }
    }
  sommets=new_sommets;
}

void Transformer::verifie_type_elem()
{
  Elem_geom& type_elem=domaine().zone(0).type_elem();
  if (type_elem->que_suis_je() == "Hexaedre_VEF")
    {
      Cerr << "------------------------------------------------------------------" << finl;
      Cerr << "It is advised to not use directly \"Transformer\" on a mesh" << finl;
      Cerr << "with VEF hexahedra since the method Hexaedre_VEF::reordonner" << finl;
      Cerr << "is not fairly general in TRUST." << finl;
      Cerr << "If your purpose is to make tetrahedra on your mesh," << finl;
      Cerr << "create it first after reading your hexahedral meshing," << finl;
      Cerr << "then use \"Transformer\"." << finl;
      Cerr << "------------------------------------------------------------------" << finl;
      exit();
    }
}

void Transformer::transformation_complete(Noms& les_fcts)
{
  Scatter::uninit_sequential_domain(domaine());
  transformer(domaine(), les_fcts);

  // Transformation de l'element parfois necessaire
  Elem_geom& type_elem=domaine().zone(0).type_elem();
  if (type_elem->que_suis_je() == "Rectangle")
    {
      if (ref_cast(Rectangle,type_elem.valeur()).reordonner_elem()==-1) // Le reordonner_elem revele que l'on n'a plus des Rectangle
        {
          type_elem.typer("Quadrangle");
          type_elem.associer_zone(domaine().zone(0));
        }
    }
  if (type_elem->que_suis_je() == "Hexaedre")
    {
      if (ref_cast(Hexaedre,type_elem.valeur()).reordonner_elem()==-1) // Le reordonner_elem revele que l'on n'a plus des Hexaedre
        {
          type_elem.typer("Hexaedre_VEF");
          type_elem.associer_zone(domaine().zone(0));
          Bords& les_bords=domaine().zone(0).faces_bord();
          LIST_CURSEUR(Bord) curseur=les_bords;
          while (curseur)
            {
              curseur->faces().typer(quadrangle_3D);
              ++curseur;
            }
          les_bords.associer_zone(domaine().zone(0));
        }
    }
  // Il faut reordonner apres un Transformer -y x par exemple car
  // la numerotation des elements est melangee... Le probleme c'est
  // que tous les reordonner ne sont pas correctement faits...
  // 2-3
  // 0-1
  domaine().reordonner();
  Scatter::init_sequential_domain(domaine());
}

