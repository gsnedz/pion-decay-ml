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

class PhotonManager
{
public:
	PhotonManager(const char* sourceFile = "data/ratioOutput.root", const char* directory = "") {
		this->loadEfficiency(sourceFile, directory);
	}

	bool areReconstructed(std::vector<Photon> photons, std::uniform_real_distribution<double_t>& rand, std::mt19937& gen) {
		double_t etaLimit = .8;

		for (Photon photon : photons) {
			double_t pt = photon.getPt();
			int bin = eTotRec->FindBin(pt);
			double_t efficiency = eTotRec->GetBinContent(bin);
			
			double_t recE = rand(gen);
			if (recE > efficiency) {
				return false; // Photon not reconstructed
			}

			if (abs(photon.getEta()) > etaLimit) {
				return false; // Not detected
			}
		}
		
		return true; // Both photons reconstructed
	}

	Pion collide(Photon pho1, Photon pho2) {
		PtEtaPhiEVector pi0FourVector = pho1.getFourVector() + pho2.getFourVector();
		Pion product(pi0FourVector.Pt(), pi0FourVector.Eta(), pi0FourVector.Phi());
		return product;
	}

	TH1* getETotRec() { return eTotRec; }

private:
	TH1* eTotRec{ nullptr };

	void loadEfficiency(const char * sourceFile = "data/ratioOutput.root", const char * dir = nullptr) {
		std::unique_ptr<TFile> ratioFile(TFile::Open(sourceFile));
		TDirectory* ratioDir = ratioFile->GetDirectory(dir);

		ratioDir->GetObject("ETotRec;1", eTotRec);
		eTotRec->SetDirectory(nullptr); // Detach from file to avoid deletion when file is closed
	}
};

