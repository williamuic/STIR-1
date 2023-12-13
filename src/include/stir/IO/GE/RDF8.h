/*
   RDF8.h

   Author:      Benjamin A. Thomas
   Author:      Kris Thielemans

   Copyright 2017, 2023 Institute of Nuclear Medicine, University College London.

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
#include <memory>
//BOOST
#include <boost/any.hpp>

#ifdef HAVE_BOOST_FILESYSTEM
#include <boost/filesystem.hpp>
#endif

namespace nmtools
{
namespace IO
{
namespace ge
{
  typedef float float32_t;
  using std::uint32_t;
  using std::int32_t;
#ifdef HAVE_BOOST_FILESYSTEM
  typedef boost::filesystem::path path_t;
#else
  typedef std::string path_t;
#endif

//Contents of rdfConstants.m from GETPETToolbox
constexpr int ACQ_MAX_BINS = 64;
constexpr int S_RDF_MAX_TEO_MASK_WIDTH = 283;
constexpr int SHARC_DOS_MAX_PATH_SIZE = 180;
constexpr int IDB_LEN_ID = 65;
constexpr int IDB_LEN_DATETIME_STR = 27;
constexpr int IDB_LEN_CAL_DESCRIPTION = 33;
constexpr int IDB_LEN_MANUFACTURER = 65;
constexpr int IDB_LEN_MODALITY = 5;
constexpr int IDB_LEN_OPERATOR = 5;
constexpr int IDB_LEN_PATIENT_HISTORY = 61;
constexpr int IDB_LEN_PATIENT_ID = 21;
constexpr int IDB_LEN_PATIENT_IDENTIFIER = 65;
constexpr int IDB_LEN_PATIENT_NAME = 65;
constexpr int IDB_LEN_RADIONUCLIDE = 7;
constexpr int IDB_LEN_REF_PHYSICIAN = 65;
constexpr int IDB_LEN_REQUISITION = 17;
constexpr int IDB_LEN_SCAN_DESCRIPTION = 65;
constexpr int IDB_LEN_SCANNER_DESC = 33;
constexpr int IDB_CNT_ID_INTS = 2;
constexpr int IDB_LEN_HOSPITAL_NAME = 33;
constexpr int IDB_LEN_EXAM_DESC = 65;
constexpr int IDB_LEN_DIAGNOSTICIAN = 33;
constexpr int IDB_LEN_LANDMARK_NAME = 65;
constexpr int IDB_LEN_LANDMARK_ABBREV = 3;
constexpr int IDB_LEN_TRACER_NAME = 41;
constexpr int IDB_LEN_BATCH_DESCRIPTION = 41;
constexpr int SYS_NUM_AXIAL_SLICES_MAX = 47;
constexpr int SYS_NUM_MAJOR_RINGS_MAX = 4;
constexpr int SYS_NUM_MINOR_RINGS_MAX = 24;
constexpr int SYS_CRYSTALS_PER_BLOCK_MAX = 54;

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

 constexpr int pad4(const int i) { return i - 1 + 4 - (i - 1) % 4; }

class RDF8Base
{
 public:
  static constexpr unsigned RDF_MAX_PATH_SIZE = 180;
  static constexpr unsigned RDF_MAX_SYS_PATH_SIZE = 256;

  virtual bool Read(const path_t) = 0;
  virtual bool GetField(const std::string sid, boost::any &data) const;
  virtual bool SetField(const std::string, const boost::any data){return false;}; //TODO: Implement this
  virtual bool Write(const path_t path){return false;}; //TODO: Implement this

protected: 

  RDF8HDROFFSETS _offsets;

  //! open file, perform checks and sets _offsets
  /*!
    param[out] fin stream-object, will be set to (binary) stream
    param[in] inFilePath
    return \c true if successful (\a fin will be closed otherwise)
  */
  bool ReadOffsets(std::ifstream& fin, const path_t inFilePath);

  typedef std::pair<std::string, boost::any> DictionaryItem;
  typedef std::map<std::string, boost::any> Dictionary;
  std::unique_ptr<Dictionary> _dict;

  //Form dictionary after a successful read.
  virtual bool populateDictionary() = 0;

  RDF8Base(){};
  virtual ~RDF8Base(){};

};

class CRDF8CONFIG : public RDF8Base
{

public:
  CRDF8CONFIG(){};

  bool Read(const path_t) override;
  bool Write(const path_t path) override {return false;} //TODO: Implement this

  float GetVersionNumber();
  bool  IsListFile() const { return _isListFile; }

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

