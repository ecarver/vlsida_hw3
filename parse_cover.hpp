// Eric Carver
// Sun Apr 13 19:39:51 EDT 2014

#ifndef PARSE_COVER_HPP_DEFINED
#define PARSE_COVER_HPP_DEFINED

#include <vector>
#include <iostream>
#include <string>

enum input_type { ZERO, ONE, DC };

std::vector<std::vector<input_type> > parse_cover() {
  int num_vars;
  std::cin >> num_vars;
  int num_cubes;
  std::cin >> num_cubes;

  std::vector<std::vector<input_type> > ret;

  for (int cube = 0; cube < num_cubes; cube++) {
    std::string line;
    std::getline(std::cin, line);
    for (int var = 0; var < num_variables; var++) {
      switch( line[var] ) {
      case '0':
        ret[cube][var] = ZERO;
        break;
      case '1':
        ret[cube][var] = ONE;
        break;
      case '_':
        ret[cube][var] = DC; // Don't care
        break;
      default:
        throw "Invalid character in cover";
      }
    }
  }

  return ret;
}

#endif
