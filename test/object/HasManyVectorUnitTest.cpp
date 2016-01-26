//
// Created by sascha on 31.12.15.
//

#include "HasManyVectorUnitTest.hpp"

#include "object/object_store.hpp"

using namespace oos;
using namespace hasmanyvector;

HasManyVectorUnitTest::HasManyVectorUnitTest()
  : unit_test("vector", "has many vector unit tests")
{
  add_test("join", std::bind(&HasManyVectorUnitTest::test_join_table, this), "test vector join table");
  add_test("const_iterator", std::bind(&HasManyVectorUnitTest::test_const_iterator, this), "test vector const iterator");
  add_test("int", std::bind(&HasManyVectorUnitTest::test_integer, this), "test vector of ints");
}

HasManyVectorUnitTest::~HasManyVectorUnitTest() { }

void HasManyVectorUnitTest::initialize() {}

void HasManyVectorUnitTest::finalize() {}

void HasManyVectorUnitTest::test_join_table()
{
  object_store store;

  store.attach<item>("item");
  store.attach<owner>("owner");

  prototype_iterator i = store.find("owner_item");
  UNIT_ASSERT_TRUE(i != store.end(), "iterator shouldn't be end");

  UNIT_ASSERT_EQUAL("owner_item", i->type(), "type should be 'owner_item'");

  object_ptr<owner> o = store.insert(new owner("hans"));

  UNIT_ASSERT_EQUAL("hans", o->name, "name should be 'hans'");

  object_ptr<item> i1 = store.insert(new item("i1"));

  UNIT_ASSERT_EQUAL("i1", i1->name, "name should be 'i1'");

  o->items.push_back(i1);

  UNIT_ASSERT_EQUAL(1U, o->items.size(), "size should be 1 (one)");

  o->items.push_back(new item("i2"));

  UNIT_ASSERT_EQUAL(2U, o->items.size(), "size should be 2 (two)");
}

void HasManyVectorUnitTest::test_const_iterator()
{
  many_ints mi;

  mi.ints.push_back(7);
  mi.ints.push_back(1);
  mi.ints.insert(mi.ints.begin(), 6);

  many_ints::int_vector_t::const_iterator first = mi.ints.begin();
  many_ints::int_vector_t::iterator i = mi.ints.end();
  many_ints::int_vector_t::const_iterator last = i;

  UNIT_ASSERT_FALSE(first == last, "first and last must not be equal");
  UNIT_ASSERT_TRUE(first == mi.ints.begin(), "first must be equal begin()");
  UNIT_ASSERT_TRUE(last == mi.ints.end(), "last must be equal end()");

  ++first;

  UNIT_ASSERT_EQUAL(*first, 7, "value of first must be '7'");

  first++;

  UNIT_ASSERT_EQUAL(*first, 1, "value of first must be '1'");

  first--;

  UNIT_ASSERT_EQUAL(*first, 7, "value of first must be '7'");

  --first;

  UNIT_ASSERT_EQUAL(*first, 6, "value of first must be '6'");
}

void HasManyVectorUnitTest::test_erase()
{
  object_store store;

  store.attach<item>("item");
  store.attach<owner>("owner");

  object_ptr<owner> o = store.insert(new owner("hans"));
  object_ptr<item> i1 = store.insert(new item("i1"));

  UNIT_ASSERT_EQUAL("i1", i1->name, "name should be 'i1'");

  o->items.push_back(i1);

  UNIT_ASSERT_EQUAL(1U, o->items.size(), "size should be 1 (one)");

  o->items.push_back(new item("i2"));

  owner::item_vector_t::iterator i = o->items.begin();

  o->items.erase(i);
}

void HasManyVectorUnitTest::test_integer()
{
  object_store store;

  store.attach<many_ints>("many_ints");

  object_ptr<many_ints> mi = store.insert(new many_ints);

  UNIT_ASSERT_EQUAL(mi->ints.size(), 0UL, "pointer vector is not empty");

  for (int i = 0; i < 20; ++i) {
    mi->ints.push_back(i);
  }

  UNIT_ASSERT_EQUAL(mi->ints.size(), 20UL, "pointer vector has invalid size");

  many_ints::int_vector_t::iterator i = mi->ints.begin();

  UNIT_ASSERT_EQUAL(*i, 0, "item is invalid");

  i++;

  UNIT_ASSERT_EQUAL(*i, 1, "item is invalid");

  many_ints::int_vector_t::item_ptr iptr = i.relation_item();

  UNIT_ASSERT_EQUAL(iptr->value(), 1, "item is invalid");
}
