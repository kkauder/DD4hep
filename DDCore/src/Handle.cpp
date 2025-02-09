//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

#include "DD4hep/detail/Handle.inl"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Printout.h"
#include "Evaluator/Evaluator.h"

/// C/C++ include files
#include <iostream>
#include <iomanip>
#include <climits>
#include <cstring>
#include <cstdio>

#if !defined(WIN32) && !defined(__ICC)
#include "cxxabi.h"
#endif

namespace dd4hep {
  dd4hep::tools::Evaluator& evaluator();
}

namespace {
  const dd4hep::tools::Evaluator& eval(dd4hep::evaluator());
}

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

namespace   {

  /// Set true for backwards compatibility
  static bool s_allow_variable_redefine = true;

  ///
  void check_evaluation(const string& value, std::pair<int,double> res, stringstream& err)   {
    if ( res.first != tools::Evaluator::OK) {
      throw runtime_error("dd4hep: "+err.str()+" : value="+value+" [Evaluation error]");
    }
  }
}

namespace dd4hep  {

  /// Steer redefinition of variable re-definition during expression evaluation. returns old value
  bool set_allow_variable_redefine(bool value)    {
    bool tmp = s_allow_variable_redefine;
    s_allow_variable_redefine = value;
    return tmp;
  }

  std::pair<int, double> _toFloatingPoint(const string& value)   {
    stringstream err;
    auto result = eval.evaluate(value, err);
    check_evaluation(value, result, err);
    return result;
  }

  std::pair<int, double> _toInteger(const string& value)    {
    string s(value);
    size_t idx = s.find("(int)");
    if (idx != string::npos)
      s.erase(idx, 5);
    idx = s.find("(long)");
    if (idx != string::npos)
      s.erase(idx, 6);
    while (s[0] == ' ')
      s.erase(0, 1);
    return _toFloatingPoint(s);
  }

  short _toShort(const string& value) {
    return (short) _toInteger(value).second;
  }

  unsigned short _toUShort(const string& value) {
    return (unsigned short) _toInteger(value).second;
  }

  int _toInt(const string& value) {
    return (int) _toInteger(value).second;
  }

  unsigned int _toUInt(const string& value) {
    return (unsigned int) _toInteger(value).second;
  }

  long _toLong(const string& value) {
    return (long) _toInteger(value).second;
  }

  unsigned long _toULong(const string& value) {
    return (unsigned long) _toInteger(value).second;
  }

  bool _toBool(const string& value) {
    return value == "true" || value == "yes" || value == "True";
  }

  /// String conversions: string to float value
  float _toFloat(const string& value) {
    return (float) _toFloatingPoint(value).second;
  }

  /// String conversions: string to double value
  double _toDouble(const string& value) {
    return _toFloatingPoint(value).second;
  }

  /// Generic type conversion from string to primitive value  \ingroup DD4HEP_CORE
  template <typename T> T _toType(const string& value)    {
    notImplemented("Value "+value+" cannot be converted to type "+typeName(typeid(T)));
    return T();
  }

  /// Generic type conversion from string to primitive value
  template <> bool _toType<bool>(const string& value)  {
    return _toBool(value);
  }

  /// Generic type conversion from string to primitive value
  template <> short _toType<short>(const string& value)  {
    return _toShort(value);
  }

  /// Generic type conversion from string to primitive value
  template <> unsigned short _toType<unsigned short>(const string& value)  {
    return (unsigned short)_toShort(value);
  }

  /// Generic type conversion from string to primitive value
  template <> int _toType<int>(const string& value)  {
    return _toInt(value);
  }

  /// Generic type conversion from string to primitive value
  template <> unsigned int _toType<unsigned int>(const string& value)  {
    return (unsigned int)_toInt(value);
  }

  /// Generic type conversion from string to primitive value
  template <> long _toType<long>(const string& value)  {
    return _toLong(value);
  }

  /// Generic type conversion from string to primitive value
  template <> unsigned long _toType<unsigned long>(const string& value)  {
    return (unsigned long)_toLong(value);
  }

