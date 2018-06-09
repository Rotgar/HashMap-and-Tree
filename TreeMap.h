#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class TreeMap
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
    size_type height;

    Node *left;
    Node *right;
    Node *parent;

    Node()
      :height(0), left(nullptr), right(nullptr), parent(nullptr)
      {}

    Node(const value_type arg)
      :data(arg) {}
  };

  Node *root;
  Node *first;
  Node *last;

  size_type Size;

public:

  /******************* Node methods *************************/

  Node* makeEmpty(Node* t)
  {
    if(t == nullptr)
      return nullptr;
    else
    {
        makeEmpty(t->left);
        makeEmpty(t->right);
        delete t;
    }
    if(last != nullptr)
      last = nullptr;
    Size = 0;
    return nullptr;
  }

  int height(Node *t)
  {
    return t == nullptr ? -1 : t->height;
  }

      /* Function to max of left/right node */
  int max(int lhs, int rhs)
  {
    return lhs > rhs ? lhs : rhs;
  }

  void insertForTest(size_type number_of_elements)
  {
    for(size_t i = 0; i < number_of_elements; ++i)
    {
      value_type data(i, "Item");
      root = insert(data, root, nullptr);
    }
    first = findMax(root);
    first->right = last;
    last->parent = first;
    first = findMin(root);
  }

     /* Function to insert data recursively */
  Node *insert(value_type data, Node *t, Node *parent)
  {
    if (t == nullptr)
    {
       t = new Node(data);
       t->height = 0;
       t->right = t->left = nullptr;
       t->parent = parent;
       Size++;
    }
    else if (data.first < t->data.first)
    {
        t->left = insert(data, t->left, t);
        if (height(t->left) - height(t->right) == 2)
        {
          if (data.first < t->left->data.first)
          {
             t = rotateWithLeftChild(t);
          }
          else
          {
             t = doubleWithLeftChild(t);
          }
        }
     }
    else if (data.first > t->data.first)
    {
      if(t->right == last)
        t->right = insert(data, nullptr, t);
      else
        t->right = insert(data, t->right, t);

      if (height(t->right) - height(t->left) == 2)
      {
        if (data.first > t->right->data.first)
        {
          t = rotateWithRightChild(t);
        }
        else
        {
          t = doubleWithRightChild(t);
        }
      }
    }
    t->height = max(height(t->left), height(t->right)) + 1;
    return t;
  }

    /* Rotate binary tree node with left child */
  Node *rotateWithLeftChild(Node* k2)
  {
   Node *k1 = k2->left;
   k2->left = k1->right;
   if(k1->right != nullptr)
    k1->right->parent = k2;

   k1->right = k2;
   k1->parent = k2->parent;
   k2->parent = k1;

   k2->height = max(height(k2->left), height(k2->right)) + 1;
   k1->height = max(height(k1->left), k2->height) + 1;
   return k1;
  }

     /* Rotate binary tree node with right child */
  Node *rotateWithRightChild(Node *k1)
  {
   Node *k2 = k1->right;
   k1->right = k2->left;
   if(k2->left != nullptr)
    k2->left->parent = k1;

   k2->left = k1;
   k2->parent = k1->parent;
   k1->parent = k2;

   k1->height = max(height(k1->left), height(k1->right)) + 1;
   k2->height = max(height(k2->right), k1->height) + 1;
   return k2;
  }

        /*
          Double rotate binary tree node: first left child
          with its right child; then node k3 with new left child
        */
  Node *doubleWithLeftChild(Node *k3)
  {
     k3->left = rotateWithRightChild(k3->left);
     return rotateWithLeftChild(k3);
  }

         /*
           Double rotate binary tree node: first right child
           with its left child; then node k1 with new right child
        */
  Node *doubleWithRightChild(Node *k1)
  {
     k1->right = rotateWithLeftChild(k1->right);
     return rotateWithRightChild(k1);
  }

      /* find smallest key, also first element */
  Node *findMin(Node *t) const
  {
    /* recursively
      if(t == nullptr)
        return nullptr;
      else if(t->left == nullptr)
        return t;
      else
        return findMin(t->left);
    */
    if(t == nullptr)
      return t;

    while(t->left != nullptr)
      t = t->left;

    return t;
  }
      /* find biggest key, also first element */
  Node *findMax(Node *t) const
  {
    /* recursively
      if(t == nullptr)
        return nullptr;
      else if(t->right == nullptr || t->right == last)
        return t;
      else
        return findMax(t->right);
    */

    if(t == nullptr)
      return t;

    while(t->right != nullptr && t->right != last)
      t = t->right;

    return t;
  }

    /* Remove Node */
  Node *removeN(const key_type key, Node *t)
  {
    Node *tmp;
    if(t == nullptr || t == last)
      return nullptr;

    else if(key < t->data.first)
    {
      t->left = removeN(key, t->left);
    }
    else if(key > t->data.first)
    {
      t->right = removeN(key, t->right);
    }

    else
    {
      if(t->left == nullptr)
      {
        if(t->right == last)
        {
          if(t == root)
            root = nullptr;

          delete t;
          return last;
        }
        tmp = t;
        if(t == root)
          root = t->right;

        t = t->right;
        delete tmp;
        return t;
      }
      else if(t->right == last)
      {
        tmp = t;
        if(t == root)
          root = t->left;

        t = t->left;
        t->right = last;
        delete tmp;
        return t;
      }

      tmp = findMin(t->right);
      if(tmp != nullptr)
      {
        Node *_node = new Node(tmp->data);
        if(t == root)
        {
          root = _node;
          _node->parent = nullptr;
        }
        else
        {
          if(t->parent->right == t)
          {
            t->parent->right = _node;
          }
          if(t->parent->left == t)
          {
            t->parent->left = _node;
          }
          _node->parent = t->parent;
        }
        _node->left = t->left;
        t->left->parent = _node;
        _node->right = removeN(tmp->data.first, t->right);

        delete t;
        return _node;
      }
      else
      {
        if(t->left != nullptr)
        {
          if(t->parent->right == t)
          {
            t->parent->right = t->left;
          }
          if(t->parent->left == t)
          {
            t->parent->left = t->left;
          }
          t->left->parent = t->parent;
          delete t;
          return t->left;
        }
        delete t;
        return nullptr;
      }
    }
    return t;
  }

  Node *findN(const key_type key, Node *t) const
  {
    if(t == nullptr || t == last)
      return nullptr;
    else if(key < t->data.first)
      return findN(key, t->left);
    else if(key > t->data.first)
      return findN(key, t->right);
    else
      return t;
  }


    /******************* treeMap methods *******************/

  TreeMap()
  {
    root = nullptr;
    last = new Node();
    first = last;
    Size = 0;
  }

  ~TreeMap()
  {
    root = makeEmpty(root);
    if(last != nullptr)
      delete last;
  }

  TreeMap(std::initializer_list<value_type> list)
    :TreeMap()
  {
    for( auto it = list.begin(); it != list.end(); ++it )
      root = insert(*it, root, nullptr);

    first = findMax(root);
    first->right = last;
    last->parent = first;
    first = findMin(root);
  }

  TreeMap(const TreeMap& other)
    :TreeMap()
  {

    if(other.root != nullptr)
    {
      for( auto it = other.begin(); it != other.end(); ++it )
      {
          root = insert(*it, root, nullptr);
      }
      first = findMax(root);
      first->right = last;
      last->parent = first;
      first = findMin(root);
    }
  }

  TreeMap(TreeMap&& other)
  {
    root = other.root;
    first = other.first;
    last = other.last;
    Size = other.Size;
    other.root = nullptr;
    other.last = nullptr;
    other.first = nullptr;
    other.Size = 0;
  }

  TreeMap& operator=(const TreeMap& other)
  {
    if(root == other.root)
    {
      return *this;
    }

      root = makeEmpty(root);
      last = new Node();

      for( auto it = other.begin(); it != other.end(); ++it )
      {
          root = insert(*it, root, nullptr);
      }
      if(root != nullptr)
      {
        first = findMax(root);
        first->right = last;
        last->parent = first;
        first = findMin(root);
      }
      return *this;
  }

  TreeMap& operator=(TreeMap&& other)
  {
    if( this->root == other.root)
    {
      return *this;
    }
      root = makeEmpty(root);
      root = other.root;
      first = other.first;
      last = other.last;
      Size = other.Size;
      other.root = nullptr;
      other.last = nullptr;
      other.first = nullptr;
      other.Size = 0;

      return *this;
  }

  bool isEmpty() const
  {
    if(root == nullptr)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  mapped_type& operator[](const key_type& key)
  {
    Node *t = findN(key, root);
    if(t == nullptr)
    {
      value_type _data(key, mapped_type());
      root = insert(_data, root, nullptr);
      t = findN(key, root);
      first = findMax(root);
      first->right = last;
      last->parent = first;
      first = findMin(root);
    }

    return t->data.second;
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    if(findN(key, root) == nullptr)
    {
      throw std::out_of_range("out of range");
    }
    else
    {
      return findN(key, root)->data.second;
    }
  }

  mapped_type& valueOf(const key_type& key)
  {
    if(findN(key, root) == nullptr)
    {
      throw std::out_of_range("out of range");
    }
    else
    {
      return findN(key, root)->data.second;
    }
  }

  const_iterator find(const key_type& key) const
  {
    Node *t = findN(key, root);
    if(t == nullptr)
    {
       t = last;
    }
    const_iterator it;
    it.cptr = t;
    it.ptrTree = this;

    return it;
  }

  iterator find(const key_type& key)
  {
    //const_iterator it = find(key);

	Node* t = findN(key, root);

    if(t == nullptr)
    {
       t = last;
    }
    const_iterator it;
    it.cptr = t;
    it.ptrTree = this;

	return iterator(it);
  }

  void remove(const key_type& key)
  {
    if(Size == 0)
      throw std::out_of_range("out of range size is 0");
    else
    {
      Node *t = findN(key, root);
      if(t == nullptr)
        throw std::out_of_range("out of range not found");
      else
        removeN(key, root);

      first = findMin(root);
      Size--;
    }
  }

  void remove(const const_iterator& it)
  {

    if(Size == 0)
      throw std::out_of_range("out of range size is 0");
    else
    {
      Node *t = findN(it->first, root);
      if(t == nullptr)
        throw std::out_of_range("out of range not found");
      else
      removeN(it->first, root);
      first = findMin(root);
      Size--;
    }
  }

  size_type getSize() const
  {
    return Size;
  }

  Node *getRoot()
  {
    return root;
  }

  bool operator==(const TreeMap& other) const
  {
    if(Size != other.Size)
      return false;
    else
    {
      auto a = begin();
      auto b = other.begin();
      for(; a != end(); ++a, ++b)
      {
        if(*a != *b)
          return false;
      }
    }
    return true;
  }

  bool operator!=(const TreeMap& other) const
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
    it.cptr = first;
    it.ptrTree = this;
    return it;
  }

  const_iterator cend() const
  {
    ConstIterator it;
    it.cptr = last;
    it.ptrTree = this;
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
class TreeMap<KeyType, ValueType>::ConstIterator
{

  friend class TreeMap;

public:
  using reference = typename TreeMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename TreeMap::value_type;
  using pointer = const typename TreeMap::value_type*;

private:
  TreeMap<KeyType, ValueType>::Node *cptr;
  const TreeMap<KeyType, ValueType> *ptrTree;

public:

  explicit ConstIterator()
  {
    cptr = nullptr;
    ptrTree = nullptr;

  }

  ConstIterator(const ConstIterator& other)
  {
    cptr = other.cptr;
    ptrTree = other.ptrTree;
  }

  ConstIterator& operator++()
  {
      if(cptr->right != nullptr)
      {
         cptr = cptr->right;
         if(cptr->left != nullptr)
          cptr = ptrTree->findMin(cptr->left);
      }
      else if(cptr == ptrTree->last)
      {
        throw std::out_of_range("out of range (on last)");
      }
      else
      {
        if(cptr->parent->data.first > cptr->data.first)
        {
          cptr = cptr->parent;
        }
        else
        {
          while(cptr->data.first > cptr->parent->data.first)
            cptr = cptr->parent;

          cptr = cptr->parent;
        }
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
    if(cptr->left != nullptr)
    {
       cptr = cptr->left;
       if(cptr->right != nullptr)
        cptr = ptrTree->findMax(cptr->right);
    }
    else
    {
      if(cptr == ptrTree->first)
        throw std::out_of_range("out of range(on first)");

      if(cptr == ptrTree->last)
      {
        cptr = cptr->parent;
      }
      else if(cptr->parent->data.first < cptr->data.first)
      {
        cptr = cptr->parent;
      }
      else
      {
        while(cptr->data.first < cptr->parent->data.first)
          cptr = cptr->parent;

        cptr = cptr->parent;
      }
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
    if(cptr == nullptr || cptr == ptrTree->last)
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
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::reference;
  using pointer = typename TreeMap::value_type*;

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

#endif /* AISDI_MAPS_MAP_H */
