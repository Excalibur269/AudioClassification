#ifndef MARSYAS_COMMON_H
#define MARSYAS_COMMON_H

/**
 \file common.h
 \ingroup NotmarCore
 */

/************************************************************************/
/*      common includes			                                            */
/************************************************************************/
#if HAVE_CONFIG_H 
#include <marsyas/config.h> 
#endif 

#include <cstdio> 
#include <sys/types.h> 
#include <complex>
#include <limits>

//#include "MrsLog.h"

/************************************************************************/
/*          common typedefs                                             */
/************************************************************************/

//	typedef double double;
//	typedef long int;
//  typedef bool mrs_bool;
//	typedef std::complex<double> mrs_complex;
//typedef std::string mrs_string;

class realvec;
typedef realvec mrs_realvec; //done at realvec.h

/************************************************************************/
/*  common defines                                                      */
/************************************************************************/
#define MRS_DEFAULT_SLICE_NSAMPLES 512
#define MRS_DEFAULT_SLICE_NOBSERVATIONS 1
#define MRS_DEFAULT_SLICE_SRATE   22050.0

#define FFT_FORWARD 1
#define FFT_INVERSE 0

#define EMPTYSTRING "MARSYAS_EMPTY"

//#define SAMPLERATE 16000

/************************************************************************/
/*  Numerical defines                                                   */
/************************************************************************/
#define MAXREAL std::numeric_limits<double>::max()
#define MINREAL std::numeric_limits<double>::min()
#define MAXNATURAL std::numeric_limits<int>::max()
#define MINNATURAL std::numeric_limits<int>::min()

//used for PCM audio => should not be changed!
#define PCM_MAXSHRT 32767 
#define PCM_FMAXSHRT 32767.0f 

#define PI 3.14159265358979323846 //double precision instead of 3.14159265359f
#define TWOPI 6.28318530717958647692 //double precision instead of 6.28318530718f

/************************************************************************/
/* LOGGING MACROS                                                       */
/************************************************************************/
#define MRSMSG(x) {std::ostringstream oss; MrsLog::mrsMessage((std::ostringstream&)(oss << x));}

#define MRSERR(x) {std::ostringstream oss; MrsLog::mrsErr((std::ostringstream&)(oss << x));}

#if MARSYAS_LOG_DIAGNOSTICS
#define MRSDIAG(x) {std::ostringstream oss; MrsLog::mrsDiagnostic((std::ostringstream&)(oss << x));}
#else
#define MRSDIAG(x)
#endif

#if MARSYAS_LOG_WARNINGS
#define MRSWARN(x) {std::ostringstream oss; MrsLog::mrsWarning((std::ostringstream&)(oss << x));}
#else 
#define MRSWARN(x) 
#endif 

#ifdef MARSYAS_LOG_DEBUGS
#define MRSDEBUG(x) {std::ostringstream oss; MrsLog::mrsDebug((std::ostringstream&)(oss << x));}
#else
#define MRSDEBUG(x)
#endif 

#if MARSYAS_ASSERTS
#define MRSASSERT(f) \
        if (f)       \
             {}      \
        else         \
           MrsLog::mrsAssert(__FILE__, __LINE__)
#else 
#define MRSASSERT(x) 
#endif 

/************************************************************************/
/*  MATLAB engine macros                                                */
/************************************************************************/
#ifdef MARSYAS_MATLAB
#include "MATLABengine.h"
#define MATLAB_PUT(var, name) {MATLABengine::getMatlabEng()->putVariable(var, name);}
#define MATLAB_GET(name, var) MATLABengine::getMatlabEng()->getVariable(name, var)
#define MATLAB_EVAL(s) {std::ostringstream oss; MATLABengine::getMatlabEng()->evalString((std::ostringstream&)(oss << s));}
#else
#define MATLAB_PUT(var, name)
#define MATLAB_GET(name, var) -1
#define MATLAB_EVAL(s)
#endif

/************************************************************************/
/*  Mutex MACROS for Multi-threaded Marsyas                             */
/************************************************************************/
#ifdef MARSYAS_QT
#define MARSYAS_MT //[!]
#define READ_LOCKER(mutex) QReadLocker locker(&mutex)
#define WRITE_LOCKER(mutex) QWriteLocker locker(&mutex)
#define LOCK_FOR_READ(mutex) mutex.lockForRead()
#define TRY_LOCK_FOR_READ(mutex, timeout) mutex.tryLockForRead(timeout)
#define LOCK_FOR_WRITE(mutex) mutex.lockForWrite()
#define TRY_LOCK_FOR_WRITE(mutex, timeout) mutex.tryLockForWrite(timeout)
#define LOCK(mutex) mutex.lock()
#define TRY_LOCK(mutex, timeout) mutex.tryLock(timeout)
#define UNLOCK(mutex) mutex.unlock()
#else
#define READ_LOCKER(mutex)
#define WRITE_LOCKER(mutex)
#define LOCK_FOR_READ(mutex)
#define TRY_LOCK_FOR_READ(mutex, timeout) true
#define LOCK_FOR_WRITE(mutex)
#define TRY_LOCK_FOR_WRITE(mutex, timeout) true
#define LOCK(mutex)
#define TRY_LOCK(mutex, timeout) true
#define UNLOCK(mutex)
#endif

/************************************************************************/
/*		WIN32 specific                                                    */
/************************************************************************/
//only  relevant for WIN32 MSVC (and ignored by all other platforms)
//For more info about the reason for this #pragma consult:
//http://msdn2.microsoft.com/en-us/library/ttcz0bys.aspx
#ifdef MARSYAS_WIN32
#pragma warning(disable : 4996)
#endif

#endif /* !MARSYAS_COMMON_H */ 

