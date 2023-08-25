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

#include <Source_QC_VEF_P1NC.h>

Implemente_instanciable(Acceleration_QC_VEF_P1NC,"Acceleration_QC_VEF_P1NC",Terme_Source_Acceleration_VEF_Face);
Sortie& Acceleration_QC_VEF_P1NC::printOn(Sortie& os) const { return Terme_Source_Acceleration_VEF_Face::printOn(os); }
Entree& Acceleration_QC_VEF_P1NC::readOn(Entree& is) { return Terme_Source_Acceleration_VEF_Face::readOn(is); }
DoubleTab& Acceleration_QC_VEF_P1NC::ajouter_mere(DoubleTab& resu) const { return Terme_Source_Acceleration_VEF_Face::ajouter(resu); }
DoubleTab& Acceleration_QC_VEF_P1NC::ajouter(DoubleTab& resu ) const { return ajouter_impl(resu); }
void Acceleration_QC_VEF_P1NC::contribuer_a_avec(const DoubleTab& resu, Matrice_Morse& mat) const { contribuer_a_avec_impl(resu,mat); }

Implemente_instanciable(Perte_Charge_Circulaire_QC_VEF_P1NC,"Perte_Charge_Circulaire_QC_VEF_P1NC",Perte_Charge_Circulaire_VEF_P1NC);
Sortie& Perte_Charge_Circulaire_QC_VEF_P1NC::printOn(Sortie& os) const { return Perte_Charge_Circulaire_VEF_P1NC::printOn(os); }
Entree& Perte_Charge_Circulaire_QC_VEF_P1NC::readOn(Entree& is) {  return Perte_Charge_Circulaire_VEF_P1NC::readOn(is); }
DoubleTab& Perte_Charge_Circulaire_QC_VEF_P1NC::ajouter_mere(DoubleTab& resu) const { return Perte_Charge_Circulaire_VEF_P1NC::ajouter(resu); }
DoubleTab& Perte_Charge_Circulaire_QC_VEF_P1NC::ajouter(DoubleTab& resu ) const { return ajouter_impl(resu); }
void Perte_Charge_Circulaire_QC_VEF_P1NC::contribuer_a_avec(const DoubleTab& resu, Matrice_Morse& mat) const { contribuer_a_avec_impl(resu,mat); }

Implemente_instanciable(Perte_Charge_Anisotrope_QC_VEF_P1NC,"Perte_Charge_Anisotrope_QC_VEF_P1NC",Perte_Charge_Anisotrope_VEF_P1NC);
Sortie& Perte_Charge_Anisotrope_QC_VEF_P1NC::printOn(Sortie& os) const { return Perte_Charge_Anisotrope_VEF_P1NC::printOn(os); }
Entree& Perte_Charge_Anisotrope_QC_VEF_P1NC::readOn(Entree& is) { return Perte_Charge_Anisotrope_VEF_P1NC::readOn(is); }
DoubleTab& Perte_Charge_Anisotrope_QC_VEF_P1NC::ajouter_mere(DoubleTab& resu) const { return Perte_Charge_Anisotrope_VEF_P1NC::ajouter(resu); }
DoubleTab& Perte_Charge_Anisotrope_QC_VEF_P1NC::ajouter(DoubleTab& resu ) const { return ajouter_impl(resu); }
void Perte_Charge_Anisotrope_QC_VEF_P1NC::contribuer_a_avec(const DoubleTab& resu, Matrice_Morse& mat) const { contribuer_a_avec_impl(resu,mat); }

Implemente_instanciable(Perte_Charge_Directionnelle_QC_VEF_P1NC,"Perte_Charge_Directionnelle_QC_VEF_P1NC",Perte_Charge_Directionnelle_VEF_P1NC);
Sortie& Perte_Charge_Directionnelle_QC_VEF_P1NC::printOn(Sortie& os) const { return Perte_Charge_Directionnelle_VEF_P1NC::printOn(os); }
Entree& Perte_Charge_Directionnelle_QC_VEF_P1NC::readOn(Entree& is) { return Perte_Charge_Directionnelle_VEF_P1NC::readOn(is); }
DoubleTab& Perte_Charge_Directionnelle_QC_VEF_P1NC::ajouter_mere(DoubleTab& resu) const { return Perte_Charge_Directionnelle_VEF_P1NC::ajouter(resu); }
DoubleTab& Perte_Charge_Directionnelle_QC_VEF_P1NC::ajouter(DoubleTab& resu ) const { return ajouter_impl(resu); }
void Perte_Charge_Directionnelle_QC_VEF_P1NC::contribuer_a_avec(const DoubleTab& resu, Matrice_Morse& mat) const { contribuer_a_avec_impl(resu,mat); }

Implemente_instanciable(Perte_Charge_Isotrope_QC_VEF_P1NC,"Perte_Charge_Isotrope_QC_VEF_P1NC",Perte_Charge_Isotrope_VEF_P1NC);
Sortie& Perte_Charge_Isotrope_QC_VEF_P1NC::printOn(Sortie& os) const { return Perte_Charge_Isotrope_VEF_P1NC::printOn(os); }
Entree& Perte_Charge_Isotrope_QC_VEF_P1NC::readOn(Entree& is) { return Perte_Charge_Isotrope_VEF_P1NC::readOn(is); }
DoubleTab& Perte_Charge_Isotrope_QC_VEF_P1NC::ajouter_mere(DoubleTab& resu) const { return Perte_Charge_Isotrope_VEF_P1NC::ajouter(resu); }
DoubleTab& Perte_Charge_Isotrope_QC_VEF_P1NC::ajouter(DoubleTab& resu ) const { return ajouter_impl(resu); }
void Perte_Charge_Isotrope_QC_VEF_P1NC::contribuer_a_avec(const DoubleTab& resu, Matrice_Morse& mat) const { contribuer_a_avec_impl(resu,mat); }

