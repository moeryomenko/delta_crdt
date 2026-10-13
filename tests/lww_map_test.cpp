#include <cstdint>
#include <string>

#include <delta_crdt/lwwmap.hh>

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

using lww_map = crdt::lwwmap<std::uint64_t, std::string>;

TEST(LastWriteWinMap, SyncByDelta) {
  lww_map replica1(kOne);
  lww_map replica2(kTwo);

  lww_map replica3(kTwo);
  replica3.merge(replica2);

  auto delta = replica1.insert(kTen, std::string{"value 10"});
  delta.merge(replica1.insert(kTen, std::string{"value 11"}));
  replica2.merge(delta);

  replica3.merge(replica1);

  EXPECT_EQ(replica1.read(kTen), std::string{"value 11"});
  EXPECT_TRUE(replica2.contains(kTen));
  EXPECT_TRUE(replica3.contains(kTen));
  EXPECT_EQ(replica2.read(kTen), std::string{"value 11"});
  EXPECT_EQ(replica3.read(kTen), std::string{"value 11"});

  EXPECT_EQ(replica1, replica2);
  EXPECT_EQ(replica2, replica3);
  EXPECT_EQ(replica1, replica3);
}

TEST(LastWriteWinMap, Associative) {
  lww_map replica1(kOne);
  lww_map replica2(kTwo);
  lww_map replica3(kThree);

  replica1.insert(kEleven, std::string{"value 11"});
  replica2.insert(kTwelve, std::string{"value 12"});
  replica3.insert(kThirteen, std::string{"value 13"});

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);
  replica1.merge(replica3);

  replica2.merge(replica3);
  replica1_snapshot.merge(replica2);

  EXPECT_TRUE(replica1.contains(kEleven));
  EXPECT_TRUE(replica1.contains(kTwelve));
  EXPECT_TRUE(replica1.contains(kThirteen));
  EXPECT_TRUE(replica1_snapshot.contains(kEleven));
  EXPECT_TRUE(replica1_snapshot.contains(kTwelve));
  EXPECT_TRUE(replica1_snapshot.contains(kThirteen));
  EXPECT_EQ(replica1, replica1_snapshot);
}

TEST(LastWriteWinMap, Commutative) {
  lww_map replica1(kOne);
  lww_map replica2(kTwo);

  replica1.insert(kEleven, std::string{"value 11"});
  replica2.insert(kTwelve, std::string{"value 12"});

  auto replica1_snapshot = replica1;

  replica1.merge(replica2);

  replica2.merge(replica1_snapshot);

  EXPECT_TRUE(replica1.contains(kEleven));
  EXPECT_TRUE(replica2.contains(kEleven));
  EXPECT_TRUE(replica1.contains(kTwelve));
  EXPECT_TRUE(replica2.contains(kTwelve));
  EXPECT_EQ(replica1, replica2);
}

TEST(LastWriteWinMap, Idempotent) {
  lww_map replica1(kOne);

  replica1.insert(kEleven, std::string{"value 11"});

  auto replica1_snapshot = replica1;

  replica1.merge(replica1_snapshot);

  replica1_snapshot.merge(replica1);

  EXPECT_TRUE(replica1.contains(kEleven));
  EXPECT_TRUE(replica1_snapshot.contains(kEleven));
  EXPECT_EQ(replica1, replica1_snapshot);
}
