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

  // initialise_proj_data_info()
  shared_ptr<Scanner> scanner_sptr(Scanner::get_scanner_from_name(exam_header.getScannerDescription()));

  shared_ptr<ProjDataInfo> local_proj_data_info_sptr (
      ProjDataInfo::ProjDataInfoCTI(scanner_sptr,
				    /*span=*/ 1,
				    scanner_sptr->get_num_rings()-1,
				    scanner_sptr->get_num_detectors_per_ring()/2,
				    scanner_sptr->get_max_num_non_arccorrected_bins(),
				    /*arc_corrected =*/ false,
				    /*tof_mash_factor = */  1));
  {
    nmtools::IO::ge::CRDF8ACQPARAMS acq_params_header;
    acq_params_header.Read(listmode_filename);
    local_proj_data_info_sptr->set_bed_position_horizontal(acq_params_header.acq_scan_params._tableLocation);
#if 0 // HDF5 code uses
    local_proj_data_info_sptr->set_bed_position_horizontal(
      this->read_dataset_int32("/HeaderData/AcqParameters/LandmarkParameters/absTableLongitude")
      / 10.F); /* units in RDF are 0.1 mm */
    // local_proj_data_info_sptr->set_gantry_tilt(this->read_dataset_uint32("/HeaderData/AcqParameters/LandmarkParameters/gantryTilt"));
    // /* units in RDF are 0.25 degrees, patient relative */
    local_proj_data_info_sptr->set_bed_position_vertical(
      this->read_dataset_int32("/HeaderData/AcqParameters/LandmarkParameters/tableElevation")
      / 10.F); /* units in RDF are 0.1 mm */
#endif
  }

  this->proj_data_info_sptr = local_proj_data_info_sptr;

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
  return this->get_exam_info().start_time_in_secs_since_1970;
}


shared_ptr <CListRecord> 
CListModeDataGERDF8::
get_empty_record_sptr() const
{
  shared_ptr<CListRecord> sptr(new CListRecordT(this->proj_data_info_sptr, this->first_time_stamp));
  return sptr;
}

Succeeded
CListModeDataGERDF8::
open_lm_file()
{
  info(boost::format("CListModeDataGERDF8: opening file %1%") % listmode_filename);
  {
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
    const auto listStartOffset = list_header.GetListStartOffset();
    stream_ptr->seekg(listStartOffset);
    this->current_lm_data_ptr.reset(
                                    new InputStreamWithRecords<CListRecordT, bool>(stream_ptr, 
                                                                                   4, 16,
                                                                                   ByteOrder::little_endian != ByteOrder::get_native_order()));
    this->first_time_stamp = list_header.GetFirstTmAbsTimeStamp();
    this->lm_duration_in_millisecs = list_header.GetListStartOffset() - this->first_time_stamp;
  }

  //void CListModeDataGERDF8::initialise_exam_info()
  {
    ExamInfo exam_info;
    exam_info.imaging_modality = ImagingModality(ImagingModality::PT);
    typedef nmtools::IO::ge::CRDF8ACQSTATS AcqStatsT;
    typedef nmtools::IO::ge::CRDF8ACQPARAMS AcqParamsT;
    AcqStatsT acq_stats_header;
    acq_stats_header.Read(listmode_filename);
    AcqParamsT acq_params_header;
    acq_params_header.Read(listmode_filename);
    {
      PatientPosition::OrientationValue orientation;
      PatientPosition::RotationValue rotation;
      switch (acq_params_header.acq_landmark_params._patientEntry)
        {
        case AcqParamsT::AcqPatientEntries::ACQ_HEAD_FIRST: orientation = PatientPosition::OrientationValue::head_in; break;
        case AcqParamsT::AcqPatientEntries::ACQ_FEET_FIRST: orientation = PatientPosition::OrientationValue::feet_in; break;
        default: orientation = PatientPosition::OrientationValue::unknown_orientation;
        }
      switch (acq_params_header.acq_landmark_params._patientPosition)
        {
        case AcqParamsT::AcqPatientPositions::ACQ_SUPINE: rotation = PatientPosition::RotationValue::supine; break;
        case AcqParamsT::AcqPatientPositions::ACQ_PRONE: rotation = PatientPosition::RotationValue::prone; break;
        case AcqParamsT::AcqPatientPositions::ACQ_LEFT_DECUB: rotation = PatientPosition::RotationValue::left; break;
        case AcqParamsT::AcqPatientPositions::ACQ_RIGHT_DECUB: rotation = PatientPosition::RotationValue::right; break;
        default: rotation = PatientPosition::RotationValue::unknown_rotation; break;
        }
      exam_info.patient_position = PatientPosition(orientation, rotation);
    }

    exam_info.set_high_energy_thres(static_cast<float>(acq_params_header.acq_edcat_params._upper_energy_limit));
    exam_info.set_low_energy_thres(static_cast<float>(acq_params_header.acq_edcat_params._lower_energy_limit));

    auto scanStartTime = double(acq_stats_header._scanStartTime);
    exam_info.start_time_in_secs_since_1970=scanStartTime;

    const double frame_start_time = acq_stats_header._frameStartTime - scanStartTime;
    const double frameDuration = acq_stats_header._frameDuration/1000;

    std::vector<std::pair<double, double> >tf{{frame_start_time,frame_start_time+frameDuration}};

    TimeFrameDefinitions tm(tf);
    exam_info.set_time_frame_definitions(tm);

    this->set_exam_info(exam_info);
  }

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
