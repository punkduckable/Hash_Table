#include <string>
#include <iostream>
#include <stdio.h>


////////////////////////////////////////////////////////////////////////////////
// Item

template<typename K, typename V>
struct Item {
  K key;
  V value;
}; // struct Item {





////////////////////////////////////////////////////////////////////////////////
// Bucket

// Bucket status'
enum class BUCKET_STATUS{EMPTY_SINCE_START, EMPTY_SINCE_REMOVAL, FULL};



// Bucket exception classes
class Bucket_Exception {
  private:
    std::string Error_Message;
  public:
    Bucket_Exception(const char* Error_Message): Error_Message(Error_Message) {}
    const char* what() const { return Error_Message.c_str(); }
}; // class Bucket_Exception {

class Empty_Bucket: public Bucket_Exception {
  public:
    Empty_Bucket(const char* Error_Message) : Bucket_Exception(Error_Message) {}
}; // class Empty_Bucket: public Bucket_Exception {



template <typename K, typename V>
class Bucket {
  private:
    Item<K, V> Item;
    BUCKET_STATUS status;

  public:
    // Constructor, destructor
    Bucket(): status(BUCKET_STATUS::EMPTY_SINCE_START) {}
    ~Bucket() {}


    // methods to modify get, set the Item
    void setKey(K key) {
      Item.key = key;
      status = BUCKET_STATUS::FULL;
    } // void setKey(K key) {
    K getKey() {
      /* Only return the key if the bucket is if full. Otherwise, throw an
      exception */
      if(status == BUCKET_STATUS::FULL) { return Item.key; }
      else {
        throw Empty_Bucket("Empty Bucket Error: Thrown by Bucket::getKey().\n"
                          "This bucket is empty, it has no key.\n");
      } // else {
    } // K getKey() {

    void setValue(V Value) {
      Item.value = Value;
      status = BUCKET_STATUS::FULL;
    } // void setValue(V Value) {
    V getValue() {
      /* only return the value of the bucket is full. Otherwise, throw an
      exception */
      if(status == BUCKET_STATUS::FULL) { return Item.value; }
      else {
        throw Empty_Bucket("Empty Bucket Error: Thrown by Bucket::getValue().\n"
                           "This bucket is empty, it has no value.\n");
      } // else {
    } // V getValue() {

    // Modify the status of the bucket
    BUCKET_STATUS getStatus() { return status; }
    void Empty() { status = BUCKET_STATUS::EMPTY_SINCE_REMOVAL; }
};





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

class Full_Table: public Hash_Table_Exception {
  public:
    Full_Table(const char * Error_Message) : Hash_Table_Exception(Error_Message) {}
}; // class Full_Table: public Hash_Table_Exception {

class Invalid_Key: public Hash_Table_Exception {
  public:
    Invalid_Key(const char * Error_Message) : Hash_Table_Exception(Error_Message) {}
}; // class Invalid_Key: public Hash_Table_Exception {



template <typename V>
class Hash_Table {
  private:
    unsigned N_Buckets;
    Bucket<unsigned, V>* Buckets;

    // Hashing function
    unsigned Hash(unsigned key) const { return (key % N_Buckets); }

    /* Function to Update bucket index (for probing). Hash_Value is the
    hashed key value, i is the number of times that the bucket index has been
    updated. */
    unsigned Calculate_Bucket_Index(unsigned Hash_Value, unsigned num_searched) const {
      unsigned bucket_index = Hash_Value + num_searched;
      if(bucket_index == N_Buckets) { bucket_index = 0; }
      return bucket_index;
    } // unsigned Calculate_Bucket_Index(unsigned Hash_Value, unsigned num_searched) const {

    // Delete the implicit = operator and copy constructor methods
    Hash_Table(const Hash_Table &) = delete;
    Hash_Table& operator=(const Hash_Table &) = delete;