  //Print info for debugging
  friend std::ostream &operator<<(std::ostream &os, const CRDF8CONFIG &rdf);
};

class CRDF8EXAM : public RDF8Base
{

public:

  bool Read(const path_t) override;
  bool Write(const path_t path) override {return false;} //TODO: Implement this

  bool WriteFile(const path_t, const path_t);
  bool WriteFile(const path_t, const path_t, std::string);

  bool SetPatientID(std::string newID);
  bool SetPatientDicomID(std::string newID);
  bool SetPatientName(std::string newName);

  bool SetExamUID(std::string examUID);
  bool SetScanUID(std::string scanUID);

  bool RemovePII();

  const std::string getPatientDOB() const { return getGEDate(_patientBirthdate); };
  const std::string getStudyScanDate() const { return getGEDate(_measDateTime); };
  const std::string getStudyScanTime() const { return getGETime(_measDateTime); };
  const std::string getScannerDescription() const { return _scannerDesc; }

protected:

  bool CleanField(std::string &, const std::size_t, std::string);

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

  //Print info for debugging
  friend std::ostream &operator<<(std::ostream &os, const CRDF8EXAM &rdf);
};

 class CRDF8ACQPARAMS : public RDF8Base
 {
 public:

    //! enum for encoding head/feet first in the RDF file
    enum AcqPatientEntries
    { ACQ_HEAD_FIRST=0, ACQ_FEET_FIRST=1};
    //! enum for encoding patient orientation in the RDF file
    enum AcqPatientPositions
    { ACQ_SUPINE=0, ACQ_PRONE=1, ACQ_LEFT_DECUB=2, ACQ_RIGHT_DECUB=3};

   bool Read(const path_t) override;

   struct AcqLandmarkParams {
     uint32_t _landmarkQualifier;
     uint32_t _patientEntry;
     uint32_t _patientPosition;
     int32_t _absTableLongitude;
     int32_t _gantryTilt;
     int32_t _tableElevation;
     uint32_t _landmarkDateTime;
     uint32_t _spares;
   };
   AcqLandmarkParams acq_landmark_params;

   struct AcqScanParams
   {
     uint32_t _scanPerspective;
     uint32_t _scanType;
     uint32_t _scanMode;
     uint32_t _eventSource;
     std::string _eventSimulation; // pad4(RDF_MAX_SYS_PATH_SIZE)); // Changed to padded in RDFv8
     uint32_t _startCondition;
     uint32_t _stopCondition;
     uint32_t _stopCondData;
     uint32_t _delayedEvents;
     uint32_t _delayedSubtractBias;
     uint32_t _thetaCompression;
     float32_t _gantryTilt;
     uint32_t _collimation;
     float32_t _tableLocation;
     uint32_t _acqDelay;
     uint32_t _acqTime;
     float32_t _startAngle;
     float32_t _deltaAngle;
     float32_t _angleThickness;
     uint32_t _startSlice;
     uint32_t _deltaSlice;
     uint32_t _slicesCompressed;
     uint32_t _singleCollect;
     uint32_t _deadtimeCollect;
     uint32_t _TransPlusEmiss;
     uint32_t _axialCompression;
     uint32_t _startCondData;
     float32_t _ct_kv;
     std::string _ct_contrast; // 64;
     std::string _frame_of_reference; // 64
     uint32_t _axialAcceptance;
     uint32_t _retroScan;
     uint32_t _tofCompressionFactor; // New for RDFv8
     uint32_t _extraRsForTFOV; // New for RDFv8
     uint32_t _spares; // Changed to 1 in RDFv8
   };
   AcqScanParams acq_scan_params;

   struct EdcatParams
   {
     int32_t _posAxialAcceptanceAngle;
     int32_t _negAxialAcceptanceAngle;
     int32_t _posCoincidenceWindow;
     int32_t _negCoincidenceWindow;
     int32_t _delayWindowOffset;
     int32_t _transAxialFOV;
     int32_t _coinOutputMode;
     uint32_t _upper_energy_limit;
     uint32_t _lower_energy_limit;
     uint32_t _majorClockPeriodFEE;
     float32_t _coincTimingPrecision;
     uint32_t _crystalsInTFOV;
     uint32_t _spares[6];
   };
   EdcatParams acq_edcat_params;

   struct AcqRxGatedParams
   {
     uint32_t _binningMode;
     uint32_t _numberOfBins;
     float32_t _binDurations[ACQ_MAX_BINS];
     uint32_t _trigRejMethod;
     uint32_t _nTrigRejections;
     uint32_t _upperRejLimit;
     uint32_t _lowerRejLimit;
     uint32_t _physioGatingType;
     uint32_t _spares[9];
   };
   AcqRxGatedParams acq_rx_gated_params;

   struct AcqTransControl
   {
     uint32_t _tsHolder1;
     uint32_t _tsHolder2;
     uint32_t _tsSpeed;
     uint32_t _tsLocation;
     uint32_t _teoMaskWidth;
     float32_t _teoMaskScaleFactor; // Changed to float in RDFv8
     int16_t _teoMaskRadialSum[pad4(S_RDF_MAX_TEO_MASK_WIDTH)];
     uint32_t _spares[10];
   };
   AcqTransControl trans_control;

   struct ImageNumbering
   {
     float32_t _locationOfImageOne;
     float32_t _locationOfImageOneIndx;
     uint32_t _prospectiveNumbOfImageSlices;
     uint32_t _spares;
   };
   ImageNumbering image_numbering_data;

   struct BackEndFilters// new in RDFv8
   {
     uint32_t _maxRingDiff;
     uint32_t _maxCoincDiffLSBs;
     float32_t _transaxialFovInMM;
     float32_t _maxEnergyKeV;
     float32_t _minEnergyKeV;
     uint32_t _spares[3];
   };
   BackEndFilters back_end_acq_filters;

   // spares = fread(fid, 2,'uint32');
 protected:
  bool populateDictionary() override { /*TODO*/ return false;}
 };


