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
class option
{
public:
  using description_t = std::string_view;
  option(key_t _k, char _sk, description_t _d)
    : key_(_k)
    , short_key_(_sk == 0? key_t {} : key_t { &_sk, 1 })
    , description_(_d)
  {
    // long key cannot be empty
    if(key_.empty()) 
    { 
      key_ = short_key_;
    }
  }
  option(key_t _k, description_t _d)
    : option(_k, 0, _d)
  {}
  option(char _sk, description_t _d)
    : option("", _sk, _d)
  {}

  auto key()              const -> key_t                { return key_; }
  auto short_key()        const -> key_t                { return short_key_; }
  auto description()      const -> description_t        { return description_; }
  auto store(value_t _v)        -> void                 { do_store(_v); }
  auto dispatch()         const -> void                 { do_dispatch(); }

  template<typename ReturnT>
    auto value() const -> std::optional<ReturnT> 
    { 
      std::optional<ReturnT> result;
      if(type_id<ReturnT>() == type_id())
      {
        const void* vptr = do_fetch();
        if(vptr)
        {
          result = *reinterpret_cast<ReturnT*>(vptr);
        }
      }
      return result;
    }
  auto operator==(key_t _k) const -> bool
    {
      return _k == key() || _k == short_key();
    }
private:
  key_t           key_;
  key_t           short_key_;
  description_t   description_;

  virtual auto do_store(value_t _v)       -> void 
    {
      throw error_option_takes_no_value(_v);
    }
  virtual auto do_dispatch()        const -> void {};
  virtual auto type_id()            const -> uintptr_t { return type_id<value_t>(); }
  virtual auto do_fetch()           const -> const void* 
    {
      static value_t _default;
      return &_default;
    }
protected:
  template<typename ObjT>
  static auto type_id() -> uintptr_t
    {
      static int        x  = 0;;
      static uintptr_t  px = (uintptr_t)&x;
      return px;
    }

  auto error_option_takes_no_value(value_t _v) -> std::runtime_error
    {
      auto msg = 
        kt::concat("Option \"", key(), "\" takes no value, but was given value \"", _v, "\"");
      return std::runtime_error(msg);
    }
};

enum class value_is { absent, optional, required };

template<typename T, value_is ValueConditional = value_is::absent>
class for_type : public option
{
  using dispatch_fn   = std::function<void(const std::optional<T>&)>;
public:
  template<typename FnT>
    for_type(key_t _k, char _sk, description_t _d, FnT&& _fn)
      : option(_k, _sk, _d)
      , dispatch_(_fn)
    {}
  template<typename FnT>
    for_type(key_t _k, char _sk, description_t _d)
      : option(_k, _sk, _d)
    {}
private:
  std::optional<T>  opt_value_;
  dispatch_fn       dispatch_;

  virtual auto type_id()  const -> uintptr_t    final override { return option::type_id<T>(); }
  virtual auto do_fetch() const -> const void*  final override { return opt_value_? &opt_value_.value() : nullptr; }

  auto do_store(value_t _v) -> void
    {
      if(_v.empty())
      {
      }
      else
      {
        if constexpr(ValueConditional == value_is::absent)
        {
          throw error_option_takes_no_value(_v);
        }
        else
        {
          opt_value_ = boost::lexical_cast<T>(_v);
        }
      }
    }
  auto do_dispatch() const -> void override
    {
      if(dispatch_)
      {
        dispatch_(opt_value_);
      }
    }
};

using table = std::vector<option>;

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
              throw std::runtime_error(kt::concat("Unexpected \"", ch, "\" at beginning of arg \"", argstr, "\""));
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
              throw std::runtime_error(kt::concat("Unexpected \"", ch, "\" at beginning of arg list \"", argstr, "\""));
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

using option_matches = std::vector<option>;

auto scan(const arg_store& _opts, const table& _handlers) -> option_matches
  {
    option_matches matches;
    auto try_coupling = [&matches]
      (const auto& opt, const auto& handler)
      {
        auto& key   = opt.first;
        auto& value = opt.second;
        if(key == handler.key() || key == handler.short_key())
        {
          // push a copy of the handler onto the job list
          matches.push_back(handler);
          auto& new_handler = matches.back();
          // store the value into the new handler object for later dispatch
          if(value.size())
          {
            new_handler.store(value); 
          }
        }
      };
    auto validate = [&_handlers, &try_coupling]
      (const auto& opt)
      {
        for(const auto& handler : _handlers)
        {
          try_coupling(opt, handler); 
        }
      };
    for(const auto& opt : _opts)
    {
      validate(opt); 
    }
    return matches;
  }

auto count(const option_matches& _matches, key_t _key) -> size_t
  {
    size_t result = 0;
    for(const auto& opt : _matches)
    {
      if(opt == _key) { ++result; }
    }
    return result;
  }
using option_map = std::multimap<key_t, std::reference_wrapper<const option>>;
auto map(const option_matches& _matches) -> option_map
  {
    option_map results;
    for(const auto& option : _matches)
    {
      results.emplace(std::make_pair(option.key(), std::cref(option)));
    }
    return results;
  }

} /* namespace options */
} /* namespace kt */
#endif//kt_options_hpp_20210919_170150_PDT
