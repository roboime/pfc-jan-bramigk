class Body{
  Vector position;
  Vector speed;

public:
  Body(Vector p, Vector s):
    position(p), speed(s) {}

  Vector getPosition(){
    return position;
  }

  Vector getSpeed(){
    return speed;
  }

  void setPosition(Vector p){
    position = p;
  }

  void setSpeed(Vector s){
    speed = s;
  }

  float getDist(Vector target) const{
    return (position - target).norm();
  }
};

typedef enum {MIN, MAX} Player;

class Robot: public Body{
  int id;
  Player player;
  Vector last_planed_pos;
  static float MAX_SPEED_SQUARE = 16;// 4 m/s * 4 m/s

public:
  int getId(){
    return id;
  }

  void setPlayer(Player p){
    player = p;
  }

  Player getPlayer(){
    return player;
  }

  Vector getNRandPos() const {
    return Vector::getNRand(last_planed_pos);
  }

  Vector getURandPos() const {
    return Vector::getURand();
  }

  Vector getLastPlanedPos() const {
    return last_planed_pos;
  }
};

class Ball: public Body{
};
