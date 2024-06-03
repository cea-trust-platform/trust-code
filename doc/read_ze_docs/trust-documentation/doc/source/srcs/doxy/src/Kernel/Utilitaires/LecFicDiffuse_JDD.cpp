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

#include <LecFicDiffuse_JDD.h>
#include <communications.h>
#include <EFichier.h>
#include <SChaine.h>
#include <Type_Verifie.h>

Implemente_instanciable_sans_constructeur(LecFicDiffuse_JDD,"LecFicDiffuse_JDD",Lec_Diffuse_base);

Entree& LecFicDiffuse_JDD::readOn(Entree& s)
{
  throw;
}

Sortie& LecFicDiffuse_JDD::printOn(Sortie& s) const
{
  throw;
}

LecFicDiffuse_JDD::LecFicDiffuse_JDD() :
  apply_verif(true)
{
  // file_.set_error_action(ERROR_CONTINUE);
}

/*! @brief ouverture du fichier name.
 *
 * Cette methode doit etre appelee sur tous les processeurs. En cas
 *   d'echec : exit()
 *
 */
LecFicDiffuse_JDD::LecFicDiffuse_JDD(const char* name,
                                     IOS_OPEN_MODE mode,
                                     bool apply_verification) :
  apply_verif(apply_verification)
{

  //file_.set_error_action(ERROR_CONTINUE);
  int ok = ouvrir(name, mode);
  if (!ok && Process::je_suis_maitre())
    {
      Cerr << "File " << name << " does not exist (LecFicDiffuse_JDD)" << finl;
      Process::exit();
    }
}


/*! @brief Ouverture du fichier.
 *
 * Cette methode doit etre appelee par tous les processeurs du groupe.
 *  Valeur de retour: 1 si ok, 0 sinon
 *
 */
int LecFicDiffuse_JDD::ouvrir(const char* name,
                              IOS_OPEN_MODE mode)
{
  int ok = 0;
  if(Process::je_suis_maitre())
    {
      Cout <<"Reading data file "<<finl;
      int fin_lu=0;
      Motcle fin("fin|end");
      EFichier file_;
      ok = file_.ouvrir(name, mode);
      if (!ok )
        {
          Cerr << "File " << name << " does not exist (LecFicDiffuse_JDD)" << finl;
          Process::exit();
        }

      SChaine prov;
      Nom motlu;
      Nom comments="";
      file_>>motlu;
      int nb_accolade=0;
      int nb_accolade_sa=-1;
      int line=1;
      while (1)
        {
          if (file_.eof())
            {
              break;
            }
          if (motlu=="{")
            nb_accolade++;
          if (motlu=="}")
            nb_accolade--;
          if (fin==motlu)
            {
              fin_lu=1;
              nb_accolade_sa=nb_accolade;
            }
          if (motlu=="#")
            {
              // Cerr<<" on passe les commentaires"<<finl;
              comments += "[" + std::string(name) + "] : Comments bloc starts at line " + std::to_string(line)+ " ";
              int jol = file_.jumpOfLines();
              for(int jump=0; jump<jol; jump++)
                prov <<"\n";
              line+=jol;
              file_>>motlu;
              while (motlu!="#")
                {
                  if (file_.eof())
                    {
                      Cerr << comments;
                      Nom msg = "\nProblem while reading some # \n # not closed.\n";
                      if (fin_lu)
                        msg+="Keyword Fin is maybe already read.\n";
                      msg+= "============================================\nExiting TRUST.";
                      Process::exit(msg);
                    }
                  jol = file_.jumpOfLines();
                  for(int jump=0; jump<jol; jump++)
                    prov <<"\n";
                  line += jol;
                  file_ >> motlu;
                }
              comments += "and ends at line " + std::to_string(line)+"\n";
              jol = file_.jumpOfLines();
              for(int jump=0; jump<jol; jump++)
                prov <<"\n";
              line += jol;

            }

          else if (motlu=="/*")
            {
              // Cerr<<" on passe le bloc de commentaires"<<finl;
              int ouvrante=1;
              comments += "[" + std::string(name) + "] : Comments bloc starts at line " + std::to_string(line) + " ";
              while (ouvrante!=0)
                {
                  int jol = file_.jumpOfLines();
                  for(int jump=0; jump<jol; jump++)
                    prov <<"\n";
                  line+=jol;
                  file_ >>motlu;
                  if (file_.eof())
                    {
                      Cerr << comments;
                      Nom msg = "\nProblem while reading some /* \n not closed.\n";
                      if (fin_lu)
                        msg+="Keyword Fin is maybe already read.\n";
                      msg+= "============================================\nExiting TRUST.";
                      Process::exit(msg);
                    }
                  if (motlu=="/*") ouvrante++;
                  if (motlu=="*/") ouvrante--;
                }
              comments += "and ends at line " + std::to_string(line)+"\n";
              int jol = file_.jumpOfLines();
              for(int jump=0; jump<jol; jump++)
                prov <<"\n";
              line+=jol;
            }

          else if ((motlu.find("}")!=-1 && motlu != "}") || (motlu.find("{")!=-1 && motlu != "{") )
            {
              Nom msg = "Error while reading '" + motlu.getString() + "' from datafile " + name + " at line " + std::to_string(line) + ".\nCheck for missing space character.\n";
              msg+= "============================================\nExiting TRUST.";
              Process::exit(msg);
            }

          /* GF ne pas reactiver ce morceau de code car cela
             est complique pour la traduction des jdd en python
             else if (motlu=="##")
             {

             Cerr<<" The comment line is crossed."<<finl;
             std::string ligne;
             std::getline(file_.get_ifstream(), ligne);

             }
          */
          else
            {
              prov<<motlu;
              int jol = file_.jumpOfLines();
              if(jol==0)
                prov << " ";
              else
                for(int jump=0; jump<jol; jump++)
                  prov <<"\n";
              line+=jol;
              if( apply_verif )
                verifie(motlu);
            }
          file_>>motlu;

        }
      if (fin_lu!=0)
        nb_accolade=nb_accolade_sa;
      if (nb_accolade!=0)
        {
          if (nb_accolade<0)
            Cerr<<"Error perhaps  extra \"}\" in data file"<<finl;
          else
            Cerr<<"Error check for missing \"}\" in data file"<<finl;
          Process::exit();
        }

      chaine_.init(prov.get_str());
      Cout <<finl;

      Process::Journal()<<"JDD interpreted: "<<finl<<prov.get_str()<<finl<<finl;
    }
  envoyer_broadcast(ok, 0);
  return ok;
}

Entree& LecFicDiffuse_JDD::get_entree_master()
{
  return chaine_;
}
