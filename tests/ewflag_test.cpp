#include <cstdint>

#include <delta_crdt/ewflag.hh>

#include <gtest/gtest.h>

namespace {
constexpr auto kOne = 1;
constexpr auto kTwo = 2;
}  // namespace

TEST(EnableWinFlag, SyncByDelta) {
  crdt::ewflag replica1(kOne);
  crdt::ewflag replica2(kTwo);

  crdt::ewflag replica3(kTwo);
  replica3.merge(replica2);

  auto delta = replica1.enable();
  replica2.merge(delta);

  replica3.merge(replica1);

  EXPECT_TRUE(replica1.is_enabled());
  EXPECT_TRUE(replica2.is_enabled());
  EXPECT_TRUE(replica3.is_enabled());

  replica2.merge(replica3.disable());
  replica1.merge(replica2);

  EXPECT_TRUE(replica1.is_disabled());
  EXPECT_TRUE(replica2.is_disabled());
  EXPECT_TRUE(replica3.is_disabled());

  EXPECT_EQ(replica1, replica2);
  EXPECT_EQ(replica2, replica3);
  EXPECT_EQ(replica1, replica3);
}

TEST(EnableWinFlag, Associative) {
  crdt::ewflag replica1(kOne);
  crdt::ewflag replica2(kTwo);
  crdt::ewflag replica3(kTwo);

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

TEST(EnableWinFlag, Commutative) {
  crdt::ewflag replica1(kOne);
  crdt::ewflag replica2(kTwo);

  replica1.enable();
  replica2.disable();

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);

  replica2.merge(replica1_snapshot);

  EXPECT_EQ(replica1, replica2);
}

TEST(EnableWinFlag, Idempotent) {
  crdt::ewflag replica1(kOne);

  replica1.enable();

  auto replica1_snapshot = replica1;

  replica1.merge(replica1_snapshot);

  EXPECT_EQ(replica1, replica1_snapshot);
}
