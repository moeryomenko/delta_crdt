#include <cstdint>

#include <delta_crdt/aworset.hh>

#include <gtest/gtest.h>

TEST(AddWinObservedRemoveSet, SyncByDelta) {
  crdt::awor_set<std::uint64_t> replica1(1);
  crdt::awor_set<std::uint64_t> replica2(2);

  crdt::awor_set<std::uint64_t> replica3(2);
  replica3.merge(replica2);

  auto delta = replica1.insert(10UL);
  delta.merge(replica1.insert(11UL));
  delta.merge(replica1.insert(12UL));

  replica1.merge(replica2.erase(12UL));
  replica2.merge(delta);

  replica3.merge(replica1);

  EXPECT_EQ(replica1, replica2);
  EXPECT_EQ(replica2, replica3);
  EXPECT_EQ(replica1, replica3);
}

TEST(AddWinObservedRemoveSet, Associative) {
  crdt::awor_set<std::uint64_t> replica1(1);
  crdt::awor_set<std::uint64_t> replica2(2);
  crdt::awor_set<std::uint64_t> replica3(3);

  replica1.insert(10UL);
  replica1.insert(11UL);
  replica2.insert(12UL);
  replica3.insert(13UL);

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);
  replica1.merge(replica3);

  replica2.merge(replica3);
  replica1_snapshot.merge(replica2);

  EXPECT_EQ(replica1, replica1_snapshot);
}

TEST(AddWinObservedRemoveSet, Commutative) {
  crdt::awor_set<std::uint64_t> replica1(1);
  crdt::awor_set<std::uint64_t> replica2(2);

  replica1.insert(10UL);
  replica1.insert(11UL);
  replica2.insert(12UL);

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);

  replica2.merge(replica1_snapshot);

  EXPECT_EQ(replica1, replica2);
}

TEST(AddWinObservedRemoveSet, Idempotent) {
  crdt::awor_set<std::uint64_t> replica1(1);

  replica1.insert(10UL);
  replica1.insert(11UL);

  auto replica1_snapshot = replica1;

  replica1.merge(replica1_snapshot);

  EXPECT_EQ(replica1, replica1_snapshot);
}
