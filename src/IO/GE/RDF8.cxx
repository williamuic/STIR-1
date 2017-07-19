/*
   RDF8.cxx

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

   GE RDF8 file reader.

 */

#include "RDF8.h"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/gregorian/conversion.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::gregorian;

namespace nmtools
{
namespace IO
{
namespace ge
{
bool RDF8Base::ReadOffsets(const fs::path inFilePath)
{
  //Reads the offsets from RDF8 file.

  std::ifstream fin(inFilePath.string().c_str(), std::ios::in | std::ios::binary);

  uint32_t BOM;

  if (fin.is_open())
  {
    fin.read(reinterpret_cast<char *>(&BOM), sizeof(uint32_t));
    fin.read(reinterpret_cast<char *>(&this->_offsets), sizeof(this->_offsets));
    fin.close();
  }
  else
    return false;

  BOOST_LOG_TRIVIAL(debug) << "BOM = " << std::hex << BOM;

  if (BOM != 0x0000FEFF)
  {
    BOOST_LOG_TRIVIAL(error) << "BOM is not valid!";
    return false;
  }

  BOOST_LOG_TRIVIAL(debug) << "Exam offset = " << this->_offsets.petExamStructOffset << " bytes";

  return true;
}

bool CRDF8EXAM::Read(const fs::path inFilePath)
{
  //Tries to read the exam portion of RDF8 file.

  //TODO: Do file checking here. E.g. big or little endian.
  if (!ReadOffsets(inFilePath))
    return false;

  std::ifstream fin(inFilePath.string().c_str(), std::ios::in | std::ios::binary);

  if (!fin.is_open())
  {
    BOOST_LOG_TRIVIAL(error) << "Could not open input file! " << inFilePath;
    return false;
  }

  //Go to exam block of header.
  fin.seekg(_offsets.petExamStructOffset);

  //Patient ID
  std::vector<char> buf(pad4(IDB_LEN_PATIENT_ID));
  fin.read(&buf[0], buf.size());
  _patientID = std::string(buf.begin(), buf.end());

  //Patient name
  buf.resize(pad4(IDB_LEN_PATIENT_NAME));
  fin.read(&buf[0], buf.size());
  _patientName = std::string(buf.begin(), buf.end());

  //Patient DOB
  buf.resize(pad4(IDB_LEN_DATETIME_STR));
  fin.read(&buf[0], buf.size());
  _patientBirthdate = std::string(buf.begin(), buf.end());

  //Patient sex
  fin.read(reinterpret_cast<char *>(&_patientSex), sizeof(uint32_t));

  //Exam ID
  fin.read(reinterpret_cast<char *>(&_examID), sizeof(uint32_t) * IDB_CNT_ID_INTS);

  //Requisition
  buf.resize(pad4(IDB_LEN_REQUISITION));
  fin.read(&buf[0], buf.size());
  _requisition = std::string(buf.begin(), buf.end());

  //Hospital name
  buf.resize(pad4(IDB_LEN_HOSPITAL_NAME));
  fin.read(&buf[0], buf.size());
  _hospitalName = std::string(buf.begin(), buf.end());

  //Scanner description
  buf.resize(pad4(IDB_LEN_SCANNER_DESC));
  fin.read(&buf[0], buf.size());
  _scannerDesc = std::string(buf.begin(), buf.end());

  //Exam description
  buf.resize(pad4(IDB_LEN_EXAM_DESC));
  fin.read(&buf[0], buf.size());
  _examDesc = std::string(buf.begin(), buf.end());

  //Referring physician
  buf.resize(pad4(IDB_LEN_REF_PHYSICIAN));
  fin.read(&buf[0], buf.size());
  _refPhysician = std::string(buf.begin(), buf.end());

  //Diagnostician
  buf.resize(pad4(IDB_LEN_DIAGNOSTICIAN));
  fin.read(&buf[0], buf.size());
  _diagnostician = std::string(buf.begin(), buf.end());

  //Operator
  buf.resize(pad4(IDB_LEN_OPERATOR));
  fin.read(&buf[0], buf.size());
  _operator = std::string(buf.begin(), buf.end());

  //Patient height
  fin.read(reinterpret_cast<char *>(&_patientHt), sizeof(float32_t));

  //Patient height
  fin.read(reinterpret_cast<char *>(&_patientWt), sizeof(float32_t));

  //Patient history
  buf.resize(pad4(IDB_LEN_PATIENT_HISTORY));
  fin.read(&buf[0], buf.size());
  _patientHistory = std::string(buf.begin(), buf.end());

  //Patient history
  buf.resize(pad4(IDB_LEN_MODALITY));
  fin.read(&buf[0], buf.size());
  _modality = std::string(buf.begin(), buf.end());

  //Manufacturer
  buf.resize(pad4(IDB_LEN_MANUFACTURER));
  fin.read(&buf[0], buf.size());
  _manufacturer = std::string(buf.begin(), buf.end());

  //Scan ID
  fin.read(reinterpret_cast<char *>(&_scanID), sizeof(uint32_t) * IDB_CNT_ID_INTS);

  //Scan description
  buf.resize(pad4(IDB_LEN_SCAN_DESCRIPTION));
  fin.read(&buf[0], buf.size());
  _scanDescription = std::string(buf.begin(), buf.end());

  //Landmark name
  buf.resize(pad4(IDB_LEN_LANDMARK_NAME));
  fin.read(&buf[0], buf.size());
  _landmarkName = std::string(buf.begin(), buf.end());

  //Landmark abbreviation
  buf.resize(pad4(IDB_LEN_LANDMARK_ABBREV));
  fin.read(&buf[0], buf.size());
  _landmarkAbbrev = std::string(buf.begin(), buf.end());

  //Tracer name
  buf.resize(pad4(IDB_LEN_TRACER_NAME));
  fin.read(&buf[0], buf.size());
  _tracerName = std::string(buf.begin(), buf.end());

  //Batch description
  buf.resize(pad4(IDB_LEN_BATCH_DESCRIPTION));
  fin.read(&buf[0], buf.size());
  _batchDescription = std::string(buf.begin(), buf.end());

  //Tracer activity
  fin.read(reinterpret_cast<char *>(&_tracerActivity), sizeof(float32_t));

  //Measurement date/time
  buf.resize(pad4(IDB_LEN_DATETIME_STR));
  fin.read(&buf[0], buf.size());
  _measDateTime = std::string(buf.begin(), buf.end());

  //Admin. date/time
  buf.resize(pad4(IDB_LEN_DATETIME_STR));
  fin.read(&buf[0], buf.size());
  _adminDateTime = std::string(buf.begin(), buf.end());

  //Radionuclide name
  buf.resize(pad4(IDB_LEN_RADIONUCLIDE));
  fin.read(&buf[0], buf.size());
  _radionuclideName = std::string(buf.begin(), buf.end());

  //Half-life
  fin.read(reinterpret_cast<char *>(&_halfLife), sizeof(float32_t));

  //Source 1 activity
  fin.read(reinterpret_cast<char *>(&_source1Activity), sizeof(float32_t));

  //Source 1 measurement date/time
  buf.resize(pad4(IDB_LEN_DATETIME_STR));
  fin.read(&buf[0], buf.size());
  _source1MeasDateTime = std::string(buf.begin(), buf.end());

  //Source 1 radionuclide name
  buf.resize(pad4(IDB_LEN_RADIONUCLIDE));
  fin.read(&buf[0], buf.size());
  _source1Radionuclide = std::string(buf.begin(), buf.end());

  //Source 1 half-life
  fin.read(reinterpret_cast<char *>(&_source1HalfLife), sizeof(float32_t));

  //Source 2 activity
  fin.read(reinterpret_cast<char *>(&_source2Activity), sizeof(float32_t));

  //Source 2 measurement date/time
  buf.resize(pad4(IDB_LEN_DATETIME_STR));
  fin.read(&buf[0], buf.size());
  _source2MeasDateTime = std::string(buf.begin(), buf.end());

  //Source 2 radionuclide name
  buf.resize(pad4(IDB_LEN_RADIONUCLIDE));
  fin.read(&buf[0], buf.size());
  _source2Radionuclide = std::string(buf.begin(), buf.end());

  //Source 2 half-life
  fin.read(reinterpret_cast<char *>(&_source2HalfLife), sizeof(float32_t));

  //Normal cal. ID
  buf.resize(pad4(IDB_LEN_ID));
  fin.read(&buf[0], buf.size());
  _normalCalID = std::string(buf.begin(), buf.end());

  //Blank cal. ID
  buf.resize(pad4(IDB_LEN_ID));
  fin.read(&buf[0], buf.size());
  _blankCalID = std::string(buf.begin(), buf.end());

  //wc cal. ID
  buf.resize(pad4(IDB_LEN_ID));
  fin.read(&buf[0], buf.size());
  _wcCalID = std::string(buf.begin(), buf.end());

  //Pre-injection volume
  fin.read(reinterpret_cast<char *>(&_preInjectionVolume), sizeof(float32_t));

  //Post-injection activity
  fin.read(reinterpret_cast<char *>(&_postInjectionActivity), sizeof(float32_t));

  //Post-injection date/time
  buf.resize(pad4(IDB_LEN_DATETIME_STR));
  fin.read(&buf[0], buf.size());
  _postInjectionDateTime = std::string(buf.begin(), buf.end());

  //Positron fraction
  fin.read(reinterpret_cast<char *>(&_positronFraction), sizeof(float32_t));

  //Scan ID DICOM
  buf.resize(pad4(IDB_LEN_ID));
  fin.read(&buf[0], buf.size());
  _scanIdDicom = std::string(buf.begin(), buf.end());

  //Exam ID DICOM
  buf.resize(pad4(IDB_LEN_ID));
  fin.read(&buf[0], buf.size());
  _examIdDicom = std::string(buf.begin(), buf.end());

  //Normal 2d cal DICOM
  buf.resize(pad4(IDB_LEN_ID));
  fin.read(&buf[0], buf.size());
  _normal2dCalID = std::string(buf.begin(), buf.end());

  //Patient ID DICOM
  buf.resize(pad4(IDB_LEN_ID));
  fin.read(&buf[0], buf.size());
  _patientIdDicom = std::string(buf.begin(), buf.end());

  //Patient type
  fin.read(reinterpret_cast<char *>(&_patientType), sizeof(uint32_t));

  //Software version
  buf.resize(pad4(IDB_LEN_ID));
  fin.read(&buf[0], buf.size());
  _softwareVersion = std::string(buf.begin(), buf.end());

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
    BOOST_LOG_TRIVIAL(error) << "CRDF8EXAM::populateDictionary - Cannot allocate DICOM dictionary!";
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

bool CRDF8EXAM::GetField(const std::string sid, boost::any &data) const
{

  if (_dict == nullptr)
  {
    BOOST_LOG_TRIVIAL(error) << "RDF8 dictionary appears to be NULL!";
    return false;
  }

  try
  {
    data = _dict->at(sid);
  }
  catch (std::out_of_range &e)
  {
    BOOST_LOG_TRIVIAL(error) << e.what();
    BOOST_LOG_TRIVIAL(warning) << sid << " not found!";
  }

  return true;
}

bool CRDF8EXAM::WriteFile(const fs::path srcFile, const fs::path dstFile)
{
  //Opens srcFile as RDF8 and writes new file, dstFile, with
  //modified header.

  if (srcFile == dstFile)
  {
    BOOST_LOG_TRIVIAL(error) << "Refusing to overwrite input file!";
    return false;
  }

  if (fs::exists(dstFile))
  {
    BOOST_LOG_TRIVIAL(error) << "Refusing to overwrite existing output file!";
    return false;
  }

  fs::path tmpFile = boost::filesystem::unique_path();

  try
  {
    fs::copy(srcFile, tmpFile);
  }
  catch (fs::filesystem_error &e)
  {
    BOOST_LOG_TRIVIAL(error) << "Could not copy " << srcFile << " to " << tmpFile;
    return false;
  }

  std::fstream fout(tmpFile.string().c_str(), std::ios::in | std::ios::out | std::ios::binary);
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

  try
  {
    fs::rename(tmpFile, dstFile);
  }
  catch (fs::filesystem_error &e)
  {
    BOOST_LOG_TRIVIAL(error) << "Could not copy" << tmpFile << " to " << dstFile;
    return false;
  }

  return true;
}

bool CRDF8EXAM::CleanField(std::string &target, const int constraint, std::string newVal)
{
  //Writes newVal into target while ensuring it conforms to the max field
  //length for header. E.g. IDB_LEN_PATIENT_ID.
  if (newVal.size() > constraint)
  {
    BOOST_LOG_TRIVIAL(error) << "New requested string: '" << newVal << "' exceeds allowable length!";
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
  //Sets a new DICOM exam UID
  if (!dcm::IsValidUID(newExamUID.c_str()))
  {
    BOOST_LOG_TRIVIAL(error) << "Cannot set exam UID to " << newExamUID << " : " << newExamUID.size();
    return false;
  }

  return CleanField(_examIdDicom, IDB_LEN_ID, newExamUID);
}

bool CRDF8EXAM::SetScanUID(std::string newScanUID)
{
  //Sets a new DICOM scan UID
  if (!dcm::IsValidUID(newScanUID.c_str()))
  {
    BOOST_LOG_TRIVIAL(error) << "Cannot set scan UID to " << newScanUID << " : " << newScanUID.size();
    return false;
  }

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

bool CRDF8CONFIG::Read(const fs::path inFilePath)
{
  //Tries to read the config part of an RDF8 file.

  if (!ReadOffsets(inFilePath))
    return false;

  std::ifstream fin(inFilePath.string().c_str(), std::ios::in | std::ios::binary);

  if (!fin.is_open())
  {
    BOOST_LOG_TRIVIAL(error) << "Could not open input file! " << inFilePath;
    return false;
  }

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
    BOOST_LOG_TRIVIAL(error) << "CRDF8CONFIG::populateDictionary - Cannot allocate RDF8 config dictionary!";
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


bool CRDF8CONFIG::GetField(const std::string sid, boost::any &data) const
{

  if (_dict == nullptr)
  {
    BOOST_LOG_TRIVIAL(error) << "RDF8 dictionary appears to be NULL!";
    return false;
  }

  try
  {
    data = _dict->at(sid);
  }
  catch (std::out_of_range &e)
  {
    BOOST_LOG_TRIVIAL(error) << e.what();
    BOOST_LOG_TRIVIAL(warning) << sid << " not found!";
  }

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
    BOOST_LOG_TRIVIAL(error) << "Could not read version number!";
    return -1;
  }

  return verAsFloat;
}

std::ostream &operator<<(std::ostream &os, const CRDF8CONFIG &rdf)
{
  //Print out info for debugging purposes.

  os << "Date test:\tDate: " << getGEDate(dt) << "\t Time: " << getGETime(dt);
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

std::string getGEDate(std::string date){
//Extracts date from RDF date/time field.

  if (date.length() != 17) {
    //Not in YYYYMMDDHHMMSS.ff
    return "NODATE";
  }

  boost::gregorian::date dateOnly;

  try {
    dateOnly = from_undelimited_string(date.substr(0,8));
  }
  catch (std::out_of_range) {
    return "NODATE";
  }

  return to_iso_extended_string(dateOnly);
}

std::string getGETime(std::string time){
//Extracts time from RDF date/time field.

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
