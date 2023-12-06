#ifndef __stir_listmode_LmToGatedProjData_H__
#define __stir_listmode_LmToGatedProjData_H__
//
// $Id: LmToGatedProjData.h,v 1.14 2009-09-28 11:58:20 kris Exp $
//
/*!
  \file 
  \ingroup listmode

  \brief Declaration of the stir::LmToGatedProjData class which is used to bin listmode data to (3d) sinograms
 
  \author Kris Thielemans
  \author Sanida Mustafovic
  
  $Date: 2009-09-28 11:58:20 $
  $Revision: 1.14 $
*/
/*
    Copyright (C) 2000- $Date: 2009-09-28 11:58:20 $, Hammersmith Imanet Ltd
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


#include "stir/listmode/LmToProjData.h"

START_NAMESPACE_STIR


/*!
  \ingroup listmode

  \brief This class is used to bin listmode data to projection data,
  i.e. (3d) sinograms.

  It provides the basic machinery to go through a list mode data file,
  and write projection data for each time frame. 

  The class can parse its parameters from an input file. This has the
  following format:

  \verbatim
  lm_to_projdata Parameters:=

  input file := some_lm_file
  output filename prefix := my_favorite_name_for_the_projdata

  ; parameters that determine the sizes etc of the output

    template_projdata := some_projdata_file
    ; the next can be used to use a smaller number of segments than given 
    ; in the template
    maximum absolute segment number to process := 

  ; parameters for saying which events will be stored

    ; time frames (see TimeFrameDefinitions doc for format)
    frame_definition file := frames.fdef
    ; or a total number of events (if  larger than 0, frame definitions will be ignored)
    ; note that this normally counts the total of prompts-delayeds (see below)
    num_events_to_store := -1

  ; parameters relating to prompts and delayeds

    ; with the default values, prompts will be added and delayed subtracted
    ; to give the usual estimate of the trues.

    ; store the prompts (value should be 1 or 0)
    store prompts := 1  ;default
    ; what to do if it's a delayed event
    store delayeds := 1  ;default


  ; parameters related to normalisation
  ; default settings mean no normalisation
  ; Use with care!

    ; in pre normalisation, each event will contribute its 
    ; 'normalisation factor' to the bin
    ; in post normalisation, an average factor for the bin will be used
    do pre normalisation  := 0 ; default is 0
    ; type of pre-normalisation (see BinNormalisation doc)
    Bin Normalisation type for pre-normalisation := None ; default
    ; type of post-normalisation (see BinNormalisation doc)
    Bin Normalisation type for post-normalisation := None ; default

  ; miscellaneous parameters

    ; list each event on stdout and do not store any files (use only for testing!)
    ; has to be 0 or 1
    List event coordinates := 0

    ; if you're short of RAM (i.e. a single projdata does not fit into memory),
    ; you can use this to process the list mode data in multiple passes.
    num_segments_in_memory := -1

  End := 
  \endverbatim
  
  Hopefully the only thing that needs explaining are the parameters related
  to prompts and delayeds. These are used to allow different ways of
  processing the data. There are really only 3 useful cases:

  <ul>
  <li> 'online' subtraction of delayeds<br>
       This is the default, and adds prompts but subtracts delayeds.
       \code
    store prompts := 1 
    store delayeds := 1
       \endcode
  </li>
  <li> store prompts only<br>
       Use 
       \code
    store prompts := 1 
    store delayeds := 0
       \endcode

  </li>
  <li> store delayeds only<br>
       Use 
       \code
    store prompts := 0 
    store delayeds := 1
       \endcode
       Note that now the delayted events will be <strong>added</strong>,
       not subtracted.
  </li>
  </ul>

  \par Notes for developers

  The class provides several
  virtual functions. If a derived class overloads these, the default behaviour
  might change. For example, get_bin_from_event() might do motion correction.

  \todo Currently, there is no support for gating or energy windows. This
  could in principle be added by a derived class, but it would be better
  to do it here.
  \todo Timing info or so for get_bin_from_event() for rotating scanners etc.
  \todo There is overlap between the normalisation and the current treatment
  of bin.get_bin_value(). This is really because we should be using 
  something like a EventNormalisation class for pre-normalisation.

  \see CListModeData for more info on list mode data. 

*/

class LmToGatedProjData : public LmToProjData
{
public:

  //! Constructor taking a filename for a parameter file
  /*! Will attempt to open and parse the file. */
  LmToGatedProjData(const char * const par_filename);

  //! Default constructor
  /*! \warning leaves parameters ill-defined. Set them by parsing. */
  LmToGatedProjData();

  //! This function does the actual work
  virtual void process_data();
  
protected:

  //! \name parsing functions
  //@{
  virtual void set_defaults();
  virtual void initialise_keymap();
  virtual bool post_processing();
  //@}

  //! \name parsing variables
  //{@
  //! frame definitions
  std::string gate_definition_filename;

};

END_NAMESPACE_STIR

#endif
