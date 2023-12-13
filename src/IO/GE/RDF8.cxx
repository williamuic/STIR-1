/*
   RDF8.cxx

   Author:      Benjamin A. Thomas

   Copyright 2017, 2023 Institute of Nuclear Medicine, University College London.

   This file is part of STIR.

   SPDX-License-Identifier: Apache-2.0

   See STIR/LICENSE.txt for details

   GE RDF8 file reader.

 */

#include "stir/IO/GE/RDF8.h"
#ifdef HAVE_BOOST_LOG
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/lexical_cast.hpp>
#else
#include "stir/warning.h"
#include "stir/info.h"
#include "stir/error.h"
#endif
#ifndef HAVE_BOOST_FILESYSTEM
#include <stdio.h> // for tmpnam
#include "stir/FilePath.h"
#endif

#ifdef HAVE_BOOST_DATETIME
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/gregorian/conversion.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#endif

#include <fstream>

namespace nmtools
{
namespace IO
{
namespace ge
{
  static std::string tostring(const path_t path)
  {
#ifdef HAVE_BOOST_FILESYSTEM
    return path.string();
#else
    return path;
#endif
  }

  template <class T> static void readField(std::ifstream& fin, T& f)
    {
      const std::size_t size = sizeof(T);
      fin.read(reinterpret_cast<char *>(&f), size);
    }


  static std::string readString(std::ifstream& fin, unsigned size)
  {
    char buf[size+1];
    fin.read(&buf[0], size);
    buf[size] = '\0';
    return std::string(buf);
  }

bool RDF8Base::GetField(const std::string sid, boost::any &data) const
{

  if (_dict == nullptr)
  {
#ifdef HAVE_BOOST_LOG
    BOOST_LOG_TRIVIAL(error) << "GE RDF8: dictionary is NULL! File not read yet?";
#else
    stir::error("GE RDF8 dictionary is NULL! File not read yet?");
#endif
    return false;
  }

  try
  {
    data = _dict->at(sid);
  }
  catch (std::out_of_range &e)
  {
#ifdef HAVE_BOOST_LOG
    BOOST_LOG_TRIVIAL(error) << e.what();
    BOOST_LOG_TRIVIAL(warning) << sid << " not found!";
#else
    stir::warning("GE RDF8 GetField: " + sid + " not found. Error: " + e.what());
#endif
  }

  return true;
}

