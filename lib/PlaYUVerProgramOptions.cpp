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
 * \file     ProgramOptions.cpp
 * \brief    Input args handler
 */

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <map>
#include <algorithm>
#include "PlaYUVerProgramOptions.h"

using namespace std;

namespace plaYUVer
{

Options::Options( const std::string& name )
{
  m_cOptionGroupName = name;
  m_bAllowUnkonw = true;
}

Options::~Options()
{
  for( Options::OptionsList::iterator it = opt_list.begin(); it != opt_list.end(); it++ )
  {
    delete *it;
  }
}

OptionBase* Options::operator[]( const std::string& optName )
{
  return getOption( optName );
}

OptionBase* Options::getOption( const std::string& optName )
{
  Options::OptionMap::iterator opt_it;
  opt_it = opt_short_map.find( optName );
  if( opt_it != opt_short_map.end() )
  {
    OptionsList opt_list = ( *opt_it ).second;
    for( Options::OptionsList::iterator it = opt_list.begin(); it != opt_list.end(); ++it )
    {
      return ( *it )->opt;
    }
  }
  opt_it = opt_long_map.find( optName );
  if( opt_it != opt_long_map.end() )
  {
    OptionsList opt_list = ( *opt_it ).second;
    for( Options::OptionsList::iterator it = opt_list.begin(); it != opt_list.end(); ++it )
    {
      return ( *it )->opt;
    }
  }
  return NULL;
}

void Options::addOption( OptionBase *opt )
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

/* Helper method to initiate adding options to Options */
OptionSpecific Options::addOptions()
{
  return OptionSpecific( *this );
}

/* for all options in opts, set their storage to their specified
 * default value */
Void Options::setDefaults()
{
//  for( Options::OptionsList::iterator it = opt_list.begin(); it != opt_list.end(); it++ )
//  {
//    ( *it )->opt->setDefault();
//  }
}

static void setOptions( Options::OptionsList& opt_list, const string& value )
{
  /* multiple options may be registered for the same name:
   *   allow each to parse value */
  for( Options::OptionsList::iterator it = opt_list.begin(); it != opt_list.end(); ++it )
  {
    ( *it )->opt->parse( value );
  }
}

Bool Options::storePair( Bool allow_long, Bool allow_short, const string& name, const string& value )
{
  bool found = false;
  Options::OptionMap::iterator opt_it;
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
      cerr << "Unknown option: `"
           << name
           << "' (value:`"
           << value
           << "')"
           << endl;
    }
    return false;
  }

  setOptions( ( *opt_it ).second, value );
  return true;
}

Bool Options::storePair( const string& name, const string& value )
{
  return storePair( true, true, name, value );
}

/**
 * returns number of extra arguments consumed
 */
UInt Options::parseLONG( unsigned argc, const char* argv[] )
{
  /* gnu style long options can take the forms:
   *  --option=arg
   *  --option arg
   */
  string arg( argv[0] );
  size_t arg_opt_start = arg.find_first_not_of( '-' );
  size_t arg_opt_sep = arg.find_first_of( '=' );
  string option = arg.substr( arg_opt_start, arg_opt_sep - arg_opt_start );

  unsigned extra_argc_consumed = 0;
  if( arg_opt_sep == string::npos )
  {
    /* no argument found => argument in argv[1] (maybe) */
    /* xxx, need to handle case where option isn't required */
#if 0
    /* commented out, to return to true GNU style processing
     * where longopts have to include an =, otherwise they are
     * booleans */
    if (argc == 1)
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
UInt Options::parseLONG( string arg )
{
  /* gnu style long options can take the forms:
   *  --option=arg
   *  --option arg
   */
  size_t arg_opt_start = arg.find_first_not_of( '-' );
  size_t arg_opt_sep = arg.find_first_of( '=' );
  string option = arg.substr( arg_opt_start, arg_opt_sep - arg_opt_start );

  unsigned extra_argc_consumed = 0;
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

unsigned Options::parseSHORT( unsigned argc, const char* argv[] )
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
    cerr << "Not processing option without argument `"
         << option
         << "'"
         << endl;
    return 0; /* run out of argv for argument */
  }
  storePair( false, true, option, string( argv[1] ) );

  return 1;
}

list<const char*> Options::scanArgv( unsigned argc, const char* argv[] )
{
  /* a list for anything that didn't get handled as an option */
  list<const char*> non_option_arguments;

  for( unsigned i = 1; i < argc; i++ )
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
      i += parsePOSIX(opts, argc - i, &argv[i]);
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

Void Options::scanArgs( std::vector<std::string> args_array)
{
  for( unsigned i = 0; i < args_array.size(); i++ )
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
static void doHelpOpt( ostream& out, const Options::Option& entry, unsigned pad_short = 0 )
{
  pad_short = min( pad_short, 8u );

  if( !entry.opt_short.empty() )
  {
    unsigned pad = max( ( int )pad_short - ( int )entry.opt_short.front().size(), 0 );
    out << "-"
        << entry.opt_short.front();
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
    out << "--"
        << entry.opt_long.front();
  }
}

/* format the help text */
Void Options::doHelp( ostream& out, unsigned columns )
{
  const unsigned pad_short = 3;
  /* first pass: work out the longest option name */
  unsigned max_width = 0;
  for( Options::OptionsList::iterator it = opt_list.begin(); it != opt_list.end(); it++ )
  {
    ostringstream line( ios_base::out );
    doHelpOpt( line, **it, pad_short );
    max_width = max( max_width, ( unsigned )line.tellp() );
  }

  unsigned opt_width = min( max_width + 2, 28u + pad_short ) + 2;
  unsigned desc_width = columns - opt_width;

  /* second pass: write out formatted option and help text.
   *  - align start of help text to start at opt_width
   *  - if the option text is longer than opt_width, place the help
   *    text at opt_width on the next line.
   */
  for( Options::OptionsList::iterator it = opt_list.begin(); it != opt_list.end(); it++ )
  {
    ostringstream line( ios_base::out );
    line << "  ";
    doHelpOpt( line, **it, pad_short );

    const string& opt_desc = ( *it )->opt->opt_desc;
    if( opt_desc.empty() )
    {
      /* no help text: output option, skip further processing */
      cout << line.str()
           << endl;
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
      /* find a suitable point to split text (avoid spliting in middle of word) */
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

    cout << line.str()
         << endl;
  }
}

}  // NAMESPACE

