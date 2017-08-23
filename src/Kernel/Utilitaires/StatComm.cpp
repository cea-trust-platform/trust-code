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
// File:        StatComm.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////
#include <StatComm.h>
#include <Nom.h>
#include <SFichier.h>

static int* tab_comm;
static int prem=1;

StatComm::StatComm()
{
  SZ_MIN=32768;
  SZ_MAX=0;
  SZ_MOY=0.0;
  nb_msg=0;
}

void StatComm::stat(int size)
{
  if(prem==1)
    {
      tab_comm = new int[10000];
      for(int i=0; i<10000; i++)
        tab_comm[i]=0;
      prem=0;
    }
  if(size<10000)
    tab_comm[size]++;

  nb_msg++;
  if(size>SZ_MAX) SZ_MAX=size;
  if(size<SZ_MIN) SZ_MIN=size;
  SZ_MOY+=(double)size;
}

int StatComm::get_szmin()
{
  return SZ_MIN;
}
int StatComm::get_szmax()
{
  return SZ_MAX;
}
double StatComm::get_szmoy()
{
  if (nb_msg!=0)
    return ((double)(SZ_MOY/(double)nb_msg)) ;
  else
    return 0.0;
}
int StatComm::get_nbmsg()
{
  return nb_msg;
}

void StatComm::print_stat(Sortie& os)
{
  if (prem==0)
    {
      Nom nf("COMM_");
      nf+=Nom(Process::me());
      nf+=".trace";
      ofstream fic((const char *)nf,ios::out);
      for(int i=0; i<10000; i++)
        if(tab_comm[i]!=0)
          fic<<i<<"\t"<<tab_comm[i]<<std::endl;
    }
  os<<"SZ_MIN : "<<SZ_MIN<<finl;
  os<<"SZ_MAX : "<<SZ_MAX<<finl;
  os<<"SZ_MOY : "<<get_szmoy()<<finl;
  os<<"NB_MSG : "<<nb_msg<<finl;
}
