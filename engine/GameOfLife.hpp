//
// Created by fla on 19.06.20.
//

#pragma once

#include "CellMatrix.hpp"

namespace engine {

class GameOfLife {

public:
	static void step(const CellMatrix<uint8_t>& current, CellMatrix<uint8_t>& next) {
		for(uint32_t y = 0; y < current.size().height(); y++) {
			for(size_t x = 0; x < current.size().width(); x++) {
				uint32_t neighbors = 0;

				neighbors += current.at(x - 1, y - 1);
				neighbors += current.at(x + 0, y - 1);
				neighbors += current.at(x + 1, y - 1);

				neighbors += current.at(x - 1, y + 0);
				auto cellValue = current.at(x, y);
				neighbors += current.at(x + 1, y + 0);

				neighbors += current.at(x - 1, y + 1);
				neighbors += current.at(x + 0, y + 1);
				neighbors += current.at(x + 1, y + 1);

				if(cellValue == 1 && neighbors > 3 || neighbors < 2) {
					next.at(x, y) = 0;
				} else if (cellValue == 0 && neighbors == 3) {
					next.at(x, y) = 1;
				} else {
					next.at(x, y) = cellValue;
				}
			}
		}
	}

};

}
