/*
    Copyright (C) 2016-2019, 2023 University College London
    This file is part of STIR.

    SPDX-License-Identifier: Apache-2.0

    See STIR/LICENSE.txt for details
*/
/*!

  \file
  \ingroup listmode
  \ingroup IO
  \ingroup GE
  \brief Implementations of class stir::GE::RDF8::IO::GERDF8ListmodeInputFileFormat

  \author Kris Thielemans
  \author Ottavia Bertolli
*/
#include "stir/warning.h"
#include "stir/ByteOrder.h"
#include "stir/listmode/CListModeDataGERDF8.h"
#include "stir/IO/GERDF8ListmodeInputFileFormat.h"

#include <string>

START_NAMESPACE_STIR

namespace GE
{
namespace RDF8
{
bool
GERDF8ListmodeInputFileFormat::actual_can_read(const FileSignature& signature, std::istream& input) const
{
  auto word = *reinterpret_cast<std::uint32_t const * const>(signature.get_signature());
  if (word==65279)
    return true;
  // try byteswap
  ByteOrder::swap_order(word);
  return word==65279;
}

std::unique_ptr<ListModeData>
GERDF8ListmodeInputFileFormat::read_from_file(std::istream& input) const
{
  warning("read_from_file for GERDF8 listmode data with istream not implemented %s:%s. Sorry", __FILE__, __LINE__);
  return std::unique_ptr<ListModeData>();
}
std::unique_ptr<ListModeData>
GERDF8ListmodeInputFileFormat::read_from_file(const std::string& filename) const
{
  return std::unique_ptr<ListModeData>(new CListModeDataGERDF8(filename));
}

} // namespace RDF8

} // namespace GE
END_NAMESPACE_STIR
