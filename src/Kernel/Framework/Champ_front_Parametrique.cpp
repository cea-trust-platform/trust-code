/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Champ_front_Parametrique.h>
#include <Param.h>
#include <Probleme_base.h>
#include <EFichier.h>
#include <EChaine.h>


Implemente_instanciable(Champ_front_Parametrique,"Champ_front_Parametrique",Champ_front_base);
// XD Champ_front_Parametrique front_field_base Champ_front_Parametrique 0 Parametric boundary field

Sortie& Champ_front_Parametrique::printOn(Sortie& os) const
{
  return Champ_front_base::printOn(os);
}

Entree& Champ_front_Parametrique::readOn(Entree& is)
{
  Nom fichier;
  Param param(que_suis_je());
  param.ajouter("fichier", &fichier, Param::REQUIRED); // XD_ADD_P chaine Filename where boundary fields are read
  param.lire_avec_accolades_depuis(is);
  // Lecture de tous les lignes du fichier parametrique:
  EFichier fic(fichier);
  Motcle motlu;
  fic >> motlu;
  if (motlu!="{") Process::exit("Waiting { !");
  while (motlu!="}")
    {
      Champ_front ch;
      fic >> ch;
      champs_.add(ch);
      Cerr << "[Parameter] Reading: " << ch.valeur().que_suis_je() << finl; //" " << ch.valeur() << finl;
      fic >> motlu;
    }
  // On fixe le premier parametre:
  int compute = newCompute();
  assert(compute>0);
  Sortie_Fichier_base::set_root("calcul"+std::to_string(compute));
  return is;
}

int Champ_front_Parametrique::newCompute() const
{
  if (champs_.size()==index_)
    return 0;
  else
    {
      index_++;
      // ToDo ameliorer message avec les caracteristiques du champ
      Cerr << "================================================" << finl;
      Cerr << "[Parameter] Updating to: " << champ().valeur().que_suis_je() << finl; //" " << champ().valeur() << finl;
      Cerr << "================================================" << finl;
      return index_;
    }
}

void Champ_front_Parametrique::calculer_derivee_en_temps(double t1, double t2)
{
  if (t1<last_t2_ && index_>1)
    {
      // Pour assurer la continuite avec le champ precedent:
      if (std::abs(t2-t1) < DMINFLOAT)
        {
          Gpoint_ = 0;
        }
      else
        {
          const DoubleTab& v1 = champs_[index_-2]->valeurs_au_temps(last_t2_);
          const DoubleTab& v2 = valeurs_au_temps(t2);
          if (v1.dimension(0) == 1)
            {
              // Champ instationnaire uniforme
              int dim = v1.dimension(1);
              if (Gpoint_.size() != dim) Gpoint_.resize(dim);
              for (int i = 0; i < dim; i++)
                Gpoint_(i) = (v2(0, i) - v1(0, i)) / (t2 - t1);
            }
          else
            {
              // Champs instationnaire variable
              Gpoint_ = v1;
              Gpoint_ *= -1;
              Gpoint_ += v2;
              Gpoint_ /= (t2 - t1);
            }
        }
    }
  else
    {
      champ()->calculer_derivee_en_temps(t1, t2);
      Gpoint_ = champ()->derivee_en_temps();
    }
  last_t2_ = t2;
}

const DoubleTab& Champ_front_Parametrique::derivee_en_temps() const
{
  return Gpoint_;
}
