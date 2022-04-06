/****************************************************************************
* Copyright (c) 2022, CEA
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
// File:        Synonyme_info.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Synonyme_info_included
#define Synonyme_info_included
#include <arch.h>

class Objet_U;
class Nom;
class Noms;
class Sortie;

// Macro to declare A and B synonyms:
#define Add_synonym(A,B) const Synonyme_info name2(name2(synonym_,A),__LINE__)(B,A::info_obj.name())

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     modelise une information de synonyme pour les Objet_U
// .SECTION voir aussi
//     Objet_U Nom
//////////////////////////////////////////////////////////////////////////////
class Synonyme_info
{
public:
  Synonyme_info() {};
  ~Synonyme_info();
  Synonyme_info(const char* name, const char* org);

  inline const char* org_name_() const
  {
    return org;
  };
  inline const char* nom() const
  {
    return n;
  };

  // Methodes statiques :
  static Sortie&           hierarchie(Sortie&) ;
  static int            est_un_synonyme(const char*) ;

  static const Synonyme_info * synonyme_info_from_name(const char * synonyme_name);
  // static Objet_U*          instance(const char* typ);
  static void ajouter_synonyme(const Synonyme_info& synonyme_info);
protected:

private:
  Synonyme_info(Synonyme_info&) {}; // Constructeur par copie invalide
  Synonyme_info& operator=(Synonyme_info&); // Operateur= invalide


  static void retirer_synonyme(const char *nom );
  static int search_synonyme_info_name(const char *nom, int& index);

  // Le nom de la classe.
  const char* n;
  const char* org;

  // Liste des Synonyme_info des classes declarees par declare_base/declare_instanciable
  // La liste est triee par ordre alphabetique (minuscules/majuscules indifferentes)
  static const Synonyme_info** les_synonymes;

  // Nombre de classes enregistrees dans "les_synonymes" et "synonymes_homonymes"
  static int nb_classes;
  // Taille memoire du tableau "les_synonymes" et "synonymes_homonymes"
  // (tableau redimensionne par blocs)
  static int les_synonymes_memsize;
};


#endif

