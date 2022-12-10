#include <boost/ut.hpp>

#include <cstdint>

#include <delta_crdt/aworset.hh>

auto main() -> int {
  using namespace boost::ut;

  "sync by delta equals sync by full state"_test = [] {
    crdt::awor_set<std::uint64_t> replica1(1);
    crdt::awor_set<std::uint64_t> replica2(2);

    crdt::awor_set<std::uint64_t> replica3(2);
    replica3.merge(replica2);

    auto delta = replica1.insert(10UL);
    delta.merge(replica1.insert(11UL));
    delta.merge(replica1.insert(12UL));
    delta.merge(replica1.erase(12UL));
    replica2.merge(delta);

    replica3.merge(replica1);

    expect(replica1.values() == std::set<std::uint64_t>{10UL, 11UL});
    expect(replica2.values() == std::set<std::uint64_t>{10UL, 11UL});
    expect(replica3.values() == std::set<std::uint64_t>{10UL, 11UL});
  };

  "associative"_test = [] {
    crdt::awor_set<std::uint64_t> replica1(1);
    crdt::awor_set<std::uint64_t> replica2(2);
    crdt::awor_set<std::uint64_t> replica3(3);

    replica1.insert(10UL);
    replica1.insert(11UL);
    replica2.insert(12UL);
    replica3.insert(13UL);

    auto replica1_snapshot = replica1;

    replica1.merge(replica2);
    replica1.merge(replica3);

    replica2.merge(replica3);
    replica1_snapshot.merge(replica2);

    expect(replica1 == replica1_snapshot);
  };

  "commutative"_test = [] {
    crdt::awor_set<std::uint64_t> replica1(1);
    crdt::awor_set<std::uint64_t> replica2(2);

    replica1.insert(10UL);
    replica1.insert(11UL);
    replica2.insert(12UL);

    auto replica1_snapshot = replica1;

    replica1.merge(replica2);

    replica2.merge(replica1_snapshot);

    expect(replica1 == replica2);
  };

  "idempotent"_test = [] {
    crdt::awor_set<std::uint64_t> replica1(1);

    replica1.insert(10UL);
    replica1.insert(11UL);

    auto replica1_snapshot = replica1;

    replica1.merge(replica1_snapshot);

    expect(replica1 == replica1_snapshot);
  };
}
