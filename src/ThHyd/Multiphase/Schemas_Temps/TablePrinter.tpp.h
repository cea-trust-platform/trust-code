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
#if defined(USE_BOOST_KARMA)
#include <boost/spirit/include/karma.hpp>
namespace karma = boost::spirit::karma;
#endif

#include <arch.h>

namespace bprinter
{
#if defined(USE_BOOST_KARMA)
template<typename T> void TablePrinter::OutputDecimalNumber(T input)
{
  *out_stream_ << karma::format(
                 karma::maxwidth(column_widths_.at(j_))[
                   karma::right_align(column_widths_.at(j_))[
                     karma::double_
                   ]
                 ], input
               );

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
}
#else
template<typename T> void TablePrinter::OutputDecimalNumber(T input)
{

  // determine what precision we need
  True_int precision = column_widths_.at(j_) - 1; // leave room for the decimal point
  if (input < 0)
    --precision; // leave room for the minus sign

  // leave room for digits before the decimal?
  if (input < -1 || input > 1)
    {
      True_int num_digits_before_decimal = 1 + (True_int)log10(std::abs(input));
      precision -= num_digits_before_decimal;
    }
  else
    precision --; // e.g. 0.12345 or -0.1234

  if (precision < 0)
    precision = 0; // don't go negative with precision

  *out_stream_ << std::setiosflags(std::ios::fixed)
               << std::setprecision(std::min(precision, 5))
               << std::setw(column_widths_.at(j_));
  out_stream_->unsetf(std::ios_base::floatfield);
  *out_stream_ << input;

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
}
#endif //USE_BOOST_KARMA
}
