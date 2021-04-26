FROM alpine:3.13.5
RUN apk add --no-cache \
    make \ 
    gcc \
    cmake \
    git \
    g++ \
    pkgconfig \ 
    build-base \
    cairo-dev \
    cairo \
    cairo-tools \
    graphicsmagick-dev \
    openssl-dev \
    boost-dev \
    jpeg-dev \
    xz-dev \
    tiff-dev \
    libpng-dev
WORKDIR /opt
RUN git clone -b master --single-branch --recurse-submodules https://github.com/cpp-io2d/P0267_RefImpl
# TODO (davidnet) This should be cherrypicking but does not work in the repo.
RUN wget -q "https://raw.githubusercontent.com/cpp-io2d/P0267_RefImpl/3100528b316167e94cf932c7a271467fedda26c1/P0267_RefImpl/Tests/CMakeLists.txt" \
    -O P0267_RefImpl/P0267_RefImpl/Tests/CMakeLists.txt && \
    cd P0267_RefImpl/P0267_RefImpl/Samples/svg/external/svgpp && \
    git checkout master 
RUN cd P0267_RefImpl && \
    mkdir Debug && \
    cd Debug && \
    cmake --config Debug "-DCMAKE_BUILD_TYPE=Debug" .. && \
    cmake --build . && \
    make install
# TODO (davidnet) Maybe set-up for a release?
RUN git clone https://github.com/jdgalviss/osm-route-planner.git --recurse-submodules
RUN cd osm-route-planner && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make
WORKDIR /opt/osm-route-planner/build
CMD ["./OSM_A_star_search"]