  bool RDF8Base::ReadOffsets(std::ifstream& fin, const path_t inFilePath)
{
  //Reads the offsets from RDF8 file.

  fin.open(tostring(inFilePath).c_str(), std::ios::in | std::ios::binary);

  uint32_t BOM;

  if (fin.is_open())
  {
    fin.read(reinterpret_cast<char *>(&BOM), sizeof(uint32_t));
    fin.read(reinterpret_cast<char *>(&this->_offsets), sizeof(this->_offsets));
  }
  else
    return false;

  // BOOST_LOG_TRIVIAL(debug) << "BOM = " << std::hex << BOM;

  if (BOM != 0x0000FEFF)
  {
    // TODO should check byte-swap
#ifdef HAVE_BOOST_LOG
    BOOST_LOG_TRIVIAL(error) << "BOM is not valid!";
#else
    stir::error("GE RDF reader: first bytes invalid!");
#endif
    fin.close();
    return false;
  }

#ifdef HAVE_BOOST_LOG
  int status = 0;
  char* demangled = abi::__cxa_demangle(typeid(*this).name(), 0, 0, &status);
  BOOST_LOG_TRIVIAL(debug) << demangled << " - Reading...";
  free(demangled);
#endif
  return true;
}

   
bool CRDF8EXAM::Read(const path_t inFilePath)
{
  std::ifstream fin;
  if (!ReadOffsets(fin, inFilePath))
    return false;

  //Go to exam block of header.
  fin.seekg(_offsets.petExamStructOffset);

 _patientID = readString(fin, pad4(IDB_LEN_PATIENT_ID));
 _patientName = readString(fin, pad4(IDB_LEN_PATIENT_NAME));
 _patientBirthdate = readString(fin, pad4(IDB_LEN_DATETIME_STR));

  //Patient sex
  fin.read(reinterpret_cast<char *>(&_patientSex), sizeof(uint32_t));

  //Exam ID
  fin.read(reinterpret_cast<char *>(&_examID), sizeof(uint32_t) * IDB_CNT_ID_INTS);

 _requisition = readString(fin, pad4(IDB_LEN_REQUISITION));

 _hospitalName = readString(fin, pad4(IDB_LEN_HOSPITAL_NAME));

 _scannerDesc = readString(fin, pad4(IDB_LEN_SCANNER_DESC));

 _examDesc = readString(fin, pad4(IDB_LEN_EXAM_DESC));

 _refPhysician = readString(fin, pad4(IDB_LEN_REF_PHYSICIAN));

 _diagnostician = readString(fin, pad4(IDB_LEN_DIAGNOSTICIAN));

  //Operator
 _operator = readString(fin, pad4(IDB_LEN_OPERATOR));

  //Patient height
  fin.read(reinterpret_cast<char *>(&_patientHt), sizeof(float32_t));

  //Patient height
  fin.read(reinterpret_cast<char *>(&_patientWt), sizeof(float32_t));

 _patientHistory = readString(fin, pad4(IDB_LEN_PATIENT_HISTORY));

 _modality = readString(fin, pad4(IDB_LEN_MODALITY));

 _manufacturer = readString(fin, pad4(IDB_LEN_MANUFACTURER));

  //Scan ID
  fin.read(reinterpret_cast<char *>(&_scanID), sizeof(uint32_t) * IDB_CNT_ID_INTS);

 _scanDescription = readString(fin, pad4(IDB_LEN_SCAN_DESCRIPTION));

 _landmarkName = readString(fin, pad4(IDB_LEN_LANDMARK_NAME));

 _landmarkAbbrev = readString(fin, pad4(IDB_LEN_LANDMARK_ABBREV));

 _tracerName = readString(fin, pad4(IDB_LEN_TRACER_NAME));

 _batchDescription = readString(fin, pad4(IDB_LEN_BATCH_DESCRIPTION));

  //Tracer activity
  fin.read(reinterpret_cast<char *>(&_tracerActivity), sizeof(float32_t));

 _measDateTime = readString(fin, pad4(IDB_LEN_DATETIME_STR));

 _adminDateTime = readString(fin, pad4(IDB_LEN_DATETIME_STR));

 _radionuclideName = readString(fin, pad4(IDB_LEN_RADIONUCLIDE));

  //Half-life
  fin.read(reinterpret_cast<char *>(&_halfLife), sizeof(float32_t));

  //Source 1 activity
  fin.read(reinterpret_cast<char *>(&_source1Activity), sizeof(float32_t));

 _source1MeasDateTime = readString(fin, pad4(IDB_LEN_DATETIME_STR));

 _source1Radionuclide = readString(fin, pad4(IDB_LEN_RADIONUCLIDE));

  //Source 1 half-life
  fin.read(reinterpret_cast<char *>(&_source1HalfLife), sizeof(float32_t));

  //Source 2 activity
  fin.read(reinterpret_cast<char *>(&_source2Activity), sizeof(float32_t));

 _source2MeasDateTime = readString(fin, pad4(IDB_LEN_DATETIME_STR));

 _source2Radionuclide = readString(fin, pad4(IDB_LEN_RADIONUCLIDE));

  //Source 2 half-life
  fin.read(reinterpret_cast<char *>(&_source2HalfLife), sizeof(float32_t));

 _normalCalID = readString(fin, pad4(IDB_LEN_ID));

 _blankCalID = readString(fin, pad4(IDB_LEN_ID));

 _wcCalID = readString(fin, pad4(IDB_LEN_ID));

  //Pre-injection volume
  fin.read(reinterpret_cast<char *>(&_preInjectionVolume), sizeof(float32_t));

  //Post-injection activity
  fin.read(reinterpret_cast<char *>(&_postInjectionActivity), sizeof(float32_t));

 _postInjectionDateTime = readString(fin, pad4(IDB_LEN_DATETIME_STR));

  //Positron fraction
  fin.read(reinterpret_cast<char *>(&_positronFraction), sizeof(float32_t));

 _scanIdDicom = readString(fin, pad4(IDB_LEN_ID));

 _examIdDicom = readString(fin, pad4(IDB_LEN_ID));

 _normal2dCalID = readString(fin, pad4(IDB_LEN_ID));

 _patientIdDicom = readString(fin, pad4(IDB_LEN_ID));

  //Patient type
  fin.read(reinterpret_cast<char *>(&_patientType), sizeof(uint32_t));

 _softwareVersion = readString(fin, pad4(IDB_LEN_ID));

  //Isotope has prompt gamma
  fin.read(reinterpret_cast<char *>(&_isotopeHasPromptGamma), sizeof(uint32_t));

  //Spares
  fin.read(reinterpret_cast<char *>(&_spares), sizeof(uint32_t) * 9);

  fin.close();

  return populateDictionary();
}

bool CRDF8EXAM::populateDictionary(){

  this->_dict = std::unique_ptr<Dictionary>(new Dictionary);
  //C++17 std::make_unique

  if (this->_dict == nullptr) {
#ifdef HAVE_BOOST_LOG
    int status = 0;
    char* demangled = abi::__cxa_demangle(typeid(*this).name(), 0, 0, &status);
    BOOST_LOG_TRIVIAL(error) << demangled << "::populateDictionary - Cannot allocate RDF8 dictionary!";
    free(demangled);
#else
    stir::error("GE RDF8: populateDictionary - Cannot allocate dictionary");
#endif
    return false;
  }

  _dict->insert(DictionaryItem("PATIENT_NAME", _patientName));
  _dict->insert(DictionaryItem("PATIENT_ID", _patientID));
  _dict->insert(DictionaryItem("PATIENT_DOB", this->getPatientDOB()));
  _dict->insert(DictionaryItem("STUDY_SCAN_DATE", this->getStudyScanDate()));
  _dict->insert(DictionaryItem("STUDY_SCAN_TIME", this->getStudyScanTime())); //should probably be using acq_stats.scanStartTime.
  _dict->insert(DictionaryItem("SERIES_DESCRIPTION", _scanDescription));
  //_dict->insert(DictionaryItem("IMAGE_TYPE", _imageType));
  _dict->insert(DictionaryItem("MANUFACTURER", _manufacturer));
  _dict->insert(DictionaryItem("MODALITY_TYPE", _modality));
  _dict->insert(DictionaryItem("MODEL_NAME", _scannerDesc));

  return true;
}

bool CRDF8EXAM::WriteFile(const path_t srcFile, const path_t dstFile)
{
  //Opens srcFile as RDF8 and writes new file, dstFile, with
  //modified header.

  if (srcFile == dstFile)
  {
#ifdef HAVE_BOOST_LOG
    BOOST_LOG_TRIVIAL(error) << "Refusing to overwrite input file!";
#else
    stir::error("Refusing to overwrite input file!");
#endif
    return false;
  }

#ifdef HAVE_BOOST_FILESYSTEM
  if (fs::exists(dstFile))
  {
#ifdef HAVE_BOOST_LOG
    BOOST_LOG_TRIVIAL(error) << "Refusing to overwrite existing output file!";
#else
    stir::error("Refusing to overwrite existing output file!");
#endif
    return false;
  }
#else
  if (stir::FilePath::exists(dstFile))
    stir::error("Refusing to overwrite existing output file!");
#endif

#ifdef HAVE_BOOST_FILESYSTEM
  path_t tmpFile = boost::filesystem::unique_path();
  try
  {
    fs::copy(srcFile, tmpFile);
  }
  catch (fs::filesystem_error &e)
  {
#ifdef HAVE_BOOST_LOG
    BOOST_LOG_TRIVIAL(error) << "Could not copy " << srcFile << " to " << tmpFile;
#else
    stir::error("Could not copy " + tostring(srcFile) + " to " + tostring(tmpFile));
#endif
    return false;
  }
#else
  // TODO tmpnam is considered insecure.
  path_t tmpFile = tmpnam(NULL);
  {
    std::ifstream  src(tostring(srcFile).c_str(), std::ios::binary);
    std::ofstream  dst(tostring(tmpFile).c_str(), std::ios::binary);

    if (!src || !dst)
      stir::error("Could not copy " + tostring(srcFile) + " to " + tostring(tmpFile));

    dst << src.rdbuf();
  }
#endif

  std::fstream fout(tostring(tmpFile).c_str(), std::ios::in | std::ios::out | std::ios::binary);
  if (fout.is_open())
  {
    fout.seekp(_offsets.petExamStructOffset);
    fout << _patientID;
    fout << _patientName;
    fout << _patientBirthdate;
    fout.write((const char *)(&_patientSex), sizeof(_patientSex));
    fout.write((const char *)(&_examID), sizeof(_examID));
    fout << _requisition;
    fout << _hospitalName;
    fout << _scannerDesc;
    fout << _examDesc;
    fout << _refPhysician;
    fout << _diagnostician;
    fout << _operator;
    fout.write((const char *)(&_patientHt), sizeof(_patientHt));
    fout.write((const char *)(&_patientWt), sizeof(_patientWt));
    fout << _patientHistory;
    fout << _modality;
    fout << _manufacturer;
    fout.write((const char *)(&_scanID), sizeof(_scanID));
    fout << _scanDescription;
    fout << _landmarkName;
    fout << _landmarkAbbrev;
    fout << _tracerName;
    fout << _batchDescription;
    fout.write((const char *)(&_tracerActivity), sizeof(_tracerActivity));
    fout << _measDateTime;
    fout << _adminDateTime;
    fout << _radionuclideName;
    fout.write((const char *)(&_halfLife), sizeof(_halfLife));
    fout.write((const char *)(&_source1Activity), sizeof(_source1Activity));
    fout << _source1MeasDateTime;
    fout << _source1Radionuclide;
    fout.write((const char *)(&_source1HalfLife), sizeof(_source1HalfLife));
    fout.write((const char *)(&_source2Activity), sizeof(_source2Activity));
    fout << _source2MeasDateTime;
    fout << _source2Radionuclide;
    fout.write((const char *)(&_source2HalfLife), sizeof(_source2HalfLife));
    fout << _normalCalID;
    fout << _blankCalID;
    fout << _wcCalID;
    fout.write((const char *)(&_preInjectionVolume), sizeof(_preInjectionVolume));
    fout.write((const char *)(&_postInjectionActivity), sizeof(_postInjectionActivity));
    fout << _postInjectionDateTime;
    fout.write((const char *)(&_positronFraction), sizeof(_positronFraction));
    fout << _scanIdDicom;
    fout << _examIdDicom;
    fout << _normal2dCalID;
    fout << _patientIdDicom;
    fout.write((const char *)(&_patientType), sizeof(_patientType));
    fout << _softwareVersion;
    fout.write((const char *)(&_isotopeHasPromptGamma), sizeof(_isotopeHasPromptGamma));
    fout.write((const char *)(&_spares), sizeof(_spares));

    //int pos = fout.tellg();
    fout.close();
    //fs::resize_file(tmpFile, pos);
  }

#ifdef HAVE_BOOST_FILESYSTEM
  try
  {
    fs::rename(tmpFile, dstFile);
  }
  catch (fs::filesystem_error &e)
  {
#ifdef HAVE_BOOST_LOG
    BOOST_LOG_TRIVIAL(error) << "Could not copy" << tmpFile << " to " << dstFile;
#else
    stir::error("Could not copy" + tostring(tmpFile) + " to " + tostring(dstFile));
#endif
    return false;
  }
#else
 if (!rename(tmpFile.c_str(), dstFile.c_str()))
   stir::error("Could not copy" + tostring(tmpFile) + " to " + tostring(dstFile));
#endif
  return true;
}

bool CRDF8EXAM::CleanField(std::string &target, const std::size_t constraint, std::string newVal)
{
  //Writes newVal into target while ensuring it conforms to the max field
  //length for header. E.g. IDB_LEN_PATIENT_ID.
  if (newVal.size() > constraint)
  {
#ifdef HAVE_BOOST_LOG
    BOOST_LOG_TRIVIAL(error) << "New requested string: '" << newVal << "' exceeds allowable length!";
#else
    stir::error("GE RDF8: New requested string: '" + newVal + "' exceeds allowable length!");
#endif
    return false;
  }

  int totalLength = pad4(constraint);
  newVal.resize(totalLength, '\0');
  target = newVal;

  return true;
}

bool CRDF8EXAM::SetPatientID(std::string newID)
{
  //Sets a new patient ID
  return CleanField(_patientID, IDB_LEN_PATIENT_ID, newID);
}

bool CRDF8EXAM::SetPatientDicomID(std::string newID)
{
  //Sets a new patient DICOM ID (note. not UID)
  return CleanField(_patientIdDicom, IDB_LEN_ID, newID);
}

bool CRDF8EXAM::SetPatientName(std::string newName)
{
  //Sets a new patient name
  return CleanField(_patientName, IDB_LEN_PATIENT_NAME, newName);
}

bool CRDF8EXAM::SetExamUID(std::string newExamUID)
{
#if 0
  //Sets a new DICOM exam UID
  if (!dcm::IsValidUID(newExamUID.c_str()))
  {
#ifdef HAVE_BOOST_LOG
    BOOST_LOG_TRIVIAL(error) << "Cannot set exam UID to " << newExamUID << " : " << newExamUID.size();
#else
    stir::error("GE RDF8: Cannot set exam UID to " + newExamUID);
#endif
    return false;
  }
#endif
  return CleanField(_examIdDicom, IDB_LEN_ID, newExamUID);
}

bool CRDF8EXAM::SetScanUID(std::string newScanUID)
{
  //Sets a new DICOM scan UID
#if 0
  if (!dcm:(newScanUID.c_str()))
  {
#ifdef HAVE_BOOST_LOG
    BOOST_LOG_TRIVIAL(error) << "Cannot set scan UID to " << newScanUID << " : " << newScanUID.size();
#else
    stir::error("GE RDF8: Cannot set scan UID to " + newExamUID);
#endif
    return false;
  }
#endif
  return CleanField(_scanIdDicom, IDB_LEN_ID, newScanUID);
}

bool CRDF8EXAM::RemovePII()
{
  //Removes patient identifiable information from RDF8.

  CleanField(_patientID, IDB_LEN_PATIENT_ID, "ANON");
  CleanField(_patientIdDicom, IDB_LEN_ID, "ANON");
  CleanField(_patientName, IDB_LEN_PATIENT_NAME, "ANON");

  CleanField(_diagnostician, IDB_LEN_DIAGNOSTICIAN, "");
  CleanField(_operator, IDB_LEN_OPERATOR, "");
  CleanField(_patientBirthdate, IDB_LEN_DATETIME_STR, "");
  _patientSex = 0;
  CleanField(_refPhysician, IDB_LEN_REF_PHYSICIAN, "");
  CleanField(_hospitalName, IDB_LEN_HOSPITAL_NAME, "");

  /*
             std::string uid = dc::GetUUID();

             if (uid == "") {
                 BOOST_LOG_TRIVIAL(error) << "Generated invalid new UID";
                 return false;
             }

             BOOST_LOG_TRIVIAL(info) << "Generated new UID = " << uid;*/

  //TODO: Do this return properly! 11/01/17
  return true;
}

std::ostream &operator<<(std::ostream &os, const CRDF8EXAM &rdf)
{
  //Print out RDF8 exam info for debugging purposes.
  os << std::endl;
  os << "\t"
     << "Patient ID = " << rdf._patientID << " :" << rdf._patientID.size() << std::endl;
  os << "\t"
     << "Patient name = " << rdf._patientName << " :" << rdf._patientName.size() << std::endl;
  os << "\t"
     << "Patient DOB = " << rdf._patientBirthdate << " :" << rdf._patientBirthdate.size() << std::endl;
  os << "\t"
     << "Patient sex = " << rdf._patientSex << " :" << sizeof(rdf._patientSex) << std::endl
     << std::endl;

  os << "\t"
     << "Exam ID = " << rdf._examID << " :" << sizeof(rdf._examID) << std::endl;
  os << "\t"
     << "Requisition= " << rdf._requisition << " :" << rdf._requisition.size() << std::endl;
  os << "\t"
     << "Hospital name = " << rdf._hospitalName << " :" << rdf._hospitalName.size() << std::endl;
  os << "\t"
     << "Scanner desc. = " << rdf._scannerDesc << " :" << rdf._scannerDesc.size() << std::endl;
  os << "\t"
     << "Exam desc. = " << rdf._examDesc << " :" << rdf._examDesc.size() << std::endl;
  os << "\t"
     << "Ref. physician = " << rdf._refPhysician << " :" << rdf._refPhysician.size() << std::endl;
  os << "\t"
     << "Diagnostician = " << rdf._diagnostician << " :" << rdf._diagnostician.size() << std::endl;
  os << "\t"
     << "Operator = " << rdf._operator << " :" << rdf._operator.size() << std::endl
     << std::endl;

  os << "\t"
     << "Patient height = " << rdf._patientHt << " :" << sizeof(rdf._patientHt) << std::endl;
  os << "\t"
     << "Patient weight = " << rdf._patientWt << " :" << sizeof(rdf._patientWt) << std::endl;
  os << "\t"
     << "Patient history = " << rdf._patientHistory << " :" << rdf._patientHistory.size() << std::endl
     << std::endl;

  os << "\t"
     << "Modality = " << rdf._modality << " :" << rdf._modality.size() << std::endl;
  os << "\t"
     << "Manufacturer = " << rdf._manufacturer << " :" << rdf._manufacturer.size() << std::endl;
  os << "\t"
     << "Scan ID = " << rdf._scanID << " :" << sizeof(rdf._scanID) << std::endl;
  os << "\t"
     << "Scan desc. = " << rdf._scanDescription << " :" << rdf._scanDescription.size() << std::endl
     << std::endl;

  os << "\t"
     << "Landmark name = " << rdf._landmarkName << " :" << rdf._landmarkName.size() << std::endl;
  os << "\t"
     << "Landmark abbrev. = " << rdf._landmarkAbbrev << " :" << rdf._landmarkAbbrev.size() << std::endl
     << std::endl;

  os << "\t"
     << "Tracer name = " << rdf._tracerName << " :" << rdf._tracerName.size() << std::endl;
  os << "\t"
     << "Batch desc. = " << rdf._batchDescription << " :" << rdf._batchDescription.size() << std::endl;
  os << "\t"
     << "Tracer activity = " << rdf._tracerActivity << " :" << sizeof(rdf._tracerActivity) << std::endl;
  os << "\t"
     << "Measurement date+time = " << rdf._measDateTime << " :" << rdf._measDateTime.size() << std::endl;
  os << "\t"
     << "Admin. date+time = " << rdf._adminDateTime << " :" << rdf._adminDateTime.size() << std::endl;
  os << "\t"
     << "Radionuclide = " << rdf._radionuclideName << " :" << rdf._radionuclideName.size() << std::endl;
  os << "\t"
     << "Half-life = " << rdf._halfLife << " :" << sizeof(rdf._halfLife) << std::endl
     << std::endl;

  os << "\t"
     << "Source 1 activity = " << rdf._source1Activity << " :" << sizeof(rdf._source1Activity) << std::endl;
  os << "\t"
     << "Source 1 date+time = " << rdf._source1MeasDateTime << " :" << rdf._source1MeasDateTime.size() << std::endl;
  os << "\t"
     << "Source 1 radionuclide = " << rdf._source1Radionuclide << " :" << rdf._source1Radionuclide.size() << std::endl;
  os << "\t"
     << "Source 1 half-life = " << rdf._source1HalfLife << " :" << sizeof(rdf._source1HalfLife) << std::endl
     << std::endl;

  os << "\t"
     << "Source 2 activity = " << rdf._source2Activity << " :" << sizeof(rdf._source2Activity) << std::endl;
  os << "\t"
     << "Source 2 date+time = " << rdf._source2MeasDateTime << " :" << rdf._source2MeasDateTime.size() << std::endl;
  os << "\t"
     << "Source 2 radionuclide = " << rdf._source2Radionuclide << " :" << rdf._source2Radionuclide.size() << std::endl;
  os << "\t"
     << "Source 2 half-life = " << rdf._source2HalfLife << " :" << sizeof(rdf._source2HalfLife) << std::endl
     << std::endl;

  os << "\t"
     << "Normal cal. ID = " << rdf._normalCalID << " :" << rdf._normalCalID.size() << std::endl;
  os << "\t"
     << "Blank cal. ID = " << rdf._blankCalID << " :" << rdf._blankCalID.size() << std::endl;
  os << "\t"
     << "wc cal. ID = " << rdf._wcCalID << " :" << rdf._wcCalID.size() << std::endl
     << std::endl;

  os << "\t"
     << "Pre-injection volume = " << rdf._preInjectionVolume << " :" << sizeof(rdf._preInjectionVolume) << std::endl;
  os << "\t"
     << "Post-injection activity = " << rdf._postInjectionActivity << " :" << sizeof(rdf._postInjectionActivity) << std::endl;
  os << "\t"
     << "Post-injection date+time = " << rdf._postInjectionDateTime << " :" << rdf._postInjectionDateTime.size() << std::endl;
  os << "\t"
     << "Positron fraction = " << rdf._positronFraction << " :" << sizeof(rdf._positronFraction) << std::endl
     << std::endl;

  os << "\t"
     << "DICOM scan ID = " << rdf._scanIdDicom << " :" << rdf._scanIdDicom.size() << std::endl;
  os << "\t"
     << "DICOM exam ID = " << rdf._examIdDicom << " :" << rdf._examIdDicom.size() << std::endl;
  os << "\t"
     << "DICOM normal 2D cal ID = " << rdf._normal2dCalID << " :" << rdf._normal2dCalID.size() << std::endl
     << std::endl;

  os << "\t"
     << "DICOM patient ID = " << rdf._patientIdDicom << " :" << rdf._patientIdDicom.size() << std::endl;
  os << "\t"
     << "Patient type = " << rdf._patientType << " :" << sizeof(rdf._patientType) << std::endl
     << std::endl;

  os << "\t"
     << "Software version = " << rdf._softwareVersion << " :" << rdf._softwareVersion.size() << std::endl;
  os << "\t"
     << "Isotope has prompt gamma = " << rdf._isotopeHasPromptGamma << " :" << sizeof(rdf._isotopeHasPromptGamma) << std::endl;

  std::stringstream ss;
  ss << "[";

  for (int x = 0; x < 9; x++)
  {
    if (x < 8)
      ss << rdf._spares[x] << " : ";
    else
      ss << rdf._spares[x];
  }

  os << "\t"
     << "Spares = " << ss.str() << "]" << std::endl;

  return os;
}

bool CRDF8CONFIG::Read(const path_t inFilePath)
{
  //Tries to read the config part of an RDF8 file.
  std::ifstream fin;
  if (!ReadOffsets(fin, inFilePath))
    return false;

  //Go to exam block of header.
  fin.seekg(_offsets.RDFConfigStructOffset);

  //Major version
  fin.read(reinterpret_cast<char *>(&_majorVersion), sizeof(uint32_t));
  //Minor version
  fin.read(reinterpret_cast<char *>(&_minorVersion), sizeof(uint32_t));
  //RDF complete
  fin.read(reinterpret_cast<char *>(&_RDFComplete), sizeof(uint32_t));
  //Dead-time version
  fin.read(reinterpret_cast<char *>(&_deadTimeVersion), sizeof(uint32_t));
  //Singles version
  fin.read(reinterpret_cast<char *>(&_singlesVersion), sizeof(uint32_t));
  //Is it at list?
  fin.read(reinterpret_cast<char *>(&_isListFile), sizeof(uint32_t));
  //File size
  fin.read(reinterpret_cast<char *>(&_fileSizeInBytes), sizeof(uint64_t));
  //Spares
  fin.read(reinterpret_cast<char *>(&_spares), sizeof(uint32_t) * 2);

  fin.close();

  return this->populateDictionary();
}

bool CRDF8CONFIG::populateDictionary(){

  this->_dict = std::unique_ptr<Dictionary>(new Dictionary);
  //C++17 std::make_unique

  if (this->_dict == nullptr) {
#ifdef HAVE_BOOST_LOG
    int status = 0;
    char* demangled = abi::__cxa_demangle(typeid(*this).name(), 0, 0, &status);
    BOOST_LOG_TRIVIAL(error) << demangled << "::populateDictionary - Cannot allocate RDF8 dictionary!";
    free(demangled);
#else
    stir::error("GE RDF8: cannot allocation dictionary");
#endif
    return false;
  }

  _dict->insert(DictionaryItem("VERSION_NUMBER", this->GetVersionNumber()));
  _dict->insert(DictionaryItem("IS_COMPLETE_RDF", _RDFComplete));
  _dict->insert(DictionaryItem("DEADTIME_VERSION", _deadTimeVersion));
  _dict->insert(DictionaryItem("SINGLES_VERSION", _singlesVersion));
  _dict->insert(DictionaryItem("IS_LISTMODE", _isListFile));
  _dict->insert(DictionaryItem("FILE_SIZE", _fileSizeInBytes));
  
  return true;
}


float CRDF8CONFIG::GetVersionNumber()
{
  //Returns version of RDF file.
  std::stringstream ss;
  ss << _majorVersion << "." << _minorVersion;

  float verAsFloat;

  try
  {
    verAsFloat = stof(ss.str());
  }
  catch (...)
  {
#ifdef HAVE_BOOST_LOG
    BOOST_LOG_TRIVIAL(error) << "Could not read version number!";
#else
    stir::warning("GE RDF8: Could not read version number!");
#endif
    return -1;
  }

  return verAsFloat;
}

std::ostream &operator<<(std::ostream &os, const CRDF8CONFIG &rdf)
{
  //Print out info for debugging purposes.

  os << std::endl;
  os << "\t"
     << "Major version = " << rdf._majorVersion << std::endl;
  os << "\t"
     << "Minor version = " << rdf._minorVersion << std::endl;
  os << "\t"
     << "RDF complete = " << rdf._RDFComplete << std::endl;
  os << "\t"
     << "Dead-time version = " << rdf._deadTimeVersion << std::endl;
  os << "\t"
     << "Singles version = " << rdf._singlesVersion << std::endl;
  os << "\t"
     << "Is List = " << rdf._isListFile << std::endl;
  os << "\t"
     << "File size (bytes) = " << rdf._fileSizeInBytes << std::endl;
  os << "\t"
     << "Spares = [" << rdf._spares[0] << " : " << rdf._spares[0] << "]" << std::endl;

  return os;
}

