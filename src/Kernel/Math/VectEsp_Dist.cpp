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
// File:        VectEsp_Dist.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////
#include <VectEsp_Dist.h>

Implemente_vect(Esp_Dist);
Implemente_instanciable(VectEsp_Dist,"VectofEsp_Dist",VECT(Esp_Dist));

// Description :
//// printOn
//
Sortie& VectEsp_Dist::printOn(Sortie& s ) const
{
  return VECT(Esp_Dist)::printOn(s) ;
}

// Description :
//// readOn
//
Entree& VectEsp_Dist::readOn(Entree& s)
{
  /*
    int i;
    s >> i;
    dimensionner_force(i);
    int ii;
    while(i--)
    s>>data[ii];
    assert(sz>=0); if(sz) assert(data!=0);
    return s;
  */
  return VECT(Esp_Dist)::lit(s);
  //    return VECT(Esp_Dist)::readOn(s) ;
}

// Description :
//// copy
//
void VectEsp_Dist::copy(const VectEsp_Dist& ved)
{
  int i,j;
  int ved_sz=ved.size();
  (*this).dimensionner_force(ved_sz);
  for(i=0; i<ved_sz; i++)
    {
      const Esp_Dist& ved_i = ved[i];
      Esp_Dist& this_i = (*this)[i];
      int ved_i_sz = ved_i.size_array();
      this_i.resize_array(ved_i_sz);
      for(j=0; j<ved_i_sz; j++)
        this_i[j]=ved_i[j];
      this_i.affecte_PE_voisin(ved_i.num_PE_voisin());
      const VECT(Descripteur)& ved_i_vdesc = ved_i.desc_ed();
      VECT(Descripteur)& this_i_vdesc = this_i.desc_ed();
      int ved_i_vdesc_sz = ved_i_vdesc.size();
      this_i_vdesc.dimensionner_force(ved_i_vdesc_sz);
      for(j=0; j<ved_i_vdesc_sz; j++)
        {
          this_i_vdesc[j].affecte_deb(ved_i_vdesc[j].deb());
          this_i_vdesc[j].affecte_nb(ved_i_vdesc[j].nb());
          this_i_vdesc[j].affecte_stride(ved_i_vdesc[j].stride());
        }
    }
}

// Description :
//// copy
//
void VectEsp_Dist::append(const Esp_Dist& ed)
{
  int i,j;
  VectEsp_Dist ved;
  ved.copy(*this);
  int ved_sz=ved.size();
  (*this).dimensionner_force(ved_sz+1);
  for(i=0; i<ved_sz; i++)
    {
      Esp_Dist& ved_i = ved[i];
      Esp_Dist& this_i = (*this)[i];
      int ved_i_sz = ved_i.size_array();
      this_i.resize_array(ved_i_sz);
      for(j=0; j<ved_i_sz; j++)
        this_i[j]=ved_i[j];
      this_i.affecte_PE_voisin(ved_i.num_PE_voisin());
      VECT(Descripteur)& ved_i_vdesc = ved_i.desc_ed();
      VECT(Descripteur)& this_i_vdesc = this_i.desc_ed();
      int ved_i_vdesc_sz = ved_i_vdesc.size();
      this_i_vdesc.dimensionner_force(ved_i_vdesc_sz);
      for(j=0; j<ved_i_vdesc_sz; j++)
        {
          this_i_vdesc[j].affecte_deb(ved_i_vdesc[j].deb());
          this_i_vdesc[j].affecte_nb(ved_i_vdesc[j].nb());
          this_i_vdesc[j].affecte_stride(ved_i_vdesc[j].stride());
        }
    }
  Esp_Dist& this_i = (*this)[ved_sz];
  int ed_sz = ed.size_array();
  this_i.resize_array(ed_sz);
  for(j=0; j<ed_sz; j++)
    this_i[j]=ed[j];
  this_i.affecte_PE_voisin(ed.num_PE_voisin());
  const VECT(Descripteur)& ed_vdesc = ed.desc_ed();
  VECT(Descripteur)& this_i_vdesc = this_i.desc_ed();
  int ed_vdesc_sz = ed_vdesc.size();
  this_i_vdesc.dimensionner_force(ed_vdesc_sz);
  for(j=0; j<ed_vdesc_sz; j++)
    {
      this_i_vdesc[j].affecte_deb(ed_vdesc[j].deb());
      this_i_vdesc[j].affecte_nb(ed_vdesc[j].nb());
      this_i_vdesc[j].affecte_stride(ed_vdesc[j].stride());
    }
}
