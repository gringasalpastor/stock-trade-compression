#ifndef COMPRESS_HUFFMAN_H
#define COMPRESS_HUFFMAN_H

#include <map>
#include <memory>
#include <queue>
#include <set>

#include "bits.hpp"
#include "trade.hpp"

/*
 * Collection of functions and structs for creating Huffman encodings. All functions are templated based on the Symbol
 * type, as to make it easy to switch to different symbols. For example, you could change the side from a `char` to a
 * `string` with very little code changes. It can make a best tree, create a best prefix mapping, create a canonical
 * prefix mapping, and convert a best mapping to canonical form.
 */

namespace compress {
namespace huffman {
/*
 * Simple node for creating Huffman tree
 */
template <class Symbol>
struct node {
  double                frequency;
  Symbol                symbol;
  std::shared_ptr<node> left;
  std::shared_ptr<node> right;
};

/*
 * Stores the length of the best prefix for a given symbol
 */
template <class Symbol>
struct symbol_data {
  Symbol   symbol;
  uint64_t prefix_length;
};

/*
 * Creates a symbol data struct
 */
template <class Symbol>
symbol_data<Symbol> make_symbol_data(const Symbol& symbol_, const uint64_t prefix_length_) {
  symbol_data<Symbol> s;
  s.prefix_length = prefix_length_;
  s.symbol        = symbol_;
  return s;
}

namespace detail {
/*
 * Used internally to create a best prefix mapping
 *
 * NOTE: It is best, but might not be in the canonical form
 */
template <class Symbol>
void traverse_nodes(const std::shared_ptr<node<Symbol>> node, std::map<Symbol, std::vector<bool>>& prefix_map,
                    const std::vector<bool>& previous_prefix = {}) {
  if (node->left == nullptr and node->right == nullptr) {
    prefix_map[node->symbol] = previous_prefix;
    return;
  }
  std::vector<bool> new_left = previous_prefix;
  new_left.push_back(false);
  std::vector<bool> new_right = previous_prefix;
  new_right.push_back(true);
  if (node->left) traverse_nodes(node->left, prefix_map, new_left);
  if (node->right) traverse_nodes(node->right, prefix_map, new_right);
}

template <class Symbol>
struct symbol_data_less_f {
  /*
   * Proper comparison for canonical Huffman tree construction is first length,
   * then symbol order. Your Symbol type must be comparable
   */
  bool operator()(const symbol_data<Symbol>& a, const symbol_data<Symbol>& b) const {
    if (a.prefix_length != b.prefix_length)
      return a.prefix_length < b.prefix_length;
    else
      return a.symbol < b.symbol;
  }
};

template <class Symbol>
struct node_less_f {
  /*
   * Use for the queue for nodes. Must have smallest at the top
   */
  bool operator()(const std::shared_ptr<node<Symbol>> a, const std::shared_ptr<node<Symbol>> b) const {
    // NOTE: We need to order them 'backwards'.
    return not(a->frequency < b->frequency);
  }
};
}  // namespace detail

template <class Symbol>
std::shared_ptr<node<Symbol>> make_node(const double frequency_, const Symbol& symbol_ = {}) {
  auto n       = std::make_shared<node<Symbol>>();
  n->frequency = frequency_;
  n->symbol    = symbol_;
  return n;
}

/*
 * Creates a best mapping from symbol to prefix. This is _not_ in canonical form.
 */
template <class Symbol>
std::map<Symbol, std::vector<bool>> create_prefix_map(const std::shared_ptr<node<Symbol>> root) {
  std::map<Symbol, std::vector<bool>> prefix_map;

  detail::traverse_nodes(root, prefix_map);
  return prefix_map;
}

/*
 * Creates a canonical mapping from symbol to prefix.
 */
template <class Symbol>
std::map<Symbol, std::vector<bool>> create_canonical_prefix_map(
    const std::set<symbol_data<Symbol>, detail::symbol_data_less_f<Symbol>>& symbol_data_) {
  std::map<Symbol, std::vector<bool>> canonical_prefix_map;
  std::vector<bool>                   canonical_prefix;
  for (const auto& sym : symbol_data_) {
    while (canonical_prefix.size() < sym.prefix_length) canonical_prefix.push_back(false);

    canonical_prefix_map[sym.symbol] = canonical_prefix;
    increment_bits(canonical_prefix);  // Add 1 to the code
  }

  return canonical_prefix_map;
}

/*
 * Converts a best mapping to a canonical mapping
 */
template <class Symbol>
std::map<Symbol, std::vector<bool>> create_canonical_prefix_map(const std::map<Symbol, std::vector<bool>>& prefix_map) {
  std::set<symbol_data<Symbol>, detail::symbol_data_less_f<Symbol>> symbols;
  for (const auto& p : prefix_map) {
    symbols.insert(make_symbol_data(p.first, p.second.size()));
  }

  return create_canonical_prefix_map(symbols);
}

/*
 * Implementation for Huffman algorithm. See any text on Huffman tree construction.
 */
template <class Symbol>
std::shared_ptr<node<Symbol>> make_huffman_tree(const std::map<Symbol, long>& symbol_count, const uint64_t num_trades) {
  std::priority_queue<std::shared_ptr<node<Symbol>>, std::vector<std::shared_ptr<node<Symbol>>>,
                      detail::node_less_f<Symbol>>
      symbol_queue;

  for (const auto& p : symbol_count)
    symbol_queue.push(make_node((p.second) / static_cast<double>(num_trades), p.first));

  while (symbol_queue.size() != 1) {
    auto a = symbol_queue.top();
    symbol_queue.pop();
    auto b = symbol_queue.top();
    symbol_queue.pop();
    auto node   = make_node<decltype(a->symbol)>(static_cast<double>(a->frequency + b->frequency));
    node->left  = a;
    node->right = b;
    symbol_queue.push(node);
  }

  return symbol_queue.top();
}
}  // namespace huffman
}  // namespace compress

#endif
