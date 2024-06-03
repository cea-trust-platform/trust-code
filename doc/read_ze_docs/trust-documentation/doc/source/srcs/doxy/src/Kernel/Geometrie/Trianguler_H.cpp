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

#include <Trianguler_H.h>
#include <Domaine.h>

Implemente_instanciable(Trianguler_H,"Trianguler_H",Triangulation_base);


/*! @brief Simple appel a: Interprete::printOn(Sortie&)
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Trianguler_H::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}


/*! @brief Simple appel a: Interprete::readOn(Entree&)
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Trianguler_H::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

/*! @brief Triangule tous les element d'un domaine: transforme les elements goemetriques du domaine en triangles.
 *
 *     Pour l'instant on ne sait trianguler que des Rectangles
 *     (on les coupe en 2).
 *
 * @param (Domaine& domaine) le domaine dont on veut trianguler les elements
 */
void Trianguler_H::trianguler(Domaine& domaine) const
{
  Domaine& dom=domaine;
  IntTab& les_elems=domaine.les_elems();
  int oldsz=les_elems.dimension(0);
  DoubleTab& sommets=dom.les_sommets();
  int cpt=sommets.dimension(0);
  {
    DoubleTab sommets_ajoutes(oldsz, dimension);
    domaine.type_elem()->calculer_centres_gravite(sommets_ajoutes);
    sommets.resize(cpt+oldsz, dimension);
    for(int i=0; i<oldsz; i++)
      for(int j=0; j<dimension; j++)
        sommets(cpt+i,j)=sommets_ajoutes(i,j);
  }
  if ((domaine.type_elem()->que_suis_je() == "Rectangle")|| (domaine.type_elem()->que_suis_je() == "Quadrangle"))
    {
      domaine.typer("Triangle");
      IntTab new_elems(4*oldsz, 3);
      for(int i=0; i< oldsz; i++)
        {
          int i0=les_elems(i,0);
          int i1=les_elems(i,1);
          int i2=les_elems(i,2);
          int i3=les_elems(i,3);

          new_elems(i,0)=                  i+cpt;
          new_elems(i,1)=                  i0;
          new_elems(i,2)=                  i1;

          new_elems(i+oldsz,0)=                  i+cpt;
          new_elems(i+oldsz,1)=                  i0;
          new_elems(i+oldsz,2)=                  i2;
          mettre_a_jour_sous_domaine(domaine,i,i+oldsz,1);

          new_elems(i+2*oldsz,0)=          i+cpt;
          new_elems(i+2*oldsz,1)=          i1;
          new_elems(i+2*oldsz,2)=          i3;
          mettre_a_jour_sous_domaine(domaine,i,i+2*oldsz,1);

          new_elems(i+3*oldsz,0)=          i+cpt;
          new_elems(i+3*oldsz,1)=          i2;
          new_elems(i+3*oldsz,2)=          i3;
          mettre_a_jour_sous_domaine(domaine,i,i+3*oldsz,1);
        }
      les_elems.ref(new_elems);
    }
  else
    {
      Cerr << "We do not yet know how to Trianguler_h the "
           << domaine.type_elem()->que_suis_je() <<"s"<<finl;
    }
}

