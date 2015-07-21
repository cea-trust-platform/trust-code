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
// File:        Zone_Cl_dis_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Zone_Cl_dis_base_included
#define Zone_Cl_dis_base_included




#include <Zone_dis.h>
#include <Conds_lim.h>
class Champ_Inc;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Zone_Cl_dis_base
//     Les objets Zone_Cl_dis_base representent les conditions aux limites
//     discretisees. Zone_Cl_dis_base est un morceau d'equation donc elle
//     herite de MorEqn, c'est grace a cet heritage que chaque objet
//     Zone_Cl_dis_base contient une reference vers l'equation a laquelle il
//     se rapporte. Les conditions aux limites discretisees representent les
//     conditions aux limites de la zone discretisee associee a l'equation
//     reference par Zone_Cl_dis_base.
//     Zone_Cl_dis_base a un membre representant les conditions aux limites.
// .SECTION voir aussi
//     MorEqn Conds_lim Cond_lim_base
//     Classe abstraite.
//     Methodes abstraites:
//       void completer(const Zone_dis& )
//       void imposer_cond_lim(Champ_Inc& )
//////////////////////////////////////////////////////////////////////////////
class Zone_Cl_dis_base : public MorEqn, public Objet_U
{

  Declare_base(Zone_Cl_dis_base);

public:

  const Cond_lim&  les_conditions_limites(int ) const;
  Cond_lim&        les_conditions_limites(int );
  Conds_lim&       les_conditions_limites();
  const Conds_lim& les_conditions_limites() const;
  int           nb_cond_lim() const;
  virtual void     mettre_a_jour(double temps);
  virtual void     mettre_a_jour_ss_pas_dt(double temps);
  void             completer();
  int           contient_Cl(const Nom&);
  Zone_dis&        zone_dis();
  const Zone_dis&  zone_dis() const;
  Zone&            zone();
  const Zone&      zone() const;

  virtual int   calculer_coeffs_echange(double temps);
  virtual void     imposer_cond_lim(Champ_Inc&,double ) = 0;
  int           nb_faces_Cl() const;

  virtual const Cond_lim& la_cl_de_la_face(int num_face) const;

  const Cond_lim_base& condition_limite_de_la_face_reelle(int face_globale, int& face_locale) const;
  const Cond_lim_base& condition_limite_de_la_face_virtuelle(int face_globale, int& face_locale) const;
  const Cond_lim_base& condition_limite_de_la_frontiere(Nom frontiere) const;
  Cond_lim_base& condition_limite_de_la_frontiere(Nom frontiere);
  void changer_temps_futur(double temps,int i);
  void Gpoint(double t1, double t2);
  void set_temps_defaut(double temps);
  int avancer(double temps);
  int reculer(double temps);
  virtual int initialiser(double temps);
protected:

  Conds_lim  les_conditions_limites_;
  virtual void completer(const Zone_dis& ) = 0;

};

#endif

