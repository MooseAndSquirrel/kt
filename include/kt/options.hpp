/*
  kt-options library
  Copyright (C) 2021 mooseandsquirrel@anotherwheel.com

  This program is free software; you can redistribute it and/or modify it under
  the terms of the GNU General Public License version 2 as published by the
  Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#ifndef kt_options_hpp_20210919_170150_PDT
#define kt_options_hpp_20210919_170150_PDT
#include <vector>
#include <string_view>
#include <kt/string/concat.hpp>
#include <kt/terminal.hpp>
#include <kt/string/wrap.hpp>
#include <kt/string/pad.hpp>
#include <map>
namespace kt {
 
namespace program_option {

using key_t         = std::string_view;           //!< String type used for argument keys.
using value_t       = std::string_view;           //!< String type used for argument values.
using arg_t         = std::pair<key_t, value_t>;  //!< Command line argument as a key/value string pair.
using arg_store     = std::vector<arg_t>;         //!< Vector of arguments, stored as key/value pairs.

/*! \brief    Take an option key name and decorate it with dashes; one dash for
 *           short options, and two dashes for long options.
 *  \return   The decorated name.
 */
auto decorated
    ( key_t _k        //!< Option key name.
    ) 
    -> std::string
  {
    std::string result = _k.size() == 1? "-" : "--";
    result += _k;
    return result;
  }
/*! \brief    Parse the command line arguments into pairs of keys and values.
 *  \return   A vector of key/value `string_view` pairs.
 */
auto parse(int argc, char* argv[]) -> arg_store
  {
    using namespace std;
    arg_store opts;
    arg_t     arg;
    auto parse = [&](const char* argstr)
      {
        auto current_char = argstr;
        auto peek = [&]
          {
            return *current_char;
          };
        auto advance = [&]
          {
            if(peek() != 0) ++current_char;
          };
        auto parse_value = [&]
          {
            auto begin = current_char;
            char ch;
            while(ch = peek())
            {
              advance();
            }
            return string_view(begin, current_char - begin);
          };
        auto parse_long_arg = [&]
          {
            auto ch = peek();
            if(ch == '-' || ch == '=')
            {
              throw std::runtime_error(concat("Unexpected \"", ch, "\" at beginning of arg \"", argstr, "\""));
            }
            auto begin = current_char;
            string_view key, value;
            while(ch = peek())
            {
              advance();
              if(peek() == '=')
              {
                key = string_view(begin, current_char - begin);
                advance();
                value = parse_value();
                opts.push_back(make_pair(key, value));
                return;
              }
            }
            key = string_view(begin, current_char - begin);
            opts.push_back(make_pair(key, value));
          };
        auto parse_short_args = [&]
          {
            auto ch = peek();
            if(ch == '-' || ch == '=')
            {
              throw std::runtime_error(concat("Unexpected \"", ch, "\" at beginning of arg list \"", argstr, "\""));
            }
            while(ch = peek())
            {
              auto key = string_view(current_char, 1);
              advance();
              if(peek() == '=')
              {
                advance();
                auto value = parse_value();
                opts.push_back(make_pair(key, value));
                break;
              }
              opts.push_back(make_pair(key, string_view()));
            }
          };
        auto parse_args = [&]
          {
            auto ch = peek();
            if(ch == '-')
            {
              advance();
              parse_long_arg();
            }
            else
            {
              parse_short_args();
            }
          };
        auto ch = peek();
        if(ch == '-')
        {
          advance();
          parse_args();
        }
        else
        {
          auto value = parse_value();
          opts.push_back(make_pair(string_view(), value));
        }
        
      };
    for(int arg = 0; arg < argc; ++arg)
    {
      parse(argv[arg]);
    }
    return opts;
  }

/*! \brief  Enum specifying whether an argument takes a value. */
enum class value_is 
  { absent          //!< Exception thrown if value is provided.
  , optional        //!< Value is optional.
  , required        //!< Exception thrown if no value is given.
  };


/*! \brief    Function wrapper which receives a parsed argument and its value
 *            and passes it to the given function.
 */
