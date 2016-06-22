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
// File:        Reorienter_triangles.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#include <Reorienter_triangles.h>
#include <Scatter.h>

Implemente_instanciable(Reorienter_triangles,"Reorienter_triangles",Interprete_geometrique_base);

Sortie& Reorienter_triangles::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

Entree& Reorienter_triangles::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

Entree& Reorienter_triangles::interpreter_(Entree& is)
{
  Cerr << "Reorientation of triangles for either direct..." << finl;
  if (dimension != 2)
    {
      Cerr << "we can not reorientate (Reorienter) triangles in dimension " << dimension <<finl;
      exit();
    }
  Nom nom_dom, typ_zone;
  associer_domaine(is);
  Domaine& dom=domaine();
  Scatter::uninit_sequential_domain(dom);
  Reorienter(dom);
  Scatter::init_sequential_domain(dom);
  Cerr << "Reorientation of triangles... OK" << finl;
  return is;
}


Reorienter_triangles::Sens Reorienter_triangles::test_orientation_triangle(IntTab& les_elems, int num_element, const DoubleTab& coord_sommets) const
{
  static const int SOM_Z = 0;
  static const int SOM_A = 1;
  static const int SOM_B = 2;

  const int som_Z = les_elems(num_element,SOM_Z);
  const int som_A = les_elems(num_element,SOM_A);
  const int som_B = les_elems(num_element,SOM_B);

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
      return INDIRECT;
    }
#ifdef _AFFDEBUG
  {
    Process::Journal<<"  element "<<num_element<<"  direct"<<finl;
  }
#endif

  return DIRECT;
}

Reorienter_triangles::Sens Reorienter_triangles::reorienter_triangle(IntTab& les_elems, int num_element) const
{
  static const int SOM_A = 1;
  static const int SOM_B = 2;

  //pour reorienter le triangle, on va permuter les sommets 1 et 2
  int tmp;
  tmp = les_elems(num_element,SOM_A);
  les_elems(num_element,SOM_A) = les_elems(num_element,SOM_B);
  les_elems(num_element,SOM_B) = tmp;
  return DIRECT;
}

//Cette methode permet de reorienter les triangles dans le sens direct
void Reorienter_triangles::Reorienter(Domaine& dom) const
{
  const DoubleTab& coord_sommets = dom.coord_sommets();
  int nb_zones = dom.nb_zones();
  int iz;

  //on balaye les zones du domaine
  for(iz=0 ; iz<nb_zones ; iz++)
    {
      Zone& zone = dom.zone(iz);
      if (zone.type_elem()->que_suis_je() == "Triangle" )
        {
          //zone de triangles
          IntTab& les_elems = zone.les_elems();
          int nb_elems = les_elems.dimension(0);
          int ielem;

          //balaye les triangles
          for (ielem=0 ; ielem<nb_elems ; ielem++)
            {
              if (test_orientation_triangle(les_elems, ielem, coord_sommets)==INDIRECT)
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
    }//fin balayage zones
}