  bool CRDF8ACQPARAMS::Read(const path_t inFilePath)
  {
    std::ifstream fin;
    if (!ReadOffsets(fin, inFilePath))
      return false;
    fin.seekg(_offsets.acqParamStructOffset);

    // sharcRDFAcqLandmarkParams Subheader
    readField(fin, acq_landmark_params._landmarkQualifier);
    readField(fin, acq_landmark_params._patientEntry);
    readField(fin, acq_landmark_params._patientPosition);
    readField(fin, acq_landmark_params._absTableLongitude);
    readField(fin, acq_landmark_params._gantryTilt);
    readField(fin, acq_landmark_params._tableElevation);
    readField(fin, acq_landmark_params._landmarkDateTime);
    readField(fin, acq_landmark_params._spares);

    // sharcRDFAcqScanParams Subheader
    readField(fin, acq_scan_params._scanPerspective);
    readField(fin, acq_scan_params._scanType);
    readField(fin, acq_scan_params._scanMode);
    readField(fin, acq_scan_params._eventSource);
    acq_scan_params._eventSimulation = readString(fin, pad4(RDF_MAX_SYS_PATH_SIZE)); // Changed to padded in RDFv8
    readField(fin, acq_scan_params._startCondition);
    readField(fin, acq_scan_params._stopCondition);
    readField(fin, acq_scan_params._stopCondData);
    readField(fin, acq_scan_params._delayedEvents);
    readField(fin, acq_scan_params._delayedSubtractBias);
    readField(fin, acq_scan_params._thetaCompression);
    readField(fin, acq_scan_params._gantryTilt);
    readField(fin, acq_scan_params._collimation);
    readField(fin, acq_scan_params._tableLocation);
    readField(fin, acq_scan_params._acqDelay);
    readField(fin, acq_scan_params._acqTime);
    readField(fin, acq_scan_params._startAngle);
    readField(fin, acq_scan_params._deltaAngle);
    readField(fin, acq_scan_params._angleThickness);
    readField(fin, acq_scan_params._startSlice);
    readField(fin, acq_scan_params._deltaSlice);
    readField(fin, acq_scan_params._slicesCompressed);
    readField(fin, acq_scan_params._singleCollect);
    readField(fin, acq_scan_params._deadtimeCollect);
    readField(fin, acq_scan_params._TransPlusEmiss);
    readField(fin, acq_scan_params._axialCompression);
    readField(fin, acq_scan_params._startCondData);
    readField(fin, acq_scan_params._ct_kv);
    acq_scan_params._ct_contrast = readString(fin, 64);
    acq_scan_params._frame_of_reference = readString(fin, 64);
    readField(fin, acq_scan_params._axialAcceptance);
    readField(fin, acq_scan_params._retroScan);
    readField(fin, acq_scan_params._tofCompressionFactor); // New for RDFv8
    readField(fin, acq_scan_params._extraRsForTFOV); // New for RDFv8
    readField(fin, acq_scan_params._spares); // Changed to 1 in RDFv8

    // sharcRDFEdcatParams Subheader
    readField(fin, acq_edcat_params._posAxialAcceptanceAngle);
    readField(fin, acq_edcat_params._negAxialAcceptanceAngle);
    readField(fin, acq_edcat_params._posCoincidenceWindow);
    readField(fin, acq_edcat_params._negCoincidenceWindow);
    readField(fin, acq_edcat_params._delayWindowOffset);
    readField(fin, acq_edcat_params._transAxialFOV);
    readField(fin, acq_edcat_params._coinOutputMode);
    readField(fin, acq_edcat_params._upper_energy_limit);
    readField(fin, acq_edcat_params._lower_energy_limit);
    readField(fin, acq_edcat_params._majorClockPeriodFEE);
    readField(fin, acq_edcat_params._coincTimingPrecision);
    readField(fin, acq_edcat_params._crystalsInTFOV);
    readField(fin, acq_edcat_params._spares);

    // sharcRDFAcqRxGatedParams Subheader
    readField(fin, acq_rx_gated_params._binningMode);
    readField(fin, acq_rx_gated_params._numberOfBins);
    readField(fin, acq_rx_gated_params._binDurations);
    readField(fin, acq_rx_gated_params._trigRejMethod);
    readField(fin, acq_rx_gated_params._nTrigRejections);
    readField(fin, acq_rx_gated_params._upperRejLimit);
    readField(fin, acq_rx_gated_params._lowerRejLimit);
    readField(fin, acq_rx_gated_params._physioGatingType);
    readField(fin, acq_rx_gated_params._spares);

    // sharcRDFAcqTransControl Subheader
    readField(fin, trans_control._tsHolder1);
    readField(fin, trans_control._tsHolder2);
    readField(fin, trans_control._tsSpeed);
    readField(fin, trans_control._tsLocation);
    readField(fin, trans_control._teoMaskWidth);
    readField(fin, trans_control._teoMaskScaleFactor); // Changed to float in RDFv8
    readField(fin, trans_control._teoMaskRadialSum);
    readField(fin, trans_control._spares);

    // RDFImageNumbering Subheader
    readField(fin, image_numbering_data._locationOfImageOne);
    readField(fin, image_numbering_data._locationOfImageOneIndx);
    readField(fin, image_numbering_data._prospectiveNumbOfImageSlices);
    readField(fin, image_numbering_data._spares);

    // Back end filters new in RDFv8
    readField(fin, back_end_acq_filters._maxRingDiff);
    readField(fin, back_end_acq_filters._maxCoincDiffLSBs);
    readField(fin, back_end_acq_filters._transaxialFovInMM);
    readField(fin, back_end_acq_filters._maxEnergyKeV);
    readField(fin, back_end_acq_filters._minEnergyKeV);
    readField(fin, back_end_acq_filters._spares);

    //spares = fread(fid, 2,'uint32');

    return fin.good();
  }

