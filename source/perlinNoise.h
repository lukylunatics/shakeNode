#pragma once

// System Includes
#include <vector>
#include <cmath>



class PerlinNoise {

public:
  // Constructors
  PerlinNoise();

  // Public Methods
  double calculateNoise(double weight, double time, double seed, double frequency, double strength, double fractal, double rough);

private: 
  // Private Methods
  double lerp(double valT=0.5, double valA=0.0, double valB=1.0);
  double fade(double valT=1.0);
  double gradient(int hashID=255, double valX=1.0, double valY=1.0, double valZ=1.0);
  double gradNoise(double valXYZ=1.0);
  
  // Private Data
  std::vector<int> permutation;
};
