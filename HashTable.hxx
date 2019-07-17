#if !defined(HASH_TABLE_HEADER)
#define HASH_TABLE_HEADER

#include <string>
#include <iostream>
#include <stdio.h>
#include "ItemList.hxx"


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
    const unsigned Max_List_Length = 3;     /* Longest allowed bucket list (longer list will prompt resize) */

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


    // Insert, remove, search, resize
    void insert(unsigned key, V value);     /* Insert an item into the table. */
    void remove(unsigned key);              /* remove the value with the specified key from the table. */
    V search(unsigned key) const;           /* Find the value of the item with the specified key. */
    void resize();                          /* Resie the table */


    // Printing method
    friend std::ostream & operator<<(std::ostream & os, const Hash_Table & Table) {
      unsigned N_Buckets = Table.N_Buckets;
      for(unsigned i = 0; i < N_Buckets; i++) {
        os << "Bucket " << i << ": " << Table.Buckets[i] << std::endl;
      } // for(unsigned i = 0; i < N_Buckets; i++) {

      return os;
    } // friend std::ostream & operator<<(std::ostream & os, const Hash_Table & Table) {
}; // class Hash_Table {



template <typename V>
void Hash_Table<V>::resize() {
  /* First, pick a new bucket size. This should be the first prime that is
  larger than twice the current bucket size.
  Since I haven't written a prime finding function, I'm just going to use
  twice the current size plus 1 */
  unsigned old_N_buckets = N_Buckets;
  N_Buckets = 2*(N_Buckets) + 1;

  /* Now, allocate a new bucket list. We need to keep track of the old
  bucket list, however, so that we can transfer its items and free it. */
  Item_List<unsigned, V>* Old_Buckets = Buckets;
  Buckets = new Item_List<unsigned, V>[N_Buckets];

  /* Now, we need to transfer the old buckets into the new bucket list. */
  for(unsigned i = 0; i < old_N_buckets; i++) {
    /* For each bucket, we 1-by-1 insert the items in its item list into
    the new buckets. */
    Item_Node<unsigned, V>* entry = Old_Buckets[i].Start;
    while(entry != NULL) {
      /* Insert entry into the new list of buckets */
      unsigned key = entry->getKey();
      V value = entry->getValue();
      (*this).insert(key, value);

      /* Update entry */
      entry = entry->getNext();
    } // while(entry != NULL) {
  } // for(unsigned i = 0; i < old_N_buckets; i++) {

  /* The buckets should now be transfered. Free the old bucket list. */
  delete [] Old_Buckets;
} // void Hash_Table<V>::resize() {



template <typename V>
void Hash_Table<V>::insert(unsigned key, V value) {
  // First, calculate the key of the hash
  unsigned bucket_index = Hash(key);

  // Now, add the new key-value pair into the selected bucket.
  Buckets[bucket_index].put(key, value);

  /* Finally, check if the length of that bucket's list is longer than the
  maximum list lenght. If so then resize.  */
  if(Buckets[bucket_index].GetLength() > Max_List_Length) { (*this).resize(); }
} // void Hash_Table<V>::insert(unsigned key, V value) {



template <typename V>
void Hash_Table<V>::remove(unsigned key) {
  // Calculate the bucket index.
  unsigned bucket_index = Hash(key);

  // Remove the item with the specified key from the selected bucket
  Buckets[bucket_index].remove(key);
} // void Hash_Table<V>::remove(unsigned key) {



template <typename V>
V Hash_Table<V>::search(unsigned key) const {
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
} // V Hash_Table<V>::search(unsigned key) const {

#endif
