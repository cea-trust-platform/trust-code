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
// File:        Rotation.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/3
//
//////////////////////////////////////////////////////////////////////////////

#include <Rotation.h>

Implemente_instanciable(Rotation,"Rotation",Transformer);


// Description:
//    Simple appel a: Transformer::printOn(Sortie&)
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
Sortie& Rotation::printOn(Sortie& os) const
{
  return Transformer::printOn(os);
}

// Description:
// Simple appel a: Transformer::ReadOn(Sortie&)
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
Entree& Rotation::readOn(Entree& is)
{
  return Transformer::readOn(is);
}

Entree& Rotation::interpreter_(Entree& is)
{
  associer_domaine(is);
  verifie_type_elem();

  Noms les_fcts;
  les_fcts.dimensionner(dimension);

  int dir_axe,dir1,dir2;
  double coor1,coor2,angle;
  Nom axe_direction,nomc1,nomc2,nomang;
  Nom var1,var2,var_axe;
  double pi = M_PI;
  is>>axe_direction;
  ////is>>dir_axe;
  dir_axe=-1;
  if (Motcle(axe_direction)=="X")
    dir_axe=0;
  else if (Motcle(axe_direction)=="Y")
    dir_axe=1;
  else if (Motcle(axe_direction)=="Z")
    dir_axe=2;
  else
    {
      Cerr<<"The direction of the rotation axis must be specified by X, Y or Z"<<finl;
      exit();
    }

  is>>coor1>>coor2>>angle;
  angle = (2*pi)*(angle/360.);
  nomc1 = Nom(coor1);
  nomc2 = Nom(coor2);
  nomang = Nom(angle);

  dir1=-1;
  dir2=-1;
  if (dimension==3)
    {
      if (dir_axe==0)
        {
          var1="(y-";
          var2="(z-";
          var_axe="x";
          dir1=1;
          dir2=2;
        }
      else if (dir_axe==1)
        {
          var1="(z-";
          var2="(x-";
          var_axe="y";
          dir1=2;
          dir2=0;
        }
      else if (dir_axe==2)
        {
          var1="(x-";
          var2="(y-";
          var_axe="z";
          dir1=0;
          dir2=1;
        }

      les_fcts[dir_axe] = var_axe;
    }
  else
    {
      var1="(x-";
      var2="(y-";
      dir1=0;
      dir2=1;
    }

  les_fcts[dir1] = var1;
  les_fcts[dir1] += "(";
  les_fcts[dir1] += nomc1;
  les_fcts[dir1] += ")";
  les_fcts[dir1] += ")*cos(";
  les_fcts[dir1] += nomang;
  les_fcts[dir1] += ")-";
  les_fcts[dir1] += var2;
  les_fcts[dir1] += "(";
  les_fcts[dir1] += nomc2;
  les_fcts[dir1] += ")";
  les_fcts[dir1] += ")*sin(";
  les_fcts[dir1] += nomang;
  les_fcts[dir1] += ")";
  les_fcts[dir1] += "+(";
  les_fcts[dir1] += nomc1;
  les_fcts[dir1] += ")";

  les_fcts[dir2] = var1;
  les_fcts[dir2] += "(";
  les_fcts[dir2] += nomc1;
  les_fcts[dir2] += ")";
  les_fcts[dir2] += ")*sin(";
  les_fcts[dir2] += nomang;
  les_fcts[dir2] += ")+";
  les_fcts[dir2] += var2;
  les_fcts[dir2] += "(";
  les_fcts[dir2] += nomc2;
  les_fcts[dir2] += ")";
  les_fcts[dir2] += ")*cos(";
  les_fcts[dir2] += nomang;
  les_fcts[dir2] += ")";
  les_fcts[dir2] += "+(";
  les_fcts[dir2] += nomc2;
  les_fcts[dir2] += ")";

  transformation_complete(les_fcts);
  return is;
}



