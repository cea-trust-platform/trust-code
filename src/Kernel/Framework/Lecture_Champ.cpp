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

#include <Lecture_Champ.h>
#include <Motcle.h>
#include <List_Nom.h>

Implemente_instanciable_sans_constructeur( Lecture_Champ, "Lecture_Champ", Objet_U ) ;

Lecture_Champ::Lecture_Champ()
{
  champs_lus_= false;
}

Sortie& Lecture_Champ::printOn( Sortie& os ) const
{
  Objet_U::printOn( os );
  return os;
}

Entree& Lecture_Champ::readOn( Entree& is )
{
  Objet_U::readOn( is );
  return is;
}

Entree& Lecture_Champ::lire_champs( Entree& is , List_Nom& noms_champs)
{

  Cerr << "Reading Fields\n";
  Nom nom;

  const Motcle accolade_ouverte = Motcle("{");
  const Motcle accolade_fermee = Motcle("}");

  is >> nom;

  if(Motcle(nom)!= accolade_ouverte)
    {
      Cerr << "We expected a { while reading " << que_suis_je() << finl;
      Cerr << "and not : " << nom << finl;
      Process::exit();
    }

  int lu = 0;

  while (1)
    {
      is >> nom;
      if (Motcle(nom) == accolade_fermee)
        break;

      if (noms_champs.contient(nom))
        {
          liste_champs.add(Champ());
          is >> liste_champs(lu);
          liste_champs(lu).nommer(nom);
          lu++;
        }
      else
        {
          Cerr << "Unknown field name : " << nom << finl;
          Cerr << "Expected names :" << finl;
          Cerr << noms_champs << finl;
          Process::exit();
        }
    }

  champs_lus_ =  lu==noms_champs.size();

  if (lu>0 && !champs_lus_)
    {
      Cerr << "Error while reading input fields." << finl;
      Cerr << "We expected "<< noms_champs.size() <<" fields." << finl;
      Process::exit();
    }

  return is;
}

