#include <cstddef>
#include <cstdlib>
#include <string>

#include "TreeMap.h"
#include "HashMap.h"

#include <iostream>
#include <chrono>
#include <string>


using us = std::chrono::microseconds;
using get_time = std::chrono::steady_clock;
using size_type = std::size_t;
using string = std::string;
using value_type = std::pair<const int, string>;


us testInsertHashMap( size_type number_of_elements)
{
  aisdi::HashMap<int, string> map(number_of_elements);

  auto start = get_time::now();

  for(size_type i = 0; i < number_of_elements; ++i)
  {
    value_type data(i, "Item");
    map.insertB(data);
  }
  return std::chrono::duration_cast<us>(get_time::now() - start);
}

us testInsertTreeMap( size_type number_of_elements)
{
    aisdi::TreeMap<int, string> tree;

    auto start = get_time::now();
    tree.insertForTest(number_of_elements);
    return std::chrono::duration_cast<us>(get_time::now() - start);
}

us testRemoveHashMap( size_type number_of_elements)
{
  aisdi::HashMap<int, string> map(number_of_elements);

  for(size_type i = 0; i < number_of_elements; ++i)
  {
    value_type data(i, "Item");
    map.insertB(data);
  }

  auto start = get_time::now();

  for(size_type i = 2; i < number_of_elements / 2; i += 4)
  {
    map.remove(i);
  }

  return std::chrono::duration_cast<us>(get_time::now() - start);
}

us testRemoveTreeMap( size_type number_of_elements)
{
    aisdi::TreeMap<int, string> tree;
    tree.insertForTest(number_of_elements);

    auto start = get_time::now();

    for(size_type i = 2; i < (number_of_elements / 2); ++i )
    {
      tree.removeN(i, tree.getRoot());
    }
    return std::chrono::duration_cast<us>(get_time::now() - start);
}

us testFindHashMap( size_type number_of_elements)
{
    aisdi::HashMap<int, string> map(number_of_elements);

    for(size_type i = 0; i < number_of_elements; ++i)
    {
      value_type data(i, "Item");
      map.insertB(data);
    }

    auto start = get_time::now();

    for(size_type i = 2; i < number_of_elements / 2; i += 4)
    {
      map.findB(i);
    }
    return std::chrono::duration_cast<us>(get_time::now() - start);
}

us testFindTreeMap( size_type number_of_elements)
{
    aisdi::TreeMap<int, string> tree;
    tree.insertForTest(number_of_elements);

    auto start = get_time::now();

    for(size_type i = 2; i < number_of_elements / 2; i += 4)
    {
      tree.findN(i, tree.getRoot());
    }
    return std::chrono::duration_cast<us>(get_time::now() - start);
}

int main(int argc, char** argv)
{
  const size_type repeatCount = argc > 1 ? std::atoll(argv[1]) : 60000;

  std::cout << "Test1: Inserting elements\n";
  auto diff = testInsertHashMap( repeatCount );
  std::cout << "HashMap: " << std::chrono::duration_cast<us>(diff).count()<< " us\n";
  auto diff2 = testInsertTreeMap( repeatCount );
  std::cout << "TreeMap: " << std::chrono::duration_cast<us>(diff2).count() << " us\n";
  std::cout << "Difference: " << std::chrono::duration_cast<us>(diff2-diff).count() << " us\n\n";

  std::cout << "Test2: Removing elements\n";
  diff = testRemoveHashMap( repeatCount );
  std::cout << "HashMap: " << std::chrono::duration_cast<us>(diff).count()<< " us\n";
  diff2 = testRemoveTreeMap( repeatCount );
  std::cout << "TreeMap: " << std::chrono::duration_cast<us>(diff2).count() << " us\n";
  std::cout << "Difference: " << std::chrono::duration_cast<us>(diff2-diff).count() << " us\n\n";

  std::cout << "Test1: Finding elements\n";
  diff = testFindHashMap( repeatCount );
  std::cout << "HashMap: " << std::chrono::duration_cast<us>(diff).count()<< " us\n";
  diff2 = testFindTreeMap( repeatCount );
  std::cout << "TreeMap: " << std::chrono::duration_cast<us>(diff2).count() << " us\n";
  std::cout << "Difference: " << std::chrono::duration_cast<us>(diff2-diff).count() << " us\n\n";

  return 0;
}
