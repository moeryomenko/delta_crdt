#include <boost/ut.hpp>

#include <cstdint>

#include <awormap.hh>
#include <string>

auto main() -> int {
  using namespace boost::ut;

  using awor_map = crdt::awor_map<std::uint64_t, std::string>;

  "sync by delta equals sync by full state"_test = [] {
    awor_map replica1(1);
    awor_map replica2(2);

    awor_map replica3(2);
    replica3.merge(replica2);

    auto delta = replica1.insert(10UL, std::string{"value 10"});
    replica2.merge(delta);

    replica3.merge(replica1);

    expect(replica2.contains(10UL));
    expect(replica3.contains(10UL));
  };

  "associative"_test = [] {
    awor_map replica1(1);
    awor_map replica2(2);
    awor_map replica3(3);

    replica1.insert(11UL, std::string{"value 11"});
    replica2.insert(12UL, std::string{"value 12"});
    replica3.insert(13UL, std::string{"value 13"});

    auto replica1_snapshot = replica1;

    replica1.merge(replica2);
    replica1.merge(replica3);

    replica2.merge(replica3);
    replica1_snapshot.merge(replica2);

	expect(replica1.contains(11UL));
	expect(replica1.contains(12UL));
	expect(replica1.contains(13UL));
	expect(replica1_snapshot.contains(11UL));
	expect(replica1_snapshot.contains(12UL));
	expect(replica1_snapshot.contains(13UL));
  };

  "commutative"_test = [] {
    awor_map replica1(1);
    awor_map replica2(2);

    replica1.insert(11UL, std::string{"value 11"});
    replica2.insert(12UL, std::string{"value 12"});

    auto replica1_snapshot = replica1;

    replica1.merge(replica2);

    replica2.merge(replica1_snapshot);

	expect(replica1.contains(11UL));
	expect(replica2.contains(11UL));
	expect(replica1.contains(12UL));
	expect(replica2.contains(12UL));
  };

  "idempotent"_test = [] {
    awor_map replica1(1);

    replica1.insert(11UL, std::string{"value 11"});

    auto replica1_snapshot = replica1;

    replica1.merge(replica1_snapshot);

    replica1_snapshot.merge(replica1);

	expect(replica1.contains(11UL));
	expect(replica1_snapshot.contains(11UL));
  };
}
