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
*/

#ifndef __stir_listmode_CListRecordGERDF8_H__
#define __stir_listmode_CListRecordGERDF8_H__

#include "stir/listmode/CListRecord.h"
#include "stir/listmode/CListEventCylindricalScannerWithDiscreteDetectors.h"
#include "stir/Succeeded.h"
#include "stir/ByteOrder.h"
#include "stir/ByteOrderDefine.h"
#include <boost/static_assert.hpp>
#include <boost/cstdint.hpp>
#include <iostream>

START_NAMESPACE_STIR
/*
 * The next two defines (SOE & EOE) are for marking the start and end of an 
 * event in the stream. They facilitate some limited event alignment checks. */
enum EventChecks
{
  SOE=0x5,		/* Value of Start Of Event Mark */
 EOE= 0x5		/* Value of End Of Event Mark */
};


/***********************************
 * Supported Extended Event Types
 ***********************************/
enum ExtendedEvtType
{
    TIME_MARKER_EVT = 0x0,
    COINC_WITH_ENERGY_EVT  = 0x2,
    PHYS1_TRIG_EVT		   = 0x4,
    PHYS2_TRIG_EVT		   = 0x5,
    PHYS3_TRIG_EVT		   = 0x6,
    FRAME_SYNC_EVT		= 0x8,		/* Sorter->DFE Frame  */
    END_OF_LIST_EVT		= 0xE,		/* Special Event to facilitate unli  */
    RAW_DETECTOR_EVT		= 0xF		/* a.k.a. a Singles =  */   
};


//! Class for storing and using a coincidence event from a GE Dimension listmode file
/*! \ingroup listmode
  This class cannot have virtual functions, as it needs to just store the data 4 bytes for CListRecordGERDF8 to work.
*/
class CListEventDataGERDF8
{
 public:  
  inline bool is_prompt() const { return prompt; } // TODO
  inline Succeeded set_prompt(const bool prompt = true) 
  { 
    //if (prompt) random=1; else random=0; return Succeeded::yes; 
    return Succeeded::no;
  }
  inline void get_detection_position(DetectionPositionPair<>& det_pos) const
  {
    det_pos.pos1().tangential_coord() = loXtalTransAxID;
    det_pos.pos1().axial_coord() = loXtalAxialID;
    det_pos.pos2().tangential_coord() = hiXtalTransAxID;
    det_pos.pos2().axial_coord() = hiXtalAxialID;
  }
  inline bool is_event() const
    { 
      return (coinc == 1) && (nomCoinc==1);
     } 
  inline int get_tof_bin() const
	 {
		 return static_cast<int>(deltaTime);
	 }
 private:

#if STIRIsNativeByteOrderBigEndian
  // Do byteswapping first before using this bit field.
  TODO
#else
    	boost::uint16_t soem:3;				/* Start Of Event Mark (should equal SOE constant) */
	boost::uint16_t coinc:1;				/* boolean (should always be true: Coincidence Event) */
	boost::uint16_t nomCoinc:1;			/* boolean (true:Nominal Coinc, false:Cal Coinc) */
	boost::uint16_t prompt:1;			/* boolean (true:Prompt, false:Delay) */
	boost::int16_t deltaTime:10;		/* TOF 'signed' delta time (units defined by electronics) */
	boost::uint16_t hiXtalAxialID:6;		/* system High Crystal Axial Id */
	boost::uint16_t hiXtalTransAxID:10;	/* system High Crystal Trans-Axial Id */
	boost::uint16_t loXtalAxialID:6;		/* system Low Crystal Axial Id */
	boost::uint16_t loXtalTransAxID:10;	/* system Low Crystal Trans-Axial Id */	
#endif
}; /*-coincidence event*/


//! A class for storing and using a timing 'event' from a GE RDF8 PET/MR listmode file
/*! \ingroup listmode
  This class cannot have virtual functions, as it needs to just store the data 8 bytes for CListRecordGERDF8 to work.
 */
class CListTimeDataGERDF8
{
 public:
  inline unsigned long get_time_in_millisecs() const
  { return (time_hi()<<24) | (time_mid()<<8) | time_lo(); }
  inline Succeeded set_time_in_millisecs(const unsigned long time_in_millisecs)
    { 
      data.timeMarkLS = ((1UL<<16)-1) & (time_in_millisecs); 
      data.timeMarkMS = (time_in_millisecs) >> 16; 
      // TODO return more useful value
      return Succeeded::no;
    }
  inline bool is_time() const
    { // TODO need to find out how to see if it's a timing event
	return (data.coinc==0) && (data.eventType==TIME_MARKER_EVT); 
    }// TODO
      
private:
  typedef union{
    struct {
#if STIRIsNativeByteOrderBigEndian
      TODO
#else
	boost::uint16_t soem:3;			/* Start Of Event Mark (should equal SOE constant) */
	boost::uint16_t coinc:1;			/* boolean (should always be false: non-Coinc) */
	boost::uint16_t eventType:4;		/* eventType=TIME_MARK_EVT */
	boost::uint16_t timeMarkMS:8;	/* Most Significant Time Mark Bits */
	boost::uint16_t timeMarkMid:16;	/* Middle Significant Time Mark Bits */
	boost::uint16_t timeMarkLS:8;	/* Least Significant Time Mark Bits */
	boost::uint16_t unused:4;		/* Undefined */
	boost::uint16_t eoem:4;			/* End Of Event Mark (should equal EOE constant) */
#endif
    };      
  } data_t;
  data_t data;

