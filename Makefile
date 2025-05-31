dependency:
	cd build && cmake .. --graphviz=graph.dot && dot -Tpng graph.dot -o graphImage.png

all: prepare

install:
	sudo apt-get install llvm cmake make git pkg-config curl python3-dev clang-format clang-tidy

prepare:
	rm -rf build
	mkdir build
	cd build