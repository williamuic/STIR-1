//
// $Id: CListRecordECAT962.h,v 1.8 2011-12-31 16:42:45 kris Exp $
//
/*
    Copyright (C) 2003-2011 Hammersmith Imanet Ltd (CListRecordECAT.h)
    Copyright (C) 2013 University College London (major mods for GE Dimension data)
*/
/*!
  \file
  \ingroup listmode
  \brief Classes for listmode records of GE Dimension console data

  This file is based on GE proprietary information and can therefore not be distributed outside UCL
  without approval from GE.
    
  \author Kris Thielemans
      
  $Date: 2011-12-31 16:42:45 $
  $Revision: 1.8 $
*/

#ifndef __UCL_listmode_CListRecordGEDimension_H__
#define __UCL_listmode_CListRecordGEDimension_H__

#include "stir/listmode/CListRecord.h"
#include "stir/listmode/CListEventCylindricalScannerWithDiscreteDetectors.h"
#include "stir/Succeeded.h"
#include "stir/ByteOrder.h"
#include "stir/ByteOrderDefine.h"
#include <boost/static_assert.hpp>
#include <boost/cstdint.hpp>

START_NAMESPACE_STIR
namespace UCL {


//! Class for storing and using a coincidence event from a GE Dimension listmode file
/*! \ingroup listmode
  This class cannot have virtual functions, as it needs to just store the data 4 bytes for CListRecordGEDimension to work.
*/
class CListEventDataGEDimension
{
 public:  
  inline bool is_prompt() const { return random == 1; }
  inline Succeeded set_prompt(const bool prompt = true) 
  { if (prompt) random=1; else random=0; return Succeeded::yes; }
  inline void get_detection_position(DetectionPositionPair<>& det_pos) const
  {
    det_pos.pos1().tangential_coord() = det_1;
    det_pos.pos1().axial_coord() = ring_1;
    det_pos.pos2().tangential_coord() = det_2;
    det_pos.pos2().axial_coord() = ring_2;
  }
  inline bool is_event() const
    { return type == 0; }

 private:

#if STIRIsNativeByteOrderBigEndian
  // Do byteswapping first before using this bit field.
  boost::uint32_t    type    : 1; /* 0-coincidence event, 1-time tick or something else*/
  boost::uint32_t    det_1 : 10;
  boost::uint32_t    ring_1 : 5;
  boost::uint32_t    random  : 1; /* 1 for prompt */
  boost::uint32_t    det_2 : 10;
  boost::uint32_t    ring_2 : 5;
#else
  boost::uint32_t    ring_2 : 5;
  boost::uint32_t    det_2 : 10;
  boost::uint32_t    random  : 1; /* 1 for prompt */
  boost::uint32_t    ring_1 : 5;
  boost::uint32_t    det_1 : 10;
  boost::uint32_t    type    : 1; /* 0-coincidence event, 1-time tick or something else*/  
#endif
}; /*-coincidence event*/


//! A class for storing and using a timing 'event' from a GE Dimension listmode file
/*! \ingroup listmode
  This class cannot have virtual functions, as it needs to just store the data 8 bytes for CListRecordGEDimension to work.
 */
class CListTimeDataGEDimension
{
 public:
  inline unsigned long get_time_in_millisecs() const
    { return (time_hi()<<24) | time_lo(); }
  inline Succeeded set_time_in_millisecs(const unsigned long time_in_millisecs)
    { 
      words[0].value = ((1UL<<24)-1) & (time_in_millisecs); 
      words[1].value = (time_in_millisecs) >> 24; 
      // TODO return more useful value
      return Succeeded::yes;
    }
  inline bool is_time() const
    { return (words[0].signature==4) && (words[1].signature==6); }
      
private:
  typedef union{
    struct {
#if STIRIsNativeByteOrderBigEndian
      boost::uint32_t signature : 3;
      boost::uint32_t reserved : 5;
      boost::uint32_t value : 24;
#else
      boost::uint32_t value : 24;
      boost::uint32_t reserved : 5;
      boost::uint32_t signature : 3;
#endif
    };      
    boost::uint32_t raw;
  } oneword_t;
  oneword_t words[2];

