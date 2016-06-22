/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Ensemble_Lagrange_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Ensemble_Lagrange_base_included
#define Ensemble_Lagrange_base_included

#include <Zone.h>

class Equation_base;
//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Ensemble_Lagrange_base
//     Classe de base des classes representant une structure geometrique constituee
//     d un ensemble de points dont on realise un suivi Lagrangien
//     Actuellement une seule classe derivant instanciable : Maillage_FT_Disc
//     -un ensemble Lagrangien est caracterise par les coordonnees de ses points

// .SECTION voir aussi
//
//     Classe abstraite.
//     Methodes abstraites:
//       void associer_equation_transport)
//       Equation_base& equation_associee()
//////////////////////////////////////////////////////////////////////////////
class Ensemble_Lagrange_base : public Objet_U
{
  Declare_base_sans_constructeur(Ensemble_Lagrange_base);

public :
  Ensemble_Lagrange_base();
  virtual void associer_equation_transport(const Equation_base&
                                           equation) = 0;
  virtual const Equation_base& equation_associee() const = 0;
  void associer_zone(const Zone& zone);
  virtual inline const Zone& zone() const;
  void remplir_sommets_tmp(DoubleTab& soms_tmp);
  void generer_marqueurs_sz(DoubleTab& soms_tmp);
  inline const IntVect& nb_marqs_par_sz() const;
  inline const DoubleTab& sommets_lu() const;
  inline DoubleTab& sommets_lu();

protected :

  Noms nom_sz;                //nom des sous zones ou l on genere des particules
  IntVect nb_marqs_sz;        //nombre de marqueurs par sous zone
  IntTab nb_marqs_par_dir;        //nombre de marqueurs dans chacune des directions d une sous zone
  //si distribution uniforme sur la sous zone
  REF(Zone) ma_zone_;                //REF a la Zone du maillage Eulerien

  DoubleTab sommets_lu_;      //Coordonnees des sommets lus dans le cas d une lecture dans un fichier

private :

};

inline const Zone& Ensemble_Lagrange_base::zone() const
{
  return ma_zone_.valeur();
}

inline const IntVect& Ensemble_Lagrange_base::nb_marqs_par_sz() const
{
  return nb_marqs_sz;
}

inline const DoubleTab& Ensemble_Lagrange_base::sommets_lu() const
{
  return sommets_lu_;
}

inline DoubleTab& Ensemble_Lagrange_base::sommets_lu()
{
  return sommets_lu_;
}

#endif
