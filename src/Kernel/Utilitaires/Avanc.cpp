/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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

#include <Avanc.h>
#include <Motcle.h>
#include <DerOu_.h>
#include <communications.h>

inline void erreur(const Nom& id)
{
  Cerr << finl;
  Cerr << "The restarting has failed. The tag " << id << " has not been" << finl;
  Cerr << "found in the restarting file. Check the restarting time in your data set." << finl;
  Process::exit();
}

extern void convert_to(const char *s, double& ob);
double extract_time_from_string(const Nom& ident_lu, const int digits)
{
  double temps;
  int length = ident_lu.longueur();
  convert_to(ident_lu.substr_old(length-digits,digits),temps);
  return temps;
}

// In the file fich, go up to the identifier id and return the time found
double avancer_fichier(Entree& fich, const Nom& id, const int read_header)
{
  // Possible evolution:
  // avancer_fichier(fich,id) -> fich.avancer_fichier(id);
  // Implementation of double Entree::seek(const Nom& id):
  // diffuse_ redescend dans LecFicDiffuseBase
  /* Surcharge par:
   double LecFicDiffuse::seek(const Nom& id)
   {
      double time;
      diffuse_=0;
      if (Process::je_suis_maitre()) time = Entree::seek(id);
      diffuse_=1;
      envoyer_broadcast(temps, 0);
      return time;
   }
  */
  double temps=0;
  fich.set_diffuse(0); // Entree::set_diffuse() set to 1, only Lec_Diffuse_base::set_diffuse() can set to 0
  if (fich.get_diffuse() || Process::je_suis_maitre())
    {
      Cerr << "Looking for the tag " << id << " in the file." << finl;
      Nom ident_lu;
      Nom type_lu;
      int digits=12; // Number of digits to write the time
      fich >> ident_lu;

      if (read_header)
        {
          if (ident_lu!="format_sauvegarde:")
            {
              digits=8; // Old time format
            }
          else
            {
              // New format
              int format;
              fich >> format;
              fich >> ident_lu;
            }
        }

      fich >> type_lu;
      while (Motcle(ident_lu) != Motcle(id))
        {
          DerObjU un_objet;
          un_objet.typer(type_lu);
          Cerr << "-> Passing object " << ident_lu << " ... " << flush;
          un_objet.reprendre(fich);
          Cerr << "OK" << finl;
          temps = extract_time_from_string(ident_lu, digits);
          if (!fich.get_istream()) erreur(id);
          fich >> ident_lu;
          if (id!=(Nom)"fin" && ident_lu == (Nom)"fin") erreur(id);
          fich >> type_lu;
        }
      Cerr << "-> Find object " << id << " and reading data in it ..." << finl;
    }
  envoyer_broadcast(temps, 0);
  fich.set_diffuse(1);
  return temps;
}

double get_last_time(Entree& fich)
{
  return avancer_fichier(fich, "fin", 1);
}
