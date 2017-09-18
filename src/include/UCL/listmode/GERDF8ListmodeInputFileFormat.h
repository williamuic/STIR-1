//
// $Id: GERDF8ListmodeInputFileFormat.h,v 1.1 2011-06-28 14:46:08 kris Exp $
//
#ifndef __UCL_IO_GERDF8ListmodeInputFileFormat_h__
#define __UCL_IO_GERDF8ListmodeInputFileFormat_h__
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
  \ingroup GERDF8
  \brief Declaration of class stir::ecat::ecat7::GERDF8ListmodeInputFileFormat

  \author Kris Thielemans

  $Date: 2011-06-28 14:46:08 $
  $Revision: 1.1 $
*/
#include "stir/IO/InputFileFormat.h"
#include "UCL/listmode/CListModeDataGERDF8.h"
#include "stir/warning.h"
#include <boost/format.hpp>
#include "stir/ByteOrder.h"
#include <string>



START_NAMESPACE_STIR
namespace UCL {

//! Class for reading list mode data from the GERDF8  scanner
/*! \ingroup GERDF8
  \ingroup listmode
*/
class GERDF8ListmodeInputFileFormat :
public InputFileFormat<CListModeData >
{
 public:
  virtual const std::string
    get_name() const
  {  return "GERDF8"; }


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
  virtual unique_ptr<data_type>
    read_from_file(std::istream& input) const
  {
    // cannot do this 
    warning(boost::format("read_from_file for GERDF8 listmode data with istream not implemented %s:%s. Sorry") %
	  __FILE__ % __LINE__);
    return
      unique_ptr<data_type>();
  }
  virtual unique_ptr<data_type>
    read_from_file(const std::string& filename) const
  {	
    return unique_ptr<data_type>(new CListModeDataGERDF8(filename)); 
  }
};

} // namespace UCL
END_NAMESPACE_STIR

#endif
