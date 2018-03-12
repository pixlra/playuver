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
 * \file     CalypOptions.cpp
 * \brief    Options handler
 */

#include "CalypOptions.h"

#include "config.h"
#include "lib/CalypModuleIf.h"

#include <cstring>
#include <iostream>
#include <sstream>

using namespace std;

struct ParseFailure : public std::exception
{
  ClpString arg;
  ClpString val;
  ParseFailure( ClpString arg0, ClpString val0 ) throw()
      : arg( arg0 ), val( val0 ) {}
  ~ParseFailure() throw() {}
  const char* what() const throw() { return "Option Parse Failure"; }
};

/** Type specific option storage */
class boolOption : public OptionBase
{
public:
  boolOption( const ClpString& name, const ClpString& desc )
      : OptionBase( name, desc ) { is_binary = true; }
  void parse( const ClpString& arg ) { arg_count++; }
};

/** Type specific option storage */
template <typename T>
class StandardOption : public OptionBase
{
public:
  StandardOption( const ClpString& name, T& storage, const ClpString& desc )
      : OptionBase( name, desc ), opt_storage( storage )
  {
    is_binary = false;
  }

  void parse( const ClpString& arg );
  T& opt_storage;
  std::vector<T> opt_storage_array;
};

/* Generic parsing */
template <typename T>
inline void StandardOption<T>::parse( const ClpString& arg )
{
  if( !is_binary )
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
  }
  arg_count++;
}

