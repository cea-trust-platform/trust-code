/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <TRUST_Deriv.h>

Implemente_instanciable_sans_constructeur_ni_destructeur(TRUST_Deriv_Objet_U, "TRUST_Deriv_Objet_U", Objet_U_ptr);

Sortie& TRUST_Deriv_Objet_U::printOn(Sortie& os) const
{
  const Objet_U * objet = get_Objet_U_ptr();
  if (objet)
    {
      os << objet->le_type() << finl;
      os << (*objet);
    }
  else os << "vide" << finl;

  return os;
}

Entree& TRUST_Deriv_Objet_U::readOn(Entree& is)
{
  detach(); // Efface l'objet existant
  static Nom nom_type; // static pour le pas creer un Objet_U a chaque fois
  is >> nom_type;
  Objet_U * objet = (Objet_U*) 0;
  if (nom_type != "vide")
    {
      objet = typer(nom_type);
      if (! objet) Process::exit();
    }

  set_Objet_U_ptr(objet);
  if (objet) is >> (*objet); // On lit

  return is;
}

Objet_U * TRUST_Deriv_Objet_U::typer(const char * type)
{
  const Type_info * type_info = Type_info::type_info_from_name(type); // Type_info du type demande
  const Type_info& type_base = get_info_ptr(); // Type de base du DERIV

  if ( get_Objet_U_ptr()) detach();

  Objet_U * instance = 0;

  if (type_info == 0)
    {
      const Synonyme_info* syn_info= Synonyme_info::synonyme_info_from_name(type);

      if (syn_info!=0) return typer(syn_info->org_name_());
      else
        {
          Cerr << "Error in Deriv_::typer_(const char* const type)" << finl << type << " is not a type." << finl;
          Cerr << "Type required : derived from " << type_base.name() << finl << finl;
          Cerr << type << " is not a recognized keyword." << finl << "Check your data set." << finl;
          Nom nompb = type;
          if (nompb.find("TURBULENT")!=-1 )
            Cerr << "Since TRUST V1.8.0, turbulence models are in TrioCFD and not anymore in TRUST.\nTry using TrioCFD executable or contact trust support." << finl;
          Process::exit();
        }
    }
  if (! type_info->instanciable())
    {
      Cerr << "Error in Deriv_::typer_(const char* const type).\n" << type << " is not instanciable." << finl;
      Process::exit();
    }

  if (! type_info->has_base(&type_base))
    Cerr << "Error in Deriv_::typer_(const char* const type).\n " << type << " is not a subtype of " << type_base.name() << finl;
  else
    instance = type_info->instance(); // Cree une instance du type decrit dans type_info

  set_Objet_U_ptr(instance);
  return instance;
}

TRUST_Deriv_Objet_U::~TRUST_Deriv_Objet_U() { detach(); }

TRUST_Deriv_Objet_U::TRUST_Deriv_Objet_U() : Objet_U_ptr(), pointeur_(0) { }

TRUST_Deriv_Objet_U::TRUST_Deriv_Objet_U(const TRUST_Deriv_Objet_U& t) : Objet_U_ptr(), pointeur_(0)
{
  if (t.non_nul()) recopie(t.valeur());
}

TRUST_Deriv_Objet_U::TRUST_Deriv_Objet_U(const Objet_U& t) : Objet_U_ptr(), pointeur_(0)
{
  recopie(t);
}

const TRUST_Deriv_Objet_U& TRUST_Deriv_Objet_U::operator=(const Objet_U& t)
{
  detach();
  recopie(t);
  return *this;
}

const TRUST_Deriv_Objet_U& TRUST_Deriv_Objet_U::operator=(const TRUST_Deriv_Objet_U& t)
{
  detach();
  if (t.non_nul()) recopie(t.valeur());
  else set_Objet_U_ptr((Objet_U*) 0);
  return *this;
}

const Type_info& TRUST_Deriv_Objet_U::get_info_ptr() const
{
  const Type_info *type_info = Objet_U::info();
  return *type_info; /* type de base accepte par la ref */
}

void TRUST_Deriv_Objet_U::set_Objet_U_ptr(Objet_U *objet)
{
  Objet_U_ptr::set_Objet_U_ptr(objet);
  if (objet)  pointeur_ = (Objet_U*) objet;
  else pointeur_ = (Objet_U*) 0;
}

void TRUST_Deriv_Objet_U::deplace(TRUST_Deriv_Objet_U& deriv_obj)
{
  detach();
  Objet_U& objet = deriv_obj.valeur();
  set_Objet_U_ptr(&objet);
  deriv_obj.set_Objet_U_ptr((Objet_U*) 0);
}

int TRUST_Deriv_Objet_U::reprendre(Entree& is)
{
  return valeur().reprendre(is);
}

int TRUST_Deriv_Objet_U::sauvegarder(Sortie& os) const
{
  return valeur().sauvegarder(os);
}

void TRUST_Deriv_Objet_U::detach()
{
  Objet_U *ptr = get_Objet_U_ptr();
  if (ptr)
    delete ptr;
  set_Objet_U_ptr((Objet_U*) 0);
}

int TRUST_Deriv_Objet_U::associer_(Objet_U& objet)
{
  Objet_U *ptr = get_Objet_U_ptr_check();
  assert(ptr != 0);
  int resu = ptr->associer_(objet);
  return resu;
}
