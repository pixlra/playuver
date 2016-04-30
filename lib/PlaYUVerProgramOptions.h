/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2014, ITU/ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file     ProgramOptions.h
 * \brief    Input args handler
 */

#ifndef __PROGRAMOPTIONS_H__
#define __PROGRAMOPTIONS_H__

#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "PlaYUVerDefs.h"


struct Options;

struct ParseFailure: public std::exception
{
  ParseFailure( String arg0, String val0 ) throw() :
          arg( arg0 ),
          val( val0 )
  {
  }

  ~ParseFailure() throw()
  {
  }

  String arg;
  String val;

  const char* what() const throw()
  {
    return "Option Parse Failure";
  }
};

/** OptionBase: Virtual base class for storing information relating to a
 * specific option This base class describes common elements.  Type specific
 * information should be stored in a derived class. */
struct OptionBase
{
  OptionBase( const String& name, const String& desc ) :
          arg_count( 0 ),
          opt_string( name ),
          opt_desc( desc )
  {
  }

  virtual ~OptionBase()
  {
  }

  /* parse argument arg, to obtain a value for the option */
  virtual void parse( const String& arg ) = 0;
  /* set the argument to the default value */
  //virtual void setDefault() = 0;
  int count()
  {
    return arg_count;
  }
  Bool isBinary()
  {
    return is_binary;
  }

  int arg_count;
  String opt_string;
  String opt_desc;
  Bool is_binary;
};

/** Type specific option storage */
struct FlagOpt: public OptionBase
{
  FlagOpt( const String& name, Bool default_val, const String& desc ) :
          OptionBase( name, desc ),
          opt_storage( false ),
          opt_default_val( default_val )
  {
    is_binary = true;
  }

  void parse( const String& arg )
  {
    opt_storage = true;
    arg_count++;
  }

  void setDefault()
  {
    opt_storage = opt_default_val;
  }
  Bool opt_storage;
  Bool opt_default_val;
};

/** Type specific option storage */
template<typename T>
struct StandardOpt: public OptionBase
{
  StandardOpt( const String& name, T& storage, const String& desc ) :
          OptionBase( name, desc ),
          opt_storage( storage )
  {
    is_binary = false;
  }

  void parse( const String& arg );

//  void setDefault()
//  {
//    opt_storage = opt_default_val;
//  }

  T& opt_storage;
};

/* Generic parsing */
template<typename T>
inline void StandardOpt<T>::parse( const String& arg )
{
  std::istringstream arg_ss( arg, std::istringstream::in );
  arg_ss.exceptions( std::ios::failbit );
  try
  {
    arg_ss >> opt_storage;
  }
  catch( ... )
  {
    throw ParseFailure( opt_string, arg );
  }
  arg_count++;
}

template<>
inline void StandardOpt< std::vector<UInt> >::parse( const String& arg )
{
  UInt aux_opt_storage;
  std::istringstream arg_ss( arg, std::istringstream::in );
  arg_ss.exceptions( std::ios::failbit );
  try
  {
    arg_ss >> aux_opt_storage;
    opt_storage.push_back( aux_opt_storage );
  }
  catch( ... )
  {
    throw ParseFailure( opt_string, arg );
  }
  arg_count++;
}

template<>
inline void StandardOpt< std::vector<Int> >::parse( const String& arg )
{
  Int aux_opt_storage;
  std::istringstream arg_ss( arg, std::istringstream::in );
  arg_ss.exceptions( std::ios::failbit );
  try
  {
    arg_ss >> aux_opt_storage;
    opt_storage.push_back( aux_opt_storage );
  }
  catch( ... )
  {
    throw ParseFailure( opt_string, arg );
  }
  arg_count++;
}

/* string parsing is specialized */
template<>
inline void StandardOpt<String>::parse( const String& arg )
{
  opt_storage = arg;
  arg_count++;
}

