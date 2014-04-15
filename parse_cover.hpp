// Eric Carver
// Sun Apr 13 19:39:51 EDT 2014

#ifndef PARSE_COVER_HPP_DEFINED
#define PARSE_COVER_HPP_DEFINED

#include <vector>
#include <iostream>
#include <string>

enum input_type { ZERO, ONE, DC };

typedef std::vector<input_type> cube_t;
typedef std::vector<cube_t> cover_t;

cover_t parse_cover() {
  int num_vars;
  std::cin >> num_vars;
  int num_cubes;
  std::cin >> num_cubes;

  cover_t ret;

  for (int cube = 0; cube < num_cubes; cube++) {
    std::string line;
    std::getline(std::cin, line);
    for (int var = 0; var < num_vars; var++) {
      switch( line[var] ) {
      case '0':
        ret[cube][var] = ZERO;
        break;
      case '1':
        ret[cube][var] = ONE;
        break;
      case '_':
      case '-':
        ret[cube][var] = DC; // Don't care
        break;
      default:
        throw "Invalid character in cover";
      }
    }
  }

  return ret;
}

std::ostream& serialize_cover(std::ostream& out, const cover_t& cover) {
  out << cover[0].size() << std::endl;
  out << cover.size() << std::endl;
  for (const cube_t& cube : cover) {
    for (const input_type& var : cube) {
      switch ( var ) {
      case ZERO:
        out << '0';
        break;
      case ONE:
        out << '1';
        break;
      case DC:
        out << '-';
        break;
      default:
        throw "Tried to serialize invalid cover";
      }
    }
    out << std::endl;
  }

  return out;
}

#endif
