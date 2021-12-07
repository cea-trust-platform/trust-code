/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        O_U_Ptr.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <O_U_Ptr.h>
#include <Nom.h>
#include <stdio.h>
#include <verif_cast.h>

Implemente_base_sans_constructeur_ni_destructeur(O_U_Ptr,"O_U_Ptr",Objet_U);

// Description:
//    Surcharge Objet_U::printOn(Sortie&)
//    Ecriture de la cle d'un O_U_Ptr sur un flot de sortie.
//    A definir avec readOn. Pour l'instant :
//    os << cle_ << finl;
Sortie& O_U_Ptr::printOn(Sortie& os) const
{
  get_O_U_Ptr_check();
  return os << cle_ << finl;
}

// Description:
//    Surcharge Objet_U::readOn(Entree&)
//    Lecture d'un O_U_Ptr (par sa cle) dans un flot d'entree
//    ATTENTION: Le comportement de cette fonction est a definir.
//               Pour l'instant: ne fait rien.
Entree& O_U_Ptr::readOn(Entree& is)
{
  // Il y aurait deux choix pertinents:
  // choix 1:
  //    is >> cle_ >> pointeur_;
  // choix 2:
  //    is >> cle_;
  //    if (cle_ >= 0)
  //      pointeur_ = la_memoire().objet_u(cle_);
  //    else
  //      pointeur_ = 0;

  // B.Mathieu (8/7/2004)
  // Pour l'instant, ne fait rien : necessaire pour compatibilite
  // avec le reste du code (exemple Op_Diff_negligeable::readOn)
  return is;
}

// Description:
//     Destructeur. Il ne detruit pas l'objet en reference
O_U_Ptr::~O_U_Ptr()
{
  // Paranoia : on rend le pointeur invalide
  cle_ = -2;
}

// Description:
//     construit un pointeur nul (cle a -1)
O_U_Ptr::O_U_Ptr() :
  cle_(-1),
  ptr_object_id_(-1)
{
}

// Description:
//   Constructeur par copie
O_U_Ptr::O_U_Ptr(const O_U_Ptr& ptr): Objet_U(ptr)
{
  assert(0);
}

// Description:
//   Operateur copie
const O_U_Ptr& O_U_Ptr::operator=(const O_U_Ptr& ptr)
{
  assert(0);
  return *this;
}

// Description:
//     Renvoie 1 si le pointeur est non_nul
//     Renvoie 0 sinon.
int O_U_Ptr::non_nul() const
{
  assert(get_O_U_Ptr_check() || 1);
  return (cle_ >= 0);
}

// Description:
//   Verifie si le pointeur est valide. Le pointeur est valide
//     si cle_==-1
//     ou si la_memoire().objet_u(cle_) a le meme object_id_ que
//        celui enregistre dans ptr_object_id_.
//   Si le pointeur n'est pas valide, arret du programme.
// Renvoie l'adresse de l'objet pointe (de type Objet_U)
Objet_U * O_U_Ptr::get_O_U_Ptr_check() const
{
  if (cle_ != -1 || ptr_object_id_ != -1)
    {
      Objet_U& objet = Memoire::Instance().objet_u(cle_);
      Objet_U * addr = &objet;
      const int id = objet.get_object_id();
      if (id == ptr_object_id_)
        return addr;
      else
        {
          Cerr << "(PE" << me() << ") ";
          Cerr << "Error in O_U_Ptr::get_O_U_Ptr_check() : id != ptr_object_id_\n";
          Cerr << " Pointer type " << le_type();
          Cerr << "\n cle_           = " << cle_;
          Cerr << "\n ptr_object_id_ = " << ptr_object_id_;
          Cerr << "\n id             = " << id;
          std::cerr << "\n &la_memoire().objet_u(cle_) = " << addr << std::endl;
          // Si ca plante a cet endroit, c'est que l'objet en reference
          // a ete detruit et que la reference est encore utilisee.
          assert(0);
          exit();
        }
    }
  return 0;
}

