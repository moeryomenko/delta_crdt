#include <cstdint>
#include <string>

#include <delta_crdt/awormap.hh>
#include <delta_crdt/lwwreg.hh>

#include <gtest/gtest.h>

using awor_map = crdt::awor_map<std::uint64_t, std::string>;

TEST(AddWinObservedRemoveMap, SyncByDelta) {
  awor_map replica1(1);
  awor_map replica2(2);

  awor_map replica3(2);
  replica3.merge(replica2);

  auto delta = replica1.insert(10UL, std::string{"value 10"});
  replica2.merge(delta);

  replica3.merge(replica1);

  EXPECT_TRUE(replica1.contains(10UL));
  EXPECT_TRUE(replica2.contains(10UL));
  EXPECT_TRUE(replica3.contains(10UL));
  EXPECT_EQ(replica1, replica2);
  EXPECT_EQ(replica2, replica3);
  EXPECT_EQ(replica1, replica3);
}

TEST(AddWinObservedRemoveMap, Associative) {
  awor_map replica1(1);
  awor_map replica2(2);
  awor_map replica3(3);

  replica1.insert(11UL, std::string{"value 11"});
  replica2.insert(12UL, std::string{"value 12"});
  replica3.insert(13UL, std::string{"value 13"});

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);
  replica1.merge(replica3);

  replica2.merge(replica3);
  replica1_snapshot.merge(replica2);

  EXPECT_TRUE(replica1.contains(11UL));
  EXPECT_TRUE(replica1.contains(12UL));
  EXPECT_TRUE(replica1.contains(13UL));
  EXPECT_TRUE(replica1_snapshot.contains(11UL));
  EXPECT_TRUE(replica1_snapshot.contains(12UL));
  EXPECT_TRUE(replica1_snapshot.contains(13UL));
  EXPECT_EQ(replica1, replica1_snapshot);
}

TEST(AddWinObservedRemoveMap, Commutative) {
  awor_map replica1(1);
  awor_map replica2(2);

  replica1.insert(11UL, std::string{"value 11"});
  replica2.insert(12UL, std::string{"value 12"});

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);

  replica2.merge(replica1_snapshot);

  EXPECT_TRUE(replica1.contains(11UL));
  EXPECT_TRUE(replica2.contains(11UL));
  EXPECT_TRUE(replica1.contains(12UL));
  EXPECT_TRUE(replica2.contains(12UL));
  EXPECT_EQ(replica1, replica2);
}

TEST(AddWinObservedRemoveMap, Idempotent) {
  awor_map replica1(1);

  replica1.insert(11UL, std::string{"value 11"});

  auto replica1_snapshot = replica1;

  replica1.merge(replica1_snapshot);

  replica1_snapshot.merge(replica1);

  EXPECT_TRUE(replica1.contains(11UL));
  EXPECT_TRUE(replica1_snapshot.contains(11UL));
  EXPECT_EQ(replica1, replica1_snapshot);
}

TEST(AddWinObservedRemoveMap, CRDTTypeComposition) {
  crdt::awor_map<int, crdt::lwwreg<std::string>> replica1(1UL);
  crdt::awor_map<int, crdt::lwwreg<std::string>> replica2(2UL);

  auto delta1 = replica1.insert({10, crdt::lwwreg<std::string>(1, "test")});
  auto delta2 = replica2.insert({10, crdt::lwwreg<std::string>(2, "test1")});
  replica2.merge(delta1);
  replica1.merge(delta2);

  EXPECT_EQ(replica1[10].value().read(), std::string{"test1"});
  EXPECT_EQ(replica1[10], replica2[10]);

  delta2 = replica2.erase(10);
  replica1.merge(delta2);
  delta1 = replica1.insert({10, crdt::lwwreg<std::string>(1, "new value")});
  replica2.merge(delta1);

  EXPECT_EQ(replica2[10].value().read(), std::string{"new value"});
  EXPECT_EQ(replica1[10], replica2[10]);
}
