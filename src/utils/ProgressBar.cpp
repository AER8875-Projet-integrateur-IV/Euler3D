#include "utils/ProgressBar.hpp"
#include <iostream>
#include <string>

ProgressBar::ProgressBar(std::string title, int numOperation, std::string nameOperation) : _totalOperation(numOperation), _nameOperation(nameOperation) {
	std::cout << title << "\n";
	_currentOperation = 0;
	Update(0);
}

void ProgressBar::Update(int numOperation) {
	float percent = numOperation / _totalOperation * 100;
	std::cout << "\r"
	          << "[" << percent << std::string(percent / 5, '~') << std::string(100 / 5 - percent / 5, ' ') << "]"
	          << percent << "%"
	          << " [" << _nameOperation << " " << numOperation + 1 << " of " << _totalOperation << "]";
	std::cout.flush();
	_currentOperation = numOperation;
	if (numOperation >= _totalOperation) {
		std::cout << "\n";
	}
}