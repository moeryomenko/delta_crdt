#include <cstdint>

#include <delta_crdt/dot.hh>

#include <gtest/gtest.h>

namespace {
constexpr auto kOne = 1;
constexpr auto kTwo = 2;
constexpr auto kThree = 3;
constexpr auto kFour = 4;
constexpr auto kFive = 5;
constexpr auto kSeven = 7;
constexpr auto kEight = 8;
constexpr auto kTen = 10;
}  // namespace

namespace {

TEST(DotContext, CompactionPreservesClockAndClosesCloudGaps) {
  crdt::dot_context<> context;
  context.clock.vector.insert_or_assign(kOne, kFive);
  context.clock.vector.insert_or_assign(kTwo, kOne);
  context.insert({kTwo, kThree})
      .insert({kTwo, kTwo})
      .insert({kThree, kFour})
      .compact();

  EXPECT_EQ(context.clock.vector.at(kOne), kFive);
  EXPECT_EQ(context.clock.vector.at(kTwo), kThree);
  EXPECT_EQ(context.dot_cloud.size(), kOne);
  EXPECT_TRUE(context.dot_cloud.contains({kThree, kFour}));
}

TEST(DotContext, ContainsOnlyObservedCounters) {
  crdt::dot_context<> context;
  context.clock.vector.insert_or_assign(kOne, kFour);
  context.insert({kTwo, kSeven});

  EXPECT_TRUE(context.contains({kOne, kOne}));
  EXPECT_TRUE(context.contains({kOne, kFour}));
  EXPECT_FALSE(context.contains({kOne, kFive}));
  EXPECT_TRUE(context.contains({kTwo, kSeven}));
  EXPECT_FALSE(context.contains({kTwo, kEight}));
}

TEST(DotContext, NextDotDoesNotReuseCloudDots) {
  crdt::dot_context<> context;
  context.clock.vector.insert_or_assign(kSeven, kOne);
  context.insert({kSeven, kThree});

  EXPECT_EQ(context.next_dot(kSeven), (crdt::dot{kSeven, kTwo}));
  EXPECT_EQ(context.next_dot(kSeven), (crdt::dot{kSeven, kFour}));
}

TEST(DotKernel, DelayedEntryDoesNotResurrectRemovedDot) {
  crdt::dot_kernel<std::uint64_t> source;
  crdt::dot_kernel<std::uint64_t> receiver;

  const auto insertion = source.insert(kOne, kTen);
  receiver.merge(insertion);
  const auto removal = source.erase({kOne, kOne});

  receiver.merge(removal);
  receiver.merge(insertion);

  EXPECT_TRUE(receiver.entries.empty());
  EXPECT_TRUE(receiver.context.contains({kOne, kOne}));
}

}  // namespace
