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
// File:        Boundary_field_uniform_keps_from_ud.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#include <Boundary_field_uniform_keps_from_ud.h>
#include <Param.h>

Implemente_instanciable(Boundary_field_uniform_keps_from_ud,"Boundary_field_uniform_keps_from_ud",Champ_front_uniforme);



// Description:
//    Imprime le champ sur flot de sortie.
//    Imprime la taille du champ et la valeur (constante) sur
//    la frontiere.
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Boundary_field_uniform_keps_from_ud::printOn(Sortie& os) const
{
  const DoubleTab& tab=valeurs();
  os << tab.size() << " ";
  for(int i=0; i<tab.size(); i++)
    os << tab(0,i);
  return os;
}


// Description:
//    Lit le champ a partir d'un flot d'entree.
//    Format:
//      Boundary_field_uniform_keps_from_ud nb_compo u_value d_value
//      we expect that nb_compo=2 (one for u and one for d)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree& is
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: uniforme field K and Eps are deduced from
//                the flow rate u and the hydraulic diameter d
Entree& Boundary_field_uniform_keps_from_ud::readOn(Entree& is)
{
  double u=0,d=0;
  DoubleTab& tab=les_valeurs->valeurs();

  Param param(que_suis_je());
  param.ajouter("U", &u,Param::REQUIRED);
  param.ajouter("D", &d,Param::REQUIRED);
  param.lire_avec_accolades(is);

  int keps_size=2;// k and eps
  tab.resize(1,keps_size);
  fixer_nb_comp(keps_size);
  // is >> u; //initial velocity magnitude
  // is >> d; //hydraulic diameter

  //Now compute associated K and Eps by appling formula given by :
  //http://en.wikipedia.org/wiki/Turbulence_kinetic_energy
  //http://www.cfd-online.com/Wiki/Turbulence_length_
  //http://support.esi-cfd.com/esi-users/turb_parameters/

  //Hypothesis:
  //Cmu=0.09 : k-eps parameter
  //l=0.038 Dh where Dh is the hydraulic diameter
  //Note that Cmu^(3/4)=0.1643
  double I=0.05;  //: initial turbulence intensity

  //in 3D we have
  //k = 3/2*(u*I)^2
  //in 2D we have
  //k = (u*I)^2
  //with u = initial velocity magnitude
  //eps = Cmu^(3/4) * k^(3/2) * l^(-1)
  //where l is the turbulence length scale which can be expressed as 0.038 Dh
  //Dh is the hydraulic diameter
  double l=0.038*d;
  double k = ( dimension == 2 ? 1 : 3./2. )*(u*I)*(u*I);
  double eps = 0.1643*pow(k,1.5)/l;
  tab(0,0)=k;
  tab(0,1)=eps;
  return is;
}
