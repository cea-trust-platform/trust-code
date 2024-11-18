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

#include <Parser_U.h>
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
#define KOKKOS_IMPL_PUBLIC_INCLUDE
#include <Kokkos_UniqueToken.hpp>

class ParserView : public Parser
{
public:
  /**
  * Constructors (generally by copy of a ParserU)
  */
  ParserView(Parser_U& p) : Parser(p.getParser()) {}
  ParserView(std::string& expr, int nvar) : Parser(expr,nvar) {}
  /**
  * Parse string
  */
  void parseString() override
  {
    Parser::parseString();
    les_var_view = Kokkos::View<double**>("les_vars", getNbVar(), token.size());
    PNodes_view = Kokkos::View<PNodePod*>("PNodes device", PNodes.size());
    // Copy data from std::vector to the host mirror of the Kokkos View
    auto host_PNodes_view = Kokkos::create_mirror_view(PNodes_view);
    for (size_t i = 0; i < PNodes.size(); i++)
      host_PNodes_view(i) = PNodes[i];
    // Copy host mirror to device
    Kokkos::deep_copy(PNodes_view, host_PNodes_view);
  }
  KOKKOS_INLINE_FUNCTION void setVar(int i, double val, int threadId) const
  {
    assert(i>=0 && i<ivar);
    les_var_view(i, threadId) = val;
  }
  KOKKOS_INLINE_FUNCTION double eval(int threadId) const
  {
    return eval(PNodes_view[0], threadId);
  }
  /**
  * Token: get a unic threadId to fill safely les_var_view
  */
  int acquire() const { return token.acquire(); }
  void release(int threadId) const { token.release(threadId); }
private:
  Kokkos::Experimental::UniqueToken<Kokkos::DefaultExecutionSpace> token;
  Kokkos::View<double**> les_var_view;
  Kokkos::View<PNodePod*> PNodes_view;

  struct StackEntry
  {
    int node_idx;    // Index in PNodes_view array
    double result;
    int state;       // 0: new, 1: need right, 2: done
    bool is_root;    // To identify if this is the root node passed by reference
  };
  KOKKOS_INLINE_FUNCTION double eval(const PNodePod& node, int threadId) const
  {
    // Direct evaluation for simple cases of the input node
    if (node.type == 2) return node.nvalue;         // VALUE
    if (node.type == 4) return les_var_view(node.value, threadId); // VAR

    // Define a small stack structure for the device
    const int MAX_STACK = 64; // Adjust size as needed
    StackEntry stack[MAX_STACK];

    // Initialize stack with the input node state
    int stack_ptr = 0;
    stack[stack_ptr] = {0, 0.0, 0, true};  // Mark as root node

    while (stack_ptr >= 0)
      {
        StackEntry& current = stack[stack_ptr];
        const PNodePod& current_node = current.is_root ? node : PNodes_view[current.node_idx];

        switch (current_node.type)
          {
          case 2: // VALUE
            current.result = current_node.nvalue;
            stack_ptr--;
            break;

          case 4: // VAR
            current.result = les_var_view(current_node.value, threadId);
            stack_ptr--;
            break;

          case 1: // OP
            switch (current.state)
              {
              case 0: // Need to evaluate left
                if (current_node.left != -1)
                  {
                    current.state = 1;
                    stack_ptr++;
                    stack[stack_ptr] = {current_node.left, 0.0, 0, false};
                  }
                else
                  {
                    current.result = 0.0;
                    current.state = 1;
                  }
                break;

              case 1: // Need to evaluate right
                {
                  double left_result = (stack_ptr < MAX_STACK-1) ? stack[stack_ptr + 1].result : 0.0;
                  if (current_node.right != -1)
                    {
                      current.state = 2;
                      stack[stack_ptr].result = left_result; // Save left result
                      stack_ptr++;
                      stack[stack_ptr] = {current_node.right, 0.0, 0, false};
                    }
                  else
                    {
                      current.result = const_cast<ParserView*>(this)->evalOp(current_node, left_result, 0.0);
                      stack_ptr--;
                    }
                }
                break;

              case 2: // Both sides evaluated
                {
                  double right_result = stack[stack_ptr + 1].result;
                  current.result = const_cast<ParserView*>(this)->evalOp(current_node, current.result, right_result);
                  stack_ptr--;
                }
                break;
              }
            break;

          case 3: // FUNCTION
            if (current.state == 0)
              {
                if (current_node.value <= 0 && current_node.left != -1)
                  {
                    current.state = 1;
                    stack_ptr++;
                    stack[stack_ptr] = {current_node.left, 0.0, 0, false};
                  }
                else
                  {
                    current.result = const_cast<ParserView*>(this)->evalFunc(current_node, 0.0);
                    stack_ptr--;
                  }
              }
            else
              {
                double arg = stack[stack_ptr + 1].result;
                current.result = const_cast<ParserView*>(this)->evalFunc(current_node, arg);
                stack_ptr--;
              }
            break;

          default:
            Process::Kokkos_exit("method eval : Unknown type for this node !!!");
            return 0;
          }
      }

    return stack[0].result;
  }
};
#endif
