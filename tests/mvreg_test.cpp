#include <cstdint>

#include <delta_crdt/mvreg.hh>

#include <gtest/gtest.h>

TEST(MultiVersionRegister, SyncByDelta) {
  crdt::mvreg<std::uint64_t> replica1(1);
  crdt::mvreg<std::uint64_t> replica2(2);

  crdt::mvreg<std::uint64_t> replica3(2);
  replica3.merge(replica2);

  auto delta = replica1.set(10UL);
  delta.merge(replica1.set(12UL));
  replica2.merge(delta);

  replica3.merge(replica1);

  EXPECT_EQ(replica1.read(), replica2.read());
  EXPECT_EQ(replica1.read(), replica3.read());

  EXPECT_EQ(replica1, replica2);
  EXPECT_EQ(replica2, replica3);
  EXPECT_EQ(replica1, replica3);
}

TEST(MultiVersionRegister, Associative) {
  crdt::mvreg<std::uint64_t> replica1(1);
  crdt::mvreg<std::uint64_t> replica2(2);
  crdt::mvreg<std::uint64_t> replica3(3);

  replica1.set(10UL);
  replica2.set(12UL);
  replica3.set(13UL);

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);
  replica1.merge(replica3);

  replica2.merge(replica3);
  replica1_snapshot.merge(replica2);

  EXPECT_EQ(replica1, replica1_snapshot);
}

TEST(MultiVersionRegister, Commutative) {
  crdt::mvreg<std::uint64_t> replica1(1);
  crdt::mvreg<std::uint64_t> replica2(2);

  replica1.set(10UL);
  replica2.set(12UL);

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);

  replica2.merge(replica1_snapshot);

  EXPECT_EQ(replica1.read(), replica2.read());
  EXPECT_EQ(replica1, replica2);
}

TEST(MultiVersionRegister, Idempotent) {
  crdt::mvreg<std::uint64_t> replica1(1);

  replica1.set(10UL);

  auto replica1_snapshot = replica1;

  replica1.merge(replica1_snapshot);

  EXPECT_EQ(replica1, replica1_snapshot);
}
