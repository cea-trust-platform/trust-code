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
// File:        Param.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/33
//
//////////////////////////////////////////////////////////////////////////////

#include <TRUSTArray.h>
#include <Parser_U.h>
#include <Param.h>

Param::Param(const char * name):proprietaire_(name) { }

// Description:
// Read the is input with the parameters (defined with the Param object) inside: { ... }
// Example, to read:  { a 1 b 2 }
// Param param(que_suis_je());
// int a,b;
// param.ajouter("a",&a);
// param.ajouter("b",&b);
// param.lire_avec_accolades_depuis(is);
int Param::lire_avec_accolades_depuis(Entree& is)
{
  int ok=read(is);
  ok=check();
  if (!ok)
    Process::exit();
  return 1;
}

int Param::lire_sans_accolade(Entree& is)
{
  Motcle bidon;
  LIST_CURSEUR(Objet_a_lire) curseur(list_parametre_a_lire_);
  while (curseur)
    {
      Objet_a_lire& obj=curseur.valeur();
      if (obj.is_optional())
        {
          Cerr<<proprietaire_<<" has optional param. not implemented for lire_sans_accolade"<<finl;
          Process::exit();
        }
      obj.read(bidon,is);
      Motcle mot=obj.get_name();
      list_parametre_lu_.add(mot);

      ++curseur;
    }
  int ok=check();
  if (!ok)
    Process::exit();
  return 1;

}
int Param::read(Entree& is,int with_acco)
{
  assert(with_acco==1);
  Motcle motlu;
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  is >> motlu;
  if (motlu != accolade_ouverte)
    {
      Cerr << "Error while reading the parameters of "<<proprietaire_<<  finl;
      Cerr << "We expected : " << accolade_ouverte << " and not "<<motlu<<finl;
      motlu.exit();
    }
  is >> motlu;
  while (motlu != accolade_fermee)
    {
      bool found=false;
      Motcle mot(motlu);
      LIST_CURSEUR(Objet_a_lire) curseur(list_parametre_a_lire_);
      while ((curseur)&&(!(found)))
        {
          Objet_a_lire& obj=curseur.valeur();

          if (obj.comprend_name(mot))
            {
              obj.read(mot,is);
              list_parametre_lu_.add(mot);
              found=true;
              break;
            }
          ++curseur;
        }
      if (!found)
        {
          Cerr<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<finl;
          Cerr<<motlu << " is not understood by "<<proprietaire_<<finl;
          Cerr<<"The understood keywords are : ";
          for (int i=0; i<list_parametre_a_lire_.size(); i++)
            Cerr<<list_parametre_a_lire_(i).get_names_message()<<" ";
          Cerr<<finl;
          // Evolution des options du mot cle Transformation
          // On previent les utilisateurs
          if (proprietaire_=="Transformation")
            {
              if (motlu=="fonction")
                {
                  Cerr << "Syntax has changed after the 1.6.3 version and fonction keyword is obsoleted." << finl;
                  Cerr << "Have a look at the TRUST documentation about Transformation keyword." << finl;
                  Cerr << "You should replace 'fonction' by 'methode formule expression nb_comp'" << finl;
                  Cerr << "where nb_comp is the number of components of your formulae following the old keyword 'fonction'" << finl;
                  Process::exit();
                }
              if (motlu=="vecteur")
                {
                  Cerr << "Syntax has changed after the 1.6.3 version." << finl;
                  Cerr << "Have a look at the TRUST documentation about Transformation keyword" << finl;
                  Cerr << "You should replace 'vecteur' by 'methode vecteur expression'" << finl;
                  Process::exit();
                }
              if (motlu=="produit_scalaire")
                {
                  Cerr << "Syntax has changed after the 1.6.3 version." << finl;
                  Cerr << "Have a look at the TRUST documentation about Transformation keyword" << finl;
                  Cerr << "You should replace 'produit_scalaire' by 'methode produit_scalaire'" << finl;
                  Process::exit();
                }
            }
          Process::exit();
        }
      is >>motlu;
    }
  return 1;
}
void Param::print(Sortie& s) const
{

  Motcle motlu;
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  s<<accolade_ouverte<<finl;

  for (int i=0; i<list_parametre_a_lire_.size(); i++)
    {
      const Objet_a_lire& obj=list_parametre_a_lire_(i);
      obj.print(s);
      s<<finl;
    }
  s<<accolade_fermee<<finl;
}

