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

#ifndef Domaine_Cl_VDF_included
#define Domaine_Cl_VDF_included

#include <Domaine_Cl_dis_base.h>
#include <Champ_Inc.h>

class Domaine_VDF;

namespace TypeAreteBordVDF
{
enum type_arete
{
  VIDE = -1,
  PAROI_PAROI = 0,
  FLUIDE_FLUIDE = 1,
  PAROI_FLUIDE = 2,
  NAVIER_NAVIER = 3,
  PERIO_PERIO = 4,
  PAROI_NAVIER = 5,
  FLUIDE_NAVIER = 6
};
} // namespace TypeAreteBordVDF

namespace TypeAreteCoinVDF
{
enum type_arete
{
  VIDE = -1,
  PERIO_PERIO = 0,
  PERIO_PAROI = 1,
  PAROI_NAVIER = 3,
  FLUIDE_NAVIER = 4,
  PAROI_PAROI = 5,
  PERIO_FLUIDE = 7,
  NAVIER_NAVIER = 8,
  PAROI_FLUIDE = 14,
  FLUIDE_PAROI = 15,
  FLUIDE_FLUIDE = 16
};
} // namespace TypeAreteCoinVDF

/*! @brief class Domaine_Cl_VDF
 *
 * @sa Domaine_Cl_dis_base
 */

class Domaine_Cl_VDF : public Domaine_Cl_dis_base
{
  Declare_instanciable(Domaine_Cl_VDF);
public :
  void associer(const Domaine_dis_base&) override;
  void completer(const Domaine_dis_base&) override;
  void imposer_cond_lim(Champ_Inc&, double) override;
  int nb_faces_sortie_libre() const;
  int nb_faces_bord() const;
  Domaine_VDF& domaine_VDF();
  const Domaine_VDF& domaine_VDF() const;

  inline int num_Cl_face(int numfa) const { return num_Cl_face_[numfa]; }
  inline const Cond_lim& la_cl_de_la_face(int numfa) const override { return les_conditions_limites_[num_Cl_face_[numfa]]; }
  inline int type_arete_bord(int num_arete) const { return type_arete_bord_[num_arete]; }
  inline const IntVect& type_arete_bord() const { return type_arete_bord_; }
  inline const int& type_arete_coin(int num_arete) const { return type_arete_coin_[num_arete]; }
  inline const IntVect& type_arete_coin() const { return type_arete_coin_; }

private:
  //  type des aretes bord:
  //  type = 0 si l'arete separe deux faces de paroi
  //  type = 1 si l'arete separe deux faces "fluide"
  //  type = 2 si l'arete separe une face de paroi et une face "fluide"
  //  type = 3 si l'arete separe deux faces de navier (symmetrie/paroi frottante)
  //  type = 3 si l'arete separe deux faces de periodicite
  //  type = 5 si l'arete separe une face de navier (symmetrie/paroi frottante) et une face de paroi
  //  type = 6 si l'arete separe une face de navier (symmetrie/paroi frottante) et une face "fluide"
  IntVect type_arete_bord_;

  //  type des aretes coin:
  //  type = 0 si l'arete contient deux faces de periodicite
  //  type = 1 si l'arete contient une face de periodicite et une face de paroi
  IntVect type_arete_coin_;

  //  pour chaque face de bord numero de la condition limite a laquelle elle se rattache
  IntVect num_Cl_face_;
};

#endif /* Domaine_Cl_VDF_included */
