/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Eval_VDF_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Divers
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Eval_VDF_Face_included
#define Eval_VDF_Face_included

#include <Champ_Face.h>
#include <Ref_Champ_base.h>
#include <Neumann_sortie_libre.h>


//
// .DESCRIPTION class Eval_VDF_Face
//
// Cette classe represente le prototype fonctionnel des evaluateurs
// de flux associes aux equations de conservation integrees
// sur les volumes entrelaces

class Eval_VDF_Face
{

public:
  inline void associer_inconnue(const Champ_base& );
  int calculer_arete_bord() const;
  int calculer_arete_symetrie_paroi() const;
  int calculer_arete_symetrie_fluide() const;
  void flux_arete_symetrie_fluide(const DoubleTab&, int, int, int, int,
                                  double&, double&) const
  {
    ;
  }
  double flux_arete_symetrie_paroi(const DoubleTab&, int, int, int, int) const
  {
    return 0.;
  }
  void flux_arete_symetrie_fluide(const DoubleTab&, int, int,
                                  int, int, DoubleVect&, DoubleVect&) const
  {
    ;
  }
  void flux_arete_symetrie_paroi(const DoubleTab&, int, int,
                                 int, int, DoubleVect& ) const
  {
    ;
  }

  int calculer_arete_coin_fluide() const
  {
    Cerr << "arete_coin_fluide not coded for this scheme." << finl;
    Cerr << "For TRUST support: code like Eval_Amont_VDF_Face::flux_arete_coin_fluide()" << finl;
    Process::exit();
    return 1;
  }

  void flux_arete_coin_fluide(const DoubleTab&, int, int, int, int,
                              double&, double&) const
  {
    Cerr << "arete_coin_fluide not coded for this scheme." << finl;
    Cerr << "For TRUST support: code like Eval_Amont_VDF_Face::flux_arete_coin_fluide()" << finl;
    Process::exit();
  }

  void flux_arete_coin_fluide(const DoubleTab&, int, int,
                              int, int, DoubleVect&, DoubleVect&) const
  {
    Cerr << "arete_coin_fluide not coded for this scheme." << finl;
    Cerr << "For TRUST support: code like Eval_Amont_VDF_Face::flux_arete_coin_fluide()" << finl;
    Process::exit();
  };

  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // scalaires.

  void coeffs_fa7_elem(int, int, int, double& aii, double& ajj) const
  {
    ;
  }
  void coeffs_fa7_sortie_libre(int, const Neumann_sortie_libre&,
                               double& aii, double& ajj ) const
  {
    ;
  }
  void coeffs_arete_interne(int, int, int, int,
                            double& aii, double& ajj) const
  {
    ;
  }
  void coeffs_arete_mixte(int, int, int, int,
                          double& aii, double& ajj) const
  {
    ;
  }
  void coeffs_arete_symetrie(int, int, int, int,
                             double& aii1_2, double& aii3_4, double& ajj1_2) const
  {
    ;
  }
  void coeffs_arete_paroi(int, int, int, int,
                          double& aii1_2, double& aii3_4, double& ajj1_2) const
  {
    ;
  }
  void coeffs_arete_fluide(int, int, int, int,
                           double& aii1_2, double& aii3_4, double& ajj1_2) const
  {
    ;
  }
  void coeffs_arete_paroi_fluide(int, int, int, int,
                                 double& aii1_2, double& aii3_4, double& ajj1_2) const
  {
    ;
  }
  void coeffs_arete_coin_fluide(int, int, int, int,
                                double& aii1_2, double& aii3_4, double& ajj1_2) const
  {
    ;
  }
  void coeffs_arete_periodicite(int, int, int, int,
                                double& aii, double& ajj) const
  {
    ;
  }
  void coeffs_arete_symetrie_paroi(int, int, int, int,
                                   double& aii1_2, double& aii3_4, double& ajj1_2) const
  {
    ;
  }
  void coeffs_arete_symetrie_fluide(int, int, int, int,
                                    double& aii1_2, double& aii3_4, double& ajj1_2) const
  {
    ;
  }

  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l'implicite dans le cas scalaire.

