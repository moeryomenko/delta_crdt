#include <cstdint>
#include <string>

#include <delta_crdt/rwormap.hh>

#include <gtest/gtest.h>

using rwor_map = crdt::rwor_map<std::uint64_t, std::string>;

TEST(RemoveWinObservedRemoveMap, SyncByDelta) {
  rwor_map replica1(1);
  rwor_map replica2(2);

  rwor_map replica3(2);
  replica3.merge(replica2);

  auto delta = replica1.insert(10UL, std::string{"value 10"});
  delta.merge(replica1.insert(11UL, std::string{"value 11"}));
  replica1.merge(replica2.erase(11UL));
  replica2.merge(delta);

  replica3.merge(replica1);

  EXPECT_TRUE(replica1.contains(10UL));
  EXPECT_TRUE(replica2.contains(10UL));
  EXPECT_TRUE(replica3.contains(10UL));
  EXPECT_FALSE(replica1.contains(11UL));
  EXPECT_FALSE(replica2.contains(11UL));
  EXPECT_FALSE(replica3.contains(11UL));

  EXPECT_EQ(replica1, replica2);
  EXPECT_EQ(replica2, replica3);
  EXPECT_EQ(replica1, replica3);
}

TEST(RemoveWinObservedRemoveMap, Associative) {
  rwor_map replica1(1);
  rwor_map replica2(2);
  rwor_map replica3(3);

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

TEST(RemoveWinObservedRemoveMap, Commutative) {
  rwor_map replica1(1);
  rwor_map replica2(2);

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
};

TEST(RemoveWinObservedRemoveMap, Idempotent) {
  rwor_map replica1(1);

  replica1.insert(11UL, std::string{"value 11"});

  auto replica1_snapshot = replica1;

  replica1.merge(replica1_snapshot);

  replica1_snapshot.merge(replica1);

  EXPECT_TRUE(replica1.contains(11UL));
  EXPECT_TRUE(replica1_snapshot.contains(11UL));
  EXPECT_EQ(replica1, replica1_snapshot);
}
