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
// File:        Champ_Fonc_Tabule.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Fonc_Tabule.h>
#include <Champ_Uniforme.h>

Implemente_instanciable(Champ_Fonc_Tabule,"Champ_Fonc_Tabule",Champ_Fonc_base);
// XD champ_fonc_tabule champ_don_base champ_fonc_tabule 0 Field that is tabulated as a function of another field.
// XD  attr inco chaine inco 0 Name of the field (for example: temperature).
// XD  attr dim int dim 0 Number of field components.
// XD  attr bloc bloc_lecture bloc 0 Values (the table (the value of the field at any time is calculated by linear interpolation from this table) or the analytical expression (with keyword expression to use an analytical expression)).

#include <string>
#include <algorithm>

void Champ_Fonc_Tabule::Warn_old_chp_fonc_syntax(const char * nom_class, const Nom& val1, const Nom& val2, int& dim, Nom& param)
{
  const bool isNum = Check_if_int(val1);
  if (!isNum) // val1 is not a num - this is the correct new syntax
    {
      param = val1;
      dim = atoi(val2);
      return;
    }
  else   // Detect old syntax and inform user
    {
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "Error in call to " << nom_class << ":" << finl;
      Cerr << "The syntax has changed in version 1.8.2." << finl;
      Cerr << "You should now pass the dimension/number of components AFTER the field/parameter name." << finl;
      Cerr << "Please update your dataset or contact TRUST support team." << finl;
      Process::exit();
    }
}

void Champ_Fonc_Tabule::Warn_old_chp_fonc_syntax_V_184(const char * nom_class, const Nom& prob, const Nom& field)
{
  if ((std::string)nom_class == "Champ_Fonc_Tabule")
    {
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "Error in call to " << nom_class << ":" << finl;
      Cerr << "The syntax has changed in version 1.8.4. It should be now defined as follows : " << finl;
      Cerr << finl << " " << nom_class << " {  problem_name field_name [ problem2_name field2_name ] } dimension { ... your_table ... }" << finl;
      Cerr << finl << "Please update your dataset or contact TRUST support team." << finl;
      Process::exit();
    }
  else
    {
      const bool isNum = Check_if_int(prob), isNum2 = Check_if_int(field);
      if (isNum || isNum2)
        {
          Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
          Cerr << "Error in call to " << nom_class << ":" << finl;
          Cerr << "The syntax has changed in version 1.8.4. It should be now defined as follows : " << finl;
          Cerr << finl << "  " << nom_class << "   problem_name   field_name   dimension   { ... your_expression ... }" << finl;
          Cerr << finl << "Please update your dataset or contact TRUST support team." << finl;
          Process::exit();
        }
    }
}

bool Champ_Fonc_Tabule::Check_if_int(const Nom& val)
{
  std::string s((const char*)val);
  // lambda checking whehter a char is not a digit:
  auto checkNotDig = [](unsigned char c) {  return !std::isdigit(c); };
  return (!s.empty() && std::find_if(s.begin(), s.end(), checkNotDig) == s.end());
}

Sortie& Champ_Fonc_Tabule::printOn(Sortie& os) const
{
  return os;
}

// Description:
//     Lecture du Champ a partir d'un flot d'entree,
//     (On ne sait traiter que les champs scalaires.)
//     exemple:
//     Champ_Fonc_Tabule { probleme ch }
//     1 (nombre de composantes)
//     {
//     2
//     0 500 0 250  (ch(0)=0 && ch(500)=250
//     }
// Precondition: seul les champs scalaires sont traites
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade fermante attendue
// Exception: accolade ouvrante attendue
// Effets de bord:
// Postcondition:
Entree& Champ_Fonc_Tabule::readOn(Entree& is)
{
  int old_table_syntax_ = 0;

  Nom motlu;
  const Motcle accolade_ouverte("{"), accolade_fermee("}");
  int nbcomp;
  is >> motlu;
  if (motlu != accolade_ouverte)  // Warn_old_chp_fonc_syntax_V_184("Champ_Fonc_Tabule","rien","rien");
    {
      noms_champs_parametre_.add(motlu);
      old_table_syntax_ = 1;
    }
  else
    {
      assert (old_table_syntax_ == 0);
      while (true)
        {
          is >> motlu;
          if (motlu == "}") break;
          noms_pbs_.add(motlu);
          is >> motlu;
          noms_champs_parametre_.add(motlu);
        }
    }
  const int nb_param = noms_champs_parametre_.size();
  if (old_table_syntax_ && noms_pbs_.size() != 0 && nb_param != 1) throw;
  is >> nbcomp;

  fixer_nb_comp(nbcomp);
  is >> motlu;
  if (motlu == accolade_ouverte)
    {
      /* 1. lecture de la grille de parametres */
      VECT(DoubleVect) params;
      for (int n = 0; n < nb_param; n++)
        {
          const int nb_val = lire_dimension(is, que_suis_je());
          DoubleVect param(nb_val);
          for (int i = 0; i < nb_val; i++) is >> param[i];
          params.add(param);
        }

      /* 2. lecture des valeurs des parametres */
      // taille totale du tableau de valeurs
      int size = nbcomp;
      for (int n = 0; n < nb_param; n++) size *= params[n].size();

      // lecture : tout dans un tableau 1D
      DoubleVect tab_valeurs(size);
      for (int i = 0; i < size; i++) is >> tab_valeurs[i];
      la_table.remplir(params, tab_valeurs);

      is >> motlu;
      if (motlu != accolade_fermee)
        {
          Cerr << "Error reading from an object of type Champ_Fonc_Tabule" << finl;
          Cerr << "We expected keyword } instead of " << motlu << finl;
          exit();
        }
    }
  else if (motlu=="fonction")
    {
      Cerr<<"The syntax has changed..." << finl;
      Cerr<<"The syntax is now Champ_Fonc_fonction 1 field_expression"<<finl;
      exit();
      Cerr<<"We read the analytic function "<<finl;
      la_table.lire_f(is,0);
    }
  else
    {
      Cerr << "Error reading from an object of type Champ_Fonc_Tabule" << finl;
      Cerr << "We expected keyword { or fonction instead of " << motlu << finl;
      exit();
    }
  return is;
}

Champ_base& Champ_Fonc_Tabule::affecter_(const Champ_base& ch)
{
  if(!le_champ_tabule_discretise().non_nul())
    Cerr << le_nom() << "type : " << que_suis_je()
         << " can not be assigned to " << ch.le_nom()
         << " because " << le_nom() << " is incomplete " << finl;
  else
    {
      if(sub_type(Champ_Uniforme, ch))
        valeurs()=ch.valeurs()(0,0);
      else
        valeurs()=ch.valeurs();
    }
  return *this;
}