void Objet_a_lire::print(Sortie& s) const
{

  switch (type)
    {
    case INTEGER :
      if (dictionnaire_noms.size() == 0)
        {
          s<< get_name()<<" "<<(*int_a_lire);
        }
      else
        {
          Cerr<<" dico print not coded for this case "<< get_name()<<finl;
          Process::exit();
        }
      break;
    case DOUBLE  :
      s<<get_name()<<" "<< (*double_a_lire);
      break;
    case FLAG    :
      if  (*flag_a_lire) s<<get_name() ;
      break;
    case NON_STD :
      s<< get_name() << " non standard !!!!";
      break;
    default      :
      Cerr<<"print not coded for this case "<< get_name()<<finl;
      Process::exit();
      break;
    }
}
int Param::check()
{
  {
    // ok on a fini de lire est ce que tous les attributs non optionnels ont ete lus ?
    int err=0;
    LIST_CURSEUR(Objet_a_lire) curseur(list_parametre_a_lire_);
    for (; curseur; ++curseur)
      {
        Objet_a_lire& obj=curseur.valeur();
        if (!obj.is_optional())
          {
            Motcle name_var(obj.get_name());
            if (!list_parametre_lu_.contient(name_var))
              {
                Cerr<<obj.get_name() <<" of " << proprietaire_ <<" has not been read while it is required."<<finl;
                err=1;
              }
          }
      }
    if (err)
      return 0;
    //Process::exit();

  }
  // tests des autres conditions
  int val=1;
  LIST(Nom) list_mot;
  int size=list_parametre_a_lire_.size();
  for (int i=0; i<size; i++)
    {
      Nom mot_lu("is_read_");
      mot_lu+= list_parametre_a_lire_(i).get_name();
      list_mot.add(mot_lu);
    }
  for (int i=0; i<size; i++)
    {
      const Objet_a_lire& obj=list_parametre_a_lire_(i);
      if (obj.is_type_simple())
        {
          Nom mot_lu("value_of_");
          mot_lu+=list_parametre_a_lire_(i).get_name();
          list_mot.add(mot_lu);
        }
    }
  int size_agrandi=list_mot.size();
  int nb_condition=list_conditions_.size();
  for (int cond=0; cond<nb_condition; cond++)
    {
      Nom condition=list_conditions_(cond);
      Parser_U parser;
      parser.setNbVar(size_agrandi);
      for (int j=0; j<size_agrandi; j++)
        parser.addVar(list_mot(j));
      parser.setString(condition);
      parser.parseString();
      for (int j=0; j<size; j++)
        parser.setVar(j,0);
      for (int lu=0; lu<list_parametre_lu_.size(); lu++)
        {
          Nom mot_lu("is_read_");
          mot_lu+= list_parametre_lu_(lu);
          parser.setVar(mot_lu,1);
        }
      for (int inc=size; inc<size_agrandi; inc++)
        {
          Nom mot_lu(list_mot(inc));
          double xx=get_value(mot_lu);
          parser.setVar(mot_lu,xx);
        }
      if (parser.eval()==0)
        {
          val=0;
          Cerr<<" Condition "<< list_nom_conditions_(cond)<<" not verified: "<<condition<<" it's equal to "<<parser.eval()<<finl;
          Cerr<<list_message_erreur_conditions_(cond)<<finl;
        }


    }
  return val;
}

