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

#ifndef Source_QC_VEF_P1NC_included
#define Source_QC_VEF_P1NC_included

#include <Source_Darcy_VEF_Face.h>
#include <Terme_Source_Qdm_VEF_Face.h>
#include <Source_Forchheimer_VEF_Face.h>
#include <Source_Fluide_Dilatable_Face.h>
#include <Perte_Charge_Isotrope_VEF_P1NC.h>
#include <Perte_Charge_Reguliere_VEF_P1NC.h>
#include <Perte_Charge_Singuliere_VEF_Face.h>
#include <Perte_Charge_Circulaire_VEF_P1NC.h>
#include <Perte_Charge_Anisotrope_VEF_P1NC.h>
#include <Terme_Source_Acceleration_VEF_Face.h>
#include <Perte_Charge_Directionnelle_VEF_P1NC.h>

/// \cond DO_NOT_DOCUMENT
class Source_QC_VEF_P1NC
{ };
/// \endcond

/*
 * HERITAGE V ABSOLUMENT INUTILE :-(
 * JUSTE POUR COMPLIQUER LA VIE
 * C'EST JUSTE DU BRICOLAGE POUR QUE CHECK_SOURCES ET VERIFIE_PERE MARCHENT ... DESOLEE ...
 */

class Acceleration_QC_VEF_P1NC : public Terme_Source_Acceleration_VEF_Face, public Source_Fluide_Dilatable_Face<Acceleration_QC_VEF_P1NC>
{
  Declare_instanciable(Acceleration_QC_VEF_P1NC);
public:
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override;
  DoubleTab& ajouter(DoubleTab& ) const override;
  DoubleTab& ajouter_mere(DoubleTab& ) const;
};

class Perte_Charge_Circulaire_QC_VEF_P1NC : public Perte_Charge_Circulaire_VEF_P1NC, public Source_Fluide_Dilatable_Face<Perte_Charge_Circulaire_QC_VEF_P1NC>
{
  Declare_instanciable(Perte_Charge_Circulaire_QC_VEF_P1NC);
public:
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override;
  DoubleTab& ajouter(DoubleTab& ) const override;
  DoubleTab& ajouter_mere(DoubleTab& ) const;
};

class Perte_Charge_Anisotrope_QC_VEF_P1NC : public Perte_Charge_Anisotrope_VEF_P1NC, public Source_Fluide_Dilatable_Face<Perte_Charge_Anisotrope_QC_VEF_P1NC>
{
  Declare_instanciable(Perte_Charge_Anisotrope_QC_VEF_P1NC);
public:
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override;
  DoubleTab& ajouter(DoubleTab& ) const override;
  DoubleTab& ajouter_mere(DoubleTab& ) const;
};

class Perte_Charge_Directionnelle_QC_VEF_P1NC : public Perte_Charge_Directionnelle_VEF_P1NC, public Source_Fluide_Dilatable_Face<Perte_Charge_Directionnelle_QC_VEF_P1NC>
{
  Declare_instanciable(Perte_Charge_Directionnelle_QC_VEF_P1NC);
public:
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override;
  DoubleTab& ajouter(DoubleTab& ) const override;
  DoubleTab& ajouter_mere(DoubleTab& ) const;
};

class Perte_Charge_Isotrope_QC_VEF_P1NC : public Perte_Charge_Isotrope_VEF_P1NC, public Source_Fluide_Dilatable_Face<Perte_Charge_Isotrope_QC_VEF_P1NC>
{
  Declare_instanciable(Perte_Charge_Isotrope_QC_VEF_P1NC);
public:
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override;
  DoubleTab& ajouter(DoubleTab& ) const override;
  DoubleTab& ajouter_mere(DoubleTab& ) const;
};

class Perte_Charge_Reguliere_QC_VEF_P1NC : public Perte_Charge_Reguliere_VEF_P1NC, public Source_Fluide_Dilatable_Face<Perte_Charge_Reguliere_QC_VEF_P1NC>
{
  Declare_instanciable(Perte_Charge_Reguliere_QC_VEF_P1NC);
public:
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override;
  DoubleTab& ajouter(DoubleTab& ) const override;
  DoubleTab& ajouter_mere(DoubleTab& ) const;
};

class Perte_Charge_Singuliere_QC_VEF_P1NC : public Perte_Charge_Singuliere_VEF_Face, public Source_Fluide_Dilatable_Face<Perte_Charge_Singuliere_QC_VEF_P1NC>
{
  Declare_instanciable(Perte_Charge_Singuliere_QC_VEF_P1NC);
public:
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override;
  DoubleTab& ajouter(DoubleTab& ) const override;
  DoubleTab& ajouter_mere(DoubleTab& ) const;
};

class Source_qdm_QC_VEF_P1NC : public Terme_Source_Qdm_VEF_Face, public Source_Fluide_Dilatable_Face<Source_qdm_QC_VEF_P1NC>
{
  Declare_instanciable(Source_qdm_QC_VEF_P1NC);
public:
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override;
  DoubleTab& ajouter(DoubleTab& ) const override;
  DoubleTab& ajouter_mere(DoubleTab& ) const;
};

class Darcy_QC_VEF_P1NC : public Source_Darcy_VEF_Face, public Source_Fluide_Dilatable_Face<Darcy_QC_VEF_P1NC>
{
  Declare_instanciable(Darcy_QC_VEF_P1NC);
public:
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override;
  DoubleTab& ajouter(DoubleTab& ) const override;
  DoubleTab& ajouter_mere(DoubleTab& ) const;
};

class Forchheimer_QC_VEF_P1NC : public Source_Forchheimer_VEF_Face, public Source_Fluide_Dilatable_Face<Forchheimer_QC_VEF_P1NC>
{
  Declare_instanciable(Forchheimer_QC_VEF_P1NC);
public:
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override;
  DoubleTab& ajouter(DoubleTab& ) const override;
  DoubleTab& ajouter_mere(DoubleTab& ) const;
};

#endif /* Source_QC_VEF_P1NC_included */
