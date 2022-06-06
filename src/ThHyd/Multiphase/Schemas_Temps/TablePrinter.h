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
#ifndef TablePrinter_included
#define TablePrinter_included

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>

namespace bprinter
{
class endl {};
/** \class TablePrinter

  Print a pretty table into your output of choice.

  Usage:
    TablePrinter tp(&std::cout);
    tp.AddColumn("Name", 25);
    tp.AddColumn("Age", 3);
    tp.AddColumn("Position", 30);

    tp.PrintHeader();
    tp << "Dat Chu" << 25 << "Research Assistant";
    tp << "John Doe" << 26 << "Professional Anonymity";
    tp << "Jane Doe" << tp.SkipToNextLine();
    tp << "Tom Doe" << 7 << "Student";
    tp.PrintFooter();

  \todo Add support for padding in each table cell
  */
class TablePrinter
{
public:
  TablePrinter(std::ostream * output, const std::string& separator = "│");
  ~TablePrinter();

  int get_num_columns() const;
  int get_table_width() const;
  void set_separator(const std::string& separator);
  void set_flush_left();
  void set_flush_right();

  void AddColumn(const std::string& header_name, int column_width);
  void PrintHeader();
  void PrintFooter();

  TablePrinter& operator<<(endl input)
  {
    while (j_ != 0)
      {
        *this << "";
      }
    return *this;
  }

  // Can we merge these?
  TablePrinter& operator<<(float input);
  TablePrinter& operator<<(double input);

  template<typename T> TablePrinter& operator<<(T input)
  {
    if (j_ == 0)
      *out_stream_ << "│";

    if(flush_left_)
      *out_stream_ << std::left;
    else
      *out_stream_ << std::right;

    // Leave 3 extra space: One for negative sign, one for zero, one for decimal
    *out_stream_ << std::setw(column_widths_.at(j_))
                 << input;

    if (j_ == get_num_columns()-1)
      {
        *out_stream_ << "│\n";
        i_ = i_ + 1;
        j_ = 0;
      }
    else
      {
        *out_stream_ << separator_;
        j_ = j_ + 1;
      }

    return *this;
  }

private:
  void PrintHorizontalLine(int updown = 0);

  template<typename T> void OutputDecimalNumber(T input);

  std::ostream * out_stream_;
  std::vector<std::string> column_headers_;
  std::vector<int> column_widths_;
  std::string separator_;

  int i_; // index of current row
  int j_; // index of current column

  int table_width_;
  bool flush_left_;
};

}

#include <TablePrinter.tpp.h>
#endif
