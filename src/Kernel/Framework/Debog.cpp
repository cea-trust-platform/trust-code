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

#include <Champ_Inc.h>
#include <Debog_Pb.h>
#include <Debog.h>

void Debog::verifier(const char* const msg, double x)
{
  if (Debog_Pb::get_debog_instance().non_nul())
    Debog_Pb::get_debog_instance()->verifier(msg, x);
}

void Debog::verifier(const char* const msg, int x)
{
  if (Debog_Pb::get_debog_instance().non_nul())
    Debog_Pb::get_debog_instance()->verifier(msg, x);
}

void Debog::verifier_bord(const char* const msg, const DoubleVect& arr, int num_deb)
{
  // Ne fait rien
}

void Debog::verifier(const char* const msg, const Champ_Inc& x)
{
  if (Debog_Pb::get_debog_instance().non_nul())
    Debog_Pb::get_debog_instance()->verifier(msg, x->valeurs());
}

void Debog::verifier(const char* const msg, const Champ_Inc_base& x)
{
  if (Debog_Pb::get_debog_instance().non_nul())
    Debog_Pb::get_debog_instance()->verifier(msg, x.valeurs());
}

void Debog::set_nom_pb_actuel(const Nom& nom)
{
  if (Debog_Pb::get_debog_instance().non_nul())
    Debog_Pb::get_debog_instance()->set_nom_pb_actuel(nom);
}

// Description: teste le contenu du vecteur v en supposant qu'il contient des
//  indices d'items associes au descripteur md. Exemple: face_keps_imposee_
//  Les valeurs negatives doivent etre identiques a la reference,
//  les valeurs positives ou nulles sont interpretees comme un indice et
//  doivent etre egale a l'indice de reference apres renumerotation.
void Debog::verifier_indices_items(const char* const msg, const MD_Vector& md, const IntVect& v)
{
  // pas code
}

// Description: renvoie 1 si on est en mode Debog, 0 sinon
int Debog::active()
{
  return Debog_Pb::get_debog_instance().non_nul();
}

// Description: like verifier(), but, in "read&compare" mode, put the reference
//  value found in the file in the ref variable.
void Debog::verifier_getref(const char* const msg, double x, double& ref)
{
  if (Debog_Pb::get_debog_instance().non_nul())
    Debog_Pb::get_debog_instance()->verifier(msg, x, &ref);
}

// Description: like verifier(), but, in "read&compare" mode, put the reference
//  value found in the file in the ref variable.
void Debog::verifier_getref(const char* const msg, int x, int& ref)
{
  if (Debog_Pb::get_debog_instance().non_nul())
    Debog_Pb::get_debog_instance()->verifier(msg, x, &ref);
}

void Debog::verifier(const char* const msg, const DoubleVect& x)
{
  if (Debog_Pb::get_debog_instance().non_nul())
    Debog_Pb::get_debog_instance()->verifier(msg, x);
}

// Description: like verifier(), but, in "read&compare" mode, put the reference
//  value found in the file in the ref variable. x and ref can point to the same object.
void Debog::verifier_getref(const char* const msg, const DoubleVect& x, DoubleVect& ref)
{
  if (Debog_Pb::get_debog_instance().non_nul())
    Debog_Pb::get_debog_instance()->verifier(msg, x, &ref);
}

void Debog::verifier(const char* const msg, const IntVect& x)
{
  if (Debog_Pb::get_debog_instance().non_nul())
    Debog_Pb::get_debog_instance()->verifier(msg, x);
}

// Description: like verifier(), but, in "read&compare" mode, put the reference
//  value found in the file in the ref variable. x and ref can point to the same object.
void Debog::verifier_getref(const char* const msg, const IntVect& x, IntVect& ref)
{
  if (Debog_Pb::get_debog_instance().non_nul())
    Debog_Pb::get_debog_instance()->verifier(msg, x, &ref);
}

void Debog::verifier_Mat_elems(const char* const msg, const Matrice_Base& la_matrice)
{
  if (Debog_Pb::get_debog_instance().non_nul())
    Debog_Pb::get_debog_instance()->verifier_Mat_elems(msg, la_matrice);
}