class CRDF8ACQSTATS : public RDF8Base
{
public:

  bool Read(const path_t) override;

 public:
  std::uint32_t _terminationCondition;
  std::uint32_t _totalPrompts;
  std::uint32_t _totalDelays;
  std::uint32_t _acceptedTriggers;
  std::uint32_t _rejectedTriggers;
  std::uint32_t _scanStartTime;
  std::uint32_t _frameStartTime;
  std::uint32_t _frameDuration;
  std::string _frameID;
  std::uint32_t _binNumber;
  std::uint32_t _accumBinDuration[ACQ_MAX_BINS];
  std::uint32_t _totalPromptsMs;
  std::uint32_t _totalDelaysMs;
  std::uint32_t _sorterFilteredEvtsLS;
  std::uint32_t _sorterFilteredEvtsMS;
  std::uint32_t _badCoincStreamEvts;
  std::uint32_t _frameNumber;
  std::uint32_t _isRejectBin;
  std::uint32_t _frameStartCoincTStamp;
  std::uint32_t _readyToScanUTC; // New in RDFv8
  std::uint32_t _spares[5]; // changed to 5 in RDF8

 protected:
  bool populateDictionary() override { /*TODO*/ return false;}
};

class CRDF8SYSTEMGEO : public RDF8Base {
 public:
  bool Read(const path_t) override;

  static constexpr unsigned RDF_NUM_MAJOR_RINGS_MAX = 6;
  static constexpr unsigned RDF_NUM_MINOR_RINGS_MAX = 60;
  static constexpr unsigned RDF_NUM_AXIAL_SLICES_MAX = (2*RDF_NUM_MINOR_RINGS_MAX)-1;
  static constexpr unsigned RDF_CRYSTALS_PER_BLOCK_MAX = 120;


  //protected:
  std::uint32_t _radialModulesPerSystem;
  std::uint32_t _radialBlocksPerModule;
  std::uint32_t _radialCrystalsPerBlock;
  std::uint32_t _axialModulesPerSystem;
  std::uint32_t _axialBlocksPerModule;
  std::uint32_t _axialCrystalsPerBlock;
  float32_t _detectorRadialSize;
  float32_t _detectorAxialSize;
  float32_t _axialCrystalGap;
  float32_t _radialCrystalGap;
  float32_t _axialBlockGap;
  float32_t _radialBlockGap;
  float32_t _axialCassetteGap;
  float32_t _radialCassetteGap;
  float32_t _sourceRadius;
  float32_t _collimatorInnerRadius;
  float32_t _collimatorOuterRadius;
  float32_t _delaysCorrectionFactor;
  float32_t _effectiveRingDiameter;
  std::uint32_t _blockRepeatFactor;
  float32_t _interCrystalPitch;
  float32_t _interBlockPitch;
  float32_t _scatterHrParameters[10];
  float32_t _scatterHsParameters[10];
  float32_t _dt_intCorrectionConstant;
  float32_t _dt_muxCorrectionConstant;
  float32_t _dt_timingCorrectionConstant;
  std::int32_t _numCoincAsics;
  float32_t _dt_asicChipFactors[7];
  float32_t _dt_3dasicChipFactors[7];
  float32_t _dt_3dintCorrectionConstant;
  float32_t _dt_3dmuxCorrectionConstant;
  float32_t _dt_3dtimingCorrectionConstant;
  float32_t _transaxial_crystal_0_offset;
  float32_t _vqc_XaxisTranslation;
  float32_t _vqc_YaxisTranslation;
  float32_t _vqc_ZaxisTranslation;
  float32_t _vqc_XaxisTilt;
  float32_t _vqc_YaxisSwivel;
  float32_t _vqc_ZaxisRoll;
  std::uint32_t _scanner_first_slice;
  std::uint32_t _collimatorType;
  std::uint32_t _timingResolutionInPico;
  float32_t _avgBlockDeadtime;
  float32_t _avgCrystalSingles;
  float32_t _spares[5];
  float32_t _dt_crossRingFactors[RDF_NUM_MAJOR_RINGS_MAX];
  float32_t _dt_3dpileUp_factors[RDF_NUM_MINOR_RINGS_MAX];
  float32_t _dt_hrPileUp_factors[RDF_NUM_AXIAL_SLICES_MAX];
  float32_t _dt_hsPileUp_factors[RDF_NUM_AXIAL_SLICES_MAX];
  float32_t _dt_3dCrystalPileupFactors[RDF_CRYSTALS_PER_BLOCK_MAX];
//bulk_spares = fread(fid, 8,'uint32');

