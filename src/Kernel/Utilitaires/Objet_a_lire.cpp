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
// File:        Objet_a_lire.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/2
//
//////////////////////////////////////////////////////////////////////////////

#include <Objet_a_lire.h>
#include <Param.h>
#include <Entree_complete.h>
#include <ArrOfDouble.h>

Implemente_instanciable_sans_constructeur(Objet_a_lire,"Objet_a_lire",Objet_U);
Implemente_liste(Objet_a_lire);

Objet_a_lire::Objet_a_lire(): int_a_lire(NULL),double_a_lire(NULL), obj_a_lire(NULL), objet_lu(NULL),arrofint_a_lire(NULL),arrofdouble_a_lire(NULL),flag_a_lire(NULL)
{
}
Entree& Objet_a_lire::readOn(Entree& is)
{
  return Objet_U::readOn(is);
}
Sortie& Objet_a_lire::printOn(Sortie& os) const
{
  return Objet_U::printOn(os);
}

void Objet_a_lire::set_entier( int* quoi  )
{
  type=INTEGER;
  int_a_lire=quoi;
}

void Objet_a_lire::set_double(double* quoi  )
{
  type=DOUBLE;
  double_a_lire=quoi;
}

void Objet_a_lire::set_objet(Objet_U* quoi  )
{
  obj_a_lire=quoi;
  type=OBJECT;
}

void Objet_a_lire::set_arrofint(ArrOfInt* quoi  )
{
  arrofint_a_lire=quoi;
  type=ArrOfInt_size_imp;
}

void Objet_a_lire::set_arrofdouble(ArrOfDouble* quoi  )
{
  arrofdouble_a_lire=quoi;
  type=ArrOfDouble_size_imp;
}

void Objet_a_lire::set_deriv(Deriv_* quoi, const char *prefixe)
{
  obj_a_lire=quoi;
  type=DERIV;
  prefixe_deriv = prefixe;
}

void Objet_a_lire::set_flag( int* quoi  )
{
  flag_a_lire=quoi;
  // initialisation du flag a 0
  *flag_a_lire=0;
  type=FLAG;
}

void Objet_a_lire::set_non_std(Objet_U* quoi  )
{
  objet_lu=quoi;
  type=NON_STD;
}
Param& Objet_a_lire::create_param(const char* aname)
{
  type=PARAM;
  param_interne.create(aname);
  return param_interne.valeur();
}

bool Objet_a_lire::is_optional(void) const
{
  return (nature == OPTIONAL);
}


void Objet_a_lire::set_name(const LIST(Nom)& n)
{
  CONST_LIST_CURSEUR(Nom) curseur(n);
  name=curseur.valeur();
  ++curseur;
  while (curseur)
    {
      names.add(curseur.valeur());
      ++curseur;
    }
}

int Objet_a_lire::comprend_name(Motcle& mot) const
{
  if (mot==name)
    return 1;
  //int size=names.size();
  CONST_LIST_CURSEUR(Nom) curseur(names);
  while (curseur)
    //for (int i=0; i<size; i++)
    {
      // Cerr<<"ici "<<names(i)<<finl;
      if (mot==curseur.valeur())
        {
          //Cerr<<names.rang(mot)<<finl;
          mot=name;
          return 1;
        }
      ++curseur;
    }
  return 0;
}
Nom Objet_a_lire::get_names_message(void) const
{
  Nom titi(name);
  int size=names.size();
  for (int i=size-1; i>=0; i--)
    {
      titi+=Nom("|");
      titi+=names(i);
    }
  return titi;
}

const Nom& Objet_a_lire::get_name(void) const
{
  return name;
}

ptrParam& Objet_a_lire::add_dict(const char* nom_option, int valeur,const char* aname)
{
  // Le dictionnaire ne fonctionne que pour des parametres de type int:
  assert(int_a_lire != 0);
  // L'option ne doit pas encore exister dans le dictionnaire:
  assert(dictionnaire_noms.search(nom_option) < 0);

  dictionnaire_noms.add(nom_option);
  dictionnaire_valeurs.set_smart_resize(1);
  dictionnaire_valeurs.append_array(valeur);
  dictionnaire_params.add(ptrParam());
  ptrParam&  ptr=  dictionnaire_params[dictionnaire_params.size()-1];
  if (aname!=0)
    {
      ptr.create(aname);
    }
  return ptr;
}
void Objet_a_lire::set_nature(Objet_a_lire::Nature n)
{
  nature = n;
}

bool Objet_a_lire::is_type_simple() const
{
  // return ((double_a_lire!=NULL)||(int_a_lire!=NULL)||(flag_a_lire!=NULL));
  return ((type==INTEGER)||(type==DOUBLE)||(type==FLAG));
}

double Objet_a_lire::get_value() const
{

  switch (type)
    {
    case INTEGER :
      return (*int_a_lire);
      break;
    case DOUBLE  :
      return (*double_a_lire);
      break;
    case FLAG    :
      return (*flag_a_lire);
      break;
    default      :
      Cerr<<"get_value not coded for this case"<<finl;
      Process::exit();
      break;
    }
  // pour les compilos
  return 0.;
}

void Objet_a_lire::read(Motcle const& motcle, Entree& is)
{
  switch (type)
    {
    case INTEGER :
      if (dictionnaire_noms.size() == 0)
        {
          is>>*int_a_lire;
        }
      else
        {
          Motcle motlu;
          is >> motlu;
          const int rang = dictionnaire_noms.search(motlu);
          if (rang < 0)
            {
              Cerr << "Error while reading parameter " << name
                   << "\n Found: " << motlu
                   << "\n Expected one on these keywords: " << dictionnaire_noms << finl;
              barrier();
              exit();
            }
          *int_a_lire = dictionnaire_valeurs[rang];
          ptrParam& ptr=dictionnaire_params[rang];
          if (ptr.non_nul())
            {
              ptr.valeur().lire_avec_accolades_depuis(is);
            }
        }
      break;
    case DOUBLE  :
      is >> (*double_a_lire);
      break;
    case OBJECT  :
      is >> (*obj_a_lire);
      break;
    case FLAG    :
      (*flag_a_lire)=1;
      break;
    case NON_STD :
      (*objet_lu).lire_motcle_non_standard(motcle,is);
      break;
    case DERIV   :
      {
        Motcle type_complet(prefixe_deriv);
        Motcle the_type;
        is >> the_type;
        type_complet += the_type;
        Entree_complete is2(type_complet, is);
        is2 >> (*obj_a_lire);
        break;
      }
    case ArrOfInt_size_imp:
      {
        ArrOfInt& arr=*arrofint_a_lire;
        int size=arr.size_array();
        // on ne veut pas de taille nulle pour etre sur que l'utilisateur a bien fait dimensionner...
        assert(size>0);
        for (int i=0; i<size; i++) is >>arr[i];
        break;
      }
    case ArrOfDouble_size_imp:
      {
        ArrOfDouble& arr=*arrofdouble_a_lire;
        int size=arr.size_array();
        // on ne veut pas de taille nulle pour etre sur que l'utilisateur a bien fait dimensionner...
        assert(size>0);
        for (int i=0; i<size; i++) is >>arr[i];
        break;
      }
    case PARAM:
      {
        param_interne.valeur().lire_avec_accolades_depuis(is);
        break;
      }
    default      :
      Cerr << "Invalid parameter type" << finl;
      Process::exit();
      break;
    }
}
