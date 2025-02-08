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
  PROCESSING,
  GUARANTEED_WIN,
  POSSIBLE_LOSS,
  DRAW
};

std::string node_state_to_str(NodeState node_state) {
  switch (node_state) {
    case PROCESSING:
      return "processing";
    case GUARANTEED_WIN:
      return "guaranteed win";
    case POSSIBLE_LOSS:
      return "possible loss";
    case DRAW:
      return "draw";
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
  
  // can add either hand to either hand of other player
  // 4 possible moves if a hand is not 0
  for (int i = 0; i < 2; i++) {
    // cannot add an empty hand to the other player
    if (current_player.hands[i] == 0) continue;

    for (int j = 0; j < 2; j++) {
      int next_hand = current_player.hands[i] + next_player.hands[j];
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
  auto insert_it = node_state_map.insert(std::make_pair(node, DRAW));

  if (node.players[0].total_hand() == 0) {
    insert_it.second = POSSIBLE_LOSS;
    return POSSIBLE_LOSS;
  }

  if (node.players[1].total_hand() == 0) {
    insert_it.second = GUARANTEED_WIN;
    return GUARANTEED_WIN;
  }
  
  std::vector<Node> next_nodes = get_next_nodes(node);

  for (Node next_node : next_nodes) {
    NodeState node_state = get_node_state(next_node, node_state_map);

    // need to be able to win for all the second player's possible moves
    if (node_state == POSSIBLE_LOSS && node.turn == 1) {
      insert_it.second = POSSIBLE_LOSS;
      return POSSIBLE_LOSS;
    }
  
    // need to be able to win for just one of the first player's possible moves
    if (node_state == GUARANTEED_WIN && node.turn == 0) {
      insert_it.second = GUARANTEED_WIN;
      return GUARANTEED_WIN;
    }
  }

  // if we've gone through the entire list of possible next moves and
  // haven't found a guaranteed win for player 1: it's possible to lose
  // can't possibly lose for any player 2 move: player 1 has a guaranteed win 
  NodeState result = node.turn == 0 ? POSSIBLE_LOSS : GUARANTEED_WIN;
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
  std::cout << "final state: " << node_state_to_str(node_state) << "\n";
}