// retourne une liste de mot venant de nom.split('|')
LIST(Nom) split_mot(const Nom& nom)
{
  LIST(Nom) res;
  Nom n0(nom);
  // on cherche le dernier |
  const char * marq= strchr(n0,'|');
  while (marq)
    {
      Nom n1(n0);
      n1.prefix(marq);
      res.add(n1);

      marq++;
      Nom n1b(n0);
      n1b.prefix(marq);
      Nom n2(n0);
      n2.suffix(n1b);
      //Cerr<<" IIIII "<< n0 <<" "<<n1<<" "<<n2<<finl;
      n0=n2;
      marq= strchr(n0,'|');
    }
  //Cerr<<" "<<n0<<finl;
  res.add(n0);
  //Cerr<<"YYY "<<res<<finl;
  return res;
}

// Description:
//  Si le dernier parametre ajoute est de type "int", associe a ce
//   parametre une ou plusieurs chaines de caracteres et pour chacune une valeur numerique.
//  Il faut appeler dictionnaire pour chaque motcle autorise pour ce parametre
//  Le parametre lu doit alors etre un motcle parmi ceux du dictionnaire de noms autorises.
void Param::dictionnaire(const char * nom_option, int valeur)
{
  const int sz = list_parametre_a_lire_.size();
  assert(sz > 0);
  Objet_a_lire& last = list_parametre_a_lire_[sz-1];
  last.add_dict(nom_option, valeur);
}
Param& Param::dictionnaire_param(const char * nom_option, int valeur)
{
  const int sz = list_parametre_a_lire_.size();
  assert(sz > 0);
  Objet_a_lire& last = list_parametre_a_lire_[sz-1];
  Nom name(proprietaire_);
  name+="/";
  name+=Nom(nom_option);
  return last.add_dict(nom_option, valeur,name).valeur();
}

// Description : renvoit Objet_a_lire correspondant au mot
//               le rajoute au besoin a la liste
Objet_a_lire& Param::create_or_get_objet_a_lire(const char * mot)
{
  LIST(Nom) split_noms=split_mot(Nom(mot));

  CONST_LIST_CURSEUR(Nom) curseur_noms(split_noms);
  for(; curseur_noms; ++curseur_noms)
    {
      Motcle name_var(curseur_noms.valeur());
      LIST_CURSEUR(Objet_a_lire) curseur(list_parametre_a_lire_);
      //for (int i=0; i<list_parametre_a_lire_.size(); i++)
      while (curseur)
        {
          if (curseur.valeur().comprend_name(name_var))
            {
              Cerr<<name_var <<" was already a keyword of "<<proprietaire_<<finl;
              Cerr<<"the old one will be ignored"<<finl;
              Process::exit();

              return curseur.valeur();
            }
          ++curseur;
        }
    }
  Objet_a_lire toto;
  Objet_a_lire& obj=list_parametre_a_lire_.add(toto);
  obj.set_name(split_noms);
  return obj;
}

void Param::supprimer(const char* mot)
{
  LIST(Nom) split_noms=split_mot(Nom(mot));

  Motcle motcle(split_noms(0));
  LIST_CURSEUR( Objet_a_lire) curseur(list_parametre_a_lire_);
  for (; curseur; ++curseur)
    {
      Objet_a_lire& obj=curseur.valeur();
      //Motcle mot(motlu);
      if (obj.comprend_name(motcle))
        {
          list_parametre_a_lire_.suppr(obj);
          return;
        }
    }
  Cerr<<"Error in Param::supprimer parametre "<<mot <<" not found in "<<proprietaire_<<finl;
  Process::exit();
}

Objet_a_lire::Nature convert_nature(Param::Nature nat)
{
  if (nat==Param::REQUIRED)
    return Objet_a_lire::REQUIRED;
  else
    return Objet_a_lire::OPTIONAL;
}

