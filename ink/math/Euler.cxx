/**
 * Copyright (C) 2021-2022 Hypertheory
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Euler.h"
#include <cmath>

namespace Ink {

Euler::Euler(float x, float y, float z, int o) :
x(x), y(y), z(z), order(o) {}

Euler::Euler(Vec3 r, int o) :
x(r.x), y(r.y), z(r.z), order(o) {}

Mat3 Euler::to_rotation_matrix() const {
	Mat3 rotation_x = {
		1       , 0       , 0       ,
		0       , cosf(x) , -sinf(x),
		0       , sinf(x) , cosf(x) ,
	};
	Mat3 rotation_y = {
		cosf(y) , 0       , sinf(y),
		0       , 1       , 0       ,
		-sinf(y), 0       , cosf(y) ,
	};
	Mat3 rotation_z = {
		cosf(z) , -sinf(z), 0       ,
		sinf(z) , cosf(z) , 0       ,
		0       , 0       , 1       ,
	};
	if (order == EULER_XYZ) {
		return rotation_x * rotation_y * rotation_z;
	}
	if (order == EULER_XZY) {
		return rotation_x * rotation_z * rotation_y;
	}
	if (order == EULER_YXZ) {
		return rotation_y * rotation_x * rotation_z;
	}
	if (order == EULER_YZX) {
		return rotation_y * rotation_z * rotation_x;
	}
	if (order == EULER_ZXY) {
		return rotation_z * rotation_x * rotation_y;
	}
	/*       ... EULER_ZYX */ {
		return rotation_z * rotation_y * rotation_x;
	}
}

}
