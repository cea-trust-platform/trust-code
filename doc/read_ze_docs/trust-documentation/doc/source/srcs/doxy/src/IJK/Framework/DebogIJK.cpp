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


#include <DebogIJK.h>
#include <IJK_Field.h>
#include <Param.h>
#include <TRUSTTabs.h>
#include <communications.h>

Implemente_instanciable(DebogIJK,"DebogIJK",Interprete) ;

int DebogIJK::debog_mode_ = DebogIJK::DISABLED;
Nom DebogIJK::filename_;
EFichier DebogIJK::infile_;
SFichier DebogIJK::outfile_;
double DebogIJK::seuil_absolu_ = 1e-4;
double DebogIJK::seuil_relatif_ = 1e-8;
double DebogIJK::seuil_minimum_relatif_ = 1e-4;

Sortie& DebogIJK::printOn(Sortie& os) const
{
  Objet_U::printOn(os);
  return os;
}

Entree& DebogIJK::readOn(Entree& is)
{
  return is;
}

Entree& DebogIJK::interpreter(Entree& is)
{
  filename_ = "DEBOG.txt";
  Param param(que_suis_je());
  param.ajouter("mode", &debog_mode_);
  param.dictionnaire("disabled", (int)DISABLED);
  param.dictionnaire("write_pass", (int)WRITE_PASS);
  param.dictionnaire("check_pass", (int)CHECK_PASS);
  param.ajouter("filename", &filename_);
  param.ajouter("seuil_absolu", &seuil_absolu_);
  param.ajouter("seuil_relatif", &seuil_relatif_);
  param.ajouter("seuil_minimum_relatif", &seuil_minimum_relatif_);
  param.lire_avec_accolades(is);

  if (je_suis_maitre())
    {
      if (debog_mode_ == WRITE_PASS)
        outfile_.ouvrir(filename_);
      if (debog_mode_ == CHECK_PASS)
        infile_.ouvrir(filename_);
    }
  return is;
}

// Calcule une signature numerique du champ. On veut une signature qui varie
// lineairement avec les valeurs du champ et qui detecte de petites variations.
// On va calculer quelques sommes ponderees des valeurs du champ avec des
// ponderations pseudo-aleatoires.
void DebogIJK::compute_signature(const IJK_Field_float& field, ArrOfDouble& signature)
{
  const int sig_size = 5;
  signature.resize_array(sig_size);
  ArrOfDouble facteurs(sig_size);

  // Generation de facteurs, nombres irrationnels et non multiples entre eux
  facteurs[0] = 1.35914091422952;
  for (int i = 1; i < sig_size; i++)
    facteurs[i] = facteurs[i-1] * facteurs[0];

  for (int i = 0; i < sig_size; i++)
    signature[i] = 0.;

  VECT(DoubleTab) sig_factors(3);
  for (int dir = 0; dir < 3; dir++)
    {
      const int n = field.nb_elem_local(dir);
      sig_factors[dir].resize(n, sig_size);
      const int offset = field.get_splitting().get_offset_local(dir);
      for (int i = 0; i < n; i++)
        for (int j = 0; j < sig_size; j++)
          sig_factors[dir](i,j) = cos(facteurs[j] * (i+offset));
    }

  const int ni = field.ni();
  const int nj = field.nj();
  const int nk = field.nk();
  for (int k = 0; k < nk; k++)
    {
      for (int j = 0; j < nj; j++)
        {
          for (int i = 0; i < ni; i++)
            {
              double data = field(i,j,k);
              for (int l = 0; l < sig_size; l++)
                {
                  signature[l] += data * sig_factors[0](i,l) * sig_factors[1](j,l) * sig_factors[2](k,l);
                }
            }
        }
    }
  mp_sum_for_each_item(signature);
}

