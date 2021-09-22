#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <fstream>
#include <iostream>
#include <kt/options.hpp>

namespace fs = boost::filesystem;

auto do_set_int(const std::optional<int>& _value)
  {
    using namespace std;
    if(_value)
    {
      cout << *_value << endl;
    }
    else
    {
      cout << "no value given" << endl;
    }
  }

auto do_set_string(const std::optional<std::string>& _value)
  {
    using namespace std;
    if(_value)
    {
      cout << *_value << endl;
    }
    else
    {
      cout << "no value given" << endl;
    }
  }

int main(int argc, char* argv[]) try
{
  using namespace std;
  namespace po = kt::program_option;
  auto args = po::parse(argc, argv);
  auto opts = po::table
    { po::heading("Allowed options: ") 
    , po::option("help",            'h', "Show help.")
    , po::option("int",             'i', "test int arg", po::for_type<int>([](const auto& opt) { do_set_int(opt); })) 
    , po::option("required-string", 'r', "test int arg", po::for_type<string, po::value_is::required>([](const auto& opt) { do_set_string(opt); })) 
    , po::option("no-value",        'n', "test int arg", po::for_type<int, po::value_is::absent>  ([](const auto& opt) { do_set_int(opt); })) 
    , po::hidden("",                '0', "positional?",  po::for_type<string>([](const auto& opt) { do_set_string(opt); }))
    };

  po::scan(args, opts, [&]
      (const auto& jobs)
      {
        if(po::count(jobs, "help"))
        {
          cout << opts << endl;
          return 1;
        }
        return 0;
      }
    );
  return 0;
}
catch(const std::exception& e)
{
  using namespace std;
  cout << "Error: " << e.what() << endl;
}
