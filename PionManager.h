#pragma once

#include <iostream>
#include <random>
#include <cmath>
#include <TGenPhaseSpace.h>
#include <TLorentzVector.h>
#include <Math/LorentzVector.h>
#include <Math/Vector4D.h>
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TDirectory.h"
#include "RootTools.h"
#include "Pion.h"
#include "Photon.h"
using ROOT::Math::PtEtaPhiEVector;

class PionManager
{
public:
	PionManager(const char * sourceFile = "data/AnalysisResults-trainOutput733805-Charlize-MC-Pi0Task-OO.root",
		const char * directory = "pi0eta-to-gammagamma-mc-pcmpcm;1/Generated;1/Pi0;1") {
		std::unique_ptr<TFile> pi0File(TFile::Open(sourceFile));
		TDirectory* dirPi0 = pi0File->GetDirectory(directory);
		TH2D* dist;
		dirPi0->GetObject("hPtY;1", dist);
		ptEtaDist = new TH2D(*dist);
	}

	Pion generateRandPi0(std::uniform_real_distribution<double_t>& rand, std::mt19937& gen) {
		double_t pt, eta, phi;

		ptEtaDist->GetRandom2(pt, eta); // Sample phi, eta from hist

		double_t flipEta = rand(gen);
		if (flipEta < .5) {
			eta = -eta; // Hist only has |y|
		}
		phi = genRandPhi(rand, gen);
		return Pion(pt, eta, phi);
	}

	static std::vector<Photon> decay(Pion pion, std::uniform_real_distribution<double_t>& rand, std::mt19937& gen) {
		// Randomly generate two photons in rest frame
		int nPhotons = 2;
		std::vector<Photon> photons;
		
		double_t theta1 = genRandTheta(rand, gen);
		double_t phi1 = genRandPhi(rand, gen);
		double_t phoPt1 = pion.getMass() / 2 * sin(theta1);
		double_t phoEta1 = -log(tan(theta1 / 2)); // Pseudorapidity = Rapidity for photons
		Photon photon1(phoPt1, phoEta1, phi1);

		double_t theta2 = M_PI - theta1;
		double_t phi2 = M_PI + phi1;
		double_t phoPt2 = pion.getMass() / 2 * sin(theta2);
		double_t phoEta2 = -log(tan(theta2 / 2));
		Photon photon2(phoPt2, phoEta2, phi2);

		photon1.boost(pion.getBetaVector()); // Boost to lab frame
		photon2.boost(pion.getBetaVector());
		
		photons.push_back(photon1);
		photons.push_back(photon2);

		return photons;
	}

private:
	TH2* ptEtaDist{ nullptr };

	static double_t genRandPhi(std::uniform_real_distribution<double_t>& rand, std::mt19937& gen) {
		return rand(gen) * 2 * M_PI;
	}

	static double_t genRandTheta(std::uniform_real_distribution<double_t>& rand, std::mt19937& gen) {
		return rand(gen) * M_PI;
	}
};

