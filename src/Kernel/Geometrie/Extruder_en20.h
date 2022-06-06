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

#ifndef Extruder_en20_included
#define Extruder_en20_included

#include <Interprete_geometrique_base.h>

class Domaine;
class Faces;
//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Extruder_en20
//    Cette classe est un interprete qui sert a lire et executer
//    la directive Extruder_en20:
//        Extruder_en20 nom_domaine
//    Cette directive est a utiliser en discretisation VEF 2D pour obtenir
//    un maillage 3D par extrusion puis decoupage.
// .SECTION voir aussi
//    Interprete Extruder_en20
//    Cette classe est utilisable en 3D
//////////////////////////////////////////////////////////////////////////////
class Extruder_en20 : public Interprete_geometrique_base
{
  Declare_instanciable_sans_constructeur(Extruder_en20);

public :

  Extruder_en20();
  Entree& interpreter_(Entree&) override;
  inline void setDirection(double lx, double ly, double lz)
  {
    direction[0]=lx;
    direction[1]=ly;
    direction[2]=lz;
  };
  inline void setNbTranches(int n)
  {
    NZ = n;
  };

  void extruder(Domaine&) ;


protected:
  virtual void extruder_dvt(Domaine&, Faces&, int, int ) ;

  ArrOfDouble direction;
  int NZ;

private:
  void traiter_faces_dvt(Faces&, Faces&, int, int, int);



};
#endif
