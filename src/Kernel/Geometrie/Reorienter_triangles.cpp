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

#include <Reorienter_triangles.h>
#include <Scatter.h>

Implemente_instanciable_32_64(Reorienter_triangles_32_64,"Reorienter_triangles",Interprete_geometrique_base_32_64<_T_>);
// XD reorienter_triangles interprete reorienter_triangles -1 not_set
// XD attr domain_name ref_domaine domain_name 0 Name of domain.

template <typename _SIZE_>
Sortie& Reorienter_triangles_32_64<_SIZE_>::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

template <typename _SIZE_>
Entree& Reorienter_triangles_32_64<_SIZE_>::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

template <typename _SIZE_>
Entree& Reorienter_triangles_32_64<_SIZE_>::interpreter_(Entree& is)
{
  Cerr << "Reorientation of triangles for either direct..." << finl;
  if (Objet_U::dimension != 2)
    {
      Cerr << "we can not reorientate (Reorienter) triangles in dimension " << Objet_U::dimension <<finl;
      Process::exit();
    }
  this->associer_domaine(is);
  Domaine_t& dom=this->domaine();
  Scatter::uninit_sequential_domain(dom);
  this->reorienter(dom);
  Scatter::init_sequential_domain(dom);
  Cerr << "Reorientation of triangles... OK" << finl;
  return is;
}

template <typename _SIZE_>
Sens_Orient Reorienter_triangles_32_64<_SIZE_>::test_orientation_triangle(IntTab_t& les_elems, int_t ielem, const DoubleTab_t& coord_sommets) const
{
  static const int SOM_Z = 0;
  static const int SOM_A = 1;
  static const int SOM_B = 2;

  const int_t som_Z = les_elems(ielem,SOM_Z);
  const int_t som_A = les_elems(ielem,SOM_A);
  const int_t som_B = les_elems(ielem,SOM_B);

  double ZA0 = coord_sommets(som_A,0) - coord_sommets(som_Z,0);
  double ZB0 = coord_sommets(som_B,0) - coord_sommets(som_Z,0);
  double ZA1 = coord_sommets(som_A,1) - coord_sommets(som_Z,1);
  double ZB1 = coord_sommets(som_B,1) - coord_sommets(som_Z,1);

  //calcul pdt vect ZAxZB
  double pdtvect = ZA0*ZB1 - ZA1*ZB0;

  if (pdtvect<0.)
    {
      //le pdt scalaire est negatif : il s'agit d'un triangle mal oriente
#ifdef _AFFDEBUG
      {
        Process::Journal<<"  element "<<num_element<<"  indirect"<<finl;
      }
#endif
      return Sens_Orient::INDIRECT;
    }
#ifdef _AFFDEBUG
  {
    Process::Journal<<"  element "<<num_element<<"  direct"<<finl;
  }
#endif

  return Sens_Orient::DIRECT;
}

template <typename _SIZE_>
Sens_Orient Reorienter_triangles_32_64<_SIZE_>::reorienter_triangle(IntTab_t& les_elems, int_t num_element) const
{
  static const int SOM_A = 1;
  static const int SOM_B = 2;

  //pour reorienter le triangle, on va permuter les sommets 1 et 2
  int_t tmp;
  tmp = les_elems(num_element,SOM_A);
  les_elems(num_element,SOM_A) = les_elems(num_element,SOM_B);
  les_elems(num_element,SOM_B) = tmp;
  return Sens_Orient::DIRECT;
}

/*!
 * Cette methode permet de reorienter les triangles dans le sens direct
 */
template <typename _SIZE_>
void Reorienter_triangles_32_64<_SIZE_>::reorienter(Domaine_t& dom) const
{
  const DoubleTab_t& coord_sommets = dom.coord_sommets();

  if (dom.type_elem()->que_suis_je() == "Triangle" )
    {
      //domaine de triangles
      IntTab_t& les_elems = dom.les_elems();
      int_t nb_elems = les_elems.dimension(0);

      //balaye les triangles
      for (int_t ielem=0 ; ielem<nb_elems ; ielem++)
        {
          if (test_orientation_triangle(les_elems, ielem, coord_sommets)==Sens_Orient::INDIRECT)
            {
              //triangle oriente en sens indirect -> a reorienter
              reorienter_triangle(les_elems, ielem);

#ifdef _AFFDEBUG
              {
                Process::Journal<<"  #element reoriente "<<ielem<<finl;
                static const int SOM_Z = 0;
                static const int SOM_A = 1;
                static const int SOM_B = 2;
                const int som_Z = les_elems(ielem,SOM_Z);
                const int som_A = les_elems(ielem,SOM_A);
                const int som_B = les_elems(ielem,SOM_B);
                Process::Journal<<"    somZ= "<<som_Z<<"  coords= "<<coord_sommets(som_Z, 0)<<" "<<coord_sommets(som_Z, 1)<<finl;
                Process::Journal<<"    somA= "<<som_A<<"  coords= "<<coord_sommets(som_A, 0)<<" "<<coord_sommets(som_A, 1)<<finl;
                Process::Journal<<"    somB= "<<som_B<<"  coords= "<<coord_sommets(som_B, 0)<<" "<<coord_sommets(som_B, 1)<<finl;
              }
#endif
            }
        }
    }
}


template class Reorienter_triangles_32_64<int>;
#if INT_is_64_ == 2
template class Reorienter_triangles_32_64<trustIdType>;
#endif


