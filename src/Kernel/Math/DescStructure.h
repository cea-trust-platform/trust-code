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
// File:        DescStructure.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef DescStructure_included
#define DescStructure_included

#include <assert.h>
#include <VectEsp_Virt.h>
#include <VectEsp_Dist.h>



//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////
class DescStructure : public Objet_U
{
  Declare_instanciable(DescStructure);
public:
  Nom& identificateur();
  const Nom& identificateur() const;
  VectEsp_Virt& esp_virt();
  const VectEsp_Virt& esp_virt() const;
  VectEsp_Dist& esp_dist();
  const VectEsp_Dist& esp_dist() const;
  VECT(Descripteur)& descripteur();
  const VECT(Descripteur)& descripteur() const;

  void ajoute_espace_virtuel(int pe);
  // void ajoute_espace_virtuel(int pe, int sz);
  void ajoute_espace_virtuel(int pe, const VECT(Descripteur)& vdesc);
  void complete_espace_virtuel(int pe, int sz);
  // void ajoute_espace_distant(int pe, const ArrOfInt& v);

  void ajoute_descripteur(int deb, int nb, int stride=1);
  void ajoute_descripteur(const DescStructure&);


private:
  Nom identificateur_;
  VectEsp_Virt virt_data_;
  VectEsp_Dist dist_data_;
  VECT(Descripteur) desc_;
};
#endif
