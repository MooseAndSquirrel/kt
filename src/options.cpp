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
#include <kt/options.hpp>

extern char** environ;

namespace kt {
namespace program_option {

auto decorated(key_t _k) -> std::string
  {
    std::string result = _k.size() == 1? "-" : "--";
    result += _k;
    return result;
  }

auto parse(const std::string& _filename) -> arg_store
  {
    using namespace std;
    auto file = std::fstream(_filename, ios::in);
    return parse(file);
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
            while((ch = peek()))
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
            while((ch = peek()))
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
            while((ch = peek()))
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
auto job_map(const jobs& _jobs) -> std::multimap<key_t, job>
  {
    std::multimap<key_t, job> results;
    for(const auto& job : _jobs)
    {
      results.emplace(job.first.get().long_key(), job.second);
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
auto environ() -> env_map
  {
    using namespace std;
    env_map results;
    for(size_t idx = 0; ::environ[idx] != NULL; ++idx)
    {
      size_t current_char = 0;
      auto end_of_line = [&] 
        { 
          return ::environ[idx][current_char] == 0; 
        };
      auto getch = [&] 
        {
          return end_of_line()? 0 : ::environ[idx][current_char++];
        };
      char ch;
      string key;
      while((ch = getch()) != '=' && end_of_line() == false)
      {
        key += ch;
      }
      string value;
      while((ch = getch()) != 0)
      {
        value += ch;
      }
      results.emplace(make_pair(key, value));
    }
    return results;
  }
  
} /* namespace kt */
} /* namespace options */
