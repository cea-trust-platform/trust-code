/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Table.h>
#include <utility>

Implemente_instanciable_sans_constructeur_ni_destructeur(Table,"Table",Objet_U);


/*! @brief Constructeur par defaut.
 *
 * Table vide.
 *
 */
Table::Table() : les_valeurs() , les_parametres() { }

Table::Table(const Table& t): Objet_U(t),Parser_Eval()
{
  (*this)=t;
}

//Lecture d'une expression analytique dependant des valeurs (val) d'un champ parametre
//isf est fixe a 1
Entree& Table::lire_f(Entree& is, const int nb_comp)
{
  isf=1;
  Nom tmp;

  VECT(Parser_U)& fval=fonction();
  fval.dimensionner(nb_comp);
  for (int i = 0; i < nb_comp; i++)
    {
      is >> tmp;
      Cerr << "Reading and interpretation of the function " << tmp << finl;
      fval[i].setNbVar(1);
      fval[i].setString(tmp);
      fval[i].addVar("val");
      fval[i].parseString();
    }
  Cerr << "Interpretation of the function " << tmp << " OK" << finl;

  return is;
}

//Lecture d'une expression analytique dependant des valeurs (val) d'un champ parametre,
//de l espace (x,y,z) et du temps (t)
//isf est fixe a 2
Entree& Table::lire_fxyzt(Entree& is,const int dim)
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

/*! @brief Ecriture sur un flot de Sortie N'ecrit que le type et le nom de l'objet
 *
 * @param (Sortie& s) le flot de sortie a utiliser
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Table::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}



/*! @brief Lecture sur un flot d'entree Lecture des parametres et des valeurs de la table.
 *
 * @param (Entree& s) le flot d'entree a utiliser
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Table::readOn(Entree& s )
{
  s >> les_parametres;
  s >> les_valeurs;
  return s ;
}

// Computes the n-linear interpolant of data at point x inside a hyper-rectangle defined by gridpoints.
// The shapes should be (n) for x, (n, 2) for gridpoints and (2, 2, 2, ..., 2) n times for data (stored as a single vector here)
double nlinear_interpolation(const std::vector<double>& x, const std::vector<std::pair<double, double>>& gridpoints, const std::vector<double>& data)
{
  const int n = (int)x.size();

  // on construit les poids pour chaque sommet de l'hyper-rectangle
  std::vector<double> weight(n);
  for (int i = 0; i < n; i++)
    weight[i] = std::min(1.0, std::max(0.0, (x[i] - gridpoints[i].first) / (gridpoints[i].second - gridpoints[i].first)));

  // le calcul est effecte en utilisant une representation en bit j (int) -> index (n valeurs 0 ou 1)
  // j=0 -> index=00...000 ; j=1 -> index=00...001 ; j=2 -> index=00...010
  double interpolant = 0.0, prod;
  for (int j = 0, i; j < (1<<n); interpolant += prod, j++)
    for (prod = data[j], i = 0; i < n; i++) prod *= (j >> (n - 1 - i)) & 1 ? weight[i] : 1.0 - weight[i];

  return interpolant;
}

// n'utilise pas nlinear_interpolation : codage pour nb_comp = 1 et nb_param = 1
double Table::val_simple(double vp) const
{
  assert(les_parametres.size() == 1);
  const DoubleVect& p = les_parametres[0];
  const int size = p.size();
  if (p[0] >= vp) return les_valeurs(0);
  else
    {
      for (int i = 1; i < size; i++)
        if (p[i] == vp)     return les_valeurs(i);
        else if (p[i] > vp) return (les_valeurs(i - 1) + ((les_valeurs(i) - les_valeurs(i - 1)) / (p[i] - p[i - 1])) * (vp - p[i - 1]));
    }
  return les_valeurs(size - 1);
}

double Table::val(const double val_param, int ncomp) const
{
  std::vector<double> vals_param {val_param};
  return val(vals_param, ncomp);
}

double Table::val(const std::vector<double>& vals_param, int ncomp) const
{
  const int nb_param = les_parametres.size();
  int nb_comp = les_valeurs.size();
  if (nb_param == (int)vals_param.size())
    {
      std::vector<double> data;
      std::vector<int> icube;
      std::vector<std::pair<double, double>> gridpoints;

      for (int ip = 0; ip < nb_param; ip++)
        {
          const DoubleVect& p = les_parametres[ip];
          nb_comp /= p.size();
          int i_interval = p.size() - 1;
          for (int i = 1; i < p.size(); i++)
            if (vals_param[ip] < p[i])
              {
                i_interval = i;
                break;
              }
          icube.push_back(i_interval - 1);
          gridpoints.push_back({p[i_interval - 1], p[i_interval]});
        }
      for (int j = 0; j < (1 << nb_param); j++)
        {
          std::vector<int> index(nb_param, 0);
          for (int k = 0; k < nb_param; k++) index[nb_param - 1 - k] = icube[nb_param - 1 - k] + ((j >> k) & 1);

          // indice dans le tableau global des valeurs tabulees
          int k = index[0];
          for (int i = 1; i < nb_param; i++)
            k = k * les_parametres[i].size() + index[i];
          k = k * nb_comp + ncomp;

          data.push_back(les_valeurs[k]);
        }

      return nlinear_interpolation(vals_param, gridpoints, data);
    }
  else if (isf == 1 && vals_param.size() == 1)
    {
      // Plus rapide que parser(0).setVar("val",val_param);
      parser(ncomp).setVar(0,vals_param[0]);
      return parser(ncomp).eval();
    }
  else Process::exit("Error in a Table::val : wrong number of parameters.");

  return 0;
}

/*! @brief Pas encore code.
 *
 * Sort en erreur.
 *
 */
