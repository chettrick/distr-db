#ifndef JULIA_SET_HPP
#define JULIA_SET_HPP

#include <iostream>
#include <complex>
#include <boost/multi_array.hpp>
#include "thread_pool.hpp"
#include <cmath>
#include <math.h>

namespace ra::fractal {
	template <class Real>
	void compute_julia_set(const std::complex<Real>& bottom_left,
			const std::complex<Real>& top_right, const std::complex<Real>& c,
			int max_iters, boost::multi_array<int, 2>& a, int num_threads)
	{
		ra::concurrency::thread_pool tp(num_threads);

		// each row will be computed as a single task.
		// so create a lambda function that calculates each value
		// in a row and sets the corresponding element in matrix a
		// then schedule that task to the thread pool
		for(std::size_t cur_row = 0; cur_row < a.shape()[0]; ++cur_row) {
			std::function<void()> calculate_row = 
				std::function([cur_row, bottom_left, top_right, c, max_iters, &a]() {


					// make some labels for values to make it easier
					// to read the code.
					Real u_0(real(bottom_left));
					Real u_1(imag(bottom_left));
					Real v_0(real(top_right));
					Real v_1(imag(top_right));
					std::size_t H = a.shape()[0];
					std::size_t W = a.shape()[1];

					// for each column, calculate the julia set value
					for(std::size_t cur_col = 0; cur_col < a.shape()[1]; ++cur_col) {
						// initialize cur_z
						Real l(cur_row);
						Real k(cur_col);
						std::complex<Real> cur_z = {(u_0 + (Real(k) / Real(W - 1)) * (v_0 - u_0)), // real
									(u_1 + (Real(l) / Real(H - 1)) * (v_1 - u_1))};  // imag

						std::cout << "cur_row, cur_col: " << cur_row << ", " << cur_col << 
							"cur_z: " << cur_z << std::endl;

						// iterate up to max_iter times
						// I'll want to know how many iterations occured, so I'm
						// scoping the iterator i to exist outside the loop
						int i;
						for(i = 0; i <= max_iters; ++i) {
							// rather than check if |cur_z| < 2, I will check if
							// |cur_z| ^ 2 < 2 ^ 2.  This avoids having to take
							// square roots, which are very computationally
							// expensive
							Real threshold = 2;
							Real z_mag= std::abs(cur_z);
							if (z_mag > threshold) {
								break;
							} else { // calculate next iteration
								cur_z = cur_z * cur_z + c;
							}
						} // for i: 0 -> max_iters
						a[cur_row][cur_col] = std::min(max_iters, i);
					} // for each col
				}); // end of lambda function calculate_row
			tp.schedule(std::move(calculate_row));
		} // for each row
	} // compute_julia_set function
} // namespace

#endif
