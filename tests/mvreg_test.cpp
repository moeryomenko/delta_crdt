#include <boost/ut.hpp>

#include <cstdint>

#include <delta_crdt/mvreg.hh>

auto main() -> int {
  using namespace boost::ut;

  "sync by delta equals sync by full state"_test = [] {
    crdt::mvreg<std::uint64_t> replica1(1);
    crdt::mvreg<std::uint64_t> replica2(2);

    crdt::mvreg<std::uint64_t> replica3(2);
    replica3.merge(replica2);

    auto delta = replica1.set(10UL);
    delta.merge(replica1.set(12UL));
    replica2.merge(delta);

    replica3.merge(replica1);

    expect(replica1.read() == replica2.read());
    expect(replica1.read() == replica3.read());
  };

  "associative"_test = [] {
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

    expect(replica1.read() == replica1_snapshot.read());
  };

  "commutative"_test = [] {
    crdt::mvreg<std::uint64_t> replica1(1);
    crdt::mvreg<std::uint64_t> replica2(2);

    replica1.set(10UL);
    replica2.set(12UL);

    auto replica1_snapshot = replica1;

    replica1.merge(replica2);

    replica2.merge(replica1_snapshot);

    expect(replica1.read() == replica2.read());
  };

  "idempotent"_test = [] {
    crdt::mvreg<std::uint64_t> replica1(1);

    replica1.set(10UL);

    auto replica1_snapshot = replica1;

    replica1.merge(replica1_snapshot);

    expect(replica1.read() == replica1_snapshot.read());
  };
}