  /// Generic type conversion from string to primitive value
  template <> float _toType<float>(const string& value)  {
    return _toFloat(value);
  }

  /// Generic type conversion from string to primitive value
  template <> double _toType<double>(const string& value)  {
    return _toDouble(value);
  }

  /// Generic type conversion from string to primitive value
  template <> string _toType<string>(const string& value)  {
    return value;
  }

  template <> char _multiply<char>(const string& left, const string& right) {
    double val = _toDouble(left + "*" + right);
    if ( val >= double(SCHAR_MIN) && val <= double(SCHAR_MAX) )
      return (char) (int)val;
    except("_multiply<char>",
           "Multiplication %e = %s * %s out of bounds for conversion to char.",
           val, left.c_str(), right.c_str());
    return 0;
  }

  template <> unsigned char _multiply<unsigned char>(const string& left, const string& right) {
    double val = _toDouble(left + "*" + right);
    if ( val >= 0 && val <= double(UCHAR_MAX) )
      return (unsigned char) (int)val;
    except("_multiply<unsigned char>",
           "Multiplication %e = %s * %s out of bounds for conversion to unsigned char.",
           val, left.c_str(), right.c_str());
    return 0;
  }

  template <> short _multiply<short>(const string& left, const string& right) {
    double val = _toDouble(left + "*" + right);
    if ( val >= double(SHRT_MIN) && val <= double(SHRT_MAX) )
      return (short) val;
    except("_multiply<short>",
           "Multiplication %e = %s * %s out of bounds for conversion to short.",
           val, left.c_str(), right.c_str());
    return 0;
  }

  template <> unsigned short _multiply<unsigned short>(const string& left, const string& right) {
    double val = _toDouble(left + "*" + right);
    if ( val >= 0 && val <= double(USHRT_MAX) )
      return (unsigned short)val;
    except("_multiply<unsigned short>",
           "Multiplication %e = %s * %s out of bounds for conversion to unsigned short.",
           val, left.c_str(), right.c_str());
    return 0;
  }

  template <> int _multiply<int>(const string& left, const string& right) {
    return (int) _toDouble(left + "*" + right);
  }

  template <> unsigned int _multiply<unsigned int>(const string& left, const string& right) {
    return (unsigned int) _toDouble(left + "*" + right);
  }

  template <> long _multiply<long>(const string& left, const string& right) {
    return (long) _toDouble(left + "*" + right);
  }

  template <> unsigned long _multiply<unsigned long>(const string& left, const string& right) {
    return (unsigned long) _toDouble(left + "*" + right);
  }

  template <> float _multiply<float>(const string& left, const string& right) {
    return _toFloat(left + "*" + right);
  }

  template <> double _multiply<double>(const string& left, const string& right) {
    return _toDouble(left + "*" + right);
  }

  void _toDictionary(const string& name, const string& value) {
    _toDictionary(name, value, "number");
  }

  /// Enter name value pair to the dictionary.  \ingroup DD4HEP_CORE
  void _toDictionary(const string& name, const string& value, const string& typ)   {
    if ( typ == "string" )  {
      eval.setEnviron(name.c_str(),value.c_str());
      return;
    }
    else  {
      int status;
      stringstream err;
      string n = name, v = value;
      size_t idx = v.find("(int)");
      if (idx != string::npos)
        v.erase(idx, 5);
      idx = v.find("(float)");
      if (idx != string::npos)
        v.erase(idx, 7);
      while (v[0] == ' ')
        v.erase(0, 1);
      auto result = eval.evaluate(v, err);
      check_evaluation(v, result, err);
      err.str("");
      status = eval.setVariable(n, result.second, err);
      if ( status != tools::Evaluator::OK )   {
	stringstream err_msg;
	err_msg << "name=" << name << " value=" << value
		<< "  " << err.str() << " [setVariable error]";
	if ( status == tools::Evaluator::WARNING_EXISTING_VARIABLE )   {
	  if ( s_allow_variable_redefine )
	    printout(WARNING,"Evaluator","+++ Overwriting variable: "+err_msg.str());
	  else
	    except("Evaluator","+++ Overwriting variable: "+err_msg.str());
	}
      }
    }
  }

