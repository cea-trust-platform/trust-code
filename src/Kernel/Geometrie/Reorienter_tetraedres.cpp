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
// File:        Reorienter_tetraedres.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#include <Reorienter_tetraedres.h>
#include <Scatter.h>

Implemente_instanciable(Reorienter_tetraedres,"Reorienter_tetraedres",Interprete_geometrique_base);

Sortie& Reorienter_tetraedres::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

Entree& Reorienter_tetraedres::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

Entree& Reorienter_tetraedres::interpreter_(Entree& is)
{
  Cerr << "Reorientation of tetrahedra for either direct..." << finl;
  if (dimension != 3)
    {
      Cerr << "we can not reorientate (Reorienter) in dimension " << dimension <<finl;
      exit();
    }
  associer_domaine(is);
  Domaine& dom=domaine();
  Scatter::uninit_sequential_domain(dom);
  Reorienter(dom);
  Scatter::init_sequential_domain(dom);
  Cerr << "Reorientation of tetrahedra... OK" << finl;
  return is;
}


Reorienter_tetraedres::Sens Reorienter_tetraedres::test_orientation_tetra(IntTab& les_elems, int num_element, const DoubleTab& coord_sommets) const
{
  static const int SOM_Z = 0;
  static const int SOM_A = 1;
  static const int SOM_B = 2;
  static const int SOM_C = 3;

  const int som_Z = les_elems(num_element,SOM_Z);
  const int som_A = les_elems(num_element,SOM_A);
  const int som_B = les_elems(num_element,SOM_B);
  const int som_C = les_elems(num_element,SOM_C);

  double ZA[3], ZB[3], ZC[3], pdtvect[3], pdtscal;
  int k;
  for (k=0 ; k<dimension ; k++)
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
  for (k=0 ; k<dimension ; k++)
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
      return INDIRECT;
    }
#ifdef _AFFDEBUG
  {
    Process::Journal<<"  element "<<num_element<<"  direct"<<finl;
  }
#endif

  return DIRECT;
}

Reorienter_tetraedres::Sens Reorienter_tetraedres::reorienter_tetra(IntTab& les_elems, int num_element) const
{
  static const int SOM_A = 1;
  static const int SOM_B = 2;

  //pour reorienter le tetraedre, on va permuter les sommets 1 et 2
  int tmp;
  tmp = les_elems(num_element,SOM_A);
  les_elems(num_element,SOM_A) = les_elems(num_element,SOM_B);
  les_elems(num_element,SOM_B) = tmp;
  return DIRECT;
}

//
//
//
void Reorienter_tetraedres::Reorienter(Domaine& dom) const
{
  const DoubleTab& coord_sommets = dom.coord_sommets();
  int nb_zones = dom.nb_zones();
  int iz;

  //on balaye les zones du domaine
  for(iz=0 ; iz<nb_zones ; iz++)
    {
      Zone& zone = dom.zone(iz);
      if (zone.type_elem()->que_suis_je() == "Tetraedre" )
        {
          //zone de tetraedres
          IntTab& les_elems = zone.les_elems();
          int nb_elems = les_elems.dimension(0);
          int ielem;

          //balaye les tetraedres
          for (ielem=0 ; ielem<nb_elems ; ielem++)
            {
              if (test_orientation_tetra(les_elems, ielem, coord_sommets)==INDIRECT)
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
    }//fin balayage zones
}
