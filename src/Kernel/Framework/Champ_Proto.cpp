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

#include <Champ_Proto.h>
#include <Nom.h>

/*! @brief Verification de la dimension du champ Renvoie la dimension du champ
 *
 * @return (int) la dimension du champ
 */
int Champ_Proto::lire_dimension(Entree& is , const Nom& le_nom_)
{
  if(!is.check_types()) abort();
  // verification meme en optim
  int dim = -1;
  is >> dim;
  return lire_dimension(dim, le_nom_);
}

/*! @brief Verification de la dimension du champ Renvoie la dimension du champ
 *
 * @return (int) la dimension du champ
 */
int Champ_Proto::lire_dimension(int dim , const Nom& le_nom_)
{
  if (dim < 1)
    {
      Cerr << "Error in int Champ_Proto::lire_dimension() for " << le_nom_
           << "\n bad dimension : " << dim << finl;
      Process::exit();
    }
  return dim;
}


/*! @brief Operateur d'indexation 1D.
 *
 * Renvoie la i-ieme composantes du champ.
 *     (version const par valeur)
 *
 * @param (int i) indice de la composante du champ
 * @return (double) la i-ieme composantes du champ
 */
double Champ_Proto::operator()(int i) const
{
  return valeurs()(i);
}



/*! @brief Operateur d'indexation 1D.
 *
 * Renvoie la i-ieme composantes du champ.
 *
 * @param (int i) indice de la composante du champ
 * @return (double&) la i-ieme composantes du champ
 */
double& Champ_Proto::operator()(int i)
{
  return valeurs()(i);
}



/*! @brief Operateur d'indexation 2D.
 *
 * Renvoie la composante (i,j) du champ.
 *     (version const par valeur)
 *
 * @param (int i) indice suivant la premiere dimension du champ
 * @param (int j) indice suivant la seconde dimension du champ
 * @return (double) la composante (i,j) du champ
 */
double Champ_Proto::operator()(int i, int j) const
{
  return valeurs()(i,j);
}



/*! @brief Operateur d'indexation 2D.
 *
 * Renvoie la composante (i,j) du champ.
 *
 * @param (int i) indice suivant la premiere dimension du champ
 * @param (int j) indice suivant la seconde dimension du champ
 * @return (double&) la composante (i,j) du champ
 */
double& Champ_Proto::operator()(int i, int j)
{
  return valeurs()(i,j);
}
