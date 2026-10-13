#include <cstdint>
#include <unordered_map>

#include <delta_crdt/causal_counter.hh>

#include <gtest/gtest.h>

TEST(CausalCounter, SyncByDelta) {
  crdt::causal_counter replica1(1);
  crdt::causal_counter replica2(2);

  crdt::causal_counter replica3(3);
  replica3.merge(replica2);

  auto delta = (replica1 += 10UL);
  replica1.merge(replica2 += 1UL);
  replica2.merge(delta);

  replica3.merge(replica1);

  EXPECT_EQ(replica1.read(), replica2.read());
  EXPECT_EQ(replica1.read(), replica3.read());
  EXPECT_EQ(replica1.read(), 11UL);
  EXPECT_EQ(replica1, replica2);
  EXPECT_EQ(replica2, replica3);
  EXPECT_EQ(replica1, replica3);
}

TEST(CausalCounter, Associative) {
  crdt::causal_counter replica1(1);
  crdt::causal_counter replica2(2);
  crdt::causal_counter replica3(3);

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
  crdt::causal_counter replica1(1);
  crdt::causal_counter replica2(2);

  ++replica1;

  ++replica2;
  ++replica2;

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);

  replica2.merge(replica1_snapshot);

  EXPECT_EQ(replica1, replica2);
};

TEST(CausalCounter, Idempotent) {
  crdt::causal_counter replica1(1);

  ++replica1;

  crdt::causal_counter replica1_snapshot(1);

  ++replica1_snapshot;

  replica1.merge(replica1_snapshot);

  EXPECT_EQ(replica1, replica1_snapshot);
}

TEST(CausalCounter, ReplacementRemovesPreviouslyReplicatedContribution) {
  crdt::causal_counter sender(1);
  crdt::causal_counter receiver(2);

  const auto first = sender += 10;
  receiver.merge(first);
  const auto replacement = sender += 5;
  receiver.merge(replacement);
  receiver.merge(first);

  EXPECT_EQ(sender.read(), 15);
  EXPECT_EQ(receiver.read(), 15);
  EXPECT_EQ(sender, receiver);
}

TEST(CausalCounter, SupportsNegativeValues) {
  crdt::causal_counter replica(1);

  const auto delta = replica -= 3;

  EXPECT_EQ(replica.read(), -3);
  EXPECT_EQ(delta.read(), -3);
}
