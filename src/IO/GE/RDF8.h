/*
   RDF8.h

   Author:      Benjamin A. Thomas

   Copyright 2017 Institute of Nuclear Medicine, University College London.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   GE RDF8 file reader class

 */

#ifndef _RDF8_H
#define _RDF8_H

#include <iostream>
#include <fstream>
#include <vector>

//BOOST
#include <boost/cstdfloat.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/lexical_cast.hpp>

#include "dicom/DicomTools.h"
#include "common/IRawFileIO.h"

namespace dcm = nmtools::IO::dicom;
namespace fs = boost::filesystem;

#ifndef float32_t
#define float32_t boost::float32_t
#endif

namespace nmtools
{
namespace IO
{
namespace ge
{
//Contents of rdfConstants.m from GETPETToolbox
const int ACQ_MAX_BINS = 64;
const int S_RDF_MAX_TEO_MASK_WIDTH = 283;
const int SHARC_DOS_MAX_PATH_SIZE = 180;
const int IDB_LEN_ID = 65;
const int IDB_LEN_DATETIME_STR = 27;
const int IDB_LEN_CAL_DESCRIPTION = 33;
const int IDB_LEN_MANUFACTURER = 65;
const int IDB_LEN_MODALITY = 5;
const int IDB_LEN_OPERATOR = 5;
const int IDB_LEN_PATIENT_HISTORY = 61;
const int IDB_LEN_PATIENT_ID = 21;
const int IDB_LEN_PATIENT_IDENTIFIER = 65;
const int IDB_LEN_PATIENT_NAME = 65;
const int IDB_LEN_RADIONUCLIDE = 7;
const int IDB_LEN_REF_PHYSICIAN = 65;
const int IDB_LEN_REQUISITION = 17;
const int IDB_LEN_SCAN_DESCRIPTION = 65;
const int IDB_LEN_SCANNER_DESC = 33;
const int IDB_CNT_ID_INTS = 2;
const int IDB_LEN_HOSPITAL_NAME = 33;
const int IDB_LEN_EXAM_DESC = 65;
const int IDB_LEN_DIAGNOSTICIAN = 33;
const int IDB_LEN_LANDMARK_NAME = 65;
const int IDB_LEN_LANDMARK_ABBREV = 3;
const int IDB_LEN_TRACER_NAME = 41;
const int IDB_LEN_BATCH_DESCRIPTION = 41;
const int SYS_NUM_AXIAL_SLICES_MAX = 47;
const int SYS_NUM_MAJOR_RINGS_MAX = 4;
const int SYS_NUM_MINOR_RINGS_MAX = 24;
const int SYS_CRYSTALS_PER_BLOCK_MAX = 54;

struct RDF8HDROFFSETS
{
  uint32_t RDFConfigStructOffset;
  uint32_t sorterStructOffset;
  uint32_t singlesStructOffset;
  uint32_t deadTimeStructOffset;
  uint32_t acqParamStructOffset;
  uint32_t computeParmStructOffset;
  uint32_t petExamStructOffset;
  uint32_t acqStatsStructOffset;
  uint32_t Norm3DStructOffset;
  uint32_t sysGeometryStructOffset;
  uint32_t calSetStructOffset;
  uint32_t ctcCrystalTimeDiffStructOffset;
  uint32_t compressStructOffset;
  uint32_t listHeaderOffset;
  uint32_t detModuleSignatureOffset;
  uint32_t spares[2];
};

class RDF8Base
{

protected: 

  RDF8HDROFFSETS _offsets;
  bool ReadOffsets(const fs::path);

  typedef std::pair<std::string, boost::any> DictionaryItem;
  typedef std::map<std::string, boost::any> Dictionary;
  std::unique_ptr<Dictionary> _dict;

  //Form dictionary after a successful read.
  bool populateDictionary();

