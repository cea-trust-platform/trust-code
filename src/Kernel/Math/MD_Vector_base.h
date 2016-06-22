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
//////////////////////////////////////////////////////////////////////////////
//
// File:        MD_Vector_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MD_Vector_base_included
#define MD_Vector_base_included
#include <Objet_U.h>

class MD_Vector;
class Echange_EV_Options;
class ArrOfInt;
class IntVect;
class DoubleVect;
class Schema_Comm_Vecteurs;

/////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//////////////////////////////////////////////////////////////

// .DESCRIPTION
// Classe de base des descripteurs de vecteurs distribues.
// Voir la documentation des methodes dans MD_Vector_base2 et MD_Vector_std
class MD_Vector_base : public Objet_U
{
  Declare_base_sans_constructeur_ni_destructeur(MD_Vector_base);
public:
  MD_Vector_base() : ref_count_(0) {};
  // attention, ne pas copier le ref_count_ !
  MD_Vector_base(const MD_Vector_base& ) : Objet_U(),ref_count_(0) {};
  ~MD_Vector_base() {};
  virtual int get_nb_items_reels() const = 0;
  virtual int get_nb_items_tot() const = 0;
  virtual void initialize_comm(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, DoubleVect&) const = 0;
  virtual void prepare_send_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, DoubleVect&) const = 0;
  virtual void process_recv_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, DoubleVect&) const = 0;
  virtual void initialize_comm(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, IntVect&) const = 0;
  virtual void prepare_send_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, IntVect&) const = 0;
  virtual void process_recv_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs&, IntVect&) const = 0;
  virtual const ArrOfInt& get_items_to_compute() const = 0;
  virtual const ArrOfInt& get_items_to_sum() const = 0;
  virtual int           nb_items_seq_tot() const = 0;
  virtual int           nb_items_seq_local() const = 0;
private:
  // Cette classe a le droit de modifier ref_count_:
  friend class MD_Vector;
  // Combien d'objets de type MD_Vector pointent sur cette instance ?
  mutable int ref_count_;
};
#endif
