#include <cstdint>
#include <unordered_map>

#include <delta_crdt/gcounter.hh>

#include <gtest/gtest.h>

namespace {
constexpr auto kOne = 1;
constexpr auto kTwo = 2;
constexpr auto kThree = 3;
}  // namespace

TEST(GrowOnlyCounter, SyncByDelta) {
  crdt::gcounter replica1(kOne);
  crdt::gcounter replica2(kTwo);

  crdt::gcounter replica3(kThree);
  replica3.merge(replica2);

  auto delta = ++replica1;
  replica2.merge(delta);

  replica3.merge(replica1);

  EXPECT_EQ(replica1.read(), replica2.read());
  EXPECT_EQ(replica1.read(), replica3.read());

  EXPECT_EQ(replica1, replica2);
  EXPECT_EQ(replica2, replica3);
  EXPECT_EQ(replica1, replica3);
}

TEST(GrowOnlyCounter, Associative) {
  crdt::gcounter replica1(kOne);
  crdt::gcounter replica2(kTwo);
  crdt::gcounter replica3(kThree);

  ++replica1;

  ++replica2;
  ++replica2;

  ++replica3;
  ++replica3;

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);
  replica1.merge(replica3);

  replica2.merge(replica3);
  replica1_snapshot.merge(replica2);

  EXPECT_EQ(replica1, replica1_snapshot);
}

TEST(GrowOnlyCounter, Commutative) {
  crdt::gcounter replica1(kOne);
  crdt::gcounter replica2(kTwo);

  ++replica1;

  ++replica2;
  ++replica2;

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);

  replica2.merge(replica1_snapshot);

  EXPECT_EQ(replica1, replica2);
}

TEST(GrowOnlyCounter, Idempotent) {
  crdt::gcounter replica1(kOne);

  ++replica1;

  crdt::gcounter replica1_snapshot(kOne);

  ++replica1_snapshot;

  replica1.merge(replica1_snapshot);

  EXPECT_EQ(replica1, replica1_snapshot);
}
