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

#ifndef Op_Conv_VDF_base_included
#define Op_Conv_VDF_base_included

#include <Operateur_Conv.h>
#include <Iterateur_VDF.h>

/*! @brief class Op_Conv_VDF_base Classe de base des operateurs de convection VDF
 *
 */
class Op_Conv_VDF_base : public Operateur_Conv_base
{
  Declare_base(Op_Conv_VDF_base);

public:
  inline Op_Conv_VDF_base( const Iterateur_VDF_base& iter_base) : iter(iter_base) { } // constructeur
  void completer() override;
  void preparer_calcul() override;
  void associer_zone_cl_dis(const Zone_Cl_dis_base& zcl) override { iter->associer_zone_cl_dis(zcl); }
  void calculer_dt_local(DoubleTab&) const override ; //Local time step calculation
  void calculer_pour_post(Champ& espace_stockage,const Nom& option,int comp) const override;
  double calculer_dt_stab() const override;

  int impr(Sortie& os) const override;
  Motcle get_localisation_pour_post(const Nom& option) const override;
  virtual const Champ_base& vitesse() const = 0;
  virtual Champ_base& vitesse() = 0;
  inline DoubleTab& calculer(const DoubleTab& inco, DoubleTab& resu ) const override { return iter->calculer(inco, resu); }
  inline const Iterateur_VDF& get_iter() const { return iter; }
  inline Iterateur_VDF& get_iter() { return iter; }

  inline int has_interface_blocs() const override { return 1; }
  void ajouter_blocs(matrices_t , DoubleTab& , const tabs_t& ) const override;
  void dimensionner_blocs_elem(matrices_t , const tabs_t& ) const;
  void dimensionner_blocs_face(matrices_t , const tabs_t& ) const;

  void associer_champ_temp(const Champ_Inc&, bool) const override;
  void contribuer_au_second_membre(DoubleTab& resu) const override
  {
    Cerr << "Op_Conv_VDF_base::" << __func__ << " should not be called !" << finl;
    Process::exit();
  }

protected:
  Iterateur_VDF iter;
  void associer_champ_convecte_elem();
  void associer_champ_convecte_face();
};

// Fonction utile pour le calcul du pas de temps de stabilite
inline void eval_fluent(const double , const int , const int , DoubleVect& );

#endif /* Op_Conv_VDF_base_included */
