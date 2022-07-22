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

#include <Raccord_base.h>

Implemente_base(Raccord_base,"Raccord_base",Frontiere);
Implemente_base(Raccord_local,"Raccord_local",Raccord_base);
Implemente_base(Raccord_distant,"Raccord_distant",Raccord_base);
Implemente_instanciable(Raccord_local_homogene,"Raccord_local_homogene",Raccord_local);

/*! @brief Simple appel a: Frontiere::printOn(Sortie& )
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Raccord_base::printOn(Sortie& s ) const
{
  return Frontiere::printOn(s) ;
}


/*! @brief Simple appel a: Frontiere::readOn(Entree& )
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Raccord_base::readOn(Entree& s)
{
  return Frontiere::readOn(s) ;
}


/*! @brief Simple appel a: Raccord_base::printOn(Sortie& )
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Raccord_local::printOn(Sortie& s ) const
{
  return Raccord_base::printOn(s) ;
}




/*! @brief Simple appel a: Raccord_base::readOn(Entree& )
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Raccord_local::readOn(Entree& s)
{
  return Raccord_base::readOn(s) ;
}


/*! @brief Simple appel a: Raccord_base::printOn(Sortie& )
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Raccord_distant::printOn(Sortie& s ) const
{
  return Raccord_base::printOn(s) ;
}




//    Simple appel a: Raccord_base::readOn(Entree& )
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Raccord_distant::readOn(Entree& s)
{
  return Raccord_base::readOn(s) ;
}



/*! @brief Simple appel a: Raccord_local::printOn(Sortie& )
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Raccord_local_homogene::printOn(Sortie& s ) const
{
  return Raccord_local::printOn(s) ;
}

/*! @brief Simple appel a: Raccord_local::readOn(Entree& )
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Raccord_local_homogene::readOn(Entree& s)
{
  return Raccord_local::readOn(s) ;
}
void Raccord_local_homogene::trace_elem_distant(const DoubleTab& x, DoubleTab& y) const
{
// en suppposant les numerotations des faces identiques sur le raccord des deux cotes
  return trace_elem_local(x,y);
}
void Raccord_local_homogene::trace_face_distant(const DoubleTab& x, DoubleTab& y) const
{
  return trace_face_local(x,y);
}
void Raccord_local_homogene::trace_face_distant(const DoubleVect& x, DoubleVect& y) const
{
  return trace_face_local(x,y);
}
