#pragma once
#include "TH1F.h"
#include "TH1.h"
#include <iostream>

class RootTools
{
public:
	// Divides two histograms and returns the result
	static TH1* takeRatio(TH1*hist1, TH1*hist2, const char* title = "ratio", const char* xTitle = "", const char* yTitle="") {
		TH1 *ratio = (TH1*)hist1->Clone(title);
		ratio->Divide(hist2);
		ratio->SetTitle(title);
		ratio->GetXaxis()->SetTitle(xTitle);
		ratio->GetYaxis()->SetTitle(yTitle);
		ratio->Draw();
		return ratio;
	}

	// Rebins a histogram according to the bins of another one
	static TH1* synchronizeBins(TH1* histToRebin, TH1* comparisonHist, const char* newName) {
		const int nBins = comparisonHist->GetNbinsX();
		std::vector<Double_t> bins(nBins + 1);
		for (int i = 0; i < nBins; i++) {
			bins[i] = comparisonHist->GetXaxis()->GetBinLowEdge(i + 1);
		}
		bins[bins.size() - 1] = comparisonHist->GetXaxis()->GetBinUpEdge(nBins);

		TH1* rebinned = histToRebin->Rebin(nBins, newName, bins.data());
		return rebinned;
	}
};

