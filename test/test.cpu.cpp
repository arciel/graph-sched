#include <thread>
#include <cassert>
#include <iterator>

#include "Manager.h"
#include "node_dsl.hpp"

#define CATCH_CONFIG_MAIN
#include <catch/catch.hpp>

TEST_CASE( "Node DSL constructs graph correctly.", "[node-dsl]" ) {

	auto dummy_func = [](){};

	Manager m;
	auto& node0 = m.append_node(0, dummy_func);
	auto& node1 = m.append_node(1, dummy_func);
	auto& node2 = m.append_node(2, dummy_func);
	auto& node3 = m.append_node(3, dummy_func);

	// Use the Node DSL to construct a graph.
	node0 >> (node1, node2) >> node3;

	// Expected parent/child relationship of the nodes.
	std::set<int> expected_node0_parents {};
	std::set<int> expected_node0_children {1, 2};
	std::set<int> expected_node1_parents {0};
	std::set<int> expected_node1_children {3};
	std::set<int> expected_node2_parents {0};
	std::set<int> expected_node2_children {3};
	std::set<int> expected_node3_parents {1,2};
	std::set<int> expected_node3_children {};
	
	REQUIRE(node0.parents == expected_node0_parents);
	REQUIRE(node1.parents == expected_node1_parents);
	REQUIRE(node2.parents == expected_node2_parents);
	REQUIRE(node3.parents == expected_node3_parents);

	REQUIRE(node0.children == expected_node0_children);
	REQUIRE(node1.children == expected_node1_children);
	REQUIRE(node2.children == expected_node2_children);
	REQUIRE(node3.children == expected_node3_children);
}


TEST_CASE( "Graph execution order is correct.", "[manager]" ) {
	auto fun0 = []() {};
	auto fun1 = []() { std::this_thread::sleep_for(std::chrono::microseconds(5000)); };

	Manager m;

	auto& node0 = m.append_node(0, fun0);
	auto& node1 = m.append_node(1, fun1);
	auto& node2 = m.append_node(2, fun0);
	auto& node3 = m.append_node(3, fun0);

	node0 >> (node1, node2) >> node3;
	m.execute();

	std::vector<int> expected_order = {0, 2, 1, 3};
	REQUIRE(m.execution_order() == expected_order);
}
