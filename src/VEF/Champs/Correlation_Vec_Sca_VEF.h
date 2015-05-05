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
// File:        Correlation_Vec_Sca_VEF.h
// Directory:   $TRUST_ROOT/src/VEF/Champs
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Correlation_Vec_Sca_VEF_included
#define Correlation_Vec_Sca_VEF_included


#include <Champ_Fonc_P1NC.h>

//
//.DESCRIPTION  classe Correlation_Vec_Sca_VEF
//
// .SECTION voir aussi
//  Champ_Fonc_P1NC Correlation_Vec_Sca_VEF

class Correlation_Vec_Sca_VEF : public Champ_Fonc_P1NC

{

  Declare_instanciable(Correlation_Vec_Sca_VEF);

public:

  inline const Champ_base& mon_champ_Vec() const;
  inline const Champ_base& mon_champ_Sca() const;
  inline void mettre_a_jour(double );
  void associer_champ_Vec(const Champ_base& );
  void associer_champ_Sca(const Champ_base& );

protected:

  REF(Champ_base) mon_champ_Vec_;
  REF(Champ_base) mon_champ_Sca_;
};

inline const Champ_base& Correlation_Vec_Sca_VEF::mon_champ_Vec() const
{
  return mon_champ_Vec_.valeur();
}

inline const Champ_base& Correlation_Vec_Sca_VEF::mon_champ_Sca() const
{
  return mon_champ_Sca_.valeur();
}

inline void Correlation_Vec_Sca_VEF::mettre_a_jour(double tps)
{
  DoubleTab& valeurs_Vec = mon_champ_Vec_->valeurs();
  DoubleTab& valeurs_Sca = mon_champ_Sca_->valeurs();
  int nb_faces = valeurs_Sca.size();

  DoubleTab& correlation = valeurs();
  int face, ncom;
  for (face=0; face<nb_faces; face++)
    {
      correlation(face,0) = valeurs_Sca(face);
      for (ncom=0; ncom<dimension; ncom++)
        correlation(face,ncom+1) = valeurs_Vec(face,ncom);
    }

  changer_temps(tps);
  Champ_Fonc_base::mettre_a_jour(tps);

  /*  int nb_elem=zone().nb_elem();
      DoubleTab centres_de_gravites(nb_elem, dimension);
      zone().calculer_centres_gravite(centres_de_gravites);

      DoubleTab& correlation = valeurs();
      int elem, ncom;

      // Interpolation du champ scalaire de la correlation au
      // centre des mailles.
      DoubleTab valeurs_Sca(nb_elem, mon_champ_Sca_->nb_comp());
      mon_champ_Sca_->valeur_aux(centres_de_gravites, valeurs_Sca);

      // Interpolation du champ vecteur de la correlation au
      // centre des mailles.
      DoubleTab valeurs_Vec(nb_elem, mon_champ_Vec_->nb_comp());
      mon_champ_Vec_->valeur_aux(centres_de_gravites, valeurs_Vec);

      for(elem=0; elem<nb_elem; elem++)
      {
      correlation(elem,0) = valeurs_Sca(elem,0);
      for (ncom=0; ncom<dimension; ncom++)
      correlation(elem,ncom+1) = valeurs_Vec(elem,ncom);
      }

      changer_temps(tps);
      Champ_Fonc_base::mettre_a_jour(tps);*/
}



#endif
