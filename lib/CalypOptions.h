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
 * \file     CalypOptions.h
 * \ingroup	 CalypLibGrp
 * \brief    Options handler
 */

#ifndef __CALYPOPTIONS_H__
#define __CALYPOPTIONS_H__

#include "CalypDefs.h"

#include <list>
#include <map>

/**
 * \class    OptionBase
 * \ingroup  CalypLibGrp
 * \brief    Virtual base class for storing information relating to a
 * specific option This base class describes common elements.
 * Type specific information should be stored in a derived class.
 */
class OptionBase
{
public:
  OptionBase( const ClpString& name, const ClpString& desc )
      : arg_count( 0 ), opt_string( name ), opt_desc( desc ) {}
  virtual ~OptionBase() {}
  /* parse argument arg, to obtain a value for the option */
  virtual void parse( const ClpString& arg ) = 0;
  int count() { return arg_count; }
  bool isBinary() { return is_binary; }
  int arg_count;
  ClpString opt_string;
  ClpString opt_desc;
  bool is_binary;
};

/**
 * \class    CalypOptions
 * \ingroup  CalypLibGrp
 * \brief    Main class to store options
 */
class CalypOptions
{
public:
  struct Option
  {
    Option();
    ~Option();
    std::list<ClpString> opt_long;
    std::list<ClpString> opt_short;
    OptionBase* opt;
  };
  typedef std::list<Option*> OptionsList;

  CalypOptions( const ClpString& name = "" );
  ~CalypOptions();

  bool parse( unsigned int argc, char* argv[] );
  void parse( std::vector<ClpString> args_array );

  std::list<const char*>& getUnhandledArgs() { return m_aUnhandledArgs; }
  void doHelp( std::ostream& out, unsigned columns = 80 );

  OptionBase* operator[]( const ClpString& optName );
  OptionBase* getOption( const ClpString& optName );

  bool hasOpt( const ClpString& optName );

  OptionsList getOptionList() { return opt_list; }
  void addDefaultOptions();
  CalypOptions& addOptions();
  void addOption( OptionBase* opt );

  /**
   * Add option described by name to the parent Options list,
   *   with storage for the option's value
   *   with default_val as the default value
   *   with desc as an optional help description
   */
  CalypOptions& operator()( const ClpString& name, const ClpString& desc );

  /**
   * Add option described by name to the parent Options list,
   *   with storage for the option's value
   *   with default_val as the default value
   *   with desc as an optional help description
   */
  template <typename T>
  CalypOptions& operator()( const ClpString& name, T& storage, const ClpString& desc );

  bool checkListingOpts();

private:
  typedef std::map<ClpString, OptionsList> OptionMap;
  OptionMap opt_long_map;
  OptionMap opt_short_map;

  OptionsList opt_list;

  ClpString m_cOptionGroupName;
  bool m_bAllowUnkonw;
  std::list<const char*> m_aUnhandledArgs;

  bool storePair( bool allow_long, bool allow_short, const ClpString& name, const ClpString& value );
  bool storePair( const ClpString& name, const ClpString& value );
  unsigned int parseLONG( unsigned int argc, char* argv[] );
  unsigned int parseLONG( ClpString arg );
  unsigned int parseSHORT( unsigned int argc, char* argv[] );
  std::list<const char*> scanArgv( unsigned int argc, char* argv[] );
};

#endif  // __CALYPOPTIONS_H__