void DebogIJK::verifier(const char *msg, const IJK_Field_float& f)
{
  if (debog_mode_ == DISABLED)
    return;
  ArrOfDouble sig;
  compute_signature(f, sig);
  if (Process::je_suis_maitre())
    {
      Nom s("");
      char ss[1000];
      for (int i = 0; i < sig.size_array(); i++)
        {
          snprintf(ss, 1000, "%20.13g ", sig[i]);
          s += ss;
        }
      s += msg;
      Journal() << "DEBOG1:" << s << finl;
      if (debog_mode_ == WRITE_PASS)
        outfile_ << s << finl;
      else
        {
          ArrOfDouble sig2(sig.size_array());
          Nom s2("");
          for (int i = 0; i < sig.size_array(); i++)
            {
              infile_ >> sig2[i];
              snprintf(ss, 1000, "%20.13g ", sig2[i]);
              s2 += ss;
            }
          std::string ligne;
          std::getline(infile_.get_ifstream(), ligne);
          Journal() << "DEBOG2:" << s2 << ligne.c_str() << finl;

          bool erreur = false;
          for (int i = 0; i < sig.size_array(); i++)
            {
              double m = std::max(fabs(sig[i]),fabs(sig2[i]));
              m = std::max(seuil_minimum_relatif_, m);
              if (fabs(sig[i] - sig2[i]) > seuil_absolu_
                  || fabs(sig[i] - sig2[i]) / m > seuil_relatif_)
                erreur = true;
            }
          if (erreur)
            {
              Cerr << "DEBOG: erreur" << finl << "THIS:" << s << finl << "REF: " << s2 << finl;
            }
        }
    }
}
// Calcule une signature numerique du champ. On veut une signature qui varie
// lineairement avec les valeurs du champ et qui detecte de petites variations.
// On va calculer quelques sommes ponderees des valeurs du champ avec des
// ponderations pseudo-aleatoires.
void DebogIJK::compute_signature(const IJK_Field_double& field, ArrOfDouble& signature)
{
  const int sig_size = 5;
  signature.resize_array(sig_size);
  ArrOfDouble facteurs(sig_size);

  // Generation de facteurs, nombres irrationnels et non multiples entre eux
  facteurs[0] = 1.35914091422952;
  for (int i = 1; i < sig_size; i++)
    facteurs[i] = facteurs[i-1] * facteurs[0];

  for (int i = 0; i < sig_size; i++)
    signature[i] = 0.;

  VECT(DoubleTab) sig_factors(3);
  for (int dir = 0; dir < 3; dir++)
    {
      const int n = field.nb_elem_local(dir);
      sig_factors[dir].resize(n, sig_size);
      const int offset = field.get_splitting().get_offset_local(dir);
      for (int i = 0; i < n; i++)
        for (int j = 0; j < sig_size; j++)
          sig_factors[dir](i,j) = cos(facteurs[j] * (i+offset));
    }

  const int ni = field.ni();
  const int nj = field.nj();
  const int nk = field.nk();
  for (int k = 0; k < nk; k++)
    {
      for (int j = 0; j < nj; j++)
        {
          for (int i = 0; i < ni; i++)
            {
              double data = field(i,j,k);
              for (int l = 0; l < sig_size; l++)
                {
                  signature[l] += data * sig_factors[0](i,l) * sig_factors[1](j,l) * sig_factors[2](k,l);
                }
            }
        }
    }
  mp_sum_for_each_item(signature);
}

void DebogIJK::verifier(const char *msg, const IJK_Field_double& f)
{
  if (debog_mode_ == DISABLED)
    return;
  ArrOfDouble sig;
  compute_signature(f, sig);
  if (Process::je_suis_maitre())
    {
      Nom s("");
      char ss[1000];
      for (int i = 0; i < sig.size_array(); i++)
        {
          snprintf(ss, 1000, "%20.13g ", sig[i]);
          s += ss;
        }
      s += msg;
      Journal() << "DEBOG1:" << s << finl;
      if (debog_mode_ == WRITE_PASS)
        outfile_ << s << finl;
      else
        {
          ArrOfDouble sig2(sig.size_array());
          Nom s2("");
          for (int i = 0; i < sig.size_array(); i++)
            {
              infile_ >> sig2[i];
              snprintf(ss, 1000, "%20.13g ", sig2[i]);
              s2 += ss;
            }
          std::string ligne;
          std::getline(infile_.get_ifstream(), ligne);
          Journal() << "DEBOG2:" << s2 << ligne.c_str() << finl;

          bool erreur = false;
          for (int i = 0; i < sig.size_array(); i++)
            {
              double m = std::max(fabs(sig[i]),fabs(sig2[i]));
              m = std::max(seuil_minimum_relatif_, m);
              if (fabs(sig[i] - sig2[i]) > seuil_absolu_
                  || fabs(sig[i] - sig2[i]) / m > seuil_relatif_)
                erreur = true;
            }
          if (erreur)
            {
              Cerr << "DEBOG: erreur" << finl << "THIS:" << s << finl << "REF: " << s2 << finl;
            }
        }
    }
}

