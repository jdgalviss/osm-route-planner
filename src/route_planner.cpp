#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y,
                           float end_x, float end_y)
    : m_Model(model) {
  // Convert inputs to percentage:
  start_x *= 0.01;
  start_y *= 0.01;
  end_x *= 0.01;
  end_y *= 0.01;
  // Find the closest nodes in the map corresponding to the starting and ending
  // coordinates
  start_node = &m_Model.FindClosestNode(start_x, start_y);
  end_node = &m_Model.FindClosestNode(end_x, end_y);
}

// Heuristic function
float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
  return node->distance(*end_node);
}

// Expand the current node by adding all unvisited neighbors to the open list
void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
  // Find the current node's neighbors
  current_node->FindNeighbors();
  // For each neighbor node update h value, g value, parent node and visited flag
  for (auto *node : current_node->neighbors) {
    node->h_value = CalculateHValue(node);
    node->g_value = current_node->g_value + node->distance(*current_node);
    node->parent = current_node;
    node->visited = true;
    open_list.push_back(node); // Add nodes to open list
  }
}

// Return a pointer to the next node (node with the lowest f value)
RouteModel::Node *RoutePlanner::NextNode() {
  // Sort the open list
  std::sort(open_list.begin(), open_list.end(),
            [](const RouteModel::Node *a, const RouteModel::Node *b) {
              return (a->g_value + a->h_value) > (b->g_value + b->h_value);
            });
  // Get a pointer to the node with the lowest f=g+h value and return it
  RouteModel::Node *lowest_f_node = open_list.back();
  open_list.pop_back();
  return lowest_f_node;
}

// Return the final path found after running A*
std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
  distance = 0.0f;
  std::vector<RouteModel::Node> path_found;
  // Trace the route back from the goal node to the start node (the start node has no parent)
  while (current_node->parent) {
    // Add the distance from the node to its parent
    distance += current_node->distance(*(current_node->parent));
    path_found.push_back(*current_node); // Add node to the path
    current_node = current_node->parent;
  }
  path_found.push_back(*current_node); // Add the start node
  std::reverse(path_found.begin(), path_found.end()); // Invert the path
  distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map in meters
  return path_found;
}

// Implementation of the A* Search algorithm
void RoutePlanner::AStarSearch() {
  // Initialize current node as the start node and calculate h and g values
  start_node->h_value = CalculateHValue(start_node);
  start_node->g_value = 0;
  start_node->parent = nullptr;
  RouteModel::Node *current_node = start_node;
  // Add the current node to the open list
  current_node->visited = true;
  open_list.push_back(current_node);
 
  // Loop while the open list is non empty
  while (open_list.size() > 0) {
    // Get the next node (node with the smallest f value) and assign to the current node
    current_node = NextNode();
    // Explore the current node's neighbors and add them to the open list
    AddNeighbors(current_node);
    // Check if goal was reached
    if (current_node->x == end_node->x && current_node->y == end_node->y) {
      std::cout << "Goal Reached! \n";
      // Use parent nodes to trace back the whole route
      std::vector<RouteModel::Node> path_found = ConstructFinalPath(current_node);
      m_Model.path = path_found;
      return;
    }
  }
  std::cout << "No path was found! \n";
}