/*
   RDF8.h

   Author:      Benjamin A. Thomas

   Copyright 2017 Institute of Nuclear Medicine, University College London.

   This file is part of STIR.

   SPDX-License-Identifier: Apache-2.0

   See STIR/LICENSE.txt for details

   GE RDF8 file reader class

 */

#ifndef _RDF8_H
#define _RDF8_H

#include <iostream>
#include <fstream>
#include <vector>
#include <typeinfo>
#include <string>
#include <map>
#include <cstdint>
//BOOST
#include <boost/filesystem.hpp>
#include <boost/any.hpp>

namespace fs = boost::filesystem;


namespace nmtools
{
namespace IO
{
namespace ge
{
  typedef float float32_t;

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
  std::uint32_t RDFConfigStructOffset;
  std::uint32_t sorterStructOffset;
  std::uint32_t singlesStructOffset;
  std::uint32_t deadTimeStructOffset;
  std::uint32_t acqParamStructOffset;
  std::uint32_t computeParmStructOffset;
  std::uint32_t petExamStructOffset;
  std::uint32_t acqStatsStructOffset;
  std::uint32_t Norm3DStructOffset;
  std::uint32_t sysGeometryStructOffset;
  std::uint32_t calSetStructOffset;
  std::uint32_t ctcCrystalTimeDiffStructOffset;
  std::uint32_t compressStructOffset;
  std::uint32_t listHeaderOffset;
  std::uint32_t detModuleSignatureOffset;
  std::uint32_t spares[2];
};

std::string getGEDate(std::string date);
std::string getGETime(std::string time);

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

class CRDF8CONFIG : public RDF8Base
{

public:
  CRDF8CONFIG(){};

  bool Read(const fs::path);
  bool GetField(const std::string sid, boost::any &data) const;
  bool SetField(const std::string, const boost::any data){return false;}; //TODO: Implement this
  bool Write(const boost::filesystem::path path){return false;}; //TODO: Implement this

  ~CRDF8CONFIG(){};

protected:

  std::uint32_t _majorVersion;
  std::uint32_t _minorVersion;
  std::uint32_t _RDFComplete;
  std::uint32_t _deadTimeVersion;
  std::uint32_t _singlesVersion;
  std::uint32_t _isListFile;
  std::uint64_t _fileSizeInBytes;
  std::uint32_t _spares[2];

  bool populateDictionary();
  float GetVersionNumber();

  //Print info for debugging
  friend std::ostream &operator<<(std::ostream &os, const CRDF8CONFIG &rdf);
};

class CRDF8EXAM : public RDF8Base
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
  std::uint32_t _patientSex;
  std::uint32_t _examID[2];
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
  std::uint32_t _scanID[2];
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
  std::uint32_t _patientType;
  std::string _softwareVersion;
  std::uint32_t _isotopeHasPromptGamma;
  std::uint32_t _spares[9];

  bool populateDictionary();

  const std::string getPatientDOB() const { return getGEDate(_patientBirthdate); };
  const std::string getStudyScanDate() const { return getGEDate(_measDateTime); };
  const std::string getStudyScanTime() const { return getGETime(_measDateTime); };

  //Print info for debugging
  friend std::ostream &operator<<(std::ostream &os, const CRDF8EXAM &rdf);
};

class CRDF8ACQ : public RDF8Base
{
public:

  bool Read(const fs::path);
  bool GetField(const std::string sid, boost::any &data) const;

protected:
  std::uint32_t _termCondition;
  std::uint32_t _totalPrompts;
  std::uint32_t _totalDelays;
  std::uint32_t _acceptedTriggers;
  std::uint32_t _rejectedTriggers;
  std::uint32_t _scanStartTime;
  std::uint32_t _frameStartTime;
  std::uint32_t _frameDuration;
  std::string _frameID;
  std::uint32_t _binNumber;
  std::unique_ptr<std::vector<std::uint32_t>> _accumBinDuration;
  std::uint32_t _totalPromptsMs;
  std::uint32_t _totalDelaysMs;
  std::uint32_t _sorterFilteredEvtsLS;
  std::uint32_t _sorterFilteredEvtsMS;
  std::uint32_t _badCoincStreamEvts;
  std::uint32_t _frameNumber;
  std::uint32_t _isRejectBin;
  std::uint32_t _frameStartCoincTStamp;
  std::uint32_t _readyToScanUTC;
  std::uint32_t spares[5];
  
};

class RDF8Info {

private:
  std::unique_ptr<CRDF8CONFIG> config;
  std::unique_ptr<CRDF8EXAM> exam;

};

} //namespace ge
} //IO
} //namespace nmtools
#endif
