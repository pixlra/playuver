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
 * \file     PlaYUVerOptions.h
 * \brief    Options handler
 */

#ifndef __PROGRAMOPTIONS_H__
#define __PROGRAMOPTIONS_H__

#include "PlaYUVerDefs.h"

#include <list>
#include <map>

/** OptionBase: Virtual base class for storing information relating to a
 * specific option This base class describes common elements.
 * Type specific information should be stored in a derived class.
 */
class OptionBase
{
public:
  OptionBase( const String& name, const String& desc )
      : arg_count( 0 ), opt_string( name ), opt_desc( desc ) {}
  virtual ~OptionBase() {}
  /* parse argument arg, to obtain a value for the option */
  virtual void parse( const String& arg ) = 0;
  Int count() { return arg_count; }
  Bool isBinary() { return is_binary; }
  int arg_count;
  String opt_string;
  String opt_desc;
  Bool is_binary;
};

class PlaYUVerOptions
{
public:
  struct Option
  {
    Option()
        : opt( 0 ) {}
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

  PlaYUVerOptions( const String& name = "" );
  ~PlaYUVerOptions();

  Bool parse( UInt argc, Char* argv[] );
  Void parse( std::vector<String> args_array );

  std::list<const Char*>& getUnhandledArgs() { return m_aUnhandledArgs; }
  Void doHelp( std::ostream& out, unsigned columns = 80 );

  OptionBase* operator[]( const String& optName );
  OptionBase* getOption( const String& optName );

  Bool hasOpt( const String& optName );

  OptionsList getOptionList() { return opt_list; }
  Void addDefaultOptions();
  PlaYUVerOptions& addOptions();
  void addOption( OptionBase* opt );

  /**
   * Add option described by name to the parent Options list,
   *   with storage for the option's value
   *   with default_val as the default value
   *   with desc as an optional help description
   */
  PlaYUVerOptions& operator()( const String& name, const String& desc );

  /**
   * Add option described by name to the parent Options list,
   *   with storage for the option's value
   *   with default_val as the default value
   *   with desc as an optional help description
   */
  template <typename T>
  PlaYUVerOptions& operator()( const String& name, T& storage, const String& desc );

  /**
   * Add option described by name to the parent Options list,
   *   with desc as an optional help description
   * instead of storing the value somewhere, a function of type
   * OptionFunc::Func is called.  It is upto this function to correctly
   * handle evaluating the option's value.
   */
  //   PlaYUVerOptions&
  //   operator()( const String& name, OptionFunc::Func *func, const String&
  //   desc );

  Bool checkListingOpts();
  //Void listModules();

private:
  typedef std::map<String, OptionsList> OptionMap;
  OptionMap opt_long_map;
  OptionMap opt_short_map;

  OptionsList opt_list;

  String m_cOptionGroupName;
  Bool m_bAllowUnkonw;
  std::list<const Char*> m_aUnhandledArgs;

  Bool storePair( Bool allow_long, Bool allow_short, const String& name, const String& value );
  Bool storePair( const String& name, const String& value );
  UInt parseLONG( UInt argc, Char* argv[] );
  UInt parseLONG( String arg );
  UInt parseSHORT( UInt argc, Char* argv[] );
  std::list<const char*> scanArgv( UInt argc, Char* argv[] );
};

#endif  // __PROGRAMOPTIONS_H__
