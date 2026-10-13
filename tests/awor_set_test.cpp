#include <cstdint>

#include <delta_crdt/aworset.hh>

#include <gtest/gtest.h>

namespace {
constexpr auto kOne = 1;
constexpr auto kTwo = 2;
constexpr auto kThree = 3;
constexpr auto kTen = 10;
constexpr auto kEleven = 11;
constexpr auto kTwelve = 12;
constexpr auto kThirteen = 13;
}  // namespace

TEST(AddWinObservedRemoveSet, SyncByDelta) {
  crdt::awor_set<std::uint64_t> replica1(kOne);
  crdt::awor_set<std::uint64_t> replica2(kTwo);

  crdt::awor_set<std::uint64_t> replica3(kTwo);
  replica3.merge(replica2);

  auto delta = replica1.insert(kTen);
  delta.merge(replica1.insert(kEleven));
  delta.merge(replica1.insert(kTwelve));

  replica1.merge(replica2.erase(kTwelve));
  replica2.merge(delta);

  replica3.merge(replica1);

  EXPECT_EQ(replica1, replica2);
  EXPECT_EQ(replica2, replica3);
  EXPECT_EQ(replica1, replica3);
}

TEST(AddWinObservedRemoveSet, Associative) {
  crdt::awor_set<std::uint64_t> replica1(kOne);
  crdt::awor_set<std::uint64_t> replica2(kTwo);
  crdt::awor_set<std::uint64_t> replica3(kThree);

  replica1.insert(kTen);
  replica1.insert(kEleven);
  replica2.insert(kTwelve);
  replica3.insert(kThirteen);

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);
  replica1.merge(replica3);

  replica2.merge(replica3);
  replica1_snapshot.merge(replica2);

  EXPECT_EQ(replica1, replica1_snapshot);
}

TEST(AddWinObservedRemoveSet, Commutative) {
  crdt::awor_set<std::uint64_t> replica1(kOne);
  crdt::awor_set<std::uint64_t> replica2(kTwo);

  replica1.insert(kTen);
  replica1.insert(kEleven);
  replica2.insert(kTwelve);

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);

  replica2.merge(replica1_snapshot);

  EXPECT_EQ(replica1, replica2);
}

TEST(AddWinObservedRemoveSet, Idempotent) {
  crdt::awor_set<std::uint64_t> replica1(kOne);

  replica1.insert(kTen);
  replica1.insert(kEleven);

  auto replica1_snapshot = replica1;

  replica1.merge(replica1_snapshot);

  EXPECT_EQ(replica1, replica1_snapshot);
}
