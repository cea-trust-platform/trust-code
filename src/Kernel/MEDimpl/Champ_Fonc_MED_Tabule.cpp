/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Champ_Fonc_MED_Tabule.cpp
// Directory:   $TRUST_ROOT/src/Kernel/MEDimpl
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Fonc_MED_Tabule.h>

Implemente_instanciable( Champ_Fonc_MED_Tabule, "Champ_Fonc_MED_Tabule", Champ_Fonc_MED ) ;
// XD Champ_Fonc_MED_Tabule champ_fonc_med Champ_Fonc_MED_Tabule -1 not_set
Sortie& Champ_Fonc_MED_Tabule::printOn( Sortie& os ) const
{
  Champ_Fonc_MED::printOn( os );
  return os;
}

Entree& Champ_Fonc_MED_Tabule::readOn( Entree& is )
{
  temps1_=-1e9;
  temps2_=-1e9;
  temps_calc_=-2e9;
  Champ_Fonc_MED::readOn( is );
  return is;
}

void Champ_Fonc_MED_Tabule::mettre_a_jour(double le_temps)
{

  // temps1_ temps2_ tel que temps [temps1_ , temps2_[
  if (est_egal(le_temps,temps_calc_))
    return;

  if ((temps2_<-1e8)||(le_temps<=temps_calc_))
    {
      // initialisation   de -1e9 a T[0] on met la meme chose
      lire(temps_sauv_[0]);
      tab1_=valeurs();
      tab2_=valeurs();
      temps2_=temps_sauv_[0];
    }
  if (le_temps>=temps2_)
    {
      Cerr<<" il faut mettre a jour les tab"<<finl;
      int i,nbt=temps_sauv_.size_array();
      int trouve=0;
      for (i=0; i<nbt-1; i++)
        {
          if ((le_temps>=temps_sauv_[i])&&(le_temps<=temps_sauv_[i+1]))
            {
              trouve=1;
              temps1_=temps_sauv_[i];
              lire(temps1_);
              tab1_=valeurs();
              temps2_=temps_sauv_[i+1];
              lire(temps2_);
              tab2_=valeurs();
            }
        }
      if (trouve==0)
        {
          temps1_=temps_sauv_[nbt-1];
          assert(le_temps>=temps_sauv_[nbt-1]);
          lire(temps1_);
          tab1_=valeurs();
          tab2_=tab1_;
          temps2_=DMAXFLOAT;
        }
    }

  if ((le_temps>=temps1_) && (le_temps<temps2_))
    {
      valeurs()=tab2_;
      valeurs()-=tab1_;
      valeurs()*=(le_temps-temps1_)/(temps2_-temps1_);
      valeurs()+=tab1_;
      temps_calc_=le_temps;
      //  Cerr<<temps_calc_<<" YYYYYY "<<valeurs()[0]<<" "<< tab2_[0]<< " "<<tab1_[0]<<finl;
    }
  else
    {
      Cerr<<"internal error"<<finl;
      exit();
    }

}