/* string vector parsing is specialized */
template<>
inline void StandardOpt<std::vector<String> >::parse( const String& arg )
{
  opt_storage.push_back( arg );
  arg_count++;
}

/** Option class for argument handling using a user provided function */
struct OptionFunc: public OptionBase
{
  typedef Void (Func)( Options&, const String& );

  OptionFunc( const String& name, Options& parent_, Func *func_, const String& desc ) :
          OptionBase( name, desc ),
          parent( parent_ ),
          func( func_ )
  {
    is_binary = false;
  }

  void parse( const String& arg )
  {
    func( parent, arg );
    arg_count++;
  }

  void setDefault()
  {
    return;
  }

private:
  Options& parent;
  void (*func)( Options&, const String& );
};

class OptionSpecific;
class Options
{
public:

  struct Option
  {
    Option() :
            opt( 0 )
    {
    }
    ;
    ~Option()
    {
      if( opt )
        delete opt;
    }
    std::list<String> opt_long;
    std::list<String> opt_short;
    OptionBase* opt;
  };
  typedef std::list<Option*> OptionsList;

  Options( const String& name = "" );
  ~Options();

  std::list<const char*> scanArgv( unsigned argc, const char* argv[] );
  Void scanArgs( std::vector<String> args_array);
  Void doHelp( std::ostream& out, unsigned columns = 80 );

  OptionSpecific addOptions();
  Void setDefaults();

  OptionBase* operator[]( const String& optName );
  OptionBase* getOption( const String& optName );

  OptionsList& getOptionList()
  {
    return opt_list;
  }

  Bool hasOpt( const String& optName );

  void addOption( OptionBase *opt );

private:

  typedef std::map<String, OptionsList> OptionMap;
  OptionMap opt_long_map;
  OptionMap opt_short_map;

  OptionsList opt_list;

  String m_cOptionGroupName;
  Bool m_bAllowUnkonw;

  Bool storePair( Bool allow_long, Bool allow_short, const String& name, const String& value );
  Bool storePair( const String& name, const String& value );
  UInt parseLONG( unsigned argc, const char* argv[] );
  UInt parseLONG( String arg );
  unsigned parseSHORT( unsigned argc, const char* argv[] );
};

/* Class with templated overloaded operator(), for use by Options::addOptions() */
class OptionSpecific
{
public:
  OptionSpecific( Options& parent_ ) :
          parent( parent_ )
  {
  }

  /**
   * Add option described by name to the parent Options list,
   *   with storage for the option's value
   *   with default_val as the default value
   *   with desc as an optional help description
   */
  template<typename T>
  OptionSpecific&
  operator()( const String& name, T& storage, const String& desc )
  {
    parent.addOption( new StandardOpt<T>( name, storage, desc ) );
    return *this;
  }

  /**
   * Add option described by name to the parent Options list,
   *   with storage for the option's value
   *   with default_val as the default value
   *   with desc as an optional help description
   */
  OptionSpecific&
  operator()( const String& name, const String& desc )
  {
    parent.addOption( new FlagOpt( name, false, desc ) );
    return *this;
  }

  /**
   * Add option described by name to the parent Options list,
   *   with storage for the option's value
   *   with default_val as the default value
   *   with desc as an optional help description
   */
  OptionSpecific&
  operator()( const String& name, Bool default_val, const String& desc )
  {
    parent.addOption( new FlagOpt( name, default_val, desc ) );
    return *this;
  }

  /**
   * Add option described by name to the parent Options list,
   *   with desc as an optional help description
   * instead of storing the value somewhere, a function of type
   * OptionFunc::Func is called.  It is upto this function to correctly
   * handle evaluating the option's value.
   */
  OptionSpecific&
  operator()( const String& name, OptionFunc::Func *func, const String& desc )
  {
    parent.addOption( new OptionFunc( name, parent, func, desc ) );
    return *this;
  }
private:
  Options& parent;
};

#endif // __PROGRAMOPTIONS_H__
