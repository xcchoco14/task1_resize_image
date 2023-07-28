FROM ubuntu

RUN apt-get update && apt-get install -y \
    # OpenCV dependencies
    build-essential cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev \
    python3-dev python3-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libdc1394-22-dev \
    libcanberra-gtk-module libcanberra-gtk3-module

# Clone, build and install OpenCV
RUN git clone https://github.com/opencv/opencv.git && \
    cd /opencv && mkdir build && cd build && \
    cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D INSTALL_C_EXAMPLES=ON -D INSTALL_PYTHON_EXAMPLES=ON -D OPENCV_GENERATE_PKGCONFIG=ON -D OPENCV_EXTRA_MODULES_PATH=~/opencv_build/opencv_contrib/modules -D BUILD_EXAMPLES=ON .. && \
    make -j"$(nproc)" && \
    make install && \
    rm -rf /opencv
# Clone, build and install bootstap
RUN wget https://boostorg.jfrog.io/artifactory/main/release/1.80.0/source/boost_1_80_0.tar.gz \
    tar xvf boost_1_80_0.tar.gz \
    cd boost_1_80_0 \
    ./bootstrap.sh --prefix=/usr/ \
    sudo ./b2 install


COPY . /app

WORKDIR /app

RUN cmake .
RUN make

#to run the HTTP server program and test the program
CMD ["./VSCode"]
RUN exec bash -c "curl -X POST -H \"Content-Type: application/json\" -d '{\"input_jpeg\":\"data:image/jpeg;base64,/9j/ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/\"}' http://localhost:8080/resize_image"