  boost::uint64_t time_lo() const
  { return data.timeMarkLS; }
  boost::uint64_t time_hi() const
  { return data.timeMarkMS; }
  boost::uint64_t time_mid() const
  { return data.timeMarkMid; }
};

#if 0
//! A class for storing and using a trigger 'event' from a GE Dimension listmode file
/*! \ingroup listmode
  This class cannot have virtual functions, as it needs to just store the data 8 bytes for CListRecordGERDF8 to work.
 */
class CListGatingDataGERDF8
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

#endif

//! A class for a general element (or "record") of a GE Dimension listmode file
/*! \ingroup listmode
  All types of records are stored in a (private) union with the "basic" classes such as CListEventDataGERDF8.
  This class essentially just forwards the work to the "basic" classes.

  A complication for GE Dimension data is that not all events are the same size:
  coincidence events are 4 bytes, and others are 8 bytes. 

  \todo Currently we always assume the data is from a DSTE. We should really read this from the RDF header.
*/
class CListRecordGERDF8 : public CListRecord, public CListTime, // public CListGatingInput,
    public  CListEventCylindricalScannerWithDiscreteDetectors
{
  typedef CListEventDataGERDF8 DataType;
  typedef CListTimeDataGERDF8 TimeType;
  //typedef CListGatingDataGERDF8 GatingType;

 public:  
  CListRecordGERDF8() :
  CListEventCylindricalScannerWithDiscreteDetectors(shared_ptr<Scanner>(new Scanner(Scanner::Discovery690)))
    {}

  bool is_time() const
  { 
   return this->time_data.is_time();
  }
#if 0
  bool is_gating_input() const
  {
    return this->gating_data.is_gating_input();
  }
#endif

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
#if 0
  virtual CListGatingInput&  gating_input()
    { return *this; }
  virtual const CListGatingInput&  gating_input() const
  { return *this; }
#endif
  bool operator==(const CListRecord& e2) const
  {
    return false;
#if 0
// TODO
dynamic_cast<CListRecordGERDF8 const *>(&e2) != 0 &&
      raw[0] == static_cast<CListRecordGERDF8 const &>(e2).raw[0] &&
      (this->is_event() || (raw[1] == static_cast<CListRecordGERDF8 const &>(e2).raw[1]));
#endif
  }	    

  // time 
  inline unsigned long get_time_in_millisecs() const 
    { return time_data.get_time_in_millisecs(); }
  inline Succeeded set_time_in_millisecs(const unsigned long time_in_millisecs)
    { return time_data.set_time_in_millisecs(time_in_millisecs); }
#if 0
  inline unsigned int get_gating() const
    { return gating_data.get_gating(); }
  inline Succeeded set_gating(unsigned int g) 
    { return gating_data.set_gating(g); }
#endif
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
    // TODO: get size of record from the file, whereas here I have hard-coded as being 6bytes (I know it's the case for the Orsay data) OtB 15/09

    return std::size_t(6); // std::size_t(data_ptr[0]&0x80);
  }

  virtual Succeeded init_from_data_ptr(const char * const data_ptr, 
                                       const std::size_t
#ifndef NDEBUG
                                       size // only used within assert, so don't define otherwise to avoid compiler warning
#endif
                                       , const bool do_byte_swap)
  {
//    std::cout << " Size  =" << size << " \n" ;
    assert(size >= 6);
//std::cout << " Got to here \n" ;
    std::copy(data_ptr, data_ptr+6, reinterpret_cast<char *>(&this->raw[0]));
    // TODO might have to swap raw[0] and raw[1] if byteswap

    if (do_byte_swap)
      {
        ByteOrder::swap_order(this->raw[0]);
      }
    if (this->is_event() || this->is_time())
      {
//	std::cout << "This is an event \n" ;
        assert(size >= 6);
	
        std::copy(data_ptr+6, data_ptr+6, reinterpret_cast<char *>(&this->raw[1]));
//	std::cout << "after assert an event \n" ;
      }
    if (do_byte_swap)
      {
	error("don't know how to byteswap");
        ByteOrder::swap_order(this->raw[1]);
      }
	  
	  if (this->is_event())
	  {
#ifdef STIR_TOF
	    // set TOF info in ps
	   this->delta_time = this->event_data.get_tof_bin() *this-> get_scanner_ptr()->get_size_of_timing_bin();
#endif
	  }
 
	  
	  
    return Succeeded::yes;
  }

private:
  union {
    DataType  event_data;
    TimeType   time_data; 
    //GatingType gating_data;
    boost::int32_t  raw[2];
  };
  BOOST_STATIC_ASSERT(sizeof(boost::int32_t)==4);
  BOOST_STATIC_ASSERT(sizeof(DataType)==6); 
  BOOST_STATIC_ASSERT(sizeof(TimeType)==6); 
  //BOOST_STATIC_ASSERT(sizeof(GatingType)==8); 

};


END_NAMESPACE_STIR

#endif
