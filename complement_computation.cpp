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
      else {
        split_cube.push_back(DC);
      }
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

// Returns false if the variable is don't care for all cubes
// Otherwise, returns true
inline bool valid_split_index(const unsigned int& index, const cover_t& cover) {
  for (const cube_t& cube : cover) {
    if ( cube[index] != DC ) {
      return true;
    }
  }
  return false;
}

cover_t concatenate_cover(cover_t& complement_x, cover_t& complement_x_prime,
                          const unsigned int variable_num, const bool positive_unate,
                          const bool negative_unate) {
  // if ( positive_unate && negative_unate ) {
  //   throw "Variable specified as both positive and negative unate";
  // }
  // int largest;
  // if ( complement_x_prime.empty() && complement_x.empty() ) {
  //   largest = 0;
  // }
  // else if ( complement_x_prime.empty() ) {
  //   largest = (int)complement_x[0].size();
  // }
  // else if ( complement_x.empty() ) {
  //   largest = (int)complement_x_prime[0].size();
  // }
  // else if ( complement_x_prime[0].size() > complement_x[0].size() ) {
  //   largest = (int)complement_x_prime[0].size();
  // }
  // else {
  //   largest = (int)complement_x[0].size();
  // }
  // if ( complement_x_prime.size() == 1 && complement_x_prime[0].size() == 1
  //      && complement_x_prime[0][0] == DC ) {
  //   for (int i = 0; i < largest-1; i++) {
  //     complement_x_prime[0].push_back(DC);
  //   }
  // }
  // if ( complement_x.size() == 1 && complement_x[0].size() == 1
  //      && complement_x[0][0] == DC ) {
  //   for (int i = 0; i < largest-1; i++) {
  //     complement_x[0].push_back(DC);
  //   }
  // }
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
    ret.push_back(temp);
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
      if ( cube[var_num] != first_value || cube[var_num] == DC ) {
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
cover_t gen_complement(const cover_t& cover, bool* const split_vars, const unsigned int& num_vars) {
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
  }

  // None of the termination conditions were satisfied; split the cover and recurse
  int unate_var = find_unate(cover);
  unsigned int split_var = 0;
  // for (; split_vars[split_var]; split_var++) {
  //   if ( split_var >= num_vars ) {
  //     throw "Ran out of splitting variables";
  //   }
  // }
  while ( !valid_split_index(split_var, cover) ) {
    ++split_var;
  }
  bool pos_unate = false;
  bool neg_unate = false;
  if ( unate_var >= 0 && valid_split_index(unate_var, cover) ) {
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
  split_vars[split_var] = true;
  std::pair<cover_t, cover_t> covers = split_cover(split_var, cover);
  cover_t complement_x = gen_complement(covers.first, split_vars, num_vars);
  cover_t complement_x_prime = gen_complement(covers.second, split_vars, num_vars);
  // if ( complement_x_prime.size() == 1 && complement_x_prime[0].size() == 1
  //      && complement_x_prime[0][0] == DC ) {
  //   for (unsigned int i = 0; i < complement_x.size()-1; i++) {
  //     complement_x_prime[0].push_back(DC);
  //   }
  // }
  // if ( complement_x.size() == 1 && complement_x[0].size() == 1
  //      && complement_x[0][0] == DC ) {
  //   for (unsigned int i = 0; i < complement_x_prime.size()-1; i++) {
  //     complement_x[0].push_back(DC);
  //   }
  // }

  return concatenate_cover(complement_x, complement_x_prime, split_var, pos_unate, neg_unate);
}

int main() {
  unsigned int num_vars;
  cover_t cover = parse_cover(num_vars);
  bool* split_vars = new bool[num_vars];
  cover_t complement = gen_complement(cover, split_vars, num_vars);
  delete[] split_vars;
  std::cout << serialize_cover(complement);
  // }
  // catch (char const * msg) {
  //   std::cerr << msg << std::endl;
  //   return 1;
  // }
  return 0;
}
