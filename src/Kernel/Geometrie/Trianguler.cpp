/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Trianguler.h>
#include <Domaine.h>

Implemente_instanciable(Trianguler,"Trianguler",Triangulation_base);
// XD triangulate interprete trianguler -1 To achieve a triangular mesh from a mesh comprising rectangles (2 triangles per rectangle). Should be used in VEF discretization. Principle: NL2 \includepng{{trianguler.pdf}}{{10}}
// XD attr domain_name ref_domaine domain_name 0 Name of domain.

/*! @brief Simple appel a: Interprete::printOn(Sortie&)
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Trianguler::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}


/*! @brief Simple appel a: Interprete::readOn(Entree&)
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Trianguler::readOn(Entree& is)
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
void Trianguler::trianguler(Domaine& domaine) const
{
  if ((domaine.type_elem()->que_suis_je() == "Rectangle") || (domaine.type_elem()->que_suis_je() == "Quadrangle"))
    {
      domaine.typer("Triangle");
      IntTab& les_elems=domaine.les_elems();
      const DoubleTab& som =domaine.les_sommets();
      int oldsz=les_elems.dimension(0);
      IntTab new_elems(2*oldsz, 3);
      for(int i=0; i< oldsz; i++)
        {
          int i0=les_elems(i,0);
          int i1=les_elems(i,1);
          int i2=les_elems(i,2);
          int i3=les_elems(i,3);

          // NEW on coupe par la petite diagonale si elles n'ont pas meme longueur
          double d1=0,d2=0;
          for (int dir=0; dir<dimension; dir++)
            {
              d1+=(som(i1,dir)-som(i2,dir))*(som(i1,dir)-som(i2,dir));
              d2+=(som(i3,dir)-som(i0,dir))*(som(i3,dir)-som(i0,dir));
            }
          int test;
          if (est_egal(d1,d2))
            test=(i%2)==0;
          else
            test=(d1<d2);
          if (test)
            {
              new_elems(i,0)=i0;
              new_elems(i,1)=i1;
              new_elems(i,2)=i2;
              new_elems(i+oldsz,0)=i1;
              new_elems(i+oldsz,1)=i2;
              new_elems(i+oldsz,2)=i3;
            }
          else
            {
              new_elems(i,0)=i0;
              new_elems(i,1)=i1;
              new_elems(i,2)=i3;
              new_elems(i+oldsz,0)=i0;
              new_elems(i+oldsz,1)=i2;
              new_elems(i+oldsz,2)=i3;
            }
          mettre_a_jour_sous_domaine(domaine,i,i+oldsz,1);
        }
      les_elems.ref(new_elems);
    }
  else
    {
      Cerr << "We do not yet know how to Trianguler the "
           << domaine.type_elem()->que_suis_je() <<"s"<<finl;
    }
}

