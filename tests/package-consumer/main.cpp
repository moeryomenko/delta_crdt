#include <delta_crdt/awormap.hh>
#include <delta_crdt/aworset.hh>
#include <delta_crdt/causal_counter.hh>
#include <delta_crdt/dwflag.hh>
#include <delta_crdt/ewflag.hh>
#include <delta_crdt/gcounter.hh>
#include <delta_crdt/gset.hh>
#include <delta_crdt/lwwmap.hh>
#include <delta_crdt/lwwreg.hh>
#include <delta_crdt/mvreg.hh>
#include <delta_crdt/pn_counter.hh>
#include <delta_crdt/rwormap.hh>
#include <delta_crdt/rworset.hh>

int main() {
  crdt::gcounter grow_only(1);
  crdt::gset<int> grow_set(2);
  crdt::pn_counter positive_negative(3);
  crdt::causal_counter causal(4);
  crdt::ewflag enable_wins(5);
  crdt::dwflag disable_wins(6);
  crdt::awor_set<int> add_wins_set(7);
  crdt::rwor_set<int> remove_wins_set(8);
  crdt::awor_map<int, int> add_wins_map(9);
  crdt::rwor_map<int, int> remove_wins_map(10);
  crdt::lwwmap<int, int> last_write_map(11);
  crdt::lwwreg<int> last_write_register(12);
  crdt::mvreg<int> multi_value_register(13);

  ++grow_only;
  grow_set.add(1);
  ++positive_negative;
  ++causal;
  enable_wins.enable();
  disable_wins.disable();
  add_wins_set.insert(1);
  remove_wins_set.insert(1);
  add_wins_map.insert(1, 1);
  remove_wins_map.insert(1, 1);
  last_write_map.insert(1, 1);
  last_write_map.insert(1, 2);
  last_write_register.set(1);
  multi_value_register.set(1);

  return grow_only.read() == 1 && grow_set.contains(1) &&
                 positive_negative.read() == 1 && causal.read() == 1 &&
                 enable_wins.is_enabled() &&
                 disable_wins.is_disabled() && add_wins_set.contains(1) &&
                 remove_wins_set.contains(1) && add_wins_map.contains(1) &&
                 remove_wins_map.contains(1) && last_write_map[1] == 2 &&
                 last_write_register.read() == 1 &&
                 multi_value_register.read().contains(1) &&
                 grow_only == grow_only && grow_set == grow_set &&
                 positive_negative == positive_negative && causal == causal &&
                 enable_wins == enable_wins && disable_wins == disable_wins &&
                 add_wins_set == add_wins_set &&
                 remove_wins_set == remove_wins_set &&
                 add_wins_map == add_wins_map &&
                 remove_wins_map == remove_wins_map &&
                 last_write_map == last_write_map &&
                 last_write_register == last_write_register &&
                 multi_value_register == multi_value_register
             ? 0
             : 1;
}