  RDF8Base(){};
  ~RDF8Base(){};

};

class CRDF8CONFIG : public RDF8Base, IRawFileIO
{

public:
  CRDF8CONFIG(){};

  bool Read(const fs::path);
  bool GetField(const std::string sid, boost::any &data) const;
  bool SetField(const std::string, const boost::any data){return false;}; //TODO: Implement this
  bool Write(const boost::filesystem::path path){return false;}; //TODO: Implement this

  ~CRDF8CONFIG(){};

protected:

  uint32_t _majorVersion;
  uint32_t _minorVersion;
  uint32_t _RDFComplete;
  uint32_t _deadTimeVersion;
  uint32_t _singlesVersion;
  uint32_t _isListFile;
  uint64_t _fileSizeInBytes;
  uint32_t _spares[2];

  bool populateDictionary();
  float GetVersionNumber();

  //Print info for debugging
  friend std::ostream &operator<<(std::ostream &os, const CRDF8CONFIG &rdf);
};

class CRDF8EXAM : public RDF8Base, IRawFileIO
{

public:
  CRDF8EXAM(){};

  bool Read(const fs::path);
  bool GetField(const std::string sid, boost::any &data) const;
  bool SetField(const std::string, const boost::any data){return false;}; //TODO: Implement this
  bool Write(const boost::filesystem::path path){return false;}; //TODO: Implement this

  bool WriteFile(const fs::path, const fs::path);
  bool WriteFile(const fs::path, const fs::path, std::string);

  bool SetPatientID(std::string newID);
  bool SetPatientDicomID(std::string newID);
  bool SetPatientName(std::string newName);

  bool SetExamUID(std::string examUID);
  bool SetScanUID(std::string scanUID);

  bool RemovePII();

  ~CRDF8EXAM(){};

protected:
  int pad4(const int i) { return i - 1 + 4 - (i - 1) % 4; };

  bool CleanField(std::string &, const int, std::string);

  std::string _patientID;
  std::string _patientName;
  std::string _patientBirthdate;
  uint32_t _patientSex;
  uint32_t _examID[2];
  std::string _requisition;
  std::string _hospitalName;
  std::string _scannerDesc;
  std::string _examDesc;
  std::string _refPhysician;
  std::string _diagnostician;
  std::string _operator;
  float32_t _patientHt;
  float32_t _patientWt;
  std::string _patientHistory;
  std::string _modality;
  std::string _manufacturer;
  uint32_t _scanID[2];
  std::string _scanDescription;
  std::string _landmarkName;
  std::string _landmarkAbbrev;
  std::string _tracerName;
  std::string _batchDescription;
  float32_t _tracerActivity;
  std::string _measDateTime;
  std::string _adminDateTime;
  std::string _radionuclideName;
  float32_t _halfLife;
  float32_t _source1Activity;
  std::string _source1MeasDateTime;
  std::string _source1Radionuclide;
  float32_t _source1HalfLife;
  float32_t _source2Activity;
  std::string _source2MeasDateTime;
  std::string _source2Radionuclide;
  float32_t _source2HalfLife;
  std::string _normalCalID;
  std::string _blankCalID;
  std::string _wcCalID;
  float32_t _preInjectionVolume;
  float32_t _postInjectionActivity;
  std::string _postInjectionDateTime;
  float32_t _positronFraction;
  std::string _scanIdDicom;
  std::string _examIdDicom;
  std::string _normal2dCalID;
  std::string _patientIdDicom;
  uint32_t _patientType;
  std::string _softwareVersion;
  uint32_t _isotopeHasPromptGamma;
  uint32_t _spares[9];

  //Print info for debugging
  friend std::ostream &operator<<(std::ostream &os, const CRDF8EXAM &rdf);
};

class RDF8Info : public IRawFileIO {

private:
  std::unique_ptr<CRDF8CONFIG> config;
  std::unique_ptr<CRDF8EXAM> exam;

};

} //namespace ge
} //IO
} //namespace nmtools
#endif
