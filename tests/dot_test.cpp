#include <cstdint>

#include <delta_crdt/dot.hh>

#include <gtest/gtest.h>

namespace {

TEST(DotContext, CompactionPreservesClockAndClosesCloudGaps) {
  crdt::dot_context<> context;
  context.clock.vector[1] = 5;
  context.clock.vector[2] = 1;
  context.insert({2, 3}).insert({2, 2}).insert({3, 4}).compact();

  EXPECT_EQ(context.clock.vector.at(1), 5);
  EXPECT_EQ(context.clock.vector.at(2), 3);
  EXPECT_EQ(context.dot_cloud.size(), 1);
  EXPECT_TRUE(context.dot_cloud.contains({3, 4}));
}

TEST(DotContext, ContainsOnlyObservedCounters) {
  crdt::dot_context<> context;
  context.clock.vector[1] = 4;
  context.insert({2, 7});

  EXPECT_TRUE(context.contains({1, 1}));
  EXPECT_TRUE(context.contains({1, 4}));
  EXPECT_FALSE(context.contains({1, 5}));
  EXPECT_TRUE(context.contains({2, 7}));
  EXPECT_FALSE(context.contains({2, 8}));
}

TEST(DotContext, NextDotDoesNotReuseCloudDots) {
  crdt::dot_context<> context;
  context.clock.vector[7] = 1;
  context.insert({7, 3});

  EXPECT_EQ(context.next_dot(7), (crdt::dot{7, 2}));
  EXPECT_EQ(context.next_dot(7), (crdt::dot{7, 4}));
}

TEST(DotKernel, DelayedEntryDoesNotResurrectRemovedDot) {
  crdt::dot_kernel<std::uint64_t> source;
  crdt::dot_kernel<std::uint64_t> receiver;

  const auto insertion = source.insert(1, 10);
  receiver.merge(insertion);
  const auto removal = source.erase({1, 1});

  receiver.merge(removal);
  receiver.merge(insertion);

  EXPECT_TRUE(receiver.entries.empty());
  EXPECT_TRUE(receiver.context.contains({1, 1}));
}

} // namespace
