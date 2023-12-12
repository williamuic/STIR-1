//
//
/*!
  \file
  \ingroup listmode
  \brief Declarations of classes stir::CListRecord, and stir::CListEvent which
  are used for list mode data.
    

  \author Nikos Efthimiou
  \author Daniel Deidda
  \author Kris Thielemans

*/
/*
    Copyright (C) 2003- 2011, Hammersmith Imanet Ltd
    Copyright (C) 2016, University of Hull
    Copyright (C) 2019, National Physical Laboratory
    Copyright (C) 2019, 2023, University College of London
    This file is part of STIR.

    SPDX-License-Identifier: Apache-2.0

    See STIR/LICENSE.txt for details
*/

#ifndef __stir_listmode_CListRecord_H__
#define __stir_listmode_CListRecord_H__

#include "stir/listmode/ListRecord.h"
#include "stir/listmode/ListRecordWithGatingInput.h"

START_NAMESPACE_STIR
class Bin;
class ProjDataInfo;
class Succeeded;
class ListGatingInput;

template <typename coordT> class CartesianCoordinate3D;
template <typename coordT> class LORAs2Points;

//! Class for storing and using a coincidence event from a list mode file
/*! \ingroup listmode
    CListEvent is used to provide an interface to the actual events (i.e.
    detected counts) in the list mode stream.

    \todo this is still under development. Things to add are for instance
    energy windows. Also, get_bin() would need
    time info or so for rotating scanners.

    \see CListModeData for more info on PET list mode data. 
*/
class CListEvent : public ListEvent
{
public:

  //! Changes the event from prompt to delayed or vice versa
  /*! Default implementation just returns Succeeded::no. */
  virtual 
    Succeeded
    set_prompt(const bool prompt = true);

    double get_delta_time() const { return delta_time; }
protected:
    //! The detection time difference, between the two photons.
    double delta_time;

}; /*-coincidence event*/

class CListRecord : public virtual ListRecord
{
public:
  
  //! Used in TOF reconstruction to get both the geometric and the timing
  //!  component of the event
  virtual void full_event(Bin&, const ProjDataInfo&) const
  {error("CListRecord::full_event() is implemented only for records which "
         "hold timing and spatial information.");}
};

class CListRecordWithGatingInput : public CListRecord, public ListRecordWithGatingInput
{
};

END_NAMESPACE_STIR

#endif
