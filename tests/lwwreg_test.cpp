#include <cstdint>

#include <delta_crdt/lwwreg.hh>

#include <gtest/gtest.h>

namespace {
constexpr auto kOne = 1;
constexpr auto kTwo = 2;
constexpr auto kThree = 3;
constexpr auto kFour = 4;
constexpr auto kTen = 10;
constexpr auto kTwelve = 12;
constexpr auto kThirteen = 13;
constexpr auto kTwenty = 20;
}  // namespace

TEST(LastWriteWinRegister, SyncByDelta) {
  crdt::lwwreg<std::uint64_t> replica1(kOne);
  crdt::lwwreg<std::uint64_t> replica2(kTwo);

  crdt::lwwreg<std::uint64_t> replica3(kTwo);
  replica3.merge(replica2);

  auto delta = replica1.set(kTen);
  delta.merge(replica1.set(kTwelve));
  replica2.merge(delta);

  replica3.merge(replica1);

  EXPECT_EQ(replica1.read(), kTwelve);
  EXPECT_EQ(replica1.read(), replica2.read());
  EXPECT_EQ(replica1.read(), replica3.read());

  EXPECT_EQ(replica1, replica2);
  EXPECT_EQ(replica2, replica3);
  EXPECT_EQ(replica1, replica3);
}

TEST(LastWriteWinRegister, Associative) {
  crdt::lwwreg<std::uint64_t> replica1(kOne);
  crdt::lwwreg<std::uint64_t> replica2(kTwo);
  crdt::lwwreg<std::uint64_t> replica3(kThree);

  replica1.set(kTen);
  replica2.set(kTwelve);
  replica3.set(kThirteen);

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);
  replica1.merge(replica3);

  replica2.merge(replica3);
  replica1_snapshot.merge(replica2);

  EXPECT_EQ(replica1, replica1_snapshot);
}

TEST(LastWriteWinRegister, Commutative) {
  crdt::lwwreg<std::uint64_t> replica1(kOne);
  crdt::lwwreg<std::uint64_t> replica2(kTwo);

  replica1.set(kTen);
  auto replica1_snapshot = replica1;
  replica2.set(kTwelve);

  replica1.merge(replica2);

  replica2.merge(replica1_snapshot);

  EXPECT_EQ(replica1, replica2);
}

TEST(LastWriteWinRegister, Idempotent) {
  crdt::lwwreg<std::uint64_t> replica1(kOne);

  replica1.set(kTen);

  auto replica1_snapshot = replica1;

  replica1.merge(replica1_snapshot);

  EXPECT_EQ(replica1, replica1_snapshot);
}

TEST(LastWriteWinRegister, ResolvesEqualLogicalTimesByReplicaID) {
  crdt::lwwreg<std::uint64_t> first(kOne);
  crdt::lwwreg<std::uint64_t> second(kTwo);
  crdt::lwwreg<std::uint64_t> left(kThree);
  crdt::lwwreg<std::uint64_t> right(kFour);

  const auto first_delta = first.set(kTen);
  const auto second_delta = second.set(kTwenty);

  left.merge(first_delta);
  left.merge(second_delta);
  right.merge(second_delta);
  right.merge(first_delta);

  EXPECT_EQ(left.read(), kTwenty);
  EXPECT_EQ(right.read(), kTwenty);
  EXPECT_EQ(left, right);
}
