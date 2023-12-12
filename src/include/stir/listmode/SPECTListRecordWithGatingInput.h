///
//
/*!
  \file
  \ingroup listmode
  \brief Declarations of classes stir::SPECTListRecordWithGatingInput which
  is used for list mode data.

  \author Daniel Deidda
  \author Kris Thielemans

*/
/*
    Copyright (C) 2019, National Physical Laboratory
    Copyright (C) 2019, 2023, University College of London
    This file is part of STIR.

    SPDX-License-Identifier: Apache-2.0

    See STIR/LICENSE.txt for details
*/

#ifndef __stir_listmode_SPECTListRecordWithGatingInput_H__
#define __stir_listmode_SPECTListRecordWithGatingInput_H__

#include "stir/listmode/SPECTListRecord.h"
#include "stir/listmode/ListGatingInput.h"
#include "stir/listmode/ListRecordWithGatingInput.h"
#include "stir/Succeeded.h"

START_NAMESPACE_STIR

class SPECTListRecordWithGatingInput : public SPECTListRecord, public ListRecordWithGatingInput
{
};

END_NAMESPACE_STIR

#endif
