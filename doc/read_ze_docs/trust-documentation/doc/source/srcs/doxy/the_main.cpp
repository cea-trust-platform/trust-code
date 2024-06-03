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

#include <MAIN.h>
#include <mon_main.h>

/*! \mainpage Welcome to the TRUST C++ API Doxygen Documentation !
*
*
* TRUST is a thermalhydraulic software package for CFD simulations.
* This software was originally designed for conduction, incompressible single-phase, and Low Mach Number (LMN) flows with a robust Weakly-Compressible (WC) multi-species solver.
* However, a huge effort has been conducted recently, and now TRUST is able to simulate real compressible multi-phase flows.
* TRUST is also being progressively ported to support GPU acceleration (NVidia/AMD).
* The software is OpenSource (BSD license).
*
*
* To visit the CEA TRUST Platform's organization on Github, see: https://github.com/cea-trust-platform
*
* Have a look at the platform's website available here : https://cea-trust-platform.github.io
*
* To go back to the main page of the TRUST Documentation project, click here : https://cea-trust-platform.readthedocs.io
*/

True_int main(True_int argc, char** argv)
{

  mon_main* p=NULL;

  int st= main_TRUST(argc,argv,p,0);
  if (p)
    delete p;
  return st;
}
