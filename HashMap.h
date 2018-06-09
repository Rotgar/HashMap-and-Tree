#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <string>
//#include <vector>
#include <sstream>
#include <functional>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class HashMap
{
public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<const key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

private:
  struct Node
  {
    value_type data;
    Node *next;
    Node *prev;

    Node()
      :next(nullptr), prev(nullptr){}

    Node(const value_type arg)
      :data(arg) {}
  };
  Node **head;
  Node *last;
  size_type Size, nrElem;
  size_type * bucketSize;
  const size_type defaultSize = 10000;

public:

  size_type Hash(const key_type key) const
  {
    size_type x = std::hash<size_type>{}(key);
    return x % Size;


/*   other hash functions
    key_type x = key;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x%Size;


    std::hash <key_type> convert;
    size_type x = convert(key);
    return x%Size;

    long x = 184432;
    std::stringstream convert;
    convert << key;
    std::string tmp = convert.str();

    for(size_type i=0; i<tmp.length(); ++i)
    {
      x = (x<<6)^(x>>26)^tmp[i];
    }
    return x % Size;
    */
  }

  void makeEmpty()
  {
    nrElem = 0;
    for(size_type i = 0; i < Size; ++i)
    {
      if(!IsEmptyB(i))
      {
        while(!IsEmptyB(i))
        {
          Node* tmp = head[i];
          head[i] = head[i]->next;
          delete tmp;
        }
      }
      bucketSize[i] = 0;
    }
    delete last;
    last = nullptr;
  }

  bool IsEmptyB(size_type &keyIndex) const
  {
    if(keyIndex < Size)
    {
      return head[keyIndex] == nullptr;
    }
    return true;
  }

  bool IsFull()
  {
	  try
	  {
		   Node* location = new Node();
		   delete location;
		   return false;
	   }
	   catch(std::bad_alloc&)
	   {
		   return true;
	   }
  }

  void insertB(const value_type _data)
  {
    if(IsFull())
    {
      throw std::out_of_range("out of range, list full");
    }
    else
    {
      size_type keyIndex = Hash(_data.first);
		  Node* newNode = new Node(_data);   // add new node
      newNode->next = newNode->prev = nullptr;

		  if(IsEmptyB(keyIndex))
		  {
			   head[keyIndex] = newNode;
		  }
		  else
		  {
			   Node* tmp = head[keyIndex];
			   while(tmp->next != nullptr)
			   {
				    tmp = tmp->next;
			   }
			   tmp->next = newNode;
         newNode->prev = tmp;
         tmp = nullptr;
		  }
      newNode = nullptr;
		  ++bucketSize[keyIndex];
	    ++nrElem;
    }
  }

  Node *findB(const key_type key) const
  {
    Node* tmp;
    size_type keyIndex = Hash(key);

    if(IsEmptyB(keyIndex))
    {
      return nullptr;
    }
    else if( head[keyIndex]->data.first == key)
    {
      return head[keyIndex];
    }
    else
    {
      for(tmp = head[keyIndex]; tmp->next != nullptr; tmp = tmp->next)
      {
        if(tmp->next->data.first == key)
        {
          return tmp->next;
        }
      }
    }
    return nullptr;
  }

  void removeB(const key_type key)
  {
    if(findB(key) == nullptr)
      throw std::out_of_range("out of range, no such elem to remove");
    else
    {
      Node* tmp;
      size_type keyIndex = Hash(key);

      if( head[keyIndex]->data.first == key)
      {
        tmp = head[keyIndex];
        head[keyIndex] = head[keyIndex]->next;
        --nrElem;
        --bucketSize[keyIndex];
        delete tmp;
      }
      else
      {
        for(tmp = head[keyIndex]; tmp->next != nullptr; tmp = tmp->next)
        {
          if(tmp->next->data.first == key)
          {
            Node* deleteNode = tmp->next;
            tmp->next = tmp->next->next;
            delete deleteNode;
            --nrElem;
            --bucketSize[keyIndex];
            break;
          }
        }
      }
    }
  }


  HashMap()
  {
    Size = defaultSize;
    head = new Node*[Size+1];
    bucketSize = new size_type[Size];
    for(size_type i = 0; i < Size; ++i)
    {
      head[i] = nullptr;
      bucketSize[i] = 0;
    }
    head[Size] = new Node();   // end
    last = head[Size];
    nrElem = 0;
  }

  HashMap(size_type hashSize)
  {
    Size = hashSize;
    head = new Node*[Size+1];
    bucketSize = new size_type[Size];
    for(size_type i = 0; i < Size; ++i)
    {
      head[i] = nullptr;
      bucketSize[i] = 0;
    }
    head[Size] = new Node(); // end
    last = head[Size];
    nrElem = 0;
  }

  ~HashMap()
  {
    makeEmpty();
    if(last != nullptr)
      delete last;

    delete[] head;
    delete[] bucketSize;
  }

  HashMap(std::initializer_list<value_type> list)
    :HashMap(list.size())
  {
    for(auto it = list.begin(); it != list.end(); ++it)
      insertB(*it);
  }

  HashMap(const HashMap& other)
    :HashMap(other.Size)
  {
    for(auto it = other.begin(); it != other.end(); ++it)
    {
      insertB(*it);
    }
  }

  HashMap(HashMap&& other)
  {
    head = other.head;
    last = other.last;
    Size = other.Size;
    nrElem = other.nrElem;
    bucketSize = other.bucketSize;
    other.head = nullptr;
    other.last = nullptr;
    other.Size = 0;
    other.nrElem = 0;
    other.bucketSize = nullptr;
  }

  HashMap& operator=(const HashMap& other)
  {
    if(head == other.head)
      return *this;

    makeEmpty();
    HashMap(other.Size);
    for(auto it = other.begin(); it != other.end(); ++it)
    {
      insertB(*it);
    }
    return *this;
  }

  HashMap& operator=(HashMap&& other)
  {
    if(head == other.head)
      return *this;

    makeEmpty();
    delete[] head;
    delete[] bucketSize;
    bucketSize = nullptr;
    head = other.head;
    last = other.last;
    Size = other.Size;
    nrElem = other.nrElem;
    bucketSize = other.bucketSize;
    other.head = nullptr;
    other.last = nullptr;
    other.Size = 0;
    other.nrElem = 0;
    other.bucketSize = nullptr;
    return *this;
  }

  bool isEmpty() const
  {
    if(nrElem == 0)
      return true;
    else
      return false;
  }

  mapped_type& operator[](const key_type& key)
  {
    Node* t = findB(key);
    if(t == nullptr)
    {
      value_type _data(key, mapped_type());
      insertB(_data);
      t = findB(key);
    }
    return t->data.second;
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    if(findB(key) == nullptr)
    {
      throw std::out_of_range("out of range");
    }
    else
    {
      return findB(key)->data.second;
    }
  }

  mapped_type& valueOf(const key_type& key)
  {
    if(findB(key) == nullptr)
    {
      throw std::out_of_range("out of range");
    }
    else
    {
      return findB(key)->data.second;
    }
  }

  const_iterator find(const key_type& key) const
  {
    Node* t = findB(key);

    if(t == nullptr)
    {
       t = last;
    }
    ConstIterator it;
    it.cptr = t;
    it.ptrMap = this;

    return it;
  }

  iterator find(const key_type& key)
  {
    //const_iterator it = find(key);
    Node* t = findB(key);

    if(t == nullptr)
    {
       t = last;
    }
    ConstIterator it;
    it.cptr = t;
    it.ptrMap = this;

	return iterator(it);
  }

  void remove(const key_type& key)
  {
    removeB(key);
  }

  void remove(const const_iterator& it)
  {
    removeB(it->first);
  }

  size_type getSize() const
  {
    return nrElem;
  }

  bool operator==(const HashMap& other) const
  {
    if(getSize() != other.getSize() || Size != other.Size)
      return false;

    for(const auto& a : other)
    {
      const_iterator i = find(a.first);
      if(i == end() || i->second != a.second)
        return false;
    }
    return true;
  }

  bool operator!=(const HashMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
    return iterator(cbegin());
  }

  iterator end()
  {
    return iterator(cend());
  }

  const_iterator cbegin() const
  {
    ConstIterator it;

    if(isEmpty())
      it.cptr = last;
    else
    {
      for(size_type i = 0; i < Size; ++i)
      {
        if(bucketSize[i] != 0)
        {
          it.cptr = head[i];
          break;
        }
      }
    }
    it.ptrMap = this;
    return it;
  }

  const_iterator cend() const
  {
    ConstIterator it;
    it.cptr = last;
    it.ptrMap = this;
    return it;
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator
{
  friend class HashMap;

public:
  using reference = typename HashMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename HashMap::value_type;
  using pointer = const typename HashMap::value_type*;

private:
  HashMap<KeyType, ValueType>::Node *cptr;
  const HashMap<KeyType, ValueType> *ptrMap;

public:

  explicit ConstIterator()
  {
    cptr = nullptr;
    ptrMap = nullptr;
  }

  ConstIterator(const ConstIterator& other)
  {
    cptr = other.cptr;
    ptrMap = other.ptrMap;
  }

  ConstIterator& operator++()
  {
    size_type i = ptrMap->Hash(cptr->data.first);
    if(cptr->next == nullptr)
    {
      if(cptr == ptrMap->last)
        throw std::out_of_range("out of range (on last)");
      else
      {
        if(ptrMap->head[i+1] == nullptr)
        {
          for(size_type j = i+2; j < ptrMap->Size; ++j)
          {
            if(ptrMap->bucketSize[j] != 0)
            {
              cptr = ptrMap->head[j];
              return *this;
            }
          }
          cptr = ptrMap->last;
        }
        else
          cptr = ptrMap->head[i+1];
      }
    }
    else
    {
      cptr = cptr->next;
    }
    return *this;
  }

  ConstIterator operator++(int)
  {
    ConstIterator it = *this;
    ++(*this);
    return it;
  }

  ConstIterator& operator--()
  {
    size_type i;
    if(cptr == ptrMap->last)
      i = ptrMap->Size;
    else
      i = ptrMap->Hash(cptr->data.first);

    if(cptr->prev == nullptr)
    {
      if(cptr == ptrMap->head[0])
        throw std::out_of_range("out of range (on first)");
      else
      {
        if(ptrMap->head[i-1] == nullptr)
        {
          for(size_type j = i-2; j > 0; --j)
          {
            if(ptrMap->bucketSize[j] != 0)
            {
              cptr = ptrMap->head[j];
              while(cptr->next != nullptr)
              {
                cptr = cptr->next;
              }
              return *this;
            }
          }
          throw std::out_of_range("out of range (on first)");
        }
        else
          cptr = ptrMap->head[i+1];
      }
    }
    else
    {
      cptr = cptr->prev;
    }
    return *this;
  }

  ConstIterator operator--(int)
  {
    ConstIterator it = *this;
    --(*this);
    return it;
  }

  reference operator*() const
  {
      if(cptr == nullptr || cptr == ptrMap->last)
        throw std::out_of_range("out of range operator*");

      return cptr->data;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    return cptr == other.cptr;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::reference;
  using pointer = typename HashMap::value_type*;

  explicit Iterator()
  {}

  Iterator(const ConstIterator& other)
    : ConstIterator(other)
  {}

  Iterator& operator++()
  {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator& operator--()
  {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};

}

#endif /* AISDI_MAPS_HASHMAP_H */