  bool CRDF8ACQSTATS::Read(const path_t inFilePath)
  {
    std::ifstream fin;
    if (!ReadOffsets(fin, inFilePath))
      return false;

    //Go to block of header.
    fin.seekg(_offsets.acqStatsStructOffset);
    readField(fin, _terminationCondition);
    readField(fin, _totalPrompts);
    readField(fin, _totalDelays);
    readField(fin, _acceptedTriggers);
    readField(fin, _rejectedTriggers);
    readField(fin, _scanStartTime);
    readField(fin, _frameStartTime);
    readField(fin, _frameDuration);
    _frameID = readString(fin, IDB_LEN_ID);
    readField(fin, _binNumber);
    readField(fin, _accumBinDuration);
    readField(fin, _totalPromptsMs);
    readField(fin, _totalDelaysMs);
    readField(fin, _sorterFilteredEvtsLS);
    readField(fin, _sorterFilteredEvtsMS);
    readField(fin, _badCoincStreamEvts);
    readField(fin, _frameNumber);
    readField(fin, _isRejectBin);
    readField(fin, _frameStartCoincTStamp);
    readField(fin, _readyToScanUTC);
    readField(fin, _spares);

    return fin.good();
  };

  bool CRDF8SYSTEMGEO::Read(const path_t inFilePath)
  {
    std::ifstream fin;
    if (!ReadOffsets(fin, inFilePath))
      return false;

    //Go to block of header.
    fin.seekg(_offsets.sysGeometryStructOffset);
    fin.read(reinterpret_cast<char *>(&_radialModulesPerSystem), sizeof(std::uint32_t));
    fin.read(reinterpret_cast<char *>(&_radialBlocksPerModule), sizeof(std::uint32_t));
    fin.read(reinterpret_cast<char *>(&_radialCrystalsPerBlock), sizeof(std::uint32_t));
    fin.read(reinterpret_cast<char *>(&_axialModulesPerSystem), 1*sizeof(std::uint32_t));
    fin.read(reinterpret_cast<char *>(&_axialBlocksPerModule), 1*sizeof(std::uint32_t));
    fin.read(reinterpret_cast<char *>(&_axialCrystalsPerBlock), 1*sizeof(std::uint32_t));
    fin.read(reinterpret_cast<char *>(&_detectorRadialSize), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_detectorAxialSize), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_axialCrystalGap), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_radialCrystalGap), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_axialBlockGap), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_radialBlockGap), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_axialCassetteGap), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_radialCassetteGap), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_sourceRadius), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_collimatorInnerRadius), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_collimatorOuterRadius), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_delaysCorrectionFactor), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_effectiveRingDiameter), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_blockRepeatFactor), 1*sizeof(std::int32_t));
    fin.read(reinterpret_cast<char *>(&_interCrystalPitch), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_interBlockPitch), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_scatterHrParameters), 10*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_scatterHsParameters), 10*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_dt_intCorrectionConstant), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_dt_muxCorrectionConstant), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_dt_timingCorrectionConstant), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_numCoincAsics), 1*sizeof(std::int32_t));
    fin.read(reinterpret_cast<char *>(&_dt_asicChipFactors), 7*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_dt_3dasicChipFactors), 7*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_dt_3dintCorrectionConstant), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_dt_3dmuxCorrectionConstant), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_dt_3dtimingCorrectionConstant), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_transaxial_crystal_0_offset), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_vqc_XaxisTranslation), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_vqc_YaxisTranslation), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_vqc_ZaxisTranslation), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_vqc_XaxisTilt), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_vqc_YaxisSwivel), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_vqc_ZaxisRoll), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_scanner_first_slice), 1*sizeof(std::uint32_t));
    fin.read(reinterpret_cast<char *>(&_collimatorType), 1*sizeof(std::uint32_t));
    fin.read(reinterpret_cast<char *>(&_timingResolutionInPico), 1*sizeof(std::uint32_t));
    fin.read(reinterpret_cast<char *>(&_avgBlockDeadtime), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_avgCrystalSingles), 1*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_spares), 5*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_dt_crossRingFactors), RDF_NUM_MAJOR_RINGS_MAX*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_dt_3dpileUp_factors), RDF_NUM_MINOR_RINGS_MAX*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_dt_hrPileUp_factors), RDF_NUM_AXIAL_SLICES_MAX*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_dt_hsPileUp_factors), RDF_NUM_AXIAL_SLICES_MAX*sizeof(float32_t));
    fin.read(reinterpret_cast<char *>(&_dt_3dCrystalPileupFactors), RDF_CRYSTALS_PER_BLOCK_MAX*sizeof(float32_t));
    //bulk_spares = fread(fid, 8,'uint32');
    return static_cast<bool>(fin);
  }

  bool CRDF8SYSTEMGEO::populateDictionary()
  {
    // TODO
    return false;
  }

  bool CRDF8SORTERDATA::Read(const path_t inFilePath)
  {
    std::ifstream fin;
    if (!ReadOffsets(fin, inFilePath))
      return false;

    fin.seekg(_offsets.sorterStructOffset);
    fin.read(reinterpret_cast<char *>(&_dataOrientation), 1*sizeof(std::uint32_t));
    fin.read(reinterpret_cast<char *>(&_dimension1Size), 1*sizeof(std::uint32_t));
    fin.read(reinterpret_cast<char *>(&_dimension2Size), 1*sizeof(std::uint32_t));
    fin.read(reinterpret_cast<char *>(&_histogramCellSize), 1*sizeof(std::uint32_t));
    fin.read(reinterpret_cast<char *>(&_sinoAlignCorr), 1*sizeof(std::uint32_t));
    fin.read(reinterpret_cast<char *>(&_DHMErrorFifoDepth), 1*sizeof(std::uint32_t));
    fin.read(reinterpret_cast<char *>(&_acquisitionNumber), 1*sizeof(std::uint32_t));
    fin.read(reinterpret_cast<char *>(&_numberOfAcquisitions), 1*sizeof(std::uint32_t));

    for (unsigned seg = 0; seg < numSegments; ++seg)
      {
        fin.read(reinterpret_cast<char *>(&_acqDataSegmentParams[seg]._segmentType), 1*sizeof(std::uint32_t));
        fin.read(reinterpret_cast<char *>(&_acqDataSegmentParams[seg]._dimension3Size), 1*sizeof(std::uint32_t));
        fin.read(reinterpret_cast<char *>(&_acqDataSegmentParams[seg]._numScaleFactors), 1*sizeof(std::uint32_t));
        fin.read(reinterpret_cast<char *>(&_acqDataSegmentParams[seg]._scaleFactorsOffset), 1*sizeof(std::uint32_t));
        fin.read(reinterpret_cast<char *>(&_acqDataSegmentParams[seg]._dataSegmentOffset), 1*sizeof(std::uint64_t));
        fin.read(reinterpret_cast<char *>(&_acqDataSegmentParams[seg]._compDataSegOffset), 1*sizeof(std::uint64_t));
        fin.read(reinterpret_cast<char *>(&_acqDataSegmentParams[seg]._compDataSegSize), 1*sizeof(std::uint64_t));
        fin.read(reinterpret_cast<char *>(&_acqDataSegmentParams[seg]._segFirstCvtEntryOffset), 1*sizeof(std::uint64_t));
        fin.read(reinterpret_cast<char *>(&_acqDataSegmentParams[seg]._segCvtEntries), 1*sizeof(std::uint32_t));
        fin.read(reinterpret_cast<char *>(&_acqDataSegmentParams[seg]._tofCollapsed), 1*sizeof(std::uint32_t));
        fin.read(reinterpret_cast<char *>(&_acqDataSegmentParams[seg]._spares), 6*sizeof(std::uint32_t));
      }
    return static_cast<bool>(fin);
  }

  bool CRDF8SORTERDATA::populateDictionary()
  {
    // TODO
    return false;
  }

