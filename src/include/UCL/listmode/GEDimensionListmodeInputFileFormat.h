//
// $Id: GEDimensionListmodeInputFileFormat.h,v 1.1 2011-06-28 14:46:08 kris Exp $
//
#ifndef __UCL_IO_GEDimensionListmodeInputFileFormat_h__
#define __UCL_IO_GEDimensionListmodeInputFileFormat_h__
/*
    Copyright (C) 2006- $Date: 2011-06-28 14:46:08 $, Hammersmith Imanet Ltd
    This file is part of STIR.
    This file is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This file is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    See STIR/LICENSE.txt for details
*/
/*!

  \file
  \ingroup GEDimension
  \brief Declaration of class stir::ecat::ecat7::GEDimensionListmodeInputFileFormat

  \author Kris Thielemans

  $Date: 2011-06-28 14:46:08 $
  $Revision: 1.1 $
*/
#include "stir/IO/InputFileFormat.h"
#include "UCL/listmode/CListModeDataGEDimension.h"

#include "stir/ByteOrder.h"
#include <string>



START_NAMESPACE_STIR
namespace UCL {

//! Class for reading list mode data from the GEDimension  scanner
/*! \ingroup GEDimension
  \ingroup listmode
*/
class GEDimensionListmodeInputFileFormat :
public InputFileFormat<CListModeData >
{
 public:
  virtual const std::string
    get_name() const
  {  return "GEDimension"; }


 protected:
  virtual 
    bool 
    actual_can_read(const FileSignature& signature,
		    std::istream& input) const
  {
    boost::uint32_t word = *reinterpret_cast<uint32_t const * const>(signature.get_signature());
    if (word==65279)
      return true;
    // try byteswap
    ByteOrder::swap_order(word);
    return word==65279;
  }
 public:
  virtual std::auto_ptr<data_type>
    read_from_file(std::istream& input) const
  {
    // cannot do this 
    warning("read_from_file for GEDimension listmode data with istream not implemented %s:%s. Sorry",
	  __FILE__, __LINE__);
    return
      std::auto_ptr<data_type>
      (0);
  }
  virtual std::auto_ptr<data_type>
    read_from_file(const std::string& filename) const
  {	
    return std::auto_ptr<data_type>(new CListModeDataGEDimension(filename)); 
  }
};

} // namespace UCL
END_NAMESPACE_STIR

#endif
