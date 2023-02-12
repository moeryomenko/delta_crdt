#include <boost/ut.hpp>

#include <cstdint>

#include <delta_crdt/dwflag.hh>

auto main() -> int {
  using namespace boost::ut;

  "sync by delta equals sync by full state"_test = [] {
    crdt::dwflag replica1(1);
    crdt::dwflag replica2(2);

    crdt::dwflag replica3(2);
    replica3.merge(replica2);

    auto delta = replica1.enable();
    replica2.merge(delta);

    replica3.merge(replica1);

    expect(replica1.is_enaled());
    expect(replica2.is_enaled());
    expect(replica3.is_enaled());

    replica2.merge(replica3.disable());
    replica1.merge(replica2);

    expect(replica1.is_disaled());
    expect(replica2.is_disaled());
    expect(replica3.is_disaled());

    expect(replica1 == replica2);
    expect(replica2 == replica3);
    expect(replica1 == replica3);
  };

  "associative"_test = [] {
    crdt::dwflag replica1(1);
    crdt::dwflag replica2(2);
    crdt::dwflag replica3(2);

    replica1.enable();
    replica2.enable();
    replica3.disable();

    auto replica1_snapshot = replica1;

    replica1.merge(replica2);
    replica1.merge(replica3);

    replica2.merge(replica3);
    replica1_snapshot.merge(replica2);

    expect(replica1 == replica1_snapshot);
  };

  "commutative"_test = [] {
    crdt::dwflag replica1(1);
    crdt::dwflag replica2(2);

    replica1.enable();
    replica2.disable();

    auto replica1_snapshot = replica1;

    replica1.merge(replica2);

    replica2.merge(replica1_snapshot);

    expect(replica1 == replica2);
  };

  "idempotent"_test = [] {
    crdt::dwflag replica1(1);

    replica1.enable();

    auto replica1_snapshot = replica1;

    replica1.merge(replica1_snapshot);

    expect(replica1 == replica1_snapshot);
  };
}
