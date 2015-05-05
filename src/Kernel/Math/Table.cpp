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
// File:        Table.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////

#include <Table.h>

Implemente_instanciable_sans_constructeur_ni_destructeur(Table,"Table",Objet_U);


// Description:
//    Constructeur par defaut.
//    Table vide.
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
Table::Table() : les_valeurs() , les_parametres(),isf(0)  { }

Table::Table(const Table& t): Objet_U(t),Parser_Eval()
{
  (*this)=t;
}

//Lecture d'une expression analytique dependant des valeurs (val) d'un champ parametre
//isf est fixe a 1
Entree& Table::lire_f(Entree& is)
{
  isf=1;
  Nom tmp;
  is >> tmp;

  Cerr << "Reading and interpretation of the function " << tmp << finl;
  Cerr<< "unknow : val"<<finl;
  VECT(Parser_U)& fval=fonction();
  fval.dimensionner(1);
  fval[0].setNbVar(1);
  fval[0].setString(tmp);
  fval[0].addVar("val");
  fval[0].parseString();
  Cerr << "Interpretation of the function " << tmp << " OK" << finl;

  return is;
}

//Lecture d'une expression analytique dependant des valeurs (val) d'un champ parametre,
//de l espace (x,y,z) et du temps (t)
//isf est fixe a 2
Entree& Table::lire_fxyzt(Entree& is,const int& dim)
{
  isf=2;
  Nom tmp;

  VECT(Parser_U)& fval=fonction();
  fval.dimensionner(dim);
  for (int i=0; i<dim; i++)
    {
      is >> tmp;
      fval[i].setNbVar(5);
      fval[i].setString(tmp);
      fval[i].addVar("t");
      fval[i].addVar("x");
      fval[i].addVar("y");
      fval[i].addVar("z");
      fval[i].addVar("val");
      fval[i].parseString();
    }
  Cerr << "Interpretation of the function " << tmp << " OK" << finl;


  return is;
}

// Description:
//    Ecriture sur un flot de Sortie
//    N'ecrit que le type et le nom de l'objet
// Precondition:
// Parametre: Sortie& s
//    Signification: le flot de sortie a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& Table::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}



// Description:
//    Lecture sur un flot d'entree
//    Lecture des parametres et des valeurs de la table.
// Precondition:
// Parametre: Entree& s
//    Signification: le flot d'entree a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Table::readOn(Entree& s )
{
  s >> les_parametres;
  s >> les_valeurs;
  return s ;
}


// Description:
//    Retourne la valeur calculee pour le point val_param
//    La valeur est exacte si le point correspond a un parametre donne.
//    Sinon, la valeur est interpolee (interp. lineaire d'ordre 1)
// Precondition:
// Parametre: const double& val_param
//    Signification: le point en lequel calculer la valeur
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: la valeur calculee
//    Contraintes:
// Exception:
//    Sort en erreur s'il manque des parametres
// Effets de bord:
// Postcondition:
double Table::val(const double& val_param) const
{
  if (les_parametres.size() == 1)
    {
      int size;
      const DoubleVect& p = les_parametres[0];
      if (p[0] >= val_param)
        return les_valeurs[0];
      else
        {
          size=p.size();
          for (int i=1; i<size; i++)
            if (p[i] == val_param)
              return les_valeurs[i];
            else if (p[i] > val_param)
              return (les_valeurs[i-1]+((les_valeurs[i]-les_valeurs[i-1])/(p[i]-p[i-1]))*(val_param-p[i-1]) );
        }
      return les_valeurs[size-1];
    }
  else if (isf==1)
    {
      // Plus rapide que parser(0).setVar("val",val_param);
      parser(0).setVar(0,val_param);
      return parser(0).eval();
    }
  else
    {
      Cerr << "Error in a Table::val : it misses some parameters." << finl;
      exit();
    }
  return 0;
}


// Description:
//    Pas encore code. Sort en erreur.
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
double Table::val(const DoubleVect& val_param) const
{
  Cerr << "Table::val(const DoubleVect& ) is not coded yet." << finl;
  exit();
  return 0;
}


// Description:
//    Retourne les valeurs calculees pour le point val_param (cas du tableau valeurs a 2 dimensions)
//    Les valeurs sont exacteS si le point correspond a un parametre donne.
//    Sinon, les valeurs sont interpolees (interp. lineaire d'ordre 1)
// Precondition:
// Parametre: DoubleVect& x
//    Signification: vecteur des valeurs
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Parametre: const double& val_param
//    Signification: le point en lequel calculer la valeur
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleVect& x
//    Signification: x modifie
//    Contraintes:
// Exception:
//    Sort en erreur s'il manque des parametres
// Effets de bord:
// Postcondition:
DoubleVect& Table::valeurs(DoubleVect& x, const double& val_param) const
{

  if (les_parametres.size() == 1)
    {
      int size;
      const DoubleVect& p = les_parametres[0];
      int size_p=p.size();
      if (p[0] >= val_param)
        {
          size=x.size();
          for(int j=0; j<size; j++)
            x[j] = les_valeurs(0,j);
        }
      else if (p[size_p-1] <= val_param)
        {
          size=x.size();
          for(int j=0; j<size; j++)
            x[j] = les_valeurs(size_p-1,j);
        }
      else
        {
          for (int i=1; i<size_p; i++)
            if (p[i] == val_param)
              {
                size=x.size();
                for(int j=0; j<size; j++)
                  x[j] = les_valeurs(i,j);
                break;
              }
            else if (p[i] > val_param)
              {
                size=x.size();
                for(int j=0; j<size; j++)
                  x[j] = les_valeurs(i-1,j)+
                         ((les_valeurs(i,j)-les_valeurs(i-1,j))/(p[i]-p[i-1]))
                         *(val_param-p[i-1]) ;
                break;
              }
        }
    }
  else
    {
      Cerr << "Error in a Table : it misses some parameters." << finl;
      exit();
    }
  return x;
}

// Description:
//    Pas encore code. Sort en erreur.
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
DoubleVect& Table::valeurs(DoubleVect& x, const DoubleVect& val_param) const
{
  Cerr << "Table::val(const DoubleVect& ) is not coded yet." << finl;
  exit();
  return x;
}

//Evaluation d un tableau de valeurs (val) a partir d une expression analytique fonction des valeurs (val_param)
//d un champ parametre, de l espace (pos) et du temps (t)
DoubleTab& Table::valeurs(const DoubleTab& val_param,const DoubleTab& pos,const double& tps,DoubleTab& aval) const
{
  eval_fct(pos,tps,val_param,aval);
  return aval;

}

// Description:
//    Affecte les parametres et les valeurs de la table
// Precondition:
// Parametre: const DoubleVect& param
//    Signification: les parametres
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: const DoubleVect& val
//    Signification: les valeurs
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Table::remplir(const DoubleVect& param,const DoubleVect& aval)
{
  //DoubleVect& les_valeurs2=ref_cast(DoubleVect,les_valeurs);
  //les_valeurs2.ref(val);
  les_valeurs=aval;
  les_parametres.dimensionner(1);
  les_parametres[0].ref(param);
}


// Description:
//    Affecte les parametres et les valeurs de la table
// Precondition:
// Parametre: const DoubleVect& param
//    Signification: les parametres
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: const DoubleTab& val
//    Signification: les valeurs
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Table::remplir(const DoubleVect& param,const DoubleTab& aval)
{
  les_valeurs.ref(aval);
  les_parametres.dimensionner(1);
  les_parametres[0].ref(param);
}
