#include <cstdlib>
#include <iostream>
#include "HashTable.cxx"

// Unit testing stuff
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

// Test Item Node
TEST_CASE("Item Node tests", "[Item_Node]") {
  // First, make two Item Nodes.
  unsigned key1 = 1;
  unsigned key2 = 5;
  double value1 = 2.2;
  double value2 = 3.23;

  Item_Node<unsigned, double> One(key1, value1);
  Item_Node<unsigned, double> Two(key2, value2);

  // Now, check that they set up the values correctly
  REQUIRE(One.getKey() == key1);
  REQUIRE(One.getValue() == value1);
  REQUIRE(Two.getKey() == key2);
  REQUIRE(Two.getValue() == value2);

  // Now check that item nodes can update their next paramater as expected.
  REQUIRE(One.getNext() == NULL);
  One.setNext(&Two);
  Two.setNext(&One);
  REQUIRE(One.getNext() == &Two );
  REQUIRE(Two.getNext() == &One );

  // Now check that Item_Nodes can have their values updated
  double new_value = 23.232;
  One.setValue(new_value);
  REQUIRE(One.getValue() == new_value);
} // TEST_CASE("Item Node tests", "[Item_Node]") {



// Test Item List
TEST_CASE("Item List tests", "[Item_List]") {
  // let's make an item list.
  Item_List<unsigned, double> List;

  // Let's search for an item in the list. The list should be empty, so this
  // should throw an exception
  REQUIRE_THROWS( List.get(12) );

  /* Add an item, see that it's accessible, remove that item (so that the list
  is empty again) and check that the item is now inaccessible. */
  double value0 = 20392.20;
  List.put(1, value0);
  REQUIRE( List.get(1) == value0 );
  List.remove(1);
  REQUIRE_THROWS( List.get(1) );

  // Now, let's add some items and see that they're accessible
  double value1 = 2.332;
  double value2 = -1.293;
  List.put(12, value1);
  List.put(14, value2);
  REQUIRE( List.get(14) == value2 );
  REQUIRE( List.get(12) == value1 );
  //REQUIRE_THROWS( List.get(5) );

  // Now, update one of the values and see that it worked
  double value3 = 29392.32;
  List.put(12, value3);
  REQUIRE( List.get(12) == value3 );

  // Now, remove an item and make sure that it's inaccessible
  List.remove(12);
  REQUIRE_THROWS( List.get(12) );
} // TEST_CASE("Item List tests", "[Item_List]") {



void Print_Table(const Hash_Table<double> & H) { std::cout << H; }

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
  unsigned key3 = 0, key4 = 16;
  double value3 = 129.293, value4 = 5802.29;
  H.insert(key3, value3);
  H.insert(key4, value4);
  REQUIRE( H.search(key3) == value3 );
  REQUIRE( H.search(key4) == value4 );

  /* Now, test that the table can update a particular key/value pair. */
  double value5 = 123.456;
  H.insert(key4, value5);
  REQUIRE( H.search(key4) == value5 );

  // Just for fun, print out the table!
  std::cout << H;

  /* Now check that we can remove items from the table */
  H.remove(key4);
  REQUIRE_THROWS( H.search(key4) );
} // TEST_CASE("Hash Table tests!", "[Hash_Table]") {