template <>
inline void StandardOption<std::vector<unsigned int>>::parse( const ClpString& arg )
{
  unsigned int aux_opt_storage;
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

template <>
inline void StandardOption<std::vector<int>>::parse( const ClpString& arg )
{
  int aux_opt_storage;
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
template <>
inline void StandardOption<ClpString>::parse( const ClpString& arg )
{
  opt_storage = arg;
  arg_count++;
}

/* string vector parsing is specialized */
template <>
inline void StandardOption<std::vector<ClpString>>::parse( const ClpString& arg )
{
  opt_storage.push_back( arg );
  arg_count++;
}

/** Option class for argument handling using a user provided function */
struct FunctionOption : public OptionBase
{
public:
  typedef void( Func )( CalypOptions&, const ClpString& );

  FunctionOption( const ClpString& name, CalypOptions& parent_, Func* func_, const ClpString& desc )
      : OptionBase( name, desc ), parent( parent_ ), func( func_ )
  {
    is_binary = false;
  }

  void parse( const ClpString& arg )
  {
    func( parent, arg );
    arg_count++;
  }

private:
  CalypOptions& parent;
  void ( *func )( CalypOptions&, const ClpString& );
};

/* Helper method to initiate adding options to Options */

/**
 * Add option described by name to the parent Options list,
 *   with storage for the option's value
 *   with default_val as the default value
 *   with desc as an optional help description
 */
CalypOptions& CalypOptions::operator()( const ClpString& name, const ClpString& desc )
{
  addOption( new boolOption( name, desc ) );
  return *this;
}

/**
 * Add option described by name to the parent Options list,
 *   with storage for the option's value
 *   with desc as an optional help description
 */
template <typename T>
CalypOptions& CalypOptions::operator()( const ClpString& name, T& storage, const ClpString& desc )
{
  addOption( new StandardOption<T>( name, storage, desc ) );
  return *this;
}

template CalypOptions& CalypOptions::operator()( const ClpString& name, bool& storage, const ClpString& desc );
template CalypOptions& CalypOptions::operator()( const ClpString& name, unsigned int& storage, const ClpString& desc );
template CalypOptions& CalypOptions::operator()( const ClpString& name, int& storage, const ClpString& desc );
template CalypOptions& CalypOptions::operator()( const ClpString& name, long& storage, const ClpString& desc );
template CalypOptions& CalypOptions::operator()( const ClpString& name, long long int& storage, const ClpString& desc );
template CalypOptions& CalypOptions::operator()( const ClpString& name, ClpString& storage, const ClpString& desc );
template CalypOptions& CalypOptions::operator()( const ClpString& name, std::vector<unsigned int>& storage,
                                                 const ClpString& desc );
template CalypOptions& CalypOptions::operator()( const ClpString& name, std::vector<int>& storage,
                                                 const ClpString& desc );
template CalypOptions& CalypOptions::operator()( const ClpString& name, std::vector<ClpString>& storage,
                                                 const ClpString& desc );

CalypOptions::Option::Option()
    : opt( 0 ) {}

CalypOptions::Option::~Option()
{
  if( opt )
    delete opt;
}

CalypOptions::CalypOptions( const ClpString& name )
{
  m_cOptionGroupName = name;
  m_bAllowUnkonw = true;
}

CalypOptions::~CalypOptions()
{
  for( CalypOptions::OptionsList::iterator it = opt_list.begin(); it != opt_list.end(); it++ )
  {
    delete *it;
  }
}

OptionBase* CalypOptions::operator[]( const ClpString& optName )
{
  return getOption( optName );
}

OptionBase* CalypOptions::getOption( const ClpString& optName )
{
  CalypOptions::OptionMap::iterator opt_it;
  opt_it = opt_short_map.find( optName );
  if( opt_it != opt_short_map.end() )
  {
    OptionsList opt_list = ( *opt_it ).second;
    for( CalypOptions::OptionsList::iterator it = opt_list.begin(); it != opt_list.end(); ++it )
    {
      return ( *it )->opt;
    }
  }
  opt_it = opt_long_map.find( optName );
  if( opt_it != opt_long_map.end() )
  {
    OptionsList opt_list = ( *opt_it ).second;
    for( CalypOptions::OptionsList::iterator it = opt_list.begin(); it != opt_list.end(); ++it )
    {
      return ( *it )->opt;
    }
  }
  return NULL;
}

bool CalypOptions::hasOpt( const ClpString& optName )
{
  OptionBase* opt = getOption( optName );
  return opt ? opt->count() > 0 ? true : false : false;
}

void CalypOptions::addDefaultOptions()
{
  addOptions()                                                /**/
      ( "help,h", "produce help message" )                    /**/
      ( "version", "show version and exit" )                  /**/
      ( "pel_fmts", "list pixel formats" )                    /**/
      ( "quality_metrics", "list supported quality metrics" ) /**/
      ( "module_list", "list supported modules" )             /**/
      ( "module_list_full", "detailed list supported modules" );
}

CalypOptions& CalypOptions::addOptions()
{
  return *this;
}

void CalypOptions::addOption( OptionBase* opt )
{
  Option* names = new Option();
  names->opt = opt;
  string& opt_string = opt->opt_string;

  size_t opt_start = 0;
  for( size_t opt_end = 0; opt_end != string::npos; )
  {
    opt_end = opt_string.find_first_of( ',', opt_start );
    bool force_short = 0;
    if( opt_string[opt_start] == '-' )
    {
      opt_start++;
      force_short = 1;
    }
    string opt_name = opt_string.substr( opt_start, opt_end - opt_start );
    if( force_short || opt_name.size() == 1 )
    {
      names->opt_short.push_back( opt_name );
      opt_short_map[opt_name].push_back( names );
    }
    else
    {
      names->opt_long.push_back( opt_name );
      opt_long_map[opt_name].push_back( names );
    }
    opt_start += opt_end + 1;
  }
  opt_list.push_back( names );
}

static void setOptions( CalypOptions::OptionsList& opt_list, const string& value )
{
  /* multiple options may be registered for the same name:
   *   allow each to parse value */
  for( CalypOptions::OptionsList::iterator it = opt_list.begin(); it != opt_list.end(); ++it )
  {
    ( *it )->opt->parse( value );
  }
}

bool CalypOptions::storePair( bool allow_long, bool allow_short, const string& name, const string& value )
{
  bool found = false;
  CalypOptions::OptionMap::iterator opt_it;
  if( allow_long )
  {
    opt_it = opt_long_map.find( name );
    if( opt_it != opt_long_map.end() )
    {
      found = true;
    }
  }

  /* check for the short list */
  if( allow_short && !( found && allow_long ) )
  {
    opt_it = opt_short_map.find( name );
    if( opt_it != opt_short_map.end() )
    {
      found = true;
    }
  }

  if( !found )
  {
    if( !m_bAllowUnkonw )
    {
      /* not found */
      cerr << "Unknown option: `" << name << "' (value:`" << value << "')" << endl;
    }
    return false;
  }

  setOptions( ( *opt_it ).second, value );
  return true;
}

bool CalypOptions::storePair( const string& name, const string& value )
{
  return storePair( true, true, name, value );
}

/**
 * returns number of extra arguments consumed
 */
unsigned int CalypOptions::parseLONG( unsigned int argc, char* argv[] )
{
  /* gnu style long options can take the forms:
   *  --option=arg
   *  --option arg
   */
  string arg( argv[0] );
  size_t arg_opt_start = arg.find_first_not_of( '-' );
  size_t arg_opt_sep = arg.find_first_of( '=' );
  string option = arg.substr( arg_opt_start, arg_opt_sep - arg_opt_start );

  unsigned int extra_argc_consumed = 0;
  if( arg_opt_sep == string::npos )
  {
/* no argument found => argument in argv[1] (maybe) */
/* xxx, need to handle case where option isn't required */
#if 0
    /* commented out, to return to true GNU style processing
     * where longopts have to include an =, otherwise they are
     * booleans */
    if( argc == 1 )
    return 0; /* run out of argv for argument */
    extra_argc_consumed = 1;
#endif
    if( !storePair( true, false, option, "1" ) )
    {
      return 0;
    }
  }
  else
  {
    /* argument occurs after option_sep */
    string val = arg.substr( arg_opt_sep + 1 );
    storePair( true, false, option, val );
  }
  return extra_argc_consumed;
}

/**
 * returns number of extra arguments consumed
 */
unsigned int CalypOptions::parseLONG( string arg )
{
  /* gnu style long options can take the forms:
   *  --option=arg
   *  --option arg
   */
  size_t arg_opt_start = arg.find_first_not_of( '-' );
  size_t arg_opt_sep = arg.find_first_of( '=' );
  string option = arg.substr( arg_opt_start, arg_opt_sep - arg_opt_start );

  unsigned int extra_argc_consumed = 0;
  if( arg_opt_sep == string::npos )
  {
    /* no argument found => argument in argv[1] (maybe) */
    /* xxx, need to handle case where option isn't required */
    if( !storePair( true, false, option, "1" ) )
    {
      return 0;
    }
  }
  else
  {
    /* argument occurs after option_sep */
    string val = arg.substr( arg_opt_sep + 1 );
    storePair( true, false, option, val );
  }
  return extra_argc_consumed;
}

unsigned int CalypOptions::parseSHORT( unsigned int argc, char* argv[] )
{
  /* short options can take the forms:
   *  --option arg
   *  -option arg
   */
  string arg( argv[0] );
  size_t arg_opt_start = arg.find_first_not_of( '-' );
  string option = arg.substr( arg_opt_start );
  /* lookup option */

  /* argument in argv[1] */
  /* xxx, need to handle case where option isn't required */
  if( argc == 1 )
  {
    cerr << "Not processing option without argument `" << option << "'" << endl;
    return 0; /* run out of argv for argument */
  }
  storePair( false, true, option, string( argv[1] ) );

  return 1;
}

bool CalypOptions::parse( unsigned int argc, char* argv[] )
{
  try
  {
    m_aUnhandledArgs = scanArgv( argc, argv );

    if( checkListingOpts() )
    {
      return false;
    }
  }
  catch( std::exception& e )
  {
    std::cerr << "error: " << e.what() << "\n";
    return false;
  }
  catch( ... )
  {
    std::cerr << "Exception of unknown type!\n";
  }
  return true;
}

list<const char*> CalypOptions::scanArgv( unsigned int argc, char* argv[] )
{
  /* a list for anything that didn't get handled as an option */
  list<const char*> non_option_arguments;

  for( unsigned int i = 1; i < argc; i++ )
  {
    if( argv[i][0] != '-' )
    {
      non_option_arguments.push_back( argv[i] );
      continue;
    }

    if( argv[i][1] == 0 )
    {
      /* a lone single dash is an argument (usually signifying stdin) */
      non_option_arguments.push_back( argv[i] );
      continue;
    }

    if( argv[i][1] != '-' )
    {
/* handle short (single dash) options */
#if 0
      i += parsePOSIX( opts, argc - i, &argv[i] );
#else
      i += parseSHORT( argc - i, &argv[i] );
#endif
      continue;
    }

    if( argv[i][2] == 0 )
    {
      /* a lone double dash ends option processing */
      while( ++i < argc )
        non_option_arguments.push_back( argv[i] );
      break;
    }

    /* handle long (double dash) options */
    i += parseLONG( argc - i, &argv[i] );
  }

  return non_option_arguments;
}

void CalypOptions::parse( std::vector<ClpString> args_array )
{
  for( unsigned int i = 0; i < args_array.size(); i++ )
  {
    i += parseLONG( args_array[i] );
  }
  return;
}

static const char spaces[41] = "                                        ";

/* format help text for a single option:
 * using the formatting: "-x, --long",
 * if a short/long option isn't specified, it is not printed
 */
static void doHelpOpt( ostream& out, const CalypOptions::Option& entry, unsigned int pad_short = 0 )
{
  pad_short = min( pad_short, 8u );

  if( !entry.opt_short.empty() )
  {
    unsigned int pad = max( (int)pad_short - (int)entry.opt_short.front().size(), 0 );
    out << "-" << entry.opt_short.front();
    if( !entry.opt_long.empty() )
    {
      out << ", ";
    }
    out << &( spaces[40 - pad] );
  }
  else
  {
    out << "   ";
    out << &( spaces[40 - pad_short] );
  }

  if( !entry.opt_long.empty() )
  {
    out << "--" << entry.opt_long.front();
  }
}

/* format the help text */
void CalypOptions::doHelp( ostream& out, unsigned int columns )
{
  const unsigned int pad_short = 3;
  /* first pass: work out the longest option name */
  unsigned int max_width = 0;
  for( CalypOptions::OptionsList::iterator it = opt_list.begin(); it != opt_list.end(); it++ )
  {
    ostringstream line( ios_base::out );
    doHelpOpt( line, **it, pad_short );
    max_width = max( max_width, (unsigned int)line.tellp() );
  }

  unsigned int opt_width = min( max_width + 2, 28u + pad_short ) + 2;
  unsigned int desc_width = columns - opt_width;

  /* second pass: write out formatted option and help text.
   *  - align start of help text to start at opt_width
   *  - if the option text is longer than opt_width, place the help
   *    text at opt_width on the next line.
   */
  for( CalypOptions::OptionsList::iterator it = opt_list.begin(); it != opt_list.end(); it++ )
  {
    ostringstream line( ios_base::out );
    line << "  ";
    doHelpOpt( line, **it, pad_short );

    const string& opt_desc = ( *it )->opt->opt_desc;
    if( opt_desc.empty() )
    {
      /* no help text: output option, skip further processing */
      cout << line.str() << endl;
      continue;
    }
    size_t currlength = size_t( line.tellp() );
    if( currlength > opt_width )
    {
      /* if option text is too long (and would collide with the
       * help text, split onto next line */
      line << endl;
      currlength = 0;
    }
    /* split up the help text, taking into account new lines,
     *   (add opt_width of padding to each new line) */
    for( size_t newline_pos = 0, cur_pos = 0; cur_pos != string::npos; currlength = 0 )
    {
      /* print any required padding space for vertical alignment */
      line << &( spaces[40 - opt_width + currlength] );
      newline_pos = opt_desc.find_first_of( '\n', newline_pos );
      if( newline_pos != string::npos )
      {
        /* newline found, print substring (newline needn't be stripped) */
        newline_pos++;
        line << opt_desc.substr( cur_pos, newline_pos - cur_pos );
        cur_pos = newline_pos;
        continue;
      }
      if( cur_pos + desc_width > opt_desc.size() )
      {
        /* no need to wrap text, remainder is less than avaliable width */
        line << opt_desc.substr( cur_pos );
        break;
      }
      /* find a suitable point to split text (avoid spliting in middle of word)
       */
      size_t split_pos = opt_desc.find_last_of( ' ', cur_pos + desc_width );
      if( split_pos != string::npos )
      {
        /* eat up multiple space characters */
        split_pos = opt_desc.find_last_not_of( ' ', split_pos ) + 1;
      }

      /* bad split if no suitable space to split at.  fall back to width */
      bool bad_split = split_pos == string::npos || split_pos <= cur_pos;
      if( bad_split )
      {
        split_pos = cur_pos + desc_width;
      }
      line << opt_desc.substr( cur_pos, split_pos - cur_pos );

      /* eat up any space for the start of the next line */
      if( !bad_split )
      {
        split_pos = opt_desc.find_first_not_of( ' ', split_pos );
      }
      cur_pos = newline_pos = split_pos;

      if( cur_pos >= opt_desc.size() )
      {
        break;
      }
      line << endl;
    }

    cout << line.str() << endl;
  }
}

bool CalypOptions::checkListingOpts()
{
  bool bRet = false;

  if( hasOpt( "version" ) )
  {
    std::cout << "Calyp version " << CALYP_VERSION_STRING << "\n";
    bRet |= true;
  }
  if( hasOpt( "pel_fmts" ) )
  {
    printf( "Calyp supported pixel formats: \n" );
    for( unsigned int i = 0; i < CalypFrame::supportedPixelFormatListNames().size(); i++ )
    {
      printf( "   %s\n", CalypFrame::supportedPixelFormatListNames()[i].c_str() );
    }
    bRet |= true;
  }
  if( hasOpt( "quality_metrics" ) )
  {
    printf( "Calyp supported quality metrics: \n" );
    for( unsigned int i = 0; i < CalypFrame::supportedQualityMetricsList().size(); i++ )
    {
      printf( "   %s\n", CalypFrame::supportedQualityMetricsList()[i].c_str() );
    }
    bRet |= true;
  }
  return bRet;
}
