/*
 * test_julia_set.cpp
 * Dusty Miller
 * V00160288
 * SENG475 - Summer 2019
 * Assignment 6: cpp_concurrency
 */

#define CATCH_CONFIG_MAIN // this tells catch to provide a main()

#include <catch2/catch.hpp>
#include <iostream>
#include <complex>
#include <string>
//#include <string>
//#include <thread>
//#include <chrono> // for timeouts
//#include "ra/queue.hpp"
#include "ra/julia_set.hpp"

TEMPLATE_TEST_CASE("julia set test", "float", float) {
	using Array2 = boost::multi_array<int, 2>;
	using complex_real = std::complex<TestType>;
	int rows = 255, cols = 255;
	int H = rows, W = cols;
	Array2 a(boost::extents[rows][cols]);

	complex_real bottom_left(-1.25, -1.25);
	complex_real top_right(1.25, 1.25);
	complex_real c(0.37, -0.16);
	int max_iters = 255;
	const std::size_t NUM_THREADS = 8;
	ra::fractal::compute_julia_set<TestType>(bottom_left, top_right, c, max_iters, a, NUM_THREADS);
/*
	std::cout << "printing contents: " << std::endl;
	for(int i = 0; i < rows; ++i) {
		for(int j = 0; j < cols; ++j) {
			std::cout << a[i][j] << " ";
		}
		std::cout << std::endl;
	}
	*/

	// saving to file
	std::string filename = "fractal.pnm";
	std::ofstream ofile;
	ofile.open (filename);
	ofile << "P2 " << W << " " << H << "\n" << "255\n";
	for(int cur_row = 0; cur_row < H; ++cur_row) {
		for(int cur_col = 0; cur_col < W; ++cur_col) {
			ofile << a[H - 1 - cur_row][cur_col];
			if(cur_col == W - 1) {
				ofile << "\n";
			} else {
				ofile << " ";
			}
		}
	}
	ofile.close();
}
