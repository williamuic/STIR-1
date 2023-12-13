/*
    Copyright (C) 2013 University College London
    This file is part of STIR.

    SPDX-License-Identifier: Apache-2.0

    See STIR/LICENSE.txt for details
*/
/*!
  \file
  \ingroup listmode
  \brief Declaration of class stir::CListModeDataGERDF8
    
  \author Kris Thielemans
*/

#ifndef __listmode_CListModeDataGERDF8_H__
#define __listmode_CListModeDataGERDF8_H__

#include "stir/listmode/CListModeData.h"
#include "stir/listmode/CListRecordGERDF8.h"
#include "stir/IO/InputStreamWithRecords.h"
#include "stir/shared_ptr.h"
#include <iostream>
#include <string>


START_NAMESPACE_STIR

//! A class that reads the listmode data for GE RDF8 console scanners
/*!  \ingroup listmode
    This file format is used by GE RDF8 console scanners (e.g. 690 and 710).
*/
class CListModeDataGERDF8 : public CListModeData
{
public:
  //! Constructor taking a filename
  CListModeDataGERDF8(const std::string& listmode_filename);

  virtual std::string
    get_name() const  override;

  virtual shared_ptr<const stir::ProjDataInfo>
    get_proj_data_info_sptr() const override;

  virtual
    std::time_t get_scan_start_time_in_secs_since_1970() const;

  virtual 
    shared_ptr <CListRecord> get_empty_record_sptr() const  override;

  virtual 
    Succeeded get_next_record(CListRecord& record) const  override;

  virtual 
    Succeeded reset()  override;

  virtual
    SavedPosition save_get_position()  override;

  virtual
    Succeeded set_get_position(const SavedPosition&)  override;

  //! returns \c true, as GERDF8 listmode data does not store delayed events
  /*! \todo this depends on the acquisition parameters */
  virtual bool has_delayeds() const { return false; }

private:
  typedef CListRecordGERDF8 CListRecordT;
  std::string listmode_filename;
  shared_ptr<InputStreamWithRecords<CListRecordT, bool> > current_lm_data_ptr;
  unsigned long first_time_stamp;
  unsigned long lm_duration_in_millisecs;
  
  Succeeded open_lm_file(); 
};

END_NAMESPACE_STIR

#endif
