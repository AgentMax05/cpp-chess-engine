#include <iostream>
#include <vector>

#include "Types.hpp"
#include "Engine.hpp"

int main()
{
	
	/*uint8_t blockermask = 2;

	for (int index = 0; index < 2; index++) {
		uint8_t blockerboard = blockermask;
		
		int bitIndex = 0;

		for (int i = 0; i < 8; i++) {
			if ((blockermask & (1 << i)) != 0) {
				if ((index & (1 << bitIndex)) == 0) {
					blockerboard &= ~(1 << i);
				}
				bitIndex++;
			}
		}

		std::cout << Board::print_bitboard(blockerboard) << "\n\n";
		std::cin.get();
	}*/
	
	// initialize move database

	//std::cout << "starting storage load\n";
	//Storage storage;
	//std::cout << "done loading storage\n";

	/*std::string position = ""
		"00000000"
		"00000000"
		"00000000"
		"00000000"
		"00000000"
		"00000000"
		"00001000"
		"00000000";

	std::string position2 = ""
		"00000000"
		"00000000"
		"00000000"
		"00000000"
		"00000000"
		"00001000"
		"00000000"
		"00000000";*/

	//bitboard b = Board::stringToBitboard(position);
	//std::cout << Board::print_bitboard(b);

	//std::cout << "\nmoveboard for rook:\n";
	//bitboard m = storage.getMoveboard(3, 4, b, Piece::Rook);
	//std::cout << Board::print_bitboard(m);

	//Storage s;

	Storage::init();
	Board b("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
	
	/*b.makeMove(b.getMoves(true)[0]);
	std::cout << b.print_board();*/
	
	Engine engine(b, 4);

	std::cout << "getting..." << "\n\n";
	Move best = engine.getBestMove();

	std::cout << Board::print_bitboard(best.to | best.from);
}