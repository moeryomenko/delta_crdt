#include <string>
#include <assert.h>

#include <delta_crdt/lwwmap.hh>

auto main() -> int {
  crdt::lwwmap<std::string, std::string> simple_replicated_map(/* replica id */ 1);
  crdt::lwwmap<std::string, std::string> second_replicated_map(/* replica id */ 2);

  auto delta = simple_replicated_map.insert("key", "value");
  second_replicated_map.merge(delta);

  assert(second_replicated_map.contains("key") && "second replica must contains key 'key'");
}
