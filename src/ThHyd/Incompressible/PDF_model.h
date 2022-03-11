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
// File:        PDF_model.h
// Directory:   $TRUST_ROOT/src/ThHyd/Incompressible
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef PDF_model_included
#define PDF_model_included

#include <Vect_Parser_U.h>
#include <Champ_Don.h>
#include <TRUSTTab.h>
#include <Zone_VF.h>
#include <Parser.h>
#include <Motcle.h>

/////////////////////////////////////////////////////////////////////////////
// .NAME        : PDF_model
// .HEADER      : genepi3 genepi3/src/Equations/Plaques
// .LIBRARY     : ?
// .DESCRIPTION : class PDF_model
//
// <Description of class PDF_model>
//
/////////////////////////////////////////////////////////////////////////////

class PDF_model : public Objet_U
{

  Declare_instanciable(PDF_model) ;

public :
  double get_vitesse_imposee(ArrOfDouble&,int);
  void affecter_vitesse_imposee(Zone_VF&, const DoubleTab&);
  double eta() const
  {
    return eta_;
  }
protected :
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  Champ_Don vitesse_imposee_lu_;
  Champ_Don vitesse_imposee_;
  double eta_;
  double coefku_;
  double temps_relax_;
  double echelle_relax_;
  int type_vitesse_imposee_;
  int local_;
  friend class Source_PDF_base;
  friend class Source_PDF_EF;
private:
  VECT(Parser_U) parsers_;
};

#endif /* PDF_model */