void Param::ajouter(const char* mot, int* quoi, Param::Nature nat ) //int opt)
{
  Objet_a_lire& obj=create_or_get_objet_a_lire(mot);
  obj.set_nature(convert_nature(nat));
  obj.set_entier(quoi);
}
void Param::ajouter(const char* mot, double* quoi,  Param::Nature nat ) //int opt)
{
  Objet_a_lire& obj=create_or_get_objet_a_lire(mot);
  obj.set_nature(convert_nature(nat));
  obj.set_double(quoi);
}
void Param::ajouter(const char* mot, Objet_U* quoi,  Param::Nature nat ) //int opt)
{
  Objet_a_lire& obj=create_or_get_objet_a_lire(mot);
  obj.set_nature(convert_nature(nat));
  obj.set_objet(quoi);
}
Param& Param::ajouter_param(const char* mot,Param::Nature nat)
{
  Objet_a_lire& obj=create_or_get_objet_a_lire(mot);
  obj.set_nature(convert_nature(nat));
  Nom name(proprietaire_);
  name+="/";
  name+=obj.get_name();
  return obj.create_param(name);
}
void Param::ajouter_arr_size_predefinie(const char* mot, ArrOfInt* quoi,  Param::Nature nat ) //int opt)
{
  Objet_a_lire& obj=create_or_get_objet_a_lire(mot);
  obj.set_nature(convert_nature(nat));
  obj.set_arrofint(quoi);
}
void Param::ajouter_arr_size_predefinie(const char* mot, ArrOfDouble* quoi,  Param::Nature nat ) //int opt)
{
  Objet_a_lire& obj=create_or_get_objet_a_lire(mot);
  obj.set_nature(convert_nature(nat));
  obj.set_arrofdouble(quoi);
}

// Description:
//  Lecture d'un objet de type deriv. Format attendu : Titi { parametres }
//  L'objet sera type avec le type prefixeTiti, puis le readOn de l'objet sera appele.
void Param::ajouter_deriv(const char* mot, const char *prefixe, Deriv_* quoi,  Param::Nature nat ) //int opt)
{
  Objet_a_lire& obj=create_or_get_objet_a_lire(mot);
  obj.set_nature(convert_nature(nat));
  obj.set_deriv(quoi, prefixe);
}

void Param::ajouter_flag(const char* mot, int* quoi,  Param::Nature nat ) //int opt)
{
  Objet_a_lire& obj=create_or_get_objet_a_lire(mot);
  obj.set_nature(convert_nature(nat));
  obj.set_flag(quoi);
}
void Param::ajouter_non_std(const char* mot, Objet_U* quoi,  Param::Nature nat ) //int opt)
{
  Objet_a_lire& obj=create_or_get_objet_a_lire(mot);
  obj.set_nature(convert_nature(nat));
  obj.set_non_std(quoi);
}



void Param::ajouter_condition(const char* condition, const char* message, const char* name)
{
  list_conditions_.add(Nom(condition));
  list_message_erreur_conditions_.add(Nom(message));
  Nom nom;
  if (name==0)
    {
      nom="condition_";
      nom+=Nom(list_conditions_.size());
    }
  else
    nom=Nom(name);
  list_nom_conditions_.add(nom);
}
void Param::supprimer_condition( const char* name)
{
  Nom nom(name);
  int i=list_nom_conditions_.rang(nom);
  if (i<0)
    {
      Cerr<<"Error in Param::supprimer_condition, condition "<<nom << " not found in "<<proprietaire_<<finl;
      Process::exit();
    }


  list_conditions_.suppr(list_conditions_(i));
  list_message_erreur_conditions_.suppr(list_message_erreur_conditions_(i));

  list_nom_conditions_.suppr(list_nom_conditions_(i));

}

double Param::get_value(const Nom& mot_lu) const
{
  int size=list_parametre_a_lire_.size();
  Motcle val("value_of_");
  for (int i=0; i<size; i++)
    {
      const Objet_a_lire& obj= list_parametre_a_lire_(i);
      if (obj.is_type_simple())
        {
          Motcle nom(val);
          nom+=obj.get_name();
          if (nom==mot_lu)
            {
              return obj.get_value();
            }
        }
    }
  Cerr<<mot_lu<<" not found in Param::get_value"<<finl;
  Cerr<<"the understood keywords are : ";
  for (int i=0; i<list_parametre_a_lire_.size(); i++)
    if (list_parametre_a_lire_(i).is_type_simple())
      Cerr<<" value_of_"<<list_parametre_a_lire_(i).get_name();
  Cerr<<finl;
  mot_lu.exit();
  return -1.;
}




