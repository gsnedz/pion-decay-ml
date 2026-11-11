#pragma once
#include "Particle.h"

class Photon : public Particle
{
public:
	Photon(double_t pt, double_t eta, double_t phi) : Particle(phoMass, pt, eta, phi) { }

	static constexpr double_t phoMass { 0 };
	
};

