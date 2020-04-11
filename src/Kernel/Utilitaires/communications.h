/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        communications.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#ifndef communications_H
#define communications_H


// .DESCRIPTION Methodes de communications
// Voir la documentation de envoyer_() dans communications.cpp
#include <Vect.h>
#include <vector>
class Objet_U;
class DoubleTabs;
class ArrOfInt;
class ArrOfDouble;
class DoubleTab;
class VECT(ArrOfInt);

int envoyer(const int&   t, int source, int cible, int canal );
int envoyer(const long&     t, int source, int cible, int canal );
int envoyer(const float&    t, int source, int cible, int canal );
int envoyer(const double&   t, int source, int cible, int canal );
int envoyer(const Objet_U& t, int source, int cible, int canal );

int envoyer(const int&   t, int cible, int canal );
int envoyer(const long&     t, int cible, int canal );
int envoyer(const float&    t, int cible, int canal );
int envoyer(const double&   t, int cible, int canal );
int envoyer(const Objet_U& t, int cible, int canal );

int recevoir(int& t, int source, int cible, int canal );
int recevoir(long&     t, int source, int cible, int canal );
int recevoir(float&    t, int source, int cible, int canal );
int recevoir(double&   t, int source, int cible, int canal );
int recevoir(Objet_U& t, int source, int cible, int canal );

int recevoir(int& t, int source, int canal );
int recevoir(long&     t, int source, int canal );
int recevoir(float&    t, int source, int canal );
int recevoir(double&   t, int source, int canal );
int recevoir(Objet_U& t, int source, int canal );

int envoyer_broadcast(int& t, int source);
int envoyer_broadcast(long&     t, int source);
int envoyer_broadcast(long long&     t, int source);
int envoyer_broadcast(float&    t, int source);
int envoyer_broadcast(double&   t, int source);
int envoyer_broadcast(Objet_U& t, int source);

int mppartial_sum(int i);
long long mppartial_sum(long long i);
int mp_max(int i);
int mp_min(int i);
void mpsum_multiple(double& x1, double& x2);

void mp_sum_for_each_item(ArrOfInt&);
void mp_sum_for_each_item(ArrOfDouble&);
void mp_max_for_each_item(ArrOfInt&);
void mp_max_for_each_item(ArrOfDouble&);
void mp_min_for_each_item(ArrOfInt&);
void mp_min_for_each_item(ArrOfDouble&);

int envoyer_all_to_all(const DoubleTabs& src, DoubleTabs& dest);
int envoyer_all_to_all(const VECT(ArrOfInt) & src, VECT(ArrOfInt) & dest);
int envoyer_all_to_all(const ArrOfInt& src, ArrOfInt& dest);
int envoyer_all_to_all(const ArrOfDouble& src, ArrOfDouble& dest);
int envoyer_all_to_all(std::vector<long long>& src, std::vector<long long>& dest);
void   envoyer_all_to_all(const DoubleTab& src, DoubleTab& dest);

int reverse_send_recv_pe_list(const ArrOfInt& src_list, ArrOfInt& dest_list);

int comm_check_enabled();
void assert_parallel(const double x);
void assert_parallel(const int x);
void assert_parallel(const Objet_U& obj);
int is_parallel_object(const double x);
int is_parallel_object(const int x);
int is_parallel_object(const Objet_U& obj);

#endif
