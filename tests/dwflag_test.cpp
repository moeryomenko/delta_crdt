#include <cstdint>

#include <delta_crdt/dwflag.hh>

#include <gtest/gtest.h>

TEST(DisableWinFlag, SyncByDelta) {
  crdt::dwflag replica1(1);
  crdt::dwflag replica2(2);

  crdt::dwflag replica3(2);
  replica3.merge(replica2);

  auto delta = replica1.enable();
  replica2.merge(delta);

  replica3.merge(replica1);

  EXPECT_TRUE(replica1.is_enaled());
  EXPECT_TRUE(replica2.is_enaled());
  EXPECT_TRUE(replica3.is_enaled());

  replica2.merge(replica3.disable());
  replica1.merge(replica2);

  EXPECT_TRUE(replica1.is_disaled());
  EXPECT_TRUE(replica2.is_disaled());
  EXPECT_TRUE(replica3.is_disaled());

  EXPECT_EQ(replica1, replica2);
  EXPECT_EQ(replica2, replica3);
  EXPECT_EQ(replica1, replica3);
}

TEST(DisableWinFlag, Associative) {
  crdt::dwflag replica1(1);
  crdt::dwflag replica2(2);
  crdt::dwflag replica3(2);

  replica1.enable();
  replica2.enable();
  replica3.disable();

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);
  replica1.merge(replica3);

  replica2.merge(replica3);
  replica1_snapshot.merge(replica2);

  EXPECT_EQ(replica1, replica1_snapshot);
}

TEST(DisableWinFlag, Commutative) {
  crdt::dwflag replica1(1);
  crdt::dwflag replica2(2);

  replica1.enable();
  replica2.disable();

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);

  replica2.merge(replica1_snapshot);

  EXPECT_EQ(replica1, replica2);
}

TEST(DisableWinFlag, Idempotent) {
  crdt::dwflag replica1(1);

  replica1.enable();

  auto replica1_snapshot = replica1;

  replica1.merge(replica1_snapshot);

  EXPECT_EQ(replica1, replica1_snapshot);
}