double Table::val(const DoubleVect& val_param) const
{
  Cerr << "Table::val(const DoubleVect& ) is not coded yet." << finl;
  exit();
  return 0;
}


/*! @brief Retourne les valeurs calculees pour le point val_param (cas du tableau valeurs a 2 dimensions) Les valeurs sont exacteS si le point correspond a un parametre donne.
 *
 *     Sinon, les valeurs sont interpolees (interp. lineaire d'ordre 1)
 *
 * @param (DoubleVect& x) vecteur des valeurs
 * @param (const double& val_param) le point en lequel calculer la valeur
 * @return (DoubleVect& x) x modifie
 * @throws Sort en erreur s'il manque des parametres
 */
DoubleVect& Table::valeurs(DoubleVect& x, const double val_param) const
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

/*! @brief Pas encore code.
 *
 * Sort en erreur.
 *
 */
DoubleVect& Table::valeurs(DoubleVect& x, const DoubleVect& val_param) const
{
  Cerr << "Table::val(const DoubleVect& ) is not coded yet." << finl;
  exit();
  return x;
}

//Evaluation d un tableau de valeurs (val) a partir d une expression analytique fonction des valeurs (val_param)
//d un champ parametre, de l espace (pos) et du temps (t)
DoubleTab& Table::valeurs(const DoubleTab& val_param,const DoubleTab& pos,const double tps,DoubleTab& aval) const
{
  eval_fct(pos,tps,val_param,aval);
  return aval;

}

/*! @brief Affecte les parametres et les valeurs de la table
 *
 * @param (const DoubleVect& param) les parametres
 * @param (const DoubleTab& val) les valeurs
 */
void Table::remplir(const DoubleVect& param,const DoubleTab& aval)
{
  // on verifie que param est strictement monotone
  double val_ = param[0];
  for (int i = 1; i < param.size(); i++)
    if (val_ < param[i]) val_ = param[i];
    else Process::exit("A table is not strictly monotonic!");

  les_valeurs.ref(aval);
  les_parametres.dimensionner(1);
  les_parametres[0].ref(param);
}

void Table::remplir(const DoubleVects& params, const DoubleVect& aval)
{
  // on verifie que les params sont strictement monotones
  for (int n = 0; n < params.size(); n++)
    {
      double val_ = params[n][0];
      for (int i = 1; i < params[n].size(); i++)
        if (val_ < params[n][i]) val_ = params[n][i];
        else Process::exit("A table is not strictly monotonic!");
    }

  les_valeurs = aval;
  les_parametres.dimensionner(params.size());
  for (int i = 0; i < params.size(); i++) les_parametres[i].ref(params[i]);
}