bool CRDF8LIST::Read(const path_t inFilePath)
{
  //Tries to read the list part of an RDF8 file.
  std::ifstream fin;
  if (!ReadOffsets(fin, inFilePath))
    return false;

  //Go to block of header.
  fin.seekg(_offsets.listHeaderOffset);

  fin.read(reinterpret_cast<char *>(&_listType), sizeof(uint32_t));
  fin.read(reinterpret_cast<char *>(&_numAssocListFiles), sizeof(uint32_t));
  fin.read(reinterpret_cast<char *>(&_whichAssocLFile), sizeof(uint32_t));
  fin.read(reinterpret_cast<char *>(&_listAcqTime), sizeof(uint32_t));
  fin.read(reinterpret_cast<char *>(&_listStartOffset), sizeof(uint32_t));
  fin.read(reinterpret_cast<char *>(&_isListCompressed), sizeof(uint32_t));
  fin.read(reinterpret_cast<char *>(&_listCompressionAlg), sizeof(uint32_t));
  fin.read(reinterpret_cast<char *>(&_evalAsBadCompress), sizeof(uint32_t)); // New to RDFv8
  fin.read(reinterpret_cast<char *>(&_areEvtTimeStampsKnown), sizeof(uint32_t)); // New to RDFv8
  fin.read(reinterpret_cast<char *>(&_firstTmAbsTimeStamp), sizeof(uint32_t)); // New to RDFv8
  fin.read(reinterpret_cast<char *>(&_lastTmAbsTimeStamp), sizeof(uint32_t)); // New to RDFv8
  {
    std::uint32_t spares;
    fin.read(reinterpret_cast<char *>(&spares), sizeof(uint32_t));
  }
  fin.read(reinterpret_cast<char *>(&_sizeOfCompressedList), sizeof(uint64_t)); // New to RDFv8
  fin.read(reinterpret_cast<char *>(&_sizeOfList), sizeof(uint64_t));
  fin.read(reinterpret_cast<char *>(&_listCompAlgCoefs), RDF_NUM_LIST_COMPRESS_ALG_COEFS * sizeof(double)); // New to RDFv8

  fin.close();

  return this->populateDictionary();
}

