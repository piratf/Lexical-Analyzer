resources = main.cpp Preprocessor.hpp LexicalAnalyzer.hpp regtree.hpp nfa.hpp dfa.hpp dfalink.hpp nfatable.hpp pclock.hpp

main: $(resources)
	g++ --std=c++11 main.cpp -o main

g: $(resources)
	g++ --std=c++11 -o0 -g main.cpp -o main

o: $(resources)
	g++ --std=c++11 -o3 main.cpp -o main

debug: $(resources)
	g++ --std=c++11 -D DEBUG main.cpp -o main

run: $(resources)
	g++ --std=c++11 -o3 main.cpp -o main
	./main input.txt code.txt

check: $(resources)
	g++ --std=c++11 -o0 -g main.cpp -o main
	valgrind --leak-check=full ./main input.txt main.cpp 2>1.txt
	for i in `seq 0 3`; do
		echo -e '\a'
		sleep 0.4
	done


clean:
	rm main