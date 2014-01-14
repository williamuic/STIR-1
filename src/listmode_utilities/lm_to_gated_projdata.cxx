//
// $Id: lm_to_projdata.cxx,v 1.10 2009-06-22 13:49:48 kris Exp $
//
/*!
  \file 
  \ingroup listmode_utilities

  \brief Program to bin listmode data to 3d sinograms

  \see class stir::LmToProjData for info on parameter file format

  \author Kris Thielemans
  \author Sanida Mustafovic
  
  $Date: 2009-06-22 13:49:48 $
  $Revision: 1.10 $
*/
/*
    Copyright (C) 2000- $Date: 2009-06-22 13:49:48 $, Hammersmith Imanet Ltd
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

#include "UCL/listmode/LmToGatedProjData.h"

#ifndef STIR_NO_NAMESPACES
using std::cerr;
#endif

USING_NAMESPACE_STIR



int main(int argc, char * argv[])
{
  
  if (argc!=1 && argc!=2) {
    cerr << "Usage: " << argv[0] << " [par_file]\n";
    exit(EXIT_FAILURE);
  }
  LmToGatedProjData application(argc==2 ? argv[1] : 0);
  application.process_data();

  return EXIT_SUCCESS;
}

