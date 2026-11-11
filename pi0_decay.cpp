// pi0_decay.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "RootTools.h"
#include "PionManager.h"
#include "PhotonManager.h"
#include <fstream>
using ROOT::Math::PtEtaPhiEVector;

void saveVector(std::vector<double_t> vector, std::string fileName) {
    std::ofstream file("data/Arrays/" + fileName + ".csv");
	file << fileName << std::endl;
    for (double_t value : vector) {
        file << value << std::endl;
    }
	file.close();
}

std::vector<double_t> loadVector(std::string fileName) {
    std::vector<double_t> contents;
    std::ifstream file("data/Arrays/" + fileName + ".csv");
    std::string line;
    double_t val;
    std::getline(file, line); // Skip the first line
    while (std::getline(file, line)) {
		std::stringstream ss(line);
        while (ss >> val) {
            contents.push_back((double_t)val);
            if (ss.peek() == ',') {
                ss.ignore();
            }
		}
    }
    file.close();
    return contents;
}

void calculateRatios() {
    // Load Data
    std::unique_ptr<TFile> file1(TFile::Open("data/AnalysisResults-trainOutput733804-charlize-MC-QC-OO.root"));
    std::unique_ptr<TFile> file2(TFile::Open("data/AnalysisResults-trainOutput733805-Charlize-MC-Pi0Task-OO.root"));
    TDirectory* dirGenerate = file1->GetDirectory("pcm-qc-mc/Generated");
    TDirectory* dirV0 = file1->GetDirectory("pcm-qc-mc/V0/primary");

    TH1* genHPt = nullptr;
    TH1* genHPtConversionPhoton = nullptr;
    TH1* v0HPt = nullptr;
    dirGenerate->GetObject("hPt;1", genHPt);
    dirGenerate->GetObject("hPt_ConversionPhoton;1", genHPtConversionPhoton);
    dirV0->GetObject("hPt;1", v0HPt);

    TDirectory* dirPhoton = file2->GetDirectory("pi0eta-to-gammagamma-mc-pcmpcm;1/Generated;1/Pi0;1");
    TH1* pi0Pt = nullptr;
    dirPhoton->GetObject("hPt;1", pi0Pt);

    // Rebin v0HPt to match others
    TH1* v0HPtRebinned = RootTools::synchronizeBins(v0HPt, genHPt, "v0HPtRebinned");

    // Take Ratios
    TH1* PConv = RootTools::takeRatio(genHPtConversionPhoton, genHPt, "photon conversion rate", "p_{T,y} (GeV/c)", "#gamma_{conv}");
    TH1* EPhoRec = RootTools::takeRatio(v0HPtRebinned, genHPtConversionPhoton, "photon reconstruction efficiency", "p_{T,y} (GeV/c)", "#Epsilon_{rec}^{#gamma}");
    TH1* ETotRec = RootTools::takeRatio(v0HPtRebinned, genHPt, "total reconstruction efficiency", "p_{T,y} (GeV/c)", "#Epsilon_{rec}^{#gamma}");

    // Save
    TFile* ratioOutput = new TFile("data/ratioOutput.root", "RECREATE");
    PConv->Write("PConv");
    EPhoRec->Write("EPhoRec");
    ETotRec->Write("ETotRec");
    ratioOutput->Close();
}

void runSimulation(int nDataPoints = 10000) {
    std::uniform_real_distribution<double_t> rand(0.0, 1.0);
    std::random_device rd;
    std::mt19937 gen(rd());

    int reconstructedCounter = 0;
    std::vector<double_t> pi0Pts;
    std::vector<double_t> alphas;
    std::vector<double_t> photonPts;
    std::vector<double_t> asymmetries;

    PionManager pionManager = PionManager();
    PhotonManager photonManager = PhotonManager();

    // for (int i = 0; i < nRuns; ++i) {
    int counter = 0;
    int failure = 0;
    while (counter < nDataPoints) { // Repeat until the histogram has 1000000 events
        Pion pi0 = pionManager.generateRandPi0(rand, gen); // Generate random pi0
        std::vector<Photon> photons = pionManager.decay(pi0, rand, gen); // Decay into two photons
        bool reconstructed = photonManager.areReconstructed(photons, rand, gen); // Check if they are reconstructed
		failure += !reconstructed;
        if (reconstructed) {
            Particle reconstructedPi0 = photonManager.collide(photons[0], photons[1]); // If yes, reconstruct Pion
            pi0Pts.push_back(reconstructedPi0.getPt());
            alphas.push_back(photons[0].getOpeningAngle(photons[1]));
            photonPts.push_back(photons[0].getPt());
            //photonPts.push_back(photons[1].getPt());
            double_t asymmetry = abs((photons[0].getEnergy() - photons[1].getEnergy()) / (photons[0].getEnergy()
                + photons[1].getEnergy()));
            asymmetries.push_back(asymmetry);

            //std::cout << pi0.getFourVector().Px() << " " << photons[0].getFourVector().Px() + photons[1].getFourVector().Px() << std::endl;

            ++counter;
            if (counter % 100 == 0) {
                std::cout << counter << std::endl;
            }
        }
    }

    saveVector(pi0Pts, "Pi0Pts" + nDataPoints);
    saveVector(alphas, "SeparationAngles" + nDataPoints);
    saveVector(photonPts, "PhotonPts" + nDataPoints);
    saveVector(asymmetries, "asymmetries" + nDataPoints);

	TH1* modelHist = photonManager.getETotRec();

    TFile* reconstruction = new TFile("data/reconstruction.root", "RECREATE");
	TH1D pi0Hist = RootTools::binPt("pi0Hist", "Reconstructed Pi0 Pt Distribution", "p_T (GeV/c)", pi0Pts, modelHist);
	TH2D alphaHist = RootTools::binAgainstPt("alphaHist", "Separation Angle Distribution", "p_T (GeV/c)", "Separation Angle (rad)", pi0Pts, alphas, modelHist);
	TH2D photonHist = RootTools::binAgainstPt("photonHist", "Photon Pt Distribution", "p_T (GeV/c)", "Photon p_T (GeV/c)", pi0Pts, photonPts, modelHist);
	TH2D asymmetryHist = RootTools::binAgainstPt("asymmetryHist", "Photon Asymmetry Distribution", "p_T (GeV/c)", "Photon Asymmetry", pi0Pts, asymmetries, modelHist);

	pi0Hist.Write("pi0Pt");
	alphaHist.Write("separationAngle");
	photonHist.Write("photonPt");
	asymmetryHist.Write("asymmetry");
    reconstruction->Close();
}

int main()
{
	std::cout << "Starting!" << std::endl;
    //calculateRatios();
    //std::cout << "ratios done" << std::endl;
    runSimulation(10000);


}
