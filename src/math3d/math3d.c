#include "math3d.h"

/*
 * Computes the dot product of the vectors (ax, ay, az) and (bx, by, bz).
 */
double math3d_dot(double ax, double ay, double az, double bx, double by, double bz) {
    return ax * bx + ay * by + az * bz;
}

/*
 * Normalizes the vector (vx, vy, vz).
 */
void normalize(double *vx, double *vy, double *vz) {
    double magnitude = sqrt(math3d_dot(*vx, *vy, *vz, *vx, *vy, *vz));
    *vx /= magnitude;
    *vy /= magnitude;
    *vz /= magnitude;
}
