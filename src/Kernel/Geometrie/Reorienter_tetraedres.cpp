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

#include <Reorienter_tetraedres.h>
#include <Scatter.h>

Implemente_instanciable_32_64(Reorienter_tetraedres_32_64,"Reorienter_tetraedres",Interprete_geometrique_base_32_64<_T_>);
// XD reorienter_tetraedres interprete reorienter_tetraedres -1 This keyword is mandatory for front-tracking computations with the VEF discretization. For each tetrahedral element of the domain, it checks if it has a positive volume. If the volume (determinant of the three vectors) is negative, it swaps two nodes to reverse the orientation of this tetrahedron.
// XD attr domain_name ref_domaine domain_name 0 Name of domain.

template <typename _SIZE_>
Sortie& Reorienter_tetraedres_32_64<_SIZE_>::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

template <typename _SIZE_>
Entree& Reorienter_tetraedres_32_64<_SIZE_>::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

template <typename _SIZE_>
Entree& Reorienter_tetraedres_32_64<_SIZE_>::interpreter_(Entree& is)
{
  Cerr << "Reorientation of tetrahedra for either direct..." << finl;
  if (Objet_U::dimension != 3)
    {
      Cerr << "we can not reorientate (Reorienter) in dimension " << Objet_U::dimension <<finl;
      Process::exit();
    }
  this->associer_domaine(is);
  Domaine_t& dom=this->domaine();
  Scatter::uninit_sequential_domain(dom);
  reorienter(dom);
  Scatter::init_sequential_domain(dom);
  Cerr << "Reorientation of tetrahedra... OK" << finl;
  return is;
}

