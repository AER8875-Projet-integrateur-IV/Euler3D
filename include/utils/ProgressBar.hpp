#pragma once

#include <string>

class ProgressBar {
private:
	int _currentOperation;
	int _totalOperation;
	std::string _title;
	std::string _nameOperation;

public:
	ProgressBar(std::string title, int numOperation, std::string nameOperation);

	void Update(int numOperation);
	void operator+=(int n) {
		Update(_currentOperation + n);
	}
	void operator++() {
		Update(_currentOperation + 1);
	}
};
