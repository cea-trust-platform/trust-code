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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Champ_front_contact_VEF.h
// Directory:   $TRUST_ROOT/src/VEF/Champs
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Champ_front_contact_VEF_included
#define Champ_front_contact_VEF_included

#include <Ch_front_var_instationnaire_dep.h>
#include <Ref_Champ_Inc_base.h>
#include <Front_VF.h>
#include <Ref_Front_VF.h>
#include <Ref_Champ_front_contact_VEF.h>
#include <Motcle.h>
#include <Zone_VEF.h>

class Equation_base;
class Milieu_base;
class Zone_dis_base;
class Zone_Cl_dis_base;
class Front_dis_base;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Champ_front_contact_VEF
//     Permet le couplage scalaire (temperature ou concentration) entre problemes en calculant
//     la valeur a la paroi qui permet d'egaliser les flux.
//     calculer_coeffs_echange stocke grad_num_local et grad_fro_local tels que (aux signes pres?)
//     flux = grad_num_local + Scal_paroi*grad_fro_local
//     mettre_a_jour recupere les grad* d'en face et calcule Scal_paroi de
//     facon a egaliser les flux.
// .SECTION voir aussi
//     Champ_front_var_instationnaire Champ_Inc
//////////////////////////////////////////////////////////////////////////////
class Champ_front_contact_VEF : public Ch_front_var_instationnaire_dep
{

  Declare_instanciable(Champ_front_contact_VEF);

public:
  void completer() override;
  int initialiser(double temps, const Champ_Inc_base& inco) override;
  Champ_front_base& affecter_(const Champ_front_base& ch) override ;
  void mettre_a_jour(double temps) override;
  void calculer_coeffs_echange(double temps) override;
  void creer(const Nom&, const Nom&,const Nom&, const Nom&, const Motcle&);
  const Champ_Inc_base& inconnue() const;
  const Equation_base& equation() const;
  const Zone_dis_base& zone_dis() const;
  const Zone_Cl_dis_base& zone_Cl_dis() const ;
  const Frontiere_dis_base& front_dis() const ;
  void associer_ch_inc_base(const Champ_Inc_base&);
  const Nom& nom_bord_oppose() const;
  const Milieu_base& milieu() const;

  virtual void calcul_grads_transf(double temps);
  virtual void calcul_grads_locaux(double temps);
  void calcul_coeff_amort();
  void recupere_gradients_autre_pb(const Front_VF&, DoubleVect&, DoubleVect&);
  DoubleVect& trace_face_raccord(const Front_VF& fr_vf,const DoubleVect& y,DoubleVect& x);
  void remplir_connect_bords();
  void verifier_scalaire_bord(double temps);
  void associer_front_vf_et_ch_front_autre_pb();

  void connectivity_failed(const Zone_VEF&,  int&,  int&,
                           const Zone_VEF&,  int&,  int&, Nom&, Nom&) ;
  void remplir_faces_coin();
  void remplir_elems_voisin_bord();
  void test_faces_coin();

  inline DoubleVect& set_gradient_num_local();
  inline const DoubleVect& get_gradient_num_local() const;
  inline DoubleVect& set_gradient_fro_local();
  inline  const DoubleVect& get_gradient_fro_local() const;

  inline  DoubleVect& set_gradient_num_transf();
  inline  const DoubleVect& get_gradient_num_transf() const;
  inline  DoubleVect& set_gradient_fro_transf();
  inline  const DoubleVect& get_gradient_fro_transf() const;

  inline  DoubleVect& set_coeff_amort_num();
  inline  const DoubleVect& get_coeff_amort_num() const;
  inline DoubleVect& set_coeff_amort_denum();
  inline  const DoubleVect& get_coeff_amort_denum() const;

  inline const IntVect& get_elems_voisin_bord() const;
  inline const Nom& get_nom_bord1() const;
  inline const IntVect& get_connect_bords() const;
  inline const IntVect& get_faces_coin() const;

protected :
  int is_conduction; // Est-on dans un probleme conduction ?
  REF(Champ_Inc_base) l_inconnue1;
  REF(Champ_Inc_base) l_inconnue2;
  REF(Champ_Inc_base) l_inconnue;
  REF(Front_VF) fr_vf_autre_pb;
  REF(Champ_front_contact_VEF) ch_fr_autre_pb;

  Nom nom_bord1;
  Nom nom_bord2;
  Nom nom_bord ;
  Nom nom_pb1;
  Nom nom_pb2;

  IntVect connect_bords; // Tableau de connectivite des faces de bord des deux bord conjugues (en sequentiel)
  int connect_est_remplit;

  DoubleVect gradient_num_local;
  DoubleVect gradient_fro_local;
  DoubleVect gradient_num_transf;
  DoubleVect gradient_fro_transf;
  DoubleVect Scal_moy;

  DoubleVect coeff_amort_num;
  DoubleVect coeff_amort_denum;

  IntVect elems_voisin_bord_; //Contient le numero de l element voisin pour chaque face de bord
  IntVect faces_coin;         //Prend la valeur 1 si la face de bord est une face de coin (0 sinon)
  int verification_faces_coin;
};





inline DoubleVect& Champ_front_contact_VEF::set_gradient_num_local()
{
  return gradient_num_local;
}

inline const DoubleVect& Champ_front_contact_VEF::get_gradient_num_local() const
{
  return gradient_num_local;
}

inline DoubleVect& Champ_front_contact_VEF::set_gradient_fro_local()
{
  return gradient_fro_local;
}

inline const DoubleVect& Champ_front_contact_VEF::get_gradient_fro_local() const
{
  return gradient_fro_local;
}


inline  DoubleVect& Champ_front_contact_VEF::set_gradient_num_transf()
{
  return gradient_num_transf;
}

inline  const DoubleVect& Champ_front_contact_VEF::get_gradient_num_transf() const
{
  return gradient_num_transf;
}

inline  DoubleVect& Champ_front_contact_VEF::set_gradient_fro_transf()
{
  return gradient_fro_transf;
}

inline  const DoubleVect& Champ_front_contact_VEF::get_gradient_fro_transf() const
{
  return gradient_fro_transf;
}

inline  DoubleVect& Champ_front_contact_VEF::set_coeff_amort_num()
{
  return coeff_amort_num;
}

inline  const DoubleVect& Champ_front_contact_VEF::get_coeff_amort_num() const
{
  return coeff_amort_num;
}

inline DoubleVect& Champ_front_contact_VEF::set_coeff_amort_denum()
{
  return coeff_amort_denum;
}

inline const DoubleVect& Champ_front_contact_VEF::get_coeff_amort_denum() const
{
  return coeff_amort_denum;
}

inline const IntVect& Champ_front_contact_VEF::get_elems_voisin_bord() const
{
  return elems_voisin_bord_;
}

inline const Nom& Champ_front_contact_VEF::get_nom_bord1() const
{
  return nom_bord1;
}

inline const IntVect& Champ_front_contact_VEF::get_connect_bords() const
{
  return connect_bords;
}

inline const IntVect& Champ_front_contact_VEF::get_faces_coin() const
{
  return faces_coin;
}

#endif