  /// Evaluate string constant using environment stored in the evaluator
  string _getEnviron(const string& env)   {
    size_t id1 = env.find("${");
    size_t id2 = env.rfind("}");
    if ( id1 == string::npos || id2 == string::npos )   {
      return "";
    }
    else  {
      stringstream err;
      string v   = env.substr(id1,id2-id1+1);
      auto   ret = eval.getEnviron(v, err);
      if ( ret.first != tools::Evaluator::OK) {
	cerr << env << ": " << err.str() << endl;
	throw runtime_error("dd4hep: Severe error during environment lookup of " + env +
			    " " + err.str());
      }
      v = env.substr(0,id1);
      v += ret.second;
      v += env.substr(id2+1);
      return v;
    }
  }

  /// String manipulations: Remove unconditionally all white spaces
  string remove_whitespace(const string& v)    {
    string value;
    value.reserve(v.length()+1);
    for(const char* p = v.c_str(); *p; ++p)   {
      if ( !::isspace(*p) ) value += *p;
    }
    return value;
  }

  template <typename T> static inline string __to_string(T value, const char* fmt) {
    char text[128];
    ::snprintf(text, sizeof(text), fmt, value);
    return text;
  }

  string _toString(bool value) {
    return value ? "true" : "false";
  }

  string _toString(short value, const char* fmt) {
    return __to_string((int)value, fmt);
  }

  string _toString(int value, const char* fmt) {
    return __to_string(value, fmt);
  }

  string _toString(float value, const char* fmt) {
    return __to_string(value, fmt);
  }

  string _toString(double value, const char* fmt) {
    return __to_string(value, fmt);
  }

  string _ptrToString(const void* value, const char* fmt) {
    return __to_string(value, fmt);
  }

  // ==================================================================================
  static long s_numVerifies = 0;

  long num_object_validations() {
    return s_numVerifies;
  }
  void increment_object_validations() {
    ++s_numVerifies;
  }
  void warning_deprecated_xml_factory(const char* name)   {
    const char* edge = "++++++++++++++++++++++++++++++++++++++++++";
    size_t len = ::strlen(name);
    cerr << edge << edge << edge << endl;
    cerr << "++  The usage of the factory: \"" << name << "\" is DEPRECATED due to naming conventions."
         << setw(53-len) << right << "++" << endl;
    cerr << "++  Please use \"DD4hep_" << name << "\" instead." << setw(93-len) << right << "++" << endl;
    cerr << edge << edge << edge << endl;
  }
}

#include "DDSegmentation/Segmentation.h"
typedef DDSegmentation::Segmentation _Segmentation;
namespace dd4hep {
  template <> void Handle<_Segmentation>::assign(_Segmentation* s, const string& n, const string&) {
    this->m_element = s;
    s->setName(n);
  }
  template <> const char* Handle<_Segmentation>::name() const {
    return this->m_element ? this->m_element->name().c_str() : "";
  }
  template class dd4hep::Handle<_Segmentation>;
}

#include "DD4hep/Detector.h"
#include "TMap.h"
#include "TColor.h"

template class dd4hep::Handle<NamedObject>;
DD4HEP_SAFE_CAST_IMPLEMENTATION(NamedObject,NamedObject)

DD4HEP_INSTANTIATE_HANDLE_UNNAMED(Detector);
DD4HEP_INSTANTIATE_HANDLE_CODE(RAW,TObject,NamedObject);
DD4HEP_INSTANTIATE_HANDLE_CODE(NONE,TNamed,TObject,NamedObject);

#include "TGeoMedium.h"
#include "TGeoMaterial.h"
#include "TGeoElement.h"
DD4HEP_INSTANTIATE_HANDLE(TGeoElement);
DD4HEP_INSTANTIATE_HANDLE(TGeoMaterial);
DD4HEP_INSTANTIATE_HANDLE(TGeoMedium);

