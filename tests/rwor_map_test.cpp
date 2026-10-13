#include <cstdint>
#include <string>

#include <delta_crdt/rwormap.hh>

#include <gtest/gtest.h>

namespace {
constexpr auto kOne = 1;
constexpr auto kTwo = 2;
constexpr auto kThree = 3;
constexpr auto kFour = 4;
constexpr auto kTen = 10;
constexpr auto kEleven = 11;
constexpr auto kTwelve = 12;
constexpr auto kThirteen = 13;
}  // namespace

using rwor_map = crdt::rwor_map<std::uint64_t, std::string>;

TEST(RemoveWinObservedRemoveMap, SyncByDelta) {
  rwor_map replica1(kOne);
  rwor_map replica2(kTwo);

  rwor_map replica3(kTwo);
  replica3.merge(replica2);

  auto delta = replica1.insert(kTen, std::string{"value 10"});
  delta.merge(replica1.insert(kEleven, std::string{"value 11"}));
  replica1.merge(replica2.erase(kEleven));
  replica2.merge(delta);

  replica3.merge(replica1);

  EXPECT_TRUE(replica1.contains(kTen));
  EXPECT_TRUE(replica2.contains(kTen));
  EXPECT_TRUE(replica3.contains(kTen));
  EXPECT_FALSE(replica1.contains(kEleven));
  EXPECT_FALSE(replica2.contains(kEleven));
  EXPECT_FALSE(replica3.contains(kEleven));

  EXPECT_EQ(replica1, replica2);
  EXPECT_EQ(replica2, replica3);
  EXPECT_EQ(replica1, replica3);
}

TEST(RemoveWinObservedRemoveMap, Associative) {
  rwor_map replica1(kOne);
  rwor_map replica2(kTwo);
  rwor_map replica3(kThree);

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

TEST(RemoveWinObservedRemoveMap, Commutative) {
  rwor_map replica1(kOne);
  rwor_map replica2(kTwo);

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
};

TEST(RemoveWinObservedRemoveMap, Idempotent) {
  rwor_map replica1(kOne);

  replica1.insert(kEleven, std::string{"value 11"});

  auto replica1_snapshot = replica1;

  replica1.merge(replica1_snapshot);

  replica1_snapshot.merge(replica1);

  EXPECT_TRUE(replica1.contains(kEleven));
  EXPECT_TRUE(replica1_snapshot.contains(kEleven));
  EXPECT_EQ(replica1, replica1_snapshot);
}

TEST(RemoveWinObservedRemoveMap, ConcurrentPlainValuesUseMaximumValue) {
  rwor_map first(kOne);
  rwor_map second(kTwo);
  rwor_map left(kThree);
  rwor_map right(kFour);

  const auto first_delta = first.insert(kTen, "alpha");
  const auto second_delta = second.insert(kTen, "omega");

  left.merge(first_delta);
  left.merge(second_delta);
  right.merge(second_delta);
  right.merge(first_delta);

  EXPECT_EQ(left.read(kTen), std::optional<std::string>{"omega"});
  EXPECT_EQ(right.read(kTen), std::optional<std::string>{"omega"});
  EXPECT_EQ(left, right);
}

TEST(RemoveWinObservedRemoveMap, SequentialDeltasCarryTheEffectiveValue) {
  rwor_map sender(kOne);
  rwor_map receiver(kTwo);

  const auto first_delta = sender.insert(kTen, "omega");
  const auto second_delta = sender.insert(kTen, "alpha");

  receiver.merge(second_delta);
  receiver.merge(first_delta);

  EXPECT_EQ(sender.read(kTen), std::optional<std::string>{"omega"});
  EXPECT_EQ(receiver.read(kTen), sender.read(kTen));
  EXPECT_EQ(receiver, sender);
}
