#include <cstdint>
#include <unordered_map>

#include <delta_crdt/pn_counter.hh>

#include <gtest/gtest.h>

TEST(PositiveNegativeCounter, SyncByDelta) {
  crdt::pn_counter replica1(1);
  crdt::pn_counter replica2(2);

  crdt::pn_counter replica3(3);
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

TEST(PositiveNegativeCounter, SyncPositiveNegativeReplicas) {
  crdt::pn_counter replica1(1);
  crdt::pn_counter replica2(2);

  auto positive_delta = ++replica1;
  /* accamulate deltas */
  auto temp_delta = ++replica1;
  positive_delta.merge(temp_delta);
  replica2.merge(positive_delta);

  auto negative_delta = --replica1;
  replica2.merge(negative_delta);

  auto delta_from_replica2 = --replica2;
  replica1.merge(delta_from_replica2);

  EXPECT_EQ(replica1, replica2);
}

TEST(PositiveNegativeCounter, Associative) {
  crdt::pn_counter replica1(1);
  crdt::pn_counter replica2(2);
  crdt::pn_counter replica3(3);

  ++replica1;

  ++replica2;
  ++replica2;

  ++replica3;
  ++replica3;
  --replica3;

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);
  replica1.merge(replica3);

  replica2.merge(replica3);
  replica1_snapshot.merge(replica2);

  EXPECT_EQ(replica1, replica1_snapshot);
}

TEST(PositiveNegativeCounter, Commutative) {
  crdt::pn_counter replica1(1);
  crdt::pn_counter replica2(2);

  ++replica1;

  ++replica2;
  ++replica2;
  --replica2;

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);

  replica2.merge(replica1_snapshot);

  EXPECT_EQ(replica1, replica2);
}

TEST(PositiveNegativeCounter, Idempotent) {
  crdt::pn_counter replica1(1);

  ++replica1;

  crdt::pn_counter replica1_snapshot(1);

  ++replica1_snapshot;

  replica1.merge(replica1_snapshot);

  EXPECT_EQ(replica1, replica1_snapshot);
}
