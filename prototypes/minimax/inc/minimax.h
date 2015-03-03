#ifndef MINIMAX_MINIMAX_H
#define MINIMAX_MINIMAX_H

#include "base.h"
#include "body.h"
#include "action.h"

#include <cfloat>
#include <vector>
#include <functional>
#include <mutex>
#include <tuple>

class Team {
  friend class Board;
  std::vector<Robot> robots;

public:
  Team() {}
  Team(std::vector<Robot> n_robots) : robots(n_robots) {}

  const std::vector<Robot> &getRobots() const { return robots; }
  std::vector<Robot> &getRobots() { return robots; }

  int size() const { return robots.size(); }

  void addRobot(const Robot &robot) { robots.push_back(robot); }
};

struct Board {
  // TODO> calibrate
  static float MIN_GAP_TO_WIN;
  static float WEIGHT_TOTAL_GAP;
  static float WEIGHT_TOTAL_GAP_TEAM;
  static float WEIGHT_MAX_GAP;
  static float WEIGHT_RECEIVERS_NUM;
  static float WEIGHT_DISTANCE_TO_GOAL;
  static float WEIGHT_MOVE;

  Ball ball;
  Team max, min;
  float actionsMaxTime;
  bool maxOnLeft = true;


  Board() : min(), max(), ball(), actionsMaxTime(FLT_MAX) {}

  Board(Team &min, Team &max)
      : min(min), max(max), ball(), actionsMaxTime(FLT_MAX) {}

  Board(Team &min, Team &max, Ball &b)
      : min(min), max(max), ball(b), actionsMaxTime(FLT_MAX) {}

  Board(const Board &b)
      : max(b.max.getRobots()), min(b.min.getRobots()), ball(b.ball),
        actionsMaxTime(b.actionsMaxTime) {}

  Team &getTeam(Player p) {
    switch (p) {
    case MAX:
      return max;
    case MIN:
      return min;
    }
  }

  Ball &getBall() { return ball; }

  const Team &getTeam(Player p) const {
    switch (p) {
    case MAX:
      return max;
    case MIN:
      return min;
    }
  }

  const Team &getOtherTeam(Player p) const {
    switch (p) {
    case MAX:
      return min;
    case MIN:
      return max;
    }
  }

  bool isMaxLeft() const { return maxOnLeft; }

  static Board randomBoard() {
    Board b;

    for (int i = 0; i < 6; i++) {
      Robot r(i);
      r.setPos(Vector::getURand(fieldWidth(), fieldHeight()));
      b.min.addRobot(r);
      r.setPos(Vector::getURand(fieldWidth(), fieldHeight()));
      b.max.addRobot(r);
    }

    b.ball.setPos(Vector::getURand(fieldWidth(), fieldHeight()));
    return b;
  }

  const Ball &getBall() const { return ball; }
  const Team &getMax() const { return max; }
  const Team &getMin() const { return min; }

  bool isGameOver(Player player) const;
  // Player currentPlayer() const;

  TeamAction genKickTeamAction(Player) const;
  TeamAction genKickTeamAction(Player, MoveTable) const;
  TeamAction genKickTeamAction(Player, MoveTable, int move_id) const;
  TeamAction genPassTeamAction(Player) const;
  TeamAction genPassTeamAction(Player, MoveTable) const;
  TeamAction genPassTeamAction(Player, MoveTable, int move_id) const;
  TeamAction genActions(Player, bool, MoveTable, int move_id = -1) const;

  float totalGoalGap(Player player,
                     const Body &) const;              // sum of all gaps length
  float maxGoalGap(Player player, const Body &) const; // length of largest gap
  std::vector<std::pair<float, float>> getGoalGaps(Player player,
                                                   const Body &) const;
  std::vector<std::pair<float, float>> getGoalGaps() const {
    auto player_with_ball = getRobotWithBall().second;
    auto player = player_with_ball == MIN ? MAX : MIN;
    return getGoalGaps(player, getBall());
  }
  // bool freeKickLine(int point_index) const;
  // bool kickLineCrossRobot(const int point_index, const Robot &robot) const;
  std::vector<const Robot *> getRobotsMoving() const;
  float evaluate() const;

  Board virtualStep(float time) const;
  std::vector<const Robot *> canGetPass(Player) const;
  float teamActionTime(const TeamAction &actions) const;

  std::pair<const Robot *, Player> getRobotWithBall() const;
  std::pair<const Robot *, Player> getRobotWithVirtualBall(const Ball &) const;
  std::pair<const Robot *, Player>
  getRobotWithVirtualBall(const Ball &virt_ball,
                          std::pair<const Robot *, Player> r_rcv) const;
  std::vector<const Robot *> getOtherRobots(Player, const Robot &) const;

  float timeToBall(const Robot &robot) const;
  float timeToVirtualBall(const Robot &robot, const Ball &ball) const;
  float timeToVirtualBall(const Vector &pos, float maxV2, const Ball &virt_ball) const;
  Board applyTeamAction(const TeamAction &max_a, const TeamAction &min_a) const;

  static float fieldWidth() { return 8.090; }
  static float fieldHeight() { return 6.050; }

  // Double-size field
  static float goalX() { return fieldWidth() / 2; }
  static float goalY() { return 0; }
  static float goalWidth() { return 1.000; }
  static float goalDepth() { return 0.180; }

  Vector goalPos(Player p) const {
    float goal_x = goalX();

    if ((maxOnLeft && p == MAX) || (!maxOnLeft && p == MIN))
      goal_x *= -1;

    Vector goal_pos(goal_x, goalY());

    return goal_pos;
  }

  Vector enemyGoalPos(Player p) const {
    switch (p) {
    case MAX:
      return goalPos(MIN);
    case MIN:
      return goalPos(MAX);
    }
  }
};

struct Minimax {
  static int RAMIFICATION_NUMBER;
  static int MAX_DEPTH;

  MoveTable move_table_min, move_table_max;
  int move_count = 0;

  std::tuple<float, TeamAction, TeamAction> value_max(const Board, int depth);
  std::tuple<float, TeamAction> value_min(const Board, TeamAction max_action,
                                          int depth);
  std::tuple<float, TeamAction> value_max_only(const Board);

  TeamAction decision(const Board &);

  std::tuple<float, TeamAction, TeamAction> decision_value(const Board &);

  std::tuple<float, TeamAction, TeamAction>
  decision_experimental(const Board &);
};

#endif
