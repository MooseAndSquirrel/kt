/*
  kt-options library
  Copyright (C) 2021 

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
#include <map>
namespace kt {
 
namespace program_option {

using key_t         = std::string_view;
using value_t       = std::string_view;
using arg_t         = std::pair<key_t, value_t>;
using arg_store     = std::vector<arg_t>;

auto decorated(key_t _k) -> std::string
  {
    std::string result = _k.size() == 1? "-" : "--";
    result += _k;
    return result;
  }
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

enum class value_is { absent, optional, required };

template<typename T, value_is VI = value_is::optional>
class for_type final
{
  using handler_t         = std::function<void(key_t, value_t)>;
public:
  explicit for_type(const std::function<void(const T&)>& _ah)
    {
      handler_ = [&_ah](auto key, auto value)
        {
          if(value.empty())
          {
            if constexpr(VI == value_is::required)
            {
              throw std::runtime_error(concat("option \"", decorated(key), "\" requires a value, but none was given"));
            }
          }
          else
          {
            if constexpr(VI == value_is::absent)
            {
              throw std::runtime_error(concat("option \"", decorated(key), "\" takes no value but was given \"", value, "\""));
            }
            auto v = boost::lexical_cast<T>(value);
            _ah(v);
          }
        };
    }
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
class option final
{
public:
  using handler_t = std::function<void(key_t, value_t)>;
  using description_t = std::string_view;
  option(key_t _k, char _c, description_t _d, const handler_t& _fn)
      : long_key_(_k)
      , short_key_(_c)
      , description_(_d)
      , handler_(_fn)
    {}
  template<typename TypeT, value_is ValueProvision = value_is::optional>
  option(key_t _k, char _c, description_t _d, const for_type<TypeT, ValueProvision>& _ft)
      : long_key_(_k)
      , short_key_(_c)
      , description_(_d)
      , handler_(_ft.handler())
    {}
  option(key_t _k, char _c, description_t _d)
      : option(_k, _c, _d, for_type<int, value_is::absent>([](auto){}))
    {}
  option(char _c, description_t _d)
      : option(key_t {}, _c, _d, for_type<int, value_is::absent>([](auto){}))
    {
      // can't allow empty long option
      long_key_ = short_key();
    }
  option(key_t _k, description_t _d)
      : option(_k, 0, _d, for_type<int, value_is::absent>([](auto){}))
    {}
  auto set(value_t _v) const -> void
    {
      handler_(long_key_, _v);
    }

  auto short_key() const -> key_t { return short_key_ == 0? key_t {} : key_t { &short_key_, 1 }; }
  auto long_key()  const -> key_t { return long_key_; }

  auto operator==(key_t _k) const -> bool
    {
      return _k == long_key() || _k == short_key();
    }
private:
  key_t           long_key_;
  char            short_key_;
  description_t   description_;
  handler_t       handler_;
};
template<typename...ArgTs>
auto make(ArgTs&&..._args) -> option 
  { 
    return option(std::forward<ArgTs>(_args)...); 
  }

using table       = std::vector<option>;
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
        if(opt == arg_key)
        {
          no_match = false;
          results.emplace_back(std::make_pair(std::cref(opt), [arg_value, &opt] { opt.set(arg_value); }));
          break;
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
auto scan(const arg_store& _args, const table& _opts, FnT&& _pre_run) -> int
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
    scan(
  }
} /* namespace options */
} /* namespace kt */
#endif//kt_options_hpp_20210919_170150_PDT
