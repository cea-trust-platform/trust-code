/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Hexaedre_axi.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Hexaedre_axi_included
#define Hexaedre_axi_included




#include <Hexaedre.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Hexaedre_axi
//    Cette classe represente la deformee de l'hexaedre dans
//    le cas d'un maillage en coordonnees cylindriques
//    Les methodes de la classe Hexaedre dont Hexaedre_axi derive sont
//    surchargees pour effectuer les calculs en coordonnees cylindrique.
// .SECTION voir aussi
//    Hexaedre Elem_geom_base Elem_geom
//////////////////////////////////////////////////////////////////////////////
class Hexaedre_axi : public Hexaedre
{

  Declare_instanciable(Hexaedre_axi);

public :

  const Nom& nom_lml() const;
  void reordonner() ;
  virtual int contient(const ArrOfDouble&, int ) const;
  virtual int contient(const ArrOfInt&, int ) const;
  inline Type_Face type_face(int=0) const;
  void calculer_centres_gravite(DoubleTab& ) const ;
  void calculer_volumes(DoubleVect& ) const;

};


// Description:
//    Renvoie le i-ieme type de face.
//    Un Hexaedre (Axi) n'a qu'un seul type de face.
// Precondition:
// Parametre: int i
//    Signification: le rang du type de face a renvoyer
//    Valeurs par defaut: 0
//    Contraintes: i=0
//    Acces:
// Retour: Type_Face
//    Signification: un type de face
//    Contraintes: toujours egal a quadrangle_3D_axi (si i=0)
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline Type_Face Hexaedre_axi::type_face(int i) const
{
  assert(i==0);
  return quadrangle_3D_axi;
}

#endif
