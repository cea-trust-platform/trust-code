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


#include <Champ_Q1_EF.h>
#include <Zone_EF.h>
#include <Domaine.h>
#include <Equation.h>
#include <Debog.h>


Implemente_instanciable(Champ_Q1_EF,"Champ_Q1_EF",Champ_Inc_Q1_base);

// printOn

Sortie& Champ_Q1_EF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}


// readOn

Entree& Champ_Q1_EF::readOn(Entree& s)
{
  lire_donnees(s) ;
  return s ;
}

// Description:
//
// Precondition:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const Zone_dis_base& Champ_Q1_EF::zone_dis_base() const
{
  return la_zone_VF.valeur();
}
// Description:
//
// Precondition:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour:  z_dis
//    Signification: la zone discretise
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Champ_Q1_EF::associer_zone_dis_base(const Zone_dis_base& z_dis)
{
  la_zone_VF=ref_cast(Zone_VF, z_dis);
}

// Description:
//
// Precondition:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: la_zone_EF_Q1.valeur()
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const Zone_EF& Champ_Q1_EF::zone_EF() const
{
  return ref_cast(Zone_EF, la_zone_VF.valeur());
}

int Champ_Q1_EF::imprime(Sortie& os, int ncomp) const
{
  const Zone_dis_base& zone_dis = zone_dis_base();
  const Zone& zone = zone_dis.zone();
  const DoubleTab& coord=zone.domaine().coord_sommets();
  const int nb_som = zone.domaine().nb_som();
  const DoubleTab& val = valeurs();
  int som;
  os << nb_som << finl;
  for (som=0; som<nb_som; som++)
    {
      if (dimension==3)
        os << coord(som,0) << " " << coord(som,1) << " " << coord(som,2) << " " ;
      if (dimension==2)
        os << coord(som,0) << " " << coord(som,1) << " " ;
      if (nb_compo_ == 1)
        os << val(som) << finl;
      else
        os << val(som,ncomp) << finl;
    }
  os << finl;
  Cout << "Champ_Q1_EF::imprime FIN >>>>>>>>>> " << finl;
  return 1;
}

void Champ_Q1_EF::gradient(DoubleTab& gradient_elem)
{
  // Calcul du gradient de la vitesse pour le calcul de la vorticite
  // Gradient ordre 1 (valeur moyenne dans un element)
  // Order 1 gradient (mean value within an element)
  const Zone_EF& zone_EF_ = zone_EF();
  const DoubleTab& vitesse = equation().inconnue().valeurs();
  const IntTab& elems= zone_EF_.zone().les_elems();
  int nb_som_elem=zone_EF_.zone().nb_som_elem();
  int nb_elems=zone_EF_.zone().nb_elem_tot();
  const DoubleVect& volume_thilde=zone_EF_.volumes_thilde();
  const DoubleTab& Bij_thilde=zone_EF_.Bij_thilde();

  assert(gradient_elem.dimension_tot(0) == nb_elems);
  assert(gradient_elem.dimension(1) == dimension); // line
  assert(gradient_elem.dimension(2) == dimension); // column

  operator_egal(gradient_elem, 0.); // Espace reel uniquement

  for (int num_elem=0; num_elem<nb_elems; num_elem++)
    {
      for (int a=0; a<dimension; a++) // composante numero 1, component number 1
        {
          for (int b=0; b<dimension; b++) // composante numero 2, component number 2
            {
              for (int j=0; j<nb_som_elem; j++)
                {
                  int s = elems(num_elem,j);
                  gradient_elem(num_elem,a,b) += vitesse(s,a)*Bij_thilde(num_elem,j,b);
                }
              gradient_elem(num_elem,a,b) /= volume_thilde(num_elem);
            }
        }
    }
}

void Champ_Q1_EF::cal_rot_ordre1(DoubleTab& vorticite)
{
  const Zone_EF& zone_EF_ = zone_EF();
  int nb_elems=zone_EF_.zone().nb_elem_tot();

  DoubleTab gradient_elem(0, dimension, dimension);
  // le tableau est initialise dans la methode gradient():
  zone_EF_.zone().creer_tableau_elements(gradient_elem, Array_base::NOCOPY_NOINIT);
  gradient(gradient_elem);
  Debog::verifier("apres calcul gradient",gradient_elem);
  int num_elem;
  switch(dimension)
    {
    case 2 :
      {
        for (num_elem=0; num_elem<nb_elems; num_elem++)
          {
            vorticite(num_elem)=gradient_elem(num_elem,1,0)-gradient_elem(num_elem,0,1);
          }
      }
      break;
    case 3 :
      {
        for (num_elem=0; num_elem<nb_elems; num_elem++)
          {
            vorticite(num_elem,0)=gradient_elem(num_elem,2,1)-gradient_elem(num_elem,1,2);
            vorticite(num_elem,1)=gradient_elem(num_elem,0,2)-gradient_elem(num_elem,2,0);
            vorticite(num_elem,2)=gradient_elem(num_elem,1,0)-gradient_elem(num_elem,0,1);
          }
      }
    }
  Debog::verifier("apres calcul vort",vorticite);
  return ;
}
