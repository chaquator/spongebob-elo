#pragma once

#include <iostream>
#include <cmath>
#include <string>
#include <vector>

#include "SpongeBob.h"

#include "pch.h"

namespace Elo {
	// K is the arbitary constant which affects the sensitivity of score updating
	// with each outcome. Jeff Sonas recommends K=24.
	static const int K = 24;
	static const int Base_Rating = 1000;

	// Returns the probability of player 1 (r1) beating player 2 (r2).
	// The probability of r2 beating r1 is 1-P(r1,r2), i.e. the function is zero sum.
	double Probability(double r1, double r2);

	// Updates rankings for players 1 (r2) and 2 (r2).
	// W is true if player 1 beats player 2, false if player 1 loses.
	void UpdatedRating(double& r1, double& r2, bool W);

	class Episode {
	public:
		double Rating = Base_Rating;
		int Index;

		Episode();
		Episode(int, double);

		// for comparison for std::sort
		friend bool operator < (const Episode& l, const Episode& r) {
			return l.Rating < r.Rating;
		}

		// for outputting to an ostream (std::cout, file, etc.)
		friend std::ostream& operator << (std::ostream& os, const Episode& ep) {
			return os << ep.toString();
		}

		std::string toString() const;
		std::string present() const;
	};
}