  unsigned long time_lo() const
  { return words[0].value; }
  unsigned long time_hi() const
  { return words[1].value; }
};

//! A class for storing and using a trigger 'event' from a GE Dimension listmode file
/*! \ingroup listmode
  This class cannot have virtual functions, as it needs to just store the data 8 bytes for CListRecordGEDimension to work.
 */
class CListGatingDataGEDimension
{
 public:
  #if 0
  inline unsigned long get_time_in_millisecs() const
    { return (time_hi()<<24) | time_lo(); }
  inline Succeeded set_time_in_millisecs(const unsigned long time_in_millisecs)
    { 
      words[0].value = ((1UL<<24)-1) & (time_in_millisecs); 
      words[1].value = (time_in_millisecs) >> 24; 
      // TODO return more useful value
      return Succeeded::yes;
    }
  #endif
  inline bool is_gating_input() const
    { return (words[0].signature==21) && (words[1].signature==29); }
  inline unsigned int get_gating() const
    { return words[0].reserved; } // return "reserved" bits. might be something in there
  inline Succeeded set_gating(unsigned int g) 
    { words[0].reserved = g&7; return Succeeded::yes; }
      
private:
  typedef union{
    struct {
#if STIRIsNativeByteOrderBigEndian
      boost::uint32_t signature : 5;
      boost::uint32_t reserved : 3;
      boost::uint32_t value : 24; // timing info here in the first word, but we're ignoring it
#else
      boost::uint32_t value : 24;
      boost::uint32_t reserved : 3;
      boost::uint32_t signature : 5;
#endif
    };      
    boost::uint32_t raw;
  } oneword_t;
  oneword_t words[2];
};

//! A class for a general element (or "record") of a GE Dimension listmode file
/*! \ingroup listmode
  All types of records are stored in a (private) union with the "basic" classes such as CListEventDataGEDimension.
  This class essentially just forwards the work to the "basic" classes.

  A complication for GE Dimension data is that not all events are the same size:
  coincidence events are 4 bytes, and others are 8 bytes. 

  \todo Currently we always assume the data is from a DSTE. We should really read this from the RDF header.
*/
class CListRecordGEDimension : public CListRecordWithGatingInput, public CListTime, public CListGatingInput,
    public  CListEventCylindricalScannerWithDiscreteDetectors
{
  typedef CListEventDataGEDimension DataType;
  typedef CListTimeDataGEDimension TimeType;
  typedef CListGatingDataGEDimension GatingType;

 public:  
  CListRecordGEDimension() :
  CListEventCylindricalScannerWithDiscreteDetectors(shared_ptr<Scanner>(new Scanner(Scanner::DiscoverySTE)))
    {}

  bool is_time() const
  { 
    // note: don't need to check first if it's a 4-byte event as 
    // the 2nd word will never be checked by time_data.is_time() if it is a coincidence event
    return /*!this->is_event() && */ this->time_data.is_time();
  }
  bool is_gating_input() const
  {
    return this->gating_data.is_gating_input();
  }
  bool is_event() const
  { return this->event_data.is_event(); }
  virtual CListEvent&  event() 
    { return *this; }
  virtual const CListEvent&  event() const
    { return *this; }
  virtual CListTime&   time()
    { return *this; }
  virtual const CListTime&   time() const
    { return *this; }
  virtual CListGatingInput&  gating_input()
    { return *this; }
  virtual const CListGatingInput&  gating_input() const
  { return *this; }

  bool operator==(const CListRecord& e2) const
  {
    return dynamic_cast<CListRecordGEDimension const *>(&e2) != 0 &&
      raw[0] == static_cast<CListRecordGEDimension const &>(e2).raw[0] &&
      (~this->is_event() || (raw[1] == static_cast<CListRecordGEDimension const &>(e2).raw[1]));
  }	    

  // time 
  inline unsigned long get_time_in_millisecs() const 
    { return time_data.get_time_in_millisecs(); }
  inline Succeeded set_time_in_millisecs(const unsigned long time_in_millisecs)
    { return time_data.set_time_in_millisecs(time_in_millisecs); }
  inline unsigned int get_gating() const
    { return gating_data.get_gating(); }
  inline Succeeded set_gating(unsigned int g) 
    { return gating_data.set_gating(g); }
  // event
  inline bool is_prompt() const { return event_data.is_prompt(); }
  inline Succeeded set_prompt(const bool prompt = true) 
  { return event_data.set_prompt(prompt); }

  virtual void get_detection_position(DetectionPositionPair<>& det_pos) const
  { event_data.get_detection_position(det_pos); }

  //! This routine sets in a coincidence event from detector "indices"
  virtual void set_detection_position(const DetectionPositionPair<>&)
  {
    error("TODO");
  }

  virtual std::size_t size_of_record_at_ptr(const char * const data_ptr, const std::size_t /*size*/, 
                                            const bool do_byte_swap) const
  { 
    // coincidence events are size 4, others are size 8
    const bool is_coinc =  
      do_byte_swap ? ((data_ptr[0]&0x80) == 0) : ((data_ptr[3]&0x80) == 0);
    return is_coinc ? std::size_t(4) : std::size_t(8);
  }

  virtual Succeeded init_from_data_ptr(const char * const data_ptr, 
                                       const std::size_t
#ifndef NDEBUG
                                       size // only used within assert, so don't define otherwise to avoid compiler warning
#endif
                                       , const bool do_byte_swap)
  {
    assert(size >= 4);
    std::copy(data_ptr, data_ptr+4, reinterpret_cast<char *>(&this->raw[0]));
    // TODO might have to swap raw[0] and raw[1] if byteswap
    if (do_byte_swap)
      {
        ByteOrder::swap_order(this->raw[0]);
      }
    if (!this->is_event())
      {
        assert(size >= 8);
        std::copy(data_ptr+4, data_ptr+8, reinterpret_cast<char *>(&this->raw[1]));
      }
    if (do_byte_swap)
      {
        ByteOrder::swap_order(this->raw[1]);
      }
    return Succeeded::yes;
  }

private:
  union {
    DataType  event_data;
    TimeType   time_data; 
    GatingType gating_data;
    boost::int32_t  raw[2];
  };
  BOOST_STATIC_ASSERT(sizeof(boost::int32_t)==4);
  BOOST_STATIC_ASSERT(sizeof(DataType)==4); 
  BOOST_STATIC_ASSERT(sizeof(TimeType)==8); 
  BOOST_STATIC_ASSERT(sizeof(GatingType)==8); 

};


} // namespace UCL
END_NAMESPACE_STIR

#endif