bool CRDF8LIST::populateDictionary(){

  this->_dict = std::unique_ptr<Dictionary>(new Dictionary);
  //C++17 std::make_unique

  if (this->_dict == nullptr) {
#ifdef HAVE_BOOST_LOG
    int status = 0;
    char* demangled = abi::__cxa_demangle(typeid(*this).name(), 0, 0, &status);
    BOOST_LOG_TRIVIAL(error) << demangled << "::populateDictionary - Cannot allocate RDF8 dictionary!";
    free(demangled);
#else
    stir::error("GE RDF8: cannot allocation dictionary");
#endif
    return false;
  }

  _dict->insert(DictionaryItem("LIST_TYPE", _listType));
  _dict->insert(DictionaryItem("NUM_ASSOC_LIST_FILES", _numAssocListFiles));
  _dict->insert(DictionaryItem("WHICH_ASSOC_LIST_FILE", _whichAssocLFile));
  _dict->insert(DictionaryItem("LIST_ACQ_TIME", _listAcqTime));
  _dict->insert(DictionaryItem("LIST_START_OFFSET", _listStartOffset));
  _dict->insert(DictionaryItem("IS_LIST_COMPRESSED", _isListCompressed));
  _dict->insert(DictionaryItem("LIST_COMPRESSION_ALG", _listCompressionAlg));
  _dict->insert(DictionaryItem("EVAL_AS_BAD_COMPRESS", _evalAsBadCompress));
  _dict->insert(DictionaryItem("ARE_EVENT_TIME_STAMPS_KNOWN", _areEvtTimeStampsKnown));
  _dict->insert(DictionaryItem("FIRST_TIME_ABS_TIME_STAMP", _firstTmAbsTimeStamp));
  _dict->insert(DictionaryItem("LAST_TIME_ABS_TIME_STAMP", _lastTmAbsTimeStamp));
  _dict->insert(DictionaryItem("SIZE_OF_COMPRESSED_LIST", _sizeOfCompressedList));
  _dict->insert(DictionaryItem("SIZE_OF_LIST", _sizeOfList));
  // don't insert _listCompAlgCoefs, don't know what to do with them anyway
  
  return true;
}

std::string getGEDate(std::string date){
//Extracts date from RDF date/time field.

  if (date.length() != 17) {
    //Not in YYYYMMDDHHMMSS.ff
    return "NODATE";
  }

#ifdef HAVE_BOOST_DATETIME
  boost::gregorian::date dateOnly;
  using namespace boost::gregorian;

  try {
    dateOnly = from_undelimited_string(date.substr(0,8));
  }
  catch (std::out_of_range) {
    return "NODATE";
  }

  return to_iso_extended_string(dateOnly);
#else
  return date.substr(0,8);
#endif
}

std::string getGETime(std::string time){
//Extracts time from RDF date/time field.
//TODO: Actually check time validity.

  if (time.length() != 17) {
    //Not in YYYYMMDDHHMMSS.ff
    return "NOTIME";
  }

  std::string timeOnly = time.substr(8,6);
  return timeOnly;
}

} //namespace ge
} //namespace IO
} //namespace nmtools
