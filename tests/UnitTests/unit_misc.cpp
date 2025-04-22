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

#pragma GCC diagnostic push
#if __GNUC__ < 9
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif
#include <gtest/gtest.h>
#pragma GCC diagnostic pop

#include <Motcle.h>
#include <map>

//Nom as key to std::map:
TEST(Misc, NomMap) {
    std::map<Nom, int> my_map;
    my_map["toto"] = 3;
    const int b = my_map.at("toto"); // was throwing !! because no operator '<' on Nom

    EXPECT_EQ(b, 3);

}
//Motcle as key to std::map:
TEST(Misc, MotCleMap) {

    // Same thing with Motcle but we can ignore case:
    std::map<Motcle, int> my_map;
    my_map["toto"] = 3;
    const int b = my_map.at("ToTo"); // was throwing !! because no operator '<' on Nom

    EXPECT_EQ(b, 3);
}