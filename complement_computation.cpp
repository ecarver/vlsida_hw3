// Eric Carver
// Sun Apr 13 19:35:57 EDT 2014

#include <vector>
#include <iostream>
#include <utility>
#include <cstdlib>

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
      else {
        split_cube.push_back(DC);
      }
    }
    if ( cube[var] == ZERO || cube[var] == DC) {
      complement_x_prime.push_back(split_cube);
    }
    if ( cube[var] == ONE || cube[var] == DC) {
      complement_x.push_back(split_cube);
    }
  }
  return std::make_pair(complement_x, complement_x_prime);
}

// Returns false if the variable is don't care for all cubes
// Otherwise, returns true
inline bool valid_split_index(const cover_t& cover, const unsigned int& index) {
  for (const cube_t& cube : cover) {
    if ( cube[index] != DC ) {
      return true;
    }
  }
  return false;
}

// This function will combine two cubes that have only one variable different
inline cube_t absorb_cubes(const cube_t& first, const cube_t& second) {
  cube_t dummy;
  cube_t ret;
  if ( first.size() != second.size() ) {
    return dummy;
  }
  bool found_difference = false;
  for (unsigned int i = 0; i < first.size(); i++) {
    if ( first[i] == second[i] ) {
      ret.push_back(first[i]);
    }
    else if ( !found_difference ) {
      found_difference = true;
      ret.push_back(DC);
    }
    else {
      return dummy;
    }
  }
  return ret;
}

cover_t concatenate_cover(cover_t& complement_x, cover_t& complement_x_prime,
                          const unsigned int variable_num, const bool positive_unate,
                          const bool negative_unate) {
  cover_t ret;
  for (const cube_t& cube : complement_x) {
    cube_t temp;
    for (unsigned int i = 0; i < cube.size(); i++) {
      if ( i == variable_num ) {
        if ( positive_unate ) {
          temp.push_back(DC);
        }
        else {
          temp.push_back(ONE);
        }
      }
      else {
        temp.push_back(cube[i]);
      }
    }
    bool merged = false;
    cube_t absorbed;
    for (unsigned int i = 0; i < ret.size(); i++) {
      absorbed = absorb_cubes(ret[i], temp);
      if ( absorbed.size() ) {
        merged = true;
        ret[i] = absorbed;
        break;
      }
    }
    if ( !merged ) {
      ret.push_back(temp);
    }
  }
  for (const cube_t& cube : complement_x_prime) {
    cube_t temp;
    for (unsigned int i = 0; i < cube.size(); i++) {
      if ( i == variable_num ) {
        if ( negative_unate ) {
          temp.push_back(DC);
        }
        else {
          temp.push_back(ZERO);
        }
      }
      else {
        temp.push_back(cube[i]);
      }
    }
    bool merged = false;
    cube_t absorbed;
    for (unsigned int i = 0; i < ret.size(); i++) {
      absorbed = absorb_cubes(ret[i], temp);
      if ( absorbed.size() ) {
        merged = true;
        ret[i] = absorbed;
        break;
      }
    }
    if ( !merged ) {
      ret.push_back(temp);
    }
  }
  return ret;
}

inline bool pos_unate(const cover_t& cover, const int var_num) {
  for (const cube_t& cube : cover) {
    if ( cube[var_num] == ZERO ) {
      return false;
    }
  }
  return true;
}

inline bool neg_unate(const cover_t& cover, const int var_num) {
  for (const cube_t& cube : cover) {
    if ( cube[var_num] == ONE ) {
      return false;
    }
  }
  return true;
}

unsigned int find_split_var(const cover_t& cover, bool& positive_unate, bool& negative_unate) {
  unsigned int var = 0;
  while ( !valid_split_index(cover, var) ) {
    var++;
  }
  positive_unate = pos_unate(cover, var);
  negative_unate = neg_unate(cover, var);
  return var;
}

// Recursive method to generate complement of an arbitrary cover
cover_t gen_complement(const cover_t& cover, const unsigned int& num_vars) {
  // Check termination conditions
  // Condition 1: The cover is empty
  if ( cover.size() == 0 ) {
    cover_t ret;
    cube_t temp;
    for (unsigned int i = 0; i < num_vars; i++) {
      temp.push_back(DC);
    }
    ret.push_back(temp);
    return ret;
  }

  // Condition 2: The cover includes the universal cube, and is therefore a tautology
  for (const cube_t& cube : cover) {
    bool cover_tautology = true;
    for (const input_type& var : cube) {
      if ( cover_tautology && var != DC ) {
        cover_tautology = false;
        break;
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
    else if ( !found_one && !found_zero ) {
      continue;
    }
    else {
      condition_four_possible = false;
      break;
    }
  }
  if ( condition_four_possible && found_column ) {
    // Condition 4 is met. Return an empty cube
    cover_t ret;
    return ret;
  }

  // None of the termination conditions were satisfied; split the cover and recurse
  
  bool pos_unate = false;
  bool neg_unate = false;
  unsigned int split_var = find_split_var(cover, pos_unate, neg_unate);
  std::pair<cover_t, cover_t> covers = split_cover(split_var, cover);
  cover_t complement_x = gen_complement(covers.first, num_vars);
  cover_t complement_x_prime = gen_complement(covers.second, num_vars);

  return concatenate_cover(complement_x, complement_x_prime, split_var, pos_unate, neg_unate);
}

int main() {
  unsigned int num_vars;
  cover_t cover;
  cover = parse_cover(num_vars);
  cover_t complement = gen_complement(cover, num_vars);
  std::cout << serialize_cover(complement);

  return 0;
}
