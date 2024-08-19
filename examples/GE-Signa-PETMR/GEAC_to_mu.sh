#! /bin/bash -e

# Example script to create a mu-file in STIR units from a CTAC
# could be adapted for MRAC if slopes are adjusted
# Author: Kris Thielemans
# SPDX-License-Identifier: Apache-2.0
# See STIR/LICENSE.txt for details

if [ $# -ne 2 ]; then
  echo "Usage: $(basename $0) output_filename CT_filename"
  echo "This creates a mu-map from GE-CT (preliminary!)"
  echo "Set the kV environment variable if tube voltage was not 120."
  echo "e.g. env kV=80 $(basename $0) ..."
  exit 1
fi

# directory with some standard .par files
: ${pardir:=$(dirname $0)}

# Find default location of slopes file
: ${STIR_config_dir:=$(stir_config --config-dir)}
: ${CT_SLOPES_FILENAME:=$STIR_config_dir/ct_slopes.json}
if [ ! -r "$CT_SLOPES_FILENAME" ]; then
    echo "You need to set the CT_SLOPES_FILENAME environment variable to an existing file."
    exit 1
fi
# note: variable name is used in .par
export CT_SLOPES_FILENAME

# TODO get kV from the CT dicom header
: ${kV:=120}
export kV

# STIR 5 supports a filter that can both do the slopes and Gaussian
postfilter "$1" "$2"  "${pardir}/GE_HUToMu.par"
# lines for old STIR
# ctac_to_mu_values  -o "$1" -i "$2" -j $CT_SLOPES_FILENAME  -m GE -v $kV -k 511
# potentially postfilter with a Gaussian (usually required!)
