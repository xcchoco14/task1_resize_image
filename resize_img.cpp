#include <iostream>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <jsoncpp/json/json.h>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind/bind.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>   
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include "base64.h"


using namespace std;
using namespace cv;
using namespace Json;
using namespace boost::beast;
using namespace boost::asio;
using namespace boost::beast::websocket;

void handle_request(boost::asio::io_service& io_service, boost::asio::ip::tcp::socket& socket) {
  // Read the request.
  char request[1024];
  size_t bytes_read = socket.read_some(buffer(request, sizeof(request)));

  // Parse the request.
  Json::Value root;
  Json::Reader reader;
  if (!reader.parse(request, root)) {
    cout << "Failed to parse request" << endl;
    socket.close();
    return;
  }

  // Get the input JPEG data.
  string input_jpeg = root["input_jpeg"].asString();

  // Get the desired width and height.
  int desired_width = root["desired_width"].asInt();
  int desired_height = root["desired_height"].asInt();

  // Resize the image.
  Mat image = imread(input_jpeg, IMREAD_COLOR);
  
  resize(image, image, Size(desired_width, desired_height));

  // Encode the resized image as base64.
  string output_jpeg = base64_encode(image.data, image.total() * image.elemSize());

  // Send the response.
  boost::asio::write(socket, buffer(output_jpeg));
          
  socket.close();
}

int main() {
  // Initialize the server.
  io_service io_service;
  ip::tcp::endpoint endpoint(ip::tcp::v4(), 8080);
  ip::tcp::acceptor acceptor(io_service, endpoint);
  cout << "done 1" << endl;

  // Handle HTTP requests.
  while (true) {
    ip::tcp::socket socket(io_service);
    cout << "done 2" << endl;
    acceptor.accept(socket);
    cout << "done 3" << endl;

    boost::thread thread(boost::bind(&handle_request, boost::ref(io_service), boost::ref(socket)));
    cout << "done 4" << endl;
  }

  return 0;
}