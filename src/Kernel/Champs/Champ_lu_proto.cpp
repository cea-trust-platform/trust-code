/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Champ_lu_proto.h>
#include <EntreeSortie.h>
#include <sstream>
#include <fstream>
#include <vector>

inline int tokenize_string_if_double(const std::string line, std::vector<std::string>& result)
{
  std::istringstream iss(line);
  for (std::string s; iss >> s;)
    try
      {
        std::stod(s);
        result.push_back(s);
      }
    catch (std::exception&) { return 0; }
  return 1;
}


Nom Champ_lu_proto::put_file_into_nom(int nb_comp, const Nom& column_file) const
{
  // open column file
  std::ifstream infile(column_file);
  Nom liste_temps(""), liste_vals("");

  // read line by line
  int nb_val = 0;
  std::string line;
  for (int l = 1; std::getline(infile, line); l++)
    if (!(line.rfind("#", 0) == 0))
      {
        nb_val++;
        std::vector<std::string> result;
        if (!tokenize_string_if_double(line, result))
          {
            Cerr << "The file " << column_file << " contains non double values at line " << l << " !" << finl;
            Process::exit();
          }
        const int nb_col_lues = (int)result.size();
        if (nb_col_lues != (nb_comp + 1))
          {
            Cerr << "The file " << column_file << " should contain " << nb_comp + 1 << " columns but " << nb_col_lues << " have been detected on line " << l << finl;
            Process::exit();
          }
        liste_temps += Nom(" ") + result[0];
        for (int n = 0; n < nb_comp; n++)
          liste_vals += Nom(" ") + result[1 + n];
      }

  // send to the parent class readOn
  Nom chaine_tabulee(" { ");
  chaine_tabulee += Nom(nb_val) + liste_temps + liste_vals + Nom(" }");
  Cerr << "Creation from " << column_file << " of the field" << finl;
  Cerr << "Champ_front_Tabule " << chaine_tabulee << finl;

  return chaine_tabulee;
}
