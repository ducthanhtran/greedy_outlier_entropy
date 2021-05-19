RM = rm -f
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic -Ofast
OBJS = main.o greedy_outlier_entropy.o
PROG = greedy_outlier_entropy

all: $(PROG)

$(PROG): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(PROG) $(OBJS)

.PHONY: clean download experiments
clean:
	$(RM) $(PROG) $(OBJS)

download:
	mkdir -p ./data/ &&\
	wget -nc -P ./data/ https://archive.ics.uci.edu/ml/machine-learning-databases/lymphography/lymphography.data

experiments: $(PROG) download
	bash ./run_experiments.sh
