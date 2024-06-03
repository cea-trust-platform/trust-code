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
#include <TablePrinter.h>
#include <stdexcept>
#include <iomanip>
#include <stdexcept>

namespace bprinter
{
TablePrinter::TablePrinter(std::ostream * output, const std::string& separator)
{
  out_stream_ = output;
  i_ = 0;
  j_ = 0;
  separator_ = separator;
  flush_left_ = false;
}

TablePrinter::~TablePrinter()
{

}

int TablePrinter::get_num_columns() const
{
  return (int)column_headers_.size();
}

void TablePrinter::set_separator(const std::string& separator)
{
  separator_ = separator;
}

void TablePrinter::set_flush_left()
{
  flush_left_ = true;
}

void TablePrinter::set_flush_right()
{
  flush_left_ = false;
}

/** \brief Add a column to our table
 **
 ** \param header_name Name to be print for the header
 ** \param column_width the width of the column (has to be >=5)
 ** */
void TablePrinter::AddColumn(const std::string& header_name, int column_width)
{
  if (column_width < 4)
    {
      throw std::invalid_argument("Column size has to be >= 4");
    }

  column_headers_.push_back(header_name);
  column_widths_.push_back(column_width);
}

void TablePrinter::PrintHorizontalLine(int updown)
{
  *out_stream_ << (updown > 0 ? "┌" : updown < 0 ? "└" : "├"); // the left bar

  for (int i = 0; i < (int) column_widths_.size(); i++)
    for (int j = 0; j < column_widths_[i]; j++)
      *out_stream_ << (!i || j ? "─" : updown > 0 ? "┬─" : updown < 0 ? "┴─" : "┼─");

  *out_stream_ << (updown > 0 ? "┐" : updown < 0 ? "┘" : "┤"); // the right bar
  *out_stream_ << "\n";
}

void TablePrinter::PrintHeader()
{
  PrintHorizontalLine(1);
  *out_stream_ << "│";

  for (int i=0; i<get_num_columns(); ++i)
    {

      if(flush_left_)
        *out_stream_ << std::left;
      else
        *out_stream_ << std::right;

      *out_stream_ << std::setw(column_widths_.at(i)) << column_headers_.at(i).substr(0, column_widths_.at(i));
      if (i != get_num_columns()-1)
        {
          *out_stream_ << separator_;
        }
    }

  *out_stream_ << "│\n";
  PrintHorizontalLine();
}

void TablePrinter::PrintFooter()
{
  PrintHorizontalLine(-1);
}

TablePrinter& TablePrinter::operator<<(float input)
{
  OutputDecimalNumber<float>(input);
  return *this;
}

TablePrinter& TablePrinter::operator<<(double input)
{
  OutputDecimalNumber<double>(input);
  return *this;
}

}