template<typename T, value_is VI = value_is::optional>
class for_type final
{
  using handler_t         = std::function<void(key_t, value_t)>;
public:
  /*! \brief Construct using given function. */
  explicit for_type
      ( const std::function<void(const std::optional<T>&)>& _ah  //!< Callback function which receives the parsed value.
      )
    {
      handler_ = [&_ah](auto key, auto value)
        {
          if(value.empty())
          {
            if constexpr(VI == value_is::required)
            {
              throw std::runtime_error(concat("option \"", decorated(key), "\" requires a value, but none was given"));
            }
            _ah(std::optional<T> {});
          }
          else
          {
            if constexpr(VI == value_is::absent)
            {
              throw std::runtime_error(concat("option \"", decorated(key), "\" takes no value but was given \"", value, "\""));
            }
            try
            {
              auto v = boost::lexical_cast<T>(value);
              _ah(v);
            }
            catch(...)
            {
              throw std::runtime_error(concat("invalid value \"", value, "\" given to option \"", decorated(key), "\""));
            }
          }
        };
    }
  /*! \brief Access the stored callback function. */
  auto handler() const -> const handler_t& { return handler_; }
private:
  handler_t   handler_;
};

template<typename T>
  auto value(T& _vref) -> for_type<T>
  {
    return for_type<T>([&_vref](const T& _v)
      {
        _vref = _v;
      });
  }

enum class allow_listing  { yes, no };
enum class process        { normal, textonly, heading };

/*! \brief    Storage for a command-line parameter's definition and callback function. */
class option final
{
public:
  using handler_t = std::function<void(key_t, value_t)>;    //!< Type of the stored handler function.
  using description_t = std::string_view;                   //!< String type for the option's description text.
  option
        ( key_t             _k                            //!< Option key.                           
        , char              _c                            //!< Short key.                           
        , description_t     _d                            //!< Description text.           
        , const handler_t&  _fn                           //!< Handler function.        
        , allow_listing     _al = allow_listing::yes
        , process           _ap = process::normal       
        )
      : long_key_(_k)
      , short_key_(_c)
      , description_(_d)
      , handler_(_fn)
      , is_listed_(_al)
      , process_(_ap)
    {}
  template<typename TypeT, value_is ValueProvision = value_is::optional>
  option
        ( key_t                                   _k                           //!< Option key.
        , char                                    _c                           //!< Short key.
        , description_t                           _d                           //!< Description text.
        , const for_type<TypeT, ValueProvision>&  _ft                          //!< Handler function.
        , allow_listing                           _al = allow_listing::yes
        , process                                 _ap = process::normal
        )
      : long_key_(_k)
      , short_key_(_c)
      , description_(_d)
      , handler_(_ft.handler())
      , is_listed_(_al)
      , process_(_ap)
    {}
  option
        ( key_t            _k                           //!< Option key.
        , char             _c                           //!< Short key.
        , description_t    _d                           //!< Description text.
        , allow_listing    _al = allow_listing::yes
        , process          _ap = process::normal
        )
      : option(_k, _c, _d, for_type<int, value_is::absent>([](auto){}), _al, _ap)
    {}
  option
        ( char          _c                              //!< Short key.
        , description_t _d                              //!< Description text.
        , allow_listing _al = allow_listing::yes
        , process       _ap = process::normal
        )
      : option(key_t {}, _c, _d, for_type<int, value_is::absent>([](auto){}), _al, _ap)
    {
      // can't allow empty long option
      long_key_ = short_key();
    }
  option
        ( key_t _k          //!< Option key.
        , description_t _d  //!< Description text.
        , allow_listing _al = allow_listing::yes
        , process       _ap = process::normal
        )
      : option(_k, 0, _d, for_type<int, value_is::absent>([](auto){}), _al, _ap)
    {}
  /*! \brief      Call the argument handler. */
  auto set
      ( value_t _v    //!< The value to send to the argument handler.
      ) 
      const -> void
    {
      handler_(long_key_, _v);
    }

  auto short_key()    const -> key_t            { return short_key_ == 0? key_t {} : key_t { &short_key_, 1 }; }
  auto long_key()     const -> key_t            { return long_key_; }
  auto description()  const -> description_t    { return description_; }
  auto is_listed()    const -> allow_listing    { return is_listed_; }
  auto process_type() const -> process          { return process_; }       
  /*! \brief      Compares the stored long and short keys to the given key. 
   *  \return     True on match.
   */
  auto operator==(key_t _k) const -> bool
    {
      return _k == long_key() || _k == short_key();
    }
private:
  key_t             long_key_;
  char              short_key_;
  description_t     description_;
  handler_t         handler_;
  allow_listing     is_listed_;
  process           process_;
};

template<typename...ArgTs>
auto hidden(ArgTs&&...args) -> option
  {
    return option(std::forward<ArgTs>(args)..., allow_listing::no);
  }
