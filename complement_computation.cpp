// Eric Carver
// Sun Apr 13 19:35:57 EDT 2014

#include <vector>
#include <iostream>
#include <utility>

#include "parse_cover.hpp"

// Splits a cover into two halves using Shannon Decomposition at the specified variable index
// (see http://www.cs.columbia.edu/~cs6861/handouts/complement-handout.pdf)
std::pair<cover_t, cover_t> split_cover(const unsigned int var, const cover_t& cover) {
  cover_t complement_x, complement_x_prime;

  for (const cube_t& cube : cover) {
    cube_t split_cube;
    for (unsigned int i = 0; i < cube.size(); i++) {
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
                          const unsigned int variable_num, const bool positive_unate,
                          const bool negative_unate) {
  // if ( positive_unate && negative_unate ) {
  //   throw "Variable specified as both positive and negative unate";
  // }
  cover_t ret;
  for (const cube_t& cube : complement_x) {
    cube_t temp;
    for (unsigned int i = 0; i < cube.size(); i++) {
      if ( i == variable_num && !positive_unate ) {
        temp.push_back(ONE);
      }
      temp.push_back(cube[i]);
    }
    ret.push_back(temp);
  }
  for (const cube_t& cube : complement_x_prime) {
    cube_t temp;
    for (unsigned int i = 0; i < cube.size(); i++) {
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
  for (unsigned int var_num = 0; var_num < cover[0].size(); var_num++) {
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
  // Condition 1: The cover is empty
  if ( cover.size() == 0 ) {
    cover_t ret;
    cube_t temp;
    temp.push_back(DC);
    ret.push_back(temp);
    return ret;
  }

  // Condition 2: The cover includes the universal cube, and is therefore a tautology
  for (const cube_t& cube : cover) {
    bool cover_tautology = true;
    for (const input_type& var : cube) {
      if ( cover_tautology && var != DC ) {
        cover_tautology = false;
      }
    }
    if ( cover_tautology ) {
      // return empty cover
      cover_t ret;
      return ret;
    }
  }

  // Condition 3: The cover includes a single cube
  if ( cover.size() == 1 ) {
    cover_t ret;
    // Calculate the complement of the cover using DeMorgan's Law
    for (unsigned int var_num = 0; var_num < cover[0].size(); var_num++) {
      cube_t temp;
      for (unsigned int i = 0; i < var_num; i++) {
        temp.push_back(DC);
      }
      switch ( cover[0][var_num] ) {
      case ZERO:
        temp.push_back(ONE);
        break;
      case ONE:
        temp.push_back(ZERO);
        break;
      case DC:
        continue;
      default:
        throw "Invalid input_type when generating complement";
      }
      for (unsigned int i = var_num+1; i < cover[0].size(); i++) {
        temp.push_back(DC);
      }
      ret.push_back(temp);
    }
    return ret;
  }

  // Condition 4: The cover is single-input dependent and not unate, and is therefore a tautology
  if ( find_unate(cover) == -1 ) {
    bool found_column = false;
    bool condition_four_possible = true;
    for (unsigned int var_num = 0; var_num < cover[0].size(); var_num++) {
      bool found_one = false;
      bool found_zero = false;
      bool found_dc = false;
      for (const cube_t& cube : cover) {
        switch ( cube[var_num] ) {
        case ZERO:
          found_zero = true;
          break;
        case ONE:
          found_one = true;
          break;
        case DC:
          found_dc = true;
          break;
        default:
          throw "Invalid input_type when checking for single-input dependence";
        }
      }
      if ( found_one && found_zero && !found_dc && !found_column ) {
        found_column = true;
      }
      else {
        condition_four_possible = false;
        break;
      }
    }
    if ( condition_four_possible ) {
      // Condition 4 is met. Return an empty cube
      cover_t ret;
      return ret;
    }
  }

  // None of the termination conditions were satisfied; split the cover and recurse
  int unate_var = find_unate(cover);
  unsigned int split_var = 0;
  bool pos_unate = false;
  bool neg_unate = false;
  if ( unate_var >= 0 ) {
    split_var = (unsigned int)unate_var;
    switch ( cover[0][unate_var] ) {
    case ONE:
      pos_unate = true;
      break;
    case ZERO:
      neg_unate = true;
      break;
    case DC:
      pos_unate = true;
      neg_unate = true;
      break;
    default:
      throw "Invalid input_type when determining unateness";
    }
  }
  std::pair<cover_t, cover_t> covers = split_cover(split_var, cover);
  cover_t complement_x = gen_complement(covers.first);
  cover_t complement_x_prime = gen_complement(covers.second);
  return concatenate_cover(complement_x, complement_x_prime, split_var, pos_unate, neg_unate);
}

int main() {
  try {
    cover_t cover = parse_cover();
    cover_t complement = gen_complement(cover);
    std::cout << serialize_cover(complement);
  }
  catch (char const * msg) {
    std::cerr << msg << std::endl;
    return 1;
  }
  return 0;
}
