#include "util.h"

struct Vector2 normalize_vector(struct Vector2 vec) {
    long l = vec.x * vec.x + vec.y * vec.y;
	if (l != 0) {

		l = sqrt(l);
		vec.x /= l;
		vec.y /= l;
	}
    return vec;
}