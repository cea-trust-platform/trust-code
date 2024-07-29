/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <SupprimeBord.h>
#include <EFichier.h>
Implemente_instanciable_32_64(SupprimeBord_32_64,"Supprime_Bord",Interprete_geometrique_base_32_64<_T_>);
// XD supprime_bord interprete supprime_bord -1 Keyword to remove boundaries (named Boundary_name1 Boundary_name2 ) of the domain named domain_name.
// XD   attr domaine ref_domaine domain 0 Name of domain
// XD   attr bords list_nom bords 0 { Boundary_name1 Boundaray_name2 }

template <typename _SIZE_>
Sortie& SupprimeBord_32_64<_SIZE_>::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

template <typename _SIZE_>
Entree& SupprimeBord_32_64<_SIZE_>::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

template <typename _SIZE_>
Entree& SupprimeBord_32_64<_SIZE_>::interpreter_(Entree& is)
{
  //Nom nom_dom;
  LIST(Nom) nlistbord;
  this->associer_domaine(is);
  Nom mot;
  is >>mot;
  if (mot!="{")
    {
      Cerr<< "we expected { and not "<<mot<<finl;
      Process::exit();
    }
  is >>mot;
  while (mot!="}")
    {
      nlistbord.add(mot);
      is>>mot;
    }
  Domaine_t& dom=this->domaine();
  Cout<<"HERE  "<<nlistbord<<finl;

  for (int b=0; b<nlistbord.size(); b++)
    {

      {
        // la recup des bords et des raccords est dans la boucle
        // pour pouvoir supprimer ...
        Bords_t& listbord=dom.faces_bord();
        Raccords_t& listrac=dom.faces_raccord();
        const Nom& nombord=nlistbord[b];
        int num_b=dom.rang_frontiere(nombord);
        if (num_b<listbord.size())
          listbord.suppr(dom.bord(nombord));
        else
          listrac.suppr(dom.raccord(nombord));
      }
    }
  return is;
}


template class SupprimeBord_32_64<int>;
#if INT_is_64_ == 2
template class SupprimeBord_32_64<trustIdType>;
#endif


