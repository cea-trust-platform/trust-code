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

#ifndef ParserView_included
#define ParserView_included

#include <Parser.h>
#include <TRUST_Deriv.h>
#include <TRUST_List.h>
#include <Constante.h>
#include <algorithm>
#include <Stack.h>
#include <math.h>
#include <string>
#include <kokkos++.h>
#include <Noms.h>
#include <TRUSTArray.h>
#include <kokkos++.h>

class ParserView : public Parser
{
public:
  ParserView(std::string& expr, int n =1) : Parser(expr,n)
  {
    setNbVar(n); // ToDo fix virtual call in constructor !
  }
  void setNbVar(int nvar) override
  {
    Parser::setNbVar(nvar);
    les_var_view = les_var.view_rw();
  }
  void parseString() override
  {
    Parser::parseString();
    PNodes_view = Kokkos::View<PNodePod*>("PNodes device", PNodes.size());
    // Copy data from std::vector to the host mirror of the Kokkos View
    auto host_PNodes_view = Kokkos::create_mirror_view(PNodes_view);
    for (size_t i = 0; i < PNodes.size(); i++)
      host_PNodes_view(i) = PNodes[i];
    // Copy host mirror to device
    Kokkos::deep_copy(PNodes_view, host_PNodes_view);
  }
  KOKKOS_INLINE_FUNCTION void setVar(int i, double val) const
  {
    assert(i>=0 && i<ivar);
    les_var_view[i] = val;
  }
  KOKKOS_INLINE_FUNCTION double eval() const { return eval(PNodes_view[0]); }
private:
  KOKKOS_INLINE_FUNCTION double eval(const PNodePod& node) const
  {
    switch(node.type)
      {
      case 1 :
        return const_cast<ParserView*>(this)->evalOp(node);  // PNode_type::OP
      case 2 :
        return node.value;  // PNode_type::VALUE
      case 3 :
        return const_cast<ParserView*>(this)->evalFunc(node);// PNode_type::FUNCTION
      case 4 :
        return les_var_view[node.value]; // PNode_type::VAR
      default:
        Process::Kokkos_exit("method eval : Unknown type for this node !!!");
        return 0;
      }
  }
  DoubleArrView les_var_view;
  Kokkos::View<PNodePod*> PNodes_view;
};
#endif
