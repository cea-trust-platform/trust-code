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

#ifndef Format_Post_CGNS_included
#define Format_Post_CGNS_included

#include <TRUSTTabs_forward.h>
#include <Format_Post_base.h>

#include <cgns++.h>

class Format_Post_CGNS : public Format_Post_base
{
  Declare_instanciable_sans_constructeur(Format_Post_CGNS);
public:
  Format_Post_CGNS();

  void reset() override;
  void set_param(Param& param) override;
  int initialize_by_default(const Nom&) override;
  int initialize(const Nom&, const int, const Nom&) override;

  int completer_post(const Domaine&, const int, const Nature_du_champ&, const int, const Noms&, const Motcle&, const Nom&) override { return 1; }
  int preparer_post(const Nom&, const int, const int, const double) override { return 1; }

  int ecrire_entete(const double, const int, const int) override;
  int finir(const int) override;

  int ecrire_domaine(const Domaine&, const int) override;

private:
  Nom cgns_basename_;
  void ecrire_domaine_(const Domaine& );

#ifdef HAS_CGNS
  int fileId_ = -123, baseId_ = -123;
#endif
};

inline void verify_if_cgns(const char * nom_funct)
{
#ifdef HAS_CGNS
  return;
#else
  Cerr << "Format_Post_CGNS::" <<  nom_funct << " should not be called since TRUST is not compiled with the CGNS library !!! " << finl;
  Process::exit();
#endif
}

#endif /* Format_Post_CGNS_included */
