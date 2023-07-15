/*
    Copyright (C) 2013, 2014, 2018, 2020, 2021, 2023, University College London
*/
/*!
  \file
  \ingroup listmode  
  \brief Implementation of class stir::CListModeDataGEDimension
    
  \author Kris Thielemans
*/


#include "UCL/listmode/CListModeDataGEDimension.h"
#include "stir/Succeeded.h"
#include "stir/ExamInfo.h"
#include "stir/info.h"
#include "stir/error.h"
#include "stir/ByteOrder.h"
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
  shared_ptr<Scanner> scanner_sptr(new Scanner(Scanner::DiscoverySTE));
  this->exam_info_sptr.reset(new ExamInfo);

  this->proj_data_info_sptr.reset(
      ProjDataInfo::ProjDataInfoCTI(scanner_sptr,
				    /*span=*/ 1,
				    scanner_sptr->get_num_rings()-1,
				    scanner_sptr->get_num_detectors_per_ring()/2,
				    scanner_sptr->get_max_num_non_arccorrected_bins(),
				    /*arc_corrected =*/ false
                                    ));

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

#if STIR_VERSION < 050000
shared_ptr<stir::ProjDataInfo>
#else
shared_ptr<const stir::ProjDataInfo>
#endif
CListModeDataGEDimension::
get_proj_data_info_sptr() const
{
  return this->proj_data_info_sptr;
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
  shared_ptr<std::istream> stream_ptr(new std::fstream(listmode_filename.c_str(), std::ios::in | std::ios::binary));
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
  return static_cast<SavedPosition>(current_lm_data_ptr->save_get_position());
} 

Succeeded
CListModeDataGEDimension::
set_get_position(const CListModeDataGEDimension::SavedPosition& pos)
{
  return
    current_lm_data_ptr->set_get_position(pos);
}

} // namespace UCL

END_NAMESPACE_STIR
