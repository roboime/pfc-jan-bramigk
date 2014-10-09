#include "minimax.h"
#include <zmq.hpp>
#include <thread>
#include <iostream>

#include "discrete.pb.h"
#include "update.pb.h"
#include "timer.h"

void Minimax::run_minimax(std::function<void(Board&, std::mutex&)> run) {
  Board board;
  std::mutex board_mutex;

  //Timer tmr;
  bool should_recv(true);

  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  std::thread zmq_thread([&] () {
    zmq::context_t context (1);
    zmq::socket_t socket(context, ZMQ_REP);

    static const char * addr = "tcp://*:5555";
    socket.bind(addr);
    zmq::message_t buffer(1024);
    std::string data;

    std::cout << "listening on " << addr << std::endl;

    Board local_board;

    while (should_recv) {
      try {
        if (socket.recv(&buffer, ZMQ_RCVTIMEO)) {
          std::string buffer_str((char*)buffer.data(), buffer.size());
          roboime::Update u;
          u.ParseFromString(buffer_str);
          std::cout << u.ball().x() << std::endl;
          // TODO: update local_board with u

          {
            std::lock_guard<std::mutex> _(board_mutex);
            board = local_board;
          }

          zmq::message_t command_message(data.length());
          memcpy((void *) command_message.data(), data.c_str(), data.length());
          socket.send(command_message);
        }
      } catch(zmq::error_t e) {
        std::cerr << "error" << std::endl;
      }
    }
  });

  run(board, board_mutex);

  should_recv = false;
  zmq_thread.join();
}