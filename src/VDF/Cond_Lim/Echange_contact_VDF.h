/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Echange_contact_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Cond_Lim
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Echange_contact_VDF_included
#define Echange_contact_VDF_included


#include <Echange_global_impose.h>
class Front_VF;
class Zone_VDF;
class Faces;
#include <Ref_Champ_Inc.h>
#include <IntTab.h>

////////////////////////////////////////////////////////////////

//
//  .DESCRIPTION classe : Echange_contact_VDF
//  Outre le champ_front representant la temperature de paroi,
//  cette classe possede un autre champ_front avec autant de valeurs
//  temporelles qui represente la temperature dans l'autre probleme.

////////////////////////////////////////////////////////////////

void calculer_h_local(DoubleTab& tab,const Equation_base& une_eqn,const Zone_VDF& zvdf_2,const Front_VF& front_vf,const    Milieu_base& le_milieu,double invhparoi,int opt);



class Echange_contact_VDF  : public Echange_global_impose
{

  Declare_instanciable(Echange_contact_VDF);
public :
  virtual void completer();
  virtual int initialiser(double temps);
  virtual void mettre_a_jour(double );
  void calculer_h_autre_pb(DoubleTab& tab,double invhparoi,int opt);
  void calculer_h_mon_pb(DoubleTab&, double, int);
  inline virtual Champ_front& T_autre_pb()
  {
    return T_autre_pb_;
  };
  inline virtual const Champ_front& T_autre_pb() const
  {
    return T_autre_pb_;
  };
  virtual void calculer_Teta_paroi(DoubleTab& tab_p,const DoubleTab& mon_h,const DoubleTab& autre_h,int is_pb_fluide, double temps);
  virtual void calculer_Teta_equiv(DoubleTab& Teta_equiv,const DoubleTab& mon_h,const DoubleTab& autre_h,int is_pb_fluide, double temps);
  int verifier_correspondance() const;
  inline const DoubleTab&  T_wall() const
  {
    return T_wall_;
  };
  inline const DoubleTab&  h_autre_pb() const
  {
    return autre_h;
  };
  inline const IntTab& get_size_bloc_and_distant_elems() const
  {
    // renvoie nl, nc puis e_pb1 -> e_pb2 pour e voisin d'une face de paroi contact
    // avec nl, nc  le nombre de lignes et de colonnes de la matrice de couplage entre le pb1 et le pb2
    return size_bloc_and_distant_faces_;
  };
  inline const Nom& nom_autre_pb() const
  {
    return nom_autre_pb_;
  };

  virtual void changer_temps_futur(double temps,int i);
  virtual int avancer(double temps);
  virtual int reculer(double temps);

protected :
  double h_paroi;
  DoubleTab autre_h;
  Champ_front T_autre_pb_;
  DoubleTab T_wall_;
  Nom nom_autre_pb_;
  IntTab size_bloc_and_distant_faces_;
};
#endif