#include "TGeoMatrix.h"
DD4HEP_INSTANTIATE_HANDLE(TGeoMatrix);
DD4HEP_INSTANTIATE_HANDLE(TGeoRotation);
DD4HEP_INSTANTIATE_HANDLE(TGeoTranslation);
DD4HEP_INSTANTIATE_HANDLE(TGeoIdentity);
DD4HEP_INSTANTIATE_HANDLE(TGeoCombiTrans);
DD4HEP_INSTANTIATE_HANDLE(TGeoGenTrans);

#include "TGeoNode.h"
DD4HEP_INSTANTIATE_HANDLE_CODE(RAW,TGeoAtt);
DD4HEP_INSTANTIATE_HANDLE_CODE(RAW,TAtt3D);
DD4HEP_INSTANTIATE_HANDLE_CODE(RAW,TAttLine);
DD4HEP_INSTANTIATE_HANDLE(TGeoNode,TGeoAtt);
DD4HEP_INSTANTIATE_HANDLE(TGeoNodeMatrix);
DD4HEP_INSTANTIATE_HANDLE(TGeoNodeOffset);

// Shapes (needed by "Shapes.cpp")
#include "TGeoBBox.h"
#include "TGeoPcon.h"
#include "TGeoPgon.h"
#include "TGeoTube.h"
#include "TGeoCone.h"
#include "TGeoArb8.h"
#include "TGeoTrd1.h"
#include "TGeoTrd2.h"
#include "TGeoParaboloid.h"
#include "TGeoSphere.h"
#include "TGeoTorus.h"
#include "TGeoBoolNode.h"
#include "TGeoVolume.h"
#include "TGeoScaledShape.h"
#include "TGeoCompositeShape.h"
#include "TGeoShapeAssembly.h"
#include "DD4hep/detail/ShapesInterna.h"
DD4HEP_INSTANTIATE_HANDLE(TGeoVolumeAssembly,TGeoVolume,TGeoAtt);
DD4HEP_INSTANTIATE_HANDLE(TGeoVolumeMulti,TGeoVolume,TGeoAtt);
DD4HEP_INSTANTIATE_HANDLE(TGeoVolume,TGeoAtt,TAttLine,TAtt3D);
DD4HEP_INSTANTIATE_HANDLE(TGeoShape);
DD4HEP_INSTANTIATE_HANDLE(TGeoBBox,TGeoShape);

DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoCone);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoArb8);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoConeSeg);
//DD4HEP_INSTANTIATE_SHAPE_HANDLE(MyConeSeg);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoParaboloid);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoPcon);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoPgon,TGeoPcon);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoXtru);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoScaledShape);

DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoTube);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoHype,TGeoTube);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoEltu,TGeoTube);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoTubeSeg,TGeoTube);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoCtub,TGeoTubeSeg,TGeoTube);
using dd4hep::TwistedTubeObject;
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TwistedTubeObject,TGeoTubeSeg);

DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoTrap,TGeoArb8);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoGtra,TGeoArb8);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoTrd1);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoTrd2);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoSphere);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoTorus);

#if ROOT_VERSION_CODE > ROOT_VERSION(6,21,0)
#include "TGeoTessellated.h"
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoTessellated);
#endif

DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoHalfSpace);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoShapeAssembly);
DD4HEP_INSTANTIATE_SHAPE_HANDLE(TGeoCompositeShape);

// Volume Placements (needed by "Volumes.cpp")
#include "TGeoPhysicalNode.h"
DD4HEP_INSTANTIATE_HANDLE(TGeoPhysicalNode);

#include "TGeoBoolNode.h"
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoUnion);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoIntersection);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoSubtraction);

// Replicated Volumes (needed by "Volumes.cpp")
#include "TGeoPatternFinder.h"
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternFinder);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternX);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternY);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternZ);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternParaX);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternParaY);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternParaZ);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternTrapZ);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternCylR);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternCylPhi);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternSphR);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternSphTheta);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternSphPhi);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(TGeoPatternHoneycomb);
