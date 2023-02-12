#include <boost/ut.hpp>

#include <cstdint>
#include <unordered_map>

#include <delta_crdt/gcounter.hh>

auto main() -> int {
  using namespace boost::ut;

  "sync by delta equals sync by full state"_test = [] {
    crdt::gcounter replica1(1);
    crdt::gcounter replica2(2);

    crdt::gcounter replica3(3);
    replica3.merge(replica2);

    auto delta = ++replica1;
    replica2.merge(delta);

    replica3.merge(replica1);

    expect(replica1.read() == replica2.read());
    expect(replica1.read() == replica3.read());

    expect(replica1 == replica2);
    expect(replica2 == replica3);
    expect(replica1 == replica3);
  };

  "associative"_test = [] {
    crdt::gcounter replica1(1);
    crdt::gcounter replica2(2);
    crdt::gcounter replica3(3);

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

    expect(replica1 == replica1_snapshot);
  };

  "commutative"_test = [] {
    crdt::gcounter replica1(1);
    crdt::gcounter replica2(2);

    ++replica1;

    ++replica2;
    ++replica2;

    auto replica1_snapshot = replica1;

    replica1.merge(replica2);

    replica2.merge(replica1_snapshot);

    expect(replica1 == replica2);
  };

  "idempotent"_test = [] {
    crdt::gcounter replica1(1);

    ++replica1;

    crdt::gcounter replica1_snapshot(1);

    ++replica1_snapshot;

    replica1.merge(replica1_snapshot);

    expect(replica1 == replica1_snapshot);
  };
}
