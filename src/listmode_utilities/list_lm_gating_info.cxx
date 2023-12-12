/*
    Copyright (C) 2023, University College of London
    This file is part of STIR.

    SPDX-License-Identifier: Apache-2.0

    See STIR/LICENSE.txt for details

*/
/*!
  \file
  \ingroup listmode_utilities

  \brief Program to show gating-info in list-mode data

  \author Kris Thielemans
*/

#include "stir/listmode/ListTime.h"
#include "stir/listmode/ListGatingInput.h"
#include "stir/listmode/ListRecordWithGatingInput.h"
#include "stir/listmode/ListModeData.h"
#include "stir/IO/read_from_file.h"
#include <iostream>

USING_NAMESPACE_STIR

int main(int argc, char *argv[]) {
  const char *const program_name = argv[0];
  // skip program name
  --argc;
  ++argv;

  int gating_value = -1;
  unsigned long num_events_to_list = 0;
  while (argc > 1 && argv[0][0] == '-') {
    if (strcmp(argv[0], "--num-events-to-list") == 0) {
      num_events_to_list = atol(argv[1]);
    } else if (strcmp(argv[0], "--value") == 0) {
      gating_value = atoi(argv[1]) != 0;
    } else {
      std::cerr << "Unrecognised option\n";
      return EXIT_FAILURE;
    }
    argc -= 2;
    argv += 2;
  }

  if (argc != 1) {
    std::cerr << "Usage: " << program_name << "[options] lm_filename\n"
              << "Options:\n"
              << "--value <uint> : only print time if gating info matches the "
                 "specified value\n"
              << "--num-events-to-list <uint> : limit number of events written "
                 "to stdout\n"
              << "\nWithout the \"--value\" option, 2 tab-separated columns will be written (time and gating-info-value).\n"
              << "Time is printed as milliseconds\n";
    return EXIT_FAILURE;
  }

  shared_ptr<ListModeData> lm_data_ptr(read_from_file<ListModeData>(argv[0]));
  unsigned long num_listed_events = 0;

  // loop over all events in the listmode file
  shared_ptr<ListRecord> record_sptr = lm_data_ptr->get_empty_record_sptr();
  auto record_ptr =
      dynamic_cast<ListRecordWithGatingInput *>(record_sptr.get());
  if (!record_ptr)
    error("Listmode format does not seem to support gating input");
  auto &record = *record_ptr;

  std::uint64_t current_time = 0;
  while (num_events_to_list == 0 || num_events_to_list != num_listed_events) {
    if (lm_data_ptr->get_next_record(record) == Succeeded::no) {
      // no more events in file for some reason
      break; // get out of while loop
    }
    if (record.is_time()) {
      current_time = static_cast<std::uint64_t>(record.time().get_time_in_millisecs());
    }
    if (record.is_gating_input()) {
      const auto gating = record_ptr->gating_input().get_gating();
      if (gating_value >= 0) {
        if (gating == static_cast<unsigned>(gating_value))
          std::cout << current_time << '\n';
      } else {
        std::cout << current_time << '\t' << gating << '\n';
      }
    }
    ++num_listed_events;
  }

  return EXIT_SUCCESS;
}
