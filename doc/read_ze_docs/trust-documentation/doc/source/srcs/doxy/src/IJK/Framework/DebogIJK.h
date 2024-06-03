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

#ifndef DebogIJK_included
#define DebogIJK_included

#include <Interprete.h>
#include <EFichier.h>
#include <SFichier.h>
#include <IJK_Field_forward.h>
#include <TRUSTTabs_forward.h>

/*! @brief : class DebogIJK
 *
 *  <Description of class DebogIJK>
 *
 *
 *
 */
class DebogIJK : public Interprete
{
  Declare_instanciable(DebogIJK) ;

public :
  Entree& interpreter(Entree&) override;
  enum DebogMode { DISABLED=0, WRITE_PASS=1, CHECK_PASS=2 };
  static void verifier(const char *msg, const IJK_Field_float&);
  static void verifier(const char *msg, const IJK_Field_double&);
  static void verifier(const double);
protected :
  static double seuil_absolu_, seuil_relatif_, seuil_minimum_relatif_;
  static int debog_mode_;
  static Nom filename_;
  static EFichier infile_;
  static SFichier outfile_;
  static void compute_signature(const IJK_Field_float&, ArrOfDouble& signature);
  static void compute_signature(const IJK_Field_double&, ArrOfDouble& signature);
};

#endif /* DebogIJK_included */