template <typename _SIZE_>
Sens_Orient Reorienter_tetraedres_32_64<_SIZE_>::test_orientation_tetra(IntTab_t& les_elems, int_t ielem, const DoubleTab_t& coord_sommets) const
{
  static const int SOM_Z = 0;
  static const int SOM_A = 1;
  static const int SOM_B = 2;
  static const int SOM_C = 3;

  const int_t som_Z = les_elems(ielem,SOM_Z);
  const int_t som_A = les_elems(ielem,SOM_A);
  const int_t som_B = les_elems(ielem,SOM_B);
  const int_t som_C = les_elems(ielem,SOM_C);

  double ZA[3], ZB[3], ZC[3], pdtvect[3], pdtscal;
  int k;
  for (k=0 ; k < Objet_U::dimension ; k++)
    {
      ZA[k] = coord_sommets(som_A,k) - coord_sommets(som_Z,k);
      ZB[k] = coord_sommets(som_B,k) - coord_sommets(som_Z,k);
      ZC[k] = coord_sommets(som_C,k) - coord_sommets(som_Z,k);
    }

  //calcul pdt vect ZAxZB
  pdtvect[0] = ZA[1]*ZB[2] - ZA[2]*ZB[1];
  pdtvect[1] = ZA[2]*ZB[0] - ZA[0]*ZB[2];
  pdtvect[2] = ZA[0]*ZB[1] - ZA[1]*ZB[0];

  //calcul du pdtscal pdtvect.ZC
  pdtscal = 0.;
  for (k=0 ; k < Objet_U::dimension ; k++)
    {
      pdtscal += pdtvect[k] * ZC[k];
    }

  if (pdtscal<0.)
    {
      //le pdt scalaire est negatif : il s'agit d'un tetraedre mal oriente
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
Sens_Orient Reorienter_tetraedres_32_64<_SIZE_>::reorienter_tetra(IntTab_t& les_elems, int_t num_element) const
{
  static const int SOM_A = 1;
  static const int SOM_B = 2;

  //pour reorienter le tetraedre, on va permuter les sommets 1 et 2
  int_t tmp;
  tmp = les_elems(num_element,SOM_A);
  les_elems(num_element,SOM_A) = les_elems(num_element,SOM_B);
  les_elems(num_element,SOM_B) = tmp;
  return Sens_Orient::DIRECT;
}

template <typename _SIZE_>
void Reorienter_tetraedres_32_64<_SIZE_>::reorienter(Domaine_t& dom) const
{
  const DoubleTab_t& coord_sommets = dom.coord_sommets();

  if (dom.type_elem()->que_suis_je() == "Tetraedre" )
    {
      //domaine de tetraedres
      IntTab_t& les_elems = dom.les_elems();
      int_t nb_elems = les_elems.dimension(0);

      //balaye les tetraedres
      for (int_t ielem=0 ; ielem<nb_elems ; ielem++)
        {
          if (test_orientation_tetra(les_elems, ielem, coord_sommets) == Sens_Orient::INDIRECT)
            {
              //tetraedre oriente en sens indirect -> a reorienter
              reorienter_tetra(les_elems, ielem);

#ifdef _AFFDEBUG
              {
                Process::Journal<<"  #element reoriente "<<ielem<<finl;
                static const int SOM_Z = 0;  // indice du sommet qui servira d'origine
                static const int SOM_A = 1;     // indice du sommet qui servira pour le premier vecteur
                static const int SOM_B = 2;     // indice du sommet qui servira pour le second vecteur
                static const int SOM_C = 3;     // indice du sommet qui servira pour le troisieme vecteur
                const int som_Z = les_elems(ielem,SOM_Z);
                const int som_A = les_elems(ielem,SOM_A);
                const int som_B = les_elems(ielem,SOM_B);
                const int som_C = les_elems(ielem,SOM_C);
                Process::Journal<<"    somA= "<<som_A<<"  coords= "<<coord_sommets(som_A, 0)<<" "<<coord_sommets(som_A, 1)<<" "<<coord_sommets(som_A, 2)<<finl;
                Process::Journal<<"    somB= "<<som_B<<"  coords= "<<coord_sommets(som_B, 0)<<" "<<coord_sommets(som_B, 1)<<" "<<coord_sommets(som_B, 2)<<finl;
                Process::Journal<<"    somC= "<<som_C<<"  coords= "<<coord_sommets(som_C, 0)<<" "<<coord_sommets(som_C, 1)<<" "<<coord_sommets(som_C, 2)<<finl;
                Process::Journal<<"    somA= "<<som_A<<"  coords= "<<coord_sommets(som_A, 0)<<" "<<coord_sommets(som_A, 1)<<" "<<coord_sommets(som_A, 2)<<finl<<finl;
                Process::Journal<<"    somZ= "<<som_Z<<"  coords= "<<coord_sommets(som_Z, 0)<<" "<<coord_sommets(som_Z, 1)<<" "<<coord_sommets(som_Z, 2)<<finl;
                Process::Journal<<"    somA= "<<som_A<<"  coords= "<<coord_sommets(som_A, 0)<<" "<<coord_sommets(som_A, 1)<<" "<<coord_sommets(som_A, 2)<<finl<<finl;
                Process::Journal<<"    somZ= "<<som_Z<<"  coords= "<<coord_sommets(som_Z, 0)<<" "<<coord_sommets(som_Z, 1)<<" "<<coord_sommets(som_Z, 2)<<finl;
                Process::Journal<<"    somB= "<<som_B<<"  coords= "<<coord_sommets(som_B, 0)<<" "<<coord_sommets(som_B, 1)<<" "<<coord_sommets(som_B, 2)<<finl<<finl;
                Process::Journal<<"    somZ= "<<som_Z<<"  coords= "<<coord_sommets(som_Z, 0)<<" "<<coord_sommets(som_Z, 1)<<" "<<coord_sommets(som_Z, 2)<<finl;
                Process::Journal<<"    somC= "<<som_C<<"  coords= "<<coord_sommets(som_C, 0)<<" "<<coord_sommets(som_C, 1)<<" "<<coord_sommets(som_C, 2)<<finl<<finl;
              }
#endif
            }
        }
    }
}


template class Reorienter_tetraedres_32_64<int>;
#if INT_is_64_ == 2
template class Reorienter_tetraedres_32_64<trustIdType>;
#endif