  double secmem_fa7_elem( int, int, int) const
  {
    return 0;
  }
  double secmem_fa7_sortie_libre(int, const Neumann_sortie_libre&, int ) const
  {
    return 0;
  }
  double secmem_arete_interne(int, int, int, int) const
  {
    return 0;
  }
  double secmem_arete_mixte(int, int, int, int) const
  {
    return 0;
  }
  double secmem_arete_symetrie(int, int, int, int) const
  {
    return 0;
  }
  double secmem_arete_paroi(int, int, int, int ) const
  {
    return 0;
  }
  void secmem_arete_fluide(int, int, int, int, double&, double&) const
  {
    ;
  }
  void secmem_arete_paroi_fluide(int, int, int, int, double&, double&) const
  {
    ;
  }
  void secmem_arete_coin_fluide(int, int, int, int, double&, double&) const
  {
    ;
  }
  void secmem_arete_periodicite(int, int, int, int, double&, double&) const
  {
    ;
  }
  double secmem_arete_symetrie_paroi(int, int, int, int ) const
  {
    return 0;
  }
  void secmem_arete_symetrie_fluide(int, int, int, int, double&, double&) const
  {
    ;
  }

  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // vectorielles.

  void coeffs_fa7_elem(int, int, int, DoubleVect& aii, DoubleVect& ajj) const
  {
    ;
  }
  void coeffs_fa7_sortie_libre(int , const Neumann_sortie_libre&, DoubleVect& aii, DoubleVect& ajj) const
  {
    ;
  }
  void coeffs_arete_interne(int, int, int, int,
                            DoubleVect& aii, DoubleVect& ajj) const
  {
    ;
  }
  void coeffs_arete_mixte(int, int, int, int,
                          DoubleVect& aii, DoubleVect& ajj) const
  {
    ;
  }
  void coeffs_arete_symetrie(int, int, int, int,
                             DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
  {
    ;
  }
  void coeffs_arete_paroi(int, int, int, int,
                          DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
  {
    ;
  }
  void coeffs_arete_fluide(int, int, int, int,
                           DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
  {
    ;
  }
  void coeffs_arete_paroi_fluide(int, int, int, int,
                                 DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
  {
    ;
  }
  void coeffs_arete_coin_fluide(int, int, int, int,
                                DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
  {
    ;
  }
  void coeffs_arete_periodicite(int, int, int, int,
                                DoubleVect& aii, DoubleVect& ajj) const
  {
    ;
  }
  void coeffs_arete_symetrie_paroi(int, int, int, int,
                                   DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
  {
    ;
  }
  void coeffs_arete_symetrie_fluide(int, int, int, int,
                                    DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
  {
    ;
  }
  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l'implicite dans le cas vectoriel.

  void secmem_fa7_elem(int, int, int, DoubleVect& flux) const
  {
    ;
  }
  void secmem_fa7_sortie_libre(int , const Neumann_sortie_libre&, int, DoubleVect& flux) const
  {
    ;
  }
  void secmem_arete_interne(int, int, int, int, DoubleVect& flux) const
  {
    ;
  }
  void secmem_arete_mixte(int, int, int, int, DoubleVect& flux) const
  {
    ;
  }
  void secmem_arete_symetrie(int, int, int, int, DoubleVect& ) const
  {
    ;
  }
  void secmem_arete_paroi(int, int, int, int, DoubleVect& ) const
  {
    ;
  }
  void secmem_arete_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const
  {
    ;
  }
  void secmem_arete_paroi_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const
  {
    ;
  }
  void secmem_arete_coin_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const
  {
    ;
  }
  void secmem_arete_periodicite(int, int, int, int, DoubleVect&, DoubleVect&) const
  {
    ;
  }
  void secmem_arete_symetrie_paroi(int, int, int, int, DoubleVect& ) const
  {
    ;
  }
  void secmem_arete_symetrie_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const
  {
    ;
  }

protected:

  REF(Champ_base) inconnue;

};

//
// Fonctions inline de la classe Eval_VDF_Face
//

inline void Eval_VDF_Face::associer_inconnue(const Champ_base& inco)
{
  assert(sub_type(Champ_Face,inco));
  inconnue=ref_cast(Champ_Face,inco);
}

#endif