Implemente_instanciable(Perte_Charge_Reguliere_QC_VEF_P1NC,"Perte_Charge_Reguliere_QC_VEF_P1NC",Perte_Charge_Reguliere_VEF_P1NC);
Sortie& Perte_Charge_Reguliere_QC_VEF_P1NC::printOn(Sortie& os) const { return Perte_Charge_Reguliere_VEF_P1NC::printOn(os); }
Entree& Perte_Charge_Reguliere_QC_VEF_P1NC::readOn(Entree& is) { return Perte_Charge_Reguliere_VEF_P1NC::readOn(is); }
DoubleTab& Perte_Charge_Reguliere_QC_VEF_P1NC::ajouter_mere(DoubleTab& resu) const { return Perte_Charge_Reguliere_VEF_P1NC::ajouter(resu); }
DoubleTab& Perte_Charge_Reguliere_QC_VEF_P1NC::ajouter(DoubleTab& resu ) const { return ajouter_impl(resu); }
void Perte_Charge_Reguliere_QC_VEF_P1NC::contribuer_a_avec(const DoubleTab& resu, Matrice_Morse& mat) const { contribuer_a_avec_impl(resu,mat); }

Implemente_instanciable(Perte_Charge_Singuliere_QC_VEF_P1NC,"Perte_Charge_Singuliere_QC_VEF_P1NC",Perte_Charge_Singuliere_VEF_Face);
Sortie& Perte_Charge_Singuliere_QC_VEF_P1NC::printOn(Sortie& os) const { return Perte_Charge_Singuliere_VEF_Face::printOn(os); }
Entree& Perte_Charge_Singuliere_QC_VEF_P1NC::readOn(Entree& is) { return Perte_Charge_Singuliere_VEF_Face::readOn(is); }
DoubleTab& Perte_Charge_Singuliere_QC_VEF_P1NC::ajouter_mere(DoubleTab& resu) const { return Perte_Charge_Singuliere_VEF_Face::ajouter(resu); }
DoubleTab& Perte_Charge_Singuliere_QC_VEF_P1NC::ajouter(DoubleTab& resu ) const { return ajouter_impl(resu); }
void Perte_Charge_Singuliere_QC_VEF_P1NC::contribuer_a_avec(const DoubleTab& resu, Matrice_Morse& mat) const { contribuer_a_avec_impl(resu,mat); }

Implemente_instanciable(Source_qdm_QC_VEF_P1NC,"Source_qdm_QC_VEF_P1NC",Terme_Source_Qdm_VEF_Face);
Sortie& Source_qdm_QC_VEF_P1NC::printOn(Sortie& os) const { return Terme_Source_Qdm_VEF_Face::printOn(os); }
Entree& Source_qdm_QC_VEF_P1NC::readOn(Entree& is) { return Terme_Source_Qdm_VEF_Face::readOn(is); }
DoubleTab& Source_qdm_QC_VEF_P1NC::ajouter_mere(DoubleTab& resu) const { return Terme_Source_Qdm_VEF_Face::ajouter(resu); }
DoubleTab& Source_qdm_QC_VEF_P1NC::ajouter(DoubleTab& resu ) const { return ajouter_impl(resu); }
void Source_qdm_QC_VEF_P1NC::contribuer_a_avec(const DoubleTab& resu, Matrice_Morse& mat) const { contribuer_a_avec_impl(resu,mat); }

Implemente_instanciable(Darcy_QC_VEF_P1NC,"Darcy_QC_VEF_P1NC",Source_Darcy_VEF_Face);
Sortie& Darcy_QC_VEF_P1NC::printOn(Sortie& os) const { return Source_Darcy_VEF_Face::printOn(os); }
Entree& Darcy_QC_VEF_P1NC::readOn(Entree& is) { return Source_Darcy_VEF_Face::readOn(is); }
DoubleTab& Darcy_QC_VEF_P1NC::ajouter_mere(DoubleTab& resu) const { return Source_Darcy_VEF_Face::ajouter(resu); }
DoubleTab& Darcy_QC_VEF_P1NC::ajouter(DoubleTab& resu ) const { return ajouter_impl(resu); }
void Darcy_QC_VEF_P1NC::contribuer_a_avec(const DoubleTab& resu, Matrice_Morse& mat) const { contribuer_a_avec_impl(resu,mat); }

Implemente_instanciable(Forchheimer_QC_VEF_P1NC,"Forchheimer_QC_VEF_P1NC",Source_Forchheimer_VEF_Face);
Sortie& Forchheimer_QC_VEF_P1NC::printOn(Sortie& os) const { return Source_Forchheimer_VEF_Face::printOn(os); }
Entree& Forchheimer_QC_VEF_P1NC::readOn(Entree& is) { return Source_Forchheimer_VEF_Face::readOn(is); }
DoubleTab& Forchheimer_QC_VEF_P1NC::ajouter_mere(DoubleTab& resu) const { return Source_Forchheimer_VEF_Face::ajouter(resu); }
DoubleTab& Forchheimer_QC_VEF_P1NC::ajouter(DoubleTab& resu ) const { return ajouter_impl(resu); }
void Forchheimer_QC_VEF_P1NC::contribuer_a_avec(const DoubleTab& resu, Matrice_Morse& mat) const { contribuer_a_avec_impl(resu,mat); }
