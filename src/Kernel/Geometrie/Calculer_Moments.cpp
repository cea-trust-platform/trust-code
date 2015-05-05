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
// File:        Calculer_Moments.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <Calculer_Moments.h>
#include <Domaine.h>


Implemente_instanciable(Calculer_Moments,"Calculer_Moments",Interprete_geometrique_base);

// printOn et readOn

Sortie& Calculer_Moments::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Calculer_Moments::readOn(Entree& is )
{
  return is;
}

Entree& Calculer_Moments::interpreter_(Entree& is)
{
  Motcle nom_var;
  associer_domaine(is);
  c.resize_array(dimension);
  Domaine& dom=domaine();
  is >> nom_var;
  Cerr << nom_var << finl;
  dom.zone(0).Moments_a_imprimer()=1;
  if(nom_var=="calcul")
    {
      Cerr << "Calculation of center of gravity " << nom_var<< finl;
      dom.zone(0).calculer_mon_centre_de_gravite(c);
    }
  else if (nom_var=="centre_de_gravite")
    {
      Cerr << "Center of gravity imposed, of dimension " << dimension << finl;
      is >> c[0];
      is >> c[1];
      if (dimension==3)
        is >> c[2];
      dom.zone(0).exporter_mon_centre_de_gravite(c);
    }
  else
    {
      Cerr << nom_var << " is not recognized." << finl;
      exit();
    }
  return is;
}
