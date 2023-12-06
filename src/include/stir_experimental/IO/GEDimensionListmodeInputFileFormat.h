#ifndef __UCL_IO_GEDimensionListmodeInputFileFormat_h__
#define __UCL_IO_GEDimensionListmodeInputFileFormat_h__
/*
    Copyright (C) 2008, Hammersmith Imanet Ltd
    Copyright (C) 2013, 2019, 2020, 2023, University College London
    This file is part of STIR.

    See STIR/LICENSE.txt for details
*/
/*!

  \file
  \ingroup GEDimension
  \brief Declaration of class stir::UCL::GEDimensionListmodeInputFileFormat

  \author Kris Thielemans
*/
#include "stir/IO/InputFileFormat.h"
#include "stir/listmode/ListModeData.h"
#include <string>



START_NAMESPACE_STIR
namespace UCL {

//! Class for reading list mode data from the GEDimension  scanner
/*! \ingroup GEDimension
  \ingroup listmode
*/
class GEDimensionListmodeInputFileFormat :
public InputFileFormat<ListModeData >
{
 public:
  virtual const std::string
    get_name() const override
  {  return "GEDimension"; }


 protected:
  virtual bool 
    actual_can_read(const stir::FileSignature& signature,
		    std::istream& input) const override;
 public:
  virtual unique_ptr<data_type>
    read_from_file(std::istream& input) const override;
  virtual unique_ptr<data_type>
    read_from_file(const std::string& filename) const override;
};

} // namespace UCL
END_NAMESPACE_STIR

#endif
