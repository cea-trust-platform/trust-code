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

#include <LataJournal.h>
#include <LataLoader.h>
#include <LmlReader.h>
#include <iostream>
#include <fstream>
#include <string>

#include <MEDCouplingRefCountObject.hxx>
#include <MEDCouplingFieldDouble.hxx>
#include <MEDCouplingMemArray.hxx>
#include <MEDCouplingUMesh.hxx>

using namespace MEDCoupling;

int main(int argc,char ** argv)
{
  cerr<<"Usage : "<<argv[0]<<": file.lata field_name [ numero_temps ] [ numero_bock ] "<<endl;
  LataLoader toto(argv[1]);
  int numero_temps=-1;
  int nblock=-1;
  if (argc>=4) numero_temps=atoi(argv[3]);
  if (argc==5) nblock=atoi(argv[4]);
  MCAuto<MEDCouplingFieldDouble> field= toto.GetFieldDouble(argv[2],numero_temps,nblock);
  cerr<< field->getNumberOfValues()<< " "<<field->getArray()->getPointer()[0]<<endl;

  return 0;
}
