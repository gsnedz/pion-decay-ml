// pi0_decay.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <TGenPhaseSpace.h>
#include <TLorentzVector.h>
#include "TFile.h"
#include "TH1F.h"
#include "TDirectory.h"
#include "RootTools.h"

int main()
{
    // Load Data
    std::unique_ptr<TFile> file1(TFile::Open("data/AnalysisResults-trainOutput733804-charlize-MC-QC-OO.root"));
    std::unique_ptr<TFile> file2(TFile::Open("data/AnalysisResults-trainOutput733805-Charlize-MC-Pi0Task-OO.root"));
    TDirectory *dirGenerate = file1->GetDirectory("pcm-qc-mc/Generated");
    TDirectory *dirV0 = file1->GetDirectory("pcm-qc-mc/V0/primary");
    
    TH1 *genHPt = nullptr;
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
	return 0;
}

