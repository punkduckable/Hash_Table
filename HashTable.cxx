#include <string>
#include <iostream>
#include <stdio.h>


////////////////////////////////////////////////////////////////////////////////
// Item, Item Node, Item List

template<typename K, typename V>
struct Item {
  K key;
  V value;
}; // struct Item {



template<typename K, typename V>
class Item_Node {
  private:
    Item<K, V> Item;
    Item_Node<K,V>* Next;

  public:
    // Constructor, destructor
    Item_Node(const K& key, const V& value): Next(NULL) {
      Item.key = key;
      Item.value = value;
    } // Item_Node(const K& key, const V& value): Next(NULL) {
    ~Item_Node() {}

    ////////////////////////////////////////////////////////////////////////////
    // Key, value methods

    // determine key and value of this node
    V getValue() const { return Item.value; }
    K getKey() const { return Item.key; }

    // We need to be able to update the item's value (though not the each node's
    // key should be constant)
    void setValue(V New_Value) { Item.value = New_Value; }


    ////////////////////////////////////////////////////////////////////////////
    // Next methods
    Item_Node* getNext() const { return Next; }
    void setNext(Item_Node<K, V>* Next ) { Item_Node::Next = Next; }
}; // class Item_Node {



// Item List exception classes.
class List_Exception {
  private:
    std::string Error_Message;
  public:
    List_Exception(const char* Error_Message): Error_Message(Error_Message) {}
    const char* what() const { return Error_Message.c_str(); }
}; // class List_Exception {

class Item_Not_In_List: public List_Exception {
  public:
    Item_Not_In_List(const char * Error_Message) : List_Exception(Error_Message) {}
}; // class Item_Not_In_List_Error: public List_Exception {



// Item list
template<typename K, typename V>
class Item_List {
  private:
    Item_Node<K,V>* Start;                  // First node in the list
    Item_Node<K,V>* End;                    // Last node in the list

    /* Since lists deal with dynamic memory, we need to eliminate the default
    copy constructor and = operator */
    Item_List(const Item_List<K, V> & ) = delete;
    Item_List<K, V>& operator=(const Item_List &) = delete;

  public:
    // Constructors, destructor
    Item_List() : Start(NULL), End(NULL) {}
    ~Item_List() {
      // Starting from Start, cycle through the nodes and free them 1 by 1
      while(Start != NULL) {
        Item_Node<K, V>* Next = Start->getNext();
        delete Start;
        Start = Next;
      } // while(Start != End) {
    } // ~Item_List() {


    /* Put a new value in the list. If the new value's key matches an existing
    item's key then we update that item's value. Otherwise, add a new item
    to the end of the list. */
    void put(const K key, const V value) {
      /* Check if any of the nodes in the list have a key that matches the new
      key. If so, update that node's value. Otherwise, append a new node to the
      end of the list */
      Item_Node<K, V>* entry = Start;
      while(entry != NULL) {
        /* Check if the key of the current entry matches the key. If so, update
        that node's value and return. Otherwise, move onto the next node */
        if(entry->getKey() == key) {
          entry->setValue(value);
          return;
        } // if(entry->getKey() == key) {
        else { entry = entry->getNext(); }
      } // while(entry != Null) {

      /* If we are here then that means that new key did not match the key of
      any of the existing nodes in this list. That could mean the list is empty,
      a case that we need to handle. */

      /* If Start == NULL then this is the first node in the list. */
      if(Start == NULL) {
        Start = new Item_Node<K, V>{key, value};
        End = Start;
      } // if(Start == NULL) {

      /* Otherwise, we need to append a new node onto the end of the existing
      list */
      else {
        Item_Node<K, V>* New_End = new Item_Node<K, V>{key, value};
        End->setNext(New_End);
        End = New_End;
      } // else
    } // void put(const K key, const V value) {


    // Remove an item with a particular key from the list
    void remove(const K key) {
      /* Cycle through the nodes. If we find one whose key matches the specified
      key then remove that item from the list. */
      Item_Node<K, V>* prev = NULL;
      Item_Node<K, V>* entry = Start;

      while(entry != NULL) {
        // If entry's key matches the specified key, remove that node!
        if(entry->getKey() == key) {
          /* If the first node's key matches the specified key then we just need
          to update Start. Otherwise, we need to have prev the previous node
          point to the node after entry. */
          if(entry == Start) {
            /* If the list has just one item, then we need to also update End.
            Otherwise, just have Start point to the next node. */
            if(Start == End) { Start = End = NULL; }
            else { Start = Start->getNext(); }
          } //   if(entry == Start) {
          else { prev->setNext(entry->getNext()); }

          // Now delete the removed node
          delete entry;
        } // if(entry->getKey() == key) {

        // Otherwise, move onto the next node
        prev = entry;
        entry = entry->getNext();
      } // while(entry != NULL) {

      /* If we get here then that means that the specified key did not match the
      key of any node in the list. In this case, there is nothing to remove, so
      we're done */
      return;
    } // void remove(const K key) {


