//
// $Id: CListModeDataGEDimension.cxx,v 1.25 2012-01-09 09:04:55 kris Exp $
//
/*
    Copyright (C) 2013 University College London
*/
/*!
  \file
  \ingroup listmode  
  \brief Implementation of class stir::CListModeDataGEDimension
    
  \author Kris Thielemans
      
  $Date: 2012-01-09 09:04:55 $
  $Revision: 1.25 $
*/


#include "UCL/listmode/CListModeDataGEDimension.h"
#include "stir/Succeeded.h"
#include "stir/info.h"
#include <boost/format.hpp>
#include <iostream>
#include <fstream>

START_NAMESPACE_STIR

namespace UCL {


CListModeDataGEDimension::
CListModeDataGEDimension(const std::string& listmode_filename)
  : listmode_filename(listmode_filename)    
{
  // initialise scanner_ptr before calling open_lm_file, as it is used in that function

  warning("CListModeDataGEDimension: "
	  "Assuming this is GEDimension STE, but couldn't find scan start time etc");
  this->scanner_sptr.reset(new Scanner(Scanner::DiscoverySTE));

  if (open_lm_file() == Succeeded::no)
    error(boost::format("CListModeDataGEDimension: error opening the first listmode file for filename %s") %
	  listmode_filename);
}

std::string
CListModeDataGEDimension::
get_name() const
{
  return listmode_filename;
}

std::time_t 
CListModeDataGEDimension::
get_scan_start_time_in_secs_since_1970() const
{
  return std::time_t(-1); // TODO
}


shared_ptr <CListRecord> 
CListModeDataGEDimension::
get_empty_record_sptr() const
{
  shared_ptr<CListRecord> sptr(new CListRecordT);
  return sptr;
}

Succeeded
CListModeDataGEDimension::
open_lm_file()
{
  info(boost::format("CListModeDataGEDimension: opening file %1%") % listmode_filename);
  shared_ptr<istream> stream_ptr(new std::fstream(listmode_filename.c_str(), std::ios::in | std::ios::binary));
  if (!(*stream_ptr))
    {
      return Succeeded::no;
    }
  stream_ptr->seekg(71168); // TODO get offset from RDF
  current_lm_data_ptr.reset(
                            new InputStreamWithRecords<CListRecordT, bool>(stream_ptr, 
                                                                           4, 8,
                                                                           ByteOrder::little_endian != ByteOrder::get_native_order()));

  return Succeeded::yes;
}

Succeeded
CListModeDataGEDimension::
get_next_record(CListRecord& record_of_general_type) const
{
  CListRecordT& record = static_cast<CListRecordT&>(record_of_general_type);
  return current_lm_data_ptr->get_next_record(record);
}



Succeeded
CListModeDataGEDimension::
reset()
{
  return current_lm_data_ptr->reset();
}


CListModeData::SavedPosition
CListModeDataGEDimension::
save_get_position() 
{
  return current_lm_data_ptr->save_get_position();
} 

Succeeded
CListModeDataGEDimension::
set_get_position(const typename CListModeDataGEDimension::SavedPosition& pos)
{
  return
    current_lm_data_ptr->set_get_position(pos);
}

} // namespace UCL

END_NAMESPACE_STIR
