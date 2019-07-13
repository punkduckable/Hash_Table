#include <cstdlib>
#include <iostream>
#include "HashTable.cxx"

// Unit testing stuff
#define CATCH_CONFIG_MAIN
#include "catch.hpp"


TEST_CASE("Bucket tests!", "[Bucket]") {
  /* First, create some buckets */
  Bucket<unsigned, double> B1;
  Bucket<unsigned, double> B2;

  /* Make sure that the buckets are empty and not searchable */
  REQUIRE( B1.getStatus() == BUCKET_STATUS::EMPTY_SINCE_START );
  REQUIRE_THROWS( B2.getKey() );
  REQUIRE_THROWS( B1.getValue() );

  /* Now, insert some items into the buckets. */
  unsigned key1 = 29, key2 = 320;
  double value1 = 2392.32, value2 = 5802.2939;
  B1.setKey(key1);
  B1.setValue(value1);
  B2.setKey(key2);
  B2.setValue(value2);

  /* Now, make sure that the new key/values are accessible and the status' have
  been updated. */
  REQUIRE( B1.getKey() == key1 );
  REQUIRE( B2.getValue() == value2 );
  REQUIRE( B1.getValue() == value1 );
  REQUIRE( B1.getStatus() == BUCKET_STATUS::FULL );
  REQUIRE( B2.getStatus() == BUCKET_STATUS::FULL );

  /* Now, empty a bucket and make sure that they're not accessible (and the
  bucket status gets update) */
  B1.Empty();
  REQUIRE( B1.getStatus() == BUCKET_STATUS::EMPTY_SINCE_REMOVAL );
  REQUIRE_THROWS( B1.getKey() );
  REQUIRE_THROWS( B1.getValue() );
} // TEST_CASE("Bucket tests!", "[Bucket]") {



TEST_CASE("Hash Table tests!", "[Hash_Table]") {
  /* First, let's make a hashtable of doubles. We'll use the default number of
  buckets for now. */
  Hash_Table<double> H{};

  /* Let's try searching for some items. Since the table is empty, all of these
  attempts should throw an exception */
  for(unsigned i = 0; i < 10; i++) {
    unsigned random_key = rand();
    REQUIRE_THROWS( H.search(random_key) );
  } // for(unsigned i = 0; i < 10; i++) {

  /* Now let's try inserting some items into the table and seeing that the
  table is able to find them  */
  unsigned key1 = 52, key2 = 47;
  double value1 = 1.203, value2 = 29392.392;
  H.insert(key1, value1);
  H.insert(key2, value2);
  REQUIRE( H.search(key1) == value1 );
  REQUIRE( H.search(key2) == value2 );
  REQUIRE_THROWS( H.search(0) );

  /* Now let's make sure that the table can handle collissions */
  unsigned key3 = 0, key4 = 11;
  double value3 = 129.293, value4 = 5802.29;
  H.insert(key3, value3);
  H.insert(key4, value4);
  REQUIRE( H.search(key3) == value3 );
  REQUIRE( H.search(key4) == value4 );

  /* Now, test that the table can update a particular key/value pair. */
  double value5 = 123.456;
  H.insert(key4, value5);
  REQUIRE( H.search(key4) == value5 );


  /* Now check that we can remove items from the table */
  H.remove(key4);
  REQUIRE_THROWS( H.search(key4) );

  printf("Old (before resizing) table:\n");
  std::cout << H << "\n";

  /* Now, let's resize the hash table and check that we can still get 1, 2, and
  3 using keys 1, 2, and 3, respectivly. Recall that we just removed key 4. */
  H.resize();
  REQUIRE( H.search(key1) == value1 );
  REQUIRE( H.search(key2) == value2 );
  REQUIRE( H.search(key3) == value3 );

  printf("New (after resizing) table:\n");
  std::cout << H << "\n";
} // TEST_CASE("Hash Table tests!", "[Hash_Table]") {



TEST_CASE("More Hash Table tests!", "[Hash_Table][More]") {
  /* First, let's make a hashtable. This table should have 11 buckets (the
  default) */
  Hash_Table<double> H{};

  /* Now, add some items to the hash table and check that they can be found. */
  unsigned key1 = 0, key2 = 11;
  double value1 = 123.456, value2 = 78.90;
  H.insert(key1, value1);
  H.insert(key2, value2);
  REQUIRE( H.search(key2) == value2 );

  /* Now, remove the item with key1, make sure that it's gone (and key2 can
  still be found) and then insert a new item with key2. */
  H.remove(key1);
  REQUIRE_THROWS( H.search(key1) );
  REQUIRE( H.search(key2) == value2 );
  H.insert(key2, value1);
  REQUIRE( H.search(key2) == value1 );

  /* There may now be multiple buckets with key key2. Let's remove key2 and
  check that key2 can no longer be searched for. */
  H.remove(key2);
  REQUIRE_THROWS( H.search(key2) );
} // TEST_CASE("Hash Table tests!", "[Hash_Table]") {
