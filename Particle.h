#pragma once
#include <iostream>
#include <random>
#include <cmath>
#include <TGenPhaseSpace.h>
#include <Math/GenVector/Boost.h>
#include <Math/LorentzVector.h>
#include <Math/Vector4D.h>
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TDirectory.h"
#include "RootTools.h"
using ROOT::Math::PtEtaPhiEVector;
using ROOT::Math::LorentzVector;
using ROOT::Math::Boost;

class Particle
{
public:
	Particle(double_t mass, double_t pt, double_t eta, double_t phi) {
		setMass(mass);
		setPt(pt);
		setEta(eta);
		setPhi(phi);
	}

	double_t getPhi() { return phi; }
	void setPhi(double_t phi) { this->phi = phi; }

	double_t getMass() { return mass; }
	void setMass(double_t mass) { this->mass = mass; }

	double_t getPt() { return pt; }
	void setPt(double_t pt) { this->pt = pt; }

	double getEta() { return eta; }
	void setEta(double_t eta) { this->eta = eta; }

	double_t getEnergy() { return sqrt(getMass() * getMass() + getPt() * getPt()) * cosh(getEta()); }

	PtEtaPhiEVector getFourVector() { return PtEtaPhiEVector(getPt(), getEta(), getPhi(), getEnergy()); }

	PtEtaPhiEVector::BetaVector getBetaVector() {
		return -getFourVector().BoostToCM();
	}

	double_t getOpeningAngle(Particle part) {
		double_t alpha = abs(this->getPhi() - part.getPhi());
		if (alpha > M_PI) {
			alpha = 2 * M_PI - alpha;
		}
		return alpha;
	}

	void boost(PtEtaPhiEVector::BetaVector beta) {
		PtEtaPhiEVector currentFourVector = getFourVector();
		Boost boost(beta);
		PtEtaPhiEVector newFourVec = boost(currentFourVector);
		setPt(newFourVec.Pt());
		setEta(newFourVec.Eta());
		setPhi(newFourVec.Phi());
	}

private:
	double_t mass;
	double_t pt;
	double_t eta;
	double_t phi;
};

