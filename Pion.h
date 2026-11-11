#pragma once
#include "Particle.h"
using ROOT::Math::PtEtaPhiEVector;

class Pion : public Particle
{
public:
	Pion(double_t pt, double_t eta, double_t phi) : Particle(pi0Mass, pt, eta, phi) { }

private:
	static constexpr double_t pi0Mass = 0.1349768 ; // GeV/c^2
};

