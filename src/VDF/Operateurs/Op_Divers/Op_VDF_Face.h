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

#ifndef Op_VDF_Face_included
#define Op_VDF_Face_included

#include <Zone_VDF.h>
#include <TRUSTTab.h>

class Matrice_Morse;
class Zone_Cl_VDF;

class Op_VDF_Face
{
public :
  void dimensionner(const Zone_VDF&, const Zone_Cl_VDF&, Matrice_Morse&) const;
  void modifier_pour_Cl(const Zone_VDF&, const Zone_Cl_VDF&, Matrice_Morse&, DoubleTab&) const;

private:
  void modifier_pour_Cl_(const int , const int , const int ,  Matrice_Morse& ) const;
  inline void dimensionner_(const int , int& , IntVect& ) const;
  inline void dimensionner_(const int , const int , int& , IntVect& ) const; // pour perio
};

inline void Op_VDF_Face::dimensionner_(const int num_face, int& cpt, IntVect& tab2)const
{
  if(num_face > -1)
    {
      tab2[cpt] = num_face + 1;
      cpt++;
    }
}

inline void Op_VDF_Face::dimensionner_(const int num_face1, const int num_face2, int& cpt, IntVect& tab2)const // pour perio
{
  if (num_face1 != -1) // on remplace num_face1 par num_face2 dans tab2
    {
      assert(tab2[cpt] == num_face1+1);
      tab2[cpt] = num_face2+1;
      cpt++;
    }
}

// methode interne pour la classe Op_VDF_Face !
inline int face_bord_amont2(const Zone_VDF& la_zone , const int num_face , const int k , const int i)
{
  const int ori = la_zone.orientation(num_face);
  int elem = la_zone.face_voisins(num_face,0);
  if(elem != -1)
    {
      const int face = la_zone.elem_faces(elem, k+i*Objet_U::dimension), elem_bis = la_zone.face_voisins(face,i);
      elem = (elem_bis != -1) ? la_zone.elem_faces(elem_bis, ori+Objet_U::dimension) : -1;
    }
  else Process::exit();

  return elem;
}

#endif /* Op_VDF_Face_included */
