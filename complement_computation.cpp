// Eric Carver
// Sun Apr 13 19:35:57 EDT 2014

#include <vector>
#include <iostream>
#include <utility>

#include "parse_cover.hpp"

// Splits a cover into two halves using Shannon Decomposition at the specified variable index
// (see http://www.cs.columbia.edu/~cs6861/handouts/complement-handout.pdf)
std::pair<cover_t, cover_t> split_cover(const int var, const cover_t& cover) {
  cover_t complement_x, complement_x_prime;

  for (const cube_t& cube : cover) {
    cube_t split_cube;
    for (int i = 0; i < cube.size()-1; i++) {
      if ( i != var ) {
        split_cube.push_back(cube[i]);
      }
      // else {
      //   split_cube.push_back(DC);
      // }
    }
    if ( cube[var] == ZERO ) {
      complement_x_prime.push_back(split_cube);
    }
    else {
      complement_x.push_back(split_cube);
    }
  }
  return std::make_pair(complement_x, complement_x_prime);
}

cover_t concatenate_cover(const cover_t& complement_x, const cover_t& complement_x_prime,
                          const int variable_num, const bool positive_unate, const bool negative_unate) {
  // if ( positive_unate && negative_unate ) {
  //   throw "Variable specified as both positive and negative unate";
  // }
  cover_t ret;
  for (const cube_t& cube : complement_x) {
    cube_t temp;
    for (int i = 0; i < cube.size()-1; i++) {
      if ( i == variable_num && !positive_unate ) {
        temp.push_back(ONE);
      }
      temp.push_back(cube[i]);
    }
    ret.push_back(temp);
  }
  for (const cube_t& cube : complement_x_prime) {
    cube_t temp;
    for (int i = 0; i < cube.size()-1; i++) {
      if ( i == variable_num && !negative_unate ) {
        temp.push_back(ZERO);
      }
      temp.push_back(cube[i]);
    }
    ret.push_back(temp);
  }
  return ret;
}

// Returns the index of the first unate variable it finds
// If no variables are unate, returns -1
int find_unate(const cover_t& cover) {
  for (int var_num = 0; var_num < cover[0].size()-1; var_num++) {
    input_type first_value = cover[0][var_num];
    bool is_unate = true;
    for (const cube_t& cube : cover) {
      if ( cube[var_num] != first_value ) {
        is_unate = false;
        break;
      }
    }
    if ( is_unate ) {
      return var_num;
    }
  }
  return -1;
}

// Recursive method to generate complement of an arbitrary cover
cover_t gen_complement(const cover_t& cover) {
  // Check termination conditions

  // Recurse
}

int main() {
  cover_t cover = parse_cover();

  return 0;
}