// Description:
//  Verifie que l'objet pointe par ptr est d'un type acceptable
//  pour le pointeur (avec get_info_ptr)
int O_U_Ptr::check_O_U_Ptr_type(const Objet_U * ptr) const
{
  if (ptr == 0)
    return 1; // Le pointeur nul est valide

  const Objet_U& objet = *ptr;
  // On verifie que l'objet est du bon type :
  // type accepte par le pointeur :
  const Type_info& type_info_ptr = get_info_ptr();
  // type de l'objet :
  const Type_info& type_info_obj = *(objet.get_info());
  // Peut-on convertir type_info_obj en type_info_ptr ?
  if (! type_info_ptr.can_cast(&type_info_obj))
    {
      Cerr << "(PE" << me() << ") ";
      Cerr << "Error in O_U_Ptr::get_O_U_Ptr_check() : Type error\n";
      Cerr << " Pointer type " << le_type();
      Cerr << "\n cle_           = " << cle_;
      Cerr << "\n ptr_object_id_ = " << ptr_object_id_;
      std::cerr << "\n &memoire.objet_u(cle_) = " << ptr;
      Cerr << "\n Type accepted by the pointer : " << type_info_ptr.name();
      Cerr << "\n Object type in reference : " << type_info_obj.name();
      assert(0);
      exit();
    }
  return 1;
}

// Description:
//     Renvoie ref_().le_nom() si le pointeur est non nul
//     ou "Pointeur Nul"
const Nom& O_U_Ptr::le_nom() const
{
  static Nom nom("Pointeur Nul");
  const Objet_U * addr = get_O_U_Ptr_check();
  if (addr)
    return addr->le_nom();
  return nom;
}

// Description:
//     Pour mettre a jour les cles lorsque les
//     Objet_U ont etes renumerotes.
// Precondition:
// Parametre: const int* const new_ones
//    Signification: tableau de la nouvelle numerotation
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: la nouvelle cle du pointeur
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int O_U_Ptr::change_num(const int* const new_ones)
{
  Objet_U::change_num(new_ones);

  // Il ne faut pas faire d'appel a une fonction qui fait "verifie"
  // car la memoire est en cours de modification.
  if (cle_ > -1)
    cle_ = new_ones[cle_];
  return cle_;
}

// Description:
//     Duplique l'Objet_U obj puis change le pointeur vers cette copie.
// Precondition:
// Parametre: const Objet_U& obj
//    Signification: reference sur l'Objet_U a copier
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void O_U_Ptr::recopie(const Objet_U& obj)
{
  int cle = obj.duplique();
  Memoire& memoire = Memoire::Instance();
  Objet_U& objet = memoire.objet_u(cle);
  set_O_U_Ptr(& objet);
}

// Description:
//   Renvoie un pointeur sur l'Objet_U associe
//   ATTENTION: l'adresse peut etre nulle (si le pointeur est nul)
Objet_U * O_U_Ptr::get_O_U_Ptr() const
{
  const Objet_U * objet;
  if (cle_ < 0)
    {
      objet =  0;
    }
  else
    {
      Memoire& memoire = Memoire::Instance();
      objet = & memoire.objet_u(cle_);
    }
  assert(objet == get_O_U_Ptr_check());
  return (Objet_U*) objet;
}

// Description:
//   Fait pointer *this sur l'objet *ptr
//   L'adresse peut etre nulle (pointeur nul).
void O_U_Ptr::set_O_U_Ptr(Objet_U * ptr)
{
  if (ptr != 0)
    {
      cle_ = ptr->numero();
      ptr_object_id_ = ptr->get_object_id();
    }
  else
    {
      cle_ = -1;
      ptr_object_id_ = -1;
    }
  // Il suffit de tester le type ici : si le type est bon ici
  // et qu'ensuite l'object_id_ ne change pas, alors le
  // type est toujours bon.
  assert(check_O_U_Ptr_type(ptr));
  assert(get_O_U_Ptr_check() == ptr);
}

int O_U_Ptr::get_ptr_cle() const
{
  return cle_;
}

int O_U_Ptr::get_ptr_object_id() const
{
  return ptr_object_id_;
}
