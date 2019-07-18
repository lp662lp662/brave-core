/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/third_party/blink/brave_page_graph/graph_item/edge/edge_storage_read_call.h"
#include <string>
#include "brave/third_party/blink/brave_page_graph/graphml.h"
#include "brave/third_party/blink/brave_page_graph/graph_item/edge/edge_storage.h"
#include "brave/third_party/blink/brave_page_graph/graph_item/node/node.h"
#include "brave/third_party/blink/brave_page_graph/graph_item/node/node_script.h"
#include "brave/third_party/blink/brave_page_graph/graph_item/node/node_storage.h"
#include "brave/third_party/blink/brave_page_graph/page_graph.h"
#include "brave/third_party/blink/brave_page_graph/types.h"

using ::std::string;
using ::std::to_string;

namespace brave_page_graph {

EdgeStorageReadCall::EdgeStorageReadCall(PageGraph* const graph,
    NodeScript* const out_node, NodeStorage* const in_node,
    const string& key) :
      EdgeStorage(graph, out_node, in_node, key) {}

EdgeStorageReadCall::~EdgeStorageReadCall() {}

ItemName EdgeStorageReadCall::GetItemName() const {
  return "EdgeStorageReadCall#" + to_string(id_);
}

ItemDesc EdgeStorageReadCall::GetDescBody() const {
  return GetItemName() + " [key:" + key_ + "]";
}

GraphMLXMLList EdgeStorageReadCall::GraphMLAttributes() const {
  GraphMLXMLList items = EdgeStorage::GraphMLAttributes();
  items.push_back(
    GraphMLAttrDefForType(kGraphMLAttrDefEdgeType)->ToValue("storage read call"));
  return items;
}

}  // namespace brave_page_graph