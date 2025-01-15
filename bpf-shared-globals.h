// bpf globals functionality shared between translator and stapbpf
// Copyright (C) 2016-2021 Red Hat Inc.
//
// This file is part of systemtap, and is free software.  You can
// redistribute it and/or modify it under the terms of the GNU General
// Public License (GPL); either version 2, or (at your option) any
// later version.

#ifndef BPF_SHARED_GLOBALS_H
#define BPF_SHARED_GLOBALS_H

namespace bpf {

// PR23476 List of percpu stat fields (see struct stat_data in runtime/stat.h).
std::vector<globals::stat_field> globals::stat_fields {
  "count", "sum", // for @count(), @sum(), @avg()
  // TODO: also "shift"
  // TODO: "min", "max", // for @min(), @max()
  // TODO: "avg_s", "_M2", "variance", "variance_s", // for @variance()
  // TODO: "histogram", // PR24424 for @hist_linear(), @hist_log()
};

// XXX Use the map for this field when iterating keys or testing inclusion:
std::string globals::stat_iter_field = "count";

globals::interned_stats_map
globals::intern_stats_map(const globals::stats_map &sm) {
  globals::interned_stats_map ism;
  for (globals::stat_field sf : globals::stat_fields)
    {
      auto it = sm.find(sf);
      assert (it != sm.end());
      ism.push_back(it->second);
    }
  return ism;
}

globals::stats_map
globals::deintern_stats_map(const globals::interned_stats_map &ism) {
  globals::stats_map sm;
  for (unsigned i = 0; i < std::min(ism.size(), globals::stat_fields.size()); i++)
    {
      globals::stat_field sf = globals::stat_fields[i];
      globals::map_idx map_id = ism[i];
      sm[sf] = map_id;
    }
  return sm;
}

globals::interned_foreach_info
globals::intern_foreach_info(const globals::foreach_info &fi) {
  globals::interned_foreach_info ifi{
    (uint64_t)fi.sort_direction,
    (uint64_t)fi.sort_column,
    (uint64_t)fi.keysize,
    (uint64_t)fi.sort_column_size,
    (uint64_t)fi.sort_column_ofs,
  };
  return ifi;
}

globals::foreach_info
globals::deintern_foreach_info(const globals::interned_foreach_info &ifi) {
  assert(ifi.size() == globals::n_foreach_info_fields);
  // XXX could handle older versions depending on ifi.size()
  // eventually we'll need a magic string for .bo versioning
  globals::foreach_info fi;
  fi.sort_direction = (int)ifi[0];
  fi.sort_column = (unsigned)ifi[1];
  fi.keysize = (size_t)ifi[2];
  fi.sort_column_size = (size_t)ifi[3];
  fi.sort_column_ofs = (int)ifi[4];
  return fi;
}

};

#endif
