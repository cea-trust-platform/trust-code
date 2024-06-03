/****************************************************************************
* Copyright (c) 2022, CEA
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

#include <Champ_front_softanalytique.h>
#include <Domaine.h>
#include <Domaine_VF.h>

Implemente_instanciable(Champ_front_softanalytique,"Champ_front_fonc_xyz",Ch_front_var_stationnaire);
// XD champ_front_fonc_xyz front_field_base champ_front_fonc_xyz 0 Boundary field which is not constant in space.
// XD attr val listchaine val 0 Values of field components (mathematical expressions).

/*! @brief Imprime le champ sur flot de sortie.
 *
 * Imprime la taille du champ et la valeur (constante) sur
 *     la frontiere.
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Champ_front_softanalytique::printOn(Sortie& os) const
{

  /*  int dim =   nb_comp();
      for (int i = 0;i<dim;i++)
      {
      os << fxyz[i]->getString();
      }
      return os;*/
  //  Cout << " Dans printOn !!!!!!!!!!!!!!!!!!!!!!" << finl;
  const DoubleTab& tab=valeurs();
  os << tab.size() << " ";
  for(int i=0; i<tab.size(); i++)
    os << tab(0,i);
  return os;
}


/*! @brief Lit le champ a partir d'un flot d'entree.
 *
 * Format:
 *       Champ_front_softanalytique nb_compo vrel_1 ... [vrel_i]
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree& is) le flot d'entree modifie
 */
Entree& Champ_front_softanalytique::readOn(Entree& is)
{
  //Cout << "Dans  Champ_front_softanalytique.readOn !!!!!!!!!" << finl;
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

      fxyz[i].setNbVar(3);
      fxyz[i].setString(tmp);
      fxyz[i].addVar("x");
      fxyz[i].addVar("y");
      fxyz[i].addVar("z");
      fxyz[i].parseString();
      Cerr << "Interpretation of the function " << tmp << " Ok" << finl;
      //        Cout << "end = " << tmp << finl;
    }

  return is;
}

int Champ_front_softanalytique::initialiser(double temps, const Champ_Inc_base& inco)
{

  if (!Ch_front_var_stationnaire::initialiser(temps,inco))
    return 0;

  int dim=nb_comp();
  double x,y,z;
  //      int nbsf=faces.nb_som_faces();
  int k;
  DoubleTab& tab=valeurs();

  const Domaine_VF& domaine_vf=ref_cast(Domaine_VF,frontiere_dis().domaine_dis());
  const DoubleTab& xv=domaine_vf.xv();

  const Front_VF& le_bord = ref_cast(Front_VF,frontiere_dis());
  int nb_faces_bord_tot=le_bord.nb_faces_tot();
  for (int ind_face=0; ind_face<nb_faces_bord_tot; ind_face++)
    {
      int face = le_bord.num_face(ind_face);
      x=y=z=0;
      x=xv(face,0);
      if(dimension>1)
        y=xv(face,1);
      if(dimension>2)
        z=xv(face,2);

      for( k=0; k<dim; k++)
        {
          fxyz[k].setVar("x",x);
          fxyz[k].setVar("y",y);
          fxyz[k].setVar("z",z);
          tab(ind_face,k)=fxyz[k].eval();
          //Cout << " x y z " << x << " " << y << " " << z << " " << tab(i,k) << finl;
        }
    }

  tab.echange_espace_virtuel();
  return 1;
}

void Champ_front_softanalytique::valeur_a(DoubleVect& position, DoubleVect& valeur)
{
  assert(position.size() == Objet_U::dimension);
  double x = position[0];
  double y = position[1];
  double z = 0.;
  if (Objet_U::dimension == 3) z = position[2];

  int dim = nb_comp();
  valeur.resize(dim);

  for (int k=0; k<dim; k++)
    {
      fxyz[k].setVar("x",x);
      fxyz[k].setVar("y",y);
      fxyz[k].setVar("z",z);
      valeur[k] = fxyz[k].eval();
    }
  return;
}
