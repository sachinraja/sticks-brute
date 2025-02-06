#include <iostream>
#include <unordered_map>
#include <vector>

struct Player {
  int hands[2];

  Player(int hand1, int hand2) {
    // must always keep hands sorted for stable comparison
    if (hand1 > hand2) {
      hands[0] = hand1;
      hands[1] = hand2;
    } else {
      hands[0] = hand2;
      hands[1] = hand1;
    }
  }

  Player() = default;

  int total_hand() {
    return hands[0] + hands[1];
  }
};

struct Node {
  Player players[2];
  int turn;

    bool operator==(const Node& other) const {
      for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
          if (players[i].hands[j] != other.players[i].hands[j]) return false;
        }
      }

      return turn == other.turn;
    }

    void print() { 
      printf("Player 1: (%d, %d)\nPlayer 2: (%d, %d)\nTurn: %d\n", players[0].hands[0], players[0].hands[1], players[1].hands[0], players[1].hands[1], turn);
    }
};

enum NodeState {
  CALCULATING,
  WON,
  LOST
};

std::string node_state_to_str(NodeState node_state) {
  switch (node_state) {
    case CALCULATING:
      return "calculating";
    case WON:
      return "won";
    case LOST:
      return "lost";
  }
  return "invalid";
}

inline void hash_combine(std::size_t& seed, std::size_t value) {
    seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct NodeHasher {
    std::size_t operator()(const Node& node) const {
        std::size_t seed = 0;

        for (int i = 0; i < 2; i++)
          for (int j = 0; j < 2; j++)
            hash_combine(seed, std::hash<int>()(node.players[i].hands[j]));

        hash_combine(seed, std::hash<int>()(node.turn));
        return seed;
    }
};

std::vector<Node> get_next_nodes(Node node) {
  std::vector<Node> next_nodes;

  int next_turn = 1 - node.turn;
  Player& current_player = node.players[node.turn];
  Player& next_player = node.players[next_turn];
  
  // can add either hand to either hand of other player (4 possible moves)
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      int next_hand = next_player.hands[j] + current_player.hands[i];
      if (next_hand >= 5) next_hand = 0;

      int other_hand = 1 - j;

      Node next_node;
      next_node.players[node.turn] = current_player;
      next_node.players[next_turn] = Player(next_player.hands[other_hand], next_hand);
      next_node.turn = next_turn;
      next_nodes.push_back(next_node);
    }
  }

  // can split hands
  int* hands = current_player.hands;
  int total_hand = current_player.total_hand();
  for (int i = 0; i <= total_hand / 2; i++) {
    if (i == hands[1] || total_hand - i >= 5) continue;
  
    Player player(total_hand - i, i);
    Node next_node;
    next_node.players[node.turn] = player;
    next_node.players[next_turn] = next_player;
    next_node.turn = next_turn;

    next_nodes.push_back(next_node);
  }

  return next_nodes;
}

NodeState get_node_state(Node node, std::unordered_map<Node, NodeState, NodeHasher>& node_state_map) {
  auto it = node_state_map.find(node);
  if (it != node_state_map.end()) return it->second;
  auto insert_it = node_state_map.insert(std::make_pair(node, CALCULATING));

  if (node.players[0].total_hand() == 0) {
    insert_it.second = LOST;
    return LOST;
  }
  if (node.players[1].total_hand() == 0) {
    insert_it.second = WON;
    return WON;
  }

  std::vector<Node> next_nodes = get_next_nodes(node);
  for (Node next_node : next_nodes) {
    NodeState node_state = get_node_state(next_node, node_state_map);
    // ignore if calculating -> no need to calculate again
    if (node_state == CALCULATING) continue;
    if (node_state == WON && node.turn == 0) return WON;
    if (node_state == LOST && node.turn == 1) return LOST;
  }

  NodeState result = node.turn == 0 ? LOST : WON;
  insert_it.second = result;
  return result;
}

int main() {
  std::unordered_map<Node, NodeState, NodeHasher> node_state_map;

  Node start_node = {
    .players = { Player(1, 1), Player(1, 1) },
    .turn = 0
  };

  NodeState node_state = get_node_state(start_node, node_state_map);
  std::cout << node_state_to_str(node_state) << "\n";
}