    // Get the value of the node with a particular key. If no such node is
    // found, then throw an exception.
    V get(const K key) {
      /* Search through the items in the list until we find one whose key matches the
      specified key. If no such key is found, throw an exception. */
      Item_Node<K, V>* entry = Start;
      while(entry != NULL) {
        // if entry's key matches the specified key they return that node's value
        if(entry->getKey() == key) { return entry->getValue(); }

        // Otherwise, move onto the next item
        entry = entry->getNext();
      } // while(entry != NULL)

      /* If we get here then none of the items in the list had a key that
      matched the specified key. As such, throw an exception! */
      char Error_Message_Buffer[500];
      sprintf(Error_Message_Buffer, "Item Not In List Error: There are no items in this list with key %d\n", key);
      throw Item_Not_In_List(Error_Message_Buffer);
    } // V get(const K key) {


    // Printing method
    friend std::ostream& operator<<(std::ostream & os, const Item_List<K, V>& List) {
      Item_Node<K, V>* entry = List.Start;

      while(entry != NULL) {
        os << "{" << entry->getKey() << " : " << entry->getValue() << "}";

        // If there is another node to print then indiciate so with an arrow
        if(entry->getNext() != NULL ) { os << " -> "; }

        entry = entry->getNext();
      } // while(entry != NULL) {

      return os;
    } // std::ostream& operator<<(std::ostream & os, const Item_Node<K, V>& List) {
}; // class Item_List {





////////////////////////////////////////////////////////////////////////////////
// Hash table

// Hash Table exception classes.
class Hash_Table_Exception {
  private:
    std::string Error_Message;
  public:
    Hash_Table_Exception(const char* Error_Message): Error_Message(Error_Message) {}
    const char* what() const { return Error_Message.c_str(); }
}; // class Hash_Table_Exception {

class Invalid_Key: public Hash_Table_Exception {
  public:
    Invalid_Key(const char * Error_Message) : Hash_Table_Exception(Error_Message) {}
}; // class Invalid_Key: public Hash_Table_Exception {



template <typename V>
class Hash_Table {
  private:
    unsigned N_Buckets;
    Item_List<unsigned, V>* Buckets;

    // Hashing function
    unsigned Hash(unsigned key) const { return (key % N_Buckets); }

    // Delete the implicit = operator and copy constructor methods
    Hash_Table(const Hash_Table &) = delete;
    Hash_Table& operator=(const Hash_Table &) = delete;

  public:
    // Constructor, destructor
    Hash_Table(unsigned N_Buckets = 11) {
      /* I require that there are at least 11 buckets (I just picked a prime
      number to prevent collissions) */
      if(N_Buckets < 11) { N_Buckets = 11; }

      Hash_Table::N_Buckets = N_Buckets;
      Buckets = new Item_List<unsigned, V>[N_Buckets];
    } // Hash_Table(unsigned N_Buckets = 11) {

    ~Hash_Table() { delete [] Buckets; }


    // Insert an item into the table.
    void insert(unsigned key, V value) {
      // First, calculate the key of the hash
      unsigned bucket_index = Hash(key);

      // Now, add the new key-value pair into the selected bucket.
      Buckets[bucket_index].put(key, value);
    } // void insert(unsigned key, V value) {


    // remove the value with the specified key from the table.
    void remove(unsigned key) {
      // Calculate the bucket index.
      unsigned bucket_index = Hash(key);

      // Remove the item with the specified key from the selected bucket
      Buckets[bucket_index].remove(key);
    } // void remove(unsigned key) {


    /* Find the value of the item with the specified key. Throws an exception
    if no item with the specified key can be found */
    V search(unsigned key) const {
      // First, find the bucket index.
      unsigned bucket_index = Hash(key);

      // Now, try finding an item with the specified key in the selected bucket.
      try { return Buckets[bucket_index].get(key); }
      catch (const Item_Not_In_List& Er ) {
        /* If no item with the specified value can be found, then we raise an
        Invalid_Key exception. */
        char Error_Message_Buffer[500];
        sprintf(Error_Message_Buffer,
                "Invalid Key Error: This hash table does not have an entry with key %d\n",
                key);
        throw Invalid_Key(Error_Message_Buffer);
      } // catch (const Item_Not_In_List& Er ) {
    } // V search(unsigned key) const {


    // Printing method
    friend std::ostream & operator<<(std::ostream & os, const Hash_Table & Table) {
      unsigned N_Buckets = Table.N_Buckets;
      for(unsigned i = 0; i < N_Buckets; i++) {
        os << "Bucket " << i << ": " << Table.Buckets[i] << std::endl;
      } // for(unsigned i = 0; i < N_Buckets; i++) {

      return os;
    } // friend std::ostream & operator<<(std::ostream & os, const Hash_Table & Table) {
}; // class Hash_Table {
