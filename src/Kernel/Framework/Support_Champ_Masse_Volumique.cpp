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

#include <Support_Champ_Masse_Volumique.h>

/*! @brief Constructeur de la classe.
 *
 * Par defaut, une classe derivee deja codee appelle le constructeur sans argument. support_ok est mis a zero
 *   et on produit une erreur si Associer_champ_masse_volumique est appele.
 *   Pour signifier que le champ de masse volumique est supporte par
 *   la classe derivee, il faut appeler Declare_support_masse_volumique
 *
 */
Support_Champ_Masse_Volumique::Support_Champ_Masse_Volumique() :
  support_ok_(0)
{
}

/*! @brief Destructeur virtuel (pour eviter les warnings)
 *
 */
Support_Champ_Masse_Volumique::~Support_Champ_Masse_Volumique()
{
}

/*! @brief Le constructeur d'une classe derivee qui se sert de la masse volumique doit appeler cette fonction avec la valeur 1.
 *
 * Si une classe cliente
 *   (Navier Stokes par ex.) essaie d'associer la masse volumique a une
 *   classe qui n'a pas fait ok=1, on s'arrete : fonction non implementee.
 *
 */
void Support_Champ_Masse_Volumique::declare_support_masse_volumique(int ok)
{
  support_ok_ = ok;
}

/*! @brief Methode a appeler dans la preparation du probleme pour demander a l'objet de tenir compte du champ de masse volumique en parametre.
 *
 */
void Support_Champ_Masse_Volumique::associer_champ_masse_volumique(const Champ_base& ch)
{
  if (! support_ok_)
    {
      Cerr << "Error in Support_Champ_Masse_Volumique::Associer_champ_masse_volumique" << finl;
      Cerr << " Support from a density field was not coded for this object" << finl;
      assert(0);
      Process::exit();
    }
  ref_champ_rho_ = ch;
}

/*! @brief Annule la reference a la masse volumique.
 *
 */
void Support_Champ_Masse_Volumique::dissocier_champ_masse_volumique()
{
  REF(Champ_base) ref_nulle;
  ref_champ_rho_ = ref_nulle;
}

/*! @brief Renvoie 1 si la masse volumique a ete associee, 0 sinon.
 *
 */
int Support_Champ_Masse_Volumique::has_champ_masse_volumique() const
{
  int ref_non_nulle = ref_champ_rho_.non_nul();
  return ref_non_nulle;
}

/*! @brief Renvoie le champ de masse volumique
 *
 */
const Champ_base& Support_Champ_Masse_Volumique::get_champ_masse_volumique() const
{
  assert(ref_champ_rho_.non_nul());
  const Champ_base& ch = ref_champ_rho_.valeur();
  return ch;
}

