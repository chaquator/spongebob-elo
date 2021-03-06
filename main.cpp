#include "pch.h"

#include "Elo.h"
#include "SpongeBob.h"

#include "conio.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <iterator>

#define fs std::experimental::filesystem

//void get_episodes() {
//	std::vector<std::string> episodes;
//	for (auto &p : fs::recursive_directory_iterator("h:\\sb")) {
//		if (fs::is_regular_file(p) && p.path().filename().compare("episodes.txt") != 0) {
//			episodes.push_back(p.path().stem().string());
//		}
//	}
//	std::sort(episodes.begin(), episodes.end());
//
//	output_svec(episodes, std::cout);
//
//	std::cout << "\nwrite? (y/n)";
//	char ch = _getch();
//	if (ch == 'y') {
//		std::fstream f("h:\\sb\\episodes.txt", std::ofstream::out);
//		output_svec(episodes, f);
//	}
//}

// outputs vector to an ostream seperated by a string sequence
template <class T>
void output_vec(std::vector<T>& v, std::ostream& out, const std::string& seperator = " ") {
	typename std::vector<T>::iterator itr;
	for (itr = v.begin(); itr != v.end(); itr++) {
		out << *itr;
		if ((itr + 1) != v.end()) {
			out << seperator;
		}
	}
}

// creates new data file and fills so each episode has base rating
void create_data_file() {
	std::ofstream ofile("data.txt", std::ofstream::out);
	std::vector<int> fill(SpongeBob::EPISODE_COUNT, Elo::Base_Rating);
	output_vec(fill, ofile);
	ofile.close();
}

// saves current episode ratings to data.txt
void save_data_file(std::vector<Elo::Episode>& episodes) {
	std::ofstream ofile("data.txt", std::ofstream::out);
	std::vector<double> fill;
	for (auto p : episodes) fill.push_back(p.Rating);
	output_vec(fill, ofile);
	ofile.close();
}

// loads ratings from data.txt consecutively into vector corresponding with each episode
// pre-condition: episode vector is supposed to be empty
void load_data(std::vector<Elo::Episode>& episodes) {
	std::ifstream ifile("data.txt", std::ifstream::in);
	std::istream_iterator<double> eos;
	std::istream_iterator<double> fitr(ifile);
	for (int i = 0; fitr != eos && i < SpongeBob::EPISODE_COUNT; ++fitr) {
		episodes.push_back(Elo::Episode(i, *fitr));
		++i;
	}
	ifile.close();
}

// returns copy vector of episodes that is sorted by rating (for printing)
// cannot sort episodes vector because it would shuffle indeces
std::vector<Elo::Episode> sort_episodes(const std::vector<Elo::Episode>& episodes) {
	std::vector<Elo::Episode> ret(episodes);
	std::sort(ret.begin(), ret.end());
	std::reverse(ret.begin(), ret.end());
	return ret;
}

// generates a random pair of unique numbers within the number of spongebob episodes and writes to the pointer
// prerequesits: pair has 2 spaces
void random_pair(unsigned char* pair) {
	// random episode
	pair[0] = rand() % SpongeBob::EPISODE_COUNT;
	// offset that is 1 less than episode count is guaranteed to not be a duplicate for a pair of two
	pair[1] = (pair[0] + (rand() % (SpongeBob::EPISODE_COUNT - 1))) % SpongeBob::EPISODE_COUNT;
	if (pair[0] == pair[1]) {
		random_pair(pair);
	}
}

// presents episode matchup
void present_pair(unsigned char* pair, std::vector<Elo::Episode>& episodes) {
	std::cout << episodes.at(pair[0]).present() << "\n\tVS\n" << episodes.at(pair[1]).present() << "\n" << std::endl;
}

// prints full ranking
void print_ranking(std::vector<Elo::Episode>& sorted, bool showData = false) {
	std::cout << "RANKING:\n";
	for (int i = 0; i < sorted.size(); ++i) {
		std::cout << std::left << "#" << std::setw(5) << (i + 1)
				<< std::setw(37) << sorted.at(i).present();
		if (showData) { std::cout << " - " << std::right << std::fixed << std::setprecision(3) << std::setw(8)
								<< sorted.at(i).Rating; }
		std::cout << std::endl;
	}
}

int main() {
	// ensure existance of data.txt
	if (!fs::exists("data.txt")) {
		std::cout << "No data.txt exists, creating one...";
		create_data_file();
	}

	// load data into vector
	std::vector<Elo::Episode> episodes;
	load_data(episodes);

	// ensure loaded data is good
	if (episodes.size() != SpongeBob::EPISODE_COUNT) {
		std::cout << "Bad data! Recreate data file? If no, exit. (y/n) ";
		char c = _getch();
		if (c == 'y') {
			episodes.erase(episodes.begin(), episodes.end());
			create_data_file();
			load_data(episodes);
		}
		else {
			std::cout << "\nExiting...";
			return 1;
		}
	}

	// introduction
	std::cout << "SpongeBob Elo\n\nWhen presented with two episodes, press 1 or 2 for which of the two \
episodes is better.\n\nr - display all episodes ranked\nd - display rank with raw elo scores\ns - save data\n\
q - quit (saves data)\n\nPress any key to continue...\n" << std::endl;

	if (_getch() == 'q') return 0;

	// seed random
	srand((unsigned int)time(0));

	// working variables
	unsigned char pair[2];
	bool good = true;
	bool dont_shuffle = false;
	char ch = 'a';
	std::vector<Elo::Episode> sorted;

	// the main loop of the world
	do {
		if (dont_shuffle) {
			dont_shuffle = false;
		}
		else {
			// shuffle and present
			random_pair(pair);
			present_pair(pair, episodes);
		}

		ch = _getch();

		switch (ch) {
			// answer chosen
		case '1':
		case '2':
			// update ratings
			Elo::UpdatedRating(episodes.at(pair[0]).Rating, episodes.at(pair[1]).Rating, ch == '1');
			// output winner
			std::cout << episodes.at((ch == 1) ? pair[0] : pair[1]).present() << " wins.\n" << std::endl;
			break;

			// show ranking
		case 'r':
		case 'd': // show raw elo scores with d
			sorted = sort_episodes(episodes);
			print_ranking(sorted, ch == 'd');
			std::cout << "\n\nPress any key to continue...\n" << std::endl;

			if (_getch() == 'q') good = false;
			break;

			// quit
		case 'q':
			good = false;
			break;

			// saving
		case 's':
			std::cout << "Saving...\n";
			save_data_file(episodes); // TODO: try-catch
			std::cout << "\nPress any key to continue...\n" << std::endl;

			if (_getch() == 'q') good = false;
			break;

			// change nothing until a key is pressed
		default:
			dont_shuffle = true;
			break;
		}
	} while (good);

	// save to file
	save_data_file(episodes);

	return 0;
}
