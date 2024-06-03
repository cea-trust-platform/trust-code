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

#include <Synonyme_info.h>
#include <Objet_U_ptr.h>
#include <Memoire.h>
#include <Nom.h>

Implemente_base_sans_constructeur_ni_destructeur(Objet_U_ptr,"Objet_U_ptr",Objet_U);

Sortie& Objet_U_ptr::printOn(Sortie& os) const
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

Entree& Objet_U_ptr::readOn(Entree& is)
{
  detach(); // Efface l'objet existant
  static Nom nom_type; // static pour le pas creer un Objet_U a chaque fois
  is >> nom_type;
  Objet_U * objet = nullptr;
  if (nom_type != "vide")
    {
      objet = typer(nom_type);
      if (! objet) Process::exit();
    }

  set_Objet_U_ptr(objet);
  if (objet) is >> (*objet); // On lit

  return is;
}

/*! @brief Destructeur.
 *
 * Il ne detruit pas l'objet en reference
 *
 */
Objet_U_ptr::~Objet_U_ptr()
{
  cle_ = -2; // Paranoia : on rend le pointeur invalide
}

/*! @brief construit un pointeur nul (cle a -1)
 *
 */
Objet_U_ptr::Objet_U_ptr() : cle_(-1), ptr_object_id_(-1) { }

void Objet_U_ptr::detach()
{
  Objet_U * ptr = get_Objet_U_ptr();
  if (ptr) delete ptr;
  set_Objet_U_ptr(nullptr);
}

int Objet_U_ptr::associer_(Objet_U& objet)
{
  Objet_U * ptr = get_Objet_U_ptr_check();
  assert(ptr != nullptr);
  int resu = ptr->associer_(objet);
  return resu;
}

/*! @brief Renvoie true si le pointeur est non_nul Renvoie false sinon.
 *
 */
bool Objet_U_ptr::non_nul() const
{
  assert(get_Objet_U_ptr_check() || 1);
  return (cle_ >= 0) ? true : false;
}

bool Objet_U_ptr::est_nul() const
{
  assert(get_Objet_U_ptr_check() || 1);
  return (cle_ >= 0) ? false : true;
}

/*! @brief Verifie si le pointeur est valide.
 *
 * Le pointeur est valide si cle_==-1
 *      ou si la_memoire().objet_u(cle_) a le meme object_id_ que
 *         celui enregistre dans ptr_object_id_.
 *    Si le pointeur n'est pas valide, arret du programme.
 *  Renvoie l'adresse de l'objet pointe (de type Objet_U)
 *
 */
Objet_U * Objet_U_ptr::get_Objet_U_ptr_check() const
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
          Cerr << "Error in Objet_U_ptr::get_Objet_U_ptr_check() : id != ptr_object_id_\n";
          Cerr << " Pointer type " << le_type();
          Cerr << "\n cle_           = " << cle_;
          Cerr << "\n ptr_object_id_ = " << ptr_object_id_;
          Cerr << "\n id             = " << id;
          std::cerr << "\n &la_memoire().objet_u(cle_) = " << addr << std::endl;
          // Si ca plante a cet endroit, c'est que l'objet en reference
          // a ete detruit et que la reference est encore utilisee.
          exit();
        }
    }
  return 0;
}

/*! @brief Verifie que l'objet pointe par ptr est d'un type acceptable pour le pointeur (avec get_info_ptr)
 *
 */
int Objet_U_ptr::check_Objet_U_ptr_type(const Objet_U * ptr) const
{
  if (ptr == nullptr) return 1; // Le pointeur nul est valide

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
      Cerr << "Error in Objet_U_ptr::get_Objet_U_ptr_check() : Type error\n";
      Cerr << " Pointer type " << le_type();
      Cerr << "\n cle_           = " << cle_;
      Cerr << "\n ptr_object_id_ = " << ptr_object_id_;
      std::cerr << "\n &memoire.objet_u(cle_) = " << ptr;
      Cerr << "\n Type accepted by the pointer : " << type_info_ptr.name();
      Cerr << "\n Object type in reference : " << type_info_obj.name();
      Process::exit();
    }
  return 1;
}

/*! @brief Renvoie ref_().
 *
 * le_nom() si le pointeur est non nul ou "Pointeur Nul"
 *
 */
const Nom& Objet_U_ptr::le_nom() const
{
  static Nom nom("Pointeur Nul");
  const Objet_U * addr = get_Objet_U_ptr_check();
  if (addr)
    return addr->le_nom();
  return nom;
}

/*! @brief Pour mettre a jour les cles lorsque les Objet_U ont etes renumerotes.
 *
 * @param (const int* const new_ones) tableau de la nouvelle numerotation
 * @return (int) la nouvelle cle du pointeur
 */
int Objet_U_ptr::change_num(const int* const new_ones)
{
  Objet_U::change_num(new_ones);

  // Il ne faut pas faire d'appel a une fonction qui fait "verifie"
  // car la memoire est en cours de modification.
  if (cle_ > -1)
    cle_ = new_ones[cle_];
  return cle_;
}

/*! @brief Duplique l'Objet_U obj puis change le pointeur vers cette copie.
 *
 * @param (const Objet_U& obj) reference sur l'Objet_U a copier
 */
void Objet_U_ptr::recopie(const Objet_U& obj)
{
  int cle = obj.duplique();
  Memoire& memoire = Memoire::Instance();
  Objet_U& objet = memoire.objet_u(cle);
  set_Objet_U_ptr(& objet);
}

/*! @brief Renvoie un pointeur sur l'Objet_U associe ATTENTION: l'adresse peut etre nulle (si le pointeur est nul)
 *
 */
Objet_U * Objet_U_ptr::get_Objet_U_ptr() const
{
  const Objet_U * objet;
  if (cle_ < 0)
    {
      objet = nullptr;
    }
  else
    {
      Memoire& memoire = Memoire::Instance();
      objet = & memoire.objet_u(cle_);
    }
  assert(objet == get_Objet_U_ptr_check());
  return (Objet_U*) objet;
}

/*! @brief Fait pointer *this sur l'objet *ptr L'adresse peut etre nulle (pointeur nul).
 *
 */
void Objet_U_ptr::set_Objet_U_ptr(Objet_U * ptr)
{
  if (ptr != nullptr)
    {
      cle_ = ptr->numero();
      ptr_object_id_ = ptr->get_object_id();
    }
  else
    {
      cle_ = -1;
      ptr_object_id_ = -1;
    }
  // Il suffit de tester le type ici : si le type est bon ici et qu'ensuite l'object_id_ ne change pas, alors le type est toujours bon.
  assert(check_Objet_U_ptr_type(ptr));
  assert(get_Objet_U_ptr_check() == ptr);
}

/*! @brief Essaie de creer une instance du type "type".
 *
 * si type n'est pas un type ou type n'est pas instanciable=>arret
 *    si type n'est pas un sous-type du type du pointeur=>retour 0
 *    si ok, renvoie l'adresse de l'objet cree.
 */
Objet_U * Objet_U_ptr::typer(const char * type)
{
  const Type_info * type_info = Type_info::type_info_from_name(type); // Type_info du type demande
  const Type_info& type_base = get_info_ptr(); // Type de base du DERIV

  if ( get_Objet_U_ptr()) detach();

  Objet_U * instance = nullptr;

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
          if (nompb.find("TURBULENT") != -1 || nompb.find("TURBULENCE") != -1)
            Cerr << finl << "*** NOTE :: Since TRUST V1.8.0, turbulence models are in TrioCFD and not anymore in TRUST.\nTry using TrioCFD executable or contact TRUST support." << finl;
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

