/*
    Copyright (C) 2013, 2023 University College London
    This file is part of STIR.

    SPDX-License-Identifier: Apache-2.0

    See STIR/LICENSE.txt for details
*/
/*!
  \file
  \ingroup listmode  
  \brief Implementation of class stir::CListModeDataGERDF8
    
  \author Kris Thielemans
*/


#include "stir/listmode/CListModeDataGERDF8.h"
#include "stir/Succeeded.h"
#include "stir/ExamInfo.h"
#include "stir/info.h"
#include "stir/warning.h"
#include "stir/IO/GE/RDF8.h"
#include <boost/format.hpp>
#include <iostream>
#include <fstream>

START_NAMESPACE_STIR

CListModeDataGERDF8::
CListModeDataGERDF8(const std::string& listmode_filename)
  : listmode_filename(listmode_filename)    
{
  nmtools::IO::ge::CRDF8CONFIG config;
  if (!config.Read(listmode_filename))
    {
      error("Error reading '" + listmode_filename + "' as GE RDF8");
    }
  if (!config.IsListFile())
    {
      error("Error reading '" + listmode_filename + "' as GE RDF8 (not a listmode file)");
    }

  nmtools::IO::ge::CRDF8EXAM exam_header;
  if (!exam_header.Read(listmode_filename))
    {
      error("Error reading exam data from '" + listmode_filename + "' as GE RDF8");
    }

  // initialise scanner_ptr before calling open_lm_file, as it is used in that function

  warning("CListModeDataGERDF8: didn't read start time etc");
  shared_ptr<Scanner> scanner_sptr(Scanner::get_scanner_from_name(exam_header.getScannerDescription()));
  this->exam_info_sptr.reset(new ExamInfo);

  this->proj_data_info_sptr.reset(
      ProjDataInfo::ProjDataInfoCTI(scanner_sptr,
				    /*span=*/ 1,
				    scanner_sptr->get_num_rings()-1,
				    scanner_sptr->get_num_detectors_per_ring()/2,
				    scanner_sptr->get_max_num_non_arccorrected_bins(),
				    /*arc_corrected =*/ false,
				    /*tof_mash_factor = */  1));

  if (open_lm_file() == Succeeded::no)
    error(boost::format("CListModeDataGERDF8: error opening the first listmode file for filename %s") %
	  listmode_filename);
}

std::string
CListModeDataGERDF8::
get_name() const
{
  return listmode_filename;
}


shared_ptr<const stir::ProjDataInfo>
CListModeDataGERDF8::
get_proj_data_info_sptr() const
{
  return this->proj_data_info_sptr;
}

std::time_t 
CListModeDataGERDF8::
get_scan_start_time_in_secs_since_1970() const
{
  return std::time_t(-1); // TODO
}


shared_ptr <CListRecord> 
CListModeDataGERDF8::
get_empty_record_sptr() const
{
  shared_ptr<CListRecord> sptr(new CListRecordT(this->proj_data_info_sptr));
  return sptr;
}

Succeeded
CListModeDataGERDF8::
open_lm_file()
{
  info(boost::format("CListModeDataGERDF8: opening file %1%") % listmode_filename);
  nmtools::IO::ge::CRDF8LIST list_header;
  if (!list_header.Read(listmode_filename))
    {
      error("Error reading listheader from '" + listmode_filename + "' as GE RDF8");
    }
  if (list_header.IsListCompressed())
    {
      error("'" + listmode_filename + "' is GE RDF8 list-file, but it is compressed. Cannot handle that.");
    }
  shared_ptr<std::istream> stream_ptr(new std::fstream(listmode_filename.c_str(), std::ios::in | std::ios::binary));
  if (!(*stream_ptr))
    {
      return Succeeded::no;
    }
#if 0
  scannerParams scannerParams;
  off_t listStartOffset;
  
  if (GEgetListOffsetAndScannerParams( listmode_filename.c_str(),
				       &listStartOffset,
				       &scannerParams ) != SYS_OK )
    { 
      warning( "unable to get start of list file offset" );
      return Succeeded::no; 
    }
  stream_ptr->seekg(listStartOffset);
#else
  const auto listStartOffset = list_header.GetListStartOffset();
  stream_ptr->seekg(listStartOffset);
#endif
  current_lm_data_ptr.reset(
                            new InputStreamWithRecords<CListRecordT, bool>(stream_ptr, 
                                                                           4, 16,
                                                                           ByteOrder::little_endian != ByteOrder::get_native_order()));

  return Succeeded::yes;
}

Succeeded
CListModeDataGERDF8::
get_next_record(CListRecord& record_of_general_type) const
{
  CListRecordT& record = static_cast<CListRecordT&>(record_of_general_type);
  return current_lm_data_ptr->get_next_record(record);
}



Succeeded
CListModeDataGERDF8::
reset()
{
  return current_lm_data_ptr->reset();
}


CListModeData::SavedPosition
CListModeDataGERDF8::
save_get_position() 
{
  return static_cast<SavedPosition>(current_lm_data_ptr->save_get_position());
} 

Succeeded
CListModeDataGERDF8::
set_get_position(const CListModeDataGERDF8::SavedPosition& pos)
{
  return
    current_lm_data_ptr->set_get_position(pos);
}

END_NAMESPACE_STIR
