#include <boost/ut.hpp>

#include <cstdint>
#include <unordered_map>

#include <delta_crdt/causal_counter.hh>

auto main() -> int {
  using namespace boost::ut;

  "sync by delta equals sync by full state"_test = [] {
    crdt::causal_counter replica1(1);
    crdt::causal_counter replica2(2);

    crdt::causal_counter replica3(3);
    replica3.merge(replica2);

    auto delta = (replica1 += 10UL);
    replica1.merge(replica2 += 1UL);
    replica2.merge(delta);

    replica3.merge(replica1);

    expect(replica1.read() == replica2.read());
    expect(replica1.read() == replica3.read());
    expect(replica1.read() == 11UL);
  };

  "associative"_test = [] {
    crdt::causal_counter replica1(1);
    crdt::causal_counter replica2(2);
    crdt::causal_counter replica3(3);

    ++replica1;

    ++replica2;
    ++replica2;

    ++replica3;
    ++replica3;

    auto replica1_snapshot = replica1;

    replica1.merge(replica2);
    replica1.merge(replica3);

    replica2.merge(replica3);
    replica1_snapshot.merge(replica2);

    expect(replica1.read() == replica1_snapshot.read());
  };

  "commutative"_test = [] {
    crdt::causal_counter replica1(1);
    crdt::causal_counter replica2(2);

    ++replica1;

    ++replica2;
    ++replica2;

    auto replica1_snapshot = replica1;

    replica1.merge(replica2);

    replica2.merge(replica1_snapshot);

    expect(replica1.read() == replica2.read());
  };

  "idempotent"_test = [] {
    crdt::causal_counter replica1(1);

    ++replica1;

    crdt::causal_counter replica1_snapshot(1);

    ++replica1_snapshot;

    replica1.merge(replica1_snapshot);

    expect(replica1.read() == replica1_snapshot.read());
  };
}
