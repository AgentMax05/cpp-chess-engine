#include <iostream>
#include <vector>
#include <chrono>

#include "Types.hpp"
#include "Engine.hpp"

int main()
{
	Storage::init();
	Board b("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
	
	Engine engine(b, 4);

	std::cout << "getting..." << "\n\n";
	auto start = std::chrono::high_resolution_clock::now();

	Move best = engine.getBestMove();

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	std::cout << Board::print_bitboard(best.to | best.from);
	std::cout << "Time elapsed: " << duration.count() / 1000.0 << " seconds\n";
}