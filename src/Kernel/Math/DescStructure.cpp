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
// File:        DescStructure.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <DescStructure.h>

Implemente_instanciable(DescStructure,"DescStructure",Objet_U);


// Description:
//    Lecture d'un descripteur de structure
//    Un descripteur de structure est constitue d'un
//    identificateur, d'un vecteur d'espaces virtuels,
//    d'un vecteur d'espaces distants et d'un descripteur de
//    structure interne
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& DescStructure::readOn(Entree& is)
{
  //  is >> identificateur_;
  is >> virt_data_;
  is >> dist_data_;
  {
    int i;
    is >> i;
    desc_.dimensionner_force(i);
    while(i--)
      is>>desc_[i];
    //    assert(desc_.size()>=0); if(desc_.size()) assert(desc_!=0);
  }
  return is;
  //
  //  is >> desc_;
  //  return is;
}


// Description:
//    Ecriture d'un descripteur de structure
//    Un descripteur de structure est constitue d'un
//    identificateur, d'un vecteur d'espaces virtuels,
//    d'un vecteur d'espaces distants et d'un descripteur de
//    structure interne
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& DescStructure::printOn(Sortie& os) const
{
  //  os <<  identificateur_<< finl;
  os << virt_data_ << finl;
  os << dist_data_ << finl;
  os << desc_ << finl;
  return os;
}

// Description:
// Precondition:
//    Renvoi le vecteur des espaces virtuels
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
VectEsp_Virt& DescStructure::esp_virt()
{
  return virt_data_;
}
const VectEsp_Virt& DescStructure::esp_virt() const
{
  return virt_data_;
}


// Description:
//    Renvoi le vecteur des espaces distants
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
VectEsp_Dist& DescStructure::esp_dist()
{
  return dist_data_;
}
const VectEsp_Dist& DescStructure::esp_dist() const
{
  return dist_data_;
}


// Description:
//    Renvoie le descripteur de structure
// Precondition:
//    Le descripteur doit etre valide (desc_ doit contenir au moins un element)
VECT(Descripteur)& DescStructure::descripteur()
{
  // Si ca plante ici, c'est qu'on a oublie d'initialiser le descripteur
  // dans DoubleVect ou IntVect (utiliser ajoute_descripteur())
  assert(desc_.size() > 0);
  return desc_;
}

// Description:
//    Renvoie le descripteur de structure (version const)
// Precondition:
//    Le descripteur doit etre valide (desc_ doit contenir au moins un element)
const VECT(Descripteur)& DescStructure::descripteur() const
{
  // Si ca plante ici, c'est qu'on a oublie d'initialiser le descripteur
  // dans DoubleVect ou IntVect (utiliser ajoute_descripteur())
  assert(desc_.size() > 0);
  return desc_;
}

// Description:
//    Renvoi le nom de la structure
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Nom& DescStructure::identificateur()
{
  return identificateur_;
}
const Nom& DescStructure::identificateur() const
{
  return identificateur_;
}

// Description: ajoute un espace virtuel vide pour le PE pe
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void DescStructure::ajoute_espace_virtuel(int pe)
{
  Esp_Virt to_add;
  to_add.affecte_PE_voisin(pe);
  virt_data_.add(to_add);
}

// Description:
//     Ajout d'un espace virtuel associe au processeur pe
//     dont les valeurs sont contenues dans v.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
// void DescStructure::ajoute_espace_virtuel(int pe, int sz)
// {
//   Esp_Virt to_add;

//   to_add.affecte_PE_voisin(pe);
//   to_add.affecte_deb(sz);
//   to_add.affecte_nb(sz);
//   virt_data_.add(to_add);
// }

// Description:
//     Ajout d'un espace virtuel associe au processeur pe
//     dont les valeurs sont contenues dans v.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void DescStructure::complete_espace_virtuel(int pe, int sz)
{
  // Pour l'instant on suppose le tableau associe est monostructure
  assert(desc_.size()==1);
  int ok=0;
  int sz_esp_virt = virt_data_.size();
  for(int i=0; i<sz_esp_virt; i++)
    {
      if (virt_data_[i].num_PE_voisin()==pe)
        {
          ok=1;
          virt_data_[i].affecte_nb(virt_data_[i].nb()+sz);
        }
      if(ok)
        virt_data_[i].affecte_deb(virt_data_[i].deb()+sz);
    }
  if(!ok)
    {
      // Il n'y a pas d'espace virtuel associe au processeur pe
      Esp_Virt to_add;
      VECT(Descripteur) desc(1);
      //      Process::Journal()<<"%%% Esp_Virt a ajouter : "<< to_add<<finl;

      if(sz_esp_virt==0)
        desc[0].affecte_deb(desc_[0].nb());
      else
        {
          Esp_Virt& last_desc=virt_data_[sz_esp_virt-1];
          desc[0].affecte_deb(last_desc.deb()+last_desc.nb());
        }
      desc[0].affecte_nb(sz);
      desc[0].affecte_stride(1);
      to_add.affecte_PE_voisin(pe);
      to_add.affecte_descripteur(desc);
      //Process::Journal()<<"%%% Esp_Virt a ajouter : "<< to_add<<finl;
      virt_data_.append(to_add);
    }
}
// Description:
//     Ajout d'un espace virtuel associe au processeur pe
//     dont la structrure est decrite par vdesc
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void DescStructure::ajoute_espace_virtuel(int pe, const VECT(Descripteur)& vdesc)
{

  Esp_Virt to_add;
  to_add.affecte_PE_voisin(pe);
  to_add.affecte_descripteur(vdesc);
  virt_data_.append(to_add);

}

// Description: ajoute un descripteur simple
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void DescStructure::ajoute_descripteur(int deb, int nb, int stride)
{
  Descripteur desc;
  desc.affecte_deb(deb);
  desc.affecte_nb(nb);
  desc.affecte_stride(stride);
  desc_.add(desc);
}

// Description: ajoute un descripteur
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void DescStructure::ajoute_descripteur(const DescStructure& desc)
{
  virt_data_.copy(desc.esp_virt());
  dist_data_.copy(desc.esp_dist());
  desc_ = desc.descripteur();
  identificateur_ = desc.identificateur_;
}
