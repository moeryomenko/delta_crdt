#include <cstdint>
#include <unordered_map>

#include <delta_crdt/causal_counter.hh>

#include <gtest/gtest.h>

namespace {
constexpr auto kOne = 1;
constexpr auto kTwo = 2;
constexpr auto kThree = 3;
constexpr auto kFive = 5;
constexpr auto kTen = 10;
constexpr auto kEleven = 11;
constexpr auto kFifteen = 15;
}  // namespace

TEST(CausalCounter, SyncByDelta) {
  crdt::causal_counter replica1(kOne);
  crdt::causal_counter replica2(kTwo);

  crdt::causal_counter replica3(kThree);
  replica3.merge(replica2);

  auto delta = (replica1 += kTen);
  replica1.merge(replica2 += kOne);
  replica2.merge(delta);

  replica3.merge(replica1);

  EXPECT_EQ(replica1.read(), replica2.read());
  EXPECT_EQ(replica1.read(), replica3.read());
  EXPECT_EQ(replica1.read(), kEleven);
  EXPECT_EQ(replica1, replica2);
  EXPECT_EQ(replica2, replica3);
  EXPECT_EQ(replica1, replica3);
}

TEST(CausalCounter, Associative) {
  crdt::causal_counter replica1(kOne);
  crdt::causal_counter replica2(kTwo);
  crdt::causal_counter replica3(kThree);

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

TEST(CausalCounter, Commutative) {
  crdt::causal_counter replica1(kOne);
  crdt::causal_counter replica2(kTwo);

  ++replica1;

  ++replica2;
  ++replica2;

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);

  replica2.merge(replica1_snapshot);

  EXPECT_EQ(replica1, replica2);
};

TEST(CausalCounter, Idempotent) {
  crdt::causal_counter replica1(kOne);

  ++replica1;

  crdt::causal_counter replica1_snapshot(kOne);

  ++replica1_snapshot;

  replica1.merge(replica1_snapshot);

  EXPECT_EQ(replica1, replica1_snapshot);
}

TEST(CausalCounter, ReplacementRemovesPreviouslyReplicatedContribution) {
  crdt::causal_counter sender(kOne);
  crdt::causal_counter receiver(kTwo);

  const auto first = sender += kTen;
  receiver.merge(first);
  const auto replacement = sender += kFive;
  receiver.merge(replacement);
  receiver.merge(first);

  EXPECT_EQ(sender.read(), kFifteen);
  EXPECT_EQ(receiver.read(), kFifteen);
  EXPECT_EQ(sender, receiver);
}

TEST(CausalCounter, SupportsNegativeValues) {
  crdt::causal_counter replica(kOne);

  const auto delta = replica -= kThree;

  EXPECT_EQ(replica.read(), -kThree);
  EXPECT_EQ(delta.read(), -kThree);
}
