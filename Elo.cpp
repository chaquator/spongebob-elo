#include "Elo.h"
#include "pch.h"

// Elo rating stuff

double Elo::Probability(double r1, double r2) {
	return 1.0 * 1.0 / (1 + 1.0 *
		pow(10, 1.0 * (r1 - r2) / 400.0));
}

void Elo::UpdatedRating(double& r1, double& r2, bool W) {
	double p1 = Probability(r2, r1);
	r1 = r1 + (double)K*((W ? 1.0 : 0.0) - p1);
	r2 = r2 + (double)K*((W ? 0.0 : 1.0) - 1 + p1);
}

// Episode stuff

Elo::Episode::Episode() {

}

Elo::Episode::Episode(int index, double rating) {
	Index = index;
	Rating = rating;
}

std::string Elo::Episode::toString() const {
	return present() + " " + std::to_string(Rating);
}

std::string Elo::Episode::present() const {
	return SpongeBob::EPISODE_LIST[Index];
}
