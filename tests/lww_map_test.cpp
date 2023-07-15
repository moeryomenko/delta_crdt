#include <cstdint>
#include <string>

#include <delta_crdt/lwwmap.hh>

#include <gtest/gtest.h>

using lww_map = crdt::lwwmap<std::uint64_t, std::string>;

TEST(LastWriteWinMap, SyncByDelta) {
  lww_map replica1(1);
  lww_map replica2(2);

  lww_map replica3(2);
  replica3.merge(replica2);

  auto delta = replica1.insert(10UL, std::string{"value 10"});
  delta.merge(replica1.insert(10UL, std::string{"value 11"}));
  replica2.merge(delta);

  replica3.merge(replica1);

  EXPECT_EQ(replica1[10UL], std::string{"value 11"});
  EXPECT_TRUE(replica2.contains(10UL));
  EXPECT_TRUE(replica3.contains(10UL));
  EXPECT_EQ(replica2[10UL], std::string{"value 11"});
  EXPECT_EQ(replica3[10UL], std::string{"value 11"});

  EXPECT_EQ(replica1, replica2);
  EXPECT_EQ(replica2, replica3);
  EXPECT_EQ(replica1, replica3);
}

TEST(LastWriteWinMap, Associative) {
  lww_map replica1(1);
  lww_map replica2(2);
  lww_map replica3(3);

  replica1.insert(11UL, std::string{"value 11"});
  replica2.insert(12UL, std::string{"value 12"});
  replica3.insert(13UL, std::string{"value 13"});

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);
  replica1.merge(replica3);

  replica2.merge(replica3);
  replica1_snapshot.merge(replica2);

  EXPECT_TRUE(replica1.contains(11UL));
  EXPECT_TRUE(replica1.contains(12UL));
  EXPECT_TRUE(replica1.contains(13UL));
  EXPECT_TRUE(replica1_snapshot.contains(11UL));
  EXPECT_TRUE(replica1_snapshot.contains(12UL));
  EXPECT_TRUE(replica1_snapshot.contains(13UL));
  EXPECT_EQ(replica1, replica1_snapshot);
}

TEST(LastWriteWinMap, Commutative) {
  lww_map replica1(1);
  lww_map replica2(2);

  replica1.insert(11UL, std::string{"value 11"});
  replica2.insert(12UL, std::string{"value 12"});

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);

  replica2.merge(replica1_snapshot);

  EXPECT_TRUE(replica1.contains(11UL));
  EXPECT_TRUE(replica2.contains(11UL));
  EXPECT_TRUE(replica1.contains(12UL));
  EXPECT_TRUE(replica2.contains(12UL));
  EXPECT_EQ(replica1, replica2);
}

TEST(LastWriteWinMap, Idempotent) {
  lww_map replica1(1);

  replica1.insert(11UL, std::string{"value 11"});

  auto replica1_snapshot = replica1;

  replica1.merge(replica1_snapshot);

  replica1_snapshot.merge(replica1);

  EXPECT_TRUE(replica1.contains(11UL));
  EXPECT_TRUE(replica1_snapshot.contains(11UL));
  EXPECT_EQ(replica1, replica1_snapshot);
}