 protected:
  bool populateDictionary() override;
};

class CRDF8SORTERDATA : public RDF8Base {
  bool Read(const path_t) override;

  //protected:
  std::uint32_t _dataOrientation;
  std::uint32_t _dimension1Size;
  std::uint32_t _dimension2Size;
  std::uint32_t _histogramCellSize;
  std::uint32_t _sinoAlignCorr;
  std::uint32_t _DHMErrorFifoDepth;
  std::uint32_t _acquisitionNumber;
  std::uint32_t _numberOfAcquisitions;

/*
%  data segment headers (0:7 possible segments)
% Segment 0: Unused (Transmission Prompts, CTAC Raw Data)
% Segment 1: Unused (Transmission Delays)
% Segment 2: Emission Prompts
% Segment 3: Unused (Emission Delays)
% Segment 4: Cal
% Segment 5: Unused
% Segment 6: Unused
% Segment 7: Unused (TOF orientation)
*/
 static constexpr unsigned numSegments = 8;
 struct ACQDATASEGMENTPARAMS
 {
   std::uint32_t _segmentType;
   std::uint32_t _dimension3Size;
   std::uint32_t _numScaleFactors;
   std::uint32_t _scaleFactorsOffset;
   std::uint64_t _dataSegmentOffset;
   std::uint64_t _compDataSegOffset;
   std::uint64_t _compDataSegSize;
   std::uint64_t _segFirstCvtEntryOffset;
   std::uint32_t _segCvtEntries;
   std::uint32_t _tofCollapsed;
   std::uint32_t _spares[6];
 };
 ACQDATASEGMENTPARAMS _acqDataSegmentParams[numSegments];

 protected:
  bool populateDictionary() override;
};

class CRDF8LIST : public RDF8Base {
 public:

  static const unsigned RDF_NUM_LIST_COMPRESS_ALG_COEFS = 4U;
  bool Read(const path_t) override;

  std::uint32_t GetListStartOffset() const { return _listStartOffset; }
  std::uint32_t IsListCompressed() const { return _isListCompressed; }
  std::uint32_t GetFirstTmAbsTimeStamp() const { return _firstTmAbsTimeStamp; }
  std::uint32_t GetLastTmAbsTimeStamp() const { return _lastTmAbsTimeStamp; }

 protected:
  std::uint32_t _listType;
  std::uint32_t _numAssocListFiles;
  std::uint32_t _whichAssocLFile;
  std::uint32_t _listAcqTime;
  std::uint32_t _listStartOffset;
  std::uint32_t _isListCompressed;
  std::uint32_t _listCompressionAlg;
  std::uint32_t _evalAsBadCompress; // New to RDFv8
  std::uint32_t _areEvtTimeStampsKnown; // New to RDFv8
  std::uint32_t _firstTmAbsTimeStamp; // New to RDFv8
  std::uint32_t _lastTmAbsTimeStamp; // New to RDFv8
  //std::uint32_t _spares;
  std::uint64_t _sizeOfCompressedList; // New to RDFv8
  std::uint64_t _sizeOfList;
  double _listCompAlgCoefs[RDF_NUM_LIST_COMPRESS_ALG_COEFS]; // New to RDFv8

  bool populateDictionary() override;
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
