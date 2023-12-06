#ifndef __stir_IO_GERDF8ListmodeInputFileFormat_h__
#define __stir_IO_GERDF8ListmodeInputFileFormat_h__
/*
    Copyright (C) 2023 University College London
    This file is part of STIR.

    SPDX-License-Identifier: Apache-2.0

    See STIR/LICENSE.txt for details
*/
/*!

  \file
  \ingroup IO
  \ingroup GE
  \brief Declaration of class stir::GE::RDF8::IO::GERDF8ListmodeInputFileFormat

  \author Kris Thielemans
  \author Ottavia Bertolli
  \author Palak Wadhwa
  \author Nikos Efthimiou
*/
#include "stir/IO/InputFileFormat.h"

START_NAMESPACE_STIR

namespace GE
{
namespace RDF8
{

//! Class for being able to read list mode data from the GE PET scanners that use RDF8 via the listmode-data registry.
/*!
  \ingroup listmode
  \ingroup IO
  \ingroup GE
*/
class GERDF8ListmodeInputFileFormat : public InputFileFormat<ListModeData>
{
public:
  virtual const std::string get_name() const override { return "GERDF8"; }

protected:
  virtual bool actual_can_read(const FileSignature& signature, std::istream& input) const override;

public:
  virtual unique_ptr<data_type> read_from_file(std::istream& input) const override;
  virtual unique_ptr<data_type> read_from_file(const std::string& filename) const override;
};

} // namespace RDF8
} // namespace GE
END_NAMESPACE_STIR

#endif
