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
// File:        Deriv_.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#include <Deriv_.h>
#include <Nom.h>
#include <Synonyme_info.h>

extern const Nom& deriv_vide();

Implemente_base_sans_destructeur(Deriv_,"Deriv_",O_U_Ptr);

Deriv_::~Deriv_()
{
  // Rien, la destruction est faite au-dessus
  // (pas de pointeur disponible)
}

Sortie& Deriv_::printOn(Sortie& os)  const
{
  const Objet_U * objet = get_O_U_Ptr();
  if (objet)
    {
      os << objet->le_type() << finl;
      os << (*objet);
    }
  else
    {
      os << "vide" << finl;
    }
  return os;
}

Entree& Deriv_::readOn(Entree& is)
{
  // Efface l'objet existant :
  detach();
  // static pour le pas creer un Objet_U a chaque fois.
  static Nom nom_type;
  is >> nom_type;
  Objet_U * objet = (Objet_U*) 0;
  if (nom_type != "vide")
    {
      objet = typer(nom_type);
      if (! objet)
        {
          assert(0);
          exit();
        }
    }
  // Appel a la methode virtuelle : met a jour le pointeur_
  set_O_U_Ptr(objet);
  // Lecture de l'objet derive
  if (objet)
    is >> (*objet);
  return is;
}

void Deriv_::detach()
{
  Objet_U * ptr = get_O_U_Ptr();
  if (ptr)
    delete ptr;
  set_O_U_Ptr((Objet_U *) 0);
}

// Description:
//  Essaie de creer une instance du type "type".
//   si type n'est pas un type ou type n'est pas instanciable=>arret
//   si type n'est pas un sous-type du type du pointeur=>retour 0
//   si ok, renvoie l'adresse de l'objet cree.

Objet_U * Deriv_::typer(const char* type)
{
  // Type_info du type demande
  const Type_info * type_info = Type_info::type_info_from_name(type);
  // Type de base du DERIV:
  const Type_info& type_base = get_info_ptr();
  if ( get_O_U_Ptr())
    {
      //cerr<<"on avait deja typer ???"<<endl;
      detach();
    }
  Objet_U * instance = 0;

  if (type_info == 0)
    {
      const Synonyme_info* syn_info= Synonyme_info::synonyme_info_from_name(type);
      if (syn_info!=0)
        {
          return typer(syn_info->org_name_());
        }
      else
        {
          Cerr << "Error in Deriv_::typer_(const char* const type)\n";
          Cerr << type << " is not a type." << finl;
          Cerr << "Type required : derived from " << type_base.name() << finl;
          Cerr << finl;
          Cerr << type << " is not a recognized keyword." << finl;
          Cerr << "Check your data set." << finl;
          exit();
        }
    }
  if (! type_info->instanciable())
    {
      Cerr << "Error in Deriv_::typer_(const char* const type)\n ";
      Cerr << type << " is not instanciable." << finl;
      assert(0);
      exit();
    }
  if (! type_info->has_base(&type_base))
    {
      Cerr << "Error in Deriv_::typer_(const char* const type)\n ";
      Cerr << type << " is not a subtype of " << type_base.name() << finl;
    }
  else
    {
      // Cree une instance du type decrit dans type_info
      instance = type_info->instance();
    }
  set_O_U_Ptr(instance);
  return instance;
}

int Deriv_::associer_(Objet_U& objet)
{
  Objet_U * ptr = get_O_U_Ptr_check();
  assert(ptr != 0);
  int resu = ptr->associer_(objet);
  return resu;
}