  public:
    // Constructor, destructor
    Hash_Table(unsigned N_Buckets = 11) {
      /* I require that there are at least 11 buckets (I just picked a prime
      number to prevent collisions) */
      if(N_Buckets < 11) { N_Buckets = 11; }

      Hash_Table::N_Buckets = N_Buckets;
      Buckets = new Bucket<unsigned, V>[N_Buckets];
    } // Hash_Table(unsigned N_Buckets = 11) {
    ~Hash_Table() { delete [] Buckets; }


    // Insert an item into the table.
    void insert(unsigned key, V value) {
      // First, calculate the hash index
      unsigned hash_index = Hash(key);

      /* Initialize num_searched and bucket index */
      unsigned num_searched = 0;
      unsigned bucket_index = Calculate_Bucket_Index(hash_index, num_searched);

      /* Loop through the buckets until we either find one that isn't full or
      whose key matches the specified key. If such a bucket is found then
      we set that bucket's key/value

      We keep track of how many buckets we've looked through. If that number
      excedes the number of buckets then no item with the specified key is in
      the table. */
      while(num_searched < N_Buckets ) {
        /* First, check if the current bucket is full. If it is, then check if
        its key matches the specified key. If so, update that bucket. */
        if(Buckets[bucket_index].getStatus() == BUCKET_STATUS::FULL) {
          if(Buckets[bucket_index].getKey() == key) {
            Buckets[bucket_index].setKey(key);
            Buckets[bucket_index].setValue(value);
            return;
          } // if(Buckets[bucket_index].getKey() == key) {
        } // if(Buckets[bucket_index].getStatus() == BUCKET_STATUS::FULL) {

        /* If the current bucket is not full then populate that bucket using the
        specified key and value.*/
        else { // if(Buckets[bucket_index].getStatus() != BUCKET_STATUS::FULL) {
          Buckets[bucket_index].setKey(key);
          Buckets[bucket_index].setValue(value);
          return;
        } // else {

        /* If we're here then it means that the current bucket was full but its
        key did not match the specified key. Calculate the new bucket index. */
        num_searched++;
        bucket_index = Calculate_Bucket_Index(hash_index, num_searched);
      } // while(num_searched < N_Buckets ) {

      /* If we get here then it means that the entire list was searched and
      every bucket was full. We need to throw an exception. */
      throw Full_Table("Full Table Error: Thrown by Hash_Table::insert\n"
                       "This table is full. There is nowhere to insert a new item\n");
    } // void insert(unsigned key, V value) {


    // remove the value with the specified key from the table.
    void remove(unsigned key) {
      // Calculate the hash index.
      unsigned hash_index = Hash(key);

      /* Initialize num_searched and bucket index */
      unsigned num_searched = 0;
      unsigned bucket_index = Calculate_Bucket_Index(hash_index, num_searched);


      /* Now, cycle through the buckets until we find a bucket that has been
      empty since start. At each step, we check if the key of the current bucket
      matches the specified key. If it does, we empty that bucket.

      In theory, it is possible for more than one bucket to have the same key.
      To understand how, consider the following: there are three items: a, b,
      and c. Items b and c have the same key, all three items get hashed to
      the same bucket. Item a is inserted into bucket 0. Item b is then inserted
      it collides with item a, so it gets put in bucket 1. Item a is then
      removed. Item c is then inserted. Since bucket 0 is now empty, it gets
      inserted into bucket 0. Now buckets 0 and 1 have items with the same key.
      This is fine, however, since the search function will return the value of
      the item in bucket 0 (the newest one).

      Because of this, we have to remove ALL buckets that have the specified
      key. Importantly, however, as soon as we hit a bucket that has been
      empty since start, we know that there no other buckets that can have
      the specified key (becuase they would have been inserted in an earlier
      bucket) so we're done.

      We keep track of how many buckets we've looked through. If that number
      excedes the number of buckets then no item with the specified key is in
      the table. */
      while(Buckets[bucket_index].getStatus() != BUCKET_STATUS::EMPTY_SINCE_START && num_searched < N_Buckets) {
        /* If the current buckets' key matches the specified key then empty
        that bucket. */
        if(Buckets[bucket_index].getKey() == key) { Buckets[bucket_index].Empty(); }

        /* Otherwise, calculate the new bucket index. */
        num_searched++;
        bucket_index = Calculate_Bucket_Index(hash_index, num_searched);
      } // while(Buckets[bucket_index].getStatus() != BUCKET_STATUS::EMPTY_SINCE_START && num_searched < N_Buckets) {
    } // void remove(unsigned key) {


