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

#include <Reordonner.h>
#include <Domaine.h>

Implemente_instanciable_32_64(Reordonner_32_64,"Reordonner",Interprete_geometrique_base_32_64<_T_>);
// XD resequencing interprete reordonner -1 The Reordonner_32_64 interpretor is required sometimes for a VDF mesh which is not produced by the internal mesher. Example where this is used: NL2 Read_file dom fichier.geom NL2 Reordonner_32_64 dom NL2 Observations: This keyword is redundant when the mesh that is read is correctly sequenced in the TRUST sense. This significant mesh operation may take some time... The message returned by TRUST is not explicit when the Reordonner_32_64 (Resequencing) keyword is required but not included in the data set...
// XD attr domain_name ref_domaine domain_name 0 Name of domain to resequence.


/*! @brief Ecrit le type de l'objet sur un flot de sortie.
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
template <typename _SIZE_>
Sortie& Reordonner_32_64<_SIZE_>::printOn(Sortie& s ) const
{
  return s << this->que_suis_je() << finl;
}


/*! @brief NE FAIT RIEN
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree
 */
template <typename _SIZE_>
Entree& Reordonner_32_64<_SIZE_>::readOn(Entree& is )
{
  return is;
}


/*! @brief Fonction principale de l'interprete Mailler Reordonne les noeuds du domaine specifie par
 *
 *     la directive.
 *        Reordonner_32_64 dom
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 * @throws l'objet a reordonner n'est pas du type Domaine
 */
template <typename _SIZE_>
Entree& Reordonner_32_64<_SIZE_>::interpreter_(Entree& is)
{
  this->associer_domaine(is);
  this->domaine().reordonner();
  return is;
}


template class Reordonner_32_64<int>;
#if INT_is_64_ == 2
template class Reordonner_32_64<trustIdType>;
#endif


