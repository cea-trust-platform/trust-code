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
// File:        med++.cpp
// Directory:   $TRUST_ROOT/src/Kernel/MEDimpl
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////
#include <med++.h>
#include <string.h>
#ifdef MED23
//extern
med_err MEDmaaInfo(med_idt fid,int i,char* maa,med_int* dim)
{
  med_maillage med_mail;
  char * desc= new char[MED_COMMENT_SIZE+1];
  med_err ret=MEDmaaInfo( fid, i, maa, dim,&med_mail,desc);
  delete [] desc;
  return ret;
}



med_err
MEDmaaCr(med_idt fid, char *maillage, med_int dim)
{
  med_maillage type;
  char * desc= new char[MED_COMMENT_SIZE+1];
  strcpy(desc,"no description");
  type=MED_NON_STRUCTURE;
  med_err ret= MEDmaaCr(fid,maillage,dim,type,desc);
  delete [] desc;
  return ret;
}



med_err
MEDchampLire(med_idt fid,char *maa, char *cha, unsigned char *val,med_mode_switch interlace,med_int numco,
             char *profil,med_entity_type type_ent, med_geometry_type type_geo,
             med_int numdt, med_int numo)
{
  char* locname=new char[MED_NAME_SIZE+1];
  med_err ret= MEDchampLire(fid,maa,cha,val,interlace,numco,locname,profil,MED_NO_PFLMOD,type_ent,type_geo,numdt,numo);
  delete [] locname;
  return ret;
}




#endif
