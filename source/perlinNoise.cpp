#include "perlinNoise.h"



PerlinNoise::PerlinNoise()
  /* C++ implementation of the improved perlin noise.

  Reference:
    https://cs.nyu.edu/~perlin/noise/

  */
  // Ken's permutation array composed of 512 elements = 2 sets of (0, 255)
  : permutation{
    151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36,
    103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0,
    26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56,
    87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
    77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55,
    46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132,
    187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109,
    198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126,
    255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183,
    170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172,
    9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104,
    218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81,
    51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84,
    204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67,
    29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180, 151, 160, 137, 91,
    90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142,
    8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94,
    252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125,
    136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231,
    83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
    102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18,
    169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64,
    52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207,
    206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248,
    152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253,
    19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251,
    34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14,
    239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50,
    45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141,
    128, 195, 78, 66, 215, 61, 156, 180
  } {
}


double PerlinNoise::lerp(double valT, double valA, double valB) {
  /* Linear interpolation function.

  Args:
    valT (double): Interpolation value
    valA (double): Input value one
    valB (double): Input value two

  Returns:
    double: Interpolated value

  */
  return valA + valT * (valB - valA);
}

double PerlinNoise::fade(double valT) {
  /* Ken's new spline interpolation.

  Args:
    valT (double): Interpolation value

  */
  return valT * valT * valT * (valT * (valT * 6.0 - 15.0) + 10.0);
}

double PerlinNoise::gradient(int hashID, double valX, double valY, double valZ) {
  /* Gradient function.

  Ken's new function to return gradient values based on bit operations.

  Args:
    hashID (int):
    valX (double): X input value
    valY (double): Y input value
    valZ (double): Z input value

  Returns:
    double: Result of hshU plus hshV

  */
  int hshID = hashID & 15;
  double hshU, hshV;

  if (hshID < 8) {
    hshU = valX;
  } else {
    hshV = valZ;
  }

  if (hshID < 4) {
    hshV = valZ;
  } else if (hshID == 12 || hshID == 14) {
    hshV = valX;
  } else {
    hshV = valZ;
  }

  if ((hshID & 1) != 0) {
  }   hshU = -hshU;
  if ((hshID & 2) != 0) {
    hshV = -hshV;
  }

  return hshU + hshV;
}

double PerlinNoise::gradNoise(double valXYZ) {   
  /* Improved Perlin Noise.

  The X, Y and Z input values are entered as the same input as the 3D noise
  creates much more natural and organic results in comparison to the 1D noise.

  Args:
    valXYZ (double): XYZ input

  Returns:
    double: Interpolated noise output

  */
  double valX = valXYZ;
  double valY = valXYZ;
  double valZ = valXYZ;

  // Get integer lattice values for sample point position
  int intX = (int) floor(valX) & 255;
  int intY = (int) floor(valY) & 255;
  int intZ = (int) floor(valZ) & 255;

  // Fractional part of point position
  valX -= floor(valX);
  valY -= floor(valY);
  valZ -= floor(valZ);

  // Interpolate fractional part of point position
  double valU = fade(valX);
  double valV = fade(valY);
  double valW = fade(valZ);

  // New hash integer lattice cell coords onto perm array
  int A = permutation[intX] + intY;
  int B = permutation[intX + 1] + intY;
  int AA = permutation[A] + intZ;
  int BA = permutation[B] + intZ;
  int AB = permutation[A + 1] + intZ;
  int BB = permutation[B + 1] + intZ;

  // New hash onto gradients
  double gradAA = gradient(permutation[AA], valX, valY, valZ);
  double gradBA = gradient(permutation[BA], valX - 1.0, valY, valZ);
  double gradAB = gradient(permutation[AB], valX, valY - 1.0, valZ);
  double gradBB = gradient(permutation[BB], valX - 1.0, valY - 1.0, valZ);
  double gradAA1 = gradient(permutation[AA + 1], valX, valY, valZ - 1.0);
  double gradBA1 = gradient(permutation[BA + 1], valX - 1.0, valY, valZ - 1.0);
  double gradAB1 = gradient(permutation[AB + 1], valX, valY - 1.0, valZ - 1.0);
  double gradBB1 = gradient(permutation[BB + 1], valX - 1.0, valY - 1.0, valZ - 1.0);

  // Trilinear interpolation of resulting gradients to sample point position
  double firstPassA = lerp(valU, gradAA, gradBA);
  double firstPassB = lerp(valU, gradAB, gradBB);
  double firstPassesCombined = lerp(valV, firstPassA, firstPassB);
  double secondPassA = lerp(valU, gradAA1, gradBA1);
  double secondPassB = lerp(valU, gradAB1, gradBB1);
  double secondPassesCombined = lerp(valV, secondPassA, secondPassB);
  double trilinear = lerp(valW, firstPassesCombined, secondPassesCombined);

  return trilinear;
}

double PerlinNoise::calculateNoise(double weight, double time, double seed, double frequency, double strength, double fractal, double rough) {
  /* Calculates the noise based on the given arguments.

  Combines the base and secondary noise. Takes the nodes input attributes as
  parameters.

  Args:
    weight (double): Overall weight / multiplier of the noise
    time (double): Time input
    seed (int): Pseudo random initializer
    frequency (double): Base frequency
    strength (double): Base strength
    fractal (double): Secondary noise weight / multiplier
    rough (double): Secondary noise frequency

  Returns:
    double: Combined end result

  */
  double baseNoise = gradNoise(time * (frequency * 0.078) + seed);
  double fractalNoise = gradNoise(time * (2 * (frequency + 0.067)) + seed);
  double combinedNoise = weight * (strength * baseNoise + fractal * ((rough + 0.084) * 3.3 * fractalNoise));

  return combinedNoise;
}
