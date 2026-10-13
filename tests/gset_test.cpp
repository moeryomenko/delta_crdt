#include <cstdint>

#include <delta_crdt/gset.hh>

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

TEST(GrowOnlySet, SyncByDelta) {
  crdt::gset<std::uint64_t> replica1(kOne);
  crdt::gset<std::uint64_t> replica2(kTwo);

  crdt::gset<std::uint64_t> replica3(kTwo);
  replica3.merge(replica2);

  auto delta = replica1.add(kTen);
  replica2.merge(delta);

  replica3.merge(replica1);

  EXPECT_TRUE(replica2.contains(kTen));
  EXPECT_TRUE(replica3.contains(kTen));

  EXPECT_EQ(replica1, replica2);
  EXPECT_EQ(replica2, replica3);
  EXPECT_EQ(replica1, replica3);
};

TEST(GrowOnlySet, Associative) {
  crdt::gset<std::uint64_t> replica1(kOne);
  crdt::gset<std::uint64_t> replica2(kTwo);
  crdt::gset<std::uint64_t> replica3(kThree);

  replica1.add(kTen);
  replica1.add(kEleven);
  replica2.add(kTwelve);
  replica3.add(kThirteen);

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);
  replica1.merge(replica3);

  replica2.merge(replica3);
  replica1_snapshot.merge(replica2);

  EXPECT_EQ(replica1, replica1_snapshot);
}

TEST(GrowOnlySet, Commutative) {
  crdt::gset<std::uint64_t> replica1(kOne);
  crdt::gset<std::uint64_t> replica2(kTwo);

  replica1.add(kTen);
  replica1.add(kEleven);
  replica2.add(kTwelve);

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);

  replica2.merge(replica1_snapshot);

  EXPECT_EQ(replica1, replica2);
}

TEST(GrowOnlySet, Idempotent) {
  crdt::gset<std::uint64_t> replica1(kOne);

  replica1.add(kTen);
  replica1.add(kEleven);

  auto replica1_snapshot = replica1;

  replica1.merge(replica1_snapshot);

  EXPECT_EQ(replica1, replica1_snapshot);
}
