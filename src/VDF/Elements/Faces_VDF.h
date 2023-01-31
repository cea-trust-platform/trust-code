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

#ifndef Faces_VDF_included
#define Faces_VDF_included

#include <Faces.h>
#include <Ref_Domaine_VDF.h>




class Aretes;

/*! @brief class Faces_VDF
 *
 */


class Faces_VDF : public Faces
{
  Declare_instanciable(Faces_VDF);

public:

  inline int orientation(int) const;
  inline int orientation(int);
  inline void associer(const Domaine_VDF&);
  inline const Domaine_VDF& domaine_VDF() const;
  void calculer_orientation(IntVect&, int&, int&, int& );
  void reordonner(IntVect& );
  void reordonner_pour_debog(IntVect& );

private :

  IntVect orientation_;       // orientation des faces :
  //    0 pour une face d'equation x = cte
  //    1 pour une face d'equation y = cte
  //    2 pour une face d'equation z = cte
  REF(Domaine_VDF) le_dom_VDF;
  void swap(int, int);

};

inline int Faces_VDF::orientation(int i) const
{
  return orientation_(i);
}

inline int Faces_VDF::orientation(int i)
{
  return orientation_(i);
}

/*! @brief se lie au domaine
 *
 */
inline void Faces_VDF::associer(const Domaine_VDF& une_domaine_VDF)
{
  le_dom_VDF=une_domaine_VDF;
}

/*! @brief Renvoie le domaine
 *
 */
inline const Domaine_VDF& Faces_VDF::domaine_VDF() const
{
  return le_dom_VDF.valeur();
}

#endif
