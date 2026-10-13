#include <cstdint>
#include <string>

#include <delta_crdt/awormap.hh>
#include <delta_crdt/lwwreg.hh>

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

using awor_map = crdt::awor_map<std::uint64_t, std::string>;

TEST(AddWinObservedRemoveMap, SyncByDelta) {
  awor_map replica1(kOne);
  awor_map replica2(kTwo);

  awor_map replica3(kTwo);
  replica3.merge(replica2);

  auto delta = replica1.insert(kTen, std::string{"value 10"});
  replica2.merge(delta);

  replica3.merge(replica1);

  EXPECT_TRUE(replica1.contains(kTen));
  EXPECT_TRUE(replica2.contains(kTen));
  EXPECT_TRUE(replica3.contains(kTen));
  EXPECT_EQ(replica1, replica2);
  EXPECT_EQ(replica2, replica3);
  EXPECT_EQ(replica1, replica3);
}

TEST(AddWinObservedRemoveMap, Associative) {
  awor_map replica1(kOne);
  awor_map replica2(kTwo);
  awor_map replica3(kThree);

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

TEST(AddWinObservedRemoveMap, Commutative) {
  awor_map replica1(kOne);
  awor_map replica2(kTwo);

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

TEST(AddWinObservedRemoveMap, Idempotent) {
  awor_map replica1(kOne);

  replica1.insert(kEleven, std::string{"value 11"});

  auto replica1_snapshot = replica1;

  replica1.merge(replica1_snapshot);

  replica1_snapshot.merge(replica1);

  EXPECT_TRUE(replica1.contains(kEleven));
  EXPECT_TRUE(replica1_snapshot.contains(kEleven));
  EXPECT_EQ(replica1, replica1_snapshot);
}

TEST(AddWinObservedRemoveMap, CRDTTypeComposition) {
  crdt::awor_map<int, crdt::lwwreg<std::string>> replica1(kOne);
  crdt::awor_map<int, crdt::lwwreg<std::string>> replica2(kTwo);

  auto delta1 =
      replica1.insert({kTen, crdt::lwwreg<std::string>(kOne, "test")});
  auto delta2 =
      replica2.insert({kTen, crdt::lwwreg<std::string>(kTwo, "test1")});
  replica2.merge(delta1);
  replica1.merge(delta2);

  EXPECT_EQ(replica1.read(kTen).value().read(), std::string{"test1"});
  EXPECT_EQ(replica1.read(kTen), replica2.read(kTen));

  delta2 = replica2.erase(kTen);
  replica1.merge(delta2);
  delta1 =
      replica1.insert({kTen, crdt::lwwreg<std::string>(kOne, "new value")});
  replica2.merge(delta1);

  EXPECT_EQ(replica2.read(kTen).value().read(), std::string{"new value"});
  EXPECT_EQ(replica1.read(kTen), replica2.read(kTen));
}

TEST(AddWinObservedRemoveMap, ConcurrentPlainValuesUseMaximumValue) {
  awor_map first(kOne);
  awor_map second(kTwo);
  awor_map left(kThree);
  awor_map right(kFour);

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

TEST(AddWinObservedRemoveMap, SequentialDeltasCarryTheEffectiveValue) {
  awor_map sender(kOne);
  awor_map receiver(kTwo);

  const auto first_delta = sender.insert(kTen, "omega");
  const auto second_delta = sender.insert(kTen, "alpha");

  receiver.merge(second_delta);
  receiver.merge(first_delta);

  EXPECT_EQ(sender.read(kTen), std::optional<std::string>{"omega"});
  EXPECT_EQ(receiver.read(kTen), sender.read(kTen));
  EXPECT_EQ(receiver, sender);
}