template<typename StringT>
auto text(StringT&& _description) -> option
  {
    return option("", _description, allow_listing::yes, process::textonly);
  }
template<typename StringT>
auto heading(StringT&& _description) -> option
  {
    return option("", _description, allow_listing::yes, process::heading);
  }
using table       = std::vector<option>;
template<typename StreamT>
auto operator<<(StreamT&& _stream, const table& _table) -> StreamT&
  {
    auto cols = terminal::get_columns();
    auto rows = terminal::get_rows();
    constexpr size_t column_separator_width = 2; 
    size_t left_col_max_width = 30;
    int left_col_width = std::min(left_col_max_width, cols / 2 - column_separator_width);
    int right_col_width = cols / 2;
    auto column_separator = std::string { column_separator_width, ' ' };
    if(left_col_width < 8)
    {
      throw std::runtime_error("Your display is impossibly small, therefore we could not print the help message--although it doesn't matter what this message says, you probably can't see it anyway.");
    }
    for(auto& opt : _table)
    {
      if(opt.is_listed() == allow_listing::yes)
      {
        if(opt.process_type() == process::textonly)
        {
          _stream << opt.description() << "\n";        
        }
        else if(opt.process_type() == process::heading)
        {
          _stream << "\n  " << opt.description() << "\n\n";        
        }
        else
        {
          auto left_column_rows = [&]
            {
              std::string s;
              auto lk = opt.long_key();
              auto sk = opt.short_key();
              s += decorated(lk);
              s += (lk == sk || sk.empty()? "" : concat(", ", decorated(sk)));
              return word_wrap(s, left_col_width);
            };
          auto right_column_rows = [&]
            {
              return word_wrap(opt.description(), right_col_width);
            };
          auto long_key   = decorated(opt.long_key());
          auto short_key  = decorated(opt.short_key());
          auto lc = left_column_rows();
          auto rc = right_column_rows();
          for(size_t i = 0; i < lc.size() && i < rc.size(); ++i)
          {
            auto l_str = i < lc.size()? pad_copy(lc[i], left_col_width)  : pad_copy("", left_col_width);
            auto r_str = i < rc.size()? pad_copy(rc[i], right_col_width) : pad_copy("", right_col_width);
            _stream << l_str << column_separator << r_str << "\n";
          }
        }
      }
    }
    return _stream;
  }
using job         = std::function<void(void)>;
using option_ref  = std::reference_wrapper<const option>;
using jobs        = std::vector<std::pair<option_ref, job>>;

auto scan(const arg_store& _args, const table& _opts) -> jobs
  {
    jobs results;
    // skip the first arg, since it's just the executable name
    auto arg = _args.begin();
    for(++arg; arg != _args.end(); ++arg)
    {
      auto arg_key    = arg->first;
      auto arg_value  = arg->second;
      bool no_match = true;
      for(const auto& opt : _opts)
      {
        if(opt.process_type() == process::normal)
        {
          if(opt == arg_key)
          {
            no_match = false;
            results.emplace_back(std::make_pair(std::cref(opt), [arg_value, &opt] { opt.set(arg_value); }));
            break;
          }
        }
      }
      if(no_match)
      {
        throw std::runtime_error(concat("unrecognized option: ", decorated(arg_key)));
      }
    }
    return results;
  }

auto counts(const jobs& _jobs) -> std::map<key_t, size_t>
  {
    std::map<key_t, size_t> results;
    for(const auto& job : _jobs)
    {
      results[job.first.get().long_key()] += 1;
    }
    return results;
  }
auto count(const jobs& _jobs, key_t _key) -> size_t
  {
    size_t result = 0;
    for(const auto& job : _jobs)
    {
      if(job.first.get() == _key)
      {
        ++result;
      }
    }
    return result;
  }
auto run(const jobs& _jobs) -> void
  {
    for(const auto& job : _jobs)
    {
      auto& do_action = job.second;
      do_action(); 
    }
  }
template<typename FnT>
auto scan(const arg_store& _args, const table& _opts, FnT&& _pre_run) -> void
  {
    auto jobs = scan(_args, _opts);
    if(_pre_run(jobs) == 0)
    {
      run(jobs);
    }
  }
template<typename FnT>
auto scan_run(const arg_store& _args, const table& _opts) -> void
  {
    scan(_args, _opts, [](auto&&) { return 0; });
  }
} /* namespace options */
} /* namespace kt */
#endif//kt_options_hpp_20210919_170150_PDT
