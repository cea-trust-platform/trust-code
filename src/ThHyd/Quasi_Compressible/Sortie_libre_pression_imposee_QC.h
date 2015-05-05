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
// File:        Sortie_libre_pression_imposee_QC.h
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Sortie_libre_pression_imposee_QC_included
#define Sortie_libre_pression_imposee_QC_included


#include <Neumann_sortie_libre.h>



//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Sortie_libre_pression_imposee
//    Cette classe derive de Neumann_sortie_libre
//    Elle represente une frontiere ouverte avec condition de pression imposee.
//    L'objet de type Champ_bord le_champ_bord contient la pression et la
//    fonction flux_impose() renvoie les valeurs de cette pression.
//    champ_ext contient une valeur de la vitesse du fluide a l'exterieur
//    accessible par la methode val_ext()
// .SECTION voir aussi
//     Neumann_sortie_libre
//////////////////////////////////////////////////////////////////////////////
class Sortie_libre_pression_imposee_QC : public Neumann_sortie_libre
{

  Declare_instanciable(Sortie_libre_pression_imposee_QC);

public :

  int compatible_avec_eqn(const Equation_base&) const;
  double flux_impose(int i) const;
  double flux_impose(int i,int j) const;
  void completer();

  double get_Pth()
  {
    return Pthn;  // retourne la pression thermohydraulique
  }
  void set_Pth(double P)
  {
    Pthn = P;  // fixe la pression  thermohydraulique
  }


protected:
  double Pthn; // pression thermohydraulique au temps n
  double d_rho;
};



#endif
