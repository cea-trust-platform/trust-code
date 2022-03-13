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
// File:        Source_QC_Chaleur.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Quasi_Compressible/Sources
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#include <Fluide_Quasi_Compressible.h>
#include <Source_QC_Chaleur.h>
#include <Schema_Temps_base.h>
#include <Equation_base.h>

Implemente_base(Source_QC_Chaleur,"Source_QC_Chaleur",Source_Chaleur_Fluide_Dilatable_base);

Sortie& Source_QC_Chaleur::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

Entree& Source_QC_Chaleur::readOn(Entree& is)
{
  return is;
}


DoubleTab& Source_QC_Chaleur::ajouter(DoubleTab& resu) const
{
  if(has_interface_blocs())
    {
      ajouter_blocs({}, resu);
      return resu;
    }
  return ajouter_(resu);
}


// Description:
//    Ajoute les termes sources
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleTab& Source_QC_Chaleur::ajouter_(DoubleTab& resu) const
{
  double dt_= mon_equation->schema_temps().temps_courant() - mon_equation->schema_temps().temps_precedent();

  if (dt_<=0) return resu; // On calcul pas ce terme source si dt<=0

  /*
   * The source term corresponds to :
   * d P_tot / d t = del P / del t + u.grad(P_tot)
   * Here grad(P_tot) = 0 (uniform in space)
   */

  int i, nsom = resu.dimension(0);
  const Fluide_Quasi_Compressible& FQC = ref_cast(Fluide_Quasi_Compressible,le_fluide.valeur());
  double Pth = FQC.pression_th(), Pthn = FQC.pression_thn();
  double dpth = ( Pth - Pthn ) / dt_;
  for (i=0 ; i<nsom ; i++) resu(i) += dpth * volumes(i) * porosites(i);

  return resu;
}
