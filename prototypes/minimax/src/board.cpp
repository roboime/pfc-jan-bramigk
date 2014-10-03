#include <vector>
#include <armadillo>
#include <cfloat>
#include "base.h"
#include "body.h"
#include "action.h"
#include "minimax.h"

constexpr int RAMIFICATION_NUMBER = 10;

using namespace std;

bool Board::isGameOver(){
  if(openGoalArea() > MIN_AREA_TO_MARK)
    return true;

  return false;
}

Player Board::currentPlayer(){
  return player;
}

vector<Action> Board::getActions(){
  vector<Action> actions;

  if(playerWithBall() == player){
    // Pass
    Robot robotWithBall = getRobotWithBall();

    for(auto robot: canGetPass()){
      Pass *pass = new Pass(robotWithBall, robot);
      actions.push_back(*pass);
    }

    // Kick
    actions.push_back(*new Kick(robotWithBall));
  }

  // Move
  for(auto robot: getRobots2Move()){
    actions.push_back(*new Move(robot));
  }

  return actions;
}

vector<vector<Action> > Board::getRobotsActions(){
  vector<vector<Action> > robotsActions;

  for(int i=0 ; i<RAMIFICATION_NUMBER ; i++){
    robotsActions.push_back(getActions());
  }

  return robotsActions;
}

Robot Board::getRobotWithBall(){
  // TODO: don't concatenate vectors
  vector<Robot> robots;

  // preallocate memory
  robots.reserve(min.getRobots()->size() + max.getRobots()->size());
  robots.insert(robots.end(), min.getRobots()->begin(), min.getRobots()->end());
  robots.insert(robots.end(), max.getRobots()->begin(), max.getRobots()->end());

  float min_time = FLT_MAX;
  Robot *robotWithBall = nullptr;

  float time;
  for(auto& robot: robots){
    time = getTimeToBall(robot);

    if(time < min_time){
      robotWithBall = &robot;
      min_time = time;
    }
  }

  return *robotWithBall;
}

float Board::getTimeToBall(const Robot& robot){
  /* TODO
   * vb.t + pb = vr.t + pr, t_min? vr?
   * => 0 = (vr^2 - vb^2)t^2 - |pr - pb|^2 - 2.vb.(pb - pr).t
   * => delta = 4.[ (vb.(pb - pr))^2 + (vr^2 - vb^2).|pr - pb|^2]
   * b = -2.vb.(pb - pr) = 2.vb.(pr - pb)
   * b_div_2 = vb.(pr - pb)
   * a = (vr^2 - vb^2)
   * c = -|pr - pb|^2
   * t = -b_div_2 +- sqrt(delta_div_4)
   *    -----------------
   *           a
   */

  // vb.(pb - pr)
  float a = (Robot.MAX_SPEED_SQUARE - ball.getSpeed() * ball.getSpeed());
  float c = - ((robot.getPosition() - ball.getPosition()) * (robot.getPosition() - ball.getPosition()));
  float b_div_2 = ball.getSpeed() * (ball.getPosition() - robot.getPosition());
  float delta_div_4 = b_div_2 * b_div_2 - a * c;

  if(a != 0){
    // It's impossible to reach the ball
    if(delta_div_4 < 0) return FLT_MAX;
    else if( delta_div_4 == 0){
      float t = - b_div_2 / a;

      if(t >= 0) return t;
      else return FLT_MAX;
    } else {
      // delta_div_4 > 0
      float t1 = -b - sqrt(delta_div_4), t2 =  -b - sqrt(delta_div_4);
      t1 /= a;
      t2 /= a;

    }
  } else{
    // a == 0

  }
}

float Board::getTimeToVirtualBall(const Robot& robot, const Ball &ball){
  // TODO
  // vb.t + pb = vr.t + pr, t_min? vr?
  // => 0 = (vr^2 - vb^2)t^2 - |pr - pb|^2 - 2.vb.(pb - pr).t
  // => delta = 4.[ (vb.(pb - pr))^2 + (vr^2 - vb^2).|pr - pb|^2]
  // t = -b +- sqrt(delta)
  //    -----------------
  //           2.a
}

Player Board::playerWithBall(){
  return getRobotWithBall().getPlayer();
}

vector<Robot> Board::canGetPass(){
  // TODO
  // create ball with after kick
  // float getTimeToBall(const Robot& robot, const Ball ball){
}

float Board::openGoalArea(){
  // TODO
}

float Board::evaluate(){
  // TODO
}

Board Board::applyRobotsActions(const vector<class Action>& actions){
  // TODO
}

float Board::getRobotsActionsTime(const vector<class Action>& actions){
  // TODO: get maximum time
}

vector<class Robot> Board::getRobots2Move(){
  // TODO
}
