
resources = main.cpp Preprocessor.hpp LexicalAnalyzer.hpp regtree.hpp nfa.hpp dfa.hpp dfalink.hpp

main: $(resources)
	g++ --std=c++11 main.cpp -o main

o: $(resources)
	g++ --std=c++11 -o3 main.cpp -o main

debug: $(resources)
	g++ --std=c++11 -D DEBUG main.cpp -o main

clean:
	rm main