//
// $Id: CListModeDataGEDimension.h,v 1.12 2011-06-28 14:48:09 kris Exp $
//
/*
    Copyright (C) 2013 University College London
*/
/*!
  \file
  \ingroup listmode
  \brief Declaration of class stir::CListModeDataGEDimension
    
  \author Kris Thielemans
*/

#ifndef __UCL_listmode_CListModeDataGEDimension_H__
#define __UCL_listmode_CListModeDataGEDimension_H__

#include "stir/listmode/CListModeData.h"
#include "UCL/listmode/CListRecordGEDimension.h"
#include "stir/IO/InputStreamWithRecords.h"
#include "stir/shared_ptr.h"
#include <iostream>
#include <string>


START_NAMESPACE_STIR
namespace UCL {

//! A class that reads the listmode data for GE Dimension console scanners
/*!  \ingroup listmode
    This file format is used by GE Dimension console scanners (e.g. DSTE and RX).
*/
class CListModeDataGEDimension : public CListModeData
{
public:
  //! Constructor taking a filename
  CListModeDataGEDimension(const std::string& listmode_filename);

  virtual std::string
    get_name() const;

  virtual
    std::time_t get_scan_start_time_in_secs_since_1970() const;

  virtual 
    shared_ptr <CListRecord> get_empty_record_sptr() const;

  virtual 
    Succeeded get_next_record(CListRecord& record) const;

  virtual 
    Succeeded reset();

  virtual
    SavedPosition save_get_position();

  virtual
    Succeeded set_get_position(const SavedPosition&);

  //! returns \c true, as GEDimension listmode data stores delayed events (and prompts)
  /*! \todo this depends on the acquisition parameters */
  virtual bool has_delayeds() const { return true; }

private:
  typedef CListRecordGEDimension CListRecordT;
  std::string listmode_filename;
  shared_ptr<InputStreamWithRecords<CListRecordT, bool> > current_lm_data_ptr;
  float lm_start_time;
  float lm_duration;
  
  Succeeded open_lm_file(); 
};

} // namespace UCL
END_NAMESPACE_STIR

#endif
