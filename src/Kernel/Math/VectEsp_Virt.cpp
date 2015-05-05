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
// File:        VectEsp_Virt.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////
#include <VectEsp_Virt.h>

Implemente_vect(Esp_Virt);
Implemente_instanciable(VectEsp_Virt,"VectofEsp_Virt",VECT(Esp_Virt));

// Description :
//// printOn
//
Sortie& VectEsp_Virt::printOn(Sortie& s ) const
{
  return VECT(Esp_Virt)::printOn(s) ;
}

// Description :
//// readOn
//
Entree& VectEsp_Virt::readOn(Entree& s)
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
  return VECT(Esp_Virt)::lit(s);
  //  return VECT(Esp_Virt)::readOn(s) ;
}

// Description :
//// copy
//
void VectEsp_Virt::copy(const VectEsp_Virt& vev)
{
  int i,j;
  int vev_sz=vev.size();
  (*this).dimensionner_force(vev_sz);
  for(i=0; i<vev_sz; i++)
    {
      const Esp_Virt& vev_i = vev[i];
      Esp_Virt& this_i = (*this)[i];
      this_i.affecte_PE_voisin(vev_i.num_PE_voisin());
      const VECT(Descripteur)& vev_i_vdesc = vev_i.desc_ev();
      VECT(Descripteur)& this_i_vdesc = this_i.desc_ev();
      int vev_i_vdesc_sz = vev_i_vdesc.size();
      this_i_vdesc.dimensionner_force(vev_i_vdesc_sz);
      for(j=0; j<vev_i_vdesc_sz; j++)
        {
          this_i_vdesc[j].affecte_deb(vev_i_vdesc[j].deb());
          this_i_vdesc[j].affecte_nb(vev_i_vdesc[j].nb());
          this_i_vdesc[j].affecte_stride(vev_i_vdesc[j].stride());
        }
    }
}

// Description :
//// copy
//
void VectEsp_Virt::append(const Esp_Virt& ev)
{
  int i,j;
  VectEsp_Virt vev;
  vev.copy(*this);
  int vev_sz=vev.size();
  (*this).dimensionner_force(vev_sz+1);
  for(i=0; i<vev_sz; i++)
    {
      Esp_Virt& vev_i = vev[i];
      Esp_Virt& this_i = (*this)[i];
      this_i.affecte_PE_voisin(vev_i.num_PE_voisin());
      VECT(Descripteur)& vev_i_vdesc = vev_i.desc_ev();
      VECT(Descripteur)& this_i_vdesc = this_i.desc_ev();
      int vev_i_vdesc_sz = vev_i_vdesc.size();
      this_i_vdesc.dimensionner_force(vev_i_vdesc_sz);
      for(j=0; j<vev_i_vdesc_sz; j++)
        {
          this_i_vdesc[j].affecte_deb(vev_i_vdesc[j].deb());
          this_i_vdesc[j].affecte_nb(vev_i_vdesc[j].nb());
          this_i_vdesc[j].affecte_stride(vev_i_vdesc[j].stride());
        }
    }
  Esp_Virt& this_i = (*this)[vev_sz];
  this_i.affecte_PE_voisin(ev.num_PE_voisin());
  const VECT(Descripteur)& ev_vdesc = ev.desc_ev();
  VECT(Descripteur)& this_i_vdesc = this_i.desc_ev();
  int ev_vdesc_sz = ev_vdesc.size();
  this_i_vdesc.dimensionner_force(ev_vdesc_sz);
  for(j=0; j<ev_vdesc_sz; j++)
    {
      this_i_vdesc[j].affecte_deb(ev_vdesc[j].deb());
      this_i_vdesc[j].affecte_nb(ev_vdesc[j].nb());
      this_i_vdesc[j].affecte_stride(ev_vdesc[j].stride());
    }
}