    /* Find the value of the item with the specified key. Throws an exception
    if no item with the specified key can be found */
    V search(unsigned key) const {
      // First, calculate the hash index
      unsigned hash_index = Hash(key);

      /* Initialize num_searched and the bucket index */
      unsigned num_searched = 0;
      unsigned bucket_index = Calculate_Bucket_Index(hash_index, num_searched);

      /* Now, try finding a bucket in the list whose key matches the specified
      key. If no such key can be found then we throw an exception.

      We keep track of how many buckets we've looked through. If that number
      excedes the number of buckets then no item with the specified key is in
      the table. */
      while(Buckets[bucket_index].getStatus() != BUCKET_STATUS::EMPTY_SINCE_START && num_searched < N_Buckets) {
        /* Check if the current bucket is full. If so, see if its key matches
        the specified key. If so then we have found our match! */
        if(Buckets[bucket_index].getStatus() == BUCKET_STATUS::FULL) {
          if(Buckets[bucket_index].getKey() == key) { return Buckets[bucket_index].getValue(); }
        } // if(Buckets[bucket_index].getStatus() == BUCKET_STATUS::FULL) {

        /* Otherwise, calculate the new bucket index */
        num_searched++;
        bucket_index = Calculate_Bucket_Index(hash_index, num_searched);
      } // while(Buckets[bucket_index].getStatus() != BUCKET_STATUS::EMPTY_SINCE_START && num_searched < N_Buckets) {

      /* If we get here then it means that no item in the table has the
      requested key. Throw an exception */
      char Error_Message_Buffer[500];
      sprintf(Error_Message_Buffer,
              "Invalid Key Error: Thrown by Hash_Table::search\n"
              "This hash table does not have an entry with key %d\n",
              key);
      throw Invalid_Key(Error_Message_Buffer);
    } // V search(unsigned key) const {


    // Printing method
    friend std::ostream & operator<<(std::ostream & out, const Hash_Table & Table) {
      unsigned N_Buckets = Table.N_Buckets;
      for(unsigned i = 0; i < N_Buckets; i++) {
        out << "Bucket " << i << " - ";

        if(Table.Buckets[i].getStatus() == BUCKET_STATUS::EMPTY_SINCE_START) {
          out << "Empty since start. " << std::endl;
        } // if(Table.Buckets[i].getStatus == BUCKET_STATUS::EMPTY_SINCE_START) {
        else if(Table.Buckets[i].getStatus() == BUCKET_STATUS::EMPTY_SINCE_REMOVAL) {
          out << "Empty since removal. " << std::endl;
        } // else if(Table.Buckets[i].getStatus == BUCKET_STATUS::EMPTY_SINCE_REMOVAL) {
        else if(Table.Buckets[i].getStatus() == BUCKET_STATUS::FULL) {
          out << "Full: " << "(" << Table.Buckets[i].getKey() << ", " << Table.Buckets[i].getValue() << ")" << std::endl;
        } // else if(Table.Buckets[i].getStatus == BUCKET_STATUS::FULL) {
      } // for(unsigned i = 0; i < N_Buckets; i++) {

      return out;
    } // friend std::ostream & operator<<(std::ostream & out, const Hash_Table & Table) {
}; // class Hash_Table {
