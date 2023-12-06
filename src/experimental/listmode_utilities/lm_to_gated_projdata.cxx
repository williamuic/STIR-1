/*!
  \file 
  \ingroup listmode_utilities

  \brief Program to bin listmode data to 3d sinograms

  \see class stir::LmToProjData for info on parameter file format

  \author Kris Thielemans
  \author Sanida Mustafovic
  
*/
/*
    Copyright (C) 2000- 2009, Hammersmith Imanet Ltd
    This file is part of STIR.

    SPDX-License-Identifier: Apache-2.0

    See STIR/LICENSE.txt for details
*/

#include "stir_experimental/listmode/LmToGatedProjData.h"

using std::cerr;

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

