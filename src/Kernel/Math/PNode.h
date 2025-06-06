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
#ifndef PNode_included
#define PNode_included
#include <arch.h>

enum PNode_type { NOTHING=-1, OP=1, VALUE=2, FUNCTION=3, VAR=4, PARAMETER=5 };
class PNode
{
public :
  // static variables are on host, enum are constant expressions available host&device
  /*
  static const int NOTHING;
  static const int OP;
  static const int VALUE;
  static const int FUNCTION;
  static const int VAR;
  static const int PARAMETER;
   */

  PNode* left;
  PNode* right;
  int type;
  True_int value;
  double nvalue = -100.;

  PNode();
  PNode(PNode*);
  void replace(PNode*);

private :
};

class PNodePod
{
public :
  int left;
  int right;
  int type;
  True_int value;
  double nvalue = -100.;

//    PNodePod();
//    PNodePod(PNodePod*);
//  void replace(PNodePod*);

private :
};

#endif
