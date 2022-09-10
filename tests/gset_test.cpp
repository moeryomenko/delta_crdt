#include <boost/ut.hpp>

#include <cstdint>

#include <delta_crdt/gset.hh>

auto main() -> int {
  using namespace boost::ut;

  "sync by delta equals sync by full state"_test = [] {
    crdt::gset<std::uint64_t> replica1(1);
    crdt::gset<std::uint64_t> replica2(2);

    crdt::gset<std::uint64_t> replica3(2);
    replica3.merge(replica2);

    auto delta = replica1.add(10UL);
    replica2.merge(delta);

    replica3.merge(replica1);

    expect(replica2.contains(10UL));
    expect(replica3.contains(10UL));
  };

  "associative"_test = [] {
    crdt::gset<std::uint64_t> replica1(1);
    crdt::gset<std::uint64_t> replica2(2);
    crdt::gset<std::uint64_t> replica3(3);

    replica1.add(10UL);
    replica1.add(11UL);
    replica2.add(12UL);
    replica3.add(13UL);

    auto replica1_snapshot = replica1;

    replica1.merge(replica2);
    replica1.merge(replica3);

    replica2.merge(replica3);
    replica1_snapshot.merge(replica2);

    expect(replica1 == replica1_snapshot);
  };

  "commutative"_test = [] {
    crdt::gset<std::uint64_t> replica1(1);
    crdt::gset<std::uint64_t> replica2(2);

    replica1.add(10UL);
    replica1.add(11UL);
    replica2.add(12UL);

    auto replica1_snapshot = replica1;

    replica1.merge(replica2);

    replica2.merge(replica1_snapshot);

    expect(replica1 == replica2);
  };

  "idempotent"_test = [] {
    crdt::gset<std::uint64_t> replica1(1);

    replica1.add(10UL);
    replica1.add(11UL);

    auto replica1_snapshot = replica1;

    replica1.merge(replica1_snapshot);

    expect(replica1 == replica1_snapshot);
  };
}
